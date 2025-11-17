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
#include "net/https/fetch.h"
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/likely.h"
#include "libc/macros.h"
#include "libc/serialize.h"
#include "libc/sock/sock.h"
#include "libc/stdio/append.h"
#include "libc/stdio/rand.h"
#include "libc/str/slice.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"
#include "libc/sysv/consts/tcp.h"
#include "net/http/http.h"
#include "net/http/url.h"
#include "net/https/https.h"
#include "third_party/mbedtls/aes.h"
#include "third_party/mbedtls/ctr_drbg.h"
#include "third_party/mbedtls/ssl.h"
#include "third_party/musl/netdb.h"

#define FAILNEG1(x)          \
  do {                       \
    if (UNLIKELY((x) == -1)) \
      goto OnFailure;        \
  } while (0)

#define FAILZERO(x)     \
  do {                  \
    if (UNLIKELY(!(x))) \
      goto OnFailure;   \
  } while (0)

#define HasHeader(H)    (!!msg.headers[H].a)
#define HeaderData(H)   (p + msg.headers[H].a)
#define HeaderLength(H) (msg.headers[H].b - msg.headers[H].a)
#define HeaderEqualCase(H, S) \
  SlicesEqualCase(S, strlen(S), HeaderData(H), HeaderLength(H))

struct Fetch {
  int sock;
};

static bool Tune(int fd, int a, int b, int x) {
  if (!b)
    return false;
  return setsockopt(fd, a, b, &x, sizeof(x)) != -1;
}

static int Socket(int family, int type, int protocol) {
  int fd;
  if ((fd = socket(family, type, protocol)) != -1) {
    Tune(fd, SOL_TCP, TCP_FASTOPEN_CONNECT, 1);
    Tune(fd, SOL_TCP, TCP_QUICKACK, 1);
    Tune(fd, SOL_TCP, TCP_NODELAY, 1);
  }
  return fd;
}

static int GetSslEntropy(void *c, unsigned char *p, size_t n) {
  if (getentropy(p, n))
    notpossible;
  return 0;
}

static int TlsSend(void *c, const unsigned char *p, size_t n) {
  ssize_t rc;
  struct Fetch *f = c;
  if ((rc = write(f->sock, p, n)) == -1)
    return -errno;
  return rc;
}

static int TlsRecv(void *c, unsigned char *p, size_t n, uint32_t waitms) {
  ssize_t rc;
  struct Fetch *f = c;
  if ((rc = read(f->sock, p, n)) == -1)
    return -errno;
  return rc;
}

