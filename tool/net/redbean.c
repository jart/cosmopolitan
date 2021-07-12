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
#include "libc/bits/bits.h"
#include "libc/bits/likely.h"
#include "libc/bits/popcnt.h"
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/sigbits.h"
#include "libc/calls/struct/dirent.h"
#include "libc/calls/struct/flock.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/stat.h"
#include "libc/dns/dns.h"
#include "libc/dns/hoststxt.h"
#include "libc/dos.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/log/backtrace.internal.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/alloca.h"
#include "libc/mem/fmt.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/bsf.h"
#include "libc/nexgen32e/bsr.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/rand/rand.h"
#include "libc/runtime/clktck.h"
#include "libc/runtime/directmap.internal.h"
#include "libc/runtime/gc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/undeflate.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/dt.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/inaddr.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/lock.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/msync.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/rusage.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/consts/shut.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"
#include "libc/sysv/consts/tcp.h"
#include "libc/sysv/consts/w.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "libc/zip.h"
#include "net/http/escape.h"
#include "net/http/http.h"
#include "net/http/ip.h"
#include "net/http/url.h"
#include "net/https/https.h"
#include "third_party/getopt/getopt.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/ltests.h"
#include "third_party/lua/lua.h"
#include "third_party/lua/lualib.h"
#include "third_party/mbedtls/asn1.h"
#include "third_party/mbedtls/asn1write.h"
#include "third_party/mbedtls/cipher.h"
#include "third_party/mbedtls/config.h"
#include "third_party/mbedtls/ctr_drbg.h"
#include "third_party/mbedtls/debug.h"
#include "third_party/mbedtls/ecp.h"
#include "third_party/mbedtls/entropy.h"
#include "third_party/mbedtls/entropy_poll.h"
#include "third_party/mbedtls/error.h"
#include "third_party/mbedtls/iana.h"
#include "third_party/mbedtls/md5.h"
#include "third_party/mbedtls/oid.h"
#include "third_party/mbedtls/pk.h"
#include "third_party/mbedtls/rsa.h"
#include "third_party/mbedtls/san.h"
#include "third_party/mbedtls/sha1.h"
#include "third_party/mbedtls/ssl.h"
#include "third_party/mbedtls/ssl_ticket.h"
#include "third_party/mbedtls/x509.h"
#include "third_party/mbedtls/x509_crt.h"
#include "third_party/regex/regex.h"
#include "third_party/zlib/zlib.h"
#include "tool/build/lib/case.h"

/**
 * @fileoverview redbean - single-file distributable web server
 *
 * redbean makes it possible to share web applications that run offline
 * as a single-file αcτµαlly pδrταblε εxεcµταblε zip archive which
 * contains your assets. All you need to do is download the redbean.com
 * program below, change the filename to .zip, add your content in a zip
 * editing tool, and then change the extension back to .com.
 *
 * redbean can serve 1 million+ gzip encoded responses per second on a
 * cheap personal computer. That performance is thanks to zip and gzip
 * using the same compression format, which enables kernelspace copies.
 * Another reason redbean goes fast is that it's a tiny static binary,
 * which makes fork memory paging nearly free.
 *
 * redbean is also easy to modify to suit your own needs. The program
 * itself is written as a single .c file. It embeds the Lua programming
 * language and SQLite which let you write dynamic pages.
 */

#ifndef REDBEAN
#define REDBEAN "redbean"
#endif

#define CHUNK            (128 * 1024)
#define HASH_LOAD_FACTOR /* 1. / */ 4
#define read(F, P, N)    readv(F, &(struct iovec){P, N}, 1)
#define write(F, P, N)   writev(F, &(struct iovec){P, N}, 1)
#define LockInc(P)       asm volatile("lock incq\t%0" : "=m"(*(P)))
#define AppendCrlf(P)    mempcpy(P, "\r\n", 2)
#define HasHeader(H)     (!!msg.headers[H].a)
#define HeaderData(H)    (inbuf.p + msg.headers[H].a)
#define HeaderLength(H)  (msg.headers[H].b - msg.headers[H].a)
#define HeaderEqualCase(H, S) \
  SlicesEqualCase(S, strlen(S), HeaderData(H), HeaderLength(H))

static const uint8_t kGzipHeader[] = {
    0x1F,        // MAGNUM
    0x8B,        // MAGNUM
    0x08,        // CM: DEFLATE
    0x00,        // FLG: NONE
    0x00,        // MTIME: NONE
    0x00,        //
    0x00,        //
    0x00,        //
    0x00,        // XFL
    kZipOsUnix,  // OS
};

static const char *const kIndexPaths[] = {
#ifndef STATIC
    "index.lua",
#endif
    "index.html",
};

static const char *const kAlpn[] = {
    "http/1.1",
    NULL,
};

static const char kRegCode[][8] = {
    "OK",     "NOMATCH", "BADPAT", "COLLATE", "ECTYPE", "EESCAPE", "ESUBREG",
    "EBRACK", "EPAREN",  "EBRACE", "BADBR",   "ERANGE", "ESPACE",  "BADRPT",
};

struct Buffer {
  size_t n, c;
  char *p;
};

struct TlsBio {
  int fd, c;
  unsigned a, b;
  unsigned char t[4000];
  unsigned char u[1430];
};

struct Strings {
  size_t n, c;
  struct String {
    size_t n;
    const char *s;
  } * p;
};

struct DeflateGenerator {
  int t;
  void *b;
  size_t i;
  uint32_t c;
  z_stream s;
  struct Asset *a;
};

static struct Ips {
  size_t n;
  uint32_t *p;
} ips;

static struct Ports {
  size_t n;
  uint16_t *p;
} ports;

static struct Servers {
  size_t n;
  struct Server {
    int fd;
    struct sockaddr_in addr;
  } * p;
} servers;

static struct Freelist {
  size_t n, c;
  void **p;
} freelist;

static struct Unmaplist {
  size_t n, c;
  struct Unmap {
    int f;
    void *p;
    size_t n;
  } * p;
} unmaplist;

static struct Certs {
  size_t n;
  struct Cert {
    mbedtls_x509_crt *cert;
    mbedtls_pk_context *key;
  } * p;
} certs;

static struct Redirects {
  size_t n;
  struct Redirect {
    int code;
    struct String path;
    struct String location;
  } * p;
} redirects;

static struct Assets {
  uint32_t n;
  struct Asset {
    bool istext;
    uint32_t hash;
    uint64_t cf;
    uint64_t lf;
    int64_t lastmodified;
    char *lastmodifiedstr;
    struct File {
      struct String path;
      struct stat st;
    } * file;
  } * p;
} assets;

static struct Shared {
  int workers;
  long double nowish;
  long double lastreindex;
  long double lastmeltdown;
  char currentdate[32];
  struct rusage server;
  struct rusage children;
  struct Counters {
#define C(x) long x;
#include "tool/net/counters.inc"
#undef C
  } c;
} * shared;

static const char kCounterNames[] =
#define C(x) #x "\0"
#include "tool/net/counters.inc"
#undef C
    ;

typedef ssize_t (*reader_f)(int, void *, size_t);
typedef ssize_t (*writer_f)(int, struct iovec *, int);

static bool usessl;
static bool suiteb;
static bool killed;
static bool istext;
static bool zombied;
static bool gzipped;
static bool branded;
static bool funtrace;
static bool meltdown;
static bool printport;
static bool daemonize;
static bool logrusage;
static bool logbodies;
static bool sslcliused;
static bool loglatency;
static bool terminated;
static bool uniprocess;
static bool invalidated;
static bool logmessages;
static bool checkedmethod;
static bool sslfetchverify;
static bool sslclientverify;
static bool connectionclose;
static bool hasonworkerstop;
static bool hasonworkerstart;
static bool hasonhttprequest;
static bool keyboardinterrupt;
static bool listeningonport443;
static bool hasonprocesscreate;
static bool hasonprocessdestroy;
static bool loggednetworkorigin;
static bool hasonclientconnection;

static int zfd;
static int frags;
static int gmtoff;
static int client;
static int changeuid;
static int changegid;
static int statuscode;
static int oldloglevel;
static int maxpayloadsize;
static int messageshandled;
static uint32_t clientaddrsize;

static lua_State *L;
static size_t zsize;
static char *content;
static uint8_t *zmap;
static uint8_t *zbase;
static uint8_t *zcdir;
static size_t hdrsize;
static size_t msgsize;
static size_t amtread;
static reader_f reader;
static writer_f writer;
static char *extrahdrs;
static char *luaheaderp;
static const char *zpath;
static const char *brand;
static char gzip_footer[8];
static const char *pidpath;
static const char *logpath;
static struct pollfd *polls;
static struct Strings loops;
static size_t payloadlength;
static size_t contentlength;
static int64_t cacheseconds;
static const char *serverheader;
static struct Strings stagedirs;
static struct Strings hidepaths;
static mbedtls_x509_crt *cachain;
static const char *launchbrowser;
static ssize_t (*generator)(struct iovec[3]);

static struct Buffer inbuf;
static struct Buffer oldin;
static struct Buffer hdrbuf;
static struct Buffer outbuf;
static struct timeval timeout;
static struct Buffer effectivepath;

static struct Url url;
static struct HttpMessage msg;

static struct stat zst;
static long double startread;
static long double lastrefresh;
static long double startserver;
static long double startrequest;
static long double lastheartbeat;
static long double startconnection;
static struct sockaddr_in clientaddr;
static struct sockaddr_in *serveraddr;

static mbedtls_ssl_config conf;
static mbedtls_ssl_context ssl;
static mbedtls_ctr_drbg_context rng;
static mbedtls_ssl_ticket_context ssltick;

static mbedtls_ssl_config confcli;
static mbedtls_ssl_context sslcli;
static mbedtls_ctr_drbg_context rngcli;

static struct TlsBio g_bio;
static char slashpath[PATH_MAX];
static struct DeflateGenerator dg;

static char *Route(const char *, size_t, const char *, size_t);
static char *RouteHost(const char *, size_t, const char *, size_t);
static char *RoutePath(const char *, size_t);
static char *HandleAsset(struct Asset *, const char *, size_t);
static char *ServeAsset(struct Asset *, const char *, size_t);
static char *SetStatus(unsigned, const char *);

static void OnChld(void) {
  zombied = true;
}

static void OnUsr1(void) {
  invalidated = true;
}

static void OnUsr2(void) {
  meltdown = true;
}

static void OnTerm(void) {
  if (!terminated) {
    terminated = true;
  } else {
    killed = true;
  }
}

static void OnInt(void) {
  keyboardinterrupt = true;
  OnTerm();
}

static void OnHup(void) {
  if (daemonize) {
    OnUsr1();
  } else {
    OnTerm();
  }
}

static long ParseInt(const char *s) {
  return strtol(s, 0, 0);
}

forceinline bool SlicesEqual(const char *a, size_t n, const char *b, size_t m) {
  return n == m && !memcmp(a, b, n);
}

forceinline bool SlicesEqualCase(const char *a, size_t n, const char *b,
                                 size_t m) {
  return n == m && !memcasecmp(a, b, n);
}

static int CompareSlices(const char *a, size_t n, const char *b, size_t m) {
  int c;
  if ((c = memcmp(a, b, MIN(n, m)))) return c;
  if (n < m) return -1;
  if (n > m) return +1;
  return 0;
}

static int CompareSlicesCase(const char *a, size_t n, const char *b, size_t m) {
  int c;
  if ((c = memcasecmp(a, b, MIN(n, m)))) return c;
  if (n < m) return -1;
  if (n > m) return +1;
  return 0;
}

static bool StartsWithIgnoreCase(const char *s, const char *prefix) {
  for (;;) {
    if (!*prefix) return true;
    if (!*s) return false;
    if (kToLower[*s++ & 255] != (*prefix++ & 255)) return false;
  }
}

static void *FreeLater(void *p) {
  if (p) {
    if (++freelist.n > freelist.c) {
      freelist.c = freelist.n + (freelist.n >> 1);
      freelist.p = xrealloc(freelist.p, freelist.c * sizeof(*freelist.p));
    }
    freelist.p[freelist.n - 1] = p;
  }
  return p;
}

static void UnmapLater(int f, void *p, size_t n) {
  if (++unmaplist.n > unmaplist.c) {
    unmaplist.c = unmaplist.n + (unmaplist.n >> 1);
    unmaplist.p = xrealloc(unmaplist.p, unmaplist.c * sizeof(*unmaplist.p));
  }
  unmaplist.p[unmaplist.n - 1].f = f;
  unmaplist.p[unmaplist.n - 1].p = p;
  unmaplist.p[unmaplist.n - 1].n = n;
}

static void CollectGarbage(void) {
  __log_level = oldloglevel;
  DestroyHttpMessage(&msg);
  while (freelist.n) {
    free(freelist.p[--freelist.n]);
  }
  while (unmaplist.n) {
    --unmaplist.n;
    LOGIFNEG1(munmap(unmaplist.p[unmaplist.n].p, unmaplist.p[unmaplist.n].n));
    LOGIFNEG1(close(unmaplist.p[unmaplist.n].f));
  }
}

static void UseOutput(void) {
  content = FreeLater(outbuf.p);
  contentlength = outbuf.n;
  outbuf.p = 0;
  outbuf.n = 0;
  outbuf.c = 0;
}

static void DropOutput(void) {
  FreeLater(outbuf.p);
  outbuf.p = 0;
  outbuf.n = 0;
  outbuf.c = 0;
}

static void ClearOutput(void) {
  outbuf.n = 0;
}

static void Grow(size_t n) {
  do {
    if (outbuf.c) {
      outbuf.c += outbuf.c >> 1;
    } else {
      outbuf.c = 16 * 1024;
    }
  } while (n > outbuf.c);
  outbuf.p = xrealloc(outbuf.p, outbuf.c);
}

static void AppendData(const char *data, size_t size) {
  size_t n;
  n = outbuf.n + size;
  if (n > outbuf.c) Grow(n);
  memcpy(outbuf.p + outbuf.n, data, size);
  outbuf.n = n;
}

static void Append(const char *fmt, ...) {
  int n;
  char *p;
  va_list va, vb;
  va_start(va, fmt);
  va_copy(vb, va);
  n = vsnprintf(outbuf.p + outbuf.n, outbuf.c - outbuf.n, fmt, va);
  if (n >= outbuf.c - outbuf.n) {
    Grow(outbuf.n + n + 1);
    vsnprintf(outbuf.p + outbuf.n, outbuf.c - outbuf.n, fmt, vb);
  }
  va_end(vb);
  va_end(va);
  outbuf.n += n;
}

static char *MergePaths(const char *p, size_t n, const char *q, size_t m,
                        size_t *z) {
  char *r;
  if (n && p[n - 1] == '/') --n;
  if (m && q[0] == '/') ++q, --m;
  r = xmalloc(n + 1 + m + 1);
  mempcpy(mempcpy(mempcpy(mempcpy(r, p, n), "/", 1), q, m), "", 1);
  if (z) *z = n + 1 + m;
  return r;
}

static long FindRedirect(const char *s, size_t n) {
  int c, m, l, r, z;
  l = 0;
  r = redirects.n - 1;
  while (l <= r) {
    m = (l + r) >> 1;
    c = CompareSlices(redirects.p[m].path.s, redirects.p[m].path.n, s, n);
    if (c < 0) {
      l = m + 1;
    } else if (c > 0) {
      r = m - 1;
    } else {
      return m;
    }
  }
  return -1;
}

static void LogCertificate(const char *msg, mbedtls_x509_crt *cert) {
  char *s;
  size_t n;
  if (LOGGABLE(kLogDebug)) {
    if ((s = gc(malloc((n = 15000))))) {
      if (mbedtls_x509_crt_info(s, n, " ", cert) > 0) {
        DEBUGF("%s\n%s", msg, chomp(s));
      }
    }
  }
}

static char *FormatX509Name(mbedtls_x509_name *name) {
  char *s = calloc(1, 1000);
  CHECK_GT(mbedtls_x509_dn_gets(s, 1000, name), 0);
  return s;
}

static bool IsSelfSigned(mbedtls_x509_crt *cert) {
  return !mbedtls_x509_name_cmp(&cert->issuer, &cert->subject);
}

static mbedtls_x509_crt *GetTrustedCertificate(mbedtls_x509_name *name) {
  size_t i;
  for (i = 0; i < certs.n; ++i) {
    if (certs.p[i].cert &&
        !mbedtls_x509_name_cmp(name, &certs.p[i].cert->subject)) {
      return certs.p[i].cert;
    }
  }
  return 0;
}

static void UseCertificate(mbedtls_ssl_config *c, struct Cert *kp) {
  VERBOSEF("using %s certificate %`'s", mbedtls_pk_get_name(&kp->cert->pk),
           gc(FormatX509Name(&kp->cert->subject)));
  CHECK_EQ(0, mbedtls_ssl_conf_own_cert(c, kp->cert, kp->key));
}

static bool ChainCertificate(mbedtls_x509_crt *cert, mbedtls_x509_crt *parent) {
  if (!mbedtls_x509_crt_check_signature(cert, parent, 0)) {
    DEBUGF("chaining %`'s to %`'s", gc(FormatX509Name(&cert->subject)),
           gc(FormatX509Name(&parent->subject)));
    cert->next = parent;
    return true;
  } else {
    WARNF("signature check failed for %`'s -> %`'s",
          gc(FormatX509Name(&cert->subject)),
          gc(FormatX509Name(&parent->subject)));
    return false;
  }
}

static void AppendCert(mbedtls_x509_crt *cert, mbedtls_pk_context *key) {
  certs.p = realloc(certs.p, ++certs.n * sizeof(*certs.p));
  certs.p[certs.n - 1].cert = cert;
  certs.p[certs.n - 1].key = key;
}

static void InternCertificate(mbedtls_x509_crt *cert, mbedtls_x509_crt *prev) {
  int r;
  size_t i;
  if (cert->next) InternCertificate(cert->next, cert);
  if (prev) {
    if (mbedtls_x509_crt_check_parent(prev, cert, 1)) {
      DEBUGF("unbundling %`'s from %`'s", gc(FormatX509Name(&prev->subject)),
             gc(FormatX509Name(&cert->subject)));
      prev->next = 0;
    } else if ((r = mbedtls_x509_crt_check_signature(prev, cert, 0))) {
      WARNF("invalid signature for %`'s -> %`'s (-0x%04x)",
            gc(FormatX509Name(&prev->subject)),
            gc(FormatX509Name(&cert->subject)), -r);
    }
  }
  if (mbedtls_x509_time_is_past(&cert->valid_to)) {
    WARNF("certificate is expired", gc(FormatX509Name(&cert->subject)));
  } else if (mbedtls_x509_time_is_future(&cert->valid_from)) {
    WARNF("certificate is from the future", gc(FormatX509Name(&cert->subject)));
  }
  for (i = 0; i < certs.n; ++i) {
    if (!certs.p[i].cert) continue;
    if (mbedtls_pk_get_type(&cert->pk) ==
            mbedtls_pk_get_type(&certs.p[i].cert->pk) &&
        !mbedtls_x509_name_cmp(&cert->subject, &certs.p[i].cert->subject)) {
      VERBOSEF("%s %`'s is already loaded", mbedtls_pk_get_name(&cert->pk),
               gc(FormatX509Name(&cert->subject)));
      return;
    }
  }
  for (i = 0; i < certs.n; ++i) {
    if (!certs.p[i].cert && certs.p[i].key &&
        !mbedtls_pk_check_pair(&cert->pk, certs.p[i].key)) {
      certs.p[i].cert = cert;
      return;
    }
  }
  LogCertificate("loaded certificate", cert);
  if (!cert->next && !IsSelfSigned(cert)) {
    for (i = 0; i < certs.n; ++i) {
      if (!certs.p[i].cert) continue;
      if (mbedtls_pk_can_do(&cert->pk, certs.p[i].cert->sig_pk) &&
          !mbedtls_x509_crt_check_parent(cert, certs.p[i].cert, 1) &&
          !IsSelfSigned(certs.p[i].cert)) {
        if (ChainCertificate(cert, certs.p[i].cert)) break;
      }
    }
  }
  if (!IsSelfSigned(cert)) {
    for (i = 0; i < certs.n; ++i) {
      if (!certs.p[i].cert) continue;
      if (certs.p[i].cert->next) continue;
      if (mbedtls_pk_can_do(&certs.p[i].cert->pk, cert->sig_pk) &&
          !mbedtls_x509_crt_check_parent(certs.p[i].cert, cert, 1)) {
        ChainCertificate(certs.p[i].cert, cert);
      }
    }
  }
  AppendCert(cert, 0);
}

static void ProgramCertificate(const char *p, size_t n) {
  int rc;
  unsigned char *waqapi;
  mbedtls_x509_crt *cert;
  waqapi = malloc(n + 1);
  memcpy(waqapi, p, n);
  waqapi[n] = 0;
  cert = calloc(1, sizeof(mbedtls_x509_crt));
  rc = mbedtls_x509_crt_parse(cert, waqapi, n + 1);
  mbedtls_platform_zeroize(waqapi, n);
  free(waqapi);
  if (rc < 0) {
    WARNF("failed to load certificate (grep -0x%04x)\n", rc);
    return;
  } else if (rc > 0) {
    VERBOSEF("certificate bundle partially loaded");
  }
  InternCertificate(cert, 0);
}

static void ProgramPrivateKey(const char *p, size_t n) {
  int rc;
  size_t i;
  unsigned char *waqapi;
  mbedtls_pk_context *key;
  waqapi = malloc(n + 1);
  memcpy(waqapi, p, n);
  waqapi[n] = 0;
  key = calloc(1, sizeof(mbedtls_pk_context));
  rc = mbedtls_pk_parse_key(key, waqapi, n + 1, 0, 0);
  mbedtls_platform_zeroize(waqapi, n);
  free(waqapi);
  if (rc != 0) {
    fprintf(stderr, "error: load key (grep -0x%04x)\n", -rc);
    exit(1);
  }
  for (i = 0; i < certs.n; ++i) {
    if (certs.p[i].cert && !certs.p[i].key &&
        !mbedtls_pk_check_pair(&certs.p[i].cert->pk, key)) {
      certs.p[i].key = key;
      return;
    }
  }
  VERBOSEF("loaded private key");
  AppendCert(0, key);
}

static void ProgramFile(const char *path, void program(const char *, size_t)) {
  char *p;
  size_t n;
  DEBUGF("ProgramFile(%`'s)", path);
  if ((p = xslurp(path, &n))) {
    program(p, n);
    mbedtls_platform_zeroize(p, n);
    free(p);
  } else {
    fprintf(stderr, "error: failed to read file: %s\n", path);
    exit(1);
  }
}

static void ProgramPort(long port) {
  if (!(0 <= port && port <= 65535)) {
    fprintf(stderr, "error: bad port: %d\n", port);
    exit(1);
  }
  if (port == 443) listeningonport443 = true;
  ports.p = realloc(ports.p, ++ports.n * sizeof(*ports.p));
  ports.p[ports.n - 1] = port;
}

static void ProgramMaxPayloadSize(long x) {
  maxpayloadsize = MAX(1450, x);
}

static uint32_t ResolveIp(const char *addr) {
  ssize_t rc;
  uint32_t ip;
  struct addrinfo *ai = NULL;
  struct addrinfo hint = {AI_NUMERICSERV, AF_INET, SOCK_STREAM, IPPROTO_TCP};
  if ((rc = getaddrinfo(addr, "0", &hint, &ai)) != EAI_SUCCESS) {
    fprintf(stderr, "error: bad addr: %s (EAI_%s)\n", addr, gai_strerror(rc));
    exit(1);
  }
  ip = ntohl(ai->ai_addr4->sin_addr.s_addr);
  freeaddrinfo(ai);
  return ip;
}

static void ProgramAddr(const char *addr) {
  uint32_t ip;
  if (IsTiny()) {
    ip = ParseIp(addr, -1);
  } else {
    ip = ResolveIp(addr);
  }
  ips.p = realloc(ips.p, ++ips.n * sizeof(*ips.p));
  ips.p[ips.n - 1] = ip;
}

static void ProgramRedirect(int code, const char *sp, size_t sn, const char *dp,
                            size_t dn) {
  long i, j;
  struct Redirect r;
  if (code && code != 301 && code != 302 && code != 307 && code != 308) {
    fprintf(stderr, "error: unsupported redirect code %d\n", code);
    exit(1);
  }
  r.code = code;
  r.path.s = sp;
  r.path.n = sn;
  r.location.s = dp;
  r.location.n = dn;
  if ((i = FindRedirect(r.path.s, r.path.n)) != -1) {
    redirects.p[i] = r;
  } else {
    i = redirects.n;
    redirects.p = xrealloc(redirects.p, (i + 1) * sizeof(*redirects.p));
    for (j = i; j; --j) {
      if (CompareSlices(r.path.s, r.path.n, redirects.p[j - 1].path.s,
                        redirects.p[j - 1].path.n) < 0) {
        redirects.p[j] = redirects.p[j - 1];
      } else {
        break;
      }
    }
    redirects.p[j] = r;
    ++redirects.n;
  }
}

static void ProgramRedirectArg(int code, const char *s) {
  size_t n;
  const char *p;
  n = strlen(s);
  if (!(p = memchr(s, '=', n))) {
    fprintf(stderr, "error: redirect arg missing '='\n");
    exit(1);
  }
  ProgramRedirect(code, s, p - s, p + 1, n - (p - s + 1));
}

static void DescribeAddress(char buf[32], uint32_t addr, uint16_t port) {
  char *p;
  const char *s;
  p = buf;
  p += uint64toarray_radix10((addr & 0xFF000000) >> 030, p), *p++ = '.';
  p += uint64toarray_radix10((addr & 0x00FF0000) >> 020, p), *p++ = '.';
  p += uint64toarray_radix10((addr & 0x0000FF00) >> 010, p), *p++ = '.';
  p += uint64toarray_radix10((addr & 0x000000FF) >> 000, p), *p++ = ':';
  p += uint64toarray_radix10(port, p);
  *p++ = '\0';
}

static inline void GetServerAddr(uint32_t *ip, uint16_t *port) {
  *ip = ntohl(serveraddr->sin_addr.s_addr);
  if (port) *port = ntohs(serveraddr->sin_port);
}

static inline void GetClientAddr(uint32_t *ip, uint16_t *port) {
  *ip = ntohl(clientaddr.sin_addr.s_addr);
  if (port) *port = ntohs(clientaddr.sin_port);
}

static inline void GetRemoteAddr(uint32_t *ip, uint16_t *port) {
  GetClientAddr(ip, port);
  if (HasHeader(kHttpXForwardedFor) &&
      (IsPrivateIp(*ip) || IsLoopbackIp(*ip))) {
    ParseForwarded(HeaderData(kHttpXForwardedFor),
                   HeaderLength(kHttpXForwardedFor), ip, port);
  }
}

static char *DescribeClient(void) {
  uint32_t ip;
  uint16_t port;
  static char clientaddrstr[32];
  GetRemoteAddr(&ip, &port);
  DescribeAddress(clientaddrstr, ip, port);
  return clientaddrstr;
}

static char *DescribeServer(void) {
  uint32_t ip;
  uint16_t port;
  static char serveraddrstr[32];
  GetServerAddr(&ip, &port);
  DescribeAddress(serveraddrstr, ip, port);
  return serveraddrstr;
}

static void ProgramBrand(const char *s) {
  char *p;
  free(brand);
  free(serverheader);
  if (!(p = EncodeHttpHeaderValue(s, -1, 0))) {
    fprintf(stderr, "error: brand isn't latin1 encodable: %`'s", s);
    exit(1);
  }
  brand = strdup(s);
  serverheader = xasprintf("Server: %s\r\n", p);
  free(p);
}

static void ProgramUid(long x) {
  changeuid = x;
}

static void ProgramGid(long x) {
  changegid = x;
}

static void ProgramTimeout(long ms) {
  ldiv_t d;
  if (ms < 0) {
    timeout.tv_sec = ms; /* -(keepalive seconds) */
    timeout.tv_usec = 0;
  } else {
    if (ms <= 30) {
      fprintf(stderr, "error: timeout needs to be 31ms or greater\n");
      exit(1);
    }
    d = ldiv(ms, 1000);
    timeout.tv_sec = d.quot;
    timeout.tv_usec = d.rem * 1000;
  }
}

static void ProgramCache(long x) {
  cacheseconds = x;
}

