#ifndef COSMOPOLITAN_LIBC_TYPENAME_H_
#define COSMOPOLITAN_LIBC_TYPENAME_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * Returns string identifier for scalar type of expression.
 *
 * @see TYPE_FMT()
 */
#define TYPE_NAME(X)                         \
  _Generic((X), long double                  \
           : "long double", double           \
           : "double", float                 \
           : "float", char                   \
           : "char", signed char             \
           : "signed char", unsigned char    \
           : "unsigned char", short          \
           : "short", unsigned short         \
           : "unsigned short", int           \
           : "int", unsigned                 \
           : "unsigned", long                \
           : "long", unsigned long           \
           : "unsigned long long", long long \
           : "long long", unsigned long long \
           : "unsigned long long")

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TYPENAME_H_ */
