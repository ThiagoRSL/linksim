#ifndef __LAYERS_H__
#define __LAYERS_H__

#include "rnd.h"

#define MAX_STREAM 1000

struct upper
{
    unsigned char *data;
    size_t         size;
    size_t         read;
    size_t         written;
};

struct physical
{
    int fd;
    rnd_pcg_t random;
    unsigned char *streams[MAX_STREAM];
    size_t sizes[MAX_STREAM];
    uint64_t timeout[MAX_STREAM];
    int total_streams;
};

struct link
{
    struct upper *here;
    struct upper *there;
    struct physical physical;

    int frame_size;
    int received;
};

#endif