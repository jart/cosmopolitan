#ifndef COSMOPOLITAN_LIBC_NT_ENUM_PROCESSINFOCLASS_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_PROCESSINFOCLASS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

enum NtProcessInfoClass {
  kNtProcessBasicInformation = 0,
  kNtProcessQuotaLimits = 1,
  kNtProcessIoCounters = 2,
  kNtProcessVmCounters = 3,
  kNtProcessTimes = 4,
  kNtProcessBasePriority = 5,
  kNtProcessRaisePriority = 6,
  kNtProcessDebugPort = 7,
  kNtProcessExceptionPort = 8,
  kNtProcessAccessToken = 9,
  kNtProcessWow64Information = 26,
  kNtProcessImageFileName = 27
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_ENUM_PROCESSINFOCLASS_H_ */
