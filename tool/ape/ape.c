/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/dns/dns.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/goodsocket.internal.h"
#include "libc/sock/sock.h"
#include "libc/stdio/append.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/slice.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/sock.h"
#include "net/http/http.h"
#include "net/http/url.h"
#include "net/https/https.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/mbedtls/ctr_drbg.h"
#include "third_party/mbedtls/debug.h"
#include "third_party/mbedtls/error.h"
#include "third_party/mbedtls/iana.h"
#include "third_party/mbedtls/net_sockets.h"
#include "third_party/mbedtls/ssl.h"
#include "third_party/mbedtls/x509.h"

/**
 * @fileoverview Downloads HTTP URL to stdout.
 */

#define HasHeader(H)    (!!msg.headers[H].a)
#define HeaderData(H)   (p + msg.headers[H].a)
#define HeaderLength(H) (msg.headers[H].b - msg.headers[H].a)
#define HeaderEqualCase(H, S) \
  SlicesEqualCase(S, strlen(S), HeaderData(H), HeaderLength(H))

static int sock;
static int outfd;
static const char *prog;
static const char *outpath;

static wontreturn void PrintUsage(int fd, int rc) {
  tinyprint(fd, "usage: ", prog, " [-iksvV] URL\n", NULL);
  exit(rc);
}

static const char *DescribeErrno(void) {
  const char *reason;
  if (!(reason = _strerdoc(errno))) reason = "Unknown error";
  return reason;
}

static int GetSslEntropy(void *c, unsigned char *p, size_t n) {
  if (getrandom(p, n, 0) != n) {
    perror("getrandom");
    exit(1);
  }
  return 0;
}

static void OnSslDebug(void *ctx, int level, const char *file, int line,
                       const char *message) {
  char sline[12];
  char slevel[12];
  FormatInt32(sline, line);
  FormatInt32(slevel, level);
  tinyprint(2, file, ":", sline, ": (", slevel, ") ", message, "\n", NULL);
}

static void WriteOutput(const void *p, size_t n) {
  if (!outfd) {
    if (outpath) {
      if ((outfd = creat(outpath, 0644)) <= 0) {
        perror(outpath);
        exit(1);
      }
    } else {
      outfd = 1;
      outpath = "<stdout>";
    }
  }
  ssize_t rc;
  for (size_t i = 0; i < n; i += rc) {
    rc = write(outfd, p, n);
    if (rc <= 0) {
      perror(outpath);
      exit(1);
    }
  }
}

static int TlsSend(void *c, const unsigned char *p, size_t n) {
  int rc;
  if ((rc = write(*(int *)c, p, n)) == -1) {
    perror("TlsSend");
    exit(1);
  }
  return rc;
}

static int TlsRecv(void *c, unsigned char *p, size_t n, uint32_t o) {
  int r;
  struct iovec v[2];
  static unsigned a, b;
  static unsigned char t[4096];
  if (a < b) {
    r = MIN(n, b - a);
    memcpy(p, t + a, r);
    if ((a += r) == b) {
      a = b = 0;
    }
    return r;
  }
  v[0].iov_base = p;
  v[0].iov_len = n;
  v[1].iov_base = t;
  v[1].iov_len = sizeof(t);
  if ((r = readv(*(int *)c, v, 2)) == -1) {
    perror("TlsRecv");
    exit(1);
  }
  if (r > n) {
    b = r - n;
  }
  return MIN(n, r);
}

