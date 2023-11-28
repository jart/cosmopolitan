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

#define XED_SEG_ES 1
#define XED_SEG_CS 2
#define XED_SEG_SS 3
#define XED_SEG_DS 4
#define XED_SEG_FS 5
#define XED_SEG_GS 6

#define xed_modrm_mod(M)           (((M)&0xff) >> 6)
#define xed_modrm_reg(M)           (((M)&0b00111000) >> 3)
#define xed_modrm_rm(M)            ((M)&7)
#define xed_sib_base(M)            ((M)&7)
#define xed_sib_index(M)           (((M)&0b00111000) >> 3)
#define xed_sib_scale(M)           (((M)&0xff) >> 6)
#define xed_get_modrm_reg_field(M) (((M)&0x38) >> 3)

COSMOPOLITAN_C_START_

#define XED_MACHINE_MODE_REAL           XED_MODE_REAL
#define XED_MACHINE_MODE_LEGACY_32      XED_MODE_LEGACY
#define XED_MACHINE_MODE_LONG_64        XED_MODE_LONG
#define XED_MACHINE_MODE_UNREAL         (1 << 2 | XED_MODE_REAL)
#define XED_MACHINE_MODE_LEGACY_16      (2 << 2 | XED_MODE_REAL)
#define XED_MACHINE_MODE_LONG_COMPAT_16 (3 << 2 | XED_MODE_REAL)
#define XED_MACHINE_MODE_LONG_COMPAT_32 (4 << 2 | XED_MODE_LEGACY)
#define XED_MACHINE_MODE_LAST           (XED_MACHINE_MODE_LONG_COMPAT_32 + 1)

#define XED_ERROR_NONE                         0
#define XED_ERROR_BUFFER_TOO_SHORT             1
#define XED_ERROR_GENERAL_ERROR                2
#define XED_ERROR_INVALID_FOR_CHIP             3
#define XED_ERROR_BAD_REGISTER                 4
#define XED_ERROR_BAD_LOCK_PREFIX              5
#define XED_ERROR_BAD_REP_PREFIX               6
#define XED_ERROR_BAD_LEGACY_PREFIX            7
#define XED_ERROR_BAD_REX_PREFIX               8
#define XED_ERROR_BAD_EVEX_UBIT                9
#define XED_ERROR_BAD_MAP                      10
#define XED_ERROR_BAD_EVEX_V_PRIME             11
#define XED_ERROR_BAD_EVEX_Z_NO_MASKING        12
#define XED_ERROR_NO_OUTPUT_POINTER            13
#define XED_ERROR_NO_AGEN_CALL_BACK_REGISTERED 14
#define XED_ERROR_BAD_MEMOP_INDEX              15
#define XED_ERROR_CALLBACK_PROBLEM             16
#define XED_ERROR_GATHER_REGS                  17
#define XED_ERROR_INSTR_TOO_LONG               18
#define XED_ERROR_INVALID_MODE                 19
#define XED_ERROR_BAD_EVEX_LL                  20
#define XED_ERROR_UNIMPLEMENTED                21
#define XED_ERROR_LAST                         22

#define XED_ADDRESS_WIDTH_INVALID 0
#define XED_ADDRESS_WIDTH_16b     2
#define XED_ADDRESS_WIDTH_32b     4
#define XED_ADDRESS_WIDTH_64b     8
#define XED_ADDRESS_WIDTH_LAST    9

