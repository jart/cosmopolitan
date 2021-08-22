#ifndef COSMOPOLITAN_LIBC_NT_ENUM_FILEFLAGANDATTRIBUTES_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_FILEFLAGANDATTRIBUTES_H_

/**
 * MS-DOS File Attributes.
 *
 * @see GetFileInformationByHandle()
 * @see libc/sysv/consts.sh
 */
#define kNtFileAttributeReadonly    0x00000001u
#define kNtFileAttributeHidden      0x00000002u
#define kNtFileAttributeSystem      0x00000004u
#define kNtFileAttributeVolumelabel 0x00000008u
#define kNtFileAttributeDirectory   0x00000010u
#define kNtFileAttributeArchive     0x00000020u

/**
 * NT File Attributes.
 */
#define kNtFileAttributeDevice            0x00000040u
#define kNtFileAttributeNormal            0x00000080u
#define kNtFileAttributeTemporary         0x00000100u
#define kNtFileAttributeSparseFile        0x00000200u
#define kNtFileAttributeReparsePoint      0x00000400u
#define kNtFileAttributeCompressed        0x00000800u
#define kNtFileAttributeOffline           0x00001000u
#define kNtFileAttributeNotContentIndexed 0x00002000u
#define kNtFileAttributeEncrypted         0x00004000u

/**
 * NT File Flags.
 */
#define kNtFileFlagWriteThrough      0x80000000u
#define kNtFileFlagOverlapped        0x40000000u
#define kNtFileFlagNoBuffering       0x20000000u
#define kNtFileFlagRandomAccess      0x10000000u
#define kNtFileFlagSequentialScan    0x08000000u
#define kNtFileFlagDeleteOnClose     0x04000000u
#define kNtFileFlagBackupSemantics   0x02000000u
#define kNtFileFlagPosixSemantics    0x01000000u
#define kNtFileFlagOpenReparsePoint  0x00200000u
#define kNtFileFlagOpenNoRecall      0x00100000u
#define kNtFileFlagFirstPipeInstance 0x00080000u

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_FILEFLAGANDATTRIBUTES_H_ */
