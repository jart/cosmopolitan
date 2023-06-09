#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_MOUNT_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_MOUNT_H_
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

#define __tmpcosmo_MS_NOSUID       -1606122789
#define __tmpcosmo_MNT_NOSUID      -1447475041
#define __tmpcosmo_MS_NODEV        1395530069
#define __tmpcosmo_MNT_NODEV       1936393733
#define __tmpcosmo_MS_NOEXEC       617413422
#define __tmpcosmo_MNT_NOEXEC      -306506772
#define __tmpcosmo_MS_SYNCHRONOUS  957465984
#define __tmpcosmo_MNT_SYNCHRONOUS 934826893
#define __tmpcosmo_MS_REMOUNT      413574493
#define __tmpcosmo_MNT_UPDATE      920970183
#define __tmpcosmo_MS_MANDLOCK     -831013869
#define __tmpcosmo_MS_DIRSYNC      -1479267152
#define __tmpcosmo_MS_NOATIME      1168305905
#define __tmpcosmo_MNT_NOATIME     765975990
#define __tmpcosmo_MS_NODIRATIME   259117157
#define __tmpcosmo_MS_BIND         731286897
#define __tmpcosmo_MS_MOVE         -808863326
#define __tmpcosmo_MS_REC          -888579348
#define __tmpcosmo_MS_SILENT       1518034597
#define __tmpcosmo_MS_POSIXACL     -1836436617
#define __tmpcosmo_MS_UNBINDABLE   -1064128266
#define __tmpcosmo_MS_PRIVATE      -869857793
#define __tmpcosmo_MS_SLAVE        -1484401575
#define __tmpcosmo_MS_SHARED       1527550647
#define __tmpcosmo_MS_RELATIME     -1251574211
#define __tmpcosmo_MNT_RELATIME    1860322269
#define __tmpcosmo_MS_KERNMOUNT    -2091601207
#define __tmpcosmo_MS_I_VERSION    -323807485
#define __tmpcosmo_MS_STRICTATIME  -1587037133
#define __tmpcosmo_MNT_STRICTATIME -1140691829
#define __tmpcosmo_MS_LAZYTIME     1112443532
#define __tmpcosmo_MS_ACTIVE       -2142880216
#define __tmpcosmo_MS_NOUSER       1270935574
#define __tmpcosmo_MS_RMT_MASK     376610379
#define __tmpcosmo_MS_MGC_VAL      -1272539746
#define __tmpcosmo_MS_MGC_MSK      1522806541
#define __tmpcosmo_MNT_ASYNC       -1141454736
#define __tmpcosmo_MNT_RELOAD      814423179
#define __tmpcosmo_MNT_SUIDDIR     -1859059495
#define __tmpcosmo_MNT_NOCLUSTERR  -1032466906
#define __tmpcosmo_MNT_NOCLUSTERW  203320937
#define __tmpcosmo_MNT_SNAPSHOT    -1441878956

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_MOUNT_H_ */
