PLATFORM = win32
#PLATFORM = linux

####### DEFAULT SETTINGS #######

CC     = cc -std=c17 -pipe
CFLAGS = -Wpedantic -Wall -Wextra -Wvla -Wshadow -fstrict-aliasing
LDLIBS = -lm 

SOURCE = main.c
TARGET = linksim

####### PLATFORM SPECIFIC STUFF HERE #######

# Windows (MSYS2)
ifeq ($(PLATFORM),win32)
CFLAGS += $$(pkgconf --cflags SDL2)
LDLIBS += $$(pkgconf --libs   SDL2) -lws2_32
endif

# Linux
ifeq ($(PLATFORM),linux)
CFLAGS += $$(pkg-config --cflags sdl2)
LDLIBS += $$(pkg-config --libs   sdl2)
endif

####### RECIPES #######

.c.h:
	touch $@

all:$(TARGET)

$(TARGET):$(SOURCE)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SOURCE) $(LDLIBS)

main.c:lifecycle.h socket.h

clean:
	rm -f $(TARGET)

# Based on this exemplary Makefile:
# https://github.com/pete-gordon/planet-hively/blob/master/makefile
