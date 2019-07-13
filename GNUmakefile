# See LICENSE for license details.
BIN_NAME = dwm-status

OBJECTS = $(BIN_NAME).o
HEADERS = config.h

X11INC ?= /usr/include/X11
X11LIB ?= /usr/lib/X11

CFLAGS ?= -O0 -g -Werror
CFLAGS += -D_BSD_SOURCE -std=c99
CFLAGS += -Wpedantic -Wall -Wextra \
	      -Wmissing-prototypes -Wpointer-arith \
	      -Wstrict-prototypes -Wshadow

CPPFLAGS += -I$(X11INC)
LDFLAGS  += -L$(X11LIB) -lX11 -ltinyalsa

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< $(CPPFLAGS) $(CFLAGS)

all: $(BIN_NAME)
$(BIN_NAME): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

config.h: config.def.h
	cp $< $@

clean:
	$(RM) $(BIN_NAME) $(OBJECTS)
