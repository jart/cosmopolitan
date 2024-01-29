#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_X86FEATURE_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_X86FEATURE_H_
#ifdef __x86_64__
#include "libc/nexgen32e/kcpuids.h"
#include "libc/nexgen32e/x86compiler.h"

/* Returns true if x86 FEATURE is present.
   This performs a runtime check in normal portable builds.
   It's usually dead code eliminated for -march=native builds. */
#define X86_HAVE(FEATURE) _X86_HAVE(X86_##FEATURE)

/* Returns true if x86 FEATURE is present.
   This will *always* perform a runtime check.
   It's useful for checking compile-time cpu features are present. */
#define X86_CHECK(FEATURE) _X86_CHECK(X86_##FEATURE)

/* Returns true if x86 FEATURE is mandatory at build time.
   This will never perform runtime checks; it's always constant. */
#define X86_NEED(FEATURE) _X86_NEED(X86_##FEATURE)

/* clang-format off */
/*      --- FEATURE              LEAF       REG BIT  COMPILE-TIME-DEFINE */
#define X86_ACC                  1H,        EDX, 29, 0
#define X86_ACPI                 1H,        EDX, 22, 0
#define X86_ADX                  7H,        EBX, 19, _X86_CC_ADX              /* broadwell c. 2014 */
#define X86_AES                  1H,        ECX, 25, _X86_CC_AES              /* westmere c. 2010 */
#define X86_APIC                 1H,        EDX,  9, 0
#define X86_ARCH_CAPABILITIES    7H,        EDX, 29, 0
#define X86_AVX                  1H,        ECX, 28, _X86_CC_AVX              /* sandybridge c. 2012 */
#define X86_AVX2                 7H,        EBX,  5, _X86_CC_AVX2             /* haswell c. 2013 */
#define X86_AVXVNNI              7H_1H,     EAX,  4, _X86_CC_AVXVNNI
#define X86_AVXVNNIINT8          7H_1H,     EDX,  4, _X86_CC_AVXVNNIINT8
#define X86_AVXVNNIINT16         7H_1H,     EDX, 10, _X86_CC_AVXVNNIINT16
#define X86_AVX512BW             7H,        EBX, 30, 0
#define X86_AVX512CD             7H,        EBX, 28, 0
#define X86_AVX512DQ             7H,        EBX, 17, 0
#define X86_AVX512ER             7H,        EBX, 27, 0
#define X86_AVX512F              7H,        EBX, 16, _X86_CC_AVX512F
#define X86_AVX512IFMA           7H,        EBX, 21, 0
#define X86_AVX512PF             7H,        EBX, 26, 0
#define X86_AVX512VBMI           7H,        ECX,  1, _X86_CC_AVX512VBMI
#define X86_AVX512VL             7H,        EBX, 31, 0
#define X86_AVX512_4FMAPS        7H,        EDX,  3, 0
#define X86_AVX512_4VNNIW        7H,        EDX,  2, _X86_CC_AVX5124VNNIW
#define X86_AVX512_BF16          7H,        EAX,  5, _X86_CC_AVX512BF16
#define X86_AVX512_BITALG        7H,        ECX, 12, 0
#define X86_AVX512_VBMI2         7H,        ECX,  6, 0
#define X86_AVX512_VNNI          7H,        ECX, 11, _X86_CC_AVX512VNNI
#define X86_AVX512_VP2INTERSECT  7H,        EDX,  8, 0
#define X86_AVX512_VPOPCNTDQ     7H,        ECX, 14, 0
#define X86_BMI                  7H,        EBX,  3, _X86_CC_BMI              /* haswell c. 2013 */
#define X86_BMI2                 7H,        EBX,  8, _X86_CC_BMI2             /* haswell c. 2013 */
#define X86_CID                  1H,        ECX, 10, 0
#define X86_CLDEMOTE             7H,        ECX, 25, 0
#define X86_CLFLUSH              1H,        EDX, 19, _X86_CC_SSE2
#define X86_CLFLUSHOPT           7H,        EBX, 23, _X86_CC_CLFLUSHOPT       /* skylake/zen  */
#define X86_CLWB                 7H,        EBX, 24, 0                        /* skylake/zen2 */
#define X86_CMOV                 1H,        EDX, 15, 0
#define X86_CQM                  7H,        EBX, 12, 0
#define X86_CX16                 1H,        ECX, 13, 0
#define X86_CX8                  1H,        EDX,  8, 0
#define X86_DCA                  1H,        ECX, 18, 0
#define X86_DE                   1H,        EDX,  2, 0
#define X86_DS                   1H,        EDX, 21, 0
#define X86_DSCPL                1H,        ECX,  4, 0
#define X86_DTES64               1H,        ECX,  2, 0
#define X86_ERMS                 7H,        EBX,  9, 0                        /* broaadwell c. 2014 */
#define X86_EST                  1H,        ECX,  7, 0
#define X86_F16C                 1H,        ECX, 29, _X86_CC_F16C
#define X86_FDP_EXCPTN_ONLY      7H,        EBX,  6, 0
#define X86_FLUSH_L1D            7H,        EDX, 28, 0
#define X86_FMA                  1H,        ECX, 12, _X86_CC_FMA              /* haswell c. 2013 */
#define X86_FPU                  1H,        EDX,  0, 0
#define X86_FSGSBASE             7H,        EBX,  0, 0
#define X86_FXSR                 1H,        EDX, 24, 0
#define X86_GBPAGES              80000001H, EDX, 26, 0
#define X86_GFNI                 7H,        ECX,  8, 0
#define X86_HLE                  7H,        EBX,  4, 0
#define X86_HT                   1H,        EDX, 28, 0
#define X86_HYPERVISOR           1H,        ECX, 31, 0
#define X86_IA64                 1H,        EDX, 30, 0
#define X86_INTEL_PT             7H,        EBX, 25, 0
#define X86_INTEL_STIBP          7H,        EDX, 27, 0
#define X86_INVPCID              1H,        EBX, 10, 0
#define X86_INVTSC               80000007H, EDX,  8, _X86_CC_POPCNT           /* i.e. not a K8 */
#define X86_LA57                 7H,        ECX, 16, 0
#define X86_LAHF_LM              80000001H, ECX,  0, 0
#define X86_LM                   80000001H, EDX, 29, 0
#define X86_MCA                  1H,        EDX, 14, 0
#define X86_MCE                  1H,        EDX,  7, 0
#define X86_MD_CLEAR             7H,        EDX, 10, 0
#define X86_MMX                  1H,        EDX, 23, 0
#define X86_MOVBE                1H,        ECX, 22, 0
#define X86_MOVDIR64B            7H,        ECX, 28, 0
#define X86_MOVDIRI              7H,        ECX, 27, 0
#define X86_MP                   80000001H, EDX, 19, 0
#define X86_MPX                  7H,        EBX, 14, 0
#define X86_MSR                  1H,        EDX,  5, 0
#define X86_MTRR                 1H,        EDX, 12, 0
#define X86_MWAIT                1H,        ECX,  3, 0
#define X86_NX                   80000001H, EDX, 20, 0
#define X86_OSPKE                7H,        ECX,  4, 0
#define X86_OSXSAVE              1H,        ECX, 27, 0
#define X86_PAE                  1H,        EDX,  6, 0
#define X86_PAT                  1H,        EDX, 16, 0
#define X86_PBE                  1H,        EDX, 31, 0
#define X86_PCID                 1H,        ECX, 17, 0
#define X86_PCLMUL               1H,        ECX,  1, _X86_CC_PCLMUL           /* westmere c. 2010 */
#define X86_PCONFIG              7H,        EDX, 18, 0
#define X86_PDCM                 1H,        ECX, 15, 0
#define X86_PGE                  1H,        EDX, 13, 0
#define X86_PKU                  7H,        ECX,  3, 0
#define X86_PN                   1H,        EDX, 18, 0
#define X86_POPCNT               1H,        ECX, 23, _X86_CC_POPCNT           /* nehalem c. 2008 */
#define X86_PSE                  1H,        EDX,  3, 0
#define X86_PSE36                1H,        EDX, 17, 0
#define X86_RDPID                7H,        ECX, 22, _X86_CC_RDPID            /* cannonlake c. 2018 */
#define X86_RDRND                1H,        ECX, 30, _X86_CC_RDRND            /* ivybridge c. 2012 */
#define X86_RDSEED               7H,        EBX, 18, _X86_CC_RDSEED           /* broadwell c. 2014 */
#define X86_RDTSCP               80000001H, EDX, 27, 0
#define X86_RDT_A                7H,        EBX, 15, 0
#define X86_RTM                  7H,        EBX, 11, 0
#define X86_SDBG                 1H,        ECX, 11, 0
#define X86_SELFSNOOP            1H,        EDX, 27, 0
#define X86_SEP                  1H,        EDX, 11, 0
#define X86_SHA                  7H,        EBX, 29, _X86_CC_SHA              /* goldmont (2016) */
#define X86_SMAP                 7H,        EBX, 20, 0
#define X86_SMEP                 7H,        EBX,  7, 0
#define X86_SMX                  1H,        ECX,  6, 0
#define X86_SPEC_CTRL            7H,        EDX, 26, 0
#define X86_SPEC_CTRL_SSBD       7H,        EDX, 31, 0
#define X86_SSE                  1H,        EDX, 25, _X86_CC_SSE              /* pentium c. 1999 */
#define X86_SSE2                 1H,        EDX, 26, _X86_CC_SSE2             /* pentium c. 2001 */
#define X86_SSE3                 1H,        ECX,  0, _X86_CC_SSE3             /* k8 c. 2005 */
#define X86_SSE4_1               1H,        ECX, 19, _X86_CC_SSE4_1           /* core c. 2006 */
#define X86_SSE4_2               1H,        ECX, 20, _X86_CC_SSE4_2           /* nehalem c. 2008 */
#define X86_SSSE3                1H,        ECX,  9, _X86_CC_SSSE3            /* westmere c. 2010 */
#define X86_SYSCALL              80000001H, EDX, 11, 0
#define X86_TM2                  1H,        ECX,  8, 0
#define X86_TME                  7H,        ECX, 13, 0
#define X86_TSC                  1H,        EDX,  4, 0
#define X86_TSC_ADJUST           7H,        EBX,  1, 0
#define X86_TSC_DEADLINE_TIMER   1H,        ECX, 24, 0
#define X86_TSX_FORCE_ABORT      7H,        EDX, 13, 0
#define X86_UMIP                 7H,        ECX,  2, 0
#define X86_VAES                 7H,        ECX,  9, 0
#define X86_VME                  1H,        EDX,  1, 0
#define X86_VMX                  1H,        ECX,  5, 0
#define X86_VPCLMULQDQ           7H,        ECX, 10, 0
#define X86_WAITPKG              7H,        ECX,  5, 0
#define X86_X2APIC               1H,        ECX, 21, 0
#define X86_XSAVE                1H,        ECX, 26, _X86_CC_XSAVE            /* sandybridge c. 2012 */
#define X86_XTPR                 1H,        ECX, 14, 0
#define X86_ZERO_FCS_FDS         7H,        EBX, 13, 0
#define X86_JIT                  80000001H, ECX, 31, 0                        /* IsGenuineBlink() */
/* clang-format on */

