/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/sysinfo.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/bsr.h"
#include "libc/intrin/hilbert.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.internal.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/mem/sortedints.internal.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/paths.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/sysconf.h"
#include "libc/serialize.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/stdio/append.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/slice.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/rusage.h"
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
#include "libc/zip.internal.h"
#include "net/http/escape.h"
#include "net/http/http.h"
#include "net/http/ip.h"
#include "net/http/tokenbucket.h"
#include "net/http/url.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/nsync/counter.h"
#include "third_party/nsync/cv.h"
#include "third_party/nsync/mu.h"
#include "third_party/nsync/note.h"
#include "third_party/nsync/time.h"
#include "third_party/sqlite3/sqlite3.h"
#include "third_party/stb/stb_image_write.h"
#include "third_party/zlib/zconf.h"
#include "third_party/zlib/zlib.h"
#include "tool/net/lfuncs.h"

/**
 * @fileoverview production webserver for turfwar online game
 */

#define PORT               8080    // default server listening port
#define CPUS               64      // number of cpus to actually use
#define XN                 64      // plot width in pixels
#define YN                 64      // plot height in pixels
#define WORKERS            500     // size of http client thread pool
#define SUPERVISE_MS       1000    // how often to stat() asset files
#define KEEPALIVE_MS       60000   // max time to keep idle conn open
#define MELTALIVE_MS       2000    // panic keepalive under heavy load
#define SCORE_H_UPDATE_MS  10000   // how often to regenerate /score/hour
#define SCORE_D_UPDATE_MS  30000   // how often to regenerate /score/day
#define SCORE_W_UPDATE_MS  70000   // how often to regenerate /score/week
#define SCORE_M_UPDATE_MS  100000  // how often to regenerate /score/month
#define SCORE_UPDATE_MS    210000  // how often to regenerate /score
#define PLOTS_UPDATE_MS    999000  // how often to regenerate /plot/xxx
#define ACCEPT_DEADLINE_MS 100     // how long accept() can take to find worker
#define CLAIM_DEADLINE_MS  100     // how long /claim may block if queue is full
#define CONCERN_LOAD       .75     // avoid keepalive, upon this connection load
#define PANIC_LOAD         .85     // meltdown if this percent of pool connected
#define PANIC_MSGS         10      // msgs per conn can't exceed it in meltdown
#define QUEUE_MAX          800     // maximum pending claim items in queue
#define BATCH_MAX          64      // max claims to insert per transaction
#define NICK_MAX           40      // max length of user nickname string
#define TB_INTERVAL        1000    // millis between token replenishes
#define TB_CIDR            24      // token bucket cidr specificity
#define SOCK_MAX           100     // max length of socket queue
#define MSG_BUF            512     // small response lookaside

#define INBUF_SIZE  FRAMESIZE
#define OUTBUF_SIZE 8192

#define TB_BYTES (1u << TB_CIDR)
#define TB_WORDS (TB_BYTES / 8)

#define GETOPTS "idvp:w:k:W:"
#define USAGE \
  "\
Usage: turfwar.com [-dv] ARGS...\n\
  -i          integrity check and vacuum at startup\n\
  -d          daemonize\n\
  -v          verbosity\n\
  -W IP       whitelist\n\
  -p INT      port\n\
  -w INT      workers\n\
  -k INT      keepalive\n\
"

#define STANDARD_RESPONSE_HEADERS \
  "Server: turfwar\r\n"           \
  "Referrer-Policy: origin\r\n"   \
  "Access-Control-Allow-Origin: *\r\n"

#define MS2CASH(x)      (x / 1000 / 2)
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

// logging is line-buffered when LOG("foo\n") is used
// log lines show ephemerally when LOG("foo") is used
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

// cosmo's CHECK_EQ() macros are designed to succeed or die
// these macros are similar but designed to return on error
#define CHECK_MEM(x)                        \
  do {                                      \
    if (!CheckMem(__FILE__, __LINE__, x)) { \
      ++g_memfails;                         \
      goto OnError;                         \
    }                                       \
  } while (0)
#define CHECK_SYS(x)                        \
  do {                                      \
    if (!CheckSys(__FILE__, __LINE__, x)) { \
      ++g_sysfails;                         \
      goto OnError;                         \
    }                                       \
  } while (0)
#define CHECK_SQL(x)                        \
  do {                                      \
    int e = errno;                          \
    if (!CheckSql(__FILE__, __LINE__, x)) { \
      ++g_dbfails;                          \
      goto OnError;                         \
    }                                       \
    errno = e;                              \
  } while (0)
#define CHECK_DB(x)                            \
  do {                                         \
    int e = errno;                             \
    if (!CheckDb(__FILE__, __LINE__, x, db)) { \
      ++g_dbfails;                             \
      goto OnError;                            \
    }                                          \
    errno = e;                                 \
  } while (0)

// mandatory header for gzip payloads
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

// 1x1 pixel transparent gif data
static const char kPixel[43] =
    "\x47\x49\x46\x38\x39\x61\x01\x00\x01\x00\x80\x00\x00\xff\xff\xff"
    "\x00\x00\x00\x21\xf9\x04\x01\x00\x00\x00\x00\x2c\x00\x00\x00\x00"
    "\x01\x00\x01\x00\x00\x02\x02\x44\x01\x00\x3b";

struct Data {
  char *p;
  size_t n;
};

struct Asset {
  int cash;
  char *path;
  nsync_mu lock;
  const char *type;
  struct Data data;
  struct Data gzip;
  struct timespec mtim;
  char lastmodified[32];
};

struct Blackhole {
  struct sockaddr_un addr;
  int fd;
} g_blackhole = {{
    AF_UNIX,
    "/var/run/blackhole.sock",
}};

// cli flags
bool g_integrity;
bool g_daemonize;
int g_port = PORT;
int g_workers = WORKERS;
int g_keepalive = KEEPALIVE_MS;
struct SortedInts g_whitelisted;

// lifecycle vars
pthread_t g_listener;
nsync_time g_started;
nsync_counter g_ready;
atomic_int g_connections;
nsync_note g_shutdown[3];
int g_hilbert[YN * XN][2];

// whitebox metrics
atomic_long g_banned;
atomic_long g_accepts;
atomic_long g_dbfails;
atomic_long g_proxied;
atomic_long g_messages;
atomic_long g_memfails;
atomic_long g_sysfails;
atomic_long g_rejected;
atomic_long g_unproxied;
atomic_long g_readfails;
atomic_long g_notfounds;
atomic_long g_meltdowns;
atomic_long g_parsefails;
atomic_long g_iprequests;
atomic_long g_queuefulls;
atomic_long g_htmlclaims;
atomic_long g_ratelimits;
atomic_long g_emptyclaims;
atomic_long g_acceptfails;
atomic_long g_badversions;
atomic_long g_plainclaims;
atomic_long g_imageclaims;
atomic_long g_invalidnames;
atomic_long g_ipv6forwards;
atomic_long g_assetrequests;
atomic_long g_claimrequests;
atomic_long g_claimsenqueued;
atomic_long g_claimsprocessed;
atomic_long g_statuszrequests;

union TokenBucket {
  atomic_schar *b;
  atomic_uint_fast64_t *w;
} g_tok;

// http worker objects
struct Worker {
  pthread_t th;
  atomic_int msgcount;
  atomic_int shutdown;
  atomic_int connected;
  struct timespec startread;
} *g_worker;

// recentworker wakeup
struct Recent {
  nsync_mu mu;
  nsync_cv cv;
} g_recent;

// global date header
struct Nowish {
  nsync_mu lock;
  struct timespec ts;
  struct tm tm;
} g_nowish;

// static assets
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
  struct Asset plot[256];
} g_asset;

// queues ListenWorker() to HttpWorker()
struct Clients {
  int pos;
  int count;
  nsync_mu mu;
  nsync_cv non_full;
  nsync_cv non_empty;
  struct Client {
    int sock;
    uint32_t size;
    struct sockaddr_in addr;
  } data[SOCK_MAX];
} g_clients;

// queues /claim to ClaimWorker()
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

