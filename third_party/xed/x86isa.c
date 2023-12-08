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
#include "third_party/xed/x86.h"
#include "third_party/xed/x86isa.h"

asm(".ident\t\"\\n\\n\
Xed (Apache 2.0)\\n\
Copyright 2018 Intel Corporation\\n\
Copyright 2019 Justine Alexandra Roberts Tunney\\n\
Modifications: Trimmed down to 3kb [2019-03-22 jart]\"");
asm(".include \"libc/disclaimer.inc\"");

bool xed_isa_set_is_valid_for_chip(int isa_set, int chip) {
  unsigned n, r;
  n = isa_set / 64;
  r = isa_set - (64 * n);
  return !!(kXedChipFeatures[chip][n] & (1ul << r));
}

bool xed_test_chip_features(struct XedChipFeatures *p, int isa_set) {
  unsigned n, r;
  n = isa_set / 64;
  r = isa_set - (64 * n);
  return !!(p->f[n] & (1ul << r));
}

void xed_get_chip_features(struct XedChipFeatures *p, int chip) {
  if (p) {
    if (chip < XED_CHIP_LAST) {
      p->f[0] = kXedChipFeatures[chip][0];
      p->f[1] = kXedChipFeatures[chip][1];
      p->f[2] = kXedChipFeatures[chip][2];
    } else {
      p->f[0] = 0;
      p->f[1] = 0;
      p->f[2] = 0;
    }
  }
}
