#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include <winsock2.h>

#include <sys/socket.h>
#include <fcntl.h>

// Microsoft moment.
void fix_errno(void)
{
    static int const errors[] = {
        /*     6 */ [WSA_INVALID_HANDLE        ] =  -1,
        /*     8 */ [WSA_NOT_ENOUGH_MEMORY     ] =  -1,
        /*    87 */ [WSA_INVALID_PARAMETER     ] =  -1,
        /*   995 */ [WSA_OPERATION_ABORTED     ] =  -1,
        /*   996 */ [WSA_IO_INCOMPLETE         ] =  -1,
        /*   997 */ [WSA_IO_PENDING            ] =  -1,
        /* 10004 */ [WSAEINTR                  ] =  EINTR,
        /* 10009 */ [WSAEBADF                  ] =  EBADF,
        /* 10013 */ [WSAEACCES                 ] =  EACCES,
        /* 10014 */ [WSAEFAULT                 ] =  EFAULT,
        /* 10022 */ [WSAEINVAL                 ] =  EINVAL,
        /* 10024 */ [WSAEMFILE                 ] =  EMFILE,
        /* 10035 */ [WSAEWOULDBLOCK            ] =  EWOULDBLOCK,
        /* 10036 */ [WSAEINPROGRESS            ] =  EINPROGRESS,
        /* 10037 */ [WSAEALREADY               ] =  EALREADY,
        /* 10038 */ [WSAENOTSOCK               ] =  ENOTSOCK,
        /* 10039 */ [WSAEDESTADDRREQ           ] =  EDESTADDRREQ,
        /* 10040 */ [WSAEMSGSIZE               ] =  EMSGSIZE,
        /* 10041 */ [WSAEPROTOTYPE             ] =  EPROTOTYPE,
        /* 10042 */ [WSAENOPROTOOPT            ] =  ENOPROTOOPT,
        /* 10043 */ [WSAEPROTONOSUPPORT        ] =  EPROTONOSUPPORT,
        /* 10044 */ [WSAESOCKTNOSUPPORT        ] =  -1,
        /* 10045 */ [WSAEOPNOTSUPP             ] =  EOPNOTSUPP,
        /* 10046 */ [WSAEPFNOSUPPORT           ] =  -1,
        /* 10047 */ [WSAEAFNOSUPPORT           ] =  EAFNOSUPPORT,
        /* 10048 */ [WSAEADDRINUSE             ] =  EADDRINUSE,
        /* 10049 */ [WSAEADDRNOTAVAIL          ] =  EADDRNOTAVAIL,
        /* 10050 */ [WSAENETDOWN               ] =  ENETDOWN,
        /* 10051 */ [WSAENETUNREACH            ] =  ENETUNREACH,
        /* 10052 */ [WSAENETRESET              ] =  ENETRESET,
        /* 10053 */ [WSAECONNABORTED           ] =  ECONNABORTED,
        /* 10054 */ [WSAECONNRESET             ] =  ECONNRESET,
        /* 10055 */ [WSAENOBUFS                ] =  ENOBUFS,
        /* 10056 */ [WSAEISCONN                ] =  EISCONN,
        /* 10057 */ [WSAENOTCONN               ] =  ENOTCONN,
        /* 10058 */ [WSAESHUTDOWN              ] =  -1,
        /* 10059 */ [WSAETOOMANYREFS           ] =  -1,
        /* 10060 */ [WSAETIMEDOUT              ] =  ETIMEDOUT,
        /* 10061 */ [WSAECONNREFUSED           ] =  ECONNREFUSED,
        /* 10062 */ [WSAELOOP                  ] =  ELOOP,
        /* 10063 */ [WSAENAMETOOLONG           ] =  ENAMETOOLONG,
        /* 10064 */ [WSAEHOSTDOWN              ] =  -1,
        /* 10065 */ [WSAEHOSTUNREACH           ] =  EHOSTUNREACH,
        /* 10066 */ [WSAENOTEMPTY              ] =  ENOTEMPTY,
        /* 10067 */ [WSAEPROCLIM               ] =  -1,
        /* 10068 */ [WSAEUSERS                 ] =  -1,
        /* 10069 */ [WSAEDQUOT                 ] =  -1,
        /* 10070 */ [WSAESTALE                 ] =  -1,
        /* 10071 */ [WSAEREMOTE                ] =  -1,
        /* 10091 */ [WSASYSNOTREADY            ] =  -1,
        /* 10092 */ [WSAVERNOTSUPPORTED        ] =  -1,
        /* 10093 */ [WSANOTINITIALISED         ] =  -1,
        /* 10101 */ [WSAEDISCON                ] =  -1,
        /* 10102 */ [WSAENOMORE                ] =  -1,
        /* 10103 */ [WSAECANCELLED             ] =  ECANCELED,
        /* 10104 */ [WSAEINVALIDPROCTABLE      ] =  -1,
        /* 10105 */ [WSAEINVALIDPROVIDER       ] =  -1,
        /* 10106 */ [WSAEPROVIDERFAILEDINIT    ] =  -1,
        /* 10107 */ [WSASYSCALLFAILURE         ] =  -1,
        /* 10108 */ [WSASERVICE_NOT_FOUND      ] =  -1,
        /* 10109 */ [WSATYPE_NOT_FOUND         ] =  -1,
        /* 10110 */ [WSA_E_NO_MORE             ] =  -1,
        /* 10111 */ [WSA_E_CANCELLED           ] =  ECANCELED,
        /* 10112 */ [WSAEREFUSED               ] =  -1,
        /* 11001 */ [WSAHOST_NOT_FOUND         ] =  -1,
        /* 11002 */ [WSATRY_AGAIN              ] =  -1,
        /* 11003 */ [WSANO_RECOVERY            ] =  ENOTRECOVERABLE,
        /* 11004 */ [WSANO_DATA                ] =  ENODATA,
        /* 11005 */ [WSA_QOS_RECEIVERS         ] =  -1,
        /* 11006 */ [WSA_QOS_SENDERS           ] =  -1,
        /* 11007 */ [WSA_QOS_NO_SENDERS        ] =  -1,
        /* 11008 */ [WSA_QOS_NO_RECEIVERS      ] =  -1,
        /* 11009 */ [WSA_QOS_REQUEST_CONFIRMED ] =  -1,
        /* 11010 */ [WSA_QOS_ADMISSION_FAILURE ] =  -1,
        /* 11011 */ [WSA_QOS_POLICY_FAILURE    ] =  -1,
        /* 11012 */ [WSA_QOS_BAD_STYLE         ] =  -1,
        /* 11013 */ [WSA_QOS_BAD_OBJECT        ] =  -1,
        /* 11014 */ [WSA_QOS_TRAFFIC_CTRL_ERROR] =  -1,
        /* 11015 */ [WSA_QOS_GENERIC_ERROR     ] =  -1,
        /* 11016 */ [WSA_QOS_ESERVICETYPE      ] =  -1,
        /* 11017 */ [WSA_QOS_EFLOWSPEC         ] =  -1,
        /* 11018 */ [WSA_QOS_EPROVSPECBUF      ] =  -1,
        /* 11019 */ [WSA_QOS_EFILTERSTYLE      ] =  -1,
        /* 11020 */ [WSA_QOS_EFILTERTYPE       ] =  -1,
        /* 11021 */ [WSA_QOS_EFILTERCOUNT      ] =  -1,
        /* 11022 */ [WSA_QOS_EOBJLENGTH        ] =  -1,
        /* 11023 */ [WSA_QOS_EFLOWCOUNT        ] =  -1,
        /* 11024 */ [WSA_QOS_EUNKOWNPSOBJ      ] =  -1,
        /* 11025 */ [WSA_QOS_EPOLICYOBJ        ] =  -1,
        /* 11026 */ [WSA_QOS_EFLOWDESC         ] =  -1,
        /* 11027 */ [WSA_QOS_EPSFLOWSPEC       ] =  -1,
        /* 11028 */ [WSA_QOS_EPSFILTERSPEC     ] =  -1,
        /* 11029 */ [WSA_QOS_ESDMODEOBJ        ] =  -1,
        /* 11030 */ [WSA_QOS_ESHAPERATEOBJ     ] =  -1,
        /* 11031 */ [WSA_QOS_RESERVED_PETYPE   ] =  -1,
    };

    int error = WSAGetLastError();

    if (error != 0)
    {
        assert("Unknown error."  && error >= 0);
        assert("Unknown error."  && (size_t)error < sizeof (errors) / sizeof (*errors));

        errno = errors[error];

        assert("Unmapped error." && errno != -1);
        assert("Unknown error."  && errno !=  0);
    }
}

