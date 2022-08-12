#ifndef __FRAME_H__
#define __FRAME_H__

#include <stdbool.h>

#define MAX_FRAME_SIZE 512
#define FRAME_DATA_MAX 200

struct frame
{
    int           fcode;
    int           id_ack;
    int           id_frame;
    unsigned char data[FRAME_DATA_MAX];
    size_t        data_size;
    // Frame fields;

    unsigned char bytes[MAX_FRAME_SIZE];
    size_t        n_byte;
    // Stream fields.

    bool          escaping;
    // Internal state fields.
};

bool frame_enframe(struct frame *frame, unsigned char byte);
void frame_deframe(struct frame *frame);

// `frame_enframe()` received bytes from a stream and build frame fields.
// `frame_deframe()` reads frame fields and sets stream fields.
// CRC and byte-stuffing happens here.

#endif