int _curl(int argc, char *argv[]) {

  if (!NoDebug()) {
    ShowCrashReports();
  }

  prog = argv[0];
  if (!prog) {
    prog = "curl";
  }

  /*
   * Read flags.
   */
  int opt;
  while ((opt = getopt(argc, argv, "hV")) != -1) {
    switch (opt) {
      case 'V':
        ++mbedtls_debug_threshold;
        break;
      case 'h':
        PrintUsage(1, 0);
      default:
        PrintUsage(2, 1);
    }
  }

  /*
   * Get argument.
   */
  const char *urlarg;
  if (optind == argc) {
    tinyprint(2, prog, ": missing url\n", NULL);
    PrintUsage(2, 1);
  }
  urlarg = argv[optind];

  /*
   * Parse URL.
   */
  struct Url url;
  char *host, *port;
  gc(ParseUrl(urlarg, -1, &url, kUrlPlus));
  gc(url.params.p);
  if (url.host.n) {
    if (url.scheme.n &&
        !(url.scheme.n == 5 && !memcasecmp(url.scheme.p, "https", 5))) {
      tinyprint(2, prog, ": not an https url: ", urlarg, "\n", NULL);
      exit(1);
    }
    host = gc(strndup(url.host.p, url.host.n));
    if (url.port.n) {
      port = gc(strndup(url.port.p, url.port.n));
    } else {
      port = "443";
    }
  } else {
    host = "127.0.0.1";
    port = "443";
  }
  if (!IsAcceptableHost(host, -1)) {
    tinyprint(2, prog, ": invalid host: ", urlarg, "\n", NULL);
    exit(1);
  }
  url.fragment.p = 0, url.fragment.n = 0;
  url.scheme.p = 0, url.scheme.n = 0;
  url.user.p = 0, url.user.n = 0;
  url.pass.p = 0, url.pass.n = 0;
  url.host.p = 0, url.host.n = 0;
  url.port.p = 0, url.port.n = 0;
  if (!url.path.n || url.path.p[0] != '/') {
    char *p = gc(malloc(1 + url.path.n));
    mempcpy(mempcpy(p, "/", 1), url.path.p, url.path.n);
    url.path.p = p;
    ++url.path.n;
  }

  char *request = 0;
  appendf(&request,
          "GET %s HTTP/1.1\r\n"
          "Connection: close\r\n"
          "User-Agent: ape/1.1\r\n"
          "Host: %s:%d\r\n"
          "\r\n",
          gc(EncodeUrl(&url, 0)), host, port);

  /*
   * Perform DNS lookup.
   */
  struct addrinfo *addr;
  struct addrinfo hints = {.ai_family = AF_UNSPEC,
                           .ai_socktype = SOCK_STREAM,
                           .ai_protocol = IPPROTO_TCP,
                           .ai_flags = AI_NUMERICSERV};
  if (getaddrinfo(host, port, &hints, &addr) != EAI_SUCCESS) {
    tinyprint(2, prog, ": could not resolve host: ", host, "\n", NULL);
    exit(1);
  }

  /*
   * Connect to server.
   */
  int ret;
  if ((sock = GoodSocket(addr->ai_family, addr->ai_socktype, addr->ai_protocol,
                         false, &(struct timeval){-60})) == -1) {
    perror("socket");
    exit(1);
  }
  if (connect(sock, addr->ai_addr, addr->ai_addrlen)) {
    tinyprint(2, prog, ": failed to connect to ", host, " port ", port, ": ",
              DescribeErrno(), "\n", NULL);
    exit(1);
  }
  freeaddrinfo(addr);

  /*
   * Setup crypto.
   */
  mbedtls_ssl_config conf;
  mbedtls_ssl_context ssl;
  mbedtls_ctr_drbg_context drbg;
  mbedtls_ssl_init(&ssl);
  mbedtls_ctr_drbg_init(&drbg);
  mbedtls_ssl_config_init(&conf);
  unassert(!mbedtls_ctr_drbg_seed(&drbg, GetSslEntropy, 0, "justine", 7));
  unassert(!mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT,
                                        MBEDTLS_SSL_TRANSPORT_STREAM,
                                        MBEDTLS_SSL_PRESET_DEFAULT));
  mbedtls_ssl_conf_ca_chain(&conf, GetSslRoots(), 0);
  mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &drbg);
#ifndef NDEBUG
  mbedtls_ssl_conf_dbg(&conf, OnSslDebug, 0);
