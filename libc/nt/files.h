#ifndef COSMOPOLITAN_LIBC_NT_FILES_H_
#define COSMOPOLITAN_LIBC_NT_FILES_H_
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/fileinfobyhandleclass.h"
#include "libc/nt/enum/filemovemethod.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/enum/findexinfolevels.h"
#include "libc/nt/enum/findexsearchops.h"
#include "libc/nt/enum/getfileexinfolevels.h"
#include "libc/nt/enum/movefileexflags.h"
#include "libc/nt/enum/securityimpersonationlevel.h"
#include "libc/nt/enum/securityinformation.h"
#include "libc/nt/enum/tokentype.h"
#include "libc/nt/struct/filesegmentelement.h"
#include "libc/nt/struct/filetime.h"
#include "libc/nt/thunk/msabi.h"
#if 0
/*                            ░░░░
                       ▒▒▒░░░▒▒▒▒▒▒▒▓▓▓░
                      ▒▒▒▒░░░▒▒▒▒▒▒▓▓▓▓▓▓░
                     ▒▒▒▒░░░▒▒▒▒▒▒▒▓▓▓▓▓▓  ▒▓░
                     ▒▒▒░░░░▒▒▒▒▒▒▓▓▓▓▓▓   ▓▓▓▓▓▓▒        ▒▒▒▓▓█
                    ▒▒▒▒░░░▒▒▒▒▒▒▒▓▓▓▓▓▓  ▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓▓
                   ░▒▒▒░░░░▒▒▒▒▒▒▓▓▓▓▓▓   █▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓█
                   ▒▒▒▒░░░▒▒▒▒▒▒▒▓▓▓▓▓░  ▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓▓
                  ▒▒▒▒░░░▒▒▒▒▒▒▒▓▓▓▓▓▓  ▒▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓▒
                  ▒▒▒▒▓▓      ▓▒▒▓▓▓▓   ▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓█
                                   ▒▓  ▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓
                  ░░░░░░░░░░░▒▒▒▒      ▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓█
                ▒▒░░░░░░░░░░▒▒▒▒▒▓▓▓     ▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓▓
               ░▒░░░░░░░░░░░▒▒▒▒▒▓▓   ▓░      ░▓███▓
               ▒▒░░░░░░░░░░▒▒▒▒▒▓▓░  ▒▓▓▓▒▒▒         ░▒▒▒▓   ████████████
              ▒▒░░░░░░░░░░░▒▒▒▒▒▓▓  ▒▓▓▓▓▒▒▒▒▒▒▒▒░░░▒▒▒▒▒░           ░███
              ▒░░░░░░░░░░░▒▒▒▒▒▓▓   ▓▓▓▓▒▒▒▒▒▒▒▒░░░░▒▒▒▒▓            ███
             ▒▒░░░░░░░░░░▒▒▒▒▒▒▓▓  ▒▓▓▓▒▒▒▒▒▒▒▒░░░░▒▒▒▒▒            ▓██
             ▒░░░░░░░░░░░▒▒▒▒▒▓▓   ▓▓▓▓▒▒▒▒▒▒▒▒░░░▒▒▒▒▒▓           ▓██
            ▒▒░░░▒▒▒░░░▒▒░▒▒▒▓▓▒  ▒▓▓▓▒▒▒▒▒▒▒▒░░░░▒▒▒▒▒           ███
                            ░▒▓  ░▓▓▓▓▒▒▒▒▒▒▒▒░░░░▒▒▒▒▓          ▓██
╔────────────────────────────────────────────────────────────────▀▀▀─────────│─╗
│ cosmopolitan § new technology » files                                    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/
#endif

/* CopyFileEx */
#define NT_PROGRESS_CONTINUE                     0
#define NT_PROGRESS_CANCEL                       1
#define NT_PROGRESS_STOP                         2
#define NT_PROGRESS_QUIET                        3
#define NT_CALLBACK_CHUNK_FINISHED               0x00000000
#define NT_CALLBACK_STREAM_SWITCH                0x00000001
#define NT_COPY_FILE_FAIL_IF_EXISTS              0x00000001
#define NT_COPY_FILE_RESTARTABLE                 0x00000002
#define NT_COPY_FILE_OPEN_SOURCE_FOR_WRITE       0x00000004
#define NT_COPY_FILE_ALLOW_DECRYPTED_DESTINATION 0x00000008
#define NT_COPY_FILE_COPY_SYMLINK                0x00000800
#define NT_COPY_FILE_NO_BUFFERING                0x00001000
#define NT_COPY_FILE_REQUEST_SECURITY_PRIVILEGES 0x00002000 /* Win8+ */
#define NT_COPY_FILE_RESUME_FROM_PAUSE           0x00004000 /* Win8+ */
#define NT_COPY_FILE_REQUEST_SECURITY_PRIVILEGES 0x00002000 /* Win8+ */
#define NT_COPY_FILE_NO_OFFLOAD                  0x00040000 /* Win8+ */
#define NT_COPY_FILE_IGNORE_EDP_BLOCK            0x00400000 /* Win10+ */
#define NT_COPY_FILE_IGNORE_SOURCE_ENCRYPTION    0x00800000 /* Win10+ */

