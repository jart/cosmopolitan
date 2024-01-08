/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/serialize.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "net/http/http.h"
#include "net/http/ip.h"
#include "third_party/maxmind/maxminddb.h"

#define PATH(...) \
  (const char *const[]) { __VA_ARGS__, 0 }

MMDB_s *ipdb, *asdb;

int PrintIpInfo(const char *ipstr) {
  int error;
  bool uhoh;
  int64_t ip;
  uint16_t cc;
  MMDB_entry_data_s e, f;
  MMDB_lookup_result_s r;
  if ((ip = ParseIp(ipstr, -1)) == -1) {
    fprintf(stderr, "BAD IP: %s\n", ipstr);
    return -1;
  }
  r = MMDB_lookup(ipdb, ip, &error);
  CHECK_EQ(MMDB_SUCCESS, error);
  if (!r.found_entry) {
    fprintf(stderr, "NOT FOUND: %s\n", ipstr);
    return -1;
  }
  printf("%hhu.%hhu.%hhu.%hhu\n", ip >> 24, ip >> 16, ip >> 8, ip);
  if (!MMDB_aget_value(&r.entry, &e, PATH("location", "metro_code"))) {
    printf("%s: %s\n", "metro", GetMetroName(e.uint16));
  }
  if (!MMDB_aget_value(&r.entry, &e, PATH("postal", "code"))) {
    printf("%s: %.*s\n", "postal", e.data_size, e.utf8_string);
  }
  if (!MMDB_aget_value(&r.entry, &e, PATH("city", "names", "en"))) {
    printf("%s: %.*s\n", "city", e.data_size, e.utf8_string);
  }
  if (!MMDB_aget_value(&r.entry, &e, PATH("subdivisions", "0", "iso_code")) &&
      !MMDB_aget_value(&r.entry, &f,
                       PATH("subdivisions", "0", "names", "en"))) {
    printf("%s: %.*s (%.*s)\n", "region", e.data_size, e.utf8_string,
           f.data_size, f.utf8_string);
  }
  if (!MMDB_aget_value(&r.entry, &e, PATH("country", "iso_code")) &&
      !MMDB_aget_value(&r.entry, &f, PATH("country", "names", "en"))) {
    printf("%s: %.*s (%.*s)\n", "country", e.data_size, e.utf8_string,
           f.data_size, f.utf8_string);
    cc = READ16LE(e.utf8_string);
  } else {
    cc = 0;
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
       e.boolean)) {
    printf("%s: %s\n", "is_in_european_union", "true");
  }
  if (!MMDB_aget_value(&r.entry, &e, PATH("continent", "code")) &&
      !MMDB_aget_value(&r.entry, &f, PATH("continent", "names", "en"))) {
    printf("%s: %.*s (%.*s)\n", "continent", e.data_size, e.utf8_string,
           f.data_size, f.utf8_string);
  }
  if (!MMDB_aget_value(&r.entry, &e, PATH("location", "time_zone"))) {
    printf("%s: %.*s\n", "timezone", e.data_size, e.utf8_string);
  }
  if (!MMDB_aget_value(&r.entry, &e, PATH("location", "latitude")) &&
      !MMDB_aget_value(&r.entry, &f, PATH("location", "longitude"))) {
    printf("%s: %g %g\n", "location", e.double_value, f.double_value);
  }
  if (!MMDB_aget_value(&r.entry, &e, PATH("location", "accuracy_radius"))) {
    printf("%s: %dkm\n", "accuracy", e.uint16);
  }
  printf("%s: %s\n", "category", GetIpCategoryName(CategorizeIp(ip)));
  if ((r = MMDB_lookup(asdb, ip, &error)).found_entry &&
      !MMDB_aget_value(&r.entry, &e, PATH("autonomous_system_number")) &&
      !MMDB_aget_value(&r.entry, &f, PATH("autonomous_system_organization"))) {
    printf("%s: AS%u (%.*s)\n", "asn", e.uint32, f.data_size, f.utf8_string);
  }
  return 0;
}

int main(int argc, char *argv[]) {
  int i, rc;
  ipdb = gc(calloc(1, sizeof(MMDB_s)));
  CHECK_EQ(MMDB_SUCCESS,
           MMDB_open("/usr/local/share/maxmind/GeoLite2-City.mmdb", 0, ipdb));
  asdb = gc(calloc(1, sizeof(MMDB_s)));
  CHECK_EQ(MMDB_SUCCESS,
           MMDB_open("/usr/local/share/maxmind/GeoLite2-ASN.mmdb", 0, asdb));
  for (rc = 0, i = 1; i < argc; ++i) {
    if (PrintIpInfo(argv[i]) != -1) {
      if (i + 1 < argc) printf("\n");
    } else {
      fprintf(stderr, "NOT FOUND: %s\n", argv[i]);
      rc = 1;
    }
  }
  MMDB_close(asdb);
  MMDB_close(ipdb);
  return rc;
}
