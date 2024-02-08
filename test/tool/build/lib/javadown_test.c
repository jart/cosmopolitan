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
#include "tool/build/lib/javadown.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(ParseJavadown, testOneLiner) {
  const char *comment = "/** Parses javadown. */";
  struct Javadown *jd;
  jd = ParseJavadown(comment + 3, strlen(comment) - 3 - 2);
  EXPECT_FALSE(jd->isfileoverview);
  EXPECT_STREQ("Parses javadown.", jd->title);
  EXPECT_STREQ("", jd->text);
  EXPECT_EQ(0, jd->tags.n);
  FreeJavadown(jd);
}

TEST(ParseJavadown, testShortAndTerse) {
  const char *comment = "\
/**\n\
 * Parses javadown.\n\
 * @see love\n\
 */";
  struct Javadown *jd;
  jd = ParseJavadown(comment + 3, strlen(comment) - 3 - 2);
  EXPECT_FALSE(jd->isfileoverview);
  EXPECT_STREQ("Parses javadown.", jd->title);
  EXPECT_STREQ("", jd->text);
  EXPECT_EQ(1, jd->tags.n);
  EXPECT_STREQ("see", jd->tags.p[0].tag);
  EXPECT_STREQ("love", jd->tags.p[0].text);
  FreeJavadown(jd);
}

TEST(ParseJavadown, testBlankLineOmitted) {
  const char *comment = "\
/**\n\
 * Parses javadown.\n\
 *\n\
 * Description.\n\
 * @see love\n\
 */";
  struct Javadown *jd;
  jd = ParseJavadown(comment + 3, strlen(comment) - 3 - 2);
  EXPECT_FALSE(jd->isfileoverview);
  EXPECT_STREQ("Parses javadown.", jd->title);
  EXPECT_STREQ("Description.", jd->text);
  EXPECT_EQ(1, jd->tags.n);
  EXPECT_STREQ("see", jd->tags.p[0].tag);
  EXPECT_STREQ("love", jd->tags.p[0].text);
  FreeJavadown(jd);
}

TEST(ParseJavadown, testContentInterpretation) {
  const char *comment = "\
/**\n\
 * Parses javadown    \n\
 * and that is the bottom line.\n\
 *\n\
 * Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do\n\
 * eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim \n\
 * ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut\n\
 * aliquip ex ea commodo consequat.\n\
 *\n\
 * Duis aute irure dolor in reprehenderit in voluptate velit esse cillum\n\
 * dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat\n\
 * non proident, sunt in culpa qui officia deserunt mollit anim id est\n\
 * laborum\n\
 *\n\
 * @param data should point to text inside the slash star markers\n\
 *     lorem ipsum dolla dollaz yo\n\
 * @param size is length of data in bytes\n\
 * @return object that should be passed to FreeJavadown()\n\
 * @asyncsignalsafe\n\
 */";
  const char *title = "Parses javadown and that is the bottom line.";
  const char *description = "\
Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do\n\
eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim\n\
ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut\n\
aliquip ex ea commodo consequat.\n\
\n\
Duis aute irure dolor in reprehenderit in voluptate velit esse cillum\n\
dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat\n\
non proident, sunt in culpa qui officia deserunt mollit anim id est\n\
laborum\n";
  struct Javadown *jd;
  jd = ParseJavadown(comment + 3, strlen(comment) - 3 - 2);
  EXPECT_FALSE(jd->isfileoverview);
  EXPECT_STREQ(title, jd->title);
  EXPECT_STREQ(description, jd->text);
  EXPECT_EQ(4, jd->tags.n);
  EXPECT_STREQ("param", jd->tags.p[0].tag);
  EXPECT_STREQ("data should point to text inside the slash star markers\n"
               "lorem ipsum dolla dollaz yo",
               jd->tags.p[0].text);
  EXPECT_STREQ("param", jd->tags.p[1].tag);
  EXPECT_STREQ("size is length of data in bytes", jd->tags.p[1].text);
  EXPECT_STREQ("return", jd->tags.p[2].tag);
  EXPECT_STREQ("object that should be passed to FreeJavadown()",
               jd->tags.p[2].text);
  EXPECT_STREQ("asyncsignalsafe", jd->tags.p[3].tag);
  FreeJavadown(jd);
}

TEST(ParseJavadown, testTabFormatting1) {
  const char *comment = "\
/**\n\
\tParses javadown    \n\
\tand that is the bottom line.\n\
\n\
\tLorem ipsum dolor sit amet, consectetur adipiscing elit, sed do\n\
\teiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim \n\
\tad minim veniam, quis nostrud exercitation ullamco laboris nisi ut\n\
\taliquip ex ea commodo consequat.\n\
\n\
\tDuis aute irure dolor in reprehenderit in voluptate velit esse cillum\n\
\tdolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat\n\
\tnon proident, sunt in culpa qui officia deserunt mollit anim id est\n\
\tlaborum\n\
\n\
\t@param data should point to text inside the slash star markers\n\
\t\tlorem ipsum dolla dollaz yo\n\
\t@param size is length of data in bytes\n\
\t@return object that should be passed to FreeJavadown()\n\
\t@asyncsignalsafe\n\
 */";
  const char *title = "Parses javadown and that is the bottom line.";
  const char *description = "\
Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do\n\
eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim\n\
ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut\n\
aliquip ex ea commodo consequat.\n\
\n\
Duis aute irure dolor in reprehenderit in voluptate velit esse cillum\n\
dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat\n\
non proident, sunt in culpa qui officia deserunt mollit anim id est\n\
laborum\n";
  struct Javadown *jd;
  jd = ParseJavadown(comment + 3, strlen(comment) - 3 - 2);
  EXPECT_FALSE(jd->isfileoverview);
  EXPECT_STREQ(title, jd->title);
  EXPECT_STREQ(description, jd->text);
  EXPECT_EQ(4, jd->tags.n);
  EXPECT_STREQ("param", jd->tags.p[0].tag);
  EXPECT_STREQ("data should point to text inside the slash star markers\n"
               "lorem ipsum dolla dollaz yo",
               jd->tags.p[0].text);
  EXPECT_STREQ("param", jd->tags.p[1].tag);
  EXPECT_STREQ("size is length of data in bytes", jd->tags.p[1].text);
  EXPECT_STREQ("return", jd->tags.p[2].tag);
  EXPECT_STREQ("object that should be passed to FreeJavadown()",
               jd->tags.p[2].text);
  EXPECT_STREQ("asyncsignalsafe", jd->tags.p[3].tag);
  FreeJavadown(jd);
}

