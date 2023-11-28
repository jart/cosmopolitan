#ifndef Py_PGEN_H
#define Py_PGEN_H
#include "third_party/python/Include/grammar.h"
COSMOPOLITAN_C_START_

/* Parser generator interface */

extern grammar *meta_grammar(void);

struct _node;
extern grammar *pgen(struct _node *);

COSMOPOLITAN_C_END_
#endif /* !Py_PGEN_H */
