# See LICENSE for license details.
BIN_NAME = dwm-status

OBJECTS = $(BIN_NAME).o
HEADERS = config.h

CFLAGS ?= -O0 -g -Werror
CFLAGS += -D_DEFAULT_SOURCE -std=c99
CFLAGS += -Wpedantic -Wall -Wextra \
	      -Wmissing-prototypes -Wpointer-arith \
	      -Wstrict-prototypes -Wshadow

LDFLAGS += -ltinyalsa

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< $(CPPFLAGS) $(CFLAGS)

all: $(BIN_NAME)
$(BIN_NAME): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

config.h: config.def.h
	cp $< $@

clean:
	$(RM) $(BIN_NAME) $(OBJECTS)
