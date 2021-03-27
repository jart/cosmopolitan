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
#include "libc/bits/bswap.h"
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/weirdtypes.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/fmt.h"
#include "libc/fmt/itoa.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/rand/rand.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/thompike.h"
#include "libc/str/undeflate.h"
#include "libc/str/utf16.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/clock.h"
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
#include "libc/sysv/consts/w.h"
#include "libc/sysv/errfuns.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "libc/zip.h"
#include "libc/zipos/zipos.internal.h"
#include "net/http/escape.h"
#include "net/http/http.h"
#include "third_party/getopt/getopt.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/ltests.h"
#include "third_party/lua/lua.h"
#include "third_party/lua/lualib.h"
#include "third_party/zlib/zlib.h"

#define USAGE \
  " [-hvdsm] [-p PORT]\n\
\n\
DESCRIPTION\n\
\n\
  redbean - single-file distributable web server\n\
\n\
FLAGS\n\
\n\
  -h        help\n\
  -v        verbosity\n\
  -d        daemonize\n\
  -u        uniprocess\n\
  -z        print port\n\
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
  - HTTP v0.9\n\
  - HTTP v1.0\n\
  - HTTP v1.1\n\
  - Lua v5.4.x\n\
  - Gzip Content-Encoding\n\
  - Range / Content-Range\n\
  - Last-Modified / If-Modified-Since\n\
\n\
USAGE\n\
\n\
  This executable is also a ZIP file that contains static assets.\n\
\n\
    unzip -vl redbean.com  # shows listing of zip contents\n\
\n\
  Audio video content should not be compressed in your ZIP files.\n\
  Uncompressed assets enable browsers to send Range HTTP request.\n\
  On the other hand compressed assets are best for gzip encoding.\n\
\n\
    zip redbean.com index.html    # adds file\n\
    zip -0 redbean.com video.mp4  # adds without compression\n\
\n\
  You can run redbean interactively in your terminal as follows:\n\
\n\
    redbean.com -vv\n\
    CTRL-C                        # 1x: graceful shutdown\n\
    CTRL-C                        # 2x: forceful shutdown\n\
\n\
  You can have redbean run as a daemon by doing the following:\n\
\n\
    redbean.com -vv -d -L redbean.log -P redbean.pid\n\
    kill -TERM $(cat redbean.pid) # 1x: graceful shutdown\n\
    kill -TERM $(cat redbean.pid) # 2x: forceful shutdown\n\
\n\
  redbean imposes a 32kb limit on requests to limit the memory of\n\
  connection processes, which grow to whatever number your system\n\
  limits and tcp stack configuration allow. If fork() should fail\n\
  then redbean starts shutting idle connections down.\n\
\n\
  Redirects emit a 307 response unless the location exists in\n\
  the zip directory, in which case it transparently rewrites.\n\
\n\
\n"

#define HASH_LOAD_FACTOR /* 1. / */ 4
#define DEFAULT_PORT     8080

#define AppendHeaderName(p, s) stpcpy(stpcpy(p, s), ": ")

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