static void SetDefaults(void) {
  ProgramBrand(REDBEAN "/1.4");
  __log_level = kLogInfo;
  maxpayloadsize = 64 * 1024;
  ProgramCache(-1);
  ProgramTimeout(60 * 1000);
  sslfetchverify = true;
  if (IsWindows()) uniprocess = true;
}

static void AddString(struct Strings *l, const char *s, size_t n) {
  if (++l->n > l->c) {
    l->c = l->n + (l->n >> 1);
    l->p = realloc(l->p, l->c * sizeof(*l->p));
  }
  l->p[l->n - 1].s = s;
  l->p[l->n - 1].n = n;
}

static bool HasString(struct Strings *l, const char *s, size_t n) {
  size_t i;
  for (i = 0; i < l->n; ++i) {
    if (SlicesEqual(l->p[i].s, l->p[i].n, s, n)) {
      return true;
    }
  }
  return false;
}

static void ProgramDirectory(const char *path) {
  char *s;
  size_t n;
  s = strdup(path);
  n = strlen(s);
  while (n && (s[n - 1] == '/' || s[n - 1] == '\\')) s[--n] = 0;
  if (!n || !isdirectory(s)) {
    fprintf(stderr, "error: not a directory: %`'s\n", s);
    exit(1);
  }
  AddString(&stagedirs, s, n);
}

static void ProgramHeader(const char *s) {
  char *p, *v, *h;
  if ((p = strchr(s, ':')) && IsValidHttpToken(s, p - s) &&
      (v = EncodeLatin1(p + 1, -1, 0, kControlC0 | kControlC1 | kControlWs))) {
    switch (GetHttpHeader(s, p - s)) {
      case kHttpDate:
      case kHttpConnection:
      case kHttpContentLength:
      case kHttpContentEncoding:
      case kHttpContentRange:
        fprintf(stderr, "error: can't program header: %`'s\n", s);
        exit(1);
      case kHttpServer:
        ProgramBrand(p + 1);
        break;
      default:
        p = xasprintf("%s%.*s:%s\r\n", extrahdrs ? extrahdrs : "", p - s, s, v);
        free(extrahdrs);
        extrahdrs = p;
        break;
    }
    free(v);
  } else {
    fprintf(stderr, "error: illegal header: %`'s\n", s);
    exit(1);
  }
}

static void ProgramLogPath(const char *s) {
  logpath = strdup(s);
}

static void ProgramPidPath(const char *s) {
  pidpath = strdup(s);
}

static bool IsServerFd(int fd) {
  size_t i;
  for (i = 0; i < servers.n; ++i) {
    if (servers.p[i].fd == fd) {
      return true;
    }
  }
  return false;
}

static void ChangeUser(void) {
  if (changegid) LOGIFNEG1(setgid(changegid));
  if (changeuid) LOGIFNEG1(setuid(changeuid));
}

static void Daemonize(void) {
  char ibuf[21];
  int i, fd, pid;
  for (i = 0; i < 256; ++i) {
    if (!IsServerFd(i)) {
      close(i);
    }
  }
  if ((pid = fork()) > 0) exit(0);
  setsid();
  if ((pid = fork()) > 0) _exit(0);
  umask(0);
  if (pidpath) {
    fd = open(pidpath, O_CREAT | O_WRONLY, 0644);
    write(fd, ibuf, uint64toarray_radix10(getpid(), ibuf));
    close(fd);
  }
  if (!logpath) ProgramLogPath("/dev/null");
  open("/dev/null", O_RDONLY);
  open(logpath, O_APPEND | O_WRONLY | O_CREAT, 0640);
  dup2(1, 2);
  ChangeUser();
}

static bool LuaOnClientConnection(void) {
  bool dropit;
  uint32_t ip, serverip;
  uint16_t port, serverport;
  lua_getglobal(L, "OnClientConnection");
  GetClientAddr(&ip, &port);
  GetServerAddr(&serverip, &serverport);
  lua_pushnumber(L, ip);
  lua_pushnumber(L, port);
  lua_pushnumber(L, serverip);
  lua_pushnumber(L, serverport);
  if (lua_pcall(L, 4, 1, 0) == LUA_OK) {
    dropit = lua_toboolean(L, -1);
  } else {
    WARNF("%s: %s", "OnClientConnection", lua_tostring(L, -1));
    dropit = false;
  }
  lua_pop(L, 1);
  return dropit;
}

static void LuaOnProcessCreate(int pid) {
  uint32_t ip, serverip;
  uint16_t port, serverport;
  lua_getglobal(L, "OnProcessCreate");
  GetClientAddr(&ip, &port);
  GetServerAddr(&serverip, &serverport);
  lua_pushnumber(L, pid);
  lua_pushnumber(L, ip);
  lua_pushnumber(L, port);
  lua_pushnumber(L, serverip);
  lua_pushnumber(L, serverport);
  if (lua_pcall(L, 5, 0, 0) != LUA_OK) {
    WARNF("%s: %s", "OnProcessCreate", lua_tostring(L, -1));
    lua_pop(L, 1);
  }
}

static void LuaOnProcessDestroy(int pid) {
  lua_getglobal(L, "OnProcessDestroy");
  lua_pushnumber(L, pid);
  if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
    WARNF("%s: %s", "OnProcessDestroy", lua_tostring(L, -1));
    lua_pop(L, 1);
  }
}

static inline bool IsHookDefined(const char *s) {
#ifndef STATIC
  bool res = !!lua_getglobal(L, s);
  lua_pop(L, 1);
  return res;
#else
  return false;
#endif
}

static void CallSimpleHook(const char *s) {
  lua_getglobal(L, s);
  if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
    WARNF("%s: %s", s, lua_tostring(L, -1));
    lua_pop(L, 1);
  }
}

static void CallSimpleHookIfDefined(const char *s) {
  if (IsHookDefined(s)) {
    CallSimpleHook(s);
  }
}

static void ReportWorkerExit(int pid, int ws) {
  --shared->workers;
  if (WIFEXITED(ws)) {
    if (WEXITSTATUS(ws)) {
      LockInc(&shared->c.failedchildren);
      WARNF("%d exited with %d (%,d workers remain)", pid, WEXITSTATUS(ws),
            shared->workers);
    } else {
      DEBUGF("%d exited (%,d workers remain)", pid, shared->workers);
    }
  } else {
    LockInc(&shared->c.terminatedchildren);
    WARNF("%d terminated with %s (%,d workers remain)", pid,
          strsignal(WTERMSIG(ws)), shared->workers);
  }
}

static void AppendResourceReport(struct rusage *ru, const char *nl) {
  long utime, stime;
  long double ticks;
  if (ru->ru_maxrss) {
    Append("ballooned to %,ldkb in size%s", ru->ru_maxrss, nl);
  }
  if ((utime = ru->ru_utime.tv_sec * 1000000 + ru->ru_utime.tv_usec) |
      (stime = ru->ru_stime.tv_sec * 1000000 + ru->ru_stime.tv_usec)) {
    ticks = ceill((long double)(utime + stime) / (1000000.L / CLK_TCK));
    Append("needed %,ldµs cpu (%d%% kernel)%s", utime + stime,
           (int)((long double)stime / (utime + stime) * 100), nl);
    if (ru->ru_idrss) {
      Append("needed %,ldkb memory on average%s", lroundl(ru->ru_idrss / ticks),
             nl);
    }
    if (ru->ru_isrss) {
      Append("needed %,ldkb stack on average%s", lroundl(ru->ru_isrss / ticks),
             nl);
    }
    if (ru->ru_ixrss) {
      Append("mapped %,ldkb shared on average%s", lroundl(ru->ru_ixrss / ticks),
             nl);
    }
  }
  if (ru->ru_minflt || ru->ru_majflt) {
    Append("caused %,ld page faults (%d%% memcpy)%s",
           ru->ru_minflt + ru->ru_majflt,
           (int)((long double)ru->ru_minflt / (ru->ru_minflt + ru->ru_majflt) *
                 100),
           nl);
  }
  if (ru->ru_nvcsw + ru->ru_nivcsw > 1) {
    Append(
        "%,ld context switches (%d%% consensual)%s",
        ru->ru_nvcsw + ru->ru_nivcsw,
        (int)((long double)ru->ru_nvcsw / (ru->ru_nvcsw + ru->ru_nivcsw) * 100),
        nl);
  }
  if (ru->ru_msgrcv || ru->ru_msgsnd) {
    Append("received %,ld message%s and sent %,ld%s", ru->ru_msgrcv,
           ru->ru_msgrcv == 1 ? "" : "s", ru->ru_msgsnd, nl);
  }
  if (ru->ru_inblock || ru->ru_oublock) {
    Append("performed %,ld read%s and %,ld write i/o operations%s",
           ru->ru_inblock, ru->ru_inblock == 1 ? "" : "s", ru->ru_oublock, nl);
  }
  if (ru->ru_nsignals) {
    Append("received %,ld signals%s", ru->ru_nsignals, nl);
  }
  if (ru->ru_nswap) {
    Append("got swapped %,ld times%s", ru->ru_nswap, nl);
  }
}

static void AddTimeval(struct timeval *x, const struct timeval *y) {
  x->tv_sec += y->tv_sec;
  x->tv_usec += y->tv_usec;
  if (x->tv_usec >= 1000000) {
    x->tv_usec -= 1000000;
    x->tv_sec += 1;
  }
}

static void AddRusage(struct rusage *x, const struct rusage *y) {
  AddTimeval(&x->ru_utime, &y->ru_utime);
  AddTimeval(&x->ru_stime, &y->ru_stime);
  x->ru_maxrss = MAX(x->ru_maxrss, y->ru_maxrss);
  x->ru_ixrss += y->ru_ixrss;
  x->ru_idrss += y->ru_idrss;
  x->ru_isrss += y->ru_isrss;
  x->ru_minflt += y->ru_minflt;
  x->ru_majflt += y->ru_majflt;
  x->ru_nswap += y->ru_nswap;
  x->ru_inblock += y->ru_inblock;
  x->ru_oublock += y->ru_oublock;
  x->ru_msgsnd += y->ru_msgsnd;
  x->ru_msgrcv += y->ru_msgrcv;
  x->ru_nsignals += y->ru_nsignals;
  x->ru_nvcsw += y->ru_nvcsw;
  x->ru_nivcsw += y->ru_nivcsw;
}

static void ReportWorkerResources(int pid, struct rusage *ru) {
  const char *s;
  if (logrusage || LOGGABLE(kLogDebug)) {
    AppendResourceReport(ru, "\n");
    if (outbuf.n) {
      if ((s = IndentLines(outbuf.p, outbuf.n - 1, 0, 1))) {
        flogf(kLogInfo, __FILE__, __LINE__, NULL,
              "resource report for pid %d\n%s", pid, s);
        free(s);
      }
      ClearOutput();
    }
  }
}

static void HandleWorkerExit(int pid, int ws, struct rusage *ru) {
  LockInc(&shared->c.connectionshandled);
  AddRusage(&shared->children, ru);
  ReportWorkerExit(pid, ws);
  ReportWorkerResources(pid, ru);
  if (hasonprocessdestroy) {
    LuaOnProcessDestroy(pid);
  }
}

static void WaitAll(void) {
  int ws, pid;
  struct rusage ru;
  for (;;) {
    if ((pid = wait4(-1, &ws, 0, &ru)) != -1) {
      HandleWorkerExit(pid, ws, &ru);
    } else {
      if (errno == ECHILD) break;
      if (errno == EINTR) {
        if (killed) {
          killed = false;
          terminated = false;
          WARNF("redbean shall terminate harder");
          LOGIFNEG1(kill(0, SIGTERM));
        }
        continue;
      }
      FATALF("wait error %s", strerror(errno));
    }
  }
}

static void ReapZombies(void) {
  int ws, pid;
  struct rusage ru;
  do {
    zombied = false;
    if ((pid = wait4(-1, &ws, WNOHANG, &ru)) != -1) {
      if (pid) {
        HandleWorkerExit(pid, ws, &ru);
      } else {
        break;
      }
    } else {
      if (errno == ECHILD) break;
      if (errno == EINTR) continue;
      FATALF("wait error %s", strerror(errno));
    }
  } while (!terminated);
}

static ssize_t WritevAll(int fd, struct iovec *iov, int iovlen) {
  int i;
  ssize_t rc;
  size_t wrote, total;
  i = 0;
  total = 0;
  do {
    if (i) {
      while (i < iovlen && !iov[i].iov_len) ++i;
      if (i == iovlen) break;
    }
    if ((rc = writev(fd, iov + i, iovlen - i)) != -1) {
      wrote = rc;
      total += wrote;
      do {
        if (wrote >= iov[i].iov_len) {
          wrote -= iov[i++].iov_len;
        } else {
          iov[i].iov_base = (char *)iov[i].iov_base + wrote;
          iov[i].iov_len -= wrote;
          wrote = 0;
        }
      } while (wrote);
    } else if (errno == EINTR) {
      LockInc(&shared->c.writeinterruputs);
      if (killed || (meltdown && nowl() - startread > 2)) {
        return total ? total : -1;
      }
    } else {
      return total ? total : -1;
    }
  } while (i < iovlen);
  return total;
}

static int TlsFlush(struct TlsBio *bio, const unsigned char *buf, size_t len) {
  struct iovec v[2];
  if (len || bio->c > 0) {
    v[0].iov_base = bio->u;
    v[0].iov_len = MAX(0, bio->c);
    v[1].iov_base = buf;
    v[1].iov_len = len;
    if (WritevAll(bio->fd, v, 2) != -1) {
      if (bio->c > 0) bio->c = 0;
    } else if (errno == EINTR) {
      return MBEDTLS_ERR_NET_CONN_RESET;
    } else if (errno == EAGAIN) {
      return MBEDTLS_ERR_SSL_TIMEOUT;
    } else if (errno == EPIPE || errno == ECONNRESET || errno == ENETRESET) {
      return MBEDTLS_ERR_NET_CONN_RESET;
    } else {
      WARNF("TlsSend error %s", strerror(errno));
      return MBEDTLS_ERR_NET_SEND_FAILED;
    }
  }
  return 0;
}

static int TlsSend(void *ctx, const unsigned char *buf, size_t len) {
  int rc;
  struct iovec v[2];
  struct TlsBio *bio = ctx;
  if (bio->c >= 0 && bio->c + len <= sizeof(bio->u)) {
    memcpy(bio->u + bio->c, buf, len);
    bio->c += len;
    return len;
  }
  if ((rc = TlsFlush(bio, buf, len)) < 0) return rc;
  return len;
}

static int TlsRecvImpl(void *ctx, unsigned char *p, size_t n, uint32_t o) {
  int r;
  ssize_t s;
  struct iovec v[2];
  struct TlsBio *bio = ctx;
  if ((r = TlsFlush(bio, 0, 0)) < 0) return r;
  if (bio->a < bio->b) {
    r = MIN(n, bio->b - bio->a);
    memcpy(p, bio->t + bio->a, r);
    if ((bio->a += r) == bio->b) bio->a = bio->b = 0;
    return r;
  }
  v[0].iov_base = p;
  v[0].iov_len = n;
  v[1].iov_base = bio->t;
  v[1].iov_len = sizeof(bio->t);
  while ((r = readv(bio->fd, v, 2)) == -1) {
    if (errno == EINTR) {
      return MBEDTLS_ERR_SSL_WANT_READ;
    } else if (errno == EAGAIN) {
      return MBEDTLS_ERR_SSL_TIMEOUT;
    } else if (errno == EPIPE || errno == ECONNRESET || errno == ENETRESET) {
      return MBEDTLS_ERR_NET_CONN_RESET;
    } else {
      WARNF("tls read() error %s", strerror(errno));
      return MBEDTLS_ERR_NET_RECV_FAILED;
    }
  }
  if (r > n) bio->b = r - n;
  return MIN(n, r);
}

static int TlsRecv(void *ctx, unsigned char *buf, size_t len, uint32_t tmo) {
  int rc;
  struct TlsBio *bio = ctx;
  if (oldin.n) {
    rc = MIN(oldin.n, len);
    memcpy(buf, oldin.p, rc);
    oldin.p += rc;
    oldin.n -= rc;
    return rc;
  }
  return TlsRecvImpl(ctx, buf, len, tmo);
}

static void TlsDebug(void *ctx, int level, const char *file, int line,
                     const char *message) {
  flogf(level, file, line, 0, "TLS %s", message);
}

static ssize_t SslRead(int fd, void *buf, size_t size) {
  int rc;
  rc = mbedtls_ssl_read(&ssl, buf, size);
  if (!rc) {
    errno = ECONNRESET;
    rc = -1;
  } else if (rc < 0) {
    if (rc == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
      rc = 0;
    } else if (rc == MBEDTLS_ERR_NET_CONN_RESET ||
               rc == MBEDTLS_ERR_SSL_TIMEOUT) {
      errno = ECONNRESET;
      rc = -1;
    } else if (rc == MBEDTLS_ERR_SSL_WANT_READ) {
      errno = EINTR;
      rc = -1;
    } else {
      WARNF("%s SslRead error -0x%04x", DescribeClient(), -rc);
      errno = EIO;
      rc = -1;
    }
  }
  return rc;
}

static ssize_t SslWrite(int fd, struct iovec *iov, int iovlen) {
  int i;
  size_t n;
  ssize_t rc;
  const unsigned char *p;
  for (i = 0; i < iovlen; ++i) {
    p = iov[i].iov_base;
    n = iov[i].iov_len;
    while (n) {
      if ((rc = mbedtls_ssl_write(&ssl, p, n)) > 0) {
        p += rc;
        n -= rc;
      } else {
        WARNF("%s SslWrite error -0x%04x", DescribeClient(), -rc);
        errno = EIO;
        return -1;
      }
    }
  }
  return 0;
}

static void NotifyClose(void) {
#ifndef UNSECURE
  if (usessl) {
    DEBUGF("SSL notifying close");
    mbedtls_ssl_close_notify(&ssl);
  }
#endif
}

static void WipeKeySigningKeys(void) {
  size_t i;
  for (i = 0; i < certs.n; ++i) {
    if (!certs.p[i].key) continue;
    if (!certs.p[i].cert) continue;
    if (!certs.p[i].cert->ca_istrue) continue;
    mbedtls_pk_free(certs.p[i].key);
    certs.p[i].key = 0;
  }
}

static void WipeServingKeys(void) {
  mbedtls_ssl_ticket_free(&ssltick);
  mbedtls_ssl_key_cert_free(conf.key_cert);
}

static bool TlsSetup(void) {
  int r;
  oldin.p = inbuf.p;
  oldin.n = amtread;
  inbuf.p += amtread;
  inbuf.n -= amtread;
  inbuf.c = amtread;
  amtread = 0;
  g_bio.fd = client;
  g_bio.a = 0;
  g_bio.b = 0;
  g_bio.c = 0;
  for (;;) {
    if (!(r = mbedtls_ssl_handshake(&ssl)) && TlsFlush(&g_bio, 0, 0) != -1) {
      LockInc(&shared->c.sslhandshakes);
      g_bio.c = -1;
      usessl = true;
      reader = SslRead;
      writer = SslWrite;
      WipeServingKeys();
      VERBOSEF("SHAKEN %s %s %s", DescribeClient(),
               mbedtls_ssl_get_ciphersuite(&ssl),
               mbedtls_ssl_get_version(&ssl));
      return true;
    } else if (r == MBEDTLS_ERR_SSL_WANT_READ) {
      LockInc(&shared->c.handshakeinterrupts);
      if (terminated || killed || (meltdown && nowl() - startread > 2)) {
        return false;
      }
    } else {
      LockInc(&shared->c.sslhandshakefails);
      mbedtls_ssl_session_reset(&ssl);
      switch (r) {
        case MBEDTLS_ERR_SSL_CONN_EOF:
          DEBUGF("%s SSL handshake EOF", DescribeClient());
          return false;
        case MBEDTLS_ERR_NET_CONN_RESET:
          DEBUGF("%s SSL handshake reset", DescribeClient());
          return false;
        case MBEDTLS_ERR_SSL_TIMEOUT:
          LockInc(&shared->c.ssltimeouts);
          DEBUGF("%s %s", DescribeClient(), "ssltimeouts");
          return false;
        case MBEDTLS_ERR_SSL_NO_CIPHER_CHOSEN:
          LockInc(&shared->c.sslnociphers);
          WARNF("%s %s", DescribeClient(), "sslnociphers");
          return false;
        case MBEDTLS_ERR_SSL_NO_USABLE_CIPHERSUITE:
          LockInc(&shared->c.sslcantciphers);
          WARNF("%s %s", DescribeClient(), "sslcantciphers");
          return false;
        case MBEDTLS_ERR_SSL_BAD_HS_PROTOCOL_VERSION:
          LockInc(&shared->c.sslnoversion);
          WARNF("%s %s", DescribeClient(), "sslnoversion");
          return false;
        case MBEDTLS_ERR_SSL_INVALID_MAC:
          LockInc(&shared->c.sslshakemacs);
          WARNF("%s %s", DescribeClient(), "sslshakemacs");
          return false;
        case MBEDTLS_ERR_SSL_NO_CLIENT_CERTIFICATE:
          LockInc(&shared->c.sslnoclientcert);
          WARNF("%s %s", DescribeClient(), "sslnoclientcert");
          NotifyClose();
          return false;
        case MBEDTLS_ERR_X509_CERT_VERIFY_FAILED:
          LockInc(&shared->c.sslverifyfailed);
          WARNF("%s SSL %s", DescribeClient(),
                gc(DescribeSslVerifyFailure(
                    ssl.session_negotiate->verify_result)));
          return false;
        case MBEDTLS_ERR_SSL_FATAL_ALERT_MESSAGE:
          switch (ssl.fatal_alert) {
            case MBEDTLS_SSL_ALERT_MSG_CERT_UNKNOWN:
              LockInc(&shared->c.sslunknowncert);
              DEBUGF("%s %s", DescribeClient(), "sslunknowncert");
              return false;
            case MBEDTLS_SSL_ALERT_MSG_UNKNOWN_CA:
              LockInc(&shared->c.sslunknownca);
              DEBUGF("%s %s", DescribeClient(), "sslunknownca");
              return false;
            default:
              WARNF("%s SSL shakealert %s", DescribeClient(),
                    GetAlertDescription(ssl.fatal_alert));
              return false;
          }
        default:
          WARNF("%s SSL handshake failed -0x%04x", DescribeClient(), -r);
          return false;
      }
    }
  }
}

static int GetEntropy(void *c, unsigned char *p, size_t n) {
  CHECK_EQ(n, getrandom(p, n, 0));
  return 0;
}

static void InitializeRng(mbedtls_ctr_drbg_context *r) {
  volatile unsigned char b[64];
  mbedtls_ctr_drbg_init(r);
  CHECK(getrandom(b, 64, 0) == 64);
  CHECK(!mbedtls_ctr_drbg_seed(r, GetEntropy, 0, b, 64));
  mbedtls_platform_zeroize(b, 64);
}

static void GenerateSerial(mbedtls_x509write_cert *wcert,
                           mbedtls_ctr_drbg_context *kr) {
  mbedtls_mpi x;
  mbedtls_mpi_init(&x);
  mbedtls_mpi_fill_random(&x, 128 / 8, mbedtls_ctr_drbg_random, kr);
  mbedtls_x509write_crt_set_serial(wcert, &x);
  mbedtls_mpi_free(&x);
}

static void ChooseCertificateLifetime(char notbefore[16], char notafter[16]) {
  struct tm tm;
  int64_t past, now, future, lifetime, tolerance;
  tolerance = 60 * 60 * 24;
  lifetime = 60 * 60 * 24 * 365;
  now = nowl();
  past = now - tolerance;
  future = now + tolerance + lifetime;
  FormatSslTime(notbefore, gmtime_r(&past, &tm));
  FormatSslTime(notafter, gmtime_r(&future, &tm));
}

static void ConfigureCertificate(mbedtls_x509write_cert *cw, struct Cert *ca,
                                 int usage, int type) {
  int r;
  const char *s;
  bool isduplicate;
  size_t i, j, k, nsan;
  struct HostsTxt *htxt;
  struct mbedtls_san *san;
  const mbedtls_x509_name *xname;
  char *name, *subject, *issuer, notbefore[16], notafter[16], hbuf[256];
  san = 0;
  nsan = 0;
  name = 0;
  htxt = GetHostsTxt();
  strcpy(hbuf, "localhost");
  gethostname(hbuf, sizeof(hbuf));
  for (i = 0; i < htxt->entries.i; ++i) {
    for (j = 0; j < ips.n; ++j) {
      if (ips.p[j] == READ32BE(htxt->entries.p[i].ip)) {
        isduplicate = false;
        s = htxt->strings.p + htxt->entries.p[i].name;
        if (!name) name = s;
        for (k = 0; k < nsan; ++k) {
          if (san[k].tag == MBEDTLS_X509_SAN_DNS_NAME &&
              !strcasecmp(s, san[k].val)) {
            isduplicate = true;
            break;
          }
        }
        if (!isduplicate) {
          san = realloc(san, ++nsan * sizeof(*san));
          san[nsan - 1].tag = MBEDTLS_X509_SAN_DNS_NAME;
          san[nsan - 1].val = s;
        }
      }
    }
  }
  for (i = 0; i < ips.n; ++i) {
    san = realloc(san, ++nsan * sizeof(*san));
    san[nsan - 1].tag = MBEDTLS_X509_SAN_IP_ADDRESS;
    san[nsan - 1].ip4 = ips.p[i];
  }
  ChooseCertificateLifetime(notbefore, notafter);
  subject = xasprintf("CN=%s", name ? name : hbuf);
  if (ca) {
    issuer = calloc(1, 1000);
    CHECK_GT(mbedtls_x509_dn_gets(issuer, 1000, &ca->cert->subject), 0);
  } else {
    issuer = strdup(subject);
  }
  if ((r = mbedtls_x509write_crt_set_subject_alternative_name(cw, san, nsan)) ||
      (r = mbedtls_x509write_crt_set_validity(cw, notbefore, notafter)) ||
      (r = mbedtls_x509write_crt_set_basic_constraints(cw, false, -1)) ||
      (r = mbedtls_x509write_crt_set_subject_key_identifier(cw)) ||
      (r = mbedtls_x509write_crt_set_authority_key_identifier(cw)) ||
      (r = mbedtls_x509write_crt_set_key_usage(cw, usage)) ||
      (r = mbedtls_x509write_crt_set_ext_key_usage(cw, type)) ||
      (r = mbedtls_x509write_crt_set_subject_name(cw, subject)) ||
      (r = mbedtls_x509write_crt_set_issuer_name(cw, issuer))) {
    FATALF("configure certificate (grep -0x%04x)", -r);
  }
  free(subject);
  free(issuer);
  free(san);
}

static struct Cert *GetKeySigningKey(void) {
  size_t i;
  for (i = 0; i < certs.n; ++i) {
    if (!certs.p[i].key) continue;
    if (!certs.p[i].cert) continue;
    if (!certs.p[i].cert->ca_istrue) continue;
    if (mbedtls_x509_crt_check_key_usage(certs.p[i].cert,
                                         MBEDTLS_X509_KU_KEY_CERT_SIGN)) {
      continue;
    }
    return certs.p + i;
  }
  return NULL;
}

static mbedtls_pk_context *InitializeKey(struct Cert *ca,
                                         mbedtls_x509write_cert *wcert,
                                         int type) {
  mbedtls_pk_context *k;
  mbedtls_ctr_drbg_context kr;
  k = calloc(1, sizeof(mbedtls_pk_context));
  mbedtls_x509write_crt_init(wcert);
  mbedtls_x509write_crt_set_issuer_key(wcert, ca ? ca->key : k);
  mbedtls_x509write_crt_set_subject_key(wcert, k);
  mbedtls_x509write_crt_set_md_alg(
      wcert, suiteb ? MBEDTLS_MD_SHA384 : MBEDTLS_MD_SHA256);
  mbedtls_x509write_crt_set_version(wcert, MBEDTLS_X509_CRT_VERSION_3);
  CHECK_EQ(0, mbedtls_pk_setup(k, mbedtls_pk_info_from_type(type)));
  return k;
}

