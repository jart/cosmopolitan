#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/calls/calls.h"
#include "libc/calls/struct/iovec.h"
#include "libc/dce.h"
#include "libc/dns/dns.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/fmt.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/goodsocket.internal.h"
#include "libc/sock/sock.h"
#include "libc/stdio/append.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/slice.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/dt.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/shut.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"
#include "libc/sysv/consts/tcp.h"
#include "libc/time/struct/tm.h"
#include "libc/x/x.h"
#include "libc/x/xsigaction.h"
#include "net/http/http.h"
#include "net/http/url.h"
#include "net/https/https.h"
#include "net/https/sslcache.h"
#include "third_party/getopt/getopt.h"
#include "third_party/mbedtls/ctr_drbg.h"
#include "third_party/mbedtls/debug.h"
#include "third_party/mbedtls/error.h"
#include "third_party/mbedtls/pk.h"
#include "third_party/mbedtls/ssl.h"
#include "third_party/mbedtls/ssl_ticket.h"
#include "tool/curl/cmd.h"

/**
 * @fileoverview Downloads HTTP URL to stdout.
 */

#define HasHeader(H)    (!!msg.headers[H].a)
#define HeaderData(H)   (p + msg.headers[H].a)
#define HeaderLength(H) (msg.headers[H].b - msg.headers[H].a)
#define HeaderEqualCase(H, S) \
  SlicesEqualCase(S, strlen(S), HeaderData(H), HeaderLength(H))

int sock;

static bool TuneSocket(int fd, int a, int b, int x) {
  if (!b) return false;
  return setsockopt(fd, a, b, &x, sizeof(x)) != -1;
}

static void Write(const void *p, size_t n) {
  ssize_t rc;
  rc = write(1, p, n);
  if (rc == -1 && errno == EPIPE) {
    close(sock);
    exit(128 + SIGPIPE);
  }
  if (rc != n) {
    fprintf(stderr, "write failed: %m\n");
    exit(1);
  }
}

static int TlsSend(void *c, const unsigned char *p, size_t n) {
  int rc;
  NOISEF("begin send %zu", n);
  CHECK_NE(-1, (rc = write(*(int *)c, p, n)));
  NOISEF("end   send %zu", n);
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
    if ((a += r) == b) a = b = 0;
    return r;
  }
  v[0].iov_base = p;
  v[0].iov_len = n;
  v[1].iov_base = t;
  v[1].iov_len = sizeof(t);
  NOISEF("begin recv %zu", n + sizeof(t) - b);
  CHECK_NE(-1, (r = readv(*(int *)c, v, 2)));
  NOISEF("end   recv %zu", r);
  if (r > n) b = r - n;
  return MIN(n, r);
}

static wontreturn void PrintUsage(FILE *f, int rc) {
  fprintf(f, "usage: %s [-iksvV] URL\n", program_invocation_name);
  exit(rc);
}

