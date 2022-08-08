#ifndef __UNISTD_H__
#define __UNISTD_H__

#include <stddef.h>

extern int   optind;
extern int   opterr;
extern int   optopt;
extern char *optarg;

int close(int fd);
int getopt(int argc, char * const argv[], const char *optstring);

#endif
