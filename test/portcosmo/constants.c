/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ This is free and unencumbered software released into the public domain.      │
│                                                                              │
│ Anyone is free to copy, modify, publish, use, compile, sell, or              │
│ distribute this software, either in source code form or as a compiled        │
│ binary, for any purpose, commercial or non-commercial, and by any            │
│ means.                                                                       │
│                                                                              │
│ In jurisdictions that recognize copyright laws, the author or authors        │
│ of this software dedicate any and all copyright interest in the              │
│ software to the public domain. We make this dedication for the benefit       │
│ of the public at large and to the detriment of our heirs and                 │
│ successors. We intend this dedication to be an overt act of                  │
│ relinquishment in perpetuity of all present and future rights to this        │
│ software under copyright law.                                                │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,              │
│ EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF           │
│ MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.       │
│ IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR            │
│ OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        │
│ ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        │
│ OTHER DEALINGS IN THE SOFTWARE.                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "test/portcosmo/constants.h"

const int TWO = 175;
const int THREE = 201;

/* the above int values are unknown at compile-time, but
 * ifswitch.c and initstruct.c will both compile without
 * raising an error when the -fportcosmo flag is passed
 * to a gcc patched with third_party/gcc/portcosmo.patch.
 *
 * what -fportcosmo does is it modifies the AST so that 
 * the following errors are intercepted and avoided:
 *
 *  - "case is not constant" -- if there is a switch stmt
 *    that uses the values TWO or THREE in its cases, it
 *    will be rewritten into an if-else statement so as
 *    to prevent this "case is not constant" error.
 *
 *  - "initializer element is not constant" - if there is
 *    global or static struct that is being initialized 
 *    with the values TWO or THREE, temporaries will be
 *    used to prevent this error, and a small conditional 
 *    or an __attribute__ ((constructor)) will added to 
 *    the code that later fills the correct struct values.
 */
