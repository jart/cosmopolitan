/*-*-mode:c++;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8-*-│
│ vi: set et ft=c++ ts=2 sts=2 sw=2 fenc=utf-8                             :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "third_party/double-conversion/wrapper.h"
#include "libc/serialize.h"
#include "libc/str/str.h"
#include "third_party/double-conversion/double-conversion.h"
#include "third_party/double-conversion/double-to-string.h"

namespace double_conversion {
extern "C" {

char* DoubleToEcmascript(char buf[128], double x) {
  StringBuilder b(buf, 128);
  const DoubleToStringConverter& dc =
      DoubleToStringConverter::EcmaScriptConverter();
  dc.ToShortest(x, &b);
  return b.Finalize();
}

char* DoubleToJson(char buf[128], double x) {
  StringBuilder b(buf, 128);
  static const DoubleToStringConverter kDoubleToJson(
      DoubleToStringConverter::UNIQUE_ZERO |
          DoubleToStringConverter::EMIT_POSITIVE_EXPONENT_SIGN,
      "null", "null", 'e', -6, 21, 6, 0);
  kDoubleToJson.ToShortest(x, &b);
  b.Finalize();
  if (READ32LE(buf) != READ32LE("-nul")) {
    return buf;
  } else {
    return strcpy(buf, "null");
  }
}

char* DoubleToLua(char buf[128], double x) {
  static const DoubleToStringConverter kDoubleToLua(
      DoubleToStringConverter::EMIT_TRAILING_DECIMAL_POINT |
          DoubleToStringConverter::NO_TRAILING_ZERO,
      "math.huge", "0/0", 'e', -6, 21, 6, 0);
  StringBuilder b(buf, 128);
  kDoubleToLua.ToShortest(x, &b);
  return b.Finalize();
}

double StringToDouble(const char* s, size_t n, int* out_processed) {
  if (n == -1ull) n = strlen(s);
  int flags = StringToDoubleConverter::ALLOW_CASE_INSENSITIVITY |
              StringToDoubleConverter::ALLOW_LEADING_SPACES |
              StringToDoubleConverter::ALLOW_TRAILING_JUNK |
              StringToDoubleConverter::ALLOW_TRAILING_SPACES;
  StringToDoubleConverter converter(flags, 0.0, 1.0, "Infinity", "NaN");
  int processed;
  double res = converter.StringToDouble(s, n, &processed);
  if (out_processed) {
    *out_processed = processed;
  }
  return res;
}

}  // extern "C"
}  // namespace double_conversion
