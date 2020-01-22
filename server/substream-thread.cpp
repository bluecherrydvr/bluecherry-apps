#include "substream-thread.h"
#include <unistd.h>
#include "bc-server.h"
#include "lavf_device.h"

substream::substream()
	: /*rec(0),*/ exit_flag(false)
{
}

void substream::run(struct bc_record *r)
{
	/*rec = r;*/

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
				if (bc_streaming_setup(r, r->bc->substream_input->properties()))
					r->log.log(Error, "Unable to setup live broadcast from substream");
		}
//...
		/* Send packet to streaming clients */
		if (bc_streaming_is_active(r)) {
			int ret;
			stream_packet liveview_packet;

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

			if (bc_streaming_packet_write(r, liveview_packet) == -1) {
				goto error;
			}
		} else {
			sleep(1);
		}


		continue;
	error:
		bc_streaming_destroy(r);
		r->bc->substream_input->stop();
		sleep(10);
	}

	bc_streaming_destroy(r);
}

void substream::stop()
{
	exit_flag = true;
}
