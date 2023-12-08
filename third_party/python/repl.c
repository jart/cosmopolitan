/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/runtime/stack.h"
#include "third_party/python/Include/yoink.h"
#include "third_party/python/runpythonmodule.h"
#include "tool/args/args.h"

STATIC_STACK_ALIGN(GetStackSize());

int
main(int argc, char **argv)
{
    LoadZipArgs(&argc, &argv);
    return RunPythonModule(argc, argv);
}
