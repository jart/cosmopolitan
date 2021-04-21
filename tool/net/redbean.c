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
#include "libc/bits/popcnt.h"
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/calls/struct/rusage.h"
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
#include "libc/mem/fmt.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/bsf.h"
#include "libc/nexgen32e/bsr.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/nt/synchronization.h"
#include "libc/rand/rand.h"
#include "libc/runtime/clktck.h"
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
#include "libc/sysv/consts/mlock.h"
#include "libc/sysv/consts/msync.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/s.h"
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
#include "net/http/base64.h"
#include "net/http/escape.h"
#include "net/http/http.h"
#include "net/http/url.h"
#include "third_party/getopt/getopt.h"
#include "third_party/lua/lauxlib.h"
#include "third_party/lua/ltests.h"
#include "third_party/lua/lua.h"
#include "third_party/lua/lualib.h"
#include "third_party/regex/regex.h"
#include "third_party/zlib/zlib.h"

#define HASH_LOAD_FACTOR /* 1. / */ 4
#define DEFAULT_PORT     8080

#define HeaderData(H)   (inbuf.p + msg.headers[H].a)
#define HeaderLength(H) (msg.headers[H].b - msg.headers[H].a)
#define HeaderEqualCase(H, S) \
  SlicesEqualCase(S, strlen(S), HeaderData(H), HeaderLength(H))

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

static const char *const kIndexPaths[] = {
#ifndef STATIC
    "index.lua",
#endif
    "index.html",
};

static const struct ContentTypeExtension {
  unsigned char ext[8];
  const char *mime;
} kContentTypeExtension[] = {
    {"7z", "application/x-7z-compressed"},     //
    {"S", "text/plain"},                       //
    {"aac", "audio/aac"},                      //
    {"apng", "image/apng"},                    //
    {"avi", "video/x-msvideo"},                //
    {"avif", "image/avif"},                    //
    {"bmp", "image/bmp"},                      //
    {"c", "text/plain"},                       //
    {"cc", "text/plain"},                      //
    {"css", "text/css"},                       //
    {"csv", "text/csv"},                       //
    {"gif", "image/gif"},                      //
    {"h", "text/plain"},                       //
    {"htm", "text/html"},                      //
    {"html", "text/html"},                     //
    {"i", "text/plain"},                       //
    {"ico", "image/vnd.microsoft.icon"},       //
    {"jar", "appliaction/java-archive"},       //
    {"jpeg", "image/jpeg"},                    //
    {"jpg", "image/jpeg"},                     //
    {"js", "application/javascript"},          //
    {"json", "application/json"},              //
    {"m4a", "audio/mpeg"},                     //
    {"markdown", "text/plain"},                //
    {"md", "text/plain"},                      //
    {"mp2", "audio/mpeg"},                     //
    {"mp3", "audio/mpeg"},                     //
    {"mp4", "video/mp4"},                      //
    {"mpeg", "video/mpeg"},                    //
    {"mpg", "video/mpeg"},                     //
    {"oga", "audio/ogg"},                      //
    {"ogg", "application/ogg"},                //
    {"ogv", "video/ogg"},                      //
    {"ogx", "application/ogg"},                //
    {"otf", "font/otf"},                       //
    {"pdf", "application/pdf"},                //
    {"png", "image/png"},                      //
    {"rar", "application/vnd.rar"},            //
    {"rtf", "application/rtf"},                //
    {"s", "text/plain"},                       //
    {"sh", "application/x-sh"},                //
    {"svg", "image/svg+xml"},                  //
    {"swf", "application/x-shockwave-flash"},  //
    {"tar", "application/x-tar"},              //
    {"tiff", "image/tiff"},                    //
    {"ttf", "font/ttf"},                       //
    {"txt", "text/plain"},                     //
    {"wav", "audio/x-wav"},                    //
    {"weba", "audio/webm"},                    //
    {"webm", "video/webm"},                    //
    {"webp", "image/webp"},                    //
    {"woff", "font/woff"},                     //
    {"woff2", "font/woff2"},                   //
    {"xhtml", "application/xhtml+xml"},        //
    {"xls", "application/vnd.ms-excel"},       //
    {"xml", "application/xml"},                //
    {"zip", "application/zip"},                //
};

struct Buffer {
  size_t n;
  char *p;
};

struct Strings {
  size_t n;
  char **p;
};

static struct Freelist {
  size_t n, c;
  void **p;
} freelist;

static struct Redirects {
  size_t n;
  struct Redirect {
    int code;
    const char *path;
    size_t pathlen;
    const char *location;
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
      char *path;
      struct stat st;
    } * file;
  } * p;
} assets;

static struct Shared {
  int workers;
  long requestshandled;
} * shared;

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
static bool logbodies;
static bool terminated;
static bool uniprocess;
static bool invalidated;
static bool logmessages;
static bool checkedmethod;
static bool connectionclose;
static bool keyboardinterrupt;
static bool encouragekeepalive;

static int frags;
static int gmtoff;
static int server;
static int client;
static int daemonuid;
static int daemongid;
static int statuscode;
static int requestshandled;
static uint32_t clientaddrsize;

static lua_State *L;
static size_t zsize;
static char *content;
static uint8_t *cdir;
static uint8_t *zmap;
static size_t hdrsize;
static size_t msgsize;
static size_t amtread;
static char *luaheaderp;
static const char *sauce;
static const char *brand;
static const char *pidpath;
static const char *logpath;
static int64_t programtime;
static size_t contentlength;
static int64_t cacheseconds;
static uint8_t gzip_footer[8];
static const char *serverheader;
static struct Strings stagedirs;
static struct Strings hidepaths;

static struct Buffer inbuf;
static struct Buffer hdrbuf;
static struct Buffer outbuf;

static struct Url url;
static struct HttpRequest msg;

static long double nowish;
static long double startread;
static long double startserver;
static long double lastmeltdown;
static long double startrequest;
static long double startconnection;
static struct sockaddr_in serveraddr;
static struct sockaddr_in clientaddr;

static char currentdate[32];
static char clientaddrstr[32];
static char serveraddrstr[32];

static wontreturn void PrintUsage(FILE *f, int rc) {
  /* clang-format off */
  fprintf(f, "\
SYNOPSIS\n\
\n\
  %s [-hvdsm] [-p PORT] [-- SCRIPTARGS...]\n\
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
  -b        log message bodies\n\
  -k        encourage keep-alive\n\
  -D DIR    serve assets from directory\n\
  -c INT    cache seconds\n\
  -r /X=/Y  redirect X to Y\n\
  -R /X=/Y  rewrite X to Y\n\
  -l ADDR   listen ip [default 0.0.0.0]\n\
  -p PORT   listen port [default 8080]\n\
  -L PATH   log file location\n\
  -P PATH   pid file location\n\
  -U INT    daemon set user id\n\
  -G INT    daemon set group id\n\
  -B STR    changes brand\n\
\n\
FEATURES\n\
\n"
#ifndef STATIC
"  - Lua v5.4\n"
#endif
"  - HTTP v0.9\n\
  - HTTP v1.0\n\
  - HTTP v1.1\n\
  - Content-Encoding\n\
  - Range / Content-Range\n\
  - Last-Modified / If-Modified-Since\n\
\n\
USAGE\n\
\n\
  This executable is also a ZIP file that contains static assets.\n\
  You can run redbean interactively in your terminal as follows:\n\
\n\
    redbean.com -vv               # starts web server\n\
    open http://127.0.0.1:8080/   # shows zip listing page\n\
    CTRL-C                        # 1x: graceful shutdown\n\
    CTRL-C                        # 2x: forceful shutdown\n\
\n\
  You can override the default listing page by adding:\n\
\n"
#ifndef STATIC
"    zip redbean.com index.lua     # lua server pages take priority\n"
#endif
"    zip redbean.com index.html    # default page for directory\n\
\n\
  The listing page only applies to the root directory. However the\n\
  default index page applies to subdirectories too. In order for it\n\
  to work, there needs to be an empty directory entry in the zip.\n\
  That should already be the default practice of your zip editor.\n\
\n\
    wget                     \\\n\
      --mirror               \\\n\
      --convert-links        \\\n\
      --adjust-extension     \\\n\
      --page-requisites      \\\n\
      --no-parent            \\\n\
      --no-if-modified-since \\\n\
      http://a.example/index.html\n\
    zip -r redbean.com a.example/  # default page for directory\n\
\n\
  redbean normalizes the trailing slash for you automatically:\n\
\n\
    $ printf 'GET /a.example HTTP/1.0\\n\\n' | nc 127.0.0.1 8080\n\
    HTTP/1.0 307 Temporary Redirect\n\
    Location: /a.example/\n\
\n\
  Virtual hosting is accomplished this way too. The Host is simply\n\
  prepended to the path, and if it doesn't exist, it gets removed.\n\
\n\
    $ printf 'GET / HTTP/1.1\\nHost:a.example\\n\\n' | nc 127.0.0.1 8080\n\
    HTTP/1.1 200 OK\n\
    Link: <http://127.0.0.1/a.example/index.html>; rel=\"canonical\"\n\
\n\
  If you mirror a lot of websites within your redbean then you can\n\
  actually tell your browser that redbean is your proxy server, in\n\
  which redbean will act as your private version of the Internet.\n\
\n\
    $ printf 'GET http://a.example HTTP/1.0\\n\\n' | nc 127.0.0.1 8080\n\
    HTTP/1.0 200 OK\n\
    Link: <http://127.0.0.1/a.example/index.html>; rel=\"canonical\"\n\
\n\
  redbean will display an error page using the /redbean.png logo\n\
  by default, embedded as a bas64 data uri. You can override the\n\
  custom page for various errors by adding files to the zip root.\n\
\n\
    zip redbean.com 404.html      # custom not found page\n\
\n\
  Audio video content should not be compressed in your ZIP files.\n\
  Uncompressed assets enable browsers to send Range HTTP request.\n\
  On the other hand compressed assets are best for gzip encoding.\n\
\n\
    zip redbean.com index.html    # adds file\n\
    zip -0 redbean.com video.mp4  # adds without compression\n\
\n\
  You can have redbean run as a daemon by doing the following:\n\
\n\
    redbean.com -vv -d -L redbean.log -P redbean.pid\n\
    kill -TERM $(cat redbean.pid) # 1x: graceful shutdown\n\
    kill -TERM $(cat redbean.pid) # 2x: forceful shutdown\n\
\n\
  redbean currently has a 32kb limit on request messages and 64kb\n\
  including the payload. redbean will grow to whatever the system\n\
  limits allow. Should fork() or accept() fail redbean will react\n\
  by going into \"meltdown mode\" which closes lingering workers.\n\
  You can trigger this at any time using:\n\
\n\
    kill -USR2 $(cat redbean.pid)\n\
\n\
  Another failure condition is running out of disk space in which\n\
  case redbean reacts by truncating the log file. Lastly, redbean\n\
  does the best job possible reporting on resource usage when the\n\
  logger is in debug mode noting that NetBSD is the best at this.\n\
\n\
  Your redbean is an actually portable executable, that's able to\n\
  run on six different operating systems. To do that, it needs to\n\
  overwrite its own MZ header at startup, with ELF or Mach-O, and\n\
  then puts the original back once the program is loaded.\n\
\n\
SEE ALSO\n\
\n\
  https://justine.lol/redbean/index.html\n\
  https://news.ycombinator.com/item?id=26271117\n\
\n", program_invocation_name);
  /* clang-format on */
  exit(rc);
}

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

