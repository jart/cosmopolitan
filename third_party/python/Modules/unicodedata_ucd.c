/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pymem.h"
#include "third_party/python/Modules/unicodedata.h"
#include "third_party/python/Modules/unicodedata_unidata.h"

PyTypeObject UCD_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "unicodedata.UCD",              /*tp_name*/
        sizeof(PreviousDBVersion),      /*tp_basicsize*/
};
