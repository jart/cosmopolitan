#ifndef COSMOPOLITAN_LIBC_NT_NTDLL_H_
#define COSMOPOLITAN_LIBC_NT_NTDLL_H_
#include "libc/nt/struct/context.h"
#include "libc/nt/struct/criticalsection.h"
#include "libc/nt/struct/filebasicinformation.h"
#include "libc/nt/struct/filenetworkopeninformation.h"
#include "libc/nt/struct/iostatusblock.h"
#include "libc/nt/struct/ntexceptionrecord.h"
#include "libc/nt/struct/objectattributes.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/nt/typedef/ioapcroutine.h"
#include "libc/nt/typedef/pknormalroutine.h"
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
│ cosmopolitan § new technology » beyond the pale                          ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│─╝
    “The functions and structures in [for these APIs] are internal to
     the operating system and subject to change from one release of
     Windows to the next, and possibly even between service packs for
     each release.” ──Quoth MSDN */

#define __nt_system_call_dispatcher (wambda *)0x7ffe0308

extern const struct NtUnicodeString *const RtlNtdllName;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § new technology » beyond the pale » eponymous runtime      ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define NT_PROCESS_FLAGS_CREATE_SUSPENDED 0x00000001
#define NT_PROCESS_FLAGS_INHERIT_HANDLES  0x00000002
#define NT_PROCESS_FLAGS_NO_SYNCHRONIZE   0x00000004
#define NT_RTL_CLONE_PARENT               0
#define NT_RTL_CLONE_CHILD                297

NtStatus NtCallbackReturn(void *opt_Result, uint32_t ResultLength,
                          int32_t Status);
NtStatus NtTestAlert(void);

NtStatus NtOpenFile(int64_t *out_FileHandle, uint32_t DesiredAccess,
                    struct NtObjectAttributes *ObjectAttributes,
                    struct NtIoStatusBlock *out_IoStatusBlock,
                    uint32_t ShareAccess, uint32_t OpenOptions);

NtStatus NtQueryInformationToken(int64_t TokenHandle,
                                 uint32_t TokenInformationClass,
                                 void *out_TokenInformation,
                                 uint32_t TokenInformationLength,
                                 uint32_t *out_ReturnLength);
NtStatus NtYieldExecution(void);
NtStatus NtQuerySystemInformation(uint32_t info_class, void *out_info,
                                  uint32_t info_size,
                                  uint32_t *out_bytes_received);
NtStatus NtReadVirtualMemory(int64_t ProcessHandle, const void *BaseAddress,
                             void *out_Buffer, size_t BufferLength,
                             size_t *opt_out_ReturnLength);
NtStatus NtCreateTimer(void **out_TimerHandle, uint32_t DesiredAccess,
                       struct NtObjectAttributes *ObjectAttributes,
                       uint32_t TimerType);
NtStatus NtSetTimer(void *TimerHandle, int64_t *DueTime, void *TimerApcRoutine,
                    void *TimerContext, int32_t Resume, int32_t Period,
                    int32_t *out_PreviousState);
NtStatus NtQueryObject(void *ObjectHandle, int ObjectInformationClass,
                       void *out_ObjectInformation,
                       uint32_t ObjectInformationLength,
                       uint32_t *opt_out_ReturnLength);
NtStatus NtQueryFullAttributesFile(
    struct NtObjectAttributes *attributes,
    struct NtFileNetworkOpenInformation *out_info);
NtStatus NtCreateKey(void **out_KeyHandle, uint32_t DesiredAccess,
                     struct NtObjectAttributes *ObjectAttributes,
                     uint32_t TitleIndex, struct NtUnicodeString *opt_Class,
                     uint32_t CreateOptions, uint32_t *opt_out_Disposition);
NtStatus NtOpenKey(void **out_KeyHandle, uint32_t DesiredAccess,
                   struct NtObjectAttributes *ObjectAttributes);
NtStatus NtSetValueKey(void *KeyHandle, struct NtUnicodeString *ValueName,
                       uint32_t opt_TitleIndex, uint32_t Type, void *Data,
                       uint32_t DataSize);