#endif
  unassert(!mbedtls_ssl_setup(&ssl, &conf));
  unassert(!mbedtls_ssl_set_hostname(&ssl, host));
  mbedtls_ssl_set_bio(&ssl, &sock, TlsSend, 0, TlsRecv);
  if ((ret = mbedtls_ssl_handshake(&ssl))) {
    tinyprint(2, prog, ": ssl negotiation with ", host,
              " failed: ", DescribeSslClientHandshakeError(&ssl, ret), "\n",
              NULL);
    exit(1);
  }

  /*
   * Send HTTP Message.
   */
  ssize_t rc;
  size_t i, n;
  n = appendz(request).i;
  for (i = 0; i < n; i += rc) {
    rc = mbedtls_ssl_write(&ssl, request + i, n - i);
    if (rc <= 0) {
      tinyprint(2, prog, ": ssl send failed: ", DescribeMbedtlsErrorCode(rc),
                "\n", NULL);
      exit(1);
    }
  }

  /*
   * Handle response.
   */
  int t;
  char *p;
  ssize_t paylen;
  size_t g, hdrlen;
  struct HttpMessage msg;
  struct HttpUnchunker u;
  InitHttpMessage(&msg, kHttpResponse);
  for (p = 0, hdrlen = paylen = t = i = n = 0;;) {
    if (i == n) {
      n += 1000;
      n += n >> 1;
      p = realloc(p, n);
    }
    if ((rc = mbedtls_ssl_read(&ssl, p + i, n - i)) < 0) {
      if (rc == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
        rc = 0;
      } else {
        tinyprint(2, prog, ": recv failed: ", DescribeMbedtlsErrorCode(rc),
                  "\n", NULL);
        exit(1);
      }
    }
    g = rc;
    i += g;
    switch (t) {
      case kHttpClientStateHeaders:
        unassert(g);
        if ((rc = ParseHttpMessage(&msg, p, i)) == -1) {
          tinyprint(2, prog, ": ", host, " sent bad http message\n", NULL);
          exit(1);
        }
        if (rc) {
          hdrlen = rc;
          if (100 <= msg.status && msg.status <= 199) {
            DestroyHttpMessage(&msg);
            InitHttpMessage(&msg, kHttpResponse);
            memmove(p, p + hdrlen, i - hdrlen);
            i -= hdrlen;
            break;
          }
          if (msg.status == 204 || msg.status == 304) {
            goto Finished;
          }
          if (!HasHeader(kHttpContentLength) ||
              (paylen = ParseContentLength(HeaderData(kHttpContentLength),
                                           HeaderLength(kHttpContentLength))) ==
                  -1 ||
              (HasHeader(kHttpTransferEncoding) &&
               !HeaderEqualCase(kHttpTransferEncoding, "identity"))) {
            tinyprint(2, prog, ": ", host, " sent bad transfer\n", NULL);
            exit(1);
          }
          t = kHttpClientStateBodyLengthed;
          if (paylen > i - hdrlen) {
            WriteOutput(p + hdrlen, i - hdrlen);
          } else {
            WriteOutput(p + hdrlen, paylen);
            goto Finished;
          }
        }
        break;
      case kHttpClientStateBodyLengthed:
        unassert(g);
        if (i - hdrlen > paylen) {
          g = hdrlen + paylen - (i - g);
        }
        WriteOutput(p + i - g, g);
        if (i - hdrlen >= paylen) {
          goto Finished;
        }
        break;
      default:
        abort();
    }
  }

/*
 * Close connection.
 */
Finished:
  if (close(sock)) {
    tinyprint(2, prog, ": close failed: ", DescribeErrno(), "\n", NULL);
    exit(1);
  }

  /*
   * Free memory.
   */
  mbedtls_ssl_free(&ssl);
  mbedtls_ctr_drbg_free(&drbg);
  mbedtls_ssl_config_free(&conf);
  mbedtls_ctr_drbg_free(&drbg);
  free(p);

  return 0;
}
