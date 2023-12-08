/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/nexgen32e/x86feature.h"
#include "libc/nexgen32e/x86info.h"
#include "libc/runtime/runtime.h"
#include "third_party/getopt/getopt.internal.h"

/**
 * @fileoverview Command for printing `-march=native` flags.
 *
 * In recent years (2023) compilers have decided to remove support for
 * the `-march=native` flag, even on x86. That's unfortunate, since as
 * we can see below, grokking all the various microarchitecture is not
 * something a compiler should reasonably expect from users especially
 * not for a flag as important as this one, which can have a night and
 * day impact for apps that do scientific computing.
 *
 * This is a tiny program, that makes it easy for shell scripts to get
 * these flags.
 */

#define VERSION                       \
  "-march=native flag printer v0.1\n" \
  "copyright 2023 justine alexandra roberts tunney\n"

#define USAGE                        \
  "usage: march-native.com [-hvc]\n" \
  "  -h          show help\n"        \
  "  -v          show version\n"     \
  "  -c          assume we're using clang (not gcc)\n"

static bool isclang;

static void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "hvc")) != -1) {
    switch (opt) {
      case 'c':
        isclang = true;
        break;
      case 'v':
        tinyprint(1, VERSION, NULL);
        exit(0);
      case 'h':
        tinyprint(1, VERSION, USAGE, NULL);
        exit(0);
      default:
        tinyprint(2, VERSION, USAGE, NULL);
        exit(1);
    }
  }
}

static void Puts(const char *s) {
  tinyprint(1, s, "\n", NULL);
}

int main(int argc, char *argv[]) {
  GetOpts(argc, argv);
#ifdef __x86_64__
  struct X86ProcessorModel *model;
  if (X86_HAVE(XOP)) Puts("-mxop");
  if (X86_HAVE(SSE4A)) Puts("-msse4a");
  if (X86_HAVE(SSE3)) Puts("-msse3");
  if (X86_HAVE(SSSE3)) Puts("-mssse3");
  if (X86_HAVE(SSE4_1)) Puts("-msse4.1");
  if (X86_HAVE(SSE4_2)) Puts("-msse4.2");
  if (X86_HAVE(AVX)) Puts("-mavx");
  if (X86_HAVE(AVX2)) {
    Puts("-mavx2");
    if (!isclang) {
      Puts("-msse2avx");
      Puts("-Wa,-msse2avx");
    }
  }
  if (X86_HAVE(AVX512F)) Puts("-mavx512f");
  if (X86_HAVE(AVX512PF)) Puts("-mavx512pf");
  if (X86_HAVE(AVX512ER)) Puts("-mavx512er");
  if (X86_HAVE(AVX512CD)) Puts("-mavx512cd");
  if (X86_HAVE(AVX512VL)) Puts("-mavx512vl");
  if (X86_HAVE(AVX512BW)) Puts("-mavx512bw");
  if (X86_HAVE(AVX512DQ)) Puts("-mavx512dq");
  if (X86_HAVE(AVX512IFMA)) Puts("-mavx512ifma");
  if (X86_HAVE(AVX512VBMI)) Puts("-mavx512vbmi");
  if (X86_HAVE(SHA)) Puts("-msha");
  if (X86_HAVE(AES)) Puts("-maes");
  if (X86_HAVE(VAES)) Puts("-mvaes");
  if (X86_HAVE(PCLMUL)) Puts("-mpclmul");
  if (X86_HAVE(FSGSBASE)) Puts("-mfsgsbase");
  if (X86_HAVE(F16C)) Puts("-mf16c");
  if (X86_HAVE(FMA)) Puts("-mfma");
  if (X86_HAVE(POPCNT)) Puts("-mpopcnt");
  if (X86_HAVE(BMI)) Puts("-mbmi");
  if (X86_HAVE(BMI2)) Puts("-mbmi2");
  if (X86_HAVE(ADX)) Puts("-madx");
  if (X86_HAVE(FXSR)) Puts("-mfxsr");
  if ((model = (void *)getx86processormodel(kX86ProcessorModelKey))) {
    switch (model->march) {
      case X86_MARCH_CORE2:
        Puts("-march=core2");
        break;
      case X86_MARCH_NEHALEM:
        Puts("-march=nehalem");
        break;
      case X86_MARCH_WESTMERE:
        Puts("-march=westmere");
        break;
      case X86_MARCH_SANDYBRIDGE:
        Puts("-march=sandybridge");
        break;
      case X86_MARCH_IVYBRIDGE:
        Puts("-march=ivybridge");
        break;
      case X86_MARCH_HASWELL:
        Puts("-march=haswell");
        break;
      case X86_MARCH_BROADWELL:
        Puts("-march=broadwell");
        break;
      case X86_MARCH_SKYLAKE:
      case X86_MARCH_KABYLAKE:
        Puts("-march=skylake");
        break;
      case X86_MARCH_CANNONLAKE:
        Puts("-march=cannonlake");
        break;
      case X86_MARCH_ICELAKE:
        if (model->grade >= X86_GRADE_SERVER) {
          Puts("-march=icelake-server");
        } else {
          Puts("-march=icelake-client");
        }
        break;
      case X86_MARCH_TIGERLAKE:
        Puts("-march=tigerlake");
        break;
      case X86_MARCH_BONNELL:
      case X86_MARCH_SALTWELL:
        Puts("-march=bonnell");
        break;
      case X86_MARCH_SILVERMONT:
      case X86_MARCH_AIRMONT:
        Puts("-march=silvermont");
        break;
      case X86_MARCH_GOLDMONT:
        Puts("-march=goldmont");
        break;
      case X86_MARCH_GOLDMONTPLUS:
        Puts("-march=goldmont-plus");
        break;
      case X86_MARCH_TREMONT:
        Puts("-march=tremont");
        break;
      case X86_MARCH_KNIGHTSLANDING:
        Puts("-march=knl");
        break;
      case X86_MARCH_KNIGHTSMILL:
        Puts("-march=knm");
        break;
    }
  }
#elif defined(__aarch64__)
  // TODO(jart): How can we determine CPU features on AARCH64?
#else
  // otherwise do nothing (it's usually best)
#endif
}
