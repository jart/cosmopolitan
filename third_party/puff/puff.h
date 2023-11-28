#ifndef COSMOPOLITAN_THIRD_PARTY_ZLIB_PUFF_H_
#define COSMOPOLITAN_THIRD_PARTY_ZLIB_PUFF_H_
COSMOPOLITAN_C_START_

#ifndef NIL
#define NIL ((unsigned char *)0) /* for no output option */
#endif

int _puff(unsigned char *dest, unsigned long *destlen,
          const unsigned char *source, unsigned long *sourcelen);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_ZLIB_PUFF_H_ */
