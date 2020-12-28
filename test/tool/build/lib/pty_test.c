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
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/unicode/unicode.h"
#include "tool/build/lib/pty.h"

char *render(struct Pty *pty) {
  static struct Buffer b;
  int y;
  b.i = 0;
  for (y = 0; y < pty->yn; ++y) {
    PtyAppendLine(pty, &b, y);
  }
  b.p[b.i] = 0;
  return b.p;
}

const char widelatin[] forcealign(16) = "\
Ａ-ＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ\r\n\
ａｂ-ｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ\r\n\
０１２-３４５６７８９\r\n\
ＡＢＣＤ-ＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ\r\n\
ａｂｃｄｅ-ｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ\r\n\
０１２３４５-６７８９\r\n\
ＡＢＣＤＥＦＧ-ＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ\r\n\
ａｂｃｄｅｆｇｈ-ｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ\r\n\
０１２３４５６７８-９";

static const char widelatin_golden[] = "\
Ａ-ＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ                           \
ａｂ-ｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ                           \
０１２-３４５６７８９                                                           \
ＡＢＣＤ-ＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ                           \
ａｂｃｄｅ-ｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ                           \
０１２３４５-６７８９                                                           \
ＡＢＣＤＥＦＧ-ＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ                           \
ａｂｃｄｅｆｇｈ-ｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ                           \
０１２３４５６７８-９▂                                                          \
                                                                                \
                                                                                \
                                                                                \
                                                                                \
                                                                                \
                                                                                \
                                                                                \
                                                                                \
                                                                                \
                                                                                \
                                                                                \
                                                                                \
                                                                                \
                                                                                \
                                                                                \
                                                                                ";

TEST(pty, testFunWidth) {
  struct Pty *pty = NewPty();
  PtyWrite(pty, widelatin, ARRAYLEN(widelatin) - 1);
  EXPECT_STREQ(widelatin_golden, render(pty));
  FreePty(pty);
}

const char hyperion[] forcealign(16) = "\
Fanatics have their dreams, wherewith they weave \
A paradise for a sect; the savage too \
From forth the loftiest fashion of his sleep \
Guesses at Heaven; pity these have not \
Trac'd upon vellum or wild Indian leaf \
The shadows of melodious utterance. \
But bare of laurel they live, dream, and die; \
For Poesy alone can tell her dreams, \
With the fine spell of words alone can save \
Imagination from the sable charm \
And dumb enchantment. Who alive can say, \
'Thou art no Poet may'st not tell thy dreams?' \
Since every man whose soul is not a clod \
Hath visions, and would speak, if he had loved \
And been well nurtured in his mother tongue. \
Whether the dream now purpos'd to rehearse \
Be poet's or fanatic's will be known \
When this warm scribe my hand is in the grave.";

static const wchar_t hyperion_golden[] = L"\
Fanatics have their dreams, wherewith they weave A paradise for a sect; the sava\
ge too From forth the loftiest fashion of his sleep Guesses at Heaven; pity thes\
e have not Trac'd upon vellum or wild Indian leaf The shadows of melodious utter\
ance. But bare of laurel they live, dream, and die; For Poesy alone can tell her\
 dreams, With the fine spell of words alone can save Imagination from the sable \
charm And dumb enchantment. Who alive can say, 'Thou art no Poet may'st not tell\
 thy dreams?' Since every man whose soul is not a clod Hath visions, and would s\
peak, if he had loved And been well nurtured in his mother tongue. Whether the d\
ream now purpos'd to rehearse Be poet's or fanatic's will be known When this war\
m scribe my hand is in the grave.                                               \
                                                                                \
                                                                                \
                                                                                \
                                                                                \
                                                                                \
                                                                                \
                                                                                \
                                                                                \
                                                                                \
                                                                                \
                                                                                \
                                                                                \
                                                                                \
                                                                                \
                                                                                ";

TEST(pty, testPureAscii_benefitsFromVectorization) {
  struct Pty *pty = NewPty();
  PtyWrite(pty, hyperion, ARRAYLEN(hyperion) - 1);
  EXPECT_STREQN(pty->wcs, hyperion_golden, ARRAYLEN(hyperion_golden) - 1);
  FreePty(pty);
}