NtStatus NtDeleteKey(void *KeyHandle);
NtStatus NtQueryValueKey(void *KeyHandle, struct NtUnicodeString *ValueName,
                         int KeyValueInformationClass,
                         void *out_KeyValueInformation, uint32_t Length,
                         uint32_t *out_ResultLength);
NtStatus NtFlushKey(void *KeyHandle);
NtStatus NtEnumerateKey(int64_t hkey, uint32_t index, int info_class,
                        void *out_key_info, uint32_t key_info_size,
                        uint32_t *out_bytes_received);
NtStatus NtEnumerateValueKey(int64_t hKey, uint32_t index, int info_class,
                             void *out_key_info, uint32_t key_info_size,
                             uint32_t *out_bytes_received);
NtStatus NtQuerySystemTime(int64_t *SystemTime);
NtStatus NtDeleteFile(struct NtObjectAttributes *ObjectAttributes);
NtStatus NtFlushBuffersFile(int64_t FileHandle,
                            struct NtIoStatusBlock *out_IoStatusBlock);
NtStatus NtCreateIoCompletion(void **out_IoCompletionHandle,
                              uint32_t DesiredAccess,
                              struct NtObjectAttributes *ObjectAttributes,
                              uint32_t NumberOfConcurrentThreads);
NtStatus NtRaiseHardError(int32_t ErrorStatus, uint32_t NumberOfArguments,
                          uint32_t UnicodeStringArgumentsMask, void *Arguments,
                          uint32_t MessageBoxType,
                          uint32_t *out_MessageBoxResult);
NtStatus NtRaiseException(struct NtExceptionRecord *ExceptionRecord,
                          struct NtContext *Context, int32_t SearchFrames);
NtStatus NtCreateEvent(void **out_EventHandle, uint32_t DesiredAccess,
                       struct NtObjectAttributes *ObjectAttributes,
                       int EventType, int32_t InitialState);
NtStatus NtWaitForSingleObject(void *ObjectHandle, int32_t Alertable,
                               int64_t *TimeOut);
NtStatus NtSetEvent(void *EventHandle, int32_t *opt_out_PreviousState);
NtStatus NtClearEvent(void *EventHandle);
NtStatus NtSignalAndWaitForSingleObject(void *ObjectToSignal,
                                        void *WaitableObject, int32_t Alertable,
                                        int64_t *opt_Time);
NtStatus NtQueryPerformanceCounter(int64_t *out_PerformanceCount,
                                   int64_t *opt_out_PerformanceFrequency);
NtStatus NtFsControlFile(int64_t FileHandle, void *opt_Event,
                         NtIoApcRoutine opt_ApcRoutine, void *opt_ApcContext,
                         struct NtIoStatusBlock *out_IoStatusBlock,
                         uint32_t FsControlCode, void *opt_InputBuffer,
                         uint32_t InputBufferLength, void *opt_out_OutputBuffer,
                         uint32_t OutputBufferLength);
NtStatus NtCancelIoFile(int64_t FileHandle,
                        struct NtIoStatusBlock *out_IoStatusBlock);
NtStatus NtCreateProfile(void **out_ProfileHandle, int64_t ProcessHandle,
                         void *Base, uint32_t Size, uint32_t BucketShift,
                         uint32_t *Buffer, uint32_t BufferLength, int Source,
                         uint32_t ProcessorMask);
NtStatus NtSetIntervalProfile(uint32_t Interval, int Source);
NtStatus NtQueryIntervalProfile(int Source, uint32_t *out_Interval);
NtStatus NtStartProfile(void *ProfileHandle);
NtStatus NtStopProfile(void *ProfileHandle);
NtStatus NtCreateDirectoryObject(int64_t *out_DirectoryHandle,
                                 uint32_t DesiredAccess,
                                 struct NtObjectAttributes *ObjectAttributes);
NtStatus NtOpenDirectoryObject(int64_t *out_DirectoryHandle,
                               uint32_t DesiredAccess,
                               struct NtObjectAttributes *ObjectAttributes);
NtStatus NtOpenSymbolicLinkObject(int64_t *out_DirectoryHandle,
                                  uint32_t DesiredAccess,
                                  struct NtObjectAttributes *ObjectAttributes);
NtStatus NtQuerySymbolicLinkObject(int64_t DirectoryHandle,
                                   struct NtUnicodeString *inout_TargetName,
                                   uint32_t *opt_out_ReturnLength);
