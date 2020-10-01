#ifndef COSMOPOLITAN_THIRD_PARTY_XED_X86_H_
#define COSMOPOLITAN_THIRD_PARTY_XED_X86_H_
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

#define XED_HINT_NTAKEN 2
#define XED_HINT_TAKEN  4
#define XED_HINT_ALTER  6

#define xed_modrm_mod(M)           (((M)&0xff) >> 6)
#define xed_modrm_reg(M)           (((M)&0b00111000) >> 3)
#define xed_modrm_rm(M)            ((M)&7)
#define xed_sib_base(M)            ((M)&7)
#define xed_sib_index(M)           (((M)&0b00111000) >> 3)
#define xed_sib_scale(M)           (((M)&0xff) >> 6)
#define xed_get_modrm_reg_field(M) (((M)&0x38) >> 3)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

enum XedMachineMode {
  XED_MACHINE_MODE_REAL = XED_MODE_REAL,
  XED_MACHINE_MODE_LEGACY_32 = XED_MODE_LEGACY,
  XED_MACHINE_MODE_LONG_64 = XED_MODE_LONG,
  XED_MACHINE_MODE_UNREAL = 1 << 2 | XED_MODE_REAL,
  XED_MACHINE_MODE_LEGACY_16 = 2 << 2 | XED_MODE_REAL,
  XED_MACHINE_MODE_LONG_COMPAT_16 = 3 << 2 | XED_MODE_REAL,
  XED_MACHINE_MODE_LONG_COMPAT_32 = 4 << 2 | XED_MODE_LEGACY,
  XED_MACHINE_MODE_LAST,
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
  XED_ILD_MAP0,  // 8086+  ...
  XED_ILD_MAP1,  // 286+   0x0F,...
  XED_ILD_MAP2,  // Core2+ 0x0F,0x38,...
  XED_ILD_MAP3,  // Core2+ 0x0F,0x3A,...
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

struct XedOperands { /*
  ┌rep
  │ ┌log₂𝑏
  │ │ ┌mode
  │ │ │ ┌eamode
  │ │ │ │ ┌mod
  │ │ │ │ │
  │ │ │ │ │  ┌sego
  │ │ │ │ │  │
  │ │ │ │ │  │   ┌rex         REGISTER
  │ │ │ │ │  │   │┌rexb       DISPATCH
  │ │ │ │ │  │   ││┌srm       ENCODING
  │ │ │ │ │  │   │││  ┌rex
  │ │ │ │ │  │   │││  │┌rexb
  │ │ │ │ │  │   │││  ││┌rm
  │ │ │ │ │  │   │││  │││  ┌rexw
  │ │ │ │ │  │   │││  │││  │┌osz
  │ │ │ │ │  │   │││  │││  ││┌rex
  │ │ │ │ │  │   │││  │││  │││┌rexr
  │ │ │ │ │  │   │││  │││  ││││┌reg
  │3│2│2│2│2 │   │││  │││  │││││
  │0│8│6│4│2 │18 │││12│││ 7│││││ 0
  ├┐├┐├┐├┐├┐ ├─┐ ││├─┐││├─┐││││├─┐
  00000000000000000000000000000000*/
  uint32_t rde;
  union {
    struct {
      union {
        uint8_t opcode;
        uint8_t srm : 3;
      };
      uint8_t map : 4;  // enum XedIldMap
    };
    uint16_t dispatch;
  };
  union {
    uint8_t sib;
    struct {
      uint8_t base : 3;
      uint8_t index : 3;
      uint8_t scale : 2;
    };
  };
  bool osz : 1;    // operand size override prefix
  bool rexw : 1;   // rex.w or rex.wb or etc. 64-bit override
  bool rexb : 1;   // rex.b or rex.wb or etc. see modrm table
  bool rexr : 1;   // rex.r or rex.wr or etc. see modrm table
  bool rex : 1;    // any rex prefix including rex
  bool rexx : 1;   // rex.x or rex.wx or etc. see sib table
  bool rexrr : 1;  // evex
  bool asz : 1;    // address size override
  int64_t disp;    // displacement(%xxx) mostly sign-extended
  uint64_t uimm0;  // $immediate mostly sign-extended
  bool out_of_bytes : 1;
  bool is_intel_specific : 1;
  bool ild_f2 : 1;
  bool ild_f3 : 1;
  bool has_sib : 1;
  bool realmode : 1;
  bool amd3dnow : 1;
  bool lock : 1;
  union {
    uint8_t modrm;  // selects address register
    struct {
      uint8_t rm : 3;
      uint8_t reg : 3;
      uint8_t mod : 2;
    };
  };
  uint8_t max_bytes;
  uint8_t rep : 2;  // 0, 2 (0xf2 repnz), 3 (0xf3 rep/repe)
  uint8_t has_modrm : 2;
  bool imm_signed : 1;        // internal
  bool disp_unsigned : 1;     // internal
  uint8_t seg_ovd : 3;        // XED_SEG_xx
  uint8_t error : 5;          // enum XedError
  uint8_t mode : 2;           // real,legacy,long
  uint8_t hint : 3;           // static branch prediction
  uint8_t uimm1;              // enter $x,$y
  uint8_t disp_width;         // in bits
  uint8_t imm_width;          // in bits
  uint8_t mode_first_prefix;  // see xed_set_chip_modes()
  uint8_t nrexes;
  uint8_t nprefixes;
  uint8_t nseg_prefixes;
  uint8_t ubit;        // vex
  uint8_t vexvalid;    // vex
  uint8_t vexdest3;    // vex
  uint8_t vexdest4;    // vex
  uint8_t vexdest210;  // vex
  uint8_t vex_prefix;  // vex
  uint8_t zeroing;     // evex
  uint8_t bcrc;        // evex
  uint8_t llrc;        // evex
  uint8_t vl;          // evex
  uint8_t mask;        // evex
  uint8_t imm1_bytes;  // evex
  uint8_t pos_disp;
  uint8_t pos_imm;
  uint8_t pos_imm1;
  uint8_t pos_modrm;
  uint8_t pos_opcode;
  uint8_t pos_sib;
};

struct XedDecodedInst {
  unsigned char length;
  uint8_t bytes[15];
  struct XedOperands op;
};

forceinline void xed_operands_set_mode(struct XedOperands *p,
                                       enum XedMachineMode mmode) {
  p->realmode = false;
  switch (mmode) {
    default:
    case XED_MACHINE_MODE_LONG_64:
      p->mode = XED_MODE_LONG;
      return;
    case XED_MACHINE_MODE_LEGACY_32:
    case XED_MACHINE_MODE_LONG_COMPAT_32:
      p->mode = XED_MODE_LEGACY;
      break;
    case XED_MACHINE_MODE_REAL:
      p->realmode = true;
      p->mode = XED_MODE_REAL;
      break;
    case XED_MACHINE_MODE_UNREAL:
      p->realmode = true;
      p->mode = XED_MODE_LEGACY;
      break;
    case XED_MACHINE_MODE_LEGACY_16:
    case XED_MACHINE_MODE_LONG_COMPAT_16:
      p->mode = XED_MODE_REAL;
      break;
  }
}

forceinline void xed_set_chip_modes(struct XedDecodedInst *d,
                                    enum XedChip chip) {
  switch (chip) {
    case XED_CHIP_INVALID:
      break;
    case XED_CHIP_I86:
    case XED_CHIP_I86FP:
    case XED_CHIP_I186:
    case XED_CHIP_I186FP:
    case XED_CHIP_I286REAL:
    case XED_CHIP_I286:
    case XED_CHIP_I2186FP:
    case XED_CHIP_I386REAL:
    case XED_CHIP_I386:
    case XED_CHIP_I386FP:
    case XED_CHIP_I486REAL:
    case XED_CHIP_I486:
    case XED_CHIP_QUARK:
    case XED_CHIP_PENTIUM:
    case XED_CHIP_PENTIUMREAL:
    case XED_CHIP_PENTIUMMMX:
    case XED_CHIP_PENTIUMMMXREAL:
      d->op.mode_first_prefix = 1;
      break;
    default:
      break;
  }
  switch (chip) {
    case XED_CHIP_INVALID:
    case XED_CHIP_ALL:
    case XED_CHIP_AMD:
      break;
    default:
      d->op.is_intel_specific = 1;
      break;
  }
}

extern const char kXedErrorNames[];
extern const uint64_t xed_chip_features[XED_CHIP_LAST][3];

struct XedDecodedInst *xed_decoded_inst_zero_set_mode(struct XedDecodedInst *,
                                                      enum XedMachineMode);

enum XedError xed_instruction_length_decode(struct XedDecodedInst *,
                                            const void *, size_t);

bool xed_isa_set_is_valid_for_chip(enum XedIsaSet, enum XedChip);
bool xed_test_chip_features(struct XedChipFeatures *, enum XedIsaSet);
void xed_get_chip_features(struct XedChipFeatures *, enum XedChip);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_XED_X86_H_ */
