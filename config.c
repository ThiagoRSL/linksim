#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/socket.h>
#include <unistd.h>

#include <SDL.h>

#include "config.h"

#define BUFFER_INCREMENT 200000

static struct config config;

static void config_quit(void);

struct config config_init(int argc, char *argv[])
{
    if (argc != 2 || '\0' == argv[1][0])
    {
        printf("USAGE: linksim[.exe] <file>\n");
	exit(EXIT_FAILURE);
    }

    atexit(SDL_Quit);

    if (SDL_Init(SDL_INIT_TIMER) != 0)
    {
        SDL_Log("SDL initialization failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

#ifdef _WIN32
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0)
    {
        SDL_Log("Failed network initialization.\n");
        exit(EXIT_FAILURE);
    }
#endif

    int sockets[2] = {-1, -1};

    if (-1 == socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0, sockets))
    {
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    unsigned char *buffer = NULL;
    int buffer_size       = 0;
    int buffer_capacity   = 0;
    int tries_left        = 500;
    int result            = -1;

    FILE *file = fopen(argv[1], "rb");

    if (NULL == file)
    {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    enum { READ_NORMAL, READ_LOW_COUNT, READ_LOW_COUNT_AGAIN } read_state = READ_NORMAL;

    do
    {
        unsigned char *buffer_new = realloc(buffer, (buffer_capacity = buffer_size + BUFFER_INCREMENT));

        if (NULL == buffer_new)
        {
            free(buffer);
            perror(NULL);
            exit(EXIT_FAILURE);
        }

        buffer = buffer_new;

        result = fread(buffer + buffer_size, 1, BUFFER_INCREMENT, file);

	printf("I got %d for this fread()!\n", result);

        if (result < BUFFER_INCREMENT)
        {
            if (ferror(file))
            {
                if (tries_left > 0)
                {
                    clearerr(file);
                    tries_left--;
                }
                else
                {
                    fprintf(stderr, "Failed to read stdin.\n");
                    exit(EXIT_FAILURE);
                }
            }

	    read_state++;
        }
	else
	{
            read_state = READ_NORMAL;
	}

        buffer_size += result;
    }
    while (BUFFER_INCREMENT == result || read_state != READ_LOW_COUNT_AGAIN || !feof(file)); 

    if (EOF == fclose(file))
    {
        printf("Closed %s with error! But that's okay!\n", argv[1]);
    }

    printf("I've read %d bytes! %d errors while reading!\n", buffer_size, 500 - tries_left);

    config = (struct config){{sockets[0], sockets[1]}, buffer, buffer_size};

    atexit(config_quit);

    return config;
}

static void config_quit(void)
{
    free(config.buffer);

    config.buffer = NULL;

    close(config.sockets[0]);
    close(config.sockets[1]);

#ifdef _WIN32
    WSACleanup();
#endif

#if SDL_VERSION_ATLEAST(2,0,16)
    SDL_TLSCleanup();
#endif
}
