#ifndef COSMOPOLITAN_LIBC_NT_ENUM_FILEINFOBYHANDLECLASS_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_FILEINFOBYHANDLECLASS_H_

#define kNtFileBasicInfo                  0 /* struct NtFileBasicInformation */
#define kNtFileStandardInfo               1 /* struct NtFileStandardInformation */
#define kNtFileNameInfo                   2 /* struct NtFileNameInformation */
#define kNtFileStreamInfo                 7 /* struct NtFileStreamInformation */
#define kNtFileCompressionInfo            8 /* struct NtFileCompressionInfo */
#define kNtFileAttributeTagInfo           9 /* struct NtFileAttributeTagInformation */
#define kNtFileIdBothDirectoryInfo        10
#define kNtFileIdBothDirectoryRestartInfo 11
#define kNtFileRemoteProtocolInfo         13
#define kNtFileFullDirectoryInfo          14 /* NtFileFullDirectoryInformation */
#define kNtFileFullDirectoryRestartInfo   15
#define kNtFileStorageInfo                16 /* win8+ */
#define kNtFileAlignmentInfo              17 /* win8+ */
#define kNtFileIdInfo                     18 /* win8+ */
#define kNtFileIdExtdDirectoryInfo        19 /* win8+ */
#define kNtFileIdExtdDirectoryRestartInfo 20 /* win8+ */

#define kNtFileRenameInfo         3
#define kNtFileDispositionInfo    4
#define kNtFileAllocationInfo     5
#define kNtFileEndOfFileInfo      6
#define kNtFileIoPriorityHintInfo 12
#define kNtFileDispositionInfoEx  21 /* win10+ */
#define kNtFileRenameInfoEx       22 /* win10+ */

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_FILEINFOBYHANDLECLASS_H_ */
