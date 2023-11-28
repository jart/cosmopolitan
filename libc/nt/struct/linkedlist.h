#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_LINKEDLIST_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_LINKEDLIST_H_

/**
 * Dynamic linked list overlay.
 */
struct NtLinkedList {
  struct NtLinkedList *Next;
  struct NtLinkedList *Prev;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_LINKEDLIST_H_ */
