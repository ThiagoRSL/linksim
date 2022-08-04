#ifndef __LIFECYCLE_H__
#define __LIFECYCLE_H__

#include <SDL.h>

#include "socket.h"

static void lifecycle_begin(int argc, char *argv[], int sockets[2])
{
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

    for (int socket_i = 0; socket_i < 2; socket_i++)
    {
        if (-1 == (sockets[socket_i] = socket(AF_UNIX, SOCK_STREAM, 0)))
        {
            perror(NULL);
            exit(EXIT_FAILURE);
        }

        if (fcntl(sockets[socket_i], F_SETFL, O_NONBLOCK) != 0)
        {
            perror(NULL);
            exit(EXIT_FAILURE);
        }
    }

    (void)argc;
    (void)argv;
}

static void lifecycle_end(int sockets[2])
{
    close(sockets[0]);
    close(sockets[1]);

#ifdef _WIN32
    WSACleanup();
#endif
}

#endif
