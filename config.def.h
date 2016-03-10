/* Delay used between changes of the status text. */
static const int delay = 5;

/* Format to use in the curtime function for the current time. */
static const char *timefmt = "%d %b %Y %H:%M:%S";

/* Array of functions to use in the status bar text. */
static char* (* const sfuncs[])(void) = {
	timefmt,
};
