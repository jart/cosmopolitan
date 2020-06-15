#ifndef COSMOPOLITAN_LIBC_FMT_TYPEFMT_H_
#define COSMOPOLITAN_LIBC_FMT_TYPEFMT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * Returns printf directive to format scalar type of expression.
 *
 * @see TYPE_NAME()
 */
#define TYPE_FMT(X, DXOB, UXOB)       \
  _Generic((X), long double           \
           : "Lf", double             \
           : "f", float               \
           : "f", char                \
           : "c", signed char         \
           : "hh" DXOB, unsigned char \
           : "hh" UXOB, short         \
           : "h" DXOB, unsigned short \
           : "h" UXOB, int            \
           : DXOB, unsigned           \
           : UXOB, long               \
           : "l" DXOB, unsigned long  \
           : "l" UXOB)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_FMT_TYPEFMT_H_ */
