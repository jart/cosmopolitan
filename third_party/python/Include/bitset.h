#ifndef Py_BITSET_H
#define Py_BITSET_H
#include "third_party/python/Include/pgenheaders.h"
COSMOPOLITAN_C_START_

#define BYTE		char

typedef BYTE *bitset;

bitset newbitset(int nbits);
void delbitset(bitset bs);
#define testbit(ss, ibit) (((ss)[BIT2BYTE(ibit)] & BIT2MASK(ibit)) != 0)
int addbit(bitset bs, int ibit); /* Returns 0 if already set */
int samebitset(bitset bs1, bitset bs2, int nbits);
void mergebitset(bitset bs1, bitset bs2, int nbits);

#define BITSPERBYTE	(8*sizeof(BYTE))
#define NBYTES(nbits)	(((nbits) + BITSPERBYTE - 1) / BITSPERBYTE)

#define BIT2BYTE(ibit)	((ibit) / BITSPERBYTE)
#define BIT2SHIFT(ibit)	((ibit) % BITSPERBYTE)
#define BIT2MASK(ibit)	(1 << BIT2SHIFT(ibit))
#define BYTE2BIT(ibyte)	((ibyte) * BITSPERBYTE)

COSMOPOLITAN_C_END_
#endif /* !Py_BITSET_H */