/* AMD specific features */
#define X86_ABM            80000001H, ECX, 5, _X86_CC_ABM
#define X86_3DNOW          80000001H, EDX, 31, 0
#define X86_3DNOWEXT       80000001H, EDX, 30, 0
#define X86_3DNOWPREFETCH  80000001H, ECX, 8, 0
#define X86_BPEXT          80000001H, ECX, 26, 0
#define X86_CMP_LEGACY     80000001H, ECX, 1, 0
#define X86_CR8_LEGACY     80000001H, ECX, 4, 0
#define X86_EXTAPIC        80000001H, ECX, 3, 0
#define X86_FMA4           80000001H, ECX, 16, 0
#define X86_FXSR_OPT       80000001H, EDX, 25, 0
#define X86_IBS            80000001H, ECX, 10, 0
#define X86_LWP            80000001H, ECX, 15, 0
#define X86_MISALIGNSSE    80000001H, ECX, 7, 0
#define X86_MMXEXT         80000001H, EDX, 22, 0
#define X86_MWAITX         80000001H, ECX, 29, 0
#define X86_NODEID_MSR     80000001H, ECX, 19, 0
#define X86_OSVW           80000001H, ECX, 9, 0
#define X86_OVERFLOW_RECOV 80000007H, EBX, 0, 0
#define X86_PERFCTR_CORE   80000001H, ECX, 23, 0
#define X86_PERFCTR_LLC    80000001H, ECX, 28, 0
#define X86_PERFCTR_NB     80000001H, ECX, 24, 0
#define X86_PTSC           80000001H, ECX, 27, 0
#define X86_SKINIT         80000001H, ECX, 12, 0
#define X86_SMCA           80000007H, EBX, 3, 0
#define X86_SSE4A          80000001H, ECX, 6, 0
#define X86_SUCCOR         80000007H, EBX, 1, 0
#define X86_SVM            80000001H, ECX, 2, 0
#define X86_TBM            80000001H, ECX, 21, 0
#define X86_TCE            80000001H, ECX, 17, 0
#define X86_TOPOEXT        80000001H, ECX, 22, 0
#define X86_WDT            80000001H, ECX, 13, 0
#define X86_XOP            80000001H, ECX, 11, 0

