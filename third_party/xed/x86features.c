/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2018 Intel Corporation                                             │
│ Copyright 2019 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Licensed under the Apache License, Version 2.0 (the "License");              │
│ you may not use this file except in compliance with the License.             │
│ You may obtain a copy of the License at                                      │
│                                                                              │
│     http://www.apache.org/licenses/LICENSE-2.0                               │
│                                                                              │
│ Unless required by applicable law or agreed to in writing, software          │
│ distributed under the License is distributed on an "AS IS" BASIS,            │
│ WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.     │
│ See the License for the specific language governing permissions and          │
│ limitations under the License.                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/xed/x86isa.h"

asm(".ident\t\"\\n\\n\
Xed (Apache 2.0)\\n\
Copyright 2018 Intel Corporation\\n\
Copyright 2019 Justine Alexandra Roberts Tunney\\n\
Modifications: Trimmed down to 3kb [2019-03-22 jart]\"");
asm(".include \"libc/disclaimer.inc\"");

/**
 * Mapping of enum XedChip -> bitset<enum XedIsaSet>.
 *
 * See related APIs, e.g. xed_isa_set_is_valid_for_chip().
 *
 * This information can be reproduced by building Xed and running the C
 * preprocessor on xed-chip-features-table.c (see xed-chips.txt) which
 * turns several thousand lines of non-evolving code into fifty. For
 * example, 0x2800000ul was calculated as: 1UL<<(XED_ISA_SET_I86-64) |
 * 1UL<<(XED_ISA_SET_LAHF-64).
 */
const uint64_t kXedChipFeatures[XED_CHIP_LAST][3] = {
  {0,                   0,                  0,     },
  {0,                   0x02800000,         0,     },  /*I86*/
  {0,                   0x02800000,         0x02000},  /*I86FP*/
  {0,                   0x02820000,         0,     },  /*I186*/
  {0,                   0x02820000,         0x02000},  /*I186FP*/
  {0,                   0x028a0000,         0x02000},  /*I286REAL*/
  {0,                   0x028e0000,         0x02000},  /*I286*/
  {0,                   0x028e0000,         0x02000},  /*I2186FP*/
  {0,                   0x028a0000,         0x02000},  /*I386REAL*/
  {0,                   0x029e0000,         0x02000},  /*I386*/
  {0,                   0x029e0000,         0x02000},  /*I386FP*/
  {0,                   0x02ca0000,         0x02000},  /*I486REAL*/
  {0,                   0x02fe0000,         0x02000},  /*I486*/
  {0,                   0x2002ca0000,       0x02000},  /*PENTIUMREAL*/
  {0,                   0x2002fe0000,       0x02000},  /*PENTIUM*/
  {0,                   0x2002fe0000,       0x02000},  /*QUARK*/
  {0,                   0x402002ca0000,     0x02000},  /*PEN..MMXREAL*/
  {0,                   0x403002fe0000,     0x02000},  /*PENTIUMMMX*/
  {0,                   0x402002ca0000,     0x02000},  /*ALLREAL*/
  {0,                   0x492002fe0c80,     0x02000},  /*PENTIUMPRO*/
  {0,                   0x493002fe4c80,     0x02000},  /*PENTIUM2*/
  {0,                   0x200493002fe4c80,  0x02006},  /*PENTIUM3*/
  {0,                   0xe00493202fe4c90,  0x02006},  /*PENTIUM4*/
  {0,                   0x3e00493216fecd90, 0x02006},  /*P4PRESCOTT*/
  {0,                   0x3e00493214fecd90, 0x02000},  /*P4PR..NOLAHF*/
  {0,                   0x3e00493216fecd90, 0x02406},  /*P4PRESC..VTX*/
  {0,                   0x3f00493216fecd90, 0x0241e},  /*CORE2*/
  {0,                   0x7f00493216fecd90, 0x0241e},  /*PENRYN*/
  {0,                   0x7f00493216fecd90, 0x0a41e},  /*PENRYN_E*/
  {0,                   0xff0249b216fecd90, 0x0241e},  /*NEHALEM*/
  {8,                   0xff0249b616fecd90, 0x0241e},  /*WESTMERE*/
  {0,                   0x3e00493256fecd90, 0x0241e},  /*BONNELL*/
  {0,                   0x3e00493256fecd90, 0x0241e},  /*SALTWELL*/
  {8,                   0xff02cbb656fecd90, 0x0241e},  /*SILVERMONT*/
  {18,                  0x00000a0020002040, 0x04061},  /*AMD*/
  {8,                   0xffc7cbb756fecd98, 0x7a41e},  /*GOLDMONT*/
  {8,                   0xffd7fbb756fecd98, 0x7a41e},  /*GOLDMONTPLUS*/
  {8,                   0xfff7fbb7d6ffcdbc, 0x7ac1e},  /*TREMONT*/
  {0x2000000000000028,  0xff0249b616fecd90, 0x2a41e},  /*SANDYBRIDGE*/
  {0x2000000000000028,  0xff06c9b616fecf90, 0x2a41e},  /*IVYBRIDGE*/
  {0xa0000000000000e8,  0xff0ec9b65ffedf91, 0x2a51e},  /*HASWELL*/
  {0xa0000000000000ec,  0xff8fcbb65ffedf91, 0x2a51e},  /*BROADWELL*/
  {0xa0000000000000ec,  0xff9fcbb75ffedf99, 0x7a51e},  /*SKYLAKE*/
  {0xa00000003f3fffec,  0xff9fcbf75ffedfb9, 0x7a51e},  /*SKYL..SERVER*/
  {0xa07000003f3fffec,  0xff9fcbf75ffedfb9, 0x7a51e},  /*CASCADE_LAKE*/
  {0xa00000007ac080ec,  0xff07cdb65efedf91, 0x2a41e},  /*KNL*/
  {0xb0000003fac080ec,  0xff07cdb65efedf91, 0x2a51e},  /*KNM*/
  {0xa00e07003f3fffec,  0xffdfcbf75ffedf99, 0x7a51e},  /*CANNONLAKE*/
  {0xfffffffc3f3fffec,  0xffdfebf75fffdfb9, 0x7a79e},  /*ICELAKE*/
  {0xfffffffc3f3fffec,  0xffffebff5fffdfb9, 0x7b79e},  /*ICEL..SERVER*/
  {0xfffffffc3f3fffec,  0xffdffbf75fffdfbb, 0x7a79e},  /*FUTURE*/
  {0xfffffffffffffffe,  0xffffffffffffffff, 0x7ffff}   /*ALL*/
};
