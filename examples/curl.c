#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/bits/safemacros.internal.h"
#include "libc/dns/dns.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ai.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/shut.h"
#include "libc/sysv/consts/sock.h"
#include "libc/x/x.h"
#include "net/http/uri.h"

/**
 * @fileoverview Downloads HTTP URL to stdout.
 *
 *     make -j8 o//examples/curl.com
 *     o//examples/curl.com http://justine.lol/ape.html
 */

int main(int argc, char *argv[]) {
  int sock;
  ssize_t rc;
  unsigned long need;
  struct UriSlice path;
  size_t i, got, toto, msglen;
  char buf[1500], host[256], port[7];
  const char *url, *msg, *pathstr, *crlfcrlf, *contentlength;
  struct UriSlice us[16];
  struct Uri u = {.segs.p = us, .segs.n = ARRAYLEN(us)};
  struct addrinfo *addr, *addrs;
  struct addrinfo hints = {.ai_family = AF_INET,
                           .ai_socktype = SOCK_STREAM,
                           .ai_protocol = IPPROTO_TCP,
                           .ai_flags = AI_NUMERICSERV};
  if (argc != 2) {
    fprintf(stderr, "USAGE: %s URL\n", argv[0]);
    exit(1);
  }
  url = argv[1];
  CHECK_NE(-1, uriparse(&u, url, strlen(url)), "BAD URL: %`'s", url);
  CHECK_EQ(kUriSchemeHttp, urischeme(u.scheme, url));
  urislice2cstr(host, sizeof(host), u.host, url, "127.0.0.1");
  urislice2cstr(port, sizeof(port), u.port, url, "80");
  path = uripath(&u);
  pathstr = path.n ? url + path.i : "/";
  msg = _gc(xstrcat("GET ", pathstr,
                    " HTTP/1.1\r\n"
                    "Host: ",
                    host,
                    "\r\n"
                    "Connection: close\r\n"
                    "Content-Length: 0\r\n"
                    "Accept: text/plain; */*\r\n"
                    "Accept-Encoding: identity\r\n"
                    "User-Agent: github.com/jart/cosmopolitan\r\n"
                    "\r\n"));
  msglen = strlen(msg);
  CHECK_EQ(EAI_SUCCESS, getaddrinfo(host, port, &hints, &addrs));
  for (addr = addrs; addr; addr = addr->ai_next) {
    CHECK_NE(-1, (sock = socket(addr->ai_family, addr->ai_socktype,
                                addr->ai_protocol)));
    CHECK_NE(-1, connect(sock, addr->ai_addr, addr->ai_addrlen));
    CHECK_EQ(msglen, write(sock, msg, msglen));
    shutdown(sock, SHUT_WR);
    buf[0] = '\0';
    CHECK_NE(-1, (rc = read(sock, buf, sizeof(buf))));
    got = rc;
    CHECK(startswith(buf, "HTTP/1.1 200"), "%`'.*s", got, buf);
    CHECK_NOTNULL((crlfcrlf = memmem(buf, got, "\r\n\r\n", 4)));
    need = strtol((char *)firstnonnull(
                      memmem(buf, crlfcrlf - buf, "\r\nContent-Length: ", 18),
                      firstnonnull(memmem(buf, crlfcrlf - buf,
                                          "\r\ncontent-length: ", 18),
                                   "\r\nContent-Length: -1")) +
                      18,
                  NULL, 10);
    got = MIN(got - (crlfcrlf + 4 - buf), need);
    CHECK_EQ(got, write(1, crlfcrlf + 4, got));
    for (toto = got; toto < need; toto += got) {
      CHECK_NE(-1, (rc = read(sock, buf, sizeof(buf))));
      if (!(got = rc)) exit(18);
      got = MIN(got, need - toto);
      CHECK_EQ(got, write(1, buf, got));
    }
    close(sock);
    break;
  }
  return 0;
}
