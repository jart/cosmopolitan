#ifndef COSMOPOLITAN_THIRD_PARTY_XED_X86_H_
#define COSMOPOLITAN_THIRD_PARTY_XED_X86_H_
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
/*           ▓▓▓▓▓▓▓▓▓▓▓▓▓                      ▄▄▄▄
             ▓▓▓▓▓▓▓▓▓▓▓▓▓     ▄▓▓▓▓▓▓▄      ▄▓▓▓▓▓▓▓▓            ▄▓▓▓▀
             ▓▓▓▓    ▓▓▓▓▓    ▓     ▓▓▓▓    ▓▓       ▓▓▓      ▄▓▓▓▓
▬▬▬▬▬▬▬▬▬▬▬▬▬▓▓▓▓▓▓▓▓▓▓▓▓▓▬▬▬▬▬▬▬▬▬▬▬▓▓▓▬▬▬▓▓▓▬▬▬▬▬▬▬▬▓▓▬▬▬▬▓▓▓▓▓▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬
        │    ▓▓▓▓    ▓▓▓▓▓          ▓▓▓     ▓▓▓▄     ▓▓▓   ▓▓▓▓             │
▬▬▬▬▬▬▬▬▬▬▬▬▬▓▓▓▓    ▓▓▓▓▓▬▬▬▬▬▬▬▬▓▓▓▓▓▓▓▬▬▬▀▓▓▓▓▄▄▄▓▓▓▬▬▓▓▓▓▓▓▓▓▓▓▓▓▓▬▬▬▬▬▬▬▬▬▬
        │    ▓▓▓▓    ▓▓▓▓▓          ▓▓▓▓▓▄   ▄▄▓▓▓▓▓▓▓▓▓  ▓▓▓▓      ▓▓▓▄    │
▬▬▬▬▬▬▬▬▬▬▬▬▬▓▓▓▓    ▓▓▓▓▓▬▬▬▬▬▬▬▬▬▬▬▓▓▓▓▓▄▄▓▓▀  ▀▀▓▓▓▓▓▓▓▓▓▓▬▬▬▬▬▬▬▬▓▓▓▬▬▬▬▬▬▬▬
▬▬▬▬▬▬▬▬║▬▬▬▬▓▓▓▓    ▓▓▓▓▓▬▬▬▬▬▬▬▬▬▬▬▬▓▓▓▓▓▓▓▬▬▬▬▬▬▬▬▬▓▓▓ ▓▓▓▬▬▬▬▬▬▬▬▓▓▓▬▬▬▬║▬▬▬
▬▬▬▬▬▬▬▬▬▬▬▬▬▓▓▓▓    ▓▓▓▓▓▬▬▬▬▬▬▬▬▬▬▬▓▓▓▓▬▬▓▓▓▬▬▬▬▬▬▬▬▓▓▓▬▓▓▓▓▬▬▬▬▬▬▬▓▓▓▬▬▬▬▬▬▬▬
■■■■■■■■║■■■■▓▓▓▓    ▓▓▓▓▓■■■▓▓▓▄▄▄▓▓▓▓■■■■▬▓▓▓▓▄▄▄▄▓▓▓■■■■▬▓▓▓▓▄▄▄▓▓▓▓▀■■■■║■■■
■■■■■■■■■■■■■▓▓▓▓▓▓▓▓▓▓▓▓▓■■■■■▀▓▓▓■■■■■■■■■■■■■■▀▀■■■■■■■■■■■■▀▓▓▀■■■■■■■■■■■■■
        ║▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓║
╔───────╨───────────────────────────────────────────────────────────────────╨──╗
│ cosmopolitan § virtual machine » byte code language                          │
╚─────────────────────────────────────────────────────────────────────────────*/

#define XED_MAX_INSTRUCTION_BYTES 15

#define XED_MODE_REAL   0
#define XED_MODE_LEGACY 1
#define XED_MODE_LONG   2

#define XED_HINT_NTAKEN 1
#define XED_HINT_TAKEN  2
#define XED_HINT_ALTER  3

#define xed_modrm_mod(M)           (((M)&0xff) >> 6)
#define xed_modrm_reg(M)           (((M) >> 3) & 7)
#define xed_modrm_rm(M)            ((M)&7)
#define xed_sib_base(M)            ((M)&7)
#define xed_sib_index(M)           (((M) >> 3) & 7)
#define xed_sib_scale(M)           (((M)&0xff) >> 6)
#define xed_get_modrm_reg_field(M) (((M)&0x38) >> 3)

