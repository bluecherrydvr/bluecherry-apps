#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <sys/mman.h>
#include <errno.h>

#include <libbluecherry.h>

#define BOUNDARY	"MJPEGBOUNDARY"

static int ch;
static int mp;

static void print_error(char *msg, ...)
{
	va_list ap;

	fprintf(stdout, "Content-type: text/html; charset=ISO-8859-1\r\n\r\n");
	fprintf(stdout, "<html><body><pre>\n");

	va_start(ap, msg);
	vfprintf(stdout, msg, ap);
	va_end(ap);

	fprintf(stdout, "\n</pre></body></html>\n");

	exit(1);
}

static void parse_query_string(void)
{
	char *str;
	char *tok;

	if ((str = getenv("QUERY_STRING")) == NULL)
		return;

	while ((tok = strtok_r(str, "&", &str)) != NULL) {
		char *param_name;
		char *param_value;
		int val;

		param_name = strtok_r(tok, "=", &param_value);
		if (strlen(param_value) == 0)
			val = 0;
		else
			val = strtol(param_value, NULL, 0);

		if (strcmp(param_name, "channel") == 0)
			ch = val;
		else if (strcmp(param_name, "multipart") == 0)
			mp = val;
	}
}

static void print_image(struct bc_handle *bc)
{
	bc_buf_get(bc);

	if (fprintf(stdout, "Content-type: image/jpeg\r\n") < 0)
		exit(1);
	if (fprintf(stdout, "Content-size: %lu\r\n\r\n", bc_buf_size(bc)) < 0)
		exit(1);
	if (fwrite(bc_buf_data(bc), bc_buf_size(bc), 1, stdout) != 1)
		exit(1);
}

int main(int argc, char **argv)
{
	struct bc_handle *bc;
	char dev[256];

	parse_query_string();

	if (ch < 0 || ch > 15)
		print_error("Invalid channel %d", ch);

	/* Setup the device */
	sprintf(dev, "/dev/video%d", ch + 1);
	if ((bc = bc_handle_get(dev)) == NULL)
		print_error("%s: error opening device: %m", dev);

	/* Setup for MJPEG, leave everything else as default */
	bc->vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	if (ioctl(bc->dev_fd, VIDIOC_S_FMT, &bc->vfmt) < 0)
		print_error("%s: error setting mjpeg: %m", dev);

	if (bc_handle_start(bc))
		print_error("%s: error starting stream: %m", dev);

	/* Now grab frame(s) */
	if (fprintf(stdout, "Cache-Control: no-cache\r\n" \
		    "Cache-Control: private\r\n" \
		    "Pragma: no-cache\r\n") < 0)
		exit(1);

	/* Single frame */
	if (!mp) {
		print_image(bc);
		exit(0);
	}

	/* Multi part (motion jpeg) */
	if (fprintf(stdout, "Content-type: multipart/x-mixed-replace;" \
		    "boundary=" BOUNDARY "\r\n\r\n") < 0)
		exit(1);

	while(1) {
		if (fprintf(stdout, "--" BOUNDARY "\r\n") < 0)
			exit(1);
		print_image(bc);
	}

	bc_handle_free(bc);

	exit(0);
}
