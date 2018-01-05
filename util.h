#define die(msg)                                                               \
	do {                                                                   \
		perror(msg);                                                   \
		exit(EXIT_FAILURE);                                            \
	} while (0)

#define MAX(A, B) ((A) > (B) ? (A) : (B))
#define MIN(A, B) ((A) < (B) ? (A) : (B))

#define zsnprintf(DEST, SIZE, FORMAT, ...) \
	(unsigned int)MAX(0, snprintf(DEST, SIZE, FORMAT, __VA_ARGS__))
#define xsnprintf(DEST, SIZE, FORMAT, ...) \
	MIN(SIZE, zsnprintf(DEST, SIZE, FORMAT, __VA_ARGS__))
