/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2018 Intel Corporation                                             │
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Licensed under the Apache License, Version 2.0 (the "License");              │
│ you may not use this file except in compliance with the License.             │
│ You may obtain a copy of the License at                                      │
│                                                                              │
│     http://www.apache.org/licenses/LICENSE-2.0                               │
│                                                                              │
│ Unless required by applicable law or agreed to in writing, software          │
│ distributed under the License is distributed on an "AS IS" BASIS,            │
│ WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.     │
│ See the License for the specific language governing permissions and          │
│ limitations under the License.                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/macros.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "third_party/xed/avx.h"
#include "third_party/xed/avx512.h"
#include "third_party/xed/private.h"
#include "third_party/xed/x86.h"

asm(".ident\t\"\\n\\n\
Xed (Apache 2.0)\\n\
Copyright 2018 Intel Corporation\\n\
Copyright 2019 Justine Alexandra Roberts Tunney\\n\
Modifications: Trimmed down to 3kb [2019-03-22 jart]\"");
asm(".include \"libc/disclaimer.inc\"");

#define XED_ILD_HASMODRM_IGNORE_MOD 2

#define XED_I_LF_BRDISP8_BRDISP_WIDTH_CONST_l2            1
#define XED_I_LF_BRDISPz_BRDISP_WIDTH_OSZ_NONTERM_EOSZ_l2 2
#define XED_I_LF_DISP_BUCKET_0_l1                         3
#define XED_I_LF_EMPTY_DISP_CONST_l2                      4
#define XED_I_LF_MEMDISPv_DISP_WIDTH_ASZ_NONTERM_EASZ_l2  5
#define XED_I_LF_RESOLVE_BYREG_DISP_map0x0_op0xc7_l1      6

#define XED_I_LF_0_IMM_WIDTH_CONST_l2                     1
#define XED_I_LF_RESOLVE_BYREG_IMM_WIDTH_map0x0_op0xc7_l1 2
#define XED_I_LF_RESOLVE_BYREG_IMM_WIDTH_map0x0_op0xf6_l1 3
#define XED_I_LF_RESOLVE_BYREG_IMM_WIDTH_map0x0_op0xf7_l1 4
#define XED_I_LF_SIMM8_IMM_WIDTH_CONST_l2                 5
#define XED_I_LF_SIMMz_IMM_WIDTH_OSZ_NONTERM_DF64_EOSZ_l2 6
#define XED_I_LF_SIMMz_IMM_WIDTH_OSZ_NONTERM_EOSZ_l2      7
#define XED_I_LF_UIMM16_IMM_WIDTH_CONST_l2                8
#define XED_I_LF_UIMM8_IMM_WIDTH_CONST_l2                 9
#define XED_I_LF_UIMMv_IMM_WIDTH_OSZ_NONTERM_EOSZ_l2      10
#define xed_i_ild_hasimm_map0x0_op0xc8_l1                 11
#define xed_i_ild_hasimm_map0x0F_op0x78_l1                12

#define XED_LF_SIMMz_IMM_WIDTH_OSZ_NONTERM_IGNORE66_EOSZ_l2(X) \
  xed_set_simmz_imm_width_eosz(X, kXed.OSZ_NONTERM_IGNORE66_EOSZ)
#define XED_LF_SIMMz_IMM_WIDTH_OSZ_NONTERM_REFINING66_EOSZ_l2(X) \
  xed_set_simmz_imm_width_eosz(X, kXed.OSZ_NONTERM_REFINING66_EOSZ)
#define XED_LF_SIMMz_IMM_WIDTH_OSZ_NONTERM_DF64_FORCE64_EOSZ_l2(X) \
  xed_set_simmz_imm_width_eosz(X, kXed.OSZ_NONTERM_DF64_FORCE64_EOSZ)
#define XED_LF_SIMMz_IMM_WIDTH_OSZ_NONTERM_FORCE64_EOSZ_l2(X) \
  xed_set_simmz_imm_width_eosz(X, kXed.OSZ_NONTERM_FORCE64_EOSZ)
#define XED_LF_SIMMz_IMM_WIDTH_OSZ_NONTERM_EOSZ_l2(X) \
  xed_set_simmz_imm_width_eosz(X, kXed.OSZ_NONTERM_EOSZ)
#define XED_LF_SIMMz_IMM_WIDTH_OSZ_NONTERM_REFINING66_CR_WIDTH_EOSZ_l2(X) \
  xed_set_simmz_imm_width_eosz(X, kXed.OSZ_NONTERM_REFINING66_CR_WIDTH_EOSZ)
#define XED_LF_SIMMz_IMM_WIDTH_OSZ_NONTERM_DF64_EOSZ_l2(X) \
  xed_set_simmz_imm_width_eosz(X, kXed.OSZ_NONTERM_DF64_EOSZ)
#define XED_LF_SIMMz_IMM_WIDTH_OSZ_NONTERM_DF64_IMMUNE66_LOOP64_EOSZ_l2(X) \
  xed_set_simmz_imm_width_eosz(X, kXed.OSZ_NONTERM_DF64_IMMUNE66_LOOP64_EOSZ)
#define XED_LF_SIMMz_IMM_WIDTH_OSZ_NONTERM_IMMUNE66_EOSZ_l2(X) \
  xed_set_simmz_imm_width_eosz(X, kXed.OSZ_NONTERM_IMMUNE66_EOSZ)
#define XED_LF_SIMMz_IMM_WIDTH_OSZ_NONTERM_CR_WIDTH_EOSZ_l2(X) \
  xed_set_simmz_imm_width_eosz(X, kXed.OSZ_NONTERM_CR_WIDTH_EOSZ)
#define XED_LF_SIMMz_IMM_WIDTH_OSZ_NONTERM_IMMUNE_REXW_EOSZ_l2(X) \
  xed_set_simmz_imm_width_eosz(X, kXed.OSZ_NONTERM_IMMUNE_REXW_EOSZ)
#define XED_LF_UIMMv_IMM_WIDTH_OSZ_NONTERM_IGNORE66_EOSZ_l2(X) \
  xed_set_uimmv_imm_width_eosz(X, kXed.OSZ_NONTERM_IGNORE66_EOSZ)
#define XED_LF_UIMMv_IMM_WIDTH_OSZ_NONTERM_REFINING66_EOSZ_l2(X) \
  xed_set_uimmv_imm_width_eosz(X, kXed.OSZ_NONTERM_REFINING66_EOSZ)
#define XED_LF_UIMMv_IMM_WIDTH_OSZ_NONTERM_DF64_FORCE64_EOSZ_l2(X) \
  xed_set_uimmv_imm_width_eosz(X, kXed.OSZ_NONTERM_DF64_FORCE64_EOSZ)
