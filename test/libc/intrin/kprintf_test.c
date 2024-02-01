/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/kprintf.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/log/libfatal.internal.h"
#include "libc/macros.internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/serialize.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

#define S(x) ((uintptr_t)(x))

/**
 * returns random bytes that don't have exclamation mark
 * since that would disable memory safety in the fuzzing
 */
static uint64_t Rando(void) {
  uint64_t x;
  do x = lemur64();
  while (((x ^ READ64LE("!!!!!!!!")) - 0x0101010101010101) &
         ~(x ^ READ64LE("!!!!!!!!")) & 0x8080808080808080);
  return x;
}

static const struct {
  const char *want;
  const char *fmt;
  uintptr_t arg1;
  uintptr_t arg2;
} V[] = {
    {"!!WONTFMT", (const char *)31337, 123},              //
    {"!!31337", "%s", 0x31337},                           //
    {"!!1", "%#s", 1},                                    //
    {"!!feff800000031337", "%s", 0xfeff800000031337ull},  //
    {"!!ffff800000031337", "%s", 0xffff800000031337ull},  //
    {"123", "%d", 123},                                   //
    {"2147483647", "%d", INT_MAX},                        //
    {"-2147483648", "%d", INT_MIN},                       //
    {"9223372036854775807", "%ld", LONG_MAX},             //
    {"-9223372036854775808", "%ld", LONG_MIN},            //
    {"9'223'372'036'854'775'807", "%'ld", LONG_MAX},      //
    {"-9'223'372'036'854'775'808", "%'ld", LONG_MIN},     //
    {"9,223,372,036,854,775,807", "%,ld", LONG_MAX},      //
    {"-9,223,372,036,854,775,808", "%,ld", LONG_MIN},     //
    {"9_223_372_036_854_775_807", "%_ld", LONG_MAX},      //
    {"-9_223_372_036_854_775_808", "%_ld", LONG_MIN},     //
    {"true", "%hhhd", 0xffff},                            //
    {"true", "%hhhd", 0xff00},                            //
    {"false", "%hhhd"},                                   //
    {"fa", "%hhh.2d"},                                    //
    {"  0x001337", "%#010.6x", 0x1337},                   //
    {"0x001337  ", "%#-010.6x", 0x1337},                  //
    {"0x1337    ", "%#-010.2x", 0x1337},                  //
    {"    0x1337", "%#010.2x", 0x1337},                   //
    {"0000001337", "%010d", 1337},                        //
    {"+000001337", "%+010d", 1337},                       //
    {"    001337", "%010.6d", 1337},                      //
    {"   +001337", "%+010.6d", 1337},                     //
    {"    001337", "%010.6x", 0x1337},                    //
    {"      1337", "%010.2x", 0x1337},                    //
    {"1337      ", "%-010d", 1337},                       //
    {"001337    ", "%-010.6d", 1337},                     //
    {"+1337     ", "%+-010d", 1337},                      //
    {"+001337   ", "%+-010.6d", 1337},                    //
    {"001337    ", "%-010.6x", 0x1337},                   //
    {"1337      ", "%-010.2x", 0x1337},                   //
    {"000001'337", "%'010d", 1337},                       //
    {"      1337", "%*d", 10, 1337},                      //
    {"1337      ", "%*d", -10, 1337},                     //
    {"0", "%#x"},                                         //
    {"0", "%#o"},                                         //
    {"0", "%#b"},                                         //
    {"0", "%#d"},                                         //
    {"0", "%p"},                                          //
    {"-1", "%p", S(MAP_FAILED)},                          //
    {"00000000", "%#.8x"},                                //
    {"00000000", "%#.8b"},                                //
    {"00000000", "%#.8o"},                                //
    {"  123", "%5d", 123},                                //
    {" -123", "%5d", -123},                               //
    {"  123", "%*d", 5, 123},                             //
    {" -123", "%*d", 5, -123},                            //
    {"123  ", "%-5d", 123},                               //
    {"-123 ", "%-5d", -123},                              //
    {" +123", "%+5d", 123},                               //
    {"00123", "%05d", 123},                               //
    {"-0123", "%05d", -123},                              //
    {"    0", "%5d"},                                     //
    {"   +0", "%+5d"},                                    //
    {"00000", "%05d"},                                    //
    {"            deadbeef", "%20x", 0xdeadbeef},         //
    {"          0xdeadbeef", "%20p", 0xdeadbeef},         //
    {"101", "%b", 0b101},                                 //
    {"123", "%x", 0x123},                                 //
    {"deadbeef", "%x", 0xdeadbeef},                       //
    {"DEADBEEF", "%X", 0xdeadbeef},                       //
    {"0", "%hd", INT_MIN},                                //
    {"123", "%o", 0123},                                  //
    {"+0", "%+d"},                                        //
    {"+123", "%+d", 123},                                 //
    {"-123", "%+d", -123},                                //
    {" 0", "% d"},                                        //
    {" 123", "% d", 123},                                 //
    {"-123", "% d", -123},                                //
    {"x", "%c", 'x'},                                     //
    {"☺", "%hc", u'☺'},                                   //
    {"☺", "%lc", L'☺'},                                   //
    {"☺", "%C", L'☺'},                                    //
    {"0x31337", "%p", 0x31337},                           //
    {"0xffff800000031337", "%p", 0xffff800000031337ull},  //
    {"0xfeff800000031337", "%p", 0xfeff800000031337ull},  //
    {"65535", "%hu", 0xffffffffu},                        //
    {"0", "%hu", 0x80000000u},                            //
    {"123", "%hd", 123},                                  //
    {"32767", "%hd", SHRT_MAX},                           //
    {"-32768", "%hd", SHRT_MIN},                          //
    {"-1", "%hhd", 0xffff},                               //
    {"-128", "%hhd", 0xff80},                             //
    {"255", "%hhu", 0xffffffffu},                         //
    {"'x'", "%#c", 'x'},                                  //
    {"u'☺'", "%#hc", u'☺'},                               //
    {"L'☺'", "%#lc", L'☺'},                               //
    {"L'☺'", "%#C", L'☺'},                                //
    {"L'\\''", "%#C", L'\''},                             //
    {"hello world\n", "%s", S("hello world\n")},          //
    {"☺☻♥♦♣♠!\n", "%s", S("☺☻♥♦♣♠!\n")},                  //
    {"␁", "%s", S("\1")},                                 //
    {"\1", "%.*s", 1, S("\1")},                           //
    {"\\001", "%'s", S("\1")},                            //
    {"\"\\001\"", "%#s", S("\1")},                        //
    {"", "%.*s", 0},                                      //
    {"☺☻♥♦♣♠!", "%hhs", S("\1\2\3\4\5\6!")},              //
    {"☺☻", "%.*hhs", 2, S("\1\2\3\4\5\6!")},              //
    {"u\"☺☻\"", "%#.*hhs", 2, S("\1\2\3\4\5\6!")},        //
    {"u\" ☻\"", "%#.*hhs", 2, S("\0\2\3\4\5\6!")},        //
    {"", "% s", S("")},                                   //
    {" a", "% s", S("a")},                                //
    {"", "% .*s", 0, S("a")},                             //
    {"", "% s"},                                          //
    {"𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷", "%hs", S(u"𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷")},                  //
    {"☺☻♥♦♣♠!", "%ls", S(L"☺☻♥♦♣♠!")},                    //
    {"HELLO", "%^s", S("hello")},                         //
    {"eeeeeee   ", "%10s", S("eeeeeee")},                 //
    {"hello", "%.*s", 5, S("hello world")},               //
    {"þell", "%.*s", 5, S("þello world")},                //
    {"þello", "%.*hs", 5, S(u"þello world")},             //
    {"þeeeeee   ", "%10s", S("þeeeeee")},                 //
    {"☺☻♥♦♣♠!   ", "%10s", S("☺☻♥♦♣♠!")},                 //
    {"☺☻♥♦♣♠    ", "%10hs", S(u"☺☻♥♦♣♠")},                //
    {"𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷  ", "%10hs", S(u"𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷")},              //
    {"☺☻♥♦♣♠!   ", "%10ls", S(L"☺☻♥♦♣♠!")},               //
    {"\"xx\"", "%#s", S("xx")},                           //
    {"u\"☺☺\"", "%#hs", S(u"☺☺")},                        //
    {"L\"☺☺\"", "%#ls", S(L"☺☺")},                        //
    {"\"\\\\\\\"\\177\"", "%#s", S("\\\"\177")},          //
    {"%%", "%%%%"},                                       //
    {"%", "%.%"},                                         //
    {"=", "%="},                                          //
};

