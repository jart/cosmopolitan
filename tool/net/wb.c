/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/dns/dns.h"
#include "libc/errno.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/rand/rand.h"
#include "libc/runtime/gc.internal.h"
#include "libc/sock/goodsocket.internal.h"
#include "libc/sock/sock.h"
#include "libc/stdio/append.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/slice.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"
#include "libc/sysv/consts/tcp.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "net/http/http.h"
#include "net/http/url.h"
#include "net/https/https.h"
#include "third_party/getopt/getopt.h"
#include "third_party/mbedtls/ctr_drbg.h"
#include "third_party/mbedtls/debug.h"
#include "third_party/mbedtls/error.h"
#include "third_party/mbedtls/ssl.h"

#define Micros(t)       ((int64_t)((t)*1e6))
#define HasHeader(H)    (!!msg.headers[H].a)
#define HeaderData(H)   (inbuf.p + msg.headers[H].a)
#define HeaderLength(H) (msg.headers[H].b - msg.headers[H].a)
#define HeaderEqualCase(H, S) \
  SlicesEqualCase(S, strlen(S), HeaderData(H), HeaderLength(H))

struct Buffer {
  size_t n, c;
  char *p;
};

struct Headers {
  size_t n;
  char **p;
} headers;

bool suiteb;
char *request;
bool isdone;
char *urlarg;
int method = kHttpGet;
bool authmode = MBEDTLS_SSL_VERIFY_NONE;

char *host;
char *port;
bool usessl;
uint32_t ip;
struct Url url;
struct addrinfo *addr;
struct Buffer inbuf;

long fetch_count;
long error_count;
long failure_count;
long response_count;
double *latencies;
size_t latencies_n;
size_t latencies_c;
long double start_run;
long double end_run;
long double start_fetch;
long double end_fetch;

mbedtls_x509_crt *cachain;
mbedtls_ssl_config conf;
mbedtls_ssl_context ssl;
mbedtls_ctr_drbg_context drbg;

struct addrinfo hints = {.ai_family = AF_INET,
                         .ai_socktype = SOCK_STREAM,
                         .ai_protocol = IPPROTO_TCP,
                         .ai_flags = AI_NUMERICSERV};

void OnInt(int sig) {
  isdone = true;
}

static int TlsSend(void *c, const unsigned char *p, size_t n) {
  int rc;
  if ((rc = write(*(int *)c, p, n)) == -1) {
    if (errno == EINTR) {
      return MBEDTLS_ERR_SSL_WANT_WRITE;
    } else if (errno == EAGAIN) {
      return MBEDTLS_ERR_SSL_TIMEOUT;
    } else if (errno == EPIPE || errno == ECONNRESET || errno == ENETRESET) {
      return MBEDTLS_ERR_NET_CONN_RESET;
    } else {
      VERBOSEF("tls write() error %s", strerror(errno));
      return MBEDTLS_ERR_NET_RECV_FAILED;
    }
  }
  return rc;
}

static int TlsRecv(void *c, unsigned char *p, size_t n, uint32_t o) {
  int r;
  if ((r = read(*(int *)c, p, n)) == -1) {
    if (errno == EINTR) {
      return MBEDTLS_ERR_SSL_WANT_READ;
    } else if (errno == EAGAIN) {
      return MBEDTLS_ERR_SSL_TIMEOUT;
    } else if (errno == EPIPE || errno == ECONNRESET || errno == ENETRESET) {
      return MBEDTLS_ERR_NET_CONN_RESET;
    } else {
      VERBOSEF("tls read() error %s", strerror(errno));
      return MBEDTLS_ERR_NET_RECV_FAILED;
    }
  }
  return r;
}

static wontreturn void PrintUsage(FILE *f, int rc) {
  fprintf(f, "usage: %s [-ksvV] URL\n", program_invocation_name);
  exit(rc);
}

