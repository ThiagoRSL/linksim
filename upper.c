#include <stdio.h>
#include <stdlib.h>

#include "upper.h"

#define BUFFER_INCREMENT 200000

struct upper upper_init(char const *filepath)
{
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

    return (struct upper){buffer, buffer_size};
}

void upper_quit(struct upper *upper)
{
    free(upper->data);

    upper->data = NULL;
    upper->size = 0;
}
