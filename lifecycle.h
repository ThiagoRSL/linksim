#ifndef __LIFECYCLE_H__
#define __LIFECYCLE_H__

#include <SDL.h>
#include <SDL_net.h>

static void lifecycle_begin(int argc, char *argv[])
{
    if (argc != 2 && argc != 3)
    {
        SDL_Log("Usage: linksim sender|receiver [host]\n");
        exit(EXIT_FAILURE);
    }

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

    IPaddress address = {INADDR_NONE, 0};
    TCPsocket socket  = NULL;

    if (3 == argc && SDLNet_ResolveHost(&address, argv[2], 6868) != 0)
    {
        SDL_Log("Couldn't solve host %s: %s\n", argv[2], SDLNet_GetError());
    }

    if (NULL == (socket = SDLNet_TCP_Open(&address)))
    {
        SDL_Log("Couldn't open port: %s\n", SDLNet_GetError());
	exit(EXIT_FAILURE);
    }

    {
        char const *resolution = SDLNet_ResolveIP(&address);

        if (NULL == resolution)
        {
            SDL_Log("Couldn't resolve IP: %s\n", SDLNet_GetError());
            exit(EXIT_FAILURE);
        }

        SDL_Log("Host resolved to %s\n", resolution);
    }
}

static void lifecycle_end(void)
{
    ;
}

#endif
