#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_CLIENTID_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_CLIENTID_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtClientId {
  void *UniqueProcess;
  void *UniqueThread;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_CLIENTID_H_ */