enum XedMachineMode {
  XED_MACHINE_MODE_INVALID,
  XED_MACHINE_MODE_LONG_64,
  XED_MACHINE_MODE_LONG_COMPAT_32,
  XED_MACHINE_MODE_LONG_COMPAT_16,
  XED_MACHINE_MODE_LEGACY_32,
  XED_MACHINE_MODE_LEGACY_16,
  XED_MACHINE_MODE_REAL,
  XED_MACHINE_MODE_UNREAL,
  XED_MACHINE_MODE_LAST
};

enum XedError {
  XED_ERROR_NONE,
  XED_ERROR_BUFFER_TOO_SHORT,
  XED_ERROR_GENERAL_ERROR,
  XED_ERROR_INVALID_FOR_CHIP,
  XED_ERROR_BAD_REGISTER,
  XED_ERROR_BAD_LOCK_PREFIX,
  XED_ERROR_BAD_REP_PREFIX,
  XED_ERROR_BAD_LEGACY_PREFIX,
  XED_ERROR_BAD_REX_PREFIX,
  XED_ERROR_BAD_EVEX_UBIT,
  XED_ERROR_BAD_MAP,
  XED_ERROR_BAD_EVEX_V_PRIME,
  XED_ERROR_BAD_EVEX_Z_NO_MASKING,
  XED_ERROR_NO_OUTPUT_POINTER,
  XED_ERROR_NO_AGEN_CALL_BACK_REGISTERED,
  XED_ERROR_BAD_MEMOP_INDEX,
  XED_ERROR_CALLBACK_PROBLEM,
  XED_ERROR_GATHER_REGS,
  XED_ERROR_INSTR_TOO_LONG,
  XED_ERROR_INVALID_MODE,
  XED_ERROR_BAD_EVEX_LL,
  XED_ERROR_UNIMPLEMENTED,
  XED_ERROR_LAST
};

enum XedAddressWidth {
  XED_ADDRESS_WIDTH_INVALID = 0,
  XED_ADDRESS_WIDTH_16b = 2,
  XED_ADDRESS_WIDTH_32b = 4,
  XED_ADDRESS_WIDTH_64b = 8,
  XED_ADDRESS_WIDTH_LAST
};

enum XedChip {
  XED_CHIP_INVALID = 1,
  XED_CHIP_I86 = 2,
  XED_CHIP_I86FP = 3,
  XED_CHIP_I186 = 4,
  XED_CHIP_I186FP = 5,
  XED_CHIP_I286REAL = 6,
  XED_CHIP_I286 = 7,
  XED_CHIP_I2186FP = 8,
  XED_CHIP_I386REAL = 9,
  XED_CHIP_I386 = 10,
  XED_CHIP_I386FP = 11,
  XED_CHIP_I486REAL = 12,
  XED_CHIP_I486 = 13,
  XED_CHIP_PENTIUMREAL = 14,
  XED_CHIP_PENTIUM = 15,
  XED_CHIP_QUARK = 16,
  XED_CHIP_PENTIUMMMXREAL = 17,
  XED_CHIP_PENTIUMMMX = 18,
  XED_CHIP_ALLREAL = 19,
  XED_CHIP_PENTIUMPRO = 20,
  XED_CHIP_PENTIUM2 = 21,
  XED_CHIP_PENTIUM3 = 22,
  XED_CHIP_PENTIUM4 = 23,
  XED_CHIP_P4PRESCOTT = 24,
  XED_CHIP_P4PRESCOTT_NOLAHF = 25,
  XED_CHIP_P4PRESCOTT_VTX = 26,
  XED_CHIP_CORE2 = 27,
  XED_CHIP_PENRYN = 28,
  XED_CHIP_PENRYN_E = 29,
  XED_CHIP_NEHALEM = 30,
  XED_CHIP_WESTMERE = 31,
  XED_CHIP_BONNELL = 32,
  XED_CHIP_SALTWELL = 33,
  XED_CHIP_SILVERMONT = 34,
  XED_CHIP_AMD = 35,
  XED_CHIP_GOLDMONT = 36,
  XED_CHIP_GOLDMONT_PLUS = 37,
  XED_CHIP_TREMONT = 38,
  XED_CHIP_SANDYBRIDGE = 39,
  XED_CHIP_IVYBRIDGE = 40,
  XED_CHIP_HASWELL = 41,
  XED_CHIP_BROADWELL = 42,
  XED_CHIP_SKYLAKE = 43,
  XED_CHIP_SKYLAKE_SERVER = 44,
  XED_CHIP_CASCADE_LAKE = 45,
  XED_CHIP_KNL = 46,
  XED_CHIP_KNM = 47,
  XED_CHIP_CANNONLAKE = 48,
  XED_CHIP_ICELAKE = 49,
  XED_CHIP_ICELAKE_SERVER = 50,
  XED_CHIP_FUTURE = 51,
  XED_CHIP_ALL = 52,
  XED_CHIP_LAST
};