#define XED_LF_UIMMv_IMM_WIDTH_OSZ_NONTERM_FORCE64_EOSZ_l2(X) \
  xed_set_uimmv_imm_width_eosz(X, kXed.OSZ_NONTERM_FORCE64_EOSZ)
#define XED_LF_UIMMv_IMM_WIDTH_OSZ_NONTERM_EOSZ_l2(X) \
  xed_set_uimmv_imm_width_eosz(X, kXed.OSZ_NONTERM_EOSZ)
#define XED_LF_UIMMv_IMM_WIDTH_OSZ_NONTERM_REFINING66_CR_WIDTH_EOSZ_l2(X) \
  xed_set_uimmv_imm_width_eosz(X, kXed.OSZ_NONTERM_REFINING66_CR_WIDTH_EOSZ)
#define XED_LF_UIMMv_IMM_WIDTH_OSZ_NONTERM_DF64_EOSZ_l2(X) \
  xed_set_uimmv_imm_width_eosz(X, kXed.OSZ_NONTERM_DF64_EOSZ)
#define XED_LF_UIMMv_IMM_WIDTH_OSZ_NONTERM_DF64_IMMUNE66_LOOP64_EOSZ_l2(X) \
  xed_set_uimmv_imm_width_eosz(X, kXed.OSZ_NONTERM_DF64_IMMUNE66_LOOP64_EOSZ)
#define XED_LF_UIMMv_IMM_WIDTH_OSZ_NONTERM_IMMUNE66_EOSZ_l2(X) \
  xed_set_uimmv_imm_width_eosz(X, kXed.OSZ_NONTERM_IMMUNE66_EOSZ)
#define XED_LF_UIMMv_IMM_WIDTH_OSZ_NONTERM_CR_WIDTH_EOSZ_l2(X) \
  xed_set_uimmv_imm_width_eosz(X, kXed.OSZ_NONTERM_CR_WIDTH_EOSZ)
#define XED_LF_UIMMv_IMM_WIDTH_OSZ_NONTERM_IMMUNE_REXW_EOSZ_l2(X) \
  xed_set_uimmv_imm_width_eosz(X, kXed.OSZ_NONTERM_IMMUNE_REXW_EOSZ)

extern const uint32_t xed_prefix_table_bit[8] hidden;
extern const uint8_t xed_imm_bits_2d[2][256] hidden;
extern const uint8_t xed_has_modrm_2d[XED_ILD_MAP2][256] hidden;
extern const uint8_t xed_has_sib_table[3][4][8] hidden;
extern const uint8_t xed_has_disp_regular[3][4][8] hidden;
extern const uint8_t xed_disp_bits_2d[XED_ILD_MAP2][256] hidden;

