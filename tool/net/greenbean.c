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
#include "libc/alg/arraylist2.internal.h"
#include "libc/bits/bits.h"
#include "libc/bits/bswap.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/fmt.h"
#include "libc/fmt/itoa.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/rand/rand.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/epoll.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/undeflate.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/epoll.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/inaddr.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/itimer.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/shut.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"
#include "libc/sysv/consts/tcp.h"
#include "libc/sysv/errfuns.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "libc/zip.h"
#include "libc/zipos/zipos.internal.h"
#include "net/http/http.h"
#include "third_party/getopt/getopt.h"
#include "third_party/zlib/zlib.h"

/**
 * @fileoverview redbean.c modified for epoll tutorial
 * This'll work on Linux and Windows but BSD is TODO
 * TODO(jart): fix memory bug
 */

#define USAGE \
  " [-hvdsm] [-p PORT]\n\
\n\
DESCRIPTION\n\
\n\
  greenbean distributable static web server\n\
\n\
FLAGS\n\
\n\
  -h        help\n\
  -v        verbosity\n\
  -d        daemonize\n\
  -s        uniprocess\n\
  -m        log messages\n\
  -c INT    cache seconds\n\
  -r /X=/Y  redirect X to Y\n\
  -l ADDR   listen ip [default 0.0.0.0]\n\
  -p PORT   listen port [default 8080]\n\
  -L PATH   log file location\n\
  -P PATH   pid file location\n\
  -U INT    daemon set user id\n\
  -G INT    daemon set group id\n\
  -B STR    changes server header\n\
\n\
FEATURES\n\
\n\
  - HTTP v1.1\n\
  - Content-Encoding\n\
  - Range / Content-Range\n\
  - Last-Modified / If-Modified-Since\n\
\n\
USAGE\n\
\n\
  This executable is also a ZIP file that contains static assets.\n\
\n\
    unzip -vl greenbean.com  # shows listing of zip contents\n\
\n\
  Audio video content should not be compressed in your ZIP files.\n\
  Uncompressed assets enable browsers to send Range HTTP request.\n\
  On the other hand compressed assets are best for gzip encoding.\n\
\n\
    zip greenbean.com index.html    # adds file\n\
    zip -0 greenbean.com video.mp4  # adds without compression\n\
\n\
  Each connection uses a point in time snapshot of your ZIP file.\n\
  If your ZIP is deleted then serving continues. If it's replaced\n\
  then issuing SIGUSR1 (or SIGHUP if daemon) will reindex the zip\n\
  for subsequent connections without interrupting active ones. If\n\
  SIGINT or SIGTERM is issued then a graceful shutdown is started\n\
  but if it's issued a second time, active connections are reset.\n\
\n"

#define HASH_LOAD_FACTOR     /* 1. / */ 4
#define DEFAULT_PORT         8080
#define DEFAULT_SERVER       "greenbean/0.1"
#define DEFAULT_CONTENT_TYPE "application/octet-stream"
#define DEFAULT_PATH         "/tool/net/redbean.html"
#define FAVICON              "tool/net/redbean.ico"

#define STPCPY(p, s)           mempcpy(p, s, strlen(s))
#define AppendHeaderName(p, s) STPCPY(STPCPY(p, s), ": ")

struct Client {
  uint32_t poison;
  struct Client *prev;
  struct Client *next;
  int fd;
  int iovlen;
  bool close;
  uint32_t insize;
  uint32_t addrsize;
  uint8_t gzip_footer[8];
  struct sockaddr_in addr;
  long double startrequest;
  long double startconnection;
  struct HttpRequest req;
  struct iovec iov[8];
  char addrstr[32];
  char outbuf[1500];
  char inbuf[2048];
};

static const struct itimerval kHeartbeat = {
    {0, 500000},
    {0, 500000},
};

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

static const struct ContentTypeExtension {
  unsigned char ext[8];
  const char *mime;
} kContentTypeExtension[] = {
    {"S", "text/plain"},                  //
    {"bmp", "image/x-ms-bmp"},            //
    {"c", "text/plain"},                  //
    {"cc", "text/plain"},                 //
    {"css", "text/css"},                  //
    {"csv", "text/csv"},                  //
    {"gif", "image/gif"},                 //
    {"h", "text/plain"},                  //
    {"html", "text/html"},                //
    {"i", "text/plain"},                  //
    {"ico", "image/vnd.microsoft.icon"},  //
    {"jpeg", "image/jpeg"},               //
    {"jpg", "image/jpeg"},                //
    {"js", "application/javascript"},     //
    {"json", "application/json"},         //
    {"m4a", "audio/mpeg"},                //
    {"mp2", "audio/mpeg"},                //
    {"mp3", "audio/mpeg"},                //
    {"mp4", "video/mp4"},                 //
    {"mpg", "video/mpeg"},                //
    {"otf", "font/otf"},                  //
    {"pdf", "application/pdf"},           //
    {"png", "image/png"},                 //
    {"png", "image/png"},                 //
    {"png", "image/png"},                 //
    {"png", "image/png"},                 //
    {"s", "text/plain"},                  //
    {"svg", "image/svg+xml"},             //
    {"tiff", "image/tiff"},               //
    {"ttf", "font/ttf"},                  //
    {"txt", "text/plain"},                //
    {"wav", "audio/x-wav"},               //
    {"woff", "font/woff"},                //
    {"woff2", "font/woff2"},              //
    {"xml", "application/xml"},           //
    {"zip", "application/zip"},           //
};

