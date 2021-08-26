/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=4 sts=4 sw=4 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
/* clang-format off */

/* Generated automatically from ./Modules/config.c.in by makesetup. */
/* -*- C -*- ***********************************************
Copyright (c) 2000, BeOpen.com.
Copyright (c) 1995-2000, Corporation for National Research Initiatives.
Copyright (c) 1990-1995, Stichting Mathematisch Centrum.
All rights reserved.

See the file "Misc/COPYRIGHT" for information on usage and
redistribution of this file, and for a DISCLAIMER OF ALL WARRANTIES.
******************************************************************/

/* Module configuration */

/* !!! !!! !!! This file is edited by the makesetup script !!! !!! !!! */

/* This file contains the table of built-in modules.
   See create_builtin() in import.c. */

#include "third_party/python/Include/pyport.h"
#include "third_party/python/Include/pyport.h"
#include "third_party/python/Include/Python.h"

PyObject* PyInit__decimal(void);
PyObject* PyInit_audioop(void);
PyObject* PyInit_posix(void);
PyObject* PyInit_errno(void);
PyObject* PyInit_pwd(void);
PyObject* PyInit__sre(void);
PyObject* PyInit__codecs(void);
PyObject* PyInit__functools(void);
PyObject* PyInit__operator(void);
PyObject* PyInit__collections(void);
PyObject* PyInit_itertools(void);
PyObject* PyInit_atexit(void);
PyObject* PyInit__signal(void);
PyObject* PyInit__stat(void);
PyObject* PyInit_time(void);
PyObject* PyInit__locale(void);
PyObject* PyInit__io(void);
PyObject* PyInit_zipimport(void);
PyObject* PyInit_faulthandler(void);
PyObject* PyInit__tracemalloc(void);
PyObject* PyInit__symtable(void);
PyObject* PyInit_array(void);
PyObject* PyInit_cmath(void);
PyObject* PyInit_math(void);
PyObject* PyInit__struct(void);
PyObject* PyInit__weakref(void);
PyObject* PyInit__testcapi(void);
PyObject* PyInit__random(void);
PyObject* PyInit__elementtree(void);
PyObject* PyInit__pickle(void);
PyObject* PyInit__datetime(void);
PyObject* PyInit__bisect(void);
PyObject* PyInit__heapq(void);
PyObject* PyInit_unicodedata(void);
PyObject* PyInit_fcntl(void);
PyObject* PyInit_grp(void);
PyObject* PyInit_select(void);
PyObject* PyInit_mmap(void);
PyObject* PyInit__csv(void);
PyObject* PyInit__socket(void);
PyObject* PyInit_resource(void);
PyObject* PyInit__posixsubprocess(void);
PyObject* PyInit__multiprocessing(void);
PyObject* PyInit__md5(void);
PyObject* PyInit__sha1(void);
PyObject* PyInit__sha256(void);
PyObject* PyInit__sha512(void);
PyObject* PyInit__sha3(void);
PyObject* PyInit_syslog(void);
PyObject* PyInit_binascii(void);
PyObject* PyInit_parser(void);
PyObject* PyInit_fpectl(void);
PyObject* PyInit_zlib(void);
PyObject* PyInit_pyexpat(void);
PyObject* PyInit__multibytecodec(void);
PyObject* PyInit__codecs_cn(void);
PyObject* PyInit__codecs_hk(void);
PyObject* PyInit__codecs_iso2022(void);
PyObject* PyInit__codecs_jp(void);
PyObject* PyInit__codecs_kr(void);
PyObject* PyInit__codecs_tw(void);
PyObject* PyInit__json(void);
PyObject* PyInit__lsprof(void);
PyObject* PyInit__opcode(void);
PyObject* PyInit_termios(void);
PyObject *PyInit__sqlite3(void);

/* -- ADDMODULE MARKER 1 -- */

