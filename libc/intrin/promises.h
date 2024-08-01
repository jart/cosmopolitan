#ifndef COSMOPOLITAN_LIBC_INTRIN_PROMISES_H_
#define COSMOPOLITAN_LIBC_INTRIN_PROMISES_H_

#define PROMISE_STDIO     0
#define PROMISE_RPATH     1
#define PROMISE_WPATH     2
#define PROMISE_CPATH     3
#define PROMISE_DPATH     4
#define PROMISE_FLOCK     5
#define PROMISE_FATTR     6
#define PROMISE_INET      7
#define PROMISE_UNIX      8
#define PROMISE_DNS       9
#define PROMISE_TTY       10
#define PROMISE_RECVFD    11
#define PROMISE_PROC      12
#define PROMISE_EXEC      13
#define PROMISE_ID        14
#define PROMISE_UNVEIL    15
#define PROMISE_SENDFD    16
#define PROMISE_SETTIME   17
#define PROMISE_PROT_EXEC 18
#define PROMISE_VMINFO    19
#define PROMISE_TMPPATH   20
#define PROMISE_CHOWN     21
#define PROMISE_ANET      22
#define PROMISE_LEN_      23

#define PLEDGED(x) ((~__promises >> PROMISE_##x) & 1)

COSMOPOLITAN_C_START_

extern unsigned long __promises;
extern unsigned long __execpromises;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_PROMISES_H_ */
