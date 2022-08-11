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
    int errorType = rnd_pcg_range(&random, 0, 3);

    RND_U32 chance = rnd_pcg_next( &random );
    if (chance % 10)
    {               
        printf("Thread with fd %d received %d bytes.\n", link->physical.fd, n_byte);
        return link_process(link, bytes, n_byte);
    }  

    switch (errorType)
    {
        case 0: // Erro de bit
            int flippedByte = rnd_pcg_range( &random, 0, n_byte-1 );
            int position = rnd_pcg_range( &random, 0, 7 );
            bytes[flippedByte] = bytes[flippedByte] ^ (0x1 << position);
            break;
        case 1: // Erro de rajada
            int totalFlippedBytes = rnd_pcg_range( &random, 2, n_byte-1 );
            for (int i = 0; i < totalFlippedBytes; i++)
            {
                int flippedByte = rnd_pcg_range( &random, 0, n_byte-1 );
                int position = rnd_pcg_range( &random, 0, 7 );
                bytes[flippedByte] = bytes[flippedByte] ^ (0x1 << position);
            }
            break;
        case 2: // Quadro fora de ordem
            int frame1 = rnd_pcg_range( &random, 2, n_byte-1 );
            int frame2 = rnd_pcg_range( &random, 2, n_byte-1 );
            unsigned char* aux = link->physical.streams[frame1];
            link->physical.streams[frame1] = link->physical.streams[frame2];
            link->physical.streams[frame2] = aux;
            break;
        case 3: // Quadro perdido
            n_byte = 0;
            break;
    }

    link->physical.streams[link->physical.total_streams] = bytes;
    link->physical.sizes[link->physical.total_streams] = n_byte;    
    link->physical.total_streams++;


    printf("Thread with fd %d received %d bytes.\n", link->physical.fd, n_byte);
    return link_process(link, bytes, n_byte);
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
