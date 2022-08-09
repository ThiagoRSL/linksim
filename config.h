#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "upper.h"

struct config
{
    int           sockets[2];
    struct upper  uppers[2];
    struct buffer buffer;
};

struct config config_init(int argc, char *argv[]);

#endif // __CONFIG_H__
