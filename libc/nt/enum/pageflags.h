#ifndef COSMOPOLITAN_LIBC_NT_ENUM_PAGEFLAGS_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_PAGEFLAGS_H_

/* Pick One */
#define kNtPageNoaccess         0x001
#define kNtPageReadonly         0x002
#define kNtPageReadwrite        0x004
#define kNtPageWritecopy        0x008
#define kNtPageExecute          0x010
#define kNtPageExecuteRead      0x020
#define kNtPageExecuteReadwrite 0x040
#define kNtPageExecuteWritecopy 0x080
#define kNtPageGuard            0x100
#define kNtPageNocache          0x200
#define kNtPageWritecombine     0x400

/* These may be OR'd */
#define kNtSecReserve        0x04000000
#define kNtSecCommit         0x08000000 /* default */
#define kNtSecImageNoExecute 0x11000000
#define kNtSecImage          0x01000000
#define kNtSecNocache        0x10000000
#define kNtSecLargePages     0x80000000
#define kNtSecWritecombine   0x40000000

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_PAGEFLAGS_H_ */
