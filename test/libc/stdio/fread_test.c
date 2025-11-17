/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/temp.h"
#include "libc/testlib/benchmark.h"
#include "libc/testlib/testlib.h"

static const char kChinese8[] = "\
此開卷第一回也．作者自云：因曾歷過一番夢幻之后，故將真事隱去，\n\
而借\"通靈\"之說，撰此《石頭記》一書也．故曰\"甄士隱\"云云．但書中所記\n\
何事何人？自又云：“今風塵碌碌，一事無成，忽念及當日所有之女子，一\n\
一細考較去，覺其行止見識，皆出于我之上．何我堂堂須眉，誠不若彼裙釵\n\
哉？實愧則有余，悔又無益之大無可如何之日也！當此，則自欲將已往所賴\n\
天恩祖德，錦衣紈褲之時，飫甘饜肥之日，背父兄教育之恩，負師友規談之\n\
德，以至今日一技無成，半生潦倒之罪，編述一集，以告天下人：我之罪固\n\
不免，然閨閣中本自歷歷有人，万不可因我之不肖，自護己短，一并使其泯\n\
滅也．雖今日之茅椽蓬牖，瓦灶繩床，其晨夕風露，階柳庭花，亦未有妨我\n\
之襟怀筆墨者．雖我未學，下筆無文，又何妨用假語村言，敷演出一段故事\n\
來，亦可使閨閣昭傳，复可悅世之目，破人愁悶，不亦宜乎？\"故曰\"賈雨村\n";

static const wchar_t kChinese32[] = L"\
此開卷第一回也．作者自云：因曾歷過一番夢幻之后，故將真事隱去，\n\
而借\"通靈\"之說，撰此《石頭記》一書也．故曰\"甄士隱\"云云．但書中所記\n\
何事何人？自又云：“今風塵碌碌，一事無成，忽念及當日所有之女子，一\n\
一細考較去，覺其行止見識，皆出于我之上．何我堂堂須眉，誠不若彼裙釵\n\
哉？實愧則有余，悔又無益之大無可如何之日也！當此，則自欲將已往所賴\n\
天恩祖德，錦衣紈褲之時，飫甘饜肥之日，背父兄教育之恩，負師友規談之\n\
德，以至今日一技無成，半生潦倒之罪，編述一集，以告天下人：我之罪固\n\
不免，然閨閣中本自歷歷有人，万不可因我之不肖，自護己短，一并使其泯\n\
滅也．雖今日之茅椽蓬牖，瓦灶繩床，其晨夕風露，階柳庭花，亦未有妨我\n\
之襟怀筆墨者．雖我未學，下筆無文，又何妨用假語村言，敷演出一段故事\n\
來，亦可使閨閣昭傳，复可悅世之目，破人愁悶，不亦宜乎？\"故曰\"賈雨村\n";

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(fread, eofIsSticky) {
  FILE *fo, *fi;
  char b[10] = "hello";
  char c[10] = {0};
  ASSERT_NE(NULL, (fo = fopen("foo", "w")));
  ASSERT_NE(NULL, (fi = fopen("foo", "r")));
  ASSERT_EQ(0, fread(b, 1, 8, fi));
  ASSERT_TRUE(feof(fi) != 0);
  ASSERT_EQ(8, fwrite(b, 1, 8, fo));
  ASSERT_EQ(0, fflush(fo));
  ASSERT_EQ(0, fread(c, 1, 10, fi));
  ASSERT_TRUE(feof(fi) != 0);
  clearerr(fi);
  ASSERT_EQ(8, fread(c, 1, 10, fi));
  ASSERT_TRUE(feof(fi) != 0);
  ASSERT_STREQ("hello", c);
  ASSERT_EQ(0, fseek(fi, 0, SEEK_SET));
  ASSERT_FALSE(feof(fi));
  ASSERT_EQ('h', fgetc(fi));
  ASSERT_FALSE(feof(fi));
  ASSERT_EQ(7, fread(c, 1, 10, fi));
  ASSERT_EQ(0, fclose(fi));
  ASSERT_EQ(0, fclose(fo));
}

TEST(fread, seekWithBuffer) {
  FILE *f;
  char b[8] = "hellosup";
  char c[8] = {0};
  char d[8] = {0};
  ASSERT_NE(NULL, (f = fopen("foo", "w")));
  ASSERT_EQ(8, fwrite(b, 1, 8, f));
  ASSERT_EQ(0, fclose(f));
  ASSERT_NE(NULL, (f = fopen("foo", "r")));
  ASSERT_EQ(5, fread(c, 1, 5, f));
  ASSERT_STREQ("hello", c);
  ASSERT_EQ(0, fseek(f, 1, SEEK_SET));
  ASSERT_EQ(5, fread(d, 1, 5, f));
  ASSERT_STREQ("ellos", d);
  ASSERT_EQ(0, fclose(f));
}