/* ReplaceFile */
#define NT_REPLACEFILE_WRITE_THROUGH       0x00000001
#define NT_REPLACEFILE_IGNORE_MERGE_ERRORS 0x00000002
#define NT_REPLACEFILE_IGNORE_ACL_ERRORS   0x00000004
#define NT_REPLACEFILE_WRITE_THROUGH       0x00000001
#define NT_REPLACEFILE_IGNORE_MERGE_ERRORS 0x00000002
#define NT_REPLACEFILE_IGNORE_ACL_ERRORS   0x00000004

/* SetHandleInformation */
#define kNtHandleFlagInherit          0x00000001
#define kNtHandleFlagProtectFromClose 0x00000002

#define kNtFindFirstExCaseSensitive 0x00000001
#define kNtFindFirstExLargeFetch    0x00000002

#define kNtDuplicateCloseSource 0x00000001
#define kNtDuplicateSameAccess  0x00000002

#define kNtSymbolicLinkFlagDirectory 1

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct NtByHandleFileInformation;
struct NtFileTime;
struct NtGenericMapping;
struct NtOverlapped;
struct NtPrivilegeSet;
struct NtSecurityAttributes;
struct NtSecurityDescriptor;
struct NtWin32FindData;
struct NtWin32FindData;

intptr_t LoadResource(int64_t hModule, int64_t hResInfo);
uint32_t SetHandleCount(uint32_t uNumber);
uint32_t GetLogicalDrives(void);
bool32 FlushFileBuffers(int64_t hFile);

int64_t ReOpenFile(int64_t hOriginalFile, uint32_t dwDesiredAccess,
                   uint32_t dwShareMode, uint32_t dwFlagsAndAttributes);

bool32 DeleteFile(const char16_t *lpFileName) paramsnonnull();

bool32 CopyFile(const char16_t *lpExistingFileName,
                const char16_t *lpNewFileName, bool32 bFailIfExists)
    paramsnonnull();

bool32 MoveFile(const char16_t *lpExistingFileName,
                const char16_t *lpNewFileName) paramsnonnull();
bool32 MoveFileEx(const char16_t *lpExistingFileName,
                  const char16_t *lpNewFileName, enum NtMoveFileExFlags dwFlags)
    paramsnonnull();

bool32 SetCurrentDirectory(const char16_t *lpPathName);
uint32_t GetCurrentDirectory(uint32_t nBufferLength, char16_t *out_lpBuffer);

bool32 CreateDirectory(const char16_t *lpPathName,
                       struct NtSecurityAttributes *lpSecurityAttributes);
bool32 RemoveDirectory(const char16_t *lpPathName);

int32_t DuplicateHandle(int64_t hSourceProcessHandle, int64_t hSourceHandle,
                        int64_t hTargetProcessHandle, int64_t *lpTargetHandle,
                        uint32_t dwDesiredAccess, bool32 bInheritHandle,
                        uint32_t dwOptions);

bool32 GetHandleInformation(int64_t hObject, uint32_t *out_lpdwFlags);
bool32 SetHandleInformation(int64_t hObject, uint32_t dwMask, uint32_t dwFlags);
enum NtFileType GetFileType(int64_t hFile);

bool32 GetFileInformationByHandleEx(int64_t hFile,
                                    uint32_t FileInformationClass,
                                    void *out_lpFileInformation,
                                    uint32_t dwBufferSize);

bool32 GetFileInformationByHandle(
    int64_t hFile, struct NtByHandleFileInformation *lpFileInformation);

uint32_t GetFileAttributes(const char16_t *lpFileName);
bool32 GetFileAttributesEx(
    const char16_t *lpFileName,
    enum NtGetFileexInfoLevels fInfoLevelId /* kNtGetFileExInfoStandard */,
    void *out_lpFileInformation /* → struct NtWin32FileAttributeData * */)
    paramsnonnull();

uint32_t GetCompressedFileSize(const char16_t *lpFileName,
                               uint32_t *lpFileSizeHigh);
bool32 SetFileAttributes(const char16_t *lpFileName, uint32_t dwFileAttributes);
bool32 GetFileTime(int64_t hFile, struct NtFileTime *opt_lpCreationFileTime,
                   struct NtFileTime *opt_lpLastAccessFileTime,
                   struct NtFileTime *opt_lpLastWriteFileTime);
bool32 SetFileTime(int64_t hFile,
                   const struct NtFileTime *opt_lpCreationFileTime,
                   const struct NtFileTime *opt_lpLastAccessFileTime,
                   const struct NtFileTime *opt_lpLastWriteFileTime);

bool32 DeviceIoControl(int64_t hDevice, uint32_t dwIoControlCode,
                       void *lpInBuffer, uint32_t nInBufferSize,
                       void *lpOutBuffer, uint32_t nOutBufferSize,
                       uint32_t *lpBytesReturned,
                       struct NtOverlapped *lpOverlapped);

bool32 LockFile(int64_t hFile, uint32_t dwFileOffsetLow,
                uint32_t dwFileOffsetHigh, uint32_t nNumberOfBytesToLockLow,
                uint32_t nNumberOfBytesToLockHigh);
