#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/socket.h>
#include <unistd.h>

#include <SDL.h>

#include "config.h"
#include "shenanigans.h"
#include "upper.h"

static struct config config;

static void config_quit(void);

struct config config_init(int argc, char *argv[])
{
    atexit(SDL_Quit);

    if (SDL_Init(SDL_INIT_TIMER) != 0)
    {
        SDL_Log("SDL initialization failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    network_start();

    int sockets[2] = {-1, -1};

    if (-1 == socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0, sockets))
    {
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    opterr = 0;
    int option = -1;
    int mbs = 2000;

    char const usage[] = "USAGE: linksim[.exe] [-m <mbs>] <file>";

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

    if (optind != argc - 1 || '\0' == argv[optind][0])
    {
        printf("USAGE: linksim[.exe] [-m <mbs>] <file> [<file>]\n");
	exit(EXIT_FAILURE);
    }

    struct upper upper = {0};
    
    upper_init(&upper, argv[optind], 1000000 * (size_t)mbs);

    config = (struct config){{sockets[0], sockets[1]}, upper};

    atexit(config_quit);

    return config;
}

static void config_quit(void)
{
    upper_quit(&(config.upper));

    close(config.sockets[0]);
    close(config.sockets[1]);

    network_quit();

#if SDL_VERSION_ATLEAST(2,0,16)
    SDL_TLSCleanup();
#endif
}
