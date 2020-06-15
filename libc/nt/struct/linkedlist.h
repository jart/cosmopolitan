#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_LINKEDLIST_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_LINKEDLIST_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * Dynamic linked list overlay.
 */
struct NtLinkedList {
  struct NtLinkedList *Next;
  struct NtLinkedList *Prev;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_LINKEDLIST_H_ */
