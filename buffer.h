#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <stdlib.h>

struct buffer
{
    unsigned char *it;
    size_t         used;
    size_t         capacity;
};

static inline void buffer_init(struct buffer *buffer, size_t capacity)
{
    assert(buffer != NULL);

#ifdef _HEAP_MAXREQ
    if (capacity >= _HEAP_MAXREQ)
    {
        fprintf(stderr, "You are requesting too much memory!\n");
        exit(EXIT_FAILURE);
    }
#endif

    if (NULL == (buffer->it = malloc(capacity)))
    {
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    
    buffer->used     = 0;
    buffer->capacity = capacity;
}

static inline size_t buffer_unused(struct buffer *buffer)
{
    return buffer->capacity - buffer->used;
}

static inline unsigned char * buffer_next(struct buffer *buffer)
{
    return buffer->it + buffer->used;
}

// Auxiliary data structure to keep things one place.

#endif