/* Defined but not loaded by kCpuids.S */
#define X86_ARAT            6H, EAX, 2, 0
#define X86_AVIC            8000000AH, EDX, 13, 0
#define X86_CLZERO          80000008H, EBX, 0, 0
#define X86_DECODEASSISTS   8000000AH, EDX, 7, 0
#define X86_DTHERM          6H, EAX, 0, 0
#define X86_FLUSHBYASID     8000000AH, EDX, 6, 0
#define X86_HWP             6H, EAX, 7, 0
#define X86_HWP_ACT_WINDOW  6H, EAX, 9, 0
#define X86_HWP_EPP         6H, EAX, 10, 0
#define X86_HWP_NOTIFY      6H, EAX, 8, 0
#define X86_HWP_PKG_REQ     6H, EAX, 11, 0
#define X86_IBPB            80000008H, EBX, 12, 0
#define X86_IBRS            80000008H, EBX, 14, 0
#define X86_IDA             6H, EAX, 1, 0
#define X86_IRPERF          80000008H, EBX, 1, 0
#define X86_LBRV            8000000AH, EDX, 1, 0
#define X86_NPT             8000000AH, EDX, 0, 0
#define X86_NRIPS           8000000AH, EDX, 3, 0
#define X86_PAUSEFILTER     8000000AH, EDX, 10, 0
#define X86_PFTHRESHOLD     8000000AH, EDX, 12, 0
#define X86_PLN             6H, EAX, 4, 0
#define X86_PTS             6H, EAX, 6, 0
#define X86_SSBD            80000008H, EBX, 24, 0
#define X86_SSB_NO          80000008H, EBX, 26, 0
#define X86_STIBP           80000008H, EBX, 15, 0
#define X86_STIBP_ALWAYS_ON 80000008H, EBX, 17, 0
#define X86_SVML            8000000AH, EDX, 2, 0
#define X86_TSCRATEMSR      8000000AH, EDX, 4, 0
#define X86_VGIF            8000000AH, EDX, 16, 0
#define X86_VIRT_SSBD       80000008H, EBX, 25, 0
#define X86_VMCBCLEAN       8000000AH, EDX, 5, 0
#define X86_V_VMSAVE_VMLOAD 8000000AH, EDX, 15, 0
#define X86_WBNOINVD        80000008H, EBX, 9, 0
#define X86_XGETBV1         DH, EAX, 2, 0
#define X86_XSAVEC          DH, EAX, 1, 0
#define X86_XSAVEERPTR      80000008H, EBX, 2, 0
#define X86_XSAVEOPT        DH, EAX, 0, 0
#define X86_XSAVES          DH, EAX, 3, 0

