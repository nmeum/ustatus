#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/audioio.h>

#include "config.h"
#include "util.h"

/* post-ironic macro magic for fun and profit. */
#define EACHDEV(FD, DEVINFO, MAX) \
	for (DEVINFO.index = 0; DEVINFO.index < MAX; DEVINFO.index++)   \
		if (ioctl(FD, AUDIO_MIXER_DEVINFO, &DEVINFO) == -1)     \
			die("ioctl AUDIO_MIXER_DEVINFO failed");        \
		else                                                    \

static int
offord(struct audio_mixer_enum *e)
{
	int i;

	for (i = 0; i < e->num_mem; i++) {
		if (!strcmp(e->member[i].label.name, AudioNoff))
			return e->member[i].ord;
	}

	return -1;
}

static int
outmix(int fd, int ndev, mixer_devinfo_t *out)
{
	int cs;
	mixer_devinfo_t dinfo;

	cs = -1;
	EACHDEV(fd, dinfo, ndev) {
		if (dinfo.type == AUDIO_MIXER_CLASS &&
				!strcmp(dinfo.label.name, AudioCoutputs)) {
			cs = dinfo.index;
			break;
		}
	}

	if (cs == -1)
		return -1;

	EACHDEV(fd, dinfo, ndev) {
		if (dinfo.mixer_class == cs &&
				!strcmp(dinfo.label.name, mixer)) {
			memcpy(out, &dinfo, sizeof(dinfo));
			return 0;
		}
	}

	return -1;
}

static void
findctl(int fd, int *val, int *enu, int *ord)
{
	int ndev;
	mixer_devinfo_t dinfo;

	*enu = *val = -1;
	for (ndev = 0; ; ndev++) {
		dinfo.index = ndev;
		if (ioctl(fd, AUDIO_MIXER_DEVINFO, &dinfo) == -1)
			break;
	}

	if (outmix(fd, ndev, &dinfo) == -1) {
		fprintf(stderr, "no output mixer named '%s' exists\n", mixer);
		exit(EXIT_FAILURE);
	}

	for (; dinfo.next != AUDIO_MIXER_LAST; dinfo.index = dinfo.next) {
		if (ioctl(fd, AUDIO_MIXER_DEVINFO, &dinfo) == -1)
			die("ioctl AUDIO_MIXER_DEVINFO failed");

		switch (dinfo.type) {
		case AUDIO_MIXER_ENUM:
			if ((*ord = offord(&dinfo.un.e)) == -1)
				break;
			*enu = dinfo.index;
			break;
		case AUDIO_MIXER_VALUE:
			*val = dinfo.index;
			break;
		}
	}

	if (*val == -1) {
		fprintf(stderr, "mixer '%s' has no AUDIO_MIXER_VALUE device\n", mixer);
		exit(EXIT_FAILURE);
	}
}

size_t
curvol(char *dest, size_t n)
{
	int fd, enu, val, off;
	mixer_ctrl_t mctl;

	if ((fd = open(mixdev, O_RDONLY)) == -1)
		die("open failed");

	off = 0;
	findctl(fd, &val, &enu, &off);

	if (enu != -1) {
		mctl.dev = enu;
		if (ioctl(fd, AUDIO_MIXER_READ, &mctl) == -1)
			die("ioctl AUDIO_MIXER_READ failed for AUDIO_MIXER_ENUM");

		if (mctl.un.ord != off) {
			close(fd);
			return xsnprintf(dest, n, "%s", "Off");
		}
	}

	mctl.dev = val;
	if (ioctl(fd, AUDIO_MIXER_READ, &mctl) == -1)
		die("ioctl AUDIO_MIXER_READ failed for AUDIO_MIXER_VALUE");

	close(fd);
	return xsnprintf(dest, n, "%d%%",
		mctl.un.value.level[0] * 100 / 255);
}
