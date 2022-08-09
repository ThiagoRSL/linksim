#ifndef __UPPER_H__
#define __UPPER_H__

#include <stdio.h>
#include <stddef.h>

#include "buffer.h"

struct upper
{
    unsigned char *data;
    size_t         size;
    size_t         read;
    size_t         written;
};

void upper_init (struct upper uppers[2], char *paths[2], struct buffer *buffer);
int  upper_read (struct upper *upper, unsigned char **excerpt, size_t length);
int  upper_write(struct upper *upper, unsigned char  *excerpt, size_t length);

// Module emcompasses layers above device of link layer.
// One instance reads data for a device, and a copy on the other end checks it.
// This makes two instances per device: one source and a copy of the other end.
// Second in `paths` of `upper_init()` can be NULL for receiver only device.
// `upper_init()` reads `paths` entirely for transfer.
// `upper_read()` makes `excerpt` point to `upper.data` appropriately.
// `upper_read()` returns length of `excerpt`, possibly less than `length`.
// `upper_read()` returns zero and sets `excerpt` to NULL on EOF.
// `upper_write()` returns bytes remaining to be written.
// `upper_write()` negates its returned value when `excerpt` doesn't match.
// `upper_write()` returns zero on EOF, further calls aborts the program.

#endif
