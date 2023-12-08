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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/macros.internal.h"
#include "libc/nt/enum/memflags.h"
#include "libc/nt/memory.h"
#include "libc/nt/struct/memorybasicinformation.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#if defined(__x86_64__) && SupportsWindows()

/**
 * @fileoverview WIN32 Virtual Memory Layout Dump Utility
 */

static const struct DescribeFlags kNtMemState[] = {
    {kNtMemCommit, "Commit"},    //
    {kNtMemFree, "Free"},        //
    {kNtMemReserve, "Reserve"},  //
};

const char *DescribeNtMemState(char buf[64], uint32_t x) {
  return DescribeFlags(buf, 64, kNtMemState, ARRAYLEN(kNtMemState), "kNtMem",
                       x);
}

static const struct DescribeFlags kNtMemType[] = {
    {kNtMemImage, "Image"},      //
    {kNtMemMapped, "Mapped"},    //
    {kNtMemPrivate, "Private"},  //
};

const char *DescribeNtMemType(char buf[64], uint32_t x) {
  return DescribeFlags(buf, 64, kNtMemType, ARRAYLEN(kNtMemType), "kNtMem", x);
}

int main(int argc, char *argv[]) {
  char *p, b[5][64];
  struct NtMemoryBasicInformation mi;
  if (!IsWindows()) {
    fprintf(stderr, "error: %s is intended for windows\n",
            program_invocation_short_name);
    exit(1);
  }
  printf("%-12s %-12s %10s %16s %16s %32s %32s\n", "Allocation", "BaseAddress",
         "RegionSize", "State", "Type", "AllocationProtect", "Protect");
  for (p = 0;; p = (char *)mi.BaseAddress + mi.RegionSize) {
    bzero(&mi, sizeof(mi));
    if (!VirtualQuery(p, &mi, sizeof(mi))) break;
    sizefmt(b[0], mi.RegionSize, 1024);
    printf("%.12lx %.12lx %10s %16s %16s %32s %32s\n", mi.AllocationBase,
           mi.BaseAddress, b[0], DescribeNtMemState(b[1], mi.State),
           DescribeNtMemType(b[2], mi.Type),
           (DescribeNtPageFlags)(b[3], mi.AllocationProtect),
           (DescribeNtPageFlags)(b[4], mi.Protect));
  }
}

#else
int main(int argc, char *argv[]) {
  fprintf(stderr,
          "virtualquery not supported on this cpu arch or build config\n");
  return 1;
}
#endif /* __x86_64__ && SupportsWindows() */
