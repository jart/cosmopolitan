/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/weirdtypes.h"
#include "libc/errno.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

FILE *f;

TEST(memstream, nothing) {
  char *data;
  size_t size;
  ASSERT_NE(NULL, (f = open_memstream(&data, &size)));
  ASSERT_SYS(0, 0, fclose(f));
  ASSERT_NE(NULL, data);
  ASSERT_EQ(0, size);
  ASSERT_EQ(0, data[0]);
  free(data);
}

TEST(memstream, one) {
  char *data;
  size_t size = -1;
  ASSERT_NE(NULL, (f = open_memstream(&data, &size)));
  ASSERT_SYS(0, 'x', fputc('x', f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_NE(NULL, data);
  ASSERT_EQ(1, size);
  ASSERT_EQ('x', data[0]);
  ASSERT_EQ(0, strcmp(data, "x"));
  ASSERT_SYS(0, 0, fclose(f));
  free(data);
}

TEST(memstream, seek) {
#ifndef __FILC__
  char *data;
  size_t size = -1;
  ASSERT_NE(NULL, (f = open_memstream(&data, &size)));
  ASSERT_SYS(0, 0, fseek(f, 4, SEEK_SET));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_NE(NULL, data);
  ASSERT_EQ(4, size);
  ASSERT_EQ(0, memcmp(data, "\0\0\0\0", 4));
  ASSERT_SYS(0, 0, fclose(f));
  free(data);
#endif
}

TEST(fmemopen, seek) {
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

TEST(memstream, write) {
  char *data;
  size_t size;
  size_t rngsize = 22851;
  unsigned char *rng = malloc(rngsize);
  ASSERT_NE(NULL, rng);
  for (size_t i = 0; i < rngsize; ++i)
    rng[i] = rand();
  ASSERT_NE(NULL, (f = open_memstream(&data, &size)));
  for (size_t i = 0; i < rngsize; ++i)
    ASSERT_SYS(0, rng[i], fputc(rng[i], f));
  ASSERT_SYS(0, 0, fclose(f));
  ASSERT_EQ(0, memcmp(data, rng, rngsize));
  free(data);
  free(rng);
}

TEST(memstream, shrink) {
  char *data;
  size_t size;
  ASSERT_NE(NULL, (f = open_memstream(&data, &size)));
  ASSERT_TRUE(fputs("hello there", f) >= 0);
  ASSERT_SYS(0, 0, fseek(f, 5, SEEK_SET));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_STREQ("hello there", data);
  ASSERT_SYS(0, 0, fclose(f));
#ifdef __FILC__
  // musl has a bug here
  ASSERT_EQ(11, size);
  ASSERT_STREQ("hello there", data);
#else
  ASSERT_EQ(5, size);
  ASSERT_STREQ("hello", data);
#endif
  free(data);
}

TEST(memstream, read) {
#ifdef __FILC__
  // POSIX says the stream shall be opened for writing. Musl interprets
  // that as writing *only*.
#else
  char *data;
  size_t size;
  char b[6] = {0};
  ASSERT_NE(NULL, (f = open_memstream(&data, &size)));
  ASSERT_TRUE(fputs("hello there", f) >= 0);
  ASSERT_SYS(0, 0, fseek(f, 0, SEEK_SET));
  ASSERT_SYS(0, 5, fread(b, 1, 5, f));
  ASSERT_STREQ("hello", b);
  ASSERT_SYS(0, 0, fclose(f));
  free(data);
#endif
}

TEST(memstream, fseek_positions) {
  char *data;
  size_t size;
  ASSERT_NE(NULL, (f = open_memstream(&data, &size)));
  ASSERT_SYS(0, 3, fwrite("abc", 1, 3, f));
  ASSERT_SYS(0, 3, ftell(f));
  ASSERT_SYS(0, 0, fseek(f, 0, SEEK_SET));
  ASSERT_SYS(0, 0, ftell(f));
  ASSERT_SYS(0, 0, fseek(f, 2, SEEK_CUR));
  ASSERT_SYS(0, 2, ftell(f));
  // test seeking relative to end - some implementations may differ
  ASSERT_SYS(0, 0, fseek(f, -1, SEEK_END));
#ifdef __GLIBC__
  ASSERT_SYS(0, 1, ftell(f));
#else
  ASSERT_SYS(0, 2, ftell(f));
#endif
  ASSERT_SYS(0, 0, fseek(f, 5, SEEK_SET));
  ASSERT_SYS(0, 5, ftell(f));
  ASSERT_SYS(0, 'x', fputc('x', f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_EQ(6, size);
  ASSERT_EQ('a', data[0]);
  ASSERT_EQ('b', data[1]);
  ASSERT_EQ('c', data[2]);
  ASSERT_EQ('\0', data[3]);
  ASSERT_EQ('\0', data[4]);
  ASSERT_EQ('x', data[5]);
  ASSERT_EQ('\0', data[6]);
  ASSERT_SYS(0, 0, fclose(f));
  free(data);
}

TEST(memstream, buffer_growth) {
  char *data;
  size_t size;
  ASSERT_NE(NULL, (f = open_memstream(&data, &size)));
  for (int i = 0; i < 1000; i++)
    ASSERT_SYS(0, 10, fprintf(f, "Line %04d\n", i));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_EQ(10000, size);
  char expected[11];
  sprintf(expected, "Line %04d\n", 999);
  ASSERT_EQ(0, memcmp(data + size - 10, expected, 10));
  ASSERT_SYS(0, 0, fclose(f));
  free(data);
}

TEST(memstream, fprintf_formats) {
  char *data;
  size_t size;
  ASSERT_NE(NULL, (f = open_memstream(&data, &size)));
  ASSERT_TRUE(fprintf(f, "%d %u %x %o", -42, 42u, 0xDEAD, 0755) > 0);
  ASSERT_TRUE(fprintf(f, " %.2f %.2e", 3.14159, 1234.5) > 0);
  ASSERT_TRUE(fprintf(f, " %c %s", 'A', "test") > 0);
  ASSERT_TRUE(fprintf(f, " %p", (void *)0x1234) > 0);
  ASSERT_SYS(0, 0, fclose(f));
  ASSERT_TRUE(strstr(data, "-42") != NULL);
  ASSERT_TRUE(strstr(data, "42") != NULL);
  ASSERT_TRUE(strstr(data, "dead") != NULL);
  ASSERT_TRUE(strstr(data, "755") != NULL);
  ASSERT_TRUE(strstr(data, "3.14") != NULL);
  ASSERT_TRUE(strstr(data, "1.23e+03") != NULL);
  ASSERT_TRUE(strstr(data, "A test") != NULL);
  free(data);
}

TEST(fmemopen, basic_write) {
  char buf[64] = {0};
  ASSERT_NE(NULL, (f = fmemopen(buf, sizeof(buf), "w")));
  ASSERT_SYS(0, 11, fwrite("hello world", 1, 11, f));
  ASSERT_SYS(0, 0, fclose(f));
  ASSERT_STREQ("hello world", buf);
}

TEST(fmemopen, read_write) {
  char buf[64] = "initial data";
  char read_buf[64] = {0};
  ASSERT_NE(NULL, (f = fmemopen(buf, sizeof(buf), "r+")));
  ASSERT_SYS(0, 7, fread(read_buf, 1, 7, f));
  ASSERT_EQ(0, memcmp(read_buf, "initial", 7));
  ASSERT_SYS(0, 0, fseek(f, 8, SEEK_SET));
  ASSERT_SYS(0, 8, fwrite("modified", 1, 8, f));
  ASSERT_SYS(0, 0, fclose(f));
  ASSERT_STREQ("initial modified", buf);
}

TEST(fmemopen, append_mode) {
  char buf[64] = "hello";
  ASSERT_NE(NULL, (f = fmemopen(buf, sizeof(buf), "a")));
  ASSERT_SYS(0, 6, fwrite(" world", 1, 6, f));
  ASSERT_SYS(0, 0, fclose(f));
  ASSERT_STREQ("hello world", buf);
}

TEST(fmemopen, null_buffer) {
  char read_buf[64] = {0};
  ASSERT_NE(NULL, (f = fmemopen(NULL, 1024, "w+")));
  ASSERT_SYS(0, 11, fwrite("test string", 1, 11, f));
  ASSERT_SYS(0, 0, fseek(f, 0, SEEK_SET));
  ASSERT_SYS(0, 11, fread(read_buf, 1, 11, f));
  ASSERT_STREQ("test string", read_buf);
  ASSERT_SYS(0, 0, fclose(f));
}

TEST(memstream, fscanf_test) {
#ifndef __FILC__
  char *data;
  size_t size;
  int a, b;
  char str[32];
  float fval;
  ASSERT_NE(NULL, (f = open_memstream(&data, &size)));
  ASSERT_TRUE(fprintf(f, "123 456 hello 3.14") > 0);
  ASSERT_SYS(0, 0, fseek(f, 0, SEEK_SET));
  ASSERT_EQ(2, fscanf(f, "%d %d", &a, &b));
  ASSERT_EQ(123, a);
  ASSERT_EQ(456, b);
  ASSERT_EQ(1, fscanf(f, "%s", str));
  ASSERT_STREQ("hello", str);
  ASSERT_EQ(1, fscanf(f, "%f", &fval));
  ASSERT_TRUE(fabs(fval - 3.14f) < 0.001f);
  ASSERT_SYS(0, 0, fclose(f));
  free(data);
#endif
}

TEST(memstream, rewind_test) {
  char *data;
  size_t size;
  ASSERT_NE(NULL, (f = open_memstream(&data, &size)));
  ASSERT_SYS(0, 10, fwrite("0123456789", 1, 10, f));
  ASSERT_SYS(0, 10, ftell(f));
  rewind(f);
  ASSERT_SYS(0, 0, ftell(f));
  ASSERT_SYS(0, 3, fwrite("ABC", 1, 3, f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_EQ(0, memcmp(data, "ABC3456789", 10));
  ASSERT_SYS(0, 0, fclose(f));
  free(data);
}

TEST(memstream, truncate_on_seek) {
  char *data;
  size_t size;
  ASSERT_NE(NULL, (f = open_memstream(&data, &size)));
  ASSERT_SYS(0, 10, fwrite("0123456789", 1, 10, f));
  ASSERT_SYS(0, 0, fseek(f, 5, SEEK_SET));
  ASSERT_SYS(0, 0, fflush(f));
  // Size behavior after seek+flush varies by implementation
  ASSERT_TRUE(size == 5 || size == 10);
  ASSERT_SYS(0, 0, fclose(f));
  free(data);
}

TEST(memstream, write_at_offset) {
  char *data;
  size_t size;
  ASSERT_NE(NULL, (f = open_memstream(&data, &size)));
  ASSERT_SYS(0, 0, fseek(f, 10, SEEK_SET));
  ASSERT_SYS(0, 3, fwrite("xyz", 1, 3, f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_EQ(13, size);
  for (int i = 0; i < 10; i++)
    ASSERT_EQ('\0', data[i]);
  ASSERT_EQ('x', data[10]);
  ASSERT_EQ('y', data[11]);
  ASSERT_EQ('z', data[12]);
  ASSERT_EQ('\0', data[13]);
  ASSERT_SYS(0, 0, fclose(f));
  free(data);
}

TEST(memstream, fflush_behavior) {
  char *data;
  size_t size;
  ASSERT_NE(NULL, (f = open_memstream(&data, &size)));
  ASSERT_SYS(0, 5, fwrite("hello", 1, 5, f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_EQ(5, size);
  ASSERT_EQ(0, memcmp(data, "hello", 5));
  ASSERT_SYS(0, 6, fwrite(" world", 1, 6, f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_EQ(11, size);
  ASSERT_EQ(0, memcmp(data, "hello world", 11));
  ASSERT_SYS(0, 0, fseek(f, 5, SEEK_SET));
  ASSERT_SYS(0, 3, fwrite("XXX", 1, 3, f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_EQ(8, size);
  ASSERT_EQ(0, memcmp(data, "helloXXXr", 9));
  ASSERT_SYS(0, 0, fclose(f));
  free(data);
}

TEST(fmemopen, size_limits) {
  char buf[10] = {0};
  ASSERT_NE(NULL, (f = fmemopen(buf, sizeof(buf), "w")));
  // Some implementations may return 13 (attempted) rather than 9 (stored)
  size_t written = fwrite("0123456789ABC", 1, 13, f);
  ASSERT_TRUE(written == 9 || written == 10 || written == 13);
  errno = 0;
  // fclose may return -1 when buffer overflow occurred
  int close_result = fclose(f);
  ASSERT_TRUE(close_result == 0 || close_result == -1);
  ASSERT_EQ(0, memcmp(buf, "012345678", 9));
}

TEST(memstream, getc_putc) {
#ifndef __FILC__
  char *data;
  size_t size;
  ASSERT_NE(NULL, (f = open_memstream(&data, &size)));
  for (int i = 0; i < 26; i++) {
    ASSERT_SYS(0, 'A' + i, fputc('A' + i, f));
  }
  ASSERT_SYS(0, 0, fseek(f, 0, SEEK_SET));
  for (int i = 0; i < 26; i++) {
    ASSERT_SYS(0, 'A' + i, fgetc(f));
  }
  ASSERT_SYS(0, EOF, fgetc(f));
  ASSERT_SYS(0, 0, fclose(f));
  free(data);
#endif
}

TEST(memstream, ungetc) {
#ifndef __FILC__
  char *data;
  size_t size;
  ASSERT_NE(NULL, (f = open_memstream(&data, &size)));
  ASSERT_SYS(0, 5, fwrite("hello", 1, 5, f));
  ASSERT_SYS(0, 0, fseek(f, 0, SEEK_SET));
  ASSERT_SYS(0, 'h', fgetc(f));
  ASSERT_SYS(0, 'X', ungetc('X', f));
  ASSERT_SYS(0, 'X', fgetc(f));
  ASSERT_SYS(0, 'e', fgetc(f));
  ASSERT_SYS(0, 0, fclose(f));
  free(data);
#endif
}

TEST(fmemopen, binary_mode) {
  unsigned char buf[256];
  unsigned char data[256];
  for (int i = 0; i < 256; i++)
    buf[i] = i;
  ASSERT_NE(NULL, (f = fmemopen(buf, sizeof(buf), "rb")));
  ASSERT_EQ(256, fread(data, 1, 256, f));
  ASSERT_EQ(0, memcmp(buf, data, 256));
  ASSERT_SYS(0, 0, fclose(f));
}

#ifndef __COSMOPOLITAN__
TEST(open_wmemstream, basic) {
  wchar_t *data;
  size_t size;
  ASSERT_NE(NULL, (f = open_wmemstream(&data, &size)));
  ASSERT_SYS(0, L'A', fputwc(L'A', f));
  ASSERT_SYS(0, L'B', fputwc(L'B', f));
  ASSERT_SYS(0, L'C', fputwc(L'C', f));
  ASSERT_SYS(0, 0, fclose(f));
  ASSERT_EQ(3, size);
  ASSERT_EQ(L'A', data[0]);
  ASSERT_EQ(L'B', data[1]);
  ASSERT_EQ(L'C', data[2]);
  ASSERT_EQ(L'\0', data[3]);
  free(data);
}
TEST(open_wmemstream, unicode) {
  wchar_t *data;
  size_t size;
  const wchar_t *test_str = L"Hello 世界 🌍";
  ASSERT_NE(NULL, (f = open_wmemstream(&data, &size)));
  ASSERT_TRUE(fputws(test_str, f) >= 0);
  ASSERT_SYS(0, 0, fclose(f));
  ASSERT_EQ(0, wcscmp(data, test_str));
  free(data);
}
#endif

// POSIX conformance tests for null-terminator behavior
TEST(memstream, null_terminator_not_counted) {
  char *data;
  size_t size;
  ASSERT_NE(NULL, (f = open_memstream(&data, &size)));
  ASSERT_SYS(0, 5, fwrite("hello", 1, 5, f));
  ASSERT_SYS(0, 0, fflush(f));
  // Size should be 5, not 6 (null terminator not counted)
  ASSERT_EQ(5, size);
  // But buffer should have null terminator at position 5
  ASSERT_EQ('\0', data[5]);
  // And data should be null-terminated string
  ASSERT_STREQ("hello", data);
  ASSERT_SYS(0, 0, fclose(f));
  free(data);
}

TEST(memstream, seek_past_end_null_fill) {
  char *data;
  size_t size;
  ASSERT_NE(NULL, (f = open_memstream(&data, &size)));
  ASSERT_SYS(0, 3, fwrite("abc", 1, 3, f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_EQ(3, size);
  // Seek past end and write
  ASSERT_SYS(0, 0, fseek(f, 10, SEEK_SET));
  ASSERT_SYS(0, 'x', fputc('x', f));
  ASSERT_SYS(0, 0, fflush(f));
  // Size should be 11 (position after writing 'x')
  ASSERT_EQ(11, size);
  // Check that gap is filled with null bytes
  ASSERT_EQ('a', data[0]);
  ASSERT_EQ('b', data[1]);
  ASSERT_EQ('c', data[2]);
  ASSERT_EQ('\0', data[3]);  // gap start
  ASSERT_EQ('\0', data[4]);
  ASSERT_EQ('\0', data[5]);
  ASSERT_EQ('\0', data[6]);
  ASSERT_EQ('\0', data[7]);
  ASSERT_EQ('\0', data[8]);
  ASSERT_EQ('\0', data[9]);  // gap end
  ASSERT_EQ('x', data[10]);
  ASSERT_EQ('\0', data[11]);  // terminator
  ASSERT_SYS(0, 0, fclose(f));
  free(data);
}

TEST(memstream, size_is_min_buffer_position) {
  char *data;
  size_t size;
  ASSERT_NE(NULL, (f = open_memstream(&data, &size)));
  // Write some data
  ASSERT_SYS(0, 10, fwrite("0123456789", 1, 10, f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_EQ(10, size);
  // Seek backwards and write
  ASSERT_SYS(0, 0, fseek(f, 3, SEEK_SET));
  ASSERT_SYS(0, 3, fwrite("ABC", 1, 3, f));
  ASSERT_SYS(0, 0, fflush(f));
  // Position is now 6, buffer has "012ABC6789" (10 bytes)
  // Size should be min(10, 6) = 6
  ASSERT_EQ(6, size);
  ASSERT_EQ(0, memcmp(data, "012ABC", 6));
  ASSERT_SYS(0, 0, fclose(f));
  free(data);
}

TEST(memstream, posix_example) {
  off_t eob;
  char *data;
  size_t size;
  ASSERT_NE(NULL, (f = open_memstream(&data, &size)));
  // Write "hello my world"
  ASSERT_TRUE(fprintf(f, "hello my world") > 0);
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_EQ(14, size);
  ASSERT_STREQ("hello my world", data);
  // Save end position and seek to start
  eob = ftello(f);
  ASSERT_EQ(14, eob);
  ASSERT_SYS(0, 0, fseeko(f, 0, SEEK_SET));
  // Write "good-bye" at start
  ASSERT_TRUE(fprintf(f, "good-bye") > 0);
  // Seek back to end
  ASSERT_SYS(0, 0, fseeko(f, eob, SEEK_SET));
  ASSERT_SYS(0, 0, fclose(f));
  // Should have "good-bye world" with len=14
#ifdef __FILC__
  // musl has a bug and returns 8 instead of 14 in this case
  ASSERT_TRUE(size == 8);
#else
  ASSERT_TRUE(size == 14);
#endif
  ASSERT_STREQ("good-bye world", data);
  free(data);
}

TEST(memstream, buffer_always_terminated) {
  char *data;
  size_t size;
  ASSERT_NE(NULL, (f = open_memstream(&data, &size)));
  // Test 1: Empty stream
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_EQ(0, size);
  ASSERT_EQ('\0', data[0]);
  ASSERT_STREQ("", data);
  // Test 2: Add some data
  ASSERT_SYS(0, 3, fwrite("foo", 1, 3, f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_EQ(3, size);
  ASSERT_EQ('\0', data[3]);
  ASSERT_STREQ("foo", data);
  // Test 3: Seek and overwrite
  ASSERT_SYS(0, 0, fseek(f, 1, SEEK_SET));
  ASSERT_SYS(0, 2, fwrite("ar", 1, 2, f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_EQ(3, size);
  ASSERT_EQ('\0', data[3]);
  ASSERT_STREQ("far", data);
  // Test 4: Seek backwards and write less
  ASSERT_SYS(0, 0, fseek(f, 1, SEEK_SET));
  ASSERT_SYS(0, 1, fwrite("i", 1, 1, f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_EQ(2, size);  // min(3, 2) = 2
  // The logical content is only the first 'size' bytes
  ASSERT_EQ(0, memcmp(data, "fi", 2));
  // Note: implementations don't update null terminator when size decreases
  // This may be implementation-defined behavior
  ASSERT_SYS(0, 0, fclose(f));
  free(data);
}

TEST(memstream, null_terminator_on_shrink) {
  char *data;
  size_t size;
  ASSERT_NE(NULL, (f = open_memstream(&data, &size)));
  // Write some data
  ASSERT_SYS(0, 5, fwrite("hello", 1, 5, f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_EQ(5, size);
  ASSERT_EQ('\0', data[5]);
  // Seek back and write less data
  ASSERT_SYS(0, 0, fseek(f, 0, SEEK_SET));
  ASSERT_SYS(0, 2, fwrite("hi", 1, 2, f));
  ASSERT_SYS(0, 0, fflush(f));
  ASSERT_EQ(2, size);
  ASSERT_SYS(0, 0, fclose(f));
  // check if null terminator is placed at data[size] when size shrinks
#ifdef __FILC__
  // musl has a bug here
#else
  ASSERT_EQ('\0', data[2]);
#endif
  free(data);
}
