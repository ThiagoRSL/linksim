#ifndef __UPPER_H__
#define __UPPER_H__

struct upper
{
    unsigned char *data;
    int            size;
    int            read;
    int            written;
};

void upper_init (struct upper *upper, char const *filepath);
void upper_quit (struct upper *upper);
int  upper_read (struct upper *upper, unsigned char **excerpt);
int  upper_write(struct upper *upper, unsigned char  *excerpt, int length);

// `upper_read()`  returns length of `excerpt`.
// `upper_write()` returns bytes remaining to be written, negated on error.
// On both `upper_read()` and `upper_write()` zero means end.

#endif