long GetTotalRam(void) {
  struct sysinfo si;
  si.totalram = 256 * 1024 * 1024;
  sysinfo(&si);
  return si.totalram;
}

// easy string sender
ssize_t Write(int fd, const char *s) {
  return write(fd, s, strlen(s));
}

// turns relative timeout into an absolute timeout
struct timespec WaitFor(int millis) {
  return timespec_add(timespec_real(), timespec_frommillis(millis));
}

// helper functions for check macro implementation
bool CheckMem(const char *file, int line, void *ptr) {
  if (ptr) return true;
  kprintf("%s:%d: %P: out of memory: %s\n", file, line, strerror(errno));
  return false;
}
bool CheckSys(const char *file, int line, long rc) {
  if (rc != -1) return true;
  kprintf("%s:%d: %P: %s\n", file, line, strerror(errno));
  return false;
}
bool CheckSql(const char *file, int line, int rc) {
  if (rc == SQLITE_OK) return true;
  kprintf("%s:%d: %P: %s\n", file, line, sqlite3_errstr(rc));
  return false;
}
bool CheckDb(const char *file, int line, int rc, sqlite3 *db) {
  if (rc == SQLITE_OK) return true;
  kprintf("%s:%d: %P: %s: %s\n", file, line, sqlite3_errstr(rc),
          sqlite3_errmsg(db));
  return false;
}

// if we try to open a WAL database at the same time from multiple
// threads then it's likely we'll get a SQLITE_BUSY conflict since
// WAL mode does a complicated dance to initialize itself thus all
// we need to do is wait a little bit, and use exponential backoff
int DbOpen(const char *path, sqlite3 **db) {
  int i, rc;
  char sql[128];
  rc = sqlite3_open(path, db);
  if (rc != SQLITE_OK) return rc;
  if (!IsWindows() && !IsOpenbsd()) {
    ksnprintf(sql, sizeof(sql), "PRAGMA mmap_size=%ld", GetTotalRam());
    rc = sqlite3_exec(*db, sql, 0, 0, 0);
    if (rc != SQLITE_OK) return rc;
  }
  for (i = 0; i < 7; ++i) {
    rc = sqlite3_exec(*db, "PRAGMA journal_mode=WAL", 0, 0, 0);
    if (rc == SQLITE_OK) break;
    if (rc != SQLITE_BUSY) return rc;
    usleep(1000L << i);
  }
  return sqlite3_exec(*db, "PRAGMA synchronous=NORMAL", 0, 0, 0);
}

int DbStep(sqlite3_stmt *stmt) {
  int i, rc;
  for (i = 0; i < 12; ++i) {
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) break;
    if (rc == SQLITE_DONE) break;
    if (rc != SQLITE_BUSY) return rc;
    usleep(1000L << i);
  }
  return rc;
}

// why not make the statement prepare api a little less hairy too
int DbPrepare(sqlite3 *db, sqlite3_stmt **stmt, const char *sql) {
  return sqlite3_prepare_v2(db, sql, -1, stmt, 0);
}

bool Blackhole(uint32_t ip) {
  char buf[4];
  WRITE32BE(buf, ip);
  if (sendto(g_blackhole.fd, buf, 4, 0, (struct sockaddr *)&g_blackhole.addr,
             sizeof(g_blackhole.addr)) == 4) {
    return true;
  } else {
    kprintf("error: sendto(%#s) failed: %s\n", g_blackhole.addr.sun_path,
            strerror(errno));
    return false;
  }
}

// validates name registration validity
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

// turn unix timestamp into string the easy way
char *FormatUnixHttpDateTime(char *s, int64_t t) {
  struct tm tm;
  gmtime_r(&t, &tm);
  FormatHttpDateTime(s, &tm);
  return s;
}

// gmtime_r() does a shocking amount of compute
// so we try to handle that globally right here
void UpdateNow(void) {
  int64_t secs;
  struct tm tm;
  g_nowish.ts = timespec_real();
  secs = g_nowish.ts.tv_sec;
  gmtime_r(&secs, &tm);
  //!//!//!//!//!//!//!//!//!//!//!//!//!/
  nsync_mu_lock(&g_nowish.lock);
  g_nowish.tm = tm;
  nsync_mu_unlock(&g_nowish.lock);
  //!//!//!//!//!//!//!//!//!//!//!//!//!/
}

// the standard strftime() function is dismally slow
// this function is non-generalized for just http so
// it needs 25 cycles rather than 709 cycles so cool
char *FormatDate(char *p) {
  ////////////////////////////////////////
  nsync_mu_rlock(&g_nowish.lock);
  p = FormatHttpDateTime(p, &g_nowish.tm);
  nsync_mu_runlock(&g_nowish.lock);
  ////////////////////////////////////////
  return p;
}

