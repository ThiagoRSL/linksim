#include <stdio.h>

#include <SDL.h>

#include "lifecycle.h"
#include "socket.h"

static int TestThread(void *ptr)
{
    int fd = *(int *)ptr;

    int total_sent     = 0;
    int total_received = 0;

    for (int i = 0; i < 100; i++)
    {
        int count = send(fd, "hello", 6, 0);
        int error = errno;

        if (-1 == count)
        {
            if (!(EAGAIN == error || EWOULDBLOCK == error))
            {
                perror(NULL);
            }
        }
        else
        {
            total_sent += count;
            printf("I sent \"hello\" to fd %d for the %dth time! It received only %d bytes!\n", fd, i, count);
        }

        char buffer[10000];

        count = recv(fd, buffer, 10000, 0);

        if (-1 == count)
        {
            perror(NULL);
        }
        else
        {
            total_received += count;
            printf("I received %*s from fd %d! I received only %d bytes!\n", count, buffer, fd, count);
        }
    }

    printf("I have fd %d and sent %d bytes and received %d!\n", fd, total_sent, total_received);

    return fd;
}

int main(int argc, char *argv[])
{
    int sockets[2];

    lifecycle_begin(argc, argv, sockets);

    printf("fds = %d and %d.\n", sockets[0], sockets[1]);

    SDL_Thread *thread1 = NULL;
    SDL_Thread *thread2 = NULL;

    if (NULL == (thread1 = SDL_CreateThread(TestThread, "test1", sockets + 0)))
    {
        SDL_Log("Thread creation failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    if (NULL == (thread2 = SDL_CreateThread(TestThread, "test2", sockets + 1)))
    {
        SDL_Log("Thread creation failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_WaitThread(thread1, NULL);
    SDL_WaitThread(thread2, NULL);

    lifecycle_end(sockets);

    return 0;
}
