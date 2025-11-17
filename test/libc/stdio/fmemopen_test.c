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
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/temp.h"
#include "libc/testlib/testlib.h"

FILE *f;
char a[16];
char b[16];
char c[16];

TEST(fmemopen, einval) {
  ASSERT_SYS(EINVAL, 0, fmemopen("dog", 3, ""));
  ASSERT_SYS(EINVAL, 0, fmemopen("dog", 3, "q"));
  ASSERT_SYS(ENOMEM, 0, fmemopen(0, -1, "w+"));
}

TEST(fmemopen, seekPastEnd) {
  ASSERT_TRUE(!!(f = fmemopen("hi", 2, "r")));
  ASSERT_SYS(0, 0, ftell(f));
  ASSERT_SYS(0, 0, fseek(f, 0, SEEK_END));
  ASSERT_SYS(0, 2, ftell(f));
  ASSERT_SYS(EINVAL, -1, fseek(f, 3, SEEK_SET));
  ASSERT_EQ(0, fclose(f));
}

TEST(fmemopen, seekNegative) {
  ASSERT_TRUE(!!(f = fmemopen("hi", 2, "r")));
  ASSERT_SYS(EINVAL, -1, fseek(f, -2, SEEK_SET));
  ASSERT_EQ(0, fclose(f));
}

TEST(fmemopen, testWriteRewindRead) {
  char c;
  ASSERT_TRUE(!!(f = fmemopen(NULL, BUFSIZ, "w+")));
  ASSERT_EQ(1, fwrite("c", 1, 1, f));
  rewind(f);
  ASSERT_EQ(1, fread(&c, 1, 1, f));
  ASSERT_EQ('c', c);
  ASSERT_EQ(0, fclose(f));
}

TEST(fmemopen_fprintf, test) {
  ASSERT_TRUE(!!(f = fmemopen(NULL, BUFSIZ, "w+")));
  ASSERT_SYS(0, 1, fprintf(f, "%ld", 0L));
  rewind(f);
  ASSERT_SYS(0, 1, fread(a, 1, 8, f));
  ASSERT_EQ('0', a[0]);
  ASSERT_SYS(0, 0, fclose(f));
}

TEST(fmemopen, seekEofRead) {
  ASSERT_TRUE(!!(f = fmemopen("x", 1, "r+")));
  ASSERT_SYS(EINVAL, -1, fseek(f, -1, SEEK_SET));
  ASSERT_SYS(EINVAL, -1, fseek(f, +1, SEEK_END));
  ASSERT_EQ(0, fseek(f, 0, SEEK_END));
  ASSERT_FALSE(feof(f));
  ASSERT_EQ(-1, fgetc(f));
  ASSERT_TRUE(feof(f));
  ASSERT_EQ(0, fclose(f));
}

TEST(tmpfile_fprintf, test) {
  ASSERT_TRUE(!!(f = tmpfile()));
  ASSERT_SYS(0, 1, fprintf(f, "%ld", 0L));
  rewind(f);
  ASSERT_SYS(0, 1, fread(a, 1, 8, f));
  ASSERT_EQ('0', a[0]);
  ASSERT_SYS(0, 0, fclose(f));
}

TEST(fmemopen, small) {
  char m[16];
  memset(m, 1, 16);
  ASSERT_TRUE(!!(f = fmemopen(m, 1, "w+")));
  ASSERT_EQ(3, fprintf(f, "%ld", 333L));
  ASSERT_TRUE(errno == ENOSPC || errno == 0);
  errno = 0;
  ASSERT_FALSE(ferror(f));
  ASSERT_FALSE(feof(f));
  int rc = fflush(f);
  ASSERT_TRUE(rc == 0 || rc == EOF);
  ASSERT_TRUE(errno == ENOSPC || errno == 0);
  errno = 0;
  ASSERT_SYS(0, 0, fseek(f, 0, SEEK_SET));
#ifndef __GLIBC__
  ASSERT_EQ(1, fread(a, 1, 8, f));
  ASSERT_EQ('3', m[0]);
#endif
  ASSERT_EQ(1, m[1]);
  ASSERT_EQ(0, fclose(f));
}

TEST(fmemopen, read) {
  char s[] = "foo bar baz\n"
             "bog hog dog\n";
  ASSERT_TRUE(!!(f = fmemopen(s, sizeof(s) - 1, "r")));
  ASSERT_SYS(0, 3, fscanf(f, "%s%s%s", a, b, c));
  ASSERT_STREQ("foo", a);
  ASSERT_STREQ("bar", b);
  ASSERT_STREQ("baz", c);
  ASSERT_SYS(0, 3, fscanf(f, "%s%s%s", a, b, c));
  ASSERT_STREQ("bog", a);
  ASSERT_STREQ("hog", b);
  ASSERT_STREQ("dog", c);
  ASSERT_SYS(0, -1, fscanf(f, "%s%s%s", a, b, c));
#ifdef __MUSL__
  ASSERT_SYS(0, 0, fwrite(a, 1, 16, f));
#else
  ASSERT_SYS(EBADF, 0, fwrite(a, 1, 16, f));
#endif
  ASSERT_SYS(0, 0, fclose(f));
}

