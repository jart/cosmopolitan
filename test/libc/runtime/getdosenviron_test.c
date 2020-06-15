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
#include "libc/runtime/getdosenviron.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(getdosenviron, testOneVariable) {
#define kEnv u"A=Und wird die Welt auch in Flammen stehen\0"
  size_t max = 2;
  size_t size = sizeof(kEnv) >> 1;
  char *block = tmalloc(size);
  char16_t *env = memcpy(tmalloc(sizeof(kEnv)), kEnv, sizeof(kEnv));
  char **envp = tmalloc(max * sizeof(char *));
  EXPECT_EQ(1, getdosenviron(env, block, size, envp, max));
  EXPECT_STREQ("A=Und wird die Welt auch in Flammen stehen", envp[0]);
  EXPECT_EQ(NULL, envp[1]);
  ASSERT_BINEQ(u"A=Und wird die Welt auch in Flammen stehen  ", block);
  tfree(envp);
  tfree(env);
  tfree(block);
#undef kEnv
}

TEST(getdosenviron, testTwoVariables) {
#define kEnv                                                      \
  (u"𐌰𐌱𐌲𐌳=Und wird die Welt auch in Flammen stehen\0" \
   u"𐌴𐌵𐌶𐌷=Wir werden wieder auferstehen\0")
  size_t max = 3;
  size_t size = 1024;
  char *block = tmalloc(size);
  char16_t *env = memcpy(tmalloc(sizeof(kEnv)), kEnv, sizeof(kEnv));
  char **envp = tmalloc(max * sizeof(char *));
  EXPECT_EQ(2, getdosenviron(env, block, size, envp, max));
  EXPECT_STREQ("𐌰𐌱𐌲𐌳=Und wird die Welt auch in Flammen stehen", envp[0]);
  EXPECT_STREQ("𐌴𐌵𐌶𐌷=Wir werden wieder auferstehen", envp[1]);
  EXPECT_EQ(NULL, envp[2]);
  tfree(envp);
  tfree(env);
  tfree(block);
#undef kEnv
}

TEST(getdosenviron, testOverrun_truncatesWithGrace) {
#define kEnv u"A=Und wird die Welt auch in Flammen stehen\0"
  size_t max = 2;
  size_t size = sizeof(kEnv) >> 2;
  char *block = tmalloc(size);
  char16_t *env = memcpy(tmalloc(sizeof(kEnv)), kEnv, sizeof(kEnv));
  char **envp = tmalloc(max * sizeof(char *));
  EXPECT_EQ(1, getdosenviron(env, block, size, envp, max));
  EXPECT_STREQ("A=Und wird die Welt ", envp[0]);
  EXPECT_EQ(NULL, envp[1]);
  ASSERT_BINEQ(u"A=Und wird die Welt   ", block);
  tfree(envp);
  tfree(env);
  tfree(block);
#undef kEnv
}

TEST(getdosenviron, testEmpty_doesntTouchMemory) {
  EXPECT_EQ(0, getdosenviron(u"", NULL, 0, NULL, 0));
}

TEST(getdosenviron, testEmpty_zeroTerminatesWheneverPossible_1) {
  size_t max = 1;
  char **envp = tmalloc(max * sizeof(char *));
  EXPECT_EQ(0, getdosenviron(u"", NULL, 0, envp, max));
  EXPECT_EQ(NULL, envp[0]);
  tfree(envp);
}

TEST(getdosenviron, testEmpty_zeroTerminatesWheneverPossible_2) {
  size_t size = 1;
  char *block = tmalloc(size);
  EXPECT_EQ(0, getdosenviron(u"", block, size, NULL, 0));
  EXPECT_BINEQ(u" ", block);
  tfree(block);
}

TEST(getdosenviron, testEmpty_zeroTerminatesWheneverPossible_3) {
  size_t size = 2;
  char *block = tmalloc(size);
  EXPECT_EQ(0, getdosenviron(u"", block, size, NULL, 0));
  EXPECT_BINEQ(u"  ", block);
  tfree(block);
}
