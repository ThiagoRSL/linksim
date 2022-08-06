#ifndef __UPPER_H__
#define __UPPER_H__

struct upper
{
    unsigned char *data;
    int            size;
};

struct upper upper_init(char const *filepath);
void         upper_quit(struct upper *upper);

#endif
