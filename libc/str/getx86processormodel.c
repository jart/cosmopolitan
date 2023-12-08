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
#include "libc/mem/bisect.internal.h"
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
