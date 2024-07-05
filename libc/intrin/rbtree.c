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

#include "rbtree.h"
#include "libc/dce.h"
#include "libc/str/str.h"

#define RBTREE_DEBUG

struct rbtree *rbtree_next(struct rbtree *node) {
  if (!node)
    return 0;
  if (node->right)
    return rbtree_first(node->right);
  struct rbtree *parent = node->parent;
  while (parent && node == parent->right) {
    node = parent;
    parent = parent->parent;
  }
  return parent;
}

struct rbtree *rbtree_prev(struct rbtree *node) {
  if (!node)
    return 0;
  if (rbtree_get_left(node))
    return rbtree_last(rbtree_get_left(node));
  struct rbtree *parent = node->parent;
  while (parent && node == rbtree_get_left(parent)) {
    node = parent;
    parent = parent->parent;
  }
  return parent;
}

struct rbtree *rbtree_first(struct rbtree *node) {
  while (node && rbtree_get_left(node))
    node = rbtree_get_left(node);
  return node;
}

struct rbtree *rbtree_last(struct rbtree *node) {
  while (node && node->right)
    node = node->right;
  return node;
}

static void rbtree_rotate_left(struct rbtree **root, struct rbtree *x) {
  struct rbtree *y = x->right;
  x->right = rbtree_get_left(y);
  if (rbtree_get_left(y))
    rbtree_get_left(y)->parent = x;
  y->parent = x->parent;
  if (!x->parent) {
    *root = y;
  } else if (x == rbtree_get_left(x->parent)) {
    rbtree_set_left(x->parent, y);
  } else {
    x->parent->right = y;
  }
  rbtree_set_left(y, x);
  x->parent = y;
}

static void rbtree_rotate_right(struct rbtree **root, struct rbtree *y) {
  struct rbtree *x = rbtree_get_left(y);
  rbtree_set_left(y, x->right);
  if (x->right)
    x->right->parent = y;
  x->parent = y->parent;
  if (!y->parent) {
    *root = x;
  } else if (y == y->parent->right) {
    y->parent->right = x;
  } else {
    rbtree_set_left(y->parent, x);
  }
  x->right = y;
  y->parent = x;
}

static void rbtree_insert_fixup(struct rbtree **root, struct rbtree *node) {
  rbtree_set_red(node, 1);
  while (node != *root && rbtree_get_red(node->parent)) {
    if (node->parent == rbtree_get_left(node->parent->parent)) {
      struct rbtree *uncle = node->parent->parent->right;
      if (uncle && rbtree_get_red(uncle)) {
        rbtree_set_red(node->parent, 0);
        rbtree_set_red(uncle, 0);
        rbtree_set_red(node->parent->parent, 1);
        node = node->parent->parent;
      } else {
        if (node == node->parent->right) {
          node = node->parent;
          rbtree_rotate_left(root, node);
        }
        rbtree_set_red(node->parent, 0);
        rbtree_set_red(node->parent->parent, 1);
        rbtree_rotate_right(root, node->parent->parent);
      }
    } else {
      struct rbtree *uncle = rbtree_get_left(node->parent->parent);
      if (uncle && rbtree_get_red(uncle)) {
        rbtree_set_red(node->parent, 0);
        rbtree_set_red(uncle, 0);
        rbtree_set_red(node->parent->parent, 1);
        node = node->parent->parent;
      } else {
        if (node == rbtree_get_left(node->parent)) {
          node = node->parent;
          rbtree_rotate_right(root, node);
        }
        rbtree_set_red(node->parent, 0);
        rbtree_set_red(node->parent->parent, 1);
        rbtree_rotate_left(root, node->parent->parent);
      }
    }
  }
  rbtree_set_red(*root, 0);
}

void rbtree_insert(struct rbtree **root, struct rbtree *node,
                   rbtree_cmp_f *cmp) {
  bzero(node, sizeof(*node));
  if (!*root) {
    *root = node;
  } else {
    struct rbtree *search = *root;
    struct rbtree *parent = 0;
    do {
      parent = search;
      if (cmp(node, search) < 0) {
        search = rbtree_get_left(search);
      } else {
        search = search->right;
      }
    } while (search);
    if (cmp(node, parent) < 0) {
      rbtree_set_left(parent, node);
    } else {
      parent->right = node;
    }
    node->parent = parent;
    rbtree_insert_fixup(root, node);
  }
}

static void rbtree_transplant(struct rbtree **root, struct rbtree *u,
                              struct rbtree *v) {
  if (!u->parent) {
    *root = v;
  } else if (u == rbtree_get_left(u->parent)) {
    rbtree_set_left(u->parent, v);
  } else {
    u->parent->right = v;
  }
  if (v)
    v->parent = u->parent;
}

