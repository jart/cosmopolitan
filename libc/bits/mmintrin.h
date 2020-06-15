#ifndef COSMOPOLITAN_LIBC_BITS_MMINTRIN_H_
#define COSMOPOLITAN_LIBC_BITS_MMINTRIN_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

typedef long long __m64 _Vector_size(8);
typedef float __v2sf _Vector_size(8);
typedef int __v2si _Vector_size(8);
typedef short __v4hi _Vector_size(8);
typedef char __v8qi _Vector_size(8);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_MMINTRIN_H_ */