static const struct XedDenseMagnums {
  unsigned vex_prefix_recoding[4];
  xed_bits_t eamode_table[2][3];
  xed_bits_t BRDISPz_BRDISP_WIDTH[4];
  xed_bits_t MEMDISPv_DISP_WIDTH[4];
  xed_bits_t SIMMz_IMM_WIDTH[4];
  xed_bits_t UIMMv_IMM_WIDTH[4];
  xed_bits_t ASZ_NONTERM_EASZ[2][3];
  xed_bits_t OSZ_NONTERM_CR_WIDTH_EOSZ[2][2][3];
  xed_bits_t OSZ_NONTERM_DF64_EOSZ[2][2][3];
  xed_bits_t OSZ_NONTERM_DF64_FORCE64_EOSZ[2][2][3];
  xed_bits_t OSZ_NONTERM_DF64_IMMUNE66_LOOP64_EOSZ[2][2][3];
  xed_bits_t OSZ_NONTERM_EOSZ[2][2][3];
  xed_bits_t OSZ_NONTERM_FORCE64_EOSZ[2][2][3];
  xed_bits_t OSZ_NONTERM_IGNORE66_EOSZ[2][2][3];
  xed_bits_t OSZ_NONTERM_IMMUNE66_EOSZ[2][2][3];
  xed_bits_t OSZ_NONTERM_IMMUNE_REXW_EOSZ[2][2][3];
  xed_bits_t OSZ_NONTERM_REFINING66_CR_WIDTH_EOSZ[2][2][3];
  xed_bits_t OSZ_NONTERM_REFINING66_EOSZ[2][2][3];
} kXed = {
    .vex_prefix_recoding = {0, 1, 3, 2},
    .BRDISPz_BRDISP_WIDTH = {0x00, 0x10, 0x20, 0x20},
    .MEMDISPv_DISP_WIDTH = {0x00, 0x10, 0x20, 0x40},
    .SIMMz_IMM_WIDTH = {0x00, 0x10, 0x20, 0x20},
    .UIMMv_IMM_WIDTH = {0x00, 0x10, 0x20, 0x40},
    .ASZ_NONTERM_EASZ =
        {
            [0][0] = 0x1,
            [1][0] = 0x2,
            [0][1] = 0x2,
            [1][1] = 0x1,
            [0][2] = 0x3,
            [1][2] = 0x2,
        },
    .OSZ_NONTERM_CR_WIDTH_EOSZ =
        {
            [0][0][0] = 0x2,
            [1][0][0] = 0x2,
            [0][1][0] = 0x2,
            [1][1][0] = 0x2,
            [0][1][1] = 0x2,
            [1][1][1] = 0x2,
            [0][0][1] = 0x2,
            [1][0][1] = 0x2,
            [0][1][2] = 0x3,
            [0][0][2] = 0x3,
            [1][1][2] = 0x3,
            [1][0][2] = 0x3,
        },
    .OSZ_NONTERM_DF64_EOSZ =
        {
            [0][0][0] = 0x1,
            [1][0][0] = 0x1,
            [0][1][0] = 0x2,
            [1][1][0] = 0x2,
            [0][1][1] = 0x1,
            [1][1][1] = 0x1,
            [0][0][1] = 0x2,
            [1][0][1] = 0x2,
            [0][1][2] = 0x1,
            [0][0][2] = 0x3,
            [1][1][2] = 0x3,
            [1][0][2] = 0x3,
        },
    .OSZ_NONTERM_DF64_FORCE64_EOSZ =
        {
            [0][0][0] = 0x1,
            [1][0][0] = 0x1,
            [0][1][0] = 0x2,
            [1][1][0] = 0x2,
            [0][1][1] = 0x1,
            [1][1][1] = 0x1,
            [0][0][1] = 0x2,
            [1][0][1] = 0x2,
            [0][1][2] = 0x3,
            [0][0][2] = 0x3,
            [1][1][2] = 0x3,
            [1][0][2] = 0x3,
        },
    .OSZ_NONTERM_DF64_IMMUNE66_LOOP64_EOSZ =
        {
            [0][0][0] = 0x1,
            [1][0][0] = 0x1,
            [0][1][0] = 0x2,
            [1][1][0] = 0x2,
            [0][1][1] = 0x1,
            [1][1][1] = 0x1,
            [0][0][1] = 0x2,
            [1][0][1] = 0x2,
            [0][1][2] = 0x3,
            [0][0][2] = 0x3,
            [1][1][2] = 0x3,
            [1][0][2] = 0x3,
        },
    .OSZ_NONTERM_EOSZ =
        {
            [0][0][0] = 0x1,
            [1][0][0] = 0x1,
            [0][1][0] = 0x2,
            [1][1][0] = 0x2,
            [0][1][1] = 0x1,
            [1][1][1] = 0x1,
            [0][0][1] = 0x2,
            [1][0][1] = 0x2,
            [0][1][2] = 0x1,
            [0][0][2] = 0x2,
            [1][1][2] = 0x3,
            [1][0][2] = 0x3,
        },
    .OSZ_NONTERM_FORCE64_EOSZ =
        {
            [0][0][0] = 0x1,
            [1][0][0] = 0x1,
            [0][1][0] = 0x2,
            [1][1][0] = 0x2,
            [0][1][1] = 0x1,
            [1][1][1] = 0x1,
            [0][0][1] = 0x2,
            [1][0][1] = 0x2,
            [0][1][2] = 0x3,
            [0][0][2] = 0x3,
            [1][1][2] = 0x3,
            [1][0][2] = 0x3,
        },
    .OSZ_NONTERM_IGNORE66_EOSZ =
        {
            [0][0][0] = 0x1,
            [1][0][0] = 0x1,
            [0][1][0] = 0x1,
            [1][1][0] = 0x1,
            [0][1][1] = 0x2,
            [1][1][1] = 0x2,
            [0][0][1] = 0x2,
            [1][0][1] = 0x2,
            [0][1][2] = 0x2,
            [0][0][2] = 0x2,
            [1][1][2] = 0x3,
            [1][0][2] = 0x3,
        },
    .OSZ_NONTERM_IMMUNE66_EOSZ =
        {
            [0][0][0] = 0x2,
            [1][0][0] = 0x2,
            [0][1][0] = 0x2,
            [1][1][0] = 0x2,
            [0][1][1] = 0x2,
            [1][1][1] = 0x2,
            [0][0][1] = 0x2,
            [1][0][1] = 0x2,
            [0][1][2] = 0x2,
            [0][0][2] = 0x2,
            [1][1][2] = 0x3,
            [1][0][2] = 0x3,
        },
    .OSZ_NONTERM_IMMUNE_REXW_EOSZ =
        {
            [0][0][0] = 0x1,
            [1][0][0] = 0x1,
            [0][1][0] = 0x2,
            [1][1][0] = 0x2,
            [0][1][1] = 0x1,
            [1][1][1] = 0x1,
            [0][0][1] = 0x2,
            [1][0][1] = 0x2,
            [0][1][2] = 0x1,
            [0][0][2] = 0x2,
            [1][1][2] = 0x2,
            [1][0][2] = 0x2,
        },
    .OSZ_NONTERM_REFINING66_CR_WIDTH_EOSZ =
        {
            [0][0][0] = 0x2,
            [1][0][0] = 0x2,
            [0][1][0] = 0x2,
            [1][1][0] = 0x2,
            [0][1][1] = 0x2,
            [1][1][1] = 0x2,
            [0][0][1] = 0x2,
            [1][0][1] = 0x2,
            [0][1][2] = 0x3,
            [0][0][2] = 0x3,
            [1][1][2] = 0x3,
            [1][0][2] = 0x3,
        },
    .OSZ_NONTERM_REFINING66_EOSZ =
        {
            [0][0][0] = 0x1,
            [1][0][0] = 0x1,
            [0][1][0] = 0x1,
            [1][1][0] = 0x1,
            [0][1][1] = 0x2,
            [1][1][1] = 0x2,
            [0][0][1] = 0x2,
            [1][0][1] = 0x2,
            [0][1][2] = 0x2,
            [0][0][2] = 0x2,
            [1][1][2] = 0x3,
            [1][0][2] = 0x3,
        },
    .eamode_table =
        {
            [0][XED_MODE_LEGACY] = XED_MODE_LEGACY,
            [0][XED_MODE_LONG] = XED_MODE_LONG,
            [1][XED_MODE_REAL] = XED_MODE_LEGACY,
            [1][XED_MODE_LONG] = XED_MODE_LEGACY,
        },
};

privileged static void xed_too_short(struct XedDecodedInst *d) {
  d->operands.out_of_bytes = 1;
  if (d->operands.max_bytes >= 15) {
    d->operands.error = XED_ERROR_INSTR_TOO_LONG;
  } else {
    d->operands.error = XED_ERROR_BUFFER_TOO_SHORT;
  }
}

privileged static void xed_bad_map(struct XedDecodedInst *d) {
  d->operands.map = XED_ILD_MAP_INVALID;
  d->operands.error = XED_ERROR_BAD_MAP;
}

privileged static void xed_bad_v4(struct XedDecodedInst *d) {
  d->operands.error = XED_ERROR_BAD_EVEX_V_PRIME;
}

privileged static void xed_bad_z_aaa(struct XedDecodedInst *d) {
  d->operands.error = XED_ERROR_BAD_EVEX_Z_NO_MASKING;
}

privileged static xed_bits_t xed_get_prefix_table_bit(xed_bits_t a) {
  return (xed_prefix_table_bit[a >> 5] >> (a & 0x1F)) & 1;
}

privileged static size_t xed_bits2bytes(unsigned bits) {
  return bits >> 3;
}

privileged static size_t xed_bytes2bits(unsigned bytes) {
  return bytes << 3;
}

privileged static void xed_set_chip_modes(struct XedDecodedInst *d,
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
      d->operands.mode_first_prefix = 1;
      break;
    default:
      break;
  }
}

privileged static xed_bool_t xed3_mode_64b(struct XedDecodedInst *d) {
  return d->operands.mode == XED_MODE_LONG;
}

privileged static void xed_set_hint(char b, struct XedDecodedInst *d) {
  switch (b) {
    case 0x2e:
      d->operands.hint = XED_HINT_NTAKEN;
      return;
    case 0x3e:
      d->operands.hint = XED_HINT_TAKEN;
      return;
    default:
      break;
  }
}