enum XedIsaSet {
  XED_ISA_SET_INVALID,
  XED_ISA_SET_3DNOW,
  XED_ISA_SET_ADOX_ADCX,
  XED_ISA_SET_AES,
  XED_ISA_SET_AMD,
  XED_ISA_SET_AVX,
  XED_ISA_SET_AVX2,
  XED_ISA_SET_AVX2GATHER,
  XED_ISA_SET_AVX512BW_128,
  XED_ISA_SET_AVX512BW_128N,
  XED_ISA_SET_AVX512BW_256,
  XED_ISA_SET_AVX512BW_512,
  XED_ISA_SET_AVX512BW_KOP,
  XED_ISA_SET_AVX512CD_128,
  XED_ISA_SET_AVX512CD_256,
  XED_ISA_SET_AVX512CD_512,
  XED_ISA_SET_AVX512DQ_128,
  XED_ISA_SET_AVX512DQ_128N,
  XED_ISA_SET_AVX512DQ_256,
  XED_ISA_SET_AVX512DQ_512,
  XED_ISA_SET_AVX512DQ_KOP,
  XED_ISA_SET_AVX512DQ_SCALAR,
  XED_ISA_SET_AVX512ER_512,
  XED_ISA_SET_AVX512ER_SCALAR,
  XED_ISA_SET_AVX512F_128,
  XED_ISA_SET_AVX512F_128N,
  XED_ISA_SET_AVX512F_256,
  XED_ISA_SET_AVX512F_512,
  XED_ISA_SET_AVX512F_KOP,
  XED_ISA_SET_AVX512F_SCALAR,
  XED_ISA_SET_AVX512PF_512,
  XED_ISA_SET_AVX512_4FMAPS_512,
  XED_ISA_SET_AVX512_4FMAPS_SCALAR,
  XED_ISA_SET_AVX512_4VNNIW_512,
  XED_ISA_SET_AVX512_BITALG_128,
  XED_ISA_SET_AVX512_BITALG_256,
  XED_ISA_SET_AVX512_BITALG_512,
  XED_ISA_SET_AVX512_GFNI_128,
  XED_ISA_SET_AVX512_GFNI_256,
  XED_ISA_SET_AVX512_GFNI_512,
  XED_ISA_SET_AVX512_IFMA_128,
  XED_ISA_SET_AVX512_IFMA_256,
  XED_ISA_SET_AVX512_IFMA_512,
  XED_ISA_SET_AVX512_VAES_128,
  XED_ISA_SET_AVX512_VAES_256,
  XED_ISA_SET_AVX512_VAES_512,
  XED_ISA_SET_AVX512_VBMI2_128,
  XED_ISA_SET_AVX512_VBMI2_256,
  XED_ISA_SET_AVX512_VBMI2_512,
  XED_ISA_SET_AVX512_VBMI_128,
  XED_ISA_SET_AVX512_VBMI_256,
  XED_ISA_SET_AVX512_VBMI_512,
  XED_ISA_SET_AVX512_VNNI_128,
  XED_ISA_SET_AVX512_VNNI_256,
  XED_ISA_SET_AVX512_VNNI_512,
  XED_ISA_SET_AVX512_VPCLMULQDQ_128,
  XED_ISA_SET_AVX512_VPCLMULQDQ_256,
  XED_ISA_SET_AVX512_VPCLMULQDQ_512,
  XED_ISA_SET_AVX512_VPOPCNTDQ_128,
  XED_ISA_SET_AVX512_VPOPCNTDQ_256,
  XED_ISA_SET_AVX512_VPOPCNTDQ_512,
  XED_ISA_SET_AVXAES,
  XED_ISA_SET_AVX_GFNI,
  XED_ISA_SET_BMI1,
  XED_ISA_SET_BMI2,
  XED_ISA_SET_CET,
  XED_ISA_SET_CLDEMOTE,
  XED_ISA_SET_CLFLUSHOPT,
  XED_ISA_SET_CLFSH,
  XED_ISA_SET_CLWB,
  XED_ISA_SET_CLZERO,
  XED_ISA_SET_CMOV,
  XED_ISA_SET_CMPXCHG16B,
  XED_ISA_SET_F16C,
  XED_ISA_SET_FAT_NOP,
  XED_ISA_SET_FCMOV,
  XED_ISA_SET_FMA,
  XED_ISA_SET_FMA4,
  XED_ISA_SET_FXSAVE,
  XED_ISA_SET_FXSAVE64,
  XED_ISA_SET_GFNI,
  XED_ISA_SET_I186,
  XED_ISA_SET_I286PROTECTED,
  XED_ISA_SET_I286REAL,
  XED_ISA_SET_I386,
  XED_ISA_SET_I486,
  XED_ISA_SET_I486REAL,
  XED_ISA_SET_I86,
  XED_ISA_SET_INVPCID,
  XED_ISA_SET_LAHF,
  XED_ISA_SET_LONGMODE,
  XED_ISA_SET_LZCNT,
  XED_ISA_SET_MONITOR,
  XED_ISA_SET_MONITORX,
  XED_ISA_SET_MOVBE,
  XED_ISA_SET_MOVDIR,
  XED_ISA_SET_MPX,
  XED_ISA_SET_PAUSE,
  XED_ISA_SET_PCLMULQDQ,
  XED_ISA_SET_PCONFIG,
  XED_ISA_SET_PENTIUMMMX,
  XED_ISA_SET_PENTIUMREAL,
  XED_ISA_SET_PKU,
  XED_ISA_SET_POPCNT,
  XED_ISA_SET_PPRO,
  XED_ISA_SET_PREFETCHW,
  XED_ISA_SET_PREFETCHWT1,
  XED_ISA_SET_PREFETCH_NOP,
  XED_ISA_SET_PT,
  XED_ISA_SET_RDPID,
  XED_ISA_SET_RDPMC,
  XED_ISA_SET_RDRAND,
  XED_ISA_SET_RDSEED,
  XED_ISA_SET_RDTSCP,
  XED_ISA_SET_RDWRFSGS,
  XED_ISA_SET_RTM,
  XED_ISA_SET_SGX,
  XED_ISA_SET_SGX_ENCLV,
  XED_ISA_SET_SHA,
  XED_ISA_SET_SMAP,
  XED_ISA_SET_SMX,
  XED_ISA_SET_SSE,
  XED_ISA_SET_SSE2,
  XED_ISA_SET_SSE2MMX,
  XED_ISA_SET_SSE3,
  XED_ISA_SET_SSE3X87,
  XED_ISA_SET_SSE4,
  XED_ISA_SET_SSE42,
  XED_ISA_SET_SSE4A,
  XED_ISA_SET_SSEMXCSR,
  XED_ISA_SET_SSE_PREFETCH,
  XED_ISA_SET_SSSE3,
  XED_ISA_SET_SSSE3MMX,
  XED_ISA_SET_SVM,
  XED_ISA_SET_TBM,
  XED_ISA_SET_VAES,
  XED_ISA_SET_VMFUNC,
  XED_ISA_SET_VPCLMULQDQ,
  XED_ISA_SET_VTX,
  XED_ISA_SET_WAITPKG,
  XED_ISA_SET_WBNOINVD,
  XED_ISA_SET_X87,
  XED_ISA_SET_XOP,
  XED_ISA_SET_XSAVE,
  XED_ISA_SET_XSAVEC,
  XED_ISA_SET_XSAVEOPT,
  XED_ISA_SET_XSAVES,
  XED_ISA_SET_LAST
};

