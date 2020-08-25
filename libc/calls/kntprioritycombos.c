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
#include "libc/calls/kntprioritycombos.h"
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/nt/enum/processcreationflags.h"
#include "libc/nt/enum/threadpriority.h"

#define FFS(x) __builtin_ffs(x)

const struct NtPriorityCombo kNtPriorityCombos[] = {
    {-20, FFS(kNtHighPriorityClass), kNtThreadPriorityHighest, 15},
    {-18, FFS(kNtHighPriorityClass), kNtThreadPriorityTimeCritical, 15},
    {-17, FFS(kNtNormalPriorityClass), kNtThreadPriorityTimeCritical, 15},
    {-15, FFS(kNtIdlePriorityClass), kNtThreadPriorityTimeCritical, 15},
    {-13, FFS(kNtHighPriorityClass), kNtThreadPriorityAboveNormal, 14},
    {-11, FFS(kNtHighPriorityClass), kNtThreadPriorityNormal, 13},
    {-9, FFS(kNtHighPriorityClass), kNtThreadPriorityBelowNormal, 12},
    {-7, FFS(kNtNormalPriorityClass), kNtThreadPriorityHighest, 11},
    {-5, FFS(kNtHighPriorityClass), kNtThreadPriorityLowest, 11},
    {-3, FFS(kNtNormalPriorityClass), kNtThreadPriorityAboveNormal, 10},
    {-1, FFS(kNtNormalPriorityClass), kNtThreadPriorityHighest, 9},
    {0, FFS(kNtNormalPriorityClass), kNtThreadPriorityNormal, 9},
    {1, FFS(kNtNormalPriorityClass), kNtThreadPriorityAboveNormal, 8},
    {2, FFS(kNtNormalPriorityClass), kNtThreadPriorityBelowNormal, 8},
    {3, FFS(kNtNormalPriorityClass), kNtThreadPriorityNormal, 7},
    {4, FFS(kNtNormalPriorityClass), kNtThreadPriorityLowest, 7},
    {5, FFS(kNtIdlePriorityClass), kNtThreadPriorityHighest, 6},
    {6, FFS(kNtNormalPriorityClass), kNtThreadPriorityBelowNormal, 6},
    {7, FFS(kNtIdlePriorityClass), kNtThreadPriorityAboveNormal, 5},
    {9, FFS(kNtNormalPriorityClass), kNtThreadPriorityLowest, 5},
    {11, FFS(kNtIdlePriorityClass), kNtThreadPriorityNormal, 4},
    {13, FFS(kNtIdlePriorityClass), kNtThreadPriorityBelowNormal, 3},
    {15, FFS(kNtIdlePriorityClass), kNtThreadPriorityLowest, 2},
    {17, FFS(kNtHighPriorityClass), kNtThreadPriorityIdle, 1},
    {18, FFS(kNtNormalPriorityClass), kNtThreadPriorityIdle, 1},
    {19, FFS(kNtIdlePriorityClass), kNtThreadPriorityIdle, 1},
};

const unsigned kNtPriorityCombosLen = ARRAYLEN(kNtPriorityCombos);
