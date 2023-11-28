#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_MOUNT_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_MOUNT_H_
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

#define MS_RDONLY       0x00000001
#define MNT_RDONLY      0x00000001
#define MS_NOSUID       MS_NOSUID
#define MNT_NOSUID      MNT_NOSUID
#define MS_NODEV        MS_NODEV
#define MNT_NODEV       MNT_NODEV
#define MS_NOEXEC       MS_NOEXEC
#define MNT_NOEXEC      MNT_NOEXEC
#define MS_SYNCHRONOUS  MS_SYNCHRONOUS
#define MNT_SYNCHRONOUS MNT_SYNCHRONOUS
#define MS_REMOUNT      MS_REMOUNT
#define MNT_UPDATE      MNT_UPDATE
#define MS_MANDLOCK     MS_MANDLOCK
#define MS_DIRSYNC      MS_DIRSYNC
#define MS_NOATIME      MS_NOATIME
#define MNT_NOATIME     MNT_NOATIME
#define MS_NODIRATIME   MS_NODIRATIME
#define MS_BIND         MS_BIND
#define MS_MOVE         MS_MOVE
#define MS_REC          MS_REC
#define MS_SILENT       MS_SILENT
#define MS_POSIXACL     MS_POSIXACL
#define MS_UNBINDABLE   MS_UNBINDABLE
#define MS_PRIVATE      MS_PRIVATE
#define MS_SLAVE        MS_SLAVE
#define MS_SHARED       MS_SHARED
#define MS_RELATIME     MS_RELATIME
#define MNT_RELATIME    MNT_RELATIME
#define MS_KERNMOUNT    MS_KERNMOUNT
#define MS_I_VERSION    MS_I_VERSION
#define MS_STRICTATIME  MS_STRICTATIME
#define MNT_STRICTATIME MNT_STRICTATIME
#define MS_LAZYTIME     MS_LAZYTIME
#define MS_ACTIVE       MS_ACTIVE
#define MS_NOUSER       MS_NOUSER
#define MS_RMT_MASK     MS_RMT_MASK
#define MS_MGC_VAL      MS_MGC_VAL
#define MS_MGC_MSK      MS_MGC_MSK
#define MNT_ASYNC       MNT_ASYNC
#define MNT_RELOAD      MNT_RELOAD
#define MNT_SUIDDIR     MNT_SUIDDIR
#define MNT_NOCLUSTERR  MNT_NOCLUSTERR
#define MNT_NOCLUSTERW  MNT_NOCLUSTERW
#define MNT_SNAPSHOT    MNT_SNAPSHOT


COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_MOUNT_H_ */