static void rbtree_remove_fixup(struct rbtree **root, struct rbtree *node,
                                struct rbtree *parent) {
  while (node != *root && (!node || !rbtree_get_red(node))) {
    if (node == rbtree_get_left(parent)) {
      struct rbtree *sibling = parent->right;
      if (rbtree_get_red(sibling)) {
        rbtree_set_red(sibling, 0);
        rbtree_set_red(parent, 1);
        rbtree_rotate_left(root, parent);
        sibling = parent->right;
      }
      if ((!rbtree_get_left(sibling) ||
           !rbtree_get_red(rbtree_get_left(sibling))) &&
          (!sibling->right || !rbtree_get_red(sibling->right))) {
        rbtree_set_red(sibling, 1);
        node = parent;
        parent = node->parent;
      } else {
        if (!sibling->right || !rbtree_get_red(sibling->right)) {
          rbtree_set_red(rbtree_get_left(sibling), 0);
          rbtree_set_red(sibling, 1);
          rbtree_rotate_right(root, sibling);
          sibling = parent->right;
        }
        rbtree_set_red(sibling, rbtree_get_red(parent));
        rbtree_set_red(parent, 0);
        rbtree_set_red(sibling->right, 0);
        rbtree_rotate_left(root, parent);
        node = *root;
        break;
      }
    } else {
      struct rbtree *sibling = rbtree_get_left(parent);
      if (rbtree_get_red(sibling)) {
        rbtree_set_red(sibling, 0);
        rbtree_set_red(parent, 1);
        rbtree_rotate_right(root, parent);
        sibling = rbtree_get_left(parent);
      }
      if ((!sibling->right || !rbtree_get_red(sibling->right)) &&
          (!rbtree_get_left(sibling) ||
           !rbtree_get_red(rbtree_get_left(sibling)))) {
        rbtree_set_red(sibling, 1);
        node = parent;
        parent = node->parent;
      } else {
        if (!rbtree_get_left(sibling) ||
            !rbtree_get_red(rbtree_get_left(sibling))) {
          rbtree_set_red(sibling->right, 0);
          rbtree_set_red(sibling, 1);
          rbtree_rotate_left(root, sibling);
          sibling = rbtree_get_left(parent);
        }
        rbtree_set_red(sibling, rbtree_get_red(parent));
        rbtree_set_red(parent, 0);
        rbtree_set_red(rbtree_get_left(sibling), 0);
        rbtree_rotate_right(root, parent);
        node = *root;
        break;
      }
    }
  }
  if (node)
    rbtree_set_red(node, 0);
}

void rbtree_remove(struct rbtree **root, struct rbtree *node) {
  struct rbtree *y = node;
  struct rbtree *x = 0;
  struct rbtree *x_parent = 0;
  int y_original_color = rbtree_get_red(y);
  if (!rbtree_get_left(node)) {
    x = node->right;
    rbtree_transplant(root, node, node->right);
    x_parent = node->parent;
  } else if (!node->right) {
    x = rbtree_get_left(node);
    rbtree_transplant(root, node, rbtree_get_left(node));
    x_parent = node->parent;
  } else {
    y = rbtree_first(node->right);
    y_original_color = rbtree_get_red(y);
    x = y->right;
    if (y->parent == node) {
      if (x)
        x->parent = y;
      x_parent = y;
    } else {
      rbtree_transplant(root, y, y->right);
      y->right = node->right;
      y->right->parent = y;
      x_parent = y->parent;
    }
    rbtree_transplant(root, node, y);
    rbtree_set_left(y, rbtree_get_left(node));
    rbtree_get_left(y)->parent = y;
    rbtree_set_red(y, rbtree_get_red(node));
  }
  if (!y_original_color)
    rbtree_remove_fixup(root, x, x_parent);
}

struct rbtree *rbtree_get(const struct rbtree *node, const struct rbtree *key,
                          rbtree_cmp_f *cmp) {
  while (node) {
    int c = cmp(key, node);
    if (c < 0) {
      node = rbtree_get_left(node);
    } else if (c > 0) {
      node = node->right;
    } else {
      return (struct rbtree *)node;
    }
  }
  return 0;
}

struct rbtree *rbtree_floor(const struct rbtree *node, const struct rbtree *key,
                            rbtree_cmp_f *cmp) {
  while (node) {
    if (cmp(key, node) < 0) {
      node = rbtree_get_left(node);
    } else {
      node = node->right;
    }
  }
  return (struct rbtree *)node;
}

struct rbtree *rbtree_ceil(const struct rbtree *node, const struct rbtree *key,
                           rbtree_cmp_f *cmp) {
  while (node) {
    if (cmp(node, key) < 0) {
      node = rbtree_get_left(node);
    } else {
      node = node->right;
    }
  }
  return (struct rbtree *)node;
}