TEST(ParseJavadown, testTabFormatting2) {
  const char *comment = "\
/**\n\
\tParses javadown    \n\
\tand that is the bottom line.\n\
\t\n\
\tLorem ipsum dolor sit amet, consectetur adipiscing elit, sed do\n\
\t\teiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim \n\
\t\tad minim veniam, quis nostrud exercitation ullamco laboris nisi ut\n\
\t\taliquip ex ea commodo consequat.\n\
\t\n\
\tDuis aute irure dolor in reprehenderit in voluptate velit esse cillum\n\
\tdolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat\n\
\tnon proident, sunt in culpa qui officia deserunt mollit anim id est\n\
\tlaborum\n\
\t\n\
\t@param data should point to text inside the slash star markers\n\
\t\tlorem ipsum dolla dollaz yo\n\
\t@param size is length of data in bytes\n\
\t@return object that should be passed to FreeJavadown()\n\
\t@asyncsignalsafe\n\
*/";
  const char *title = "Parses javadown and that is the bottom line.";
  const char *description = "\
Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do\n\
\teiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim\n\
\tad minim veniam, quis nostrud exercitation ullamco laboris nisi ut\n\
\taliquip ex ea commodo consequat.\n\
\n\
Duis aute irure dolor in reprehenderit in voluptate velit esse cillum\n\
dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat\n\
non proident, sunt in culpa qui officia deserunt mollit anim id est\n\
laborum\n";
  struct Javadown *jd;
  jd = ParseJavadown(comment + 3, strlen(comment) - 3 - 2);
  EXPECT_FALSE(jd->isfileoverview);
  EXPECT_STREQ(title, jd->title);
  EXPECT_STREQ(description, jd->text);
  EXPECT_EQ(4, jd->tags.n);
  EXPECT_STREQ("param", jd->tags.p[0].tag);
  EXPECT_STREQ("data should point to text inside the slash star markers\n"
               "lorem ipsum dolla dollaz yo",
               jd->tags.p[0].text);
  EXPECT_STREQ("param", jd->tags.p[1].tag);
  EXPECT_STREQ("size is length of data in bytes", jd->tags.p[1].text);
  EXPECT_STREQ("return", jd->tags.p[2].tag);
  EXPECT_STREQ("object that should be passed to FreeJavadown()",
               jd->tags.p[2].text);
  EXPECT_STREQ("asyncsignalsafe", jd->tags.p[3].tag);
  FreeJavadown(jd);
}

TEST(ParseJavadown, testAsmSyntax) {
  const char *comment = "\
/\tParses javadown    \n\
/\tand that is the bottom line.\n\
/\n\
/\tLorem ipsum dolor sit amet, consectetur adipiscing elit, sed do\n\
/\t\teiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim \n\
/\t\tad minim veniam, quis nostrud exercitation ullamco laboris nisi ut\n\
/\t\taliquip ex ea commodo consequat.\n\
/\n\
/\tDuis aute irure dolor in reprehenderit in voluptate velit esse cillum\n\
/\tdolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat\n\
/\tnon proident, sunt in culpa qui officia deserunt mollit anim id est\n\
/\tlaborum\n\
/\n\
/\t@param data should point to text inside the slash star markers\n\
/\t\tlorem ipsum dolla dollaz yo\n\
/\t@param size is length of data in bytes\n\
/\t@return object that should be passed to FreeJavadown()\n\
/\t@asyncsignalsafe\n";
  const char *title = "Parses javadown and that is the bottom line.";
  const char *description = "\
Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do\n\
\teiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim\n\
\tad minim veniam, quis nostrud exercitation ullamco laboris nisi ut\n\
\taliquip ex ea commodo consequat.\n\
\n\
Duis aute irure dolor in reprehenderit in voluptate velit esse cillum\n\
dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat\n\
non proident, sunt in culpa qui officia deserunt mollit anim id est\n\
laborum\n";
  struct Javadown *jd;
  jd = ParseJavadown(comment, strlen(comment));
  EXPECT_FALSE(jd->isfileoverview);
  EXPECT_STREQ(title, jd->title);
  EXPECT_STREQ(description, jd->text);
  EXPECT_EQ(4, jd->tags.n);
  EXPECT_STREQ("param", jd->tags.p[0].tag);
  EXPECT_STREQ("data should point to text inside the slash star markers\n"
               "lorem ipsum dolla dollaz yo",
               jd->tags.p[0].text);
  EXPECT_STREQ("param", jd->tags.p[1].tag);
  EXPECT_STREQ("size is length of data in bytes", jd->tags.p[1].text);
  EXPECT_STREQ("return", jd->tags.p[2].tag);
  EXPECT_STREQ("object that should be passed to FreeJavadown()",
               jd->tags.p[2].text);
  EXPECT_STREQ("asyncsignalsafe", jd->tags.p[3].tag);
  FreeJavadown(jd);
}
