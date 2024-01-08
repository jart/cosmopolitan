/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/stat.h"
#include "libc/fmt/conv.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/mem/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/o.h"
#include "libc/x/x.h"
#include "libc/x/xasprintf.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/python/Include/bytesobject.h"
#include "third_party/python/Include/compile.h"
#include "third_party/python/Include/fileutils.h"
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/marshal.h"
#include "third_party/python/Include/pydebug.h"
#include "third_party/python/Include/pylifecycle.h"
#include "third_party/python/Include/pymacro.h"
#include "third_party/python/Include/pythonrun.h"
#include "third_party/python/Include/ucnhash.h"
#include "third_party/python/Include/yoink.h"
#include "libc/serialize.h"
#include "tool/build/lib/stripcomponents.h"

__static_yoink("_PyUnicode_GetCode");

#define MANUAL "\
SYNOPSIS\n\
\n\
  pycomp.com [FLAGS] SOURCE\n\
\n\
OVERVIEW\n\
\n\
  Python Compiler\n\
\n\
FLAGS\n\
\n\
  -o PATH      specified output pyc file\n\
  -O0          don't optimize [default]\n\
  -O1          remove debug statements\n\
  -O2          remove debug statements and docstrings\n\
  -n           do nothing\n\
  -h           help\n\
\n\
EXAMPLE\n\
\n\
  pycomp.com -o foo/__pycache__/__init__.cpython-3.6.pyc foo/__init__.py\n\
\n"

int optimize;
char *inpath;
char *outpath;

void
GetOpts(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "hnO:o:")) != -1) {
        switch (opt) {
        case 'O':
            optimize = atoi(optarg);
            break;
        case 'o':
            outpath = optarg;
            break;
        case 'n':
            exit(0);
        case 'h':
            fputs(MANUAL, stdout);
            exit(0);
        default:
            fputs(MANUAL, stderr);
            exit(1);
        }
    }
    if (argc - optind != 1) {
        fputs("error: need one input file\n", stderr);
        exit(1);
    }
    inpath = argv[optind];
    if (!outpath) {
        outpath = xasprintf("%sc", inpath);
    }
}

int
main(int argc, char *argv[])
{
    int fd;
    char *name;
    ssize_t rc;
    size_t i, n;
    struct stat st;
    char *p, m[12];
    PyObject *code, *marshalled;
    ShowCrashReports();
    GetOpts(argc, argv);
    marshalled = 0;
    if (stat(inpath, &st) == -1) perror(inpath), exit(1);
    CHECK_NOTNULL((p = gc(xslurp(inpath, &n))));
    Py_NoUserSiteDirectory++;
    Py_NoSiteFlag++;
    Py_IgnoreEnvironmentFlag++;
    Py_FrozenFlag++;
    /* Py_VerboseFlag++; */
    Py_SetProgramName(gc(utf8to32(argv[0], -1, 0)));
    _Py_InitializeEx_Private(1, 0);
    name = gc(xjoinpaths("/zip/.python", StripComponents(inpath, 3)));
    code = Py_CompileStringExFlags(p, name, Py_file_input, NULL, optimize);
    if (!code) goto error;
    marshalled = PyMarshal_WriteObjectToString(code, Py_MARSHAL_VERSION);
    Py_CLEAR(code);
    if (!marshalled) goto error;
    assert(PyBytes_CheckExact(marshalled));
    p = PyBytes_AS_STRING(marshalled);
    n = PyBytes_GET_SIZE(marshalled);
    CHECK_NE(-1, (fd = open(outpath, O_CREAT|O_TRUNC|O_WRONLY, 0644)));
    WRITE16LE(m+0, 3390); /* Python 3.7a1 */
    WRITE16LE(m+2, READ16LE("\r\n"));
    WRITE32LE(m+4, st.st_mtim.tv_sec); /* tsk tsk y2038 */
    WRITE32LE(m+8, n);
    CHECK_EQ(sizeof(m), write(fd, m, sizeof(m)));
    for (i = 0; i < n; i += rc) {
        CHECK_NE(-1, (rc = write(fd, p + i, n - i)));
    }
    CHECK_NE(-1, close(fd));
    Py_CLEAR(marshalled);
    Py_Finalize();
    return 0;
error:
    PyErr_Print();
    Py_Finalize();
    if (marshalled) Py_DECREF(marshalled);
    return 1;
}
