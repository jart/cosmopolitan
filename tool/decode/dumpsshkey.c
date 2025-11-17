/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/x/x.h"
#include "net/http/ssh.h"

void PrintOpensshPublicKey(struct OpensshPublicKey *key, int indent) {
  printf("%*salgo: %`#s\n", indent, "", key->algo);
  printf("%*ssize: %zu\n", indent, "", key->size);
  printf("%*sdata: %`#.*s\n", indent, "", (int)key->size, key->data);
  printf("%*scomment: %`#s\n", indent, "", key->comment);
}

void PrintOpensshPrivateKey(struct OpensshPrivateKey *key, int indent) {
  printf("%*srng1: 0x%08x\n", indent, "", key->rng1);
  printf("%*srng2: 0x%08x\n", indent, "", key->rng2);
  printf("%*spub:\n", indent, "");
  PrintOpensshPublicKey(&key->pub, indent + 2);
  printf("%*ssize: %zu\n", indent, "", key->size);
  printf("%*sdata: %`#.*s\n", indent, "", (int)key->size, key->data);
  printf("%*scomment: %`#s\n", indent, "", key->comment);
}

void PrintOpensshKey(struct OpensshKey *key, int indent) {
  printf("%*spub:\n", indent, "");
  PrintOpensshPublicKey(&key->pub, indent + 2);
  printf("%*skey:\n", indent, "");
  PrintOpensshPrivateKey(&key->key, indent + 2);
}

void PrintOpensshKeyV1(struct OpensshKeyV1 *key, int indent) {
  printf("%*scipher: %`#s\n", indent, "", key->cipher);
  printf("%*skdfname: %`#s\n", indent, "", key->kdfname);
  printf("%*skdfsize: %zu\n", indent, "", key->kdfsize);
  printf("%*skdfparams: %`#.*s\n", indent, "", (int)key->kdfsize,
         key->kdfparams);
  printf("%*skeycount: %zu\n", indent, "", key->keycount);
  for (size_t i = 0; i < key->keycount; ++i) {
    printf("%*skeys[%zu]:\n", indent, "", i);
    PrintOpensshKey(&key->keys[i], indent + 2);
  }
}

int main(int argc, char *argv[]) {
  for (int i = 1; i < argc; ++i) {
    int err;
    char *p;
    size_t n;
    struct OpensshKeyV1 key;
    if (!(p = xslurp(argv[i], &n))) {
      perror(argv[i]);
      exit(1);
    }
    err = ParseOpensshKeyV1(p, n, &key);
    printf("parse %s as ssh private key\n", argv[i]);
    PrintOpensshKeyV1(&key, 2);
    if (err) {
      char msg[64];
      DescribeOpensshParseError(msg, sizeof(msg), err);
      fprintf(stderr, "error: %s\n", msg);
      struct OpensshPublicKey key;
      err = ParseOpensshPublicKey(p, n, &key);
      printf("parse %s as ssh public key\n", argv[i]);
      PrintOpensshPublicKey(&key, 2);
      if (err) {
        char msg[64];
        DescribeOpensshParseError(msg, sizeof(msg), err);
        fprintf(stderr, "error: %s\n", msg);
      }
      DestroyOpensshPublicKey(&key);
    }
    DestroyOpensshKeyV1(&key);
  }
}
