#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMREGISTRYQUOTAINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMREGISTRYQUOTAINFORMATION_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtSystemRegistryQuotaInformation {
  uint32_t RegistryQuotaAllowed;
  uint32_t RegistryQuotaUsed;
  void *Reserved1;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMREGISTRYQUOTAINFORMATION_H_ */
