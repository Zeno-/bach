# Configuration

PROGNAME := zenocpu

# Library dependencies. You can use pkg-config/sdl-config to get these.
INCLUDES := -I/usr/include/SDL -D_REENTRANT -D_GNU_SOURCE
LIBS := -lSDL

# Whatever CFLAGS/LDFLAGS you want to use.
CFLAGS := -Wall -O2 $(INCLUDES)
LDFLAGS := -s $(LIBS)

# --------------------------------------------------------------------------

CFLAGS += -I. -Icommon -Ivpu/backend -Ivpu/fonts $(INCLUDES) \
	  -DPROGNAME=\"$(PROGNAME)\"

OBJECTS := \
	vpu/backend/base.o	\
	vpu/backend/render.o	\
	vpu/backend/text.o	\
	vpu/fonts/bmfonts.o	\
	vpu/tests/test.o	\
	vpu/tests/genterrain.o

.c.o:
	$(CC) -o $@ -c $(CFLAGS) $<

all: $(OBJECTS)
	$(CC) -o $(PROGNAME) $(CFLAGS) $(OBJECTS) $(LDFLAGS)

clean:
	rm -f $(OBJECTS) $(PROGNAME)

.PHONY:
	all clean
	