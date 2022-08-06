PLATFORM = win32
#PLATFORM = linux

####### DEFAULT SETTINGS #######

CC     = cc -std=c17 -pipe -g
CFLAGS = -Wpedantic -Wall -Wextra -Wvla -Wshadow -fstrict-aliasing
LDLIBS = -lm 

SDLCFLAGS =
SDLLDLIBS =

SOURCE = main.c
TARGET = linksim
OBJECT = config.o upper.o

####### PLATFORM SPECIFIC STUFF HERE #######

# Windows (MSYS2)
ifeq ($(PLATFORM),win32)
CFLAGS += $$(pkgconf --cflags SDL2) -I .
LDLIBS += $$(pkgconf --libs   SDL2) -lws2_32
TARGET  = linksim.exe
OBJECT += socket.o
endif

# Linux
ifeq ($(PLATFORM),linux)
CFLAGS += $$(pkg-config --cflags sdl2)
LDLIBS += $$(pkg-config --libs   sdl2)
endif

####### RECIPES #######

all:$(TARGET)

clean:
	rm -f $(TARGET) $(OBJECT)

$(TARGET):$(SOURCE) $(OBJECT)
	$(CC) $(CFLAGS) $(SDLCFLAGS) -o $@ $(SOURCE) $(OBJECT) $(LDLIBS) $(SDLLDLIBS)

main.c:config.h config.h upper.h
	touch $@
config.o:config.c config.h upper.h
upper.o:upper.c upper.h

####### PLATFORM SPECIFIC RECIPES #######

socket.o:socket.c sys/socket.h fcntl.h
	$(CC) $(CFLAGS) -c $<

# Based on this exemplary Makefile:
# https://github.com/pete-gordon/planet-hively/blob/master/makefile
