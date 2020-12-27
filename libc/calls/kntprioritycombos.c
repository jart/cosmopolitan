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