static int PerformFetch(char **response_data, const char *urlarg, int tries) {
  if (tries == 10) {
    errno = ELOOP;
    return -1;
  }

  int retcode;
  uint64_t method = kHttpGet;
  struct Fetch f = {.sock = -1};
  struct Url url = {0};
  char *host = 0;
  char *port = 0;
  char *urldata = 0;
  char *request = 0;
  char *funpath = 0;
  bool usessl = false;
  char *url2 = 0;
  char *p = 0;

  struct HttpMessage msg;
  InitHttpMessage(&msg, kHttpResponse);

  mbedtls_ssl_config conf;
  mbedtls_ssl_context ssl;
  mbedtls_ctr_drbg_context drbg;
  mbedtls_ssl_init(&ssl);
  mbedtls_ctr_drbg_init(&drbg);
  mbedtls_ssl_config_init(&conf);

  if (!(urldata = ParseUrl(urlarg, -1, &url, kUrlPlus)))
    goto BadParameter;
  if (url.scheme.n) {
    if (url.scheme.n == 5 && !memcasecmp(url.scheme.p, "https", 5)) {
      usessl = true;
    } else if (!(url.scheme.n == 4 && !memcasecmp(url.scheme.p, "http", 4))) {
      goto BadParameter;
    }
  }
  if (url.host.n) {
    FAILZERO((host = strndup(url.host.p, url.host.n)));
    if (url.port.n) {
      FAILZERO((port = strndup(url.port.p, url.port.n)));
    } else {
      FAILZERO((port = strdup(usessl ? "443" : "80")));
    }
  } else {
    FAILZERO((host = strdup("127.0.0.1")));
    FAILZERO((port = strdup(usessl ? "443" : "80")));
  }
  url.fragment.p = 0, url.fragment.n = 0;
  url.scheme.p = 0, url.scheme.n = 0;
  url.user.p = 0, url.user.n = 0;
  url.pass.p = 0, url.pass.n = 0;
  url.host.p = 0, url.host.n = 0;
  url.port.p = 0, url.port.n = 0;
  if (!url.path.n || url.path.p[0] != '/') {
    FAILZERO((funpath = malloc(1 + url.path.n)));
    mempcpy(mempcpy(funpath, "/", 1), url.path.p, url.path.n);
    url.path.p = funpath;
    ++url.path.n;
  }

  // build message
  char methodstr[9] = {0};
  WRITE64LE(methodstr, method);
  FAILNEG1(appends(&request, methodstr));
  FAILNEG1(appends(&request, " "));
  char *encoded_url;
  FAILZERO((encoded_url = EncodeUrl(&url, 0)));
  FAILNEG1(appends(&request, encoded_url));
  free(encoded_url);
  FAILNEG1(appends(&request, " HTTP/1.1\r\n"
                             "Connection: close\r\n"));

  // append host header
  FAILNEG1(appends(&request, "Host: "));
  FAILNEG1(appends(&request, host));
  FAILNEG1(appendw(&request, ':'));
  FAILNEG1(appends(&request, port));
  FAILNEG1(appends(&request, "\r\n"));

  // finish headers
  FAILNEG1(appends(&request, "\r\n"));

  // initialize crypto
  if (usessl) {
    if (mbedtls_ctr_drbg_seed(&drbg, GetSslEntropy, 0, "justine", 7))
      goto OutOfMemory;
    mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT,
                                MBEDTLS_SSL_TRANSPORT_STREAM,
                                MBEDTLS_SSL_PRESET_DEFAULT);
    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_ca_chain(&conf, GetSslRoots(), 0);
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &drbg);
    if (mbedtls_ssl_setup(&ssl, &conf))
      goto OutOfMemory;
    if (mbedtls_ssl_set_hostname(&ssl, host))
      goto BadParameter;
  }

  // perform dns lookup
  struct addrinfo *addr;
  struct addrinfo hints = {.ai_family = AF_UNSPEC,
                           .ai_socktype = SOCK_STREAM,
                           .ai_protocol = IPPROTO_TCP,
                           .ai_flags = AI_NUMERICSERV};
  if (getaddrinfo(host, port, &hints, &addr)) {
    errno = ENETUNREACH;
    goto OnFailure;
  }

  // connect to server
  if ((f.sock = Socket(addr->ai_family, addr->ai_socktype,
                       addr->ai_protocol)) == -1 ||
      connect(f.sock, addr->ai_addr, addr->ai_addrlen)) {
    freeaddrinfo(addr);
    goto OnFailure;
  }
  freeaddrinfo(addr);

  // secure connection
  if (usessl) {
    mbedtls_ssl_set_bio(&ssl, &f.sock, TlsSend, 0, TlsRecv);
    if (mbedtls_ssl_handshake(&ssl))
      goto AccessDenied;
  }

  // send message
  ssize_t rc;
  size_t i, n;
  n = appendz(request).i;
  for (i = 0; i < n; i += rc) {
    if (usessl) {
      rc = mbedtls_ssl_write(&ssl, request + i, n - i);
      if (rc <= 0)
        goto OnFailure;
    } else {
      rc = write(f.sock, request + i, n - i);
      if (rc <= 0)
        goto OnFailure;
    }
  }

  // handle response
  int t;
  struct HttpUnchunker u;
  size_t g, hdrlen, paylen;
  FAILZERO((p = malloc((n = 32768))));
  for (hdrlen = paylen = t = i = 0;;) {
    if (i >= n) {
      n = i + 4096;
      char *p2;
      FAILZERO((p2 = realloc(p, n)));
      p = p2;
    }
    if (usessl) {
      if ((rc = mbedtls_ssl_read(&ssl, p + i, n - i)) < 0) {
        if (rc == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
          rc = 0;
        } else {
          goto OnFailure;
        }
      }
    } else {
      FAILNEG1((rc = read(f.sock, p + i, n - i)));
    }
    g = rc;
    i += g;
    switch (t) {
      case kHttpClientStateHeaders:
        if ((rc = ParseHttpMessage(&msg, p, i, n)) == -1)
          goto BadMessage;
        if (rc) {
          hdrlen = rc;

          // handle provisional responses
          if (100 <= msg.status && msg.status <= 199) {
            DestroyHttpMessage(&msg);
            InitHttpMessage(&msg, kHttpResponse);
            memmove(p, p + hdrlen, i - hdrlen);
            i -= hdrlen;
            break;
          }

          // handle redirects
          if (300 <= msg.status && msg.status <= 399) {
            if (!HasHeader(kHttpLocation))
              goto BadMessage;
            if (HeaderLength(kHttpLocation) &&
                HeaderData(kHttpLocation)[0] == '/') {
              FAILNEG1(appends(&url2, usessl ? "https://" : "http://"));
              FAILNEG1(appends(&url2, host));
              FAILNEG1(appends(&url2, ":"));
              FAILNEG1(appends(&url2, port));
            }
            FAILNEG1(appendd(&url2, HeaderData(kHttpLocation),
                             HeaderLength(kHttpLocation)));
            close(f.sock);
            f.sock = -1;
            free(p);
            p = 0;
            retcode = PerformFetch(response_data, url2, tries + 1);
            if (retcode == -1 && errno == EINVAL)
              errno = EBADMSG;
            goto OnReturn;
          }

          // handle empty responses
          if (method == kHttpHead || msg.status == 204 || msg.status == 304)
            goto Finished;

          // handle content
          if (HasHeader(kHttpTransferEncoding) &&
              !HeaderEqualCase(kHttpTransferEncoding, "identity")) {
            if (!HeaderEqualCase(kHttpTransferEncoding, "chunked"))
              goto BadMessage;
            t = kHttpClientStateBodyChunked;
            memset(&u, 0, sizeof(u));
            goto Chunked;
          } else if (HasHeader(kHttpContentLength)) {
            if ((rc = ParseContentLength(HeaderData(kHttpContentLength),
                                         HeaderLength(kHttpContentLength))) ==
                -1)
              goto BadMessage;
            t = kHttpClientStateBodyLengthed;
            paylen = rc;
            if (paylen > i - hdrlen) {
              FAILNEG1(appendd(response_data, p + hdrlen, i - hdrlen));
            } else {
              FAILNEG1(appendd(response_data, p + hdrlen, paylen));
              goto Finished;
            }
          } else {
            t = kHttpClientStateBody;
            FAILNEG1(appendd(response_data, p + hdrlen, i - hdrlen));
          }
        }
        break;
      case kHttpClientStateBody:
        FAILNEG1(appendd(response_data, p + i - g, g));
        if (!g)
          goto Finished;
        break;
      case kHttpClientStateBodyLengthed:
        if (i - hdrlen > paylen)
          g = hdrlen + paylen - (i - g);
        FAILNEG1(appendd(response_data, p + i - g, g));
        if (i - hdrlen >= paylen)
          goto Finished;
        break;
      case kHttpClientStateBodyChunked:
      Chunked:
        if ((rc = Unchunk(&u, p + hdrlen, i - hdrlen, &paylen)) == -1)
          goto BadMessage;
        if (rc) {
          FAILNEG1(appendd(response_data, p + hdrlen, paylen));
          goto Finished;
        }
        break;
      default:
        goto BadMessage;
    }
  }

  // we're done
