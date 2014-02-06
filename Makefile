# Configuration

PROGNAME := bach

# Library dependencies. You can use pkg-config/sdl-config to get these.
INCLUDES := -I/usr/include/SDL -D_REENTRANT -D_GNU_SOURCE
LIBS := -lSDL -llua

# Whatever CFLAGS/LDFLAGS you want to use.
CFLAGS := -Wall -O2 $(INCLUDES)
LDFLAGS := -s $(LIBS)

# --------------------------------------------------------------------------

CFLAGS += -I. $(INCLUDES) \
	  -DPROGNAME=\"$(PROGNAME)\"

OBJECTS := \
    main.o \
    machine.o \
    vpu/fonts/bmfonts.o \
    vpu/render.o \
    vpu/text.o \
    vpu/tests/test_basiccolours.o \
    vpu/tests/test_scroll.o \
    vpu/tests/test_genterrain.o \
    hal/sdl/hal_init.o \
    hal/sdl/hal_timer.o \
    hal/sdl/video.o \
    hal/sdl/hal_events.o \
    lua/lua_iface.o
.c.o:
	$(CC) -o $@ -c $(CFLAGS) $<

all: $(OBJECTS)
	$(CC) -o $(PROGNAME) $(CFLAGS) $(OBJECTS) $(LDFLAGS)

clean:
	rm -f $(OBJECTS) $(PROGNAME)

.PHONY:
	all clean
	