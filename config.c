#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/socket.h>
#include <unistd.h>

#include <SDL.h>

#include "config.h"
#include "upper.h"

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

    config = (struct config){{sockets[0], sockets[1]}, upper_init(argv[1])};

    atexit(config_quit);

    return config;
}

static void config_quit(void)
{
    upper_quit(&(config.upper));

    close(config.sockets[0]);
    close(config.sockets[1]);

#ifdef _WIN32
    WSACleanup();
#endif

#if SDL_VERSION_ATLEAST(2,0,16)
    SDL_TLSCleanup();
#endif
}
