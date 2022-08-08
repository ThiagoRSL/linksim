#ifndef __SHENANIGANS_H__
#define __SHENANIGANS_H__

void network_start(void);
void network_quit(void);
void fix_errno(void);

#ifndef _WIN32
void network_start(void) { ; }
void network_quit(void)  { ; }
void fix_errno(void)     { ; }
#endif

#endif
