#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>

#include "sys/types.h"

#include "config.h"
#include "util.h"

double
readnum(char *bfp, char *fn)
{
	FILE *file;
	size_t rclen;
	char buf[16], fp[PATH_MAX], *rc;

	rc = NULL;
	snprintf(fp, PATH_MAX, "%s/%s", bfp, fn);

	if (!(file = fopen(fp, "r")))
		die("fopen failed");

	if (!(rc = fgets(buf, 16, file)))
		die("fgets failed");

	rclen = strlen(buf);
	if (rc[rclen - 1] == '\n')
		rc[rclen - 1] = '\0';

	fclose(file);
	return atof(rc);
}

size_t
batcap(char *dest, size_t n)
{
	double res, curc, maxc;

	curc = readnum(SYSBAT, SYSCUR);
	maxc = readnum(SYSBAT, SYSFULL);

	res = 100.0 * (curc / maxc);
	return xsnprintf(dest, n, "%.2f%%", res);
}
