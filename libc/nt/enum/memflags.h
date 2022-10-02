#ifndef COSMOPOLITAN_LIBC_NT_ENUM_MEMFLAGS_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_MEMFLAGS_H_

#define kNtMemCommit     0x1000 /* perform physical memory reservation step */
#define kNtMemReserve    0x2000 /* perform virtual memory reservation step */
#define kNtMemDecommit   0x4000
#define kNtMemRelease    0x8000
#define kNtMemFree       0x10000
#define kNtMemPrivate    0x20000
#define kNtMemMapped     0x40000
#define kNtMemReset      0x80000
#define kNtMemTopDown    0x100000
#define kNtMemWriteWatch 0x200000
#define kNtMemPhysical   0x400000
#define kNtMemImage      0x1000000
#define kNtMemLargePages 0x20000000
#define kNtMem4mbPages   0x80000000

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_MEMFLAGS_H_ */
