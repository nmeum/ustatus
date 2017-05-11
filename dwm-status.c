/* See LICENSE for license details. */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>

#include <sys/types.h>
#include <X11/Xlib.h>
#include <tinyalsa/asoundlib.h>

size_t alsavol(char*, size_t);
size_t batcap(char*, size_t);
size_t loadavg(char*, size_t);
size_t curtime(char*, size_t);
size_t seperator(char*, size_t);

#include "config.h"

enum {
	STATUSSZ = 128,
};

static char ststr[STATUSSZ];

void
die(char *errstr, ...)
{
	va_list ap;

	va_start(ap, errstr);
	vfprintf(stderr, errstr, ap);
	va_end(ap);
	exit(1);
}

double
readnum(char *bfp, char *fn)
{
	FILE *file;
	size_t rclen;
	char buf[16], *rc = NULL, fp[PATH_MAX];

	snprintf(fp, PATH_MAX, "%s/%s", bfp, fn);
	if (!(file = fopen(fp, "r")))
		die("couldn't open '%s': %s\n", fp, strerror(errno));

	if (!(rc = fgets(buf, 16, file))) {
		fclose(file);
		die("'%s' seems to be empty\n", fp);
	}

	rclen = strlen(buf);
	if (rc[rclen - 1] == '\n')
		rc[rclen - 1] = '\0';

	fclose(file);
	return atof(rc);
}

size_t
actlstr(char *buf, size_t n, char *ch, struct mixer *mx) {
	size_t ret;
	char *status;
	struct mixer_ctl *ctl;

	if (!(ctl = mixer_get_ctl_by_name(mx, ch))) {
		mixer_close(mx);
		die("couldn't find mixer ctl '%s'\n", ch);
	}

	switch (mixer_ctl_get_type(ctl)) {
	case MIXER_CTL_TYPE_INT:
		if ((ret = snprintf(buf, n, "%d%%",
				mixer_ctl_get_percent(ctl, 0))) > n)
			ret = n;
		break;
	case MIXER_CTL_TYPE_BOOL:
		status = mixer_ctl_get_value(ctl, 0) ? "On" : "Off";
		ret = stpncpy(buf, status, n) - buf;
		break;
	default:
		mixer_close(mx);
		die("unsupported ctl type '%s'\n",
			mixer_ctl_get_type_string(ctl));
	};

	return ret;
}

size_t
batcap(char *dest, size_t n)
{
	size_t ret;
	double res, curc, maxc;

	curc = readnum((char*)sysbat, "charge_now");
	maxc = readnum((char*)sysbat, "charge_full_design");

	res = 100.0 * (curc / maxc);
	if ((ret = snprintf(dest, n, "%.2f%%", res)) > n)
		ret = n;
	return ret;
}

size_t
alsavol(char *dest, size_t n)
{
	size_t ret;
	struct mixer *mx;

	if (!(mx = mixer_open(sndcrd)))
		die("couldn't open mixer for card %d\n", sndcrd);

	ret = actlstr(dest, n, (char*)swtchname, mx);
	if (strcmp(dest, "Off"))
		ret = actlstr(dest, n, (char*)volumname, mx);

	mixer_close(mx);
	return ret;
}

size_t
loadavg(char* dest, size_t n)
{
	size_t ret;
	double avgs[3];

	if (!getloadavg(avgs, 3))
		die("getloadavg failed: %s\n", strerror(errno));

	if ((ret = snprintf(dest, n, "%.2f %.2f %.2f",
			avgs[0], avgs[1], avgs[2])) > n)
		ret = n;
	return ret;
}

size_t
curtime(char *dest, size_t n)
{
	time_t tim;
	struct tm *timtm;

	if ((tim = time(NULL)) == (time_t)-1)
		die("time failed: %s\n", strerror(errno));

	if (!(timtm = localtime(&tim)))
		die("Couldn't determine localtime\n");

	return strftime(dest, n, timefmt, timtm);
}

size_t
seperator(char *dest, size_t n)
{
	return stpncpy(dest, statsep, n) - dest;
}

int
main(void)
{
	Display *dpy;
	Window root;
	size_t i, x, ret, len;

	if (!(dpy = XOpenDisplay(NULL)))
		die("Couldn't open display '%s'\n", XDisplayName(NULL));
	root = DefaultRootWindow(dpy);

	len = sizeof(sfuncs) / sizeof(sfuncs[0]);
	for (;;) {
		memset(ststr, '\0', STATUSSZ);
		for (i = 0, x = 0; i < len; i++, x += ret)
			ret = (*sfuncs[i])(&(ststr[x]), STATUSSZ - x);

		XStoreName(dpy, root, ststr);
		XSync(dpy, False);
		sleep(delay);
	}

	XCloseDisplay(dpy);
	return 0;
}