Finished:
  retcode = msg.status;
OnReturn:
  if (f.sock != -1)
    close(f.sock);
  free(p);
  free(url2);
  free(host);
  free(port);
  free(urldata);
  free(request);
  free(funpath);
  free(url.params.p);
  mbedtls_ssl_free(&ssl);
  mbedtls_ssl_config_free(&conf);
  mbedtls_ctr_drbg_free(&drbg);
  DestroyHttpMessage(&msg);
  return retcode;
OnFailure:
  retcode = -1;
  goto OnReturn;
OutOfMemory:
  errno = ENOMEM;
  goto OnFailure;
BadParameter:
  errno = EINVAL;
  goto OnFailure;
BadMessage:
  errno = EBADMSG;
  goto OnFailure;
AccessDenied:
  errno = EPERM;
  goto OnFailure;
}

/**
 * Appends content at URL to `response_data`.
 *
 * - http and https URLs are supported.
 * - Chunked responses will be decoded.
 * - 30x redirect codes will be followed.
 *
 * @return http status code on success, or -1 w/ errno
 * @raise EINVAL if URL parsing failed
 * @raise EPERM if SSL handshake failed
 * @raise ENOMEM if we ran out of memory
 * @raise ENETUNREACH if DNS lookup failed
 * @raise EBADMSG if HTTP message parsing broke
 * @raise ELOOP if more than ten redirects happened
 */
int AppendFetch(char **response_data, const char *url) {
  int sav = errno;
  int res = PerformFetch(response_data, url, 0);
  if (res != -1)
    errno = sav;
  return res;
}
