/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/pledge.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.internal.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/sysconf.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/stdio/append.h"
#include "libc/stdio/stdio.h"
#include "libc/str/slice.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"
#include "libc/sysv/consts/tcp.h"
#include "libc/thread/thread.h"
#include "libc/thread/thread2.h"
#include "libc/time/struct/tm.h"
#include "libc/x/x.h"
#include "libc/x/xasprintf.h"
#include "libc/zip.h"
#include "net/http/escape.h"
#include "net/http/http.h"
#include "net/http/url.h"
#include "third_party/getopt/getopt.h"
#include "third_party/nsync/cv.h"
#include "third_party/nsync/mu.h"
#include "third_party/nsync/note.h"
#include "third_party/sqlite3/sqlite3.h"
#include "third_party/zlib/zconf.h"
#include "third_party/zlib/zlib.h"
#include "tool/net/lfuncs.h"

/**
 * @fileoverview production webserver for turfwar online game
 */

#define PORT              8080   // default server listening port
#define WORKERS           9001   // size of http client thread pool
#define KEEPALIVE_MS      60000  // max time to keep idle conn open
#define MELTALIVE_MS      2000   // panic keepalive under heavy load
#define POLL_ASSETS_MS    1000   // how often to stat() asset files
#define DATE_UPDATE_MS    500    // how often to do tzdata crunching
#define SCORE_UPDATE_MS   60000  // how often to regeenrate /score json
#define SCORE_H_UPDATE_MS 15000  // how often to regeenrate /score json
#define SCORE_D_UPDATE_MS 15000  // how often to regeenrate /score json
#define SCORE_W_UPDATE_MS 15000  // how often to regeenrate /score json
#define SCORE_M_UPDATE_MS 15000  // how often to regeenrate /score json
#define CLAIM_DEADLINE_MS 100    // how long /claim may block if queue is full
#define PANIC_LOAD        .85    // meltdown if this percent of pool connected
#define PANIC_MSGS        10     // msgs per conn can't exceed it in meltdown
#define QUEUE_MAX         800    // maximum pending claim items in queue
#define BATCH_MAX         64     // max claims to insert per transaction
#define NICK_MAX          40     // max length of user nickname string
#define MSG_BUF           512    // small response lookaside

#define INBUF_SIZE  PAGESIZE
#define OUTBUF_SIZE PAGESIZE

#define GETOPTS "dvp:w:k:"
#define USAGE \
  "\
Usage: turfwar.com [-dv] ARGS...\n\
  -d          daemonize\n\
  -v          verbosity\n\
  -p INT      port\n\
  -w INT      workers\n\
  -k INT      keepalive\n\
"

#define STANDARD_RESPONSE_HEADERS \
  "Server: turfwar\r\n"           \
  "Referrer-Policy: origin\r\n"   \
  "Access-Control-Allow-Origin: *\r\n"

#define HasHeader(H)    (!!msg->headers[H].a)
#define HeaderData(H)   (inbuf + msg->headers[H].a)
#define HeaderLength(H) (msg->headers[H].b - msg->headers[H].a)
#define HeaderEqual(H, S) \
  SlicesEqual(S, strlen(S), HeaderData(H), HeaderLength(H))
#define HeaderEqualCase(H, S) \
  SlicesEqualCase(S, strlen(S), HeaderData(H), HeaderLength(H))
#define UrlEqual(S) \
  SlicesEqual(inbuf + msg->uri.a, msg->uri.b - msg->uri.a, S, strlen(S))
#define UrlStartsWith(S)                   \
  (msg->uri.b - msg->uri.a >= strlen(S) && \
   !memcmp(inbuf + msg->uri.a, S, strlen(S)))

#if 1
#define LOG(...) kprintf("\r\e[K" __VA_ARGS__)
#else
#define LOG(...) (void)0
#endif

#if 0
#define DEBUG(...) kprintf("\r\e[K" __VA_ARGS__)
#else
#define DEBUG(...) (void)0
#endif

#define CHECK_MEM(x)                        \
  do {                                      \
    if (!CheckMem(__FILE__, __LINE__, x)) { \
      goto OnError;                         \
    }                                       \
  } while (0)
#define CHECK_SYS(x)                        \
  do {                                      \
    if (!CheckSys(__FILE__, __LINE__, x)) { \
      goto OnError;                         \
    }                                       \
  } while (0)
#define CHECK_SQL(x)                        \
  do {                                      \
    int e = errno;                          \
    if (!CheckSql(__FILE__, __LINE__, x)) { \
      goto OnError;                         \
    }                                       \
    errno = e;                              \
  } while (0)
#define CHECK_DB(x)                            \
  do {                                         \
    int e = errno;                             \
    if (!CheckDb(__FILE__, __LINE__, x, db)) { \
      goto OnError;                            \
    }                                          \
    errno = e;                                 \
  } while (0)

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

static const char kPixel[43] =
    "\x47\x49\x46\x38\x39\x61\x01\x00\x01\x00\x80\x00\x00\xff\xff\xff"
    "\x00\x00\x00\x21\xf9\x04\x01\x00\x00\x00\x00\x2c\x00\x00\x00\x00"
    "\x01\x00\x01\x00\x00\x02\x02\x44\x01\x00\x3b";

struct Data {
  char *p;
  size_t n;
};

struct Asset {
  char *path;
  nsync_mu lock;
  const char *type;
  const char *cache;
  struct Data data;
  struct Data gzip;
  struct timespec mtim;
  char lastmod[32];
};

bool g_daemonize;
int g_port = PORT;
int g_workers = WORKERS;
int g_keepalive = KEEPALIVE_MS;

nsync_note g_shutdown;
nsync_note g_terminate;
atomic_int g_connections;

struct Worker {
  pthread_t th;
  atomic_int msgcount;
  atomic_bool shutdown;
  atomic_bool connected;
  struct timespec startread;
} * g_worker;

struct Recent {
  nsync_mu mu;
  nsync_cv cv;
} g_recent;

struct Nowish {
  nsync_mu lock;
  struct timespec ts;
  struct tm tm;
} g_nowish;

struct Assets {
  struct Asset index;
  struct Asset about;
  struct Asset user;
  struct Asset score;
  struct Asset score_hour;
  struct Asset score_day;
  struct Asset score_week;
  struct Asset score_month;
  struct Asset recent;
  struct Asset favicon;
} g_asset;

struct Claims {
  int pos;
  int count;
  nsync_mu mu;
  nsync_cv non_full;
  nsync_cv non_empty;
  struct Claim {
    uint32_t ip;
    int64_t created;
    char name[NICK_MAX + 1];
  } data[QUEUE_MAX];
} g_claims;

bool CheckMem(const char *file, int line, void *ptr) {
  if (ptr) return true;
  kprintf("%s:%d: out of memory: %s\n", file, line, strerror(errno));
  return false;
}

