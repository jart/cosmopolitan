#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_LIMITS_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_LIMITS_H_
COSMOPOLITAN_C_START_

extern const int NGROUPS_MAX;
extern const int SOMAXCONN;
extern const int _ARG_MAX;
extern const int _NAME_MAX;
extern const int _NSIG;
extern const int _PATH_MAX;
extern const int LINK_MAX;
extern const int MAX_CANON;
extern const int MAX_INPUT;

#define PIPE_BUF 512 /* is 4096 on linux and windows */

#define NGROUPS_MAX NGROUPS_MAX
#define SOMAXCONN   SOMAXCONN
#define _ARG_MAX    _ARG_MAX
#define _NAME_MAX   _NAME_MAX
#define _NSIG       _NSIG
#define _PATH_MAX   _PATH_MAX
#define LINK_MAX    LINK_MAX
#define MAX_CANON   MAX_CANON
#define MAX_INPUT   MAX_INPUT

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_LIMITS_H_ */
