/* Regression test for issue #768 follow-up: RTP session rotation.
 *
 * Covers rtp_wrap_guard (pre-wrap window + wedge detection) and the
 * stream_properties::session_id cut signal, compiled against the real
 * lib/input_device.cpp TU with only bc_log stubbed.
 *
 * Build and run from a configured tree (needs the staged libav/pugixml
 * headers; STAGE defaults to $(TOPDIR)/stage):
 *
 *   g++ -std=c++14 -o /tmp/test_rtp_rotation test_rtp_rotation.cpp \
 *       lib/input_device.cpp -Ilib -isystem $(STAGE)/include \
 *       -L$(STAGE)/lib -Wl,-rpath,$(STAGE)/lib -lavutil -lavcodec \
 *       && /tmp/test_rtp_rotation
 *
 * Expected output: RTP-ROTATION-ALL-PASS
 */

#include <stdio.h>
#include <stdint.h>
#include <time.h>

/* input_device.h relies on these coming first (as via libbluecherry.h) */
#include <linux/videodev2.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include "logc.h"
#include "input_device.h"

/* Stub: sanitize() references bc_log, but these tests never fire it. */
extern "C" void bc_log(enum log_level l, const char *msg, ...)
{
	(void)l; (void)msg;
}
extern "C" void bc_vlog(enum log_level l, const char *msg, va_list args)
{
	(void)l; (void)msg; (void)args;
}

static int failures = 0;
#define CHECK(cond, name) do { \
	if (cond) { printf("PASS %s\n", name); } \
	else { printf("FAIL %s\n", name); failures++; } \
} while (0)

static const int64_t U32 = (int64_t)0x100000000ULL;

int main(void)
{
	time_t t0 = 1000000;

	/* T1: pre-wrap fires inside the window after a long healthy session. */
	{
		rtp_wrap_guard g;
		CHECK(g.observe(1000, 1, 90000, true, t0) == rtp_wrap_guard::Ok, "T1-anchor");
		CHECK(g.observe(U32 - 27000000 - 1, 1, 90000, true, t0 + 10) == rtp_wrap_guard::Ok, "T1-below-edge");
		CHECK(g.observe(U32 - 27000000, 1, 90000, true, t0 + 11) == rtp_wrap_guard::PreWrap, "T1-fire");
	}

	/* T2: session that STARTED near the top must not fire immediately
	 * (delivered < margin); the wedge backstop covers the aftermath. */
	{
		rtp_wrap_guard g;
		CHECK(g.observe(U32 - 1000, 1, 90000, true, t0) == rtp_wrap_guard::Ok, "T2-anchor");
		CHECK(g.observe(U32 - 500, 1, 90000, true, t0 + 1) == rtp_wrap_guard::Ok, "T2-near-top-no-fire");
	}

	/* T3: wrap already happened between packets (base high, raw small). */
	{
		rtp_wrap_guard g;
		g.observe(U32 - 1000, 1, 90000, true, t0);
		CHECK(g.observe(500, 1, 90000, true, t0 + 1) == rtp_wrap_guard::PreWrap, "T3-missed-wrap");
	}

	/* T4: small backward jumps (reorder) never fire. */
	{
		rtp_wrap_guard g;
		g.observe(1000000000LL, 1, 90000, true, t0);
		CHECK(g.observe(1000000000LL - 5000, 1, 90000, true, t0 + 1) == rtp_wrap_guard::Ok, "T4-reorder");
	}

	/* T5: pre-wrap window applies to 90kHz only. */
	{
		rtp_wrap_guard g;
		g.observe(1000, 1, 1000000, true, t0);
		CHECK(g.observe(U32 - 100, 1, 1000000, true, t0 + 1) == rtp_wrap_guard::Ok, "T5-non90k");
	}

	/* T6: out-of-range raw stamps are ignored for pre-wrap purposes. */
	{
		rtp_wrap_guard g;
		g.observe(1000, 1, 90000, true, t0);
		CHECK(g.observe(-830103483316929823LL, 1, 90000, true, t0 + 1) == rtp_wrap_guard::Ok, "T6-garbage-raw");
		CHECK(g.observe(U32 + 100, 1, 90000, true, t0 + 2) == rtp_wrap_guard::Ok, "T6-extended");
	}

	/* T7: wedge fires after sustained unusable stamps, clears on usable. */
	{
		rtp_wrap_guard g;
		for (int i = 0; i < 5; i++)
			g.observe(1000 + i * 11250, 1, 90000, true, t0 + i);
		CHECK(g.observe(0, 1, 90000, false, t0 + 5) == rtp_wrap_guard::Ok, "T7-streak-start");
		CHECK(g.observe(0, 1, 90000, false, t0 + 5 + 29) == rtp_wrap_guard::Ok, "T7-before-timeout");
		CHECK(g.observe(0, 1, 90000, false, t0 + 5 + 30) == rtp_wrap_guard::Wedged, "T7-fire");
		CHECK(g.observe(2000000, 1, 90000, true, t0 + 40) == rtp_wrap_guard::Ok, "T7-recover");
		CHECK(g.observe(0, 1, 90000, false, t0 + 41) == rtp_wrap_guard::Ok, "T7-streak-restarted");
	}

	/* T8: a source that never emitted usable stamps never wedges. */
	{
		rtp_wrap_guard g;
		int r = rtp_wrap_guard::Ok;
		for (int i = 0; i < 100; i++)
			r = g.observe(0, 1, 90000, false, t0 + i * 30);
		CHECK(r == rtp_wrap_guard::Ok, "T8-never-healthy");
	}

	/* T9: session_id participates in properties inequality. */
	{
		stream_properties a, b;
		CHECK(!(a != b), "T9-default-equal");
		a.session_id = input_device::next_session_id();
		b.session_id = input_device::next_session_id();
		CHECK(a.session_id != b.session_id, "T9-ids-unique");
		CHECK(a != b, "T9-restart-cuts");
		b.session_id = a.session_id;
		CHECK(!(a != b), "T9-same-id-equal");
	}

	/* T10: reset() clears a fired guard. */
	{
		rtp_wrap_guard g;
		g.observe(1000, 1, 90000, true, t0);
		g.observe(U32 - 100, 1, 90000, true, t0 + 1);
		g.reset();
		CHECK(g.observe(1000, 1, 90000, true, t0 + 2) == rtp_wrap_guard::Ok, "T10-reset");
	}

	if (failures == 0)
		printf("RTP-ROTATION-ALL-PASS\n");
	else
		printf("RTP-ROTATION-FAILURES=%d\n", failures);
	return failures != 0;
}
