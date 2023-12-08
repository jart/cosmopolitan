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
#include "libc/mem/mem.h"
#include "libc/runtime/internal.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(GetDosEnviron, testOneVariable) {
#define kEnv u"a=Und wird die Welt auch in Flammen stehen\0"
  size_t max = 2;
  size_t size = sizeof(kEnv) >> 1;
  char *block = calloc(1, size);
  char16_t *env = memcpy(calloc(1, sizeof(kEnv)), kEnv, sizeof(kEnv));
  char **envp = calloc(1, max * sizeof(char *));
  EXPECT_EQ(1, GetDosEnviron(env, block, size, envp, max));
  EXPECT_STREQ("A=Und wird die Welt auch in Flammen stehen", envp[0]);
  EXPECT_EQ(NULL, envp[1]);
  ASSERT_BINEQ(u"A=Und wird die Welt auch in Flammen stehen  ", block);
  free(envp);
  free(env);
  free(block);
#undef kEnv
}

TEST(GetDosEnviron, testTwoVariables) {
#define kEnv                                                      \
  (u"𐌰𐌱𐌲𐌳=Und wird die Welt auch in Flammen stehen\0" \
   u"𐌴𐌵𐌶𐌷=Wir werden wieder auferstehen\0")
  size_t max = 3;
  size_t size = 1024;
  char *block = calloc(1, size);
  char16_t *env = memcpy(calloc(1, sizeof(kEnv)), kEnv, sizeof(kEnv));
  char **envp = calloc(1, max * sizeof(char *));
  EXPECT_EQ(2, GetDosEnviron(env, block, size, envp, max));
  EXPECT_STREQ("𐌰𐌱𐌲𐌳=Und wird die Welt auch in Flammen stehen", envp[0]);
  EXPECT_STREQ("𐌴𐌵𐌶𐌷=Wir werden wieder auferstehen", envp[1]);
  EXPECT_EQ(NULL, envp[2]);
  free(envp);
  free(env);
  free(block);
#undef kEnv
}

TEST(GetDosEnviron, testOverrun_truncatesWithGrace) {
#define kEnv u"A=Und wird die Welt auch in Flammen stehen\0"
  size_t max = 2;
  size_t size = sizeof(kEnv) >> 2;
  char *block = calloc(1, size);
  char16_t *env = memcpy(calloc(1, sizeof(kEnv)), kEnv, sizeof(kEnv));
  char **envp = calloc(1, max * sizeof(char *));
  EXPECT_EQ(1, GetDosEnviron(env, block, size, envp, max));
  EXPECT_STREQ("A=Und wird die Welt ", envp[0]);
  EXPECT_EQ(NULL, envp[1]);
  ASSERT_BINEQ(u"A=Und wird die Welt   ", block);
  free(envp);
  free(env);
  free(block);
#undef kEnv
}

TEST(GetDosEnviron, testEmpty_doesntTouchMemory) {
  EXPECT_EQ(0, GetDosEnviron(u"", NULL, 0, NULL, 0));
}

TEST(GetDosEnviron, testEmpty_zeroTerminatesWheneverPossible_1) {
  size_t max = 1;
  char **envp = calloc(1, max * sizeof(char *));
  EXPECT_EQ(0, GetDosEnviron(u"", NULL, 0, envp, max));
  EXPECT_EQ(NULL, envp[0]);
  free(envp);
}

TEST(GetDosEnviron, testEmpty_zeroTerminatesWheneverPossible_2) {
  size_t size = 1;
  char *block = calloc(1, size);
  EXPECT_EQ(0, GetDosEnviron(u"", block, size, NULL, 0));
  EXPECT_BINEQ(u" ", block);
  free(block);
}

TEST(GetDosEnviron, testEmpty_zeroTerminatesWheneverPossible_3) {
  size_t size = 2;
  char *block = calloc(1, size);
  EXPECT_EQ(0, GetDosEnviron(u"", block, size, NULL, 0));
  EXPECT_BINEQ(u" ", block);
  free(block);
}