bool CheckSys(const char *file, int line, long rc) {
  if (rc != -1) return true;
  kprintf("%s:%d: %s\n", file, line, strerror(errno));
  return false;
}

bool CheckSql(const char *file, int line, int rc) {
  if (rc == SQLITE_OK) return true;
  kprintf("%s:%d: %s\n", file, line, sqlite3_errstr(rc));
  return false;
}

bool CheckDb(const char *file, int line, int rc, sqlite3 *db) {
  if (rc == SQLITE_OK) return true;
  kprintf("%s:%d: %s: %s\n", file, line, sqlite3_errstr(rc),
          sqlite3_errmsg(db));
  return false;
}

bool IsValidNick(const char *s, size_t n) {
  size_t i;
  if (n == -1) n = strlen(s);
  if (!n) return false;
  if (n > NICK_MAX) return false;
  for (i = 0; i < n; ++i) {
    if (!(isalnum(s[i]) ||  //
          s[i] == '@' ||    //
          s[i] == '/' ||    //
          s[i] == ':' ||    //
          s[i] == '.' ||    //
          s[i] == '^' ||    //
          s[i] == '+' ||    //
          s[i] == '!' ||    //
          s[i] == '-' ||    //
          s[i] == '_' ||    //
          s[i] == '*')) {
      return false;
    }
  }
  return true;
}

char *FormatUnixHttpDateTime(char *s, int64_t t) {
  struct tm tm;
  gmtime_r(&t, &tm);
  FormatHttpDateTime(s, &tm);
  return s;
}

void UpdateNow(void) {
  int64_t secs;
  struct tm tm;
  clock_gettime(CLOCK_REALTIME, &g_nowish.ts);
  secs = g_nowish.ts.tv_sec;
  gmtime_r(&secs, &tm);
  //!//!//!//!//!//!//!//!//!//!//!//!//!/
  nsync_mu_lock(&g_nowish.lock);
  g_nowish.tm = tm;
  nsync_mu_unlock(&g_nowish.lock);
  //!//!//!//!//!//!//!//!//!//!//!//!//!/
}

char *FormatDate(char *p) {
  ////////////////////////////////////////
  nsync_mu_rlock(&g_nowish.lock);
  p = FormatHttpDateTime(p, &g_nowish.tm);
  nsync_mu_runlock(&g_nowish.lock);
  ////////////////////////////////////////
  return p;
}

bool AddClaim(struct Claims *q, const struct Claim *v, nsync_time dead) {
  bool wake = false;
  bool added = false;
  nsync_mu_lock(&q->mu);
  while (q->count == ARRAYLEN(q->data)) {
    if (nsync_cv_wait_with_deadline(&q->non_full, &q->mu, dead, g_shutdown)) {
      break;  // must be ETIMEDOUT or ECANCELED
    }
  }
  if (q->count != ARRAYLEN(q->data)) {
    int i = q->pos + q->count;
    if (ARRAYLEN(q->data) <= i) i -= ARRAYLEN(q->data);
    memcpy(q->data + i, v, sizeof(*v));
    if (!q->count) wake = true;
    q->count++;
    added = true;
  }
  nsync_mu_unlock(&q->mu);
  if (wake) {
    nsync_cv_broadcast(&q->non_empty);
  }
  return added;
}

int GetClaims(struct Claims *q, struct Claim *out, int len, nsync_time dead) {
  int got = 0;
  nsync_mu_lock(&q->mu);
  while (!q->count) {
    if (nsync_cv_wait_with_deadline(&q->non_empty, &q->mu, dead, g_terminate)) {
      break;  // must be ETIMEDOUT or ECANCELED
    }
  }
  while (got < len && q->count) {
    memcpy(out + got, q->data + q->pos, sizeof(*out));
    if (q->count == ARRAYLEN(q->data)) {
      nsync_cv_broadcast(&q->non_full);
    }
    ++got;
    q->pos++;
    q->count--;
    if (q->pos == ARRAYLEN(q->data)) {
      q->pos = 0;
    }
  }
  nsync_mu_unlock(&q->mu);
  return got;
}

static bool GetNick(char *inbuf, struct HttpMessage *msg, struct Claim *v) {
  size_t i, n;
  struct Url url;
  void *f[2] = {0};
  bool found = false;
  f[0] = ParseUrl(inbuf + msg->uri.a, msg->uri.b - msg->uri.a, &url,
                  kUrlPlus | kUrlLatin1);
  f[1] = url.params.p;
  for (i = 0; i < url.params.n; ++i) {
    if (SlicesEqual("name", 4, url.params.p[i].key.p, url.params.p[i].key.n) &&
        url.params.p[i].val.p &&
        IsValidNick(url.params.p[i].val.p, url.params.p[i].val.n)) {
      memcpy(v->name, url.params.p[i].val.p, url.params.p[i].val.n);
      found = true;
      break;
    }
  }
  free(f[1]);
  free(f[0]);
  return found;
}

void *NewSafeBuffer(size_t n) {
  char *p;
  size_t m = ROUNDUP(n, PAGESIZE);
  _npassert((p = valloc(m + PAGESIZE)));
  _npassert(!mprotect(p + m, PAGESIZE, PROT_NONE));
  return p;
}

void FreeSafeBuffer(void *p) {
  size_t n = malloc_usable_size(p);
  size_t m = ROUNDDOWN(n, PAGESIZE);
  _npassert(!mprotect(p, m, PROT_READ | PROT_WRITE));
  free(p);
}

void OnlyRunOnCpu(int i) {
  cpu_set_t cpus;
  if (GetCpuCount() > i + 1) {
    CPU_ZERO(&cpus);
    CPU_SET(i, &cpus);
    CHECK_EQ(0, pthread_setaffinity_np(pthread_self(), sizeof(cpus), &cpus));
  }
}

void DontRunOnFirstCpus(int i) {
  int n;
  cpu_set_t cpus;
  if ((n = GetCpuCount()) > 1) {
    CPU_ZERO(&cpus);
    for (; i < n; ++i) {
      CPU_SET(i, &cpus);
    }
    CHECK_EQ(0, pthread_setaffinity_np(pthread_self(), sizeof(cpus), &cpus));
  } else {
    notpossible;
  }
}

