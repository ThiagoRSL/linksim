#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "upper.h"

struct config
{
    int sockets[2];
    struct upper upper;
};

struct config config_init(int argc, char *argv[]);

#endif // __CONFIG_H__
