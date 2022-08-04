#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_MOUNT_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_MOUNT_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const unsigned long MS_RDONLY;
extern const int MNT_RDONLY;
extern const unsigned long MS_NOSUID;
extern const int MNT_NOSUID;
extern const unsigned long MS_NODEV;
extern const int MNT_NODEV;
extern const unsigned long MS_NOEXEC;
extern const int MNT_NOEXEC;
extern const unsigned long MS_SYNCHRONOUS;
extern const int MNT_SYNCHRONOUS;
extern const unsigned long MS_REMOUNT;
extern const int MNT_UPDATE;
extern const unsigned long MS_MANDLOCK;
extern const unsigned long MS_DIRSYNC;
extern const unsigned long MS_NOATIME;
extern const int MNT_NOATIME;
extern const unsigned long MS_NODIRATIME;
extern const unsigned long MS_BIND;
extern const unsigned long MS_MOVE;
extern const unsigned long MS_REC;
extern const unsigned long MS_SILENT;
extern const unsigned long MS_POSIXACL;
extern const unsigned long MS_UNBINDABLE;
extern const unsigned long MS_PRIVATE;
extern const unsigned long MS_SLAVE;
extern const unsigned long MS_SHARED;
extern const unsigned long MS_RELATIME;
extern const int MNT_RELATIME;
extern const unsigned long MS_KERNMOUNT;
extern const unsigned long MS_I_VERSION;
extern const unsigned long MS_STRICTATIME;
extern const int MNT_STRICTATIME;
extern const unsigned long MS_LAZYTIME;
extern const unsigned long MS_ACTIVE;
extern const unsigned long MS_NOUSER;
extern const unsigned long MS_RMT_MASK;
extern const unsigned long MS_MGC_VAL;
extern const unsigned long MS_MGC_MSK;
extern const int MNT_ASYNC;
extern const int MNT_RELOAD;
extern const int MNT_SUIDDIR;
extern const int MNT_NOCLUSTERR;
extern const int MNT_NOCLUSTERW;
extern const int MNT_SNAPSHOT;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define MS_RDONLY       LITERALLY(0x00000001)
#define MNT_RDONLY      LITERALLY(0x00000001)
#define MS_NOSUID       SYMBOLIC(MS_NOSUID)
#define MNT_NOSUID      SYMBOLIC(MNT_NOSUID)
#define MS_NODEV        SYMBOLIC(MS_NODEV)
#define MNT_NODEV       SYMBOLIC(MNT_NODEV)
#define MS_NOEXEC       SYMBOLIC(MS_NOEXEC)
#define MNT_NOEXEC      SYMBOLIC(MNT_NOEXEC)
#define MS_SYNCHRONOUS  SYMBOLIC(MS_SYNCHRONOUS)
#define MNT_SYNCHRONOUS SYMBOLIC(MNT_SYNCHRONOUS)
#define MS_REMOUNT      SYMBOLIC(MS_REMOUNT)
#define MNT_UPDATE      SYMBOLIC(MNT_UPDATE)
#define MS_MANDLOCK     SYMBOLIC(MS_MANDLOCK)
#define MS_DIRSYNC      SYMBOLIC(MS_DIRSYNC)
#define MS_NOATIME      SYMBOLIC(MS_NOATIME)
#define MNT_NOATIME     SYMBOLIC(MNT_NOATIME)
#define MS_NODIRATIME   SYMBOLIC(MS_NODIRATIME)
#define MS_BIND         SYMBOLIC(MS_BIND)
#define MS_MOVE         SYMBOLIC(MS_MOVE)
#define MS_REC          SYMBOLIC(MS_REC)
#define MS_SILENT       SYMBOLIC(MS_SILENT)
#define MS_POSIXACL     SYMBOLIC(MS_POSIXACL)
#define MS_UNBINDABLE   SYMBOLIC(MS_UNBINDABLE)
#define MS_PRIVATE      SYMBOLIC(MS_PRIVATE)
#define MS_SLAVE        SYMBOLIC(MS_SLAVE)
#define MS_SHARED       SYMBOLIC(MS_SHARED)
#define MS_RELATIME     SYMBOLIC(MS_RELATIME)
#define MNT_RELATIME    SYMBOLIC(MNT_RELATIME)
#define MS_KERNMOUNT    SYMBOLIC(MS_KERNMOUNT)
#define MS_I_VERSION    SYMBOLIC(MS_I_VERSION)
#define MS_STRICTATIME  SYMBOLIC(MS_STRICTATIME)
#define MNT_STRICTATIME SYMBOLIC(MNT_STRICTATIME)
#define MS_LAZYTIME     SYMBOLIC(MS_LAZYTIME)
#define MS_ACTIVE       SYMBOLIC(MS_ACTIVE)
#define MS_NOUSER       SYMBOLIC(MS_NOUSER)
#define MS_RMT_MASK     SYMBOLIC(MS_RMT_MASK)
#define MS_MGC_VAL      SYMBOLIC(MS_MGC_VAL)
#define MS_MGC_MSK      SYMBOLIC(MS_MGC_MSK)
#define MNT_ASYNC       SYMBOLIC(MNT_ASYNC)
#define MNT_RELOAD      SYMBOLIC(MNT_RELOAD)
#define MNT_SUIDDIR     SYMBOLIC(MNT_SUIDDIR)
#define MNT_NOCLUSTERR  SYMBOLIC(MNT_NOCLUSTERR)
#define MNT_NOCLUSTERW  SYMBOLIC(MNT_NOCLUSTERW)
#define MNT_SNAPSHOT    SYMBOLIC(MNT_SNAPSHOT)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_MOUNT_H_ */
