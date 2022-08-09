#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "files.h"
#include "upper.h"

#define MIN(A, B) ((A) < (B) ? (A) : (B))

void upper_init(struct upper uppers[2], char *paths[2], struct buffer *buffer)
{
    assert(uppers   != NULL);
    assert(paths    != NULL);
    assert(paths[0] != NULL);
    assert(buffer   != NULL);

    FILE          *files[2] = {NULL, NULL};
    unsigned char *datas[2] = {NULL, NULL};
    size_t         sizes[2] = {0   , 0   };

    files_open(files, paths);

    for (int upper_i = 0; upper_i < 2; upper_i++)
    {
        if (NULL == paths[upper_i])
        {
            continue;
        }

        size_t unused = buffer_unused(buffer);

        datas[upper_i] = buffer_next(buffer);
        sizes[upper_i] = fread(datas[upper_i], 1, unused, files[upper_i]);

        if (unused == sizes[upper_i] && fgetc(files[upper_i]) != EOF)
        {
            printf("Not enough memory to read file.\n");
            exit(EXIT_FAILURE);
        }

        buffer->used += sizes[upper_i];
    }

    uppers[0] = (struct upper){datas[0], sizes[0], 0, 0};
    uppers[1] = (struct upper){datas[1], sizes[1], 0, 0};

    files_close(files);
}

int upper_read(struct upper *upper, unsigned char **excerpt, size_t length)
{
    assert(upper   != NULL);
    assert(excerpt != NULL);

    size_t const length_actual = MIN(length, upper->size - upper->read);

    *excerpt     = length_actual ? upper->data + upper->read : NULL;
    upper->read += length_actual;

    return length_actual;
}

int upper_write(struct upper *upper, unsigned char *excerpt, size_t length)
{
    assert(upper   != NULL);
    assert(excerpt != NULL);
    assert(length <= upper->size - upper->written);

    int error = memcmp(upper->data + upper->written, excerpt, length) ? -1 : 1;

    upper->written += length;

    return error * (upper->size - upper->written);
}