static const char kKiloAnsi[] = "\
\e[?25l\e[H\
#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐\e[39m\e[0K\r\n\
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi─────────\e[39m\e[0K\r\n\
#\e[39m\e[0K\r\n\
# SYNOPSIS\e[39m\e[0K\r\n\
#\e[39m\e[0K\r\n\
#   Freestanding Hermetically-Sealed Monolithic Repository\e[39m\e[0K\r\n\
#\e[39m\e[0K\r\n\
# REQUIREMENTS\e[39m\e[0K\r\n\
#\e[39m\e[0K\r\n\
#   You can run your programs on any operating system, but you have\e[39m\e[0K\r\n\
#   to build them on Linux 2.6+ (or WSL) using GNU Make. A modern C\e[39m\e[0K\r\n\
#   compiler that\'s statically-linked comes included as a courtesy.\e[39m\e[0K\r\n\
#\e[39m\e[0K\r\n\
# EXAMPLES\e[39m\e[0K\r\n\
#\e[39m\e[0K\r\n\
#   # build and run everything\e[39m\e[0K\r\n\
#   make -j8 -O\e[39m\e[0K\r\n\
#   make -j8 -O MODE=dbg\e[39m\e[0K\r\n\
#   make -j8 -O MODE=opt\e[39m\e[0K\r\n\
#   make -j8 -O MODE=rel\e[39m\e[0K\r\n\
#   make -j8 -O MODE=tiny\e[39m\e[0K\r\n\
#\e[39m\e[0K\r\n\
#   # build individual target\e[39m\e[0K\r\n\
\e[0K\e[7mMakefile - 340 lines                                                       1/340\e[0m\r\n\
\e[0KHELP: Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find\e[1;1H\e[?25h";

static const wchar_t kKiloGolden[] = L"\
#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐         \
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi─────────                        \
#                                                                               \
# SYNOPSIS                                                                      \
#                                                                               \
#   Freestanding Hermetically-Sealed Monolithic Repository                      \
#                                                                               \
# REQUIREMENTS                                                                  \
#                                                                               \
#   You can run your programs on any operating system, but you have             \
#   to build them on Linux 2.6+ (or WSL) using GNU Make. A modern C             \
#   compiler that's statically-linked comes included as a courtesy.             \
#                                                                               \
# EXAMPLES                                                                      \
#                                                                               \
#   # build and run everything                                                  \
#   make -j8 -O                                                                 \
#   make -j8 -O MODE=dbg                                                        \
#   make -j8 -O MODE=opt                                                        \
#   make -j8 -O MODE=rel                                                        \
#   make -j8 -O MODE=tiny                                                       \
#                                                                               \
#   # build individual target                                                   \
Makefile - 340 lines                                                       1/340\
HELP: Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find                             \
";

TEST(pty, testLongestPossibleCharacter) {
  EXPECT_EQ(60, strlen("\e[21;22;27;24;25;29;38;2;255;255;255;48;2;255;255;"
                       "255m\377\277\277\277\277\277"));
  struct Buffer b = {0};
  struct Pty *pty = NewPty();
  const char *s = "\e[1;2;3;4;5;6;7;9m"
                  "h"
                  "\e[0;"
                  "38;2;255;255;255;"
                  "48;2;255;255;255m"
                  "\377\277\277\277\277\277"
                  "\e[0m";
  PtyWrite(pty, s, strlen(s));
  PtyAppendLine(pty, &b, 0);
  AppendChar(&b, '\0');
  EXPECT_STREQ("\e[1;2;4;7;5;9m"
               "𝒉"
               "\e[22;24;27;25;29;38;2;255;255;255;48;2;255;255;255m"
               "\377\277\277\277\277\277"
               "\e[0m▂                                                       "
               "                      ",
               b.p);
  FreePty(pty);
  free(b.p);
}

TEST(pty, test) {
  struct Pty *pty = NewPty();
  PtyWrite(pty, kKiloAnsi, strlen(kKiloAnsi));
  EXPECT_STREQN(kKiloGolden, pty->wcs, wcslen(kKiloGolden));
  FreePty(pty);
}

BENCH(pty, bench) {
  struct Pty *pty = NewPty();
  EZBENCH2("pty write ascii", donothing,
           PtyWrite(pty, hyperion, sizeof(hyperion) - 1));
  EZBENCH2("pty write kilo", donothing,
           PtyWrite(pty, kKiloAnsi, sizeof(kKiloAnsi) - 1));
  EZBENCH2("pty render", donothing, render(pty));
  FreePty(pty);
}