PyObject* PyMarshal_Init(void);
PyObject* PyInit_imp(void);
PyObject* PyInit_gc(void);
PyObject* PyInit__ast(void);
PyObject* _PyWarnings_Init(void);
PyObject* PyInit__string(void);

struct _inittab _PyImport_Inittab[] = {

    {"_decimal", PyInit__decimal},
    {"posix", PyInit_posix},
    {"errno", PyInit_errno},
    {"pwd", PyInit_pwd},
    {"_sre", PyInit__sre},
    {"_codecs", PyInit__codecs},
    {"_functools", PyInit__functools},
    {"_operator", PyInit__operator},
    {"_collections", PyInit__collections},
    {"itertools", PyInit_itertools},
    {"atexit", PyInit_atexit},
    {"_signal", PyInit__signal},
    {"_stat", PyInit__stat},
    {"time", PyInit_time},
    {"_locale", PyInit__locale},
    {"_io", PyInit__io},
    {"faulthandler", PyInit_faulthandler},
    {"_tracemalloc", PyInit__tracemalloc},
    {"_symtable", PyInit__symtable},
    {"array", PyInit_array},
    {"cmath", PyInit_cmath},
    {"math", PyInit_math},
    {"_struct", PyInit__struct},
    {"_weakref", PyInit__weakref},
    {"_testcapi", PyInit__testcapi},
    {"_random", PyInit__random},
    {"_pickle", PyInit__pickle},
    {"_datetime", PyInit__datetime},
    {"_bisect", PyInit__bisect},
    {"_heapq", PyInit__heapq},
    {"unicodedata", PyInit_unicodedata},
    {"fcntl", PyInit_fcntl},
    {"grp", PyInit_grp},
    {"select", PyInit_select},
    {"mmap", PyInit_mmap},
    {"_csv", PyInit__csv},
    {"_socket", PyInit__socket},
    {"resource", PyInit_resource},
    {"_posixsubprocess", PyInit__posixsubprocess},
    {"_multiprocessing", PyInit__multiprocessing},
    {"_md5", PyInit__md5},
    {"_sha1", PyInit__sha1},
    {"_sha256", PyInit__sha256},
    {"_sha512", PyInit__sha512},
    {"_sha3", PyInit__sha3},
    {"syslog", PyInit_syslog},
    {"binascii", PyInit_binascii},
    {"parser", PyInit_parser},
    {"fpectl", PyInit_fpectl},
    {"pyexpat", PyInit_pyexpat},
    {"_multibytecodec", PyInit__multibytecodec},
    {"_json", PyInit__json},
    {"_opcode", PyInit__opcode},
    {"termios", PyInit_termios},

#if !IsTiny()
    {"zlib", PyInit_zlib},
    {"sqlite3", PyInit__sqlite3},
    {"zipimport", PyInit_zipimport},
    {"_elementtree", PyInit__elementtree},
    {"_codecs_cn", PyInit__codecs_cn},
    {"_codecs_hk", PyInit__codecs_hk},
    {"_codecs_iso2022", PyInit__codecs_iso2022},
    {"_codecs_jp", PyInit__codecs_jp},
    {"_codecs_kr", PyInit__codecs_kr},
    {"_codecs_tw", PyInit__codecs_tw},
    {"audioop", PyInit_audioop},
    {"_lsprof", PyInit__lsprof},
#endif

/* -- ADDMODULE MARKER 2 -- */

    /* This module lives in marshal.c */
    {"marshal", PyMarshal_Init},

    /* This lives in import.c */
    {"_imp", PyInit_imp},

    /* This lives in Python/Python-ast.c */
    {"_ast", PyInit__ast},

    /* These entries are here for sys.builtin_module_names */
    {"builtins", NULL},
    {"sys", NULL},

    /* This lives in gcmodule.c */
    {"gc", PyInit_gc},

    /* This lives in _warnings.c */
    {"_warnings", _PyWarnings_Init},

    /* This lives in Objects/unicodeobject.c */
    {"_string", PyInit__string},

    /* Sentinel */
    {0, 0}
};
