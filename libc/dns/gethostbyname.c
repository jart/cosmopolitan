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
#include "libc/dns/ent.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"

struct hostent *gethostbyname(const char *name) {
  static struct hostent *ptr0, he0;
  static char h_name[DNS_NAME_MAX + 1];
  static char *h_aliases[1];
  static char *h_addr_list[2];
  static char h_addr_list0[4];
  struct addrinfo *result = NULL;

  if (!ptr0) {
    he0.h_name = h_name;

    he0.h_aliases = h_aliases;
    he0.h_aliases[0] = NULL;

    he0.h_addrtype = AF_INET;
    he0.h_length = 4;
    he0.h_addr_list = h_addr_list;

    he0.h_addr_list[0] = h_addr_list0;
    he0.h_addr_list[1] = NULL;

    ptr0 = &he0;
  }

  if (getaddrinfo(name, NULL, NULL, &result) || result == NULL) return NULL;

  /* if getaddrinfo is successful, result->ai_canonname is non-NULL,
   * (see newaddrinfo) but the string can still be empty */
  if (result->ai_canonname[0])
    memccpy(ptr0->h_name, result->ai_canonname, '\0', DNS_NAME_MAX);
  else
    memccpy(ptr0->h_name, name, '\0', DNS_NAME_MAX);

  *((uint32_t *)ptr0->h_addr_list[0]) = (result->ai_addr4->sin_addr.s_addr);
  /* TODO: if result has ai_next, fit multiple entries for h_addr_list */

  freeaddrinfo(result);
  return ptr0;
}
