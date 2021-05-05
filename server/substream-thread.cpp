#include "substream-thread.h"
#include <unistd.h>
#include "bc-server.h"
#include "lavf_device.h"
#include <thread>

substream::substream()
	: /*rec(0),*/ exit_flag(false)
{
}

void substream::run(struct bc_record *r)
{
	/*rec = r;*/
	int ret;
	stream_packet liveview_packet;

	r->log.log(Debug, "Starting substream thread %u ...", std::this_thread::get_id());

	while (!exit_flag)
	{
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
				if (bc_streaming_setup(r, BC_RTP, r->bc->substream_input->properties()))
					r->log.log(Error, "Unable to setup live broadcast from substream");
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
		/* Send packet to streaming clients */
		if (bc_streaming_is_active(r)) {
			if (bc_streaming_packet_write(r, liveview_packet) == -1) {
				r->log.log(Error, "Failed to write substream packet");
				goto error;
			}
		} else {
			r->log.log(Debug, "Substream: no active RTSP clients, sleeping...");
			sleep(1);
		}


		continue;
	error:
		bc_streaming_destroy_rtp(r);
		r->bc->substream_input->stop();
		sleep(3);
	}

	bc_streaming_destroy_rtp(r);
	r->bc->substream_input->stop();
}

void substream::stop()
{
	exit_flag = true;
}
