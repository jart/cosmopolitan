#ifndef COSMOPOLITAN_LIBC_NT_ENUM_PAGEFLAGS_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_PAGEFLAGS_H_

/* Pick One */
#define kNtPageNoaccess 0x01
#define kNtPageReadonly 0x02
#define kNtPageReadwrite 0x04
#define kNtPageWritecopy 0x08
#define kNtPageExecute 0x10
#define kNtPageExecuteRead 0x20
#define kNtPageExecuteReadwrite 0x40
#define kNtPageExecuteWritecopy 0x80
#define kNtPageGuard 0x100
#define kNtPageNocache 0x200
#define kNtPageWritecombine 0x400

/* These may be OR'd */
#define kNtSecReserve 0x4000000
#define kNtSecCommit 0x8000000 /* ←default */
#define kNtSecImage 0x1000000
#define kNtSecImageNoExecute 0x11000000
#define kNtSecLargePages 0x80000000
#define kNtSecNocache 0x10000000
#define kNtSecWritecombine 0x40000000

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_PAGEFLAGS_H_ */
