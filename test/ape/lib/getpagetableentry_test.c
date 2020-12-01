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
#include "ape/lib/pc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(getpagetableentry, testLowestAddress) {
  static struct PageTable pml4t;
  static struct PageTable stack[3];
  uint64_t ptsp = (uintptr_t)&stack + sizeof(stack);
  memset(&pml4t, 0, sizeof(pml4t));
  memset(&stack, 0, sizeof(stack));
  uint64_t vaddr = 0;
  uint64_t paddr = 0x31337000;
  *__getpagetableentry(vaddr, 3, &pml4t, &ptsp) = paddr | PAGE_V;
  EXPECT_EQ(&stack[2].p[0], pml4t.p[0] & PAGE_TA);    /* pml4t → pdpt */
  EXPECT_EQ(&stack[1].p[0], stack[2].p[0] & PAGE_TA); /* pdpt → pdt */
  EXPECT_EQ(&stack[0].p[0], stack[1].p[0] & PAGE_TA); /* pdt → pd */
  EXPECT_EQ(stack[0].p[0] & PAGE_TA, paddr);          /* page */
  EXPECT_EQ(&stack, ptsp);
  EXPECT_TRUE(pml4t.p[0] & PAGE_V);
  EXPECT_TRUE(stack[2].p[0] & PAGE_V);
  EXPECT_TRUE(stack[1].p[0] & PAGE_V);
  EXPECT_TRUE(stack[0].p[0] & PAGE_V);
  EXPECT_FALSE(stack[0].p[1] & PAGE_V);
}

TEST(getpagetableentry, testHigherAddress) {
  static struct PageTable pml4t;
  static struct PageTable stack[3];
  uint64_t ptsp = (uintptr_t)&stack + sizeof(stack);
  memset(&pml4t, 0, sizeof(pml4t));
  memset(&stack, 0, sizeof(stack));
  uint64_t vaddr = 0x133731337000;
  uint64_t paddr = 0x123000;
  *__getpagetableentry(vaddr, 3, &pml4t, &ptsp) = paddr | PAGE_V;
  EXPECT_EQ(&stack[2].p[0], pml4t.p[38] & PAGE_TA);     /* pml4t → pdpt */
  EXPECT_EQ(&stack[1].p[0], stack[2].p[220] & PAGE_TA); /* pdpt → pdt */
  EXPECT_EQ(&stack[0].p[0], stack[1].p[393] & PAGE_TA); /* pdt → pd */
  EXPECT_EQ(stack[0].p[311] & PAGE_TA, paddr);          /* page */
  EXPECT_EQ(&stack, ptsp);
  EXPECT_TRUE(pml4t.p[38] & PAGE_V);
  EXPECT_TRUE(stack[2].p[220] & PAGE_V);
  EXPECT_TRUE(stack[1].p[393] & PAGE_V);
  EXPECT_TRUE(stack[0].p[311] & PAGE_V);
  EXPECT_FALSE(stack[0].p[0] & PAGE_V);
}
