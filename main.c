#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/socket.h>

#include <SDL.h>

#include "config.h"

struct filme
{
    int            buffer_size;
    unsigned char *original;
    unsigned char *vessel;
    int            fdSender;
    int            fdReceiver;
};

#define MIN(A, B) ((A) < (B) ? (A) : (B))

#define FRAME_SIZE 500

static int FilmeSender(void *ptr)
{
    assert(ptr != NULL);

    struct filme filme = *(struct filme *)ptr;

    int total_sent     = 0;
    int tries_left = 100;
    int unavailable = 10;

    while (total_sent != filme.buffer_size)
    {
        int count = send(filme.fdSender, filme.original + total_sent, MIN(FRAME_SIZE, filme.buffer_size - total_sent) , 0);

        if (-1 == count)
        {
            perror(NULL);

            if (EAGAIN == errno || EWOULDBLOCK == errno)
            {
                unavailable--;

                if (unavailable <= 0)
                {
                    SDL_Delay(100);
                    unavailable = 10;
                }
            }
            else
            {
                fprintf(stderr, "Failure on socket send (fd %d)!\n", filme.fdSender);

                tries_left--;

                if (tries_left <= 0)
                {
                    fprintf(stderr, "Bailing out (fd %d)!\n", filme.fdSender);
                    exit(EXIT_FAILURE);
                }
            }
        }
        else
        {
            printf("Thread with fd %d sent %d bytes.\n", filme.fdSender, count);

            total_sent += count;
        }

        SDL_Delay(1);
    }

    printf("Sent %d bytes! My fd is %d.\n", total_sent, filme.fdSender);

    return filme.fdSender;
}

static int FilmeReceiver(void *ptr)
{
    assert(ptr != NULL);

    struct filme filme = *(struct filme *)ptr;

    int total_received = 0;
    int tries_left = 100;

    while (total_received != filme.buffer_size)
    {
        SDL_Delay(1);

        int count = recv(filme.fdReceiver, filme.vessel + total_received, filme.buffer_size - total_received, 0);

        if (-1 == count)
        {
            perror(NULL);

            if (!(EAGAIN == errno || EWOULDBLOCK == errno))
            {
                fprintf(stderr, "Failure on socket receive (fd %d)!\n", filme.fdReceiver);

                tries_left--;

                if (0 == tries_left)
                {
                    fprintf(stderr, "Bailing out (fd %d)!\n", filme.fdReceiver);
                    exit(EXIT_FAILURE);
                }
            }
        }
        else
        {
            printf("Thread with fd %d received %d bytes.\n", filme.fdReceiver, count);

            total_received += count;
        }
    }

    printf("Received %d bytes! My fd is %d.\n", total_received, filme.fdReceiver);

    return filme.fdReceiver;
}

int main(int argc, char *argv[])
{
    printf("Hey!\n");

    struct config config = config_init(argc, argv);

    assert(config.upper.size > 0);

    unsigned char *buffer2 = malloc(config.upper.size);

    if (NULL == buffer2)
    {
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    printf("fds = %d and %d.\n", config.sockets[0], config.sockets[1]);

    SDL_Thread *thread1 = NULL;
    SDL_Thread *thread2 = NULL;
    
    struct filme filme = {config.upper.size, config.upper.data, buffer2, config.sockets[0], config.sockets[1]};

    if (NULL == (thread1 = SDL_CreateThread(FilmeSender, "sender", &filme)))
    {
        SDL_Log("Thread creation failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    if (NULL == (thread2 = SDL_CreateThread(FilmeReceiver, "receiver", &filme)))
    {
        SDL_Log("Thread creation failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_WaitThread(thread1, NULL);
    SDL_WaitThread(thread2, NULL);

    if (!memcmp(buffer2, config.upper.data, config.upper.size))
    {
        printf("Both sides have the same thing!!!!\n");
    }
    else
    {
        printf("Contents are different!!!!\n");
    }

    free(buffer2);

    return 0;
}
