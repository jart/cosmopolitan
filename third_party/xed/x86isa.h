#ifndef COSMOPOLITAN_THIRD_PARTY_XED_X86ISA_H_
#define COSMOPOLITAN_THIRD_PARTY_XED_X86ISA_H_
#include "third_party/xed/x86.h"
COSMOPOLITAN_C_START_

#define XED_CHIP_INVALID           1
#define XED_CHIP_I86               2
#define XED_CHIP_I86FP             3
#define XED_CHIP_I186              4
#define XED_CHIP_I186FP            5
#define XED_CHIP_I286REAL          6
#define XED_CHIP_I286              7
#define XED_CHIP_I2186FP           8
#define XED_CHIP_I386REAL          9
#define XED_CHIP_I386              10
#define XED_CHIP_I386FP            11
#define XED_CHIP_I486REAL          12
#define XED_CHIP_I486              13
#define XED_CHIP_PENTIUMREAL       14
#define XED_CHIP_PENTIUM           15
#define XED_CHIP_QUARK             16
#define XED_CHIP_PENTIUMMMXREAL    17
#define XED_CHIP_PENTIUMMMX        18
#define XED_CHIP_ALLREAL           19
#define XED_CHIP_PENTIUMPRO        20
#define XED_CHIP_PENTIUM2          21
#define XED_CHIP_PENTIUM3          22
#define XED_CHIP_PENTIUM4          23
#define XED_CHIP_P4PRESCOTT        24
#define XED_CHIP_P4PRESCOTT_NOLAHF 25
#define XED_CHIP_P4PRESCOTT_VTX    26
#define XED_CHIP_CORE2             27
#define XED_CHIP_PENRYN            28
#define XED_CHIP_PENRYN_E          29
#define XED_CHIP_NEHALEM           30
#define XED_CHIP_WESTMERE          31
#define XED_CHIP_BONNELL           32
#define XED_CHIP_SALTWELL          33
#define XED_CHIP_SILVERMONT        34
#define XED_CHIP_AMD               35
#define XED_CHIP_GOLDMONT          36
#define XED_CHIP_GOLDMONT_PLUS     37
#define XED_CHIP_TREMONT           38
#define XED_CHIP_SANDYBRIDGE       39
#define XED_CHIP_IVYBRIDGE         40
#define XED_CHIP_HASWELL           41
#define XED_CHIP_BROADWELL         42
#define XED_CHIP_SKYLAKE           43
#define XED_CHIP_SKYLAKE_SERVER    44
#define XED_CHIP_CASCADE_LAKE      45
#define XED_CHIP_KNL               46
#define XED_CHIP_KNM               47
#define XED_CHIP_CANNONLAKE        48
#define XED_CHIP_ICELAKE           49
#define XED_CHIP_ICELAKE_SERVER    50
#define XED_CHIP_FUTURE            51
#define XED_CHIP_ALL               52
#define XED_CHIP_LAST              53

