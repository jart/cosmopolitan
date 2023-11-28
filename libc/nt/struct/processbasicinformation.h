#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_PROCESSBASICINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_PROCESSBASICINFORMATION_H_

struct NtProcessBasicInformation {
  int32_t ExitStatus;
  struct NtPeb *PebBaseAddress;
  uint32_t *AffinityMask;
  int32_t BasePriority;
  int64_t UniqueProcessId;
  int64_t InheritedFromUniqueProcessId;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_PROCESSBASICINFORMATION_H_ */
