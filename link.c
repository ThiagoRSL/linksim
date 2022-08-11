#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>

#include <sys/socket.h>
#include <unistd.h>

#include "link.h"
#include "shenanigans.h"
#include "upper.h"
#include "layers.h"
#include "physical.h"

void link_check(struct link *link)
{
    assert(link != NULL);
    assert(link->physical.fd > 0);
    assert(link->frame_size > 0);
    assert(link->frame_size <= LINK_FRAME_BYTE_MAX);
    assert(0 == link->received);
}

int link_process(struct link *link, unsigned char *bytes, size_t n_byte)
{
    assert(link  != NULL);
    assert(bytes != NULL);

    int remaining = upper_write(link->there, bytes, n_byte);

    if (remaining < 0)
    {
        printf("Something wrong! Remaining = %d for fd %d (got %zu bytes).\n", remaining, link->physical.fd, n_byte);
        return -1;
    }

    unsigned char *buffer = NULL;
    int to_send = upper_read(link->here, &buffer, link->frame_size);

    physical_send(&link->physical, buffer, to_send);

    /*for (int sent = 0, count = 0; sent != to_send; sent += count)
    {
        count = send(link->physical.fd, (void *)(buffer + sent), to_send - sent, 0);

        if (-1 == count)
        {
            fix_errno();

            if (EAGAIN != errno && EWOULDBLOCK != errno)
            {
                fprintf(stderr, "Failure on socket send (fd %d)!\n", link->physical.fd);
            }

            count = 0;
        }
    }*/

    printf("Remaining at fd %d: %d, to_send = %d (received %zu bytes)\n", link->physical.fd, remaining, to_send, n_byte);

    return remaining != 0 || to_send != 0;
}
