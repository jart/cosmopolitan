/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/calls.h"
#include "libc/runtime/runtime.h"
#include "third_party/chibicc/chibicc.h"

long alloc_node_count;
long alloc_token_count;
long alloc_obj_count;
long alloc_type_count;

wontreturn void __oom_hook(size_t request) {
  tinyprint(2, "error: chibicc ran out of memory\n", NULL);
  exit(1);
}

static void *alloc(size_t n, long *c) {
  void *r;
  if ((r = calloc(1, n))) {
    ++*c;
    return r;
  } else {
    __oom_hook(n);
  }
}

Node *alloc_node(void) {
  return alloc(sizeof(Node), &alloc_node_count);
}

Token *alloc_token(void) {
  return alloc(sizeof(Token), &alloc_token_count);
}

Obj *alloc_obj(void) {
  return alloc(sizeof(Obj), &alloc_obj_count);
}

Type *alloc_type(void) {
  return alloc(sizeof(Type), &alloc_type_count);
}