int _curl(int argc, char *argv[]) {
  if (!NoDebug()) ShowCrashReports();

  /*
   * Read flags.
   */
  int opt;
  struct Headers {
    size_t n;
    char **p;
  } headers = {0};
  int method = 0;
  bool authmode = MBEDTLS_SSL_VERIFY_REQUIRED;
  bool includeheaders = false;
  const char *postdata = NULL;
  const char *agent = "hurl/1.o (https://github.com/jart/cosmopolitan)";
  __log_level = kLogWarn;
  while ((opt = getopt(argc, argv, "qiksvVIX:H:A:d:")) != -1) {
    switch (opt) {
      case 's':
      case 'q':
        break;
      case 'v':
        ++__log_level;
        break;
      case 'i':
        includeheaders = true;
        break;
      case 'I':
        method = kHttpHead;
        break;
      case 'A':
        agent = optarg;
        break;
      case 'H':
        headers.p = realloc(headers.p, ++headers.n * sizeof(*headers.p));
        headers.p[headers.n - 1] = optarg;
        break;
      case 'd':
        postdata = optarg;
        break;
      case 'X':
        CHECK((method = GetHttpMethod(optarg, strlen(optarg))));
        break;
      case 'V':
        ++mbedtls_debug_threshold;
        break;
      case 'k':
        authmode = MBEDTLS_SSL_VERIFY_NONE;
        break;
      case 'h':
        PrintUsage(stdout, EXIT_SUCCESS);
      default:
        PrintUsage(stderr, EX_USAGE);
    }
  }

  /*
   * Get argument.
   */
  const char *urlarg;
  if (optind == argc) PrintUsage(stderr, EX_USAGE);
  urlarg = argv[optind];

  /*
   * Parse URL.
   */
  struct Url url;
  char *host, *port;
  bool usessl = false;
  _gc(ParseUrl(urlarg, -1, &url, kUrlPlus));
  _gc(url.params.p);
  if (url.scheme.n) {
    if (url.scheme.n == 5 && !memcasecmp(url.scheme.p, "https", 5)) {
      usessl = true;
    } else if (!(url.scheme.n == 4 && !memcasecmp(url.scheme.p, "http", 4))) {
      fprintf(stderr, "error: not an http/https url: %s\n", urlarg);
      exit(1);
    }
  }
  if (url.host.n) {
    host = _gc(strndup(url.host.p, url.host.n));
    if (url.port.n) {
      port = _gc(strndup(url.port.p, url.port.n));
    } else {
      port = usessl ? "443" : "80";
    }
  } else {
    host = "127.0.0.1";
    port = usessl ? "443" : "80";
  }
  if (!IsAcceptableHost(host, -1)) {
    fprintf(stderr, "error: invalid host: %s\n", urlarg);
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
  if (!method) method = postdata ? kHttpPost : kHttpGet;

  char *request = 0;
  appendf(&request,
          "%s %s HTTP/1.1\r\n"
          "Connection: close\r\n"
          "User-Agent: %s\r\n",
          kHttpMethod[method], _gc(EncodeUrl(&url, 0)), agent);

  bool senthost = false, sentcontenttype = false, sentcontentlength = false;
  for (int i = 0; i < headers.n; ++i) {
    appendf(&request, "%s\r\n", headers.p[i]);
    if (!strncasecmp("Host:", headers.p[i], 5)) senthost = true;
    else if (!strncasecmp("Content-Type:", headers.p[i], 13)) sentcontenttype = true;
    else if (!strncasecmp("Content-Length:", headers.p[i], 15)) sentcontentlength = true;
  }
  if (!senthost) appendf(&request, "Host: %s:%s\r\n", host, port);
  if (postdata) {
    if (!sentcontenttype) appends(&request, "Content-Type: application/x-www-form-urlencoded\r\n");
    if (!sentcontentlength) appendf(&request, "Content-Length: %d\r\n", strlen(postdata));
  }
  appendf(&request, "\r\n");
  if (postdata) appends(&request, postdata);

  /*
   * Setup crypto.
   */
  mbedtls_ssl_config conf;
  mbedtls_ssl_context ssl;
  mbedtls_ctr_drbg_context drbg;
  if (usessl) {
    mbedtls_ssl_init(&ssl);
    mbedtls_ctr_drbg_init(&drbg);
    mbedtls_ssl_config_init(&conf);
    CHECK_EQ(0, mbedtls_ctr_drbg_seed(&drbg, GetEntropy, 0, "justine", 7));
    CHECK_EQ(0, mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT,
                                            MBEDTLS_SSL_TRANSPORT_STREAM,
                                            MBEDTLS_SSL_PRESET_DEFAULT));
    mbedtls_ssl_conf_authmode(&conf, authmode);
    mbedtls_ssl_conf_ca_chain(&conf, GetSslRoots(), 0);
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &drbg);
    if (!IsTiny()) mbedtls_ssl_conf_dbg(&conf, TlsDebug, 0);
    CHECK_EQ(0, mbedtls_ssl_setup(&ssl, &conf));
    CHECK_EQ(0, mbedtls_ssl_set_hostname(&ssl, host));
  }

  /*
   * Perform DNS lookup.
   */
  struct addrinfo *addr;
  struct addrinfo hints = {.ai_family = AF_UNSPEC,
                           .ai_socktype = SOCK_STREAM,
                           .ai_protocol = IPPROTO_TCP,
                           .ai_flags = AI_NUMERICSERV};
  CHECK_EQ(EAI_SUCCESS, getaddrinfo(host, port, &hints, &addr));

  /*
   * Connect to server.
   */
  int ret;
  CHECK_NE(-1, (sock = GoodSocket(addr->ai_family, addr->ai_socktype,
                                  addr->ai_protocol, false,
                                  &(struct timeval){-60})));
  CHECK_NE(-1, connect(sock, addr->ai_addr, addr->ai_addrlen));
  freeaddrinfo(addr);
  if (usessl) {
    mbedtls_ssl_set_bio(&ssl, &sock, TlsSend, 0, TlsRecv);
    if ((ret = mbedtls_ssl_handshake(&ssl))) {
      TlsDie("ssl handshake", ret);
    }
  }

  /*
   * Send HTTP Message.
   */
  size_t n;
  n = appendz(request).i;
  if (usessl) {
    ret = mbedtls_ssl_write(&ssl, request, n);
    if (ret != n) TlsDie("ssl write", ret);
  } else {
    CHECK_EQ(n, write(sock, request, n));
  }

  xsigaction(SIGPIPE, SIG_IGN, 0, 0, 0);

  /*
   * Handle response.
   */
  int t;
  char *p;
  ssize_t rc;
  struct HttpMessage msg;
  struct HttpUnchunker u;
  size_t g, i, hdrlen, paylen;
  InitHttpMessage(&msg, kHttpResponse);
  for (p = 0, hdrlen = paylen = t = i = n = 0;;) {
    if (i == n) {
      n += 1000;
      n += n >> 1;
      p = realloc(p, n);
    }
    if (usessl) {
      if ((rc = mbedtls_ssl_read(&ssl, p + i, n - i)) < 0) {
        if (rc == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
          rc = 0;
        } else {
          TlsDie("ssl read", rc);
        }
      }
    } else {
      CHECK_NE(-1, (rc = read(sock, p + i, n - i)));
    }
    g = rc;
    i += g;
    switch (t) {
      case kHttpClientStateHeaders:
        CHECK(g);
        CHECK_NE(-1, (rc = ParseHttpMessage(&msg, p, i)));
        if (rc) {
          hdrlen = rc;
          if (100 <= msg.status && msg.status <= 199) {
            CHECK(!HasHeader(kHttpContentLength) ||
                  HeaderEqualCase(kHttpContentLength, "0"));
            CHECK(!HasHeader(kHttpTransferEncoding) ||
                  HeaderEqualCase(kHttpTransferEncoding, "identity"));
            DestroyHttpMessage(&msg);
            InitHttpMessage(&msg, kHttpResponse);
            memmove(p, p + hdrlen, i - hdrlen);
            i -= hdrlen;
            break;
          }
          if (method == kHttpHead || includeheaders) {
            Write(p, hdrlen);
          }
          if (method == kHttpHead || msg.status == 204 || msg.status == 304) {
            goto Finished;
          }
          if (HasHeader(kHttpTransferEncoding) &&
              !HeaderEqualCase(kHttpTransferEncoding, "identity")) {
            CHECK(HeaderEqualCase(kHttpTransferEncoding, "chunked"));
            t = kHttpClientStateBodyChunked;
            memset(&u, 0, sizeof(u));
            goto Chunked;
          } else if (HasHeader(kHttpContentLength)) {
            CHECK_NE(-1, (rc = ParseContentLength(
                              HeaderData(kHttpContentLength),
                              HeaderLength(kHttpContentLength))));
            t = kHttpClientStateBodyLengthed;
            paylen = rc;
            if (paylen > i - hdrlen) {
              Write(p + hdrlen, i - hdrlen);
            } else {
              Write(p + hdrlen, paylen);
              goto Finished;
            }
          } else {
            t = kHttpClientStateBody;
            Write(p + hdrlen, i - hdrlen);
          }
        }
        break;
      case kHttpClientStateBody:
        if (!g) goto Finished;
        Write(p + i - g, g);
        break;
      case kHttpClientStateBodyLengthed:
        CHECK(g);
        if (i - hdrlen > paylen) g = hdrlen + paylen - (i - g);
        Write(p + i - g, g);
        if (i - hdrlen >= paylen) goto Finished;
        break;
      case kHttpClientStateBodyChunked:
      Chunked:
        CHECK_NE(-1, (rc = Unchunk(&u, p + hdrlen, i - hdrlen, &paylen)));
        if (rc) {
          Write(p + hdrlen, paylen);
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
  CHECK_NE(-1, close(sock));

  /*
   * Free memory.
   */
  free(p);
  free(headers.p);
  if (usessl) {
    mbedtls_ssl_free(&ssl);
    mbedtls_ctr_drbg_free(&drbg);
    mbedtls_ssl_config_free(&conf);
    mbedtls_ctr_drbg_free(&drbg);
  }

  return 0;
}
