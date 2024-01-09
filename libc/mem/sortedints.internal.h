#ifndef COSMOPOLITAN_LIBC_MEM_SORTEDINTS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_MEM_SORTEDINTS_INTERNAL_H_
COSMOPOLITAN_C_START_

struct SortedInts {
  int n;
  int c;
  int *p;
};

bool ContainsInt(const struct SortedInts *, int) libcesque;
bool InsertInt(struct SortedInts *, int, bool) libcesque;
int CountInt(const struct SortedInts *, int) libcesque;
int LeftmostInt(const struct SortedInts *, int) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_MEM_SORTEDINTS_INTERNAL_H_ */
