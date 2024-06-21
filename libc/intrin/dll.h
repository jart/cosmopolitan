#ifdef _COSMO_SOURCE
#ifndef COSMOPOLITAN_LIBC_INTRIN_DLL_H_
#define COSMOPOLITAN_LIBC_INTRIN_DLL_H_
#define dll_make_first   __dll_make_first
#define dll_make_last    __dll_make_last
#define dll_remove       __dll_remove
#define dll_splice_after __dll_splice_after
COSMOPOLITAN_C_START_

#define DLL_CONTAINER(t, f, p) ((t *)(((char *)(p)) - offsetof(t, f)))

struct Dll {
  struct Dll *next;
  struct Dll *prev;
};

forceinline void dll_init(struct Dll *e) {
  e->next = e;
  e->prev = e;
}

forceinline int dll_is_alone(struct Dll *e) {
  return e->next == e && e->prev == e;
}

forceinline int dll_is_empty(struct Dll *list) {
  return !list;
}

forceinline struct Dll *dll_last(struct Dll *list) {
  return list;
}

forceinline struct Dll *dll_first(struct Dll *list) {
  struct Dll *first = 0;
  if (list)
    first = list->next;
  return first;
}

forceinline struct Dll *dll_next(struct Dll *list, struct Dll *e) {
  struct Dll *next = 0;
  if (e != list)
    next = e->next;
  return next;
}

forceinline struct Dll *dll_prev(struct Dll *list, struct Dll *e) {
  struct Dll *prev = 0;
  if (e != list->next)
    prev = e->prev;
  return prev;
}

void dll_remove(struct Dll **, struct Dll *) paramsnonnull() libcesque;
void dll_make_last(struct Dll **, struct Dll *) paramsnonnull((1)) libcesque;
void dll_make_first(struct Dll **, struct Dll *) paramsnonnull((1)) libcesque;
void dll_splice_after(struct Dll *, struct Dll *) paramsnonnull((1)) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_DLL_H_ */
#endif /* _COSMO_SOURCE */
