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
