// Copyright 2024 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.
#include "libc/intrin/x86.h"

/**
 * Returns microarchitecture name, e.g.
 *
 *     puts(__cpu_march(__cpu_model.__cpu_subtype));
 *
 */
const char *__cpu_march(unsigned subtype) {
  switch (subtype) {
    case INTEL_COREI7_NEHALEM:
      return "nehalem";
    case INTEL_COREI7_WESTMERE:
      return "westmere";
    case INTEL_COREI7_SANDYBRIDGE:
      return "sandybridge";
    case AMDFAM10H_BARCELONA:
      return "amdfam10";
    case AMDFAM10H_SHANGHAI:
      return "amdfam10";
    case AMDFAM10H_ISTANBUL:
      return "amdfam10";
    case AMDFAM15H_BDVER1:
      return "bdver2";
    case AMDFAM15H_BDVER2:
      return "bdver2";
    case AMDFAM15H_BDVER3:
      return "bdver3";
    case AMDFAM15H_BDVER4:
      return "bdver4";
    case AMDFAM17H_ZNVER1:
      return "znver2";
    case INTEL_COREI7_IVYBRIDGE:
      return "ivybridge";
    case INTEL_COREI7_HASWELL:
      return "haswell";
    case INTEL_COREI7_BROADWELL:
      return "broadwell";
    case INTEL_COREI7_SKYLAKE:
      return "skylake";
    case INTEL_COREI7_SKYLAKE_AVX512:
      return "skylake-avx512";
    case INTEL_COREI7_CANNONLAKE:
      return "cannonlake";
    case INTEL_COREI7_ICELAKE_CLIENT:
      return "icelake-client";
    case INTEL_COREI7_ICELAKE_SERVER:
      return "icelake-server";
    case AMDFAM17H_ZNVER2:
      return "znver2";
    case INTEL_COREI7_CASCADELAKE:
      return "cascadelake";
    case INTEL_COREI7_TIGERLAKE:
      return "tigerlake";
    case INTEL_COREI7_COOPERLAKE:
      return "cooperlake";
    case INTEL_COREI7_SAPPHIRERAPIDS:
      return "sapphirerapids";
    case INTEL_COREI7_ALDERLAKE:
      return "alderlake";
    case AMDFAM19H_ZNVER3:
      return "znver3";
    case INTEL_COREI7_ROCKETLAKE:
      return "rocketlake";
    case AMDFAM19H_ZNVER4:
      return "znver4";
    case INTEL_COREI7_GRANITERAPIDS:
      return "graniterapids";
    case INTEL_COREI7_GRANITERAPIDS_D:
      return "graniterapids-d";
    case INTEL_COREI7_ARROWLAKE:
      return "arrowlake";
    case INTEL_COREI7_ARROWLAKE_S:
      return "arrowlake-s";
    case INTEL_COREI7_PANTHERLAKE:
      return "pantherlake";
    default:
      return 0;
  }
}