// thousands of http client servicing threads
// load balance incoming connections for port 8080 across all threads
// hangup on any browser clients that lag for more than a few seconds
void *HttpWorker(void *arg) {
  int server;
  int yes = 1;
  char name[16];
  sigset_t mask;
  int id = (intptr_t)arg;
  char *msgbuf = _gc(malloc(MSG_BUF));
  char *inbuf = NewSafeBuffer(INBUF_SIZE);
  char *outbuf = NewSafeBuffer(OUTBUF_SIZE);
  struct timeval timeo = {g_keepalive / 1000, g_keepalive % 1000};
  struct HttpMessage *msg = _gc(xmalloc(sizeof(struct HttpMessage)));
  struct sockaddr_in addr = {.sin_family = AF_INET, .sin_port = htons(g_port)};

  sigfillset(&mask);
  DontRunOnFirstCpus(2);
  sigdelset(&mask, SIGUSR1);
  sigprocmask(SIG_SETMASK, &mask, 0);
  ksnprintf(name, sizeof(name), "HTTP #%d", id);
  pthread_setname_np(pthread_self(), name);
  CHECK_NE(-1, (server = socket(AF_INET, SOCK_STREAM, 0)));
  setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, &timeo, sizeof(timeo));
  setsockopt(server, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo));
  setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
  setsockopt(server, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes));
  setsockopt(server, SOL_TCP, TCP_FASTOPEN, &yes, sizeof(yes));
  setsockopt(server, SOL_TCP, TCP_QUICKACK, &yes, sizeof(yes));
  CHECK_NE(-1, bind(server, &addr, sizeof(addr)));
  CHECK_NE(-1, listen(server, 1));
  errno = 0;

  // connection loop
  while (!nsync_note_is_notified(g_shutdown)) {
    struct Data d;
    struct Url url;
    ssize_t got, sent;
    uint32_t ip, clientip;
    char ipbuf[32], *p, *q;
    uint32_t clientaddrsize;
    struct sockaddr_in clientaddr;
    int client, inmsglen, outmsglen;

    // wait for client connection
    clientaddrsize = sizeof(clientaddr);
    client = accept(server, (struct sockaddr *)&clientaddr, &clientaddrsize);
    if (client == -1) continue;
    ip = clientip = ntohl(clientaddr.sin_addr.s_addr);
    g_worker[id].connected = true;
    g_worker[id].msgcount = 0;
    ++g_connections;

    // strict message loop w/o pipelining
    do {
      struct Asset *a;
      bool comp, ipv6;
      sigdelset(&mask, SIGUSR1);
      sigprocmask(SIG_SETMASK, &mask, 0);
      InitHttpMessage(msg, kHttpRequest);
      g_worker[id].startread = _timespec_real();
      if ((got = read(client, inbuf, INBUF_SIZE)) <= 0) break;
      sigaddset(&mask, SIGUSR1);
      sigprocmask(SIG_SETMASK, &mask, 0);
      if ((inmsglen = ParseHttpMessage(msg, inbuf, got)) <= 0) break;
      if (msg->version != 11) break;  // cloudflare won't send 0.9 or 1.0
      ++g_worker[id].msgcount;

      // get the ip address again
      // we assume a firewall only lets the frontend talk to this server
      ipv6 = false;
      if (HasHeader(kHttpXForwardedFor) &&
          ParseForwarded(HeaderData(kHttpXForwardedFor),
                         HeaderLength(kHttpXForwardedFor), &ip, 0) == -1) {
        ipv6 = true;
      }
      ksnprintf(ipbuf, sizeof(ipbuf), "%hhu.%hhu.%hhu.%hhu", ip >> 24, ip >> 16,
                ip >> 8, ip);

      if (UrlEqual("/") || UrlStartsWith("/index.html")) {
        a = &g_asset.index;
      } else if (UrlStartsWith("/favicon.ico")) {
        a = &g_asset.favicon;
      } else if (UrlStartsWith("/about.html")) {
        a = &g_asset.about;
      } else if (UrlStartsWith("/user.html")) {
        a = &g_asset.user;
      } else if (UrlStartsWith("/score/hour")) {
        a = &g_asset.score_hour;
      } else if (UrlStartsWith("/score/day")) {
        a = &g_asset.score_day;
      } else if (UrlStartsWith("/score/week")) {
        a = &g_asset.score_week;
      } else if (UrlStartsWith("/score/month")) {
        a = &g_asset.score_month;
      } else if (UrlStartsWith("/score")) {
        a = &g_asset.score;
      } else if (UrlStartsWith("/recent")) {
        a = &g_asset.recent;
      } else {
        a = 0;
      }

      if (a) {
        struct iovec iov[2];
        comp = a->gzip.n < a->data.n &&
               HeaderHas(msg, inbuf, kHttpAcceptEncoding, "gzip", 4);
        p = stpcpy(outbuf, "HTTP/1.1 200 OK\r\n" STANDARD_RESPONSE_HEADERS
                           "Vary: Accept-Encoding\r\n"
                           "Date: ");
        p = FormatDate(p);
        ////////////////////////////////////////
        nsync_mu_rlock(&a->lock);
        p = stpcpy(p, "\r\nLast-Modified: ");
        p = stpcpy(p, a->lastmod);
        p = stpcpy(p, "\r\nContent-Type: ");
        p = stpcpy(p, a->type);
        p = stpcpy(p, "\r\nCache-Control: ");
        p = stpcpy(p, a->cache);
        if (comp) p = stpcpy(p, "\r\nContent-Encoding: gzip");
        p = stpcpy(p, "\r\nContent-Length: ");
        d = comp ? a->gzip : a->data;
        p = FormatInt32(p, d.n);
        p = stpcpy(p, "\r\n\r\n");
        iov[0].iov_base = outbuf;
        iov[0].iov_len = p - outbuf;
        iov[1].iov_base = d.p;
        iov[1].iov_len = d.n;
        sent = writev(client, iov, 2);
        outmsglen = iov[0].iov_len + iov[1].iov_len;
        nsync_mu_runlock(&a->lock);
        ////////////////////////////////////////

      } else if (UrlStartsWith("/ip")) {
        if (!ipv6) {
          p = stpcpy(outbuf, "HTTP/1.1 200 OK\r\n" STANDARD_RESPONSE_HEADERS
                             "Vary: Accept\r\n"
                             "Content-Type: text/plain\r\n"
                             "Cache-Control: max-age=3600, private\r\n"
                             "Date: ");
          p = FormatDate(p);
          p = stpcpy(p, "\r\nContent-Length: ");
          p = FormatInt32(p, strlen(ipbuf));
          p = stpcpy(p, "\r\n\r\n");
          p = stpcpy(p, ipbuf);
          outmsglen = p - outbuf;
          sent = write(client, outbuf, outmsglen);
        } else {
        Ipv6Warning:
          DEBUG("%.*s via %s: 400 Need IPv4\n",
                HeaderLength(kHttpXForwardedFor),
                HeaderData(kHttpXForwardedFor), ipbuf);
          q = "IPv4 Games only supports IPv4 right now";
          p = stpcpy(outbuf,
                     "HTTP/1.1 400 Need IPv4\r\n" STANDARD_RESPONSE_HEADERS
                     "Vary: Accept\r\n"
                     "Content-Type: text/plain\r\n"
                     "Cache-Control: private\r\n"
                     "Connection: close\r\n"
                     "Date: ");
          p = FormatDate(p);
          p = stpcpy(p, "\r\nContent-Length: ");
          p = FormatInt32(p, strlen(q));
          p = stpcpy(p, "\r\n\r\n");
          p = stpcpy(p, q);
          outmsglen = p - outbuf;
          sent = write(client, outbuf, p - outbuf);
          break;
        }

      } else if (UrlStartsWith("/claim")) {
        if (ipv6) goto Ipv6Warning;
        struct Claim v = {.ip = ip, .created = g_nowish.ts.tv_sec};
        if (GetNick(inbuf, msg, &v)) {
          if (AddClaim(
                  &g_claims, &v,
                  _timespec_add(_timespec_real(),
                                _timespec_frommillis(CLAIM_DEADLINE_MS)))) {
            LOG("%s claimed by %s\n", ipbuf, v.name);
            if (HasHeader(kHttpAccept) &&
                (HeaderHas(msg, inbuf, kHttpAccept, "image/*", 7) ||
                 HeaderHas(msg, inbuf, kHttpAccept, "image/gif", 9))) {
              p = stpcpy(outbuf, "HTTP/1.1 200 OK\r\n" STANDARD_RESPONSE_HEADERS
                                 "Vary: Accept\r\n"
                                 "Cache-Control: private\r\n"
                                 "Content-Type: image/gif\r\n"
                                 "Date: ");
              p = FormatDate(p);
              p = stpcpy(p, "\r\nContent-Length: ");
              p = FormatInt32(p, sizeof(kPixel));
              p = stpcpy(p, "\r\n\r\n");
              p = mempcpy(p, kPixel, sizeof(kPixel));
            } else if (HasHeader(kHttpAccept) &&
                       HeaderHas(msg, inbuf, kHttpAccept, "text/plain", 10) &&
                       !HeaderHas(msg, inbuf, kHttpAccept, "text/html", 9)) {
              ksnprintf(msgbuf, MSG_BUF, "The land at %s was claimed for %s\n",
                        ipbuf, v.name);
              q = msgbuf;
              p = stpcpy(outbuf, "HTTP/1.1 200 OK\r\n" STANDARD_RESPONSE_HEADERS
                                 "Vary: Accept\r\n"
                                 "Cache-Control: private\r\n"
                                 "Content-Type: text/plain\r\n"
                                 "Date: ");
              p = FormatDate(p);
              p = stpcpy(p, "\r\nContent-Length: ");
              p = FormatInt32(p, strlen(q));
              p = stpcpy(p, "\r\n\r\n");
              p = stpcpy(p, q);
            } else if (!HasHeader(kHttpAccept) ||
                       (HeaderHas(msg, inbuf, kHttpAccept, "text/html", 9) ||
                        HeaderHas(msg, inbuf, kHttpAccept, "text/*", 6) ||
                        HeaderHas(msg, inbuf, kHttpAccept, "*/*", 3))) {
              ksnprintf(msgbuf, MSG_BUF,
                        "<!doctype html>\n"
                        "<title>The land at %s was claimed for %s.</title>\n"
                        "<meta name=\"viewport\" "
                        "content=\"width=device-width, initial-scale=1\">\n"
                        "The land at %s was claimed for <a "
                        "href=\"/user.html?name=%s\">%s</a>.\n"
                        "<p>\n<a href=/>Back to homepage</a>\n",
                        ipbuf, v.name, ipbuf, v.name, v.name);
              q = msgbuf;
              p = stpcpy(outbuf, "HTTP/1.1 200 OK\r\n" STANDARD_RESPONSE_HEADERS
                                 "Vary: Accept\r\n"
                                 "Cache-Control: private\r\n"
                                 "Content-Type: text/html\r\n"
                                 "Date: ");
              p = FormatDate(p);
              p = stpcpy(p, "\r\nContent-Length: ");
              p = FormatInt32(p, strlen(q));
              p = stpcpy(p, "\r\n\r\n");
              p = stpcpy(p, q);
            } else {
              p = stpcpy(outbuf,
                         "HTTP/1.1 204 No Content\r\n" STANDARD_RESPONSE_HEADERS
                         "Vary: Accept\r\n"
                         "Cache-Control: private\r\n"
                         "Content-Length: 0\r\n"
                         "Date: ");
              p = FormatDate(p);
              p = stpcpy(p, "\r\n\r\n");
            }
            outmsglen = p - outbuf;
            sent = write(client, outbuf, p - outbuf);
          } else {
            LOG("%s: 502 Claims Queue Full\n", ipbuf);
            q = "Claims Queue Full";
            p = stpcpy(
                outbuf,
                "HTTP/1.1 502 Claims Queue Full\r\n" STANDARD_RESPONSE_HEADERS
                "Content-Type: text/plain\r\n"
                "Cache-Control: private\r\n"
                "Connection: close\r\n"
                "Date: ");
            p = FormatDate(p);
            p = stpcpy(p, "\r\nContent-Length: ");
            p = FormatInt32(p, strlen(q));
            p = stpcpy(p, "\r\n\r\n");
            p = stpcpy(p, q);
            outmsglen = p - outbuf;
            sent = write(client, outbuf, p - outbuf);
            break;
          }
        } else {
          LOG("%s: 400 invalid name\n", ipbuf);
          q = "invalid name";
          p = stpcpy(outbuf,
                     "HTTP/1.1 400 Invalid Name\r\n" STANDARD_RESPONSE_HEADERS
                     "Content-Type: text/plain\r\n"
                     "Cache-Control: private\r\n"
                     "Connection: close\r\n"
                     "Date: ");
          p = FormatDate(p);
          p = stpcpy(p, "\r\nContent-Length: ");
          p = FormatInt32(p, strlen(q));
          p = stpcpy(p, "\r\n\r\n");
          p = stpcpy(p, q);
          outmsglen = p - outbuf;
          sent = write(client, outbuf, p - outbuf);
          break;
        }

      } else {
        LOG("%s: 400 not found %#.*s\n", ipbuf, msg->uri.b - msg->uri.a,
            inbuf + msg->uri.a);
        q = "<!doctype html>\r\n"
            "<title>404 not found</title>\r\n"
            "<h1>404 not found</h1>\r\n";
        p = stpcpy(outbuf,
                   "HTTP/1.1 404 Not Found\r\n" STANDARD_RESPONSE_HEADERS
                   "Content-Type: text/html; charset=utf-8\r\n"
                   "Date: ");
        p = FormatDate(p);
        p = stpcpy(p, "\r\nContent-Length: ");
        p = FormatInt32(p, strlen(q));
        p = stpcpy(p, "\r\n\r\n");
        p = stpcpy(p, q);
        outmsglen = p - outbuf;
        sent = write(client, outbuf, p - outbuf);
      }

      // if the client isn't pipelining and write() wrote the full
      // amount, then since we sent the content length and checked
      // that the client didn't attach a payload, we are so synced
      // thus we can safely process more messages
    } while (got == inmsglen &&    //
             sent == outmsglen &&  //
             !msg->headers[kHttpContentLength].a &&
             !msg->headers[kHttpTransferEncoding].a &&
             (msg->method == kHttpGet || msg->method == kHttpHead) &&
             !nsync_note_is_notified(g_shutdown));
    DestroyHttpMessage(msg);
    close(client);
    g_worker[id].connected = false;
    --g_connections;
  }

  LOG("HttpWorker #%d exiting", id);
  g_worker[id].shutdown = true;
  FreeSafeBuffer(outbuf);
  FreeSafeBuffer(inbuf);
  close(server);
  return 0;
}

