#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include <sys/types.h>
#include <X11/Xlib.h>

#include "util.h"
#include "config.h"

enum {
	STATUSSZ = 128,
};

static char ststr[STATUSSZ];

size_t
loadavg(char* dest, size_t n)
{
	double avgs[3];

	if (!getloadavg(avgs, 3))
		die("getloadavg failed");

	return xsnprintf(dest, n, "%.2f %.2f %.2f",
		avgs[0], avgs[1], avgs[2]);
}

size_t
curtime(char *dest, size_t n)
{
	time_t tim;
	struct tm *timtm;

	if ((tim = time(NULL)) == (time_t)-1)
		die("time failed");

	if (!(timtm = localtime(&tim)))
		die("localtime failed");

	return strftime(dest, n, TIMEFMT, timtm);
}

size_t
seperator(char *dest, size_t n)
{
	return stpncpy(dest, STATSEP, n) - dest;
}

int
main(void)
{
	Display *dpy;
	Window root;
	size_t i, x, fns, max;

	if (!(dpy = XOpenDisplay(NULL))) {
		fprintf(stderr, "XOpenDisplay failed\n");
		return EXIT_FAILURE;
	}
	root = DefaultRootWindow(dpy);

	max = STATUSSZ - 1;
	fns = sizeof(sfuncs) / sizeof(sfuncs[0]);

	for (;;) {
		memset(ststr, '\0', STATUSSZ);
		for (i = 0, x = 0; i < fns && x < max; i++)
			x += (*sfuncs[i])(&(ststr[x]), max - x);

		assert(x < STATUSSZ);
		ststr[x] = '\0';

		XStoreName(dpy, root, ststr);
		XSync(dpy, False);
		sleep(DELAY);
	}

	XCloseDisplay(dpy);
	return EXIT_SUCCESS;
}