#define XED_ISA_SET_INVALID               0
#define XED_ISA_SET_3DNOW                 1
#define XED_ISA_SET_ADOX_ADCX             2
#define XED_ISA_SET_AES                   3
#define XED_ISA_SET_AMD                   4
#define XED_ISA_SET_AVX                   5
#define XED_ISA_SET_AVX2                  6
#define XED_ISA_SET_AVX2GATHER            7
#define XED_ISA_SET_AVX512BW_128          8
#define XED_ISA_SET_AVX512BW_128N         9
#define XED_ISA_SET_AVX512BW_256          10
#define XED_ISA_SET_AVX512BW_512          11
#define XED_ISA_SET_AVX512BW_KOP          12
#define XED_ISA_SET_AVX512CD_128          13
#define XED_ISA_SET_AVX512CD_256          14
#define XED_ISA_SET_AVX512CD_512          15
#define XED_ISA_SET_AVX512DQ_128          16
#define XED_ISA_SET_AVX512DQ_128N         17
#define XED_ISA_SET_AVX512DQ_256          18
#define XED_ISA_SET_AVX512DQ_512          19
#define XED_ISA_SET_AVX512DQ_KOP          20
#define XED_ISA_SET_AVX512DQ_SCALAR       21
#define XED_ISA_SET_AVX512ER_512          22
#define XED_ISA_SET_AVX512ER_SCALAR       23
#define XED_ISA_SET_AVX512F_128           24
#define XED_ISA_SET_AVX512F_128N          25
#define XED_ISA_SET_AVX512F_256           26
#define XED_ISA_SET_AVX512F_512           27
#define XED_ISA_SET_AVX512F_KOP           28
#define XED_ISA_SET_AVX512F_SCALAR        29
#define XED_ISA_SET_AVX512PF_512          30
#define XED_ISA_SET_AVX512_4FMAPS_512     31
#define XED_ISA_SET_AVX512_4FMAPS_SCALAR  32
#define XED_ISA_SET_AVX512_4VNNIW_512     33
#define XED_ISA_SET_AVX512_BITALG_128     34
#define XED_ISA_SET_AVX512_BITALG_256     35
#define XED_ISA_SET_AVX512_BITALG_512     36
#define XED_ISA_SET_AVX512_GFNI_128       37
#define XED_ISA_SET_AVX512_GFNI_256       38
#define XED_ISA_SET_AVX512_GFNI_512       39
#define XED_ISA_SET_AVX512_IFMA_128       40
#define XED_ISA_SET_AVX512_IFMA_256       41
#define XED_ISA_SET_AVX512_IFMA_512       42
#define XED_ISA_SET_AVX512_VAES_128       43
#define XED_ISA_SET_AVX512_VAES_256       44
#define XED_ISA_SET_AVX512_VAES_512       45
#define XED_ISA_SET_AVX512_VBMI2_128      46
#define XED_ISA_SET_AVX512_VBMI2_256      47
#define XED_ISA_SET_AVX512_VBMI2_512      48
#define XED_ISA_SET_AVX512_VBMI_128       49
#define XED_ISA_SET_AVX512_VBMI_256       50
#define XED_ISA_SET_AVX512_VBMI_512       51
#define XED_ISA_SET_AVX512_VNNI_128       52
#define XED_ISA_SET_AVX512_VNNI_256       53
#define XED_ISA_SET_AVX512_VNNI_512       54
#define XED_ISA_SET_AVX512_VPCLMULQDQ_128 55
#define XED_ISA_SET_AVX512_VPCLMULQDQ_256 56
#define XED_ISA_SET_AVX512_VPCLMULQDQ_512 57
#define XED_ISA_SET_AVX512_VPOPCNTDQ_128  58
#define XED_ISA_SET_AVX512_VPOPCNTDQ_256  59
#define XED_ISA_SET_AVX512_VPOPCNTDQ_512  60
#define XED_ISA_SET_AVXAES                61
#define XED_ISA_SET_AVX_GFNI              62
#define XED_ISA_SET_BMI1                  63
#define XED_ISA_SET_BMI2                  64
#define XED_ISA_SET_CET                   65
#define XED_ISA_SET_CLDEMOTE              66
#define XED_ISA_SET_CLFLUSHOPT            67
#define XED_ISA_SET_CLFSH                 68
#define XED_ISA_SET_CLWB                  69
#define XED_ISA_SET_CLZERO                70
#define XED_ISA_SET_CMOV                  71
#define XED_ISA_SET_CMPXCHG16B            72
#define XED_ISA_SET_F16C                  73
#define XED_ISA_SET_FAT_NOP               74
#define XED_ISA_SET_FCMOV                 75
#define XED_ISA_SET_FMA                   76
#define XED_ISA_SET_FMA4                  77
#define XED_ISA_SET_FXSAVE                78
#define XED_ISA_SET_FXSAVE64              79
#define XED_ISA_SET_GFNI                  80
#define XED_ISA_SET_I186                  81
#define XED_ISA_SET_I286PROTECTED         82
#define XED_ISA_SET_I286REAL              83
#define XED_ISA_SET_I386                  84
#define XED_ISA_SET_I486                  85
#define XED_ISA_SET_I486REAL              86
#define XED_ISA_SET_I86                   87
#define XED_ISA_SET_INVPCID               88
#define XED_ISA_SET_LAHF                  89
#define XED_ISA_SET_LONGMODE              90
#define XED_ISA_SET_LZCNT                 91
#define XED_ISA_SET_MONITOR               92
#define XED_ISA_SET_MONITORX              93
#define XED_ISA_SET_MOVBE                 94
#define XED_ISA_SET_MOVDIR                95
#define XED_ISA_SET_MPX                   96
#define XED_ISA_SET_PAUSE                 97
#define XED_ISA_SET_PCLMULQDQ             98
#define XED_ISA_SET_PCONFIG               99
#define XED_ISA_SET_PENTIUMMMX            100
#define XED_ISA_SET_PENTIUMREAL           101
#define XED_ISA_SET_PKU                   102
#define XED_ISA_SET_POPCNT                103
#define XED_ISA_SET_PPRO                  104
#define XED_ISA_SET_PREFETCHW             105
#define XED_ISA_SET_PREFETCHWT1           106
#define XED_ISA_SET_PREFETCH_NOP          107
#define XED_ISA_SET_PT                    108
#define XED_ISA_SET_RDPID                 109
#define XED_ISA_SET_RDPMC                 110
#define XED_ISA_SET_RDRAND                111
#define XED_ISA_SET_RDSEED                112
#define XED_ISA_SET_RDTSCP                113
#define XED_ISA_SET_RDWRFSGS              114
#define XED_ISA_SET_RTM                   115
#define XED_ISA_SET_SGX                   116
#define XED_ISA_SET_SGX_ENCLV             117
#define XED_ISA_SET_SHA                   118
#define XED_ISA_SET_SMAP                  119
#define XED_ISA_SET_SMX                   120
#define XED_ISA_SET_SSE                   121
#define XED_ISA_SET_SSE2                  122
#define XED_ISA_SET_SSE2MMX               123
#define XED_ISA_SET_SSE3                  124
#define XED_ISA_SET_SSE3X87               125
#define XED_ISA_SET_SSE4                  126
#define XED_ISA_SET_SSE42                 127
#define XED_ISA_SET_SSE4A                 128
#define XED_ISA_SET_SSEMXCSR              129
#define XED_ISA_SET_SSE_PREFETCH          130
#define XED_ISA_SET_SSSE3                 131
#define XED_ISA_SET_SSSE3MMX              132
#define XED_ISA_SET_SVM                   133
#define XED_ISA_SET_TBM                   134
#define XED_ISA_SET_VAES                  135
#define XED_ISA_SET_VMFUNC                136
#define XED_ISA_SET_VPCLMULQDQ            137
#define XED_ISA_SET_VTX                   138
#define XED_ISA_SET_WAITPKG               139
#define XED_ISA_SET_WBNOINVD              140
#define XED_ISA_SET_X87                   141
#define XED_ISA_SET_XOP                   142
#define XED_ISA_SET_XSAVE                 143
#define XED_ISA_SET_XSAVEC                144
#define XED_ISA_SET_XSAVEOPT              145
#define XED_ISA_SET_XSAVES                146
#define XED_ISA_SET_LAST                  147