struct Data Gzip(struct Data data) {
  char *p;
  void *tmp;
  uint32_t crc;
  char footer[8];
  z_stream zs = {0};
  struct Data res = {0};
  crc = crc32_z(0, data.p, data.n);
  WRITE32LE(footer + 0, crc);
  WRITE32LE(footer + 4, data.n);
  if (Z_OK != deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -MAX_WBITS,
                           DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY)) {
    return (struct Data){0};
  }
  zs.next_in = (const Bytef *)data.p;
  zs.avail_in = data.n;
  zs.avail_out = compressBound(data.n);
  if (!(zs.next_out = tmp = malloc(zs.avail_out))) {
    deflateEnd(&zs);
    return (struct Data){0};
  }
  CHECK_EQ(Z_STREAM_END, deflate(&zs, Z_FINISH));
  CHECK_EQ(Z_OK, deflateEnd(&zs));
  res.n = sizeof(kGzipHeader) + zs.total_out + sizeof(footer);
  if (!(p = res.p = malloc(res.n))) {
    free(tmp);
    return (struct Data){0};
  }
  p = mempcpy(p, kGzipHeader, sizeof(kGzipHeader));
  p = mempcpy(p, tmp, zs.total_out);
  p = mempcpy(p, footer, sizeof(footer));
  free(tmp);
  return res;
}