static struct Redirects {
  size_t i, n;
  struct Redirect {
    const char *path;
    size_t pathlen;
    const char *dest;
  } * p;
} redirects;

static struct Assets {
  uint32_t n;
  struct Asset {
    uint32_t hash;
    uint32_t cf;
    int64_t lastmodified;
    char *lastmodifiedstr;
  } * p;
} assets;

static bool killed;
static bool notimer;
static bool heartbeat;
static bool daemonize;
static bool terminated;
static bool uniprocess;
static bool legacyhttp;
static bool invalidated;
static bool logmessages;

static int epfd;
static int gmtoff;
static int server;
static int daemonuid;
static int daemongid;
static int cacheseconds;

static void *zmap;
static uint8_t *zbase;
static uint8_t *zcdir;
static size_t zmapsize;
static const char *pidpath;
static const char *logpath;
static int64_t programtime;
static struct Client *clients;
static const char *programfile;
static const char *serverheader;

static long double nowish;
static struct sockaddr_in serveraddr;

static char currentdate[32];
static char serveraddrstr[32];

static void OnHup(void) {
  invalidated = true;
}

static void OnAlarm(void) {
  heartbeat = true;
}

static void OnTerminate(void) {
  if (terminated) {
    killed = true;
  } else {
    terminated = true;
  }
}

static void AddRedirect(const char *arg) {
  const char *p;
  struct Redirect r;
  CHECK_NOTNULL((p = strchr(arg, '=')));
  CHECK_GT(p - arg, 0);
  r.path = arg;
  r.pathlen = p - arg;
  r.dest = strdup(p + 1);
  APPEND(&redirects.p, &redirects.i, &redirects.n, &r);
}

static int CompareRedirects(const struct Redirect *a,
                            const struct Redirect *b) {
  return strcmp(a->path, b->path);
}

static void SortRedirects(void) {
  qsort(redirects.p, redirects.i, sizeof(struct Redirect),
        (void *)CompareRedirects);
}

static const char *LookupRedirect(const char *path, size_t n) {
  int c, m, l, r, z;
  l = 0;
  r = redirects.i - 1;
  while (l <= r) {
    m = (l + r) >> 1;
    c = memcmp(redirects.p[m].path, path, MIN(redirects.p[m].pathlen, n));
    if (c < 0) {
      l = m + 1;
    } else if (c > 0) {
      r = m - 1;
    } else if (redirects.p[m].pathlen < n) {
      l = m + 1;
    } else if (redirects.p[m].pathlen > n) {
      r = m - 1;
    } else {
      return redirects.p[m].dest;
    }
  }
  return NULL;
}

static int CompareInts(const uint64_t x, uint64_t y) {
  return x > y ? 1 : x < y ? -1 : 0;
}

static const char *LookupContentType(uint64_t ext) {
  int c, m, l, r;
  l = 0;
  r = ARRAYLEN(kContentTypeExtension) - 1;
  while (l <= r) {
    m = (l + r) >> 1;
    c = CompareInts(READ64BE(kContentTypeExtension[m].ext), ext);
    if (c < 0) {
      l = m + 1;
    } else if (c > 0) {
      r = m - 1;
    } else {
      return kContentTypeExtension[m].mime;
    }
  }
  return DEFAULT_CONTENT_TYPE;
}

static const char *GetContentType(const char *path, size_t n) {
  size_t i;
  uint64_t x;
  const char *p;
  if ((p = memrchr(path, '.', n))) {
    for (x = 0, i = n; i-- > p + 1 - path;) {
      x <<= 8;
      x |= path[i] & 0xFF;
    }
    return LookupContentType(bswap_64(x));
  } else {
    return DEFAULT_CONTENT_TYPE;
  }
}

static wontreturn void PrintUsage(FILE *f, int rc) {
  fprintf(f, "SYNOPSIS\n\n  %s%s", program_invocation_name, USAGE);
  exit(rc);
}

static void DescribeAddress(char buf[32], const struct sockaddr_in *addr) {
  char *p = buf;
  const uint8_t *ip4 = (const uint8_t *)&addr->sin_addr.s_addr;
  p += uint64toarray_radix10(ip4[0], p), *p++ = '.';
  p += uint64toarray_radix10(ip4[1], p), *p++ = '.';
  p += uint64toarray_radix10(ip4[2], p), *p++ = '.';
  p += uint64toarray_radix10(ip4[3], p), *p++ = ':';
  p += uint64toarray_radix10(ntohs(addr->sin_port), p);
  *p = '\0';
}

