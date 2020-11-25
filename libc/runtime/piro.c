/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=8 sts=2 sw=2 fenc=utf-8                                :vi│
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
╠──────────────────────────────────────────────────────────────────────────────╣
│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀█░█▀█░▀█▀░█░█░█▀█░█░░░█░░░█░█░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀█░█░▄░░█░░█░█░█▀█░█░░░█░░░▀█▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░▀░▀░▀▀▀░░▀░░▀▀▀░▀░▀░▀▀▀░▀▀▀░░▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀█░█▀█░█▀█░▀█▀░█▀█░█▀█░█░░░█▀▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀▀░█ █░██▀░░█░░█▀█░█▀█░█░░░█▀▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░▀░░░▀▀▀░▀░▀░░▀░░▀░▀░▀▀▀░▀▀▀░▀▀▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀▀░█░█░█▀▀░█▀█░█░█░▀█▀░█▀█░█▀█░█░░█▀▀░░░░░░░░░░░░░░░░░░░░░░░░▄▄░░░▐█░░│
│░░░░░░░█▀▀░▄▀▄░█▀▀░█░▄░█░█░░█░░█▀█░█▀█░█░░█▀▀░░░░░░░░░░░░▄▄▄░░░▄██▄░░█▀░░░█░▄░│
│░░░░░░░▀▀▀░▀░▀░▀▀▀░▀▀▀░▀▀▀░░▀░░▀░▀░▀▀▀░▀▀░▀▀▀░░░░░░░░░░▄██▀█▌░██▄▄░░▐█▀▄░▐█▀░░│
│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░▐█▀▀▌░░░▄▀▌░▌░█░▌░░▌░▌░░│
╠──────────────────────────────────────────────────────▌▀▄─▐──▀▄─▐▄─▐▄▐▄─▐▄─▐▄─│
│ αcτµαlly pδrταblε εxεcµταblε § post-initialization read-only                 │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/bits/safemacros.internal.h"
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/prot.h"

#define getaddr(section) ((intptr_t)weakaddr(section))

static textstartup void _piro_protect(intptr_t start, intptr_t end, int prot) {
  ssize_t len = end - start;
  if (len > 0 && start && start % PAGESIZE == 0 && len % PAGESIZE == 0) {
    if (mprotect((void *)(unsigned long)start, len, prot) == -1) abort();
  }
}

/**
 * Protects memory initialized at startup.
 * e.g. function hooks, unpacked data structures, etc.
 *
 * This is only performed for executables of nontrivial size. It won't
 * break the build if the αpε linker script wasn't used. Test code is
 * protected too, so we don't end up like Knight Capital.
 *
 * @param prot can have PROT_{NONE,READ,WRITE,EXEC}
 * @see ape/ape.lds
 * @see libc/_start.S
 */
textstartup void _piro(int prot) {
  if (getaddr("main") < getaddr("__test_start")) {
    _piro_protect(getaddr("__test_start"), getaddr("__test_end"), PROT_NONE);
  }
  _piro_protect(getaddr("__ro"), getaddr("_etext"), PROT_READ);
  _piro_protect(getaddr("__piro_start"), getaddr("__piro_end"), prot);
}
