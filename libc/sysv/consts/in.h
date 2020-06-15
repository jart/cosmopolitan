#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_IN_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_IN_H_
#include "libc/runtime/symbolic.h"

#define IN_ACCESS SYMBOLIC(IN_ACCESS)
#define IN_ALL_EVENTS SYMBOLIC(IN_ALL_EVENTS)
#define IN_ATTRIB SYMBOLIC(IN_ATTRIB)
#define IN_CLOEXEC SYMBOLIC(IN_CLOEXEC)
#define IN_CLOSE SYMBOLIC(IN_CLOSE)
#define IN_CLOSE_NOWRITE SYMBOLIC(IN_CLOSE_NOWRITE)
#define IN_CLOSE_WRITE SYMBOLIC(IN_CLOSE_WRITE)
#define IN_CREATE SYMBOLIC(IN_CREATE)
#define IN_DELETE SYMBOLIC(IN_DELETE)
#define IN_DELETE_SELF SYMBOLIC(IN_DELETE_SELF)
#define IN_DONT_FOLLOW SYMBOLIC(IN_DONT_FOLLOW)
#define IN_EXCL_UNLINK SYMBOLIC(IN_EXCL_UNLINK)
#define IN_IGNORED SYMBOLIC(IN_IGNORED)
#define IN_ISDIR SYMBOLIC(IN_ISDIR)
#define IN_LOOPBACKNET SYMBOLIC(IN_LOOPBACKNET)
#define IN_MASK_ADD SYMBOLIC(IN_MASK_ADD)
#define IN_MODIFY SYMBOLIC(IN_MODIFY)
#define IN_MOVE SYMBOLIC(IN_MOVE)
#define IN_MOVED_FROM SYMBOLIC(IN_MOVED_FROM)
#define IN_MOVED_TO SYMBOLIC(IN_MOVED_TO)
#define IN_MOVE_SELF SYMBOLIC(IN_MOVE_SELF)
#define IN_NONBLOCK SYMBOLIC(IN_NONBLOCK)
#define IN_ONESHOT SYMBOLIC(IN_ONESHOT)
#define IN_ONLYDIR SYMBOLIC(IN_ONLYDIR)
#define IN_OPEN SYMBOLIC(IN_OPEN)
#define IN_Q_OVERFLOW SYMBOLIC(IN_Q_OVERFLOW)
#define IN_UNMOUNT SYMBOLIC(IN_UNMOUNT)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long IN_ACCESS;
hidden extern const long IN_ALL_EVENTS;
hidden extern const long IN_ATTRIB;
hidden extern const long IN_CLOEXEC;
hidden extern const long IN_CLOSE;
hidden extern const long IN_CLOSE_NOWRITE;
hidden extern const long IN_CLOSE_WRITE;
hidden extern const long IN_CREATE;
hidden extern const long IN_DELETE;
hidden extern const long IN_DELETE_SELF;
hidden extern const long IN_DONT_FOLLOW;
hidden extern const long IN_EXCL_UNLINK;
hidden extern const long IN_IGNORED;
hidden extern const long IN_ISDIR;
hidden extern const long IN_LOOPBACKNET;
hidden extern const long IN_MASK_ADD;
hidden extern const long IN_MODIFY;
hidden extern const long IN_MOVE;
hidden extern const long IN_MOVED_FROM;
hidden extern const long IN_MOVED_TO;
hidden extern const long IN_MOVE_SELF;
hidden extern const long IN_NONBLOCK;
hidden extern const long IN_ONESHOT;
hidden extern const long IN_ONLYDIR;
hidden extern const long IN_OPEN;
hidden extern const long IN_Q_OVERFLOW;
hidden extern const long IN_UNMOUNT;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_IN_H_ */