void GetOpts(int argc, char *argv[]) {
  int opt;
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(DEFAULT_PORT);
  serveraddr.sin_addr.s_addr = INADDR_ANY;
  while ((opt = getopt(argc, argv, "hduvml:p:w:r:c:L:P:U:G:B:")) != -1) {
    switch (opt) {
      case 'v':
        g_loglevel++;
        break;
      case 'd':
        daemonize = true;
        break;
      case 'u':
        uniprocess = true;
        break;
      case 'm':
        logmessages = true;
        break;
      case 'r':
        AddRedirect(optarg);
        break;
      case 'c':
        cacheseconds = atoi(optarg);
        break;
      case 'p':
        CHECK_NE(0xFFFF, (serveraddr.sin_port = htons(parseport(optarg))));
        break;
      case 'l':
        CHECK_EQ(1, inet_pton(AF_INET, optarg, &serveraddr.sin_addr));
        break;
      case 'B':
        serverheader = optarg;
        break;
      case 'L':
        logpath = optarg;
        break;
      case 'P':
        pidpath = optarg;
        break;
      case 'U':
        daemonuid = atoi(optarg);
        break;
      case 'G':
        daemonuid = atoi(optarg);
        break;
      case 'h':
        PrintUsage(stdout, EXIT_SUCCESS);
      default:
        PrintUsage(stderr, EX_USAGE);
    }
  }
  SortRedirects();
  if (logpath) {
    CHECK_NOTNULL(freopen(logpath, "a", stderr));
  }
}

static void Daemonize(void) {
  char ibuf[21];
  int i, fd, pid;
  for (i = 0; i < 128; ++i) close(i);
  xsigaction(SIGHUP, OnHup, 0, 0, 0);
  CHECK_NE(-1, (pid = fork()));
  if (pid > 0) exit(0);
  if (pid == -1) return;
  CHECK_NE(-1, setsid());
  CHECK_NE(-1, (pid = fork()));
  if (pid > 0) _exit(0);
  LOGIFNEG1(umask(0));
  if (pidpath) {
    CHECK_NE(-1, (fd = open(pidpath, O_CREAT | O_EXCL | O_WRONLY, 0644)));
    CHECK_NE(-1, write(fd, ibuf, uint64toarray_radix10(getpid(), ibuf)));
    LOGIFNEG1(close(fd));
  }
  if (!logpath) logpath = "/dev/null";
  CHECK_NOTNULL(freopen("/dev/null", "r", stdin));
  CHECK_NOTNULL(freopen(logpath, "a", stdout));
  CHECK_NOTNULL(freopen(logpath, "a", stderr));
}

static int CompareHeaderValue(struct Client *c, int h, const char *s) {
  return strncmp(s, c->inbuf + c->req.headers[h].a,
                 c->req.headers[h].b - c->req.headers[h].a);
}

static size_t GetIovSize(struct iovec *iov, int iovlen) {
  int i;
  size_t size;
  for (size = i = 0; i < iovlen; ++i) {
    DCHECK_NOTNULL(iov[i].iov_base);
    size += iov[i].iov_len;
  }
  return size;
}

static ssize_t WriteSome(int fd, struct iovec *iov, int iovlen) {
  ssize_t rc;
  size_t wrote, n;
  if ((rc = writev(fd, iov, iovlen)) != -1) {
    wrote = n = rc;
    do {
      if (n >= iov->iov_len) {
        iov->iov_len = 0;
        n -= iov->iov_len;
        ++iov;
        --iovlen;
      } else {
        iov->iov_base = (char *)iov->iov_base + n;
        iov->iov_len -= n;
        n = 0;
      }
    } while (n);
    return wrote;
  } else if (errno == EINTR) {
    return 0;
  } else {
    return -1;
  }
}

static uint32_t Hash(const void *data, size_t size) {
  uint32_t h;
  h = crc32c(0, data, size);
  if (!h) h = 1;
  return h;
}

static bool HasHeader(struct Client *c, int h) {
  return c->req.headers[h].b > c->req.headers[h].a;
}

int64_t GetGmtOffset(void) {
  int64_t t;
  struct tm tm;
  t = nowl();
  localtime_r(&t, &tm);
  return tm.tm_gmtoff;
}

static int64_t LocoTimeToZulu(int64_t x) {
  return x - gmtoff;
}

static int64_t GetLastModifiedZip(uint8_t *cfile) {
  uint8_t *p, *pe;
  for (p = ZIP_CFILE_EXTRA(cfile), pe = p + ZIP_CFILE_EXTRASIZE(cfile); p < pe;
       p += ZIP_EXTRA_SIZE(p)) {
    if (ZIP_EXTRA_HEADERID(p) == kZipExtraNtfs) {
      return LocoTimeToZulu(READ64LE(ZIP_EXTRA_CONTENT(p) + 8) /
                                HECTONANOSECONDS -
                            MODERNITYSECONDS);
    } else if (ZIP_EXTRA_HEADERID(p) == kZipExtraExtendedTimestamp) {
      return READ32LE(ZIP_EXTRA_CONTENT(p) + 1);
    }
  }
  return LocoTimeToZulu(DosDateTimeToUnix(ZIP_CFILE_LASTMODIFIEDDATE(cfile),
                                          ZIP_CFILE_LASTMODIFIEDTIME(cfile)));
}

static bool IsCompressed(struct Asset *a) {
  return ZIP_CFILE_COMPRESSIONMETHOD(zbase + a->cf) == kZipCompressionDeflate;
}

static int GetHttpVersion(struct Client *c) {
  return ParseHttpVersion(c->inbuf + c->req.version.a,
                          c->req.version.b - c->req.version.a);
}

static bool IsNotModified(struct Client *c, struct Asset *a) {
  if (!HasHeader(c, kHttpIfModifiedSince)) return false;
  return a->lastmodified >=
         ParseHttpDateTime(c->inbuf + c->req.headers[kHttpIfModifiedSince].a,
                           c->req.headers[kHttpIfModifiedSince].b -
                               c->req.headers[kHttpIfModifiedSince].a);
}