#define X86_WORD(FEATURE) _X86_WORD(X86_##FEATURE)
#define X86_LEAF(FEATURE) _X86_LEAF(X86_##FEATURE)
#define X86_REG(FEATURE)  _X86_REG(X86_##FEATURE)
#define X86_BIT(FEATURE)  _X86_BIT(X86_##FEATURE)

#define _X86_CHECK(FEATURE) __X86_CHECK(FEATURE)
#define _X86_HAVE(FEATURE)  __X86_HAVE(FEATURE)
#define _X86_NEED(FEATURE)  __X86_NEED(FEATURE)
#define _X86_WORD(FEATURE)  __X86_WORD(FEATURE)
#define _X86_LEAF(FEATURE)  __X86_LEAF(FEATURE)
#define _X86_REG(FEATURE)   __X86_REG(FEATURE)
#define _X86_BIT(FEATURE)   __X86_BIT(FEATURE)

#define __X86_CHECK(LEAF, REG, BIT, MANDATORY) \
  ___X86_CHECK(LEAF, REG, BIT, MANDATORY)
#define __X86_HAVE(LEAF, REG, BIT, MANDATORY) \
  ___X86_HAVE(LEAF, REG, BIT, MANDATORY)
#define __X86_NEED(LEAF, REG, BIT, MANDATORY) MANDATORY
#define __X86_WORD(LEAF, REG, BIT, MANDATORY) KCPUIDS(LEAF, REG)
#define __X86_LEAF(LEAF, REG, BIT, MANDATORY) LEAF
#define __X86_REG(LEAF, REG, BIT, MANDATORY)  REG
#define __X86_BIT(LEAF, REG, BIT, MANDATORY)  BIT

#define ___X86_CHECK(LEAF, REG, BIT, MANDATORY) \
  !!(KCPUIDS(LEAF, REG) & (1u << BIT))

#ifndef __ASSEMBLER__
#define ___X86_HAVE(LEAF, REG, BIT, MANDATORY) \
  !!(MANDATORY || KCPUIDS(LEAF, REG) & (1u << BIT))
#else
#define ___X86_HAVE(LEAF, REG, BIT, MANDATORY) \
  $1 << (BIT % 8), BIT / 8 + KCPUIDS(LEAF, REG)
#endif

#else
#define X86_HAVE(FEATURE)  0
#define X86_NEED(FEATURE)  0
#define X86_CHECK(FEATURE) 0
#endif /* __x86_64__ */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_X86FEATURE_H_ */
