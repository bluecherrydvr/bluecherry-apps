/*
 * Copyright (C) 2009-2010 Ben Collins <bcollins@bluecherry.net>
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <libbluecherry.h>

void *bc_buf_data(struct bc_handle *bc)
{
	if (bc->q_cnt == 0)
		return NULL;
	return bc->p_buf[bc->q_buf[bc->q_cnt - 1].index].data;
}

size_t bc_buf_size(struct bc_handle *bc)
{
	if (bc->q_cnt == 0)
		return 0;
	return bc->q_buf[bc->q_cnt - 1].bytesused;
}

struct v4l2_buffer *bc_buf_v4l2(struct bc_handle *bc)
{
	if (bc->q_cnt == 0)
		return NULL;
	return &bc->q_buf[bc->q_cnt - 1];
}

int bc_buf_key_frame(struct bc_handle *bc)
{
	unsigned char *p = bc_buf_data(bc);

	if (bc_buf_v4l2(bc)->flags & V4L2_BUF_FLAG_KEYFRAME)
		return 1;
	/* Fallback */
        if (p[2] == 0x01 && p[3] == 0x00)
                return 1;

        return 0;
}