static uint32_t GetServerIp(void) {
  return ntohl(serveraddr.sin_addr.s_addr);
}

static uint32_t GetClientIp(void) {
  return ntohl(clientaddr.sin_addr.s_addr);
}

static bool IsLocalIp(uint32_t x) {
  return (x & 0xFF000000) == 0x7F000000; /* 127.0.0.0/8 */
}

static bool IsPrivateIp(uint32_t x) {
  return ((0x0A000000u <= x && x <= 0x0AFFFFFFu) /* 10.0.0.0/8     */ ||
          (0xAC100000u <= x && x <= 0xAC1FFFFFu) /* 172.16.0.0/12  */ ||
          (0xC0A80000u <= x && x <= 0xC0A8FFFFu) /* 192.168.0.0/16 */);
}

static bool IsTestIp(uint32_t x) {
  return (((x & 0xFFFFFF00u) == 0xC0000200u) /* 192.0.2.0/24 (RFC5737§3) */ ||
          ((x & 0xFFFFFF00u) == 0xC0000200u) /* 198.51.100.0/24 */ ||
          ((x & 0xFFFFFF00u) == 0xCB007100u) /* 203.0.113.0/24 */);
}

static bool IsPublicIp(uint32_t x) {
  return !IsLocalIp(x) && !IsPrivateIp(x) && !IsTestIp(x);
}

static bool SlicesEqual(const char *a, size_t n, const char *b, size_t m) {
  return n == m && !memcmp(a, b, n);
}

