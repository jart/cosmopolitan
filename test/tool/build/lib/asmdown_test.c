/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=8 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "tool/build/lib/asmdown.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(ParseAsmdown, test) {
  struct Asmdown *ad;
  const char *s = "\
#include \"libc/macros.internal.h\"\n\
.source	__FILE__\n\
\n\
/	Returns absolute value of double.\n\
/\n\
/	@param	xmm0 has double in lower half\n\
/	@return	xmm0 has result in lower half\n\
	.ftrace1\n\
fabs:	.ftrace2\n\
	.leafprologue\n\
	mov	$0x7fffffffffffffff,%rax\n\
	movq	%xmm0,%rdx\n\
	and	%rax,%rdx\n\
	movq	%rdx,%xmm0\n\
	.leafepilogue\n\
	.endfn	fabs,globl\n\
\n\
/	Returns arc cosine of 𝑥.\n\
/\n\
/	This is a description.\n\
/\n\
/	@param	𝑥 is double scalar in low half of %xmm0\n\
/	@return	double scalar in low half of %xmm0\n\
tinymath_acos:\n\
	ezlea	tinymath_acosl,ax\n\
	jmp	_d2ld2\n\
	.endfn	tinymath_acos,globl\n\
";
  ad = ParseAsmdown(s, strlen(s));
  ASSERT_EQ(2, ad->symbols.n);

  EXPECT_EQ(4, ad->symbols.p[0].line);
  EXPECT_STREQ("fabs", ad->symbols.p[0].name);
  EXPECT_FALSE(ad->symbols.p[0].javadown->isfileoverview);
  EXPECT_STREQ("Returns absolute value of double.",
               ad->symbols.p[0].javadown->title);
  EXPECT_STREQ("", ad->symbols.p[0].javadown->text);
  EXPECT_EQ(2, ad->symbols.p[0].javadown->tags.n);
  EXPECT_STREQ("param", ad->symbols.p[0].javadown->tags.p[0].tag);
  EXPECT_STREQ("xmm0 has double in lower half",
               ad->symbols.p[0].javadown->tags.p[0].text);
  EXPECT_STREQ("return", ad->symbols.p[0].javadown->tags.p[1].tag);
  EXPECT_STREQ("xmm0 has result in lower half",
               ad->symbols.p[0].javadown->tags.p[1].text);

  EXPECT_EQ(18, ad->symbols.p[1].line);
  EXPECT_STREQ("tinymath_acos", ad->symbols.p[1].name);
  EXPECT_FALSE(ad->symbols.p[1].javadown->isfileoverview);
  EXPECT_STREQ("Returns arc cosine of 𝑥.", ad->symbols.p[1].javadown->title);
  EXPECT_STREQ("This is a description.\n", ad->symbols.p[1].javadown->text);
  EXPECT_EQ(2, ad->symbols.p[1].javadown->tags.n);
  EXPECT_STREQ("param", ad->symbols.p[1].javadown->tags.p[0].tag);
  EXPECT_STREQ("𝑥 is double scalar in low half of %xmm0",
               ad->symbols.p[1].javadown->tags.p[0].text);
  EXPECT_STREQ("return", ad->symbols.p[1].javadown->tags.p[1].tag);
  EXPECT_STREQ("double scalar in low half of %xmm0",
               ad->symbols.p[1].javadown->tags.p[1].text);

  FreeAsmdown(ad);
}