privileged static void XED_LF_SIMM8_IMM_WIDTH_CONST_l2(
    struct XedDecodedInst *x) {
  x->operands.imm_width = 0x8;
}

privileged static void XED_LF_UIMM16_IMM_WIDTH_CONST_l2(
    struct XedDecodedInst *x) {
  x->operands.imm_width = 0x10;
}

privileged static void XED_LF_SE_IMM8_IMM_WIDTH_CONST_l2(
    struct XedDecodedInst *x) {
  x->operands.imm_width = 0x8;
}

privileged static void XED_LF_UIMM32_IMM_WIDTH_CONST_l2(
    struct XedDecodedInst *x) {
  x->operands.imm_width = 0x20;
}

privileged static void xed_set_simmz_imm_width_eosz(
    struct XedDecodedInst *x, const xed_bits_t eosz[2][2][3]) {
  x->operands.imm_width =
      kXed.SIMMz_IMM_WIDTH[eosz[x->operands.rexw][x->operands.osz]
                               [x->operands.mode]];
}

privileged static void xed_set_uimmv_imm_width_eosz(
    struct XedDecodedInst *x, const xed_bits_t eosz[2][2][3]) {
  x->operands.imm_width =
      kXed.UIMMv_IMM_WIDTH[eosz[x->operands.rexw][x->operands.osz]
                               [x->operands.mode]];
}

privileged static void XED_LF_UIMM8_IMM_WIDTH_CONST_l2(
    struct XedDecodedInst *x) {
  x->operands.imm_width = 0x8;
}
privileged static void XED_LF_0_IMM_WIDTH_CONST_l2(struct XedDecodedInst *x) {
  x->operands.imm_width = 0;
}

privileged static void XED_LF_RESOLVE_BYREG_IMM_WIDTH_map0x0_op0xc7_l1(
    struct XedDecodedInst *x) {
  switch (x->operands.reg) {
    case 0:
      XED_LF_SIMMz_IMM_WIDTH_OSZ_NONTERM_EOSZ_l2(x);
      break;
    case 7:
      XED_LF_0_IMM_WIDTH_CONST_l2(x);
      break;
    default:
      break;
  }
}

privileged static void XED_LF_RESOLVE_BYREG_IMM_WIDTH_map0x0_op0xf6_l1(
    struct XedDecodedInst *x) {
  if (x->operands.reg <= 1) {
    XED_LF_SIMM8_IMM_WIDTH_CONST_l2(x);
  } else if (2 <= x->operands.reg && x->operands.reg <= 7) {
    XED_LF_0_IMM_WIDTH_CONST_l2(x);
  }
}

privileged static void XED_LF_RESOLVE_BYREG_IMM_WIDTH_map0x0_op0xf7_l1(
    struct XedDecodedInst *x) {
  if (x->operands.reg <= 1) {
    XED_LF_SIMMz_IMM_WIDTH_OSZ_NONTERM_EOSZ_l2(x);
  } else if (2 <= x->operands.reg && x->operands.reg <= 7) {
    XED_LF_0_IMM_WIDTH_CONST_l2(x);
  }
}

privileged static void xed_ild_hasimm_map0x0F_op0x78_l1(
    struct XedDecodedInst *x) {
  if (x->operands.osz || x->operands.ild_f2) {
    x->operands.imm_width = xed_bytes2bits(1);
    x->operands.imm1_bytes = 1;
  }
}

privileged static void xed_ild_hasimm_map0x0_op0xc8_l1(
    struct XedDecodedInst *x) {
  x->operands.imm_width = xed_bytes2bits(2);
  x->operands.imm1_bytes = 1;
}

privileged static void xed_set_imm_bytes(struct XedDecodedInst *d) {
  if (!d->operands.imm_width && d->operands.map < XED_ILD_MAP2) {
    switch (xed_imm_bits_2d[d->operands.map][d->operands.nominal_opcode]) {
      case XED_I_LF_0_IMM_WIDTH_CONST_l2:
        XED_LF_0_IMM_WIDTH_CONST_l2(d);
        break;
      case XED_I_LF_RESOLVE_BYREG_IMM_WIDTH_map0x0_op0xc7_l1:
        XED_LF_RESOLVE_BYREG_IMM_WIDTH_map0x0_op0xc7_l1(d);
        break;
      case XED_I_LF_RESOLVE_BYREG_IMM_WIDTH_map0x0_op0xf6_l1:
        XED_LF_RESOLVE_BYREG_IMM_WIDTH_map0x0_op0xf6_l1(d);
        break;
      case XED_I_LF_RESOLVE_BYREG_IMM_WIDTH_map0x0_op0xf7_l1:
        XED_LF_RESOLVE_BYREG_IMM_WIDTH_map0x0_op0xf7_l1(d);
        break;
      case XED_I_LF_SIMM8_IMM_WIDTH_CONST_l2:
        XED_LF_SIMM8_IMM_WIDTH_CONST_l2(d);
        break;
      case XED_I_LF_SIMMz_IMM_WIDTH_OSZ_NONTERM_DF64_EOSZ_l2:
        XED_LF_SIMMz_IMM_WIDTH_OSZ_NONTERM_DF64_EOSZ_l2(d);
        break;
      case XED_I_LF_SIMMz_IMM_WIDTH_OSZ_NONTERM_EOSZ_l2:
        XED_LF_SIMMz_IMM_WIDTH_OSZ_NONTERM_EOSZ_l2(d);
        break;
      case XED_I_LF_UIMM16_IMM_WIDTH_CONST_l2:
        XED_LF_UIMM16_IMM_WIDTH_CONST_l2(d);
        break;
      case XED_I_LF_UIMM8_IMM_WIDTH_CONST_l2:
        XED_LF_UIMM8_IMM_WIDTH_CONST_l2(d);
        break;
      case XED_I_LF_UIMMv_IMM_WIDTH_OSZ_NONTERM_EOSZ_l2:
        XED_LF_UIMMv_IMM_WIDTH_OSZ_NONTERM_EOSZ_l2(d);
        break;
      case xed_i_ild_hasimm_map0x0_op0xc8_l1:
        xed_ild_hasimm_map0x0_op0xc8_l1(d);
        break;
      case xed_i_ild_hasimm_map0x0F_op0x78_l1:
        xed_ild_hasimm_map0x0F_op0x78_l1(d);
        break;
      default:
        d->operands.error = XED_ERROR_GENERAL_ERROR;
        return;
    }
  }
}

