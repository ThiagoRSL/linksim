#ifndef __LIFECYCLE_H__
#define __LIFECYCLE_H__

#include <assert.h>

#include <stdlib.h>
#include <stdio.h>

#include <sys/socket.h>
#include <unistd.h>

#include <SDL.h>

#define BUFFER_INCREMENT 200000

static void lifecycle_begin(int argc, char *argv[], int sockets[2], unsigned char **buffer, int *buffer_size)
{
    assert(buffer      != NULL);
    assert(buffer_size != NULL);
    assert(NULL == *buffer);

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

    if (-1 == socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0, sockets))
    {
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    *buffer             = NULL;
    int buffer_capacity = 0;
    int tries_left      = 500;
    int result          = -1;

    do
    {
        unsigned char *buffer_new = realloc(*buffer, (buffer_capacity = *buffer_size + BUFFER_INCREMENT));

        if (NULL == buffer_new)
        {
            free(*buffer);
            perror(NULL);
            exit(EXIT_FAILURE);
        }

        *buffer = buffer_new;

        result = fread(*buffer + *buffer_size, 1, BUFFER_INCREMENT, stdin);

        if (result < BUFFER_INCREMENT)
        {
            if (ferror(stdin))
            {
                if (tries_left > 0)
                {
                    clearerr(stdin);
                    tries_left--;
                }
                else
                {
                    fprintf(stderr, "Failed to read stdin.\n");
                    exit(EXIT_FAILURE);
                }
            }
        }

        *buffer_size += result;
    }
    while (BUFFER_INCREMENT == result || !feof(stdin)); 

    (void)argc;
    (void)argv;
}

static void lifecycle_end(int sockets[2], unsigned char **buffer)
{
    free(*buffer);

    *buffer = NULL;

    close(sockets[0]);
    close(sockets[1]);

#ifdef _WIN32
    WSACleanup();
#endif
}

#endif
