#ifndef COSMOPOLITAN_LIBC_NT_ENUM_THREADSTATE_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_THREADSTATE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

enum NtThreadState {
  kNtStateInitialized = 0,
  kNtStateReady,
  kNtStateRunning,
  kNtStateStandby,
  kNtStateTerminated,
  kNtStateWait,
  kNtStateTransition,
  kNtStateUnknown
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_ENUM_THREADSTATE_H_ */