TEST(fread, zero) {
  FILE *f;
  char buf[8] = {0};
  ASSERT_NE(NULL, (f = fopen("foo", "w")));
  ASSERT_EQ(2, fwrite("hi", 1, 2, f));
  ASSERT_EQ(0, fclose(f));
  ASSERT_NE(NULL, (f = fopen("foo", "r")));
  ASSERT_EQ(0, fread(buf, 0, 0, f));
  ASSERT_EQ(0, ferror(stdin));
  ASSERT_EQ(0, feof(stdin));
  ASSERT_STREQ("", buf);
  ASSERT_EQ(0, fclose(f));
}

TEST(fread, partial) {
  FILE *f;
  char buf[8] = {0};
  ASSERT_NE(NULL, (f = fopen("foo", "w")));
  ASSERT_EQ(2, fwrite("hi", 1, 2, f));
  ASSERT_EQ(0, fclose(f));
  ASSERT_NE(NULL, (f = fopen("foo", "r")));
  ASSERT_EQ(0, fread(buf, 8, 1, f));
  ASSERT_EQ(0, ferror(stdin));
  ASSERT_EQ(0, feof(stdin));
  ASSERT_EQ(0, fclose(f));
  ASSERT_STREQ("hi", buf);
}

TEST(stdio, mixedBuffering) {
  int fd;
  FILE *f;
  char path[] = "/tmp/fwrite_test.XXXXXX";
  ASSERT_NE(-1, (fd = mkstemp(path)));
  ASSERT_SYS(0, 0, close(fd));
  ASSERT_NE(NULL, (f = fopen(path, "w+")));
  ASSERT_SYS(0, 4, fwrite("love", 1, 4, f));
  ASSERT_SYS(0, 0, fclose(f));
  ASSERT_NE(NULL, (f = fopen(path, "r+")));
  ASSERT_SYS(0, 'L', fputc('L', f));
  ASSERT_SYS(0, 0, fclose(f));
  ASSERT_NE(NULL, (f = fopen(path, "r+")));
  char b[16] = {0};
  ASSERT_SYS(0, 4, fread(b, 1, 16, f));
  ASSERT_STREQ("Love", b);
  ASSERT_SYS(0, 0, fclose(f));
  ASSERT_NE(NULL, (f = fopen(path, "r+")));
  ASSERT_SYS(0, 'L', fgetc(f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_SYS(0, 'O', fputc('O', f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_SYS(0, 'v', fgetc(f));
  ASSERT_SYS(0, 0, fclose(f));
  ASSERT_NE(NULL, (f = fopen(path, "r+")));
  char a[16] = {0};
  ASSERT_SYS(0, 4, fread(a, 1, 16, f));
  ASSERT_STREQ("LOve", a);
  ASSERT_SYS(0, 0, fclose(f));
  ASSERT_SYS(0, 0, unlink(path));
}

TEST(stdio, pushback) {
  int fd;
  FILE *f;
  char path[] = "/tmp/fwrite_test.XXXXXX";
  ASSERT_NE(-1, (fd = mkstemp(path)));
  ASSERT_SYS(0, 0, close(fd));
  ASSERT_NE(NULL, (f = fopen(path, "w+")));
  ASSERT_SYS(0, 4, fwrite("love", 1, 4, f));
  ASSERT_SYS(0, 0, fclose(f));
  ASSERT_NE(NULL, (f = fopen(path, "r+")));
  ASSERT_SYS(0, 'l', fgetc(f));
  ASSERT_SYS(0, 'o', fgetc(f));
  ASSERT_SYS(0, 'Z', ungetc('Z', f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_SYS(0, 'O', fputc('O', f));
  ASSERT_SYS(0, 0, fclose(f));
  ASSERT_NE(NULL, (f = fopen(path, "r+")));
  char b[16] = {0};
  ASSERT_SYS(0, 4, fread(b, 1, 16, f));
  ASSERT_STREQ("lOve", b);
  ASSERT_SYS(0, 0, fclose(f));
  ASSERT_SYS(0, 0, unlink(path));
}

TEST(stdio, pushback2) {
  int fd;
  FILE *f;
  char buf[512];
  char path[] = "/tmp/fwrite_test.XXXXXX";
  ASSERT_NE(-1, (fd = mkstemp(path)));
  ASSERT_SYS(0, 0, close(fd));
  ASSERT_NE(NULL, (f = fopen(path, "w+")));
  ASSERT_SYS(0, 4, fwrite("love", 1, 4, f));
  ASSERT_SYS(0, 0, fclose(f));
  ASSERT_NE(NULL, (f = fopen(path, "r+")));
  ASSERT_SYS(0, 0, setvbuf(f, buf, _IOFBF, 512));
  ASSERT_SYS(0, 'Z', ungetc('Z', f));
  int rc = fflush(f);
  ASSERT_TRUE(rc == 0 || rc == EOF);
  ASSERT_TRUE(errno == 0 || errno == EINVAL);
  errno = 0;
  ASSERT_SYS(0, 'L', fputc('L', f));
  ASSERT_SYS(0, 0, fclose(f));
  ASSERT_NE(NULL, (f = fopen(path, "r+")));
  char b[16] = {0};
  ASSERT_SYS(0, 4, fread(b, 1, 16, f));
  ASSERT_STREQ("Love", b);
  ASSERT_SYS(0, 0, fclose(f));
  ASSERT_SYS(0, 0, unlink(path));
}

TEST(stdio, pushback3) {
  int fd;
  FILE *f;
  char path[] = "/tmp/fwrite_test.XXXXXX";
  ASSERT_NE(-1, (fd = mkstemp(path)));
  ASSERT_SYS(0, 0, close(fd));
  ASSERT_NE(NULL, (f = fopen(path, "w+")));
  ASSERT_SYS(0, 4, fwrite("love", 1, 4, f));
  ASSERT_SYS(0, 0, fclose(f));
  ASSERT_NE(NULL, (f = fopen(path, "r+")));
  ASSERT_SYS(0, 'L', fputc('L', f));
  ASSERT_SYS(0, 'O', fputc('O', f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_SYS(0, 'Z', ungetc('Z', f));
  ASSERT_SYS(0, 'z', ungetc('z', f));
  ASSERT_SYS(0, 'z', fgetc(f));
  ASSERT_SYS(0, 'Z', fgetc(f));
  ASSERT_SYS(0, 0, fclose(f));
  ASSERT_NE(NULL, (f = fopen(path, "r+")));
  char b[16] = {0};
  ASSERT_SYS(0, 4, fread(b, 1, 16, f));
  ASSERT_STREQ("LOve", b);
  ASSERT_SYS(0, 0, fclose(f));
  ASSERT_SYS(0, 0, unlink(path));
}

TEST(stdio, pushback4) {
  int fd;
  FILE *f;
  char path[] = "/tmp/fwrite_test.XXXXXX";
  ASSERT_NE(-1, (fd = mkstemp(path)));
  ASSERT_SYS(0, 0, close(fd));
  ASSERT_NE(NULL, (f = fopen(path, "w+")));
  ASSERT_SYS(0, 4, fwrite("love", 1, 4, f));
  ASSERT_SYS(0, 0, fclose(f));
  ASSERT_NE(NULL, (f = fopen(path, "r+")));
  ASSERT_SYS(0, 'L', fputc('L', f));
  ASSERT_SYS(0, 'O', fputc('O', f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_SYS(0, 'Z', ungetc('Z', f));
  ASSERT_SYS(0, 0, fflush(f));
#ifdef __GLIBC__
  // xxx: this causes memory corruption in glibc
  // ASSERT_SYS(0, 'L', fgetc(f));
#else
  ASSERT_SYS(0, 'O', fgetc(f));
#endif
  ASSERT_SYS(0, 0, fclose(f));
  ASSERT_NE(NULL, (f = fopen(path, "r+")));
  char b[16] = {0};
  ASSERT_SYS(0, 4, fread(b, 1, 16, f));
  ASSERT_STREQ("LOve", b);
  ASSERT_SYS(0, 0, fclose(f));
  ASSERT_SYS(0, 0, unlink(path));
}

TEST(fgetc, readFromWriteOnly) {
  int fd;
  FILE *f;
  char path[] = "/tmp/fwrite_test.XXXXXX";
  ASSERT_NE(-1, (fd = mkstemp(path)));
  ASSERT_SYS(0, 0, close(fd));
  ASSERT_NE(NULL, (f = fopen(path, "w")));
  ASSERT_EQ(EOF, fgetc(f));
  ASSERT_TRUE(ferror(f) != 0);
  ASSERT_FALSE(feof(f));
  ASSERT_TRUE(errno == EBADF || errno == 0);
  errno = 0;
  ASSERT_SYS(0, 0, fclose(f));
  ASSERT_SYS(0, 0, unlink(path));
}

TEST(fmemopen, readFromWriteOnly) {
  FILE *f;
  char buf[128];
  ASSERT_NE(NULL, (f = fmemopen(buf, 128, "w")));
  ASSERT_EQ(EOF, fgetc(f));
  ASSERT_TRUE(ferror(f) != 0);
  ASSERT_FALSE(feof(f));
  ASSERT_TRUE(errno == EBADF || errno == 0);
  errno = 0;
  ASSERT_SYS(0, 0, fclose(f));
}

TEST(fmemopen, writeToReadOnly) {
  FILE *f;
  char buf[128] = {0};
  ASSERT_NE(NULL, (f = fmemopen(buf, 128, "r")));
  ASSERT_EQ(EOF, fputc('1', f));
  ASSERT_TRUE(ferror(f) != 0);
  ASSERT_FALSE(feof(f));
  ASSERT_TRUE(errno == EBADF || errno == 0);
  errno = 0;
  ASSERT_SYS(0, 0, fclose(f));
}

TEST(fgetc, eof) {
  int fd;
  FILE *f;
  char path[] = "/tmp/fwrite_test.XXXXXX";
  ASSERT_NE(-1, (fd = mkstemp(path)));
  ASSERT_SYS(0, 0, close(fd));
  ASSERT_NE(NULL, (f = fopen(path, "r")));
  ASSERT_EQ(EOF, fgetc(f));
  ASSERT_FALSE(ferror(f));
  ASSERT_TRUE(feof(f));
  ASSERT_EQ(0, errno);
  ASSERT_SYS(0, 0, fclose(f));
  ASSERT_SYS(0, 0, unlink(path));
}

void PutcTmpfile(void) {
  FILE *f = tmpfile();
  for (size_t i = 0; i < sizeof(kChinese8) - 1; ++i)
    fputc(kChinese8[i], f);
  fclose(f);
}

void PutcUnlockedTmpfile(void) {
  FILE *f = tmpfile();
  for (size_t i = 0; i < sizeof(kChinese8) - 1; ++i)
    fputc_unlocked(kChinese8[i], f);
  fclose(f);
}

void PutwcTmpfile(void) {
  FILE *f = tmpfile();
  for (size_t i = 0; kChinese32[i]; ++i)
    fputwc(kChinese32[i], f);
  fclose(f);
}

void FwriteTmpfile(void) {
  FILE *f = tmpfile();
  for (size_t i = 0; i < sizeof(kChinese8) - 1; ++i)
    fwrite(&kChinese8[i], 1, 1, f);
  fclose(f);
}

void GetcTmpfile(const char *path, size_t n) {
  FILE *f = fopen(path, "r");
  for (size_t i = 0; i < n; ++i)
    ASSERT_NE(EOF, fgetc(f));
  fclose(f);
}

void UnlockedGetcTmpfile(const char *path, size_t n) {
#ifndef __FreeBSD__
  FILE *f = fopen(path, "r");
  for (size_t i = 0; i < n; ++i)
    ASSERT_NE(EOF, fgetc_unlocked(f));
  fclose(f);
#endif
}

void FwriteTmpfileNumbas(size_t n) {
  FILE *f = tmpfile();
  for (size_t i = 0; i < n; ++i)
    fprintf(f, "%d %d %d\n", 31337, 12345, 0xdeadbeef);
  fclose(f);
}

void FwriteTmpfileWidthStrings(size_t n) {
  FILE *f = tmpfile();
  for (size_t i = 0; i < n; ++i)
    fprintf(f, "%10d %10d %10d\n", 31337, 12345, 0xdeadbeef);
  fclose(f);
}

BENCH(fputc_unlocked, test) {
  BENCHMARK(10, 1, PutcTmpfile());
  BENCHMARK(10, 1, PutcUnlockedTmpfile());
  BENCHMARK(10, 1, PutwcTmpfile());
  BENCHMARK(10, 1, FwriteTmpfile());
  BENCHMARK(10, 1, FwriteTmpfileNumbas(1000));
  BENCHMARK(10, 1, FwriteTmpfileWidthStrings(1000));
  int fd;
  char path[] = "/tmp/fwrite_test.XXXXXX";
  ASSERT_NE(-1, (fd = mkstemp(path)));
  size_t n = sizeof(kChinese8) - 1;
  (void)!write(fd, kChinese8, n);
  ASSERT_SYS(0, 0, close(fd));
  BENCHMARK(10, 1, GetcTmpfile(path, sizeof(kChinese8) - 1));
  BENCHMARK(10, 1, UnlockedGetcTmpfile(path, sizeof(kChinese8) - 1));
  ASSERT_SYS(0, 0, unlink(path));
}