static char *FormatUnixHttpDateTime(char *s, int64_t t) {
  struct tm tm;
  gmtime_r(&t, &tm);
  FormatHttpDateTime(s, &tm);
  return s;
}

static void FreeAssetsIndex(struct Asset *p, size_t n) {
  int i;
  if (p) {
    for (i = 0; i < n; ++i) {
      free(p[i].lastmodifiedstr);
    }
    free(p);
  }
}

static bool IndexAssets(const uint8_t *base, const uint8_t *cdir) {
  bool ok;
  int64_t lm;
  struct Asset *p;
  uint32_t i, n, m, cf, step, hash;
  DCHECK_GE(HASH_LOAD_FACTOR, 2);
  n = ZIP_CDIR_RECORDS(cdir);
  m = roundup2pow(MAX(1, n) * HASH_LOAD_FACTOR);
  p = calloc(m, sizeof(struct Asset));
  ok = ZIP_CDIR_MAGIC(cdir) == kZipCdirHdrMagic;
  if (p && ok) {
    for (cf = ZIP_CDIR_OFFSET(cdir); n--; cf += ZIP_CFILE_HDRSIZE(base + cf)) {
      if (ZIP_CFILE_MAGIC(base + cf) == kZipCfileHdrMagic) {
        hash = Hash(ZIP_CFILE_NAME(base + cf), ZIP_CFILE_NAMESIZE(base + cf));
        step = 0;
        do {
          i = (hash + (step * (step + 1)) >> 1) & (m - 1);
          ++step;
        } while (p[i].hash);
        lm = GetLastModifiedZip(base + cf);
        p[i].hash = hash;
        p[i].cf = cf;
        p[i].lastmodified = lm;
        p[i].lastmodifiedstr = FormatUnixHttpDateTime(xmalloc(30), lm);
      } else {
        WARNF("corrupt zip central directory entry");
        ok = false;
        break;
      }
    }
  } else {
    WARNF("corrupt zip central directory");
  }
  if (ok) {
    FreeAssetsIndex(assets.p, assets.n);
    assets.p = p;
    assets.n = m;
  } else {
    FreeAssetsIndex(p, m);
  }
  return ok;
}

static bool OpenZip(const char *path) {
  int fd;
  bool ok;
  void *map;
  struct stat st;
  const uint8_t *cdir;
  if (zmap) {
    LOGIFNEG1(munmap(zmap, zmapsize));
  }
  if (!zmap && ZIP_CDIR_MAGIC(__zip_end) == kZipCdirHdrMagic) {
    if (IndexAssets(_base, __zip_end)) {
      ok = true;
      zbase = _base;
      zcdir = __zip_end;
    } else {
      ok = false;
    }
  } else {
    fd = -1;
    map = MAP_FAILED;
    if ((fd = open(path, O_RDONLY)) != -1 && fstat(fd, &st) != -1 &&
        st.st_size &&
        (map = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0)) &&
        (cdir = zipfindcentraldir(zmap, zmapsize)) && IndexAssets(map, cdir)) {
      ok = true;
      zmap = map;
      zbase = map;
      zcdir = cdir;
      map = MAP_FAILED;
      zmapsize = st.st_size;
    } else {
      ok = false;
    }
    if (map != MAP_FAILED) LOGIFNEG1(munmap(map, st.st_size));
    if (fd != -1) LOGIFNEG1(close(fd));
  }
  return ok;
}