static struct Cert FinishCertificate(struct Cert *ca,
                                     mbedtls_x509write_cert *wcert,
                                     mbedtls_ctr_drbg_context *kr,
                                     mbedtls_pk_context *key) {
  int i, n, rc;
  unsigned char *p;
  mbedtls_x509_crt *cert;
  p = malloc((n = FRAMESIZE));
  i = mbedtls_x509write_crt_der(wcert, p, n, mbedtls_ctr_drbg_random, kr);
  if (i < 0) FATALF("write key (grep -0x%04x)", -i);
  cert = calloc(1, sizeof(mbedtls_x509_crt));
  mbedtls_x509_crt_parse(cert, p + n - i, i);
  if (ca) cert->next = ca->cert;
  mbedtls_x509write_crt_free(wcert);
  mbedtls_ctr_drbg_free(kr);
  free(p);
  if ((rc = mbedtls_pk_check_pair(&cert->pk, key))) {
    FATALF("generate key (grep -0x%04x)", -rc);
  }
  LogCertificate(
      gc(xasprintf("generated %s certificate", mbedtls_pk_get_name(&cert->pk))),
      cert);
  return (struct Cert){cert, key};
}

static struct Cert GenerateEcpCertificate(struct Cert *ca) {
  mbedtls_pk_context *key;
  mbedtls_ctr_drbg_context kr;
  mbedtls_x509write_cert wcert;
  InitializeRng(&kr);
  key = InitializeKey(ca, &wcert, MBEDTLS_PK_ECKEY);
  CHECK_EQ(0, mbedtls_ecp_gen_key(
                  suiteb ? MBEDTLS_ECP_DP_SECP384R1 : MBEDTLS_ECP_DP_SECP256R1,
                  mbedtls_pk_ec(*key), mbedtls_ctr_drbg_random, &kr));
  GenerateSerial(&wcert, &kr);
  ConfigureCertificate(&wcert, ca, MBEDTLS_X509_KU_DIGITAL_SIGNATURE,
                       MBEDTLS_X509_NS_CERT_TYPE_SSL_SERVER |
                           MBEDTLS_X509_NS_CERT_TYPE_SSL_CLIENT);
  return FinishCertificate(ca, &wcert, &kr, key);
}

static struct Cert GenerateRsaCertificate(struct Cert *ca) {
  mbedtls_pk_context *key;
  mbedtls_ctr_drbg_context kr;
  mbedtls_x509write_cert wcert;
  InitializeRng(&kr);
  key = InitializeKey(ca, &wcert, MBEDTLS_PK_RSA);
  CHECK_EQ(0, mbedtls_rsa_gen_key(mbedtls_pk_rsa(*key), mbedtls_ctr_drbg_random,
                                  &kr, suiteb ? 4096 : 2048, 65537));
  GenerateSerial(&wcert, &kr);
  ConfigureCertificate(
      &wcert, ca,
      MBEDTLS_X509_KU_DIGITAL_SIGNATURE | MBEDTLS_X509_KU_KEY_ENCIPHERMENT,
      MBEDTLS_X509_NS_CERT_TYPE_SSL_SERVER |
          MBEDTLS_X509_NS_CERT_TYPE_SSL_CLIENT);
  return FinishCertificate(ca, &wcert, &kr, key);
}

static void LoadCertificates(void) {
  size_t i;
  struct Cert *ksk, ecp, rsa;
  bool havecert, haveclientcert;
  havecert = false;
  haveclientcert = false;
  for (i = 0; i < certs.n; ++i) {
    if (certs.p[i].key && certs.p[i].cert && !certs.p[i].cert->ca_istrue &&
        !mbedtls_x509_crt_check_key_usage(certs.p[i].cert,
                                          MBEDTLS_X509_KU_DIGITAL_SIGNATURE)) {
      if (!mbedtls_x509_crt_check_extended_key_usage(
              certs.p[i].cert, MBEDTLS_OID_SERVER_AUTH,
              MBEDTLS_OID_SIZE(MBEDTLS_OID_SERVER_AUTH))) {
        LogCertificate("using server certificate", certs.p[i].cert);
        UseCertificate(&conf, certs.p + i);
        havecert = true;
      }
      if (!mbedtls_x509_crt_check_extended_key_usage(
              certs.p[i].cert, MBEDTLS_OID_CLIENT_AUTH,
              MBEDTLS_OID_SIZE(MBEDTLS_OID_CLIENT_AUTH))) {
        LogCertificate("using client certificate", certs.p[i].cert);
        UseCertificate(&confcli, certs.p + i);
        haveclientcert = true;
      }
    }
  }
  if (!havecert || !haveclientcert) {
    if ((ksk = GetKeySigningKey())) {
      DEBUGF("generating ssl certificates using %`'s",
             gc(FormatX509Name(&ksk->cert->subject)));
    } else {
      VERBOSEF("could not find non-CA SSL certificate key pair with"
               " -addext keyUsage=digitalSignature"
               " -addext extendedKeyUsage=serverAuth");
      VERBOSEF("could not find CA key signing key pair with"
               " -addext keyUsage=keyCertSign");
      LOGF("generating self-signed ssl certificates");
    }
#ifdef MBEDTLS_ECP_C
    ecp = GenerateEcpCertificate(ksk);
    if (!havecert) UseCertificate(&conf, &ecp);
    if (!haveclientcert) UseCertificate(&confcli, &ecp);
#endif
#ifdef MBEDTLS_RSA_C
    rsa = GenerateRsaCertificate(ksk);
    if (!havecert) UseCertificate(&conf, &rsa);
    if (!haveclientcert) UseCertificate(&confcli, &rsa);
#endif
  }
  WipeKeySigningKeys();
}

static bool ClientAcceptsGzip(void) {
  return msg.version >= 10 && /* RFC1945 § 3.5 */
         HeaderHas(&msg, inbuf.p, kHttpAcceptEncoding, "gzip", 4);
}

static void UpdateCurrentDate(long double now) {
  int64_t t;
  struct tm tm;
  t = now;
  shared->nowish = now;
  gmtime_r(&t, &tm);
  FormatHttpDateTime(shared->currentdate, &tm);
}

static int64_t GetGmtOffset(int64_t t) {
  struct tm tm;
  localtime_r(&t, &tm);
  return tm.tm_gmtoff;
}

static int64_t LocoTimeToZulu(int64_t x) {
  return x - gmtoff;
}

static int64_t GetZipCfileLastModified(const uint8_t *zcf) {
  const uint8_t *p, *pe;
  for (p = ZIP_CFILE_EXTRA(zcf), pe = p + ZIP_CFILE_EXTRASIZE(zcf); p + 4 <= pe;
       p += ZIP_EXTRA_SIZE(p)) {
    if (ZIP_EXTRA_HEADERID(p) == kZipExtraNtfs &&
        ZIP_EXTRA_CONTENTSIZE(p) >= 4 + 4 + 8 &&
        READ16LE(ZIP_EXTRA_CONTENT(p) + 4) == 1 &&
        READ16LE(ZIP_EXTRA_CONTENT(p) + 6) >= 8) {
      return READ64LE(ZIP_EXTRA_CONTENT(p) + 8) / HECTONANOSECONDS -
             MODERNITYSECONDS; /* TODO(jart): update access time */
    }
  }
  for (p = ZIP_CFILE_EXTRA(zcf), pe = p + ZIP_CFILE_EXTRASIZE(zcf); p + 4 <= pe;
       p += ZIP_EXTRA_SIZE(p)) {
    if (ZIP_EXTRA_HEADERID(p) == kZipExtraExtendedTimestamp &&
        ZIP_EXTRA_CONTENTSIZE(p) >= 1 + 4 && (*ZIP_EXTRA_CONTENT(p) & 1)) {
      return (int32_t)READ32LE(ZIP_EXTRA_CONTENT(p) + 1);
    }
  }
  for (p = ZIP_CFILE_EXTRA(zcf), pe = p + ZIP_CFILE_EXTRASIZE(zcf); p + 4 <= pe;
       p += ZIP_EXTRA_SIZE(p)) {
    if (ZIP_EXTRA_HEADERID(p) == kZipExtraUnix &&
        ZIP_EXTRA_CONTENTSIZE(p) >= 4 + 4) {
      return (int32_t)READ32LE(ZIP_EXTRA_CONTENT(p) + 4);
    }
  }
  return LocoTimeToZulu(DosDateTimeToUnix(ZIP_CFILE_LASTMODIFIEDDATE(zcf),
                                          ZIP_CFILE_LASTMODIFIEDTIME(zcf)));
}

static int64_t GetZipCfileCreation(const uint8_t *zcf) {
  const uint8_t *p, *pe;
  for (p = ZIP_CFILE_EXTRA(zcf), pe = p + ZIP_CFILE_EXTRASIZE(zcf); p + 4 <= pe;
       p += ZIP_EXTRA_SIZE(p)) {
    if (ZIP_EXTRA_HEADERID(p) == kZipExtraNtfs &&
        ZIP_EXTRA_CONTENTSIZE(p) >= 4 + 4 + 8 * 3 &&
        READ16LE(ZIP_EXTRA_CONTENT(p) + 4) == 1 &&
        READ16LE(ZIP_EXTRA_CONTENT(p) + 6) >= 24) {
      return READ64LE(ZIP_EXTRA_CONTENT(p) + 8 + 8 + 8) / HECTONANOSECONDS -
             MODERNITYSECONDS;
    }
  }
  for (p = ZIP_CFILE_EXTRA(zcf), pe = p + ZIP_CFILE_EXTRASIZE(zcf); p + 4 <= pe;
       p += ZIP_EXTRA_SIZE(p)) {
    if (ZIP_EXTRA_HEADERID(p) == kZipExtraExtendedTimestamp &&
        ZIP_EXTRA_CONTENTSIZE(p) >= 1 && (*ZIP_EXTRA_CONTENT(p) & 4) &&
        ZIP_EXTRA_CONTENTSIZE(p) >=
            1 + popcnt((*ZIP_EXTRA_CONTENT(p) & 7)) * 4) {
      return (int32_t)READ32LE(ZIP_EXTRA_CONTENT(p) + 1 +
                               popcnt((*ZIP_EXTRA_CONTENT(p) & 3)) * 4);
    }
  }
  return GetZipCfileLastModified(zcf);
}

forceinline bool IsCompressed(struct Asset *a) {
  return !a->file &&
         ZIP_LFILE_COMPRESSIONMETHOD(zbase + a->lf) == kZipCompressionDeflate;
}

forceinline int GetMode(struct Asset *a) {
  return a->file ? a->file->st.st_mode : GetZipCfileMode(zbase + a->cf);
}

static char *FormatUnixHttpDateTime(char *s, int64_t t) {
  struct tm tm;
  gmtime_r(&t, &tm);
  FormatHttpDateTime(s, &tm);
  return s;
}

forceinline bool IsCompressionMethodSupported(int method) {
  return method == kZipCompressionNone || method == kZipCompressionDeflate;
}

static inline unsigned Hash(const void *p, unsigned long n) {
  unsigned h, i;
  for (h = i = 0; i < n; i++) {
    h += ((unsigned char *)p)[i];
    h *= 0x9e3779b1;
  }
  return MAX(1, h);
}

static void Free(void *p) {
  free(*(void **)p);
  *(void **)p = 0;
}

static void FreeAssets(void) {
  size_t i;
  for (i = 0; i < assets.n; ++i) {
    free(assets.p[i].lastmodifiedstr);
  }
  Free(&assets.p);
  assets.n = 0;
}

static void FreeStrings(struct Strings *l) {
  size_t i;
  for (i = 0; i < l->n; ++i) {
    free(l->p[i].s);
  }
  Free(&l->p);
  l->n = 0;
}

static void IndexAssets(void) {
  int64_t lm;
  uint64_t cf;
  struct Asset *p;
  uint32_t i, n, m, step, hash;
  DEBUGF("indexing assets (inode %#lx)", zst.st_ino);
  CHECK_GE(HASH_LOAD_FACTOR, 2);
  CHECK(READ32LE(zcdir) == kZipCdir64HdrMagic ||
        READ32LE(zcdir) == kZipCdirHdrMagic);
  n = GetZipCdirRecords(zcdir);
  m = roundup2pow(MAX(1, n) * HASH_LOAD_FACTOR);
  p = xcalloc(m, sizeof(struct Asset));
  for (cf = GetZipCdirOffset(zcdir); n--; cf += ZIP_CFILE_HDRSIZE(zbase + cf)) {
    CHECK_EQ(kZipCfileHdrMagic, ZIP_CFILE_MAGIC(zbase + cf));
    if (!IsCompressionMethodSupported(
            ZIP_CFILE_COMPRESSIONMETHOD(zbase + cf))) {
      LOGF("don't understand zip compression method %d used by %`'.*s",
           ZIP_CFILE_COMPRESSIONMETHOD(zbase + cf),
           ZIP_CFILE_NAMESIZE(zbase + cf), ZIP_CFILE_NAME(zbase + cf));
      continue;
    }
    hash = Hash(ZIP_CFILE_NAME(zbase + cf), ZIP_CFILE_NAMESIZE(zbase + cf));
    step = 0;
    do {
      i = (hash + (step * (step + 1)) >> 1) & (m - 1);
      ++step;
    } while (p[i].hash);
    lm = GetZipCfileLastModified(zbase + cf);
    p[i].hash = hash;
    p[i].cf = cf;
    p[i].lf = GetZipCfileOffset(zbase + cf);
    p[i].istext = !!(ZIP_CFILE_INTERNALATTRIBUTES(zbase + cf) & kZipIattrText);
    p[i].lastmodified = lm;
    p[i].lastmodifiedstr = FormatUnixHttpDateTime(xmalloc(30), lm);
  }
  assets.p = p;
  assets.n = m;
}

static bool OpenZip(bool force) {
  int fd;
  size_t n;
  struct stat st;
  uint8_t *m, *b, *d, *p;
  if (stat(zpath, &st) != -1) {
    if (force || st.st_ino != zst.st_ino || st.st_size > zst.st_size) {
      if (st.st_ino == zst.st_ino) {
        fd = zfd;
      } else if ((fd = open(zpath, O_RDWR)) == -1) {
        WARNF("open() failed w/ %m");
        return false;
      }
      if ((m = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) !=
          MAP_FAILED) {
        n = st.st_size;
        if (endswith(zpath, ".com.dbg") && (p = memmem(m, n, "MZqFpD", 6))) {
          b = p;
          n -= p - m;
        } else {
          b = m;
        }
        if ((d = GetZipCdir(b, n))) {
          if (zmap) {
            LOGIFNEG1(munmap(zmap, zbase + zsize - zmap));
          }
          zmap = m;
          zbase = b;
          zsize = n;
          zcdir = d;
          DCHECK(IsZipCdir32(zbase, zsize, zcdir - zbase) ||
                 IsZipCdir64(zbase, zsize, zcdir - zbase));
          memcpy(&zst, &st, sizeof(st));
          IndexAssets();
          return true;
        } else {
          WARNF("couldn't locate central directory");
        }
      } else {
        WARNF("mmap() failed w/ %m");
      }
    }
  } else {
    WARNF("stat() failed w/ %m");
  }
  return false;
}

static struct Asset *GetAssetZip(const char *path, size_t pathlen) {
  uint32_t i, step, hash;
  if (pathlen > 1 && path[0] == '/') ++path, --pathlen;
  hash = Hash(path, pathlen);
  for (step = 0;; ++step) {
    i = (hash + (step * (step + 1)) >> 1) & (assets.n - 1);
    if (!assets.p[i].hash) return NULL;
    if (hash == assets.p[i].hash &&
        pathlen == ZIP_CFILE_NAMESIZE(zbase + assets.p[i].cf) &&
        memcmp(path, ZIP_CFILE_NAME(zbase + assets.p[i].cf), pathlen) == 0) {
      return &assets.p[i];
    }
  }
}

static struct Asset *GetAssetFile(const char *path, size_t pathlen) {
  size_t i;
  struct Asset *a;
  if (stagedirs.n) {
    a = FreeLater(xcalloc(1, sizeof(struct Asset)));
    a->file = FreeLater(xmalloc(sizeof(struct File)));
    for (i = 0; i < stagedirs.n; ++i) {
      LockInc(&shared->c.stats);
      a->file->path.s = FreeLater(MergePaths(stagedirs.p[i].s, stagedirs.p[i].n,
                                             path, pathlen, &a->file->path.n));
      if (stat(a->file->path.s, &a->file->st) != -1) {
        a->lastmodifiedstr = FormatUnixHttpDateTime(
            FreeLater(xmalloc(30)),
            (a->lastmodified = a->file->st.st_mtim.tv_sec));
        return a;
      } else {
        LockInc(&shared->c.statfails);
      }
    }
  }
  return NULL;
}

static struct Asset *GetAsset(const char *path, size_t pathlen) {
  char *path2;
  struct Asset *a;
  if (!(a = GetAssetFile(path, pathlen))) {
    if (!(a = GetAssetZip(path, pathlen))) {
      if (pathlen > 1 && path[pathlen - 1] != '/' &&
          pathlen + 1 <= sizeof(slashpath)) {
        memcpy(mempcpy(slashpath, path, pathlen), "/", 1);
        a = GetAssetZip(slashpath, pathlen + 1);
      }
    }
  }
  return a;
}

static char *AppendHeader(char *p, const char *k, const char *v) {
  if (!v) return p;
  return AppendCrlf(stpcpy(stpcpy(stpcpy(p, k), ": "), v));
}

static char *AppendContentType(char *p, const char *ct) {
  p = stpcpy(p, "Content-Type: ");
  p = stpcpy(p, ct);
  if (startswith(ct, "text/")) {
    istext = true;
    if (!strchr(ct, ';')) {
      p = stpcpy(p, "; charset=utf-8");
    }
  }
  return AppendCrlf(p);
}

static char *AppendExpires(char *p, int64_t t) {
  struct tm tm;
  gmtime_r(&t, &tm);
  p = stpcpy(p, "Expires: ");
  p = FormatHttpDateTime(p, &tm);
  return AppendCrlf(p);
}

static char *AppendCache(char *p, int64_t seconds) {
  if (seconds < 0) return p;
  p = stpcpy(p, "Cache-Control: max-age=");
  p += uint64toarray_radix10(seconds, p);
  if (seconds) {
    p = stpcpy(p, ", public");
  } else {
    p = stpcpy(p, ", no-store");
  }
  p = AppendCrlf(p);
  return AppendExpires(p, (int64_t)shared->nowish + seconds);
}

static inline char *AppendContentLength(char *p, size_t n) {
  p = stpcpy(p, "Content-Length: ");
  p += uint64toarray_radix10(n, p);
  return AppendCrlf(p);
}

static char *AppendContentRange(char *p, long a, long b, long c) {
  p = stpcpy(p, "Content-Range: bytes ");
  if (a >= 0 && b > 0) {
    p += uint64toarray_radix10(a, p);
    *p++ = '-';
    p += uint64toarray_radix10(a + b - 1, p);
  } else {
    *p++ = '*';
  }
  *p++ = '/';
  p += uint64toarray_radix10(c, p);
  return AppendCrlf(p);
}

static bool Inflate(void *dp, size_t dn, const void *sp, size_t sn) {
  int rc;
  z_stream zs;
  struct DeflateState ds;
  LockInc(&shared->c.inflates);
  if (IsTiny()) {
    return undeflate(dp, dn, sp, sn, &ds) != -1;
  } else {
    zs.next_in = sp;
    zs.avail_in = sn;
    zs.total_in = sn;
    zs.next_out = dp;
    zs.avail_out = dn;
    zs.total_out = dn;
    zs.zfree = Z_NULL;
    zs.zalloc = Z_NULL;
    CHECK_EQ(Z_OK, inflateInit2(&zs, -MAX_WBITS));
    switch ((rc = inflate(&zs, Z_NO_FLUSH))) {
      case Z_STREAM_END:
        CHECK_EQ(Z_OK, inflateEnd(&zs));
        return true;
      case Z_DATA_ERROR:
        inflateEnd(&zs);
        WARNF("Z_DATA_ERROR");
        return false;
      case Z_NEED_DICT:
        inflateEnd(&zs);
        WARNF("Z_NEED_DICT");
        return false;
      case Z_MEM_ERROR:
        FATALF("Z_MEM_ERROR");
      default:
        FATALF("inflate()→%d dn=%ld sn=%ld "
               "next_in=%ld avail_in=%ld next_out=%ld avail_out=%ld",
               rc, dn, sn, (char *)zs.next_in - (char *)sp, zs.avail_in,
               (char *)zs.next_out - (char *)dp, zs.avail_out);
    }
  }
}

static bool Verify(void *data, size_t size, uint32_t crc) {
  uint32_t got;
  LockInc(&shared->c.verifies);
  if (crc == (got = crc32_z(0, data, size))) {
    return true;
  } else {
    LockInc(&shared->c.thiscorruption);
    WARNF("corrupt zip file at %`'.*s had crc 0x%08x but expected 0x%08x",
          msg.uri.b - msg.uri.a, inbuf.p + msg.uri.a, got, crc);
    return false;
  }
}

static void *Deflate(const void *data, size_t size, size_t *out_size) {
  void *res;
  z_stream zs;
  LockInc(&shared->c.deflates);
  CHECK_EQ(Z_OK, deflateInit2(memset(&zs, 0, sizeof(zs)), 4, Z_DEFLATED,
                              -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY));
  zs.next_in = data;
  zs.avail_in = size;
  zs.avail_out = compressBound(size);
  zs.next_out = res = xmalloc(zs.avail_out);
  CHECK_EQ(Z_STREAM_END, deflate(&zs, Z_FINISH));
  CHECK_EQ(Z_OK, deflateEnd(&zs));
  *out_size = zs.total_out;
  return xrealloc(res, zs.total_out);
}

static void *LoadAsset(struct Asset *a, size_t *out_size) {
  int mode;
  size_t size;
  uint8_t *data;
  if (S_ISDIR(GetMode(a))) {
    WARNF("can't load directory");
    return NULL;
  }
  if (!a->file) {
    size = GetZipLfileUncompressedSize(zbase + a->lf);
    if (size == SIZE_MAX || !(data = malloc(size + 1))) return NULL;
    if (IsCompressed(a)) {
      if (!Inflate(data, size, ZIP_LFILE_CONTENT(zbase + a->lf),
                   GetZipCfileCompressedSize(zbase + a->cf))) {
        free(data);
        return NULL;
      }
    } else {
      memcpy(data, ZIP_LFILE_CONTENT(zbase + a->lf), size);
    }
    if (!Verify(data, size, ZIP_LFILE_CRC32(zbase + a->lf))) {
      free(data);
      return NULL;
    }
    data[size] = '\0';
    if (out_size) *out_size = size;
    return data;
  } else {
    LockInc(&shared->c.slurps);
    return xslurp(a->file->path.s, out_size);
  }
}

static wontreturn void PrintUsage(FILE *f, int rc) {
  size_t n;
  const char *p;
  struct Asset *a;
  if ((a = GetAssetZip("/help.txt", 9)) && (p = LoadAsset(a, &n))) {
    fwrite(p, 1, n, f);
    free(p);
  }
  exit(rc);
}

static void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv,
                       "jkazhdugvVsmbfB"
                       "l:p:r:R:H:c:L:P:U:G:D:t:M:C:K:F:")) != -1) {
    switch (opt) {
      CASE('v', ++__log_level);
      CASE('s', --__log_level);
      CASE('V', ++mbedtls_debug_threshold);
      CASE('B', suiteb = true);
      CASE('f', funtrace = true);
      CASE('b', logbodies = true);
      CASE('z', printport = true);
      CASE('d', daemonize = true);
      CASE('a', logrusage = true);
      CASE('u', uniprocess = true);
      CASE('g', loglatency = true);
      CASE('m', logmessages = true);
      CASE('k', sslfetchverify = false);
      CASE('j', sslclientverify = true);
      CASE('l', ProgramAddr(optarg));
      CASE('H', ProgramHeader(optarg));
      CASE('L', ProgramLogPath(optarg));
      CASE('P', ProgramPidPath(optarg));
      CASE('D', ProgramDirectory(optarg));
      CASE('U', ProgramUid(atoi(optarg)));
      CASE('G', ProgramGid(atoi(optarg)));
      CASE('p', ProgramPort(ParseInt(optarg)));
      CASE('R', ProgramRedirectArg(0, optarg));
      CASE('c', ProgramCache(ParseInt(optarg)));
      CASE('r', ProgramRedirectArg(307, optarg));
      CASE('t', ProgramTimeout(ParseInt(optarg)));
      CASE('h', PrintUsage(stdout, EXIT_SUCCESS));
      CASE('M', ProgramMaxPayloadSize(ParseInt(optarg)));
#ifndef UNSECURE
      CASE('C', ProgramFile(optarg, ProgramCertificate));
      CASE('K', ProgramFile(optarg, ProgramPrivateKey));
#endif
      default:
        PrintUsage(stderr, EX_USAGE);
    }
  }
}

static void AppendLogo(void) {
  size_t n;
  char *p, *q;
  struct Asset *a;
  if ((a = GetAsset("/redbean.png", 12)) && (p = LoadAsset(a, &n))) {
    q = EncodeBase64(p, n, &n);
    Append("<img alt=\"[logo]\" src=\"data:image/png;base64,");
    AppendData(q, n);
    Append("\">\r\n");
    free(q);
    free(p);
  }
}

static ssize_t Send(struct iovec *iov, int iovlen) {
  ssize_t rc;
  if ((rc = writer(client, iov, iovlen)) == -1) {
    if (errno == ECONNRESET) {
      LockInc(&shared->c.writeresets);
      DEBUGF("%s write reset", DescribeClient());
    } else if (errno == EAGAIN) {
      LockInc(&shared->c.writetimeouts);
      WARNF("%s write timeout", DescribeClient());
    } else {
      LockInc(&shared->c.writeerrors);
      WARNF("%s write error %s", DescribeClient(), strerror(errno));
    }
    connectionclose = true;
  }
  return rc;
}

static char *CommitOutput(char *p) {
  uint32_t crc;
  if (!contentlength) {
    if (istext && outbuf.n >= 100) {
      p = stpcpy(p, "Vary: Accept-Encoding\r\n");
      if (!IsTiny() && ClientAcceptsGzip()) {
        gzipped = true;
        crc = crc32_z(0, outbuf.p, outbuf.n);
        WRITE32LE(gzip_footer + 0, crc);
        WRITE32LE(gzip_footer + 4, outbuf.n);
        content = FreeLater(Deflate(outbuf.p, outbuf.n, &contentlength));
        DropOutput();
      } else {
        UseOutput();
      }
    } else {
      UseOutput();
    }
  } else {
    DropOutput();
  }
  return p;
}

static char *ServeDefaultErrorPage(char *p, unsigned code, const char *reason) {
  p = AppendContentType(p, "text/html; charset=ISO-8859-1");
  reason = FreeLater(EscapeHtml(reason, -1, 0));
  Append("\
<!doctype html>\r\n\
<title>");
  Append("%d %s", code, reason);
  Append("\
</title>\r\n\
<style>\r\n\
html { color: #111; font-family: sans-serif; }\r\n\
img { vertical-align: middle; }\r\n\
</style>\r\n\
<h1>\r\n");
  AppendLogo();
  Append("%d %s\r\n", code, reason);
  Append("</h1>\r\n");
  UseOutput();
  return p;
}

static char *ServeErrorImpl(unsigned code, const char *reason) {
  size_t n;
  char *p, *s;
  struct Asset *a;
  LockInc(&shared->c.errors);
  ClearOutput();
  p = SetStatus(code, reason);
  s = xasprintf("/%d.html", code);
  a = GetAsset(s, strlen(s));
  free(s);
  if (!a) {
    return ServeDefaultErrorPage(p, code, reason);
  } else if (a->file) {
    LockInc(&shared->c.slurps);
    content = FreeLater(xslurp(a->file->path.s, &contentlength));
    return AppendContentType(p, "text/html; charset=utf-8");
  } else {
    content = (char *)ZIP_LFILE_CONTENT(zbase + a->lf);
    contentlength = GetZipCfileCompressedSize(zbase + a->cf);
    if (IsCompressed(a)) {
      n = GetZipLfileUncompressedSize(zbase + a->lf);
      if ((s = FreeLater(malloc(n))) && Inflate(s, n, content, contentlength)) {
        content = s;
        contentlength = n;
      } else {
        return ServeDefaultErrorPage(p, code, reason);
      }
    }
    if (Verify(content, contentlength, ZIP_LFILE_CRC32(zbase + a->lf))) {
      return AppendContentType(p, "text/html; charset=utf-8");
    } else {
      return ServeDefaultErrorPage(p, code, reason);
    }
  }
}