bool32 LockFileEx(int64_t hFile, uint32_t dwFlags, uint32_t dwReserved,
                  uint32_t nNumberOfBytesToLockLow,
                  uint32_t nNumberOfBytesToLockHigh,
                  struct NtOverlapped *lpOverlapped) paramsnonnull();
bool32 UnlockFile(int64_t hFile, uint32_t dwFileOffsetLow,
                  uint32_t dwFileOffsetHigh, uint32_t nNumberOfBytesToUnlockLow,
                  uint32_t nNumberOfBytesToUnlockHigh);
bool32 UnlockFileEx(int64_t hFile, uint32_t dwReserved,
                    uint32_t nNumberOfBytesToUnlockLow,
                    uint32_t nNumberOfBytesToUnlockHigh,
                    struct NtOverlapped *lpOverlapped) paramsnonnull();

bool32 CreateHardLink(const char16_t *lpFileName,
                      const char16_t *lpExistingFileName,
                      struct NtSecurityAttributes *reserved)
    paramsnonnull((1, 2));
bool32 CreateSymbolicLink(const char16_t *lpSymlinkFileName,
                          const char16_t *lpTargetPathName, uint32_t dwFlags)
    paramsnonnull();

uint32_t SetFilePointer(int64_t hFile, int32_t lDistanceToMove,
                        int32_t *optional_lpDistanceToMoveHigh,
                        enum NtFileMoveMethod dwMoveMethod);
bool32 SetFilePointerEx(int64_t hFile, int64_t liDistanceToMove,
                        int64_t *optional_lpNewFilePointer,
                        enum NtFileMoveMethod dwMoveMethod);

bool32 SetEndOfFile(int64_t hFile);
bool32 SetFileValidData(int64_t hFile, int64_t ValidDataLength);

bool32 GetFileSecurity(const char16_t *lpFileName,
                       uint32_t RequestedInformation,
                       struct NtSecurityDescriptor *pSecurityDescriptor,
                       uint32_t nLength, uint32_t *lpnLengthNeeded);

bool32 OpenProcessToken(int64_t hProcessHandle, uint32_t dwDesiredAccess,
                        int64_t *out_hTokenHandle);
bool32 DuplicateToken(int64_t hExistingTokenHandle,
                      enum NtSecurityImpersonationLevel dwImpersonationLevel,
                      int64_t *out_hDuplicateTokenHandle);
bool32 DuplicateTokenEx(int64_t hExistingToken, unsigned int dwDesiredAccess,
                        struct NtSecurityAttributes *lpTokenAttributes,
                        enum NtSecurityImpersonationLevel ImpersonationLevel,
                        enum NtTokenType TokenType, int64_t *out_phNewToken);

bool32 AccessCheck(struct NtSecurityDescriptor *pSecurityDescriptor,
                   int64_t ClientToken, unsigned int DesiredAccess,
                   struct NtGenericMapping *lpGenericMapping,
                   struct NtPrivilegeSet *lpPrivilegeSet,
                   unsigned int *PrivilegeSetLength,
                   unsigned int *GrantedAccess, bool32 *AccessStatus);

void MapGenericMask(uint32_t *AccessMask,
                    struct NtGenericMapping *GenericMapping);

int64_t FindFirstFile(const char16_t *lpFileName,
                      struct NtWin32FindData *out_lpFindFileData);
int64_t FindFirstFileEx(const char16_t *lpFileName,
                        enum NtFindexInfoLevels fInfoLevelId,
                        void *out_lpFindFileData,
                        enum NtFindexSearchOps fSearchOp,
                        void *reserved_lpSearchFilter,
                        uint32_t dwAdditionalFlags);
bool32 FindNextFile(int64_t hFindFile,
                    struct NtWin32FindData *out_lpFindFileData);
bool32 FindClose(int64_t inout_hFindFile);

int64_t FindFirstVolume(char16_t *out_lpszVolumeName, uint32_t cchBufferLength);
bool32 FindNextVolume(int64_t inout_hFindVolume, char16_t *out_lpszVolumeName,
                      uint32_t cchBufferLength);
bool32 FindVolumeClose(int64_t hFindVolume);

bool32 ReadFileScatter(
    int64_t hFileOpenedWithOverlappedAndNoBuffering,
    const union NtFileSegmentElement
        aNullTerminatedPageAlignedSizedSegmentArray[],
    uint32_t nNumberOfBytesToReadThatsMultipleOfFileVolumeSectorSize,
    uint32_t *lpReserved, struct NtOverlapped *inout_lpOverlapped)
    paramsnonnull();
bool32 WriteFileGather(int64_t hFileOpenedWithOverlappedAndNoBuffering,
                       const union NtFileSegmentElement aSegmentArray[],
                       uint32_t nNumberOfBytesToWrite, uint32_t *lpReserved,
                       struct NtOverlapped inout_lpOverlapped) paramsnonnull();

#if ShouldUseMsabiAttribute()
#include "libc/nt/thunk/files.inc"
#endif /* ShouldUseMsabiAttribute() */
COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_FILES_H_ */
