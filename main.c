#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <unistd.h>

#include <SDL.h>

#include "config.h"
#include "link.h"
#include "shenanigans.h"
#include "upper.h"

static int device(void *ptr)
{
    assert(ptr != NULL);

    struct link   *link                       = (struct link *)ptr;
    int            n_byte                     = 0;
    unsigned char  bytes[LINK_FRAME_BYTE_MAX] = {0};

    int tries_left = 10000;

    int code = 0;

    while (1 == (code = link_process(link, bytes, n_byte)))
    {
        n_byte = recv(link->fd, (void *)bytes, link->frame_size, 0);

        if (-1 == n_byte)
        {
            fix_errno();
            if (EAGAIN != errno && EWOULDBLOCK != errno)
            {
                fprintf(stderr, "Failure on socket (fd %d)!\n", link->fd);

		tries_left--;

                if (0 == tries_left)
                {
                    return 1;
                }
            }

            n_byte = 0;
        }
        else
        {
            tries_left = 10000;
        }

        printf("Thread with fd %d received %d bytes.\n", link->fd, n_byte);
    }

    return -1 == code ? 1 : 0;
}

int main(int argc, char *argv[])
{
    printf("Hey!\n");

    struct config config = config_init(argc, argv);

    assert(config.upper.size > 0);

    printf("fds = %d and %d.\n", config.sockets[0], config.sockets[1]);

    SDL_Thread *sender   = NULL;
    SDL_Thread *receiver = NULL;

    struct upper empty = {0};

    upper_empty(&empty);
    
    struct link sending   = {&config.upper, &empty       , config.sockets[0], 500, 0};
    struct link receiving = {&empty       , &config.upper, config.sockets[1], 500, 0};

    link_check(&sending);
    link_check(&receiving);

    if (NULL == (sender = SDL_CreateThread(device, "sender", &sending)))
    {
        SDL_Log("Thread creation failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    if (NULL == (receiver = SDL_CreateThread(device, "receiver", &receiving)))
    {
        SDL_Log("Thread creation failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    int error_sender   = 0;
    int error_receiver = 0;

    printf("Okay, awaiting sender.\n");
    SDL_WaitThread(sender, &error_sender);

    printf("Okay, awaiting receiver.\n");
    SDL_WaitThread(receiver, &error_receiver);

    printf("Okay, done!\n");

    switch (error_sender + (error_receiver << 1))
    {
        case 0:  printf("Both sides have the same thing!!!!\n");  break;
        case 1:  printf("Sender had errors!!!!!\n");              break;
        case 2:  printf("Receiver had errors!!!!!\n");            break;
        case 3:  printf("Sender and receiver had errors!!!!!\n"); break;
        default: assert(!"Impossible status combination!");       break;
    }

    return 0;
}
