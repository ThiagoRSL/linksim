#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>

#include <SDL.h>

#include "lifecycle.h"

struct filme
{
    int            buffer_size;
    unsigned char *original;
    unsigned char *vessel;
    int            fd;
};

#define MIN(A, B) ((A) < (B) ? (A) : (B))

#define FRAME_SIZE 500

static int FilmeThread(void *ptr)
{
    assert(ptr != NULL);

    struct filme filme = *(struct filme *)ptr;

    int total_sent     = 0;
    int total_received = 0;

    int tries_left = 100;

    int unavailable = 10;
#if 1
    while (total_sent != filme.buffer_size || total_received != filme.buffer_size)
#else
    for (int i = 0; i < 1000; i++)
#endif
    {
        SDL_Delay(1);

        if (total_sent != filme.buffer_size)
        {
            int count = send(filme.fd, filme.original + total_sent, MIN(FRAME_SIZE, filme.buffer_size - total_sent) , 0);

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
                    fprintf(stderr, "Failure on socket send (fd %d)!\n", filme.fd);

                    tries_left--;

                    if (tries_left <= 0)
                    {
                        fprintf(stderr, "Bailing out (fd %d)!\n", filme.fd);
                        exit(EXIT_FAILURE);
                    }
                }
            }
            else
            {
                printf("Thread with fd %d sent %d bytes.\n", filme.fd, count);

                total_sent += count;
            }
        }

        if (total_received != filme.buffer_size)
        {
            int count = recv(filme.fd, filme.vessel + total_received, filme.buffer_size - total_received, 0);

            if (-1 == count)
            {
                perror(NULL);

                if (!(EAGAIN == errno || EWOULDBLOCK == errno))
                {
                    fprintf(stderr, "Failure on socket receive (fd %d)!\n", filme.fd);

                    tries_left--;

                    if (0 == tries_left)
                    {
                        fprintf(stderr, "Bailing out (fd %d)!\n", filme.fd);
                        exit(EXIT_FAILURE);
                    }
                }
            }
	    else
	    {
                printf("Thread with fd %d received %d bytes.\n", filme.fd, count);

                total_received += count;
	    }
        }
    }

    printf("Sent %d and received %d bytes! My fd is %d.\n", total_sent, total_received, filme.fd);

    return filme.fd;
}

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
    printf("Hey!\n");

    int sockets[2];

    int buffer_size = 0;
    unsigned char *buffer = NULL;

    printf("Hey!\n");

    lifecycle_begin(argc, argv, sockets, &buffer, &buffer_size);

    assert(buffer_size > 0);

    unsigned char *buffer2 = malloc(buffer_size);

    if (NULL == buffer2)
    {
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    unsigned char *buffer3 = malloc(buffer_size);

    if (NULL == buffer3)
    {
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    printf("fds = %d and %d.\n", sockets[0], sockets[1]);

    SDL_Thread *thread1 = NULL;
    SDL_Thread *thread2 = NULL;
    
    struct filme filme1 = {buffer_size, buffer, buffer2, sockets[0]};
    struct filme filme2 = {buffer_size, buffer, buffer3, sockets[1]};

    if (NULL == (thread1 = SDL_CreateThread(FilmeThread, "test1", &filme1)))
    {
        SDL_Log("Thread creation failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    if (NULL == (thread2 = SDL_CreateThread(FilmeThread, "test2", &filme2)))
    {
        SDL_Log("Thread creation failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_WaitThread(thread1, NULL);
    SDL_WaitThread(thread2, NULL);

    if (!memcmp(buffer2, buffer3, buffer_size))
    {
        printf("Both sides have the same thing!!!!\n");
    }
    else
    {
        printf("Contents are different!!!!\n");
    }

#if 1
    free(buffer2);
    free(buffer3);
#endif

    lifecycle_end(sockets, &buffer);

    SDL_TLSCleanup();

    return 0;
}
