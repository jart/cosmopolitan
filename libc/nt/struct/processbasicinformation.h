#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_PROCESSBASICINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_PROCESSBASICINFORMATION_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtProcessBasicInformation {
  int32_t ExitStatus;
  struct NtPeb *PebBaseAddress;
  uint32_t *AffinityMask;
  int32_t BasePriority;
  int64_t UniqueProcessId;
  int64_t InheritedFromUniqueProcessId;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_PROCESSBASICINFORMATION_H_ */
