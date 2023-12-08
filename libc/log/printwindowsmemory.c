/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/fmt/conv.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/nt/enum/memflags.h"
#include "libc/nt/memory.h"
#include "libc/nt/struct/memorybasicinformation.h"
#include "libc/str/str.h"

static const struct DescribeFlags kNtMemState[] = {
    {kNtMemCommit, "Commit"},    //
    {kNtMemFree, "Free"},        //
    {kNtMemReserve, "Reserve"},  //
};

static const char *DescribeNtMemState(char buf[64], uint32_t x) {
  return DescribeFlags(buf, 64, kNtMemState, ARRAYLEN(kNtMemState), "kNtMem",
                       x);
}

static const struct DescribeFlags kNtMemType[] = {
    {kNtMemImage, "Image"},      //
    {kNtMemMapped, "Mapped"},    //
    {kNtMemPrivate, "Private"},  //
};

static const char *DescribeNtMemType(char buf[64], uint32_t x) {
  return DescribeFlags(buf, 64, kNtMemType, ARRAYLEN(kNtMemType), "kNtMem", x);
}

/**
 * Prints to stderr all memory mappings that exist according to WIN32.
 *
 * The `high` and `size` parameters may optionally be specified so that
 * memory mappings which overlap `[high,high+size)` will get printed in
 * ANSI bold red text.
 */
void PrintWindowsMemory(const char *high, size_t size) {
  char *p, b[5][64];
  struct NtMemoryBasicInformation mi;
  kprintf("%-12s %-12s %10s %16s %16s %32s %32s\n", "Allocation", "BaseAddress",
          "RegionSize", "State", "Type", "AllocationProtect", "Protect");
  for (p = 0;; p = (char *)mi.BaseAddress + mi.RegionSize) {
    const char *start, *stop;
    bzero(&mi, sizeof(mi));
    if (!VirtualQuery(p, &mi, sizeof(mi))) break;
    sizefmt(b[0], mi.RegionSize, 1024);
    if (MAX(high, (char *)mi.BaseAddress) <
        MIN(high + size, (char *)mi.BaseAddress + mi.RegionSize)) {
      start = "\e[1;31m";
      stop = "\e[0m";
    } else {
      start = "";
      stop = "";
    }
    kprintf("%s%.12lx %.12lx %10s %16s %16s %32s %32s%s\n", start,
            mi.AllocationBase, mi.BaseAddress, b[0],
            DescribeNtMemState(b[1], mi.State),
            DescribeNtMemType(b[2], mi.Type),
            (DescribeNtPageFlags)(b[3], mi.AllocationProtect),
            (DescribeNtPageFlags)(b[4], mi.Protect), stop);
  }
}