static char *ServeError(unsigned code, const char *reason) {
  LOGF("ERROR %d %s", code, reason);
  return ServeErrorImpl(code, reason);
}

static char *ServeFailure(unsigned code, const char *reason) {
  LOGF("FAILURE %d %s %s HTTP%02d %.*s %`'.*s %`'.*s %`'.*s %`'.*s", code,
       reason, DescribeClient(), msg.version, msg.xmethod.b - msg.xmethod.a,
       inbuf.p + msg.xmethod.a, HeaderLength(kHttpHost), HeaderData(kHttpHost),
       msg.uri.b - msg.uri.a, inbuf.p + msg.uri.a, HeaderLength(kHttpReferer),
       HeaderData(kHttpReferer), HeaderLength(kHttpUserAgent),
       HeaderData(kHttpUserAgent));
  return ServeErrorImpl(code, reason);
}

static ssize_t DeflateGenerator(struct iovec v[3]) {
  int i, rc;
  size_t no;
  void *res;
  i = 0;
  if (!dg.t) {
    v[0].iov_base = kGzipHeader;
    v[0].iov_len = sizeof(kGzipHeader);
    ++dg.t;
    ++i;
  } else if (dg.t == 3) {
    return 0;
  }
  if (dg.t != 2) {
    CHECK_EQ(0, dg.s.avail_in);
    dg.s.next_in = (void *)(content + dg.i);
    dg.s.avail_in = MIN(CHUNK, contentlength - dg.i);
    dg.c = crc32_z(dg.c, dg.s.next_in, dg.s.avail_in);
    dg.i += dg.s.avail_in;
  }
  dg.s.next_out = dg.b;
  dg.s.avail_out = CHUNK;
  rc = deflate(&dg.s, dg.i < contentlength ? Z_SYNC_FLUSH : Z_FINISH);
  if (rc != Z_OK && rc != Z_STREAM_END) FATALF("deflate()→%d", rc);
  no = CHUNK - dg.s.avail_out;
  if (no) {
    v[i].iov_base = dg.b;
    v[i].iov_len = no;
    ++i;
  }
  if (rc == Z_OK) {
    CHECK_GT(no, 0);
    dg.t = dg.s.avail_out ? 1 : 2;
  } else if (rc == Z_STREAM_END) {
    CHECK_EQ(contentlength, dg.i);
    CHECK_EQ(Z_OK, deflateEnd(&dg.s));
    WRITE32LE(gzip_footer + 0, dg.c);
    WRITE32LE(gzip_footer + 4, contentlength);
    v[i].iov_base = gzip_footer;
    v[i].iov_len = sizeof(gzip_footer);
    dg.t = 3;
  }
  return v[0].iov_len + v[1].iov_len + v[2].iov_len;
}

static char *ServeAssetCompressed(struct Asset *a) {
  char *p;
  uint32_t crc;
  LockInc(&shared->c.deflates);
  LockInc(&shared->c.compressedresponses);
  DEBUGF("ServeAssetCompressed()");
  dg.t = 0;
  dg.i = 0;
  dg.c = 0;
  gzipped = true;
  generator = DeflateGenerator;
  CHECK_EQ(Z_OK, deflateInit2(memset(&dg.s, 0, sizeof(dg.s)), 4, Z_DEFLATED,
                              -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY));
  dg.b = FreeLater(malloc(CHUNK));
  p = SetStatus(200, "OK");
  p = stpcpy(p, "Content-Encoding: gzip\r\n");
  return p;
}

static ssize_t InflateGenerator(struct iovec v[3]) {
  int i, rc;
  size_t no;
  void *res;
  i = 0;
  if (!dg.t) {
    ++dg.t;
  } else if (dg.t == 3) {
    return 0;
  }
  if (dg.t != 2) {
    CHECK_EQ(0, dg.s.avail_in);
    dg.s.next_in = (void *)(content + dg.i);
    dg.s.avail_in = MIN(CHUNK, contentlength - dg.i);
    dg.i += dg.s.avail_in;
  }
  dg.s.next_out = dg.b;
  dg.s.avail_out = CHUNK;
  rc = inflate(&dg.s, Z_NO_FLUSH);
  if (rc != Z_OK && rc != Z_STREAM_END) FATALF("inflate()→%d", rc);
  no = CHUNK - dg.s.avail_out;
  if (no) {
    v[i].iov_base = dg.b;
    v[i].iov_len = no;
    dg.c = crc32_z(dg.c, dg.b, no);
    ++i;
  }
  if (rc == Z_OK) {
    CHECK_GT(no, 0);
    dg.t = dg.s.avail_out ? 1 : 2;
  } else if (rc == Z_STREAM_END) {
    CHECK_EQ(Z_OK, inflateEnd(&dg.s));
    CHECK_EQ(ZIP_CFILE_CRC32(zbase + dg.a->cf), dg.c);
    dg.t = 3;
  }
  return v[0].iov_len + v[1].iov_len + v[2].iov_len;
}

static char *ServeAssetDecompressed(struct Asset *a) {
  char *p;
  size_t size;
  uint32_t crc;
  LockInc(&shared->c.inflates);
  LockInc(&shared->c.decompressedresponses);
  size = GetZipCfileUncompressedSize(zbase + a->cf);
  DEBUGF("ServeAssetDecompressed(%ld) -> %ld", contentlength, size);
  if (msg.method == kHttpHead) {
    content = 0;
    contentlength = size;
    return SetStatus(200, "OK");
  } else if (!IsTiny()) {
    dg.t = 0;
    dg.i = 0;
    dg.c = 0;
    dg.a = a;
    generator = InflateGenerator;
    CHECK_EQ(Z_OK, inflateInit2(memset(&dg.s, 0, sizeof(dg.s)), -MAX_WBITS));
    dg.b = FreeLater(malloc(CHUNK));
    return SetStatus(200, "OK");
  } else if ((p = FreeLater(malloc(size))) &&
             Inflate(p, size, content, contentlength) &&
             Verify(p, size, ZIP_CFILE_CRC32(zbase + a->cf))) {
    content = p;
    contentlength = size;
    return SetStatus(200, "OK");
  } else {
    return ServeError(500, "Internal Server Error");
  }
}

static inline char *ServeAssetIdentity(struct Asset *a, const char *ct) {
  LockInc(&shared->c.identityresponses);
  DEBUGF("ServeAssetIdentity(%`'s)", ct);
  return SetStatus(200, "OK");
}

static inline char *ServeAssetPrecompressed(struct Asset *a) {
  size_t size;
  uint32_t crc;
  DEBUGF("ServeAssetPrecompressed()");
  LockInc(&shared->c.precompressedresponses);
  gzipped = true;
  crc = ZIP_CFILE_CRC32(zbase + a->cf);
  size = GetZipCfileUncompressedSize(zbase + a->cf);
  WRITE32LE(gzip_footer + 0, crc);
  WRITE32LE(gzip_footer + 4, size);
  return SetStatus(200, "OK");
}

static char *ServeAssetRange(struct Asset *a) {
  char *p;
  long rangestart, rangelength;
  DEBUGF("ServeAssetRange()");
  if (ParseHttpRange(HeaderData(kHttpRange), HeaderLength(kHttpRange),
                     contentlength, &rangestart, &rangelength) &&
      rangestart >= 0 && rangelength >= 0 && rangestart < contentlength &&
      rangestart + rangelength <= contentlength) {
    LockInc(&shared->c.partialresponses);
    p = SetStatus(206, "Partial Content");
    p = AppendContentRange(p, rangestart, rangelength, contentlength);
    content += rangestart;
    contentlength = rangelength;
    return p;
  } else {
    LockInc(&shared->c.badranges);
    LOGF("bad range %`'.*s", HeaderLength(kHttpRange), HeaderData(kHttpRange));
    p = SetStatus(416, "Range Not Satisfiable");
    p = AppendContentRange(p, -1, -1, contentlength);
    content = "";
    contentlength = 0;
    return p;
  }
}

static char *GetAssetPath(uint8_t *zcf, size_t *out_size) {
  char *p1, *p2;
  size_t n1, n2;
  p1 = ZIP_CFILE_NAME(zcf);
  n1 = ZIP_CFILE_NAMESIZE(zcf);
  p2 = xmalloc(1 + n1 + 1);
  n2 = 1 + n1 + 1;
  p2[0] = '/';
  memcpy(p2 + 1, p1, n1);
  p2[1 + n1] = '\0';
  if (out_size) *out_size = 1 + n1;
  return p2;
}

static bool IsHiddenPath(const char *s, size_t n) {
  size_t i;
  for (i = 0; i < hidepaths.n; ++i) {
    if (n >= hidepaths.p[i].n &&
        !memcmp(s, hidepaths.p[i].s, hidepaths.p[i].n)) {
      return true;
    }
  }
  return false;
}

static char *GetBasicAuthorization(size_t *z) {
  size_t n;
  const char *p, *q;
  struct HttpSlice *g;
  g = msg.headers + (HasHeader(kHttpProxyAuthorization)
                         ? kHttpProxyAuthorization
                         : kHttpAuthorization);
  p = inbuf.p + g->a;
  n = g->b - g->a;
  if ((q = memchr(p, ' ', n)) && SlicesEqualCase(p, q - p, "Basic", 5)) {
    return DecodeBase64(q + 1, n - (q + 1 - p), z);
  } else {
    return NULL;
  }
}

static const char *GetSystemUrlLauncherCommand(void) {
  if (IsWindows()) {
    return "explorer";
  } else if (IsXnu()) {
    return "open";
  } else {
    return "xdg-open";
  }
}

static void LaunchBrowser(const char *path) {
  int pid, ws;
  struct in_addr addr;
  const char *u, *prog;
  sigset_t chldmask, savemask;
  struct sigaction ignore, saveint, savequit;
  path = firstnonnull(path, "/");
  addr = serveraddr->sin_addr;
  if (!addr.s_addr) addr.s_addr = htonl(INADDR_LOOPBACK);
  if (*path != '/') path = gc(xasprintf("/%s", path));
  if ((prog = commandv(GetSystemUrlLauncherCommand(), gc(malloc(PATH_MAX))))) {
    u = gc(xasprintf("http://%s:%d%s", inet_ntoa(addr),
                     ntohs(serveraddr->sin_port), gc(EscapePath(path, -1, 0))));
    DEBUGF("opening browser with command %s %s\n", prog, u);
    ignore.sa_flags = 0;
    ignore.sa_handler = SIG_IGN;
    sigemptyset(&ignore.sa_mask);
    sigaction(SIGINT, &ignore, &saveint);
    sigaction(SIGQUIT, &ignore, &savequit);
    sigemptyset(&chldmask);
    sigaddset(&chldmask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &chldmask, &savemask);
    CHECK_NE(-1, (pid = fork()));
    if (!pid) {
      setpgid(getpid(), getpid());
      sigaction(SIGINT, &saveint, 0);
      sigaction(SIGQUIT, &savequit, 0);
      sigprocmask(SIG_SETMASK, &savemask, 0);
      execv(prog, (char *const[]){prog, u, 0});
      _exit(127);
    }
    while (wait4(pid, &ws, 0, 0) == -1) {
      CHECK_EQ(EINTR, errno);
    }
    sigaction(SIGINT, &saveint, 0);
    sigaction(SIGQUIT, &savequit, 0);
    sigprocmask(SIG_SETMASK, &savemask, 0);
    if (!(WIFEXITED(ws) && WEXITSTATUS(ws) == 0)) {
      WARNF("%s failed with %d", GetSystemUrlLauncherCommand(),
            WIFEXITED(ws) ? WEXITSTATUS(ws) : 128 + WEXITSTATUS(ws));
    }
  } else {
    WARNF("can't launch browser because %s isn't installed",
          GetSystemUrlLauncherCommand());
  }
}

static char *BadMethod(void) {
  LockInc(&shared->c.badmethods);
  return stpcpy(ServeError(405, "Method Not Allowed"), "Allow: GET, HEAD\r\n");
}

static int GetDecimalWidth(long x) {
  int w = x ? ceil(log10(x)) : 1;
  return w + (w - 1) / 3;
}

static int GetOctalWidth(int x) {
  return !x ? 1 : x < 8 ? 2 : 1 + bsr(x) / 3;
}

static const char *DescribeCompressionRatio(char rb[8], uint8_t *zcf) {
  long percent;
  if (ZIP_CFILE_COMPRESSIONMETHOD(zcf) == kZipCompressionNone) {
    return "n/a";
  } else {
    percent = lround(100 - (double)GetZipCfileCompressedSize(zcf) /
                               GetZipCfileUncompressedSize(zcf) * 100);
    sprintf(rb, "%ld%%", MIN(999, MAX(-999, percent)));
    return rb;
  }
}

static char *ServeListing(void) {
  long x;
  ldiv_t y;
  int w[3];
  uint8_t *zcf;
  struct tm tm;
  const char *and;
  int64_t lastmod;
  struct rusage ru;
  char *p, *q, *path;
  char rb[8], tb[20], *rp[6];
  size_t i, n, pathlen, rn[6];
  LockInc(&shared->c.listingrequests);
  if (msg.method != kHttpGet && msg.method != kHttpHead) return BadMethod();
  Append("\
<!doctype html>\r\n\
<meta charset=\"utf-8\">\r\n\
<title>redbean zip listing</title>\r\n\
<style>\r\n\
html { color: #111; font-family: sans-serif; }\r\n\
a { text-decoration: none; }\r\n\
pre a:hover { color: #00e; border-bottom: 1px solid #ccc; }\r\n\
h1 a { color: #111; }\r\n\
img { vertical-align: middle; }\r\n\
footer { color: #555; font-size: 10pt; }\r\n\
td { padding-right: 3em; }\r\n\
.eocdcomment { max-width: 800px; color: #333; font-size: 11pt; }\r\n\
</style>\r\n\
<header><h1>\r\n");
  AppendLogo();
  rp[0] = EscapeHtml(brand, -1, &rn[0]);
  AppendData(rp[0], rn[0]);
  free(rp[0]);
  Append("</h1>\r\n"
         "<div class=\"eocdcomment\">%.*s</div>\r\n"
         "<hr>\r\n"
         "</header>\r\n"
         "<pre>\r\n",
         strnlen(GetZipCdirComment(zcdir), GetZipCdirCommentSize(zcdir)),
         GetZipCdirComment(zcdir));
  memset(w, 0, sizeof(w));
  n = GetZipCdirRecords(zcdir);
  for (zcf = zbase + GetZipCdirOffset(zcdir); n--;
       zcf += ZIP_CFILE_HDRSIZE(zcf)) {
    CHECK_EQ(kZipCfileHdrMagic, ZIP_CFILE_MAGIC(zcf));
    path = GetAssetPath(zcf, &pathlen);
    if (!IsHiddenPath(path, pathlen)) {
      w[0] = min(80, max(w[0], strwidth(path, 0) + 2));
      w[1] = max(w[1], GetOctalWidth(GetZipCfileMode(zcf)));
      w[2] = max(w[2], GetDecimalWidth(GetZipCfileUncompressedSize(zcf)));
    }
    free(path);
  }
  n = GetZipCdirRecords(zcdir);
  for (zcf = zbase + GetZipCdirOffset(zcdir); n--;
       zcf += ZIP_CFILE_HDRSIZE(zcf)) {
    CHECK_EQ(kZipCfileHdrMagic, ZIP_CFILE_MAGIC(zcf));
    path = GetAssetPath(zcf, &pathlen);
    if (!IsHiddenPath(path, pathlen)) {
      rp[0] = VisualizeControlCodes(path, pathlen, &rn[0]);
      rp[1] = EscapePath(path, pathlen, &rn[1]);
      rp[2] = EscapeHtml(rp[1], rn[1], &rn[2]);
      rp[3] = VisualizeControlCodes(
          ZIP_CFILE_COMMENT(zcf),
          strnlen(ZIP_CFILE_COMMENT(zcf), ZIP_CFILE_COMMENTSIZE(zcf)), &rn[3]);
      rp[4] = EscapeHtml(rp[0], rn[0], &rn[4]);
      lastmod = GetZipCfileLastModified(zcf);
      localtime_r(&lastmod, &tm);
      iso8601(tb, &tm);
      if (IsCompressionMethodSupported(ZIP_CFILE_COMPRESSIONMETHOD(zcf)) &&
          IsAcceptablePath(path, pathlen)) {
        Append("<a href=\"%.*s\">%-*.*s</a> %s  %0*o %4s  %,*ld  %'s\r\n",
               rn[2], rp[2], w[0], rn[4], rp[4], tb, w[1], GetZipCfileMode(zcf),
               DescribeCompressionRatio(rb, zcf), w[2],
               GetZipCfileUncompressedSize(zcf), rp[3]);
      } else {
        Append("%-*.*s %s  %0*o %4s  %,*ld  %'s\r\n", w[0], rn[4], rp[4], tb,
               w[1], GetZipCfileMode(zcf), DescribeCompressionRatio(rb, zcf),
               w[2], GetZipCfileUncompressedSize(zcf), rp[3]);
      }
      free(rp[4]);
      free(rp[3]);
      free(rp[2]);
      free(rp[1]);
      free(rp[0]);
    }
    free(path);
  }
  Append("</pre><footer><hr>\r\n");
  Append("<table border=\"0\"><tr>\r\n");
  Append("<td valign=\"top\">\r\n");
  Append("<a href=\"/statusz\">/statusz</a>\r\n");
  if (shared->c.connectionshandled) {
    Append("says your redbean<br>\r\n");
    AppendResourceReport(&shared->children, "<br>\r\n");
  }
  Append("<td valign=\"top\">\r\n");
  and = "";
  x = nowl() - startserver;
  y = ldiv(x, 24L * 60 * 60);
  if (y.quot) {
    Append("%,ld day%s ", y.quot, y.quot == 1 ? "" : "s");
    and = "and ";
  }
  y = ldiv(y.rem, 60 * 60);
  if (y.quot) {
    Append("%,ld hour%s ", y.quot, y.quot == 1 ? "" : "s");
    and = "and ";
  }
  y = ldiv(y.rem, 60);
  if (y.quot) {
    Append("%,ld minute%s ", y.quot, y.quot == 1 ? "" : "s");
    and = "and ";
  }
  Append("%s%,ld second%s of operation<br>\r\n", and, y.rem,
         y.rem == 1 ? "" : "s");
  x = shared->c.messageshandled;
  Append("%,ld message%s handled<br>\r\n", x, x == 1 ? "" : "s");
  x = shared->c.connectionshandled;
  Append("%,ld connection%s handled<br>\r\n", x, x == 1 ? "" : "s");
  x = shared->workers;
  Append("%,ld connection%s active<br>\r\n", x, x == 1 ? "" : "s");
  Append("</table>\r\n");
  Append("</footer>\r\n");
  p = SetStatus(200, "OK");
  p = AppendContentType(p, "text/html");
  p = AppendCache(p, 0);
  return CommitOutput(p);
}

static const char *MergeNames(const char *a, const char *b) {
  return FreeLater(xasprintf("%s.%s", a, b));
}

static void AppendLong1(const char *a, long x) {
  if (x) Append("%s: %ld\r\n", a, x);
}

static void AppendLong2(const char *a, const char *b, long x) {
  if (x) Append("%s.%s: %ld\r\n", a, b, x);
}

static void AppendTimeval(const char *a, struct timeval *tv) {
  AppendLong2(a, "tv_sec", tv->tv_sec);
  AppendLong2(a, "tv_usec", tv->tv_usec);
}

static void AppendRusage(const char *a, struct rusage *ru) {
  AppendTimeval(MergeNames(a, "ru_utime"), &ru->ru_utime);
  AppendTimeval(MergeNames(a, "ru_stime"), &ru->ru_stime);
  AppendLong2(a, "ru_maxrss", ru->ru_maxrss);
  AppendLong2(a, "ru_ixrss", ru->ru_ixrss);
  AppendLong2(a, "ru_idrss", ru->ru_idrss);
  AppendLong2(a, "ru_isrss", ru->ru_isrss);
  AppendLong2(a, "ru_minflt", ru->ru_minflt);
  AppendLong2(a, "ru_majflt", ru->ru_majflt);
  AppendLong2(a, "ru_nswap", ru->ru_nswap);
  AppendLong2(a, "ru_inblock", ru->ru_inblock);
  AppendLong2(a, "ru_oublock", ru->ru_oublock);
  AppendLong2(a, "ru_msgsnd", ru->ru_msgsnd);
  AppendLong2(a, "ru_msgrcv", ru->ru_msgrcv);
  AppendLong2(a, "ru_nsignals", ru->ru_nsignals);
  AppendLong2(a, "ru_nvcsw", ru->ru_nvcsw);
  AppendLong2(a, "ru_nivcsw", ru->ru_nivcsw);
}

static void ServeCounters(void) {
  const long *c;
  const char *s;
  for (c = (const long *)&shared->c, s = kCounterNames; *s;
       ++c, s += strlen(s) + 1) {
    AppendLong1(s, *c);
  }
}

static char *ServeStatusz(void) {
  char *p;
  LockInc(&shared->c.statuszrequests);
  if (msg.method != kHttpGet && msg.method != kHttpHead) return BadMethod();
  AppendLong1("pid", getpid());
  AppendLong1("ppid", getppid());
  AppendLong1("now", nowl());
  AppendLong1("nowish", shared->nowish);
  AppendLong1("gmtoff", gmtoff);
  AppendLong1("CLK_TCK", CLK_TCK);
  AppendLong1("startserver", startserver);
  AppendLong1("lastmeltdown", shared->lastmeltdown);
  AppendLong1("workers", shared->workers);
  AppendLong1("assets.n", assets.n);
  ServeCounters();
  AppendRusage("server", &shared->server);
  AppendRusage("children", &shared->children);
  p = SetStatus(200, "OK");
  p = AppendContentType(p, "text/plain");
  p = AppendCache(p, 0);
  return CommitOutput(p);
}

static char *RedirectSlash(void) {
  size_t n;
  char *p, *e;
  LockInc(&shared->c.redirects);
  p = SetStatus(307, "Temporary Redirect");
  p = stpcpy(p, "Location: ");
  e = EscapePath(url.path.p, url.path.n, &n);
  p = mempcpy(p, e, n);
  p = stpcpy(p, "/\r\n");
  free(e);
  return p;
}

static char *ServeIndex(const char *path, size_t pathlen) {
  size_t i, n;
  char *p, *q;
  for (p = 0, i = 0; !p && i < ARRAYLEN(kIndexPaths); ++i) {
    q = MergePaths(path, pathlen, kIndexPaths[i], strlen(kIndexPaths[i]), &n);
    p = RoutePath(q, n);
    free(q);
  }
  return p;
}

static char *GetLuaResponse(void) {
  char *p;
  if (!(p = luaheaderp)) {
    p = SetStatus(200, "OK");
    p = AppendContentType(p, "text/html");
  }
  return p;
}

static char *LuaOnHttpRequest(void) {
  effectivepath.p = url.path.p;
  effectivepath.n = url.path.n;
  lua_getglobal(L, "OnHttpRequest");
  if (lua_pcall(L, 0, 0, 0) == LUA_OK) {
    return CommitOutput(GetLuaResponse());
  } else {
    WARNF("%s", lua_tostring(L, -1));
    lua_pop(L, 1);
    return ServeError(500, "Internal Server Error");
  }
}

static char *ServeLua(struct Asset *a, const char *s, size_t n) {
  char *code;
  LockInc(&shared->c.dynamicrequests);
  effectivepath.p = s;
  effectivepath.n = n;
  if ((code = FreeLater(LoadAsset(a, NULL)))) {
    if (luaL_dostring(L, code) == LUA_OK) {
      return CommitOutput(GetLuaResponse());
    } else {
      WARNF("%s", lua_tostring(L, -1));
      lua_pop(L, 1);
    }
  }
  return ServeError(500, "Internal Server Error");
}

static char *HandleRedirect(struct Redirect *r) {
  int code;
  struct Asset *a;
  if (!r->code && (a = GetAsset(r->location.s, r->location.n))) {
    LockInc(&shared->c.rewrites);
    DEBUGF("rewriting to %`'s", r->location.s);
    if (!HasString(&loops, r->location.s, r->location.n)) {
      AddString(&loops, r->location.s, r->location.n);
      return RoutePath(r->location.s, r->location.n);
    } else {
      LockInc(&shared->c.loops);
      return SetStatus(508, "Loop Detected");
    }
  } else if (msg.version < 10) {
    return ServeError(505, "HTTP Version Not Supported");
  } else {
    LockInc(&shared->c.redirects);
    code = r->code;
    if (!code) code = 307;
    DEBUGF("%d redirecting %`'s", code, r->location);
    return AppendHeader(
        SetStatus(code, GetHttpReason(code)), "Location",
        FreeLater(EncodeHttpHeaderValue(r->location.s, r->location.n, 0)));
  }
}

static char *HandleFolder(const char *path, size_t pathlen) {
  char *p;
  if (url.path.n && url.path.p[url.path.n - 1] != '/' &&
      SlicesEqual(path, pathlen, url.path.p, url.path.n)) {
    return RedirectSlash();
  }
  if ((p = ServeIndex(path, pathlen))) {
    return p;
  } else {
    LockInc(&shared->c.forbiddens);
    LOGF("directory %`'.*s lacks index page", pathlen, path);
    return ServeError(403, "Forbidden");
  }
}

static bool Reindex(void) {
  if (OpenZip(false)) {
    LockInc(&shared->c.reindexes);
    return true;
  } else {
    return false;
  }
}

static const char *LuaCheckPath(lua_State *L, int idx, size_t *pathlen) {
  const char *path;
  if (lua_isnoneornil(L, idx)) {
    path = url.path.p;
    *pathlen = url.path.n;
  } else {
    path = luaL_checklstring(L, idx, pathlen);
    if (!IsReasonablePath(path, *pathlen)) {
      WARNF("bad path %`'.*s", *pathlen, path);
      luaL_argerror(L, idx, "bad path");
      unreachable;
    }
  }
  return path;
}

static const char *LuaCheckHost(lua_State *L, int idx, size_t *hostlen) {
  const char *host;
  if (lua_isnoneornil(L, idx)) {
    host = url.host.p;
    *hostlen = url.host.n;
  } else {
    host = luaL_checklstring(L, idx, hostlen);
    if (!IsAcceptableHost(host, *hostlen)) {
      WARNF("bad host %`'.*s", *hostlen, host);
      luaL_argerror(L, idx, "bad host");
      unreachable;
    }
  }
  return host;
}

static int LuaServeListing(lua_State *L) {
  luaheaderp = ServeListing();
  return 0;
}

static int LuaServeStatusz(lua_State *L) {
  luaheaderp = ServeStatusz();
  return 0;
}

static int LuaServeAsset(lua_State *L) {
  size_t pathlen;
  struct Asset *a;
  const char *path;
  path = LuaCheckPath(L, 1, &pathlen);
  if ((a = GetAsset(path, pathlen)) && !S_ISDIR(GetMode(a))) {
    luaheaderp = ServeAsset(a, path, pathlen);
    lua_pushboolean(L, true);
  } else {
    lua_pushboolean(L, false);
  }
  return 1;
}

static int LuaServeIndex(lua_State *L) {
  size_t pathlen;
  const char *path;
  path = LuaCheckPath(L, 1, &pathlen);
  lua_pushboolean(L, !!(luaheaderp = ServeIndex(path, pathlen)));
  return 1;
}

static int LuaRoutePath(lua_State *L) {
  size_t pathlen;
  const char *path;
  path = LuaCheckPath(L, 1, &pathlen);
  lua_pushboolean(L, !!(luaheaderp = RoutePath(path, pathlen)));
  return 1;
}

static int LuaRouteHost(lua_State *L) {
  size_t hostlen, pathlen;
  const char *host, *path;
  host = LuaCheckHost(L, 1, &hostlen);
  path = LuaCheckPath(L, 2, &pathlen);
  lua_pushboolean(L, !!(luaheaderp = RouteHost(host, hostlen, path, pathlen)));
  return 1;
}

static int LuaRoute(lua_State *L) {
  size_t hostlen, pathlen;
  const char *host, *path;
  host = LuaCheckHost(L, 1, &hostlen);
  path = LuaCheckPath(L, 2, &pathlen);
  lua_pushboolean(L, !!(luaheaderp = Route(host, hostlen, path, pathlen)));
  return 1;
}

