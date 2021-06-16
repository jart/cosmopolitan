#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/log/check.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "third_party/mbedtls/include/mbedtls/ctr_drbg.h"
#include "third_party/mbedtls/include/mbedtls/dhm.h"
#include "third_party/mbedtls/include/mbedtls/entropy.h"
#include "third_party/mbedtls/include/mbedtls/error.h"
#include "third_party/mbedtls/include/mbedtls/net_sockets.h"
#include "third_party/mbedtls/include/mbedtls/pk.h"
#include "third_party/mbedtls/include/mbedtls/platform.h"
#include "third_party/mbedtls/include/mbedtls/ssl.h"
#include "third_party/mbedtls/include/mbedtls/ssl_cache.h"
#include "third_party/mbedtls/include/mbedtls/x509_crt.h"

#define R(e) \
  if ((r = e)) goto Die

char buf[1024], ebuf[100];
mbedtls_net_context server, client;
mbedtls_x509_crt cert;
mbedtls_x509_crt cacert;
mbedtls_entropy_context entropy;
mbedtls_ssl_cache_context cache;
mbedtls_ctr_drbg_context ctrdrbg;
mbedtls_pk_context pkey;
mbedtls_ssl_config conf;
mbedtls_ssl_context ssl;

void OnDebug(void *ctx, int lev, const char *file, int line, const char *str) {
  fprintf(stderr, "%s:%04d: %s", file, line, str);
  fflush(stderr);
}

int main(int argc, char *argv[]) {
  int r, len;

  system("openssl req -x509 -newkey rsa:2048 -keyout key.pem -out cert.pem "
         "-days 3650 -nodes -subj '/CN=localhost'");

  mbedtls_net_init(&server);
  mbedtls_net_init(&client);
  mbedtls_ssl_init(&ssl);
  mbedtls_ssl_config_init(&conf);
  mbedtls_ssl_cache_init(&cache);
  mbedtls_entropy_init(&entropy);
  mbedtls_x509_crt_init(&cert);
  mbedtls_pk_init(&pkey);
  mbedtls_ctr_drbg_init(&ctrdrbg);

  R(mbedtls_pk_parse_keyfile(&pkey, "key.pem", 0));
  R(mbedtls_x509_crt_parse_file(&cert, "cert.pem"));
  R(mbedtls_net_bind(&server, "0.0.0.0", "8080", MBEDTLS_NET_PROTO_TCP));
  R(mbedtls_ctr_drbg_seed(&ctrdrbg, mbedtls_entropy_func, &entropy,
                          (const unsigned char *)"redbean", 7));
  R(mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_SERVER,
                                MBEDTLS_SSL_TRANSPORT_STREAM,
                                MBEDTLS_SSL_PRESET_DEFAULT));
  mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctrdrbg);
  mbedtls_ssl_conf_dbg(&conf, OnDebug, 0);
  mbedtls_ssl_conf_session_cache(&conf, &cache, mbedtls_ssl_cache_get,
                                 mbedtls_ssl_cache_set);
  R(mbedtls_ssl_conf_own_cert(&conf, &cert, &pkey));
  R(mbedtls_ssl_setup(&ssl, &conf));

Reset:
  mbedtls_net_free(&client);
  R(mbedtls_ssl_session_reset(&ssl));
  R(mbedtls_net_accept(&server, &client, 0, 0, 0));
  mbedtls_ssl_set_bio(&ssl, &client, mbedtls_net_send, mbedtls_net_recv, 0);

  while ((r = mbedtls_ssl_handshake(&ssl)) != 0) {
    if (r != MBEDTLS_ERR_SSL_WANT_READ && r != MBEDTLS_ERR_SSL_WANT_WRITE) {
      printf(" failed\n  ! mbedtls_ssl_handshake returned %d\n", r);
      goto Reset;
    }
  }

  do {
    len = sizeof(buf) - 1;
    memset(buf, 0, sizeof(buf));
    r = mbedtls_ssl_read(&ssl, (void *)buf, len);
    if (r == MBEDTLS_ERR_SSL_WANT_READ || r == MBEDTLS_ERR_SSL_WANT_WRITE)
      continue;
    if (r <= 0) {
      switch (r) {
        case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
          printf("connection was closed gracefully\n");
          break;
        case MBEDTLS_ERR_NET_CONN_RESET:
          printf("connection was reset by peer\n");
          break;
        default:
          printf("mbedtls_ssl_read returned -0x%x\n", -r);
          break;
      }
      break;
    }
    len = r;
    printf("%d bytes read\n%s", len, buf);
    if (r > 0) break;
  } while (1);

  len = sprintf(buf,
                "HTTP/1.0 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "\r\n"
                "<h2>Mbed TLS Test Server</h2>\r\n"
                "<p>Successful connection using: %s\r\n",
                mbedtls_ssl_get_ciphersuite(&ssl));

  while ((r = mbedtls_ssl_write(&ssl, (void *)buf, len)) <= 0) {
    if (r == MBEDTLS_ERR_NET_CONN_RESET) {
      printf("failed\n  ! peer closed the connection\n");
      goto Reset;
    }
    if (r != MBEDTLS_ERR_SSL_WANT_READ && r != MBEDTLS_ERR_SSL_WANT_WRITE) {
      printf("failed\n  ! mbedtls_ssl_write returned %d\n", r);
      exit(1);
    }
  }

  while ((r = mbedtls_ssl_close_notify(&ssl)) < 0) {
    if (r != MBEDTLS_ERR_SSL_WANT_READ && r != MBEDTLS_ERR_SSL_WANT_WRITE) {
      printf("error: mbedtls_ssl_close_notify returned %d\n", r);
      goto Reset;
    }
  }

  printf("ok\n");

  r = 0;
  goto Reset;

  mbedtls_net_free(&client);
  mbedtls_net_free(&server);
  mbedtls_x509_crt_free(&cert);
  mbedtls_pk_free(&pkey);
  mbedtls_ssl_cache_free(&cache);
  return 0;
Die:
  mbedtls_strerror(r, ebuf, 100);
  printf("last error was: %d - %s\n", r, ebuf);
  return 1;
}