bool AddClient(struct Clients *q, const struct Client *v, nsync_time dead) {
  bool wake = false;
  bool added = false;
  nsync_mu_lock(&q->mu);
  while (q->count == ARRAYLEN(q->data)) {
    if (nsync_cv_wait_with_deadline(&q->non_full, &q->mu, dead,
                                    g_shutdown[0])) {
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

int GetClient(struct Clients *q, struct Client *out) {
  int got = 0;
  int len = 1;
  nsync_mu_lock(&q->mu);
  while (!q->count) {
    if (nsync_cv_wait_with_deadline(&q->non_empty, &q->mu,
                                    nsync_time_no_deadline, g_shutdown[1])) {
      break;  // must be ECANCELED
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

// inserts ip:name claim into blocking message queue
// may be interrupted by absolute deadline
// may be cancelled by server shutdown
bool AddClaim(struct Claims *q, const struct Claim *v, nsync_time dead) {
  bool wake = false;
  bool added = false;
  nsync_mu_lock(&q->mu);
  while (q->count == ARRAYLEN(q->data)) {
    if (nsync_cv_wait_with_deadline(&q->non_full, &q->mu, dead,
                                    g_shutdown[1])) {
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

// removes batch of ip:name claims from blocking message queue
// has no deadline or cancellation; enqueued must be processed
int GetClaims(struct Claims *q, struct Claim *out, int len) {
  int got = 0;
  nsync_mu_lock(&q->mu);
  while (!q->count) {
    if (nsync_cv_wait_with_deadline(&q->non_empty, &q->mu,
                                    nsync_time_no_deadline, g_shutdown[2])) {
      break;  // must be ECANCELED
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

// parses request uri query string and extracts ?name=value
static bool GetNick(char *inbuf, struct HttpMessage *msg, struct Claim *v) {
  size_t i;
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

// allocates memory with hardware-accelerated buffer overflow detection
// so if it gets hacked it'll at least crash instead of get compromised
void *NewSafeBuffer(size_t n) {
  char *p;
  long pagesize = sysconf(_SC_PAGESIZE);
  size_t m = ROUNDUP(n, pagesize);
  npassert((p = valloc(m + pagesize)));
  npassert(!mprotect(p + m, pagesize, PROT_NONE));
  return p;
}

// frees memory with hardware-accelerated buffer overflow detection
void FreeSafeBuffer(void *p) {
  long pagesize = sysconf(_SC_PAGESIZE);
  size_t n = malloc_usable_size(p);
  size_t m = ROUNDDOWN(n, pagesize);
  npassert(!mprotect(p, m, PROT_READ | PROT_WRITE));
  free(p);
}

// signals by default get delivered to any random thread
// solution is to block every signal possible in threads
void BlockSignals(void) {
  sigset_t mask;
  sigfillset(&mask);
  sigprocmask(SIG_SETMASK, &mask, 0);
}

// main thread uses sigusr1 to deliver io cancellations
void AllowSigusr1(void) {
  sigset_t mask;
  sigfillset(&mask);
  sigdelset(&mask, SIGUSR1);
  sigprocmask(SIG_SETMASK, &mask, 0);
}

char *Statusz(char *p, const char *s, long x) {
  p = stpcpy(p, s);
  p = stpcpy(p, ": ");
  p = FormatInt64(p, x);
  p = stpcpy(p, "\n");
  return p;
}

// public /statusz endpoint for monitoring server internals
void ServeStatusz(int client, char *outbuf) {
  char *p;
  struct rusage ru;
  struct timespec now;
  now = timespec_real();
  p = outbuf;
  p = stpcpy(outbuf, "HTTP/1.1 200 OK\r\n"
                     "Content-Type: text/plain\r\n"
                     "Cache-Control: max-age=0, must-revalidate\r\n"
                     "Connection: close\r\n"
                     "\r\n");
  p = Statusz(p, "qps",
              g_messages / MAX(1, timespec_sub(now, g_started).tv_sec));
  p = Statusz(p, "started", g_started.tv_sec);
  p = Statusz(p, "now", now.tv_sec);
  p = Statusz(p, "messages", g_messages);
  p = Statusz(p, "connections", g_connections);
  p = Statusz(p, "banned", g_banned);
  p = Statusz(p, "workers", g_workers);
  p = Statusz(p, "accepts", g_accepts);
  p = Statusz(p, "dbfails", g_dbfails);
  p = Statusz(p, "proxied", g_proxied);
  p = Statusz(p, "memfails", g_memfails);
  p = Statusz(p, "sysfails", g_sysfails);
  p = Statusz(p, "rejected", g_rejected);
  p = Statusz(p, "unproxied", g_unproxied);
  p = Statusz(p, "readfails", g_readfails);
  p = Statusz(p, "notfounds", g_notfounds);
  p = Statusz(p, "meltdowns", g_meltdowns);
  p = Statusz(p, "parsefails", g_parsefails);
  p = Statusz(p, "iprequests", g_iprequests);
  p = Statusz(p, "queuefulls", g_queuefulls);
  p = Statusz(p, "htmlclaims", g_htmlclaims);
  p = Statusz(p, "ratelimits", g_ratelimits);
  p = Statusz(p, "emptyclaims", g_emptyclaims);
  p = Statusz(p, "acceptfails", g_acceptfails);
  p = Statusz(p, "badversions", g_badversions);
  p = Statusz(p, "plainclaims", g_plainclaims);
  p = Statusz(p, "imageclaims", g_imageclaims);
  p = Statusz(p, "invalidnames", g_invalidnames);
  p = Statusz(p, "ipv6forwards", g_ipv6forwards);
  p = Statusz(p, "assetrequests", g_assetrequests);
  p = Statusz(p, "claimrequests", g_claimrequests);
  p = Statusz(p, "claimsenqueued", g_claimsenqueued);
  p = Statusz(p, "claimsprocessed", g_claimsprocessed);
  p = Statusz(p, "statuszrequests", g_statuszrequests);
  if (!getrusage(RUSAGE_SELF, &ru)) {
    p = Statusz(p, "ru_utime.tv_sec", ru.ru_utime.tv_sec);
    p = Statusz(p, "ru_utime.tv_usec", ru.ru_utime.tv_usec);
    p = Statusz(p, "ru_stime.tv_sec", ru.ru_stime.tv_sec);
    p = Statusz(p, "ru_stime.tv_usec", ru.ru_stime.tv_usec);
    p = Statusz(p, "ru_maxrss", ru.ru_maxrss);
    p = Statusz(p, "ru_ixrss", ru.ru_ixrss);
    p = Statusz(p, "ru_idrss", ru.ru_idrss);
    p = Statusz(p, "ru_isrss", ru.ru_isrss);
    p = Statusz(p, "ru_minflt", ru.ru_minflt);
    p = Statusz(p, "ru_majflt", ru.ru_majflt);
    p = Statusz(p, "ru_nswap", ru.ru_nswap);
    p = Statusz(p, "ru_inblock", ru.ru_inblock);
    p = Statusz(p, "ru_oublock", ru.ru_oublock);
    p = Statusz(p, "ru_msgsnd", ru.ru_msgsnd);
    p = Statusz(p, "ru_msgrcv", ru.ru_msgrcv);
    p = Statusz(p, "ru_nsignals", ru.ru_nsignals);
    p = Statusz(p, "ru_nvcsw", ru.ru_nvcsw);
    p = Statusz(p, "ru_nivcsw", ru.ru_nivcsw);
  }
  write(client, outbuf, p - outbuf);
}

void *ListenWorker(void *arg) {
  int server;
  int no = 0;
  int yes = 1;
  int fastopen = 5;
  struct Client client;
  struct timeval timeo = {g_keepalive / 1000, g_keepalive % 1000};
  struct sockaddr_in addr = {.sin_family = AF_INET, .sin_port = htons(g_port)};
  AllowSigusr1();
  pthread_setname_np(pthread_self(), "Listener");
  CHECK_NE(-1, (server = socket(AF_INET, SOCK_STREAM, 0)));
  setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, &timeo, sizeof(timeo));
  setsockopt(server, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo));
  setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
  setsockopt(server, SOL_TCP, TCP_FASTOPEN, &fastopen, sizeof(fastopen));
  setsockopt(server, SOL_TCP, TCP_QUICKACK, &no, sizeof(no));
  setsockopt(server, SOL_TCP, TCP_CORK, &no, sizeof(no));
  setsockopt(server, SOL_TCP, TCP_NODELAY, &yes, sizeof(yes));
  bind(server, (struct sockaddr *)&addr, sizeof(addr));
  CHECK_NE(-1, listen(server, 1));
  while (!nsync_note_is_notified(g_shutdown[0])) {
    client.size = sizeof(client.addr);
    client.sock = accept(server, (struct sockaddr *)&client.addr, &client.size);
    if (client.sock == -1) {
      if (errno != EAGAIN) {  // spinning on SO_RCVTIMEO
        ++g_acceptfails;
      }
      continue;
    }
    if (!AddClient(&g_clients, &client, WaitFor(ACCEPT_DEADLINE_MS))) {
      ++g_rejected;
      LOG("503 Accept Queue Full\n");
      Write(client.sock, "HTTP/1.1 503 Accept Queue Full\r\n"
                         "Content-Type: text/plain\r\n"
                         "Connection: close\r\n"
                         "\r\n"
                         "Accept Queue Full\n");
      close(client.sock);
    }
  }
  close(server);
  nsync_note_notify(g_shutdown[1]);
  return 0;
}

// make thousands of http client handler threads
// load balance incoming connections for port 8080 across all threads
// hangup on any browser clients that lag for more than a few seconds
void *HttpWorker(void *arg) {
  struct Client client;
  int id = (intptr_t)arg;
  char *msgbuf = gc(xmalloc(MSG_BUF));
  char *inbuf = NewSafeBuffer(INBUF_SIZE);
  char *outbuf = NewSafeBuffer(OUTBUF_SIZE);
  struct HttpMessage *msg = gc(xcalloc(1, sizeof(struct HttpMessage)));

  BlockSignals();
  pthread_setname_np(pthread_self(), gc(xasprintf("HTTP%d", id)));

  // connection loop
  while (GetClient(&g_clients, &client)) {
    struct Data d;
    ssize_t got, sent;
    uint32_t ip, clientip;
    int tok, inmsglen, outmsglen;
    char ipbuf[32], *p, *q, cashbuf[64];

    clientip = ntohl(client.addr.sin_addr.s_addr);
    g_worker[id].connected = true;
    g_worker[id].msgcount = 0;
    ++g_accepts;
    ++g_connections;

    // simple http/1.1 message loop
    // let's assume we're behind a well-behaved frontend
    // each read() should give us just *one* HTTP message
    // if we get less than one message, we drop connection
    // if we get more than one message, we Connection: close
    // let's not bother with cray proto stuff like 100-expect
    do {
      struct Asset *a;
      bool comp, ipv6;

      // wait for http message
      // this may be cancelled by sigusr1
      AllowSigusr1();
      DestroyHttpMessage(msg);
      InitHttpMessage(msg, kHttpRequest);
      g_worker[id].startread = timespec_real();
      if ((got = read(client.sock, inbuf, INBUF_SIZE)) <= 0) {
        ++g_readfails;
        break;
      }
      BlockSignals();

      // parse http message
      // we're only doing one-shot parsing right now
      if ((inmsglen = ParseHttpMessage(msg, inbuf, got)) <= 0) {
        ++g_parsefails;
        break;
      }
      ++g_messages;
      ++g_worker[id].msgcount;

      ipv6 = false;
      ip = clientip;

      // get client address from frontend
      if (HasHeader(kHttpXForwardedFor)) {
        if (!IsLoopbackIp(clientip) &&  //
            !IsPrivateIp(clientip) &&   //
            !IsCloudflareIp(clientip)) {
          LOG("Got X-Forwarded-For from untrusted IPv4 client address "
              "%hhu.%hhu.%hhu.%hhu\n",
              clientip >> 24, clientip >> 16, clientip >> 8, clientip);
          ipv6 = false;
          ip = clientip;
          ++g_unproxied;
        } else if (ParseForwarded(HeaderData(kHttpXForwardedFor),
                                  HeaderLength(kHttpXForwardedFor), &ip,
                                  0) != -1) {
          ipv6 = false;
          ++g_proxied;
        } else {
          ipv6 = true;
          ip = clientip;
          ++g_ipv6forwards;
          ++g_proxied;
        }
      } else {
        ipv6 = false;
        ip = clientip;
        ++g_unproxied;
      }

      ksnprintf(ipbuf, sizeof(ipbuf), "%hhu.%hhu.%hhu.%hhu", ip >> 24, ip >> 16,
                ip >> 8, ip);

      if (UrlStartsWith("/plot/") && (_rand64() % 256)) {
        goto SkipSecurity;
      }
      if (!ipv6 && !ContainsInt(&g_whitelisted, ip) &&
          (tok = AcquireToken(g_tok.b, ip, TB_CIDR)) < 32) {
        if (tok > 4) {
          LOG("%s rate limiting client\n", ipbuf, msg->version);
          Write(client.sock, "HTTP/1.1 429 Too Many Requests\r\n"
                             "Content-Type: text/plain\r\n"
                             "Connection: close\r\n"
                             "\r\n"
                             "429 Too Many Requests\n");
        } else {
          Blackhole(ip);
          ++g_banned;
        }
        ++g_ratelimits;
        break;
      }
    SkipSecurity:

      // we don't support http/1.0 and http/0.9 right now
      if (msg->version != 11) {
        LOG("%s used unsupported http/%d version\n", ipbuf, msg->version);
        Write(client.sock, "HTTP/1.1 505 HTTP Version Not Supported\r\n"
                           "Content-Type: text/plain\r\n"
                           "Connection: close\r\n"
                           "\r\n"
                           "HTTP Version Not Supported\n");
        ++g_badversions;
        break;
      }

      // access log
      LOG("%6P %16s %.*s %.*s %.*s %.*s %#.*s\n", ipbuf,
          msg->xmethod.b - msg->xmethod.a, inbuf + msg->xmethod.a,
          msg->uri.b - msg->uri.a, inbuf + msg->uri.a,
          HeaderLength(kHttpCfIpcountry), HeaderData(kHttpCfIpcountry),
          HeaderLength(kHttpSecChUaPlatform), HeaderData(kHttpSecChUaPlatform),
          HeaderLength(kHttpReferer), HeaderData(kHttpReferer));

      // export monitoring data
      if (UrlEqual("/statusz")) {
        ServeStatusz(client.sock, outbuf);
        ++g_statuszrequests;
        break;
      }

      // asset routing
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
      } else if (UrlStartsWith("/plot/")) {
        int i, block = 0;
        for (i = msg->uri.a + 6; i < msg->uri.b && isdigit(inbuf[i]); ++i) {
          block *= 10;
          block += inbuf[i] - '0';
          block &= 255;
        }
        a = g_asset.plot + block;
      } else {
        a = 0;
      }

      // assert serving
      if (a) {
        struct iovec iov[2];
        ++g_assetrequests;
        comp = a->gzip.n < a->data.n &&
               HeaderHas(msg, inbuf, kHttpAcceptEncoding, "gzip", 4);
        ////////////////////////////////////////
        nsync_mu_rlock(&a->lock);
        if (HasHeader(kHttpIfModifiedSince) &&
            a->mtim.tv_sec <=
                ParseHttpDateTime(HeaderData(kHttpIfModifiedSince),
                                  HeaderLength(kHttpIfModifiedSince))) {
          p = stpcpy(outbuf,
                     "HTTP/1.1 304 Not Modified\r\n" STANDARD_RESPONSE_HEADERS
                     "Vary: Accept-Encoding\r\n"
                     "Date: ");
          p = FormatDate(p);
          p = stpcpy(p, "\r\nLast-Modified: ");
          p = stpcpy(p, a->lastmodified);
          p = stpcpy(p, "\r\nContent-Type: ");
          p = stpcpy(p, a->type);
          p = stpcpy(p, "\r\nCache-Control: ");
          ksnprintf(cashbuf, sizeof(cashbuf), "max-age=%d, must-revalidate",
                    a->cash);
          p = stpcpy(p, cashbuf);
          p = stpcpy(p, "\r\n\r\n");
          outmsglen = p - outbuf;
          sent = write(client.sock, outbuf, outmsglen);
        } else {
          p = stpcpy(outbuf, "HTTP/1.1 200 OK\r\n" STANDARD_RESPONSE_HEADERS
                             "Vary: Accept-Encoding\r\n"
                             "Date: ");
          p = FormatDate(p);
          p = stpcpy(p, "\r\nLast-Modified: ");
          p = stpcpy(p, a->lastmodified);
          p = stpcpy(p, "\r\nContent-Type: ");
          p = stpcpy(p, a->type);
          p = stpcpy(p, "\r\nCache-Control: ");
          ksnprintf(cashbuf, sizeof(cashbuf), "max-age=%d, must-revalidate",
                    a->cash);
          p = stpcpy(p, cashbuf);
          if (comp) p = stpcpy(p, "\r\nContent-Encoding: gzip");
          p = stpcpy(p, "\r\nContent-Length: ");
          d = comp ? a->gzip : a->data;
          p = FormatInt32(p, d.n);
          p = stpcpy(p, "\r\n\r\n");
          iov[0].iov_base = outbuf;
          iov[0].iov_len = p - outbuf;
          iov[1].iov_base = d.p;
          iov[1].iov_len = msg->method == kHttpHead ? 0 : d.n;
          outmsglen = iov[0].iov_len + iov[1].iov_len;
          sent = writev(client.sock, iov, 2);
        }
        nsync_mu_runlock(&a->lock);
        ////////////////////////////////////////

      } else if (UrlStartsWith("/ip")) {
        // what is my ip endpoint
        ++g_iprequests;
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
          sent = write(client.sock, outbuf, outmsglen);
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
          sent = write(client.sock, outbuf, p - outbuf);
          break;
        }

      } else if (UrlStartsWith("/claim")) {
        // ip:name registration endpoint
        ++g_claimrequests;
        if (ipv6) goto Ipv6Warning;
        struct Claim v = {.ip = ip, .created = g_nowish.ts.tv_sec};
        if (GetNick(inbuf, msg, &v)) {
          if (AddClaim(&g_claims, &v,
                       timespec_add(timespec_real(),
                                    timespec_frommillis(CLAIM_DEADLINE_MS)))) {
            ++g_claimsenqueued;
            DEBUG("%s claimed by %s\n", ipbuf, v.name);
            if (HasHeader(kHttpAccept) &&
                (HeaderHas(msg, inbuf, kHttpAccept, "image/*", 7) ||
                 HeaderHas(msg, inbuf, kHttpAccept, "image/gif", 9))) {
              ++g_imageclaims;
              p = stpcpy(outbuf, "HTTP/1.1 200 OK\r\n" STANDARD_RESPONSE_HEADERS
                                 "Vary: Accept\r\n"
                                 "Cache-Control: private\r\n"
                                 "Content-Type: image/gif\r\n"
                                 "Connection: close\r\n"
                                 "Date: ");
              p = FormatDate(p);
              p = stpcpy(p, "\r\nContent-Length: ");
              p = FormatInt32(p, sizeof(kPixel));
              p = stpcpy(p, "\r\n\r\n");
              p = mempcpy(p, kPixel, sizeof(kPixel));
            } else if (HasHeader(kHttpAccept) &&
                       HeaderHas(msg, inbuf, kHttpAccept, "text/plain", 10) &&
                       !HeaderHas(msg, inbuf, kHttpAccept, "text/html", 9)) {
              ++g_plainclaims;
              ksnprintf(msgbuf, MSG_BUF, "The land at %s was claimed for %s\n",
                        ipbuf, v.name);
              q = msgbuf;
              p = stpcpy(outbuf, "HTTP/1.1 200 OK\r\n" STANDARD_RESPONSE_HEADERS
                                 "Vary: Accept\r\n"
                                 "Cache-Control: private\r\n"
                                 "Content-Type: text/plain\r\n"
                                 "Connection: close\r\n"
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
              ++g_htmlclaims;
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
                                 "Connection: close\r\n"
                                 "Date: ");
              p = FormatDate(p);
              p = stpcpy(p, "\r\nContent-Length: ");
              p = FormatInt32(p, strlen(q));
              p = stpcpy(p, "\r\n\r\n");
              p = stpcpy(p, q);
            } else {
              ++g_emptyclaims;
              p = stpcpy(outbuf,
                         "HTTP/1.1 204 No Content\r\n" STANDARD_RESPONSE_HEADERS
                         "Vary: Accept\r\n"
                         "Cache-Control: private\r\n"
                         "Content-Length: 0\r\n"
                         "Connection: close\r\n"
                         "Date: ");
              p = FormatDate(p);
              p = stpcpy(p, "\r\n\r\n");
            }
            outmsglen = p - outbuf;
            sent = write(client.sock, outbuf, p - outbuf);
            break;
          } else {
            LOG("%s: 503 Claims Queue Full\n", ipbuf);
            Write(client.sock, "HTTP/1.1 503 Claims Queue Full\r\n"
                               "Content-Type: text/plain\r\n"
                               "Connection: close\r\n"
                               "\r\n"
                               "Claims Queue Full\n");
            ++g_queuefulls;
            break;
          }
        } else {
          ++g_invalidnames;
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
          sent = write(client.sock, outbuf, p - outbuf);
          break;
        }

      } else {
        // default endpoint
        ++g_notfounds;
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
        sent = write(client.sock, outbuf, p - outbuf);
      }

      // if the client isn't pipelining and write() wrote the full
      // amount, then since we sent the content length and checked
      // that the client didn't attach a payload, we are so synced
      // thus we can safely process more messages
    } while (got == inmsglen &&                                //
             sent == outmsglen &&                              //
             !HasHeader(kHttpContentLength) &&                 //
             !HasHeader(kHttpTransferEncoding) &&              //
             !HeaderEqualCase(kHttpConnection, "close") &&     //
             (msg->method == kHttpGet ||                       //
              msg->method == kHttpHead) &&                     //
             1. / g_workers * g_connections < CONCERN_LOAD &&  //
             !nsync_note_is_notified(g_shutdown[1]));
    DestroyHttpMessage(msg);
    close(client.sock);
    g_worker[id].connected = false;
    --g_connections;
  }

  LOG("HttpWorker #%d exiting", id);
  g_worker[id].shutdown = true;
  FreeSafeBuffer(outbuf);
  FreeSafeBuffer(inbuf);
  return 0;
}

// helper to precompress gzip responses in background
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

// slurps asset off disk once during startup
struct Asset LoadAsset(const char *path, const char *type, int cash) {
  struct stat st;
  struct Asset a = {0};
  CHECK_EQ(0, stat(path, &st));
  CHECK_NOTNULL((a.data.p = xslurp(path, &a.data.n)));
  a.type = type;
  a.cash = cash;
  CHECK_NOTNULL((a.path = strdup(path)));
  a.mtim = st.st_mtim;
  CHECK_NOTNULL((a.gzip = Gzip(a.data)).p);
  FormatUnixHttpDateTime(a.lastmodified, a.mtim.tv_sec);
  return a;
}

// reslurps asset off disk if its mtim changed
bool ReloadAsset(struct Asset *a) {
  int fd;
  void *f[2];
  ssize_t rc;
  struct stat st;
  char lastmodified[32];
  struct Data data = {0};
  struct Data gzip = {0};
  CHECK_SYS((fd = open(a->path, O_RDONLY)));
  CHECK_SYS(fstat(fd, &st));
  if (timespec_cmp(st.st_mtim, a->mtim) > 0) {
    FormatUnixHttpDateTime(lastmodified, st.st_mtim.tv_sec);
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
    memcpy(a->lastmodified, lastmodified, 32);
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

// asynchronous handler of sigint, sigterm, and sighup signals
// this handler is always invoked from within the main thread,
// because our helper and worker threads always block signals.
void OnCtrlC(int sig) {
  if (!nsync_note_is_notified(g_shutdown[0])) {
    LOG("Received %s shutting down...\n", strsignal(sig));
    nsync_note_notify(g_shutdown[0]);
  } else {
    // there's no way to deliver signals to workers atomically, unless
    // we pay the cost of ppoll() which isn't necessary in this design
    // so if a user smashes that ctrl-c then we tkill the workers more
    LOG("Received %s again so sending another volley...\n", strsignal(sig));
    for (int i = 0; i < g_workers; ++i) {
      pthread_kill(g_listener, SIGUSR1);
      if (!g_worker[i].shutdown) {
        pthread_kill(g_worker[i].th, SIGUSR1);
      }
    }
  }
}

// parses cli arguments
void GetOpts(int argc, char *argv[]) {
  int opt;
  int64_t ip;
  while ((opt = getopt(argc, argv, GETOPTS)) != -1) {
    switch (opt) {
      case 'i':
        g_integrity = true;
        break;
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
      case 'W':
        if ((ip = ParseIp(optarg, -1)) != -1) {
          if (InsertInt(&g_whitelisted, ip, true)) {
            LOG("whitelisted %s", optarg);
          }
        } else {
          kprintf("error: could not parse -w %#s IP address\n", optarg);
          _Exit(1);
        }
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

// atomically swaps out asset with newer version
void Update(struct Asset *a, bool gen(struct Asset *, long, long), long x,
            long y) {
  void *f[2];
  struct Asset t;
  if (gen(&t, x, y)) {
    //!//!//!//!//!//!//!//!//!//!//!//!//!/
    nsync_mu_lock(&a->lock);
    f[0] = a->data.p;
    f[1] = a->gzip.p;
    a->data = t.data;
    a->gzip = t.gzip;
    a->mtim = t.mtim;
    a->type = t.type;
    a->cash = t.cash;
    memcpy(a->lastmodified, t.lastmodified, 32);
    nsync_mu_unlock(&a->lock);
    //!//!//!//!//!//!//!//!//!//!//!//!//!/
    free(f[0]);
    free(f[1]);
  }
}

// generator function for the big board
bool GenerateScore(struct Asset *out, long secs, long cash) {
  int rc;
  char *sb = 0;
  sqlite3 *db = 0;
  size_t sblen = 0;
  struct Asset a = {0};
  sqlite3_stmt *stmt = 0;
  bool namestate = false;
  char name1[NICK_MAX + 1] = {0};
  char name2[NICK_MAX + 1];
  DEBUG("GenerateScore %ld\n", secs);
  a.type = "application/json";
  a.cash = cash;
  a.mtim = timespec_real();
  FormatUnixHttpDateTime(a.lastmodified, a.mtim.tv_sec);
  CHECK_SYS(appends(&a.data.p, "{\n"));
  CHECK_SYS(appendf(&a.data.p, "\"now\":[%ld,%ld],\n", a.mtim.tv_sec,
                    a.mtim.tv_nsec));
  CHECK_SYS(appends(&a.data.p, "\"score\":{\n"));
  CHECK_SQL(DbOpen("db.sqlite3", &db));
  if (secs == -1) {
    CHECK_DB(DbPrepare(db, &stmt,
                       "SELECT nick, (ip >> 24), COUNT(*)\n"
                       "FROM land\n"
                       "GROUP BY nick, (ip >> 24)"));
  } else {
    CHECK_DB(DbPrepare(db, &stmt,
                       "SELECT nick, (ip >> 24), COUNT(*)\n"
                       " FROM land\n"
                       "WHERE created NOT NULL\n"
                       "  AND created >= ?1\n"
                       "GROUP BY nick, (ip >> 24)"));
    CHECK_DB(sqlite3_bind_int64(stmt, 1, a.mtim.tv_sec - secs));
  }
  // be sure to always use transactions with sqlite as in always
  // otherwise.. you can use --strace to see the fcntl bloodbath
  CHECK_SQL(sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, 0));
  while ((rc = DbStep(stmt)) != SQLITE_DONE) {
    if (rc != SQLITE_ROW) CHECK_DB(rc);
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

// generator function for the big board
bool GeneratePlot(struct Asset *out, long block, long cash) {
  _Static_assert(IS2POW(XN * YN), "area must be 2-power");
  _Static_assert(XN == YN, "hilbert algorithm needs square");
  int rc, out_len;
  sqlite3 *db = 0;
  struct Asset a = {0};
  unsigned char *rgba;
  sqlite3_stmt *stmt = 0;
  unsigned x, y, i, ip, area, mask, clump;
  DEBUG("GeneratePlot %ld\n", block);
  a.type = "image/png";
  a.cash = cash;
  a.mtim = timespec_real();
  FormatUnixHttpDateTime(a.lastmodified, a.mtim.tv_sec);
  CHECK_MEM((rgba = calloc(4, YN * XN)));
  for (y = 0; y < YN; ++y) {
    for (x = 0; x < XN; ++x) {
      rgba[y * XN * 4 + x * 4 + 0] = 255;
      rgba[y * XN * 4 + x * 4 + 1] = 255;
      rgba[y * XN * 4 + x * 4 + 2] = 255;
    }
  }
  CHECK_SQL(DbOpen("db.sqlite3", &db));
  CHECK_DB(DbPrepare(db, &stmt,
                     "SELECT ip\n"
                     " FROM land\n"
                     "WHERE ip >= ?1\n"
                     "  AND ip <= ?2"));
  CHECK_DB(sqlite3_bind_int64(stmt, 1, block << 24 | 0x000000));
  CHECK_DB(sqlite3_bind_int64(stmt, 2, block << 24 | 0xffffff));
  CHECK_SQL(sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, 0));
  area = XN * YN;
  mask = area - 1;
  clump = 32 - _bsr(area) - 8;
  while ((rc = DbStep(stmt)) != SQLITE_DONE) {
    if (rc != SQLITE_ROW) CHECK_DB(rc);
    ip = sqlite3_column_int64(stmt, 0);
    i = (ip >> clump) & mask;
    y = g_hilbert[i][0];
    x = g_hilbert[i][1];
    if (rgba[y * XN * 4 + x * 4 + 3] < 255) {
      ++rgba[y * XN * 4 + x * 4 + 3];
    }
  }
  CHECK_SQL(sqlite3_exec(db, "END TRANSACTION", 0, 0, 0));
  CHECK_DB(sqlite3_finalize(stmt));
  CHECK_SQL(sqlite3_close(db));
  a.data.p = (char *)stbi_write_png_to_mem(rgba, XN * 4, XN, YN, 4, &out_len);
  a.data.n = out_len;
  a.gzip = Gzip(a.data);
  free(rgba);
  *out = a;
  return true;
OnError:
  sqlite3_finalize(stmt);
  sqlite3_close(db);
  free(a.data.p);
  free(rgba);
  return false;
}

// single thread for regenerating the user scores json
void *ScoreWorker(void *arg) {
  BlockSignals();
  pthread_setname_np(pthread_self(), "ScoreAll");
  LOG("%P Score started\n");
  long wait = SCORE_UPDATE_MS;
  Update(&g_asset.score, GenerateScore, -1, MS2CASH(wait));
  nsync_counter_add(g_ready, -1);  // #1
  do {
    Update(&g_asset.score, GenerateScore, -1, MS2CASH(wait));
  } while (!nsync_note_wait(g_shutdown[1], WaitFor(wait)));
  LOG("Score exiting\n");
  return 0;
}

// single thread for regenerating the user scores json
void *ScoreHourWorker(void *arg) {
  BlockSignals();
  pthread_setname_np(pthread_self(), "ScoreHour");
  LOG("%P ScoreHour started\n");
  long secs = 60L * 60;
  long wait = SCORE_H_UPDATE_MS;
  Update(&g_asset.score_hour, GenerateScore, secs, MS2CASH(wait));
  nsync_counter_add(g_ready, -1);  // #2
  do {
    Update(&g_asset.score_hour, GenerateScore, secs, MS2CASH(wait));
  } while (!nsync_note_wait(g_shutdown[1], WaitFor(wait)));
  LOG("ScoreHour exiting\n");
  return 0;
}

// single thread for regenerating the user scores json
void *ScoreDayWorker(void *arg) {
  BlockSignals();
  pthread_setname_np(pthread_self(), "ScoreDay");
  LOG("%P ScoreDay started\n");
  long secs = 60L * 60 * 24;
  long wait = SCORE_D_UPDATE_MS;
  Update(&g_asset.score_day, GenerateScore, secs, MS2CASH(wait));
  nsync_counter_add(g_ready, -1);  // #3
  do {
    Update(&g_asset.score_day, GenerateScore, secs, MS2CASH(wait));
  } while (!nsync_note_wait(g_shutdown[1], WaitFor(wait)));
  LOG("ScoreDay exiting\n");
  return 0;
}

// single thread for regenerating the user scores json
void *ScoreWeekWorker(void *arg) {
  BlockSignals();
  pthread_setname_np(pthread_self(), "ScoreWeek");
  LOG("%P ScoreWeek started\n");
  long secs = 60L * 60 * 24 * 7;
  long wait = SCORE_W_UPDATE_MS;
  Update(&g_asset.score_week, GenerateScore, secs, MS2CASH(wait));
  nsync_counter_add(g_ready, -1);  // #4
  do {
    Update(&g_asset.score_week, GenerateScore, secs, MS2CASH(wait));
  } while (!nsync_note_wait(g_shutdown[1], WaitFor(wait)));
  LOG("ScoreWeek exiting\n");
  return 0;
}

// single thread for regenerating the user scores json
void *ScoreMonthWorker(void *arg) {
  BlockSignals();
  pthread_setname_np(pthread_self(), "ScoreMonth");
  LOG("%P ScoreMonth started\n");
  long secs = 60L * 60 * 24 * 30;
  long wait = SCORE_M_UPDATE_MS;
  Update(&g_asset.score_month, GenerateScore, secs, MS2CASH(wait));
  nsync_counter_add(g_ready, -1);  // #5
  do {
    Update(&g_asset.score_month, GenerateScore, secs, MS2CASH(wait));
  } while (!nsync_note_wait(g_shutdown[1], WaitFor(wait)));
  LOG("ScoreMonth exiting\n");
  return 0;
}

// single thread for regenerating /8 cell background image charts
void *PlotWorker(void *arg) {
  long i, wait;
  BlockSignals();
  pthread_setname_np(pthread_self(), "Plotter");
  LOG("%P Plotter started\n");
  wait = PLOTS_UPDATE_MS;
  for (i = 0; i < 256; ++i) {
    Update(g_asset.plot + i, GeneratePlot, i, MS2CASH(wait));
  }
  nsync_counter_add(g_ready, -1);  // #6
  do {
    for (i = 0; i < 256; ++i) {
      Update(g_asset.plot + i, GeneratePlot, i, MS2CASH(wait));
    }
  } while (!nsync_note_wait(g_shutdown[1], WaitFor(wait)));
  LOG("Plotter exiting\n");
  return 0;
}

// thread for realtime json generation of recent successful claims
void *RecentWorker(void *arg) {
  bool once;
  void *f[2];
  int rc, err;
  sqlite3 *db;
  char *sb = 0;
  size_t sblen = 0;
  sqlite3_stmt *stmt;
  struct Asset *a, t;
  bool warmedup = false;
  BlockSignals();
  pthread_setname_np(pthread_self(), "RecentWorker");
  LOG("%P RecentWorker started\n");
StartOver:
  db = 0;
  stmt = 0;
  bzero(&t, sizeof(t));
  CHECK_SQL(DbOpen("db.sqlite3", &db));
  CHECK_DB(DbPrepare(db, &stmt,
                     "SELECT ip, nick, created\n"
                     "FROM land\n"
                     "WHERE created NOT NULL\n"
                     "ORDER BY created DESC\n"
                     "LIMIT 50"));
  do {
    // regenerate json
    t.mtim = timespec_real();
    FormatUnixHttpDateTime(t.lastmodified, t.mtim.tv_sec);
    CHECK_SYS(appends(&t.data.p, "{\n"));
    CHECK_SYS(appendf(&t.data.p, "\"now\":[%ld,%ld],\n", t.mtim.tv_sec,
                      t.mtim.tv_nsec));
    CHECK_SYS(appends(&t.data.p, "\"recent\":[\n"));
    CHECK_SQL(sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, 0));
    for (once = false; (rc = DbStep(stmt)) != SQLITE_DONE; once = true) {
      if (rc != SQLITE_ROW) CHECK_SQL(rc);
      if (once) CHECK_SYS(appends(&t.data.p, ",\n"));
      CHECK_SYS(
          appendf(&t.data.p, "[%ld,\"%s\",%ld]", sqlite3_column_int64(stmt, 0),
                  EscapeJsStringLiteral(
                      &sb, &sblen, (void *)sqlite3_column_text(stmt, 1), -1, 0),
                  sqlite3_column_int64(stmt, 2)));
    }
    CHECK_SQL(sqlite3_reset(stmt));
    CHECK_SQL(sqlite3_exec(db, "END TRANSACTION", 0, 0, 0));
    CHECK_SYS(appends(&t.data.p, "]}\n"));
    t.data.n = appendz(t.data.p).i;
    CHECK_MEM((t.gzip = Gzip(t.data)).p);
    // deploy json
    a = &g_asset.recent;
    //!//!//!//!//!//!//!//!//!//!//!//!//!/
    nsync_mu_lock(&a->lock);
    f[0] = a->data.p;
    f[1] = a->gzip.p;
    a->data = t.data;
    a->gzip = t.gzip;
    a->mtim = t.mtim;
    a->type = "application/json";
    a->cash = 0;
    memcpy(a->lastmodified, t.lastmodified, 32);
    nsync_mu_unlock(&a->lock);
    //!//!//!//!//!//!//!//!//!//!//!//!//!/
    bzero(&t, sizeof(t));
    free(f[0]);
    free(f[1]);
    // handle startup condition
    if (!warmedup) {
      nsync_counter_add(g_ready, -1);  // #7
      warmedup = true;
    }
    // wait for wakeup or cancel
    nsync_mu_lock(&g_recent.mu);
    err = nsync_cv_wait_with_deadline(&g_recent.cv, &g_recent.mu,
                                      nsync_time_no_deadline, g_shutdown[1]);
    nsync_mu_unlock(&g_recent.mu);
  } while (err != ECANCELED);
  CHECK_DB(sqlite3_finalize(stmt));
  CHECK_SQL(sqlite3_close(db));
  LOG("RecentWorker exiting\n");
  free(sb);
  return 0;
OnError:
  sqlite3_finalize(stmt);
  sqlite3_close(db);
  free(t.data.p);
  free(t.gzip.p);
  goto StartOver;
}

// single thread for inserting batched claims into the database
// this helps us avoid over 9000 threads having fcntl bloodbath
void *ClaimWorker(void *arg) {
  sqlite3 *db;
  int i, n, rc;
  long processed;
  sqlite3_stmt *stmt;
  bool warmedup = false;
  struct Claim *v = gc(xcalloc(BATCH_MAX, sizeof(struct Claim)));
  BlockSignals();
  pthread_setname_np(pthread_self(), "ClaimWorker");
  LOG("%P ClaimWorker started\n");
StartOver:
  db = 0;
  stmt = 0;
  CHECK_SQL(DbOpen("db.sqlite3", &db));
  CHECK_DB(DbPrepare(db, &stmt,
                     "INSERT INTO land (ip, nick, created)\n"
                     "VALUES (?1, ?2, ?3)\n"
                     "ON CONFLICT (ip) DO\n"
                     "UPDATE SET (nick, created) = (?2, ?3)\n"
                     " WHERE nick != ?2\n"
                     "    OR created IS NULL\n"
                     "    OR ?3 - created > 3600"));
  if (!warmedup) {
    nsync_counter_add(g_ready, -1);  // #8
    warmedup = true;
  }
  while ((n = GetClaims(&g_claims, v, BATCH_MAX))) {
    processed = 0;
    CHECK_SQL(sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, 0));
    for (i = 0; i < n; ++i) {
      CHECK_DB(sqlite3_bind_int64(stmt, 1, v[i].ip));
      CHECK_DB(sqlite3_bind_text(stmt, 2, v[i].name, -1, SQLITE_TRANSIENT));
      CHECK_DB(sqlite3_bind_int64(stmt, 3, v[i].created));
      CHECK_DB(sqlite3_bind_int64(stmt, 3, v[i].created));
      CHECK_DB((rc = DbStep(stmt)) == SQLITE_DONE ? SQLITE_OK : rc);
      CHECK_DB(sqlite3_reset(stmt));
      ++processed;
    }
    CHECK_SQL(sqlite3_exec(db, "COMMIT TRANSACTION", 0, 0, 0));
    atomic_fetch_add(&g_claimsprocessed, processed);
    DEBUG("Committed %d claims\n", n);
    // wake up RecentWorker()
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
  goto StartOver;
}

// single thread for computing HTTP Date header
void *NowWorker(void *arg) {
  BlockSignals();
  pthread_setname_np(pthread_self(), "NowWorker");
  LOG("%P NowWorker started\n");
  UpdateNow();
  nsync_counter_add(g_ready, -1);  // #9
  for (struct timespec ts = {timespec_real().tv_sec};; ++ts.tv_sec) {
    if (!nsync_note_wait(g_shutdown[1], ts)) {
      UpdateNow();
    } else {
      break;
    }
  }
  LOG("NowWorker exiting\n");
  return 0;
}

// worker for refilling token buckets
void *ReplenishWorker(void *arg) {
  BlockSignals();
  pthread_setname_np(pthread_self(), "Replenisher");
  LOG("%P Replenisher started\n");
  UpdateNow();
  for (struct timespec ts = timespec_real();;
       ts = timespec_add(ts, timespec_frommillis(TB_INTERVAL))) {
    if (!nsync_note_wait(g_shutdown[1], ts)) {
      ReplenishTokens(g_tok.w, TB_WORDS);
    } else {
      break;
    }
  }
  LOG("Replenisher exiting\n");
  return 0;
}

// we're permissive in allowing http connection keepalive until the
// moment worker resources start becoming scarce. when that happens
// we'll (1) cancel read operations that have not sent us a message
// in a while; (2) cancel clients who are sending lots of messages.
void Meltdown(void) {
  int i, marks;
  struct timespec now;
  ++g_meltdowns;
  LOG("Panicking because %d out of %d workers is connected\n", g_connections,
      g_workers);
  now = timespec_real();
  for (marks = i = 0; i < g_workers; ++i) {
    if (g_worker[i].connected &&
        (g_worker[i].msgcount > PANIC_MSGS ||
         timespec_cmp(timespec_sub(now, g_worker[i].startread),
                      timespec_frommillis(MELTALIVE_MS)) >= 0)) {
      pthread_kill(g_worker[i].th, SIGUSR1);
      ++marks;
    }
  }
  LOG("Melted down %d connections\n", marks);
}

// main thread worker
void *Supervisor(void *arg) {
  for (;;) {
    if (!nsync_note_wait(g_shutdown[0], WaitFor(SUPERVISE_MS))) {
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

void CheckDatabase(void) {
  sqlite3 *db;
  if (g_integrity) {
    CHECK_SQL(DbOpen("db.sqlite3", &db));
    LOG("Checking database integrity...\n");
    CHECK_SQL(sqlite3_exec(db, "PRAGMA integrity_check", 0, 0, 0));
    LOG("Vacuuming database...\n");
    CHECK_SQL(sqlite3_exec(db, "VACUUM", 0, 0, 0));
    CHECK_SQL(sqlite3_close(db));
  }
  return;
OnError:
  exit(1);
}

int main(int argc, char *argv[]) {
  // ShowCrashReports();

  if (IsLinux()) {
    Write(2, "Enabling TCP_FASTOPEN for server sockets...\n");
    system("sudo sh -c 'echo 3 >/proc/sys/net/ipv4/tcp_fastopen'");
  }

  // we don't have proper futexes on these platforms
  // we'll be somewhat less aggressive about workers
  if (IsXnu() || IsNetbsd()) {
    g_workers = MIN(g_workers, (unsigned)__get_cpu_count());
  }

  // user interface
  GetOpts(argc, argv);
  kprintf("\
 |               _|                    \n\
 __| |   |  __| | \\ \\  \\   / _` |  __|\n\
 |   |   | |    __|\\ \\  \\ / (   | |\n\
\\__|\\__,_|_|   _|   \\_/\\_/ \\__,_|_|\n");
  CHECK_EQ(0, chdir("/opt/turfwar"));
  putenv("TMPDIR=/opt/turfwar/tmp");

  if ((g_blackhole.fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
    kprintf("error: socket(AF_UNIX) failed: %s\n", strerror(errno));
    _Exit(3);
  }
  if (!Blackhole(0)) {
    kprintf("turfwar isn't able to protect your kernel from level 4 ddos\n");
    kprintf("please run the blackholed program, see https://justine.lol/\n");
  }

  // the power to serve
  if (g_daemonize) {
    if (fork() > 0) _Exit(0);
    setsid();
    if (fork() > 0) _Exit(0);
    umask(0);
    if (closefrom(0))
      for (int i = 0; i < 256; ++i)  //
        close(i);
    npassert(0 == open(_PATH_DEVNULL, O_RDWR));
    npassert(1 == dup(0));
    npassert(2 == open("turfwar.log", O_CREAT | O_WRONLY | O_APPEND, 0644));
  }

  LOG("Generating Hilbert Curve...\n");
  for (int i = 0; i < YN * XN; ++i) {
    axdx_t h = unhilbert(XN, i);
    g_hilbert[i][0] = h.ax;
    g_hilbert[i][1] = h.dx;
  }

  // library init
  sqlite3_initialize();
  CheckDatabase();

  // fill token buckets
  g_tok.b = malloc(TB_BYTES);
  memset(g_tok.b, 127, TB_BYTES);

  // server lifecycle locks
  g_started = timespec_real();
  for (int i = 0; i < ARRAYLEN(g_shutdown); ++i) {
    g_shutdown[i] = nsync_note_new(0, nsync_time_no_deadline);
  }

  // load static assets into memory and pre-zip them
  g_asset.index = LoadAsset("index.html", "text/html; charset=utf-8", 900);
  g_asset.about = LoadAsset("about.html", "text/html; charset=utf-8", 900);
  g_asset.user = LoadAsset("user.html", "text/html; charset=utf-8", 900);
  g_asset.favicon = LoadAsset("favicon.ico", "image/vnd.microsoft.icon", 86400);

  // sandbox ourselves
  __pledge_mode = PLEDGE_PENALTY_RETURN_EPERM;
  CHECK_EQ(0, unveil("/opt/turfwar", "rwc"));
  CHECK_EQ(0, unveil(0, 0));
  if (!IsOpenbsd()) {
    // TODO(jart): why isn't pledge working on openbsd?
    CHECK_EQ(0, pledge("stdio flock rpath wpath cpath inet", 0));
  }

  // shutdown signals
  struct sigaction sa;
  sa.sa_flags = 0;
  sa.sa_handler = OnCtrlC;
  sigfillset(&sa.sa_mask);
  sigaction(SIGHUP, &sa, 0);
  sigaction(SIGINT, &sa, 0);
  sigaction(SIGTERM, &sa, 0);
  sa.sa_handler = IgnoreSignal;
  sigaction(SIGUSR1, &sa, 0);

  // make 9 helper threads
  g_ready = nsync_counter_new(10);
  pthread_t scorer, recenter, claimer, nower, replenisher, plotter;
  pthread_t scorer_hour, scorer_day, scorer_week, scorer_month;
  CHECK_EQ(0, pthread_create(&scorer, 0, ScoreWorker, 0));
  CHECK_EQ(0, pthread_create(&scorer_hour, 0, ScoreHourWorker, 0));
  CHECK_EQ(0, pthread_create(&scorer_day, 0, ScoreDayWorker, 0));
  CHECK_EQ(0, pthread_create(&scorer_week, 0, ScoreWeekWorker, 0));
  CHECK_EQ(0, pthread_create(&scorer_month, 0, ScoreMonthWorker, 0));
  CHECK_EQ(0, pthread_create(&replenisher, 0, ReplenishWorker, 0));
  CHECK_EQ(0, pthread_create(&recenter, 0, RecentWorker, 0));
  CHECK_EQ(0, pthread_create(&claimer, 0, ClaimWorker, 0));
  CHECK_EQ(0, pthread_create(&plotter, 0, PlotWorker, 0));
  CHECK_EQ(0, pthread_create(&nower, 0, NowWorker, 0));

  // wait for helper threads to warm up creating assets
  if (nsync_counter_add(g_ready, -1)) {  // #10
    nsync_counter_wait(g_ready, nsync_time_no_deadline);
  }

  // create one thread to listen
  CHECK_EQ(0, pthread_create(&g_listener, 0, ListenWorker, 0));

  // create lots of http workers to serve those assets
  LOG("Online\n");
  g_worker = xcalloc(g_workers, sizeof(*g_worker));
  for (intptr_t i = 0; i < g_workers; ++i) {
    CHECK_EQ(0, pthread_create(&g_worker[i].th, 0, HttpWorker, (void *)i));
  }

  // time to serve
  LOG("Ready\n");
  Supervisor(0);

  // cancel listen() so we stop accepting new clients
  LOG("Interrupting listen...\n");
  pthread_kill(g_listener, SIGUSR1);
  pthread_join(g_listener, 0);

  // cancel read() so that keepalive clients finish faster
  LOG("Interrupting workers...\n");
  for (int i = 0; i < g_workers; ++i) {
    pthread_kill(g_worker[i].th, SIGUSR1);
  }

  // wait for producers to finish
  LOG("Waiting for workers to finish...\n");
  for (int i = 0; i < g_workers; ++i) {
    CHECK_EQ(0, pthread_join(g_worker[i].th, 0));
  }
  LOG("Waiting for helpers to finish...\n");
  CHECK_EQ(0, pthread_join(nower, 0));
  CHECK_EQ(0, pthread_join(scorer, 0));
  CHECK_EQ(0, pthread_join(plotter, 0));
  CHECK_EQ(0, pthread_join(recenter, 0));
  CHECK_EQ(0, pthread_join(scorer_day, 0));
  CHECK_EQ(0, pthread_join(scorer_hour, 0));
  CHECK_EQ(0, pthread_join(scorer_week, 0));
  CHECK_EQ(0, pthread_join(scorer_month, 0));
  CHECK_EQ(0, pthread_join(replenisher, 0));

  // now that all workers have terminated, the claims queue must be
  // empty, therefore, it is now safe to send a cancellation to the
  // claims worker thread which waits forever for new claims.
  CHECK_EQ(0, g_claims.count);
  LOG("waiting for claims worker...\n");
  nsync_note_notify(g_shutdown[2]);
  CHECK_EQ(0, pthread_join(claimer, 0));

  // perform some sanity checks
  CHECK_EQ(g_claimsprocessed, g_claimsenqueued);

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
  for (int i = 0; i < ARRAYLEN(g_shutdown); ++i) {
    nsync_note_free(g_shutdown[i]);
  }
  nsync_counter_free(g_ready);
  free(g_worker);
  free(g_tok.b);

  LOG("Goodbye\n");
  // CheckForMemoryLeaks();
}
