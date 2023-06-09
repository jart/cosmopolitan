#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_SA_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_SA_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const uint64_t SA_NOCLDSTOP;
extern const uint64_t SA_NOCLDWAIT;
extern const uint64_t SA_NODEFER;
extern const uint64_t SA_NOMASK;
extern const uint64_t SA_ONESHOT;
extern const uint64_t SA_ONSTACK;
extern const uint64_t SA_RESETHAND;
extern const uint64_t SA_RESTART;
extern const uint64_t SA_SIGINFO;

#define SA_NOCLDSTOP SA_NOCLDSTOP
#define SA_NOCLDWAIT SA_NOCLDWAIT
#define SA_NODEFER   SA_NODEFER
#define SA_NOMASK    SA_NOMASK
#define SA_ONESHOT   SA_ONESHOT
#define SA_ONSTACK   SA_ONSTACK
#define SA_RESETHAND SA_RESETHAND
#define SA_RESTART   SA_RESTART
#define SA_SIGINFO   SA_SIGINFO

#define __tmpcosmo_SA_NOCLDSTOP -1925907945
#define __tmpcosmo_SA_NOCLDWAIT 1554343779
#define __tmpcosmo_SA_NODEFER   -1448019280
#define __tmpcosmo_SA_NOMASK    906478413
#define __tmpcosmo_SA_ONESHOT   -1992944743
#define __tmpcosmo_SA_ONSTACK   -1798112676
#define __tmpcosmo_SA_RESETHAND -1800631408
#define __tmpcosmo_SA_RESTART   724955366
#define __tmpcosmo_SA_SIGINFO   -1160286589

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_SA_H_ */