static int LuaRespond(lua_State *L, char *R(unsigned, const char *)) {
  char *p;
  int code;
  size_t reasonlen;
  const char *reason;
  code = luaL_checkinteger(L, 1);
  if (!(100 <= code && code <= 999)) {
    luaL_argerror(L, 1, "bad status code");
    unreachable;
  }
  if (lua_isnoneornil(L, 2)) {
    luaheaderp = R(code, GetHttpReason(code));
  } else {
    reason = lua_tolstring(L, 2, &reasonlen);
    if (reasonlen < 128 && (p = EncodeHttpHeaderValue(reason, reasonlen, 0))) {
      luaheaderp = R(code, p);
      free(p);
    } else {
      luaL_argerror(L, 2, "invalid");
      unreachable;
    }
  }
  return 0;
}

static int LuaSetStatus(lua_State *L) {
  return LuaRespond(L, SetStatus);
}

static int LuaServeError(lua_State *L) {
  return LuaRespond(L, ServeError);
}

static int LuaLoadAsset(lua_State *L) {
  void *data;
  struct Asset *a;
  const char *path;
  size_t size, pathlen;
  path = LuaCheckPath(L, 1, &pathlen);
  if ((a = GetAsset(path, pathlen))) {
    if (!a->file && !IsCompressed(a)) {
      /* fast path: this avoids extra copy */
      data = ZIP_LFILE_CONTENT(zbase + a->lf);
      size = GetZipLfileUncompressedSize(zbase + a->lf);
      if (Verify(data, size, ZIP_LFILE_CRC32(zbase + a->lf))) {
        lua_pushlstring(L, data, size);
        return 1;
      }
    } else if ((data = LoadAsset(a, &size))) {
      lua_pushlstring(L, data, size);
      free(data);
      return 1;
    } else {
      DEBUGF("could not load asset: %`'.*s", pathlen, path);
    }
  } else {
    DEBUGF("could not find asset: %`'.*s", pathlen, path);
  }
  return 0;
}

static void GetDosLocalTime(int64_t utcunixts, uint16_t *out_time,
                            uint16_t *out_date) {
  struct tm tm;
  CHECK_NOTNULL(localtime_r(&utcunixts, &tm));
  *out_time = DOS_TIME(tm.tm_hour, tm.tm_min, tm.tm_sec);
  *out_date = DOS_DATE(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday + 1);
}

static bool IsUtf8(const void *data, size_t size) {
  const unsigned char *p, *pe;
  for (p = data, pe = p + size; p + 2 <= pe; ++p) {
    if (p[0] >= 0300) {
      if (p[1] >= 0200 && p[1] < 0300) {
        return true;
      } else {
        return false;
      }
    }
  }
  return false;
}

static bool IsText(const void *data, size_t size) {
  const unsigned char *p, *pe;
  for (p = data, pe = p + size; p < pe; ++p) {
    if (*p <= 3) {
      return false;
    }
  }
  return true;
}

static int LuaStoreAsset(lua_State *L) {
  int64_t ft;
  int i, mode;
  uint32_t crc;
  char *comp, *p;
  long double now;
  struct Asset *a;
  struct iovec v[13];
  uint8_t *cdir, era;
  const char *path, *data, *use;
  uint16_t gflags, iattrs, mtime, mdate, dosmode, method, disk;
  size_t oldcdirsize, oldcdiroffset, records, cdiroffset, cdirsize, pathlen,
      datalen, complen, uselen;
  if (IsOpenbsd() || IsNetbsd() || IsWindows()) {
    luaL_error(L, "StoreAsset() not available on Windows/NetBSD/OpenBSD yet");
    unreachable;
  }
  path = LuaCheckPath(L, 1, &pathlen);
  if (pathlen > 0xffff) {
    luaL_argerror(L, 1, "path too long");
    unreachable;
  }
  data = luaL_checklstring(L, 2, &datalen);
  disk = gflags = iattrs = 0;
  if (IsUtf8(path, pathlen)) gflags |= kZipGflagUtf8;
  if (IsText(data, datalen)) iattrs |= kZipIattrText;
  crc = crc32_z(0, data, datalen);
  if (datalen < 100) {
    method = kZipCompressionNone;
    comp = 0;
    use = data;
    uselen = datalen;
    era = kZipEra1989;
  } else {
    comp = Deflate(data, datalen, &complen);
    if (complen < datalen) {
      method = kZipCompressionDeflate;
      use = comp;
      uselen = complen;
      era = kZipEra1993;
    } else {
      method = kZipCompressionNone;
      use = data;
      uselen = datalen;
      era = kZipEra1989;
    }
  }
  //////////////////////////////////////////////////////////////////////////////
  CHECK_NE(-1, fcntl(zfd, F_SETLKW, &(struct flock){F_WRLCK}));
  OpenZip(false);
  now = nowl();
  a = GetAsset(path, pathlen);
  mode = luaL_optinteger(L, 3, a ? GetMode(a) : 0644);
  if (!(mode & S_IFMT)) mode |= S_IFREG;
  if (pathlen > 1 && path[0] == '/') ++path, --pathlen;
  dosmode = !(mode & 0200) ? kNtFileAttributeReadonly : 0;
  ft = (now + MODERNITYSECONDS) * HECTONANOSECONDS;
  GetDosLocalTime(now, &mtime, &mdate);
  // local file header
  if (uselen >= 0xffffffff || datalen >= 0xffffffff) {
    era = kZipEra2001;
    v[2].iov_base = p = alloca((v[2].iov_len = 2 + 2 + 8 + 8));
    p = WRITE16LE(p, kZipExtraZip64);
    p = WRITE16LE(p, 8 + 8);
    p = WRITE64LE(p, uselen);
    p = WRITE64LE(p, datalen);
  } else {
    v[2].iov_len = 0;
    v[2].iov_base = 0;
  }
  v[0].iov_base = p = alloca((v[0].iov_len = kZipLfileHdrMinSize));
  p = WRITE32LE(p, kZipLfileHdrMagic);
  *p++ = era;
  *p++ = kZipOsDos;
  p = WRITE16LE(p, gflags);
  p = WRITE16LE(p, method);
  p = WRITE16LE(p, mtime);
  p = WRITE16LE(p, mdate);
  p = WRITE32LE(p, crc);
  p = WRITE32LE(p, MIN(uselen, 0xffffffff));
  p = WRITE32LE(p, MIN(datalen, 0xffffffff));
  p = WRITE16LE(p, pathlen);
  p = WRITE16LE(p, v[2].iov_len);
  v[1].iov_len = pathlen;
  v[1].iov_base = path;
  // file data
  v[3].iov_len = datalen;
  v[3].iov_base = data;
  // old central directory entries
  oldcdirsize = GetZipCdirSize(zcdir);
  oldcdiroffset = GetZipCdirOffset(zcdir);
  if (a) {
    v[4].iov_base = zbase + oldcdiroffset;
    v[4].iov_len = a->cf - oldcdiroffset;
    v[5].iov_base = zbase + oldcdiroffset + ZIP_CFILE_HDRSIZE(zbase + a->cf);
    v[5].iov_len =
        oldcdirsize - v[4].iov_len - ZIP_CFILE_HDRSIZE(zbase + a->cf);
  } else {
    v[4].iov_base = zbase + oldcdiroffset;
    v[4].iov_len = oldcdirsize;
    v[5].iov_base = 0;
    v[5].iov_len = 0;
  }
  // new central directory entry
  if (uselen >= 0xffffffff || datalen >= 0xffffffff || zsize >= 0xffffffff) {
    v[8].iov_base = p = alloca((v[8].iov_len = 2 + 2 + 8 + 8 + 8));
    p = WRITE16LE(p, kZipExtraZip64);
    p = WRITE16LE(p, 8 + 8 + 8);
    p = WRITE64LE(p, uselen);
    p = WRITE64LE(p, datalen);
    p = WRITE64LE(p, zsize);
  } else {
    v[8].iov_len = 0;
    v[8].iov_base = 0;
  }
  v[9].iov_base = p = alloca((v[9].iov_len = 2 + 2 + 4 + 2 + 2 + 8 + 8 + 8));
  p = WRITE16LE(p, kZipExtraNtfs);
  p = WRITE16LE(p, 4 + 2 + 2 + 8 + 8 + 8);
  p = WRITE32LE(p, 0);
  p = WRITE16LE(p, 1);
  p = WRITE16LE(p, 8 + 8 + 8);
  p = WRITE64LE(p, ft);
  p = WRITE64LE(p, ft);
  p = WRITE64LE(p, ft);
  v[6].iov_base = p = alloca((v[6].iov_len = kZipCfileHdrMinSize));
  p = WRITE32LE(p, kZipCfileHdrMagic);
  *p++ = kZipCosmopolitanVersion;
  *p++ = kZipOsUnix;
  *p++ = era;
  *p++ = kZipOsDos;
  p = WRITE16LE(p, gflags);
  p = WRITE16LE(p, method);
  p = WRITE16LE(p, mtime);
  p = WRITE16LE(p, mdate);
  p = WRITE32LE(p, crc);
  p = WRITE32LE(p, MIN(uselen, 0xffffffff));
  p = WRITE32LE(p, MIN(datalen, 0xffffffff));
  p = WRITE16LE(p, pathlen);
  p = WRITE16LE(p, v[8].iov_len + v[9].iov_len);
  p = WRITE16LE(p, 0);
  p = WRITE16LE(p, disk);
  p = WRITE16LE(p, iattrs);
  p = WRITE16LE(p, dosmode);
  p = WRITE16LE(p, mode);
  p = WRITE32LE(p, MIN(zsize, 0xffffffff));
  v[7].iov_len = pathlen;
  v[7].iov_base = path;
  // zip64 end of central directory
  cdiroffset =
      zsize + v[0].iov_len + v[1].iov_len + v[2].iov_len + v[3].iov_len;
  cdirsize = v[4].iov_len + v[5].iov_len + v[6].iov_len + v[7].iov_len +
             v[8].iov_len + v[9].iov_len;
  records = GetZipCdirRecords(zcdir) + !a;
  if (records >= 0xffff || cdiroffset >= 0xffffffff || cdirsize >= 0xffffffff) {
    v[10].iov_base = p =
        alloca((v[10].iov_len = kZipCdir64HdrMinSize + kZipCdir64LocatorSize));
    p = WRITE32LE(p, kZipCdir64HdrMagic);
    p = WRITE64LE(p, 2 + 2 + 4 + 4 + 8 + 8 + 8 + 8);
    p = WRITE16LE(p, kZipCosmopolitanVersion);
    p = WRITE16LE(p, kZipEra2001);
    p = WRITE32LE(p, disk);
    p = WRITE32LE(p, disk);
    p = WRITE64LE(p, records);
    p = WRITE64LE(p, records);
    p = WRITE64LE(p, cdirsize);
    p = WRITE64LE(p, cdiroffset);
    p = WRITE32LE(p, kZipCdir64LocatorMagic);
    p = WRITE32LE(p, disk);
    p = WRITE64LE(p, cdiroffset + cdirsize);
    p = WRITE32LE(p, disk);
  } else {
    v[10].iov_len = 0;
    v[10].iov_base = 0;
  }
  // end of central directory
  v[12].iov_base = GetZipCdirComment(zcdir);
  v[12].iov_len = GetZipCdirCommentSize(zcdir);
  v[11].iov_base = p = alloca((v[11].iov_len = kZipCdirHdrMinSize));
  p = WRITE32LE(p, kZipCdirHdrMagic);
  p = WRITE16LE(p, disk);
  p = WRITE16LE(p, disk);
  p = WRITE16LE(p, MIN(records, 0xffff));
  p = WRITE16LE(p, MIN(records, 0xffff));
  p = WRITE32LE(p, MIN(cdirsize, 0xffffffff));
  p = WRITE32LE(p, MIN(cdiroffset, 0xffffffff));
  p = WRITE16LE(p, v[12].iov_len);
  CHECK_NE(-1, lseek(zfd, zbase + zsize - zmap, SEEK_SET));
  CHECK_NE(-1, WritevAll(zfd, v, 13));
  CHECK_NE(-1, fcntl(zfd, F_SETLK, &(struct flock){F_UNLCK}));
  //////////////////////////////////////////////////////////////////////////////
  OpenZip(false);
  free(comp);
  return 0;
}

static void ReseedRng(mbedtls_ctr_drbg_context *r, const char *s) {
#ifndef UNSECURE
  CHECK_EQ(0, mbedtls_ctr_drbg_reseed(r, (void *)s, strlen(s)));
#endif
}

static char *TlsError(int r) {
  static char b[128];
  mbedtls_strerror(r, b, sizeof(b));
  return b;
}

static wontreturn void LuaThrowTlsError(lua_State *L, const char *s, int r) {
  const char *code;
  code = gc(xasprintf("-0x%04x", -r));
  if (!IsTiny()) {
    luaL_error(L, "tls %s failed (%s %s)", s, code, TlsError(r));
  } else {
    luaL_error(L, "tls %s failed (grep %s)", s, code);
  }
  unreachable;
}

static bool Tune(int fd, int a, int b, int x) {
  if (!b) return false;
  return setsockopt(fd, a, b, &x, sizeof(x)) != -1;
}

static int Socket(int family, int type, int protocol, bool isserver) {
  int fd;
  if ((fd = socket(family, type, protocol)) != -1) {
    if (isserver) {
      Tune(fd, SOL_TCP, TCP_FASTOPEN, 100);
      Tune(fd, SOL_SOCKET, SO_REUSEADDR, 1);
    } else {
      Tune(fd, SOL_TCP, TCP_FASTOPEN_CONNECT, 1);
    }
    if (!Tune(fd, SOL_TCP, TCP_QUICKACK, 1)) {
      Tune(fd, SOL_TCP, TCP_NODELAY, 1);
    }
    if (timeout.tv_sec < 0) {
      Tune(fd, SOL_SOCKET, SO_KEEPALIVE, 1);
      Tune(fd, SOL_TCP, TCP_KEEPIDLE, -timeout.tv_sec);
      Tune(fd, SOL_TCP, TCP_KEEPINTVL, -timeout.tv_sec);
    } else {
      setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
      setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    }
  }
  return fd;
}

static char *FoldHeader(struct HttpMessage *msg, char *b, int h, size_t *z) {
  char *p;
  size_t i, n, m;
  struct HttpHeader *x;
  n = msg->headers[h].b - msg->headers[h].a;
  p = xmalloc(n);
  memcpy(p, b + msg->headers[h].a, n);
  for (i = 0; i < msg->xheaders.n; ++i) {
    x = msg->xheaders.p + i;
    if (GetHttpHeader(b + x->k.a, x->k.b - x->k.a) == h) {
      m = x->v.b - x->v.a;
      p = xrealloc(p, n + 2 + m);
      memcpy(mempcpy(p + n, ", ", 2), b + x->v.a, m);
      n += 2 + m;
    }
  }
  *z = n;
  return p;
}

static void LuaPushLatin1(lua_State *L, const char *s, size_t n) {
  char *t;
  size_t m;
  t = DecodeLatin1(s, n, &m);
  lua_pushlstring(L, t, m);
  free(t);
}

static int LuaPushHeader(lua_State *L, struct HttpMessage *m, char *b, int h) {
  char *val;
  size_t vallen;
  if (!kHttpRepeatable[h]) {
    LuaPushLatin1(L, b + m->headers[h].a, m->headers[h].b - m->headers[h].a);
  } else {
    val = FoldHeader(m, b, h, &vallen);
    LuaPushLatin1(L, val, vallen);
    free(val);
  }
  return 1;
}

static int LuaPushHeaders(lua_State *L, struct HttpMessage *m, const char *b) {
  char *s;
  size_t i, h;
  struct HttpHeader *x;
  lua_newtable(L);
  for (h = 0; h < kHttpHeadersMax; ++h) {
    if (m->headers[h].a) {
      LuaPushHeader(L, m, b, h);
      lua_setfield(L, -2, GetHttpHeaderName(h));
    }
  }
  for (i = 0; i < m->xheaders.n; ++i) {
    x = m->xheaders.p + i;
    if ((h = GetHttpHeader(b + x->v.a, x->v.b - x->v.a)) == -1) {
      LuaPushLatin1(L, b + x->v.a, x->v.b - x->v.a);
      lua_setfield(L, -2, (s = DecodeLatin1(b + x->k.a, x->k.b - x->k.a, 0)));
      free(s);
    }
  }
  return 1;
}

static void LogMessage(const char *d, const char *s, size_t n) {
  size_t n2, n3;
  char *s2, *s3;
  if (!LOGGABLE(kLogInfo)) return;
  while (n && (s[n - 1] == '\r' || s[n - 1] == '\n')) --n;
  if ((s2 = DecodeLatin1(s, n, &n2))) {
    if ((s3 = IndentLines(s2, n2, &n3, 1))) {
      LOGF("%s %,ld byte message\n%.*s", d, n, n3, s3);
      free(s3);
    }
    free(s2);
  }
}

static void LogBody(const char *d, const char *s, size_t n) {
  char *s2, *s3;
  size_t n2, n3;
  if (!n) return;
  if (!LOGGABLE(kLogInfo)) return;
  while (n && (s[n - 1] == '\r' || s[n - 1] == '\n')) --n;
  if ((s2 = VisualizeControlCodes(s, n, &n2))) {
    if ((s3 = IndentLines(s2, n2, &n3, 1))) {
      LOGF("%s %,ld byte payload\n%.*s", d, n, n3, s3);
      free(s3);
    }
    free(s2);
  }
}

