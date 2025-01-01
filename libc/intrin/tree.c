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

#include "tree.h"

struct Tree *tree_last(struct Tree *node) {
  while (node && node->right)
    node = node->right;
  return node;
}

struct Tree *tree_first(struct Tree *node) {
  while (node && tree_get_left(node))
    node = tree_get_left(node);
  return node;
}

struct Tree *tree_next(struct Tree *node) {
  if (!node)
    return 0;
  if (node->right)
    return tree_first(node->right);
  struct Tree *parent = node->parent;
  while (parent && node == parent->right) {
    node = parent;
    parent = parent->parent;
  }
  return parent;
}

struct Tree *tree_prev(struct Tree *node) {
  struct Tree *parent;
  if (!node)
    return 0;
  if (tree_get_left(node))
    return tree_last(tree_get_left(node));
  parent = node->parent;
  while (parent && node == tree_get_left(parent)) {
    node = parent;
    parent = parent->parent;
  }
  return parent;
}

dontinstrument static void tree_rotate_left(struct Tree **root,
                                            struct Tree *x) {
  struct Tree *y = x->right;
  x->right = tree_get_left(y);
  if (tree_get_left(y))
    tree_get_left(y)->parent = x;
  y->parent = x->parent;
  if (!x->parent) {
    *root = y;
  } else if (x == tree_get_left(x->parent)) {
    tree_set_left(x->parent, y);
  } else {
    x->parent->right = y;
  }
  tree_set_left(y, x);
  x->parent = y;
}

dontinstrument static void tree_rotate_right(struct Tree **root,
                                             struct Tree *y) {
  struct Tree *x = tree_get_left(y);
  tree_set_left(y, x->right);
  if (x->right)
    x->right->parent = y;
  x->parent = y->parent;
  if (!y->parent) {
    *root = x;
  } else if (y == y->parent->right) {
    y->parent->right = x;
  } else {
    tree_set_left(y->parent, x);
  }
  y->parent = x;
  x->right = y;
}

dontinstrument static void tree_rebalance_insert(struct Tree **root,
                                                 struct Tree *node) {
  struct Tree *uncle;
  tree_set_red(node, 1);
  while (node != *root && tree_get_red(node->parent)) {
    if (node->parent == tree_get_left(node->parent->parent)) {
      uncle = node->parent->parent->right;
      if (uncle && tree_get_red(uncle)) {
        tree_set_red(node->parent, 0);
        tree_set_red(uncle, 0);
        tree_set_red(node->parent->parent, 1);
        node = node->parent->parent;
      } else {
        if (node == node->parent->right) {
          node = node->parent;
          tree_rotate_left(root, node);
        }
        tree_set_red(node->parent, 0);
        tree_set_red(node->parent->parent, 1);
        tree_rotate_right(root, node->parent->parent);
      }
    } else {
      uncle = tree_get_left(node->parent->parent);
      if (uncle && tree_get_red(uncle)) {
        tree_set_red(node->parent, 0);
        tree_set_red(uncle, 0);
        tree_set_red(node->parent->parent, 1);
        node = node->parent->parent;
      } else {
        if (node == tree_get_left(node->parent)) {
          node = node->parent;
          tree_rotate_right(root, node);
        }
        tree_set_red(node->parent, 0);
        tree_set_red(node->parent->parent, 1);
        tree_rotate_left(root, node->parent->parent);
      }
    }
  }
  tree_set_red(*root, 0);
}

void tree_insert(struct Tree **root, struct Tree *node, tree_cmp_f *cmp) {
  struct Tree *search, *parent;
  node->word = 0;
  node->right = 0;
  node->parent = 0;
  if (!*root) {
    *root = node;
  } else {
    search = *root;
    parent = 0;
    do {
      parent = search;
      if (cmp(node, search) < 0) {
        search = tree_get_left(search);
      } else {
        search = search->right;
      }
    } while (search);
    if (cmp(node, parent) < 0) {
      tree_set_left(parent, node);
    } else {
      parent->right = node;
    }
    node->parent = parent;
    tree_rebalance_insert(root, node);
  }
}

