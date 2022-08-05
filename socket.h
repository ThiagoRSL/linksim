#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
    #include <winsock2.h> 
    #include <ws2tcpip.h>
    #include <afunix.h>

    #define O_NONBLOCK 69 // Random magic number.
    #define F_SETFL    99 // Random magic number.

    // NOTE There was no effort to stringently replicate POSIX interfaces.

    static int close(int fd)
    {
        assert(fd >= 0);
        return closesocket(fd);
    }

    // Yes, flag should be optional, but that's a non-use case right now.
    static int fcntl(int fd, int cmd, int flag)
    {
        assert("Not implemented" && F_SETFL    == cmd);
        assert("Not implemented" && O_NONBLOCK == flag);

        if (ioctlsocket(fd, FIONBIO, (u_long []){1}) != 0)
        {
            errno = WSAGetLastError();
            return -1;
        }

        return 0;
    }

    #include "socketpair.h"
#else
    #include <fcntl.h>
    #include <sys/socket.h>
#endif

#endif
