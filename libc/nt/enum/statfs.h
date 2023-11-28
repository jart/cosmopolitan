#ifndef COSMOPOLITAN_LIBC_NT_ENUM_STATFS_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_STATFS_H_
COSMOPOLITAN_C_START_

#define kNtFileCasePreservedNames         0x00000002
#define kNtFileCaseSensitiveSearch        0x00000001
#define kNtFileFileCompression            0x00000010
#define kNtFileNamedStreams               0x00040000
#define kNtFilePersistentAcls             0x00000008
#define kNtFileReadOnlyVolume             0x00080000 /* ST_RDONLY */
#define kNtFileSequentialWriteOnce        0x00100000
#define kNtFileSupportsEncryption         0x00020000
#define kNtFileSupportsExtendedAttributes 0x00800000
#define kNtFileSupportsHardLinks          0x00400000
#define kNtFileSupportsObjectIds          0x00010000
#define kNtFileSupportsOpenByFileId       0x01000000
#define kNtFileSupportsReparsePoints      0x00000080
#define kNtFileSupportsSparseFiles        0x00000040
#define kNtFileSupportsTransactions       0x00200000
#define kNtFileSupportsUsnJournal         0x02000000
#define kNtFileUnicodeOnDisk              0x00000004
#define kNtFileVolumeIsCompressed         0x00008000
#define kNtFileVolumeQuotas               0x00000020
#define kNtFileSupportsBlockRefcounting   0x08000000

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_ENUM_STATFS_H_ */
