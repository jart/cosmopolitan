#ifndef Py_NODE_H
#define Py_NODE_H
#include "third_party/python/Include/pyport.h"
COSMOPOLITAN_C_START_

typedef struct _node {
    short		n_type;
    char		*n_str;
    int			n_lineno;
    int			n_col_offset;
    int			n_nchildren;
    struct _node	*n_child;
} node;

node * PyNode_New(int type);
int PyNode_AddChild(node *n, int type,
                                      char *str, int lineno, int col_offset);
void PyNode_Free(node *n);
#ifndef Py_LIMITED_API
Py_ssize_t _PyNode_SizeOf(node *n);
#endif

/* Node access functions */
#define NCH(n)		((n)->n_nchildren)

#define CHILD(n, i)	(&(n)->n_child[i])
#define RCHILD(n, i)	(CHILD(n, NCH(n) + i))
#define TYPE(n)		((n)->n_type)
#define STR(n)		((n)->n_str)
#define LINENO(n)       ((n)->n_lineno)

/* Assert that the type of a node is what we expect */
#define REQ(n, type) assert(TYPE(n) == (type))

void PyNode_ListTree(node *);

COSMOPOLITAN_C_END_
#endif /* !Py_NODE_H */
