BIN_NAME = dwm-status

OBJECTS = $(BIN_NAME).o
STALIBS = libtinyalsa.a
HEADERS = config.h

X11INC ?= /usr/include/X11
X11LIB ?= /usr/lib/X11

CFLAGS ?= -g -O0 -pedantic -Wall
CFLAGS += -D_BSD_SOURCE -std=c99 -Itinyalsa/include -I$(X11INC)

CC      ?= gcc
LDFLAGS += -L$(X11LIB) -lX11 $(STALIBS)

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(BIN_NAME)
$(BIN_NAME): $(OBJECTS) $(STALIBS)
	$(CC) -o $@ $^ $(LDFLAGS)

libtinyalsa.a: tinyalsa
	$(MAKE) -C $<
	cp tinyalsa/$@ $@

config.h: config.def.h
	cp $< $@

clean: tinyalsa
	$(MAKE) -C $< clean
	rm -f $(BIN_NAME) $(OBJECTS) $(STALIBS)
