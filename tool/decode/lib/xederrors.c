/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/xed/x86.h"
#include "tool/decode/lib/idname.h"

const struct IdName kXedErrorNames[] = {
    {XED_ERROR_NONE, "NONE"},
    {XED_ERROR_BUFFER_TOO_SHORT, "BUFFER_TOO_SHORT"},
    {XED_ERROR_GENERAL_ERROR, "GENERAL_ERROR"},
    {XED_ERROR_INVALID_FOR_CHIP, "INVALID_FOR_CHIP"},
    {XED_ERROR_BAD_REGISTER, "BAD_REGISTER"},
    {XED_ERROR_BAD_LOCK_PREFIX, "BAD_LOCK_PREFIX"},
    {XED_ERROR_BAD_REP_PREFIX, "BAD_REP_PREFIX"},
    {XED_ERROR_BAD_LEGACY_PREFIX, "BAD_LEGACY_PREFIX"},
    {XED_ERROR_BAD_REX_PREFIX, "BAD_REX_PREFIX"},
    {XED_ERROR_BAD_EVEX_UBIT, "BAD_EVEX_UBIT"},
    {XED_ERROR_BAD_MAP, "BAD_MAP"},
    {XED_ERROR_BAD_EVEX_V_PRIME, "BAD_EVEX_V_PRIME"},
    {XED_ERROR_BAD_EVEX_Z_NO_MASKING, "BAD_EVEX_Z_NO_MASKING"},
    {XED_ERROR_NO_OUTPUT_POINTER, "NO_OUTPUT_POINTER"},
    {XED_ERROR_NO_AGEN_CALL_BACK_REGISTERED, "NO_AGEN_CALL_BACK_REGISTERED"},
    {XED_ERROR_BAD_MEMOP_INDEX, "BAD_MEMOP_INDEX"},
    {XED_ERROR_CALLBACK_PROBLEM, "CALLBACK_PROBLEM"},
    {XED_ERROR_GATHER_REGS, "GATHER_REGS"},
    {XED_ERROR_INSTR_TOO_LONG, "INSTR_TOO_LONG"},
    {XED_ERROR_INVALID_MODE, "INVALID_MODE"},
    {XED_ERROR_BAD_EVEX_LL, "BAD_EVEX_LL"},
    {XED_ERROR_UNIMPLEMENTED, "UNIMPLEMENTED"},
    {0, 0},
};
