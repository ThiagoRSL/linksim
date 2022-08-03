CC=cc -std=c17 -pipe

CFLAGS=-Wpedantic -Wall -Wextra -Wvla -Wshadow -fstrict-aliasing $$(pkgconf --cflags SDL2 SDL2_net)
LDLIBS=-lm $$(pkgconf --libs SDL2 SDL2_net)

SOURCE=main.c
TARGET=sender receiver

all:$(TARGET)

$(TARGET):t2
	cp t2 $@

t2:$(SOURCE)
	$(CC) $(CFLAGS) $(LDFLAGS) -o t2 $(SOURCE) $(LDLIBS)

main.c:lifecycle.h
	touch main.c

clean:
	rm -f t2 $(TARGET)
