/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Include/bitset.h"
#include "third_party/python/Include/objimpl.h"

bitset
newbitset(int nbits)
{
    int nbytes = NBYTES(nbits);
    bitset ss = (char *)PyObject_MALLOC(sizeof(BYTE) *  nbytes);

    if (ss == NULL)
        Py_FatalError("no mem for bitset");

    ss += nbytes;
    while (--nbytes >= 0)
        *--ss = 0;
    return ss;
}

void
delbitset(bitset ss)
{
    PyObject_FREE(ss);
}

int
addbit(bitset ss, int ibit)
{
    int ibyte = BIT2BYTE(ibit);
    BYTE mask = BIT2MASK(ibit);

    if (ss[ibyte] & mask)
        return 0; /* Bit already set */
    ss[ibyte] |= mask;
    return 1;
}

#if 0 /* Now a macro */
int
testbit(bitset ss, int ibit)
{
    return (ss[BIT2BYTE(ibit)] & BIT2MASK(ibit)) != 0;
}
#endif

int
samebitset(bitset ss1, bitset ss2, int nbits)
{
    int i;

    for (i = NBYTES(nbits); --i >= 0; )
        if (*ss1++ != *ss2++)
            return 0;
    return 1;
}

void
mergebitset(bitset ss1, bitset ss2, int nbits)
{
    int i;

    for (i = NBYTES(nbits); --i >= 0; )
        *ss1++ |= *ss2++;
}