enum XedIldMap {
  XED_ILD_MAP0,
  XED_ILD_MAP1,
  XED_ILD_MAP2,
  XED_ILD_MAP3,
  XED_ILD_MAP4,
  XED_ILD_MAP5,
  XED_ILD_MAP6,
  XED_ILD_MAPAMD,
  XED_ILD_MAP_XOP8,
  XED_ILD_MAP_XOP9,
  XED_ILD_MAP_XOPA,
  XED_ILD_MAP_LAST,
  XED_ILD_MAP_INVALID
};

struct XedChipFeatures {
  uint64_t f[3];
};

struct XedOperands {
  uint8_t imm_width;
  uint8_t map;   /* enum XedIldMap */
  uint8_t error; /* enum XedError */
  uint8_t mode;
  uint8_t rexw;
  uint8_t osz;
  uint8_t max_bytes;
  uint8_t nominal_opcode;
  uint8_t out_of_bytes;
  uint8_t disp_width;
  int64_t disp;
  uint64_t uimm0;
  enum XedChip chip;
  uint8_t amd3dnow;
  uint8_t asz;
  uint8_t bcrc;
  uint8_t cldemote;
  uint8_t has_sib;
  uint8_t ild_f2;
  uint8_t ild_f3;
  uint8_t lock;
  uint8_t modep5;
  uint8_t modep55c;
  uint8_t mode_first_prefix;
  uint8_t prefix66;
  uint8_t realmode;
  uint8_t rex;
  uint8_t rexb;
  uint8_t rexr;
  uint8_t rexrr;
  uint8_t rexx;
  uint8_t ubit;
  uint8_t vexdest3;
  uint8_t vexdest4;
  uint8_t wbnoinvd;
  uint8_t zeroing;
  uint8_t first_f2f3;
  uint8_t has_modrm;
  uint8_t last_f2f3;
  uint8_t llrc;
  uint8_t mod;
  uint8_t rep;
  uint8_t sibscale;
  uint8_t vex_prefix;
  uint8_t vl;
  uint8_t hint;
  uint8_t mask;
  uint8_t reg;
  uint8_t rm;
  uint8_t seg_ovd;
  uint8_t sibbase;
  uint8_t sibindex;
  uint8_t srm;
  uint8_t vexdest210;
  uint8_t vexvalid;
  uint8_t esrc;
  uint8_t ild_seg;
  uint8_t imm1_bytes;
  uint8_t modrm_byte;
  uint8_t nprefixes;
  uint8_t nrexes;
  uint8_t nseg_prefixes;
  uint8_t pos_disp;
  uint8_t pos_imm;
  uint8_t pos_imm1;
  uint8_t pos_modrm;
  uint8_t pos_nominal_opcode;
  uint8_t pos_sib;
  uint8_t uimm1;
};

