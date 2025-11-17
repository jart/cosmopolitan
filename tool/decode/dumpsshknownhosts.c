#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justine Tunney has waived      │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "libc/ctype.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/x/x.h"
#include "libc/x/xasprintf.h"
#include "net/http/ssh.h"
#include "third_party/haclstar/haclstar.h"

void PrintOpensshPublicKey(struct OpensshPublicKey *obj, int indent) {
  printf("%*salgo: %`#s\n", indent, "", obj->algo);
  printf("%*ssize: %zu\n", indent, "", obj->size);
  printf("%*sdata: %`#.*s\n", indent, "", (int)obj->size, obj->data);
  printf("%*scomment: %`#s\n", indent, "", obj->comment);
}

void PrintOpensshKnownHost(struct OpensshKnownHost *obj, int indent) {
  printf("%*shost: %`#s\n", indent, "", obj->host);
  printf("%*ssalt: %`#.*s\n", indent, "", 20, obj->salt);
  printf("%*shash: %`#.*s\n", indent, "", 20, obj->hash);
  PrintOpensshPublicKey(&obj->pub, indent + 2);
}

void PrintOpensshKnownHosts(struct OpensshKnownHosts *obj, int indent) {
  printf("%*sn: %zu\n", indent, "", obj->n);
  printf("%*sc: %zu\n", indent, "", obj->c);
  for (size_t i = 0; i < obj->n; ++i) {
    printf("%*sp[%zu]:\n", indent, "", i);
    PrintOpensshKnownHost(&obj->p[i], indent + 2);
  }
}

bool OpensshKnownHostMatches(struct OpensshKnownHost *obj, const char *host) {
  if (obj->host)
    return !strcasecmp(obj->host, host);
  char *host2 = strdup(host);
  for (size_t i = 0; host2[i]; ++i)
    if (isascii(host2[i]))
      host2[i] = tolower(host2[i]);
  uint8_t hash[20];
  size_t n = strlen(host2);
  Hacl_HMAC_compute_sha1(hash, obj->salt, 20, (const uint8_t *)host2, n);
  bool res = !memcmp(hash, obj->hash, 20);
  free(host2);
  return res;
}

int main(int argc, char *argv[]) {

  // read file
  const char *path = xasprintf("%s/.ssh/known_hosts", getenv("HOME"));
  size_t datalen;
  char *data;
  if (!(data = xslurp(path, &datalen))) {
    perror(path);
    exit(1);
  }

  // parse file
  int err;
  struct OpensshKnownHosts known_hosts;
  if ((err = ParseOpensshKnownHosts(data, datalen, &known_hosts))) {
    char msg[64];
    DescribeOpensshParseError(msg, sizeof(msg), err);
    fprintf(stderr, "error: %s\n", msg);
    exit(1);
  }

  // search for hostnames if arguments are specified
  if (argc > 1) {
    for (int i = 1; i < argc; ++i) {
      const char *host = argv[i];
      printf("%s:\n", host);
      for (size_t i = 0; i < known_hosts.n; ++i) {
        if (OpensshKnownHostMatches(&known_hosts.p[i], host)) {
          printf("  known_hosts[%zu]:\n", i);
          PrintOpensshKnownHost(&known_hosts.p[i], 4);
        }
      }
    }
    exit(0);
  }

  // print content
  PrintOpensshKnownHosts(&known_hosts, 0);
}
