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
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/inet6.h"
#include "third_party/maxmind/maxminddb.h"

/**
 * @fileoverview MaxMind database dumper
 *
 * Prints the metadata of an mmdb file, followed by every network in
 * its search tree along with the data record associated with it.
 *
 *     make -j o//tool/viz/maxmindview
 *     o//tool/viz/maxmindview /usr/local/share/maxmind/GeoLite2-ASN.mmdb
 */

MMDB_s mmdb;
uint32_t ipv4_node;
uint8_t ip[16];
const char *prog;

static void Die(const char *thing, int error) {
  fprintf(stderr, "%s: %s: %s\n", prog, thing, MMDB_strerror(error));
  exit(1);
}

static void Dump(MMDB_entry_data_list_s *list) {
  int error;
  if ((error = MMDB_dump_entry_data_list(stdout, list, 2)) != MMDB_SUCCESS)
    Die("dump", error);
}

static void PrintMetadata(void) {
  int error;
  MMDB_entry_data_list_s *list;
  if ((error = MMDB_get_metadata_as_entry_data_list(&mmdb, &list)) !=
      MMDB_SUCCESS)
    Die("metadata", error);
  printf("metadata\n");
  Dump(list);
  MMDB_free_entry_data_list(list);
}

static bool IsIpv4Mapped(int depth) {
  // in ipv6 databases the ipv4 address space lives at ::/96
  if (mmdb.metadata.ip_version != 6 || depth < 96)
    return false;
  for (int i = 0; i < 12; ++i)
    if (ip[i])
      return false;
  return true;
}

static void PrintNetwork(int depth) {
  char buf[INET6_ADDRSTRLEN];
  if (mmdb.metadata.ip_version == 4) {
    inet_ntop(AF_INET, ip, buf, sizeof(buf));
    printf("%s/%d\n", buf, depth);
  } else if (IsIpv4Mapped(depth)) {
    inet_ntop(AF_INET, ip + 12, buf, sizeof(buf));
    printf("%s/%d\n", buf, depth - 96);
  } else {
    inet_ntop(AF_INET6, ip, buf, sizeof(buf));
    printf("%s/%d\n", buf, depth);
  }
}

static void PrintRecord(int depth, MMDB_entry_s *entry) {
  int error;
  MMDB_entry_data_list_s *list;
  if ((error = MMDB_get_entry_data_list(entry, &list)) != MMDB_SUCCESS)
    Die("data", error);
  printf("\n");
  PrintNetwork(depth);
  Dump(list);
  MMDB_free_entry_data_list(list);
}

static void Walk(uint32_t node_number, int depth);

static void Visit(uint64_t record, uint8_t type, MMDB_entry_s *entry,
                  int depth) {
  switch (type) {
    case MMDB_RECORD_TYPE_SEARCH_NODE:
      Walk(record, depth);
      break;
    case MMDB_RECORD_TYPE_DATA:
      PrintRecord(depth, entry);
      break;
    case MMDB_RECORD_TYPE_EMPTY:
      break;
    default:
      fprintf(stderr, "%s: invalid record at depth %d\n", prog, depth);
      exit(1);
  }
}

static void Walk(uint32_t node_number, int depth) {
  int error;
  MMDB_search_node_s node;
  // ipv6 databases alias ::ffff:0:0/96 and 2002::/16 to the ipv4
  // subtree, which we skip so ipv4 networks only get printed once
  if (ipv4_node && node_number == ipv4_node && !IsIpv4Mapped(depth))
    return;
  if ((error = MMDB_read_node(&mmdb, node_number, &node)) != MMDB_SUCCESS)
    Die("read node", error);
  ip[depth / 8] &= ~(0x80 >> (depth % 8));
  Visit(node.left_record, node.left_record_type, &node.left_record_entry,
        depth + 1);
  ip[depth / 8] |= 0x80 >> (depth % 8);
  Visit(node.right_record, node.right_record_type, &node.right_record_entry,
        depth + 1);
  ip[depth / 8] &= ~(0x80 >> (depth % 8));
}

static void FindIpv4Node(void) {
  // follows the left branch 96 times to find where ::/96 starts
  uint32_t node_number = 0;
  MMDB_search_node_s node;
  if (mmdb.metadata.ip_version != 6)
    return;
  for (int i = 0; i < 96; ++i) {
    if (MMDB_read_node(&mmdb, node_number, &node) != MMDB_SUCCESS ||
        node.left_record_type != MMDB_RECORD_TYPE_SEARCH_NODE)
      return;
    node_number = node.left_record;
  }
  ipv4_node = node_number;
}

int main(int argc, char *argv[]) {
  int error;
  prog = argv[0];
  if (argc != 2) {
    fprintf(stderr, "usage: %s FILE.mmdb\n", prog);
    return 1;
  }
  if ((error = MMDB_open(argv[1], MMDB_MODE_MMAP, &mmdb)) != MMDB_SUCCESS)
    Die(argv[1], error);
  PrintMetadata();
  FindIpv4Node();
  Walk(0, 0);
  MMDB_close(&mmdb);
  return 0;
}
