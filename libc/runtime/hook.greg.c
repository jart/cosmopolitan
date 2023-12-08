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
#include "ape/sections.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"

#ifdef __x86_64__
typedef uint8_t code_t;
#elif defined(__aarch64__)
typedef uint32_t code_t;
#else
#error "unsupported architecture"
#endif

static privileged bool IsVirginFunction(const code_t *func) {
#ifdef __x86_64__
  long i;
  // function must be preceeded by 9 nops
  for (i = -9; i < 0; ++i) {
    if (func[i] != 0x90) return false;
  }
  // function must start with `nop nop` or `xchg %ax,%ax`
  if (func[0] == 0x90 && func[1] == 0x90) return true;
  if (func[0] == 0x66 && func[1] == 0x90) return true;
  return false;
#elif defined(__aarch64__)
  long i;
  // function must be preceeded by 6 nops
  for (i = -6; i < 0; ++i) {
    if (func[i] != 0xd503201f) return false;
  }
  // function must start with one nop
  return func[0] == 0xd503201f;
#endif
}

static privileged void HookFunction(code_t *func, void *dest) {
  long dp;
#ifdef __x86_64__
  dp = (intptr_t)dest - (intptr_t)(func - 7 + 5);
  if (!(INT32_MIN <= dp && dp <= INT32_MAX)) return;
  // emit `ud2` signature for safety and checkability
  func[-9] = 0x0f;
  func[-8] = 0x0b;
  // emit `call dest` instruction
  func[-7] = 0xe8;
  func[-6] = dp;
  func[-5] = dp >> 8;
  func[-4] = dp >> 16;
  func[-3] = dp >> 24;
  // emit `jmp +2` instruction to re-enter hooked function
  func[-2] = 0xeb;
  func[-1] = 0x02;
  // emit `jmp -2` instruction to enter detour
  func[+0] = 0xeb;
  func[+1] = -7 - 2;
#elif defined(__aarch64__)
  dp = (code_t *)dest - (func - 3);
  if (!(-33554432 <= dp && dp <= +33554431)) return;
  func[-6] = 0xd4200000 | (31337 << 5);       // brk #31337
  func[-5] = 0xa9bf7bfd;                      // stp x29,x30,[sp, #-16]!
  func[-4] = 0x910003fd;                      // mov x29,sp
  func[-3] = 0x94000000 | (dp & 0x03ffffff);  // bl  dest
  func[-2] = 0xa8c17bfd;                      // ldp x29,x30,[sp], #16
  func[-1] = 0x14000000 | (+2 & 0x03ffffff);  // b  +1
  func[+0] = 0x14000000 | (-5 & 0x03ffffff);  // b  -5
#endif
}

/**
 * Rewrites code in memory to hook function calls.
 *
 * On x86_64 you need the compiler flag:
 *
 *     -fpatchable-function-entry=11,9
 *
 * On Aarch64 you need the compiler flag:
 *
 *     -fpatchable-function-entry=7,6
 *
 * This function can currently only be called once.
 *
 * @param dest is the address of the target function, which all hookable
 *     functions shall be reprogrammed to call from their epilogues; and
 *     must be sufficiently close in memory to the the program image, in
 *     order to meet ISA displacement requirements
 * @param st can be obtained using `GetSymbolTable()`
 * @see ape/ape.lds
 */
privileged int __hook(void *dest, struct SymbolTable *st) {
  long i;
  code_t *p, *pe;
  intptr_t lowest;
  if (!st) return -1;
  __morph_begin();
  lowest = MAX((intptr_t)__executable_start, (intptr_t)_ereal);
  for (i = 0; i < st->count; ++i) {
    if (st->symbols[i].x < 9) continue;
    if (st->addr_base + st->symbols[i].x < lowest) continue;
    if (st->addr_base + st->symbols[i].y >= (intptr_t)__privileged_start) break;
    p = (code_t *)((char *)st->addr_base + st->symbols[i].x);
    pe = (code_t *)((char *)st->addr_base + st->symbols[i].y);
    if (pe - p < 2) continue;
    if (IsVirginFunction(p)) {
      // kprintf("hooking %t\n", p);
      HookFunction(p, dest);
    } else {
      // kprintf("can't hook %t at %lx\n", p, p);
    }
  }
  __morph_end();
  return 0;
}
