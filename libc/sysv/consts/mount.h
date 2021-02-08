#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_MOUNT_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_MOUNT_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const long MS_ACTIVE;
extern const long MS_BIND;
extern const long MS_DIRSYNC;
extern const long MS_I_VERSION;
extern const long MS_KERNMOUNT;
extern const long MS_LAZYTIME;
extern const long MS_MANDLOCK;
extern const long MS_MGC_MSK;
extern const long MS_MGC_VAL;
extern const long MS_MOVE;
extern const long MS_NOATIME;
extern const long MS_NODEV;
extern const long MS_NODIRATIME;
extern const long MS_NOEXEC;
extern const long MS_NOSUID;
extern const long MS_NOUSER;
extern const long MS_POSIXACL;
extern const long MS_PRIVATE;
extern const long MS_RDONLY;
extern const long MS_REC;
extern const long MS_RELATIME;
extern const long MS_REMOUNT;
extern const long MS_RMT_MASK;
extern const long MS_SHARED;
extern const long MS_SILENT;
extern const long MS_SLAVE;
extern const long MS_STRICTATIME;
extern const long MS_SYNCHRONOUS;
extern const long MS_UNBINDABLE;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define MS_ACTIVE      SYMBOLIC(MS_ACTIVE)
#define MS_BIND        SYMBOLIC(MS_BIND)
#define MS_DIRSYNC     SYMBOLIC(MS_DIRSYNC)
#define MS_I_VERSION   SYMBOLIC(MS_I_VERSION)
#define MS_KERNMOUNT   SYMBOLIC(MS_KERNMOUNT)
#define MS_LAZYTIME    SYMBOLIC(MS_LAZYTIME)
#define MS_MANDLOCK    SYMBOLIC(MS_MANDLOCK)
#define MS_MGC_MSK     SYMBOLIC(MS_MGC_MSK)
#define MS_MGC_VAL     SYMBOLIC(MS_MGC_VAL)
#define MS_MOVE        SYMBOLIC(MS_MOVE)
#define MS_NOATIME     SYMBOLIC(MS_NOATIME)
#define MS_NODEV       SYMBOLIC(MS_NODEV)
#define MS_NODIRATIME  SYMBOLIC(MS_NODIRATIME)
#define MS_NOEXEC      SYMBOLIC(MS_NOEXEC)
#define MS_NOSUID      SYMBOLIC(MS_NOSUID)
#define MS_NOUSER      SYMBOLIC(MS_NOUSER)
#define MS_POSIXACL    SYMBOLIC(MS_POSIXACL)
#define MS_PRIVATE     SYMBOLIC(MS_PRIVATE)
#define MS_RDONLY      SYMBOLIC(MS_RDONLY)
#define MS_REC         SYMBOLIC(MS_REC)
#define MS_RELATIME    SYMBOLIC(MS_RELATIME)
#define MS_REMOUNT     SYMBOLIC(MS_REMOUNT)
#define MS_RMT_MASK    SYMBOLIC(MS_RMT_MASK)
#define MS_SHARED      SYMBOLIC(MS_SHARED)
#define MS_SILENT      SYMBOLIC(MS_SILENT)
#define MS_SLAVE       SYMBOLIC(MS_SLAVE)
#define MS_STRICTATIME SYMBOLIC(MS_STRICTATIME)
#define MS_SYNCHRONOUS SYMBOLIC(MS_SYNCHRONOUS)
#define MS_UNBINDABLE  SYMBOLIC(MS_UNBINDABLE)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_MOUNT_H_ */