dontinstrument static void tree_transplant(struct Tree **root, struct Tree *u,
                                           struct Tree *v) {
  if (!u->parent) {
    *root = v;
  } else if (u == tree_get_left(u->parent)) {
    tree_set_left(u->parent, v);
  } else {
    u->parent->right = v;
  }
  if (v)
    v->parent = u->parent;
}

dontinstrument static void tree_rebalance_remove(struct Tree **root,
                                                 struct Tree *node,
                                                 struct Tree *parent) {
  struct Tree *sibling;
  while (node != *root && (!node || !tree_get_red(node))) {
    if (node == tree_get_left(parent)) {
      sibling = parent->right;
      if (tree_get_red(sibling)) {
        tree_set_red(sibling, 0);
        tree_set_red(parent, 1);
        tree_rotate_left(root, parent);
        sibling = parent->right;
      }
      if ((!tree_get_left(sibling) || !tree_get_red(tree_get_left(sibling))) &&
          (!sibling->right || !tree_get_red(sibling->right))) {
        tree_set_red(sibling, 1);
        node = parent;
        parent = node->parent;
      } else {
        if (!sibling->right || !tree_get_red(sibling->right)) {
          tree_set_red(tree_get_left(sibling), 0);
          tree_set_red(sibling, 1);
          tree_rotate_right(root, sibling);
          sibling = parent->right;
        }
        tree_set_red(sibling, tree_get_red(parent));
        tree_set_red(parent, 0);
        tree_set_red(sibling->right, 0);
        tree_rotate_left(root, parent);
        node = *root;
        break;
      }
    } else {
      sibling = tree_get_left(parent);
      if (tree_get_red(sibling)) {
        tree_set_red(sibling, 0);
        tree_set_red(parent, 1);
        tree_rotate_right(root, parent);
        sibling = tree_get_left(parent);
      }
      if ((!sibling->right || !tree_get_red(sibling->right)) &&
          (!tree_get_left(sibling) || !tree_get_red(tree_get_left(sibling)))) {
        tree_set_red(sibling, 1);
        node = parent;
        parent = node->parent;
      } else {
        if (!tree_get_left(sibling) || !tree_get_red(tree_get_left(sibling))) {
          tree_set_red(sibling->right, 0);
          tree_set_red(sibling, 1);
          tree_rotate_left(root, sibling);
          sibling = tree_get_left(parent);
        }
        tree_set_red(sibling, tree_get_red(parent));
        tree_set_red(parent, 0);
        tree_set_red(tree_get_left(sibling), 0);
        tree_rotate_right(root, parent);
        node = *root;
        break;
      }
    }
  }
  if (node)
    tree_set_red(node, 0);
}

void tree_remove(struct Tree **root, struct Tree *node) {
  struct Tree *x = 0;
  struct Tree *y = node;
  struct Tree *x_parent = 0;
  int y_original_color = tree_get_red(y);
  if (!tree_get_left(node)) {
    x = node->right;
    tree_transplant(root, node, node->right);
    x_parent = node->parent;
  } else if (!node->right) {
    x = tree_get_left(node);
    tree_transplant(root, node, tree_get_left(node));
    x_parent = node->parent;
  } else {
    y = tree_first(node->right);
    y_original_color = tree_get_red(y);
    x = y->right;
    if (y->parent == node) {
      if (x)
        x->parent = y;
      x_parent = y;
    } else {
      tree_transplant(root, y, y->right);
      y->right = node->right;
      y->right->parent = y;
      x_parent = y->parent;
    }
    tree_transplant(root, node, y);
    tree_set_left(y, tree_get_left(node));
    tree_get_left(y)->parent = y;
    tree_set_red(y, tree_get_red(node));
  }
  if (!y_original_color)
    tree_rebalance_remove(root, x, x_parent);
}
