// Copyright 2024 Justine Alexandra Roberts Tunney
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

#include "libc/intrin/tree.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/maps.h"
#include "libc/intrin/tree.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"

#define NUMBER_CONTAINER(e) TREE_CONTAINER(struct number, elem, e)

void tree_checker(const struct Tree *node, const struct Tree *parent,
                  int black_count, int *black_height, tree_cmp_f *cmp) {
  if (!node) {
    // Leaf nodes are considered black
    if (*black_height == -1) {
      *black_height = black_count;
    } else if (black_count != *black_height) {
      // ILLEGAL TREE: Black height mismatch
      __builtin_trap();
    }
    return;
  }
  if (node->parent != parent)
    // ILLEGAL TREE: Parent link is incorrect
    __builtin_trap();
  if (parent) {
    if (tree_get_left(parent) == node && cmp(parent, node) < 0)
      // ILLEGAL TREE: Binary search property violated on left child
      __builtin_trap();
    if (parent->right == node && cmp(node, parent) < 0)
      // ILLEGAL TREE: Binary search property violated on right child
      __builtin_trap();
  }
  if (!tree_get_red(node)) {
    black_count++;
  } else if (parent && tree_get_red(parent)) {
    // ILLEGAL TREE: Red node has red child
    __builtin_trap();
  }
  tree_checker(tree_get_left(node), node, black_count, black_height, cmp);
  tree_checker(node->right, node, black_count, black_height, cmp);
}

void tree_check(struct Tree *root, tree_cmp_f *cmp) {
  if (root) {
    if (tree_get_red(root))
      // ILLEGAL TREE: root node must be black
      __builtin_trap();
    int black_height = -1;
    tree_checker(root, 0, 0, &black_height, cmp);
  }
}

struct number {
  long number;
  struct Tree elem;
};

int number_compare(const struct Tree *ra, const struct Tree *rb) {
  const struct number *a = NUMBER_CONTAINER(ra);
  const struct number *b = NUMBER_CONTAINER(rb);
  return (a->number > b->number) - (a->number < b->number);
}

struct number *number_new(int number) {
  struct number *res;
  if ((res = malloc(sizeof(struct number))))
    res->number = number;
  return res;
}

void print(struct Tree *tree) {
  for (struct Tree *e = tree_first(tree); e; e = tree_next(e))
    kprintf("%3d", NUMBER_CONTAINER(e)->number);
  kprintf("\n");
}

void print_reversed(struct Tree *tree) {
  for (struct Tree *e = tree_last(tree); e; e = tree_prev(e))
    kprintf("%3d", NUMBER_CONTAINER(e)->number);
  kprintf("\n");
}

int number_search(const void *ra, const struct Tree *rb) {
  long a = (long)ra;
  const struct number *b = NUMBER_CONTAINER(rb);
  return (a > b->number) - (a < b->number);
}

void simple_test(void) {
  //        0  2  2 23 30 32 34 34 46 52 53 65 70 74 90 94 95 95 96 96
  //       96 96 95 95 94 90 74 70 65 53 52 46 34 34 32 30 23  2  2  0
  static const long kNumba[] = {74, 53, 96, 70, 34, 95, 30, 2,  96, 46,
                                23, 2,  52, 0,  34, 94, 90, 95, 32, 65};

  // test insertion works
  struct Tree *tree = 0;
  for (int i = 0; i < 20; ++i) {
    int number = kNumba[i];
    kprintf("%3d", number);
    tree_insert(&tree, &number_new(number)->elem, number_compare);
    tree_check(tree, number_compare);
  }
  kprintf("\n");

  // test iteration works
  print(tree);

  // test reverse iteration works
  print_reversed(tree);

  // test removal works
  for (int i = 0; i < 20; ++i) {
    tree_remove(&tree, tree_get(tree, (void *)kNumba[i], number_search));
    tree_check(tree, number_compare);
    print(tree);
  }

  // use up a bunch of memory
  for (int i = 0; i < 100000; ++i)
    tree_insert(&tree, &number_new(rand())->elem, number_compare);
  tree_check(tree, number_compare);
  tree_check(__maps.maps, __maps_compare);

  // visually verify maps get coalesced
  __print_maps(0);
}

