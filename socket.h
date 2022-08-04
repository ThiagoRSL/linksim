#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <assert.h>
#include <errno.h>

#ifdef _WIN32
    #include <winsock2.h> 
    #include <afunix.h>

    #define O_NONBLOCK 69
    #define F_SETFL    99

    static int close(int fd)
    {
        assert(fd >= 0);
        return closesocket(fd);
    }

    static int fcntl(int fd, int cmd, int flag)
    {
        assert("Implement as you need" && F_SETFL    == cmd);
        assert("Implement as you need" && O_NONBLOCK == flag);

        if (ioctlsocket(fd, FIONBIO, (u_long []){1}) != 0)
        {
            errno = WSAGetLastError();
            return -1;
        }

        return 0;
    }
#else
    #include <fcntl.h>
    #include <sys/socket.h>
    #include <unistd.h>
#endif

#endif
