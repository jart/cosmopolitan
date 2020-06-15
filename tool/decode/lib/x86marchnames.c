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
#include "libc/nexgen32e/x86info.h"
#include "tool/decode/lib/x86idnames.h"

const struct IdName kX86MarchNames[] = {
    {X86_MARCH_UNKNOWN, "Unknown"},
    {X86_MARCH_CORE2, "Core 2"},
    {X86_MARCH_NEHALEM, "Nehalem"},
    {X86_MARCH_WESTMERE, "Westmere"},
    {X86_MARCH_SANDYBRIDGE, "Sandybridge"},
    {X86_MARCH_IVYBRIDGE, "Ivybridge"},
    {X86_MARCH_HASWELL, "Haswell"},
    {X86_MARCH_BROADWELL, "Broadwell"},
    {X86_MARCH_SKYLAKE, "Skylake"},
    {X86_MARCH_KABYLAKE, "Kabylake"},
    {X86_MARCH_CANNONLAKE, "Cannonlake"},
    {X86_MARCH_ICELAKE, "Icelake"},
    {X86_MARCH_TIGERLAKE, "Tigerlake"},
    {X86_MARCH_BONNELL, "Bonnell"},
    {X86_MARCH_SALTWELL, "Saltwell"},
    {X86_MARCH_SILVERMONT, "Silvermont"},
    {X86_MARCH_AIRMONT, "Airmont"},
    {X86_MARCH_GOLDMONT, "Goldmont"},
    {X86_MARCH_GOLDMONTPLUS, "Goldmont Plus"},
    {X86_MARCH_TREMONT, "Tremont"},
    {X86_MARCH_KNIGHTSLANDING, "Knights Landing"},
    {X86_MARCH_KNIGHTSMILL, "Knights Mill"},
    {0, 0},
};