privileged static int xed_consume_byte(struct XedDecodedInst *d) {
  if (d->decoded_length < d->operands.max_bytes) {
    return xed_decoded_inst_get_byte(d, d->decoded_length++);
  } else {
    xed_too_short(d);
    return -1;
  }
}

privileged static void xed_prefix_scanner(struct XedDecodedInst *d) {
  xed_bits_t b, max_bytes, length, nprefixes, nseg_prefixes, nrexes, rex;
  length = d->decoded_length;
  max_bytes = d->operands.max_bytes;
  rex = nrexes = nprefixes = nseg_prefixes = 0;
  while (length < max_bytes) {
    b = xed_decoded_inst_get_byte(d, length);
    if (xed_get_prefix_table_bit(b) == 0) goto out;
    switch (b) {
      case 0x66:
        d->operands.osz = 1;
        d->operands.prefix66 = 1;
        rex = 0;
        break;
      case 0x67:
        d->operands.asz = 1;
        rex = 0;
        break;
      case 0x2E:
      case 0x3E:
        xed_set_hint(b, d);
        /* fallthrough */
      case 0x26:
      case 0x36:
        if (!xed3_mode_64b(d)) {
          d->operands.ild_seg = b;
        }
        nseg_prefixes++;
        rex = 0;
        break;
      case 0x64:
      case 0x65:
        d->operands.ild_seg = b;
        nseg_prefixes++;
        rex = 0;
        break;
      case 0xF0:
        d->operands.lock = 1;
        rex = 0;
        break;
      case 0xF3:
        d->operands.ild_f3 = 1;
        d->operands.last_f2f3 = 3;
        if (!d->operands.first_f2f3) {
          d->operands.first_f2f3 = 3;
        }
        rex = 0;
        break;
      case 0xF2:
        d->operands.ild_f2 = 1;
        d->operands.last_f2f3 = 2;
        if (!d->operands.first_f2f3) {
          d->operands.first_f2f3 = 2;
        }
        rex = 0;
        break;
      default:
        if (xed3_mode_64b(d) && (b & 0xf0) == 0x40) {
          nrexes++;
          rex = b;
          break;
        } else {
          goto out;
        }
    }
    length++;
    nprefixes++;
  }
out:
  d->decoded_length = length;
  d->operands.nprefixes = nprefixes;
  d->operands.nseg_prefixes = nseg_prefixes;
  d->operands.nrexes = nrexes;
  if (rex) {
    d->operands.rexw = rex >> 3 & 1;
    d->operands.rexr = rex >> 2 & 1;
    d->operands.rexx = rex >> 1 & 1;
    d->operands.rexb = rex & 1;
    d->operands.rex = 1;
  }
  if (d->operands.mode_first_prefix) {
    d->operands.rep = d->operands.first_f2f3;
  } else {
    d->operands.rep = d->operands.last_f2f3;
  }
  switch (d->operands.ild_seg) {
    case 0x2e:
      d->operands.seg_ovd = 1;
      break;
    case 0x3e:
      d->operands.seg_ovd = 2;
      break;
    case 0x26:
      d->operands.seg_ovd = 3;
      break;
    case 0x64:
      d->operands.seg_ovd = 4;
      break;
    case 0x65:
      d->operands.seg_ovd = 5;
      break;
    case 0x36:
      d->operands.seg_ovd = 6;
      break;
    default:
      break;
  }
  if (length >= max_bytes) {
    xed_too_short(d);
    return;
  }
}

privileged static void xed_get_next_as_opcode(struct XedDecodedInst *d) {
  xed_bits_t b, length;
  length = d->decoded_length;
  if (length < d->operands.max_bytes) {
    b = xed_decoded_inst_get_byte(d, length);
    d->operands.nominal_opcode = b;
    d->decoded_length++;
    d->operands.srm = xed_modrm_rm(b);
  } else {
    xed_too_short(d);
  }
}

privileged static void xed_catch_invalid_rex_or_legacy_prefixes(
    struct XedDecodedInst *d) {
  if (xed3_mode_64b(d) && d->operands.rex) {
    d->operands.error = XED_ERROR_BAD_REX_PREFIX;
  } else if (d->operands.osz || d->operands.ild_f3 || d->operands.ild_f2) {
    d->operands.error = XED_ERROR_BAD_LEGACY_PREFIX;
  }
}

privileged static void xed_catch_invalid_mode(struct XedDecodedInst *d) {
  if (d->operands.realmode) {
    d->operands.error = XED_ERROR_INVALID_MODE;
  }
}

privileged static void xed_evex_vex_opcode_scanner(struct XedDecodedInst *d) {
  d->operands.nominal_opcode = xed_decoded_inst_get_byte(d, d->decoded_length);
  d->operands.pos_nominal_opcode = d->decoded_length++;
  xed_catch_invalid_rex_or_legacy_prefixes(d);
  xed_catch_invalid_mode(d);
}

privileged static void xed_opcode_scanner(struct XedDecodedInst *d) {
  xed_bits_t b, length;
  length = d->decoded_length;
  if ((b = xed_decoded_inst_get_byte(d, length)) != 0x0F) {
    d->operands.map = XED_ILD_MAP0;
    d->operands.nominal_opcode = b;
    d->operands.pos_nominal_opcode = length;
    d->decoded_length++;
  } else {
    length++;
    d->operands.pos_nominal_opcode = length;
    if (length < d->operands.max_bytes) {
      switch ((b = xed_decoded_inst_get_byte(d, length))) {
        case 0x38:
          length++;
          d->operands.map = XED_ILD_MAP2;
          d->decoded_length = length;
          xed_get_next_as_opcode(d);
          return;
        case 0x3A:
          length++;
          d->operands.map = XED_ILD_MAP3;
          d->decoded_length = length;
          d->operands.imm_width = xed_bytes2bits(1);
          xed_get_next_as_opcode(d);
          return;
        case 0x3B:
          length++;
          xed_bad_map(d);
          d->decoded_length = length;
          xed_get_next_as_opcode(d);
          return;
        case 0x39:
        case 0x3C:
        case 0x3D:
        case 0x3E:
        case 0x3F:
          length++;
          xed_bad_map(d);
          d->decoded_length = length;
          xed_get_next_as_opcode(d);
          return;
        case 0x0F:
          d->operands.amd3dnow = 1;
          length++;
          d->operands.nominal_opcode = 0x0F;
          d->operands.map = XED_ILD_MAPAMD;
          d->decoded_length = length;
          break;
        default:
          length++;
          d->operands.nominal_opcode = b;
          d->operands.map = XED_ILD_MAP1;
          d->decoded_length = length;
          break;
      }
    } else {
      xed_too_short(d);
      return;
    }
  }
  d->operands.srm = xed_modrm_rm(d->operands.nominal_opcode);
}