void network_start(void)
{
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0)
    {
        printf("Failed network initialization.\n");
        exit(EXIT_FAILURE);
    }
}

void network_quit(void)
{
    WSACleanup();
}

int close(int fd)
{
    assert(fd >= 0);
    return closesocket(fd);
}

int fcntl(int fd, int cmd, int flag)
{
    assert("Not implemented" && F_SETFL    == cmd);
    assert("Not implemented" && O_NONBLOCK == flag);

    if (ioctlsocket(fd, FIONBIO, (u_long []){1}) != 0)
    {
        fix_errno();
        return -1;
    }

    return 0;
}

// Welcome to Windows.
// Copied from https://gitlab.com/openconnect/openconnect/-/blob/master/compat.c

/* https://github.com/ncm/selectable-socketpair/blob/master/socketpair.c

Copyright 2007, 2010 by Nathan C. Myers <ncm@cantrip.org>
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

    Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

    The name of the author must not be used to endorse or promote products derived
    from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Changes:
 * 2014-02-12: merge David Woodhouse, Ger Hobbelt improvements
 *     git.infradead.org/users/dwmw2/openconnect.git/commitdiff/bdeefa54
 *     github.com/GerHobbelt/selectable-socketpair
 *   always init the socks[] to -1/INVALID_SOCKET on error, both on Win32/64
 *   and UNIX/other platforms
 * 2013-07-18: Change to BSD 3-clause license
 * 2010-03-31:
 *   set addr to 127.0.0.1 because win32 getsockname does not always set it.
 * 2010-02-25:
 *   set SO_REUSEADDR option to avoid leaking some windows resource.
 *   Windows System Error 10049, "Event ID 4226 TCP/IP has reached
 *   the security limit imposed on the number of concurrent TCP connect
 *   attempts."  Bleah.
 * 2007-04-25:
 *   preserve value of WSAGetLastError() on all error returns.
 * 2007-04-22:  (Thanks to Matthew Gregan <kinetik@flim.org>)
 *   s/EINVAL/WSAEINVAL/ fix trivial compile failure
 *   s/socket/WSASocket/ enable creation of sockets suitable as stdin/stdout
 *     of a child process.
 *   add argument make_overlapped
 */

