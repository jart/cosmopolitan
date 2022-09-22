#ifndef COSMOPOLITAN_LIBC_BITS_BIGWORD_H_
#define COSMOPOLITAN_LIBC_BITS_BIGWORD_H_

#ifndef BIGWORD
#if __AVX512F__ + 0
#define BIGWORD 64
#elif __AVX2__ + 0
#define BIGWORD 32
#elif __SSE2__ + 0
#define BIGWORD 16
#else
#define BIGWORD __BIGGEST_ALIGNMENT__
#endif
#endif /*BIGWORD*/

#endif /* COSMOPOLITAN_LIBC_BITS_BIGWORD_H_ */