struct XedInst {
  uint8_t noperands;
  uint8_t cpl;
  uint8_t flag_complex;
  uint8_t exceptions;
  uint16_t flag_info_index;
  uint16_t iform_enum;
  uint16_t operand_base;
  uint16_t attributes;
};

struct XedEncoderIforms {
  unsigned x_MEMDISPv;
  unsigned x_SIBBASE_ENCODE_SIB1;
  unsigned x_VEX_MAP_ENC;
  unsigned x_SIB_NT;
  unsigned x_UIMM8_1;
  unsigned x_SIBBASE_ENCODE;
  unsigned x_VEX_ESCVL_ENC;
  unsigned x_PREFIX_ENC;
  unsigned x_VEXED_REX;
  unsigned x_REMOVE_SEGMENT;
  unsigned x_VSIB_ENC;
  unsigned x_EVEX_REXB_ENC;
  unsigned x_MODRM_RM_ENCODE_EA64_SIB0;
  unsigned x_VEX_REXXB_ENC;
  unsigned x_EVEX_REXRR_ENC;
  unsigned x_AVX512_EVEX_BYTE3_ENC;
  unsigned x_EVEX_REXW_VVVV_ENC;
  unsigned x_VEX_REG_ENC;
  unsigned x_SIMM8;
  unsigned x_XOP_MAP_ENC;
  unsigned x_MODRM_RM_ENCODE_EA32_SIB0;
  unsigned x_UIMM8;
  unsigned x_MODRM_RM_ENCODE_EA16_SIB0;
  unsigned x_XOP_REXXB_ENC;
  unsigned x_EVEX_MAP_ENC;
  unsigned x_MEMDISP8;
  unsigned x_MODRM_RM_ENCODE;
  unsigned x_REX_PREFIX_ENC;
  unsigned x_UIMM16;
  unsigned x_VEX_TYPE_ENC;
  unsigned x_EVEX_UPP_ENC;
  unsigned x_VEX_REXR_ENC;
  unsigned x_BRDISP32;
  unsigned x_MEMDISP32;
  unsigned x_MEMDISP16;
  unsigned x_SIBINDEX_ENCODE;
  unsigned x_SE_IMM8;
  unsigned x_UIMM32;
  unsigned x_SIMMz;
  unsigned x_UIMMv;
  unsigned x_EVEX_62_REXR_ENC;
  unsigned x_DISP_NT;
  unsigned x_MODRM_MOD_ENCODE;
  unsigned x_MEMDISP;
  unsigned x_VSIB_ENC_BASE;
  unsigned x_BRDISP8;
  unsigned x_BRDISPz;
  unsigned x_EVEX_REXX_ENC;
  unsigned x_XOP_TYPE_ENC;
};