static int LuaFetch(lua_State *L) {
  char *p;
  ssize_t rc;
  bool usessl;
  uint32_t ip;
  struct Url url;
  int t, ret, sock, method;
  char *host, *port;
  struct TlsBio *bio;
  struct Buffer inbuf;
  struct addrinfo *addr;
  struct HttpMessage msg;
  struct HttpUnchunker u;
  const char *urlarg, *request, *body;
  size_t urlarglen, requestlen;
  size_t g, i, n, hdrsize, paylen;
  struct addrinfo hints = {.ai_family = AF_INET,
                           .ai_socktype = SOCK_STREAM,
                           .ai_protocol = IPPROTO_TCP,
                           .ai_flags = AI_NUMERICSERV};

  /*
   * Get args.
   */
  urlarg = luaL_checklstring(L, 1, &urlarglen);
  if (lua_isnoneornil(L, 2)) {
    body = "";
    method = kHttpGet;
  } else {
    size_t bodylen;
    body = luaL_checklstring(L, 2, &bodylen);
    method = kHttpPost;
  }

  /*
   * Parse URL.
   */
  gc(ParseUrl(urlarg, urlarglen, &url));
  gc(url.params.p);
  usessl = false;
  if (url.scheme.n) {
#ifndef UNSECURE
    if (url.scheme.n == 5 && !memcasecmp(url.scheme.p, "https", 5)) {
      usessl = true;
    } else
#endif
        if (!(url.scheme.n == 4 && !memcasecmp(url.scheme.p, "http", 4))) {
      luaL_argerror(L, 1, "bad scheme");
      unreachable;
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
    ip = ntohl(servers.p[0].addr.sin_addr.s_addr);
    host =
        gc(xasprintf("%hhu.%hhu.%hhu.%hhu", ip >> 24, ip >> 16, ip >> 8, ip));
    port = gc(xasprintf("%d", ntohs(servers.p[0].addr.sin_port)));
  }
  if (!IsAcceptableHost(host, -1)) {
    luaL_argerror(L, 1, "invalid host");
    unreachable;
  }
  url.fragment.p = 0, url.fragment.n = 0;
  url.scheme.p = 0, url.scheme.n = 0;
  url.user.p = 0, url.user.n = 0;
  url.pass.p = 0, url.pass.n = 0;
  url.host.p = 0, url.host.n = 0;
  url.port.p = 0, url.port.n = 0;
  if (!url.path.n || url.path.p[0] != '/') {
    p = gc(xmalloc(1 + url.path.n));
    mempcpy(mempcpy(p, "/", 1), url.path.p, url.path.n);
    url.path.p = p;
    ++url.path.n;
  }

  /*
   * Create HTTP message.
   */
  request = gc(xasprintf("%s %s HTTP/1.1\r\n"
                         "Host: %s:%s\r\n"
                         "Connection: close\r\n"
                         "User-Agent: %s\r\n"
                         "\r\n%s",
                         kHttpMethod[method], gc(EncodeUrl(&url, 0)), host,
                         port, brand, body));
  requestlen = strlen(request);

  /*
   * Perform DNS lookup.
   */
  DEBUGF("client resolving %s", host);
  if ((rc = getaddrinfo(host, port, &hints, &addr)) != EAI_SUCCESS) {
    luaL_error(L, "getaddrinfo(%s:%s) failed", host, port);
    unreachable;
  }

  /*
   * Connect to server.
   */
  ip = ntohl(((struct sockaddr_in *)addr->ai_addr)->sin_addr.s_addr);
  DEBUGF("client connecting %hhu.%hhu.%hhu.%hhu:%d", ip >> 24, ip >> 16,
         ip >> 8, ip, ntohs(((struct sockaddr_in *)addr->ai_addr)->sin_port));
  CHECK_NE(-1, (sock = Socket(addr->ai_family, addr->ai_socktype,
                              addr->ai_protocol, false)));
  if (connect(sock, addr->ai_addr, addr->ai_addrlen) == -1) {
    close(sock);
    luaL_error(L, "connect(%s:%s) failed: %s", host, port, strerror(errno));
    unreachable;
  }
  if (usessl) {
    if (sslcliused) {
      mbedtls_ssl_session_reset(&sslcli);
    } else {
      ReseedRng(&rngcli, "child");
    }
    sslcliused = true;
    DEBUGF("client handshaking %`'s", host);
    mbedtls_ssl_set_hostname(&sslcli, host);
    bio = gc(malloc(sizeof(struct TlsBio)));
    bio->fd = sock;
    bio->a = 0;
    bio->b = 0;
    bio->c = -1;
    mbedtls_ssl_set_bio(&sslcli, bio, TlsSend, 0, TlsRecvImpl);
    while ((ret = mbedtls_ssl_handshake(&ssl))) {
      switch (ret) {
        case MBEDTLS_ERR_SSL_WANT_READ:
          break;
        case MBEDTLS_ERR_X509_CERT_VERIFY_FAILED:
          goto VerifyFailed;
        default:
          close(sock);
          LuaThrowTlsError(L, "handshake", ret);
      }
    }
    LockInc(&shared->c.sslhandshakes);
    VERBOSEF("SHAKEN %s:%s %s %s", host, port,
             mbedtls_ssl_get_ciphersuite(&ssl), mbedtls_ssl_get_version(&ssl));
  }

  /*
   * Send HTTP Message.
   */
  DEBUGF("client sending %s request", kHttpMethod[method]);
  if (usessl) {
    ret = mbedtls_ssl_write(&sslcli, request, requestlen);
    if (ret != requestlen) {
      if (ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) goto VerifyFailed;
      close(sock);
      LuaThrowTlsError(L, "write", ret);
    }
  } else if (write(sock, request, requestlen) != requestlen) {
    close(sock);
    luaL_error(L, "write failed: %s", strerror(errno));
    unreachable;
  }
  if (logmessages) {
    LogMessage("sent", request, requestlen);
  }

  /*
   * Handle response.
   */
  memset(&inbuf, 0, sizeof(inbuf));
  InitHttpMessage(&msg, kHttpResponse);
  for (hdrsize = paylen = t = 0;;) {
    if (inbuf.n == inbuf.c) {
      inbuf.c += 1000;
      inbuf.c += inbuf.c >> 1;
      inbuf.p = realloc(inbuf.p, inbuf.c);
    }
    DEBUGF("client reading");
    if (usessl) {
      if ((rc = mbedtls_ssl_read(&sslcli, inbuf.p + inbuf.n,
                                 inbuf.c - inbuf.n)) < 0) {
        if (rc == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
          rc = 0;
        } else {
          close(sock);
          free(inbuf.p);
          DestroyHttpMessage(&msg);
          LuaThrowTlsError(L, "read", rc);
        }
      }
    } else if ((rc = read(sock, inbuf.p + inbuf.n, inbuf.c - inbuf.n)) == -1) {
      close(sock);
      free(inbuf.p);
      DestroyHttpMessage(&msg);
      luaL_error(L, "read failed: %s", strerror(errno));
      unreachable;
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
          if (logmessages) {
            LogMessage("received", inbuf.p, hdrsize);
          }
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
              memset(&u, 0, sizeof(u));
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
  if (paylen && logbodies) LogBody("received", inbuf.p + hdrsize, paylen);
  LOGF("FETCH HTTP%02d %d %s %`'.*s", msg.version, msg.status, urlarg,
       HeaderLength(kHttpServer), HeaderData(kHttpServer));
  lua_pushinteger(L, msg.status);
  LuaPushHeaders(L, &msg, inbuf.p);
  lua_pushlstring(L, inbuf.p + hdrsize, paylen);
  DestroyHttpMessage(&msg);
  free(inbuf.p);
  close(sock);
  return 3;
TransportError:
  close(sock);
  free(inbuf.p);
  DestroyHttpMessage(&msg);
  luaL_error(L, "transport error");
  unreachable;
VerifyFailed:
  LockInc(&shared->c.sslverifyfailed);
  close(sock);
  LuaThrowTlsError(
      L, gc(DescribeSslVerifyFailure(sslcli.session_negotiate->verify_result)),
      ret);
}

static int LuaGetDate(lua_State *L) {
  lua_pushinteger(L, shared->nowish);
  return 1;
}

static int LuaGetVersion(lua_State *L) {
  lua_pushinteger(L, msg.version);
  return 1;
}

static int LuaGetMethod(lua_State *L) {
  if (msg.method) {
    lua_pushstring(L, kHttpMethod[msg.method]);
  } else {
    lua_pushlstring(L, inbuf.p + msg.xmethod.a, msg.xmethod.b - msg.xmethod.a);
  }
  return 1;
}

static int LuaGetAddr(lua_State *L, void GetAddr(uint32_t *, uint16_t *)) {
  uint32_t ip;
  uint16_t port;
  GetAddr(&ip, &port);
  lua_pushinteger(L, ip);
  lua_pushinteger(L, port);
  return 2;
}

static int LuaGetServerAddr(lua_State *L) {
  return LuaGetAddr(L, GetServerAddr);
}

static int LuaGetClientAddr(lua_State *L) {
  return LuaGetAddr(L, GetClientAddr);
}

static int LuaGetRemoteAddr(lua_State *L) {
  return LuaGetAddr(L, GetRemoteAddr);
}

static int LuaFormatIp(lua_State *L) {
  char b[16];
  uint32_t ip;
  ip = htonl(luaL_checkinteger(L, 1));
  inet_ntop(AF_INET, &ip, b, sizeof(b));
  lua_pushstring(L, b);
  return 1;
}

static int LuaParseIp(lua_State *L) {
  size_t n;
  const char *s;
  s = luaL_checklstring(L, 1, &n);
  lua_pushinteger(L, ParseIp(s, n));
  return 1;
}

static int LuaIsIp(lua_State *L, bool IsIp(uint32_t)) {
  lua_pushboolean(L, IsIp(luaL_checkinteger(L, 1)));
  return 1;
}

static int LuaIsPublicIp(lua_State *L) {
  return LuaIsIp(L, IsPublicIp);
}

static int LuaIsPrivateIp(lua_State *L) {
  return LuaIsIp(L, IsPrivateIp);
}

static int LuaIsLoopbackIp(lua_State *L) {
  return LuaIsIp(L, IsLoopbackIp);
}

static int LuaCategorizeIp(lua_State *L) {
  lua_pushstring(L, GetIpCategoryName(CategorizeIp(luaL_checkinteger(L, 1))));
  return 1;
}

static int LuaGetUrl(lua_State *L) {
  char *p;
  size_t n;
  p = EncodeUrl(&url, &n);
  lua_pushlstring(L, p, n);
  free(p);
  return 1;
}

static void LuaPushUrlView(lua_State *L, struct UrlView *v) {
  if (v->p) {
    lua_pushlstring(L, v->p, v->n);
  } else {
    lua_pushnil(L);
  }
}

static int LuaGetScheme(lua_State *L) {
  LuaPushUrlView(L, &url.scheme);
  return 1;
}

static int LuaGetPath(lua_State *L) {
  LuaPushUrlView(L, &url.path);
  return 1;
}

static int LuaGetEffectivePath(lua_State *L) {
  lua_pushlstring(L, effectivepath.p, effectivepath.n);
  return 1;
}

static int LuaGetFragment(lua_State *L) {
  LuaPushUrlView(L, &url.fragment);
  return 1;
}

static int LuaGetUser(lua_State *L) {
  size_t n;
  const char *p, *q;
  if (url.user.p) {
    LuaPushUrlView(L, &url.user);
  } else if ((p = GetBasicAuthorization(&n))) {
    if (!(q = memchr(p, ':', n))) q = p + n;
    lua_pushlstring(L, p, q - p);
    free(p);
  } else {
    lua_pushnil(L);
  }
  return 1;
}

static int LuaGetPass(lua_State *L) {
  size_t n;
  const char *p, *q;
  if (url.user.p) {
    LuaPushUrlView(L, &url.pass);
  } else if ((p = GetBasicAuthorization(&n))) {
    if ((q = memchr(p, ':', n))) {
      lua_pushlstring(L, q + 1, p + n - (q + 1));
    } else {
      lua_pushnil(L);
    }
    free(p);
  } else {
    lua_pushnil(L);
  }
  return 1;
}

static int LuaGetHost(lua_State *L) {
  char b[16];
  if (url.host.n) {
    lua_pushlstring(L, url.host.p, url.host.n);
  } else {
    inet_ntop(AF_INET, &serveraddr->sin_addr.s_addr, b, sizeof(b));
    lua_pushstring(L, b);
  }
  return 1;
}

static int LuaGetPort(lua_State *L) {
  int i, x = 0;
  for (i = 0; i < url.port.n; ++i) x = url.port.p[i] - '0' + x * 10;
  if (!x) x = ntohs(serveraddr->sin_port);
  lua_pushinteger(L, x);
  return 1;
}

static int LuaFormatHttpDateTime(lua_State *L) {
  char buf[30];
  lua_pushstring(L, FormatUnixHttpDateTime(buf, luaL_checkinteger(L, 1)));
  return 1;
}

static int LuaParseHttpDateTime(lua_State *L) {
  size_t n;
  const char *s;
  s = luaL_checklstring(L, 1, &n);
  lua_pushinteger(L, ParseHttpDateTime(s, n));
  return 1;
}

static int LuaGetPayload(lua_State *L) {
  lua_pushlstring(L, inbuf.p + hdrsize, payloadlength);
  return 1;
}

static int LuaGetHeader(lua_State *L) {
  int h;
  const char *key;
  size_t i, keylen;
  key = luaL_checklstring(L, 1, &keylen);
  if ((h = GetHttpHeader(key, keylen)) != -1) {
    if (msg.headers[h].a) {
      return LuaPushHeader(L, &msg, inbuf.p, h);
    }
  } else {
    for (i = 0; i < msg.xheaders.n; ++i) {
      if (SlicesEqualCase(key, keylen, inbuf.p + msg.xheaders.p[i].k.a,
                          msg.xheaders.p[i].k.b - msg.xheaders.p[i].k.a)) {
        LuaPushLatin1(L, inbuf.p + msg.xheaders.p[i].v.a,
                      msg.xheaders.p[i].v.b - msg.xheaders.p[i].v.a);
        return 1;
      }
    }
  }
  lua_pushnil(L);
  return 1;
}

static int LuaGetHeaders(lua_State *L) {
  return LuaPushHeaders(L, &msg, inbuf.p);
}

static int LuaSetHeader(lua_State *L) {
  int h;
  ssize_t rc;
  char *p, *q;
  const char *key, *val, *eval;
  size_t i, keylen, vallen, evallen;
  key = luaL_checklstring(L, 1, &keylen);
  val = luaL_checklstring(L, 2, &vallen);
  if ((h = GetHttpHeader(key, keylen)) == -1) {
    if (!IsValidHttpToken(key, keylen)) {
      luaL_argerror(L, 1, "invalid");
      unreachable;
    }
  }
  if (!(eval = EncodeHttpHeaderValue(val, vallen, &evallen))) {
    luaL_argerror(L, 2, "invalid");
    unreachable;
  }
  p = GetLuaResponse();
  while (p - hdrbuf.p + keylen + 2 + evallen + 2 + 512 > hdrbuf.n) {
    hdrbuf.n += hdrbuf.n >> 1;
    q = xrealloc(hdrbuf.p, hdrbuf.n);
    luaheaderp = p = q + (p - hdrbuf.p);
    hdrbuf.p = q;
  }
  switch (h) {
    case kHttpConnection:
      if (SlicesEqualCase(eval, evallen, "close", 5)) {
        luaL_argerror(L, 2, "unsupported");
        unreachable;
      }
      connectionclose = true;
      break;
    case kHttpContentType:
      p = AppendContentType(p, eval);
      break;
    case kHttpServer:
      branded = true;
      p = AppendHeader(p, "Server", eval);
      break;
    default:
      p = AppendHeader(p, key, eval);
      break;
  }
  luaheaderp = p;
  free(eval);
  return 0;
}

static int LuaHasParam(lua_State *L) {
  size_t i, n;
  const char *s;
  s = luaL_checklstring(L, 1, &n);
  for (i = 0; i < url.params.n; ++i) {
    if (SlicesEqual(s, n, url.params.p[i].key.p, url.params.p[i].key.n)) {
      lua_pushboolean(L, true);
      return 1;
    }
  }
  lua_pushboolean(L, false);
  return 1;
}

static int LuaGetParam(lua_State *L) {
  size_t i, n;
  const char *s;
  s = luaL_checklstring(L, 1, &n);
  for (i = 0; i < url.params.n; ++i) {
    if (SlicesEqual(s, n, url.params.p[i].key.p, url.params.p[i].key.n)) {
      if (url.params.p[i].val.p) {
        lua_pushlstring(L, url.params.p[i].val.p, url.params.p[i].val.n);
        return 1;
      } else {
        break;
      }
    }
  }
  lua_pushnil(L);
  return 1;
}

static void LuaPushUrlParams(lua_State *L, struct UrlParams *h) {
  size_t i;
  lua_newtable(L);
  for (i = 0; i < h->n; ++i) {
    lua_newtable(L);
    lua_pushlstring(L, h->p[i].key.p, h->p[i].key.n);
    lua_seti(L, -2, 1);
    if (h->p[i].val.p) {
      lua_pushlstring(L, h->p[i].val.p, h->p[i].val.n);
      lua_seti(L, -2, 2);
    }
    lua_seti(L, -2, i + 1);
  }
}

static int LuaGetParams(lua_State *L) {
  LuaPushUrlParams(L, &url.params);
  return 1;
}

static int LuaParseParams(lua_State *L) {
  void *m;
  size_t size;
  const char *data;
  struct UrlParams h;
  data = luaL_checklstring(L, 1, &size);
  memset(&h, 0, sizeof(h));
  m = ParseParams(data, size, &h);
  LuaPushUrlParams(L, &h);
  free(h.p);
  free(m);
  return 1;
}

static void LuaSetUrlView(lua_State *L, struct UrlView *v, const char *k) {
  LuaPushUrlView(L, v);
  lua_setfield(L, -2, k);
}

static int LuaParseUrl(lua_State *L) {
  void *m;
  size_t n;
  struct Url h;
  const char *p;
  p = luaL_checklstring(L, 1, &n);
  m = ParseUrl(p, n, &h);
  lua_newtable(L);
  LuaSetUrlView(L, &h.scheme, "scheme");
  LuaSetUrlView(L, &h.user, "user");
  LuaSetUrlView(L, &h.pass, "pass");
  LuaSetUrlView(L, &h.host, "host");
  LuaSetUrlView(L, &h.port, "port");
  LuaSetUrlView(L, &h.path, "path");
  LuaSetUrlView(L, &h.fragment, "fragment");
  LuaPushUrlParams(L, &h.params);
  lua_setfield(L, -2, "params");
  free(h.params.p);
  free(m);
  return 1;
}

static int LuaParseHost(lua_State *L) {
  void *m;
  size_t n;
  struct Url h;
  const char *p;
  memset(&h, 0, sizeof(h));
  p = luaL_checklstring(L, 1, &n);
  m = ParseHost(p, n, &h);
  lua_newtable(L);
  LuaPushUrlView(L, &h.host);
  LuaPushUrlView(L, &h.port);
  free(m);
  return 1;
}

static int LuaEncodeUrl(lua_State *L) {
  void *m;
  size_t size;
  struct Url h;
  int i, j, k, n;
  const char *data;
  if (!lua_isnil(L, 1)) {
    memset(&h, 0, sizeof(h));
    luaL_checktype(L, 1, LUA_TTABLE);
    if (lua_getfield(L, 1, "scheme"))
      h.scheme.p = lua_tolstring(L, -1, &h.scheme.n);
    if (lua_getfield(L, 1, "fragment"))
      h.fragment.p = lua_tolstring(L, -1, &h.fragment.n);
    if (lua_getfield(L, 1, "user")) h.user.p = lua_tolstring(L, -1, &h.user.n);
    if (lua_getfield(L, 1, "pass")) h.pass.p = lua_tolstring(L, -1, &h.pass.n);
    if (lua_getfield(L, 1, "host")) h.host.p = lua_tolstring(L, -1, &h.host.n);
    if (lua_getfield(L, 1, "port")) h.port.p = lua_tolstring(L, -1, &h.port.n);
    if (lua_getfield(L, 1, "path")) h.path.p = lua_tolstring(L, -1, &h.path.n);
    if (lua_getfield(L, 1, "params")) {
      luaL_checktype(L, -1, LUA_TTABLE);
      lua_len(L, -1);
      n = lua_tointeger(L, -1);
      for (i = -2, k = 0, j = 1; j <= n; ++j) {
        if (lua_geti(L, i--, j)) {
          luaL_checktype(L, -1, LUA_TTABLE);
          if (lua_geti(L, -1, 1)) {
            h.params.p =
                xrealloc(h.params.p, ++h.params.n * sizeof(*h.params.p));
            h.params.p[h.params.n - 1].key.p =
                lua_tolstring(L, -1, &h.params.p[h.params.n - 1].key.n);
            if (lua_geti(L, -2, 2)) {
              h.params.p[h.params.n - 1].val.p =
                  lua_tolstring(L, -1, &h.params.p[h.params.n - 1].val.n);
            } else {
              h.params.p[h.params.n - 1].val.p = 0;
              h.params.p[h.params.n - 1].val.n = 0;
            }
          }
          i--;
        }
        i--;
      }
    }
    data = EncodeUrl(&h, &size);
    lua_pushlstring(L, data, size);
    free(data);
  } else {
    lua_pushnil(L);
  }
  return 1;
}

static int LuaWrite(lua_State *L) {
  size_t size;
  const char *data;
  if (!lua_isnil(L, 1)) {
    data = luaL_checklstring(L, 1, &size);
    AppendData(data, size);
  }
  return 0;
}

static int LuaCheckControlFlags(lua_State *L, int idx) {
  int f = luaL_checkinteger(L, idx);
  if (f & ~(kControlWs | kControlC0 | kControlC1)) {
    luaL_argerror(L, idx, "invalid control flags");
    unreachable;
  }
  return f;
}

static int LuaHasControlCodes(lua_State *L) {
  int f;
  size_t n;
  const char *p;
  p = luaL_checklstring(L, 1, &n);
  f = LuaCheckControlFlags(L, 2);
  lua_pushboolean(L, HasControlCodes(p, n, f) != -1);
  return 1;
}

static int LuaIsValid(lua_State *L, bool V(const char *, size_t)) {
  size_t size;
  const char *data;
  data = luaL_checklstring(L, 1, &size);
  lua_pushboolean(L, V(data, size));
  return 1;
}

static int LuaIsValidHttpToken(lua_State *L) {
  return LuaIsValid(L, IsValidHttpToken);
}

static int LuaIsAcceptablePath(lua_State *L) {
  return LuaIsValid(L, IsAcceptablePath);
}

static int LuaIsReasonablePath(lua_State *L) {
  return LuaIsValid(L, IsReasonablePath);
}

static int LuaIsAcceptableHost(lua_State *L) {
  return LuaIsValid(L, IsAcceptableHost);
}

static int LuaIsAcceptablePort(lua_State *L) {
  return LuaIsValid(L, IsAcceptablePort);
}

static noinline int LuaCoderImpl(lua_State *L,
                                 char *C(const char *, size_t, size_t *)) {
  void *p;
  size_t n;
  p = luaL_checklstring(L, 1, &n);
  p = C(p, n, &n);
  lua_pushlstring(L, p, n);
  free(p);
  return 1;
}

static noinline int LuaCoder(lua_State *L,
                             char *C(const char *, size_t, size_t *)) {
  return LuaCoderImpl(L, C);
}

static int LuaUnderlong(lua_State *L) {
  return LuaCoder(L, Underlong);
}

static int LuaEncodeBase64(lua_State *L) {
  return LuaCoder(L, EncodeBase64);
}

static int LuaDecodeBase64(lua_State *L) {
  return LuaCoder(L, DecodeBase64);
}

static int LuaDecodeLatin1(lua_State *L) {
  return LuaCoder(L, DecodeLatin1);
}

static int LuaEscapeHtml(lua_State *L) {
  return LuaCoder(L, EscapeHtml);
}

static int LuaEscapeParam(lua_State *L) {
  return LuaCoder(L, EscapeParam);
}

static int LuaEscapePath(lua_State *L) {
  return LuaCoder(L, EscapePath);
}

static int LuaEscapeHost(lua_State *L) {
  return LuaCoder(L, EscapeHost);
}

static int LuaEscapeIp(lua_State *L) {
  return LuaCoder(L, EscapeIp);
}

static int LuaEscapeUser(lua_State *L) {
  return LuaCoder(L, EscapeUser);
}

static int LuaEscapePass(lua_State *L) {
  return LuaCoder(L, EscapePass);
}

static int LuaEscapeSegment(lua_State *L) {
  return LuaCoder(L, EscapeSegment);
}

static int LuaEscapeFragment(lua_State *L) {
  return LuaCoder(L, EscapeFragment);
}

static int LuaEscapeLiteral(lua_State *L) {
  return LuaCoder(L, EscapeJsStringLiteral);
}

static int LuaVisualizeControlCodes(lua_State *L) {
  return LuaCoder(L, VisualizeControlCodes);
}

static int Sha224(const void *p, size_t n, uint8_t d[28]) {
  return mbedtls_sha256_ret(p, n, d, 1);
}

static int Sha256(const void *p, size_t n, uint8_t d[32]) {
  return mbedtls_sha256_ret(p, n, d, 0);
}

static int Sha384(const void *p, size_t n, uint8_t d[48]) {
  return mbedtls_sha512_ret(p, n, d, 1);
}

static int Sha512(const void *p, size_t n, uint8_t d[64]) {
  return mbedtls_sha512_ret(p, n, d, 0);
}

static noinline int LuaHasherImpl(lua_State *L, size_t k,
                                  int H(const void *, size_t, uint8_t *)) {
  void *p;
  size_t n;
  uint8_t d[64];
  p = luaL_checklstring(L, 1, &n);
  H(p, n, d);
  lua_pushlstring(L, (void *)d, k);
  mbedtls_platform_zeroize(d, sizeof(d));
  return 1;
}

static noinline int LuaHasher(lua_State *L, size_t k,
                              int H(const void *, size_t, uint8_t *)) {
  return LuaHasherImpl(L, k, H);
}

static int LuaMd5(lua_State *L) {
  return LuaHasher(L, 16, mbedtls_md5_ret);
}

static int LuaSha1(lua_State *L) {
  return LuaHasher(L, 20, mbedtls_sha1_ret);
}

static int LuaSha224(lua_State *L) {
  return LuaHasher(L, 28, Sha224);
}

static int LuaSha256(lua_State *L) {
  return LuaHasher(L, 32, Sha256);
}

static int LuaSha384(lua_State *L) {
  return LuaHasher(L, 48, Sha384);
}

static int LuaSha512(lua_State *L) {
  return LuaHasher(L, 64, Sha512);
}

static int LuaGetHttpReason(lua_State *L) {
  lua_pushstring(L, GetHttpReason(luaL_checkinteger(L, 1)));
  return 1;
}

static int LuaEncodeLatin1(lua_State *L) {
  int f;
  char *p;
  size_t n;
  p = luaL_checklstring(L, 1, &n);
  f = LuaCheckControlFlags(L, 2);
  p = EncodeLatin1(p, n, &n, f);
  lua_pushlstring(L, p, n);
  free(p);
  return 1;
}

static int LuaSlurp(lua_State *L) {
  char *p;
  size_t n;
  p = xslurp(luaL_checkstring(L, 1), &n);
  lua_pushlstring(L, p, n);
  free(p);
  return 1;
}

static int LuaIndentLines(lua_State *L) {
  void *p;
  size_t n, j;
  p = luaL_checklstring(L, 1, &n);
  j = luaL_optinteger(L, 2, 1);
  if (!(0 <= j && j <= 65535)) {
    luaL_argerror(L, 2, "not in range 0..65535");
    unreachable;
  }
  p = IndentLines(p, n, &n, j);
  lua_pushlstring(L, p, n);
  free(p);
  return 1;
}

static int LuaGetMonospaceWidth(lua_State *L) {
  int w;
  if (lua_isinteger(L, 1)) {
    w = wcwidth(lua_tointeger(L, 1));
  } else if (lua_isstring(L, 1)) {
    w = strwidth(luaL_checkstring(L, 1), luaL_optinteger(L, 2, 0) & 7);
  } else {
    luaL_argerror(L, 1, "not integer or string");
    unreachable;
  }
  lua_pushinteger(L, w);
  return 1;
}

static int LuaPopcnt(lua_State *L) {
  lua_pushinteger(L, popcnt(luaL_checkinteger(L, 1)));
  return 1;
}

static int LuaBsr(lua_State *L) {
  long x;
  if ((x = luaL_checkinteger(L, 1))) {
    lua_pushinteger(L, bsr(x));
    return 1;
  } else {
    luaL_argerror(L, 1, "zero");
    unreachable;
  }
}

static int LuaBsf(lua_State *L) {
  long x;
  if ((x = luaL_checkinteger(L, 1))) {
    lua_pushinteger(L, bsf(x));
    return 1;
  } else {
    luaL_argerror(L, 1, "zero");
    unreachable;
  }
}

static int LuaHash(lua_State *L, uint32_t H(uint32_t, const void *, size_t)) {
  long i;
  size_t n;
  const char *p;
  i = luaL_checkinteger(L, 1);
  p = luaL_checklstring(L, 2, &n);
  lua_pushinteger(L, H(i, p, n));
  return 1;
}

static int LuaCrc32(lua_State *L) {
  return LuaHash(L, crc32_z);
}

static int LuaCrc32c(lua_State *L) {
  return LuaHash(L, crc32c);
}

static noinline int LuaProgramInt(lua_State *L, void P(long)) {
  P(luaL_checkinteger(L, 1));
  return 0;
}

static int LuaProgramPort(lua_State *L) {
  return LuaProgramInt(L, ProgramPort);
}

static int LuaProgramCache(lua_State *L) {
  return LuaProgramInt(L, ProgramCache);
}

static int LuaProgramTimeout(lua_State *L) {
  return LuaProgramInt(L, ProgramTimeout);
}

static int LuaProgramUid(lua_State *L) {
  return LuaProgramInt(L, ProgramUid);
}

static int LuaProgramGid(lua_State *L) {
  return LuaProgramInt(L, ProgramGid);
}

static noinline int LuaProgramString(lua_State *L, void P(const char *)) {
  P(luaL_checkstring(L, 1));
  return 0;
}

static int LuaProgramAddr(lua_State *L) {
  return LuaProgramString(L, ProgramAddr);
}

static int LuaProgramBrand(lua_State *L) {
  return LuaProgramString(L, ProgramBrand);
}

static int LuaProgramDirectory(lua_State *L) {
  return LuaProgramString(L, ProgramDirectory);
}

static int LuaProgramLogPath(lua_State *L) {
  return LuaProgramString(L, ProgramLogPath);
}

static int LuaProgramPidPath(lua_State *L) {
  return LuaProgramString(L, ProgramPidPath);
}

static int LuaProgramPrivateKey(lua_State *L) {
#ifndef UNSECURE
  size_t n;
  const char *p;
  p = luaL_checklstring(L, 1, &n);
  ProgramPrivateKey(p, n);
#endif
  return 0;
}

static int LuaProgramCertificate(lua_State *L) {
#ifndef UNSECURE
  size_t n;
  const char *p;
  p = luaL_checklstring(L, 1, &n);
  ProgramCertificate(p, n);
#endif
  return 0;
}

static int LuaProgramHeader(lua_State *L) {
  ProgramHeader(
      gc(xasprintf("%s: %s", luaL_checkstring(L, 1), luaL_checkstring(L, 2))));
  return 0;
}

static int LuaProgramRedirect(lua_State *L) {
  int code;
  const char *from, *to;
  code = luaL_checkinteger(L, 1);
  from = luaL_checkstring(L, 2);
  to = luaL_checkstring(L, 3);
  ProgramRedirect(code, strdup(from), strlen(from), strdup(to), strlen(to));
  return 0;
}

static noinline int LuaProgramBool(lua_State *L, bool *b) {
  *b = lua_toboolean(L, 1);
  return 0;
}

static int LuaProgramSslClientVerify(lua_State *L) {
  return LuaProgramBool(L, &sslclientverify);
}

static int LuaProgramSslFetchVerify(lua_State *L) {
  return LuaProgramBool(L, &sslfetchverify);
}

static int LuaProgramLogMessages(lua_State *L) {
  return LuaProgramBool(L, &logmessages);
}

static int LuaProgramLogBodies(lua_State *L) {
  return LuaProgramBool(L, &logbodies);
}

static int LuaGetLogLevel(lua_State *L) {
  lua_pushinteger(L, __log_level);
  return 1;
}

static int LuaSetLogLevel(lua_State *L) {
  __log_level = luaL_checkinteger(L, 1);
  return 0;
}

static int LuaHidePath(lua_State *L) {
  size_t pathlen;
  const char *path;
  path = luaL_checklstring(L, 1, &pathlen);
  AddString(&hidepaths, memcpy(malloc(pathlen), path, pathlen), pathlen);
  return 0;
}

static int LuaLog(lua_State *L) {
  int level;
  char *module;
  lua_Debug ar;
  const char *msg;
  level = luaL_checkinteger(L, 1);
  if (LOGGABLE(level)) {
    msg = luaL_checkstring(L, 2);
    lua_getstack(L, 1, &ar);
    lua_getinfo(L, "nSl", &ar);
    if (!strcmp(ar.name, "main")) {
      module = strndup(effectivepath.p, effectivepath.n);
      flogf(level, module, ar.currentline, NULL, "%s", msg);
      free(module);
    } else {
      flogf(level, ar.name, ar.currentline, NULL, "%s", msg);
    }
  }
  return 0;
}

static int LuaIsHiddenPath(lua_State *L) {
  size_t n;
  const char *s;
  s = luaL_checklstring(L, 1, &n);
  lua_pushboolean(L, IsHiddenPath(s, n));
  return 1;
}

static int LuaGetZipPaths(lua_State *L) {
  char *path;
  uint8_t *zcf;
  size_t i, n, pathlen;
  lua_newtable(L);
  i = 0;
  n = GetZipCdirRecords(zcdir);
  for (zcf = zbase + GetZipCdirOffset(zcdir); n--;
       zcf += ZIP_CFILE_HDRSIZE(zcf)) {
    CHECK_EQ(kZipCfileHdrMagic, ZIP_CFILE_MAGIC(zcf));
    path = GetAssetPath(zcf, &pathlen);
    lua_pushlstring(L, path, pathlen);
    lua_seti(L, -2, ++i);
    free(path);
  }
  return 1;
}

static int LuaGetAssetMode(lua_State *L) {
  size_t pathlen;
  struct Asset *a;
  const char *path;
  path = LuaCheckPath(L, 1, &pathlen);
  if ((a = GetAsset(path, pathlen))) {
    lua_pushinteger(L, GetMode(a));
  } else {
    lua_pushnil(L);
  }
  return 1;
}

static int LuaGetLastModifiedTime(lua_State *L) {
  size_t pathlen;
  struct Asset *a;
  const char *path;
  path = LuaCheckPath(L, 1, &pathlen);
  if ((a = GetAsset(path, pathlen))) {
    if (a->file) {
      lua_pushinteger(L, a->file->st.st_mtim.tv_sec);
    } else {
      lua_pushinteger(L, GetZipCfileLastModified(zbase + a->cf));
    }
  } else {
    lua_pushnil(L);
  }
  return 1;
}

static int LuaGetAssetSize(lua_State *L) {
  size_t pathlen;
  struct Asset *a;
  const char *path;
  path = LuaCheckPath(L, 1, &pathlen);
  if ((a = GetAsset(path, pathlen))) {
    if (a->file) {
      lua_pushinteger(L, a->file->st.st_size);
    } else {
      lua_pushinteger(L, GetZipLfileUncompressedSize(zbase + a->lf));
    }
  } else {
    lua_pushnil(L);
  }
  return 1;
}

static int LuaIsCompressed(lua_State *L) {
  size_t pathlen;
  struct Asset *a;
  const char *path;
  path = LuaCheckPath(L, 1, &pathlen);
  if ((a = GetAsset(path, pathlen))) {
    lua_pushboolean(L, IsCompressed(a));
  } else {
    lua_pushnil(L);
  }
  return 1;
}

static int LuaIsDaemon(lua_State *L) {
  lua_pushboolean(L, daemonize);
  return 1;
}

static int LuaGetComment(lua_State *L) {
  struct Asset *a;
  const char *path;
  size_t pathlen, m;
  path = LuaCheckPath(L, 1, &pathlen);
  if ((a = GetAssetZip(path, pathlen)) &&
      (m = strnlen(ZIP_CFILE_COMMENT(zbase + a->cf),
                   ZIP_CFILE_COMMENTSIZE(zbase + a->cf)))) {
    lua_pushlstring(L, ZIP_CFILE_COMMENT(zbase + a->cf), m);
  } else {
    lua_pushnil(L);
  }
  return 1;
}

static void LuaSetIntField(lua_State *L, const char *k, lua_Integer v) {
  lua_pushinteger(L, v);
  lua_setfield(L, -2, k);
}

static int LuaLaunchBrowser(lua_State *L) {
  launchbrowser = strdup(luaL_optstring(L, 1, "/"));
  return 0;
}

static regex_t *LuaReCompileImpl(lua_State *L, const char *p, int f) {
  int e;
  regex_t *r;
  r = xmalloc(sizeof(regex_t));
  f &= REG_EXTENDED | REG_ICASE | REG_NEWLINE | REG_NOSUB;
  f ^= REG_EXTENDED;
  if ((e = regcomp(r, p, f)) != REG_OK) {
    free(r);
    luaL_error(L, "regcomp(%s) → REG_%s", p,
               kRegCode[MAX(0, MIN(ARRAYLEN(kRegCode) - 1, e))]);
    unreachable;
  }
  return r;
}

static int LuaReSearchImpl(lua_State *L, regex_t *r, const char *s, int f) {
  int i, n;
  regmatch_t *m;
  n = r->re_nsub + 1;
  m = xcalloc(n, sizeof(regmatch_t));
  if (regexec(r, s, n, m, f >> 8) == REG_OK) {
    for (i = 0; i < n; ++i) {
      lua_pushlstring(L, s + m[i].rm_so, m[i].rm_eo - m[i].rm_so);
    }
  } else {
    n = 0;
  }
  free(m);
  return n;
}

static int LuaReSearch(lua_State *L) {
  regex_t *r;
  int i, e, n, f;
  const char *p, *s;
  p = luaL_checkstring(L, 1);
  s = luaL_checkstring(L, 2);
  f = luaL_optinteger(L, 3, 0);
  if (f & ~(REG_EXTENDED | REG_ICASE | REG_NEWLINE | REG_NOSUB |
            REG_NOTBOL << 8 | REG_NOTEOL << 8)) {
    luaL_argerror(L, 3, "invalid flags");
    unreachable;
  }
  r = LuaReCompileImpl(L, p, f);
  n = LuaReSearchImpl(L, r, s, f);
  regfree(r);
  free(r);
  return n;
}

static int LuaReCompile(lua_State *L) {
  int f, e;
  const char *p;
  regex_t *r, **u;
  p = luaL_checkstring(L, 1);
  f = luaL_optinteger(L, 2, 0);
  if (f & ~(REG_EXTENDED | REG_ICASE | REG_NEWLINE | REG_NOSUB)) {
    luaL_argerror(L, 3, "invalid flags");
    unreachable;
  }
  r = LuaReCompileImpl(L, p, f);
  u = lua_newuserdata(L, sizeof(regex_t *));
  luaL_setmetatable(L, "regex_t*");
  *u = r;
  return 1;
}

static int LuaReRegexSearch(lua_State *L) {
  int f;
  regex_t **u;
  const char *s;
  u = luaL_checkudata(L, 1, "regex_t*");
  s = luaL_checkstring(L, 2);
  f = luaL_optinteger(L, 3, 0);
  if (!*u) {
    luaL_argerror(L, 1, "destroyed");
    unreachable;
  }
  if (f & ~(REG_NOTBOL << 8 | REG_NOTEOL << 8)) {
    luaL_argerror(L, 3, "invalid flags");
    unreachable;
  }
  return LuaReSearchImpl(L, *u, s, f);
}

static int LuaReRegexGc(lua_State *L) {
  regex_t **u;
  u = luaL_checkudata(L, 1, "regex_t*");
  if (*u) {
    regfree(*u);
    free(*u);
    *u = NULL;
  }
  return 0;
}

static const luaL_Reg kLuaRe[] = {
    {"compile", LuaReCompile},  //
    {"search", LuaReSearch},    //
    {NULL, NULL},               //
};

static const luaL_Reg kLuaReRegexMeth[] = {
    {"search", LuaReRegexSearch},  //
    {NULL, NULL},                  //
};

static const luaL_Reg kLuaReRegexMeta[] = {
    {"__gc", LuaReRegexGc},  //
    {NULL, NULL},            //
};

static void LuaReRegex(lua_State *L) {
  luaL_newmetatable(L, "regex_t*");
  luaL_setfuncs(L, kLuaReRegexMeta, 0);
  luaL_newlibtable(L, kLuaReRegexMeth);
  luaL_setfuncs(L, kLuaReRegexMeth, 0);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);
}

static int LuaRe(lua_State *L) {
  luaL_newlib(L, kLuaRe);
  LuaSetIntField(L, "BASIC", REG_EXTENDED);
  LuaSetIntField(L, "ICASE", REG_ICASE);
  LuaSetIntField(L, "NEWLINE", REG_NEWLINE);
  LuaSetIntField(L, "NOSUB", REG_NOSUB);
  LuaSetIntField(L, "NOTBOL", REG_NOTBOL << 8);
  LuaSetIntField(L, "NOTEOL", REG_NOTEOL << 8);
  LuaReRegex(L);
  return 1;
}

static bool LuaRun(const char *path) {
  size_t pathlen;
  struct Asset *a;
  const char *code;
  pathlen = strlen(path);
  if ((a = GetAsset(path, pathlen))) {
    if ((code = LoadAsset(a, NULL))) {
      effectivepath.p = path;
      effectivepath.n = pathlen;
      DEBUGF("LuaRun(%`'s)", path);
      if (luaL_dostring(L, code) != LUA_OK) {
        WARNF("%s %s", path, lua_tostring(L, -1));
      }
      free(code);
    }
  }
  return !!a;
}

static const luaL_Reg kLuaFuncs[] = {
    {"Bsf", LuaBsf},                                        //
    {"Bsr", LuaBsr},                                        //
    {"CategorizeIp", LuaCategorizeIp},                      //
    {"Crc32", LuaCrc32},                                    //
    {"Crc32c", LuaCrc32c},                                  //
    {"DecodeBase64", LuaDecodeBase64},                      //
    {"DecodeLatin1", LuaDecodeLatin1},                      //
    {"EncodeBase64", LuaEncodeBase64},                      //
    {"EncodeLatin1", LuaEncodeLatin1},                      //
    {"EncodeUrl", LuaEncodeUrl},                            //
    {"EscapeFragment", LuaEscapeFragment},                  //
    {"EscapeHost", LuaEscapeHost},                          //
    {"EscapeHtml", LuaEscapeHtml},                          //
    {"EscapeIp", LuaEscapeIp},                              //
    {"EscapeLiteral", LuaEscapeLiteral},                    //
    {"EscapeParam", LuaEscapeParam},                        //
    {"EscapePass", LuaEscapePass},                          //
    {"EscapePath", LuaEscapePath},                          //
    {"EscapeSegment", LuaEscapeSegment},                    //
    {"EscapeUser", LuaEscapeUser},                          //
    {"Fetch", LuaFetch},                                    //
    {"FormatHttpDateTime", LuaFormatHttpDateTime},          //
    {"FormatIp", LuaFormatIp},                              //
    {"GetAssetMode", LuaGetAssetMode},                      //
    {"GetAssetSize", LuaGetAssetSize},                      //
    {"GetClientAddr", LuaGetClientAddr},                    //
    {"GetComment", LuaGetComment},                          //
    {"GetDate", LuaGetDate},                                //
    {"GetEffectivePath", LuaGetEffectivePath},              //
    {"GetFragment", LuaGetFragment},                        //
    {"GetHeader", LuaGetHeader},                            //
    {"GetHeaders", LuaGetHeaders},                          //
    {"GetHost", LuaGetHost},                                //
    {"GetHttpReason", LuaGetHttpReason},                    //
    {"GetLastModifiedTime", LuaGetLastModifiedTime},        //
    {"GetLogLevel", LuaGetLogLevel},                        //
    {"GetMethod", LuaGetMethod},                            //
    {"GetMonospaceWidth", LuaGetMonospaceWidth},            //
    {"GetParam", LuaGetParam},                              //
    {"GetParams", LuaGetParams},                            //
    {"GetPass", LuaGetPass},                                //
    {"GetPath", LuaGetPath},                                //
    {"GetPayload", LuaGetPayload},                          //
    {"GetPort", LuaGetPort},                                //
    {"GetRemoteAddr", LuaGetRemoteAddr},                    //
    {"GetScheme", LuaGetScheme},                            //
    {"GetServerAddr", LuaGetServerAddr},                    //
    {"GetUrl", LuaGetUrl},                                  //
    {"GetUser", LuaGetUser},                                //
    {"GetVersion", LuaGetVersion},                          //
    {"GetZipPaths", LuaGetZipPaths},                        //
    {"HasControlCodes", LuaHasControlCodes},                //
    {"HasParam", LuaHasParam},                              //
    {"HidePath", LuaHidePath},                              //
    {"IndentLines", LuaIndentLines},                        //
    {"IsAcceptableHost", LuaIsAcceptableHost},              //
    {"IsAcceptablePath", LuaIsAcceptablePath},              //
    {"IsAcceptablePort", LuaIsAcceptablePort},              //
    {"IsCompressed", LuaIsCompressed},                      //
    {"IsDaemon", LuaIsDaemon},                              //
    {"IsHiddenPath", LuaIsHiddenPath},                      //
    {"IsLoopbackIp", LuaIsLoopbackIp},                      //
    {"IsPrivateIp", LuaIsPrivateIp},                        //
    {"IsPublicIp", LuaIsPublicIp},                          //
    {"IsReasonablePath", LuaIsReasonablePath},              //
    {"IsValidHttpToken", LuaIsValidHttpToken},              //
    {"LaunchBrowser", LuaLaunchBrowser},                    //
    {"LoadAsset", LuaLoadAsset},                            //
    {"Log", LuaLog},                                        //
    {"Md5", LuaMd5},                                        //
    {"ParseHost", LuaParseHost},                            //
    {"ParseHttpDateTime", LuaParseHttpDateTime},            //
    {"ParseIp", LuaParseIp},                                //
    {"ParseParams", LuaParseParams},                        //
    {"ParseUrl", LuaParseUrl},                              //
    {"Popcnt", LuaPopcnt},                                  //
    {"ProgramAddr", LuaProgramAddr},                        //
    {"ProgramBrand", LuaProgramBrand},                      //
    {"ProgramCache", LuaProgramCache},                      //
    {"ProgramCertificate", LuaProgramCertificate},          //
    {"ProgramDirectory", LuaProgramDirectory},              //
    {"ProgramGid", LuaProgramGid},                          //
    {"ProgramHeader", LuaProgramHeader},                    //
    {"ProgramLogBodies", LuaProgramLogBodies},              //
    {"ProgramLogMessages", LuaProgramLogMessages},          //
    {"ProgramLogPath", LuaProgramLogPath},                  //
    {"ProgramPidPath", LuaProgramPidPath},                  //
    {"ProgramPort", LuaProgramPort},                        //
    {"ProgramPrivateKey", LuaProgramPrivateKey},            //
    {"ProgramRedirect", LuaProgramRedirect},                //
    {"ProgramSslClientVerify", LuaProgramSslClientVerify},  //
    {"ProgramSslFetchVerify", LuaProgramSslFetchVerify},    //
    {"ProgramTimeout", LuaProgramTimeout},                  //
    {"ProgramUid", LuaProgramUid},                          //
    {"Route", LuaRoute},                                    //
    {"RouteHost", LuaRouteHost},                            //
    {"RoutePath", LuaRoutePath},                            //
    {"ServeAsset", LuaServeAsset},                          //
    {"ServeError", LuaServeError},                          //
    {"ServeIndex", LuaServeIndex},                          //
    {"ServeListing", LuaServeListing},                      //
    {"ServeStatusz", LuaServeStatusz},                      //
    {"SetHeader", LuaSetHeader},                            //
    {"SetLogLevel", LuaSetLogLevel},                        //
    {"SetStatus", LuaSetStatus},                            //
    {"Sha1", LuaSha1},                                      //
    {"Sha224", LuaSha224},                                  //
    {"Sha256", LuaSha256},                                  //
    {"Sha384", LuaSha384},                                  //
    {"Sha512", LuaSha512},                                  //
    {"Slurp", LuaSlurp},                                    //
    {"StoreAsset", LuaStoreAsset},                          //
    {"Underlong", LuaUnderlong},                            //
    {"VisualizeControlCodes", LuaVisualizeControlCodes},    //
    {"Write", LuaWrite},                                    //
    {"bsf", LuaBsf},                                        //
    {"bsr", LuaBsr},                                        //
    {"crc32", LuaCrc32},                                    //
    {"crc32c", LuaCrc32c},                                  //
    {"popcnt", LuaPopcnt},                                  //
};

extern int luaopen_lsqlite3(lua_State *);

static const luaL_Reg kLuaLibs[] = {
    {"re", LuaRe},                   //
    {"lsqlite3", luaopen_lsqlite3},  //
};

static void LuaSetArgv(lua_State *L) {
  size_t i;
  lua_newtable(L);
  for (i = optind; i < __argc; ++i) {
    lua_pushstring(L, __argv[i]);
    lua_seti(L, -2, i - optind + 1);
  }
  lua_setglobal(L, "argv");
}

static void LuaSetConstant(lua_State *L, const char *s, long x) {
  lua_pushinteger(L, x);
  lua_setglobal(L, s);
}

static void LuaInit(void) {
#ifndef STATIC
  size_t i;
  L = luaL_newstate();
  luaL_openlibs(L);
  for (i = 0; i < ARRAYLEN(kLuaLibs); ++i) {
    luaL_requiref(L, kLuaLibs[i].name, kLuaLibs[i].func, 1);
    lua_pop(L, 1);
  }
  for (i = 0; i < ARRAYLEN(kLuaFuncs); ++i) {
    lua_pushcfunction(L, kLuaFuncs[i].func);
    lua_setglobal(L, kLuaFuncs[i].name);
  }
  LuaSetArgv(L);
  LuaSetConstant(L, "kLogDebug", kLogDebug);
  LuaSetConstant(L, "kLogVerbose", kLogVerbose);
  LuaSetConstant(L, "kLogInfo", kLogInfo);
  LuaSetConstant(L, "kLogWarn", kLogWarn);
  LuaSetConstant(L, "kLogError", kLogError);
  LuaSetConstant(L, "kLogFatal", kLogFatal);
  if (LuaRun("/.init.lua")) {
    hasonhttprequest = IsHookDefined("OnHttpRequest");
    hasonclientconnection = IsHookDefined("OnClientConnection");
    hasonprocesscreate = IsHookDefined("OnProcessCreate");
    hasonprocessdestroy = IsHookDefined("OnProcessDestroy");
    hasonworkerstart = IsHookDefined("OnWorkerStart");
    hasonworkerstop = IsHookDefined("OnWorkerStop");
  } else {
    DEBUGF("no /.init.lua defined");
  }
#endif
}

static void LuaReload(void) {
#ifndef STATIC
  if (!LuaRun("/.reload.lua")) {
    DEBUGF("no /.reload.lua defined");
  }
#endif
}

static void LuaDestroy(void) {
#ifndef STATIC
  lua_close(L);
#endif
}

static const char *DescribeClose(void) {
  if (killed) return "killed";
  if (meltdown) return "meltdown";
  if (terminated) return "terminated";
  if (connectionclose) return "connectionclose";
  return "destroyed";
}

static void LogClose(const char *reason) {
  if (amtread || meltdown || killed) {
    LockInc(&shared->c.fumbles);
    LOGF("%s %s with %,ld unprocessed and %,d handled (%,d workers)",
         DescribeClient(), reason, amtread, messageshandled, shared->workers);
  } else {
    DEBUGF("%s %s with %,d requests handled", DescribeClient(), reason,
           messageshandled);
  }
}

static ssize_t SendString(const char *s) {
  size_t n;
  ssize_t rc;
  struct iovec iov;
  n = strlen(s);
  iov.iov_base = s;
  iov.iov_len = n;
  if (logmessages) {
    LogMessage("sending", s, n);
  }
  for (;;) {
    if ((rc = writer(client, &iov, 1)) != -1 || errno != EINTR) {
      return rc;
    }
  }
}

static ssize_t SendContinue(void) {
  return SendString("\
HTTP/1.1 100 Continue\r\n\
\r\n");
}

static ssize_t SendTimeout(void) {
  return SendString("\
HTTP/1.1 408 Request Timeout\r\n\
Connection: close\r\n\
Content-Length: 0\r\n\
\r\n");
}

static ssize_t SendServiceUnavailable(void) {
  return SendString("\
HTTP/1.1 503 Service Unavailable\r\n\
Connection: close\r\n\
Content-Length: 0\r\n\
\r\n");
}

static void EnterMeltdownMode(void) {
  if (shared->lastmeltdown && nowl() - shared->lastmeltdown < 1) return;
  WARNF("redbean is melting down (%,d workers)", shared->workers);
  LOGIFNEG1(kill(0, SIGUSR2));
  shared->lastmeltdown = nowl();
  ++shared->c.meltdowns;
}

static char *HandlePayloadDisconnect(void) {
  LockInc(&shared->c.payloaddisconnects);
  LogClose("payload disconnect");
  return ServeFailure(400, "Bad Request"); /* XXX */
}

static char *HandlePayloadDrop(void) {
  LockInc(&shared->c.dropped);
  LogClose(DescribeClose());
  return ServeFailure(503, "Service Unavailable");
}

static char *HandleBadContentLength(void) {
  LockInc(&shared->c.badlengths);
  return ServeFailure(400, "Bad Content Length");
}

static char *HandleLengthRequired(void) {
  LockInc(&shared->c.missinglengths);
  return ServeFailure(411, "Length Required");
}

static char *HandleVersionNotSupported(void) {
  LockInc(&shared->c.http12);
  return ServeFailure(505, "HTTP Version Not Supported");
}

static char *HandleConnectRefused(void) {
  LockInc(&shared->c.connectsrefused);
  return ServeFailure(501, "Not Implemented");
}

static char *HandleExpectFailed(void) {
  LockInc(&shared->c.expectsrefused);
  return ServeFailure(417, "Expectation Failed");
}

static char *HandleHugePayload(void) {
  LockInc(&shared->c.hugepayloads);
  return ServeFailure(413, "Payload Too Large");
}

static char *HandlePayloadSlowloris(void) {
  LockInc(&shared->c.slowloris);
  LogClose("payload slowloris");
  return ServeFailure(408, "Request Timeout");
}

static char *HandleTransferRefused(void) {
  LockInc(&shared->c.transfersrefused);
  return ServeFailure(501, "Not Implemented");
}

static char *HandleMapFailed(struct Asset *a, int fd) {
  LockInc(&shared->c.mapfails);
  WARNF("mmap(%`'s) failed %s", a->file->path, strerror(errno));
  close(fd);
  return ServeError(500, "Internal Server Error");
}

static char *HandleOpenFail(struct Asset *a) {
  LockInc(&shared->c.openfails);
  WARNF("open(%`'s) failed %s", a->file->path, strerror(errno));
  if (errno == ENFILE) {
    LockInc(&shared->c.enfiles);
    return ServeError(503, "Service Unavailable");
  } else if (errno == EMFILE) {
    LockInc(&shared->c.emfiles);
    return ServeError(503, "Service Unavailable");
  } else {
    return ServeError(500, "Internal Server Error");
  }
}

static char *HandlePayloadReadError(void) {
  if (errno == ECONNRESET) {
    LockInc(&shared->c.readresets);
    LogClose("payload reset");
    return ServeFailure(400, "Bad Request"); /* XXX */
  } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
    LockInc(&shared->c.readtimeouts);
    LogClose("payload read timeout");
    return ServeFailure(408, "Request Timeout");
  } else {
    LockInc(&shared->c.readerrors);
    LOGF("%s payload read error %s", DescribeClient(), strerror(errno));
    return ServeFailure(500, "Internal Server Error");
  }
}

static void HandleForkFailure(void) {
  FATALF("too many processes %s", strerror(errno));
  LockInc(&shared->c.forkerrors);
  LockInc(&shared->c.dropped);
  EnterMeltdownMode();
  SendServiceUnavailable();
  close(client);
}

static void HandleFrag(size_t got) {
  LockInc(&shared->c.frags);
  if (++frags == 32) {
    SendTimeout();
    LogClose("slowloris");
    LockInc(&shared->c.slowloris);
    return;
  } else {
    DEBUGF("%s fragged msg added %,ld bytes to %,ld byte buffer",
           DescribeClient(), amtread, got);
  }
}

static void HandleReload(void) {
  LockInc(&shared->c.reloads);
  Reindex();
  LuaReload();
}

static void HandleHeartbeat(void) {
  size_t i;
  sigset_t mask;
  if (nowl() - lastrefresh > 60 * 60) RefreshTime();
  UpdateCurrentDate(nowl());
  Reindex();
  getrusage(RUSAGE_SELF, &shared->server);
#ifndef STATIC
  LuaRun("/.heartbeat.lua");
  CollectGarbage();
#endif
  for (i = 0; i < servers.n; ++i) {
    if (polls[i].fd < 0) {
      polls[i].fd = -polls[i].fd;
    }
  }
}

static char *OpenAsset(struct Asset *a) {
  int fd;
  void *data;
  size_t size;
  if (a->file->st.st_size) {
    size = a->file->st.st_size;
    if (msg.method == kHttpHead) {
      content = 0;
      contentlength = size;
    } else {
    OpenAgain:
      if ((fd = open(a->file->path.s, O_RDONLY)) != -1) {
        data = mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (data != MAP_FAILED) {
          LockInc(&shared->c.maps);
          UnmapLater(fd, data, size);
          content = data;
          contentlength = size;
        } else {
          return HandleMapFailed(a, fd);
        }
      } else if (errno == EINTR) {
        goto OpenAgain;
      } else {
        return HandleOpenFail(a);
      }
    }
  } else {
    content = "";
    contentlength = 0;
  }
  return 0;
}

static char *ServeServerOptions(void) {
  char *p;
  LockInc(&shared->c.serveroptions);
  p = SetStatus(200, "OK");
#ifdef STATIC
  p = stpcpy(p, "Allow: GET, HEAD, OPTIONS\r\n");
#else
  p = stpcpy(p, "Accept: */*\r\n"
                "Accept-Charset: utf-8,ISO-8859-1;q=0.7,*;q=0.5\r\n"
                "Allow: GET, HEAD, POST, PUT, DELETE, OPTIONS\r\n");
#endif
  return p;
}

static void SendContinueIfNeeded(void) {
  if (msg.version >= 11 && HeaderEqualCase(kHttpExpect, "100-continue")) {
    LockInc(&shared->c.continues);
    SendContinue();
  }
}

static char *ReadMore(void) {
  size_t got;
  ssize_t rc;
  LockInc(&shared->c.frags);
  if (++frags == 64) return HandlePayloadSlowloris();
  if ((rc = reader(client, inbuf.p + amtread, inbuf.n - amtread)) != -1) {
    if (!(got = rc)) return HandlePayloadDisconnect();
    amtread += got;
  } else if (errno == EINTR) {
    LockInc(&shared->c.readinterrupts);
    if (killed || ((meltdown || terminated) && nowl() - startread > 1)) {
      return HandlePayloadDrop();
    }
  } else {
    return HandlePayloadReadError();
  }
  return NULL;
}

static char *SynchronizeLength(void) {
  char *p;
  if (hdrsize + payloadlength > amtread) {
    if (hdrsize + payloadlength > inbuf.n) return HandleHugePayload();
    SendContinueIfNeeded();
    while (amtread < hdrsize + payloadlength) {
      if ((p = ReadMore())) return p;
    }
  }
  msgsize = hdrsize + payloadlength;
  return NULL;
}

static char *SynchronizeChunked(void) {
  char *p;
  ssize_t transferlength;
  struct HttpUnchunker u = {0};
  SendContinueIfNeeded();
  while (!(transferlength = Unchunk(&u, inbuf.p + hdrsize, amtread - hdrsize,
                                    &payloadlength))) {
    if ((p = ReadMore())) return p;
  }
  if (transferlength == -1) return HandleHugePayload();
  msgsize = hdrsize + transferlength;
  return NULL;
}

char *SynchronizeStream(void) {
  int64_t cl;
  if (HasHeader(kHttpTransferEncoding) &&
      !HeaderEqualCase(kHttpTransferEncoding, "identity")) {
    if (HeaderEqualCase(kHttpTransferEncoding, "chunked")) {
      return SynchronizeChunked();
    } else {
      return HandleTransferRefused();
    }
  } else if (HasHeader(kHttpContentLength)) {
    if ((cl = ParseContentLength(HeaderData(kHttpContentLength),
                                 HeaderLength(kHttpContentLength))) != -1) {
      payloadlength = cl;
      return SynchronizeLength();
    } else {
      return HandleBadContentLength();
    }
  } else if (msg.method == kHttpPost || msg.method == kHttpPut) {
    return HandleLengthRequired();
  } else {
    msgsize = hdrsize;
    payloadlength = 0;
    return NULL;
  }
}

static void ParseRequestParameters(void) {
  uint32_t ip;
  FreeLater(ParseRequestUri(inbuf.p + msg.uri.a, msg.uri.b - msg.uri.a, &url));
  if (!url.host.p) {
    GetRemoteAddr(&ip, 0);
    if (HasHeader(kHttpXForwardedHost) &&
        (IsPrivateIp(ip) || IsLoopbackIp(ip))) {
      FreeLater(ParseHost(HeaderData(kHttpXForwardedHost),
                          HeaderLength(kHttpXForwardedHost), &url));
    } else if (HasHeader(kHttpHost)) {
      FreeLater(
          ParseHost(HeaderData(kHttpHost), HeaderLength(kHttpHost), &url));
    } else {
      FreeLater(ParseHost(DescribeServer(), -1, &url));
    }
  } else if (!url.path.n) {
    url.path.p = "/";
    url.path.n = 1;
  }
  if (!url.scheme.n) {
    if (usessl) {
      url.scheme.p = "https";
      url.scheme.n = 5;
    } else {
      url.scheme.p = "http";
      url.scheme.n = 4;
    }
  }
}

static bool HasAtMostThisElement(int h, const char *s) {
  size_t i, n;
  struct HttpHeader *x;
  if (HasHeader(h)) {
    n = strlen(s);
    if (!SlicesEqualCase(s, n, HeaderData(h), HeaderLength(h))) {
      return false;
    }
    for (i = 0; i < msg.xheaders.n; ++i) {
      x = msg.xheaders.p + i;
      if (GetHttpHeader(inbuf.p + x->k.a, x->k.b - x->k.a) == h &&
          !SlicesEqualCase(inbuf.p + x->v.a, x->v.b - x->v.a, s, n)) {
        return false;
      }
    }
  }
  return true;
}

static char *HandleRequest(void) {
  char *p;
  if (msg.version == 11) {
    LockInc(&shared->c.http11);
  } else if (msg.version < 10) {
    LockInc(&shared->c.http09);
  } else if (msg.version == 10) {
    LockInc(&shared->c.http10);
  } else {
    return HandleVersionNotSupported();
  }
  if ((p = SynchronizeStream())) return p;
  if (logbodies) LogBody("received", inbuf.p + hdrsize, payloadlength);
  if (msg.version < 11 || HeaderEqualCase(kHttpConnection, "close")) {
    connectionclose = true;
  }
  if (msg.method == kHttpOptions &&
      SlicesEqual(inbuf.p + msg.uri.a, msg.uri.b - msg.uri.a, "*", 1)) {
    return ServeServerOptions();
  }
  if (msg.method == kHttpConnect) {
    return HandleConnectRefused();
  }
  if (!HasAtMostThisElement(kHttpExpect, "100-continue")) {
    return HandleExpectFailed();
  }
  ParseRequestParameters();
  if (!url.host.n || !url.path.n || url.path.p[0] != '/' ||
      !IsAcceptablePath(url.path.p, url.path.n) ||
      !IsAcceptableHost(url.host.p, url.host.n) ||
      !IsAcceptablePort(url.port.p, url.port.n)) {
    free(url.params.p);
    LockInc(&shared->c.urisrefused);
    return ServeFailure(400, "Bad URI");
  }
  LOGF("RECEIVED %s HTTP%02d %.*s %s %`'.*s %`'.*s", DescribeClient(),
       msg.version, msg.xmethod.b - msg.xmethod.a, inbuf.p + msg.xmethod.a,
       FreeLater(EncodeUrl(&url, 0)), HeaderLength(kHttpReferer),
       HeaderData(kHttpReferer), HeaderLength(kHttpUserAgent),
       HeaderData(kHttpUserAgent));
  if (HasHeader(kHttpContentType) &&
      IsMimeType(HeaderData(kHttpContentType), HeaderLength(kHttpContentType),
                 "application/x-www-form-urlencoded")) {
    FreeLater(ParseParams(inbuf.p + hdrsize, payloadlength, &url.params));
  }
  FreeLater(url.params.p);
#ifndef STATIC
  if (hasonhttprequest) return LuaOnHttpRequest();
#endif
  return Route(url.host.p, url.host.n, url.path.p, url.path.n);
}

static char *Route(const char *host, size_t hostlen, const char *path,
                   size_t pathlen) {
  char *p;
  if (logmessages) LogMessage("received", inbuf.p, hdrsize);
  if (hostlen && (p = RouteHost(host, hostlen, path, pathlen))) {
    return p;
  }
  if (SlicesEqual(path, pathlen, "/", 1)) {
    if ((p = ServeIndex("/", 1))) return p;
    return ServeListing();
  } else if ((p = RoutePath(path, pathlen))) {
    return p;
  } else if (SlicesEqual(path, pathlen, "/statusz", 8)) {
    return ServeStatusz();
  } else {
    LockInc(&shared->c.notfounds);
    return ServeError(404, "Not Found");
  }
}

static char *RoutePath(const char *path, size_t pathlen) {
  int m;
  long r;
  char *p;
  struct Asset *a;
  DEBUGF("RoutePath(%`'.*s)", pathlen, path);
  if ((a = GetAsset(path, pathlen))) {
    if ((m = GetMode(a)) & 0004) {
      if (!S_ISDIR(m)) {
        return HandleAsset(a, path, pathlen);
      } else {
        return HandleFolder(path, pathlen);
      }
    } else {
      LockInc(&shared->c.forbiddens);
      LOGF("asset %`'.*s %#o isn't readable", pathlen, path, m);
      return ServeError(403, "Forbidden");
    }
  } else if ((r = FindRedirect(path, pathlen)) != -1) {
    return HandleRedirect(redirects.p + r);
  } else {
    return NULL;
  }
}

static char *RouteHost(const char *host, size_t hostlen, const char *path,
                       size_t pathlen) {
  size_t hn, hm;
  char *hp, *p, b[96];
  if (hostlen) {
    hn = 1 + hostlen + url.path.n;
    hm = 3 + 1 + hn;
    hp = hm <= sizeof(b) ? b : FreeLater(xmalloc(hm));
    hp[0] = '/';
    mempcpy(mempcpy(hp + 1, host, hostlen), path, pathlen);
    if ((p = RoutePath(hp, hn))) return p;
    if (!isdigit(host[0])) {
      if (hostlen > 4 &&
          READ32LE(host) == ('w' | 'w' << 8 | 'w' << 16 | '.' << 24)) {
        mempcpy(mempcpy(hp + 1, host + 4, hostlen - 4), path, pathlen);
        if ((p = RoutePath(hp, hn - 4))) return p;
      } else {
        mempcpy(mempcpy(mempcpy(hp + 1, "www.", 4), host, hostlen), path,
                pathlen);
        if ((p = RoutePath(hp, hn + 4))) return p;
      }
    }
  }
  return NULL;
}

static inline bool IsLua(struct Asset *a) {
  size_t n;
  const char *p;
  if (a->file && a->file->path.n >= 4 &&
      READ32LE(a->file->path.s + a->file->path.n - 4) ==
          ('.' | 'l' << 8 | 'u' << 16 | 'a' << 24)) {
    return true;
  }
  p = ZIP_CFILE_NAME(zbase + a->cf);
  n = ZIP_CFILE_NAMESIZE(zbase + a->cf);
  return n > 4 &&
         READ32LE(p + n - 4) == ('.' | 'l' << 8 | 'u' << 16 | 'a' << 24);
}

static char *HandleAsset(struct Asset *a, const char *path, size_t pathlen) {
#ifndef STATIC
  if (IsLua(a)) return ServeLua(a, path, pathlen);
#endif
  if (msg.method == kHttpGet || msg.method == kHttpHead) {
    LockInc(&shared->c.staticrequests);
    return stpcpy(ServeAsset(a, path, pathlen),
                  "X-Content-Type-Options: nosniff\r\n");
  } else {
    return BadMethod();
  }
}

static const char *GetContentType(struct Asset *a, const char *path, size_t n) {
  const char *r;
  if (a->file && (r = FindContentType(a->file->path.s, a->file->path.n))) {
    return r;
  }
  return firstnonnull(
      FindContentType(path, n),
      firstnonnull(FindContentType(ZIP_CFILE_NAME(zbase + a->cf),
                                   ZIP_CFILE_NAMESIZE(zbase + a->cf)),
                   a->istext ? "text/plain" : "application/octet-stream"));
}

static bool IsNotModified(struct Asset *a) {
  if (msg.version < 10) return false;
  if (!HasHeader(kHttpIfModifiedSince)) return false;
  return a->lastmodified >=
         ParseHttpDateTime(HeaderData(kHttpIfModifiedSince),
                           HeaderLength(kHttpIfModifiedSince));
}

static char *ServeAsset(struct Asset *a, const char *path, size_t pathlen) {
  char *p;
  uint32_t crc;
  const char *ct;
  ct = GetContentType(a, path, pathlen);
  if (IsNotModified(a)) {
    LockInc(&shared->c.notmodifieds);
    p = SetStatus(304, "Not Modified");
  } else {
    if (!a->file) {
      content = (char *)ZIP_LFILE_CONTENT(zbase + a->lf);
      contentlength = GetZipCfileCompressedSize(zbase + a->cf);
    } else if ((p = OpenAsset(a))) {
      return p;
    }
    if (IsCompressed(a)) {
      if (ClientAcceptsGzip()) {
        p = ServeAssetPrecompressed(a);
      } else {
        p = ServeAssetDecompressed(a);
      }
    } else if (msg.version >= 11 && HasHeader(kHttpRange)) {
      p = ServeAssetRange(a);
    } else if (!a->file) {
      LockInc(&shared->c.identityresponses);
      DEBUGF("ServeAssetZipIdentity(%`'s)", ct);
      if (Verify(content, contentlength, ZIP_LFILE_CRC32(zbase + a->lf))) {
        p = SetStatus(200, "OK");
      } else {
        return ServeError(500, "Internal Server Error");
      }
    } else if (!IsTiny() && msg.method != kHttpHead && ClientAcceptsGzip() &&
               ((contentlength >= 100 && StartsWithIgnoreCase(ct, "text/")) ||
                (contentlength >= 1000 && MeasureEntropy(content, 1000) < 6))) {
      p = ServeAssetCompressed(a);
    } else {
      p = ServeAssetIdentity(a, ct);
    }
  }
  p = AppendContentType(p, ct);
  p = stpcpy(p, "Vary: Accept-Encoding\r\n");
  p = AppendHeader(p, "Last-Modified", a->lastmodifiedstr);
  if (msg.version >= 11) {
    p = AppendCache(p, cacheseconds);
    if (!IsCompressed(a)) {
      p = stpcpy(p, "Accept-Ranges: bytes\r\n");
    }
  }
  return p;
}

static char *SetStatus(unsigned code, const char *reason) {
  if (msg.version == 10) {
    if (code == 307) code = 302;
    if (code == 308) code = 301;
  }
  statuscode = code;
  stpcpy(hdrbuf.p, "HTTP/1.0 000 ");
  hdrbuf.p[7] += msg.version & 1;
  hdrbuf.p[9] += code / 100;
  hdrbuf.p[10] += code / 10 % 10;
  hdrbuf.p[11] += code % 10;
  return AppendCrlf(stpcpy(hdrbuf.p + 13, reason));
}

static inline bool MustNotIncludeMessageBody(void) { /* RFC2616 § 4.4 */
  return msg.method == kHttpHead || (100 <= statuscode && statuscode <= 199) ||
         statuscode == 204 || statuscode == 304;
}

static bool TransmitResponse(char *p) {
  int iovlen;
  struct iovec iov[4];
  long actualcontentlength;
  if (msg.version >= 10) {
    actualcontentlength = contentlength;
    if (gzipped) {
      actualcontentlength += sizeof(kGzipHeader) + sizeof(gzip_footer);
      p = stpcpy(p, "Content-Encoding: gzip\r\n");
    }
    p = AppendContentLength(p, actualcontentlength);
    p = AppendCrlf(p);
    CHECK_LE(p - hdrbuf.p, hdrbuf.n);
    if (logmessages) {
      LogMessage("sending", hdrbuf.p, p - hdrbuf.p);
    }
    iov[0].iov_base = hdrbuf.p;
    iov[0].iov_len = p - hdrbuf.p;
    iovlen = 1;
    if (!MustNotIncludeMessageBody()) {
      if (gzipped) {
        iov[iovlen].iov_base = kGzipHeader;
        iov[iovlen].iov_len = sizeof(kGzipHeader);
        ++iovlen;
      }
      iov[iovlen].iov_base = content;
      iov[iovlen].iov_len = contentlength;
      ++iovlen;
      if (gzipped) {
        iov[iovlen].iov_base = gzip_footer;
        iov[iovlen].iov_len = sizeof(gzip_footer);
        ++iovlen;
      }
    }
  } else {
    iov[0].iov_base = content;
    iov[0].iov_len = contentlength;
    iovlen = 1;
  }
  Send(iov, iovlen);
  LockInc(&shared->c.messageshandled);
  ++messageshandled;
  return true;
}

static bool StreamResponse(char *p) {
  int rc;
  struct iovec iov[6];
  char *s, chunkbuf[23];
  assert(!MustNotIncludeMessageBody());
  if (msg.version >= 11) {
    p = stpcpy(p, "Transfer-Encoding: chunked\r\n");
  } else {
    assert(connectionclose);
  }
  p = AppendCrlf(p);
  CHECK_LE(p - hdrbuf.p, hdrbuf.n);
  if (logmessages) {
    LogMessage("sending", hdrbuf.p, p - hdrbuf.p);
  }
  memset(iov, 0, sizeof(iov));
  if (msg.version >= 10) {
    iov[0].iov_base = hdrbuf.p;
    iov[0].iov_len = p - hdrbuf.p;
  }
  if (msg.version >= 11) {
    iov[5].iov_base = "\r\n";
    iov[5].iov_len = 2;
  }
  for (;;) {
    iov[2].iov_base = 0;
    iov[2].iov_len = 0;
    iov[3].iov_base = 0;
    iov[3].iov_len = 0;
    iov[4].iov_base = 0;
    iov[4].iov_len = 0;
    if ((rc = generator(iov + 2)) <= 0) break;
    if (msg.version >= 11) {
      s = chunkbuf;
      s += uint64toarray_radix16(rc, s);
      s = AppendCrlf(s);
      iov[1].iov_base = chunkbuf;
      iov[1].iov_len = s - chunkbuf;
    }
    if (Send(iov, 6) == -1) break;
    iov[0].iov_base = 0;
    iov[0].iov_len = 0;
  }
  if (rc != -1) {
    if (msg.version >= 11) {
      iov[0].iov_base = "0\r\n\r\n";
      iov[0].iov_len = 5;
      Send(iov, 1);
    }
  } else {
    connectionclose = true;
  }
  return true;
}

static bool HandleMessage(void) {
  int rc;
  char *p;
  g_syscount = 0;
  if ((rc = ParseHttpMessage(&msg, inbuf.p, amtread)) != -1) {
    if (!rc) return false;
    hdrsize = rc;
    p = HandleRequest();
  } else {
    LockInc(&shared->c.badmessages);
    connectionclose = true;
    LOGF("%s sent garbage %`'s", DescribeClient(),
         VisualizeControlCodes(inbuf.p, MIN(128, amtread), 0));
    return true;
  }
  if (!msgsize) {
    amtread = 0;
    connectionclose = true;
    LockInc(&shared->c.synchronizationfailures);
    DEBUGF("could not synchronize message stream");
  }
  if (msg.version >= 10) {
    p = AppendCrlf(stpcpy(stpcpy(p, "Date: "), shared->currentdate));
    if (!branded) p = stpcpy(p, serverheader);
    if (extrahdrs) p = stpcpy(p, extrahdrs);
    if (connectionclose) {
      p = stpcpy(p, "Connection: close\r\n");
    } else if (timeout.tv_sec < 0 && msg.version >= 11) {
      p = stpcpy(p, "Connection: keep-alive\r\n");
    }
  }
  if (loglatency || LOGGABLE(kLogDebug)) {
    flogf(kLogDebug, __FILE__, __LINE__, NULL, "%`'.*s latency %,ldµs",
          msg.uri.b - msg.uri.a, inbuf.p + msg.uri.a,
          (long)((nowl() - startrequest) * 1e6L));
  }
  LockInc(&shared->c.messageshandled);
  ++messageshandled;
  if (!generator) {
    return TransmitResponse(p);
  } else {
    return StreamResponse(p);
  }
}

static void InitRequest(void) {
  frags = 0;
  gzipped = 0;
  branded = 0;
  content = 0;
  msgsize = 0;
  loops.n = 0;
  outbuf.n = 0;
  generator = 0;
  luaheaderp = 0;
  contentlength = 0;
  InitHttpMessage(&msg, kHttpRequest);
}

static void HandleMessages(void) {
  bool once;
  ssize_t rc;
  size_t got;
  for (once = false;;) {
    InitRequest();
    startread = nowl();
    for (;;) {
      if (!msg.i && amtread) {
        startrequest = nowl();
        if (HandleMessage()) break;
      }
      if ((rc = reader(client, inbuf.p + amtread, inbuf.n - amtread)) != -1) {
        startrequest = nowl();
        got = rc;
        amtread += got;
        if (amtread) {
#ifndef UNSECURE
          if (!once) {
            once = true;
            if (inbuf.p[0] == 22) {
              if (TlsSetup()) {
                continue;
              } else {
                return;
              }
            } else {
              WipeServingKeys();
            }
          }
#endif
          DEBUGF("%s read %,zd bytes", DescribeClient(), got);
          if (HandleMessage()) {
            break;
          } else if (got) {
            HandleFrag(got);
          }
        }
        if (!got) {
          NotifyClose();
          LogClose("disconnect");
          return;
        }
      } else if (errno == EINTR) {
        LockInc(&shared->c.readinterrupts);
      } else if (errno == EAGAIN) {
        LockInc(&shared->c.readtimeouts);
        if (amtread) SendTimeout();
        NotifyClose();
        LogClose("timeout");
        return;
      } else if (errno == ECONNRESET) {
        LockInc(&shared->c.readresets);
        LogClose("reset");
        return;
      } else {
        LockInc(&shared->c.readerrors);
        WARNF("%s read failed %s", DescribeClient(), strerror(errno));
        return;
      }
      if (killed || (terminated && !amtread) ||
          (meltdown && (!amtread || nowl() - startread > 1))) {
        if (amtread) {
          LockInc(&shared->c.dropped);
          SendServiceUnavailable();
        }
        NotifyClose();
        LogClose(DescribeClose());
        return;
      }
      if (invalidated) {
        HandleReload();
        invalidated = false;
      }
    }
    if (msgsize == amtread) {
      amtread = 0;
      if (killed) {
        LogClose(DescribeClose());
        return;
      } else if (connectionclose || terminated || meltdown) {
        NotifyClose();
        LogClose(DescribeClose());
        return;
      }
    } else {
      CHECK_LT(msgsize, amtread);
      LockInc(&shared->c.pipelinedrequests);
      DEBUGF("%,ld pipelined bytes", amtread - msgsize);
      memmove(inbuf.p, inbuf.p + msgsize, amtread - msgsize);
      amtread -= msgsize;
      if (killed) {
        LogClose(DescribeClose());
        return;
      } else if (connectionclose) {
        NotifyClose();
        LogClose(DescribeClose());
        return;
      }
    }
    CollectGarbage();
    if (invalidated) {
      HandleReload();
      invalidated = false;
    }
  }
}

static void CloseServerFds(void) {
  size_t i;
  for (i = 0; i < servers.n; ++i) {
    close(servers.p[i].fd);
  }
}

static void HandleConnection(size_t i) {
  int pid;
  clientaddrsize = sizeof(clientaddr);
  if ((client = accept4(servers.p[i].fd, &clientaddr, &clientaddrsize,
                        SOCK_CLOEXEC)) != -1) {
    startconnection = nowl();
    messageshandled = 0;
    if (hasonclientconnection && LuaOnClientConnection()) {
      close(client);
      return;
    }
    if (uniprocess) {
      pid = -1;
      connectionclose = true;
    } else {
      switch ((pid = fork())) {
        case 0:
          meltdown = false;
          connectionclose = false;
          if (funtrace && !IsTiny()) {
            ftrace_install();
          }
          if (hasonworkerstart) {
            CallSimpleHook("OnWorkerStart");
          }
          break;
        case -1:
          HandleForkFailure();
          return;
        default:
          ++shared->workers;
          close(client);
          ReseedRng(&rng, "parent");
          if (hasonprocesscreate) {
            LuaOnProcessCreate(pid);
          }
          return;
      }
    }
    if (!pid) CloseServerFds();
    VERBOSEF("ACCEPT %s VIA %s", DescribeClient(), DescribeServer());
    HandleMessages();
    DEBUGF("%s closing after %,ldµs", DescribeClient(),
           (long)((nowl() - startconnection) * 1e6L));
    if (!pid) {
      if (hasonworkerstop) {
        CallSimpleHook("OnWorkerStop");
      }
      _exit(0);
    } else {
      close(client);
      oldin.p = 0;
      oldin.n = 0;
      if (inbuf.c) {
        inbuf.p -= inbuf.c;
        inbuf.n += inbuf.c;
        inbuf.c = 0;
      }
#ifndef UNSECURE
      if (usessl) {
        usessl = false;
        reader = read;
        writer = WritevAll;
        LOGF("reset");
        mbedtls_ssl_session_reset(&ssl);
      }
#endif
      CollectGarbage();
    }
  } else if (errno == EINTR || errno == EAGAIN) {
    LockInc(&shared->c.acceptinterrupts);
  } else if (errno == ENFILE) {
    LockInc(&shared->c.enfiles);
    WARNF("too many open files");
    meltdown = true;
  } else if (errno == EMFILE) {
    LockInc(&shared->c.emfiles);
    WARNF("ran out of open file quota");
    meltdown = true;
  } else if (errno == ENOMEM) {
    LockInc(&shared->c.enomems);
    WARNF("ran out of memory");
    meltdown = true;
  } else if (errno == ENOBUFS) {
    LockInc(&shared->c.enobufs);
    WARNF("ran out of buffer");
    meltdown = true;
  } else if (errno == ENONET) {
    LockInc(&shared->c.enonets);
    WARNF("%s network gone", DescribeServer());
    polls[i].fd = -polls[i].fd;
  } else if (errno == ENETDOWN) {
    LockInc(&shared->c.enetdowns);
    WARNF("%s network down", DescribeServer());
    polls[i].fd = -polls[i].fd;
  } else if (errno == ECONNABORTED) {
    LockInc(&shared->c.acceptresets);
    WARNF("%s connection reset before accept");
  } else if (errno == ENETUNREACH || errno == EHOSTUNREACH ||
             errno == EOPNOTSUPP || errno == ENOPROTOOPT || errno == EPROTO) {
    LockInc(&shared->c.accepterrors);
    WARNF("%s ephemeral accept error %s", DescribeServer(), strerror(errno));
  } else {
    FATALF("%s accept error %s", DescribeServer(), strerror(errno));
  }
}

static void HandlePoll(void) {
  size_t i;
  if (poll(polls, servers.n, 500) != -1) {
    for (i = 0; i < servers.n; ++i) {
      if (polls[i].revents) {
        serveraddr = &servers.p[i].addr;
        HandleConnection(i);
      }
    }
  } else if (errno == EINTR || errno == EAGAIN) {
    LockInc(&shared->c.pollinterrupts);
  } else if (errno == ENOMEM) {
    LockInc(&shared->c.enomems);
    WARNF("%s ran out of memory");
    meltdown = true;
  } else {
    FATALF("poll error %s", strerror(errno));
  }
}

static void RestoreApe(void) {
  char *p;
  size_t n;
  struct Asset *a;
  extern char ape_rom_vaddr[] __attribute__((__weak__));
  if (!(SUPPORT_VECTOR & (METAL | WINDOWS | XNU))) return;
  if (IsWindows()) return; /* TODO */
  if (IsOpenbsd()) return; /* TODO */
  if (IsNetbsd()) return;  /* TODO */
  if (endswith(zpath, ".com.dbg")) return;
  close(zfd);
  zfd = OpenExecutable();
  if ((a = GetAssetZip("/.ape", 5)) && (p = LoadAsset(a, &n))) {
    write(zfd, p, n);
    free(p);
  } else {
    WARNF("/.ape not found");
  }
}

static void Listen(void) {
  char ipbuf[16];
  size_t i, j, n;
  uint32_t ip, port, addrsize, *ifs, *ifp;
  if (!ports.n) {
    ProgramPort(8080);
  }
  if (!ips.n) {
    if ((ifs = GetHostIps()) && *ifs) {
      for (ifp = ifs; *ifp; ++ifp) {
        sprintf(ipbuf, "%hhu.%hhu.%hhu.%hhu", *ifp >> 24, *ifp >> 16, *ifp >> 8,
                *ifp);
        ProgramAddr(ipbuf);
      }
    } else {
      ProgramAddr("0.0.0.0");
    }
    free(ifs);
  }
  servers.p = malloc(ips.n * ports.n * sizeof(*servers.p));
  for (n = i = 0; i < ips.n; ++i) {
    for (j = 0; j < ports.n; ++j, ++n) {
      memset(servers.p + n, 0, sizeof(*servers.p));
      servers.p[n].addr.sin_family = AF_INET;
      servers.p[n].addr.sin_port = htons(ports.p[j]);
      servers.p[n].addr.sin_addr.s_addr = htonl(ips.p[i]);
      if ((servers.p[n].fd = Socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC,
                                    IPPROTO_TCP, true)) == -1) {
        perror("socket");
        exit(1);
      }
      if (bind(servers.p[n].fd, &servers.p[n].addr,
               sizeof(servers.p[n].addr)) == -1) {
        fprintf(stderr, "error: %s: %hhu.%hhu.%hhu.%hhu:%hu\n", strerror(errno),
                ips.p[i] >> 24, ips.p[i] >> 16, ips.p[i] >> 8, ips.p[i],
                ports.p[j]);
        exit(1);
      }
      if (listen(servers.p[n].fd, 10) == -1) {
        perror("listen");
        exit(1);
      }
      addrsize = sizeof(servers.p[n].addr);
      if (getsockname(servers.p[n].fd, &servers.p[n].addr, &addrsize) == -1) {
        perror("getsockname");
        exit(1);
      }
      port = ntohs(servers.p[n].addr.sin_port);
      ip = ntohl(servers.p[n].addr.sin_addr.s_addr);
      if (ip == INADDR_ANY) ip = INADDR_LOOPBACK;
      LOGF("LISTEN http://%hhu.%hhu.%hhu.%hhu:%d", ip >> 24, ip >> 16, ip >> 8,
           ip, port);
      if (printport && !ports.p[j]) {
        printf("%d\n", port);
        fflush(stdout);
      }
    }
  }
  servers.n = n;
  polls = malloc(n * sizeof(*polls));
  for (i = 0; i < n; ++i) {
    polls[i].fd = servers.p[i].fd;
    polls[i].events = POLLIN;
    polls[i].revents = 0;
  }
}