struct Asset LoadAsset(const char *path, const char *type) {
  struct stat st;
  struct Asset a = {0};
  CHECK_EQ(0, stat(path, &st));
  CHECK_NOTNULL((a.data.p = xslurp(path, &a.data.n)));
  a.type = type;
  a.cache = "max-age=3600, must-revalidate";
  CHECK_NOTNULL((a.path = strdup(path)));
  a.mtim = st.st_mtim;
  CHECK_NOTNULL((a.gzip = Gzip(a.data)).p);
  FormatUnixHttpDateTime(a.lastmod, a.mtim.tv_sec);
  return a;
}

bool ReloadAsset(struct Asset *a) {
  int fd;
  void *f[2];
  ssize_t rc;
  struct stat st;
  char lastmod[32];
  struct Data data = {0};
  struct Data gzip = {0};
  CHECK_SYS((fd = open(a->path, O_RDONLY)));
  CHECK_SYS(fstat(fd, &st));
  if (_timespec_gt(st.st_mtim, a->mtim)) {
    FormatUnixHttpDateTime(lastmod, st.st_mtim.tv_sec);
    CHECK_MEM((data.p = malloc(st.st_size)));
    CHECK_SYS((rc = read(fd, data.p, st.st_size)));
    data.n = st.st_size;
    if (rc != st.st_size) goto OnError;
    CHECK_MEM((gzip = Gzip(data)).p);
    //!//!//!//!//!//!//!//!//!//!//!//!//!/
    nsync_mu_lock(&a->lock);
    f[0] = a->data.p;
    f[1] = a->gzip.p;
    a->data = data;
    a->gzip = gzip;
    a->mtim = st.st_mtim;
    memcpy(a->lastmod, lastmod, 32);
    nsync_mu_unlock(&a->lock);
    //!//!//!//!//!//!//!//!//!//!//!//!//!/
    free(f[0]);
    free(f[1]);
  }
  close(fd);
  return true;
OnError:
  free(data.p);
  free(gzip.p);
  close(fd);
  return false;
}

void FreeAsset(struct Asset *a) {
  free(a->path);
  free(a->data.p);
  free(a->gzip.p);
}

void IgnoreSignal(int sig) {
  // so worker i/o routines may eintr safely
}

void OnCtrlC(int sig) {
  if (!nsync_note_is_notified(g_shutdown)) {
    LOG("Received %s shutting down...\n", strsignal(sig));
    nsync_note_notify(g_shutdown);
  } else {
    // there's no way to deliver signals to workers atomically, unless
    // we pay the cost of ppoll() which isn't necessary in this design
    LOG("Received %s again so sending another volley...\n", strsignal(sig));
    for (int i = 0; i < g_workers; ++i) {
      if (!g_worker[i].shutdown) {
        tkill(pthread_getunique_np(g_worker[i].th), SIGUSR1);
      }
    }
  }
}

static void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, GETOPTS)) != -1) {
    switch (opt) {
      case 'd':
        g_daemonize = true;
        break;
      case 'p':
        g_port = atoi(optarg);
        break;
      case 'w':
        g_workers = atoi(optarg);
        break;
      case 'k':
        g_keepalive = atoi(optarg);
        break;
      case 'v':
        ++__log_level;
        break;
      case '?':
        write(1, USAGE, sizeof(USAGE) - 1);
        exit(0);
      default:
        write(2, USAGE, sizeof(USAGE) - 1);
        exit(64);
    }
  }
}

void Update(struct Asset *a, bool gen(struct Asset *, long), long arg) {
  void *f[2];
  struct Asset t;
  if (gen(&t, arg)) {
    //!//!//!//!//!//!//!//!//!//!//!//!//!/
    nsync_mu_lock(&a->lock);
    f[0] = a->data.p;
    f[1] = a->gzip.p;
    a->data = t.data;
    a->gzip = t.gzip;
    a->mtim = t.mtim;
    memcpy(a->lastmod, t.lastmod, 32);
    nsync_mu_unlock(&a->lock);
    //!//!//!//!//!//!//!//!//!//!//!//!//!/
    free(f[0]);
    free(f[1]);
  }
}

