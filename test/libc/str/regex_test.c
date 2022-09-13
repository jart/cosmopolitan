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
#include "libc/mem/mem.h"
#include "libc/mem/gc.internal.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "third_party/regex/regex.h"

TEST(regex, test) {
  regex_t rx;
  EXPECT_EQ(REG_OK, regcomp(&rx, "^[A-Za-z\x7f-\uffff]{2}$", REG_EXTENDED));
  EXPECT_EQ(REG_OK, regexec(&rx, "AZ", 0, NULL, 0));
  EXPECT_EQ(REG_OK, regexec(&rx, "→→", 0, NULL, 0));
  EXPECT_EQ(REG_NOMATCH, regexec(&rx, "A", 0, NULL, 0));
  EXPECT_EQ(REG_NOMATCH, regexec(&rx, "→", 0, NULL, 0));
  EXPECT_EQ(REG_NOMATCH, regexec(&rx, "0", 0, NULL, 0));
  regfree(&rx);
}

TEST(regex, testDns) {
  regex_t rx;
  EXPECT_EQ(REG_OK, regcomp(&rx, "^[-._0-9A-Za-z]*$", REG_EXTENDED));
  EXPECT_EQ(REG_OK, regexec(&rx, "", 0, NULL, 0));
  EXPECT_EQ(REG_OK, regexec(&rx, "foo.com", 0, NULL, 0));
  EXPECT_EQ(REG_NOMATCH, regexec(&rx, "bar@example", 0, NULL, 0));
  regfree(&rx);
}

TEST(regex, testIpBasic) {
  regex_t rx;
  EXPECT_EQ(REG_OK, regcomp(&rx,
                            "^"
                            "\\([0-9][0-9]*\\)\\."
                            "\\([0-9][0-9]*\\)\\."
                            "\\([0-9][0-9]*\\)\\."
                            "\\([0-9][0-9]*\\)"
                            "$",
                            0));
  const char *s = "127.0.0.1";
  regmatch_t *m = gc(calloc(rx.re_nsub + 1, sizeof(regmatch_t)));
  ASSERT_EQ(4, rx.re_nsub);
  EXPECT_EQ(REG_OK, regexec(&rx, s, rx.re_nsub + 1, m, 0));
  EXPECT_STREQ("127", gc(strndup(s + m[1].rm_so, m[1].rm_eo - m[1].rm_so)));
  EXPECT_STREQ("0", gc(strndup(s + m[2].rm_so, m[2].rm_eo - m[2].rm_so)));
  EXPECT_STREQ("0", gc(strndup(s + m[3].rm_so, m[3].rm_eo - m[3].rm_so)));
  EXPECT_STREQ("1", gc(strndup(s + m[4].rm_so, m[4].rm_eo - m[4].rm_so)));
  regfree(&rx);
}

TEST(regex, testIpExtended) {
  regex_t rx;
  EXPECT_EQ(REG_OK, regcomp(&rx,
                            "^"
                            "([0-9]{1,3})\\."
                            "([0-9]{1,3})\\."
                            "([0-9]{1,3})\\."
                            "([0-9]{1,3})"
                            "$",
                            REG_EXTENDED));
  const char *s = "127.0.0.1";
  regmatch_t *m = gc(calloc(rx.re_nsub + 1, sizeof(regmatch_t)));
  ASSERT_EQ(4, rx.re_nsub);
  EXPECT_EQ(REG_OK, regexec(&rx, s, rx.re_nsub + 1, m, 0));
  EXPECT_STREQ("127", gc(strndup(s + m[1].rm_so, m[1].rm_eo - m[1].rm_so)));
  EXPECT_STREQ("0", gc(strndup(s + m[2].rm_so, m[2].rm_eo - m[2].rm_so)));
  EXPECT_STREQ("0", gc(strndup(s + m[3].rm_so, m[3].rm_eo - m[3].rm_so)));
  EXPECT_STREQ("1", gc(strndup(s + m[4].rm_so, m[4].rm_eo - m[4].rm_so)));
  regfree(&rx);
}