static void HandleShutdown(void) {
  CloseServerFds();
  if (keyboardinterrupt) {
    LOGF("received keyboard interrupt");
  } else {
    LOGF("received term signal");
    if (!killed) {
      terminated = false;
    }
    DEBUGF("sending TERM to process group");
    LOGIFNEG1(kill(0, SIGTERM));
  }
  WaitAll();
}

static void HandleEvents(void) {
  long double t;
  while (!terminated) {
    if (zombied) {
      ReapZombies();
    } else if (invalidated) {
      HandleReload();
      invalidated = false;
    } else if (meltdown) {
      EnterMeltdownMode();
      meltdown = false;
    } else if ((t = nowl()) - lastheartbeat > .5) {
      lastheartbeat = t;
      HandleHeartbeat();
    } else {
      HandlePoll();
    }
  }
}

static void SigInit(void) {
  xsigaction(SIGINT, OnInt, 0, 0, 0);
  xsigaction(SIGHUP, OnHup, 0, 0, 0);
  xsigaction(SIGTERM, OnTerm, 0, 0, 0);
  xsigaction(SIGCHLD, OnChld, 0, 0, 0);
  xsigaction(SIGUSR1, OnUsr1, 0, 0, 0);
  xsigaction(SIGUSR2, OnUsr2, 0, 0, 0);
  xsigaction(SIGPIPE, SIG_IGN, 0, 0, 0);
  /* TODO(jart): SIGXCPU and SIGXFSZ */
}

