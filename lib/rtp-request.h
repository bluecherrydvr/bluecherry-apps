/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifndef __RTP_REQUEST_H
#define __RTP_REQUEST_H

#include "rtp-session.h"

void rtp_response_get_header(struct rtp_response *rp, const char *field,
			     char *val);

struct rtp_response *rtp_request_describe(struct rtp_session *rs);
struct rtp_response *rtp_request_setup(struct rtp_session *rs,
				       const char *uri);
int rtp_request_play(struct rtp_session *rs);
void rtp_request_teardown(struct rtp_session *rs);


#endif /* __RTP_REQUEST_H */
