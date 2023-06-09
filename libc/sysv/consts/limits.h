#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_LIMITS_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_LIMITS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const int NGROUPS_MAX;
extern const int PIPE_BUF;
extern const int SOMAXCONN;
extern const int _ARG_MAX;
extern const int _NAME_MAX;
extern const int _NSIG;
extern const int _PATH_MAX;
extern const int LINK_MAX;
extern const int MAX_CANON;
extern const int MAX_INPUT;

#define NGROUPS_MAX NGROUPS_MAX
#define PIPE_BUF    PIPE_BUF
#define SOMAXCONN   SOMAXCONN
#define _ARG_MAX    _ARG_MAX
#define _NAME_MAX   _NAME_MAX
#define _NSIG       _NSIG
#define _PATH_MAX   _PATH_MAX
#define LINK_MAX    LINK_MAX
#define MAX_CANON   MAX_CANON
#define MAX_INPUT   MAX_INPUT

#define __tmpcosmo_NGROUPS_MAX 1969598838
#define __tmpcosmo_PIPE_BUF    1992386673
#define __tmpcosmo_SOMAXCONN   -1069247464
#define __tmpcosmo__ARG_MAX    794592230
#define __tmpcosmo__NAME_MAX   -1301020103
#define __tmpcosmo__NSIG       1488054137
#define __tmpcosmo__PATH_MAX   -1722847523
#define __tmpcosmo_LINK_MAX    1127355053
#define __tmpcosmo_MAX_CANON   -1111005462
#define __tmpcosmo_MAX_INPUT   -1378721420

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_LIMITS_H_ */