struct XedChipFeatures {
  uint64_t f[3];
};

#define xed_set_chip_modes(d, chip)    \
  do {                                 \
    struct XedDecodedInst *__d = d;    \
    switch (chip) {                    \
      case XED_CHIP_INVALID:           \
        break;                         \
      case XED_CHIP_I86:               \
      case XED_CHIP_I86FP:             \
      case XED_CHIP_I186:              \
      case XED_CHIP_I186FP:            \
      case XED_CHIP_I286REAL:          \
      case XED_CHIP_I286:              \
      case XED_CHIP_I2186FP:           \
      case XED_CHIP_I386REAL:          \
      case XED_CHIP_I386:              \
      case XED_CHIP_I386FP:            \
      case XED_CHIP_I486REAL:          \
      case XED_CHIP_I486:              \
      case XED_CHIP_QUARK:             \
      case XED_CHIP_PENTIUM:           \
      case XED_CHIP_PENTIUMREAL:       \
      case XED_CHIP_PENTIUMMMX:        \
      case XED_CHIP_PENTIUMMMXREAL:    \
        __d->op.mode_first_prefix = 1; \
        break;                         \
      default:                         \
        break;                         \
    }                                  \
    switch (chip) {                    \
      case XED_CHIP_INVALID:           \
      case XED_CHIP_ALL:               \
      case XED_CHIP_AMD:               \
        break;                         \
      default:                         \
        __d->op.is_intel_specific = 1; \
        break;                         \
    }                                  \
  } while (0)

extern const uint64_t kXedChipFeatures[XED_CHIP_LAST][3];

bool xed_test_chip_features(struct XedChipFeatures *, int);
void xed_get_chip_features(struct XedChipFeatures *, int);
bool xed_isa_set_is_valid_for_chip(int, int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_XED_X86ISA_H_ */
