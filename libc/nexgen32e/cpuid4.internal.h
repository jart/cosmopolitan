#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_CPUID4_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_CPUID4_H_
#include "libc/nexgen32e/kcpuids.h"

#define CPUID4_KEY                        (eax & 0xff)
#define CPUID4_CACHE_TYPE                 (((eax & 0x0000001fu) >> 000) + 0)
#define CPUID4_CACHE_LEVEL                (((eax & 0x000000e0u) >> 005) + 0)
#define CPUID4_IS_FULLY_ASSOCIATIVE       (!!(eax & (1u << 9)))
#define CPUID4_IS_SELF_INITIALIZING_LEVEL (!!(eax & (1u << 8)))
#define CPUID4_MAX_THREADS_SHARING_CACHE  (((eax & 0x03ffc000u) >> 016) + 1)
#define CPUID4_MAX_CORES_IN_PHYSICAL_CPU  (((eax & 0xfc000000u) >> 032) + 1)
#define CPUID4_SYSTEM_COHERENCY_LINE_SIZE (((Ebx & 0x00000fffu) >> 000) + 1)
#define CPUID4_PHYSICAL_LINE_PARTITIONS   (((Ebx & 0x003ff000u) >> 014) + 1)
#define CPUID4_WAYS_OF_ASSOCIATIVITY      (((Ebx & 0xffc00000u) >> 026) + 1)
#define CPUID4_NUMBER_OF_SETS             (Ecx + 1u)
#define CPUID4_WBINVD_INVD_BEHAVIOR       (!!(Edx & (1u << 0)))
#define CPUID4_INCLUSIVE_OF_LOWER_LEVELS  (!!(Edx & (1u << 1)))
#define CPUID4_COMPLEX_INDEXING           (!!(Edx & (1u << 2)))
#define CPUID4_CACHE_SIZE_IN_BYTES                                  \
  (CPUID4_WAYS_OF_ASSOCIATIVITY * CPUID4_PHYSICAL_LINE_PARTITIONS * \
   CPUID4_SYSTEM_COHERENCY_LINE_SIZE * CPUID4_NUMBER_OF_SETS)

COSMOPOLITAN_C_START_

#define CPUID4_ITERATE(I, FORM)                           \
  do {                                                    \
    uint32_t eax, Ebx, Ecx, Edx;                          \
    if (KCPUIDS(0H, EAX) >= 4) {                          \
      for (I = 0;; ++I) {                                 \
        asm("push\t%%rbx\n\t"                             \
            "cpuid\n\t"                                   \
            "mov\t%%ebx,%1\n\t"                           \
            "pop\t%%rbx"                                  \
            : "=a"(eax), "=rm"(Ebx), "=c"(Ecx), "=d"(Edx) \
            : "0"(4), "2"(I));                            \
        (void)Ebx;                                        \
        (void)Ecx;                                        \
        (void)Edx;                                        \
        if (CPUID4_CACHE_TYPE) {                          \
          FORM;                                           \
        } else {                                          \
          break;                                          \
        }                                                 \
      }                                                   \
    }                                                     \
  } while (0)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_CPUID4_H_ */
