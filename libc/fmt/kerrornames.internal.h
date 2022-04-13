#ifndef COSMOPOLITAN_LIBC_FMT_KERRORNAMES_INTERNAL_H_
#define COSMOPOLITAN_LIBC_FMT_KERRORNAMES_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct ErrorName {
  int x, s;
};

extern const struct ErrorName kStrSignal[];
extern const struct ErrorName kErrorNames[];
extern const struct ErrorName kErrorNamesLong[];

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_FMT_KERRORNAMES_INTERNAL_H_ */
