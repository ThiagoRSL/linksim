PLATFORM = win32
#PLATFORM = linux

####### DEFAULT SETTINGS #######

CC     = cc -std=c17 -pipe
CFLAGS = -O2 -flto -Wpedantic -Wall -Wextra -Wvla -Wshadow -fstrict-aliasing
LDLIBS = -lm 

SDLCFLAGS =
SDLLDLIBS =

OBJECT = main.o config.o link.o upper.o
TARGET = linksim

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

$(TARGET):$(OBJECT)
	$(CC) $(CFLAGS) $(SDLCFLAGS) -o $@ $(OBJECT) $(LDLIBS) $(SDLLDLIBS)

main.o:config.h config.h main.c upper.h shenanigans.h
config.o:config.c config.h upper.h shenanigans.h buffer.h
link.o:link.c link.h upper.h shenanigans.h
upper.o:upper.c upper.h buffer.h files.h

####### PLATFORM SPECIFIC RECIPES #######

socket.o:fcntl.h socket.c sys/socket.h unistd.h shenanigans.h

# Based on this exemplary Makefile:
# https://github.com/pete-gordon/planet-hively/blob/master/makefile