static const char kHexToInt[256] = {
    0, 0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x00
    0, 0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x10
    0, 0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x20
    0, 1,  2,  3,  4,  5,  6,  7, 8, 9, 0, 0, 0, 0, 0, 0,  // 0x30
    0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x40
    0, 0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x50
    0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x60
    0, 0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x70
    0, 0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x80
    0, 0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x90
    0, 0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0xa0
    0, 0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0xb0
    0, 0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0xc0
    0, 0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0xd0
    0, 0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0xe0
    0, 0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0xf0
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

struct Buffer {
  size_t n;
  char *p;
};

struct UriParser {
  int i;
  int c;
  const char *data;
  int size;
  char *p;
  char *q;
};

struct Params {
  size_t n;
  struct Param {
    struct Buffer key;
    struct Buffer val;  // val.n may be SIZE_MAX
  } * p;
};

static struct Freelist {
  size_t n;
  void **p;
} freelist;

static struct Redirects {
  size_t n;
  struct Redirect {
    const char *path;
    size_t pathlen;
    const char *location;
  } * p;
} redirects;

static struct Assets {
  uint32_t n;
  struct Asset {
    uint32_t lf;
    uint32_t hash;
    int64_t lastmodified;
    char *lastmodifiedstr;
  } * p;
} assets;

static bool killed;
static bool istext;
static bool zombied;
static bool gzipped;
static bool branded;
static bool meltdown;
static bool heartless;
static bool printport;
static bool heartbeat;
static bool daemonize;
static bool terminated;
static bool uniprocess;
static bool invalidated;
static bool logmessages;
static bool checkedmethod;
static bool connectionclose;
static bool keyboardinterrupt;

static int frags;
static int gmtoff;
static int server;
static int client;
static int daemonuid;
static int daemongid;
static int statuscode;
static unsigned httpversion;
static uint32_t clientaddrsize;

static void *zmap;
static lua_State *L;
static void *content;
static uint8_t *zbase;
static uint8_t *zcdir;
static size_t msgsize;
static size_t amtread;
static size_t zmapsize;
static char *luaheaderp;
static const char *pidpath;
static const char *logpath;
static int64_t programtime;
static size_t contentlength;
static int64_t cacheseconds;
static uint8_t gzip_footer[8];
static const char *serverheader;

static struct Buffer body;
static struct Buffer inbuf;
static struct Buffer hdrbuf;
static struct Buffer outbuf;
static struct Buffer uripath;
static struct Params uriparams;
static struct Buffer urifragment;

static long double nowish;
static long double startread;
static long double startrequest;
static long double startconnection;
static struct sockaddr_in serveraddr;
static struct sockaddr_in clientaddr;

static struct HttpRequest req;
static char currentdate[32];
static char clientaddrstr[32];
static char serveraddrstr[32];

static void OnChld(void) {
  zombied = true;
}

static void OnAlrm(void) {
  heartbeat = true;
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

static int ComparePaths(const char *a, size_t n, const char *b, size_t m) {
  int c;
  if ((c = memcmp(a, b, MIN(n, m)))) return c;
  if (n < m) return -1;
  if (n > m) return +1;
  return 0;
}

static long FindRedirect(const char *path, size_t n) {
  int c, m, l, r, z;
  l = 0;
  r = redirects.n - 1;
  while (l <= r) {
    m = (l + r) >> 1;
    c = ComparePaths(redirects.p[m].path, redirects.p[m].pathlen, path, n);
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

static void AddRedirect(const char *arg) {
  long i, j;
  const char *p;
  struct Redirect r;
  CHECK_NOTNULL((p = strchr(arg, '=')));
  CHECK_GT(p - arg, 0);
  r.path = arg;
  r.pathlen = p - arg;
  r.location = p + 1;
  if ((i = FindRedirect(r.path, r.pathlen)) != -1) {
    redirects.p[i] = r;
  } else {
    i = redirects.n;
    redirects.p = xrealloc(redirects.p, (i + 1) * sizeof(*redirects.p));
    for (j = i; j > 0; --j) {
      if (ComparePaths(r.path, r.pathlen, redirects.p[j - 1].path,
                       redirects.p[j - 1].pathlen) < 0) {
        redirects.p[j] = redirects.p[j - 1];
      } else {
        break;
      }
    }
    redirects.p[j] = r;
    ++redirects.n;
  }
}

static int CompareInts(const uint64_t x, uint64_t y) {
  return x > y ? 1 : x < y ? -1 : 0;
}

static const char *FindContentType(uint64_t ext) {
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
  return NULL;
}

static const char *GetContentType(const char *path, size_t n) {
  size_t i;
  uint64_t x;
  const char *p, *r;
  if ((p = memrchr(path, '.', n))) {
    for (x = 0, i = n; i-- > p + 1 - path;) {
      x <<= 8;
      x |= path[i] & 0xFF;
    }
    if ((r = FindContentType(bswap_64(x)))) {
      return r;
    }
  }
  return "application/octet-stream";
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

static void SetDefaults(void) {
  cacheseconds = -1;
  serverheader = "redbean/0.2";
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(DEFAULT_PORT);
  serveraddr.sin_addr.s_addr = INADDR_ANY;
  AddRedirect("/=/tool/net/redbean.html");
  AddRedirect("/index.html=/tool/net/redbean.html");
  AddRedirect("/favicon.ico=/tool/net/redbean.ico");
}

static wontreturn void PrintUsage(FILE *f, int rc) {
  fprintf(f, "SYNOPSIS\n\n  %s%s", program_invocation_name, USAGE);
  exit(rc);
}

static void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "zhduvml:p:w:r:c:L:P:U:G:B:")) != -1) {
    switch (opt) {
      case 'v':
        __log_level++;
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
      case 'z':
        printport = true;
        break;
      case 'r':
        AddRedirect(optarg);
        break;
      case 'c':
        cacheseconds = strtol(optarg, NULL, 0);
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
        daemongid = atoi(optarg);
        break;
      case 'h':
        PrintUsage(stdout, EXIT_SUCCESS);
      default:
        PrintUsage(stderr, EX_USAGE);
    }
  }
  if (logpath) {
    CHECK_NOTNULL(freopen(logpath, "a", stderr));
  }
}

static void Daemonize(void) {
  char ibuf[21];
  int i, fd, pid;
  for (i = 0; i < 128; ++i) close(i);
  if ((pid = fork()) > 0) exit(0);
  setsid();
  if ((pid = fork()) > 0) _exit(0);
  umask(0);
  if (pidpath) {
    fd = open(pidpath, O_CREAT | O_EXCL | O_WRONLY, 0644);
    write(fd, ibuf, uint64toarray_radix10(getpid(), ibuf));
    close(fd);
  }
  if (!logpath) logpath = "/dev/null";
  freopen("/dev/null", "r", stdin);
  freopen(logpath, "a", stdout);
  freopen(logpath, "a", stderr);
  LOGIFNEG1(setuid(daemonuid));
  LOGIFNEG1(setgid(daemongid));
}

static void OnWorkerExit(int pid, int ws) {
  if (WIFEXITED(ws)) {
    if (WEXITSTATUS(ws)) {
      WARNF("worker %d exited with %d", pid, WEXITSTATUS(ws));
    } else {
      DEBUGF("worker %d exited", pid);
    }
  } else {
    WARNF("worker %d terminated with %s", pid, strsignal(WTERMSIG(ws)));
  }
}

static void WaitAll(void) {
  int ws, pid;
  for (;;) {
    if ((pid = wait(&ws)) != -1) {
      OnWorkerExit(pid, ws);
    } else {
      if (errno == ECHILD) break;
      if (errno == EINTR) {
        if (killed) {
          WARNF("%s terminating harder", serveraddrstr);
          LOGIFNEG1(kill(0, SIGTERM));
        }
        continue;
      }
      FATALF("%s wait error %s", serveraddrstr, strerror(errno));
    }
  }
}

static void ReapZombies(void) {
  int ws, pid;
  zombied = false;
  do {
    if ((pid = waitpid(-1, &ws, WNOHANG)) != -1) {
      if (pid) {
        OnWorkerExit(pid, ws);
      } else {
        break;
      }
    } else {
      if (errno == ECHILD) break;
      if (errno == EINTR) continue;
      FATALF("%s wait error %s", serveraddrstr, strerror(errno));
    }
  } while (!terminated);
}

static ssize_t WritevAll(int fd, struct iovec *iov, int iovlen) {
  ssize_t rc;
  size_t wrote;
  do {
    if ((rc = writev(fd, iov, iovlen)) != -1) {
      wrote = rc;
      do {
        if (wrote >= iov->iov_len) {
          wrote -= iov->iov_len;
          ++iov;
          --iovlen;
        } else {
          iov->iov_base = (char *)iov->iov_base + wrote;
          iov->iov_len -= wrote;
          wrote = 0;
        }
      } while (wrote);
    } else if (errno == EINTR) {
      if (killed) return -1;
    } else {
      return -1;
    }
  } while (iovlen);
  return 0;
}

static uint32_t Hash(const void *data, size_t size) {
  uint32_t h;
  h = crc32c(0, data, size);
  if (!h) h = 1;
  return h;
}

static bool HasHeader(int h) {
  return req.headers[h].b > req.headers[h].a;
}

static int CompareHeaderValue(int h, const char *s) {
  return strncmp(s, inbuf.p + req.headers[h].a,
                 req.headers[h].b - req.headers[h].a);
}

static bool ClientAcceptsGzip(void) {
  return httpversion >= 100 &&
         !!memmem(inbuf.p + req.headers[kHttpAcceptEncoding].a,
                  req.headers[kHttpAcceptEncoding].b -
                      req.headers[kHttpAcceptEncoding].a,
                  "gzip", 4);
}

static void UpdateCurrentDate(long double now) {
  int64_t t;
  struct tm tm;
  t = nowish = now;
  gmtime_r(&t, &tm);
  FormatHttpDateTime(currentdate, &tm);
}

static int64_t GetGmtOffset(void) {
  int64_t t;
  struct tm tm;
  t = nowl();
  localtime_r(&t, &tm);
  return tm.tm_gmtoff;
}

static int64_t LocoTimeToZulu(int64_t x) {
  return x - gmtoff;
}

static int64_t GetLastModifiedZip(const uint8_t *cfile) {
  const uint8_t *p, *pe;
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
  return ZIP_LFILE_COMPRESSIONMETHOD(zbase + a->lf) == kZipCompressionDeflate;
}

static bool IsNotModified(struct Asset *a) {
  if (httpversion < 100) return false;
  if (!HasHeader(kHttpIfModifiedSince)) return false;
  return a->lastmodified >=
         ParseHttpDateTime(inbuf.p + req.headers[kHttpIfModifiedSince].a,
                           req.headers[kHttpIfModifiedSince].b -
                               req.headers[kHttpIfModifiedSince].a);
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
        p[i].lf = ZIP_CFILE_OFFSET(base + cf);
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
  fd = -1;
  map = MAP_FAILED;
  if ((fd = open(path, O_RDONLY)) != -1 && fstat(fd, &st) != -1 && st.st_size &&
      (map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) !=
          MAP_FAILED &&
      (cdir = zipfindcentraldir(map, st.st_size)) && IndexAssets(map, cdir)) {
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
  return ok;
}

static struct Asset *FindAsset(const char *path, size_t pathlen) {
  uint32_t i, step, hash;
  hash = Hash(path, pathlen);
  for (step = 0;; ++step) {
    i = (hash + (step * (step + 1)) >> 1) & (assets.n - 1);
    if (!assets.p[i].hash) return NULL;
    if (hash == assets.p[i].hash &&
        pathlen == ZIP_LFILE_NAMESIZE(zbase + assets.p[i].lf) &&
        memcmp(path, ZIP_LFILE_NAME(zbase + assets.p[i].lf), pathlen) == 0) {
      return &assets.p[i];
    }
  }
}

static struct Asset *LocateAsset(const char *path, size_t pathlen) {
  char *path2;
  struct Asset *a;
  if (pathlen && path[0] == '/') ++path, --pathlen;
  if (!(a = FindAsset(path, pathlen)) &&
      (!pathlen || (pathlen && path[pathlen - 1] == '/'))) {
    path2 = xmalloc(pathlen + 10);
    memcpy(path2, path, pathlen);
    memcpy(path2 + pathlen, "index.lua", 9);
    if (!(a = FindAsset(path2, pathlen + 9))) {
      memcpy(path2 + pathlen, "index.html", 10);
      a = FindAsset(path2, pathlen + 10);
    }
    free(path2);
  }
  return a;
}

static void *FreeLater(void *p) {
  if (p) {
    freelist.p = xrealloc(freelist.p, ++freelist.n * sizeof(*freelist.p));
    freelist.p[freelist.n - 1] = p;
  }
  return p;
}

static void CollectGarbage(void) {
  size_t i;
  for (i = 0; i < freelist.n; ++i) {
    free(freelist.p[i]);
  }
  freelist.n = 0;
}

static void EmitUriParamKey(struct UriParser *u) {
  uriparams.p = xrealloc(uriparams.p, ++uriparams.n * sizeof(*uriparams.p));
  uriparams.p[uriparams.n - 1].key.p = u->q;
  uriparams.p[uriparams.n - 1].key.n = u->p - u->q;
  u->q = u->p;
}

static void EmitUriParamVal(struct UriParser *u, bool t) {
  if (!t) {
    if (u->p > u->q) {
      EmitUriParamKey(u);
      uriparams.p[uriparams.n - 1].val.p = NULL;
      uriparams.p[uriparams.n - 1].val.n = SIZE_MAX;
    }
  } else {
    uriparams.p[uriparams.n - 1].val.p = u->q;
    uriparams.p[uriparams.n - 1].val.n = u->p - u->q;
    u->q = u->p;
  }
}

static void ParseUriEscape(struct UriParser *u) {
  int a, b;
  a = u->i < u->size ? u->data[u->i++] : 0;
  b = u->i < u->size ? u->data[u->i++] : 0;
  *u->p++ = kHexToInt[a] << 4 | kHexToInt[b];
}

static void ParseUriPath(struct UriParser *u) {
  if (u->c == '/') {
    while (u->i < u->size) {
      u->c = u->data[u->i++];
      if (u->c == '#' || u->c == '?') {
        break;
      } else if (u->c != '%') {
        *u->p++ = u->c;
      } else {
        ParseUriEscape(u);
      }
    }
  }
  uripath.p = u->q;
  uripath.n = u->p - u->q;
  u->q = u->p;
}

static void ParseUriParams(struct UriParser *u) {
  bool t;
  uriparams.n = 0;
  uriparams.p = 0;
  if (u->c == '?') {
    t = false;
    while (u->i < u->size) {
      switch ((u->c = u->data[u->i++])) {
        default:
          *u->p++ = u->c;
          break;
        case '+':
          *u->p++ = ' ';
          break;
        case '%':
          ParseUriEscape(u);
          break;
        case '&':
          EmitUriParamVal(u, t);
          t = false;
          break;
        case '=':
          if (!t) {
            if (u->p > u->q) {
              EmitUriParamKey(u);
              t = true;
            }
          } else {
            *u->p++ = '=';
          }
          break;
        case '#':
          goto EndOfParams;
      }
    }
  EndOfParams:
    EmitUriParamVal(u, t);
    FreeLater(uriparams.p);
  }
}

static void ParseUriFragment(struct UriParser *u) {
  if (u->c == '#') {
    while (u->i < u->size) {
      u->c = u->data[u->i++];
      if (u->c != '%') {
        *u->p++ = u->c;
      } else {
        ParseUriEscape(u);
      }
    }
  }
  urifragment.p = u->q;
  urifragment.n = u->p - u->q;
  u->q = u->p;
}

static bool ParseRequestUri(void) {
  struct UriParser u;
  u.i = 0;
  u.c = '/';
  u.data = inbuf.p + req.uri.a;
  u.size = req.uri.b - req.uri.a;
  if (!u.size || *u.data != '/') return false;
  u.q = u.p = FreeLater(xmalloc(u.size));
  ParseUriPath(&u);
  ParseUriParams(&u);
  ParseUriFragment(&u);
  return u.i == u.size;
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

static bool IsConnectionClose(void) {
  int n;
  char *p;
  p = inbuf.p + req.headers[kHttpConnection].a;
  n = req.headers[kHttpConnection].b - req.headers[kHttpConnection].a;
  return n == 5 && memcmp(p, "close", 5) == 0;
}

static char *AppendCrlf(char *p) {
  p[0] = '\r';
  p[1] = '\n';
  return p + 2;
}

static bool MustNotIncludeMessageBody(void) { /* RFC2616 § 4.4 */
  return req.method == kHttpHead || (100 <= statuscode && statuscode <= 199) ||
         statuscode == 204 || statuscode == 304;
}

static char *SetStatus(int code, const char *reason) {
  char *p;
  statuscode = code;
  p = hdrbuf.p;
  p = stpcpy(p, "HTTP/1.");
  *p++ = httpversion == 100 ? '0' : '1';
  *p++ = ' ';
  p += uint64toarray_radix10(code, p);
  *p++ = ' ';
  p = stpcpy(p, reason);
  return AppendCrlf(p);
}

static char *AppendHeader(char *p, const char *k, const char *v) {
  return AppendCrlf(stpcpy(AppendHeaderName(p, k), v));
}

static char *AppendContentType(char *p, const char *ct) {
  p = AppendHeaderName(p, "Content-Type");
  p = stpcpy(p, ct);
  if (startswith(ct, "text/") && !strchr(ct, ';')) {
    p = stpcpy(p, "; charset=utf-8");
    istext = true;
  }
  return AppendCrlf(p);
}

static char *ServeError(int code, const char *reason) {
  char *p;
  size_t reasonlen;
  reasonlen = strlen(reason);
  p = SetStatus(code, reason);
  p = AppendContentType(p, "text/plain");
  content = FreeLater(xmalloc(reasonlen + 3));
  contentlength = reasonlen + 2;
  stpcpy(stpcpy(content, reason), "\r\n");
  WARNF("%s %s %`'.*s %d %s", clientaddrstr, kHttpMethod[req.method], uripath.n,
        uripath.p, code, reason);
  return p;
}

static char *AppendExpires(char *p, int64_t t) {
  struct tm tm;
  gmtime_r(&t, &tm);
  p = AppendHeaderName(p, "Expires");
  FormatHttpDateTime(p, &tm);
  p += 29;
  return AppendCrlf(p);
}

static char *AppendCache(char *p, int64_t seconds) {
  struct tm tm;
  if (seconds < 0) return p;
  p = AppendHeaderName(p, "Cache-Control");
  p = stpcpy(p, "max-age=");
  p += uint64toarray_radix10(seconds, p);
  if (seconds) p = stpcpy(p, ", public");
  p = AppendCrlf(p);
  return AppendExpires(p, (int64_t)nowish + seconds);
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
  p = stpcpy(p, "bytes ");
  p += uint64toarray_radix10(rangestart, p);
  *p++ = '-';
  p += uint64toarray_radix10(endrange, p);
  *p++ = '/';
  p += uint64toarray_radix10(contentlength, p);
  return AppendCrlf(p);
}

static bool Inflate(uint8_t *dp, size_t dn, const uint8_t *sp, size_t sn) {
  bool ok;
  z_stream zs;
  ok = false;
  zs.next_in = sp;
  zs.avail_in = sn;
  zs.total_in = sn;
  zs.next_out = dp;
  zs.avail_out = dn;
  zs.total_out = dn;
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

static void *Deflate(const void *data, size_t size, size_t *out_size) {
  void *res;
  z_stream zs;
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
  size_t size;
  uint8_t *data;
  size = ZIP_LFILE_UNCOMPRESSEDSIZE(zbase + a->lf);
  data = xmalloc(size + 1);
  if (ZIP_LFILE_COMPRESSIONMETHOD(zbase + a->lf) == kZipCompressionDeflate) {
    CHECK(Inflate(data, size, ZIP_LFILE_CONTENT(zbase + a->lf),
                  ZIP_LFILE_COMPRESSEDSIZE(zbase + a->lf)));
  } else {
    memcpy(data, ZIP_LFILE_CONTENT(zbase + a->lf), size);
  }
  data[size] = '\0';
  if (out_size) *out_size = size;
  return data;
}

static ssize_t Send(struct iovec *iov, int iovlen) {
  ssize_t rc;
  if ((rc = WritevAll(client, iov, iovlen)) == -1) {
    if (errno == ECONNRESET) {
      DEBUGF("%s send reset", clientaddrstr);
    } else {
      WARNF("%s send error %s", clientaddrstr, strerror(errno));
    }
    connectionclose = true;
  }
  return rc;
}

static char *ServeAsset(struct Asset *a, const char *path, size_t pathlen) {
  char *p;
  long rangestart, rangelength;
  if (IsNotModified(a)) {
    DEBUGF("%s %s %`'.*s not modified", clientaddrstr, kHttpMethod[req.method],
           pathlen, path);
    p = SetStatus(304, "Not Modified");
  } else {
    content = ZIP_LFILE_CONTENT(zbase + a->lf);
    contentlength = ZIP_LFILE_COMPRESSEDSIZE(zbase + a->lf);
    if (IsCompressed(a)) {
      if (ClientAcceptsGzip()) {
        gzipped = true;
        memcpy(gzip_footer + 0, zbase + a->lf + kZipLfileOffsetCrc32, 4);
        memcpy(gzip_footer + 4, zbase + a->lf + kZipLfileOffsetUncompressedsize,
               4);
        p = SetStatus(200, "OK");
        p = AppendHeader(p, "Content-Encoding", "gzip");
      } else {
        CHECK(Inflate(
            (content =
                 FreeLater(xmalloc(ZIP_LFILE_UNCOMPRESSEDSIZE(zbase + a->lf)))),
            (contentlength = ZIP_LFILE_UNCOMPRESSEDSIZE(zbase + a->lf)),
            ZIP_LFILE_CONTENT(zbase + a->lf),
            ZIP_LFILE_COMPRESSEDSIZE(zbase + a->lf)));
        p = SetStatus(200, "OK");
      }
    } else if (httpversion >= 101 && HasHeader(kHttpRange)) {
      if (ParseHttpRange(inbuf.p + req.headers[kHttpRange].a,
                         req.headers[kHttpRange].b - req.headers[kHttpRange].a,
                         contentlength, &rangestart, &rangelength)) {
        p = SetStatus(206, "Partial Content");
        p = AppendContentRange(p, rangestart, rangelength, contentlength);
        content = AddRange(content, rangestart, rangelength);
        contentlength = rangelength;
      } else {
        WARNF("%s %s %`'.*s bad range %`'.*s", clientaddrstr,
              kHttpMethod[req.method], pathlen, path,
              req.headers[kHttpRange].b - req.headers[kHttpRange].a,
              inbuf.p + req.headers[kHttpRange].a);
        p = SetStatus(416, "Range Not Satisfiable");
        p = AppendContentRange(p, rangestart, rangelength, contentlength);
        content = "";
        contentlength = 0;
      }
    } else {
      p = SetStatus(200, "OK");
    }
  }
  if (httpversion >= 100) {
    p = AppendHeader(p, "Last-Modified", a->lastmodifiedstr);
    p = AppendContentType(p, GetContentType(path, pathlen));
    if (httpversion >= 101) {
      p = AppendCache(p, cacheseconds);
      if (!IsCompressed(a)) {
        p = AppendHeader(p, "Accept-Ranges", "bytes");
      } else {
        p = AppendHeader(p, "Vary", "Accept-Encoding");
      }
    }
  }
  return p;
}

static bool IsValidFieldName(const char *s, size_t n) {
  size_t i;
  if (!n) return false;
  for (i = 0; i < n; ++i) {
    if (!(0x20 < s[i] && s[i] < 0x7F) || s[i] == ':') {
      return false;
    }
  }
  return true;
}

static bool IsValidFieldContent(const char *s, size_t n) {
  size_t i;
  for (i = 0; i < n; ++i) {
    if (!(0x20 <= s[i] && s[i] < 0x7F)) {
      return false;
    }
  }
  return true;
}

static int LuaServeAsset(lua_State *L) {
  size_t pathlen;
  struct Asset *a;
  const char *path;
  path = luaL_checklstring(L, 1, &pathlen);
  if (!(a = LocateAsset(path, pathlen))) {
    return luaL_argerror(L, 1, "not found");
  }
  luaheaderp = ServeAsset(a, path, pathlen);
  return 0;
}

static int LuaServeError(lua_State *L) {
  int code;
  size_t reasonlen;
  const char *reason;
  code = luaL_checkinteger(L, 1);
  if (!(100 <= code && code <= 999)) {
    return luaL_argerror(L, 1, "bad status code");
  }
  if (lua_isnoneornil(L, 2)) {
    reason = GetHttpReason(code);
  } else {
    reason = lua_tolstring(L, 2, &reasonlen);
    if (reasonlen > 128 || !IsValidFieldContent(reason, reasonlen)) {
      return luaL_argerror(L, 2, "invalid");
    }
  }
  luaheaderp = ServeError(code, reason);
  return 0;
}

static int LuaLoadAsset(lua_State *L) {
  char *data;
  struct Asset *a;
  const char *path;
  size_t size, pathlen;
  path = luaL_checklstring(L, 1, &pathlen);
  if (!(a = LocateAsset(path, pathlen))) {
    return luaL_argerror(L, 1, "not found");
  }
  data = LoadAsset(a, &size);
  lua_pushlstring(L, data, size);
  free(data);
  return 1;
}

static int LuaSetStatus(lua_State *L) {
  int code;
  size_t reasonlen;
  const char *reason;
  code = luaL_checkinteger(L, 1);
  if (!(100 <= code && code <= 999)) {
    return luaL_argerror(L, 1, "bad status code");
  }
  if (lua_isnoneornil(L, 2)) {
    reason = GetHttpReason(code);
  } else {
    reason = lua_tolstring(L, 2, &reasonlen);
    if (reasonlen > 128 || !IsValidFieldContent(reason, reasonlen)) {
      return luaL_argerror(L, 2, "invalid");
    }
  }
  luaheaderp = SetStatus(code, reason);
  return 0;
}

static int LuaGetDate(lua_State *L) {
  lua_pushinteger(L, nowish);
  return 1;
}

static int LuaGetVersion(lua_State *L) {
  lua_pushinteger(L, httpversion);
  return 1;
}

static int LuaGetMethod(lua_State *L) {
  checkedmethod = true;
  lua_pushstring(L, kHttpMethod[req.method]);
  return 1;
}

static int LuaGetPath(lua_State *L) {
  lua_pushlstring(L, uripath.p, uripath.n);
  return 1;
}

static int LuaGetFragment(lua_State *L) {
  lua_pushlstring(L, urifragment.p, urifragment.n);
  return 1;
}

static int LuaGetUri(lua_State *L) {
  lua_pushlstring(L, inbuf.p + req.uri.a, req.uri.b - req.uri.a);
  return 1;
}

static int LuaFormatDate(lua_State *L) {
  int64_t t;
  char buf[30];
  struct tm tm;
  t = luaL_checkinteger(L, 1);
  gmtime_r(&t, &tm);
  lua_pushstring(L, FormatHttpDateTime(buf, &tm));
  return 1;
}

static int LuaParseDate(lua_State *L) {
  size_t n;
  const char *s;
  s = luaL_checklstring(L, 1, &n);
  lua_pushinteger(L, ParseHttpDateTime(s, n));
  return 1;
}

static int LuaGetClientAddr(lua_State *L) {
  lua_pushstring(L, clientaddrstr);
  return 1;
}

static int LuaGetServerAddr(lua_State *L) {
  lua_pushstring(L, serveraddrstr);
  return 1;
}

static int LuaGetHeader(lua_State *L) {
  int h;
  size_t keylen, vallen;
  const char *key, *val;
  key = luaL_checklstring(L, 1, &keylen);
  if ((h = GetHttpHeader(key, keylen)) != -1) {
    val = inbuf.p + req.headers[h].a;
    vallen = req.headers[h].b - req.headers[h].a;
    if (vallen && IsValidFieldContent(val, vallen)) {
      lua_pushlstring(L, val, vallen);
    } else {
      lua_pushnil(L);
    }
    return 1;
  } else {
    return luaL_argerror(L, 1, "unsupported");
  }
}

static int LuaSetHeader(lua_State *L) {
  char *p;
  ssize_t rc;
  const char *key, *val;
  size_t i, keylen, vallen;
  key = luaL_checklstring(L, 1, &keylen);
  val = luaL_checklstring(L, 2, &vallen);
  if (!IsValidFieldName(key, keylen)) {
    return luaL_argerror(L, 1, "invalid");
  }
  if (!IsValidFieldContent(val, vallen)) {
    return luaL_argerror(L, 2, "invalid");
  }
  if (!luaheaderp) {
    p = SetStatus(200, "OK");
  } else {
    while (luaheaderp - hdrbuf.p + keylen + 2 + vallen + 2 + 512 > hdrbuf.n) {
      hdrbuf.n += hdrbuf.n >> 1;
      p = xrealloc(hdrbuf.p, hdrbuf.n);
      luaheaderp = p + (luaheaderp - hdrbuf.p);
      hdrbuf.p = p;
    }
    p = luaheaderp;
  }
  switch (GetHttpHeader(key, keylen)) {
    case kHttpDate:
    case kHttpContentRange:
    case kHttpContentLength:
    case kHttpContentEncoding:
      return luaL_argerror(L, 1, "abstracted");
    case kHttpConnection:
      if (vallen != 5 || memcmp(val, "close", 5)) {
        return luaL_argerror(L, 2, "unsupported");
      }
      connectionclose = true;
      break;
    case kHttpContentType:
      p = AppendContentType(p, val);
      break;
    case kHttpServer:
      branded = true;
      p = AppendHeader(p, key, val);
      break;
    default:
      p = AppendHeader(p, key, val);
      break;
  }
  luaheaderp = p;
  return 0;
}

static int LuaHasParam(lua_State *L) {
  const char *key;
  size_t i, keylen;
  key = luaL_checklstring(L, 1, &keylen);
  for (i = 0; i < uriparams.n; ++i) {
    if (uriparams.p[i].key.n == keylen &&
        !memcmp(uriparams.p[i].key.p, key, keylen)) {
      lua_pushboolean(L, true);
      return 1;
    }
  }
  lua_pushboolean(L, false);
  return 1;
}

static int LuaGetParam(lua_State *L) {
  const char *key;
  size_t i, keylen;
  key = luaL_checklstring(L, 1, &keylen);
  for (i = 0; i < uriparams.n; ++i) {
    if (uriparams.p[i].key.n == keylen &&
        !memcmp(uriparams.p[i].key.p, key, keylen)) {
      if (uriparams.p[i].val.n == SIZE_MAX) break;
      lua_pushlstring(L, uriparams.p[i].val.p, uriparams.p[i].val.n);
      return 1;
    }
  }
  lua_pushnil(L);
  return 1;
}

static int LuaGetParams(lua_State *L) {
  size_t i;
  lua_newtable(L);
  for (i = 0; i < uriparams.n; ++i) {
    lua_newtable(L);
    lua_pushlstring(L, uriparams.p[i].key.p, uriparams.p[i].key.n);
    lua_seti(L, -2, 1);
    if (uriparams.p[i].val.n != SIZE_MAX) {
      lua_pushlstring(L, uriparams.p[i].val.p, uriparams.p[i].val.n);
      lua_seti(L, -2, 2);
    }
    lua_seti(L, -2, i + 1);
  }
  return 1;
}

static int LuaWrite(lua_State *L) {
  int h;
  size_t size;
  const char *data;
  data = luaL_checklstring(L, 1, &size);
  outbuf.p = xrealloc(outbuf.p, outbuf.n + size);
  memcpy(outbuf.p + outbuf.n, data, size);
  outbuf.n += size;
  return 0;
}

static int LuaEscaper(lua_State *L,
                      struct EscapeResult escape(const char *, size_t)) {
  size_t size;
  const char *data;
  struct EscapeResult r;
  data = luaL_checklstring(L, 1, &size);
  r = escape(data, size);
  lua_pushlstring(L, r.data, r.size);
  free(r.data);
  return 1;
}

static int LuaEscapeHtml(lua_State *L) {
  return LuaEscaper(L, EscapeHtml);
}

static int LuaEscapeParam(lua_State *L) {
  return LuaEscaper(L, EscapeUrlParam);
}

static int LuaEscapePath(lua_State *L) {
  return LuaEscaper(L, EscapeUrlPath);
}

static int LuaEscapeSegment(lua_State *L) {
  return LuaEscaper(L, EscapeUrlPathSegment);
}

static int LuaEscapeFragment(lua_State *L) {
  return LuaEscaper(L, EscapeUrlFragment);
}

static int LuaEscapeLiteral(lua_State *L) {
  return LuaEscaper(L, EscapeJsStringLiteral);
}

static void LuaRun(const char *path) {
  struct Asset *a;
  const char *code;
  if ((a = LocateAsset(path, strlen(path)))) {
    code = LoadAsset(a, NULL);
    if (luaL_dostring(L, code) != LUA_OK) {
      WARNF("%s %s", path, lua_tostring(L, -1));
    }
    free(code);
  } else {
    DEBUGF("%s not found", path);
  }
}

static const luaL_Reg kLuaFuncs[] = {
    {"EscapeFragment", LuaEscapeFragment},  //
    {"EscapeHtml", LuaEscapeHtml},          //
    {"EscapeLiteral", LuaEscapeLiteral},    //
    {"EscapeParam", LuaEscapeParam},        //
    {"EscapePath", LuaEscapePath},          //
    {"EscapeSegment", LuaEscapeSegment},    //
    {"FormatDate", LuaFormatDate},          //
    {"GetClientAddr", LuaGetClientAddr},    //
    {"GetDate", LuaGetDate},                //
    {"GetFragment", LuaGetFragment},        //
    {"GetHeader", LuaGetHeader},            //
    {"GetMethod", LuaGetMethod},            //
    {"GetParam", LuaGetParam},              //
    {"GetParams", LuaGetParams},            //
    {"GetPath", LuaGetPath},                //
    {"GetServerAddr", LuaGetServerAddr},    //
    {"GetUri", LuaGetUri},                  //
    {"GetVersion", LuaGetVersion},          //
    {"HasParam", LuaHasParam},              //
    {"LoadAsset", LuaLoadAsset},            //
    {"ParseDate", LuaParseDate},            //
    {"ServeAsset", LuaServeAsset},          //
    {"ServeError", LuaServeError},          //
    {"SetHeader", LuaSetHeader},            //
    {"SetStatus", LuaSetStatus},            //
    {"Write", LuaWrite},                    //
};

static void LuaInit(void) {
  size_t i;
  L = luaL_newstate();
  luaL_openlibs(L);
  for (i = 0; i < ARRAYLEN(kLuaFuncs); ++i) {
    lua_pushcfunction(L, kLuaFuncs[i].func);
    lua_setglobal(L, kLuaFuncs[i].name);
  }
  LuaRun("/tool/net/.init.lua");
}

static void LuaReload(void) {
  LuaRun("/tool/net/.reload.lua");
}

static char *ServeLua(struct Asset *a) {
  char *p;
  outbuf.n = 0;
  luaheaderp = NULL;
  checkedmethod = false;
  if (luaL_dostring(L, FreeLater(LoadAsset(a, NULL))) == LUA_OK) {
    if (!checkedmethod && req.method != kHttpGet && req.method != kHttpHead) {
      return ServeError(405, "Method Not Allowed");
    }
    if (!(p = luaheaderp)) {
      p = SetStatus(200, "OK");
      p = AppendContentType(p, "text/html");
    }
    if (istext && outbuf.n >= 100 && ClientAcceptsGzip()) {
      gzipped = true;
      p = AppendHeader(p, "Content-Encoding", "gzip");
      p = AppendHeader(p, "Vary", "Accept-Encoding");
      WRITE32LE(gzip_footer + 0, crc32_z(0, outbuf.p, outbuf.n));
      WRITE32LE(gzip_footer + 4, outbuf.n);
      content = FreeLater(Deflate(outbuf.p, outbuf.n, &contentlength));
    } else {
      content = outbuf.p;
      contentlength = outbuf.n;
    }
    return p;
  } else {
    WARNF("%s %s", clientaddrstr, lua_tostring(L, -1));
    lua_pop(L, 1); /* remove message */
    connectionclose = true;
    return ServeError(500, "Internal Server Error");
  }
}

static bool IsLua(struct Asset *a) {
  return ZIP_LFILE_NAMESIZE(zbase + a->lf) >= 4 &&
         !memcmp(ZIP_LFILE_NAME(zbase + a->lf) +
                     ZIP_LFILE_NAMESIZE(zbase + a->lf) - 4,
                 ".lua", 4);
}

static char *HandleAsset(struct Asset *a, const char *path, size_t pathlen) {
  char *p;
  if (IsLua(a)) {
    p = ServeLua(a);
  } else if (req.method == kHttpGet || req.method == kHttpHead) {
    p = ServeAsset(a, path, pathlen);
    p = AppendHeader(p, "X-Content-Type-Options", "nosniff");
  } else {
    p = ServeError(405, "Method Not Allowed");
  }
  return p;
}

static char *HandleRedirect(struct Redirect *r) {
  char *p;
  struct Asset *a;
  if ((a = LocateAsset(r->location, strlen(r->location)))) {
    DEBUGF("%s %s %`'.*s rewritten %`'s", clientaddrstr,
           kHttpMethod[req.method], uripath.n, uripath.p, r->location);
    p = HandleAsset(a, r->location, strlen(r->location));
  } else if (httpversion == 9) {
    p = ServeError(505, "HTTP Version Not Supported");
  } else {
    DEBUGF("%s %s %`'.*s redirecting %`'s", clientaddrstr,
           kHttpMethod[req.method], uripath.n, uripath.p, r->location);
    p = SetStatus(307, "Temporary Redirect");
    p = AppendHeader(p, "Location", r->location);
  }
  return p;
}

static ssize_t SendString(const char *s) {
  ssize_t rc;
  for (;;) {
    if ((rc = write(client, s, strlen(s))) != -1 || errno != EINTR) {
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

static void LogClose(const char *reason) {
  if (amtread) {
    WARNF("%s %s with %,ld bytes unprocessed", clientaddrstr, reason, amtread);
  } else {
    DEBUGF("%s %s", clientaddrstr, reason);
  }
}

static const char *DescribeClose(void) {
  if (killed) return "killed";
  if (meltdown) return "meltdown";
  if (terminated) return "terminated";
  if (connectionclose) return "connectionclose";
  return "destroyed";
}

static char *HandleMessage(size_t hdrlen) {
  long r;
  ssize_t cl, rc;
  struct Asset *a;
  size_t got, need;
  httpversion =
      ParseHttpVersion(inbuf.p + req.version.a, req.version.b - req.version.a);
  if (httpversion > 101) {
    return ServeError(505, "HTTP Version Not Supported");
  }
  if (req.method > kHttpPost ||
      (HasHeader(kHttpTransferEncoding) &&
       CompareHeaderValue(kHttpTransferEncoding, "identity"))) {
    return ServeError(501, "Not Implemented");
  }
  if (HasHeader(kHttpExpect) &&
      CompareHeaderValue(kHttpExpect, "100-continue")) {
    return ServeError(417, "Expectation Failed");
  }
  if ((cl = ParseContentLength(inbuf.p + req.headers[kHttpContentLength].a,
                               req.headers[kHttpContentLength].b -
                                   req.headers[kHttpContentLength].a)) == -1) {
    if (HasHeader(kHttpContentLength)) {
      return ServeError(400, "Bad Request");
    } else if (req.method != kHttpGet && req.method != kHttpHead &&
               req.method != kHttpOptions) {
      return ServeError(411, "Length Required");
    } else {
      cl = 0;
    }
  }
  need = hdrlen + cl; /* synchronization is possible */
  if (need > inbuf.n) {
    return ServeError(413, "Payload Too Large");
  }
  if (!CompareHeaderValue(kHttpExpect, "100-continue") && httpversion >= 101) {
    SendContinue();
  }
  while (amtread < need) {
    if (++frags == 64) {
      LogClose("payload fragged");
      return ServeError(408, "Request Timeout");
    }
    if ((rc = read(client, inbuf.p + amtread, inbuf.n - amtread)) != -1) {
      if (!(got = rc)) {
        LogClose("payload disconnect");
        return ServeError(400, "Bad Request");
      }
      amtread += got;
    } else if (errno == ECONNRESET) {
      LogClose("payload reset");
      return ServeError(400, "Bad Request");
    } else if (errno == EINTR) {
      if (killed || ((meltdown || terminated) && nowl() - startread > 1)) {
        LogClose(DescribeClose());
        return ServeError(503, "Service Unavailable");
      }
    } else {
      WARNF("%s payload recv %s", clientaddrstr, strerror(errno));
      return ServeError(500, "Internal Server Error");
    }
  }
  msgsize = need; /* we are now synchronized */
  if (httpversion != 101 || IsConnectionClose()) {
    connectionclose = true;
  }
  if (!ParseRequestUri()) {
    WARNF("%s could not parse request uri %`'.*s", clientaddrstr,
          req.uri.b - req.uri.a, inbuf.p + req.uri.a);
    connectionclose = true;
    return ServeError(400, "Bad Request");
  }
  VERBOSEF("%s %s %`'.*s referrer %`'.*s", clientaddrstr,
           kHttpMethod[req.method], uripath.n, uripath.p,
           req.headers[kHttpReferer].b - req.headers[kHttpReferer].a,
           inbuf.p + req.headers[kHttpReferer].a);
  if ((a = LocateAsset(uripath.p, uripath.n))) {
    return HandleAsset(a, uripath.p, uripath.n);
  } else if ((r = FindRedirect(uripath.p, uripath.n)) != -1) {
    return HandleRedirect(redirects.p + r);
  } else {
    return ServeError(404, "Not Found");
  }
}

static bool HandleRequest(void) {
  int rc;
  char *p;
  int iovlen;
  struct iovec iov[4];
  long actualcontentlength;
  msgsize = 0;
  if ((rc = ParseHttpRequest(&req, inbuf.p, amtread)) != -1) {
    if (!rc) return false;
    p = HandleMessage(rc);
  } else {
    httpversion = 101;
    p = ServeError(400, "Bad Request");
  }
  if (!msgsize) {
    amtread = 0;
    connectionclose = true;
    DEBUGF("%s could not synchronize message stream", clientaddrstr);
  } else if (msgsize < amtread) {
    DEBUGF("%s has %,ld pipelined bytes", clientaddrstr, amtread - msgsize);
    memmove(inbuf.p, inbuf.p + msgsize, amtread - msgsize);
    amtread -= msgsize;
  } else {
    amtread = 0;
  }
  if (httpversion >= 100) {
    p = AppendHeader(p, "Date", currentdate);
    if (!branded) {
      p = AppendHeader(p, "Server", serverheader);
    }
    if (connectionclose) {
      p = AppendHeader(p, "Connection", "close");
    }
    actualcontentlength = contentlength;
    if (gzipped) {
      actualcontentlength += sizeof(kGzipHeader) + sizeof(gzip_footer);
    }
    p = AppendContentLength(p, actualcontentlength);
    p = AppendCrlf(p);
    CHECK_LE(p - hdrbuf.p, hdrbuf.n);
    if (logmessages) {
      LOGF("%s sending %,d byte message\n%.*s", clientaddrstr, p - hdrbuf.p,
           p - hdrbuf.p - 4, hdrbuf.p);
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
  CollectGarbage();
  return true;
}

static void InitRequest(void) {
  frags = 0;
  content = NULL;
  gzipped = false;
  branded = false;
  contentlength = 0;
  InitHttpRequest(&req);
}

static void ProcessRequests(void) {
  ssize_t rc;
  size_t got;
  long double now;
  for (;;) {
    InitRequest();
    startread = nowl();
    for (;;) {
      if (!req.i && amtread && HandleRequest()) break;
      if ((rc = read(client, inbuf.p + amtread, inbuf.n - amtread)) != -1) {
        startrequest = now = nowl();
        if (now - nowish > 1) UpdateCurrentDate(now);
        got = rc;
        amtread += got;
        if (amtread) {
          if (HandleRequest()) {
            break;
          } else if (got) {
            if (++frags == 32) {
              SendTimeout();
              LogClose("fragged");
              return;
            } else {
              DEBUGF("%s fragmented msg %,ld %,ld", clientaddrstr, amtread,
                     got);
            }
          }
        }
        if (!got) {
          LogClose("disconnect");
          return;
        }
      } else if (errno == ECONNRESET) {
        LogClose("reset");
        return;
      } else if (errno != EINTR) {
        WARNF("%s recv msg %s", clientaddrstr, strerror(errno));
        return;
      }
      if (killed || (terminated && !amtread) ||
          (meltdown && (!amtread || nowl() - startread > 1))) {
        if (amtread) SendServiceUnavailable();
        LogClose(DescribeClose());
        return;
      }
    }
    if (connectionclose || killed || ((terminated || meltdown) && !amtread)) {
      LogClose(DescribeClose());
      return;
    }
  }
}

static void ProcessConnection(void) {
  int pid;
  clientaddrsize = sizeof(clientaddr);
  if ((client = accept(server, &clientaddr, &clientaddrsize)) != -1) {
    startconnection = nowl();
    if (uniprocess) {
      pid = -1;
      connectionclose = true;
    } else {
      switch ((pid = fork())) {
        case 0:
          meltdown = false;
          connectionclose = false;
          break;
        case -1:
          WARNF("redbean is entering meltdown mode");
          LOGIFNEG1(kill(0, SIGUSR2));
          SendServiceUnavailable();
          /* fallthrough */
        default:
          close(client);
          return;
      }
    }
    DescribeAddress(clientaddrstr, &clientaddr);
    DEBUGF("%s accept", clientaddrstr);
    ProcessRequests();
    LOGIFNEG1(close(client));
    if (!pid) _exit(0);
  } else if (errno != EINTR) {
    FATALF("%s accept error %s", serveraddrstr, strerror(errno));
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

void RedBean(void) {
  uint32_t addrsize;
  if (IsWindows()) uniprocess = true;
  if (daemonize) Daemonize();
  gmtoff = GetGmtOffset();
  CHECK(OpenZip((const char *)getauxval(AT_EXECFN)));
  xsigaction(SIGINT, OnInt, 0, 0, 0);
  xsigaction(SIGHUP, OnHup, 0, 0, 0);
  xsigaction(SIGTERM, OnTerm, 0, 0, 0);
  xsigaction(SIGCHLD, OnChld, 0, 0, 0);
  xsigaction(SIGUSR1, OnUsr1, 0, 0, 0);
  xsigaction(SIGUSR2, OnUsr2, 0, 0, 0);
  xsigaction(SIGALRM, OnAlrm, 0, 0, 0);
  xsigaction(SIGPIPE, SIG_IGN, 0, 0, 0);
  if (setitimer(ITIMER_REAL, &kHeartbeat, NULL) == -1) {
    heartless = true;
  }
  CHECK_NE(-1, (server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)));
  TuneServerSocket();
  if (bind(server, &serveraddr, sizeof(serveraddr)) == -1) {
    if (errno == EADDRINUSE) {
      fprintf(stderr, "error: address in use\n"
                      "try passing the -p PORT flag\n");
    } else {
      fprintf(stderr, "error: bind() failed: %s\n", strerror(errno));
    }
    exit(1);
  }
  CHECK_NE(-1, listen(server, 10));
  addrsize = sizeof(serveraddr);
  CHECK_NE(-1, getsockname(server, &serveraddr, &addrsize));
  DescribeAddress(serveraddrstr, &serveraddr);
  VERBOSEF("%s listen", serveraddrstr);
  if (printport) {
    printf("%d\n", ntohs(serveraddr.sin_port));
    fflush(stdout);
  }
  UpdateCurrentDate(nowl());
  inbuf.n = 64 * 1024;
  inbuf.p = xvalloc(inbuf.n);
  hdrbuf.n = 4 * 1024;
  hdrbuf.p = xvalloc(hdrbuf.n);
  LuaInit();
  while (!terminated) {
    if (zombied) {
      ReapZombies();
    } else if (invalidated) {
      LuaReload();
      invalidated = false;
    } else if (heartbeat) {
      UpdateCurrentDate(nowl());
      heartbeat = false;
    } else {
      if (heartless) {
        UpdateCurrentDate(nowl());
      }
      ProcessConnection();
    }
  }
  VERBOSEF("%s terminated", serveraddrstr);
  LOGIFNEG1(close(server));
  if (!keyboardinterrupt) {
    if (!killed) {
      terminated = false;
    }
    LOGIFNEG1(kill(0, SIGTERM));
  }
  WaitAll();
}

int main(int argc, char *argv[]) {
  showcrashreports();
  SetDefaults();
  GetOpts(argc, argv);
  RedBean();
  return 0;
}
