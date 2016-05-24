/* See LICENSE for license details. */

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <tinyalsa/asoundlib.h>

char *alsavol(void);
char *batcap(void);
char *loadavg(void);
char *curtime(void);

#include "config.h"

void
die(char *errstr, ...)
{
	va_list ap;

	va_start(ap, errstr);
	vfprintf(stderr, errstr, ap);
	va_end(ap);
	exit(1);
}

char*
strjoi(char *strs[], char *sep)
{
	char *res, *val;
	size_t len, slen = strlen(sep);

	len = 1;
	for (int i = 0; strs[i]; i++)
		len += strlen(strs[i]) + slen;

	len -= slen;
	if (len <= 1) return "";

	if (!(res = malloc(len * sizeof(char*))))
		die("malloc failed: %s\n", strerror(errno));

	strncpy(res, strs[0], len);
	for (int i = 1; ; i++) {
		if ((val = strs[i])) {
			strncat(res, sep, len);
			strncat(res, val, len);
		} else {
			break;
		}
	}

	return res;
}

double
readnum(char *bfp, char *fn)
{
	FILE *file;
	size_t rclen;
	char buf[16], *rc = NULL,
	     fp[strlen(bfp) + strlen(fn) + 2];

	snprintf(fp, sizeof(fp), "%s/%s", bfp, fn);
	if (!(file = fopen(fp, "r")))
		die("couldn't open '%s': %s\n", fp, strerror(errno));

	if (!(rc = fgets(buf, sizeof(buf), file))) {
		fclose(file);
		die("'%s' seems to be empty\n", fp);
	}

	rclen = strlen(buf);
	if (rc[rclen - 1] == '\n')
		rc[rclen - 1] = '\0';

	fclose(file);
	return atof(rc);
}

void
actlstr(char *buf, char *ch, struct mixer *mx) {
	char *status;
	struct mixer_ctl *ctl;

	if (!(ctl = mixer_get_ctl_by_name(mx, ch))) {
		mixer_close(mx);
		die("couldn't find mixer ctl '%s'\n", ch);
	}

	switch (mixer_ctl_get_type(ctl)) {
	case MIXER_CTL_TYPE_INT:
		snprintf(buf, 5, "%d%%", mixer_ctl_get_value(ctl, 0));
		break;
	case MIXER_CTL_TYPE_BOOL:
		status = mixer_ctl_get_value(ctl, 0) ? "On" : "Off";
		strncpy(buf, status, strlen(status) + 1);
		break;
	default:
		mixer_close(mx);
		die("unsupported ctl type '%s'\n",
			mixer_ctl_get_type_string(ctl));
	};
}

char*
batcap(void)
{
	static char batstr[8];
	double res, curc, maxc;

	curc = readnum((char*)sysbat, "charge_now");
	maxc = readnum((char*)sysbat, "charge_full_design");

	res = 100.0 * (curc / maxc);
	snprintf(batstr, sizeof(batstr), "%.2f%%", res);

	return batstr;
}

char*
alsavol(void)
{
	struct mixer *mx;
	static char alsastr[8];
	char fname[18 + strlen(ctlname)],
		*swtch = " Playback Switch",
		*volme = " Playback Volume";

	if (!(mx = mixer_open(sndcrd)))
		die("couldn't open mixer for card %d\n", sndcrd);

	strncpy(fname, ctlname, strlen(ctlname) + 1);
	strncat(fname, swtch, sizeof(fname));

	actlstr(alsastr, fname, mx);
	if (strcmp(alsastr, "Off")) {
		strncpy(fname, ctlname, sizeof(fname));
		strncat(fname, volme, sizeof(fname));
		actlstr(alsastr, fname, mx);
	}

	mixer_close(mx);
	return alsastr;
}

char*
loadavg(void)
{
	double avgs[3];
	static char loadstr[BUFSIZ];

	if (getloadavg(avgs, 3) == 0)
		die("getloadavg failed: %s\n", strerror(errno));

	snprintf(loadstr, sizeof(loadstr), "%.2f %.2f %.2f",
		avgs[0], avgs[1], avgs[2]);

	return loadstr;
}

char*
curtime(void)
{
	time_t tim;
	struct tm *timtm;
	static char tmstr[BUFSIZ];

	if ((tim = time(NULL)) == (time_t)-1)
		die("time failed: %s\n", strerror(errno));

	if (!(timtm = localtime(&tim)))
		die("Couldn't determine localtime\n");

	if (!strftime(tmstr, sizeof(tmstr), timefmt, timtm))
		die("strftime returned zero\n");

	return tmstr;
}

int
main(void)
{
	int screen, i, x;
	char *text, *val;
	Display *dpy;
	Window root;
	size_t len;

	if (!(dpy = XOpenDisplay(NULL)))
		die("Couldn't open display '%s'\n", XDisplayName(NULL));

	screen = DefaultScreen(dpy);
	root = RootWindow(dpy, screen);

	for (;;) {
		len = sizeof(sfuncs) / sizeof(sfuncs[0]);
		char *sres[len + 1];

		for (i = 0, x = 0; i < len; i++) {
			val = (*sfuncs[i])();
			if (val) sres[x++] = val;
		}

		sres[x] = NULL;
		text = strjoi(sres, (char*)statsep);

		XStoreName(dpy, root, text);
		XFlush(dpy);

		free(text);
		sleep(delay);
	}

	XCloseDisplay(dpy);
	return 0;
}
