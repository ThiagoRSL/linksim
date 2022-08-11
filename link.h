#ifndef __LINK_H__
#define __LINK_H__

#include "layers.h"

#define LINK_FRAME_BYTE_MAX 1500

void link_check(struct link *link);
int  link_process(struct link *link, unsigned char *bytes, size_t n_byte);

// Fill `struct link` yourself and check with `link_check()`.
// `link_process()` returns 1 if not done, 0 if done and -1 on error.

#endif
