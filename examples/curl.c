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
#include "libc/calls/calls.h"
#include "libc/dns/dns.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/rand/rand.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/dt.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/shut.h"
#include "libc/sysv/consts/sock.h"
#include "libc/x/x.h"
#include "net/http/http.h"
#include "net/http/url.h"
#include "third_party/mbedtls/ctr_drbg.h"
#include "third_party/mbedtls/debug.h"
#include "third_party/mbedtls/error.h"
#include "third_party/mbedtls/ssl.h"

STATIC_YOINK("zip_uri_support");
STATIC_YOINK("usr/share/ssl/root/amazon.pem");
STATIC_YOINK("usr/share/ssl/root/certum.pem");
STATIC_YOINK("usr/share/ssl/root/comodo.pem");
STATIC_YOINK("usr/share/ssl/root/digicert.pem");
STATIC_YOINK("usr/share/ssl/root/dst.pem");
STATIC_YOINK("usr/share/ssl/root/geotrust.pem");
STATIC_YOINK("usr/share/ssl/root/globalsign.pem");
STATIC_YOINK("usr/share/ssl/root/godaddy.pem");
STATIC_YOINK("usr/share/ssl/root/google.pem");
STATIC_YOINK("usr/share/ssl/root/isrg.pem");
STATIC_YOINK("usr/share/ssl/root/quovadis.pem");
STATIC_YOINK("usr/share/ssl/root/redbean.pem");
STATIC_YOINK("usr/share/ssl/root/starfield.pem");
STATIC_YOINK("usr/share/ssl/root/verisign.pem");

/**
 * @fileoverview Downloads HTTP URL to stdout.
 *
 *     make -j8 o//examples/curl.com
 *     o//examples/curl.com http://justine.lol/ape.html
 */

static int GetEntropy(void *c, unsigned char *p, size_t n) {
  CHECK_EQ(n, getrandom(p, n, 0));
  return 0;
}

static int TlsSend(void *ctx, const unsigned char *buf, size_t len) {
  int rc;
  CHECK_NE(-1, (rc = write(*(int *)ctx, buf, len)));
  return rc;
}

static int TlsRecv(void *ctx, unsigned char *buf, size_t len, uint32_t tmo) {
  int rc;
  CHECK_NE(-1, (rc = read(*(int *)ctx, buf, len)));
  return rc;
}

static void TlsDebug(void *ctx, int level, const char *file, int line,
                     const char *message) {
  flogf(level, file, line, 0, "TLS %s", message);
}

static char *TlsError(int rc) {
  static char ebuf[128];
  mbedtls_strerror(rc, ebuf, sizeof(ebuf));
  return ebuf;
}

