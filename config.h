#ifndef __CONFIG_H__
#define __CONFIG_H__

struct config
{
    int sockets[2];
    unsigned char *buffer;
    int buffer_size;
};

struct config config_init(int argc, char *argv[]);

#endif // __CONFIG_H__
