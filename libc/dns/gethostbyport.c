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
#include "libc/mem/mem.h"
#include "libc/sysv/consts/af.h"

struct hostent *gethostbyaddr(const void *s_addr, socklen_t len, int type) {
  static struct hostent *ptr1, he1;
  struct sockaddr_in addr;
  char name[DNS_NAME_MAX + 1];

  if (!ptr1) {
    he1.h_name = NULL;

    he1.h_aliases = (char **)malloc(sizeof(char *) * 1);
    if (!he1.h_aliases) return NULL;
    he1.h_aliases[0] = NULL;

    he1.h_addrtype = AF_INET;
    he1.h_length = 4;
    he1.h_addr_list = (char **)malloc(sizeof(char *) * 2);
    if (!he1.h_addr_list) return NULL;

    he1.h_addr_list[0] = (char *)malloc(sizeof(uint32_t));
    if (!he1.h_addr_list[0]) return NULL;
    he1.h_addr_list[1] = NULL;

    ptr1 = &he1;
  }

  if (type != AF_INET || len != sizeof(uint32_t)) return NULL;
  addr.sin_family = AF_INET;
  addr.sin_port = 0;
  addr.sin_addr.s_addr = *(uint32_t *)(s_addr);

  if (getnameinfo((struct sockaddr *)&addr, sizeof(addr), name, sizeof(name),
                  NULL, 0, 0))
    return NULL;

  if (ptr1->h_name) free(ptr1->h_name);
  ptr1->h_name = strdup(name);
  *((uint32_t *)ptr1->h_addr_list[0]) = (addr.sin_addr.s_addr);

  return ptr1;
}
