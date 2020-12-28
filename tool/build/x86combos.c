/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/calls.h"
#include "libc/log/check.h"
#include "libc/macros.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "third_party/xed/x86.h"

const char kPrefixes[][8] = {
    {},
    {0x66},
    {0x67},
    {0x40},
    {0x6F},
    {0x66, 0x6F},
    {0x66, 0x40},
    {0x66, 0x6F},
    {0x67, 0x40},
    {0x67, 0x6F},
    {0x66, 0x67},
    {0x66, 0x67, 0x40},
    {0x66, 0x67, 0x6F},

    {0x0F},
    {0x0F, 0x66},
    {0x0F, 0x67},
    {0x0F, 0x40},
    {0x0F, 0x6F},
    {0x0F, 0x66, 0x6F},
    {0x0F, 0x66, 0x40},
    {0x0F, 0x66, 0x6F},
    {0x0F, 0x67, 0x40},
    {0x0F, 0x67, 0x6F},
    {0x0F, 0x66, 0x67},
    {0x0F, 0x66, 0x67, 0x40},
    {0x0F, 0x66, 0x67, 0x6F},

    {0xF3, 0x0F},
    {0xF3, 0x66, 0x0F},
    {0xF3, 0x67, 0x0F},
    {0xF3, 0x40, 0x0F},
    {0xF3, 0x6F, 0x0F},
    {0xF3, 0x66, 0x6F, 0x0F},
    {0xF3, 0x66, 0x40, 0x0F},
    {0xF3, 0x66, 0x6F, 0x0F},
    {0xF3, 0x67, 0x40, 0x0F},
    {0xF3, 0x67, 0x6F, 0x0F},
    {0xF3, 0x66, 0x67, 0x0F},
    {0xF3, 0x66, 0x67, 0x40, 0x0F},
    {0xF3, 0x66, 0x67, 0x6F, 0x0F},

    {0xF2, 0x0F},
    {0xF2, 0x66, 0x0F},
    {0xF2, 0x67, 0x0F},
    {0xF2, 0x40, 0x0F},
    {0xF2, 0x6F, 0x0F},
    {0xF2, 0x66, 0x6F, 0x0F},
    {0xF2, 0x66, 0x40, 0x0F},
    {0xF2, 0x66, 0x6F, 0x0F},
    {0xF2, 0x67, 0x40, 0x0F},
    {0xF2, 0x67, 0x6F, 0x0F},
    {0xF2, 0x66, 0x67, 0x0F},
    {0xF2, 0x66, 0x67, 0x40, 0x0F},
    {0xF2, 0x66, 0x67, 0x6F, 0x0F},

};

const uint8_t kModrmPicks[] = {
    0x0,  0x1,  0x2,  0x3,  0x4,  0x5,  0x6,  0x7,  0x0,  0x8,  0x10,
    0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45,
    0x46, 0x47, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80,
    0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x80, 0x88, 0x90, 0x98,
    0xa0, 0xa8, 0xb0, 0xb8, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6,
    0xc7, 0xc0, 0xc8, 0xd0, 0xd8, 0xe0, 0xe8, 0xf0, 0xf8,
};

const uint8_t kOpMap0[] = {
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,
    15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
    30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,
    45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
    60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,
    75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,
    90,  91,  92,  93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103, 104,
    105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134,
    135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149,
    150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164,
    165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179,
    180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194,
    195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
    210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224,
    225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254,
    255,
};

void WriteOp(int fd, struct XedDecodedInst *xedd) {
  int i;
  char buf[128], *p;
  p = stpcpy(buf, ".byte ");
  for (i = 0; i < xedd->length; ++i) {
    if (i) *p++ = ',';
    *p++ = '0';
    *p++ = 'x';
    *p++ = "0123456789abcdef"[(xedd->bytes[i] & 0xf0) >> 4];
    *p++ = "0123456789abcdef"[xedd->bytes[i] & 0x0f];
  }
  *p++ = '\n';
  CHECK_NE(-1, write(fd, buf, p - buf));
}

int main(int argc, char *argv[]) {
  int i, j, k, l, m, n, p, o, fd;
  uint8_t op[16];
  struct XedDecodedInst xedd[1];
  fd = open("/tmp/ops.s", O_CREAT | O_TRUNC | O_RDWR, 0644);
  for (o = 0; o < ARRAYLEN(kOpMap0); ++o) {
    for (p = 0; p < ARRAYLEN(kPrefixes); ++p) {
      memset(op, 0x55, 16);
      n = strlen(kPrefixes[p]);
      memcpy(op, kPrefixes[p], n);
      op[n] = kOpMap0[o];
      xed_decoded_inst_zero_set_mode(xedd, XED_MACHINE_MODE_LONG_64);
      if (!xed_instruction_length_decode(xedd, op, XED_MAX_INSTRUCTION_BYTES)) {
        if (xedd->op.has_modrm && xedd->op.has_sib) {
          for (i = 0; i < ARRAYLEN(kModrmPicks); ++i) {
            for (j = 0; j < ARRAYLEN(kModrmPicks); ++j) {
              memset(op, 0x55, 16);
              n = strlen(kPrefixes[p]);
              memcpy(op, kPrefixes[p], n);
              op[n] = kOpMap0[o];
              op[xedd->op.pos_modrm] = kModrmPicks[i];
              op[xedd->op.pos_sib] = kModrmPicks[j];
              xed_decoded_inst_zero_set_mode(xedd, XED_MACHINE_MODE_LONG_64);
              if (!xed_instruction_length_decode(xedd, op,
                                                 XED_MAX_INSTRUCTION_BYTES)) {
                WriteOp(fd, xedd);
              }
            }
          }
        } else if (xedd->op.has_modrm) {
          for (i = 0; i < ARRAYLEN(kModrmPicks); ++i) {
            memset(op, 0x55, 16);
            n = strlen(kPrefixes[p]);
            memcpy(op, kPrefixes[p], n);
            op[n] = kOpMap0[o];
            op[xedd->op.pos_modrm] = kModrmPicks[i];
            xed_decoded_inst_zero_set_mode(xedd, XED_MACHINE_MODE_LONG_64);
            if (!xed_instruction_length_decode(xedd, op,
                                               XED_MAX_INSTRUCTION_BYTES)) {
              WriteOp(fd, xedd);
            }
          }
        } else {
          WriteOp(fd, xedd);
        }
      }
    }
  }
  close(fd);
  system("as -o /tmp/ops.o /tmp/ops.s");
  system("objdump -wd /tmp/ops.o |"
         " grep -v data16 |"
         " grep -v addr32 |"
         " grep -v '(bad)' |"
         " sed 's/^[ :[:xdigit:]]*//' |"
         " sed 's/^[ :[:xdigit:]]*//' |"
         " sed 's/[[:space:]]#.*$//' |"
         " grep -v 'rex\\.' |"
         " sort -u");
  return 0;
}
