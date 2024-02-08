#ifndef COSMOPOLITAN_LIBC_CALLS_IPC_H_
#define COSMOPOLITAN_LIBC_CALLS_IPC_H_

#define IPC_PRIVATE 0
#define IPC_RMID    0
#define IPC_SET     1
#define IPC_STAT    2
#define IPC_INFO    3
#define IPC_CREAT   01000
#define IPC_EXCL    02000
#define IPC_NOWAIT  04000

COSMOPOLITAN_C_START_

int ftok(const char *, int) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_IPC_H_ */
