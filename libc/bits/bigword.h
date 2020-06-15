#ifndef COSMOPOLITAN_LIBC_BITS_BIGWORD_H_
#define COSMOPOLITAN_LIBC_BITS_BIGWORD_H_

#if 0
/**
 * Let BIGWORD be the the number of bytes in the largest cpu register
 * available within the instruction set architecture requirements chosen
 * at compile-time.
 *
 * In plainer terms, if you tune with flags like -mavx, you're not just
 * giving the compiler permission to generate code that's incompatible
 * with older computers; you're also asking Cosmopolitan to systemically
 * change alignment, vectoring, buffering, ABIs, memory allocation, etc.
 */
#endif
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
