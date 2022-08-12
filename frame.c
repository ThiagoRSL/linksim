#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "crc.h"
#include "frame.h"

#define ESC  27
#define FLAG 64 // @.

#define FRAME_CRC_INDEX   0
#define FRAME_FCODE_INDEX 2
#define FRAME_ID_INDEX    3
#define FRAME_ID_ACK      4

#define FRAME_RESERVED_SIZE 5

bool frame_enframe(struct frame *frame, unsigned char byte)
{
    assert(frame != NULL);

    frame->data[frame->data_size++] = byte;

    assert(frame->data_size <= FRAME_DATA_MAX);

    return true;
}

void frame_deframe(struct frame *frame)
{
    assert(frame != NULL);
    assert(frame->data_size <= MAX_FRAME_SIZE);

    memcpy(frame->bytes, frame->data, frame->data_size);

    frame->n_byte = frame->data_size;
}