static struct Asset *FindAsset(const char *path, size_t pathlen) {
  uint32_t i, step, hash;
  if (pathlen && path[0] == '/') ++path, --pathlen;
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

static struct Asset *FindFile(const char *path, size_t pathlen) {
  char *p, *buf;
  struct Asset *asset;
  if ((asset = FindAsset(path, pathlen))) return asset;
  if (pathlen == 12 && memcmp(path, "/favicon.ico", 12) == 0) {
    return FindAsset(FAVICON, strlen(FAVICON));
  } else {
    return NULL;
  }
}

static void *AddRange(char *content, long start, long length) {
  intptr_t mend, mstart;
  if (!__builtin_add_overflow((intptr_t)content, start, &mstart) ||
      !__builtin_add_overflow(mstart, length, &mend) ||
      ((intptr_t)zbase <= mstart && mstart <= (intptr_t)zbase + zmapsize) ||
      ((intptr_t)zbase <= mend && mend <= (intptr_t)zbase + zmapsize)) {
    return (void *)mstart;
  } else {
    abort();
  }
}

static bool IsConnectionClose(struct Client *c) {
  int n;
  char *p;
  p = c->inbuf + c->req.headers[kHttpConnection].a;
  n = c->req.headers[kHttpConnection].b - c->req.headers[kHttpConnection].a;
  return n == 5 && memcmp(p, "close", 5) == 0;
}

static char *AppendCrlf(char *p) {
  return STPCPY(p, "\r\n");
}

#define AppendStatus(p, c, s) AppendStatus(p, c, s, sizeof(s) - 1)
static char *(AppendStatus)(char *p, int c, const char *s, size_t n) {
  if (legacyhttp) {
    p = STPCPY(p, "HTTP/1.0 ");
  } else {
    p = STPCPY(p, "HTTP/1.1 ");
  }
  p += uint64toarray_radix10(c, p);
  *p++ = ' ';
  p = mempcpy(p, s, n);
  return AppendCrlf(p);
}

static void UpdateCurrentDate(long double now) {
  int64_t t;
  struct tm tm;
  t = nowish = now;
  gmtime_r(&t, &tm);
  FormatHttpDateTime(currentdate, &tm);
}

static char *AppendDate(char *p) {
  p = AppendHeaderName(p, "Date");
  p = mempcpy(p, currentdate, 29);
  return AppendCrlf(p);
}

static char *AppendLastModified(char *p, const char *s) {
  p = AppendHeaderName(p, "Last-Modified");
  p = mempcpy(p, s, 29);
  return AppendCrlf(p);
}

static char *AppendServer(char *p) {
  const char *s;
  if (*(s = firstnonnull(serverheader, DEFAULT_SERVER))) {
    p = AppendHeaderName(p, "Server");
    p = stpcpy(p, s);
    p = AppendCrlf(p);
  }
  return p;
}

static char *AppendConnectionClose(char *p) {
  p = AppendHeaderName(p, "Connection");
  p = STPCPY(p, "close");
  return AppendCrlf(p);
}

static char *AppendAcceptRangesBytes(char *p) {
  p = AppendHeaderName(p, "Accept-Ranges");
  p = STPCPY(p, "bytes");
  return AppendCrlf(p);
}

static char *AppendNosniff(char *p) {
  p = AppendHeaderName(p, "X-Content-Type-Options");
  p = STPCPY(p, "nosniff");
  return AppendCrlf(p);
}

static char *AppendContentType(char *p, const char *ct) {
  p = AppendHeaderName(p, "Content-Type");
  p = stpcpy(p, ct);
  if (startswith(ct, "text/")) {
    p = STPCPY(p, "; charset=utf-8");
  }
  return AppendCrlf(p);
}

static char *AppendContentTypeTextPlain(char *p) {
  return AppendContentType(p, "text/plain");
}

static char *AppendExpires(char *p, int64_t t) {
  struct tm tm;
  gmtime_r(&t, &tm);
  p = AppendHeaderName(p, "Expires");
  p = FormatHttpDateTime(p, &tm);
  return AppendCrlf(p);
}

static char *AppendVaryContentEncoding(char *p) {
  p = AppendHeaderName(p, "Vary");
  p = STPCPY(p, "Accept-Encoding");
  return AppendCrlf(p);
}

static char *AppendCache(char *p) {
  int x;
  x = cacheseconds;
  if (!x) return p;
  x = MAX(0, x);
  p = AppendHeaderName(p, "Cache-Control");
  p = STPCPY(p, "max-age=");
  p += uint64toarray_radix10(x, p);
  if (x) p = STPCPY(p, ", public");
  p = AppendCrlf(p);
  return AppendExpires(p, nowish + cacheseconds);
}

static char *AppendContentLength(char *p, size_t n) {
  p = AppendHeaderName(p, "Content-Length");
  p += uint64toarray_radix10(n, p);
  return AppendCrlf(p);
}

static char *AppendContentRange(char *p, long rangestart, long rangelength,
                                long contentlength) {
  long endrange;
  CHECK_GE(rangestart + rangelength, rangestart);
  CHECK_LE(rangestart + rangelength, contentlength);
  if (__builtin_add_overflow(rangestart, rangelength, &endrange)) abort();
  p = AppendHeaderName(p, "Content-Range");
  p = STPCPY(p, "bytes ");
  p += uint64toarray_radix10(rangestart, p);
  *p++ = '-';
  p += uint64toarray_radix10(endrange, p);
  *p++ = '/';
  p += uint64toarray_radix10(contentlength, p);
  return AppendCrlf(p);
}

static char *AppendContentEncodingGzip(char *p) {
  p = AppendHeaderName(p, "Content-Encoding");
  p = STPCPY(p, "gzip");
  return AppendCrlf(p);
}

static char *AppendRedirect(struct Client *c, char *p, const char *s) {
  VERBOSEF("%s %s %.*s redirect %s", c->addrstr, kHttpMethod[c->req.method],
           c->req.uri.b - c->req.uri.a, c->inbuf + c->req.uri.a, s);
  p = AppendStatus(p, 302, "Temporary Redirect");
  p = AppendHeaderName(p, "Location");
  p = STPCPY(p, s);
  return AppendCrlf(p);
}

static bool InflateTiny(uint8_t *outbuf, size_t outsize, const uint8_t *inbuf,
                        size_t insize) {
  struct DeflateState ds;
  return undeflate(outbuf, outsize, inbuf, insize, &ds) != -1;
}

static bool InflateZlib(uint8_t *outbuf, size_t outsize, const uint8_t *inbuf,
                        size_t insize) {
  bool ok;
  z_stream zs;
  ok = false;
  zs.next_in = inbuf;
  zs.avail_in = insize;
  zs.total_in = insize;
  zs.next_out = outbuf;
  zs.avail_out = outsize;
  zs.total_out = outsize;
  zs.zfree = Z_NULL;
  zs.zalloc = Z_NULL;
  if (inflateInit2(&zs, -MAX_WBITS) == Z_OK) {
    switch (inflate(&zs, Z_NO_FLUSH)) {
      case Z_STREAM_END:
        ok = true;
        break;
      case Z_MEM_ERROR:
        WARNF("Z_MEM_ERROR");
        break;
      case Z_DATA_ERROR:
        WARNF("Z_DATA_ERROR");
        break;
      case Z_NEED_DICT:
        WARNF("Z_NEED_DICT");
        break;
      default:
        abort();
    }
    inflateEnd(&zs);
  }
  return ok;
}

static bool Inflate(uint8_t *outbuf, size_t outsize, const uint8_t *inbuf,
                    size_t insize) {
  if (IsTiny()) {
    return InflateTiny(outbuf, outsize, inbuf, insize);
  } else {
    return InflateZlib(outbuf, outsize, inbuf, insize);
  }
}

static void LogRequestLatency(struct Client *c) {
  long double now = nowl();
  LOGF("%s latency req %,16ldns conn %,16ldns", c->addrstr,
       (long)((now - c->startrequest) * 1e9),
       (long)((now - c->startconnection) * 1e9));
}

bool HandleRequest(struct Client *c) {
  char *p;
  int msgsize;
  bool gzipped;
  void *content;
  size_t pathlen;
  struct Asset *a;
  unsigned version;
  const char *path, *location;
  long lf, contentlength, actualcontentlength, rangestart, rangelength;
  p = c->outbuf;
  content = "";
  gzipped = false;
  contentlength = -1;
  c->close = false;
  if ((msgsize = ParseHttpRequest(&c->req, c->inbuf, c->insize)) != -1) {
    if (!msgsize) return false;
    if (logmessages) {
      LOGF("%s received %,d byte message\n%.*s", c->addrstr, c->req.length,
           c->req.length - 4, c->inbuf);
    }
    version = GetHttpVersion(c);
    if (version < 101) c->close = true, legacyhttp = true;
    if (version <= 101) {
      if (IsConnectionClose(c)) c->close = true;
      path = c->inbuf + c->req.uri.a;
      pathlen = c->req.uri.b - c->req.uri.a;
      if (c->req.method == kHttpGet || c->req.method == kHttpHead) {
        if (ParseContentLength(&c->req, c->inbuf)) c->close = true;
        VERBOSEF(
            "%s %s %.*s referer %.*s", c->addrstr, kHttpMethod[c->req.method],
            pathlen, path,
            c->req.headers[kHttpReferer].b - c->req.headers[kHttpReferer].a,
            c->inbuf + c->req.headers[kHttpReferer].a);
        if ((location = LookupRedirect(path, pathlen))) {
          p = AppendRedirect(c, p, DEFAULT_PATH);
        } else if ((a = FindFile(path, pathlen))) {
          if (IsNotModified(c, a)) {
            VERBOSEF("%s %s %.*s not modified", c->addrstr,
                     kHttpMethod[c->req.method], pathlen, path);
            p = AppendStatus(p, 304, "Not Modified");
          } else {
            lf = ZIP_CFILE_OFFSET(zbase + a->cf);
            content = ZIP_LFILE_CONTENT(zbase + lf);
            contentlength = ZIP_CFILE_COMPRESSEDSIZE(zbase + a->cf);
            if (IsCompressed(a)) {
              if (memmem(c->inbuf + c->req.headers[kHttpAcceptEncoding].a,
                         c->req.headers[kHttpAcceptEncoding].b -
                             c->req.headers[kHttpAcceptEncoding].a,
                         "gzip", 4)) {
                gzipped = true;
                memcpy(c->gzip_footer + 0, zbase + a->cf + kZipCfileOffsetCrc32,
                       4);
                memcpy(c->gzip_footer + 4,
                       zbase + a->cf + kZipCfileOffsetUncompressedsize, 4);
                p = AppendStatus(p, 200, "OK");
                p = AppendContentEncodingGzip(p);
              } else if (Inflate(
                             (content = gc(xmalloc(
                                  ZIP_CFILE_UNCOMPRESSEDSIZE(zbase + a->cf)))),
                             (contentlength =
                                  ZIP_CFILE_UNCOMPRESSEDSIZE(zbase + a->cf)),
                             ZIP_LFILE_CONTENT(zbase + lf),
                             ZIP_CFILE_COMPRESSEDSIZE(zbase + a->cf))) {
                p = AppendStatus(p, 200, "OK");
              } else {
                WARNF("%s %s %.*s internal server error", c->addrstr,
                      kHttpMethod[c->req.method], pathlen, path);
                p = AppendStatus(p, 500, "Internal Server Error");
                content = "Internal Server Error\r\n";
                contentlength = -1;
              }
            } else if (HasHeader(c, kHttpRange)) {
              if (ParseHttpRange(c->inbuf + c->req.headers[kHttpRange].a,
                                 c->req.headers[kHttpRange].b -
                                     c->req.headers[kHttpRange].a,
                                 contentlength, &rangestart, &rangelength)) {
                p = AppendStatus(p, 206, "Partial Content");
                p = AppendContentRange(p, rangestart, rangelength,
                                       contentlength);
                content = AddRange(content, rangestart, rangelength);
                contentlength = rangelength;
              } else {
                WARNF(
                    "%s %s %.*s bad range %`'.*s", c->addrstr,
                    kHttpMethod[c->req.method], pathlen, path,
                    c->req.headers[kHttpRange].b - c->req.headers[kHttpRange].a,
                    c->inbuf + c->req.headers[kHttpRange].a);
                p = AppendStatus(p, 416, "Range Not Satisfiable");
                p = AppendContentRange(p, rangestart, rangelength,
                                       contentlength);
                content = "";
                contentlength = 0;
              }
            } else {
              p = AppendStatus(p, 200, "OK");
            }
          }
          p = AppendLastModified(p, a->lastmodifiedstr);
          p = AppendContentType(p, GetContentType(path, pathlen));
          p = AppendCache(p);
          if (!IsCompressed(a)) {
            p = AppendAcceptRangesBytes(p);
          } else {
            p = AppendVaryContentEncoding(p);
          }
        } else {
          WARNF("%s %s %.*s not found", c->addrstr, kHttpMethod[c->req.method],
                pathlen, path);
          p = AppendStatus(p, 404, "Not Found");
          p = AppendContentTypeTextPlain(p);
          content = "Not Found\r\n";
        }
      } else {
        WARNF("%s %s %.*s method not allowed", c->addrstr,
              kHttpMethod[c->req.method], pathlen, path);
        p = AppendStatus(p, 405, "method not allowed");
        p = AppendContentTypeTextPlain(p);
        content = "Method Not Allowed\r\n";
        c->close = true;
      }
    } else {
      WARNF("%s http version not supported %`'.*s", c->addrstr,
            c->req.version.b - c->req.version.a, c->inbuf + c->req.version.a);
      p = AppendStatus(p, 505, "HTTP Version Not Supported");
      p = AppendContentTypeTextPlain(p);
      content = "HTTP Version Not Supported\r\n";
      c->close = true;
    }
  } else {
    WARNF("%s parse error %s", c->addrstr, strerror(errno));
    p = AppendStatus(p, 400, "Bad Request");
    p = AppendContentTypeTextPlain(p);
    content = "Bad Request\r\n";
    c->close = true;
  }
  p = AppendDate(p);
  p = AppendNosniff(p);
  p = AppendServer(p);
  if (c->close) p = AppendConnectionClose(p);
  if (contentlength == -1) contentlength = strlen(content);
  actualcontentlength = contentlength;
  if (gzipped) {
    actualcontentlength += sizeof(kGzipHeader) + sizeof(c->gzip_footer);
  }
  p = AppendContentLength(p, actualcontentlength);
  p = AppendCrlf(p);
  if (logmessages) {
    LOGF("%s sending %,d byte message\n%.*s", c->addrstr, p - c->outbuf,
         p - c->outbuf - 4, c->outbuf);
  }
  CHECK_LT(p, c->outbuf + sizeof(c->outbuf));
  c->iovlen = 0;
  c->iov[c->iovlen].iov_base = c->outbuf;
  c->iov[c->iovlen].iov_len = p - c->outbuf;
  ++c->iovlen;
  if (c->req.method != kHttpHead) {
    if (gzipped) {
      c->iov[c->iovlen].iov_base = kGzipHeader;
      c->iov[c->iovlen].iov_len = sizeof(kGzipHeader);
      ++c->iovlen;
    }
    c->iov[c->iovlen].iov_base = content;
    c->iov[c->iovlen].iov_len = contentlength;
    ++c->iovlen;
    if (gzipped) {
      c->iov[c->iovlen].iov_base = c->gzip_footer;
      c->iov[c->iovlen].iov_len = sizeof(c->gzip_footer);
      ++c->iovlen;
    }
  }
  /* LogRequestLatency(); */
  if (WriteSome(c->fd, c->iov, c->iovlen) == -1) {
    VERBOSEF("%s send error %s", c->addrstr, strerror(errno));
    c->close = true;
    c->insize = 0;
    c->iovlen = 0;
  }
  if (!c->close) {
    memcpy(c->inbuf, c->inbuf + msgsize, c->insize - msgsize);
    c->insize -= msgsize;
  }
  return true;
}

static void DeleteClient(struct Client *c) {
  DEBUGF("%s close", c->addrstr);
  CHECK_NE(-1, epoll_ctl(epfd, EPOLL_CTL_DEL, c->fd, NULL));
  LOGIFNEG1(close(c->fd));
  if (c->next) {
    c->next->prev = c->prev;
  }
  if (c->prev) {
    c->prev->next = c->next;
  } else {
    clients = c->next;
  }
  memset(c, 0, sizeof(*c));
  free(c);
}

static void ProcessInbuf(struct Client *c) {
  long double now;
  for (;;) {
    c->startrequest = now = nowl();
    if (now - nowish > 1) UpdateCurrentDate(now);
    if (HandleRequest(c)) {
      if (GetIovSize(c->iov, c->iovlen)) {
        CHECK_NE(-1, epoll_ctl(epfd, EPOLL_CTL_MOD, c->fd,
                               &(struct epoll_event){EPOLLOUT, {c}}));
      } else {
        if (c->close && !c->insize) {
          DeleteClient(c);
        } else if (c->insize) {
          continue;
        }
      }
    } else if (c->insize == sizeof(c->inbuf)) {
      WARNF("%s http message too big", c->addrstr);
      DeleteClient(c);
    }
    break;
  }
}

static void ProcessRead(struct Client *c) {
  char *p;
  ssize_t rc;
  rc = read(c->fd, c->inbuf + c->insize, sizeof(c->inbuf) - c->insize);
  if (rc != -1) {
    if (rc) {
      c->insize += (size_t)rc;
      ProcessInbuf(c);
    } else {
      DeleteClient(c);
    }
  } else if (errno != EINTR) {
    if (errno == ECONNRESET) {
      DEBUGF("%s reset", c->addrstr);
    } else {
      WARNF("%s recv error %s", c->addrstr, strerror(errno));
    }
    DeleteClient(c);
  }
}

static void ProcessWrite(struct Client *c) {
  if (WriteSome(c->fd, c->iov, c->iovlen) != -1) {
    if (!GetIovSize(c->iov, c->iovlen)) {
      if (!c->close) {
        CHECK_NE(-1, epoll_ctl(epfd, EPOLL_CTL_MOD, c->fd,
                               &(struct epoll_event){EPOLLIN, {c}}));
        ProcessInbuf(c);
      } else {
        DeleteClient(c);
      }
    }
  } else {
    VERBOSEF("%s send error %s", c->addrstr, strerror(errno));
    DeleteClient(c);
  }
}

static void ProcessConnect(void) {
  int fd;
  struct Client *c;
  c = xmalloc(sizeof(struct Client));
  c->addrsize = sizeof(c->addr);
  if ((fd = accept(server, &c->addr, &c->addrsize)) != -1) {
    c->fd = fd;
    c->prev = NULL;
    c->next = clients;
    c->insize = 0;
    c->iovlen = 0;
    c->close = false;
    c->startconnection = nowl();
    c->startrequest = c->startconnection;
    DescribeAddress(c->addrstr, &c->addr);
    CHECK_NE(-1, epoll_ctl(epfd, EPOLL_CTL_ADD, c->fd,
                           &(struct epoll_event){EPOLLIN, {c}}));
    if (clients) clients->prev = c;
    clients = c;
    DEBUGF("%s accept", c->addrstr);
  } else if (errno != EINTR) {
    FATALF("%s accept error %s", serveraddrstr, strerror(errno));
    free(c);
  }
}

static void HandleSignals(void) {
  if (invalidated) {
    if (OpenZip(programfile)) {
      LOGF("%s reindexed zip", serveraddrstr);
    } else {
      WARNF("%s reindexing failed", serveraddrstr);
    }
    invalidated = false;
  }
  if (heartbeat | notimer) {
    UpdateCurrentDate(nowl());
    heartbeat = false;
  }
}

static void ProcessEvents(void) {
  int i, j, n;
  struct epoll_event events[16];
  for (;;) {
    if (terminated) {
      while (clients) {
        if (!clients->insize) {
          DeleteClient(clients);
        } else {
          break;
        }
      }
      if (!clients) break;
    }
    if ((n = epoll_wait(epfd, events, ARRAYLEN(events), 200)) != -1) {
      for (i = 0; i < n; ++i) {
        if (!events[i].data.ptr) {
          CHECK(events[i].events & EPOLLIN);
          ProcessConnect();
        } else if (events[i].events & (EPOLLIN | EPOLLERR | EPOLLHUP)) {
          ProcessRead(events[i].data.ptr);
        } else if (events[i].events & EPOLLOUT) {
          ProcessWrite(events[i].data.ptr);
        }
        HandleSignals();
      }
    } else {
      CHECK_EQ(EINTR, errno);
      HandleSignals();
    }
  }
}

static void TuneServerSocket(void) {
  int yes = 1;
  LOGIFNEG1(setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)));
  LOGIFNEG1(setsockopt(server, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes)));
  LOGIFNEG1(setsockopt(server, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes)));
  LOGIFNEG1(setsockopt(server, IPPROTO_TCP, TCP_FASTOPEN, &yes, sizeof(yes)));
  LOGIFNEG1(setsockopt(server, IPPROTO_TCP, TCP_QUICKACK, &yes, sizeof(yes)));
}

