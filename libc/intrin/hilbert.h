#ifndef COSMOPOLITAN_LIBC_BITS_HILBERT_H_
#define COSMOPOLITAN_LIBC_BITS_HILBERT_H_
#ifdef _COSMO_SOURCE
COSMOPOLITAN_C_START_
#define hilbert   __hilbert
#define unhilbert __unhilbert

libcesque long hilbert(long, long, long) pureconst;
libcesque axdx_t unhilbert(long, long) pureconst;

COSMOPOLITAN_C_END_
#endif /* _COSMO_SOURCE */
#endif /* COSMOPOLITAN_LIBC_BITS_HILBERT_H_ */