#include <string.h>

# include <ws2tcpip.h>  /* socklen_t, et al (MSVC20xx) */
# include <windows.h>
# include <io.h>

#define HAVE_AFUNIX_H

#ifdef HAVE_AFUNIX_H
#include <afunix.h>
#else
#define UNIX_PATH_MAX 108
struct sockaddr_un {
    ADDRESS_FAMILY sun_family;     /* AF_UNIX */
    char sun_path[UNIX_PATH_MAX];  /* pathname */
} SOCKADDR_UN, *PSOCKADDR_UN;
#endif /* HAS_AFUNIX_H */

/* dumb_socketpair:
 *   If make_overlapped is nonzero, both sockets created will be usable for
 *   "overlapped" operations via WSASend etc.  If make_overlapped is zero,
 *   socks[0] (only) will be usable with regular ReadFile etc., and thus
 *   suitable for use as stdin or stdout of a child process.  Note that the
 *   sockets must be closed with closesocket() regardless.
 */

#define OPENCONNECT_CMD_SOCKET SOCKET

static int dumb_socketpair(OPENCONNECT_CMD_SOCKET socks[2], int make_overlapped)
{
    union {
        struct sockaddr_un unaddr;
        struct sockaddr_in inaddr;
        struct sockaddr addr;
    } a;
    OPENCONNECT_CMD_SOCKET listener;
    int e, ii;
    int domain = AF_UNIX;
    socklen_t addrlen = sizeof(a.unaddr);
    DWORD flags = (make_overlapped ? WSA_FLAG_OVERLAPPED : 0);
    int reuse = 1;

    if (socks == 0) {
        WSASetLastError(WSAEINVAL);
        return SOCKET_ERROR;
    }
    socks[0] = socks[1] = -1;

    /* AF_UNIX/SOCK_STREAM became available in Windows 10
     * ( https://devblogs.microsoft.com/commandline/af_unix-comes-to-windows )
     *
     * We will attempt to use AF_UNIX, but fallback to using AF_INET if
     * setting up AF_UNIX socket fails in any other way, which it surely will
     * on earlier versions of Windows.
     */
    for (ii = 0; ii < 2; ii++) {
        listener = socket(domain, SOCK_STREAM, domain == AF_INET ? IPPROTO_TCP : 0);
        if (listener == INVALID_SOCKET)
            goto fallback;

        memset(&a, 0, sizeof(a));
        if (domain == AF_UNIX) {
            /* XX: Abstract sockets (filesystem-independent) don't work, contrary to
             * the claims of the aforementioned blog post:
             * https://github.com/microsoft/WSL/issues/4240#issuecomment-549663217
             *
             * So we must use a named path, and that comes with all the attendant
             * problems of permissions and collisions. Trying various temporary
             * directories and putting high-res time and PID in the filename, that
             * seems like a less-bad option.
             */
            LARGE_INTEGER ticks;
            DWORD n = 0;
            int bind_try = 0;

            for (;;) {
                switch (bind_try++) {
                case 0:
                    /* "The returned string ends with a backslash" */
                    n = GetTempPath(UNIX_PATH_MAX, a.unaddr.sun_path);
                    break;
                case 1:
                    /* Heckuva job with API consistency, Microsoft! Reversed argument order, and
                     * "This path does not end with a backslash unless the Windows directory is the root directory.."
                     */
                    n = GetWindowsDirectory(a.unaddr.sun_path, UNIX_PATH_MAX);
                    n += snprintf(a.unaddr.sun_path + n, UNIX_PATH_MAX - n, "\\Temp\\");
                    break;
                case 2:
                    n = snprintf(a.unaddr.sun_path, UNIX_PATH_MAX, "C:\\Temp\\");
                    break;
                case 3:
                    n = 0; /* Current directory */
                    break;
                case 4:
                    goto fallback;
                }

                /* GetTempFileName could be used here.
                 * (https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-gettempfilenamea)
                 * However it only adds 16 bits of time-based random bits,
                 * fails if there isn't room for a 14-character filename, and
                 * seems to offers no other apparent advantages. So we will
                 * use high-res timer ticks and PID for filename.
                 */
                QueryPerformanceCounter(&ticks);
                snprintf(a.unaddr.sun_path + n, UNIX_PATH_MAX - n,
                         "%"PRIx64"-%lu.$$$", ticks.QuadPart, GetCurrentProcessId());
                a.unaddr.sun_family = AF_UNIX;

                if (bind(listener, &a.addr, addrlen) != SOCKET_ERROR)
                    break;
            }
        } else {
            a.inaddr.sin_family = AF_INET;
            a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
            a.inaddr.sin_port = 0;

            if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
                           (char *) &reuse, (socklen_t) sizeof(reuse)) == -1)
                goto fallback;

            if (bind(listener, &a.addr, addrlen) == SOCKET_ERROR)
                goto fallback;

            memset(&a, 0, sizeof(a));
            if (getsockname(listener, &a.addr, &addrlen) == SOCKET_ERROR)
                goto fallback;

            // win32 getsockname may only set the port number, p=0.0005.
            // ( https://docs.microsoft.com/windows/win32/api/winsock/nf-winsock-getsockname ):
            a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
            a.inaddr.sin_family = AF_INET;
        }

        if (listen(listener, 1) == SOCKET_ERROR)
            goto fallback;

        socks[0] = WSASocket(domain, SOCK_STREAM, 0, NULL, 0, flags);
        if (socks[0] == INVALID_SOCKET)
            goto fallback;
        if (connect(socks[0], &a.addr, addrlen) == SOCKET_ERROR)
            goto fallback;
        if (domain == AF_UNIX)
            DeleteFile(a.unaddr.sun_path);  // Socket file no longer needed

        socks[1] = accept(listener, NULL, NULL);
        if (socks[1] == INVALID_SOCKET)
            goto fallback;

        closesocket(listener);
        return 0;

    fallback:
        domain = AF_INET;
        addrlen = sizeof(a.inaddr);

        e = WSAGetLastError();
        closesocket(listener);
        closesocket(socks[0]);
        closesocket(socks[1]);
        WSASetLastError(e);
    }

    socks[0] = socks[1] = -1;
    return SOCKET_ERROR;
}

