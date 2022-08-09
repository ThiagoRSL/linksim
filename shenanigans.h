#ifndef __SHENANIGANS_H__
#define __SHENANIGANS_H__

#ifdef _WIN32
void network_start(void);
void network_quit(void);
void fix_errno(void);
#else
static inline void network_start(void) { ; }
static inline void network_quit(void)  { ; }
static inline void fix_errno(void)     { ; }
#endif

#endif
