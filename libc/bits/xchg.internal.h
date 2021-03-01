#ifndef COSMOPOLITAN_LIBC_BITS_XCHG_H_
#define COSMOPOLITAN_LIBC_BITS_XCHG_H_
#include "libc/str/str.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * Exchanges *MEMORY into *LOCALVAR.
 *
 * @return *MEMORY
 * @see lockcmpxchg()
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

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_XCHG_H_ */
