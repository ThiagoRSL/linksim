#include <SDL.h>
#include <SDL_net.h>

#include "lifecycle.h"

int main(int argc, char *argv[])
{
    lifecycle_begin(argc, argv);
    lifecycle_end();

    return 0;
}
