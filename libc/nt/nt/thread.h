#ifndef COSMOPOLITAN_LIBC_NT_NT_THREAD_H_
#define COSMOPOLITAN_LIBC_NT_NT_THREAD_H_
#include "libc/nt/enum/status.h"
#include "libc/nt/enum/threadinfoclass.h"
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
│ cosmopolitan § new technology » beyond the pale » threads                ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│─╝
    “The functions and structures in [for these APIs] are internal to
     the operating system and subject to change from one release of
     Windows to the next, and possibly even between service packs for
     each release.” ──Quoth MSDN */

struct NtClientId;
struct NtContext;
struct NtObjectAttributes;
struct NtUserStack;

NtStatus NtCreateThread(int64_t *out_ThreadHandle, uint32_t DesiredAccess,
                        struct NtObjectAttributes *ObjectAttributes,
                        int64_t ProcessHandle, struct NtClientId *out_ClientId,
                        struct NtContext *ThreadContext,
                        struct NtUserStack *UserStack, int32_t CreateSuspended);

NtStatus NtTerminateThread(void *opt_ThreadHandle, int32_t ExitStatus);

NtStatus NtOpenThread(int64_t *out_ThreadHandle, uint32_t DesiredAccess,
                      struct NtObjectAttributes *ObjectAttributes,
                      struct NtClientId *ClientId);

NtStatus NtQueryInformationThread(int64_t ThreadHandle,
                                  enum NtThreadInfoClass ThreadInformationClass,
                                  void *out_ThreadInformation,
                                  uint32_t ThreadInformationLength,
                                  uint32_t *opt_out_ReturnLength);

NtStatus NtGetContextThread(int64_t ThreadHandle,
                            struct NtContext *out_Context);
NtStatus NtSetContextThread(int64_t ThreadHandle, struct NtContext *Context);
NtStatus NtSuspendThread(int64_t ThreadHandle,
                         uint32_t *opt_out_PreviousSuspendCount);
NtStatus NtResumeThread(int64_t ThreadHandle,
                        uint32_t *opt_out_PreviousSuspendCount);

NtStatus NtOpenThreadToken(int64_t ThreadHandle, uint32_t DesiredAccess,
                           int32_t OpenAsSelf, int64_t *out_TokenHandle);

NtStatus NtSetInformationThread(int64_t ThreadHandle,
                                enum NtThreadInfoClass ThreadInformationClass,
                                void *ThreadInformation,
                                uint32_t ThreadInformationLength);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_NT_THREAD_H_ */