void GreenBean(void) {
  gmtoff = GetGmtOffset();
  programfile = (const char *)getauxval(AT_EXECFN);
  CHECK(OpenZip(programfile));
  xsigaction(SIGINT, OnTerminate, 0, 0, 0);
  xsigaction(SIGHUP, OnTerminate, 0, 0, 0);
  xsigaction(SIGTERM, OnTerminate, 0, 0, 0);
  xsigaction(SIGCHLD, SIG_IGN, 0, 0, 0);
  xsigaction(SIGPIPE, SIG_IGN, 0, 0, 0);
  xsigaction(SIGUSR1, OnHup, 0, 0, 0);
  xsigaction(SIGALRM, OnAlarm, 0, 0, 0);
  if (setitimer(ITIMER_REAL, &kHeartbeat, NULL) == -1) notimer = true;
  CHECK_NE(-1, (epfd = epoll_create1(0)));
  CHECK_NE(-1, (server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)));
  TuneServerSocket();
  CHECK_NE(-1, bind(server, &serveraddr, sizeof(serveraddr)));
  CHECK_NE(-1, listen(server, 10));
  CHECK_NE(-1, epoll_ctl(epfd, EPOLL_CTL_ADD, server,
                         &(struct epoll_event){EPOLLIN}));
  DescribeAddress(serveraddrstr, &serveraddr);
  if (daemonize) Daemonize();
  VERBOSEF("%s listen", serveraddrstr);
  heartbeat = true;
  ProcessEvents();
  CHECK_NE(-1, epoll_ctl(epfd, EPOLL_CTL_DEL, server, NULL));
  VERBOSEF("%s terminated", serveraddrstr);
  LOGIFNEG1(close(server));
  LOGIFNEG1(close(epfd));
}

int main(int argc, char *argv[]) {
  showcrashreports();
  GetOpts(argc, argv);
  GreenBean();
  return 0;
}
