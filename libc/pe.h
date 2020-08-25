#ifndef COSMOPOLITAN_LIBC_PE_H_
#define COSMOPOLITAN_LIBC_PE_H_
#ifndef __STRICT_ANSI__
#include "libc/dce.h"
#include "libc/nt/pedef.h"
#include "libc/nt/struct/imagedosheader.h"
#include "libc/runtime/runtime.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

forceinline void *pecheckaddress(struct NtImageDosHeader *mz, size_t mzsize,
                                 void *addr, uint32_t addrsize) {
#if !(TRUSTWORTHY + PE_TRUSTWORTHY + 0)
  if ((intptr_t)addr < (intptr_t)mz ||
      (intptr_t)addr + addrsize > (intptr_t)mz + mzsize) {
    abort();
  }
#endif
  return addr;
}

forceinline void *pecomputerva(struct NtImageDosHeader *mz, size_t mzsize,
                               uint32_t reladdr, uint32_t addrsize) {
  return pecheckaddress(mz, mzsize, (void *)((intptr_t)mz + reladdr), addrsize);
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* !ANSI */
#endif /* COSMOPOLITAN_LIBC_PE_H_ */
