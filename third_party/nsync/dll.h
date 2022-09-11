#ifndef NSYNC_DLL_H_
#define NSYNC_DLL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/* A nsync_dll_element_ represents an element of a doubly-linked list of
   waiters. */
typedef struct nsync_dll_element_s_ {
  struct nsync_dll_element_s_ *next;
  struct nsync_dll_element_s_ *prev;
  /* points to the struct this nsync_dll struct is embedded in. */
  void *container;
} nsync_dll_element_;

/* A nsync_dll_list_ represents a list of nsync_dll_elements_. */
typedef nsync_dll_element_ *nsync_dll_list_; /* last elem of circular list; nil
                                                => empty; first is x.next. */

/* Initialize *e. */
void nsync_dll_init_(nsync_dll_element_ *e, void *container);

/* Return whether list is empty. */
int nsync_dll_is_empty_(nsync_dll_list_ list);

/* Remove *e from list, and returns the new list. */
nsync_dll_list_ nsync_dll_remove_(nsync_dll_list_ list, nsync_dll_element_ *e);

/* Cause element *n and its successors to come after element *p.
   Requires n and p are non-NULL and do not point at elements of the
   same list. */
void nsync_dll_splice_after_(nsync_dll_element_ *p, nsync_dll_element_ *n);

/* Make element *e the first element of list, and return the list. The
   resulting list will have *e as its first element, followed by any
   elements in the same list as *e, followed by the elements that were
   previously in list. Requires that *e not be in list. If e==NULL, list
   is returned unchanged. */
nsync_dll_list_ nsync_dll_make_first_in_list_(nsync_dll_list_ list,
                                              nsync_dll_element_ *e);

/* Make element *e the last element of list, and return the list. The
   resulting list will have *e as its last element, preceded by any
   elements in the same list as *e, preceded by the elements that were
   previously in list. Requires that *e not be in list. If e==NULL, list
   is returned unchanged. */
nsync_dll_list_ nsync_dll_make_last_in_list_(nsync_dll_list_ list,
                                             nsync_dll_element_ *e);

/* Return a pointer to the first element of list, or NULL if list is
 * empty. */
nsync_dll_element_ *nsync_dll_first_(nsync_dll_list_ list);

/* Return a pointer to the last element of list, or NULL if list is
 * empty. */
nsync_dll_element_ *nsync_dll_last_(nsync_dll_list_ list);

/* Return a pointer to the next element of list following *e, or NULL if
   there is no such element. */
nsync_dll_element_ *nsync_dll_next_(nsync_dll_list_ list,
                                    nsync_dll_element_ *e);

/* Return a pointer to the previous element of list following *e, or
   NULL if there is no such element. */
nsync_dll_element_ *nsync_dll_prev_(nsync_dll_list_ list,
                                    nsync_dll_element_ *e);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* NSYNC_DLL_H_ */