NtStatus ZwAreMappedFilesTheSame(void *Address1, void *Address2);
NtStatus NtQueryVolumeInformationFile(int64_t FileHandle,
                                      struct NtIoStatusBlock *out_IoStatusBlock,
                                      void *out_FsInformation, uint32_t Length,
                                      uint32_t FsInformationClass);
NtStatus NtQuerySecurityObject(
    int64_t handle, int RequestedInformation,
    struct NtSecurityDescriptor *out_SecurityDescriptor,
    uint32_t SecurityDescriptorLength, uint32_t *out_ReturnLength);
NtStatus NtQueueApcThread(int64_t ThreadHandle, NtPkNormalRoutine ApcRoutine,
                          void *opt_ApcContext, void *opt_Argument1,
                          void *opt_Argument2);
NtStatus NtFlushInstructionCache(int64_t ProcessHandle, void *opt_BaseAddress,
                                 size_t FlushSize);
NtStatus NtQueryAttributesFile(const struct NtObjectAttributes *object,
                               struct NtFileBasicInformation *file_information);
NtStatus NtQueryDirectoryFile(
    int64_t FileHandle, void *opt_Event, NtIoApcRoutine opt_ApcRoutine,
    void *opt_ApcContext, struct NtIoStatusBlock *out_IoStatusBlock,
    void *out_FileInformation, uint32_t FileInformationLength,
    uint32_t FileInformationClass, int32_t ReturnSingleEntry,
    struct NtUnicodeString *opt_FileName, int32_t RestartScan);
NtStatus NtFlushVirtualMemory(int64_t ProcessHandle, void **inout_BaseAddress,
                              uint32_t **inout_FlushSize,
                              struct NtIoStatusBlock *out_IoStatusBlock);
NtStatus NtQueryInformationJobObject(void *JobHandle, int JobInformationClass,
                                     void *out_JobInformation,
                                     uint32_t JobInformationLength,
                                     uint32_t *opt_out_ReturnLength);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § new technology » beyond the pale » runtime library        ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

NtStatus RtlInitializeCriticalSection(struct NtCriticalSection *out_crit);
NtStatus RtlDeleteCriticalSection(struct NtCriticalSection *crit);
NtStatus RtlEnterCriticalSection(struct NtCriticalSection *inout_crit);
NtStatus RtlLeaveCriticalSection(struct NtCriticalSection *inout_crit);
NtStatus RtlTryEnterCriticalSection(struct NtCriticalSection *inout_crit);
NtStatus RtlInitUnicodeString(struct NtUnicodeString *inout_DestinationString,
                              const char16_t *SourceString);
void RtlFreeUnicodeString(struct NtUnicodeString **string);
NtStatus RtlQueryEnvironmentVariable_U(char16_t *Environment,
                                       struct NtUnicodeString *Name,
                                       struct NtUnicodeString *Value);
NtStatus RtlConvertSidToUnicodeString(struct NtUnicodeString *out_UnicodeString,
                                      void *Sid,
                                      int32_t AllocateDestinationString);
void *RtlCreateHeap(uint32_t flags, void *base, size_t reserve_sz,
                    size_t commit_sz, void *lock, void *params);
NtStatus RtlDestroyHeap(void *base);
void *RtlAllocateHeap(int64_t heap, uint32_t flags, size_t size);
void *RtlReAllocateHeap(int64_t heap, uint32_t flags, void *ptr, size_t size);
NtStatus RtlFreeHeap(int64_t heap, uint32_t flags, void *ptr);
size_t RtlSizeHeap(int64_t heap, uint32_t flags, void *ptr);
NtStatus RtlValidateHeap(int64_t heap, uint32_t flags, void *ptr);
NtStatus RtlLockHeap(int64_t heap);
NtStatus RtlUnlockHeap(int64_t heap);
NtStatus RtlGetProcessHeaps(uint32_t count, void **out_Heaps);
NtStatus RtlWalkHeap(int64_t heap, void *out_Info);

#if ShouldUseMsabiAttribute()
#include "libc/nt/thunk/ntdll.inc"
#endif /* ShouldUseMsabiAttribute() */
COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_NTDLL_H_ */
