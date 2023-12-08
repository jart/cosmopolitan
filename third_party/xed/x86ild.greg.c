/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/dce.h"
#include "libc/serialize.h"
#include "libc/intrin/bsr.h"
#include "libc/log/libfatal.internal.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
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

static const uint32_t xed_prefix_table_bit[8] = {
    0x00000000, 0x40404040, 0x0000ffff, 0x000000f0,
    0x00000000, 0x00000000, 0x00000000, 0x000d0000,
};

static const uint8_t xed_imm_bits_2d[2][256] = {
    {1, 1, 1,  1, 5, 7, 1, 1, 1,  1,  1,  1,  9,  7,  1,  0,  1, 1, 1, 1, 5, 7,
     1, 1, 1,  1, 1, 1, 5, 7, 1,  1,  1,  1,  1,  1,  5,  7,  0, 1, 1, 1, 1, 1,
     5, 7, 0,  1, 1, 1, 1, 1, 9,  7,  0,  1,  1,  1,  1,  1,  5, 7, 0, 1, 1, 1,
     1, 1, 1,  1, 1, 1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1,
     1, 1, 1,  1, 1, 1, 1, 1, 1,  1,  1,  1,  0,  0,  0,  0,  6, 7, 5, 5, 1, 1,
     1, 1, 1,  1, 1, 1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 5, 7, 5, 5,
     1, 1, 1,  1, 1, 1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1,
     8, 1, 1,  1, 1, 1, 1, 1, 1,  1,  1,  1,  1,  1,  5,  7,  1, 1, 1, 1, 1, 1,
     9, 9, 9,  9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 9, 9, 8, 1, 1, 1,
     9, 2, 11, 1, 8, 1, 1, 9, 1,  1,  1,  1,  1,  1,  9,  9,  1, 1, 1, 1, 1, 1,
     1, 1, 1,  1, 1, 1, 1, 1, 9,  9,  9,  9,  1,  1,  8,  1,  1, 1, 1, 1, 0, 1,
     0, 0, 1,  1, 3, 4, 1, 1, 1,  1,  1,  1,  1,  1},
    {1,  1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1,
     1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
     1,  1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
     1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 1, 1, 1, 1,
     12, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 1, 0, 0,
     1,  1, 1, 1, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 1, 1, 1, 1, 1,
     1,  1, 9, 1, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

static const uint8_t xed_has_modrm_2d[XED_ILD_MAP2][256] = {
    {1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 3, 1, 1, 1, 1, 0, 0, 0, 0,
     1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 3, 0, 1, 1, 1, 1, 0, 0, 3, 0,
     1, 1, 1, 1, 0, 0, 3, 0, 1, 1, 1, 1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 1, 1, 3, 3, 3, 3, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
     1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     3, 0, 3, 3, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 1, 1, 3, 0, 0, 0, 0, 0, 3, 0, 3, 1, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1,
     0, 0, 0, 0, 0, 0, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
     1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 3, 3,
     0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

static const uint8_t xed_has_sib_table[3][4][8] = {
    {{0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0}},
    {{0, 0, 0, 0, 1, 0, 0, 0},
     {0, 0, 0, 0, 1, 0, 0, 0},
     {0, 0, 0, 0, 1, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0}},
    {{0, 0, 0, 0, 1, 0, 0, 0},
     {0, 0, 0, 0, 1, 0, 0, 0},
     {0, 0, 0, 0, 1, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0}},
};

static const uint8_t xed_has_disp_regular[3][4][8] = {
    {{0, 0, 0, 0, 0, 0, 2, 0},
     {1, 1, 1, 1, 1, 1, 1, 1},
     {2, 2, 2, 2, 2, 2, 2, 2},
     {0, 0, 0, 0, 0, 0, 0, 0}},
    {{0, 0, 0, 0, 0, 4, 0, 0},
     {1, 1, 1, 1, 1, 1, 1, 1},
     {4, 4, 4, 4, 4, 4, 4, 4},
     {0, 0, 0, 0, 0, 0, 0, 0}},
    {{0, 0, 0, 0, 0, 4, 0, 0},
     {1, 1, 1, 1, 1, 1, 1, 1},
     {4, 4, 4, 4, 4, 4, 4, 4},
     {0, 0, 0, 0, 0, 0, 0, 0}},
};

static const uint8_t xed_disp_bits_2d[XED_ILD_MAP2][256] = {
    {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 4, 4, 4, 4, 4, 4, 4, 4,
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 4, 4, 4, 4, 4, 4, 4, 0, 4,
     4, 4, 4, 4, 4, 4, 0, 4, 4, 4, 4, 4, 4, 4, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4,
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
     4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 5, 5, 5, 5, 4, 4, 4, 4,
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
     4, 4, 4, 4, 4, 4, 4, 6, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
     4, 4, 4, 4, 4, 4, 4, 4, 1, 1, 1, 1, 4, 4, 4, 4, 3, 3, 2, 1, 4, 4, 4, 4,
     0, 4, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4},
    {4, 4, 4, 4, 0, 4, 4, 4, 4, 4, 0, 4, 0, 4, 4, 0, 4, 4, 4, 4, 4, 4, 4, 4,
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4,
     4, 4, 4, 4, 4, 4, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4,
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
     4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0,
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4},
};

static const struct XedDenseMagnums {
  unsigned vex_prefix_recoding[4];
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
    .BRDISPz_BRDISP_WIDTH = {0, 16, 32, 32},
    .MEMDISPv_DISP_WIDTH = {0, 16, 32, 64},
    .SIMMz_IMM_WIDTH = {0x00, 0x10, 0x20, 0x20},
    .UIMMv_IMM_WIDTH = {0x00, 0x10, 0x20, 0x40},
    .ASZ_NONTERM_EASZ =
        {
            [0][0] = 1,
            [1][0] = 2,
            [0][1] = 2,
            [1][1] = 1,
            [0][2] = 3,
            [1][2] = 2,
        },
    .OSZ_NONTERM_CR_WIDTH_EOSZ =
        {
            [0][0][0] = 2,
            [1][0][0] = 2,
            [0][1][0] = 2,
            [1][1][0] = 2,
            [0][1][1] = 2,
            [1][1][1] = 2,
            [0][0][1] = 2,
            [1][0][1] = 2,
            [0][1][2] = 3,
            [0][0][2] = 3,
            [1][1][2] = 3,
            [1][0][2] = 3,
        },
    .OSZ_NONTERM_DF64_EOSZ =
        {
            [0][0][0] = 1,
            [1][0][0] = 1,
            [0][1][0] = 2,
            [1][1][0] = 2,
            [0][1][1] = 1,
            [1][1][1] = 1,
            [0][0][1] = 2,
            [1][0][1] = 2,
            [0][1][2] = 1,
            [0][0][2] = 3,
            [1][1][2] = 3,
            [1][0][2] = 3,
        },
    .OSZ_NONTERM_DF64_FORCE64_EOSZ =
        {
            [0][0][0] = 1,
            [1][0][0] = 1,
            [0][1][0] = 2,
            [1][1][0] = 2,
            [0][1][1] = 1,
            [1][1][1] = 1,
            [0][0][1] = 2,
            [1][0][1] = 2,
            [0][1][2] = 3,
            [0][0][2] = 3,
            [1][1][2] = 3,
            [1][0][2] = 3,
        },
    .OSZ_NONTERM_DF64_IMMUNE66_LOOP64_EOSZ =
        {
            [0][0][0] = 1,
            [1][0][0] = 1,
            [0][1][0] = 2,
            [1][1][0] = 2,
            [0][1][1] = 1,
            [1][1][1] = 1,
            [0][0][1] = 2,
            [1][0][1] = 2,
            [0][1][2] = 3,
            [0][0][2] = 3,
            [1][1][2] = 3,
            [1][0][2] = 3,
        },
    .OSZ_NONTERM_EOSZ =
        {
            [0][0][0] = 1,
            [1][0][0] = 1,
            [0][1][0] = 2,
            [1][1][0] = 2,
            [0][1][1] = 1,
            [1][1][1] = 1,
            [0][0][1] = 2,
            [1][0][1] = 2,
            [0][1][2] = 1,
            [0][0][2] = 2,
            [1][1][2] = 3,
            [1][0][2] = 3,
        },
    .OSZ_NONTERM_FORCE64_EOSZ =
        {
            [0][0][0] = 1,
            [1][0][0] = 1,
            [0][1][0] = 2,
            [1][1][0] = 2,
            [0][1][1] = 1,
            [1][1][1] = 1,
            [0][0][1] = 2,
            [1][0][1] = 2,
            [0][1][2] = 3,
            [0][0][2] = 3,
            [1][1][2] = 3,
            [1][0][2] = 3,
        },
    .OSZ_NONTERM_IGNORE66_EOSZ =
        {
            [0][0][0] = 1,
            [1][0][0] = 1,
            [0][1][0] = 1,
            [1][1][0] = 1,
            [0][1][1] = 2,
            [1][1][1] = 2,
            [0][0][1] = 2,
            [1][0][1] = 2,
            [0][1][2] = 2,
            [0][0][2] = 2,
            [1][1][2] = 3,
            [1][0][2] = 3,
        },
    .OSZ_NONTERM_IMMUNE66_EOSZ =
        {
            [0][0][0] = 2,
            [1][0][0] = 2,
            [0][1][0] = 2,
            [1][1][0] = 2,
            [0][1][1] = 2,
            [1][1][1] = 2,
            [0][0][1] = 2,
            [1][0][1] = 2,
            [0][1][2] = 2,
            [0][0][2] = 2,
            [1][1][2] = 3,
            [1][0][2] = 3,
        },
    .OSZ_NONTERM_IMMUNE_REXW_EOSZ =
        {
            [0][0][0] = 1,
            [1][0][0] = 1,
            [0][1][0] = 2,
            [1][1][0] = 2,
            [0][1][1] = 1,
            [1][1][1] = 1,
            [0][0][1] = 2,
            [1][0][1] = 2,
            [0][1][2] = 1,
            [0][0][2] = 2,
            [1][1][2] = 2,
            [1][0][2] = 2,
        },
    .OSZ_NONTERM_REFINING66_CR_WIDTH_EOSZ =
        {
            [0][0][0] = 2,
            [1][0][0] = 2,
            [0][1][0] = 2,
            [1][1][0] = 2,
            [0][1][1] = 2,
            [1][1][1] = 2,
            [0][0][1] = 2,
            [1][0][1] = 2,
            [0][1][2] = 3,
            [0][0][2] = 3,
            [1][1][2] = 3,
            [1][0][2] = 3,
        },
    .OSZ_NONTERM_REFINING66_EOSZ =
        {
            [0][0][0] = 1,
            [1][0][0] = 1,
            [0][1][0] = 1,
            [1][1][0] = 1,
            [0][1][1] = 2,
            [1][1][1] = 2,
            [0][0][1] = 2,
            [1][0][1] = 2,
            [0][1][2] = 2,
            [0][0][2] = 2,
            [1][1][2] = 3,
            [1][0][2] = 3,
        },
};

privileged static void xed_too_short(struct XedDecodedInst *d) {
  d->op.out_of_bytes = 1;
  if (d->op.max_bytes >= 15) {
    d->op.error = XED_ERROR_INSTR_TOO_LONG;
  } else {
    d->op.error = XED_ERROR_BUFFER_TOO_SHORT;
  }
}

privileged static void xed_bad_map(struct XedDecodedInst *d) {
  d->op.map = XED_ILD_MAP_INVALID;
  d->op.error = XED_ERROR_BAD_MAP;
}

privileged static void xed_bad_v4(struct XedDecodedInst *d) {
  d->op.error = XED_ERROR_BAD_EVEX_V_PRIME;
}

privileged static void xed_bad_z_aaa(struct XedDecodedInst *d) {
  d->op.error = XED_ERROR_BAD_EVEX_Z_NO_MASKING;
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

privileged static bool xed3_mode_64b(struct XedDecodedInst *d) {
  return d->op.mode == XED_MODE_LONG;
}

privileged static void xed_set_hint(char b, struct XedDecodedInst *d) {
  switch (b) {
    case 0x2e:
      d->op.hint = XED_HINT_NTAKEN;
      return;
    case 0x3e:
      d->op.hint = XED_HINT_TAKEN;
      return;
    default:
      break;
  }
}

privileged static void XED_LF_SIMM8_IMM_WIDTH_CONST_l2(
    struct XedDecodedInst *x) {
  x->op.imm_width = 8;
  x->op.imm_signed = true;
}

privileged static void XED_LF_UIMM16_IMM_WIDTH_CONST_l2(
    struct XedDecodedInst *x) {
  x->op.imm_width = 16;
}

privileged static void XED_LF_SE_IMM8_IMM_WIDTH_CONST_l2(
    struct XedDecodedInst *x) {
  x->op.imm_width = 8;
}

privileged static void XED_LF_UIMM32_IMM_WIDTH_CONST_l2(
    struct XedDecodedInst *x) {
  x->op.imm_width = 32;
}

privileged static void xed_set_simmz_imm_width_eosz(
    struct XedDecodedInst *x, const xed_bits_t eosz[2][2][3]) {
  x->op.imm_width =
      kXed.SIMMz_IMM_WIDTH[eosz[x->op.rexw][x->op.osz][x->op.mode]];
  x->op.imm_signed = true;
}

privileged static void xed_set_uimmv_imm_width_eosz(
    struct XedDecodedInst *x, const xed_bits_t eosz[2][2][3]) {
  x->op.imm_width =
      kXed.UIMMv_IMM_WIDTH[eosz[x->op.rexw][x->op.osz][x->op.mode]];
}

privileged static void XED_LF_UIMM8_IMM_WIDTH_CONST_l2(
    struct XedDecodedInst *x) {
  x->op.imm_width = 8;
}

privileged static void XED_LF_0_IMM_WIDTH_CONST_l2(struct XedDecodedInst *x) {
  x->op.imm_width = 0;
}

privileged static void XED_LF_RESOLVE_BYREG_IMM_WIDTH_map0x0_op0xc7_l1(
    struct XedDecodedInst *x) {
  switch (x->op.reg) {
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
  if (x->op.reg <= 1) {
    XED_LF_SIMM8_IMM_WIDTH_CONST_l2(x);
  } else if (2 <= x->op.reg && x->op.reg <= 7) {
    XED_LF_0_IMM_WIDTH_CONST_l2(x);
  }
}

privileged static void XED_LF_RESOLVE_BYREG_IMM_WIDTH_map0x0_op0xf7_l1(
    struct XedDecodedInst *x) {
  if (x->op.reg <= 1) {
    XED_LF_SIMMz_IMM_WIDTH_OSZ_NONTERM_EOSZ_l2(x);
  } else if (2 <= x->op.reg && x->op.reg <= 7) {
    XED_LF_0_IMM_WIDTH_CONST_l2(x);
  }
}

privileged static void xed_ild_hasimm_map0x0F_op0x78_l1(
    struct XedDecodedInst *x) {
  if (x->op.osz || x->op.ild_f2) {
    x->op.imm_width = xed_bytes2bits(1);
    x->op.imm1_bytes = 1;
  }
}

privileged static void xed_ild_hasimm_map0x0_op0xc8_l1(
    struct XedDecodedInst *x) {
  x->op.imm_width = xed_bytes2bits(2);
  x->op.imm1_bytes = 1;
}

privileged static void xed_set_imm_bytes(struct XedDecodedInst *d) {
  if (!d->op.imm_width && d->op.map < XED_ILD_MAP2) {
    switch (xed_imm_bits_2d[d->op.map][d->op.opcode]) {
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
        d->op.error = XED_ERROR_GENERAL_ERROR;
        return;
    }
  }
}

privileged static int xed_consume_byte(struct XedDecodedInst *d) {
  if (d->length < d->op.max_bytes) {
    return d->bytes[d->length++];
  } else {
    xed_too_short(d);
    return -1;
  }
}

privileged static void xed_prefix_scanner(struct XedDecodedInst *d) {
  xed_bits_t first_f2f3, last_f2f3, seg;
  xed_bits_t b, max_bytes, length, nprefixes, nseg_prefixes, nrexes, rex;
  seg = 0;
  length = d->length;
  max_bytes = d->op.max_bytes;
  first_f2f3 = last_f2f3 = rex = nrexes = nprefixes = nseg_prefixes = 0;
  while (length < max_bytes) {
    b = d->bytes[length];
    if (xed_get_prefix_table_bit(b) == 0) goto out;
    switch (b) {
      case 0x66:
        d->op.osz = true;
        rex = 0;
        break;
      case 0x67:
        d->op.asz = true;
        rex = 0;
        break;
      case 0x2E:
      case 0x3E:
        xed_set_hint(b, d);
        /* fallthrough */
      case 0x26:
      case 0x36:
        if (!xed3_mode_64b(d)) seg = b;
        nseg_prefixes++;
        rex = 0;
        break;
      case 0x64:
      case 0x65:
        seg = b;
        nseg_prefixes++;
        rex = 0;
        break;
      case 0xF0:
        d->op.lock = true;
        rex = 0;
        break;
      case 0xF3:
        d->op.ild_f3 = true;
        last_f2f3 = 3;
        if (!first_f2f3) {
          first_f2f3 = 3;
        }
        rex = 0;
        break;
      case 0xF2:
        d->op.ild_f2 = true;
        last_f2f3 = 2;
        if (!first_f2f3) {
          first_f2f3 = 2;
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
  d->length = length;
  d->op.nprefixes = nprefixes;
  d->op.nseg_prefixes = nseg_prefixes;
  d->op.nrexes = nrexes;
  if (rex) {
    d->op.rexw = rex >> 3 & 1;
    d->op.rexr = rex >> 2 & 1;
    d->op.rexx = rex >> 1 & 1;
    d->op.rexb = rex & 1;
    d->op.rex = true;
  }
  if (d->op.mode_first_prefix) {
    d->op.rep = first_f2f3;
  } else {
    d->op.rep = last_f2f3;
  }
  switch (seg) {
    case 0x26: /* ES */
      d->op.seg_ovd = 0 + 1;
      break;
    case 0x2e: /* CS */
      d->op.seg_ovd = 1 + 1;
      break;
    case 0x36: /* SS */
      d->op.seg_ovd = 2 + 1;
      break;
    case 0x3e: /* DS */
      d->op.seg_ovd = 3 + 1;
      break;
    case 0x64: /* FS */
      d->op.seg_ovd = 4 + 1;
      break;
    case 0x65: /* GS */
      d->op.seg_ovd = 5 + 1;
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
  length = d->length;
  if (length < d->op.max_bytes) {
    b = d->bytes[length];
    d->op.opcode = b;
    d->length++;
  } else {
    xed_too_short(d);
  }
}

privileged static void xed_catch_invalid_rex_or_legacy_prefixes(
    struct XedDecodedInst *d) {
  if (xed3_mode_64b(d) && d->op.rex) {
    d->op.error = XED_ERROR_BAD_REX_PREFIX;
  } else if (d->op.osz || d->op.ild_f3 || d->op.ild_f2) {
    d->op.error = XED_ERROR_BAD_LEGACY_PREFIX;
  }
}

privileged static void xed_catch_invalid_mode(struct XedDecodedInst *d) {
  if (d->op.realmode) {
    d->op.error = XED_ERROR_INVALID_MODE;
  }
}

privileged static void xed_evex_vex_opcode_scanner(struct XedDecodedInst *d) {
  d->op.opcode = d->bytes[d->length];
  d->op.pos_opcode = d->length++;
  xed_catch_invalid_rex_or_legacy_prefixes(d);
  xed_catch_invalid_mode(d);
}

privileged static void xed_opcode_scanner(struct XedDecodedInst *d) {
  xed_bits_t b, length;
  length = d->length;
  if ((b = d->bytes[length]) != 0x0F) {
    d->op.map = XED_ILD_MAP0;
    d->op.opcode = b;
    d->op.pos_opcode = length;
    d->length++;
  } else {
    length++;
    d->op.pos_opcode = length;
    if (length < d->op.max_bytes) {
      switch ((b = d->bytes[length])) {
        case 0x38:
          length++;
          d->op.map = XED_ILD_MAP2;
          d->length = length;
          xed_get_next_as_opcode(d);
          return;
        case 0x3A:
          length++;
          d->op.map = XED_ILD_MAP3;
          d->length = length;
          d->op.imm_width = xed_bytes2bits(1);
          xed_get_next_as_opcode(d);
          return;
        case 0x3B:
          length++;
          xed_bad_map(d);
          d->length = length;
          xed_get_next_as_opcode(d);
          return;
        case 0x39:
        case 0x3C:
        case 0x3D:
        case 0x3E:
        case 0x3F:
          length++;
          xed_bad_map(d);
          d->length = length;
          xed_get_next_as_opcode(d);
          return;
        case 0x0F:
          d->op.amd3dnow = true;
          length++;
          d->op.opcode = 0x0F;
          d->op.map = XED_ILD_MAPAMD;
          d->length = length;
          break;
        default:
          length++;
          d->op.opcode = b;
          d->op.map = XED_ILD_MAP1;
          d->length = length;
          break;
      }
    } else {
      xed_too_short(d);
      return;
    }
  }
}

privileged static bool xed_is_bound_instruction(struct XedDecodedInst *d) {
  return !xed3_mode_64b(d) && d->length + 1 < d->op.max_bytes &&
         (d->bytes[d->length + 1] & 0xC0) != 0xC0;
}

privileged static void xed_evex_scanner(struct XedDecodedInst *d) {
  xed_bits_t length, max_bytes;
  union XedAvx512Payload1 evex1;
  union XedAvx512Payload2 evex2;
  union XedAvx512Payload3 evex3;
  length = d->length;
  max_bytes = d->op.max_bytes;
  /* @assume prefix_scanner() checked length */
  if (d->bytes[length] != 0x62) return;
  if (xed_is_bound_instruction(d)) return;
  if (length + 4 < max_bytes) {
    evex1.u32 = d->bytes[length + 1];
    evex2.u32 = d->bytes[length + 2];
    if (xed3_mode_64b(d)) {
      d->op.rexr = ~evex1.s.r_inv & 1;
      d->op.rexx = ~evex1.s.x_inv & 1;
      d->op.rexb = ~evex1.s.b_inv & 1;
      d->op.rexrr = ~evex1.s.rr_inv & 1;
    }
    d->op.rexw = evex2.s.rexw & 1;
    d->op.map = evex1.s.map;
    d->op.vexdest3 = evex2.s.vexdest3;
    d->op.vexdest210 = evex2.s.vexdest210;
    d->op.ubit = evex2.s.ubit;
    if (evex2.s.ubit) {
      d->op.vexvalid = 2;
    } else {
      d->op.error = XED_ERROR_BAD_EVEX_UBIT;
    }
    d->op.vex_prefix = kXed.vex_prefix_recoding[evex2.s.pp];
    if (evex1.s.map == XED_ILD_MAP3) {
      d->op.imm_width = xed_bytes2bits(1);
    }
    if (evex2.s.ubit) {
      evex3.u32 = d->bytes[length + 3];
      d->op.zeroing = evex3.s.z;
      d->op.llrc = evex3.s.llrc;
      d->op.vl = evex3.s.llrc;
      d->op.bcrc = evex3.s.bcrc;
      d->op.vexdest4 = ~evex3.s.vexdest4p & 1;
      if (!xed3_mode_64b(d) && evex3.s.vexdest4p == 0) {
        xed_bad_v4(d);
      }
      d->op.mask = evex3.s.mask;
      if (evex3.s.mask == 0 && evex3.s.z == 1) {
        xed_bad_z_aaa(d);
      }
    }
    length += 4;
    d->length = length;
    xed_evex_vex_opcode_scanner(d);
  } else {
    xed_too_short(d);
  }
}

privileged static uint64_t xed_read_number(uint8_t *p, size_t n, bool s) {
  switch (s << 2 | _bsr(n)) {
    case 0b000:
      return *p;
    case 0b100:
      return (int8_t)*p;
    case 0b001:
      return READ16LE(p);
    case 0b101:
      return (int16_t)READ16LE(p);
    case 0b010:
      return READ32LE(p);
    case 0b110:
      return (int32_t)READ32LE(p);
    case 0b011:
    case 0b111:
      return READ64LE(p);
    default:
      __builtin_unreachable();
  }
}

privileged static void xed_evex_imm_scanner(struct XedDecodedInst *d) {
  uint8_t *itext, *imm_ptr;
  xed_bits_t length, imm_bytes, imm1_bytes, max_bytes;
  imm_ptr = 0;
  itext = d->bytes;
  xed_set_imm_bytes(d);
  length = d->length;
  max_bytes = d->op.max_bytes;
  if (d->op.amd3dnow) {
    if (length < max_bytes) {
      d->op.opcode = d->bytes[length];
      d->length++;
      return;
    } else {
      xed_too_short(d);
      return;
    }
  }
  imm_bytes = xed_bits2bytes(d->op.imm_width);
  imm1_bytes = d->op.imm1_bytes;
  if (imm_bytes) {
    if (length + imm_bytes <= max_bytes) {
      d->op.pos_imm = length;
      length += imm_bytes;
      d->length = length;
      if (imm1_bytes) {
        if (length + imm1_bytes <= max_bytes) {
          d->op.pos_imm1 = length;
          imm_ptr = itext + length;
          length += imm1_bytes;
          d->length = length;
          d->op.uimm1 = *imm_ptr;
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
  if (imm_bytes) {
    d->op.uimm0 =
        xed_read_number(itext + d->op.pos_imm, imm_bytes, d->op.imm_signed);
  }
}

privileged static void xed_vex_c4_scanner(struct XedDecodedInst *d) {
  unsigned length, b1, b2;
  if (xed_is_bound_instruction(d)) return;
  length = d->length;
  length++;
  if (length + 2 < d->op.max_bytes) {
    // map:   5-bit
    // rex.b: 1-bit
    // rex.x: 1-bit
    // rex.r: 1-bit
    b1 = d->bytes[length];
    d->op.rexr = !(b1 & 128);
    d->op.rexx = !(b1 & 64);
    d->op.rexb = xed3_mode_64b(d) & !(b1 & 32);
    // prefix:        2-bit → {none, osz, rep3, rep2}
    // vector_length: 1-bit → {xmm, ymm}
    // vexdest210:    3-bit
    // vexdest3:      1-bit
    // rex.w:         1-bit
    b2 = d->bytes[length + 1];
    d->op.rexw = !!(b2 & 128);
    d->op.vexdest3 = !!(b2 & 64);
    d->op.vexdest210 = (b2 >> 3) & 7;
    d->op.vl = !!(b2 & 4);
    d->op.vex_prefix = kXed.vex_prefix_recoding[b2 & 3];
    d->op.map = b1 & 31;
    if ((b1 & 3) == XED_ILD_MAP3) {
      d->op.imm_width = xed_bytes2bits(1);
    }
    d->op.vexvalid = 1;
    length += 2;
    d->length = length;
    xed_evex_vex_opcode_scanner(d);
  } else {
    d->length = length;
    xed_too_short(d);
  }
}

privileged static void xed_vex_c5_scanner(struct XedDecodedInst *d) {
  unsigned length, b;
  length = d->length;
  if (xed_is_bound_instruction(d)) return;
  length++;
  if (length + 1 < d->op.max_bytes) {
    // prefix:        2-bit → {none, osz, rep3, rep2}
    // vector_length: 1-bit → {xmm, ymm}
    // vexdest210:    3-bit
    // vexdest3:      1-bit
    // rex.r:         1-bit
    b = d->bytes[length];
    d->op.rexr = !(b & 128);
    d->op.vexdest3 = !!(b & 64);
    d->op.vexdest210 = (b >> 3) & 7;
    d->op.vl = (b >> 2) & 1;
    d->op.vex_prefix = kXed.vex_prefix_recoding[b & 3];
    d->op.map = XED_ILD_MAP1;
    d->op.vexvalid = 1;
    length++;
    d->length = length;
    xed_evex_vex_opcode_scanner(d);
  } else {
    d->length = length;
    xed_too_short(d);
  }
}

privileged static void xed_vex_scanner(struct XedDecodedInst *d) {
  if (!d->op.out_of_bytes) {
    switch (d->bytes[d->length]) {
      case 0xC5:
        xed_vex_c5_scanner(d);
        break;
      case 0xC4:
        xed_vex_c4_scanner(d);
        break;
      default:
        break;
    }
  }
}

privileged static void xed_bad_ll(struct XedDecodedInst *d) {
  d->op.error = XED_ERROR_BAD_EVEX_LL;
}

privileged static void xed_bad_ll_check(struct XedDecodedInst *d) {
  if (d->op.llrc == 3) {
    if (d->op.mod != 3) {
      xed_bad_ll(d);
    } else if (d->op.bcrc == 0) {
      xed_bad_ll(d);
    }
  }
}

privileged static void xed_set_has_modrm(struct XedDecodedInst *d) {
  if (d->op.map < ARRAYLEN(xed_has_modrm_2d)) {
    d->op.has_modrm = xed_has_modrm_2d[d->op.map][d->op.opcode];
  } else {
    d->op.has_modrm = 1;
  }
}

privileged static void xed_modrm_scanner(struct XedDecodedInst *d) {
  uint8_t b;
  xed_bits_t eamode, mod, rm, asz, mode, length, has_modrm;
  xed_set_has_modrm(d);
  has_modrm = d->op.has_modrm;
  if (has_modrm) {
    length = d->length;
    if (length < d->op.max_bytes) {
      b = d->bytes[length];
      d->op.modrm = b;
      d->op.pos_modrm = length;
      d->length++;
      mod = xed_modrm_mod(b);
      rm = xed_modrm_rm(b);
      d->op.mod = mod;
      d->op.rm = rm;
      d->op.reg = xed_modrm_reg(b);
      xed_bad_ll_check(d);
      if (has_modrm != XED_ILD_HASMODRM_IGNORE_MOD) {
        asz = d->op.asz;
        mode = d->op.mode;
        eamode = kXedEamode[asz][mode];
        d->op.disp_width =
            xed_bytes2bits(xed_has_disp_regular[eamode][mod][rm]);
        d->op.has_sib = xed_has_sib_table[eamode][mod][rm];
      }
    } else {
      xed_too_short(d);
    }
  }
}

privileged static void xed_sib_scanner(struct XedDecodedInst *d) {
  uint8_t b;
  xed_bits_t length;
  if (d->op.has_sib) {
    length = d->length;
    if (length < d->op.max_bytes) {
      b = d->bytes[length];
      d->op.pos_sib = length;
      d->op.sib = b;
      d->length++;
      if (xed_sib_base(b) == 5) {
        if (d->op.mod == 0) {
          d->op.disp_width = xed_bytes2bits(4);
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
  x->op.disp_width = 0x8;
}

privileged static void XED_LF_BRDISPz_BRDISP_WIDTH_OSZ_NONTERM_EOSZ_l2(
    struct XedDecodedInst *x) {
  x->op.disp_width =
      kXed.BRDISPz_BRDISP_WIDTH[kXed.OSZ_NONTERM_EOSZ[x->op.rexw][x->op.osz]
                                                     [x->op.mode]];
  x->op.disp_unsigned = true;
}

privileged static void XED_LF_RESOLVE_BYREG_DISP_map0x0_op0xc7_l1(
    struct XedDecodedInst *x) {
  switch (x->op.reg) {
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
  x->op.disp_width =
      kXed.MEMDISPv_DISP_WIDTH[kXed.ASZ_NONTERM_EASZ[x->op.asz][x->op.mode]];
  x->op.disp_unsigned = true;
}

privileged static void XED_LF_BRDISP32_BRDISP_WIDTH_CONST_l2(
    struct XedDecodedInst *x) {
  x->op.disp_width = 0x20;
}

privileged static void XED_LF_DISP_BUCKET_0_l1(struct XedDecodedInst *x) {
  if (x->op.mode <= XED_MODE_LEGACY) {
    XED_LF_BRDISPz_BRDISP_WIDTH_OSZ_NONTERM_EOSZ_l2(x);
    x->op.disp_unsigned = false;
  } else if (x->op.mode == XED_MODE_LONG) {
    XED_LF_BRDISP32_BRDISP_WIDTH_CONST_l2(x);
  }
}

privileged static void xed_disp_scanner(struct XedDecodedInst *d) {
  xed_bits_t length, disp_bytes, max_bytes;
  length = d->length;
  if (d->op.map < XED_ILD_MAP2) {
    switch (xed_disp_bits_2d[d->op.map][d->op.opcode]) {
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
        d->op.error = XED_ERROR_GENERAL_ERROR;
        return;
    }
  }
  disp_bytes = xed_bits2bytes(d->op.disp_width);
  if (disp_bytes) {
    max_bytes = d->op.max_bytes;
    if (length + disp_bytes <= max_bytes) {
      d->op.disp =
          xed_read_number(d->bytes + length, disp_bytes, !d->op.disp_unsigned);
      d->op.pos_disp = length;
      d->length = length + disp_bytes;
    } else {
      xed_too_short(d);
    }
  }
}

privileged static void xed_decode_instruction_length(
    struct XedDecodedInst *ild) {
  xed_prefix_scanner(ild);
  if (!ild->op.out_of_bytes) {
    xed_vex_scanner(ild);
    if (!ild->op.out_of_bytes) {
      if (!ild->op.vexvalid) xed_evex_scanner(ild);
      if (!ild->op.out_of_bytes) {
        if (!ild->op.vexvalid && !ild->op.error) {
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
 * Clears instruction decoder state.
 */
privileged struct XedDecodedInst *xed_decoded_inst_zero_set_mode(
    struct XedDecodedInst *p, int mmode) {
  __memset(p, 0, sizeof(*p));
  xed_operands_set_mode(&p->op, mmode);
  return p;
}

/**
 * Decodes machine instruction length.
 *
 * This function also decodes other useful attributes, such as the
 * offsets of displacement, immediates, etc. It works for all ISAs from
 * 1977 to 2020.
 *
 * @note binary footprint increases ~4kb if this is used
 * @see biggest code in gdb/clang/tensorflow binaries
 */
privileged int xed_instruction_length_decode(struct XedDecodedInst *xedd,
                                             const void *itext, size_t bytes) {
  __memcpy(xedd->bytes, itext, MIN(15, bytes));
  xedd->op.max_bytes = MIN(15, bytes);
  xed_decode_instruction_length(xedd);
  if (!xedd->op.out_of_bytes) {
    if (xedd->op.map != XED_ILD_MAP_INVALID) {
      return xedd->op.error;
    } else {
      return XED_ERROR_GENERAL_ERROR;
    }
  } else {
    return XED_ERROR_BUFFER_TOO_SHORT;
  }
}
