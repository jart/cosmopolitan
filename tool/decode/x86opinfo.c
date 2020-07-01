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
#include "libc/bits/bits.h"
#include "libc/macros.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "third_party/getopt/getopt.h"
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

enum XedMachineMode g_mode;
struct XedDecodedInst g_xedd;

noreturn void ShowUsage(int rc, FILE *f) {
  size_t i;
  fputs("Usage: ", f);
  fputs(program_invocation_name, f);
  fputs(" [-r] [-m MODE] HEX\n  MODE ∊ {", f);
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
  while ((opt = getopt(argc, argv, "?hrm:")) != -1) {
    switch (opt) {
      case 'r':
        g_mode = XED_MACHINE_MODE_REAL;
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
      if (++k > XED_MAX_INSTRUCTION_BYTES) ShowUsage(EX_DATAERR, stderr);
      buf[k - 1] = hextoint(argv[i][j + 0]) << 4 | hextoint(argv[i][j + 1]);
    }
  }

  xed_decoded_inst_zero_set_mode(&g_xedd, g_mode);
  if ((err = xed_instruction_length_decode(&g_xedd, buf, k)) !=
      XED_ERROR_NONE) {
    fputs("XED_ERROR_", stderr);
    fputs(findnamebyid(kXedErrorNames, err), stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
  }

#define SHOWOP(F) ShowField(#F, g_xedd.operands.F)
  SHOWOP(amd3dnow);
  SHOWOP(asz);
  SHOWOP(bcrc);
  SHOWOP(chip);
  SHOWOP(cldemote);
  SHOWOP(disp);
  SHOWOP(disp_width);
  SHOWOP(error);
  SHOWOP(esrc);
  SHOWOP(first_f2f3);
  SHOWOP(modrm);
  SHOWOP(sib);
  SHOWOP(has_modrm);
  SHOWOP(has_sib);
  SHOWOP(hint);
  SHOWOP(ild_f2);
  SHOWOP(ild_f3);
  SHOWOP(ild_seg);
  SHOWOP(imm1_bytes);
  SHOWOP(imm_width);
  SHOWOP(last_f2f3);
  SHOWOP(llrc);
  SHOWOP(lock);
  SHOWOP(map);
  SHOWOP(mask);
  SHOWOP(max_bytes);
  SHOWOP(mod);
  SHOWOP(mode);
  SHOWOP(mode_first_prefix);
  SHOWOP(nominal_opcode);
  SHOWOP(nprefixes);
  SHOWOP(nrexes);
  SHOWOP(nseg_prefixes);
  SHOWOP(osz);
  SHOWOP(out_of_bytes);
  SHOWOP(pos_disp);
  SHOWOP(pos_imm);
  SHOWOP(pos_imm1);
  SHOWOP(pos_modrm);
  SHOWOP(pos_nominal_opcode);
  SHOWOP(pos_sib);
  SHOWOP(prefix66);
  SHOWOP(realmode);
  SHOWOP(reg);
  SHOWOP(rep);
  SHOWOP(rex);
  SHOWOP(rexb);
  SHOWOP(rexr);
  SHOWOP(rexrr);
  SHOWOP(rexw);
  SHOWOP(rexx);
  SHOWOP(rm);
  SHOWOP(seg_ovd);
  SHOWOP(srm);
  SHOWOP(ubit);
  SHOWOP(uimm0);
  SHOWOP(uimm1);
  SHOWOP(vex_prefix);
  SHOWOP(vexdest210);
  SHOWOP(vexdest3);
  SHOWOP(vexdest4);
  SHOWOP(vexvalid);
  SHOWOP(vl);
  SHOWOP(wbnoinvd);
  SHOWOP(zeroing);

  return 0;
}
