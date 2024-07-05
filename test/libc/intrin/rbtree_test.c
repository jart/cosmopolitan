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

#include "libc/intrin/rbtree.h"
#include "libc/intrin/kprintf.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"

#define NUMBER_CONTAINER(e) RBTREE_CONTAINER(struct number, elem, e)

void rbtree_checker(const struct rbtree *node, const struct rbtree *parent,
                    int black_count, int *black_height, rbtree_cmp_f *cmp) {
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
    if (rbtree_get_left(parent) == node && cmp(parent, node) < 0)
      // ILLEGAL TREE: Binary search property violated on left child
      __builtin_trap();
    if (parent->right == node && cmp(node, parent) < 0)
      // ILLEGAL TREE: Binary search property violated on right child
      __builtin_trap();
  }
  if (!rbtree_get_red(node)) {
    black_count++;
  } else if (parent && rbtree_get_red(parent)) {
    // ILLEGAL TREE: Red node has red child
    __builtin_trap();
  }
  rbtree_checker(rbtree_get_left(node), node, black_count, black_height, cmp);
  rbtree_checker(node->right, node, black_count, black_height, cmp);
}

void rbtree_check(struct rbtree *root, rbtree_cmp_f *cmp) {
  if (root) {
    if (rbtree_get_red(root))
      // ILLEGAL TREE: root node must be black
      __builtin_trap();
    int black_height = -1;
    rbtree_checker(root, 0, 0, &black_height, cmp);
  }
}

struct number {
  int number;
  struct rbtree elem;
};

int number_compare(const struct rbtree *ra, const struct rbtree *rb) {
  const struct number *a = NUMBER_CONTAINER(ra);
  const struct number *b = NUMBER_CONTAINER(rb);
  return (a->number > b->number) - (a->number < b->number);
}

struct number *number_new(int number) {
  static int used;
  static struct number heap[8192];
  if (used == ARRAYLEN(heap))
    return 0;
  struct number *res = &heap[used++];
  res->number = number;
  return res;
}

struct rbtree *tree = 0;

void print(void) {
  for (struct rbtree *e = rbtree_first(tree); e; e = rbtree_next(e))
    kprintf("%3d", NUMBER_CONTAINER(e)->number);
  kprintf("\n");
}

void print_reversed(void) {
  for (struct rbtree *e = rbtree_last(tree); e; e = rbtree_prev(e))
    kprintf("%3d", NUMBER_CONTAINER(e)->number);
  kprintf("\n");
}

void simple_test(void) {
  static const int kNumba[] = {74, 53, 96, 70, 34, 95, 30, 2,  89, 46,
                               23, 2,  52, 0,  34, 12, 90, 95, 32, 65};
  for (int i = 0; i < 20; ++i) {
    int number = kNumba[i];
    kprintf("%3d", number);
    rbtree_insert(&tree, &number_new(number)->elem, number_compare);
    rbtree_check(tree, number_compare);
  }
  kprintf("\n");
  print();
  print_reversed();
  for (int i = 0; i < 20; ++i) {
    rbtree_remove(&tree, rbtree_get(tree, &(&(struct number){kNumba[i]})->elem,
                                    number_compare));
    rbtree_check(tree, number_compare);
    print();
  }
}

int main() {
  ShowCrashReports();
  simple_test();
}
