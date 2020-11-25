#ifndef COSMOPOLITAN_LIBC_NT_ENUM_COPYFILE_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_COPYFILE_H_

#define kNtCopyFileFailIfExists              0x00000001
#define kNtCopyFileRestartable               0x00000002
#define kNtCopyFileOpenSourceForWrite        0x00000004
#define kNtCopyFileAllowDecryptedDestination 0x00000008
#define kNtCopyFileCopySymlink               0x00000800
#define kNtCopyFileNoBuffering               0x00001000
#define kNtCopyFileRequestSecurityPrivileges 0x00002000 /* Win8+ */
#define kNtCopyFileResumeFromPause           0x00004000 /* Win8+ */
#define kNtCopyFileRequestSecurityPrivileges 0x00002000 /* Win8+ */
#define kNtCopyFileNoOffload                 0x00040000 /* Win8+ */
#define kNtCopyFileIgnoreEdpBlock            0x00400000 /* Win10+ */
#define kNtCopyFileIgnoreSourceEncryption    0x00800000 /* Win10+ */

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_COPYFILE_H_ */