void search_test(void) {
  struct Tree *x, *tree = 0;
  tree_insert(&tree, &number_new(1)->elem, number_compare);
  tree_insert(&tree, &number_new(3)->elem, number_compare);
  tree_insert(&tree, &number_new(5)->elem, number_compare);
  tree_insert(&tree, &number_new(7)->elem, number_compare);

  // Test tree_get()
  //
  // Returns node equal to given key.
  //
  //    [1 3 5 7]   [1 3 5 7]   [1 3 5 7]
  //       NULL        ↑               NULL
  //        4          3                8
  //
  x = tree_get(tree, (void *)4l, number_search);
  if (x)
    exit(1);
  x = tree_get(tree, (void *)3l, number_search);
  if (!x)
    exit(2);
  if (NUMBER_CONTAINER(x)->number != 3)
    exit(3);
  if (!tree_get(tree, (void *)7l, number_search))
    exit(27);
  if (tree_get(tree, (void *)8l, number_search))
    exit(28);

  // Test tree_floor()
  //
  // Returns last node less than or equal to given key.
  //
  //    [1 3 5 7]   [1 3 5 7]   [1 3 5 7]
  //       ↑           ↑               ↑
  //       4           3               8
  //
  x = tree_floor(tree, (void *)0l, number_search);
  if (x)
    exit(4);
  x = tree_floor(tree, (void *)4l, number_search);
  if (!x)
    exit(4);
  if (NUMBER_CONTAINER(x)->number != 3)
    exit(5);
  x = tree_floor(tree, (void *)3l, number_search);
  if (!x)
    exit(6);
  if (NUMBER_CONTAINER(x)->number != 3)
    exit(7);
  if (!tree_floor(tree, (void *)7l, number_search))
    exit(24);
  x = tree_floor(tree, (void *)8l, number_search);
  if (!x)
    exit(25);
  if (NUMBER_CONTAINER(x)->number != 7)
    exit(30);
  if (tree_floor(tree, (void *)0l, number_search))
    exit(31);

  // Test tree_lower()
  //
  // Returns first node not less than given key.
  //
  //    [1 3 5 7]   [1 3 5 7]   [1 3 5 7]
  //         ↑         ↑               NULL
  //         4         3                8
  //
  x = tree_lower(tree, (void *)4l, number_search);
  if (!x)
    exit(4);
  if (NUMBER_CONTAINER(x)->number != 5)
    exit(8);
  x = tree_lower(tree, (void *)3l, number_search);
  if (!x)
    exit(9);
  if (NUMBER_CONTAINER(x)->number != 3)
    exit(10);
  if (!tree_lower(tree, (void *)7l, number_search))
    exit(22);
  if (tree_lower(tree, (void *)8l, number_search))
    exit(23);

  // Test tree_ceil()
  //
  // Returns first node greater than than given key.
  //
  //    [1 3 5 7]   [1 3 5 7]   [1 3 5 7]
  //         ↑           ↑             NULL
  //         4           3              8
  //
  x = tree_ceil(tree, (void *)4l, number_search);
  if (!x)
    exit(11);
  if (NUMBER_CONTAINER(x)->number != 5)
    exit(12);
  x = tree_ceil(tree, (void *)3l, number_search);
  if (!x)
    exit(13);
  if (NUMBER_CONTAINER(x)->number != 5)
    exit(14);
  if (tree_ceil(tree, (void *)7l, number_search))
    exit(21);

  // Test tree_first()
  if (tree_first(NULL))
    exit(15);
  x = tree_first(tree);
  if (!x)
    exit(16);
  if (NUMBER_CONTAINER(x)->number != 1)
    exit(17);

  // Test tree_last()
  if (tree_last(NULL))
    exit(18);
  x = tree_last(tree);
  if (!x)
    exit(19);
  if (NUMBER_CONTAINER(x)->number != 7)
    exit(20);
}

int main() {
  ShowCrashReports();

  // show memory maps at startup
  tree_check(__maps.maps, __maps_compare);
  kprintf("\n");
  __print_maps(0);
  kprintf("\n");

  // run tests
  simple_test();
  search_test();
}
