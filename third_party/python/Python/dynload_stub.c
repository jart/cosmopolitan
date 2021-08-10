#include "third_party/python/Include/Python.h"
#include "third_party/python/Include/importdl.h"
/* clang-format off */

/* This module provides the necessary stubs for when dynamic loading is
   not present. */

const char *_PyImport_DynLoadFiletab[] = {NULL};
