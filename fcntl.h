#ifndef __FCNTL_H__
#define __FCNTL_H__

#define O_NONBLOCK 69 // Random magic number.
#define F_SETFL    99 // Random magic number.

// Yes, flag should be optional, but that's a non-use case right now.
int fcntl(int fd, int cmd, int flag);

#endif