static void TlsInit(void) {
#ifndef UNSECURE
  mbedtls_ssl_config_defaults(
      &conf, MBEDTLS_SSL_IS_SERVER, MBEDTLS_SSL_TRANSPORT_STREAM,
      suiteb ? MBEDTLS_SSL_PRESET_SUITEB : MBEDTLS_SSL_PRESET_DEFAULT);
  mbedtls_ssl_config_defaults(
      &confcli, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM,
      suiteb ? MBEDTLS_SSL_PRESET_SUITEB : MBEDTLS_SSL_PRESET_DEFAULT);
  DCHECK_EQ(0,
            mbedtls_ssl_ticket_setup(&ssltick, mbedtls_ctr_drbg_random, &rng,
                                     MBEDTLS_CIPHER_AES_256_GCM, 24 * 60 * 60));
  mbedtls_ssl_conf_session_tickets_cb(&conf, mbedtls_ssl_ticket_write,
                                      mbedtls_ssl_ticket_parse, &ssltick);
  LoadCertificates();
  mbedtls_ssl_conf_dbg(&conf, TlsDebug, 0);
  mbedtls_ssl_conf_dbg(&confcli, TlsDebug, 0);
  mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &rng);
  mbedtls_ssl_conf_rng(&confcli, mbedtls_ctr_drbg_random, &rngcli);
  mbedtls_ssl_conf_authmode(&conf, sslclientverify ? MBEDTLS_SSL_VERIFY_REQUIRED
                                                   : MBEDTLS_SSL_VERIFY_NONE);
  mbedtls_ssl_conf_authmode(&confcli, sslfetchverify
                                          ? MBEDTLS_SSL_VERIFY_REQUIRED
                                          : MBEDTLS_SSL_VERIFY_NONE);
  mbedtls_ssl_conf_ca_chain(&confcli, (cachain = GetSslRoots()), 0);
  mbedtls_ssl_set_bio(&ssl, &g_bio, TlsSend, 0, TlsRecv);
  DCHECK_EQ(0, mbedtls_ssl_conf_alpn_protocols(&conf, kAlpn));
  DCHECK_EQ(0, mbedtls_ssl_conf_alpn_protocols(&confcli, kAlpn));
  DCHECK_EQ(0, mbedtls_ssl_setup(&ssl, &conf));
  DCHECK_EQ(0, mbedtls_ssl_setup(&sslcli, &confcli));
#endif
}

static void TlsDestroy(void) {
#ifndef UNSECURE
  size_t i;
  mbedtls_ssl_free(&ssl);
  mbedtls_ssl_free(&sslcli);
  mbedtls_ctr_drbg_free(&rng);
  mbedtls_x509_crt_free(cachain);
  mbedtls_ctr_drbg_free(&rngcli);
  mbedtls_ssl_config_free(&conf);
  mbedtls_ssl_config_free(&confcli);
  mbedtls_ssl_ticket_free(&ssltick);
  for (i = 0; i < certs.n; ++i) {
    mbedtls_x509_crt_free(certs.p[i].cert);
    mbedtls_pk_free(certs.p[i].key);
  }
  free(certs.p), certs.p = 0, certs.n = 0;
  free(ports.p), ports.p = 0, ports.n = 0;
  free(ips.p), ips.p = 0, ips.n = 0;
#endif
}

static void MemDestroy(void) {
  FreeAssets();
  CollectGarbage();
  Free(&unmaplist.p), unmaplist.n = 0;
  Free(&freelist.p), freelist.n = 0;
  Free(&servers.p), servers.n = 0;
  Free(&outbuf.p), outbuf.n = 0;
  Free(&hdrbuf.p), hdrbuf.n = 0;
  Free(&inbuf.p), inbuf.n = 0;
  FreeStrings(&stagedirs);
  FreeStrings(&hidepaths);
  Free(&launchbrowser);
  Free(&serverheader);
  Free(&extrahdrs);
  Free(&pidpath);
  Free(&logpath);
  Free(&brand);
  Free(&polls);
}

void RedBean(int argc, char *argv[]) {
#ifndef UNSECURE
  InitializeRng(&rng);
  InitializeRng(&rngcli);
#endif
  reader = read;
  writer = WritevAll;
  gmtoff = GetGmtOffset((lastrefresh = startserver = nowl()));
  CHECK_GT(CLK_TCK, 0);
  CHECK_NE(MAP_FAILED,
           (shared = mmap(NULL, ROUNDUP(sizeof(struct Shared), FRAMESIZE),
                          PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,
                          -1, 0)));
  zpath = (const char *)getauxval(AT_EXECFN);
  CHECK_NE(-1, (zfd = open(zpath, O_RDONLY)));
  CHECK_NE(-1, fstat(zfd, &zst));
  OpenZip(true);
  RestoreApe();
  SetDefaults();
  GetOpts(argc, argv);
  LuaInit();
  oldloglevel = __log_level;
  if (uniprocess) shared->workers = 1;
  SigInit();
  Listen();
  TlsInit();
  if (launchbrowser) {
    LaunchBrowser(launchbrowser);
  }
  if (daemonize) {
    Daemonize();
  } else {
    setpgid(getpid(), getpid());
    if (logpath) {
      close(2);
      open(logpath, O_APPEND | O_WRONLY | O_CREAT, 0640);
    }
    ChangeUser();
  }
  UpdateCurrentDate(nowl());
  CollectGarbage();
  hdrbuf.n = 4 * 1024;
  hdrbuf.p = xmalloc(hdrbuf.n);
  inbuf.n = maxpayloadsize;
  inbuf.p = xmalloc(inbuf.n);
  CallSimpleHookIfDefined("OnServerStart");
  HandleEvents();
  HandleShutdown();
  CallSimpleHookIfDefined("OnServerStop");
  if (!IsTiny()) {
    LuaDestroy();
    TlsDestroy();
    MemDestroy();
  }
  VERBOSEF("shutdown complete");
}

int main(int argc, char *argv[]) {
  if (!IsTiny()) {
    setenv("GDB", "", true);
    showcrashreports();
  }
  RedBean(argc, argv);
  return 0;
}