privileged static bool xed_is_bound_instruction(struct XedDecodedInst *d) {
  return !xed3_mode_64b(d) && d->decoded_length + 1 < d->operands.max_bytes &&
         (xed_decoded_inst_get_byte(d, d->decoded_length + 1) & 0xC0) != 0xC0;
}

privileged static void xed_evex_scanner(struct XedDecodedInst *d) {
  xed_bits_t length, max_bytes;
  union XedAvx512Payload1 evex1;
  union XedAvx512Payload2 evex2;
  union XedAvx512Payload3 evex3;
  length = d->decoded_length;
  max_bytes = d->operands.max_bytes;
  /* @assume prefix_scanner() checked length */
  if (xed_decoded_inst_get_byte(d, length) != 0x62) return;
  if (xed_is_bound_instruction(d)) return;
  if (length + 4 < max_bytes) {
    evex1.u32 = xed_decoded_inst_get_byte(d, length + 1);
    evex2.u32 = xed_decoded_inst_get_byte(d, length + 2);
    if (xed3_mode_64b(d)) {
      d->operands.rexr = ~evex1.s.r_inv & 1;
      d->operands.rexx = ~evex1.s.x_inv & 1;
      d->operands.rexb = ~evex1.s.b_inv & 1;
      d->operands.rexrr = ~evex1.s.rr_inv & 1;
    }
    d->operands.map = evex1.s.map;
    d->operands.rexw = evex2.s.rexw & 1;
    d->operands.vexdest3 = evex2.s.vexdest3;
    d->operands.vexdest210 = evex2.s.vexdest210;
    d->operands.ubit = evex2.s.ubit;
    if (evex2.s.ubit) {
      d->operands.vexvalid = 2;
    } else {
      d->operands.error = XED_ERROR_BAD_EVEX_UBIT;
    }
    d->operands.vex_prefix = kXed.vex_prefix_recoding[evex2.s.pp];
    if (evex1.s.map == XED_ILD_MAP3) {
      d->operands.imm_width = xed_bytes2bits(1);
    }
    if (evex2.s.ubit) {
      evex3.u32 = xed_decoded_inst_get_byte(d, length + 3);
      d->operands.zeroing = evex3.s.z;
      d->operands.llrc = evex3.s.llrc;
      d->operands.vl = evex3.s.llrc;
      d->operands.bcrc = evex3.s.bcrc;
      d->operands.vexdest4 = ~evex3.s.vexdest4p & 1;
      if (!xed3_mode_64b(d) && evex3.s.vexdest4p == 0) {
        xed_bad_v4(d);
      }
      d->operands.mask = evex3.s.mask;
      if (evex3.s.mask == 0 && evex3.s.z == 1) {
        xed_bad_z_aaa(d);
      }
    }
    length += 4;
    d->decoded_length = length;
    xed_evex_vex_opcode_scanner(d);
  } else {
    xed_too_short(d);
  }
}

privileged static void xed_evex_imm_scanner(struct XedDecodedInst *d) {
  unsigned pos_imm;
  uint8_t uimm0, esrc;
  const uint8_t *itext, *imm_ptr;
  xed_bits_t length, imm_bytes, imm1_bytes, max_bytes;
  pos_imm = 0;
  imm_ptr = 0;
  itext = d->bytes;
  xed_set_imm_bytes(d);
  length = d->decoded_length;
  max_bytes = d->operands.max_bytes;
  if (d->operands.amd3dnow) {
    if (length < max_bytes) {
      d->operands.nominal_opcode = xed_decoded_inst_get_byte(d, length);
      d->decoded_length++;
      return;
    } else {
      xed_too_short(d);
      return;
    }
  }
  imm_bytes = xed_bits2bytes(d->operands.imm_width);
  imm1_bytes = d->operands.imm1_bytes;
  if (imm_bytes) {
    if (length + imm_bytes <= max_bytes) {
      d->operands.pos_imm = length;
      length += imm_bytes;
      d->decoded_length = length;
      if (imm1_bytes) {
        if (length + imm1_bytes <= max_bytes) {
          d->operands.pos_imm1 = length;
          imm_ptr = itext + length;
          length += imm1_bytes;
          d->decoded_length = length;
          d->operands.uimm1 = *imm_ptr;
        } else {
          xed_too_short(d);
          return;
        }
      }
    } else {
      xed_too_short(d);
      return;
    }
  }
  pos_imm = d->operands.pos_imm;
  imm_ptr = itext + pos_imm;
  switch (imm_bytes) {
    case 0:
      break;
    case 1:
      uimm0 = *imm_ptr;
      esrc = uimm0 >> 4;
      d->operands.uimm0 = uimm0;
      d->operands.esrc = esrc;
      break;
    case 2:
      d->operands.uimm0 = READ16LE(imm_ptr);
      break;
    case 4:
      d->operands.uimm0 = READ32LE(imm_ptr);
      break;
    case 8:
      d->operands.uimm0 = READ64LE(imm_ptr);
      break;
    default:
      unreachable;
  }
}

privileged static void xed_vex_c4_scanner(struct XedDecodedInst *d) {
  uint8_t n;
  xed_bits_t length, max_bytes;
  union XedAvxC4Payload1 c4byte1;
  union XedAvxC4Payload2 c4byte2;
  if (xed_is_bound_instruction(d)) return;
  length = d->decoded_length;
  max_bytes = d->operands.max_bytes;
  length++;
  if (length + 2 < max_bytes) {
    c4byte1.u32 = xed_decoded_inst_get_byte(d, length);
    c4byte2.u32 = xed_decoded_inst_get_byte(d, length + 1);
    d->operands.rexr = ~c4byte1.s.r_inv & 1;
    d->operands.rexx = ~c4byte1.s.x_inv & 1;
    d->operands.rexb = (xed3_mode_64b(d) & ~c4byte1.s.b_inv) & 1;
    d->operands.rexw = c4byte2.s.w & 1;
    d->operands.vexdest3 = c4byte2.s.v3;
    d->operands.vexdest210 = c4byte2.s.vvv210;
    d->operands.vl = c4byte2.s.l;
    d->operands.vex_prefix = kXed.vex_prefix_recoding[c4byte2.s.pp];
    d->operands.map = c4byte1.s.map;
    if ((c4byte1.s.map & 0x3) == XED_ILD_MAP3) {
      d->operands.imm_width = xed_bytes2bits(1);
    }
    d->operands.vexvalid = 1;
    length += 2;
    d->decoded_length = length;
    xed_evex_vex_opcode_scanner(d);
  } else {
    d->decoded_length = length;
    xed_too_short(d);
  }
}

