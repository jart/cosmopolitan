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
#include "libc/alg/alg.h"
#include "libc/macros.h"
#include "libc/testlib/testlib.h"

STATIC_YOINK("realloc");

TEST(tarjan, empty_doesNothing) {
  int sorted_vertices[1] = {-1};
  int edges[][2] = {{0, 0}};
  int vertex_count = 0;
  int edge_count = 0;
  tarjan(vertex_count, (void *)edges, edge_count, sorted_vertices, NULL, NULL);
  ASSERT_EQ(-1, sorted_vertices[0]);
}

TEST(tarjan, topologicalSort_noCycles) {
  enum VertexIndex { A = 0, B = 1, C = 2, D = 3 };
  const char *const vertices[] = {[A] = "A", [B] = "B", [C] = "C", [D] = "D"};
  int edges[][2] = {{A /* depends on → */, B /* which must come before A */},
                    {A /* depends on → */, C /* which must come before A */},
                    {A /* depends on → */, D /* which must come before A */},
                    {B /* depends on → */, C /* which must come before B */},
                    {B /* depends on → */, D /* which must come before B */}};
  /*
    $ tsort <<EOF
    B A
    C A
    D A
    C B
    D B
    EOF
    C
    D
    B
    A
  */
  int sorted[4], components[4], componentcount;
  ASSERT_EQ(0, tarjan(ARRAYLEN(vertices), (void *)edges, ARRAYLEN(edges),
                      sorted, components, &componentcount));
  EXPECT_EQ(C, sorted[0]);
  EXPECT_EQ(D, sorted[1]);
  EXPECT_EQ(B, sorted[2]);
  EXPECT_EQ(A, sorted[3]);
  ASSERT_EQ(4, componentcount);
  EXPECT_EQ(1, components[0]);
  EXPECT_EQ(2, components[1]);
  EXPECT_EQ(3, components[2]);
  EXPECT_EQ(4, components[3]);
}

TEST(tarjan, testOneBigCycle_isDetected_weDontCareAboutOrderInsideTheCycle) {
  enum VertexIndex { A = 0, B = 1, C = 2, D = 3 };
  const char *const vertices[] = {[A] = "A", [B] = "B", [C] = "C", [D] = "D"};
  /* ┌─────────┐
     └→A→B→C→D─┘ */
  int edges[][2] = {{A /* depends on → */, B /* which must come before A */},
                    {B /* depends on → */, C /* which must come before B */},
                    {C /* depends on → */, D /* which must come before C */},
                    {D /* depends on → */, A /* which must come before D */}};
  int sorted[4], components[4], componentcount;
  ASSERT_EQ(0, tarjan(ARRAYLEN(vertices), (void *)edges, ARRAYLEN(edges),
                      sorted, components, &componentcount));
  ASSERT_EQ(1, componentcount);
  EXPECT_EQ(4, components[0]);
}

TEST(tarjan, testHeaders) {
  enum Headers {
    LIBC_STR_STR,
    LIBC_BITS_BITS,
    LIBC_INTEGRAL,
    LIBC_KEYWORDS,
    LIBC_DCE,
    LIBC_MACROS,
    LIBC_MACROS_CPP,
  };
  const char *const vertices[] = {
      [LIBC_STR_STR] = "libc/str/str.h",
      [LIBC_BITS_BITS] = "libc/bits/bits.h",
      [LIBC_INTEGRAL] = "libc/integral.h",
      [LIBC_KEYWORDS] = "libc/keywords.h",
      [LIBC_DCE] = "libc/dce.h",
      [LIBC_MACROS] = "libc/macros.h",
      [LIBC_MACROS_CPP] = "libc/macros-cpp.inc",
  };
  int edges[][2] = {
      {LIBC_STR_STR, LIBC_BITS_BITS},  {LIBC_STR_STR, LIBC_INTEGRAL},
      {LIBC_STR_STR, LIBC_KEYWORDS},   {LIBC_BITS_BITS, LIBC_DCE},
      {LIBC_BITS_BITS, LIBC_INTEGRAL}, {LIBC_BITS_BITS, LIBC_KEYWORDS},
      {LIBC_BITS_BITS, LIBC_MACROS},   {LIBC_MACROS, LIBC_MACROS_CPP},
  };
  int sorted[ARRAYLEN(vertices)];
  int components[ARRAYLEN(vertices)];
  int componentcount;
  ASSERT_EQ(0, tarjan(ARRAYLEN(vertices), (void *)edges, ARRAYLEN(edges),
                      sorted, components, &componentcount));
  ASSERT_EQ(ARRAYLEN(vertices), componentcount);
  EXPECT_STREQ("libc/dce.h", vertices[sorted[0]]);
  EXPECT_STREQ("libc/integral.h", vertices[sorted[1]]);
  EXPECT_STREQ("libc/keywords.h", vertices[sorted[2]]);
  EXPECT_STREQ("libc/macros-cpp.inc", vertices[sorted[3]]);
  EXPECT_STREQ("libc/macros.h", vertices[sorted[4]]);
  EXPECT_STREQ("libc/bits/bits.h", vertices[sorted[5]]);
  EXPECT_STREQ("libc/str/str.h", vertices[sorted[6]]);
}
