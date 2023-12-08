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
#include "libc/mem/critbit0.h"
#include "libc/fmt/itoa.h"
#include "libc/mem/critbit0.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

struct Bog {
  unsigned i;
  unsigned n;
  const char *p[];
};

static __wur struct Bog *NewBog(unsigned n) {
  struct Bog *res = malloc(sizeof(struct Bog) + sizeof(const char *) * n);
  res->i = 0;
  res->n = n;
  return res;
}

static void ClearBog(struct Bog *bog) {
  bog->i = 0;
}

static void FreeBog(struct Bog **bog) {
  free(*bog), *bog = NULL;
}

static const char *const elems[] = {"a",   "aa",  "aaz", "abz",
                                    "bba", "bbc", "bbd", NULL};

static void MakeTree(struct critbit0 *tree) {
  memset(tree, 0, sizeof(*tree));
  for (unsigned i = 0; elems[i]; ++i) {
    ASSERT_EQ(true, critbit0_insert(tree, elems[i]));
  }
}

TEST(critbit0, testContains) {
  struct critbit0 tree[1];
  MakeTree(tree);
  for (unsigned i = 0; elems[i]; ++i) {
    if (!critbit0_contains(tree, elems[i])) abort();
  }
  critbit0_clear(tree);
}

static const char *const elems2[] = {"a",  "aa",  "b",   "bb", "ab",
                                     "ba", "aba", "bab", NULL};

TEST(critbit0, testDelete) {
  struct critbit0 tree = {0};
  for (unsigned i = 1; elems2[i]; ++i) {
    critbit0_clear(&tree);
    for (unsigned j = 0; j < i; ++j) critbit0_insert(&tree, elems2[j]);
    for (unsigned j = 0; j < i; ++j) {
      if (!critbit0_contains(&tree, elems2[j])) abort();
    }
    for (unsigned j = 0; j < i; ++j) {
      if (1 != critbit0_delete(&tree, elems2[j])) abort();
    }
    for (unsigned j = 0; j < i; ++j) {
      if (critbit0_contains(&tree, elems2[j])) abort();
    }
  }
  critbit0_clear(&tree);
}

static intptr_t allprefixed_cb(const char *elem, void *arg) {
  struct Bog *bog = arg;
  ASSERT_LT(bog->i, bog->n);
  bog->p[bog->i++] = elem;
  return 0;
}

TEST(critbit0, testAllPrefixed) {
  struct critbit0 tree[1];
  MakeTree(tree);
  struct Bog *a = NewBog(4);
  ASSERT_EQ(0, critbit0_allprefixed(tree, "a", allprefixed_cb, a));
  ASSERT_EQ(4, a->i);
  ASSERT_STREQ("a", a->p[0]);
  ASSERT_STREQ("aa", a->p[1]);
  ASSERT_STREQ("aaz", a->p[2]);
  ASSERT_STREQ("abz", a->p[3]);
  ClearBog(a);
  ASSERT_EQ(0, critbit0_allprefixed(tree, "aa", allprefixed_cb, a));
  ASSERT_EQ(2, a->i);
  ASSERT_STREQ("aa", a->p[0]);
  ASSERT_STREQ("aaz", a->p[1]);
  critbit0_clear(tree);
  FreeBog(&a);
}

static intptr_t allprefixed_cb_halt(const char *elem, void *arg) {
  struct Bog *bog = arg;
  ASSERT_LT(bog->i, bog->n);
  bog->p[bog->i++] = elem;
  return strcmp(elem, "aa") == 0 ? 123 : 0;
}

TEST(critbit0, testAllPrefixed_haltOnNonzero) {
  struct critbit0 tree[1];
  MakeTree(tree);
  struct Bog *a = NewBog(4);
  ASSERT_EQ(123, critbit0_allprefixed(tree, "a", allprefixed_cb_halt, a));
  ASSERT_EQ(2, a->i);
  ASSERT_STREQ("a", a->p[0]);
  ASSERT_STREQ("aa", a->p[1]);
  critbit0_clear(tree);
  FreeBog(&a);
}

TEST(critbit0, duplicate) {
  struct critbit0 tree = {0};
  ASSERT_TRUE(critbit0_insert(&tree, "hi"));
  ASSERT_FALSE(critbit0_insert(&tree, "hi"));
  critbit0_clear(&tree);
}

TEST(critbit0, manual_clear) {
  struct critbit0 tree = {0};
  ASSERT_TRUE(critbit0_insert(&tree, "hi"));
  ASSERT_TRUE(critbit0_delete(&tree, "hi"));
  ASSERT_EQ(NULL, tree.root);
}

#define N 500

char words[N][16];

void GenerateWords(void) {
  for (int i = 0; i < N; ++i) {
    FormatInt32(words[i], rand());
  }
}

void BuildTree(void) {
  struct critbit0 tree = {0};
  for (int i = 0; i < N; ++i) {
    critbit0_insert(&tree, words[i]);
  }
  critbit0_clear(&tree);
}

BENCH(critbit0, bench) {
  GenerateWords();
  EZBENCH2("critbit0", donothing, BuildTree());
}