static bool SlicesEqualCase(const char *a, size_t n, const char *b, size_t m) {
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

static long FindRedirect(const char *path, size_t n) {
  int c, m, l, r, z;
  l = 0;
  r = redirects.n - 1;
  while (l <= r) {
    m = (l + r) >> 1;
    c = CompareSlices(redirects.p[m].path, redirects.p[m].pathlen, path, n);
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

static void ProgramRedirect(int code, const char *src, const char *dst) {
  long i, j;
  struct Redirect r;
  if (code && code != 301 && code != 302 && code != 307 && code != 308) {
    fprintf(stderr, "error: unsupported redirect code %d\n", code);
    exit(1);
  }
  r.code = code;
  r.path = strdup(src);
  r.pathlen = strlen(src);
  r.location = strdup(dst);
  if ((i = FindRedirect(r.path, r.pathlen)) != -1) {
    redirects.p[i] = r;
  } else {
    i = redirects.n;
    redirects.p = xrealloc(redirects.p, (i + 1) * sizeof(*redirects.p));
    for (j = i; j; --j) {
      if (CompareSlices(r.path, r.pathlen, redirects.p[j - 1].path,
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

static void ProgramRedirectArg(int code, const char *arg) {
  char *s;
  const char *p;
  if (!(p = strchr(arg, '='))) {
    fprintf(stderr, "error: redirect arg missing '='\n");
    exit(1);
  }
  ProgramRedirect(code, (s = strndup(arg, p - arg)), p + 1);
  free(s);
}

static int CompareInts(const uint64_t x, uint64_t y) {
  return x > y ? 1 : x < y ? -1 : 0;
}

static const char *BisectContentType(uint64_t ext) {
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

static const char *FindContentType(const char *path, size_t n) {
  size_t i;
  uint64_t x;
  const char *p, *r;
  if ((p = memrchr(path, '.', n))) {
    for (x = 0, i = n; i-- > p + 1 - path;) {
      x <<= 8;
      x |= path[i] & 0xFF;
    }
    if ((r = BisectContentType(bswap_64(x)))) {
      return r;
    }
  }
  return NULL;
}

static const char *GetContentType(struct Asset *a, const char *path, size_t n) {
  const char *r;
  if (a->file && (r = FindContentType(a->file->path, strlen(a->file->path)))) {
    return r;
  }
  return firstnonnull(
      FindContentType(path, n),
      firstnonnull(FindContentType(ZIP_LFILE_NAME(zmap + a->lf),
                                   ZIP_LFILE_NAMESIZE(zmap + a->lf)),
                   a->istext ? "text/plain" : "application/octet-stream"));
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

static void ProgramBrand(const char *s) {
  free(brand);
  free(serverheader);
  brand = strdup(s);
  if (!(serverheader = EncodeHttpHeaderValue(brand, -1, 0))) {
    fprintf(stderr, "error: brand isn't latin1 encodable: %`'s", brand);
    exit(1);
  }
}

static void ProgramCache(long x) {
  cacheseconds = x;
}

static void ProgramPort(long x) {
  serveraddr.sin_port = htons(x);
}

static void SetDefaults(void) {
#ifdef STATIC
  ProgramBrand("redbean-static/0.4");
#else
  ProgramBrand("redbean/0.4");
#endif
  ProgramCache(-1);
  ProgramPort(DEFAULT_PORT);
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = INADDR_ANY;
  if (IsWindows()) uniprocess = true;
}

static char *RemoveTrailingSlashes(char *s) {
  size_t n;
  n = strlen(s);
  while (n && (s[n - 1] == '/' || s[n - 1] == '\\')) s[--n] = '\0';
  return s;
}

static void AddString(struct Strings *l, char *s) {
  l->p = xrealloc(l->p, ++l->n * sizeof(*l->p));
  l->p[l->n - 1] = s;
}

static void AddStagingDirectory(const char *dirpath) {
  char *s;
  s = RemoveTrailingSlashes(strdup(dirpath));
  if (!*s || !isdirectory(s)) {
    fprintf(stderr, "error: not a directory: %`'s\n", s);
    exit(1);
  }
  AddString(&stagedirs, s);
}

static void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "zhduvmbl:p:w:r:R:c:L:P:U:G:B:D:")) != -1) {
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
      case 'b':
        logbodies = true;
        break;
      case 'z':
        printport = true;
        break;
      case 'k':
        encouragekeepalive = true;
        break;
      case 'r':
        ProgramRedirectArg(307, optarg);
        break;
      case 'R':
        ProgramRedirectArg(0, optarg);
        break;
      case 'D':
        AddStagingDirectory(optarg);
        break;
      case 'c':
        ProgramCache(strtol(optarg, NULL, 0));
        break;
      case 'p':
        ProgramPort(strtol(optarg, NULL, 0));
        break;
      case 'l':
        CHECK_EQ(1, inet_pton(AF_INET, optarg, &serveraddr.sin_addr));
        break;
      case 'B':
        ProgramBrand(optarg);
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

static void ReportWorkerExit(int pid, int ws) {
  --shared->workers;
  if (WIFEXITED(ws)) {
    if (WEXITSTATUS(ws)) {
      WARNF("%d exited with %d (%,d workers remain)", pid, WEXITSTATUS(ws),
            shared->workers);
    } else {
      DEBUGF("%d exited (%,d workers remain)", pid, shared->workers);
    }
  } else {
    WARNF("%d terminated with %s (%,d workers remain)", pid,
          strsignal(WTERMSIG(ws)), shared->workers);
  }
}

static void ReportWorkerResources(int pid, struct rusage *ru) {
  long utime, stime;
  long double ticks;
  /*
   * NetBSD accounting literally uses calculus.
   * OpenBSD and XNU are pretty good.
   * Linux and FreeBSD track less.
   */
  if (ru->ru_maxrss) {
    DEBUGF("%d ballooned to %,ldkb of memory", pid, ru->ru_maxrss);
  }
  if ((utime = ru->ru_utime.tv_sec * 1000000 + ru->ru_utime.tv_usec) |
      (stime = ru->ru_stime.tv_sec * 1000000 + ru->ru_stime.tv_usec)) {
    ticks = ceill((long double)(utime + stime) / (1000000.L / CLK_TCK));
    DEBUGF("%d needed %,ldµs of cpu (%d%% kernel)", pid, utime + stime,
           (int)((long double)utime / (utime + stime) * 100));
    if (ru->ru_idrss) {
      DEBUGF("%d needed %,ldkb of memory on average", pid,
             lroundl(ru->ru_idrss / ticks));
    }
    if (ru->ru_isrss) {
      DEBUGF("%d needed %,ldkb of stack memory on average", pid,
             lroundl(ru->ru_isrss / ticks));
    }
    if (ru->ru_ixrss) {
      DEBUGF("%d mapped %,ldkb of shared memory on average", pid,
             lroundl(ru->ru_ixrss / ticks));
    }
  }
  if (ru->ru_minflt || ru->ru_majflt) {
    DEBUGF("%d caused %,ld page faults (%d%% memcpy)", pid,
           ru->ru_minflt + ru->ru_majflt,
           (int)((long double)ru->ru_minflt / (ru->ru_minflt + ru->ru_majflt) *
                 100));
  }
  if (ru->ru_nvcsw + ru->ru_nivcsw > 1) {
    DEBUGF("%d triggered %,ld context switches (%d%% consensual)", pid,
           ru->ru_nvcsw + ru->ru_nivcsw,
           (int)((long double)ru->ru_nvcsw / (ru->ru_nvcsw + ru->ru_nivcsw) *
                 100));
  }
  if (ru->ru_inblock || ru->ru_oublock) {
    DEBUGF("%d performed %,ld read and %,ld write i/o operations", pid,
           ru->ru_inblock, ru->ru_oublock);
  }
  if (ru->ru_msgrcv || ru->ru_msgsnd) {
    DEBUGF("%d received %,ld message and sent %,ld", pid, ru->ru_msgrcv,
           ru->ru_msgsnd);
  }
  if (ru->ru_nsignals) {
    DEBUGF("%d received %,ld signals", pid, ru->ru_nsignals);
  }
  if (ru->ru_nswap) {
    DEBUGF("%d got swapped %,ld times", pid, ru->ru_nswap);
  }
}

static void WaitAll(void) {
  int ws, pid;
  struct rusage ru;
  for (;;) {
    if ((pid = wait4(-1, &ws, 0, &ru)) != -1) {
      ReportWorkerExit(pid, ws);
      ReportWorkerResources(pid, &ru);
    } else {
      if (errno == ECHILD) break;
      if (errno == EINTR) {
        if (killed) {
          killed = false;
          terminated = false;
          WARNF("%s redbean shall terminate harder", serveraddrstr);
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
  struct rusage ru;
  do {
    zombied = false;
    if ((pid = wait4(-1, &ws, WNOHANG, &ru)) != -1) {
      if (pid) {
        ReportWorkerExit(pid, ws);
        ReportWorkerResources(pid, &ru);
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
      if (killed || (meltdown && nowl() - startread > 2)) {
        return -1;
      }
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
  return !!msg.headers[h].a;
}

static bool ClientAcceptsGzip(void) {
  return msg.version >= 10 && /* RFC1945 § 3.5 */
         HeaderHasSubstring(&msg, inbuf.p, kHttpAcceptEncoding, "gzip", 4);
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

static int64_t GetZipCfileLastModified(const uint8_t *zcf) {
  const uint8_t *p, *pe;
  for (p = ZIP_CFILE_EXTRA(zcf), pe = p + ZIP_CFILE_EXTRASIZE(zcf); p + 4 <= pe;
       p += ZIP_EXTRA_SIZE(p)) {
    if (ZIP_EXTRA_HEADERID(p) == kZipExtraNtfs &&
        ZIP_EXTRA_CONTENTSIZE(p) >= 4 + 4 + 8 * 3 &&
        READ16LE(ZIP_EXTRA_CONTENT(p) + 4) == 1 &&
        READ16LE(ZIP_EXTRA_CONTENT(p) + 6) == 24) {
      return READ64LE(ZIP_EXTRA_CONTENT(p) + 8) / HECTONANOSECONDS -
             MODERNITYSECONDS;
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

static bool IsCompressed(struct Asset *a) {
  return !a->file &&
         ZIP_LFILE_COMPRESSIONMETHOD(zmap + a->lf) != kZipCompressionNone;
}

static bool IsDeflated(struct Asset *a) {
  return !a->file &&
         ZIP_LFILE_COMPRESSIONMETHOD(zmap + a->lf) == kZipCompressionDeflate;
}

static int GetMode(struct Asset *a) {
  return a->file ? a->file->st.st_mode : GetZipCfileMode(zmap + a->cf);
}

static bool IsNotModified(struct Asset *a) {
  if (msg.version < 10) return false;
  if (!HasHeader(kHttpIfModifiedSince)) return false;
  return a->lastmodified >=
         ParseHttpDateTime(HeaderData(kHttpIfModifiedSince),
                           HeaderLength(kHttpIfModifiedSince));
}

static char *FormatUnixHttpDateTime(char *s, int64_t t) {
  struct tm tm;
  gmtime_r(&t, &tm);
  FormatHttpDateTime(s, &tm);
  return s;
}

static void *FreeLater(void *p) {
  if (p) {
    if (++freelist.n > freelist.c) {
      freelist.c = freelist.n + 2;
      freelist.c += freelist.c >> 1;
      freelist.p = xrealloc(freelist.p, freelist.c * sizeof(*freelist.p));
    }
    freelist.p[freelist.n - 1] = p;
  }
  return p;
}

static void CollectGarbage(void) {
  DestroyHttpRequest(&msg);
  while (freelist.n) {
    free(freelist.p[--freelist.n]);
  }
}

static bool IsCompressionMethodSupported(int method) {
  return method == kZipCompressionNone || method == kZipCompressionDeflate;
}

static void IndexAssets(void) {
  int64_t lm;
  uint64_t cf, lf;
  struct Asset *p;
  uint32_t i, n, m, step, hash;
  CHECK_GE(HASH_LOAD_FACTOR, 2);
  CHECK(READ32LE(cdir) == kZipCdir64HdrMagic ||
        READ32LE(cdir) == kZipCdirHdrMagic);
  n = GetZipCdirRecords(cdir);
  m = roundup2pow(MAX(1, n) * HASH_LOAD_FACTOR);
  p = xcalloc(m, sizeof(struct Asset));
  for (cf = GetZipCdirOffset(cdir); n--; cf += ZIP_CFILE_HDRSIZE(zmap + cf)) {
    CHECK_EQ(kZipCfileHdrMagic, ZIP_CFILE_MAGIC(zmap + cf));
    lf = GetZipCfileOffset(zmap + cf);
    if (!IsCompressionMethodSupported(ZIP_LFILE_COMPRESSIONMETHOD(zmap + lf))) {
      WARNF("don't understand zip compression method %d used by %`'.*s",
            ZIP_LFILE_COMPRESSIONMETHOD(zmap + lf),
            ZIP_CFILE_NAMESIZE(zmap + cf), ZIP_CFILE_NAME(zmap + cf));
      continue;
    }
    hash = Hash(ZIP_CFILE_NAME(zmap + cf), ZIP_CFILE_NAMESIZE(zmap + cf));
    step = 0;
    do {
      i = (hash + (step * (step + 1)) >> 1) & (m - 1);
      ++step;
    } while (p[i].hash);
    lm = GetZipCfileLastModified(zmap + cf);
    p[i].hash = hash;
    p[i].lf = lf;
    p[i].cf = cf;
    p[i].istext = !!(ZIP_CFILE_INTERNALATTRIBUTES(zmap + cf) & kZipIattrText);
    p[i].lastmodified = lm;
    p[i].lastmodifiedstr = FormatUnixHttpDateTime(xmalloc(30), lm);
  }
  assets.p = p;
  assets.n = m;
}

static void OpenZip(const char *path) {
  int fd;
  uint8_t *p;
  struct stat st;
  CHECK_NE(-1, (fd = open(path, O_RDONLY)));
  CHECK_NE(-1, fstat(fd, &st));
  CHECK((zsize = st.st_size));
  CHECK_NE(MAP_FAILED,
           (zmap = mmap(NULL, zsize, PROT_READ, MAP_SHARED, fd, 0)));
  CHECK_NOTNULL((cdir = GetZipCdir(zmap, zsize)));
  if (endswith(path, ".com.dbg") && (p = memmem(zmap, zsize, "MZqFpD", 6))) {
    zsize -= p - zmap;
    zmap = p;
  }
  close(fd);
}

static struct Asset *GetAssetZip(const char *path, size_t pathlen) {
  uint32_t i, step, hash;
  if (pathlen > 1 && path[0] == '/') ++path, --pathlen;
  hash = Hash(path, pathlen);
  for (step = 0;; ++step) {
    i = (hash + (step * (step + 1)) >> 1) & (assets.n - 1);
    if (!assets.p[i].hash) return NULL;
    if (hash == assets.p[i].hash &&
        pathlen == ZIP_LFILE_NAMESIZE(zmap + assets.p[i].lf) &&
        memcmp(path, ZIP_LFILE_NAME(zmap + assets.p[i].lf), pathlen) == 0) {
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
      a->file->path = FreeLater(MergePaths(
          stagedirs.p[i], strlen(stagedirs.p[i]), url.path.p, url.path.n, 0));
      if (stat(a->file->path, &a->file->st) != -1) {
        a->lastmodifiedstr = FormatUnixHttpDateTime(
            FreeLater(xmalloc(30)),
            (a->lastmodified = a->file->st.st_mtim.tv_sec));
        return a;
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
      if (pathlen > 1 && path[pathlen - 1] != '/') {
        path2 = xmalloc(pathlen + 1);
        memcpy(mempcpy(path2, path, pathlen), "/", 1);
        a = GetAssetZip(path2, pathlen + 1);
        free(path2);
      }
    }
  }
  return a;
}

static char *AppendCrlf(char *p) {
  p[0] = '\r';
  p[1] = '\n';
  return p + 2;
}

static bool MustNotIncludeMessageBody(void) { /* RFC2616 § 4.4 */
  return msg.method == kHttpHead || (100 <= statuscode && statuscode <= 199) ||
         statuscode == 204 || statuscode == 304;
}

char *SetStatus(unsigned code, const char *reason) {
  statuscode = code;
  stpcpy(hdrbuf.p, "HTTP/1.0 000 ");
  hdrbuf.p[7] += msg.version & 1;
  hdrbuf.p[9] += code / 100;
  hdrbuf.p[10] += code / 10 % 10;
  hdrbuf.p[11] += code % 10;
  return AppendCrlf(stpcpy(hdrbuf.p + 13, reason));
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

static void AppendData(const char *data, size_t size) {
  outbuf.p = xrealloc(outbuf.p, outbuf.n + size);
  memcpy(outbuf.p + outbuf.n, data, size);
  outbuf.n += size;
}

static void AppendString(const char *s) {
  AppendData(s, strlen(s));
}

static void AppendFmt(const char *fmt, ...) {
  int n;
  char *p;
  va_list va;
  va_start(va, fmt);
  n = vasprintf(&p, fmt, va);
  va_end(va);
  CHECK_NE(-1, n);
  AppendData(p, n);
  free(p);
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
  return AppendExpires(p, (int64_t)nowish + seconds);
}

static char *AppendServer(char *p, const char *s) {
  p = stpcpy(p, "Server: ");
  if (IsPublicIp(GetClientIp())) {
    p = mempcpy(p, s, strchrnul(s, '/') - s);
  } else {
    p = stpcpy(p, s);
  }
  return AppendCrlf(p);
}

static char *AppendContentLength(char *p, size_t n) {
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
  z_stream zs;
  zs.next_in = sp;
  zs.avail_in = sn;
  zs.total_in = sn;
  zs.next_out = dp;
  zs.avail_out = dn;
  zs.total_out = dn;
  zs.zfree = Z_NULL;
  zs.zalloc = Z_NULL;
  CHECK_EQ(Z_OK, inflateInit2(&zs, -MAX_WBITS));
  switch (inflate(&zs, Z_NO_FLUSH)) {
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
      abort();
  }
}

static bool Verify(void *data, size_t size, uint32_t crc) {
  uint32_t got;
  if (crc == (got = crc32_z(0, data, size))) {
    return true;
  } else {
    WARNF("corrupt zip file at %`'.*s had crc 0x%08x but expected 0x%08x",
          msg.uri.b - msg.uri.a, inbuf.p + msg.uri.a, got, crc);
    return false;
  }
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
  int mode;
  size_t size;
  uint8_t *data;
  if (!S_ISREG(GetMode(a))) {
    WARNF("can't load asset that isn't a real file %#o", GetMode(a));
    return NULL;
  }
  if (a->file) {
    return xslurp(a->file->path, out_size);
  }
  if (!IsCompressionMethodSupported(
          ZIP_LFILE_COMPRESSIONMETHOD(zmap + a->lf))) {
    WARNF("unsupported compression");
    return NULL;
  }
  size = GetZipLfileUncompressedSize(zmap + a->lf);
  data = xmalloc(size + 1);
  if (IsDeflated(a)) {
    Inflate(data, size, ZIP_LFILE_CONTENT(zmap + a->lf),
            GetZipLfileCompressedSize(zmap + a->lf));
  } else {
    memcpy(data, ZIP_LFILE_CONTENT(zmap + a->lf), size);
  }
  data[size] = '\0';
  if (out_size) *out_size = size;
  return data;
}

static void AppendLogo(void) {
  size_t n;
  char *p, *q;
  struct Asset *a;
  if ((a = GetAsset("/redbean.png", 12)) && (p = LoadAsset(a, &n))) {
    q = EncodeBase64(p, n, &n);
    AppendString("<img src=\"data:image/png;base64,");
    AppendData(q, n);
    AppendString("\">\r\n");
    free(q);
    free(p);
  }
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

static void UseOutput(void) {
  content = FreeLater(outbuf.p);
  contentlength = outbuf.n;
  outbuf.p = 0;
  outbuf.n = 0;
}

static void DropOutput(void) {
  free(outbuf.p);
  outbuf.p = 0;
  outbuf.n = 0;
}

static char *CommitOutput(char *p) {
  uint32_t crc;
  if (!contentlength) {
    if (istext && outbuf.n >= 100) {
      p = stpcpy(p, "Vary: Accept-Encoding\r\n");
      if (ClientAcceptsGzip()) {
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
  reason = FreeLater(EscapeHtml(reason, -1).data);
  AppendString("\
<!doctype html>\r\n\
<title>");
  AppendFmt("%d %s", code, reason);
  AppendString("\
</title>\r\n\
<style>\r\n\
html { color: #111; font-family: sans-serif; }\r\n\
img { vertical-align: middle; }\r\n\
</style>\r\n\
<h1>\r\n");
  AppendLogo();
  AppendFmt("%d %s\r\n", code, reason);
  AppendString("</h1>\r\n");
  UseOutput();
  return p;
}

static char *ServeError(unsigned code, const char *reason) {
  size_t n;
  char *p, *s;
  struct Asset *a;
  WARNF("%s %`'.*s %`'.*s %d %s", clientaddrstr, msg.xmethod.b - msg.xmethod.a,
        inbuf.p + msg.xmethod.a, msg.uri.b - msg.uri.a, inbuf.p + msg.uri.a,
        code, reason);
  DropOutput();
  p = SetStatus(code, reason);
  s = xasprintf("/%d.html", code);
  a = GetAsset(s, strlen(s));
  free(s);
  if (!a || (IsCompressed(a) && !IsDeflated(a))) {
    return ServeDefaultErrorPage(p, code, reason);
  } else if (a->file) {
    content = FreeLater(xslurp(a->file->path, &contentlength));
    return AppendContentType(p, "text/html; charset=utf-8");
  } else {
    content = (char *)ZIP_LFILE_CONTENT(zmap + a->lf);
    contentlength = GetZipLfileCompressedSize(zmap + a->lf);
    if (IsDeflated(a)) {
      n = GetZipLfileUncompressedSize(zmap + a->lf);
      if ((s = FreeLater(malloc(n))) && Inflate(s, n, content, contentlength)) {
        content = s;
        contentlength = n;
      } else {
        return ServeDefaultErrorPage(p, code, reason);
      }
    }
    if (Verify(content, contentlength, ZIP_LFILE_CRC32(zmap + a->lf))) {
      return AppendContentType(p, "text/html; charset=utf-8");
    } else {
      return ServeDefaultErrorPage(p, code, reason);
    }
  }
}

static char *ServeAssetCompressed(struct Asset *a) {
  uint32_t crc;
  gzipped = true;
  crc = crc32_z(0, content, contentlength);
  WRITE32LE(gzip_footer + 0, crc);
  WRITE32LE(gzip_footer + 4, contentlength);
  content = FreeLater(Deflate(content, contentlength, &contentlength));
  return SetStatus(200, "OK");
}

static char *ServeAssetPrecompressed(struct Asset *a) {
  char *buf;
  size_t size;
  uint32_t crc;
  if (IsDeflated(a)) {
    crc = ZIP_LFILE_CRC32(zmap + a->lf);
    size = GetZipLfileUncompressedSize(zmap + a->lf);
    if (ClientAcceptsGzip()) {
      gzipped = true;
      WRITE32LE(gzip_footer + 0, crc);
      WRITE32LE(gzip_footer + 4, size);
      return SetStatus(200, "OK");
    } else if ((buf = FreeLater(malloc(size))) &&
               Inflate(buf, size, content, contentlength) &&
               Verify(buf, size, crc)) {
      content = buf;
      contentlength = size;
      return SetStatus(200, "OK");
    } else {
      return ServeError(500, "Internal Server Error");
    }
  } else {
    WARNF("can't serve zip asset with compression method %d",
          ZIP_LFILE_COMPRESSIONMETHOD(zmap + a->lf));
    return ServeError(501, "Not Implemented");
  }
}

static char *ServeAssetRange(struct Asset *a) {
  char *p;
  long rangestart, rangelength;
  if (ParseHttpRange(HeaderData(kHttpRange), HeaderLength(kHttpRange),
                     contentlength, &rangestart, &rangelength) &&
      rangestart >= 0 && rangelength >= 0 && rangestart < contentlength &&
      rangestart + rangelength <= contentlength) {
    p = SetStatus(206, "Partial Content");
    p = AppendContentRange(p, rangestart, rangelength, contentlength);
    content += rangestart;
    contentlength = rangelength;
    return p;
  } else {
    WARNF("bad range %`'.*s", HeaderLength(kHttpRange), HeaderData(kHttpRange));
    p = SetStatus(416, "Range Not Satisfiable");
    p = AppendContentRange(p, -1, -1, contentlength);
    content = "";
    contentlength = 0;
    return p;
  }
}

static char *ServeAsset(struct Asset *a, const char *path, size_t pathlen) {
  char *p;
  size_t size;
  uint32_t crc;
  if (IsNotModified(a)) {
    p = SetStatus(304, "Not Modified");
  } else {
    if (a->file) {
      content = FreeLater(xslurp(a->file->path, &contentlength));
    } else {
      content = (char *)ZIP_LFILE_CONTENT(zmap + a->lf);
      contentlength = GetZipLfileCompressedSize(zmap + a->lf);
    }
    if (IsCompressed(a)) {
      p = ServeAssetPrecompressed(a);
    } else if (msg.version >= 11 && HasHeader(kHttpRange)) {
      p = ServeAssetRange(a);
    } else if (!a->file) {
      if (Verify(content, contentlength, ZIP_LFILE_CRC32(zmap + a->lf))) {
        p = SetStatus(200, "OK");
      } else {
        return ServeError(500, "Internal Server Error");
      }
    } else if (ClientAcceptsGzip()) {
      p = ServeAssetCompressed(a);
    } else {
      p = SetStatus(200, "OK");
    }
  }
  p = stpcpy(p, "Vary: Accept-Encoding\r\n");
  p = AppendHeader(p, "Last-Modified", a->lastmodifiedstr);
  p = AppendContentType(p, GetContentType(a, path, pathlen));
  if (msg.version >= 11) {
    p = AppendCache(p, cacheseconds);
    if (!IsCompressed(a)) {
      p = stpcpy(p, "Accept-Ranges: bytes\r\n");
    }
  }
  return p;
}

static char *GetAssetPath(uint64_t cf, size_t *out_size) {
  char *p1, *p2;
  size_t n1, n2;
  p1 = ZIP_CFILE_NAME(zmap + cf);
  n1 = ZIP_CFILE_NAMESIZE(zmap + cf);
  p2 = xmalloc(1 + n1 + 1);
  n2 = 1 + n1 + 1;
  p2[0] = '/';
  memcpy(p2 + 1, p1, n1);
  p2[1 + n1] = '\0';
  if (out_size) *out_size = 1 + n1;
  return p2;
}

static bool IsHiddenPath(const char *s) {
  size_t i;
  for (i = 0; i < hidepaths.n; ++i) {
    if (startswith(s, hidepaths.p[i])) {
      return true;
    }
  }
  return false;
}

static void LaunchBrowser() {
  char openbrowsercommand[255];
  char *prog;
  if (IsWindows()) {
    prog = "explorer";
  } else if (IsXnu()) {
    prog = "open";
  } else {
    prog = "xdg-open";
  }
  struct in_addr addr = serveraddr.sin_addr;
  if (addr.s_addr == INADDR_ANY) addr.s_addr = htonl(INADDR_LOOPBACK);
  snprintf(openbrowsercommand, sizeof(openbrowsercommand), "%s http://%s:%d",
           prog, inet_ntoa(addr), ntohs(serveraddr.sin_port));
  DEBUGF("Opening browser with command %s\n", openbrowsercommand);
  system(openbrowsercommand);
}

static int LuaServeAsset(lua_State *L) {
  size_t pathlen;
  struct Asset *a;
  const char *path;
  path = luaL_checklstring(L, 1, &pathlen);
  if (!(a = GetAsset(path, pathlen))) {
    return luaL_argerror(L, 1, "not found");
  }
  luaheaderp = ServeAsset(a, path, pathlen);
  return 0;
}

static int LuaRespond(lua_State *L, char *respond(unsigned, const char *)) {
  char *p;
  int code;
  size_t reasonlen;
  const char *reason;
  code = luaL_checkinteger(L, 1);
  if (!(100 <= code && code <= 999)) {
    return luaL_argerror(L, 1, "bad status code");
  }
  if (lua_isnoneornil(L, 2)) {
    luaheaderp = respond(code, GetHttpReason(code));
  } else {
    reason = lua_tolstring(L, 2, &reasonlen);
    if (reasonlen < 128 && (p = EncodeHttpHeaderValue(reason, reasonlen, 0))) {
      luaheaderp = respond(code, p);
      free(p);
    } else {
      return luaL_argerror(L, 2, "invalid");
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
  char *p;
  struct Asset *a;
  const char *path;
  size_t n, pathlen;
  path = luaL_checklstring(L, 1, &pathlen);
  if ((a = GetAsset(path, pathlen)) && (p = LoadAsset(a, &n))) {
    lua_pushlstring(L, p, n);
    free(p);
  } else {
    lua_pushnil(L);
  }
  return 1;
}

static int LuaGetDate(lua_State *L) {
  lua_pushinteger(L, nowish);
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

static int LuaGetServerIp(lua_State *L) {
  lua_pushinteger(L, GetServerIp());
  return 1;
}

static int LuaGetClientIp(lua_State *L) {
  lua_pushinteger(L, GetClientIp());
  return 1;
}

static int LuaGetServerPort(lua_State *L) {
  lua_pushinteger(L, ntohs(serveraddr.sin_port));
  return 1;
}

static int LuaGetClientPort(lua_State *L) {
  lua_pushinteger(L, ntohs(clientaddr.sin_port));
  return 1;
}

static int LuaFormatIp(lua_State *L) {
  char b[16];
  uint32_t ip;
  ip = ntohl(luaL_checkinteger(L, 1));
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

static int LuaIsLocalIp(lua_State *L) {
  lua_pushboolean(L, IsLocalIp(luaL_checkinteger(L, 1)));
  return 1;
}

static int LuaIsPrivateIp(lua_State *L) {
  lua_pushboolean(L, IsPrivateIp(luaL_checkinteger(L, 1)));
  return 1;
}

static int LuaIsTestIp(lua_State *L) {
  lua_pushboolean(L, IsTestIp(luaL_checkinteger(L, 1)));
  return 1;
}

static int LuaIsPublicIp(lua_State *L) {
  lua_pushboolean(L, IsPublicIp(luaL_checkinteger(L, 1)));
  return 1;
}

static void LuaPushLatin1(lua_State *L, const char *s, size_t n) {
  char *t;
  size_t m;
  t = DecodeLatin1(s, n, &m);
  lua_pushlstring(L, t, m);
  free(t);
}

static int LuaGetUrl(lua_State *L) {
  LuaPushLatin1(L, inbuf.p + msg.uri.a, msg.uri.b - msg.uri.a);
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

static int LuaGetUser(lua_State *L) {
  LuaPushUrlView(L, &url.user);
  return 1;
}

static int LuaGetPass(lua_State *L) {
  LuaPushUrlView(L, &url.pass);
  return 1;
}

static int LuaGetPath(lua_State *L) {
  LuaPushUrlView(L, &url.path);
  return 1;
}

static int LuaGetFragment(lua_State *L) {
  LuaPushUrlView(L, &url.fragment);
  return 1;
}

static int LuaGetHost(lua_State *L) {
  if (url.host.n) {
    lua_pushlstring(L, url.host.p, url.host.n);
    return 1;
  } else {
    return LuaGetServerIp(L);
  }
}

static int LuaGetPort(lua_State *L) {
  int i, x = 0;
  for (i = 0; i < url.port.n; ++i) x = url.port.p[i] - '0' + x * 10;
  if (!x) x = ntohs(serveraddr.sin_port);
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

static int LuaGetClientAddr(lua_State *L) {
  lua_pushstring(L, clientaddrstr);
  return 1;
}

static int LuaGetServerAddr(lua_State *L) {
  lua_pushstring(L, serveraddrstr);
  return 1;
}

static int LuaGetPayload(lua_State *L) {
  lua_pushlstring(L, inbuf.p + hdrsize, msgsize - hdrsize);
  return 1;
}

static char *FoldHeader(int h, size_t *z) {
  char *p;
  size_t i, n, m;
  struct HttpRequestHeader *x;
  n = msg.headers[h].b - msg.headers[h].a;
  p = xmalloc(n);
  memcpy(p, inbuf.p + msg.headers[h].a, n);
  for (i = 0; i < msg.xheaders.n; ++i) {
    x = msg.xheaders.p + i;
    if (GetHttpHeader(inbuf.p + x->k.a, x->k.b - x->k.a) == h) {
      m = x->v.b - x->v.a;
      p = xrealloc(p, n + 2 + m);
      memcpy(mempcpy(p + n, ", ", 2), inbuf.p + x->v.a, m);
      n += 2 + m;
    }
  }
  *z = n;
  return p;
}

static int LuaGetHeader(lua_State *L) {
  int h;
  char *val;
  const char *key;
  size_t i, keylen, vallen;
  key = luaL_checklstring(L, 1, &keylen);
  if ((h = GetHttpHeader(key, keylen)) != -1) {
    if (msg.headers[h].a) {
      if (!kHttpRepeatable[h]) {
        LuaPushLatin1(L, inbuf.p + msg.headers[h].a,
                      msg.headers[h].b - msg.headers[h].a);
      } else {
        val = FoldHeader(h, &vallen);
        LuaPushLatin1(L, val, vallen);
        free(val);
      }
      return 1;
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
  size_t i;
  char *name;
  lua_newtable(L);
  for (i = 0; i < kHttpHeadersMax; ++i) {
    if (msg.headers[i].a) {
      LuaPushLatin1(L, inbuf.p + msg.headers[i].a,
                    msg.headers[i].b - msg.headers[i].a);
      lua_setfield(L, -2, GetHttpHeaderName(i));
    }
  }
  for (i = 0; i < msg.xheaders.n; ++i) {
    LuaPushLatin1(L, inbuf.p + msg.xheaders.p[i].v.a,
                  msg.xheaders.p[i].v.b - msg.xheaders.p[i].v.a);
    lua_setfield(L, -2,
                 (name = DecodeLatin1(
                      inbuf.p + msg.xheaders.p[i].k.a,
                      msg.xheaders.p[i].k.b - msg.xheaders.p[i].k.a, 0)));
    free(name);
  }
  return 1;
}

static int LuaSetHeader(lua_State *L) {
  int h;
  char *p;
  ssize_t rc;
  const char *key, *val, *eval;
  size_t i, keylen, vallen, evallen;
  key = luaL_checklstring(L, 1, &keylen);
  val = luaL_checklstring(L, 2, &vallen);
  if ((h = GetHttpHeader(key, keylen)) == -1) {
    if (!IsValidHttpToken(key, keylen)) {
      return luaL_argerror(L, 1, "invalid");
    }
  }
  if (!(eval = EncodeHttpHeaderValue(val, vallen, &evallen))) {
    return luaL_argerror(L, 2, "invalid");
  }
  if (!luaheaderp) {
    p = SetStatus(200, "OK");
  } else {
    while (luaheaderp - hdrbuf.p + keylen + 2 + evallen + 2 + 512 > hdrbuf.n) {
      hdrbuf.n += hdrbuf.n >> 1;
      p = xrealloc(hdrbuf.p, hdrbuf.n);
      luaheaderp = p + (luaheaderp - hdrbuf.p);
      hdrbuf.p = p;
    }
    p = luaheaderp;
  }
  switch (h) {
    case kHttpConnection:
      if (evallen != 5 || memcmp(eval, "close", 5)) {
        return luaL_argerror(L, 2, "unsupported");
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
  if (h->p) {
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
  } else {
    lua_pushnil(L);
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
  int h;
  size_t size;
  const char *data;
  data = luaL_checklstring(L, 1, &size);
  AppendData(data, size);
  return 0;
}

static int LuaIsValidHttpToken(lua_State *L) {
  size_t size;
  const char *data;
  data = luaL_checklstring(L, 1, &size);
  lua_pushboolean(L, IsValidHttpToken(data, size));
  return 1;
}

static int LuaIsAcceptablePath(lua_State *L) {
  size_t size;
  const char *data;
  data = luaL_checklstring(L, 1, &size);
  lua_pushboolean(L, IsAcceptablePath(data, size));
  return 1;
}

static int LuaIsAcceptableHost(lua_State *L) {
  size_t size;
  const char *data;
  data = luaL_checklstring(L, 1, &size);
  lua_pushboolean(L, IsAcceptableHost(data, size));
  return 1;
}

static int LuaIsAcceptablePort(lua_State *L) {
  size_t size;
  const char *data;
  data = luaL_checklstring(L, 1, &size);
  lua_pushboolean(L, IsAcceptablePort(data, size));
  return 1;
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
  return LuaEscaper(L, EscapeParam);
}

static int LuaEscapePath(lua_State *L) {
  return LuaEscaper(L, EscapePath);
}

static int LuaEscapeHost(lua_State *L) {
  return LuaEscaper(L, EscapeHost);
}

static int LuaEscapeIp(lua_State *L) {
  return LuaEscaper(L, EscapeIp);
}

static int LuaEscapeUser(lua_State *L) {
  return LuaEscaper(L, EscapeUser);
}

static int LuaEscapePass(lua_State *L) {
  return LuaEscaper(L, EscapePass);
}

static int LuaEscapeSegment(lua_State *L) {
  return LuaEscaper(L, EscapeSegment);
}

static int LuaEscapeFragment(lua_State *L) {
  return LuaEscaper(L, EscapeFragment);
}

static int LuaEscapeLiteral(lua_State *L) {
  return LuaEscaper(L, EscapeJsStringLiteral);
}

static int LuaEncodeBase64(lua_State *L) {
  char *p;
  size_t size, n;
  const char *data;
  data = luaL_checklstring(L, 1, &size);
  p = EncodeBase64(data, size, &n);
  lua_pushlstring(L, p, n);
  free(p);
  return 1;
}

static int LuaDecodeBase64(lua_State *L) {
  char *p;
  size_t size, n;
  const char *data;
  data = luaL_checklstring(L, 1, &size);
  p = DecodeBase64(data, size, &n);
  lua_pushlstring(L, p, n);
  free(p);
  return 1;
}

static int LuaDecodeLatin1(lua_State *L) {
  char *p;
  size_t size, n;
  const char *data;
  data = luaL_checklstring(L, 1, &size);
  p = DecodeLatin1(data, size, &n);
  lua_pushlstring(L, p, n);
  free(p);
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
    return luaL_argerror(L, 1, "zero");
  }
}

static int LuaBsf(lua_State *L) {
  long x;
  if ((x = luaL_checkinteger(L, 1))) {
    lua_pushinteger(L, bsf(x));
    return 1;
  } else {
    return luaL_argerror(L, 1, "zero");
  }
}

static int LuaCrc32(lua_State *L) {
  long i;
  size_t n;
  const char *p;
  i = luaL_checkinteger(L, 1);
  p = luaL_checklstring(L, 2, &n);
  lua_pushinteger(L, crc32_z(i, p, n));
  return 1;
}

static int LuaCrc32c(lua_State *L) {
  long i;
  size_t n;
  const char *p;
  i = luaL_checkinteger(L, 1);
  p = luaL_checklstring(L, 2, &n);
  lua_pushinteger(L, crc32c(i, p, n));
  return 1;
}

static int LuaProgramPort(lua_State *L) {
  ProgramPort(luaL_checkinteger(L, 1));
  return 0;
}

static int LuaProgramCache(lua_State *L) {
  ProgramCache(luaL_checkinteger(L, 1));
  return 0;
}

static int LuaProgramBrand(lua_State *L) {
  ProgramBrand(luaL_checkstring(L, 1));
  return 0;
}

static int LuaProgramRedirect(lua_State *L) {
  ProgramRedirect(luaL_checkinteger(L, 1), luaL_checkstring(L, 2),
                  luaL_checkstring(L, 3));
  return 0;
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
  AddString(&hidepaths, strndup(path, pathlen));
  return 0;
}

static int LuaLog(lua_State *L) {
  int level;
  lua_Debug ar;
  const char *msg, *module;
  level = luaL_checkinteger(L, 1);
  if (LOGGABLE(level)) {
    msg = luaL_checkstring(L, 2);
    lua_getstack(L, 1, &ar);
    lua_getinfo(L, "nSl", &ar);
    module = !strcmp(ar.name, "main") ? sauce : ar.name;
    flogf(level, module, ar.currentline, NULL, "%s", msg);
  }
  return 0;
}

static int LuaIsHiddenPath(lua_State *L) {
  lua_pushboolean(L, IsHiddenPath(luaL_checkstring(L, 1)));
  return 1;
}

static int LuaGetZipPaths(lua_State *L) {
  char *path;
  uint64_t cf;
  size_t i, n, pathlen;
  lua_newtable(L);
  i = 0;
  n = GetZipCdirRecords(cdir);
  for (cf = GetZipCdirOffset(cdir); n--; cf += ZIP_CFILE_HDRSIZE(zmap + cf)) {
    CHECK_EQ(kZipCfileHdrMagic, ZIP_CFILE_MAGIC(zmap + cf));
    path = GetAssetPath(cf, &pathlen);
    lua_pushlstring(L, path, pathlen);
    lua_seti(L, -2, ++i);
    free(path);
  }
  return 1;
}

static int LuaGetAssetMode(lua_State *L) {
  size_t n;
  const char *s;
  struct Asset *a;
  s = luaL_checklstring(L, 1, &n);
  if ((a = GetAsset(s, n))) {
    lua_pushinteger(L, GetMode(a));
  } else {
    lua_pushnil(L);
  }
  return 1;
}

static int LuaGetLastModifiedTime(lua_State *L) {
  size_t n;
  const char *s;
  struct Asset *a;
  s = luaL_checklstring(L, 1, &n);
  if ((a = GetAsset(s, n))) {
    if (a->file) {
      lua_pushinteger(L, a->file->st.st_mtim.tv_sec);
    } else {
      lua_pushinteger(L, GetZipCfileLastModified(zmap + a->cf));
    }
  } else {
    lua_pushnil(L);
  }
  return 1;
}

static int LuaGetAssetSize(lua_State *L) {
  size_t n;
  const char *s;
  struct Asset *a;
  s = luaL_checklstring(L, 1, &n);
  if ((a = GetAsset(s, n))) {
    if (a->file) {
      lua_pushinteger(L, a->file->st.st_size);
    } else {
      lua_pushinteger(L, GetZipLfileUncompressedSize(zmap + a->lf));
    }
  } else {
    lua_pushnil(L);
  }
  return 1;
}

static int LuaIsCompressed(lua_State *L) {
  size_t n;
  const char *s;
  struct Asset *a;
  s = luaL_checklstring(L, 1, &n);
  if ((a = GetAsset(s, n))) {
    lua_pushboolean(L, IsCompressed(a));
  } else {
    lua_pushnil(L);
  }
  return 1;
}

static int LuaGetComment(lua_State *L) {
  size_t n, m;
  const char *s;
  struct Asset *a;
  s = luaL_checklstring(L, 1, &n);
  if ((a = GetAssetZip(s, n)) &&
      (m = strnlen(ZIP_CFILE_COMMENT(zmap + a->cf),
                   ZIP_CFILE_COMMENTSIZE(zmap + a->cf)))) {
    lua_pushlstring(L, ZIP_CFILE_COMMENT(zmap + a->cf), m);
  } else {
    lua_pushnil(L);
  }
  return 1;
}

static int LuaGetStatistics(lua_State *L) {
  lua_newtable(L);
  lua_pushinteger(L, shared->workers);
  lua_setfield(L, -2, "workers");
  lua_pushinteger(L, shared->requestshandled);
  lua_setfield(L, -2, "requestshandled");
  lua_pushinteger(L, nowl() - startserver);
  lua_setfield(L, -2, "uptime");
  return 1;
}

static int LuaLaunchBrowser(lua_State *L) {
  LaunchBrowser();
  return 1;
}

static int LuaCompileRegex(lua_State *L) {
  regex_t *r;
  int c, flags;
  const char *s, *f;
  s = luaL_checkstring(L, 1);
  f = luaL_optstring(L, 2, "");
  flags = 0;
  while ((c = *f++)) {
    switch (c) {
      case 'e':
        flags |= REG_EXTENDED;
        break;
      case 'i':
        flags |= REG_ICASE;
        break;
      case 'm':
        flags |= REG_NEWLINE;
        break;
      default:
        return luaL_argerror(L, 2, "bad flag");
    }
  }
  r = lua_newuserdata(L, sizeof(*r));
  if (regcomp(r, s, flags) != REG_OK) {
    return luaL_argerror(L, 1, "bad regex");
  }
  return 1;
}

static int LuaExecuteRegex(lua_State *L) {
  int i, n;
  regex_t *r;
  regmatch_t *m;
  const char *s;
  r = lua_touserdata(L, 1);
  s = luaL_checkstring(L, 2);
  n = r->re_nsub + 1;
  m = xcalloc(n, sizeof(regmatch_t));
  if (regexec(r, s, n, m, 0) == REG_OK) {
    for (i = 0; i < n; ++i) {
      lua_pushlstring(L, s + m[i].rm_so, m[i].rm_eo - m[i].rm_so);
    }
  } else {
    n = 0;
  }
  free(m);
  return n;
}

static int LuaReleaseRegex(lua_State *L) {
  regex_t *r;
  regfree(lua_touserdata(L, 1));
  return 0;
}

static void LuaRun(const char *path) {
  struct Asset *a;
  const char *code;
  if ((a = GetAsset(path, strlen(path)))) {
    if ((code = LoadAsset(a, NULL))) {
      sauce = path + 1;
      if (luaL_dostring(L, code) != LUA_OK) {
        WARNF("%s %s", path, lua_tostring(L, -1));
      }
      free(code);
    }
  } else {
    DEBUGF("%s not found", path);
  }
}

static const luaL_Reg kLuaFuncs[] = {
    {"CompileRegex", LuaCompileRegex},                //
    {"DecodeBase64", LuaDecodeBase64},                //
    {"DecodeLatin1", LuaDecodeLatin1},                //
    {"EncodeBase64", LuaEncodeBase64},                //
    {"EncodeUrl", LuaEncodeUrl},                      //
    {"EscapeFragment", LuaEscapeFragment},            //
    {"EscapeHost", LuaEscapeHost},                    //
    {"EscapeHtml", LuaEscapeHtml},                    //
    {"EscapeIp", LuaEscapeIp},                        //
    {"EscapeLiteral", LuaEscapeLiteral},              //
    {"EscapeParam", LuaEscapeParam},                  //
    {"EscapePass", LuaEscapePass},                    //
    {"EscapePath", LuaEscapePath},                    //
    {"EscapeSegment", LuaEscapeSegment},              //
    {"EscapeUser", LuaEscapeUser},                    //
    {"ExecuteRegex", LuaExecuteRegex},                //
    {"FormatHttpDateTime", LuaFormatHttpDateTime},    //
    {"FormatIp", LuaFormatIp},                        //
    {"GetAssetMode", LuaGetAssetMode},                //
    {"GetAssetSize", LuaGetAssetSize},                //
    {"GetClientIp", LuaGetClientIp},                  //
    {"GetClientPort", LuaGetClientPort},              //
    {"GetComment", LuaGetComment},                    //
    {"GetDate", LuaGetDate},                          //
    {"GetFragment", LuaGetFragment},                  //
    {"GetHeader", LuaGetHeader},                      //
    {"GetHeaders", LuaGetHeaders},                    //
    {"GetHost", LuaGetHost},                          //
    {"GetLastModifiedTime", LuaGetLastModifiedTime},  //
    {"GetLogLevel", LuaGetLogLevel},                  //
    {"GetMethod", LuaGetMethod},                      //
    {"GetParam", LuaGetParam},                        //
    {"GetParams", LuaGetParams},                      //
    {"GetPass", LuaGetPass},                          //
    {"GetPath", LuaGetPath},                          //
    {"GetPayload", LuaGetPayload},                    //
    {"GetPort", LuaGetPort},                          //
    {"GetScheme", LuaGetScheme},                      //
    {"GetServerIp", LuaGetServerIp},                  //
    {"GetServerPort", LuaGetServerPort},              //
    {"GetStatistics", LuaGetStatistics},              //
    {"GetUrl", LuaGetUrl},                            //
    {"GetUser", LuaGetUser},                          //
    {"GetVersion", LuaGetVersion},                    //
    {"GetZipPaths", LuaGetZipPaths},                  //
    {"HasParam", LuaHasParam},                        //
    {"HidePath", LuaHidePath},                        //
    {"IsAcceptableHost", LuaIsAcceptableHost},        //
    {"IsAcceptablePath", LuaIsAcceptablePath},        //
    {"IsAcceptablePort", LuaIsAcceptablePort},        //
    {"IsCompressed", LuaIsCompressed},                //
    {"IsHiddenPath", LuaIsHiddenPath},                //
    {"IsLocalIp", LuaIsLocalIp},                      //
    {"IsPrivateIp", LuaIsPrivateIp},                  //
    {"IsPublicIp", LuaIsPublicIp},                    //
    {"IsTestIp", LuaIsTestIp},                        //
    {"IsValidHttpToken", LuaIsValidHttpToken},        //
    {"LaunchBrowser", LuaLaunchBrowser},              //
    {"LoadAsset", LuaLoadAsset},                      //
    {"Log", LuaLog},                                  //
    {"ParseHost", LuaParseHost},                      //
    {"ParseHttpDateTime", LuaParseHttpDateTime},      //
    {"ParseIp", LuaParseIp},                          //
    {"ParseParams", LuaParseParams},                  //
    {"ParseUrl", LuaParseUrl},                        //
    {"ProgramBrand", LuaProgramBrand},                //
    {"ProgramCache", LuaProgramCache},                //
    {"ProgramPort", LuaProgramPort},                  //
    {"ProgramRedirect", LuaProgramRedirect},          //
    {"ReleaseRegex", LuaReleaseRegex},                //
    {"ServeAsset", LuaServeAsset},                    //
    {"ServeError", LuaServeError},                    //
    {"SetHeader", LuaSetHeader},                      //
    {"SetLogLevel", LuaSetLogLevel},                  //
    {"SetStatus", LuaSetStatus},                      //
    {"Write", LuaWrite},                              //
    {"bsf", LuaBsf},                                  //
    {"bsr", LuaBsr},                                  //
    {"crc32", LuaCrc32},                              //
    {"crc32c", LuaCrc32c},                            //
    {"popcnt", LuaPopcnt},                            //
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
  LuaRun(".init.lua");
#endif
}

static void LuaReload(void) {
#ifndef STATIC
  LuaRun(".reload.lua");
#endif
}

static char *ServeLua(struct Asset *a) {
  char *p, *code;
  luaheaderp = NULL;
  sauce = FreeLater(strndup(url.path.p + 1, url.path.n - 1));
  if ((code = FreeLater(LoadAsset(a, NULL)))) {
    if (luaL_dostring(L, code) == LUA_OK) {
      if (!(p = luaheaderp)) {
        p = SetStatus(200, "OK");
        p = AppendContentType(p, "text/html");
      }
      return CommitOutput(p);
    } else {
      WARNF("%s %s", clientaddrstr, lua_tostring(L, -1));
      lua_pop(L, 1); /* remove message */
      connectionclose = true;
    }
  }
  return ServeError(500, "Internal Server Error");
}

static bool IsLua(struct Asset *a) {
  if (a->file) return endswith(a->file->path, ".lua");
  return ZIP_LFILE_NAMESIZE(zmap + a->lf) >= 4 &&
         !memcmp(ZIP_LFILE_NAME(zmap + a->lf) +
                     ZIP_LFILE_NAMESIZE(zmap + a->lf) - 4,
                 ".lua", 4);
}

static char *HandleAsset(struct Asset *a, const char *path, size_t pathlen) {
#ifndef STATIC
  if (IsLua(a)) return ServeLua(a);
#endif
  if (msg.method == kHttpGet || msg.method == kHttpHead) {
    return stpcpy(ServeAsset(a, path, pathlen),
                  "X-Content-Type-Options: nosniff\r\n");
  } else {
    return ServeError(405, "Method Not Allowed");
  }
}

static char *HandleRedirect(struct Redirect *r) {
  int code;
  struct Asset *a;
  if (!r->code && (a = GetAsset(r->location, strlen(r->location)))) {
    DEBUGF("%s %s %`'.*s rewritten %`'s", clientaddrstr,
           kHttpMethod[msg.method], url.path.n, url.path.p, r->location);
    return HandleAsset(a, r->location, strlen(r->location));
  } else if (msg.version == 9) {
    return ServeError(505, "HTTP Version Not Supported");
  } else {
    code = r->code;
    if (!code) code = 307;
    DEBUGF("%s %s %`'.*s %d redirecting %`'s", clientaddrstr,
           kHttpMethod[msg.method], url.path.n, url.path.p, code, r->location);
    return AppendHeader(SetStatus(code, GetHttpReason(code)), "Location",
                        FreeLater(EncodeHttpHeaderValue(r->location, -1, 0)));
  }
}

static void LogMessage(const char *d, const char *s, size_t n) {
  size_t n2, n3, n4;
  char *s2, *s3, *s4;
  if (logmessages) {
    while (n && (s[n - 1] == '\r' || s[n - 1] == '\n')) --n;
    if ((s2 = DecodeLatin1(s, n, &n2))) {
      if ((s3 = VisualizeControlCodes(s2, n2, &n3))) {
        if ((s4 = IndentLines(s3, n3, &n4, 1))) {
          LOGF("%s %s %,ld byte message\n%.*s", clientaddrstr, d, n, n4, s4);
          free(s4);
        }
        free(s3);
      }
      free(s2);
    }
  }
}

static void LogBody(const char *d, const char *s, size_t n) {
  char *s2, *s3;
  size_t n2, n3;
  if (n && logbodies) {
    while (n && (s[n - 1] == '\r' || s[n - 1] == '\n')) --n;
    if ((s2 = VisualizeControlCodes(s, n, &n2))) {
      if ((s3 = IndentLines(s2, n2, &n3, 1))) {
        LOGF("%s %s %,ld byte payload\n%.*s", clientaddrstr, d, n, n3, s3);
        free(s3);
      }
      free(s2);
    }
  }
}

static ssize_t SendMessageString(const char *s) {
  size_t n;
  ssize_t rc;
  n = strlen(s);
  LogMessage("sending", s, n);
  return 0;
  for (;;) {
    if ((rc = write(client, s, n)) != -1 || errno != EINTR) {
      return rc;
    }
  }
}

static ssize_t SendContinue(void) {
  return SendMessageString("\
HTTP/1.1 100 Continue\r\n\
\r\n");
}

static ssize_t SendTimeout(void) {
  return SendMessageString("\
HTTP/1.1 408 Request Timeout\r\n\
Connection: close\r\n\
Content-Length: 0\r\n\
\r\n");
}

static ssize_t SendServiceUnavailable(void) {
  return SendMessageString("\
HTTP/1.1 503 Service Unavailable\r\n\
Connection: close\r\n\
Content-Length: 0\r\n\
\r\n");
}

static void LogClose(const char *reason) {
  if (amtread || meltdown || killed) {
    WARNF("%s %s with %,ld bytes unprocessed and %,d requests handled",
          clientaddrstr, reason, amtread, requestshandled);
  } else {
    DEBUGF("%s %s with %,d requests handled", clientaddrstr, reason,
           requestshandled);
  }
}

static const char *DescribeClose(void) {
  if (killed) return "killed";
  if (meltdown) return "meltdown";
  if (terminated) return "terminated";
  if (connectionclose) return "connectionclose";
  return "destroyed";
}

static const char *DescribeCompressionRatio(char rb[8], uint64_t lf) {
  long percent;
  if (ZIP_LFILE_COMPRESSIONMETHOD(zmap + lf) == kZipCompressionNone) {
    return "n/a";
  } else {
    percent = lround(100 - (double)GetZipLfileCompressedSize(zmap + lf) /
                               GetZipLfileUncompressedSize(zmap + lf) * 100);
    sprintf(rb, "%ld%%", MIN(999, MAX(-999, percent)));
    return rb;
  }
}

static int GetDecimalWidth(int x) {
  int w = x ? ceil(log10(x)) : 1;
  return w + (w - 1) / 3;
}

static int GetOctalWidth(int x) {
  return !x ? 1 : x < 8 ? 2 : 1 + bsr(x) / 3;
}

static char *ServeListing(void) {
  long x;
  ldiv_t y;
  int w[3];
  char rb[8];
  char tb[64];
  struct tm tm;
  const char *and;
  int64_t lastmod;
  uint64_t cf, lf;
  char *p, *q, *path;
  size_t i, n, pathlen;
  struct EscapeResult r[4];
  if (msg.method != kHttpGet && msg.method != kHttpHead) {
    return stpcpy(ServeError(405, "Method Not Allowed"),
                  "Allow: GET, HEAD\r\n");
  }
  if (IsPublicIp(GetClientIp())) {
    WARNF("%s listing page requested from public ip address", clientaddrstr);
    return ServeError(403, "Forbidden");
  }
  AppendString("\
<!doctype html>\r\n\
<meta charset=\"utf-8\">\r\n\
<title>redbean zip listing</title>\r\n\
<style>\r\n\
html { color: #111; font-family: sans-serif; }\r\n\
a { text-decoration: none; }\r\n\
img { vertical-align: middle; }\r\n\
footer { font-size: 11pt; }\r\n\
</style>\r\n\
<header><h1>\r\n");
  AppendLogo();
  r[0] = EscapeHtml(brand, strlen(brand));
  AppendData(r[0].data, r[0].size);
  free(r[0].data);
  AppendString("</h1><hr></header><pre>\r\n");
  memset(w, 0, sizeof(w));
  n = GetZipCdirRecords(cdir);
  for (cf = GetZipCdirOffset(cdir); n--; cf += ZIP_CFILE_HDRSIZE(zmap + cf)) {
    CHECK_EQ(kZipCfileHdrMagic, ZIP_CFILE_MAGIC(zmap + cf));
    lf = GetZipCfileOffset(zmap + cf);
    path = GetAssetPath(cf, &pathlen);
    if (!IsHiddenPath(path)) {
      w[0] = min(80, max(w[0], strwidth(path, 0) + 2));
      w[1] = max(w[1], GetOctalWidth(GetZipCfileMode(zmap + cf)));
      w[2] = max(w[2], GetDecimalWidth(GetZipLfileUncompressedSize(zmap + lf)));
    }
    free(path);
  }
  n = GetZipCdirRecords(cdir);
  for (cf = GetZipCdirOffset(cdir); n--; cf += ZIP_CFILE_HDRSIZE(zmap + cf)) {
    CHECK_EQ(kZipCfileHdrMagic, ZIP_CFILE_MAGIC(zmap + cf));
    lf = GetZipCfileOffset(zmap + cf);
    path = GetAssetPath(cf, &pathlen);
    if (!IsHiddenPath(path)) {
      r[0] = EscapeHtml(path, pathlen);
      r[1] = EscapePath(path, pathlen);
      r[2] = EscapeHtml(r[1].data, r[1].size);
      r[3] = EscapeHtml(ZIP_CFILE_COMMENT(zmap + cf),
                        strnlen(ZIP_CFILE_COMMENT(zmap + cf),
                                ZIP_CFILE_COMMENTSIZE(zmap + cf)));
      lastmod = GetZipCfileLastModified(zmap + cf);
      localtime_r(&lastmod, &tm);
      strftime(tb, sizeof(tb), "%Y-%m-%d %H:%M:%S %Z", &tm);
      if (IsCompressionMethodSupported(
              ZIP_LFILE_COMPRESSIONMETHOD(zmap + lf)) &&
          IsAcceptablePath(path, pathlen)) {
        AppendFmt("<a href=\"%.*s\">%-*.*s</a> %s  %0*o %4s  %,*ld  %'s\r\n",
                  r[2].size, r[2].data, w[0], r[0].size, r[0].data, tb, w[1],
                  GetZipCfileMode(zmap + cf), DescribeCompressionRatio(rb, lf),
                  w[2], GetZipLfileUncompressedSize(zmap + lf), r[3].data);
      } else {
        AppendFmt("%-*.*s %s  %0*o %4s  %,*ld  %'s\r\n", w[0], r[0].size,
                  r[0].data, tb, w[1], GetZipCfileMode(zmap + cf),
                  DescribeCompressionRatio(rb, lf), w[2],
                  GetZipLfileUncompressedSize(zmap + lf), r[3].data);
      }
      free(r[3].data);
      free(r[2].data);
      free(r[1].data);
      free(r[0].data);
    }
    free(path);
  }
  AppendString("</pre><footer><hr><p>\r\n");
  and = "";
  x = nowl() - startserver;
  y = ldiv(x, 24L * 60 * 60);
  if (y.quot) {
    AppendFmt("%,ld day%s ", y.quot, y.quot == 1 ? "" : "s");
    and = "and ";
  }
  y = ldiv(y.rem, 60 * 60);
  if (y.quot) {
    AppendFmt("%,ld hour%s ", y.quot, y.quot == 1 ? "" : "s");
    and = "and ";
  }
  y = ldiv(y.rem, 60);
  if (y.quot) {
    AppendFmt("%,ld minute%s ", y.quot, y.quot == 1 ? "" : "s");
    and = "and ";
  }
  AppendFmt("%s%,ld second%s of operation<br>\r\n", and, y.rem,
            y.rem == 1 ? "" : "s");
  x = shared->requestshandled;
  AppendFmt("%,ld url%s handled<br>\r\n", x, x == 1 ? "" : "s");
  x = shared->workers;
  AppendFmt("%,ld connection%s active<br>\r\n", x, x == 1 ? "" : "s");
  AppendString("</footer>\r\n");
  p = SetStatus(200, "OK");
  p = AppendCache(p, 0);
  p = AppendContentType(p, "text/html");
  return CommitOutput(p);
}

static bool HasAtMostThisElement(int h, const char *s) {
  size_t i, n;
  struct HttpRequestHeader *x;
  if (HasHeader(h)) {
    n = strlen(s);
    if (!SlicesEqualCase(s, n, inbuf.p + msg.headers[h].a,
                         msg.headers[h].b - msg.headers[h].a)) {
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

static char *SynchronizeStream(void) {
  size_t got;
  ssize_t rc;
  int64_t cl;
  if ((cl = ParseContentLength(HeaderData(kHttpContentLength),
                               HeaderLength(kHttpContentLength))) == -1) {
    if (HasHeader(kHttpContentLength)) {
      WARNF("invalid content length");
      return ServeError(400, "Bad Request");
    } else if (msg.method == kHttpPost || msg.method == kHttpPut) {
      return ServeError(411, "Length Required");
    } else {
      cl = 0;
    }
  }
  if (hdrsize + cl > amtread) {
    if (hdrsize + cl > inbuf.n) {
      return ServeError(413, "Payload Too Large");
    }
    if (msg.version >= 11 && HeaderEqualCase(kHttpExpect, "100-continue")) {
      SendContinue();
    }
    while (amtread < hdrsize + cl) {
      if (++frags == 64) {
        LogClose("payload fragged!");
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
  }
  msgsize = hdrsize + cl;
  return NULL;
}

static void ParseRequestParameters(void) {
  FreeLater(ParseRequestUri(inbuf.p + msg.uri.a, msg.uri.b - msg.uri.a, &url));
  if (!url.host.p) {
    FreeLater(ParseHost(HeaderData(kHttpHost), HeaderLength(kHttpHost), &url));
  } else if (!url.path.n) {
    url.path.p = "/";
    url.path.n = 1;
  }
#ifndef STATIC
  if (HasHeader(kHttpContentType) &&
      IsMimeType(HeaderData(kHttpContentType), HeaderLength(kHttpContentType),
                 "application/x-www-form-urlencoded")) {
    FreeLater(ParseParams(inbuf.p + hdrsize, msgsize - hdrsize, &url.params));
  }
#endif
  FreeLater(url.params.p);
}

static char *ServeServerOptions(void) {
  char *p;
  p = SetStatus(200, "OK");
#ifdef STATIC
  p = stpcpy(p, "Allow: GET, HEAD, OPTIONS\r\n");
#else
  p = stpcpy(p, "Accept: */*\r\n"
                "Accept-Charset: utf-8\r\n"
                "Allow: GET, HEAD, POST, PUT, DELETE, OPTIONS\r\n");
#endif
  return p;
}

static char *RedirectSlash(void) {
  char *p;
  struct EscapeResult r;
  if (url.path.n && url.path.p[url.path.n - 1] != '/') {
    p = SetStatus(307, "Temporary Redirect");
    p = stpcpy(p, "Location: ");
    r = EscapePath(url.path.p, url.path.n);
    p = mempcpy(p, r.data, r.size);
    p = stpcpy(p, "/\r\n");
    free(r.data);
    return p;
  } else {
    return SetStatus(508, "Loop Detected");
  }
}

static char *TryPath(const char *, size_t);
static char *TryIndex(const char *path, size_t pathlen) {
  size_t i, n;
  char *p, *q;
  p = NULL;
  for (i = 0; !p && i < ARRAYLEN(kIndexPaths); ++i) {
    q = MergePaths(path, pathlen, kIndexPaths[i], strlen(kIndexPaths[i]), &n);
    p = TryPath(q, n);
    free(q);
  }
  return p;
}

static char *TryPath(const char *path, size_t pathlen) {
  int m;
  long r;
  struct Asset *a;
  DEBUGF("TryPath(%`'.*s)", pathlen, path);
  if ((a = GetAsset(path, pathlen))) {
    if ((m = GetMode(a)) & 0004) {
      if (S_ISREG(m)) {
        return HandleAsset(a, path, pathlen);
      } else if (S_ISDIR(m)) {
        if (path[pathlen - 1] == '/') {
          return TryIndex(path, pathlen);
        } else {
          return RedirectSlash();
        }
      } else {
        WARNF("asset %`'.*s %#o is special", pathlen, path, m);
        return ServeError(403, "Forbidden");
      }
    } else {
      WARNF("asset %`'.*s %#o isn't readable", pathlen, path, m);
      return ServeError(403, "Forbidden");
    }
  } else if ((r = FindRedirect(path, pathlen)) != -1) {
    return HandleRedirect(redirects.p + r);
  } else {
    return NULL;
  }
}

static char *TryHost(const char *host, size_t hostlen) {
  size_t hn;
  char *hp, *p;
  hn = 1 + hostlen + url.path.n;
  hp = FreeLater(xmalloc(3 + 1 + hn));
  hp[0] = '/';
  mempcpy(mempcpy(hp + 1, host, hostlen), url.path.p, url.path.n);
  if ((p = TryPath(hp, hn))) return p;
  if (ParseIp(host, hostlen) == -1) {
    if (hostlen > 4 && !memcmp(host, "www.", 4)) {
      mempcpy(mempcpy(hp + 1, host + 4, hostlen - 4), url.path.p, url.path.n);
      if ((p = TryPath(hp, hn - 4))) return p;
    } else {
      mempcpy(mempcpy(mempcpy(hp + 1, "www.", 4), host, hostlen), url.path.p,
              url.path.n);
      if ((p = TryPath(hp, hn + 4))) return p;
    }
  }
  return NULL;
}

static char *HandleMessage(void) {
  char *p;
  VERBOSEF("%s %`'.*s %`'.*s %`'.*s HTTP%02d %`'.*s %`'.*s", clientaddrstr,
           msg.xmethod.b - msg.xmethod.a, inbuf.p + msg.xmethod.a,
           HeaderLength(kHttpHost), HeaderData(kHttpHost),
           msg.uri.b - msg.uri.a, inbuf.p + msg.uri.a, msg.version,
           HeaderLength(kHttpReferer), HeaderData(kHttpReferer),
           HeaderLength(kHttpUserAgent), HeaderData(kHttpUserAgent));
  if (msg.version > 11) {
    return ServeError(505, "HTTP Version Not Supported");
  }
  if ((p = SynchronizeStream())) return p;
  LogBody("received", inbuf.p + hdrsize, msgsize - hdrsize);
  if (msg.version != 11 || HeaderEqualCase(kHttpConnection, "close")) {
    connectionclose = true;
  }
  if (msg.method == kHttpConnect) {
    return ServeError(501, "Not Implemented");
  }
  if (!HasAtMostThisElement(kHttpExpect, "100-continue")) {
    return ServeError(417, "Expectation Failed");
  }
  if (!HasAtMostThisElement(kHttpTransferEncoding, "identity")) {
    return ServeError(501, "Not Implemented");
  }
  ParseRequestParameters();
  if (msg.method == kHttpOptions &&
      !CompareSlices(url.path.p, url.path.n, "*", 1)) {
    return ServeServerOptions();
  }
  if (!url.path.n || url.path.p[0] != '/' ||
      !IsAcceptablePath(url.path.p, url.path.n) ||
      !IsAcceptableHost(url.host.p, url.host.n) ||
      !IsAcceptablePort(url.port.p, url.port.n)) {
    WARNF("%s unacceptable %`'.*s %`'.*s", clientaddrstr,
          HeaderLength(kHttpHost), HeaderData(kHttpHost), msg.uri.b - msg.uri.a,
          inbuf.p + msg.uri.a);
    return ServeError(400, "Bad Request");
  }
  if (url.host.n && (p = TryHost(url.host.p, url.host.n))) return p;
  if (url.path.n == 1 && url.path.p[0] == '/') {
    if ((p = TryIndex("/", 1))) return p;
    return ServeListing();
  } else if ((p = TryPath(url.path.p, url.path.n))) {
    return p;
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
  if ((rc = ParseHttpRequest(&msg, inbuf.p, amtread)) != -1) {
    if (!rc) return false;
    hdrsize = rc;
    LogMessage("received", inbuf.p, hdrsize);
    p = HandleMessage();
  } else {
    connectionclose = true;
    p = ServeError(400, "Bad Request");
    DEBUGF("%s received garbage %`'.*s", clientaddrstr, amtread, inbuf.p);
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
  if (msg.version >= 10) {
    p = AppendHeader(p, "Date", currentdate);
    if (!branded) p = AppendServer(p, serverheader);
    if (connectionclose) {
      p = stpcpy(p, "Connection: close\r\n");
    } else if (encouragekeepalive && msg.version >= 11) {
      p = stpcpy(p, "Connection: keep-alive\r\n");
    }
    actualcontentlength = contentlength;
    if (gzipped) {
      actualcontentlength += sizeof(kGzipHeader) + sizeof(gzip_footer);
      p = stpcpy(p, "Content-Encoding: gzip\r\n");
    }
    p = AppendContentLength(p, actualcontentlength);
    p = AppendCrlf(p);
    CHECK_LE(p - hdrbuf.p, hdrbuf.n);
    LogMessage("sending", hdrbuf.p, p - hdrbuf.p);
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
  InterlockedAdd(&shared->requestshandled, 1);
  ++requestshandled;
  return true;
}

static void InitRequest(void) {
  frags = 0;
  msgsize = 0;
  content = NULL;
  gzipped = false;
  branded = false;
  contentlength = 0;
  InitHttpRequest(&msg);
}

static void HandleRequests(void) {
  ssize_t rc;
  size_t got;
  long double now;
  for (;;) {
    InitRequest();
    startread = nowl();
    for (;;) {
      if (!msg.i && amtread && HandleRequest()) break;
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
              LogClose("fragged!");
              return;
            } else {
              DEBUGF("%s fragged msg %,ld %,ld", clientaddrstr, amtread, got);
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
    CollectGarbage();
  }
}

static void EnterMeltdownMode(void) {
  if (lastmeltdown && nowl() - lastmeltdown < 1) return;
  WARNF("redbean is melting down (%,d workers)", shared->workers);
  LOGIFNEG1(kill(0, SIGUSR2));
  lastmeltdown = nowl();
}

static void HandleConnection(void) {
  int pid;
  clientaddrsize = sizeof(clientaddr);
  if ((client = accept4(server, &clientaddr, &clientaddrsize, SOCK_CLOEXEC)) !=
      -1) {
    startconnection = nowl();
    if (uniprocess) {
      pid = -1;
      requestshandled = 0;
      connectionclose = true;
    } else {
      switch ((pid = fork())) {
        case 0:
          meltdown = false;
          connectionclose = false;
          break;
        case -1:
          WARNF("%s too man processes %s", serveraddrstr, strerror(errno));
          EnterMeltdownMode();
          SendServiceUnavailable();
          /* fallthrough */
        default:
          ++shared->workers;
          close(client);
          return;
      }
    }
    DescribeAddress(clientaddrstr, &clientaddr);
    DEBUGF("%s accept", clientaddrstr);
    HandleRequests();
    LOGIFNEG1(close(client));
    if (!pid) {
      _exit(0);
    } else {
      CollectGarbage();
    }
  } else if (errno != EINTR) {
    if (errno == ENFILE) {
      WARNF("%s too many open files", serveraddrstr);
      EnterMeltdownMode();
    } else if (errno == EMFILE) {
      WARNF("%s ran out of open file quota", serveraddrstr);
      EnterMeltdownMode();
    } else if (errno == ENOMEM || errno == ENOBUFS) {
      WARNF("%s ran out of memory");
      EnterMeltdownMode();
    } else if (errno == ENONET) {
      WARNF("%s network gone", serveraddrstr);
      sleep(1);
    } else if (errno == ENETDOWN) {
      WARNF("%s network down", serveraddrstr);
      sleep(1);
    } else if (errno == ECONNABORTED) {
      WARNF("%s connection reset before accept");
    } else if (errno == ENETUNREACH || errno == EHOSTUNREACH ||
               errno == EOPNOTSUPP || errno == ENOPROTOOPT || errno == EPROTO) {
      WARNF("%s ephemeral accept error %s", serveraddrstr, strerror(errno));
    } else {
      FATALF("%s accept error %s", serveraddrstr, strerror(errno));
    }
  }
}

static void HandleHeartbeat(void) {
  UpdateCurrentDate(nowl());
}

static void TuneServerSocket(void) {
  int yes = 1;
  setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
  setsockopt(server, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes));
  setsockopt(server, IPPROTO_TCP, TCP_FASTOPEN, &yes, sizeof(yes));
  setsockopt(server, IPPROTO_TCP, TCP_QUICKACK, &yes, sizeof(yes));
}

static void RestoreApe(const char *prog) {
  char *p;
  size_t n;
  struct Asset *a;
  extern char ape_rom_vaddr[] __attribute__((__weak__));
  if (IsWindows()) return;
  if (endswith(prog, ".com.dbg")) return;
  close(OpenExecutable());
  if ((a = GetAsset(".ape", 4)) && (p = LoadAsset(a, &n))) {
    mprotect(ape_rom_vaddr, PAGESIZE, PROT_READ | PROT_WRITE);
    memcpy(ape_rom_vaddr, p, MIN(n, PAGESIZE));
    msync(ape_rom_vaddr, PAGESIZE, MS_ASYNC);
    mprotect(ape_rom_vaddr, PAGESIZE, PROT_NONE);
    free(p);
  }
}

void RedBean(int argc, char *argv[], const char *prog) {
  uint32_t addrsize;
  startserver = nowl();
  gmtoff = GetGmtOffset();
  CHECK_GT(CLK_TCK, 0);
  CHECK_NE(MAP_FAILED,
           (shared = mmap(NULL, ROUNDUP(sizeof(struct Shared), FRAMESIZE),
                          PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,
                          -1, 0)));
  OpenZip(prog);
  IndexAssets();
  RestoreApe(prog);
  SetDefaults();
  GetOpts(argc, argv);
  LuaInit();
  if (uniprocess) shared->workers = 1;
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
  CHECK_NE(-1,
           (server = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, IPPROTO_TCP)));
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
  if (daemonize) Daemonize();
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
  while (!terminated) {
    if (zombied) {
      ReapZombies();
    } else if (invalidated) {
      LuaReload();
      invalidated = false;
    } else if (heartbeat) {
      HandleHeartbeat();
      heartbeat = false;
    } else if (meltdown) {
      EnterMeltdownMode();
      meltdown = false;
    } else {
      if (heartless) HandleHeartbeat();
      HandleConnection();
    }
  }
  VERBOSEF("%s shutting down", serveraddrstr);
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
  setenv("GDB", "", true);
  showcrashreports();
  RedBean(argc, argv, (const char *)getauxval(AT_EXECFN));
  return 0;
}
