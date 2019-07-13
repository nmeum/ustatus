/* See LICENSE for license details. */

/* Delay (in seconds) used between updates of the status text. */
static const int delay = 5;

/* Seperator to use between different status function outputs. */
static const char *statsep = " | ";

/* Format to use in the curtime function for the current time. */
static const char *timefmt = "%d %b %Y %H:%M:%S";

/* Path to power supply battery directory in /sys. */
static const char *sysbat = "/sys/class/power_supply/BAT0";

/* Path to control file for current charge. */
static const char *syscur = "charge_now";

/* Path to control file for full charge. */
static const char *sysfull = "charge_full_design";

/* Sound card to use for alsa output. */
static const unsigned int sndcrd = 0;

/* Name of the control channel to use for alsa output. */
static const char* swtchname = "Master Playback Switch";
static const char* volumname = "Master Playback Volume";

/* Array of functions to use in the status bar text. */
static size_t (* const sfuncs[])(char*, size_t) = {
	batcap,
	seperator,
	alsavol,
	seperator,
	loadavg,
	seperator,
	curtime,
};
