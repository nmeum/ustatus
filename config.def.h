/* Delay (in seconds) used between updates of the status text. */
static const int delay = 5;

/* Seperator to use between different status function outputs. */
static const char *statsep = " | ";

/* Format to use in the curtime function for the current time. */
static const char *timefmt = "%d %b %Y %H:%M:%S";

/* Array of functions to use in the status bar text.
 * NOTE: You shouldn't add any of these more than once. */
static char* (* const sfuncs[])(void) = {
	curtime,
	curload,
};
