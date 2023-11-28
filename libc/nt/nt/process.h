#ifndef COSMOPOLITAN_LIBC_NT_NT_PROCESS_H_
#define COSMOPOLITAN_LIBC_NT_NT_PROCESS_H_
#include "libc/nt/enum/status.h"
#include "libc/nt/thunk/msabi.h"
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
│ cosmopolitan § new technology » beyond the pale » processes              ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│─╝
    “The functions and structures in [for these APIs] are internal to
     the operating system and subject to change from one release of
     Windows to the next, and possibly even between service packs for
     each release.” ──Quoth MSDN */

struct NtClientId;
struct NtObjectAttributes;
struct NtRtlUserProcessInformation;
struct NtRtlUserProcessParameters;
struct NtSecurityDescriptor;
struct NtUnicodeString;

NtStatus NtCreateProcess(int64_t *out_ProcessHandle, uint32_t dwDesiredAccess,
                         struct NtObjectAttributes *opt_ObjectAttributes,
                         void *InheritFromProcessHandle, int32_t InheritHandles,
                         void *opt_SectionHandle, void *opt_DebugPort,
                         void *opt_ExceptionPort);
NtStatus NtTerminateProcess(int64_t opt_ProcessHandle, int32_t ExitStatus);

NtStatus NtQueryInformationProcess(int64_t ProcessHandle,
                                   int ProcessInformationClass,
                                   void *out_ProcessInformation,
                                   uint32_t ProcessInformationLength,
                                   uint32_t *opt_out_ReturnLength);
NtStatus NtOpenProcessToken(int64_t ProcessToken, uint32_t DesiredAccess,
                            int64_t *out_TokenHandle);
NtStatus NtOpenProcess(int64_t *out_ProcessHandle, uint32_t DesiredAccess,
                       struct NtObjectAttributes *ObjectAttributes,
                       struct NtClientId *ClientId);

NtStatus RtlCreateProcessParameters(
    struct NtRtlUserProcessParameters **out_ProcessParameters,
    struct NtUnicodeString *ImageFile, struct NtUnicodeString *opt_DllPath,
    struct NtUnicodeString *opt_CurrentDirectory,
    struct NtUnicodeString *opt_CommandLine, uint32_t CreationFlags,
    struct NtUnicodeString *opt_WindowTitle,
    struct NtUnicodeString *opt_Desktop, struct NtUnicodeString *opt_Reserved,
    struct NtUnicodeString *opt_Reserved2);

NtStatus RtlDestroyProcessParameters(
    struct NtRtlUserProcessParameters *ProcessParameters);

NtStatus RtlCloneUserProcess(
    uint32_t ProcessFlags,
    struct NtSecurityDescriptor *opt_ProcessSecurityDescriptor,
    struct NtSecurityDescriptor *opt_ThreadSecurityDescriptor,
    void *opt_DebugPort,
    struct NtRtlUserProcessInformation *ProcessInformation);

#if ShouldUseMsabiAttribute()
#include "libc/nt/nt/thunk/process.inc"
#endif /* ShouldUseMsabiAttribute() */
COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_NT_PROCESS_H_ */
