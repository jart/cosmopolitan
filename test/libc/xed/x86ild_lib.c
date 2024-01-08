/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/fmt/bing.internal.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "test/libc/xed/lib.h"
#include "third_party/xed/x86.h"

__wur uint8_t *unbingx86op(const char16_t *codez) {
  size_t len;
  len = strlen16(codez);
  return unbingbuf(xmalloc(ROUNDUP(len, 16)), len, codez, 0x90);
}

/**
 * Long mode instruction length decoder.
 */
int ild(const char16_t *codez) {
  int error;
  struct XedDecodedInst xedd;
  error = xed_instruction_length_decode(
      xed_decoded_inst_zero_set_mode(&xedd, XED_MACHINE_MODE_LONG_64),
      gc(unbingx86op(codez)), strlen16(codez) + 16);
  return error == XED_ERROR_NONE ? xedd.length : -error;
}

/**
 * Real mode instruction length decoder.
 */
int ildreal(const char16_t *codez) {
  int error;
  struct XedDecodedInst xedd;
  error = xed_instruction_length_decode(
      xed_decoded_inst_zero_set_mode(&xedd, XED_MACHINE_MODE_REAL),
      gc(unbingx86op(codez)), strlen16(codez) + 16);
  return error == XED_ERROR_NONE ? xedd.length : -error;
}

/**
 * Legacy mode instruction length decoder.
 */
int ildlegacy(const char16_t *codez) {
  int error;
  struct XedDecodedInst xedd;
  error = xed_instruction_length_decode(
      xed_decoded_inst_zero_set_mode(&xedd, XED_MACHINE_MODE_LEGACY_32),
      gc(unbingx86op(codez)), strlen16(codez) + 16);
  return error == XED_ERROR_NONE ? xedd.length : -error;
}
