#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "upper.h"

#define MAX_CAPACITY 3000000000

#ifdef _HEAP_MAXREQ
_Static_assert(MAX_CAPACITY < _HEAP_MAXREQ, "Kinda glutton, eh?");
#endif

#define MIN(A, B) ((A) < (B) ? (A) : (B))

void upper_init(struct upper *upper, char const *filepath)
{
    assert(upper    != NULL);
    assert(filepath != NULL);

    upper->read    = 0;
    upper->written = 0;

    if (NULL == (upper->data = malloc(MAX_CAPACITY)))
    {
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    FILE *file = NULL;

    if (NULL == (file = fopen(filepath, "rb")))
    {
        perror(filepath);
        exit(EXIT_FAILURE);
    }

    upper->size = fread(upper->data, 1, MAX_CAPACITY, file);

    if ((size_t)MAX_CAPACITY == upper->size && fgetc(file) != EOF)
    {
        printf("Not enough memory to read file.\n");
        exit(EXIT_FAILURE);
    }

    (void)fclose(file);

    printf("I've read %zu bytes!\n", upper->size);
}

void upper_empty(struct upper *upper)
{
    assert(upper != NULL);

    upper->data    = NULL;
    upper->size    = 0;
    upper->read    = 0;
    upper->written = 0;
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

    int error = memcmp(upper->data + upper->size, excerpt, length) ? -1 : 1;

    upper->written += length;

    return error * (upper->size - upper->written);
}

void upper_quit(struct upper *upper)
{
    assert(upper != NULL);

    free(upper->data);

    upper->data = NULL;
    upper->size = 0;
}
