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
#include "third_party/xed/x86.h"

/**
 * Xed error code names.
 *
 *   puts(indexdoublenulstring(kXedErrorNames, xedd->op.error));
 *
 * @see XedError
 */
const char kXedErrorNames[] = "\
none\0\
buffer too short\0\
general error\0\
invalid for chip\0\
bad register\0\
bad lock prefix\0\
bad rep prefix\0\
bad legacy prefix\0\
bad rex prefix\0\
bad evex ubit\0\
bad map\0\
bad evex v prime\0\
bad evex z no masking\0\
no output pointer\0\
no agen call back registered\0\
bad memop index\0\
callback problem\0\
gather regs\0\
instr too long\0\
invalid mode\0\
bad evex ll\0\
unimplemented\0\
";
