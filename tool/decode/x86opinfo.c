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
#include "libc/errno.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/tab.internal.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/xed/x86.h"
#include "tool/decode/lib/idname.h"
#include "tool/decode/lib/xederrors.h"

const struct IdName kXedModeNames[] = {
    {XED_MACHINE_MODE_LONG_64, "long"},
    {XED_MACHINE_MODE_LONG_COMPAT_32, "long32"},
    {XED_MACHINE_MODE_LONG_COMPAT_16, "long16"},
    {XED_MACHINE_MODE_LEGACY_32, "legacy"},
    {XED_MACHINE_MODE_LEGACY_16, "legacy16"},
    {XED_MACHINE_MODE_REAL, "real"},
    {XED_MACHINE_MODE_UNREAL, "unreal"},
};

int g_mode;
struct XedDecodedInst g_xedd;

wontreturn void ShowUsage(int rc, FILE *f) {
  size_t i;
  fputs("Usage: ", f);
  fputs(program_invocation_name, f);
  fputs(" [-rl] [-m MODE] HEX\n  MODE ∊ {", f);
  fputs(kXedModeNames[0].name, f);
  for (i = 1; i < ARRAYLEN(kXedModeNames); ++i) {
    fputc(',', f);
    fputs(kXedModeNames[i].name, f);
  }
  fputs("}\n", f);
  exit(rc);
}

void SetMachineMode(const char *s) {
  size_t i;
  for (i = 0; i < ARRAYLEN(kXedModeNames); ++i) {
    if (strcasecmp(s, kXedModeNames[i].name) == 0) {
      g_mode = kXedModeNames[i].id;
      return;
    }
  }
  fputs("error: bad mode\n", stderr);
  ShowUsage(EXIT_FAILURE, stderr);
}

void GetOpts(int argc, char *argv[]) {
  int opt;
  g_mode = XED_MACHINE_MODE_LONG_64;
  while ((opt = getopt(argc, argv, "?hrlm:")) != -1) {
    switch (opt) {
      case 'r':
        g_mode = XED_MACHINE_MODE_REAL;
        break;
      case 'l':
        g_mode = XED_MACHINE_MODE_LEGACY_32;
        break;
      case 'm':
        SetMachineMode(optarg);
        break;
      case '?':
      case 'h':
        ShowUsage(EXIT_SUCCESS, stdout);
      default:
        ShowUsage(EX_USAGE, stderr);
    }
  }
}

void CheckHex(const char *s) {
  size_t i, n;
  if ((n = strlen(s)) % 2 == 1) {
    ShowUsage(EX_DATAERR, stderr);
  }
  for (i = 0; i < n; ++i) {
    if (!isxdigit(s[i + 0])) {
      ShowUsage(EX_DATAERR, stderr);
    }
  }
}

void ShowField(const char *name, uint64_t value) {
  if (value) {
    printf("/\t%-20s = %#lx\n", name, value);
  }
}

void ShowField2(const char *name, uint64_t value, bool cond) {
  if (value || cond) {
    printf("/\t%-20s = %#lx\n", name, value);
  }
}

void ShowOffset(const char *name, uint64_t off) {
  printf("/\t%-20s = %#lx\n", name, off);
}

int main(int argc, char *argv[]) {
  int err;
  size_t i, j, k;
  uint8_t buf[XED_MAX_INSTRUCTION_BYTES];

  GetOpts(argc, argv);

  for (k = 0, i = optind; i < argc; ++i) {
    CheckHex(argv[i]);
    for (j = 0; argv[i][j]; j += 2) {
      if (++k > XED_MAX_INSTRUCTION_BYTES) {
        ShowUsage(EX_DATAERR, stderr);
      }
      buf[k - 1] = kHexToInt[argv[i][j + 0] & 255] << 4 |
                   kHexToInt[argv[i][j + 1] & 255];
    }
  }

  xed_decoded_inst_zero_set_mode(&g_xedd, g_mode);
  if ((err = xed_instruction_length_decode(&g_xedd, buf, k)) !=
      XED_ERROR_NONE) {
    fputs("XED_ERROR_", stderr);
    fputs(findnamebyid(kXedErrorIdNames, err), stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
  }

#define SHOWOP(F)     ShowField(#F, g_xedd.op.F)
#define SHOWOP2(F, C) ShowField2(#F, g_xedd.op.F, C)

  printf("/\t%-20s = %d\n", "length", g_xedd.length);
  SHOWOP(error);

  SHOWOP(lock);
  SHOWOP(osz);
  SHOWOP(asz);
  SHOWOP(rep);
  SHOWOP(rex);
  SHOWOP(rexb);
  SHOWOP(rexr);
  SHOWOP(rexrr);
  SHOWOP(rexw);
  SHOWOP(rexx);

  SHOWOP(map);
  SHOWOP(opcode);

  if (g_xedd.op.has_modrm) {
    printf("\n");
    printf("/\t%-20s = 0b%02hhb  (%d)\n", "mod", g_xedd.op.mod, g_xedd.op.mod);
    printf("/\t%-20s = 0b%03hhb (%d)\n", "reg", g_xedd.op.reg, g_xedd.op.reg);
    printf("/\t%-20s = 0b%03hhb (%d)\n", "rm", g_xedd.op.rm, g_xedd.op.rm);
    printf("\n");
  }

  if (g_xedd.op.has_sib) {
    printf("/\t%-20s = 0b%02hhb  (%d)\n", "scale", xed_sib_scale(g_xedd.op.sib),
           xed_sib_scale(g_xedd.op.sib));
    printf("/\t%-20s = 0b%03hhb (%d)\n", "index", xed_sib_index(g_xedd.op.sib),
           xed_sib_index(g_xedd.op.sib));
    printf("/\t%-20s = 0b%03hhb (%d)\n", "base", xed_sib_base(g_xedd.op.sib),
           xed_sib_base(g_xedd.op.sib));
    printf("\n");
  }

  SHOWOP2(disp, !!g_xedd.op.pos_disp);
  SHOWOP2(uimm0, !!g_xedd.op.pos_imm);
  SHOWOP2(uimm1, !!g_xedd.op.pos_imm1);

  SHOWOP(mode);
  SHOWOP(amd3dnow);
  SHOWOP(bcrc);
  SHOWOP(disp_width);
  SHOWOP(hint);
  SHOWOP(ild_f2);
  SHOWOP(ild_f3);
  SHOWOP(imm1_bytes);
  SHOWOP(imm_width);
  SHOWOP(imm_signed);
  SHOWOP(llrc);
  SHOWOP(mask);
  SHOWOP(max_bytes);
  SHOWOP(mode_first_prefix);
  SHOWOP(nprefixes);
  SHOWOP(nrexes);
  SHOWOP(nseg_prefixes);
  SHOWOP(out_of_bytes);
  SHOWOP(pos_disp);
  SHOWOP(pos_imm);
  SHOWOP(pos_imm1);
  SHOWOP(pos_modrm);
  SHOWOP(pos_opcode);
  SHOWOP(pos_sib);
  SHOWOP(realmode);
  SHOWOP(seg_ovd);
  SHOWOP(srm);
  SHOWOP(ubit);
  SHOWOP(vex_prefix);
  SHOWOP(vexdest210);
  SHOWOP(vexdest3);
  SHOWOP(vexdest4);
  SHOWOP(vexvalid);
  SHOWOP(vl);
  SHOWOP(zeroing);

  return 0;
}
