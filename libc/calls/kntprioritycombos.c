/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/bits/bits.h"
#include "libc/calls/kntprioritycombos.internal.h"
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/nexgen32e/ffs.h"
#include "libc/nt/enum/processcreationflags.h"
#include "libc/nt/enum/threadpriority.h"

hidden const struct NtPriorityCombo kNtPriorityCombos[] = {
    {-20, ffs(kNtHighPriorityClass), kNtThreadPriorityHighest, 15},
    {-18, ffs(kNtHighPriorityClass), kNtThreadPriorityTimeCritical, 15},
    {-17, ffs(kNtNormalPriorityClass), kNtThreadPriorityTimeCritical, 15},
    {-15, ffs(kNtIdlePriorityClass), kNtThreadPriorityTimeCritical, 15},
    {-13, ffs(kNtHighPriorityClass), kNtThreadPriorityAboveNormal, 14},
    {-11, ffs(kNtHighPriorityClass), kNtThreadPriorityNormal, 13},
    {-9, ffs(kNtHighPriorityClass), kNtThreadPriorityBelowNormal, 12},
    {-7, ffs(kNtNormalPriorityClass), kNtThreadPriorityHighest, 11},
    {-5, ffs(kNtHighPriorityClass), kNtThreadPriorityLowest, 11},
    {-3, ffs(kNtNormalPriorityClass), kNtThreadPriorityAboveNormal, 10},
    {-1, ffs(kNtNormalPriorityClass), kNtThreadPriorityHighest, 9},
    {0, ffs(kNtNormalPriorityClass), kNtThreadPriorityNormal, 9},
    {1, ffs(kNtNormalPriorityClass), kNtThreadPriorityAboveNormal, 8},
    {2, ffs(kNtNormalPriorityClass), kNtThreadPriorityBelowNormal, 8},
    {3, ffs(kNtNormalPriorityClass), kNtThreadPriorityNormal, 7},
    {4, ffs(kNtNormalPriorityClass), kNtThreadPriorityLowest, 7},
    {5, ffs(kNtIdlePriorityClass), kNtThreadPriorityHighest, 6},
    {6, ffs(kNtNormalPriorityClass), kNtThreadPriorityBelowNormal, 6},
    {7, ffs(kNtIdlePriorityClass), kNtThreadPriorityAboveNormal, 5},
    {9, ffs(kNtNormalPriorityClass), kNtThreadPriorityLowest, 5},
    {11, ffs(kNtIdlePriorityClass), kNtThreadPriorityNormal, 4},
    {13, ffs(kNtIdlePriorityClass), kNtThreadPriorityBelowNormal, 3},
    {15, ffs(kNtIdlePriorityClass), kNtThreadPriorityLowest, 2},
    {17, ffs(kNtHighPriorityClass), kNtThreadPriorityIdle, 1},
    {18, ffs(kNtNormalPriorityClass), kNtThreadPriorityIdle, 1},
    {19, ffs(kNtIdlePriorityClass), kNtThreadPriorityIdle, 1},
};

hidden const unsigned kNtPriorityCombosLen = ARRAYLEN(kNtPriorityCombos);
