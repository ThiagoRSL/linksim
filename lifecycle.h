#ifndef __LIFECYCLE_H__
#define __LIFECYCLE_H__

#include <SDL.h>
#include <SDL_net.h>

static void lifecycle_begin(int argc, char *argv[])
{
    atexit(SDL_Quit);

    if (SDL_Init(SDL_INIT_TIMER) != 0)
    {
        SDL_Log("SDL initialization failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    atexit(SDLNet_Quit);

    if (SDLNet_Init() != 0)
    {
        SDL_Log("Network initialization failed: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }

    (void)argc;
    (void)argv;
}

static void lifecycle_end(void)
{
    ;
}

#endif