int socketpair(int domain, int type, int protocol, int sv[2])
{
    (void)domain;
    (void)protocol;

    SOCKET sv_temp[2] = {-1, -1};

    int code = dumb_socketpair(sv_temp, 1);

    if (-1 == code)
    {
        return -1;
    }

    for (int sv_i = 0; sv_i < 2 && (SOCK_NONBLOCK & type); sv_i++)
    {
        if (-1 == fcntl(sv_temp[sv_i], F_SETFL, O_NONBLOCK))
        {
            return -1;
        }
    }

    sv[0] = sv_temp[0];
    sv[1] = sv_temp[1];

    return 0;
}

/* A minimal POSIX getopt() implementation in ANSI C
 *
 * This is free and unencumbered software released into the public domain.
 *
 * This implementation supports the convention of resetting the option
 * parser by assigning optind to 0. This resets the internal state
 * appropriately.
 *
 * Ref: http://pubs.opengroup.org/onlinepubs/9699919799/functions/getopt.html
 */

int optind;
int opterr = 1;
int optopt;
char *optarg;

int getopt(int argc, char * const argv[], const char *optstring)
{
    static int optpos = 1;
    const char *arg;

    /* Reset? */
    if (optind == 0) {
        optind = !!argc;
        optpos = 1;
    }

    arg = argv[optind];
    if (arg && strcmp(arg, "--") == 0) {
        optind++;
        return -1;
    } else if (!arg || arg[0] != '-' || !isalnum(arg[1])) {
        return -1;
    } else {
        const char *opt = strchr(optstring, arg[optpos]);
        optopt = arg[optpos];
        if (!opt) {
            if (opterr && *optstring != ':')
                fprintf(stderr, "%s: illegal option: %c\n", argv[0], optopt);
            return '?';
        } else if (opt[1] == ':') {
            if (arg[optpos + 1]) {
                optarg = (char *)arg + optpos + 1;
                optind++;
                optpos = 1;
                return optopt;
            } else if (argv[optind + 1]) {
                optarg = (char *)argv[optind + 1];
                optind += 2;
                optpos = 1;
                return optopt;
            } else {
                if (opterr && *optstring != ':')
                    fprintf(stderr, 
                            "%s: option requires an argument: %c\n", 
                            argv[0], optopt);
                return *optstring == ':' ? ':' : '?';
            }
        } else {
            if (!arg[++optpos]) {
                optind++;
                optpos = 1;
            }
            return optopt;
        }
    }
}