TEST(ksnprintf, test) {
  char b[48], g[48];
  size_t i, j, n, rc;
  rngset(g, sizeof(g), 0, 0);
  for (i = 0; i < ARRAYLEN(V); ++i) {
    bzero(b, 48);
    n = strlen(V[i].want);
    rc = ksnprintf(b, 48, V[i].fmt, V[i].arg1, V[i].arg2);
    EXPECT_EQ(n, rc, "ksnprintf(\"%s\", %#lx, %#lx) → %zu ≠ %zu", V[i].fmt,
              V[i].arg1, V[i].arg2, rc, n);
    EXPECT_STREQ(V[i].want, b);
    memcpy(b, g, 48);
    for (j = 0; j < 40; ++j) {
      rc = ksnprintf(b, 0, V[i].fmt, V[i].arg1, V[i].arg2);
      ASSERT_EQ(n, rc, "ksnprintf(b, %zu, \"%s\", %#lx, %#lx) → %zu ≠ %zu", j,
                V[i].fmt, V[i].arg1, V[i].arg2, rc, n);
      ASSERT_EQ(READ64LE(g + j), READ64LE(b + j),
                "ksnprintf(b, %zu, \"%s\", %#lx, %#lx) → buffer overrun", j,
                V[i].fmt, V[i].arg1, V[i].arg2);
    }
  }
}

