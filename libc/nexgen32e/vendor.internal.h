#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_VENDOR_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_VENDOR_H_
#include "libc/nexgen32e/kcpuids.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/*
 * Known 80x86 Vendors (CPUID.0 EBX+EDX+ECX)
 *
 *   ╤        ╤
 *   GenuineIntel
 *   AuthenticAMD
 *   GenuineCosmo
 *   NexGenDriven
 *   AMDisbetter!
 *   CentaurHauls
 *   TransmetaCPU
 *   GenuineTMx86
 *   CyrixInstead
 *   UMC UMC UMC
 *   SiS SiS SiS
 *   Geode by NSC
 *   RiseRiseRise
 *   Vortex86 SoC
 *   VIA VIA VIA
 *   VMwareVMware
 *   XenVMMXenVMM
 *   Microsoft Hv
 *   └────┐   │
 *        G ⊕ t = 0x33 Intel
 *        A ⊕ A = 0x00 AMD
 *        G ⊕ s = 0x34 Cosmopolitan
 *        N ⊕ v = 0x38 NexGen (Modern x86)
 *        A ⊕ e = 0x24 AMD (Rank & File)
 *        C ⊕ u = 0x36 Via (DBA Centaur)
 *        T ⊕ C = 0x17 Transmeta (Historical)
 *        G ⊕ x = 0x3f Transmeta (Historical)
 *        C ⊕ e = 0x26 Cyrix (Historical)
 *        U ⊕ M = 0x18 UMC (Taiwan)
 *        S ⊕ i = 0x3a SiS (Historical)
 *        G ⊕ N = 0x09 National Semiconductors (OLPC)
 *        R ⊕ i = 0x3b Rise Technology (Historical)
 *        V ⊕ S = 0x05 DM&P (Vortex86)
 *        V ⊕ I = 0x1f Via
 *        V ⊕ a = 0x37 VMware
 *        X ⊕ V = 0x0e Xen
 *        M ⊕   = 0x6d Microsoft (Win10 Hyper-V)
 *        │   │
 *        │   │ perfect
 *        │   │ 𝑕(𝑠)=𝑠₀⊕𝑠₉
 *        ╧   ╧
 *
 * @note Parallels Desktop CPU brand string is " lrpepyh vr " since even
 *       folks designing microprocessor emulators apparently struggle w/
 *       endianness lool.
 */
#define IsAuthenticAMD() (_KCPUIDS_VENDOR() == 0x00)
#define IsGenuineIntel() (_KCPUIDS_VENDOR() == 0x33)
#define IsGenuineCosmo() (_KCPUIDS_VENDOR() == 0x34)

#define _KCPUIDS_VENDOR()                               \
  (((kCpuids[KCPUIDS_0H][KCPUIDS_EBX] >> 000) & 0xff) ^ \
   ((kCpuids[KCPUIDS_0H][KCPUIDS_EDX] >> 010) & 0xff))

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_VENDOR_H_ */
