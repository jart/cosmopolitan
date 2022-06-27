#ifndef COSMOPOLITAN_THIRD_PARTY_ZLIB_PUFF_H_
#define COSMOPOLITAN_THIRD_PARTY_ZLIB_PUFF_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int puff(unsigned char *dest, unsigned long *destlen,
         const unsigned char *source, unsigned long *sourcelen);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_ZLIB_PUFF_H_ */
