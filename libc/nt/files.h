#ifndef COSMOPOLITAN_LIBC_NT_FILES_H_
#define COSMOPOLITAN_LIBC_NT_FILES_H_
#include "libc/nt/struct/byhandlefileinformation.h"
#include "libc/nt/struct/filesegmentelement.h"
#include "libc/nt/struct/filetime.h"
#include "libc/nt/struct/genericmapping.h"
#include "libc/nt/struct/objectattributes.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/nt/struct/privilegeset.h"
#include "libc/nt/struct/securityattributes.h"
#include "libc/nt/struct/win32finddata.h"
#include "libc/nt/thunk/msabi.h"
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

#define kNtHandleFlagInherit          1 /* SetHandleInformation */
#define kNtHandleFlagProtectFromClose 2

#define kNtFindFirstExCaseSensitive 1
#define kNtFindFirstExLargeFetch    2

#define kNtDuplicateCloseSource 1
#define kNtDuplicateSameAccess  2

COSMOPOLITAN_C_START_

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
                  const char16_t *opt_lpNewFileName, int dwFlags)
    paramsnonnull((1));

bool32 SetCurrentDirectory(const char16_t *lpPathName);
uint32_t GetCurrentDirectory(uint32_t nBufferLength, char16_t *out_lpBuffer);

bool32 CreateDirectory(const char16_t *lpPathName,
                       const struct NtSecurityAttributes *lpSecurityAttributes);
bool32 RemoveDirectory(const char16_t *lpPathName);

bool32 DuplicateHandle(int64_t hSourceProcessHandle, int64_t hSourceHandle,
                       int64_t hTargetProcessHandle, int64_t *lpTargetHandle,
                       uint32_t dwDesiredAccess, bool32 bInheritHandle,
                       uint32_t dwOptions);

bool32 GetHandleInformation(int64_t hObject, uint32_t *out_lpdwFlags);
bool32 SetHandleInformation(int64_t hObject, uint32_t dwMask, uint32_t dwFlags);
int GetFileType(int64_t hFile);

bool32 GetFileInformationByHandleEx(int64_t hFile,
                                    uint32_t FileInformationClass,
                                    void *out_lpFileInformation,
                                    uint32_t dwBufferSize);

bool32 GetFileInformationByHandle(
    int64_t hFile, struct NtByHandleFileInformation *lpFileInformation);
bool32 SetFileInformationByHandle(int64_t hFile, int FileInformationClass,
                                  const void *lpFileInformation,
                                  uint32_t dwBufferSize);

uint32_t GetFileAttributes(const char16_t *lpFileName);
bool32 GetFileAttributesEx(
    const char16_t *lpFileName, int fInfoLevelId /* kNtGetFileExInfoStandard */,
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
bool32 UnlockFile(int64_t hFile, uint32_t dwFileOffsetLow,
                  uint32_t dwFileOffsetHigh, uint32_t nNumberOfBytesToUnlockLow,
                  uint32_t nNumberOfBytesToUnlockHigh);
bool32 LockFileEx(int64_t hFile, uint32_t dwFlags, uint32_t dwReserved,
                  uint32_t nNumberOfBytesToLockLow,
                  uint32_t nNumberOfBytesToLockHigh,
                  struct NtOverlapped *lpOverlapped) paramsnonnull();
bool32 UnlockFileEx(int64_t hFile, uint32_t dwReserved,
                    uint32_t nNumberOfBytesToUnlockLow,
                    uint32_t nNumberOfBytesToUnlockHigh,
                    struct NtOverlapped *lpOverlapped) paramsnonnull();

bool32 CreateHardLink(const char16_t *lpFileName,
                      const char16_t *lpExistingFileName,
                      const struct NtSecurityAttributes *reserved)
    paramsnonnull((1, 2));
bool32 CreateSymbolicLink(const char16_t *lpSymlinkFileName,
                          const char16_t *lpTargetPathName, uint32_t dwFlags)
    paramsnonnull();

bool32 SetEndOfFile(int64_t hFile);
bool32 SetFileValidData(int64_t hFile, int64_t ValidDataLength);

bool32 GetFileSecurity(const char16_t *lpFileName,
                       uint32_t RequestedInformation,
                       struct NtSecurityDescriptor *pSecurityDescriptor,
                       uint32_t nLength, uint32_t *lpnLengthNeeded);

bool32 OpenProcessToken(int64_t hProcessHandle, uint32_t dwDesiredAccess,
                        int64_t *out_hTokenHandle);
bool32 DuplicateToken(int64_t hExistingTokenHandle, int dwImpersonationLevel,
                      int64_t *out_hDuplicateTokenHandle);
bool32 DuplicateTokenEx(int64_t hExistingToken, unsigned int dwDesiredAccess,
                        const struct NtSecurityAttributes *lpTokenAttributes,
                        int ImpersonationLevel, int TokenType,
                        int64_t *out_phNewToken);

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
int64_t FindFirstFileEx(const char16_t *lpFileName, int fInfoLevelId,
                        void *out_lpFindFileData, int fSearchOp,
                        void *reserved_lpSearchFilter,
                        uint32_t dwAdditionalFlags);
bool32 FindNextFile(int64_t hFindFile,
                    struct NtWin32FindData *out_lpFindFileData);
bool32 FindClose(int64_t hFindFile);

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

#define kNtFileNameNormalized 0x0
#define kNtFileNameOpened     0x8
#define kNtVolumeNameDos      0x0 /* e.g. \\?\C:\Users\jart */
#define kNtVolumeNameGuid     0x1 /* e.g. \\?\Volume{ea38-etc.}\Users\jart */
#define kNtVolumeNameNt       0x2 /* e.g. \Device\HarddiskVolume4\Users\jart */
#define kNtVolumeNameNone     0x4 /* e.g. \Users\jart */
uint32_t GetFinalPathNameByHandle(int64_t hFile, char16_t *out_path,
                                  uint32_t arraylen, uint32_t flags);

uint32_t GetFullPathName(const char16_t *lpFileName, uint32_t nBufferLength,
                         char16_t *lpBuffer, char16_t **lpFilePart);

bool32 GetOverlappedResult(int64_t hFile, struct NtOverlapped *lpOverlapped,
                           uint32_t *lpNumberOfBytesTransferred, bool32 bWait);
bool32 GetOverlappedResultEx(int64_t hFile, struct NtOverlapped *lpOverlapped,
                             uint32_t *lpNumberOfBytesTransferred,
                             uint32_t dwMilliseconds, bool32 bAlertable);

bool32 GetVolumePathName(const char16_t *lpszFileName,
                         char16_t *lpszVolumePathName,
                         uint32_t cchBufferLength);

bool32 GetVolumeInformationByHandle(int64_t hFile,
                                    char16_t *opt_out_lpVolumeNameBuffer,
                                    uint32_t nVolumeNameSize,
                                    uint32_t *opt_out_lpVolumeSerialNumber,
                                    uint32_t *opt_out_lpMaximumComponentLength,
                                    uint32_t *opt_out_lpFileSystemFlags,
                                    char16_t *opt_out_lpFileSystemNameBuffer,
                                    uint32_t nFileSystemNameSize);

#if ShouldUseMsabiAttribute()
#include "libc/nt/thunk/files.inc"
#endif /* ShouldUseMsabiAttribute() */
COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_FILES_H_ */
