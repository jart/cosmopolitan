#ifndef COSMOPOLITAN_LIBC_BITS_SEGMENTATION_H_
#define COSMOPOLITAN_LIBC_BITS_SEGMENTATION_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * Reads scalar from memory, offset by segment.
 *
 * @return *(MEM) relative to segment
 * @see arch_prctl()
 * @see pushpop()
 */
#define fs(MEM) __peek("fs", MEM)
#define gs(MEM) __peek("gs", MEM)

#define __peek(SEGMENT, ADDRESS)                                  \
  ({                                                              \
    typeof(*(ADDRESS)) Pk;                                        \
    asm("mov\t%%" SEGMENT ":%1,%0" : "=r"(Pk) : "m"(*(ADDRESS))); \
    Pk;                                                           \
  })

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_SEGMENTATION_H_ */