TEST(regex, testUnicodeCharacterClass) {
  regex_t rx;
  EXPECT_EQ(REG_OK, regcomp(&rx, "^[[:alpha:]][[:alpha:]]$", 0));
  EXPECT_EQ(REG_OK, regexec(&rx, "𝐵𝑏", 0, 0, 0));
  EXPECT_NE(REG_OK, regexec(&rx, "₀₁", 0, 0, 0));
  regfree(&rx);
}

void A(void) {
  regex_t rx;
  regcomp(&rx, "^[-._0-9A-Za-z]*$", REG_EXTENDED);
  regexec(&rx, "foo.com", 0, NULL, 0);
  regfree(&rx);
}
void B(regex_t *rx) {
  regexec(rx, "foo.com", 0, NULL, 0);
}
void C(void) {
  regex_t rx;
  regcomp(&rx, "^[-._0-9A-Za-z]*$", 0);
  regexec(&rx, "foo.com", 0, NULL, 0);
  regfree(&rx);
}
void D(regex_t *rx, regmatch_t *m) {
  regexec(rx, "127.0.0.1", rx->re_nsub + 1, m, 0);
}

BENCH(regex, bench) {
  regex_t rx;
  regmatch_t *m;
  regcomp(&rx, "^[-._0-9A-Za-z]*$", REG_EXTENDED);
  EZBENCH2("precompiled extended", donothing, B(&rx));
  regfree(&rx);
  EZBENCH2("easy api extended", donothing, A());
  EZBENCH2("easy api basic", donothing, C());
  EXPECT_EQ(REG_OK, regcomp(&rx,
                            "^"
                            "\\([0-9][0-9]*\\)\\."
                            "\\([0-9][0-9]*\\)\\."
                            "\\([0-9][0-9]*\\)\\."
                            "\\([0-9][0-9]*\\)"
                            "$",
                            0));
  m = calloc(rx.re_nsub + 1, sizeof(regmatch_t));
  EZBENCH2("precompiled basic match", donothing, D(&rx, m));
  free(m);
  regfree(&rx);
  EXPECT_EQ(REG_OK, regcomp(&rx,
                            "^"
                            "([0-9]{1,3})\\."
                            "([0-9]{1,3})\\."
                            "([0-9]{1,3})\\."
                            "([0-9]{1,3})"
                            "$",
                            REG_EXTENDED));
  m = calloc(rx.re_nsub + 1, sizeof(regmatch_t));
  EZBENCH2("precompiled extended match", donothing, D(&rx, m));
  free(m);
  regfree(&rx);
  EXPECT_EQ(REG_OK, regcomp(&rx,
                            "^"
                            "([0-9]{1,3})\\."
                            "([0-9]{1,3})\\."
                            "([0-9]{1,3})\\."
                            "([0-9]{1,3})"
                            "$",
                            REG_EXTENDED | REG_NOSUB));
  m = calloc(rx.re_nsub + 1, sizeof(regmatch_t));
  EZBENCH2("precompiled nosub match", donothing, D(&rx, m));
  free(m);
  regfree(&rx);
  EXPECT_EQ(REG_OK, regcomp(&rx, "^[a-z]*$", REG_EXTENDED | REG_NOSUB));
  m = calloc(rx.re_nsub + 1, sizeof(regmatch_t));
  EZBENCH2("precompiled alpha", donothing,
           regexec(&rx, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 0, 0, 0));
  free(m);
  regfree(&rx);
  EXPECT_EQ(REG_OK,
            regcomp(&rx, "^[a-z]*$", REG_EXTENDED | REG_NOSUB | REG_ICASE));
  m = calloc(rx.re_nsub + 1, sizeof(regmatch_t));
  EZBENCH2("precompiled alpha icase", donothing,
           regexec(&rx, "aaaaaaaaaaaaaaaAAAAAAAAAAAAAA", 0, 0, 0));
  free(m);
  regfree(&rx);
}
