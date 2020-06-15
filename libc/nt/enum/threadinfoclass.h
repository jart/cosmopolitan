#ifndef COSMOPOLITAN_LIBC_NT_ENUM_THREADINFOCLASS_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_THREADINFOCLASS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

enum NtThreadInfoClass {
  kNtThreadBasicInformation,
  kNtThreadTimes,
  kNtThreadPriority,
  kNtThreadBasePriority,
  kNtThreadAffinityMask,
  kNtThreadImpersonationToken,
  kNtThreadDescriptorTableEntry,
  kNtThreadEnableAlignmentFaultFixup,
  kNtThreadEventPair_Reusable,
  kNtThreadQuerySetWin32StartAddress,
  kNtThreadZeroTlsCell,
  kNtThreadPerformanceCount,
  kNtThreadAmILastThread,
  kNtThreadIdealProcessor,
  kNtThreadPriorityBoost,
  kNtThreadSetTlsArrayAddress,
  kNtThreadIsIoPending,
  kNtThreadHideFromDebugger,
  kNtThreadInfoClass_MAX
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_ENUM_THREADINFOCLASS_H_ */
