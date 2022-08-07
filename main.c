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
    struct upper   upper;
    unsigned char *vessel;
    int            fdSender;
    int            fdReceiver;
    int            wrote_with_error;
};

#define MIN(A, B) ((A) < (B) ? (A) : (B))

#define FRAME_SIZE 500

static int FilmeSender(void *ptr)
{
    assert(ptr != NULL);

    struct filme filme = *(struct filme *)ptr;

    int total_sent     = 0;
    int tries_left = 500;
    int unavailable = 10;

    int            frame   = 0;
    unsigned char *excerpt = NULL;

    while ((frame = upper_read(&filme.upper, &excerpt)))
    {
        int count = send(filme.fdSender, (void *)excerpt, frame, 0);

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

    int bytes_left = filme.upper.size;

    do
    {
        SDL_Delay(1);

        int count = recv(filme.fdReceiver, (void *)filme.vessel, bytes_left, 0);

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

            int remaining = upper_write(&filme.upper, filme.vessel, count);

            if (remaining < 0)
            {
                filme.wrote_with_error = 1;
                bytes_left = -remaining;
            }
            else
            {
                bytes_left = remaining;
            }

            total_received += count;

            assert(bytes_left + total_received == filme.upper.size);
        }
    }
    while (bytes_left > 0);

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
    
    struct filme filme = {config.upper, buffer2, config.sockets[0], config.sockets[1], 0};

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

    if (filme.wrote_with_error)
    {
        printf("Contents are different!!!!\n");
    }
    else
    {
        printf("Both sides have the same thing!!!!\n");
    }

    free(buffer2);

    return 0;
}
