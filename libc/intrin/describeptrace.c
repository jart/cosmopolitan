/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/fmt/itoa.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/sysv/consts/ptrace.h"

const char *(DescribePtrace)(char buf[12], int x) {
  if (x == -1) return "-1";
  if (x == PTRACE_TRACEME) return "PTRACE_TRACEME";
  if (x == PTRACE_PEEKDATA) return "PTRACE_PEEKDATA";
  if (x == PTRACE_GETFPREGS) return "PTRACE_GETFPREGS";
  if (x == PTRACE_PEEKTEXT) return "PTRACE_PEEKTEXT";
  if (x == PTRACE_POKEDATA) return "PTRACE_POKEDATA";
  if (x == PTRACE_PEEKUSER) return "PTRACE_PEEKUSER";
  if (x == PTRACE_POKETEXT) return "PTRACE_POKETEXT";
  if (x == PTRACE_POKEUSER) return "PTRACE_POKEUSER";
  if (x == PTRACE_GETREGS) return "PTRACE_GETREGS";
  if (x == PTRACE_GETREGSET) return "PTRACE_GETREGSET";
  if (x == PTRACE_SETFPREGS) return "PTRACE_SETFPREGS";
  if (x == PTRACE_SETREGS) return "PTRACE_SETREGS";
  if (x == PTRACE_SETREGSET) return "PTRACE_SETREGSET";
  if (x == PTRACE_GETSIGINFO) return "PTRACE_GETSIGINFO";
  if (x == PTRACE_SETSIGINFO) return "PTRACE_SETSIGINFO";
  if (x == PTRACE_PEEKSIGINFO) return "PTRACE_PEEKSIGINFO";
  if (x == PTRACE_GETSIGMASK) return "PTRACE_GETSIGMASK";
  if (x == PTRACE_SETSIGMASK) return "PTRACE_SETSIGMASK";
  if (x == PTRACE_SETOPTIONS) return "PTRACE_SETOPTIONS";
  if (x == PTRACE_GETEVENTMSG) return "PTRACE_GETEVENTMSG";
  if (x == PTRACE_CONT) return "PTRACE_CONT";
  if (x == PTRACE_SINGLESTEP) return "PTRACE_SINGLESTEP";
  if (x == PTRACE_SYSCALL) return "PTRACE_SYSCALL";
  if (x == PTRACE_LISTEN) return "PTRACE_LISTEN";
  if (x == PTRACE_KILL) return "PTRACE_KILL";
  if (x == PTRACE_INTERRUPT) return "PTRACE_INTERRUPT";
  if (x == PTRACE_ATTACH) return "PTRACE_ATTACH";
  if (x == PTRACE_SEIZE) return "PTRACE_SEIZE";
  if (x == PTRACE_SECCOMP_GET_FILTER) return "PTRACE_SECCOMP_GET_FILTER";
  if (x == PTRACE_SECCOMP_GET_METADATA) return "PTRACE_SECCOMP_GET_METADATA";
  if (x == PTRACE_DETACH) return "PTRACE_DETACH";
  FormatInt32(buf, x);
  return buf;
}
