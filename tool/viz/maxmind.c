/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2026 Justin Alexander Roberts Tunney                               │
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
#include "libc/cosmo.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/serialize.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/sock.h"
#include "net/http/http.h"
#include "net/http/ip.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/maxmind/maxminddb.h"
#include "third_party/musl/netdb.h"

#define PATH(...)         \
  (const char *const[]) { \
    __VA_ARGS__, 0        \
  }

int ai_family = AF_UNSPEC;
MMDB_s ipdb, asdb;

struct Found {
  struct Found *next;
  char *name;
};

bool AddFound(struct Found **list, const char *name) {
  struct Found *e;
  for (e = *list; e; e = e->next)
    if (!strcmp(e->name, name))
      return false;
  if (!(e = malloc(sizeof(struct Found))))
    return false;
  if (!(e->name = strdup(name))) {
    free(e);
    return false;
  }
  e->next = *list;
  *list = e;
  return true;
}

void FreeFound(struct Found *list) {
  struct Found *e;
  while (list) {
    e = list->next;
    free(list->name);
    free(list);
    list = e;
  }
}

void PrintIpInfoAddr(const char *query, struct Found **found,
                     struct addrinfo *ai) {

  // turn addrinfo into ip
  char host[NI_MAXHOST];
  char serv[NI_MAXSERV];
  int gai_status =
      getnameinfo(ai->ai_addr, ai->ai_addrlen, host, sizeof(host), serv,
                  sizeof(serv), NI_NUMERICHOST | NI_NUMERICSERV);
  if (gai_status) {
    fprintf(stderr, "%s: %s\n", query, gai_strerror(gai_status));
    return;
  }

  // deduplicate results
  if (!AddFound(found, host))
    return;

  int error;
  MMDB_lookup_result_s r = MMDB_lookup_sockaddr(&ipdb, ai->ai_addr, &error);
  if (error != MMDB_SUCCESS) {
    fprintf(stderr, "%s: %s: %s\n", query, host, MMDB_strerror(error));
    return;
  }
  if (!r.found_entry) {
    fprintf(stderr, "%s: %s: not found\n", query, host);
    return;
  }

  printf("%s\n", host);

  bool uhoh;
  uint16_t cc = 0;
  MMDB_entry_data_s e, f;
  if (!MMDB_aget_value(&r.entry, &e, PATH("location", "metro_code")))
    printf("%s: %s\n", "metro", GetMetroName(e.uint16));
  if (!MMDB_aget_value(&r.entry, &e, PATH("postal", "code")))
    printf("%s: %.*s\n", "postal", e.data_size, e.utf8_string);
  if (!MMDB_aget_value(&r.entry, &e, PATH("city", "names", "en")))
    printf("%s: %.*s\n", "city", e.data_size, e.utf8_string);
  if (!MMDB_aget_value(&r.entry, &e, PATH("subdivisions", "0", "iso_code")) &&
      !MMDB_aget_value(&r.entry, &f, PATH("subdivisions", "0", "names", "en")))
    printf("%s: %.*s (%.*s)\n", "region", e.data_size, e.utf8_string,
           f.data_size, f.utf8_string);
  if (!MMDB_aget_value(&r.entry, &e, PATH("country", "iso_code")) &&
      !MMDB_aget_value(&r.entry, &f, PATH("country", "names", "en"))) {
    printf("%s: %.*s (%.*s)\n", "country", e.data_size, e.utf8_string,
           f.data_size, f.utf8_string);
    cc = READ16LE(e.utf8_string);
  }
  if (!MMDB_aget_value(&r.entry, &e, PATH("registered_country", "iso_code")) &&
      !MMDB_aget_value(&r.entry, &f,
                       PATH("registered_country", "names", "en"))) {
    uhoh = cc && cc != READ16LE(e.utf8_string);
    printf("%s%s: %.*s (%.*s)%s\n", uhoh ? "\e[1m" : "", "registered",
           e.data_size, e.utf8_string, f.data_size, f.utf8_string,
           uhoh ? "\e[0m" : "");
  }
  if ((!MMDB_aget_value(&r.entry, &e,
                        PATH("country", "is_in_european_union")) &&
       e.boolean) ||
      (!MMDB_aget_value(&r.entry, &e,
                        PATH("registered_country", "is_in_european_union")) &&
       e.boolean))
    printf("%s: %s\n", "is_in_european_union", "true");
  if (!MMDB_aget_value(&r.entry, &e, PATH("continent", "code")) &&
      !MMDB_aget_value(&r.entry, &f, PATH("continent", "names", "en")))
    printf("%s: %.*s (%.*s)\n", "continent", e.data_size, e.utf8_string,
           f.data_size, f.utf8_string);
  if (!MMDB_aget_value(&r.entry, &e, PATH("location", "time_zone")))
    printf("%s: %.*s\n", "timezone", e.data_size, e.utf8_string);
  if (!MMDB_aget_value(&r.entry, &e, PATH("location", "latitude")) &&
      !MMDB_aget_value(&r.entry, &f, PATH("location", "longitude")))
    printf("%s: %g %g\n", "location", e.double_value, f.double_value);
  if (!MMDB_aget_value(&r.entry, &e, PATH("location", "accuracy_radius")))
    printf("%s: %dkm\n", "accuracy", e.uint16);

  if (ai->ai_addr->sa_family == AF_INET)
    printf("%s: %s\n", "category",
           GetIpCategoryName(CategorizeIp(ntohl(
               ((const struct sockaddr_in *)ai->ai_addr)->sin_addr.s_addr))));

  if ((r = MMDB_lookup_sockaddr(&asdb, ai->ai_addr, &error)).found_entry &&
      !MMDB_aget_value(&r.entry, &e, PATH("autonomous_system_number")) &&
      !MMDB_aget_value(&r.entry, &f, PATH("autonomous_system_organization")))
    printf("%s: AS%u (%.*s)\n", "asn", e.uint32, f.data_size, f.utf8_string);
}

int PrintIpInfo(const char *query) {
  struct Found *found = NULL;
  struct addrinfo *addresses = NULL;
  struct addrinfo hints = {.ai_family = ai_family};
  int gai_status = getaddrinfo(query, NULL, &hints, &addresses);
  if (gai_status) {
    fprintf(stderr, "%s: %s\n", query, gai_strerror(gai_status));
    return -1;
  }
  printf("%s\n", query);
  for (struct addrinfo *ai = addresses; ai; ai = ai->ai_next)
    PrintIpInfoAddr(query, &found, ai);
  freeaddrinfo(addresses);
  FreeFound(found);
  return 0;
}

void OpenMmdb(const char *path, MMDB_s *mmdb) {
  int rc = MMDB_open(path, 0, mmdb);
  if (rc != MMDB_SUCCESS) {
    perror(path);
    exit(1);
  }
}

int main(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "46")) != -1) {
    switch (opt) {
      case '4':
        ai_family = AF_INET;
        break;
      case '6':
        ai_family = AF_INET6;
        break;
      default:
        exit(1);
    }
  }

  int i, rc;
  OpenMmdb("/usr/local/share/maxmind/GeoLite2-City.mmdb", &ipdb);
  OpenMmdb("/usr/local/share/maxmind/GeoLite2-ASN.mmdb", &asdb);
  for (rc = 0, i = optind; i < argc; ++i) {
    if (PrintIpInfo(argv[i]) != -1) {
      if (i + 1 < argc)
        printf("\n");
    } else {
      fprintf(stderr, "NOT FOUND: %s\n", argv[i]);
      rc = 1;
    }
  }

  MMDB_close(&asdb);
  MMDB_close(&ipdb);
  return rc;
}
