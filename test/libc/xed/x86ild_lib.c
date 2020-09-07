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
#include "libc/fmt/bing.h"
#include "libc/macros.h"
#include "libc/runtime/gc.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "test/libc/xed/lib.h"
#include "third_party/xed/x86.h"

testonly nodiscard uint8_t *unbingx86op(const char16_t *codez) {
  size_t len;
  len = strlen16(codez);
  return unbingbuf(xmalloc(ROUNDUP(len, 16)), len, codez, 0x90);
}

/**
 * Long mode instruction length decoder.
 */
testonly int ild(const char16_t *codez) {
  struct XedDecodedInst xedd;
  enum XedError error;
  error = xed_instruction_length_decode(
      xed_decoded_inst_zero_set_mode(&xedd, XED_MACHINE_MODE_LONG_64),
      gc(unbingx86op(codez)), strlen16(codez) + 16);
  return error == XED_ERROR_NONE ? xedd.length : -error;
}

/**
 * Real mode instruction length decoder.
 */
testonly int ildreal(const char16_t *codez) {
  struct XedDecodedInst xedd;
  enum XedError error;
  error = xed_instruction_length_decode(
      xed_decoded_inst_zero_set_mode(&xedd, XED_MACHINE_MODE_REAL),
      gc(unbingx86op(codez)), strlen16(codez) + 16);
  return error == XED_ERROR_NONE ? xedd.length : -error;
}

/**
 * Legacy mode instruction length decoder.
 */
testonly int ildlegacy(const char16_t *codez) {
  struct XedDecodedInst xedd;
  enum XedError error;
  error = xed_instruction_length_decode(
      xed_decoded_inst_zero_set_mode(&xedd, XED_MACHINE_MODE_LEGACY_32),
      gc(unbingx86op(codez)), strlen16(codez) + 16);
  return error == XED_ERROR_NONE ? xedd.length : -error;
}
