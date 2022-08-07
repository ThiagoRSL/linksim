#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "upper.h"

#define BUFFER_INCREMENT 200000
#define FRAME_SIZE       500

#define MIN(A, B) ((A) < (B) ? (A) : (B))

void upper_init(struct upper *upper, char const *filepath)
{
    assert(upper != NULL);
    assert(filepath != NULL);

    unsigned char *buffer = NULL;
    int buffer_size       = 0;
    int buffer_capacity   = 0;
    int tries_left        = 500;
    int result            = -1;

    FILE *file = fopen(filepath, "rb");

    if (NULL == file)
    {
        perror(filepath);
        exit(EXIT_FAILURE);
    }

    enum { READ_NORMAL, READ_LOW_COUNT, READ_LOW_COUNT_AGAIN } read_state = READ_NORMAL;

    do
    {
        unsigned char *buffer_new = realloc(buffer, (buffer_capacity = buffer_size + BUFFER_INCREMENT));

        if (NULL == buffer_new)
        {
            free(buffer);
            perror(NULL);
            exit(EXIT_FAILURE);
        }

        buffer = buffer_new;

        result = fread(buffer + buffer_size, 1, BUFFER_INCREMENT, file);

	printf("I got %d for this fread()!\n", result);

        if (result < BUFFER_INCREMENT)
        {
            if (ferror(file))
            {
                if (tries_left > 0)
                {
                    clearerr(file);
                    tries_left--;
                }
                else
                {
                    fprintf(stderr, "Failed to read stdin.\n");
                    exit(EXIT_FAILURE);
                }
            }

	    read_state++;
        }
	else
	{
            read_state = READ_NORMAL;
	}

        buffer_size += result;
    }
    while (BUFFER_INCREMENT == result || read_state != READ_LOW_COUNT_AGAIN || !feof(file)); 

    if (EOF == fclose(file))
    {
        printf("Closed %s with error! But that's okay!\n", filepath);
    }

    printf("I've read %d bytes! %d errors while reading!\n", buffer_size, 500 - tries_left);

    upper->data    = buffer;
    upper->size    = buffer_size;
    upper->read    = 0;
    upper->written = 0;
}

void upper_quit(struct upper *upper)
{
    assert(upper != NULL);

    free(upper->data);

    upper->data = NULL;
    upper->size = 0;
}

int upper_read(struct upper *upper, unsigned char **excerpt)
{
    assert(upper   != NULL);
    assert(excerpt != NULL);

    int const length = MIN(FRAME_SIZE, upper->size - upper->read);

    assert(length >= 0);

    *excerpt     = length ? upper->data + upper->read : NULL;
    upper->read += length;

    return length;
}

int upper_write(struct upper *upper, unsigned char *excerpt, int length)
{
    assert(upper   != NULL);
    assert(excerpt != NULL);
    assert(length >= 0);
    assert(length <= upper->size - upper->written);

    int error = memcmp(upper->data + upper->size, excerpt, length) ? -1 : 1;

    upper->written += length;

    return error * (upper->size - upper->written);
}
