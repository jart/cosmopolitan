// Copyright 2025 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#define _GNU_SOURCE
#include <assert.h>
#include <fnmatch.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test counter */
static int tests_run = 0;
static int tests_passed = 0;

/* Test macro */
#define TEST(name) static void test_##name(void)
#define RUN_TEST(name)               \
  do {                               \
    printf("Running %s... ", #name); \
    fflush(stdout);                  \
    test_##name();                   \
    tests_run++;                     \
    printf("PASSED\n");              \
    tests_passed++;                  \
  } while (0)

/* Assert macros */
#define ASSERT_MATCH(pattern, string, flags)                                   \
  do {                                                                         \
    if (fnmatch(pattern, string, flags) != 0) {                                \
      printf("\nFAILED: Expected '%s' to match '%s' with flags %d\n", pattern, \
             string, flags);                                                   \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)

#define ASSERT_NO_MATCH(pattern, string, flags)                           \
  do {                                                                    \
    if (fnmatch(pattern, string, flags) == 0) {                           \
      printf("\nFAILED: Expected '%s' to NOT match '%s' with flags %d\n", \
             pattern, string, flags);                                     \
      exit(1);                                                            \
    }                                                                     \
  } while (0)

/* Basic wildcard tests */
TEST(basic_exact_match) {
  ASSERT_MATCH("hello", "hello", 0);
  ASSERT_NO_MATCH("hello", "Hello", 0);
  ASSERT_NO_MATCH("hello", "hell", 0);
  ASSERT_NO_MATCH("hello", "helloo", 0);
}

TEST(asterisk_wildcard) {
  /* Single asterisk */
  ASSERT_MATCH("*", "", 0);
  ASSERT_MATCH("*", "anything", 0);
  ASSERT_MATCH("*", "test.txt", 0);

  /* Asterisk at beginning */
  ASSERT_MATCH("*.txt", "file.txt", 0);
  ASSERT_MATCH("*.txt", ".txt", 0);
  ASSERT_NO_MATCH("*.txt", "file.doc", 0);

  /* Asterisk at end */
  ASSERT_MATCH("test*", "test", 0);
  ASSERT_MATCH("test*", "testing", 0);
  ASSERT_MATCH("test*", "test123", 0);
  ASSERT_NO_MATCH("test*", "tes", 0);

  /* Asterisk in middle */
  ASSERT_MATCH("t*t", "tt", 0);
  ASSERT_MATCH("t*t", "test", 0);
  ASSERT_MATCH("t*t", "tenant", 0);
  ASSERT_NO_MATCH("t*t", "testing", 0);

  /* Multiple asterisks */
  ASSERT_MATCH("*.*", "file.txt", 0);
  ASSERT_MATCH("*.*", "my.file.txt", 0);
  ASSERT_NO_MATCH("*.*", "file", 0);
}

TEST(question_mark_wildcard) {
  /* Single question mark */
  ASSERT_MATCH("?", "a", 0);
  ASSERT_MATCH("?", "1", 0);
  ASSERT_NO_MATCH("?", "", 0);
  ASSERT_NO_MATCH("?", "ab", 0);

  /* Multiple question marks */
  ASSERT_MATCH("???", "abc", 0);
  ASSERT_NO_MATCH("???", "ab", 0);
  ASSERT_NO_MATCH("???", "abcd", 0);

  /* Mixed with text */
  ASSERT_MATCH("te?t", "test", 0);
  ASSERT_MATCH("te?t", "text", 0);
  ASSERT_NO_MATCH("te?t", "taet", 0);

  /* Combined with asterisk */
  ASSERT_MATCH("?.txt", "a.txt", 0);
  ASSERT_NO_MATCH("?.txt", "ab.txt", 0);
  ASSERT_MATCH("?*.txt", "a.txt", 0);
  ASSERT_MATCH("?*.txt", "ab.txt", 0);
}

TEST(character_classes) {
  /* Basic character class */
  ASSERT_MATCH("[abc]", "a", 0);
  ASSERT_MATCH("[abc]", "b", 0);
  ASSERT_MATCH("[abc]", "c", 0);
  ASSERT_NO_MATCH("[abc]", "d", 0);

  /* Character ranges */
  ASSERT_MATCH("[a-z]", "m", 0);
  ASSERT_NO_MATCH("[a-z]", "M", 0);
  ASSERT_MATCH("[A-Z]", "M", 0);
  ASSERT_MATCH("[0-9]", "5", 0);
  ASSERT_NO_MATCH("[0-9]", "a", 0);

  /* Multiple ranges */
  ASSERT_MATCH("[a-zA-Z]", "m", 0);
  ASSERT_MATCH("[a-zA-Z]", "M", 0);
  ASSERT_MATCH("[a-zA-Z0-9]", "5", 0);

  /* Negated character class */
  ASSERT_NO_MATCH("[!abc]", "a", 0);
  ASSERT_MATCH("[!abc]", "d", 0);
  ASSERT_NO_MATCH("[!a-z]", "m", 0);
  ASSERT_MATCH("[!a-z]", "M", 0);

  /* Character class in pattern */
  ASSERT_MATCH("test[0-9]", "test5", 0);
  ASSERT_NO_MATCH("test[0-9]", "testA", 0);
  ASSERT_MATCH("file[._-]txt", "file.txt", 0);
  ASSERT_MATCH("file[._-]txt", "file_txt", 0);
  ASSERT_MATCH("file[._-]txt", "file-txt", 0);
}

TEST(escape_sequences) {
  /* Escaping special characters */
  ASSERT_MATCH("\\*", "*", 0);
  ASSERT_MATCH("\\?", "?", 0);
  ASSERT_MATCH("\\[", "[", 0);
  ASSERT_MATCH("\\]", "]", 0);

  /* Escaped characters in patterns */
  ASSERT_MATCH("test\\*.txt", "test*.txt", 0);
  ASSERT_NO_MATCH("test\\*.txt", "testfile.txt", 0);
  ASSERT_MATCH("what\\?", "what?", 0);
  ASSERT_NO_MATCH("what\\?", "whats", 0);
}

TEST(flag_noescape) {
  /* With FNM_NOESCAPE, backslash doesn't escape */
  ASSERT_MATCH("test\\*", "test\\*", FNM_NOESCAPE);
  ASSERT_NO_MATCH("test\\*", "test*", FNM_NOESCAPE);
  ASSERT_MATCH("\\?", "\\?", FNM_NOESCAPE);
  ASSERT_NO_MATCH("\\?", "?", FNM_NOESCAPE);
}

TEST(flag_pathname) {
  /* With FNM_PATHNAME, * and ? don't match / */
  ASSERT_NO_MATCH("*", "dir/file", FNM_PATHNAME);
  ASSERT_NO_MATCH("dir/*", "dir/sub/file", FNM_PATHNAME);
  ASSERT_MATCH("dir/*/file", "dir/sub/file", FNM_PATHNAME);
  ASSERT_NO_MATCH("dir?file", "dir/file", FNM_PATHNAME);

  /* Without FNM_PATHNAME */
  ASSERT_MATCH("*", "dir/file", 0);
  ASSERT_MATCH("dir*file", "dir/file", 0);
}

TEST(flag_period) {
  /* With FNM_PERIOD, leading . must be matched explicitly */
  ASSERT_NO_MATCH("*", ".hidden", FNM_PERIOD);
  ASSERT_MATCH(".*", ".hidden", FNM_PERIOD);
  ASSERT_NO_MATCH("?hidden", ".hidden", FNM_PERIOD);

  /* After slash with FNM_PATHNAME */
  ASSERT_NO_MATCH("dir/*", "dir/.hidden", FNM_PATHNAME | FNM_PERIOD);
  ASSERT_MATCH("dir/.*", "dir/.hidden", FNM_PATHNAME | FNM_PERIOD);

  /* Without FNM_PERIOD */
  ASSERT_MATCH("*", ".hidden", 0);
  ASSERT_MATCH("?hidden", ".hidden", 0);
}

TEST(flag_casefold) {
  /* Case insensitive matching (if supported) */
#ifdef FNM_CASEFOLD
  ASSERT_MATCH("hello", "HELLO", FNM_CASEFOLD);
  ASSERT_MATCH("Ле́рмонтов", "ЛЕ́РМОНТОВ", FNM_CASEFOLD);
  ASSERT_MATCH("HELLO", "hello", FNM_CASEFOLD);
  ASSERT_MATCH("HeLLo", "hEllO", FNM_CASEFOLD);
  ASSERT_MATCH("*.TXT", "file.txt", FNM_CASEFOLD);
  ASSERT_MATCH("[a-z]", "M", FNM_CASEFOLD);
#else
  printf("(FNM_CASEFOLD not supported) ");
#endif
}

TEST(complex_patterns) {
  /* Complex pattern combinations */
  ASSERT_MATCH("*.[ch]", "file.c", 0);
  ASSERT_MATCH("*.[ch]", "file.h", 0);
  ASSERT_NO_MATCH("*.[ch]", "file.cpp", 0);

  ASSERT_NO_MATCH("test[0-9][0-9]?.txt", "test99.txt", 0);
  ASSERT_MATCH("test[0-9][0-9]?.txt", "test999.txt", 0);

  ASSERT_MATCH("[!.]*.txt", "file.txt", 0);
  ASSERT_NO_MATCH("[!.]*.txt", ".hidden.txt", 0);
}

TEST(edge_cases) {
  /* Empty patterns and strings */
  ASSERT_MATCH("", "", 0);
  ASSERT_NO_MATCH("", "a", 0);
  ASSERT_NO_MATCH("a", "", 0);

  /* Special characters in character classes */
  ASSERT_MATCH("[-a]", "-", 0);
  ASSERT_MATCH("[-a]", "a", 0);
  ASSERT_MATCH("[a-]", "-", 0);
  ASSERT_MATCH("[a-]", "a", 0);
  ASSERT_MATCH("[]]", "]", 0);
  ASSERT_MATCH("[!]]", "a", 0);
  ASSERT_NO_MATCH("[!]]", "]", 0);

  /* Multiple asterisks */
  ASSERT_MATCH("**", "", 0);
  ASSERT_MATCH("**", "anything", 0);
  ASSERT_MATCH("a**b", "ab", 0);
  ASSERT_MATCH("a**b", "axxxb", 0);
}

int main(void) {
  assert(!fnmatch("a", "a", 0));
  assert(!fnmatch("?a", "aa", 0));
  assert(!fnmatch("*a", "aa", 0));
  assert(!fnmatch("*", "h/h", 0));

  setlocale(LC_ALL, "C.UTF-8");

  printf("Running fnmatch() unit tests...\n");
  printf("================================\n");

  RUN_TEST(basic_exact_match);
  RUN_TEST(asterisk_wildcard);
  RUN_TEST(question_mark_wildcard);
  RUN_TEST(character_classes);
  RUN_TEST(escape_sequences);
  RUN_TEST(flag_noescape);
  RUN_TEST(flag_pathname);
  RUN_TEST(flag_period);
  RUN_TEST(flag_casefold);
  RUN_TEST(complex_patterns);
  RUN_TEST(edge_cases);

  printf("================================\n");
  printf("Tests passed: %d/%d\n", tests_passed, tests_run);

  if (tests_passed == tests_run) {
    printf("All tests passed!\n");
    return 0;
  } else {
    printf("Some tests failed!\n");
    return 1;
  }
}
