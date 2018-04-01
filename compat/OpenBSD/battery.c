#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/ioctl.h>
#include <machine/apmvar.h>

#include "config.h"
#include "util.h"

size_t
batcap(char *dest, size_t n)
{
	struct apm_power_info info;
	int fd;

	if ((fd = open(APMDEV, O_RDONLY)) == -1)
		die("open failed");

	if (ioctl(fd, APM_IOC_GETPOWER, &info) == -1)
		die("ioctl APM_IOC_GETPOWER failed");

	close(fd);
	return xsnprintf(dest, n, "%d", info.battery_life);
}
