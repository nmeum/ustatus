#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <X11/Xlib.h>

static char *batcap(void);
static char *curload(void);
static char *curtime(void);

#include "config.h"
#define LENGTH(X) (sizeof X / sizeof X[0])

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
strjo(char *strs[], size_t nfn, char *sep)
{
	char *res;
	size_t len;

	assert(nfn >= 1);

	len = (nfn - 1) * strlen(sep) + 1;
	for (int i = 0; i < nfn; i++)
		len += strlen(strs[i]);

	if (!(res = malloc(len)))
		die("malloc failed: %s\n", strerror(errno));

	strcpy(res, strs[0]);
	for (int n = 1; n < nfn; n++) {
		strcat(res, sep);
		strcat(res, strs[n]);
	}

	return res;
}

double
readint(char *bfp, char *fn)
{
	FILE *file;
	size_t rclen;
	char buf[16], *rc = NULL,
	     fp[strlen(bfp) + strlen(fn) + 2];

	snprintf(fp, sizeof(fp), "%s/%s", bfp, fn);
	if (!(file = fopen(fp, "r")))
		die("couldn't open '%s': %s\n", fp, strerror(errno));

	if (!(rc = fgets(buf, sizeof(buf), file)))
		die("'%s' seems to be empty\n", fp);

	rclen = strlen(buf);
	if (rc[rclen - 1] == '\n')
		rc[rclen - 1] = '\0';

	return atof(rc);
}

char*
batcap(void)
{
	static char buf[BUFSIZ];
	double res, curc, maxc;

	curc = readint((char*)sysbat, "charge_now");
	maxc = readint((char*)sysbat, "charge_full_design");

	res = 100 * (curc / maxc);
	snprintf(buf, sizeof(buf), "%.2f%%", res);

	return buf;
}

char*
curload(void)
{
	double avgs[3];
	static char buf[BUFSIZ];

	if (getloadavg(avgs, 3) == 0)
		die("getloadavg failed: %s\n", strerror(errno));

	snprintf(buf, sizeof(buf), "%.2f %.2f %.2f",
		avgs[0], avgs[1], avgs[2]);

	return buf;
}

char*
curtime(void)
{
	time_t tim;
	struct tm *timtm;
	static char buf[BUFSIZ];

	if ((tim = time(NULL)) == (time_t)-1)
		die("time failed: %s\n", strerror(errno));

	if (!(timtm = localtime(&tim)))
		die("Couldn't determine localtime\n");

	if (!strftime(buf, sizeof(buf), timefmt, timtm))
		die("strftime returned zero\n");

	return buf;
}

int
main(void)
{
	Display *dpy;
	Window root;
	size_t len;
	int screen;
	char *text;

	if ((dpy = XOpenDisplay(NULL)) == NULL)
		die("Couldn't open display '%s'\n", XDisplayName(NULL));

	screen = DefaultScreen(dpy);
	root = RootWindow(dpy, screen);

	for (;;) {
		len = LENGTH(sfuncs);
		char *sres[len];

		for (int i = 0; i < len; i++)
			sres[i] = (*sfuncs[i])();

		text = strjo(sres, len, (char*)statsep);
		XStoreName(dpy, root, text);

		XFlush(dpy);
		sleep(delay);
	}

	XCloseDisplay(dpy);
	return 0;
}