struct XedEncoderVars {
  struct XedEncoderIforms iforms;
  unsigned short iform_index;
  unsigned ilen;
  unsigned olen;
  unsigned bit_offset;
};

struct XedDecodedInst {
  struct XedOperands operands;
  unsigned char decoded_length;
  uint8_t *bytes;
};

union XedAvxC4Payload1 {
  struct {
    unsigned map : 5;
    unsigned b_inv : 1;
    unsigned x_inv : 1;
    unsigned r_inv : 1;
    unsigned pad : 24;
  } s;
  unsigned u32;
};

union XedAvxC4Payload2 {
  struct {
    unsigned pp : 2;
    unsigned l : 1;
    unsigned vvv210 : 3;
    unsigned v3 : 1;
    unsigned w : 1;
    unsigned pad : 24;
  } s;
  unsigned u32;
};

union XedAvxC5Payload {
  struct {
    unsigned pp : 2;
    unsigned l : 1;
    unsigned vvv210 : 3;
    unsigned v3 : 1;
    unsigned r_inv : 1;
    unsigned pad : 24;
  } s;
  unsigned u32;
};

union XedAvx512Payload1 {
  struct {
    unsigned map : 4;
    unsigned rr_inv : 1;
    unsigned b_inv : 1;
    unsigned x_inv : 1;
    unsigned r_inv : 1;
    unsigned pad : 24;
  } s;
  unsigned u32;
};

union XedAvx512Payload2 {
  struct {
    unsigned pp : 2;
    unsigned ubit : 1;
    unsigned vexdest210 : 3;
    unsigned vexdest3 : 1;
    unsigned rexw : 1;
    unsigned pad : 24;
  } s;
  unsigned u32;
};

union XedAvx512Payload3 {
  struct {
    unsigned mask : 3;
    unsigned vexdest4p : 1;
    unsigned bcrc : 1;
    unsigned llrc : 2;
    unsigned z : 1;
    unsigned pad : 24;
  } s;
  unsigned u32;
};

forceinline unsigned char xed_decoded_inst_get_byte(
    const struct XedDecodedInst *p, long byte_index) {
  return p->bytes[byte_index];
}

forceinline void xed_operands_set_mode(struct XedOperands *p,
                                       enum XedMachineMode mmode) {
  p->realmode = 0;
  switch (mmode) {
    case XED_MACHINE_MODE_LONG_64:
      p->mode = 2;
      return;
    case XED_MACHINE_MODE_LEGACY_32:
    case XED_MACHINE_MODE_LONG_COMPAT_32:
      p->mode = 1;
      break;
    case XED_MACHINE_MODE_REAL:
      p->realmode = 1;
      p->mode = 0;
      break;
    case XED_MACHINE_MODE_UNREAL:
      p->realmode = 1;
      p->mode = 1;
      break;
    case XED_MACHINE_MODE_LEGACY_16:
    case XED_MACHINE_MODE_LONG_COMPAT_16:
      p->mode = 0;
      break;
    default:
      abort();
  }
}

forceinline struct XedDecodedInst *xed_decoded_inst_zero_set_mode(
    struct XedDecodedInst *p, enum XedMachineMode mmode) {
  memset(p, 0, sizeof(*p));
  xed_operands_set_mode(&p->operands, mmode);
  return p;
}

extern const uint64_t xed_chip_features[XED_CHIP_LAST][3] hidden;

enum XedError xed_instruction_length_decode(struct XedDecodedInst *,
                                            const unsigned char *, size_t);

bool xed_isa_set_is_valid_for_chip(enum XedIsaSet, enum XedChip);
bool xed_test_chip_features(struct XedChipFeatures *, enum XedIsaSet);
void xed_get_chip_features(struct XedChipFeatures *, enum XedChip);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_XED_X86_H_ */
