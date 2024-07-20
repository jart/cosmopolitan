#ifndef COSMOPOLITAN_LIBC_BITS_XCHG_H_
#define COSMOPOLITAN_LIBC_BITS_XCHG_H_
#include "libc/str/str.h"

/**
 * Exchanges *MEMORY into *LOCALVAR.
 *
 * @return *MEMORY
 * todo(jart): what's the point of this?
 */
#define xchg(MEMORY, LOCALVAR)              \
  ({                                        \
    autotype(MEMORY) Memory = (MEMORY);     \
    typeof(Memory) LocalVar = (LOCALVAR);   \
    typeof(*Memory) Temp;                   \
    memcpy(&Temp, Memory, sizeof(Temp));    \
    memcpy(Memory, LocalVar, sizeof(Temp)); \
    memcpy(LocalVar, &Temp, sizeof(Temp));  \
    Temp;                                   \
  })

#endif /* COSMOPOLITAN_LIBC_BITS_XCHG_H_ */
