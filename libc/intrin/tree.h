#ifndef COSMOPOLITAN_TREE_H_
#define COSMOPOLITAN_TREE_H_
#define tree_first  __tree_first
#define tree_insert __tree_insert
#define tree_last   __tree_last
#define tree_next   __tree_next
#define tree_prev   __tree_prev
#define tree_remove __tree_remove
COSMOPOLITAN_C_START_

#define TREE_CONTAINER(t, f, p) ((t *)(((char *)(p)) - offsetof(t, f)))

struct Tree {
  uintptr_t word;
  struct Tree *right;
  struct Tree *parent;
};

typedef int tree_search_f(const void *, const struct Tree *);
typedef int tree_cmp_f(const struct Tree *, const struct Tree *);

forceinline struct Tree *tree_get_left(const struct Tree *node) {
  return (struct Tree *)(node->word & -2);
}

static inline void tree_set_left(struct Tree *node, struct Tree *left) {
  node->word = (uintptr_t)left | (node->word & 1);
}

static inline int tree_get_red(const struct Tree *node) {
  return node->word & 1;
}

static inline void tree_set_red(struct Tree *node, int red) {
  node->word &= -2;
  node->word |= red;
}

forceinline optimizespeed struct Tree *tree_floor(const struct Tree *node,
                                                  const void *key,
                                                  tree_search_f *cmp) {
  struct Tree *left = 0;
  while (node) {
    if (cmp(key, node) >= 0) {
      left = (struct Tree *)node;
      node = tree_get_left(node);
    } else {
      node = node->right;
    }
  }
  return left;
}

static inline struct Tree *tree_ceil(const struct Tree *node, const void *key,
                                     tree_search_f *cmp) {
  struct Tree *right = 0;
  while (node) {
    if (cmp(key, node) < 0) {
      right = (struct Tree *)node;
      node = tree_get_left(node);
    } else {
      node = node->right;
    }
  }
  return right;
}

static inline struct Tree *tree_get(const struct Tree *node, const void *key,
                                    tree_search_f *cmp) {
  while (node) {
    int c = cmp(key, node);
    if (c < 0) {
      node = tree_get_left(node);
    } else if (c > 0) {
      node = node->right;
    } else {
      return (struct Tree *)node;
    }
  }
  return 0;
}

struct Tree *tree_next(struct Tree *) libcesque;
struct Tree *tree_prev(struct Tree *) libcesque;
struct Tree *tree_first(struct Tree *) libcesque;
struct Tree *tree_last(struct Tree *) libcesque;
void tree_remove(struct Tree **, struct Tree *) libcesque;
void tree_insert(struct Tree **, struct Tree *, tree_cmp_f *) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TREE_H_ */
