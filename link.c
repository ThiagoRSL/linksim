#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <unistd.h>

#include "frame.h"
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

    struct frame frame     = {0};
    int64_t      remaining =  0;

    for (size_t byte_i = 0; byte_i < n_byte; byte_i++)
    {
        if (frame_enframe(&frame, bytes[byte_i]))
        {
            printf("Enframed something!\n");
            remaining = upper_write(link->there, frame.data, frame.data_size);

            if (remaining < 0)
            {
                printf("Something wrong! Remaining = %zd for fd %d (got %zu bytes).\n", remaining, link->physical.fd, n_byte);
                return -1;
            }
        }
    }

    unsigned char *buffer = NULL;
    size_t to_send = upper_read(link->here, &buffer, link->frame_size);

    for (int left = to_send; left > 0; left -= MAX_FRAME_SIZE)
    {
        printf("(de)Enframed something!\n");

        memcpy(frame.data, buffer + to_send - left, left % FRAME_DATA_MAX);
        frame.data_size = left % FRAME_DATA_MAX;
        frame_deframe(&frame);

        physical_send(&link->physical, frame.bytes, frame.n_byte);
    }

    printf("Remaining at fd %d: %zu, to_send = %zu (received %zu bytes)\n", link->physical.fd, remaining, to_send, n_byte);

    return remaining != 0 || to_send != 0;
}
