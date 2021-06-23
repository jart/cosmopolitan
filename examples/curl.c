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
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/shut.h"
#include "libc/sysv/consts/sock.h"
#include "libc/x/x.h"
#include "net/http/http.h"
#include "net/http/url.h"

/**
 * @fileoverview Downloads HTTP URL to stdout.
 *
 *     make -j8 o//examples/curl.com
 *     o//examples/curl.com http://justine.lol/ape.html
 */

int main(int argc, char *argv[]) {

  /*
   * Get argument.
   */
  const char *urlarg;
  if (argc != 2) {
    fprintf(stderr, "USAGE: %s URL\n", argv[0]);
    exit(1);
  }
  urlarg = argv[1];

  /*
   * Parse URL.
   */
  struct Url url;
  char *host, *port;
  _gc(ParseUrl(urlarg, -1, &url));
  _gc(url.params.p);
  if (url.scheme.n &&
      !(url.scheme.n == 4 && !memcasecmp(url.scheme.p, "http", 4))) {
    fprintf(stderr, "ERROR: NOT AN HTTP URL: %s\n", urlarg);
    exit(1);
  }
  host = firstnonnull(_gc(strndup(url.host.p, url.host.n)), "127.0.0.1");
  port = url.port.n ? _gc(strndup(url.port.p, url.port.n)) : "80";
  port = _gc(xasprintf("%hu", atoi(port)));
  if (!IsAcceptableHost(host, -1)) {
    fprintf(stderr, "ERROR: INVALID HOST: %s\n", urlarg);
    exit(1);
  }
  url.fragment.p = 0, url.fragment.n = 0;
  url.scheme.p = 0, url.scheme.n = 0;
  url.user.p = 0, url.user.n = 0;
  url.pass.p = 0, url.pass.n = 0;
  url.host.p = 0, url.host.n = 0;
  url.port.p = 0, url.port.n = 0;
  if (!url.path.n || url.path.p[0] != '/') {
    char *p = _gc(xmalloc(1 + url.path.n));
    mempcpy(mempcpy(p, "/", 1), url.path.p, url.path.n);
    url.path.p = p;
    ++url.path.n;
  }

  /*
   * Create HTTP message.
   */
  const char *msg;
  msg = _gc(xasprintf("GET %s HTTP/1.1\r\n"
                      "Host: %s:%s\r\n"
                      "Connection: close\r\n"
                      "Content-Length: 0\r\n"
                      "Accept: text/plain; */*\r\n"
                      "Accept-Encoding: identity\r\n"
                      "User-Agent: github.com/jart/cosmopolitan\r\n"
                      "\r\n",
                      _gc(EncodeUrl(&url, 0)), host, port));

  /*
   * Perform DNS lookup.
   */
  struct addrinfo *addr, *addrs;
  struct addrinfo hints = {.ai_family = AF_INET,
                           .ai_socktype = SOCK_STREAM,
                           .ai_protocol = IPPROTO_TCP,
                           .ai_flags = AI_NUMERICSERV};
  CHECK_EQ(EAI_SUCCESS, getaddrinfo(host, port, &hints, &addrs));
  for (addr = addrs; addr; addr = addr->ai_next) {

    /*
     * Send HTTP Message.
     */
    int sock;
    CHECK_NE(-1, (sock = socket(addr->ai_family, addr->ai_socktype,
                                addr->ai_protocol)));
    CHECK_NE(-1, connect(sock, addr->ai_addr, addr->ai_addrlen));
    CHECK_EQ(strlen(msg), write(sock, msg, strlen(msg)));
    shutdown(sock, SHUT_WR);

    /*
     * Handle response.
     */
    ssize_t rc;
    char buf[1500];
    size_t got, toto;
    unsigned long need;
    const char *msg, *crlfcrlf;
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
