#include "substream-thread.h"
#include <unistd.h>
#include "bc-server.h"
#include "lavf_device.h"
#include <thread>
#include <bsd/string.h>

substream::substream()
	: /*rec(0),*/ exit_flag(false)
{
}

void substream::run(struct bc_record *r)
{
	/*rec = r;*/
	int ret;
	stream_packet liveview_packet;

	r->log.log(Debug, "Starting substream thread %zu ...", std::hash<std::thread::id>{}(std::this_thread::get_id()));

	while (!exit_flag)
	{
		if (!r->bc->substream_mode || !r->bc->substream_input) {
			sleep(1);
			continue;
		}

		if (r->bc->substream_mode && !r->bc->substream_input->is_started()) {
			if (r->bc->substream_input->start() < 0) {
				if ((r->start_failed & BC_SUBSTREAM_START_FAILED) == 0) {
					r->log.log(Error, "Error starting substream: %s",
						r->bc->substream_input->get_error_message());
				}
				r->start_failed |= BC_SUBSTREAM_START_FAILED;
				goto error;
			} else if (r->start_failed & BC_SUBSTREAM_START_FAILED) {
				r->start_failed &= ~BC_SUBSTREAM_START_FAILED;
				r->log.log(Error, "Substream started after failures");
			} else if (r->bc->type == BC_DEVICE_LAVF) {
				const char *info = reinterpret_cast<lavf_device*>(r->bc->substream_input)->stream_info();
				r->log.log(Info, "Substream started: %s", info);
			}

			if (r->bc->substream_input->is_started())
			{
				if (bc_streaming_setup(r, BC_RTP, r->bc->substream_input->properties()))
					r->log.log(Error, "Unable to setup live broadcast from substream");

				if (bc_streaming_setup(r, BC_HLS, r->bc->substream_input->properties()))
					r->log.log(Error, "Unable to setup HLS stream from substream");
			}
		}

		ret = r->bc->substream_input->read_packet();

		if (ret == EAGAIN)
			continue;
		else if (ret != 0) {
			if (r->bc->type == BC_DEVICE_LAVF) {
				const char *err = reinterpret_cast<lavf_device*>(r->bc->substream_input)->get_error_message();
				r->log.log(Error, "Read error from liveview substream: %s", *err ? err : "Unknown error");
			}
			goto error;
		}

		liveview_packet = r->bc->substream_input->packet();

		/* Motion analysis consumes here when enabled, independent of
		 * live clients; recording stays on the mainstream source. */
		if (r->bc->sub_source)
			r->bc->sub_source->send(liveview_packet);

		/* Hold the streaming lock across each is_active+write
		 * pair: the device thread may otherwise destroy the
		 * muxer context between the check and the write. */
		pthread_mutex_lock(&r->streaming_mutex);
		if (bc_streaming_is_active_hls(r)) {
			if (bc_streaming_hls_packet_write(r, liveview_packet) == -1) {
				r->log.log(Error, "Failed to write HLS substream packet");
				pthread_mutex_unlock(&r->streaming_mutex);
				goto error;
			}
		}
		pthread_mutex_unlock(&r->streaming_mutex);

		/* Send packet to streaming clients */
		pthread_mutex_lock(&r->streaming_mutex);
		if (bc_streaming_is_active(r)) {
			if (bc_streaming_packet_write(r, liveview_packet) == -1) {
				r->log.log(Error, "Failed to write substream packet");
				pthread_mutex_unlock(&r->streaming_mutex);
				goto error;
			}
		}
		pthread_mutex_unlock(&r->streaming_mutex);
		if (!bc_streaming_is_active(r) && !bc_streaming_is_active_hls(r) && !r->motion_on_substream) {
			// Dont sleep if RTSP/HLS clients are active or motion analysis consumes the substream
			r->log.log(Debug, "Substream: no active RTSP or HLS clients, sleeping...");
			sleep(1);
		}

		continue;
	error:
		bc_streaming_destroy_rtp(r);
		bc_streaming_destroy_hls(r);
		r->bc->substream_input->stop();
		sleep(3);
	}

	bc_streaming_destroy_rtp(r);
	bc_streaming_destroy_hls(r);
	r->bc->substream_input->stop();
}

void substream::stop()
{
	exit_flag = true;
}

void substream::set_thread_name(const char *arg) {
	strlcpy(thread_name, arg, sizeof(thread_name));
}
