#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_NTUSERSTACK_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_NTUSERSTACK_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtUserStack {
  void *FixedStackBase;
  void *FixedStackLimit;
  void *ExpandableStackBase;
  void *ExpandableStackLimit;
  void *ExpandableStackBottom;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_NTUSERSTACK_H_ */