int fetch(void) {
  char *p;
  int status;
  ssize_t rc;
  const char *body;
  int t, ret, sock;
  struct TlsBio *bio;
  struct HttpMessage msg;
  struct HttpUnchunker u;
  size_t urlarglen, requestlen;
  size_t g, i, n, hdrsize, paylen;

  /*
   * Setup crypto.
   */
  if (usessl) {
    -mbedtls_ssl_session_reset(&ssl);
    CHECK_EQ(0, mbedtls_ssl_set_hostname(&ssl, host));
  }

  /*
   * Connect to server.
   */
  InitHttpMessage(&msg, kHttpResponse);
  ip = ntohl(((struct sockaddr_in *)addr->ai_addr)->sin_addr.s_addr);
  CHECK_NE(-1, (sock = GoodSocket(addr->ai_family, addr->ai_socktype,
                                  addr->ai_protocol, false, 0)));
  if (connect(sock, addr->ai_addr, addr->ai_addrlen) == -1) {
    goto TransportError;
  }
  if (usessl) {
    mbedtls_ssl_set_bio(&ssl, &sock, TlsSend, 0, TlsRecv);
    if ((ret = mbedtls_ssl_handshake(&ssl))) {
      goto TransportError;
    }
  }

SendAnother:

  /*
   * Send HTTP Message.
   */
  n = appendz(request).i;
  if (usessl) {
    ret = mbedtls_ssl_write(&ssl, request, n);
    if (ret != n) goto TransportError;
  } else if (write(sock, request, n) != n) {
    goto TransportError;
  }

  /*
   * Handle response.
   */
  InitHttpMessage(&msg, kHttpResponse);
  for (hdrsize = paylen = t = 0;;) {
    if (inbuf.n == inbuf.c) {
      inbuf.c += 1000;
      inbuf.c += inbuf.c >> 1;
      inbuf.p = realloc(inbuf.p, inbuf.c);
    }
    if (usessl) {
      if ((rc = mbedtls_ssl_read(&ssl, inbuf.p + inbuf.n, inbuf.c - inbuf.n)) <
          0) {
        if (rc == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
          rc = 0;
        } else {
          goto TransportError;
        }
      }
    } else if ((rc = read(sock, inbuf.p + inbuf.n, inbuf.c - inbuf.n)) == -1) {
      goto TransportError;
    }
    g = rc;
    inbuf.n += g;
    switch (t) {
      case kHttpClientStateHeaders:
        if (!g) goto TransportError;
        rc = ParseHttpMessage(&msg, inbuf.p, inbuf.n);
        if (rc == -1) goto TransportError;
        if (rc) {
          hdrsize = rc;
          if (100 <= msg.status && msg.status <= 199) {
            if ((HasHeader(kHttpContentLength) &&
                 !HeaderEqualCase(kHttpContentLength, "0")) ||
                (HasHeader(kHttpTransferEncoding) &&
                 !HeaderEqualCase(kHttpTransferEncoding, "identity"))) {
              goto TransportError;
            }
            DestroyHttpMessage(&msg);
            InitHttpMessage(&msg, kHttpResponse);
            memmove(inbuf.p, inbuf.p + hdrsize, inbuf.n - hdrsize);
            inbuf.n -= hdrsize;
            break;
          }
          if (msg.status == 204 || msg.status == 304) {
            goto Finished;
          }
          if (HasHeader(kHttpTransferEncoding) &&
              !HeaderEqualCase(kHttpTransferEncoding, "identity")) {
            if (HeaderEqualCase(kHttpTransferEncoding, "chunked")) {
              t = kHttpClientStateBodyChunked;
              bzero(&u, sizeof(u));
              goto Chunked;
            } else {
              goto TransportError;
            }
          } else if (HasHeader(kHttpContentLength)) {
            rc = ParseContentLength(HeaderData(kHttpContentLength),
                                    HeaderLength(kHttpContentLength));
            if (rc == -1) goto TransportError;
            if ((paylen = rc) <= inbuf.n - hdrsize) {
              goto Finished;
            } else {
              t = kHttpClientStateBodyLengthed;
            }
          } else {
            t = kHttpClientStateBody;
          }
        }
        break;
      case kHttpClientStateBody:
        if (!g) {
          paylen = inbuf.n;
          goto Finished;
        }
        break;
      case kHttpClientStateBodyLengthed:
        if (!g) goto TransportError;
        if (inbuf.n - hdrsize >= paylen) goto Finished;
        break;
      case kHttpClientStateBodyChunked:
      Chunked:
        rc = Unchunk(&u, inbuf.p + hdrsize, inbuf.n - hdrsize, &paylen);
        if (rc == -1) goto TransportError;
        if (rc) goto Finished;
        break;
      default:
        unreachable;
    }
  }

Finished:
  status = msg.status;
  DestroyHttpMessage(&msg);
  if (!isdone && status == 200) {
    long double now = nowl();
    end_fetch = now;
    ++response_count;
    latencies = realloc(latencies, ++latencies_n * sizeof(*latencies));
    latencies[latencies_n - 1] = end_fetch - start_fetch;
    ++fetch_count;
    start_fetch = now;
    goto SendAnother;
  }
  close(sock);
  return status;
TransportError:
  close(sock);
  DestroyHttpMessage(&msg);
  return 900;
}

