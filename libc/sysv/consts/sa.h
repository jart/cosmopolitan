#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_SA_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_SA_H_
COSMOPOLITAN_C_START_

extern const unsigned SA_NOCLDSTOP;
extern const unsigned SA_NOCLDWAIT;
extern const unsigned SA_NODEFER;
extern const unsigned SA_ONSTACK;
extern const unsigned SA_RESETHAND;
extern const unsigned SA_RESTART;
extern const unsigned SA_SIGINFO;

#define SA_NOCLDSTOP SA_NOCLDSTOP
#define SA_NOCLDWAIT SA_NOCLDWAIT
#define SA_NODEFER   SA_NODEFER
#define SA_ONSTACK   SA_ONSTACK
#define SA_RESETHAND SA_RESETHAND
#define SA_RESTART   SA_RESTART
#define SA_SIGINFO   SA_SIGINFO

/* compatibility constants */
#define SA_NOMASK  SA_NODEFER
#define SA_ONESHOT SA_RESETHAND

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_SA_H_ */
