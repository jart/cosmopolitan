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
#include "libc/alg/bisect.internal.h"
#include "libc/nexgen32e/x86info.h"

static int CmpX86ProcModelKey(const struct X86ProcessorModel *a,
                              const struct X86ProcessorModel *b) {
  return a->key > b->key ? 1 : a->key < b->key ? -1 : 0;
}

/**
 * Identifies microarchitecture of host processor.
 *
 * @param key can be kX86ProcessorModelKey for host info
 * @see https://en.wikichip.org/wiki/intel/cpuid
 * @see https://a4lg.com/tech/x86/database/x86-families-and-models.en.html
 */
const struct X86ProcessorModel *getx86processormodel(short key) {
  return bisect(&(struct X86ProcessorModel){key}, kX86ProcessorModels,
                kX86ProcessorModelCount, sizeof(struct X86ProcessorModel),
                (void *)CmpX86ProcModelKey, NULL);
}
