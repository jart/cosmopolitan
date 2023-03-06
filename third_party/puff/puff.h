#ifndef COSMOPOLITAN_THIRD_PARTY_ZLIB_PUFF_H_
#define COSMOPOLITAN_THIRD_PARTY_ZLIB_PUFF_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#ifndef NIL
#define NIL ((unsigned char *)0) /* for no output option */
#endif

int _puff(unsigned char *dest, unsigned long *destlen,
          const unsigned char *source, unsigned long *sourcelen);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_ZLIB_PUFF_H_ */
