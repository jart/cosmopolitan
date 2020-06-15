#ifndef COSMOPOLITAN_LIBC_NT_GENERICMAPPING_H_
#define COSMOPOLITAN_LIBC_NT_GENERICMAPPING_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtGenericMapping {
  unsigned int GenericRead;
  unsigned int GenericWrite;
  unsigned int GenericExecute;
  unsigned int GenericAll;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_GENERICMAPPING_H_ */