TEST(ksnprintf, testSymbols) {
  char b[2][32];
  bool hassymbols;
  hassymbols = GetSymbolTable();
  ksnprintf(b[0], 32, "%t", strlen);
  if (hassymbols) {
    ASSERT_STREQ("&strlen", b[0]);
  } else {
    ksnprintf(b[1], 32, "&%lx", strlen);
    ASSERT_STREQ(b[1], b[0]);
  }
}

TEST(ksnprintf, fuzzTheUnbreakable) {
  size_t i;
  uint64_t x;
  char *f, b[32];
  _Alignas(FRAMESIZE) static const char weasel[FRAMESIZE];
  f = (void *)__veil("r", weasel);
  EXPECT_SYS(0, 0, mprotect(f, FRAMESIZE, PROT_READ | PROT_WRITE));
  strcpy(f, "hello %s\n");
  EXPECT_EQ(12, ksnprintf(b, sizeof(b), f, "world"));
  EXPECT_STREQ("hello world\n", b);
  for (i = 0; i < 30000; ++i) {
    x = Rando();
    memcpy(f, &x, sizeof(x));
    x = Rando();
    memcpy(f + 8, &x, sizeof(x));
    f[Rando() & 15] = '%';
    ksnprintf(b, sizeof(b), f, lemur64(), lemur64(), lemur64());
  }
  EXPECT_SYS(0, 0, mprotect(f, FRAMESIZE, PROT_READ));
}

TEST(kprintf, testFailure_wontClobberErrnoAndBypassesSystemCallSupport) {
  int n;
  ASSERT_EQ(0, errno);
  EXPECT_SYS(0, 3, dup(2));
  // <LIMBO>
  if (close(2)) _Exit(200);
  n = __syscount;
  if (__syscount != n) _Exit(201);
  if (errno != 0) _Exit(202);
  if (dup2(3, 2) != 2) _Exit(203);
  // </LIMBO>
  EXPECT_SYS(0, 0, close(3));
}

TEST(ksnprintf, testy) {
  char b[32];
  EXPECT_EQ(3, ksnprintf(b, 32, "%#s", 1));
  EXPECT_STREQ("!!1", b);
}

TEST(ksnprintf, testNonTextFmt_wontFormat) {
  char b[32];
  char variable_format_string[16] = "%s";
  EXPECT_EQ(9, ksnprintf(b, 32, variable_format_string, NULL));
  EXPECT_STREQ("!!WONTFMT", b);
}

TEST(ksnprintf, testMisalignedPointer_wontFormat) {
  char b[32];
  const char16_t *s = u"hello";
  ksnprintf(b, 32, "%hs", (char *)s + 1);
  EXPECT_STARTSWITH("!!", b);
}

