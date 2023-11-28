#ifndef COSMOPOLITAN_LIBC_RUNTIME_PATHCONF_H_
#define COSMOPOLITAN_LIBC_RUNTIME_PATHCONF_H_

#define _PC_LINK_MAX           0
#define _PC_MAX_CANON          1
#define _PC_MAX_INPUT          2
#define _PC_NAME_MAX           3
#define _PC_PATH_MAX           4
#define _PC_PIPE_BUF           5
#define _PC_CHOWN_RESTRICTED   6
#define _PC_NO_TRUNC           7
#define _PC_VDISABLE           8
#define _PC_SYNC_IO            9
#define _PC_ASYNC_IO           10
#define _PC_PRIO_IO            11
#define _PC_SOCK_MAXBUF        12
#define _PC_FILESIZEBITS       13
#define _PC_REC_INCR_XFER_SIZE 14
#define _PC_REC_MAX_XFER_SIZE  15
#define _PC_REC_MIN_XFER_SIZE  16
#define _PC_REC_XFER_ALIGN     17
#define _PC_ALLOC_SIZE_MIN     18
#define _PC_SYMLINK_MAX        19
#define _PC_2_SYMLINKS         20

COSMOPOLITAN_C_START_

long fpathconf(int, int);
long pathconf(const char *, int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_RUNTIME_PATHCONF_H_ */