privileged static void xed_vex_c5_scanner(struct XedDecodedInst *d) {
  xed_bits_t max_bytes, length;
  union XedAvxC5Payload c5byte1;
  length = d->decoded_length;
  max_bytes = d->operands.max_bytes;
  if (xed_is_bound_instruction(d)) return;
  length++;
  if (length + 1 < max_bytes) {
    c5byte1.u32 = xed_decoded_inst_get_byte(d, length);
    d->operands.rexr = ~c5byte1.s.r_inv & 1;
    d->operands.vexdest3 = c5byte1.s.v3;
    d->operands.vexdest210 = c5byte1.s.vvv210;
    d->operands.vl = c5byte1.s.l;
    d->operands.vex_prefix = kXed.vex_prefix_recoding[c5byte1.s.pp];
    d->operands.map = XED_ILD_MAP1;
    d->operands.vexvalid = 1;
    length++;
    d->decoded_length = length;
    xed_evex_vex_opcode_scanner(d);
  } else {
    d->decoded_length = length;
    xed_too_short(d);
  }
}

privileged static void xed_xop_scanner(struct XedDecodedInst *d) {
  union XedAvxC4Payload1 xop_byte1;
  union XedAvxC4Payload2 xop_byte2;
  xed_bits_t map, max_bytes, length;
  length = d->decoded_length;
  max_bytes = d->operands.max_bytes;
  if (xed_is_bound_instruction(d)) return;
  length++;
  if (length + 2 < max_bytes) {
    xop_byte1.u32 = xed_decoded_inst_get_byte(d, length);
    xop_byte2.u32 = xed_decoded_inst_get_byte(d, length + 1);
    map = xop_byte1.s.map;
    if (map == 0x9) {
      d->operands.map = XED_ILD_MAP_XOP9;
      d->operands.imm_width = 0;
    } else if (map == 0x8) {
      d->operands.map = XED_ILD_MAP_XOP8;
      d->operands.imm_width = xed_bytes2bits(1);
    } else if (map == 0xA) {
      d->operands.map = XED_ILD_MAP_XOPA;
      d->operands.imm_width = xed_bytes2bits(4);
    } else {
      xed_bad_map(d);
    }
    d->operands.rexr = ~xop_byte1.s.r_inv & 1;
    d->operands.rexx = ~xop_byte1.s.x_inv & 1;
    d->operands.rexb = (xed3_mode_64b(d) & ~xop_byte1.s.b_inv) & 1;
    d->operands.rexw = xop_byte2.s.w & 1;
    d->operands.vexdest3 = xop_byte2.s.v3;
    d->operands.vexdest210 = xop_byte2.s.vvv210;
    d->operands.vl = xop_byte2.s.l;
    d->operands.vex_prefix = kXed.vex_prefix_recoding[xop_byte2.s.pp];
    d->operands.vexvalid = 3;
    length += 2;
    d->decoded_length = length;
    xed_evex_vex_opcode_scanner(d);
  } else {
    d->decoded_length = length;
    xed_too_short(d);
  }
}

privileged static xed_uint_t xed_chip_is_intel_specific(
    struct XedDecodedInst *d) {
  switch (d->operands.chip) {
    case XED_CHIP_INVALID:
    case XED_CHIP_ALL:
    case XED_CHIP_AMD:
      return 0;
    default:
      return 1;
  }
}

privileged static void xed_vex_scanner(struct XedDecodedInst *d) {
  if (!d->operands.out_of_bytes) {
    switch (xed_decoded_inst_get_byte(d, d->decoded_length)) {
      case 0xC5:
        xed_vex_c5_scanner(d);
        break;
      case 0xC4:
        xed_vex_c4_scanner(d);
        break;
      case 0x8F:
        if (!xed_chip_is_intel_specific(d)) {
          xed_xop_scanner(d);
        }
        break;
      default:
        break;
    }
  }
}

privileged static void xed_bad_ll(struct XedDecodedInst *d) {
  d->operands.error = XED_ERROR_BAD_EVEX_LL;
}

privileged static void xed_bad_ll_check(struct XedDecodedInst *d) {
  if (d->operands.llrc == 3) {
    if (d->operands.mod != 3) {
      xed_bad_ll(d);
    } else if (d->operands.bcrc == 0) {
      xed_bad_ll(d);
    }
  }
}

privileged static void xed_set_has_modrm(struct XedDecodedInst *d) {
  d->operands.has_modrm =
      d->operands.map >= XED_ILD_MAP2 ||
      xed_has_modrm_2d[d->operands.map][d->operands.nominal_opcode];
}

privileged static void xed_modrm_scanner(struct XedDecodedInst *d) {
  uint8_t b;
  xed_bits_t eamode, mod, rm, asz, mode, length, has_modrm;
  xed_set_has_modrm(d);
  has_modrm = d->operands.has_modrm;
  if (has_modrm) {
    length = d->decoded_length;
    if (length < d->operands.max_bytes) {
      b = xed_decoded_inst_get_byte(d, length);
      d->operands.modrm = b;
      d->operands.pos_modrm = length;
      d->decoded_length++;
      mod = xed_modrm_mod(b);
      rm = xed_modrm_rm(b);
      d->operands.mod = mod;
      d->operands.rm = rm;
      d->operands.reg = xed_modrm_reg(b);
      xed_bad_ll_check(d);
      if (has_modrm != XED_ILD_HASMODRM_IGNORE_MOD) {
        asz = d->operands.asz;
        mode = d->operands.mode;
        eamode = kXed.eamode_table[asz][mode];
        d->operands.disp_width =
            xed_bytes2bits(xed_has_disp_regular[eamode][mod][rm]);
        d->operands.has_sib = xed_has_sib_table[eamode][mod][rm];
      }
    } else {
      xed_too_short(d);
    }
  }
}

privileged static void xed_sib_scanner(struct XedDecodedInst *d) {
  uint8_t b;
  xed_bits_t length;
  if (d->operands.has_sib) {
    length = d->decoded_length;
    if (length < d->operands.max_bytes) {
      b = xed_decoded_inst_get_byte(d, length);
      d->operands.pos_sib = length;
      d->operands.sib = b;
      d->decoded_length++;
      if (xed_sib_base(b) == 5) {
        if (d->operands.mod == 0) {
          d->operands.disp_width = xed_bytes2bits(4);
        }
      }
    } else {
      xed_too_short(d);
    }
  }
}

privileged static void XED_LF_EMPTY_DISP_CONST_l2(struct XedDecodedInst *x) {
  /* This function does nothing for map-opcodes whose
   * disp_bytes value is set earlier in xed-ild.c
   * (regular displacement resolution by modrm/sib)*/
  (void)x;
}

