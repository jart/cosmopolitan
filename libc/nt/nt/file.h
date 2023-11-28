#ifndef COSMOPOLITAN_LIBC_NT_NT_FILE_H_
#define COSMOPOLITAN_LIBC_NT_NT_FILE_H_
#include "libc/nt/enum/fileinformationclass.h"
#include "libc/nt/enum/status.h"
#include "libc/nt/struct/iostatusblock.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/nt/typedef/ioapcroutine.h"
COSMOPOLITAN_C_START_
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
│ cosmopolitan § new technology » beyond the pale » files                  ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│─╝
    “The functions and structures in [for these APIs] are internal to
     the operating system and subject to change from one release of
     Windows to the next, and possibly even between service packs for
     each release.” ──Quoth MSDN */

struct NtIoStatusBlock;
struct NtObjectAttributes;

NtStatus NtClose(int64_t handle);

NtStatus NtCreateFile(int64_t *out_FileHandle, uint32_t DesiredAccess,
                      const struct NtObjectAttributes *ObjectAttributes,
                      struct NtIoStatusBlock *out_IoStatusBlock,
                      int64_t *opt_AllocationSize, uint32_t FileAttributes,
                      uint32_t ShareAccess, uint32_t CreateDisposition,
                      uint32_t CreateOptions, void *opt_EaBuffer,
                      uint32_t EaLength);

NtStatus NtReadFile(int64_t FileHandle, void *opt_Event,
                    NtIoApcRoutine opt_ApcRoutine, void *opt_ApcContext,
                    struct NtIoStatusBlock *out_IoStatusBlock, void *out_Buffer,
                    uint32_t Length, int64_t *opt_ByteOffset,
                    uint32_t *opt_Key);

NtStatus NtWriteFile(int64_t FileHandle, void *opt_Event,
                     NtIoApcRoutine opt_ApcRoutine, void *opt_ApcContext,
                     struct NtIoStatusBlock *out_IoStatusBlock,
                     const void *Buffer, uint32_t Length,
                     int64_t *opt_ByteOffset, uint32_t *opt_Key);

NtStatus NtDuplicateObject(int64_t SourceProcessHandle, void *SourceHandle,
                           void *TargetProcessHandle,
                           void **opt_out_TargetHandle, uint32_t DesiredAcess,
                           uint32_t Attributes, uint32_t options_t);

NtStatus NtQueryInformationFile(int64_t FileHandle,
                                struct NtIoStatusBlock *out_IoStatusBlock,
                                void *out_FileInformation,
                                uint32_t FileInformationLength,
                                uint32_t FileInformationClass);

NtStatus NtSetInformationFile(int64_t FileHandle,
                              struct NtIoStatusBlock *out_IoStatusBlock,
                              void *FileInformation,
                              uint32_t FileInformationLength,
                              uint32_t FileInformationClass);

NtStatus NtDeviceIoControlFile(
    int64_t FileHandle, int64_t opt_Event, NtIoApcRoutine opt_ApcRoutine,
    void *opt_ApcContext, struct NtIoStatusBlock *out_IoStatusBlock,
    uint32_t IoControlCode, void *opt_InputBuffer, uint32_t InputBufferLength,
    void *opt_out_OutputBuffer, uint32_t OutputBufferLength);

NtStatus NtCancelIoFileEx(int64_t FileHandle,
                          struct NtIoStatusBlock *IoRequestToCancel,
                          struct NtIoStatusBlock *IoStatusBlock);

NtStatus RtlNtStatusToDosError(NtStatus Status);

#if ShouldUseMsabiAttribute()
#include "libc/nt/thunk/ntfile.inc"
#endif /* ShouldUseMsabiAttribute() */
COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_NT_FILE_H_ */
