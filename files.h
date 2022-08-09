#ifndef __FILES_H__
#define __FILES_H__

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static inline void files_open(FILE *files[2], char *paths[2])
{
    assert(files    != NULL);
    assert(paths    != NULL);
    assert(paths[0] != NULL);

    if (NULL == (files[0] = fopen(paths[0], "rb")))
    {
        perror(paths[0]);
        exit(EXIT_FAILURE);
    }

    if (paths[1] != NULL && NULL == (files[1] = fopen(paths[1], "rb")))
    {
        perror(paths[1]);
        (void)fclose(files[0]);
        exit(EXIT_FAILURE);
    }
}

static inline void files_close(FILE *files[2])
{
    assert(files    != NULL);
    assert(files[0] != NULL);

    (void)fclose(files[0]);

    if (files[1] != NULL)
    {
        (void)fclose(files[1]);
    }
}

#endif
