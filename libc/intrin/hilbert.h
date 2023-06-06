#ifndef COSMOPOLITAN_LIBC_BITS_HILBERT_H_
#define COSMOPOLITAN_LIBC_BITS_HILBERT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
#ifdef COSMO
#define hilbert   __hilbert
#define unhilbert __unhilbert

long hilbert(long, long, long) pureconst;
axdx_t unhilbert(long, long) pureconst;

#endif /* COSMO */
COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_HILBERT_H_ */
