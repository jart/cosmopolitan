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

const struct IdName kX86GradeNames[] = {
    {X86_GRADE_UNKNOWN, "Unknown"}, {X86_GRADE_APPLIANCE, "Appliance"},
    {X86_GRADE_MOBILE, "Mobile"},   {X86_GRADE_TABLET, "Tablet"},
    {X86_GRADE_DESKTOP, "Desktop"}, {X86_GRADE_CLIENT, "Client"},
    {X86_GRADE_DENSITY, "Density"}, {X86_GRADE_SERVER, "Server"},
    {X86_GRADE_SCIENCE, "Science"}, {0, 0},
};
