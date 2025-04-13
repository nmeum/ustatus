.POSIX:

BIN_NAME = dwm-status
OBJECTS  = $(BIN_NAME).o
HEADERS  = config.h

CFLAGS ?= -O0 -g -Werror
CFLAGS += -D_DEFAULT_SOURCE -std=c99
CFLAGS += -Wpedantic -Wall -Wextra -Wmissing-prototypes \
	  -Wpointer-arith -Wstrict-prototypes -Wshadow

LDFLAGS += -ltinyalsa

$(BIN_NAME): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)
$(OBJECTS): $(HEADERS)
config.h: config.def.h
	cp $< $@

clean:
	rm -f $(BIN_NAME) $(OBJECTS)

.PHONY: clean
