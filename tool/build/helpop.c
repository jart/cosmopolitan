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
#include "libc/fmt/conv.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "third_party/getopt/getopt.internal.h"

#define USAGE \
  " [FLAGS] OPERAND..\n\
Decodes geek operand notation used by ref.x86asm.net\n\
\n\
Flags:\n\
  -s         succinct mode\n\
  -b BITS    sets 16/32/64 bit mode [default 64]\n\
  -?         shows this information\n\
\n\
Examples:\n\
  o/tool/cpu/help-operand -b64 Evqp\n\
\n"

int bits_;
bool succinct_;

void PrintUsage(int rc, FILE *f) {
  fputs("Usage: ", f);
  fputs(program_invocation_name, f);
  fputs(USAGE, f);
  exit(rc);
}

void GetOpts(int argc, char *argv[]) {
  int opt;
  bits_ = 64;
  while ((opt = getopt(argc, argv, "hbs")) != -1) {
    switch (opt) {
      case 's':
        succinct_ = true;
        break;
      case 'b':
        bits_ = atoi(optarg);
        break;
      case 'h':
        PrintUsage(EXIT_SUCCESS, stdout);
      default:
        PrintUsage(EX_USAGE, stderr);
    }
  }
}

const struct Descriptors {
  const char prefix[8];
  const char *succinct;
  const char *description;
} kDescriptors[] = {

    {"AL", "AL", "AL register"},
    {"CS", "CS", "CS register (code segment)"},

    {"va", "?",
     "Word or doubleword, according to asz address-size attribute (only REP "
     "and LOOP families)."},
    {"dqa", "?",
     "Doubleword or quadword, according to asz address-size attribute (only "
     "REP and LOOP families)."},
    {"wa", "?",
     "Word, according to asz address-size attribute (only JCXZ instruction)."},
    {"wo", "?",
     "Word, according to current operand size (e. g., MOVSW instruction)."},
    {"ws", "?",
     "Word, according to current stack size (only PUSHF and POPF instructions "
     "in 64-bit mode)."},
    {"da", "?",
     "Doubleword, according to asz address-size attribute (only JECXZ "
     "instruction)."},
    {"do", "?",
     "Doubleword, according to current osz operand size (e. g., MOVSD "
     "instruction)."},
    {"qa", "?",
     "Quadword, according to asz address-size attribute (only JRCXZ "
     "instruction)."},
    {"qs", "64/16",
     "Quadword, according to current stack size via osz operand-size (only "
     "PUSHFQ and POPFQ instructions)."},

    {"va", "16/32",
     "Word or doubleword sign extended to the size of the stack pointer (for "
     "example, PUSH (68))."},

    {"vqp", "16/32/64",
     "Word or doubleword, depending on operand-size attribute, or "
     "quadword, promoted by REX.W in 64-bit mode."},
    {"vs", "16/32",
     "Word or doubleword sign extended to the size of the stack pointer (for "
     "example, PUSH (68))."},
    {"vds", "16/32",
     "Word or doubleword, depending on operand-size attribute, or doubleword, "
     "sign-extended to 64 bits for 64-bit operand size."},
    {"vq", "64/16",
     "Quadword (default) or word if operand-size prefix is used (for "
     "example, PUSH (50))."},

    {"EST", "STi",
     "A ModR/M byte follows the opcode and specifies the x87 FPU stack "
     "register."},
    {"ES", "STi/m",
     "A ModR/M byte follows the opcode and specifies the operand. The "
     "operand is either a x87 FPU stack register or a memory address. If "
     "it is a memory address, the address is computed from a segment "
     "register and any of the following values: a base register, an "
     "index register, a scaling factor, or a displacement."},

    {"SC", "",
     "Stack operand, used by instructions which either push an operand to the "
     "stack or pop an operand from the stack. Pop-like instructions are, for "
     "example, POP, RET, IRET, LEAVE. Push-like are, for example, PUSH, CALL, "
     "INT. No Operand type is provided along with this method because it "
     "depends on source/destination operand(s)."},

    {"BA", "m",
     "Memory addressed by DS:EAX, or by rAX in 64-bit mode (only 0F01C8 "
     "MONITOR)."},
    {"BB", "m",
     "Memory addressed by DS:eBX+AL, or by rBX+AL in 64-bit mode (only XLAT)."},
    {"BD", "m",
     "Memory addressed by DS:eDI or by RDI (only 0FF7 MASKMOVQ and 660FF7 "
     "MASKMOVDQU)"},

    {"A", "ptr",
     "Direct address. The instruction has no ModR/M byte; the address of the "
     "operand is encoded in the instruction; no base register, index register, "
     "or scaling factor can be applied (for example, far JMP (EA))."},
    {"C", "CRn",
     "The reg field of the ModR/M byte selects a control register (only MOV "
     "(0F20, 0F22))."},
    {"D", "DRn",
     "The reg field of the ModR/M byte selects a debug register (only MOV "
     "(0F21, 0F23))."},

    {"I", "imm",
     "Immediate data. The operand value is encoded in subsequent bytes of the "
     "instruction."},

    {"J", "rel",
     "The instruction contains a relative offset to be added to the "
     "instruction pointer register (for example, JMP (E9), LOOP))."},

    {"E", "r/m",
     "A ModR/M byte follows the opcode and specifies the operand. The "
     "operand is either a general-purpose register or a memory address. "
     "If it is a memory address, the address is computed from a segment "
     "register and any of the following values: a base register, an index "
     "register, a scaling factor, or a displacement."},

    {"G", "r",
     "The reg field of the ModR/M byte selects a general register (for "
     "example, AX (000))."},

    {"H", "r",
     "The r/m field of the ModR/M byte always selects a general register, "
     "regardless of the mod field (for example, MOV (0F20))."},

    {"M", "rm",
     "The ModR/M byte may refer only to memory: mod != 11bin (BOUND, LEA, "
     "CALLF, JMPF, LES, LDS, LSS, LFS, LGS, CMPXCHG8B, CMPXCHG16B, F20FF0 "
     "LDDQU)."},

    {"N", "mm",
     "The R/M field of the ModR/M byte selects a packed quadword MMX "
     "technology register."},

    {"O", "moffs",
     "The instruction has no ModR/M byte; the offset of the operand is coded "
     "as a word, double word or quad word (depending on address size "
     "attribute) in the instruction. No base register, index register, or "
     "scaling factor can be applied (only MOV  (A0, A1, A2, A3))."},

    {"P", "mm",
     "The reg field of the ModR/M byte selects a packed quadword MMX "
     "technology register."},

    {"Q", "mm/m64",
     "A ModR/M byte follows the opcode and specifies the operand. The "
     "operand is either an MMX technology register or a memory address. "
     "If it is a memory address, the address is computed from a segment "
     "register and any of the following values: a base register, an index "
     "register, a scaling factor, and a displacement."},

    {"R", "r",
     "The mod field of the ModR/M byte may refer only to a general "
     "register (only MOV (0F20-0F24, 0F26))."},

    {"S", "Sreg",
     "The reg field of the ModR/M byte selects a segment register (only MOV "
     "(8C, 8E))."},

    {"T", "TRn",
     "The reg field of the ModR/M byte selects a test register (only MOV "
     "(0F24, 0F26))."},

    {"U", "xmm",
     "The R/M field of the ModR/M byte selects a 128-bit XMM register."},

    {"sr", "32real",
     "Single-real. Only x87 FPU instructions (for example, FADD)."},
    {"dr", "64real",
     "Double-real. Only x87 FPU instructions (for example, FADD)."},
    {"er", "80real",
     "Extended-real. Only x87 FPU instructions (for example, FLD)."},
    {"e", "14/28", "x87 FPU environment (for example, FSTENV)."},

    {"V", "xmm",
     "The reg field of the ModR/M byte selects a 128-bit XMM register."},

    {"W", "xmm/m",
     "A ModR/M byte follows the opcode and specifies the operand. The operand "
     "is either a 128-bit XMM register or a memory address. If it is a memory "
     "address, the address is computed from a segment register and any of the "
     "following values: a base register, an index register, a scaling factor, "
     "and a displacement"},

    {"X", "m",
     "Memory addressed by the DS:eSI or by RSI (only MOVS, CMPS, OUTS, "
     "and LODS). In 64-bit mode, only 64-bit (RSI) and 32-bit (ESI) "
     "address sizes are supported. In non-64-bit modes, only 32-bit (ESI) "
     "and 16-bit (SI) address sizes are supported."},

    {"Y", "m",
     "Memory addressed by the ES:eDI or by RDI (only MOVS, CMPS, INS, "
     "STOS, and SCAS). In 64-bit mode, only 64-bit (RDI) and 32-bit (EDI) "
     "address sizes are supported. In non-64-bit modes, only 32-bit (EDI) "
     "and 16-bit (DI) address sizes are supported. The implicit ES "
     "segment register cannot be overridden by a segment prefix."},

    {"Z", "r",
     "The instruction has no ModR/M byte; the three least-significant "
     "bits of the opcode byte selects a general-purpose register."},

    {"F", "-", "rFLAGS register."},

    {"si", "32real",
     "Doubleword integer register (e. g., eax). (unused even by Intel?)"},
    {"ss", "-",
     "Scalar element of a 128-bit packed single-precision floating data."},
    {"stx", "512", "x87 FPU and SIMD state (FXSAVE and FXRSTOR)."},
    {"st", "94/108", "x87 FPU state (for example, FSAVE)."},
    {"s", "-",
     "6-byte pseudo-descriptor, or 10-byte pseudo-descriptor in 64-bit mode "
     "(for example, SGDT)."},

    {"bcd", "80dec",
     "Packed-BCD. Only x87 FPU instructions (for example, FBLD)."},
    {"bsq", "", "(Byte, sign-extended to 64 bits.)"},
    {"bss", "8",
     "Byte, sign-extended to the size of the stack pointer (for example, PUSH "
     "(6A))."},
    {"bs", "8", "Byte, sign-extended to the size of the destination operand."},

    {"a", "16/32&16/32",
     "Two one-word operands in memory or two double-word operands in memory, "
     "depending on operand-size attribute (only BOUND)."},
    {"b", "8", "Byte, regardless of operand-size attribute."},
    {"c", "?",
     "Byte or word, depending on operand-size attribute. (unused even by "
     "Intel?)"},
    {"dqp", "32/64",
     "Doubleword, or quadword, promoted by REX.W in 64-bit mode (for example, "
     "MOVSXD)."},
    {"dq", "128",
     "Double-quadword, regardless of operand-size attribute (for example, "
     "CMPXCHG16B)."},
    {"ds", "32",
     "Doubleword, sign-extended to 64 bits (for example, CALL (E8)."},
    {"di", "32int",
     "Doubleword-integer. Only x87 FPU instructions (for example, FIADD)."},
    {"d", "32", "Doubleword, regardless of operand-size attribute."},
    {"qi", "64int",
     "Qword-integer. Only x87 FPU instructions (for example, FILD)."},
    {"qp", "64", "Quadword, promoted by REX.W (for example, IRETQ)."},
    {"q", "64",
     "Quadword, regardless of operand-size attribute (for example, CALL (FF "
     "/2))."},

    {"v", "16/32",
     "Word or doubleword, depending on operand-size attribute (for example, "
     "INC (40), PUSH (50))."},

    {"wi", "16int",
     "Word-integer. Only x87 FPU instructions (for example, FIADD)."},
    {"w", "16",
     "Word, regardless of operand-size attribute (for example, ENTER)."},

    {"pi", "-", "Quadword MMX technology data."},
    {"psq", "-", "64-bit packed single-precision floating-point data."},
    {"pd", "-", "128-bit packed double-precision floating-point data."},
    {"ps", "-", "128-bit packed single-precision floating-point data."},
    {"ptp", "16:16/32/64",
     "32-bit or 48-bit pointer, depending on operand-size attribute, or 80-bit "
     "far pointer, promoted by REX.W in 64-bit mode (for example, "
     "CALLF (FF /3))."},
    {"p", "16:16/32",
     "32-bit or 48-bit pointer, depending on operand-size attribute (for "
     "example, CALLF (9A)."},

};

void HandleOperand(const char *op) {
  int i;
  bool found;
  while (*op) {
    found = false;
    for (i = 0; i < ARRAYLEN(kDescriptors); ++i) {
      if (startswith(op, kDescriptors[i].prefix)) {
        found = true;
        op += strlen(kDescriptors[i].prefix);
        if (succinct_) {
          printf("%s ", kDescriptors[i].succinct);
        } else if (!isempty(kDescriptors[i].succinct) &&
                   strcmp(kDescriptors[i].succinct, "-") != 0 &&
                   strcmp(kDescriptors[i].succinct, "?") != 0) {
          printf("%s (%s): %s\n", kDescriptors[i].prefix,
                 kDescriptors[i].succinct, kDescriptors[i].description);
        } else {
          printf("%s: %s\n", kDescriptors[i].prefix,
                 kDescriptors[i].description);
        }
        break;
      }
    }
    if (!found) {
      printf("%c?", *op);
      if (succinct_) {
        printf(" ");
      } else {
        printf("\n");
      }
      op++;
    }
  }
  printf("\n");
}

int main(int argc, char *argv[]) {
  int i;
  GetOpts(argc, argv);
  for (i = optind; i < argc; ++i) {
    HandleOperand(argv[i]);
  }
  return 0;
}
