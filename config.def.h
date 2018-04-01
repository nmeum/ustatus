/**
 * Function prototypes
 */

size_t seperator(char *, size_t);
size_t curtime(char *, size_t);
size_t loadavg(char *, size_t);
size_t curvol(char *, size_t);
size_t batcap(char *, size_t);

/**
 * Common settings
 */

/* Array of functions to use in the status bar text. */
static size_t (* const sfuncs[])(char*, size_t) = {
	batcap,
	seperator,
	curvol,
	seperator,
	loadavg,
	seperator,
	curtime,
};

/* Delay (in seconds) used between updates of the status text. */
#define DELAY 5

/* Seperator to use between different status function outputs. */
#define STATSEP " | "

/* Format to use in the curtime function for the current time. */
#define TIMEFMT "%d %b %Y %H:%M:%S"

/**
 * Linux settings
 */

/* Path to power supply battery directory in /sys. */
#define SYSBAT "/sys/class/power_supply/BAT0"

/* Path to control file for current charge. */
#define SYSCUR "charge_now"

/* Path to control file for full charge. */
#define SYSFULL "charge_full_design"

/* Sound card to use for alsa output. */
#define SNDCRD 0

/* Name of the control channel to use for alsa output. */
#define SWTCHNAME "Master Playback Switch"
#define VOLUMNAME "Master Playback Volume"

/**
 * OpenBSD settings
 */

/* Path to the APM device. */
#define APMDEV "/dev/apm"

/* Path to the mixer device. */
#define MIXDEV "/dev/mixer"

/* Name of the audio mixer. */
#define MIXER "master"
