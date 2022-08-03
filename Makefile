CC=cc -std=c17 -pipe

CFLAGS=-Wpedantic -Wall -Wextra -Wvla -Wshadow -fstrict-aliasing $$(pkgconf --cflags SDL2)
LDLIBS=-lm $$(pkgconf --libs SDL2)

SOURCE=main.c
TARGET=linksim

all:$(TARGET)

$(TARGET):$(SOURCE)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SOURCE) $(LDLIBS)

main.c:lifecycle.h
	touch main.c

clean:
	rm -f $(TARGET)