bool GenerateScore(struct Asset *out, long seconds) {
  int rc;
  char *sb = 0;
  sqlite3 *db = 0;
  size_t sblen = 0;
  struct Asset a = {0};
  sqlite3_stmt *stmt = 0;
  bool namestate = false;
  char name1[NICK_MAX + 1] = {0};
  char name2[NICK_MAX + 1];
  DEBUG("GenerateScore %ld\n", seconds);
  a.type = "application/json";
  a.cache = "max-age=15, must-revalidate";
  CHECK_SYS(clock_gettime(CLOCK_REALTIME, &a.mtim));
  FormatUnixHttpDateTime(a.lastmod, a.mtim.tv_sec);
  CHECK_SYS(appends(&a.data.p, "{\n"));
  CHECK_SYS(appendf(&a.data.p, "\"now\":[%ld,%ld],\n", a.mtim.tv_sec,
                    a.mtim.tv_nsec));
  CHECK_SYS(appends(&a.data.p, "\"score\":{\n"));
  CHECK_SQL(sqlite3_open("db.sqlite3", &db));
  CHECK_SQL(sqlite3_exec(db, "PRAGMA journal_mode=WAL", 0, 0, 0));
  CHECK_SQL(sqlite3_exec(db, "PRAGMA synchronous=NORMAL", 0, 0, 0));
  if (seconds == -1) {
    CHECK_DB(sqlite3_prepare_v2(db,
                                "SELECT nick, (ip >> 24), COUNT(*)\n"
                                "FROM land\n"
                                "GROUP BY nick, (ip >> 24)",
                                -1, &stmt, 0));
  } else {
    CHECK_DB(sqlite3_prepare_v2(db,
                                "SELECT nick, (ip >> 24), COUNT(*)\n"
                                " FROM land\n"
                                "WHERE created NOT NULL\n"
                                "  AND created >= ?1\n"
                                "GROUP BY nick, (ip >> 24)",
                                -1, &stmt, 0));
    CHECK_DB(sqlite3_bind_int64(stmt, 1, a.mtim.tv_sec - seconds));
  }
  CHECK_SQL(sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, 0));
  while ((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
    if (rc != SQLITE_ROW) CHECK_SQL(rc);
    strlcpy(name2, (void *)sqlite3_column_text(stmt, 0), sizeof(name2));
    if (!IsValidNick(name2, -1)) continue;
    if (strcmp(name1, name2)) {
      // name changed
      if (namestate) CHECK_SYS(appends(&a.data.p, "],\n"));
      namestate = true;
      CHECK_SYS(appendf(
          &a.data.p, "\"%s\":[\n",
          EscapeJsStringLiteral(&sb, &sblen, strcpy(name1, name2), -1, 0)));
    } else {
      // name repeated
      CHECK_SYS(appends(&a.data.p, ",\n"));
    }
    CHECK_SYS(appendf(&a.data.p, "  [%ld,%ld]", sqlite3_column_int64(stmt, 1),
                      sqlite3_column_int64(stmt, 2)));
  }
  CHECK_SQL(sqlite3_exec(db, "END TRANSACTION", 0, 0, 0));
  if (namestate) CHECK_SYS(appends(&a.data.p, "]\n"));
  CHECK_SYS(appends(&a.data.p, "}}\n"));
  CHECK_DB(sqlite3_finalize(stmt));
  CHECK_SQL(sqlite3_close(db));
  a.data.n = appendz(a.data.p).i;
  a.gzip = Gzip(a.data);
  free(sb);
  *out = a;
  return true;
OnError:
  sqlite3_finalize(stmt);
  sqlite3_close(db);
  free(a.data.p);
  free(sb);
  return false;
}

// single thread for regenerating the user scores json
void *ScoreWorker(void *arg) {
  nsync_time deadline;
  LOG("ScoreWorker started\n");
  OnlyRunOnCpu(0);
  pthread_setname_np(pthread_self(), "ScoreWorker");
  for (deadline = _timespec_real();;) {
    deadline = _timespec_add(deadline, _timespec_frommillis(SCORE_UPDATE_MS));
    if (!nsync_note_wait(g_shutdown, deadline)) {
      Update(&g_asset.score, GenerateScore, -1);
    } else {
      break;
    }
  }
  LOG("ScoreWorker exiting\n");
  return 0;
}

// single thread for regenerating the user scores json
void *ScoreHourWorker(void *arg) {
  nsync_time deadline;
  LOG("ScoreHourWorker started\n");
  OnlyRunOnCpu(0);
  pthread_setname_np(pthread_self(), "ScoreHourWorker");
  for (deadline = _timespec_real();;) {
    deadline = _timespec_add(deadline, _timespec_frommillis(SCORE_D_UPDATE_MS));
    if (!nsync_note_wait(g_shutdown, deadline)) {
      Update(&g_asset.score_hour, GenerateScore, 60L * 60);
    } else {
      break;
    }
  }
  LOG("ScoreHourWorker exiting\n");
  return 0;
}

// single thread for regenerating the user scores json
void *ScoreDayWorker(void *arg) {
  nsync_time deadline;
  LOG("ScoreDayWorker started\n");
  OnlyRunOnCpu(0);
  pthread_setname_np(pthread_self(), "ScoreDayWorker");
  for (deadline = _timespec_real();;) {
    deadline = _timespec_add(deadline, _timespec_frommillis(SCORE_D_UPDATE_MS));
    if (!nsync_note_wait(g_shutdown, deadline)) {
      Update(&g_asset.score_day, GenerateScore, 60L * 60 * 24);
    } else {
      break;
    }
  }
  LOG("ScoreDayWorker exiting\n");
  return 0;
}

// single thread for regenerating the user scores json
void *ScoreWeekWorker(void *arg) {
  nsync_time deadline;
  LOG("ScoreWeekWorker started\n");
  OnlyRunOnCpu(0);
  pthread_setname_np(pthread_self(), "ScoreWeekWorker");
  for (deadline = _timespec_real();;) {
    deadline = _timespec_add(deadline, _timespec_frommillis(SCORE_D_UPDATE_MS));
    if (!nsync_note_wait(g_shutdown, deadline)) {
      Update(&g_asset.score_week, GenerateScore, 60L * 60 * 24 * 7);
    } else {
      break;
    }
  }
  LOG("ScoreWeekWorker exiting\n");
  return 0;
}

// single thread for regenerating the user scores json
void *ScoreMonthWorker(void *arg) {
  nsync_time deadline;
  LOG("ScoreMonthWorker started\n");
  OnlyRunOnCpu(0);
  pthread_setname_np(pthread_self(), "ScoreMonthWorker");
  for (deadline = _timespec_real();;) {
    deadline = _timespec_add(deadline, _timespec_frommillis(SCORE_D_UPDATE_MS));
    if (!nsync_note_wait(g_shutdown, deadline)) {
      Update(&g_asset.score_month, GenerateScore, 60L * 60 * 24 * 30);
    } else {
      break;
    }
  }
  LOG("ScoreMonthWorker exiting\n");
  return 0;
}

