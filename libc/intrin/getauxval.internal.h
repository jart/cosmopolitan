#ifndef COSMOPOLITAN_LIBC_INTRIN_GETAUXVAL_H_
#define COSMOPOLITAN_LIBC_INTRIN_GETAUXVAL_H_
COSMOPOLITAN_C_START_

struct AuxiliaryValue {
  unsigned long value;
  bool isfound;
};

struct AuxiliaryValue __getauxval(unsigned long) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_GETAUXVAL_H_ */