int main(int argc, char *argv[]) {

  /*
   * Get argument.
   */
  const char *urlarg;
  if (argc != 2) {
    fprintf(stderr, "usage: %s URL\n", argv[0]);
    exit(1);
  }
  urlarg = argv[1];

  /*
   * Parse URL.
   */
  struct Url url;
  char *host, *port;
  bool usessl = false;
  _gc(ParseUrl(urlarg, -1, &url));
  _gc(url.params.p);
  if (url.scheme.n) {
    if (url.scheme.n == 5 && !memcasecmp(url.scheme.p, "https", 5)) {
      usessl = true;
    } else if (!(url.scheme.n == 4 && !memcasecmp(url.scheme.p, "http", 4))) {
      fprintf(stderr, "error: not an http/https url: %s\n", urlarg);
      exit(1);
    }
  }
  host = firstnonnull(_gc(strndup(url.host.p, url.host.n)), "127.0.0.1");
  port = url.port.n ? _gc(strndup(url.port.p, url.port.n))
                    : (usessl ? "443" : "80");
  port = _gc(xasprintf("%hu", atoi(port)));
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
  const char *msg;
  msg = _gc(xasprintf("GET %s HTTP/1.1\r\n"
                      "Host: %s:%s\r\n"
                      "Connection: close\r\n"
                      "Content-Length: 0\r\n"
                      "Accept: text/plain; */*\r\n"
                      "User-Agent: github.com/jart/cosmopolitan\r\n"
                      "\r\n",
                      _gc(EncodeUrl(&url, 0)), host, port));

  /*
   * Load root certificates.
   */
  mbedtls_x509_crt cacert;
  if (usessl) {
    DIR *zd;
    size_t calen;
    const char *dir;
    char capath[300];
    uint8_t *cabytes;
    struct dirent *ze;
    mbedtls_x509_crt_init(&cacert);
    dir = "zip:usr/share/ssl/root";
    CHECK((zd = opendir(dir)), "%s", dir);
    while ((ze = readdir(zd))) {
      if (ze->d_type != DT_REG) continue;
      snprintf(capath, sizeof(capath), "%s/%s", dir, ze->d_name);
      CHECK((cabytes = xslurp(capath, &calen)));
      CHECK_EQ(0, mbedtls_x509_crt_parse(&cacert, cabytes, calen + 1), "%s",
               capath);
      free(cabytes);
    }
    closedir(zd);
  }

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
    CHECK_EQ(0, mbedtls_ctr_drbg_seed(&drbg, GetEntropy, 0,
                                      (const uint8_t *)"justine", 7));
    CHECK_EQ(0, mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT,
                                            MBEDTLS_SSL_TRANSPORT_STREAM,
                                            MBEDTLS_SSL_PRESET_DEFAULT));
    mbedtls_ssl_conf_ca_chain(&conf, &cacert, 0);
    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &drbg);
    mbedtls_ssl_conf_dbg(&conf, TlsDebug, 0);
    /* mbedtls_debug_set_threshold(5); */
    CHECK_EQ(0, mbedtls_ssl_setup(&ssl, &conf));
    CHECK_EQ(0, mbedtls_ssl_set_hostname(&ssl, host));
  }

  /*
   * Perform DNS lookup.
   */
  struct addrinfo *addr;
  struct addrinfo hints = {.ai_family = AF_INET,
                           .ai_socktype = SOCK_STREAM,
                           .ai_protocol = IPPROTO_TCP,
                           .ai_flags = AI_NUMERICSERV};
  CHECK_EQ(EAI_SUCCESS, getaddrinfo(host, port, &hints, &addr));

  /*
   * Connect to server.
   */
  int ret, sock;
  CHECK_NE(-1, (sock = socket(addr->ai_family, addr->ai_socktype,
                              addr->ai_protocol)));
  CHECK_NE(-1, connect(sock, addr->ai_addr, addr->ai_addrlen));
  if (usessl) {
    mbedtls_ssl_set_bio(&ssl, &sock, TlsSend, 0, TlsRecv);
    if ((ret = mbedtls_ssl_handshake(&ssl))) {
      FATALF("ssl handshake failed (-0x%04x %s)", -ret, TlsError(ret));
    }
  }

  /*
   * Send HTTP Message.
   */
  if (usessl) {
    CHECK_EQ(strlen(msg), mbedtls_ssl_write(&ssl, (void *)msg, strlen(msg)));
  } else {
    CHECK_EQ(strlen(msg), write(sock, msg, strlen(msg)));
    shutdown(sock, SHUT_WR);
  }

  /*
   * Handle response.
   * TODO(jart): use response/chunk parsers
   */
  char buf[1500];
  size_t got, toto;
  ssize_t rc, need;
  const char *crlfcrlf;
  buf[0] = '\0';
  if (usessl) {
    if ((rc = mbedtls_ssl_read(&ssl, (void *)buf, sizeof(buf))) < 0) {
      FATALF("ssl read failed (-0x%04x %s)", -ret, TlsError(rc));
    }
  } else {
    CHECK_NE(-1, (rc = read(sock, buf, sizeof(buf))));
  }
  got = rc;
  CHECK(startswith(buf, "HTTP/1.1 200") || startswith(buf, "HTTP/1.0 200"),
        "%`'.*s", got, buf);
  CHECK_NOTNULL((crlfcrlf = memmem(buf, got, "\r\n\r\n", 4)));
  need = strtol(
      (char *)firstnonnull(
          memmem(buf, crlfcrlf - buf, "\r\nContent-Length: ", 18),
          firstnonnull(memmem(buf, crlfcrlf - buf, "\r\ncontent-length: ", 18),
                       "\r\nContent-Length: -1")) +
          18,
      NULL, 10);
  got = MIN(got - (crlfcrlf + 4 - buf), need);
  CHECK_EQ(got, write(1, crlfcrlf + 4, got));
  for (toto = got; need == -1 || toto < need; toto += got) {
    if (usessl) {
      if ((rc = mbedtls_ssl_read(&ssl, (void *)buf, sizeof(buf))) < 0) {
        if (rc == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
          CHECK_EQ(0, mbedtls_ssl_close_notify(&ssl));
          got = 0;
          break;
        } else {
          FATALF("ssl read failed (-0x%04x %s)", -rc, TlsError(rc));
        }
      }
    } else {
      CHECK_NE(-1, (rc = read(sock, buf, sizeof(buf))));
    }
    if (!(got = rc)) break;
    CHECK_EQ(got, write(1, buf, got));
  }
  if (need != -1) {
    CHECK_EQ(need, toto);
  }

  /*
   * Close connection.
   */
  if (got) {
    if (usessl) {
      CHECK_EQ(0, mbedtls_ssl_close_notify(&ssl));
      CHECK_EQ(MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY,
               mbedtls_ssl_read(&ssl, (void *)buf, 1));
    } else {
      shutdown(sock, SHUT_RDWR);
      CHECK_EQ(0, read(sock, buf, 1));
    }
  }
  CHECK_NE(-1, close(sock));
  if (usessl) {
    mbedtls_ssl_free(&ssl);
    mbedtls_x509_crt_free(&cacert);
    mbedtls_ssl_config_free(&conf);
    mbedtls_ctr_drbg_free(&drbg);
  }

  return 0;
}
