#include "shenanigans.h"
#include "physical.h"
#include "layers.h"
#include "link.h"
#include "rnd.h"
#define  RND_IMPLEMENTATION

#include <time.h> // for time
#include <stdio.h>
#include <sys/socket.h>

struct physical physical_initialize(int fd)
{
    rnd_pcg_t pcg;
    time_t seconds;
    time( &seconds );
    rnd_pcg_seed( &pcg, (RND_U32) seconds );

    struct physical physical = {0};
    physical.fd = fd;
    physical.random = pcg;
    physical.total_streams = 0;

    return physical;
}

void updateSeed(rnd_pcg_t* random)
{
    time_t seconds;
    time (&seconds);
    rnd_pcg_seed(random, (RND_U32) seconds);
}

int physical_receive(struct link *link)
{
    unsigned char  bytes[LINK_FRAME_BYTE_MAX] = {0};

    int n_byte = recv(link->physical.fd, (void *)bytes, link->frame_size, 0); // azaralhar algum numero entro 0 e n_byte

    if (-1 == n_byte)
    {
        fix_errno();
        if (EAGAIN != errno && EWOULDBLOCK != errno)
        {
            fprintf(stderr, "Failure on socket (fd %d)!\n", link->physical.fd);
        }

        n_byte = 0;
    }

    rnd_pcg_t random = link->physical.random;
    updateSeed(&random);
    int errorType = rnd_pcg_range(&random, 0, 3);

    RND_U32 chance = rnd_pcg_next( &random );
    if (chance % 10 == 0)
    {
        int flippedByte, position, totalFlippedBytes, frame1, frame2;

        switch (errorType)
        {
            case 0: // Erro de bit
                flippedByte = rnd_pcg_range( &random, 0, n_byte-1 );
                position = rnd_pcg_range( &random, 0, 7 );
                bytes[flippedByte] = bytes[flippedByte] ^ (0x1 << position);
                break;
            case 1: // Erro de rajada
                totalFlippedBytes = rnd_pcg_range( &random, 2, n_byte-1 );
                for (int i = 0; i < totalFlippedBytes; i++)
                {
                    flippedByte = rnd_pcg_range( &random, 0, n_byte-1 );
                    position = rnd_pcg_range( &random, 0, 7 );
                    bytes[flippedByte] = bytes[flippedByte] ^ (0x1 << position);
                }
                break;
            case 2: // Quadro fora de ordem
                frame1 = rnd_pcg_range( &random, 2, n_byte-1 );
                frame2 = rnd_pcg_range( &random, 2, n_byte-1 );
                unsigned char* aux = link->physical.streams[frame1];
                link->physical.streams[frame1] = link->physical.streams[frame2];
                link->physical.streams[frame2] = aux;
                break;
            case 3: // Quadro perdido
                n_byte = 0;
                break;
        }
    }    

    if (n_byte != 0)
    {
        unsigned char* copy  = (unsigned char*)malloc(n_byte);
        for (int i = 0; i < n_byte; i++)
        {
            copy[i] = bytes[i];
        }
        link->physical.streams[link->physical.total_streams] = copy;
        link->physical.sizes[link->physical.total_streams] = n_byte;
        link->physical.timeout[link->physical.total_streams] = (uint64_t) 5;
        link->physical.total_streams++;
    }

    if (link->physical.total_streams == MAX_STREAM)
    {
        link->physical.total_streams = 0;
    }

    for (int i = 0; i < MAX_STREAM; i++)
    {
        link->physical.timeout[i] = (link->physical.timeout[i] > 0) ? link->physical.timeout[i] - (uint64_t) 1 : link->physical.timeout[i];
    }

    int i = 0;
    while (link->physical.timeout[i] != (uint64_t) 1)
    {
        i++;
    }

    printf("Thread with fd %d received %d bytes.\n", link->physical.fd, link->physical.sizes[i]);
    return link_process(link, link->physical.streams[i], link->physical.sizes[i]);
}

void physical_send(struct physical *physical, unsigned char *message, size_t size)
{
    for (int sent = 0, count = 0; sent != size; sent += count)
    {
        count = send(physical->fd, (void *)(message + sent), size - sent, 0);

        if (-1 == count)
        {
            fix_errno();

            if (EAGAIN != errno && EWOULDBLOCK != errno)
            {
                fprintf(stderr, "Failure on socket send (fd %d)!\n", physical->fd);
            }

            count = 0;
        }
    }
}
