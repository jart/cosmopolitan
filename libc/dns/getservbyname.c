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
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/dns/ent.h"
#include "libc/dns/servicestxt.h"
#include "libc/mem/mem.h"

struct servent *getservbyname(const char *name, const char *proto) {
  static struct servent *ptr0, se0;
  char *localproto = proto;
  int p;

  if (!ptr0) {
    se0.s_name = NULL;
    se0.s_aliases = (char **)malloc(sizeof(char *) * 1);
    if (!se0.s_aliases) return NULL;
    se0.s_aliases[0] = NULL;

    se0.s_port = 0;
    se0.s_proto = NULL;
    ptr0 = &se0;
  }

  p = LookupServicesByName(name, &localproto);
  if (p == -1) {
    // localproto got alloc'd during the lookup?
    if (!proto && localproto != proto) free(localproto);
    return NULL;
  }

  ptr0->s_port = p;
  if (ptr0->s_name) free(ptr0->s_name);
  ptr0->s_name = strdup(name);

  if (ptr0->s_proto) free(ptr0->s_proto);
  ptr0->s_proto = strdup(localproto);

  if (!proto && localproto != proto) free(localproto);

  return ptr0;
}
