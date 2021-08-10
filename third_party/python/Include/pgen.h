#ifndef Py_PGEN_H
#define Py_PGEN_H
COSMOPOLITAN_C_START_
/* clang-format off */

/* Parser generator interface */

extern grammar *meta_grammar(void);

struct _node;
extern grammar *pgen(struct _node *);

COSMOPOLITAN_C_END_
#endif /* !Py_PGEN_H */