int main(int argc, char *argv[]) {
  xsigaction(SIGPIPE, SIG_IGN, 0, 0, 0);
  xsigaction(SIGINT, OnInt, 0, 0, 0);

  /*
   * Read flags.
   */
  int opt;
  __log_level = kLogWarn;
  while ((opt = getopt(argc, argv, "BqksvIX:H:")) != -1) {
    switch (opt) {
      case 's':
      case 'q':
        break;
      case 'B':
        suiteb = true;
        break;
      case 'v':
        ++__log_level;
        break;
      case 'I':
        method = kHttpHead;
        break;
      case 'H':
        headers.p = realloc(headers.p, ++headers.n * sizeof(*headers.p));
        headers.p[headers.n - 1] = optarg;
        break;
      case 'X':
        CHECK((method = GetHttpMethod(optarg, strlen(optarg))));
        break;
      case 'k':
        authmode = MBEDTLS_SSL_VERIFY_REQUIRED;
        break;
      case 'h':
        PrintUsage(stdout, EXIT_SUCCESS);
      default:
        PrintUsage(stderr, EX_USAGE);
    }
  }

  if (optind == argc) PrintUsage(stdout, EXIT_SUCCESS);
  urlarg = argv[optind];
  cachain = GetSslRoots();

  /*
   * Parse URL.
   */
  gc(ParseUrl(urlarg, -1, &url));
  gc(url.params.p);
  usessl = false;
  if (url.scheme.n) {
    if (url.scheme.n == 5 && !memcasecmp(url.scheme.p, "https", 5)) {
      usessl = true;
    } else if (!(url.scheme.n == 4 && !memcasecmp(url.scheme.p, "http", 4))) {
      FATALF("bad scheme");
    }
  }
  if (url.host.n) {
    host = gc(strndup(url.host.p, url.host.n));
    if (url.port.n) {
      port = gc(strndup(url.port.p, url.port.n));
    } else {
      port = usessl ? "443" : "80";
    }
  } else {
    host = "127.0.0.1";
    port = "80";
  }
  CHECK(IsAcceptableHost(host, -1));
  url.fragment.p = 0, url.fragment.n = 0;
  url.scheme.p = 0, url.scheme.n = 0;
  url.user.p = 0, url.user.n = 0;
  url.pass.p = 0, url.pass.n = 0;
  url.host.p = 0, url.host.n = 0;
  url.port.p = 0, url.port.n = 0;
  if (!url.path.n || url.path.p[0] != '/') {
    char *p = gc(xmalloc(1 + url.path.n));
    mempcpy(mempcpy(p, "/", 1), url.path.p, url.path.n);
    url.path.p = p;
    ++url.path.n;
  }

  /*
   * Create HTTP message.
   */
  appendf(&request,
          "%s %s HTTP/1.1\r\n"
          "Host: %s:%s\r\n",
          kHttpMethod[method], _gc(EncodeUrl(&url, 0)), host, port);
  for (int i = 0; i < headers.n; ++i) {
    appendf(&request, "%s\r\n", headers.p[i]);
  }
  appendf(&request, "\r\n");

  /*
   * Perform DNS lookup.
   */
  int rc;
  if ((rc = getaddrinfo(host, port, &hints, &addr)) != EAI_SUCCESS) {
    FATALF("getaddrinfo(%s:%s) failed", host, port);
  }

  /*
   * Setup SSL crypto.
   */
  mbedtls_ssl_init(&ssl);
  mbedtls_ctr_drbg_init(&drbg);
  mbedtls_ssl_config_init(&conf);
  CHECK_EQ(0, mbedtls_ctr_drbg_seed(&drbg, GetEntropy, 0, "justine", 7));
  CHECK_EQ(0,
           mbedtls_ssl_config_defaults(
               &conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM,
               suiteb ? MBEDTLS_SSL_PRESET_SUITEB : MBEDTLS_SSL_PRESET_SUITEC));
  mbedtls_ssl_conf_authmode(&conf, authmode);
  mbedtls_ssl_conf_ca_chain(&conf, cachain, 0);
  mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &drbg);
  CHECK_EQ(0, mbedtls_ssl_setup(&ssl, &conf));

  int status;
  latencies_c = 1024;
  latencies = malloc(latencies_c * sizeof(*latencies));
  start_run = nowl();
  while (!isdone) {
    ++fetch_count;
    start_fetch = nowl();
    status = fetch();
    end_fetch = nowl();
    if (status == 200) {
      ++response_count;
      latencies = realloc(latencies, ++latencies_n * sizeof(*latencies));
      latencies[latencies_n - 1] = end_fetch - start_fetch;
    } else if (status == 900) {
      ++failure_count;
    } else {
      ++error_count;
    }
  }
  end_run = nowl();

  double latencies_sum = fsum(latencies, latencies_n);
  double avg_latency = latencies_sum / response_count;

  printf("\n");
  printf("run time:       %,ldµs\n", Micros(end_run - start_run));
  printf("per second:     %,ld\n",
         (int64_t)(response_count / (end_run - start_run)));
  printf("avg latency:    %,ldµs\n", Micros(avg_latency));
  printf("response count: %,ld\n", response_count);
  printf("fetch count:    %,ld\n", fetch_count - failure_count);
  printf("error count:    %,ld (non-200 responses)\n", error_count);
  printf("failure count:  %,ld (transport error)\n", failure_count);

  return 0;
}