privileged static void XED_LF_BRDISP8_BRDISP_WIDTH_CONST_l2(
    struct XedDecodedInst *x) {
  x->operands.disp_width = 0x8;
}

privileged static void XED_LF_BRDISPz_BRDISP_WIDTH_OSZ_NONTERM_EOSZ_l2(
    struct XedDecodedInst *x) {
  x->operands.disp_width =
      kXed.BRDISPz_BRDISP_WIDTH[kXed.OSZ_NONTERM_EOSZ[x->operands.rexw]
                                                     [x->operands.osz]
                                                     [x->operands.mode]];
}

privileged static void XED_LF_RESOLVE_BYREG_DISP_map0x0_op0xc7_l1(
    struct XedDecodedInst *x) {
  switch (x->operands.reg) {
    case 0:
      XED_LF_EMPTY_DISP_CONST_l2(x);
      break;
    case 7:
      XED_LF_BRDISPz_BRDISP_WIDTH_OSZ_NONTERM_EOSZ_l2(x);
      break;
    default:
      break;
  }
}

privileged static void XED_LF_MEMDISPv_DISP_WIDTH_ASZ_NONTERM_EASZ_l2(
    struct XedDecodedInst *x) {
  x->operands.disp_width =
      kXed.MEMDISPv_DISP_WIDTH[kXed.ASZ_NONTERM_EASZ[x->operands.asz]
                                                    [x->operands.mode]];
}

privileged static void XED_LF_BRDISP32_BRDISP_WIDTH_CONST_l2(
    struct XedDecodedInst *x) {
  x->operands.disp_width = 0x20;
}

privileged static void XED_LF_DISP_BUCKET_0_l1(struct XedDecodedInst *x) {
  if (x->operands.mode <= XED_MODE_LEGACY) {
    XED_LF_BRDISPz_BRDISP_WIDTH_OSZ_NONTERM_EOSZ_l2(x);
  } else if (x->operands.mode == XED_MODE_LONG) {
    XED_LF_BRDISP32_BRDISP_WIDTH_CONST_l2(x);
  }
}

privileged static void xed_disp_scanner(struct XedDecodedInst *d) {
  uint8_t *disp_ptr;
  xed_bits_t length, disp_width, disp_bytes, max_bytes;
  const xed_bits_t ilog2[] = {99, 0, 1, 99, 2, 99, 99, 99, 3};
  length = d->decoded_length;
  if (d->operands.map < XED_ILD_MAP2) {
    switch (xed_disp_bits_2d[d->operands.map][d->operands.nominal_opcode]) {
      case XED_I_LF_BRDISP8_BRDISP_WIDTH_CONST_l2:
        XED_LF_BRDISP8_BRDISP_WIDTH_CONST_l2(d);
        break;
      case XED_I_LF_BRDISPz_BRDISP_WIDTH_OSZ_NONTERM_EOSZ_l2:
        XED_LF_BRDISPz_BRDISP_WIDTH_OSZ_NONTERM_EOSZ_l2(d);
        break;
      case XED_I_LF_DISP_BUCKET_0_l1:
        XED_LF_DISP_BUCKET_0_l1(d);
        break;
      case XED_I_LF_EMPTY_DISP_CONST_l2:
        XED_LF_EMPTY_DISP_CONST_l2(d);
        break;
      case XED_I_LF_MEMDISPv_DISP_WIDTH_ASZ_NONTERM_EASZ_l2:
        XED_LF_MEMDISPv_DISP_WIDTH_ASZ_NONTERM_EASZ_l2(d);
        break;
      case XED_I_LF_RESOLVE_BYREG_DISP_map0x0_op0xc7_l1:
        XED_LF_RESOLVE_BYREG_DISP_map0x0_op0xc7_l1(d);
        break;
      default:
        d->operands.error = XED_ERROR_GENERAL_ERROR;
        return;
    }
  }
  disp_bytes = xed_bits2bytes(d->operands.disp_width);
  if (disp_bytes) {
    max_bytes = d->operands.max_bytes;
    if (length + disp_bytes <= max_bytes) {
      disp_ptr = d->bytes + length;
      switch (ilog2[disp_bytes]) {
        case 0:
          d->operands.disp = *(int8_t *)disp_ptr;
          break;
        case 1:
          d->operands.disp = (int16_t)READ16LE(disp_ptr);
          break;
        case 2:
          d->operands.disp = (int32_t)READ32LE(disp_ptr);
          break;
        case 3:
          d->operands.disp = (int64_t)READ64LE(disp_ptr);
          break;
        default:
          break;
      }
      d->operands.pos_disp = length;
      d->decoded_length = length + disp_bytes;
    } else {
      xed_too_short(d);
    }
  }
}

privileged static void xed_decode_instruction_length(
    struct XedDecodedInst *ild) {
  xed_prefix_scanner(ild);
  if (!ild->operands.out_of_bytes) {
    xed_vex_scanner(ild);
    if (!ild->operands.out_of_bytes) {
      if (!ild->operands.vexvalid) xed_evex_scanner(ild);
      if (!ild->operands.out_of_bytes) {
        if (!ild->operands.vexvalid && !ild->operands.error) {
          xed_opcode_scanner(ild);
        }
        xed_modrm_scanner(ild);
        xed_sib_scanner(ild);
        xed_disp_scanner(ild);
        xed_evex_imm_scanner(ild);
      }
    }
  }
}

/**
 * Decodes machine instruction length.
 *
 * This function also decodes other useful attributes, such as the
 * offsets of displacement, immediates, etc. It works for all ISAs from
 * 1977 to 2020.
 *
 * @return d->decoded_length is byte length, or 1 w/ error code
 * @note binary footprint increases ~4kb if this is used
 * @see biggest code in gdb/clang/tensorflow binaries
 */
privileged enum XedError xed_instruction_length_decode(
    struct XedDecodedInst *xedd, const void *itext, size_t bytes) {
  xed_set_chip_modes(xedd, xedd->operands.chip);
  xedd->bytes = itext;
  xedd->operands.max_bytes = MIN(bytes, XED_MAX_INSTRUCTION_BYTES);
  xed_decode_instruction_length(xedd);
  if (!xedd->operands.out_of_bytes) {
    if (xedd->operands.map != XED_ILD_MAP_INVALID) {
      return xedd->operands.error;
    } else {
      return XED_ERROR_GENERAL_ERROR;
    }
  } else {
    return XED_ERROR_BUFFER_TOO_SHORT;
  }
}
