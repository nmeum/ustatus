#define die(msg)                                                               \
	do {                                                                   \
		perror(msg);                                                   \
		exit(EXIT_FAILURE);                                            \
	} while (0)

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define xsnprintf(DEST, SIZE, FORMAT, ...) \
	MIN(SIZE, snprintf(DEST, SIZE, FORMAT, __VA_ARGS__))
