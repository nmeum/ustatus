/* See LICENSE for license details. */

#include <assert.h>
#include <err.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>

#include <sys/types.h>
#include <tinyalsa/asoundlib.h>

static size_t alsavol(char*, size_t);
static size_t batcap(char*, size_t);
static size_t loadavg(char*, size_t);
static size_t curtime(char*, size_t);
static size_t separator(char*, size_t);

#include "config.h"

enum {
	STATUSSZ = 128,
};

static char ststr[STATUSSZ];

static int
xsnprintf(char *restrict s, size_t n, const char *restrict fmt, ...)
{
	int ret;
	va_list ap;

	va_start(ap, fmt);
	ret = vsnprintf(s, n, fmt, ap);
	va_end(ap);

	if (ret < 0) {
		err(EXIT_FAILURE, "snprintf failed");
	} else if ((size_t)ret >= n) {
		warnx("snprintf: insufficient buffer size");
		ret = n;
	}

	return ret;
}

static double
readnum(char *bfp, char *fn)
{
	int ret;
	FILE *file;
	size_t rclen;
	char buf[16], fp[PATH_MAX + 1], *rc;

	rc = NULL;
	if ((ret = snprintf(fp, sizeof(fp), "%s/%s", bfp, fn)) < 0)
		errx(EXIT_FAILURE, "snprintf failed");
	else if ((size_t)ret >= sizeof(fp))
		errx(EXIT_FAILURE, "buffer 'fp' is too short");

	if (!(file = fopen(fp, "r")))
		err(EXIT_FAILURE, "couldn't open '%s'", fp);

	if (!(rc = fgets(buf, 16, file))) {
		fclose(file);
		errx(EXIT_FAILURE, "'%s' seems to be empty", fp);
	}

	rclen = strlen(buf);
	if (rc[rclen - 1] == '\n')
		rc[rclen - 1] = '\0';

	fclose(file);
	return atof(rc);
}

static size_t
actlstr(char *buf, size_t n, char *ch, struct mixer *mx) {
	size_t ret;
	char *status;
	struct mixer_ctl *ctl;

	if (!(ctl = mixer_get_ctl_by_name(mx, ch))) {
		mixer_close(mx);
		errx(EXIT_FAILURE, "couldn't find mixer ctl '%s'\n", ch);
	}

	switch (mixer_ctl_get_type(ctl)) {
	case MIXER_CTL_TYPE_INT:
		return xsnprintf(buf, n, "%d%%",
			mixer_ctl_get_percent(ctl, 0));
	case MIXER_CTL_TYPE_BOOL:
		status = mixer_ctl_get_value(ctl, 0) ? "On" : "Off";
		ret = stpncpy(buf, status, n) - buf;
		break;
	default:
		mixer_close(mx);
		errx(EXIT_FAILURE, "unsupported ctl type '%s'\n",
			mixer_ctl_get_type_string(ctl));
	};

	return ret;
}

static size_t
batcap(char *dest, size_t n)
{
	int cap;

	cap = readnum((char*)sysbat, "capacity");
	return xsnprintf(dest, n, "%d%%", cap);
}

static size_t
alsavol(char *dest, size_t n)
{
	size_t ret;
	struct mixer *mx;

	if (!(mx = mixer_open(sndcrd)))
		errx(EXIT_FAILURE, "couldn't open mixer for card %d\n", sndcrd);

	ret = actlstr(dest, n, (char*)swtchname, mx);
	if (strcmp(dest, "Off"))
		ret = actlstr(dest, n, (char*)volumname, mx);

	mixer_close(mx);
	return ret;
}

static size_t
loadavg(char* dest, size_t n)
{
	double avgs[3];

	if (!getloadavg(avgs, 3))
		err(EXIT_FAILURE, "getloadavg failed");

	return xsnprintf(dest, n, "%.2f %.2f %.2f",
		avgs[0], avgs[1], avgs[2]);
}

static size_t
curtime(char *dest, size_t n)
{
	time_t tim;
	struct tm *timtm;

	if ((tim = time(NULL)) == (time_t)-1)
		err(EXIT_FAILURE, "time failed");

	if (!(timtm = localtime(&tim)))
		err(EXIT_FAILURE, "localtime failed");

	return strftime(dest, n, timefmt, timtm);
}

static size_t
separator(char *dest, size_t n)
{
	return stpncpy(dest, statsep, n) - dest;
}

int
main(void)
{
	size_t i, x, fns, max;

	max = STATUSSZ - 1;
	fns = sizeof(sfuncs) / sizeof(sfuncs[0]);

	for (;;) {
		for (i = 0, x = 0; i < fns && x < max; i++) {
			assert(max >= x);
			x += (*sfuncs[i])(&(ststr[x]), max - x);
		}

		assert(x < STATUSSZ);
		ststr[x] = '\0';

		printf("%s\n", ststr);
		fflush(stdout);
		sleep(delay);
	}

	return EXIT_SUCCESS;
}
