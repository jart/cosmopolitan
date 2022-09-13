#ifndef COSMOPOLITAN_LIBC_INTRIN_GETAUXVAL_INTERNAL_H_
#define COSMOPOLITAN_LIBC_INTRIN_GETAUXVAL_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct AuxiliaryValue {
  unsigned long value;
  bool isfound;
};

struct AuxiliaryValue _getauxval(unsigned long);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_GETAUXVAL_INTERNAL_H_ */
