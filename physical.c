#include "physical.h"
#include "layers.h"
#include "rnd.h"
#define  RND_IMPLEMENTATION

#include <time.h> // for time

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