bool GenerateRecent(struct Asset *out, long arg) {
  int rc;
  char *sb = 0;
  sqlite3 *db = 0;
  size_t sblen = 0;
  bool once = false;
  struct Asset a = {0};
  sqlite3_stmt *stmt = 0;
  DEBUG("GenerateRecent\n");
  OnlyRunOnCpu(0);
  pthread_setname_np(pthread_self(), "GenerateRecent");
  a.type = "application/json";
  a.cache = "max-age=0, must-revalidate";
  CHECK_SYS(clock_gettime(CLOCK_REALTIME, &a.mtim));
  FormatUnixHttpDateTime(a.lastmod, a.mtim.tv_sec);
  CHECK_SYS(appends(&a.data.p, "{\n"));
  CHECK_SYS(appendf(&a.data.p, "\"now\":[%ld,%ld],\n", a.mtim.tv_sec,
                    a.mtim.tv_nsec));
  CHECK_SYS(appends(&a.data.p, "\"recent\":[\n"));
  CHECK_SQL(sqlite3_open("db.sqlite3", &db));
  CHECK_SQL(sqlite3_exec(db, "PRAGMA journal_mode=WAL", 0, 0, 0));
  CHECK_SQL(sqlite3_exec(db, "PRAGMA synchronous=NORMAL", 0, 0, 0));
  CHECK_DB(sqlite3_prepare_v2(db,
                              "SELECT ip, nick, created\n"
                              "FROM land\n"
                              "WHERE created NOT NULL\n"
                              "ORDER BY created DESC\n"
                              "LIMIT 50",
                              -1, &stmt, 0));
  CHECK_SQL(sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, 0));
  while ((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
    if (rc != SQLITE_ROW) CHECK_SQL(rc);
    if (once) {
      CHECK_SYS(appends(&a.data.p, ",\n"));
    } else {
      once = true;
    }
    CHECK_SYS(
        appendf(&a.data.p, "[%ld,\"%s\",%ld]", sqlite3_column_int64(stmt, 0),
                EscapeJsStringLiteral(
                    &sb, &sblen, (void *)sqlite3_column_text(stmt, 1), -1, 0),
                sqlite3_column_int64(stmt, 2)));
  }
  CHECK_SQL(sqlite3_exec(db, "END TRANSACTION", 0, 0, 0));
  CHECK_SYS(appends(&a.data.p, "]}\n"));
  CHECK_DB(sqlite3_finalize(stmt));
  CHECK_SQL(sqlite3_close(db));
  a.data.n = appendz(a.data.p).i;
  a.gzip = Gzip(a.data);
  free(sb);
  *out = a;
  return true;
OnError:
  sqlite3_finalize(stmt);
  sqlite3_close(db);
  free(a.data.p);
  free(sb);
  return false;
}

// thread for realtime json generation most recent successful claims
void *RecentWorker(void *arg) {
  int rc;
  OnlyRunOnCpu(1);
  pthread_setname_np(pthread_self(), "RecentWorker");
  LOG("RecentWorker started\n");
  for (;;) {
    nsync_mu_lock(&g_recent.mu);
    rc = nsync_cv_wait_with_deadline(&g_recent.cv, &g_recent.mu,
                                     nsync_time_no_deadline, g_shutdown);
    nsync_mu_unlock(&g_recent.mu);
    if (rc == ECANCELED) break;
    Update(&g_asset.recent, GenerateRecent, -1);
  }
  LOG("RecentWorker exiting\n");
  return 0;
}

// single thread for inserting batched claims into the database
void *ClaimWorker(void *arg) {
  int i, n, rc;
  sqlite3 *db = 0;
  sqlite3_stmt *stmt = 0;
  struct Claim *v = _gc(xcalloc(BATCH_MAX, sizeof(struct Claim)));
  pthread_setname_np(pthread_self(), "ClaimWorker");
  LOG("ClaimWorker started\n");
  OnlyRunOnCpu(0);
StartOver:
  CHECK_SQL(sqlite3_open("db.sqlite3", &db));
  CHECK_SQL(sqlite3_exec(db, "PRAGMA journal_mode=WAL", 0, 0, 0));
  CHECK_SQL(sqlite3_exec(db, "PRAGMA synchronous=NORMAL", 0, 0, 0));
  CHECK_DB(sqlite3_prepare_v2(db,
                              "INSERT INTO land (ip, nick, created)\n"
                              "VALUES (?1, ?2, ?3)\n"
                              "ON CONFLICT (ip) DO\n"
                              "UPDATE SET (nick, created) = (?2, ?3)\n"
                              "WHERE nick != ?2\n"
                              "  AND (created IS NULL\n"
                              "   OR  (?3 - created) > 3600)",
                              -1, &stmt, 0));
  LOG("ClaimWorker started\n");
  while ((n = GetClaims(&g_claims, v, BATCH_MAX, nsync_time_no_deadline))) {
    CHECK_SQL(sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, 0));
    for (i = 0; i < n; ++i) {
      CHECK_DB(sqlite3_bind_int64(stmt, 1, v[i].ip));
      CHECK_DB(sqlite3_bind_text(stmt, 2, v[i].name, -1, SQLITE_TRANSIENT));
      CHECK_DB(sqlite3_bind_int64(stmt, 3, v[i].created));
      CHECK_DB(sqlite3_bind_int64(stmt, 3, v[i].created));
      CHECK_DB((rc = sqlite3_step(stmt)) == SQLITE_DONE ? SQLITE_OK : rc);
      CHECK_DB(sqlite3_reset(stmt));
    }
    CHECK_SQL(sqlite3_exec(db, "COMMIT TRANSACTION", 0, 0, 0));
    DEBUG("Committed %d claims\n", n);
    nsync_mu_lock(&g_recent.mu);
    nsync_cv_signal(&g_recent.cv);
    nsync_mu_unlock(&g_recent.mu);
  }
  CHECK_DB(sqlite3_finalize(stmt));
  CHECK_SQL(sqlite3_close(db));
  LOG("ClaimWorker exiting\n");
  return 0;
OnError:
  sqlite3_finalize(stmt);
  sqlite3_close(db);
  stmt = 0;
  db = 0;
  usleep(100 * 1000);
  goto StartOver;
}

// single thread for computing HTTP Date header
void *NowWorker(void *arg) {
  nsync_time deadline;
  OnlyRunOnCpu(0);
  pthread_setname_np(pthread_self(), "NowWorker");
  for (deadline = _timespec_real();;) {
    deadline = _timespec_add(deadline, _timespec_frommillis(DATE_UPDATE_MS));
    if (!nsync_note_wait(g_shutdown, deadline)) {
      UpdateNow();
    } else {
      break;
    }
  }
  return 0;
}

