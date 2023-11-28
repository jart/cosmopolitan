#ifndef PYSQLITE_CACHE_H
#define PYSQLITE_CACHE_H
#include "third_party/python/Include/Python.h"

/* The LRU cache is implemented as a combination of a doubly-linked with a
 * dictionary. The list items are of type 'Node' and the dictionary has the
 * nodes as values. */

typedef struct _pysqlite_Node
{
    PyObject_HEAD
    PyObject* key;
    PyObject* data;
    long count;
    struct _pysqlite_Node* prev;
    struct _pysqlite_Node* next;
} pysqlite_Node;

typedef struct
{
    PyObject_HEAD
    int size;

    /* a dictionary mapping keys to Node entries */
    PyObject* mapping;

    /* the factory callable */
    PyObject* factory;

    pysqlite_Node* first;
    pysqlite_Node* last;

    /* if set, decrement the factory function when the Cache is deallocated.
     * this is almost always desirable, but not in the pysqlite context */
    int decref_factory;
} pysqlite_Cache;

extern PyTypeObject pysqlite_NodeType;
extern PyTypeObject pysqlite_CacheType;

int pysqlite_node_init(pysqlite_Node* self, PyObject* args, PyObject* kwargs);
void pysqlite_node_dealloc(pysqlite_Node* self);

int pysqlite_cache_init(pysqlite_Cache* self, PyObject* args, PyObject* kwargs);
void pysqlite_cache_dealloc(pysqlite_Cache* self);
PyObject* pysqlite_cache_get(pysqlite_Cache* self, PyObject* args);

int pysqlite_cache_setup_types(void);

#endif
