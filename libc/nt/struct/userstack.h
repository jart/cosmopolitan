#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_NTUSERSTACK_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_NTUSERSTACK_H_

struct NtUserStack {
  void *FixedStackBase;
  void *FixedStackLimit;
  void *ExpandableStackBase;
  void *ExpandableStackLimit;
  void *ExpandableStackBottom;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_NTUSERSTACK_H_ */
