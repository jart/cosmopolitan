#ifdef _COSMO_SOURCE
#ifndef COSMOPOLITAN_RBTREE_H_
#define COSMOPOLITAN_RBTREE_H_
#define rbtree_ceil   __rbtree_ceil
#define rbtree_first  __rbtree_first
#define rbtree_floor  __rbtree_floor
#define rbtree_get    __rbtree_get
#define rbtree_insert __rbtree_insert
#define rbtree_last   __rbtree_last
#define rbtree_next   __rbtree_next
#define rbtree_prev   __rbtree_prev
#define rbtree_remove __rbtree_remove
COSMOPOLITAN_C_START_

#define RBTREE_CONTAINER(t, f, p) ((t *)(((char *)(p)) - offsetof(t, f)))

struct rbtree {
  uintptr_t word;
  struct rbtree *right;
  struct rbtree *parent;
};

typedef int rbtree_cmp_f(const struct rbtree *, const struct rbtree *);

static inline struct rbtree *rbtree_get_left(const struct rbtree *node) {
  return (struct rbtree *)(node->word & -2);
}

static inline void rbtree_set_left(struct rbtree *node, struct rbtree *left) {
  node->word = (uintptr_t)left | (node->word & 1);
}

static inline int rbtree_get_red(const struct rbtree *node) {
  return node->word & 1;
}

static inline void rbtree_set_red(struct rbtree *node, int red) {
  node->word &= -2;
  node->word |= red;
}

struct rbtree *rbtree_next(struct rbtree *) libcesque;
struct rbtree *rbtree_prev(struct rbtree *) libcesque;
struct rbtree *rbtree_first(struct rbtree *) libcesque;
struct rbtree *rbtree_last(struct rbtree *) libcesque;
void rbtree_remove(struct rbtree **, struct rbtree *) libcesque;
void rbtree_insert(struct rbtree **, struct rbtree *, rbtree_cmp_f *) libcesque;
struct rbtree *rbtree_get(const struct rbtree *, const struct rbtree *,
                          rbtree_cmp_f *) libcesque;
struct rbtree *rbtree_ceil(const struct rbtree *, const struct rbtree *,
                           rbtree_cmp_f *) libcesque;
struct rbtree *rbtree_floor(const struct rbtree *, const struct rbtree *,
                            rbtree_cmp_f *) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_RBTREE_H_ */
#endif /* _COSMO_SOURCE */
