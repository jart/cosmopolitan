//===-- lib/extendhfdf2.c - half -> dubble conversion -------------*- C -*-===//
//
//                The Cosmopolitan Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//

#define SRC_HALF
#define DST_DOUBLE
#include "third_party/compiler_rt/fp16_extend_impl.inc"

COMPILER_RT_ABI dst_t __extendhfdf2(src_t a) {
    return __extendXfYf2__(a);
}
