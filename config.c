#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/socket.h>
#include <unistd.h>

#include <SDL.h>

#include "config.h"
#include "shenanigans.h"
#include "upper.h"
#include "layers.h"

static struct config config;

static void config_quit(void)
{
    free(config.buffer.it);

    close(config.sockets[0]);
    close(config.sockets[1]);

    network_quit();

#if SDL_VERSION_ATLEAST(2,0,16)
    SDL_TLSCleanup();
#endif
}

struct config config_init(int argc, char *argv[])
{
    atexit(SDL_Quit);

    if (SDL_Init(SDL_INIT_TIMER) != 0)
    {
        SDL_Log("SDL initialization failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    network_start();

    if (-1 == socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0, config.sockets))
    {
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    opterr = 0;
    int option = -1;
    int mbs = 2000;

    char const usage[] = "USAGE: linksim[.exe] [-m <mbs>] <file> [<file>]";

    while ((option = getopt(argc, argv, "m:")) != -1)
    {
        switch(option)
        {
            case 'm':
                mbs = atoi(optarg);
                if (mbs <= 0)
                {
                    printf("%s\n", usage);
                    exit(EXIT_FAILURE);
                }
            break;

            case '?':
                printf("%s\n", usage);
                exit(EXIT_FAILURE);
            break;
        }
    }

    int n_file = argc - optind;

    if (n_file <= 0 || n_file > 2)
    {
        printf("%s\n", usage);
        exit(EXIT_FAILURE);
    }

    if ('\0' == argv[optind][0] || (2 == n_file && '\0' == argv[optind + 1][0]))
    {
        printf("%s\n", usage);
        exit(EXIT_FAILURE);
    }

    char *paths[2] = {argv[optind], 2 == n_file ? argv[optind + 1] : NULL};

    buffer_init(&config.buffer, 1000000 * (size_t)mbs);

    upper_init(config.uppers, paths, &config.buffer);

    atexit(config_quit);

    return config;
}
