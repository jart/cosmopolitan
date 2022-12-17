#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_VENDOR_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_VENDOR_H_
#include "libc/nexgen32e/kcpuids.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

#define IsAuthenticAMD()                                        \
  (kCpuids[KCPUIDS_0H][KCPUIDS_EBX] == 0x68747541 /* Auth */ && \
   kCpuids[KCPUIDS_0H][KCPUIDS_EDX] == 0x69746e65 /* enti */ && \
   kCpuids[KCPUIDS_0H][KCPUIDS_ECX] == 0x444d4163 /* cAMD */)

#define IsGenuineIntel()                                        \
  (kCpuids[KCPUIDS_0H][KCPUIDS_EBX] == 0x756e6547 /* Genu */ && \
   kCpuids[KCPUIDS_0H][KCPUIDS_EDX] == 0x49656e69 /* ineI */ && \
   kCpuids[KCPUIDS_0H][KCPUIDS_ECX] == 0x6c65746e /* ntel */)

#define IsGenuineCosmo()                                        \
  (kCpuids[KCPUIDS_0H][KCPUIDS_EBX] == 0x756e6547 /* Genu */ && \
   kCpuids[KCPUIDS_0H][KCPUIDS_EDX] == 0x43656e69 /* ineC */ && \
   kCpuids[KCPUIDS_0H][KCPUIDS_ECX] == 0x6f6d736f /* osmo */)

#define IsGenuineBlink()                                        \
  (kCpuids[KCPUIDS_0H][KCPUIDS_EBX] == 0x756e6547 /* Genu */ && \
   kCpuids[KCPUIDS_0H][KCPUIDS_EDX] == 0x42656e69 /* ineB */ && \
   kCpuids[KCPUIDS_0H][KCPUIDS_ECX] == 0x6b6e696c /* link */)

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_VENDOR_H_ */
