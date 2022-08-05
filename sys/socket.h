#ifndef __SYS_SOCKET_H__
#define __SYS_SOCKET_H__

#include <winsock2.h>

#define SOCK_NONBLOCK 4096 // Arbitrarily chosen high number.

int socketpair(int domain, int type, int protocol, int sv[2]);

#endif