TEST(ksnprintf, testUnterminatedOverrun_truncatesAtPageBoundary) {
  char *m;
  char b[32];
  m = memset(_mapanon(FRAMESIZE * 2), 1, FRAMESIZE);
  EXPECT_SYS(0, 0, munmap(m + FRAMESIZE, FRAMESIZE));
  EXPECT_EQ(12, ksnprintf(b, 32, "%'s", m + FRAMESIZE - 3));
  EXPECT_STREQ("\\001\\001\\001", b);
  EXPECT_SYS(0, 0, munmap(m, FRAMESIZE));
}

TEST(ksnprintf, testEmptyBuffer_determinesTrueLength) {
  EXPECT_EQ(5, ksnprintf(0, 0, "hello"));
}

TEST(ksnprintf, testFormatOnly_copiesString) {
  char b[6];
  EXPECT_EQ(5, ksnprintf(b, 6, "hello"));
  EXPECT_STREQ("hello", b);
}

TEST(ksnprintf, testOneChar_justNulTerminates) {
  char b[2] = {1, 2};
  EXPECT_EQ(3, ksnprintf(b, 1, "%d", 123));
  EXPECT_EQ(0, b[0]);
  EXPECT_EQ(2, b[1]);
}

TEST(kprintf, testStringUcs2) {
  char b[32];
  EXPECT_EQ(21, ksnprintf(b, 32, "%hs", u"þ☺☻♥♦♣♠!"));
  EXPECT_EQ(0xc3, b[0] & 255);
  EXPECT_EQ(0xbe, b[1] & 255);
  EXPECT_EQ(0xe2, b[2] & 255);
  EXPECT_EQ(0x98, b[3] & 255);
  EXPECT_EQ(0xba, b[4] & 255);
  EXPECT_STREQ("þ☺☻♥♦♣♠!", b);
}

TEST(kprintf, testTruncate_addsDotsAndReturnsTrueLength) {
  char b[15];
  EXPECT_EQ(10, ksnprintf(b, 15, "%p", 0xdeadbeef));
  EXPECT_STREQ("0xdeadbeef", b);
  EXPECT_EQ(10, ksnprintf(b, 10, "%p", 0xdeadbeef));
  EXPECT_STREQ("0xdead...", b);
}

TEST(kprintf, testTruncate_preservesNewlineFromEndOfFormatString) {
  char b[14];
  EXPECT_EQ(11, ksnprintf(b, 10, "%p\n", 0xdeadbeef));
  EXPECT_STREQ("0xdea...\n", b);
}

TEST(ksnprintf, testTruncate_doesntBreakApartCharacters) {
  char b[5];
  ASSERT_EQ(6, ksnprintf(b, 5, "☻☻"));
  ASSERT_STREQ("....", b);
}

TEST(ksnprintf, badUtf16) {
  size_t i;
  char b[16];
  static const struct {
    const char *want;
    const char *fmt;
    char16_t arg[16];
  } V[] = {
      {"�         ", "%10hs", {0xd800}},
      {"�         ", "%10hs", {0xdc00}},
      {"��        ", "%10hs", {0xd800, 0xd800}},
      {"��        ", "%10hs", {0xdc00, 0xdc00}},
  };
  for (i = 0; i < ARRAYLEN(V); ++i) {
    EXPECT_EQ(strlen(V[i].want), ksnprintf(b, 16, V[i].fmt, V[i].arg));
    EXPECT_STREQ(V[i].want, b);
  }
}

TEST(ksnprintf, negativeOverflowIdiom_isSafe) {
  int i, n;
  char golden[11];
  struct {
    char underflow[11];
    char buf[11];
    char overflow[11];
  } u;
  memset(golden, -1, 11);
  memset(u.underflow, -1, 11);
  memset(u.overflow, -1, 11);
  i = 0;
  n = 11;
  i += ksnprintf(u.buf + i, n - i, "hello");
  ASSERT_STREQ("hello", u.buf);
  i += ksnprintf(u.buf + i, n - i, " world");
  ASSERT_STREQ("hello w...", u.buf);
  i += ksnprintf(u.buf + i, n - i, " i love you");
  ASSERT_STREQ("hello w...", u.buf);
  ASSERT_EQ(i, 5 + 6 + 11);
  ASSERT_EQ(0, memcmp(golden, u.underflow, 11));
  ASSERT_EQ(0, memcmp(golden, u.overflow, 11));
}