TEST(ParseAsmdown, testAlias) {
  struct Asmdown *ad;
  const char *s = "\
#include \"libc/macros.internal.h\"\n\
.source	__FILE__\n\
\n\
/	Returns arc cosine of 𝑥.\n\
/\n\
/	This is a description.\n\
/\n\
/	@param	𝑥 is double scalar in low half of %xmm0\n\
/	@return	double scalar in low half of %xmm0\n\
tinymath_acos:\n\
	ezlea	tinymath_acosl,ax\n\
	jmp	_d2ld2\n\
	.endfn	tinymath_acos,globl\n\
	.alias	tinymath_acos,acos\n\
";
  ad = ParseAsmdown(s, strlen(s));
  ASSERT_EQ(2, ad->symbols.n);

  EXPECT_EQ(4, ad->symbols.p[0].line);
  EXPECT_STREQ("tinymath_acos", ad->symbols.p[0].name);
  EXPECT_FALSE(ad->symbols.p[0].javadown->isfileoverview);
  EXPECT_STREQ("Returns arc cosine of 𝑥.", ad->symbols.p[0].javadown->title);
  EXPECT_STREQ("This is a description.\n", ad->symbols.p[0].javadown->text);
  EXPECT_EQ(2, ad->symbols.p[0].javadown->tags.n);
  EXPECT_STREQ("param", ad->symbols.p[0].javadown->tags.p[0].tag);
  EXPECT_STREQ("𝑥 is double scalar in low half of %xmm0",
               ad->symbols.p[0].javadown->tags.p[0].text);
  EXPECT_STREQ("return", ad->symbols.p[0].javadown->tags.p[1].tag);
  EXPECT_STREQ("double scalar in low half of %xmm0",
               ad->symbols.p[0].javadown->tags.p[1].text);

  EXPECT_EQ(4, ad->symbols.p[1].line);
  EXPECT_STREQ("acos", ad->symbols.p[1].name);
  EXPECT_FALSE(ad->symbols.p[1].javadown->isfileoverview);
  EXPECT_STREQ("Returns arc cosine of 𝑥.", ad->symbols.p[1].javadown->title);
  EXPECT_STREQ("This is a description.\n", ad->symbols.p[1].javadown->text);
  EXPECT_EQ(2, ad->symbols.p[1].javadown->tags.n);
  EXPECT_STREQ("param", ad->symbols.p[1].javadown->tags.p[0].tag);
  EXPECT_STREQ("𝑥 is double scalar in low half of %xmm0",
               ad->symbols.p[1].javadown->tags.p[0].text);
  EXPECT_STREQ("return", ad->symbols.p[1].javadown->tags.p[1].tag);
  EXPECT_STREQ("double scalar in low half of %xmm0",
               ad->symbols.p[1].javadown->tags.p[1].text);

  FreeAsmdown(ad);
}

TEST(ParseAsmdown, testClangIsEvil) {
  struct Asmdown *ad;
  const char *s = "\
#include \"libc/macros.internal.h\"\n\
.source	__FILE__\n\
\n\
//	Returns arc cosine of 𝑥.\n\
//\n\
//	This is a description.\n\
//\n\
//	@param	𝑥 is double scalar in low half of %xmm0\n\
//	@return	double scalar in low half of %xmm0\n\
tinymath_acos:\n\
	ezlea	tinymath_acosl,ax\n\
	jmp	_d2ld2\n\
	.endfn	tinymath_acos,globl\n\
	.alias	tinymath_acos,acos\n\
";
  ad = ParseAsmdown(s, strlen(s));
  ASSERT_EQ(2, ad->symbols.n);

  EXPECT_EQ(4, ad->symbols.p[0].line);
  EXPECT_STREQ("tinymath_acos", ad->symbols.p[0].name);
  EXPECT_FALSE(ad->symbols.p[0].javadown->isfileoverview);
  EXPECT_STREQ("Returns arc cosine of 𝑥.", ad->symbols.p[0].javadown->title);
  EXPECT_STREQ("This is a description.\n", ad->symbols.p[0].javadown->text);
  EXPECT_EQ(2, ad->symbols.p[0].javadown->tags.n);
  EXPECT_STREQ("param", ad->symbols.p[0].javadown->tags.p[0].tag);
  EXPECT_STREQ("𝑥 is double scalar in low half of %xmm0",
               ad->symbols.p[0].javadown->tags.p[0].text);
  EXPECT_STREQ("return", ad->symbols.p[0].javadown->tags.p[1].tag);
  EXPECT_STREQ("double scalar in low half of %xmm0",
               ad->symbols.p[0].javadown->tags.p[1].text);

  EXPECT_EQ(4, ad->symbols.p[1].line);
  EXPECT_STREQ("acos", ad->symbols.p[1].name);
  EXPECT_FALSE(ad->symbols.p[1].javadown->isfileoverview);
  EXPECT_STREQ("Returns arc cosine of 𝑥.", ad->symbols.p[1].javadown->title);
  EXPECT_STREQ("This is a description.\n", ad->symbols.p[1].javadown->text);
  EXPECT_EQ(2, ad->symbols.p[1].javadown->tags.n);
  EXPECT_STREQ("param", ad->symbols.p[1].javadown->tags.p[0].tag);
  EXPECT_STREQ("𝑥 is double scalar in low half of %xmm0",
               ad->symbols.p[1].javadown->tags.p[0].text);
  EXPECT_STREQ("return", ad->symbols.p[1].javadown->tags.p[1].tag);
  EXPECT_STREQ("double scalar in low half of %xmm0",
               ad->symbols.p[1].javadown->tags.p[1].text);

  FreeAsmdown(ad);
}