#define XED_ILD_MAP0        0 /* 8086+  ... */
#define XED_ILD_MAP1        1 /* 286+   0x0F,... */
#define XED_ILD_MAP2        2 /* Core2+ 0x0F,0x38,... */
#define XED_ILD_MAP3        3 /* Core2+ 0x0F,0x3A,... */
#define XED_ILD_MAP4        4
#define XED_ILD_MAP5        5
#define XED_ILD_MAP6        6
#define XED_ILD_MAPAMD      7
#define XED_ILD_MAP_XOP8    8
#define XED_ILD_MAP_XOP9    9
#define XED_ILD_MAP_XOPA    10
#define XED_ILD_MAP_LAST    11
#define XED_ILD_MAP_INVALID 12

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
      uint8_t map : 4;
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
  bool osz : 1;   /* operand size override prefix */
  bool rexw : 1;  /* rex.w or rex.wb or etc. 64-bit override */
  bool rexb : 1;  /* rex.b or rex.wb or etc. see modrm table */
  bool rexr : 1;  /* rex.r or rex.wr or etc. see modrm table */
  bool rex : 1;   /* any rex prefix including rex */
  bool rexx : 1;  /* rex.x or rex.wx or etc. see sib table */
  bool rexrr : 1; /* evex */
  bool asz : 1;   /* address size override */
  int64_t disp;   /* displacement(%xxx) mostly sign-extended */
  uint64_t uimm0; /* $immediate mostly sign-extended */
  bool out_of_bytes : 1;
  bool is_intel_specific : 1;
  bool ild_f2 : 1;
  bool ild_f3 : 1;
  bool has_sib : 1;
  bool realmode : 1;
  bool amd3dnow : 1;
  bool lock : 1;
  union {
    uint8_t modrm; /* selects address register */
    struct {
      uint8_t rm : 3;
      uint8_t reg : 3;
      uint8_t mod : 2;
    };
  };
  uint8_t max_bytes;
  uint8_t rep : 2; /* 0, 2 (0xf2 repnz), 3 (0xf3 rep/repe) */
  uint8_t has_modrm : 2;
  bool imm_signed : 1;       /* internal */
  bool disp_unsigned : 1;    /* internal */
  uint8_t seg_ovd : 3;       /* XED_SEG_xx */
  uint8_t error : 5;         /* enum XedError */
  uint8_t mode : 2;          /* real,legacy,long */
  uint8_t hint : 3;          /* static branch prediction */
  uint8_t uimm1;             /* enter $x,$y */
  uint8_t disp_width;        /* in bits */
  uint8_t imm_width;         /* in bits */
  uint8_t mode_first_prefix; /* see xed_set_chip_modes() */
  uint8_t nrexes;
  uint8_t nprefixes;
  uint8_t nseg_prefixes;
  uint8_t ubit;       /* vex */
  uint8_t vexvalid;   /* vex */
  uint8_t vexdest3;   /* vex */
  uint8_t vexdest4;   /* vex */
  uint8_t vexdest210; /* vex */
  uint8_t vex_prefix; /* vex */
  uint8_t zeroing;    /* evex */
  uint8_t bcrc;       /* evex */
  uint8_t llrc;       /* evex */
  uint8_t vl;         /* evex */
  uint8_t mask;       /* evex */
  uint8_t imm1_bytes; /* evex */
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

#define xed_operands_set_mode(p, machine_mode) \
  do {                                         \
    struct XedOperands *__p = p;               \
    __p->realmode = false;                     \
    switch (machine_mode) {                    \
      default:                                 \
      case XED_MACHINE_MODE_LONG_64:           \
        __p->mode = XED_MODE_LONG;             \
        break;                                 \
      case XED_MACHINE_MODE_LEGACY_32:         \
      case XED_MACHINE_MODE_LONG_COMPAT_32:    \
        __p->mode = XED_MODE_LEGACY;           \
        break;                                 \
      case XED_MACHINE_MODE_REAL:              \
        __p->realmode = true;                  \
        __p->mode = XED_MODE_REAL;             \
        break;                                 \
      case XED_MACHINE_MODE_UNREAL:            \
        __p->realmode = true;                  \
        __p->mode = XED_MODE_LEGACY;           \
        break;                                 \
      case XED_MACHINE_MODE_LEGACY_16:         \
      case XED_MACHINE_MODE_LONG_COMPAT_16:    \
        __p->mode = XED_MODE_REAL;             \
        break;                                 \
    }                                          \
  } while (0)

extern const char kXedErrorNames[];
extern const uint8_t kXedEamode[2][3];

struct XedDecodedInst *xed_decoded_inst_zero_set_mode(struct XedDecodedInst *,
                                                      int);
int xed_instruction_length_decode(struct XedDecodedInst *, const void *,
                                  size_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_XED_X86_H_ */
