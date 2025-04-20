#include "motion_handler.h"
#include <algorithm>

class raw_input_consumer : public stream_consumer
{
public:
	friend class motion_handler;

	raw_input_consumer()
		: stream_consumer("Motion Handling (Input)")
	{
	}
};

motion_handler::motion_handler()
	: stream_source("Motion Handling"), destroy_flag(false), prerecord_time(0), postrecord_time(0), motion_threshold_percentage(66)
{
	raw_stream = new raw_input_consumer;
}

motion_handler::~motion_handler()
{
	delete raw_stream;
}

void motion_handler::destroy()
{
	lock.lock();
	destroy_flag = true;
	lock.unlock();
	raw_stream->buffer_wait.notify_all();
}

void motion_handler::disconnect()
{
	raw_stream->disconnect();
}

stream_consumer *motion_handler::input_consumer()
{
	return raw_stream;
}

void motion_handler::set_buffer_time(int pre, int post)
{
	std::lock_guard<std::mutex> l(raw_stream->lock);
	if (pre == prerecord_time && post == postrecord_time)
		return;

	lock.lock();
	prerecord_time = pre;
	postrecord_time = post;
	lock.unlock();

	raw_stream->buffer.set_duration(prerecord_time);
}

void motion_handler::set_motion_analysis_ssw_length(int length)
{
	std::lock_guard<std::mutex> l(lock);
	ssw_motion_analysis.setSeqWindow(length);
}

void motion_handler::set_motion_analysis_percentage(int percentage)
{
	std::lock_guard<std::mutex> l(lock);
	motion_threshold_percentage = percentage;
}

void motion_handler::run()
{
	pthread_setname_np(pthread_self(), thread_name);
	std::unique_lock<std::mutex> l(raw_stream->lock);
	bool recording = false;
	time_t last_motion = 0;
	unsigned last_recorded_seq = 0;

	bc_log_context_push(log);

	int last_pkt_seq = -1;

	while (!destroy_flag)
	{
		raw_stream->buffer_wait.wait(l);
		if (raw_stream->buffer.empty())
			continue;

		const stream_keyframe_buffer &buffer = raw_stream->buffer;

		bool triggered = false;
		unsigned trigger_flags = 0;
		for (auto it = buffer.begin(); it != buffer.end(); it++) {
			if ((int)it->seq <= last_pkt_seq)
				continue;
			last_pkt_seq = it->seq;
			bc_log(Debug, "pkt: type:%c flags: 0x%02x, size: %u, pts: %" PRId64 " ts_clock: %u ts_monotonic: %u seq: %u",
			       it->type == AVMEDIA_TYPE_VIDEO ? 'V' : 'A',
			       it->flags, it->size, it->pts, (unsigned)it->ts_clock, (unsigned)it->ts_monotonic, it->seq);
			bc_log(Debug, "pkt: motion: %s, trigger: %s, pts: %" PRId64 " seq: %u",
					(it->flags & stream_packet::MotionFlag) ? "YES" : "NO ",
					(it->flags & stream_packet::TriggerFlag) ? "YES" : "NO ",
					it->pts, it->seq);

			if (it->flags & stream_packet::TriggerFlag) {
				triggered = true;
				trigger_flags = it->flags;
				bc_log(Info, "Preserving trigger flags: 0x%x", trigger_flags);
				break;
			}

			if (it->type == AVMEDIA_TYPE_VIDEO) {
				ssw_motion_analysis.push((it->flags & stream_packet::MotionFlag) ? 1 : 0);
				int percentage = 100 * ssw_motion_analysis.sum() / ssw_motion_analysis.count();
				bc_log(Debug, "count = %d; percentage = %d; motion_threshold_percentage %d",
						ssw_motion_analysis.count(), percentage, motion_threshold_percentage);

				if ((ssw_motion_analysis.count() == ssw_motion_analysis.getSeqWindow())
						&& (percentage >= motion_threshold_percentage)) {
					triggered = true;
					trigger_flags = it->flags;
					break;
				}
			}
		}

		int send_from = -1;

		if (triggered && !recording) {
			if (last_recorded_seq && last_recorded_seq >= buffer.front().seq) {
				unsigned int resume_pos = 0;
				for (; resume_pos < buffer.size(); resume_pos++) {
					if (buffer[resume_pos].seq > last_recorded_seq)
						break;
				}

				bc_log(Debug, "motion: resume recording");
			} else {
				bc_log(Debug, "motion: start recording");
				bc_log(Info, "Motion detected");
				send_from = 0;
			}
			recording = true;
		}

		if (!triggered && recording && buffer.back().ts_monotonic - last_motion > postrecord_time - prerecord_time) {
			bc_log(Debug, "motion: pause recording");
			recording = false;
		}

		if (!recording && last_recorded_seq && buffer.front().seq > last_recorded_seq) {
			bc_log(Debug, "motion: stop recording");
			last_recorded_seq = 0;
			send(stream_packet());
			continue;
		}

		if (triggered)
			last_motion = buffer.back().ts_monotonic;

		if (!recording)
			continue;

		if (send_from < 0) {
			for (int i = buffer.size()-1; i >= 0; i--) {
				if (buffer[i].seq > last_recorded_seq)
					continue;

				send_from = i+1;
				break;
			}

			if (send_from < 0) {
				bc_log(Bug, "motion_handler: last_recorded_seq is not in buffer");
				send_from = 0;
			}
		}

		for (unsigned int i = send_from; i < buffer.size(); i++) {
			stream_packet pkt = buffer[i];
			if (triggered) {
				pkt.flags = trigger_flags;
				bc_log(Debug, "Setting packet flags to 0x%x", pkt.flags);
			}
			send(pkt);
		}
		last_recorded_seq = buffer.back().seq;
	}

	bc_log(Debug, "motion_handler destroying");
	l.unlock();
	delete this;
}

static inline bool packet_seq_compare(const stream_packet &p1, const stream_packet &p2)
{
	return p1.seq < p2.seq;
}
