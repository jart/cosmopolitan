#ifndef COSMOPOLITAN_LIBC_CALLS_CREATEFILEFLAGS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_CREATEFILEFLAGS_INTERNAL_H_

// code size optimization
// <sync libc/sysv/consts.sh>
#define _O_APPEND     0x00000400  // kNtFileAppendData
#define _O_CREAT      0x00000040  // kNtOpenAlways
#define _O_EXCL       0x00000080  // kNtCreateNew
#define _O_TRUNC      0x00000200  // kNtCreateAlways
#define _O_DIRECTORY  0x00010000  // kNtFileFlagBackupSemantics
#define _O_UNLINK     0x04000100  // kNtFileAttributeTemporary|DeleteOnClose
#define _O_DIRECT     0x00004000  // kNtFileFlagNoBuffering
#define _O_NONBLOCK   0x00000800  // kNtFileFlagWriteThrough (not sent to win32)
#define _O_RANDOM     0x80000000  // kNtFileFlagRandomAccess
#define _O_SEQUENTIAL 0x40000000  // kNtFileFlagSequentialScan
#define _O_COMPRESSED 0x20000000  // kNtFileAttributeCompressed
#define _O_INDEXED    0x10000000  // !kNtFileAttributeNotContentIndexed
#define _O_NOFOLLOW   0x00020000  // kNtFileFlagOpenReparsePoint
#define _O_CLOEXEC    0x00080000
// </sync libc/sysv/consts.sh>

#endif /* COSMOPOLITAN_LIBC_CALLS_CREATEFILEFLAGS_INTERNAL_H_ */
