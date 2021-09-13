/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=4 sts=4 sw=4 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Include/Python.h"
#include "third_party/python/Include/cosmo.h"
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/pyport.h"
/* clang-format off */

PyObject *PyInit__ast(void);
PyObject *PyInit__bisect(void);
PyObject *PyInit__bz2(void);
PyObject *PyInit__codecs(void);
PyObject *PyInit__codecs_cn(void);
PyObject *PyInit__codecs_hk(void);
PyObject *PyInit__codecs_iso2022(void);
PyObject *PyInit__codecs_jp(void);
PyObject *PyInit__codecs_kr(void);
PyObject *PyInit__codecs_tw(void);
PyObject *PyInit__collections(void);
PyObject *PyInit__csv(void);
PyObject *PyInit__datetime(void);
PyObject *PyInit__decimal(void);
PyObject *PyInit__elementtree(void);
PyObject *PyInit__functools(void);
PyObject *PyInit__heapq(void);
PyObject *PyInit__io(void);
PyObject *PyInit__json(void);
PyObject *PyInit__locale(void);
PyObject *PyInit__lsprof(void);
PyObject *PyInit__md5(void);
PyObject *PyInit__multibytecodec(void);
PyObject *PyInit__multiprocessing(void);
PyObject *PyInit__opcode(void);
PyObject *PyInit__operator(void);
PyObject *PyInit__pickle(void);
PyObject *PyInit__posixsubprocess(void);
PyObject *PyInit__random(void);
PyObject *PyInit__sha1(void);
PyObject *PyInit__sha256(void);
PyObject *PyInit__sha3(void);
PyObject *PyInit__sha512(void);
PyObject *PyInit__signal(void);
PyObject *PyInit__socket(void);
PyObject *PyInit__sqlite3(void);
PyObject *PyInit__sre(void);
PyObject *PyInit__stat(void);
PyObject *PyInit__string(void);
PyObject *PyInit__struct(void);
PyObject *PyInit__symtable(void);
PyObject *PyInit__testcapi(void);
PyObject *PyInit__tracemalloc(void);
PyObject *PyInit__weakref(void);
PyObject *PyInit_array(void);
PyObject *PyInit_atexit(void);
PyObject *PyInit_audioop(void);
PyObject *PyInit_binascii(void);
PyObject *PyInit_cmath(void);
PyObject *PyInit_cosmo(void);
PyObject *PyInit_errno(void);
PyObject *PyInit_faulthandler(void);
PyObject *PyInit_fcntl(void);
PyObject *PyInit_fpectl(void);
PyObject *PyInit_gc(void);
PyObject *PyInit_grp(void);
PyObject *PyInit_imp(void);
PyObject *PyInit_itertools(void);
PyObject *PyInit_math(void);
PyObject *PyInit_mmap(void);
PyObject *PyInit_parser(void);
PyObject *PyInit_posix(void);
PyObject *PyInit_pwd(void);
PyObject *PyInit_pyexpat(void);
PyObject *PyInit_resource(void);
PyObject *PyInit_select(void);
PyObject *PyInit_syslog(void);
PyObject *PyInit_termios(void);
PyObject *PyInit_time(void);
PyObject *PyInit_unicodedata(void);
PyObject *PyInit_zipimport(void);
PyObject *PyInit_zlib(void);
PyObject *PyMarshal_Init(void);
PyObject *_PyWarnings_Init(void);

_Alignas(16) _Section(".rodata.pytab.0") const struct _inittab _PyImport_Inittab[0];
_Alignas(16) _Section(".rodata.pytab.2") const struct _inittab _PyImport_Inittab2[] = {
    {"posix", PyInit_posix},
    {"errno", PyInit_errno},
    {"_sre", PyInit__sre},
    {"_codecs", PyInit__codecs},
    {"_functools", PyInit__functools},
    {"_operator", PyInit__operator},
    {"_collections", PyInit__collections},
    {"itertools", PyInit_itertools},
    {"_signal", PyInit__signal},
    {"_locale", PyInit__locale},
    {"_io", PyInit__io},
    {"_weakref", PyInit__weakref},
    {"_heapq", PyInit__heapq},
    {"marshal", PyMarshal_Init},
    {"_imp", PyInit_imp},
    {"_cosmo", PyInit_cosmo},
    {"_ast", PyInit__ast},
    {"builtins", NULL},
    {"sys", NULL},
    {"gc", PyInit_gc},
    {"_warnings", _PyWarnings_Init}, 
    {"_string", PyInit__string},
    {0, 0}
};
