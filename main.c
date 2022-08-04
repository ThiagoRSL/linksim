#include <SDL.h>

#include "lifecycle.h"

int main(int argc, char *argv[])
{
    int sockets[2];

    lifecycle_begin(argc, argv, sockets);

    lifecycle_end(sockets);

    return 0;
}