// we're permissive in allowing http connection keepalive until the
// moment worker resources start becoming scarce. when that happens
// we'll (1) cancel read operations that have not sent us a message
// in a while; (2) cancel clients who are sending lots of messages.
void Meltdown(void) {
  int i, marks;
  struct timespec now;
  LOG("Panicking because %d out of %d workers is connected\n", g_connections,
      g_workers);
  now = _timespec_real();
  for (marks = i = 0; i < g_workers; ++i) {
    if (g_worker[i].connected &&
        (g_worker[i].msgcount > PANIC_MSGS ||
         _timespec_gte(_timespec_sub(now, g_worker[i].startread),
                       _timespec_frommillis(MELTALIVE_MS)))) {
      tkill(pthread_getunique_np(g_worker[i].th), SIGUSR1);
      ++marks;
    }
  }
  LOG("Melted down %d connections\n", marks);
}

// main thread worker
void *Supervisor(void *arg) {
  nsync_time deadline;
  OnlyRunOnCpu(0);
  pthread_setname_np(pthread_self(), "Supervisor");
  for (deadline = _timespec_real();;) {
    deadline = _timespec_add(deadline, _timespec_frommillis(POLL_ASSETS_MS));
    if (!nsync_note_wait(g_shutdown, deadline)) {
      if (g_workers > 1 && 1. / g_workers * g_connections > PANIC_LOAD) {
        Meltdown();
      }
      ReloadAsset(&g_asset.index);
      ReloadAsset(&g_asset.about);
      ReloadAsset(&g_asset.user);
      ReloadAsset(&g_asset.favicon);
    } else {
      break;
    }
  }
  return 0;
}

int main(int argc, char *argv[]) {
  // ShowCrashReports();
  GetOpts(argc, argv);

  __enable_threads();
  sqlite3_initialize();
  g_shutdown = nsync_note_new(0, nsync_time_no_deadline);
  g_terminate = nsync_note_new(0, nsync_time_no_deadline);

  CHECK_EQ(0, chdir("/opt/turfwar"));
  putenv("TMPDIR=/opt/turfwar/tmp");
  g_asset.index = LoadAsset("index.html", "text/html; charset=utf-8");
  g_asset.about = LoadAsset("about.html", "text/html; charset=utf-8");
  g_asset.user = LoadAsset("user.html", "text/html; charset=utf-8");
  g_asset.favicon = LoadAsset("favicon.ico", "image/vnd.microsoft.icon");

  CHECK_EQ(0, unveil("/opt/turfwar", "rwc"));
  CHECK_EQ(0, unveil(0, 0));
  __pledge_mode = PLEDGE_PENALTY_RETURN_EPERM;
  CHECK_EQ(0, pledge("stdio flock rpath wpath cpath inet", 0));

  // signal handling
  struct sigaction sa;
  sa.sa_flags = 0;
  sa.sa_handler = OnCtrlC;
  sigfillset(&sa.sa_mask);
  sigaction(SIGHUP, &sa, 0);
  sigaction(SIGINT, &sa, 0);
  sigaction(SIGTERM, &sa, 0);
  sa.sa_handler = IgnoreSignal;
  sigaction(SIGUSR1, &sa, 0);

  // create threads
  pthread_t scorer;
  CHECK_EQ(1, GenerateScore(&g_asset.score, -1));
  CHECK_EQ(0, pthread_create(&scorer, 0, ScoreWorker, 0));
  pthread_t scorer_hour;
  CHECK_EQ(1, GenerateScore(&g_asset.score_hour, 60L * 60));
  CHECK_EQ(0, pthread_create(&scorer_hour, 0, ScoreHourWorker, 0));
  pthread_t scorer_day;
  CHECK_EQ(1, GenerateScore(&g_asset.score_day, 60L * 60 * 24));
  CHECK_EQ(0, pthread_create(&scorer_day, 0, ScoreDayWorker, 0));
  pthread_t scorer_week;
  CHECK_EQ(1, GenerateScore(&g_asset.score_week, 60L * 60 * 24 * 7));
  CHECK_EQ(0, pthread_create(&scorer_week, 0, ScoreWeekWorker, 0));
  pthread_t scorer_month;
  CHECK_EQ(1, GenerateScore(&g_asset.score_month, 60L * 60 * 24 * 30));
  CHECK_EQ(0, pthread_create(&scorer_month, 0, ScoreMonthWorker, 0));
  pthread_t recentr;
  CHECK_EQ(1, GenerateRecent(&g_asset.recent, -1));
  CHECK_EQ(0, pthread_create(&recentr, 0, RecentWorker, 0));
  pthread_t claimer;
  CHECK_EQ(0, pthread_create(&claimer, 0, ClaimWorker, 0));
  pthread_t nower;
  UpdateNow();
  CHECK_EQ(0, pthread_create(&nower, 0, NowWorker, 0));
  g_worker = _gc(xcalloc(g_workers, sizeof(*g_worker)));
  for (intptr_t i = 0; i < g_workers; ++i) {
    LOG("Starting http worker #%d", i);
    CHECK_EQ(0, pthread_create(&g_worker[i].th, 0, HttpWorker, (void *)i));
  }

  // time to serve
  LOG("Hello\n");
  Supervisor(0);

  // cancel accept and read for fast shutdown
  LOG("Interrupting workers...\n");
  for (int i = 0; i < g_workers; ++i) {
    tkill(pthread_getunique_np(g_worker[i].th), SIGUSR1);
  }

  // wait for producers to finish
  LOG("Waiting for workers to finish...\n");
  for (int i = 0; i < g_workers; ++i) {
    CHECK_EQ(0, pthread_join(g_worker[i].th, 0));
  }
  LOG("Waiting for helpers to finish...\n");
  CHECK_EQ(0, pthread_join(recentr, 0));
  CHECK_EQ(0, pthread_join(scorer, 0));
  CHECK_EQ(0, pthread_join(scorer_hour, 0));
  CHECK_EQ(0, pthread_join(scorer_day, 0));
  CHECK_EQ(0, pthread_join(scorer_week, 0));
  CHECK_EQ(0, pthread_join(scorer_month, 0));
  CHECK_EQ(0, pthread_join(nower, 0));

  // wait for consumers to finish
  LOG("Waiting for queue to empty...\n");
  nsync_note_notify(g_terminate);
  CHECK_EQ(0, pthread_join(claimer, 0));
  CHECK_EQ(0, g_claims.count);

  // free memory
  LOG("Freeing memory...\n");
  FreeAsset(&g_asset.user);
  FreeAsset(&g_asset.about);
  FreeAsset(&g_asset.index);
  FreeAsset(&g_asset.score);
  FreeAsset(&g_asset.score_hour);
  FreeAsset(&g_asset.score_day);
  FreeAsset(&g_asset.score_week);
  FreeAsset(&g_asset.score_month);
  FreeAsset(&g_asset.recent);
  FreeAsset(&g_asset.favicon);
  nsync_note_free(g_terminate);
  nsync_note_free(g_shutdown);
  // CheckForMemoryLeaks();

  LOG("Goodbye\n");
}
