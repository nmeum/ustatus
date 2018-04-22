BIN_NAME = dwm-status
SRCFILES = $(BIN_NAME).c

SRCFILES += $(wildcard compat/$(shell uname -s)/*.c)
include compat/$(shell uname -s)/config.mk

OBJECTS = $(SRCFILES:%.c=%.o)
HEADERS = config.h util.h

CFLAGS ?= -Os -pedantic -Wall -Wextra -Wmissing-prototypes
CFLAGS += -D_BSD_SOURCE -std=c99 -I$(X11INC) -I.

LDFLAGS += -L$(X11LIB) -lX11

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(BIN_NAME)
$(BIN_NAME): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

config.h: config.def.h
	cp $< $@

clean:
	$(RM) $(BIN_NAME) $(OBJECTS)

.PHONY: all clean
