/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/alg/arraylist.internal.h"
#include "libc/dns/dns.h"
#include "libc/dns/resolvconf.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/inaddr.h"

/**
 * Parses /etc/resolv.conf file.
 *
 * The content of the file usually looks like this:
 *
 *     nameserver 8.8.8.8
 *     nameserver 8.8.4.4
 *
 * @param resolv points to a ResolvConf object, which should be zero
 *     initialized by the caller; or if it already contains items,
 *     this function will append
 * @param f is an open stream with file content
 * @return number of nameservers appended, or -1 w/ errno
 */
int parseresolvconf(struct ResolvConf *resolv, struct FILE *f) {
  /* TODO(jart): options ndots:5 */
  int rc;
  char *line;
  size_t linesize;
  struct sockaddr_in nameserver;
  char *directive, *value, *tok, *comment;
  rc = 0;
  line = NULL;
  linesize = 0;
  nameserver.sin_family = AF_INET;
  nameserver.sin_port = htons(DNS_PORT);
  while (getline(&line, &linesize, f) != -1) {
    if ((comment = strchr(line, '#'))) *comment = '\0';
    if ((directive = strtok_r(line, " \t\r\n\v", &tok)) &&
        (value = strtok_r(NULL, " \t\r\n\v", &tok))) {
      if ((strcmp(directive, "nameserver") == 0 &&
           inet_pton(AF_INET, value, &nameserver.sin_addr.s_addr) == 1)) {
        if (append(&resolv->nameservers, &nameserver) != -1) ++rc;
      }
    }
  }
  free(line);
  return rc | ferror(f);
}
