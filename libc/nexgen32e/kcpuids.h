#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_KCPUIDS_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_KCPUIDS_H_

#define KCPUIDS_0H        0
#define KCPUIDS_1H        1
#define KCPUIDS_2H        2
#define KCPUIDS_7H        3
#define KCPUIDS_80000001H 4
#define KCPUIDS_80000007H 5
#define KCPUIDS_16H       6
#define KCPUIDS_7H_1H     7
#define KCPUIDS_LEN       8
#define KCPUIDS_6H        -1 /* TBD: Thermal and Power Management */
#define KCPUIDS_DH        -1 /* TBD: Extended state features */
#define KCPUIDS_80000008H -1 /* TBD: AMD Miscellaneous */
#define KCPUIDS_8000000AH -1 /* TBD: AMD SVM */

#define KCPUIDS_EAX 0
#define KCPUIDS_EBX 1
#define KCPUIDS_ECX 2
#define KCPUIDS_EDX 3

#define KCPUIDS(LEAF, REG) _KCPUIDS(LEAF, REG)
#ifdef __ASSEMBLER__
#define _KCPUIDS(LEAF, REG) KCPUIDS_##LEAF * 16 + KCPUIDS_##REG * 4
#else
#define _KCPUIDS(LEAF, REG) kCpuids[KCPUIDS_##LEAF][KCPUIDS_##REG]
#endif

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const unsigned kCpuids[KCPUIDS_LEN][4];

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_KCPUIDS_H_ */
