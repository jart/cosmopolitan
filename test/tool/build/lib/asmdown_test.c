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
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "tool/build/lib/asmdown.h"

TEST(ParseAsmdown, test) {
  struct Asmdown *ad;
  const char *s = "\
#include \"libc/macros.h\"\n\
.source	__FILE__\n\
\n\
/	Returns absolute value of double.\n\
/\n\
/	@param	xmm0 has double in lower half\n\
/	@return	xmm0 has result in lower half\n\
fabs:	.leafprologue\n\
	.profilable\n\
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

  EXPECT_EQ(17, ad->symbols.p[1].line);
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
#include \"libc/macros.h\"\n\
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