TEST(ksnprintf, truncation) {
  char buf[16] = {0};
  rngset(buf, sizeof(buf) - 1, lemur64, -1);
  ksnprintf(0, 0, "%s", "xxxxx");
  rngset(buf, sizeof(buf) - 1, lemur64, -1);
  ksnprintf(buf, 1, "%s", "xxxxx");
  EXPECT_STREQ("", buf);
  rngset(buf, sizeof(buf) - 1, lemur64, -1);
  ksnprintf(buf, 2, "%s", "xxxxx");
  EXPECT_STREQ(".", buf);
  rngset(buf, sizeof(buf) - 1, lemur64, -1);
  ksnprintf(buf, 3, "%s", "xxxxx");
  EXPECT_STREQ("..", buf);
  rngset(buf, sizeof(buf) - 1, lemur64, -1);
  ksnprintf(buf, 4, "%s", "xxxxx");
  EXPECT_STREQ("...", buf);
  rngset(buf, sizeof(buf) - 1, lemur64, -1);
  ksnprintf(buf, 5, "%s", "xxxxx");
  EXPECT_STREQ("x...", buf);
  rngset(buf, sizeof(buf) - 1, lemur64, -1);
  ksnprintf(buf, 6, "%s", "xxxxxxxxxxx");
  EXPECT_STREQ("xx...", buf);
  rngset(buf, sizeof(buf) - 1, lemur64, -1);
  ksnprintf(buf, 7, "%s", "xxxxxxxxx");
  EXPECT_STREQ("xxx...", buf);
}

BENCH(printf, bench) {
  char b[128];
  int snprintf_(char *, size_t, const char *, ...) asm("snprintf");
  EZBENCH2("ksnprintf fmt", donothing, ksnprintf(b, 128, "."));
  EZBENCH2("snprintf  fmt", donothing, snprintf_(b, 128, "."));
  EZBENCH2("ksnprintf str", donothing,
           ksnprintf(b, 128, "%s\n", "hello world"));
  EZBENCH2("snprintf  str", donothing,
           snprintf_(b, 128, "%s\n", "hello world"));
  EZBENCH2("ksnprintf utf8", donothing,
           ksnprintf(b, 128, "%s\n", "天地玄黄宇宙洪荒天地玄黄宇宙洪荒"));
  EZBENCH2("snprintf  utf8", donothing,
           snprintf_(b, 128, "%s\n", "天地玄黄宇宙洪荒天地玄黄宇宙洪荒"));
  EZBENCH2("ksnprintf chinese", donothing,
           ksnprintf(b, 128, "%hs\n", u"天地玄黄宇宙洪荒"));
  EZBENCH2("snprintf  chinese", donothing,
           snprintf_(b, 128, "%hs\n", u"天地玄黄宇宙洪荒"));
  EZBENCH2("ksnprintf astral", donothing,
           ksnprintf(b, 128, "%hs\n", u"𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷"));
  EZBENCH2("snprintf  astral", donothing,
           snprintf_(b, 128, "%hs\n", u"𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷"));
  EZBENCH2("ksnprintf octal", donothing, ksnprintf(b, 128, "%#lo", ULONG_MAX));
  EZBENCH2("snprintf  octal", donothing, snprintf(b, 128, "%#lo", ULONG_MAX));
  EZBENCH2("ksnprintf long", donothing, ksnprintf(b, 128, "%ld", LONG_MAX));
  EZBENCH2("snprintf  long", donothing, snprintf_(b, 128, "%ld", LONG_MAX));
  EZBENCH2("ksnprintf thou", donothing, ksnprintf(b, 128, "%'ld", LONG_MAX));
  EZBENCH2("snprintf  thou", donothing, snprintf_(b, 128, "%'ld", LONG_MAX));
  EZBENCH2(
      "ksnprintf bin", donothing,
      ksnprintf(b, 128, "%#.*hhs\n", 8, "\001\002\003\004\005\006\007\008"));
  EZBENCH2(
      "snprintf bin", donothing,
      snprintf(b, 128, "%`'.*hhs\n", 8, "\001\002\003\004\005\006\007\008"));
}
