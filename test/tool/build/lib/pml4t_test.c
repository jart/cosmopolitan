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
#include "libc/log/check.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "tool/build/lib/memory.h"
#include "tool/build/lib/pml4t.h"

struct Unmapped {
  size_t i;
  struct UnmappedEntry {
    void *addr;
    size_t size;
  } p[8];
};

struct Unmapped unmapped;
uint64_t *pt2, *pt3, *pt4;
pml4t_t cr3;

void *NewPage(void) {
  void *p;
  p = tmemalign(4096, 4096);
  CHECK_ALIGNED(4096, p);
  memset(p, 0, 4096);
  return p;
}

void FreePage(void *p) {
  tfree(p);
}

int FakeMunmap(void *addr, size_t size) {
  return 0;
}

int MockMunmap(void *addr, size_t size) {
  CHECK_LT(unmapped.i, ARRAYLEN(unmapped.p));
  unmapped.p[unmapped.i].addr = addr;
  unmapped.p[unmapped.i].size = size;
  unmapped.i++;
  return 0;
}

void SetUp(void) {
  memset(cr3, 0, sizeof(cr3));
}

void TearDown(void) {
  unmapped.i = 0;
  FreePml4t(cr3, -0x800000000000, 0x800000000000, FreePage, FakeMunmap);
}

TEST(pml4t, testHighestAddress) {
  ASSERT_NE(-1,
            RegisterPml4t(cr3, 0x7fffffffe000, 0x3133700000, 0x2000, NewPage));
  ASSERT_TRUE(IsValidPage(cr3[255]));
  pt2 = UnmaskPageAddr(cr3[255]);
  ASSERT_TRUE(IsValidPage(pt2[511]));
  pt3 = UnmaskPageAddr(pt2[511]);
  ASSERT_TRUE(IsValidPage(pt3[511]));
  pt4 = UnmaskPageAddr(pt3[511]);
  ASSERT_TRUE(IsValidPage(pt4[510]));
  ASSERT_TRUE(IsValidPage(pt4[511]));
  EXPECT_EQ(0x3133700000, UnmaskPageAddr(pt4[510]));
  EXPECT_EQ(0x3133701000, UnmaskPageAddr(pt4[511]));
}

TEST(pml4t, testLowestAddress) {
  ASSERT_NE(-1,
            RegisterPml4t(cr3, -0x800000000000, 0x31337000, 0x2000, NewPage));
  ASSERT_TRUE(IsValidPage(cr3[256]));
  pt2 = UnmaskPageAddr(cr3[256]);
  ASSERT_TRUE(IsValidPage(pt2[0]));
  pt3 = UnmaskPageAddr(pt2[0]);
  ASSERT_TRUE(IsValidPage(pt3[0]));
  pt4 = UnmaskPageAddr(pt3[0]);
  ASSERT_TRUE(IsValidPage(pt4[0]));
  ASSERT_TRUE(IsValidPage(pt4[1]));
}

TEST(pml4t, testOverlapsExistingRegistration_overwritesRegistration) {
  ASSERT_NE(-1,
            RegisterPml4t(cr3, 0x7fffffffe000, 0x31337000, 0x1000, NewPage));
  ASSERT_TRUE(IsValidPage(cr3[255]));
  pt2 = UnmaskPageAddr(cr3[255]);
  ASSERT_TRUE(IsValidPage(pt2[511]));
  pt3 = UnmaskPageAddr(pt2[511]);
  ASSERT_TRUE(IsValidPage(pt3[511]));
  pt4 = UnmaskPageAddr(pt3[511]);
  EXPECT_TRUE(IsValidPage(pt4[510]));
  EXPECT_EQ(0x31337000, UnmaskPageAddr(pt4[510]));
  ASSERT_NE(-1, RegisterPml4t(cr3, 0x7fffffffe000, 0x31337000 + 0x1000, 0x1000,
                              NewPage));
  EXPECT_TRUE(IsValidPage(pt4[510]));
  EXPECT_EQ(0x31337000 + 0x1000, UnmaskPageAddr(pt4[510]));
}

TEST(pml4t, testFindPml4t_holeTooSmall_skipsOver) {
  ASSERT_NE(-1, RegisterPml4t(cr3, 0x700000000, 0, 0x1000, NewPage));
  ASSERT_NE(-1, RegisterPml4t(cr3, 0x700005000, 0, 0x1000, NewPage));
  ASSERT_EQ(0x700001000, FindPml4t(cr3, 0x700000000, 0x01000, NewPage));
  ASSERT_EQ(0x700006000, FindPml4t(cr3, 0x700000000, 0x10000, NewPage));
}

TEST(pml4t, testFreePmlt) {
  ASSERT_NE(-1, RegisterPml4t(cr3, 0x000005000, 0x123000, 0x2000, NewPage));
  ASSERT_NE(-1, RegisterPml4t(cr3, 0x000000000, 0x321000, 0x1000, NewPage));
  ASSERT_NE(-1, FreePml4t(cr3, -0x800000000000, 0x800000000000, FreePage,
                          MockMunmap));
  ASSERT_EQ(2, unmapped.i);
  EXPECT_EQ(0x321000, unmapped.p[0].addr);
  EXPECT_EQ(0x1000, unmapped.p[0].size);
  EXPECT_EQ(0x123000, unmapped.p[1].addr);
  EXPECT_EQ(0x2000, unmapped.p[1].size);
}

BENCH(pml4t, bench) {
  EZBENCH2("RegisterPml4t 1mb fresh",
           FreePml4t(cr3, -0x800000000000, 0x800000000000, free, FakeMunmap),
           RegisterPml4t(cr3, 0x00100000, 0x31337000, 0x00100000, MallocPage));
  EZBENCH2("RegisterPml4t 1gb fresh",
           FreePml4t(cr3, -0x800000000000, 0x800000000000, free, FakeMunmap),
           RegisterPml4t(cr3, 0x40000000, 0x31337000, 0x40000000, MallocPage));
  EZBENCH2("RegisterPml4t 1mb overwrite", donothing,
           RegisterPml4t(cr3, 0x00100000, 0x31337000, 0x00100000, MallocPage));
  EZBENCH2("RegisterPml4t 1gb overwrite", donothing,
           RegisterPml4t(cr3, 0x40000000, 0x31337000, 0x40000000, MallocPage));
  FreePml4t(cr3, -0x800000000000, 0x800000000000, free, FakeMunmap);
}
