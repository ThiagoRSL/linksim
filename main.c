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
    int            n_error;
};

static int FilmeSender(void *ptr)
{
    assert(ptr != NULL);

    struct filme filme = *(struct filme *)ptr;

    int            frame   = 0;
    unsigned char *excerpt = NULL;

    while ((frame = upper_read(&filme.upper, &excerpt, 500)))
    {
        int count = send(filme.fdSender, (void *)excerpt, frame, 0);

        if (-1 == count && !(EAGAIN == errno || EWOULDBLOCK == errno))
        {
            fprintf(stderr, "Failure on socket send (fd %d)!\n", filme.fdSender);
        }
        else if (count >= 0)
        {
            printf("Thread with fd %d sent %d bytes.\n", filme.fdSender, count);
        }
    }

    printf("Sent %zu bytes! My fd is %d.\n", filme.upper.size, filme.fdSender);

    return filme.fdSender;
}

static int FilmeReceiver(void *ptr)
{
    struct filme filme = *(struct filme *)ptr;

    int remaining = filme.upper.size;

    do
    {
        SDL_Delay(1);

        int count = recv(filme.fdReceiver, (void *)filme.vessel, remaining, 0);

        if (-1 == count && !(EAGAIN == errno || EWOULDBLOCK == errno))
        {
            fprintf(stderr, "Failure on socket receive (fd %d)!\n", filme.fdReceiver);
        }
        else if (count >= 0)
        {
            printf("Thread with fd %d received %d bytes.\n", filme.fdReceiver, count);

            remaining      = upper_write(&filme.upper, filme.vessel, count);
            filme.n_error += remaining < 0;
            remaining      = abs(remaining);
        }
    }
    while (remaining > 0);

    printf("Received %zu bytes! My fd is %d.\n", filme.upper.size, filme.fdReceiver);

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

    if (NULL == (thread2 = SDL_CreateThread(FilmeReceiver, "receiver", &filme)))
    {
        SDL_Log("Thread creation failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    if (NULL == (thread1 = SDL_CreateThread(FilmeSender, "sender", &filme)))
    {
        SDL_Log("Thread creation failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    printf("Okay, awaiting first thread.\n");

    SDL_WaitThread(thread1, NULL);

    printf("Okay, awaiting second thread.\n");

    SDL_WaitThread(thread2, NULL);

    printf("Okay, done!\n");

    if (filme.n_error)
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
