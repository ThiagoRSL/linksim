#include "shenanigans.h"
#include "physical.h"
#include "layers.h"
#include "link.h"
#include "rnd.h"
#define  RND_IMPLEMENTATION

#include <time.h> // for time
#include <stdio.h>
#include <sys/socket.h>

struct physical physical_initialize(int fd)
{
    rnd_pcg_t pcg;
    time_t seconds;
    time( &seconds );
    rnd_pcg_seed( &pcg, (RND_U32) seconds );

    struct physical physical = {0};
    physical.fd = fd;
    physical.random = pcg;
    physical.total_streams = 0;

    return physical;
}

int physical_receive(struct link *link, uint64_t delta)
{
    int            n_byte                     = 0;
    unsigned char  bytes[LINK_FRAME_BYTE_MAX] = {0};

    n_byte = recv(link->physical.fd, (void *)bytes, link->frame_size, 0);

    if (-1 == n_byte)
    {
        fix_errno();
        if (EAGAIN != errno && EWOULDBLOCK != errno)
        {
            fprintf(stderr, "Failure on socket (fd %d)!\n", link->physical.fd);
        }

        n_byte = 0;
    }

    printf("Thread with fd %d received %d bytes.\n", link->physical.fd, n_byte);
    return link_process(link, bytes, n_byte);
}

void physical_send(struct physical *physical, unsigned char *message, size_t size)
{
    for (int sent = 0, count = 0; sent != size; sent += count)
    {
        count = send(physical->fd, (void *)(message + sent), size - sent, 0);

        if (-1 == count)
        {
            fix_errno();

            if (EAGAIN != errno && EWOULDBLOCK != errno)
            {
                fprintf(stderr, "Failure on socket send (fd %d)!\n", physical->fd);
            }

            count = 0;
        }
    }
}
