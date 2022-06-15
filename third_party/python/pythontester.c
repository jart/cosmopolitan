/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=4 sts=4 sw=4 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "third_party/python/Include/yoink.h"
#include "third_party/python/runpythonmodule.h"
#include "tool/args/args.h"
// clang-format off

int
main(int argc, char **argv)
{
    LoadZipArgs(&argc, &argv);
    __nosync = 0x5453455454534146;
    return RunPythonModule(argc, argv);
}