TEST(fmemopen, read_nul) {
  char s[] = "foo bar baz\0"
             "bog hog dog\n";
  ASSERT_TRUE(!!(f = fmemopen(s, sizeof(s) - 1, "r")));
  ASSERT_SYS(0, 3, fscanf(f, "%s%s%s", a, b, c));
  ASSERT_STREQ("foo", a);
  ASSERT_STREQ("bar", b);
  ASSERT_EQ(0, memcmp(c, "baz\0bog\0", 8));
  ASSERT_SYS(0, 2, fscanf(f, "%s%s%s", a, b, c));
  ASSERT_STREQ("hog", a);
  ASSERT_STREQ("dog", b);
  ASSERT_EQ(-1, fscanf(f, "%s%s%s", a, b, c));
  ASSERT_SYS(0, 0, fclose(f));
}

TEST(fmemopen, a) {
  char s[4] = "a\0b\1";
  ASSERT_TRUE(!!(f = fmemopen(s, 3, "a")));
  ASSERT_SYS(0, 1, ftell(f));
  ASSERT_SYS(0, 'z', fputc('z', f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_EQ('a', s[0]);
  ASSERT_EQ('z', s[1]);
  ASSERT_EQ('\0', s[2]);
  ASSERT_SYS(0, 2, ftell(f));
  ASSERT_SYS(0, 1, fwrite("Z", 1, 1, f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_SYS(0, 3, ftell(f));
  ASSERT_EQ('a', s[0]);
  ASSERT_EQ('z', s[1]);
#if defined(__MUSL__)
  ASSERT_EQ('\0', s[2]);
#else
  ASSERT_EQ('Z', s[2]);
#endif
  ASSERT_EQ('\1', s[3]);
  ASSERT_SYS(0, 0, fseek(f, 1, SEEK_SET));
#ifdef __MUSL__
  ASSERT_SYS(0, 0, fread(a, 1, 16, f));
#else
  ASSERT_SYS(EBADF, 0, fread(a, 1, 16, f));
#endif
  ASSERT_SYS(0, 0, fclose(f));
}

TEST(fmemopen, a2) {
  char s[4] = "a\0b\1";
  ASSERT_TRUE(!!(f = fmemopen(s, 3, "a")));
  ASSERT_SYS(0, 1, fwrite("z", 1, 1, f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_EQ('a', s[0]);
  ASSERT_EQ('z', s[1]);
  ASSERT_EQ('\0', s[2]);
  ASSERT_SYS(0, 1, fwrite("Z", 1, 1, f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_EQ('a', s[0]);
  ASSERT_EQ('z', s[1]);
#if defined(__MUSL__)
  ASSERT_EQ('\0', s[2]);
#else
  ASSERT_EQ('Z', s[2]);
#endif
  ASSERT_EQ('\1', s[3]);
  ASSERT_SYS(0, 0, fclose(f));
}

TEST(fmemopen, ap) {
  char s[] = "a\0b";
  ASSERT_TRUE(!!(f = fmemopen(s, 3, "a+")));
  ASSERT_SYS(0, EOF, fgetc(f));
  ASSERT_SYS(0, 1, ftell(f));
  ASSERT_SYS(0, 0, fseek(f, 0, SEEK_SET));
  ASSERT_SYS(0, 0, ftell(f));
  ASSERT_SYS(0, 'a', fgetc(f));
  ASSERT_SYS(0, EOF, fgetc(f));
  ASSERT_SYS(0, 0, fclose(f));
}

TEST(fmemopen, ap2) {
  char s[] = "a\0b";
  ASSERT_TRUE(!!(f = fmemopen(s, 3, "a+")));
  ASSERT_SYS(0, 1, ftell(f));
  ASSERT_SYS(0, 0, fread(a, 1, 16, f));
  ASSERT_SYS(0, 1, ftell(f));
  ASSERT_SYS(0, 0, fseek(f, 0, SEEK_SET));
  ASSERT_SYS(0, 0, ftell(f));
  ASSERT_SYS(0, 1, fread(a, 1, 16, f));
  ASSERT_EQ('a', a[0]);
  ASSERT_SYS(0, 0, fclose(f));
}

TEST(fmemopen, ap3) {
  char s[] = "a\0b";
  ASSERT_TRUE(!!(f = fmemopen(s, 3, "a+")));
  ASSERT_SYS(0, 0, fseek(f, 0, SEEK_SET));
  ASSERT_SYS(0, 0, ftell(f));
  ASSERT_SYS(0, 'x', fputc('x', f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_SYS(0, 2, ftell(f));
  ASSERT_EQ('a', s[0]);
  ASSERT_EQ('x', s[1]);
  ASSERT_EQ('\0', s[2]);
  ASSERT_SYS(0, 0, fclose(f));
}

TEST(fmemopen, ap4) {
  char s[] = "a\0b";
  ASSERT_TRUE(!!(f = fmemopen(s, 3, "a+")));
  ASSERT_SYS(0, 0, fseek(f, 0, SEEK_SET));
  ASSERT_SYS(0, 0, ftell(f));
  ASSERT_SYS(0, 1, fwrite("x", 1, 1, f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_SYS(0, 2, ftell(f));
  ASSERT_EQ('a', s[0]);
  ASSERT_EQ('x', s[1]);
  ASSERT_EQ('\0', s[2]);
  ASSERT_SYS(0, 0, fclose(f));
}

TEST(fmemopen, lawlessness) {
  char s[] = "abcd";
  ASSERT_TRUE(!!(f = fmemopen(s, 2, "w+")));
  ASSERT_SYS(0, 0, fseek(f, 0, SEEK_END));
  ASSERT_SYS(0, 0, ftell(f));
  ASSERT_SYS(0, 'x', fputc('x', f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_SYS(0, 1, ftell(f));
  ASSERT_EQ('x', s[0]);
  ASSERT_EQ('\0', s[1]);
  ASSERT_EQ('c', s[2]);
  ASSERT_SYS(0, 'y', fputc('y', f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_SYS(0, 2, ftell(f));
  ASSERT_EQ('x', s[0]);
#if defined(__MUSL__) || defined(__COSMOPOLITAN__)
  ASSERT_EQ('y', s[1]);
#else
  ASSERT_EQ('\0', s[1]);
#endif
  ASSERT_EQ('c', s[2]);
  ASSERT_SYS(0, 0, fclose(f));
}

TEST(fmemopen, lawlessness2) {
  char s[] = "abcd";
  ASSERT_TRUE(!!(f = fmemopen(s, 2, "w+")));
  ASSERT_SYS(0, 0, fseek(f, 0, SEEK_END));
  ASSERT_SYS(0, 0, ftell(f));
  ASSERT_TRUE(fputs("xy", f) >= 0);
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_SYS(0, 2, ftell(f));
  ASSERT_EQ('x', s[0]);
#if defined(__MUSL__) || defined(__COSMOPOLITAN__)
  ASSERT_EQ('y', s[1]);
#else
  ASSERT_EQ('\0', s[1]);
#endif
  ASSERT_EQ('c', s[2]);
  ASSERT_SYS(0, 0, fclose(f));
}

TEST(fmemopen, seekPastNul) {
  char s[3] = "a\0b";
  ASSERT_TRUE(!!(f = fmemopen(s, 3, "a+")));
  ASSERT_SYS(0, 0, fseek(f, 0, SEEK_END));
  ASSERT_SYS(0, 1, ftell(f));
  ASSERT_SYS(0, 0, fseek(f, 3, SEEK_SET));
  ASSERT_SYS(0, 3, ftell(f));
  ASSERT_SYS(EINVAL, -1, fseek(f, 4, SEEK_SET));
  ASSERT_SYS(0, 0, fclose(f));
}

TEST(fmemopen, w) {
  char s[2] = {1, 1};
  ASSERT_TRUE(!!(f = fmemopen(s, 2, "w")));
  ASSERT_SYS(0, 'x', fputc('x', f));
  ASSERT_SYS(0, 'y', fputc('y', f));
#ifdef __COSMOPOLITAN__
  ASSERT_SYS(ENOSPC, -1, fputc('z', f));
  ASSERT_SYS(0, 2, ftell(f));
  ASSERT_SYS(0, 0, fflush(f));
#elif defined(__MUSL__)
  ASSERT_SYS(0, 'z', fputc('z', f));
  ASSERT_SYS(0, 3, ftell(f));
  ASSERT_SYS(0, 0, fflush(f));
#elif defined(__GLIBC__)
  ASSERT_SYS(0, 'z', fputc('z', f));
  ASSERT_SYS(0, 3, ftell(f));
  ASSERT_SYS(0, -1, fflush(f));
  ASSERT_SYS(0, 0, fflush(f));
#endif
  ASSERT_SYS(0, 0, fclose(f));
}

TEST(fmemopen, truncate_posix2024) {
#ifdef __COSMOPOLITAN__
  char buf[4] = {1, 1, 1, 1};
  ASSERT_TRUE(!!(f = fmemopen(buf, 4, "w")));
  ASSERT_SYS(0, 0, fseek(f, 0, SEEK_END));
  ASSERT_SYS(0, 0, ftell(f));
  ASSERT_EQ(0, buf[0]);
  ASSERT_SYS(0, 0, fclose(f));
#endif
}

TEST(fmemopen, zeroalloc) {
  ASSERT_TRUE(!!(f = fmemopen(NULL, 0, "w+")));
#ifdef __COSMOPOLITAN__
  ASSERT_SYS(ENOSPC, EOF, fputc('x', f));
#else
  ASSERT_SYS(0, 'x', fputc('x', f));
  ASSERT_SYS(ENOSPC, EOF, fflush(f));
#endif
  ASSERT_SYS(0, 0, fclose(f));
}

size_t huge = -1;
TEST(dog, test) {
  ASSERT_EQ(NULL, malloc(huge));
  ASSERT_EQ(ENOMEM, errno);
  errno = 0;
}

TEST(fmemopen, ungetc) {
  ASSERT_TRUE(!!(f = fmemopen(NULL, 128, "w+")));
  ASSERT_SYS(0, 'x', ungetc('x', f));
  ASSERT_SYS(0, 0, ftell(f));
  ASSERT_SYS(0, 'x', fgetc(f));
  ASSERT_SYS(0, 0, fclose(f));
}

TEST(fmemopen, ungetc2) {
  char buf[8] = "hello";
  ASSERT_TRUE(!!(f = fmemopen(buf, 8, "w+")));
  ASSERT_SYS(0, EOF, fgetc(f));
  ASSERT_SYS(0, 'x', ungetc('x', f));
#if defined(__COSMOPOLITAN__)
  ASSERT_SYS(0, 0, ftell(f));
#elif defined(__MUSL__)
  ASSERT_SYS(0, EOF, ftell(f));
#elif defined(__GLIBC__)
  ASSERT_SYS(EINVAL, EOF, ftell(f));
#endif
  ASSERT_SYS(0, 'x', fgetc(f));
  ASSERT_SYS(0, 0, fclose(f));
}

TEST(fmemopen, sneakyBuffer) {
  // don't understand why this is the glibc and musl behavior
  char b1[12] = {0};
  ASSERT_NE(NULL, (f = fmemopen(b1, 12, "w+")));
  ASSERT_TRUE(fputs("abacfoobar", f) >= 0);
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_EQ(0, memcmp(b1, "abacfoobar\0", 12));
  char b2[12];
  memset(b2, 1, 12);
  ASSERT_SYS(0, 0, setvbuf(f, b2, _IOFBF, 8));
  ASSERT_EQ(10, ftell(f));
  ASSERT_TRUE(fputs("x", f) >= 0);
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_EQ(0, memcmp(b1, "abacfoobarx", 12));
  ASSERT_EQ(0, memcmp(b2, "\1\1\1\1\1\1\1\1\1\1\1\1", 12));
  ASSERT_NE(-1, fclose(f));
}

TEST(fmemopen, sneakyBuffer2) {
  // don't understand why this is the glibc and musl behavior
  char b1[12] = {0};
  ASSERT_NE(NULL, (f = fmemopen(b1, 12, "w+")));
  ASSERT_TRUE(fputs("abacfoobar", f) >= 0);
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_EQ(0, memcmp(b1, "abacfoobar\0", 12));
  char b2[12];
  memset(b2, 1, 12);
  ASSERT_SYS(0, 0, setvbuf(f, b2, _IONBF, 8));
  ASSERT_EQ(10, ftell(f));
  ASSERT_TRUE(fputs("x", f) >= 0);
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_EQ(0, memcmp(b1, "abacfoobarx", 12));
  ASSERT_EQ(0, memcmp(b2, "\1\1\1\1\1\1\1\1\1\1\1\1", 12));
  ASSERT_NE(-1, fclose(f));
}

TEST(fmemopen, seek) {
  FILE *f;
  char buf[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  ASSERT_NE(NULL, (f = fmemopen(buf, 10, "w+")));
  ASSERT_SYS(0, 0, fseek(f, 4, SEEK_SET));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_SYS(0, 4, ftell(f));
  ASSERT_EQ(0, buf[0]);
  ASSERT_EQ(1, buf[1]);
  ASSERT_EQ(1, buf[2]);
  ASSERT_EQ(1, buf[3]);
  ASSERT_EQ(1, buf[4]);
  ASSERT_SYS(0, 0, fclose(f));
}
