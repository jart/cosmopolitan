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
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/pledge.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/sysinfo.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/ucontext.internal.h"
#include "libc/calls/ucontext.h"
#include "libc/cosmo.h"
#include "libc/cosmotime.h"
#include "libc/ctype.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/iscall.h"
#include "libc/intrin/kprintf.h"
#include "libc/limits.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/paths.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/sysconf.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/stdio/append.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/sysparam.h"
#include "libc/str/slice.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/rusage.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"
#include "libc/sysv/consts/tcp.h"
#include "libc/sysv/consts/timer.h"
#include "libc/thread/thread.h"
#include "libc/thread/thread2.h"
#include "libc/time.h"
#include "libc/x/x.h"
#include "libc/zip.h"
#include "net/http/escape.h"
#include "net/http/http.h"
#include "net/http/ip.h"
#include "net/http/tokenbucket.h"
#include "net/http/url.h"
#include "net/https/https.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/mbedtls/ctr_drbg.h"
#include "third_party/mbedtls/iana.h"
#include "third_party/mbedtls/net_sockets.h"
#include "third_party/mbedtls/pk.h"
#include "third_party/mbedtls/ssl.h"
#include "third_party/mbedtls/ssl_ticket.h"
#include "third_party/mbedtls/x509_crt.h"
#include "third_party/musl/passwd.h"
#include "third_party/sqlite3/sqlite3.h"
#include "third_party/zlib/zlib.h"

/**
 * @fileoverview production webserver for turfwar online game
 */

#define PORT              8080    // default server listening port
#define WORKERS           4500    // size of http client thread pool
#define SUPERVISE_MS      1000    // how often to stat() asset files
#define KEEPALIVE_MS      10000   // max time to keep idle conn open
#define MELTALIVE_MS      1000    // panic keepalive under heavy load
#define CHECKPOINT_MS     5000    // how often to checkpoint db
#define CLAIM_BATCH_MS    10      // how often to process claims
#define SCORE_H_UPDATE_MS 30000   // how often to regenerate /score/hour
#define SCORE_D_UPDATE_MS 300000  // how often to regenerate /score/day
#define SCORE_W_UPDATE_MS 300000  // how often to regenerate /score/week
#define SCORE_M_UPDATE_MS 600000  // how often to regenerate /score/month
#define SCORE_UPDATE_MS   600000  // how often to regenerate /score
#define CONCERN_LOAD      .90     // avoid keepalive, upon this connection load
#define PANIC_LOAD        .98     // meltdown if this percent of pool connected
#define PANIC_MSGS        10      // msgs per conn can't exceed it in meltdown
#define QUEUE_MAX         15000   // maximum pending claim items in queue
#define BATCH_MAX         3000    // max claims to insert per transaction
#define NICK_MAX          40      // max length of user nickname string
#define TB_INTERVAL       1000    // millis between token replenishes
#define MAX_MESSAGES      16      // max http messages per connection
#define TB_CIDR           27      // token bucket cidr specificity
#define SOCK_MAX          100     // max length of socket queue
#define MSG_BUF           512     // small response lookaside
#define DETECT_LEAKS      0       // malloc leak detector
#define CERT_PATH         "/etc/letsencrypt/live/ipv4.games"

#define BUF_SIZE 65536

#define TB_BYTES (1u << TB_CIDR)
#define TB_WORDS (TB_BYTES / 8)

#define GETOPTS "idvp:w:k:W:U:G:"
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
  "Server: Cosmopolitan Libc\r\n" \
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
      IncrementCounter(&g_memfails);        \
      goto OnError;                         \
    }                                       \
  } while (0)
#define CHECK_SYS(x)                        \
  do {                                      \
    if (!CheckSys(__FILE__, __LINE__, x)) { \
      IncrementCounter(&g_sysfails);        \
      goto OnError;                         \
    }                                       \
  } while (0)
#define CHECK_SQL(x)                        \
  do {                                      \
    int e = errno;                          \
    if (!CheckSql(__FILE__, __LINE__, x)) { \
      IncrementCounter(&g_dbfails);         \
      goto OnError;                         \
    }                                       \
    errno = e;                              \
  } while (0)
#define CHECK_DB(x)                            \
  do {                                         \
    int e = errno;                             \
    if (!CheckDb(__FILE__, __LINE__, x, db)) { \
      IncrementCounter(&g_dbfails);            \
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

static const char *const kAlpn[] = {
    "http/1.1",
    NULL,
};

struct Data {
  char *p;
  size_t n;
};

struct Asset {
  atomic_bool ready;
  int cash;
  char *path;
  pthread_rwlock_t lock;
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

struct CounterLong {
  alignas(64) atomic_long x;
};

struct Counter {
  struct CounterLong x[COSMO_SHARDS];
};

struct File {
  char *data;
  size_t size;
};

// cli flags
bool g_integrity;
bool g_daemonize;
int g_port = PORT;
int g_server;
bool g_online;
int g_crash_fd;
int g_workers = WORKERS;
int g_keepalive = KEEPALIVE_MS;
long g_checkpoint_count;
struct SortedInts g_whitelisted;
sig_atomic_t is_shutting_down;
const char *g_setuid;
const char *g_setgid;

// ssl
struct File rsacert;
struct File rsakey;
struct File ecdsacert;
struct File ecdsakey;

// threads
pthread_t scorer, claimer, replenisher;
pthread_t scorer_hour, scorer_day, scorer_week, scorer_month;
alignas(64) pthread_mutex_t report_lock = PTHREAD_MUTEX_INITIALIZER;
alignas(64) pthread_cond_t checkpoint_cond = PTHREAD_COND_INITIALIZER;
alignas(64) pthread_mutex_t checkpoint_lock = PTHREAD_MUTEX_INITIALIZER;

// lifecycle vars
struct timespec g_started;
struct Counter g_connections;
alignas(64) atomic_long g_worker_threads;

// whitebox metrics
atomic_long g_batches;
struct Counter g_evil;
struct Counter g_banned;
struct Counter g_accepts;
struct Counter g_dbfails;
struct Counter g_proxied;
struct Counter g_messages;
struct Counter g_memfails;
struct Counter g_sysfails;
struct Counter g_rejected;
struct Counter g_unproxied;
struct Counter g_notfounds;
struct Counter g_meltdowns;
struct Counter g_parsefails;
struct Counter g_iprequests;
struct Counter g_queuefulls;
struct Counter g_htmlclaims;
struct Counter g_ratelimits;
struct Counter g_emptyclaims;
struct Counter g_acceptfails;
struct Counter g_badversions;
struct Counter g_plainclaims;
struct Counter g_imageclaims;
struct Counter g_invalidnames;
struct Counter g_assetrequests;
struct Counter g_claimrequests;
struct Counter g_claimsenqueued;
struct Counter g_claimsprocessed;
struct Counter g_statuszrequests;
struct Counter g_checkpointfails;
atomic_long g_walpages;
atomic_long g_walprocessed;
atomic_long g_lastbatchsize;

struct Counter g_readeof;
struct Counter g_readfail;
alignas(64) atomic_long g_readfaileintr;
alignas(64) atomic_long g_readfailreset;
alignas(64) atomic_long g_readfailnetreset;
alignas(64) atomic_long g_readfailmisc;
alignas(64) atomic_long g_readfailtimeout;

struct Counter g_writeeof;
struct Counter g_writefail;
alignas(64) atomic_long g_writefaileintr;
alignas(64) atomic_long g_writefailreset;
alignas(64) atomic_long g_writefailnetreset;
alignas(64) atomic_long g_writefailpipe;
alignas(64) atomic_long g_writefailmisc;
alignas(64) atomic_long g_writefailtimeout;

struct Counter g_sslshake;
struct Counter g_sslshakefail;
alignas(64) atomic_long g_sslshakefaileof;
alignas(64) atomic_long g_sslshakefailreset;
alignas(64) atomic_long g_sslshakefailtimeout;
alignas(64) atomic_long g_sslshakefailnocipher;
alignas(64) atomic_long g_sslshakefailbadhello;
alignas(64) atomic_long g_sslshakefailcantcipher;
alignas(64) atomic_long g_sslshakefailnoversion;
alignas(64) atomic_long g_sslshakefailinvalidmac;
alignas(64) atomic_long g_sslshakefailnoclientcert;
alignas(64) atomic_long g_sslshakefailverifyfailed;
alignas(64) atomic_long g_sslshakefailfatalalert;
alignas(64) atomic_long g_sslshakefailunknowncert;
alignas(64) atomic_long g_sslshakefailfatalalert;
alignas(64) atomic_long g_sslshakefailunknownca;
alignas(64) atomic_long g_sslshakefailbadcert;
alignas(64) atomic_long g_sslshakefailmisc;

struct Counter g_sslreadeof;
struct Counter g_sslreadfail;
alignas(64) atomic_long g_sslreadfailclosenotify;
alignas(64) atomic_long g_sslreadfailreset;
alignas(64) atomic_long g_sslreadfailtimeout;
alignas(64) atomic_long g_sslreadfailwantread;
alignas(64) atomic_long g_sslreadfailfatalalert;
alignas(64) atomic_long g_sslreadfailinvalidrecord;
alignas(64) atomic_long g_sslreadfailinvalidmac;

struct Counter g_sslwritefail;
alignas(64) atomic_long g_sslwritefailreset;
alignas(64) atomic_long g_sslwritefailtimeout;

struct SortedInts {
  int n;
  int c;
  int *p;
};

union TokenBucket {
  atomic_schar *b;
  atomic_uint_fast64_t *w;
} g_tok;

// http worker objects
struct Worker {
  int id;
  int sock;
  pthread_t th;
  uint32_t addrsize;
  bool ssl_activated;
  bool ssl_established;
  bool got_first_recv;
  struct sockaddr_in addr;
  atomic_bool dead;
  atomic_int msgcount;
  atomic_int connected;
  struct timespec startread;
  char *msgbuf;
  char *inbuf;
  char *outbuf;
  char *preload;
  int preloadread;
  int preloadsize;
  struct HttpMessage *msg;
  struct Client *client;
  struct Certs sslcerts;
  mbedtls_ssl_context ssl;
  mbedtls_ssl_config sslconf;
  mbedtls_ctr_drbg_context sslrng;
} *g_worker;

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
} g_asset;

// queues /claim to ClaimWorker()
struct Claims {
  int pos;
  int count;
  pthread_mutex_t mu;
  pthread_cond_t non_full;
  pthread_cond_t non_empty;
  struct Claim {
    uint32_t ip;
    int64_t created;
    char name[NICK_MAX + 1];
  } data[QUEUE_MAX];
} g_claims;

void AddCounter(struct Counter *counter, long amt) {
  atomic_fetch_add_explicit(&counter->x[cosmo_shard()].x, amt,
                            memory_order_relaxed);
}

void IncrementCounter(struct Counter *counter) {
  AddCounter(counter, +1);
}

void DecrementCounter(struct Counter *counter) {
  AddCounter(counter, -1);
}

long GetCounter(struct Counter *counter) {
  long res = 0;
  for (int i = 0; i < COSMO_SHARDS; ++i)
    res += atomic_load_explicit(&counter->x[i].x, memory_order_relaxed);
  return res;
}

bool ContainsInt(const struct SortedInts *t, int k) {
  int l, m, r;
  l = 0;
  r = t->n - 1;
  while (l <= r) {
    m = (l & r) + ((l ^ r) >> 1);  // floor((a+b)/2)
    if (t->p[m] < k) {
      l = m + 1;
    } else if (t->p[m] > k) {
      r = m - 1;
    } else {
      return true;
    }
  }
  return false;
}

int LeftmostInt(const struct SortedInts *t, int k) {
  int l, m, r;
  l = 0;
  r = t->n;
  while (l < r) {
    m = (l & r) + ((l ^ r) >> 1);  // floor((a+b)/2)
    if (t->p[m] < k) {
      l = m + 1;
    } else {
      r = m;
    }
  }
  unassert(l == 0 || k >= t->p[l - 1]);
  unassert(l == t->n || k <= t->p[l]);
  return l;
}

bool InsertInt(struct SortedInts *t, int k, bool u) {
  int l;
  unassert(t->n >= 0);
  unassert(t->n <= t->c);
  if (t->n == t->c) {
    ++t->c;
    if (!IsModeDbg()) {
      t->c += t->c >> 1;
    }
    t->p = realloc(t->p, t->c * sizeof(*t->p));
  }
  l = LeftmostInt(t, k);
  if (l < t->n) {
    if (u && t->p[l] == k) {
      return false;
    }
    memmove(t->p + l + 1, t->p + l, (t->n - l) * sizeof(*t->p));
  }
  t->p[l] = k;
  t->n++;
  return true;
}

long GetTotalRam(void) {
  struct sysinfo si;
  si.totalram = 256 * 1024 * 1024;
  sysinfo(&si);
  return si.totalram;
}

// turns relative timeout into an absolute timeout
struct timespec WaitFor(int millis) {
  return timespec_add(timespec_real(), timespec_frommillis(millis));
}

// helper functions for check macro implementation
bool CheckMem(const char *file, int line, void *ptr) {
  if (ptr)
    return true;
  kprintf("%s:%d: %H: out of memory: %s\n", file, line, strerror(errno));
  return false;
}
bool CheckSys(const char *file, int line, long rc) {
  if (rc != -1)
    return true;
  kprintf("%s:%d: %H: %s\n", file, line, strerror(errno));
  return false;
}
bool CheckSql(const char *file, int line, int rc) {
  if (rc == SQLITE_OK)
    return true;
  kprintf("%s:%d: %H: %s\n", file, line, sqlite3_errstr(rc));
  return false;
}
bool CheckDb(const char *file, int line, int rc, sqlite3 *db) {
  if (rc == SQLITE_OK)
    return true;
  kprintf("%s:%d: %H: %s: %s\n", file, line, sqlite3_errstr(rc),
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
  errno = 0;
  rc = sqlite3_open(path, db);
  if (rc != SQLITE_OK)
    return rc;
  if (!IsWindows() && !IsOpenbsd()) {
    errno = 0;
    ksnprintf(sql, sizeof(sql), "PRAGMA mmap_size=%ld", GetTotalRam());
    rc = sqlite3_exec(*db, sql, 0, 0, 0);
    if (rc != SQLITE_OK)
      return rc;
  }
  for (i = 0; i < 7; ++i) {
    errno = 0;
    rc = sqlite3_exec(*db, "PRAGMA journal_mode=WAL", 0, 0, 0);
    if (rc == SQLITE_OK)
      break;
    if (rc != SQLITE_BUSY &&  //
        rc != SQLITE_LOCKED)
      return rc;
    usleep(1000L << i);
  }
  return sqlite3_exec(*db, "PRAGMA synchronous=NORMAL", 0, 0, 0);
}

int DbStep(sqlite3_stmt *stmt) {
  int i, rc;
  for (i = 0; i < 12; ++i) {
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
      break;
    if (rc == SQLITE_DONE)
      break;
    if (rc != SQLITE_BUSY &&  //
        rc != SQLITE_LOCKED)
      return rc;
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
  if (n == -1)
    n = strlen(s);
  if (!n)
    return false;
  if (n > NICK_MAX)
    return false;
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

struct Clock {
  atomic_uint roll;
  atomic_ulong time;
  atomic_ulong date;
};

static struct Clock g_clck[2];
static pthread_t g_time_thread;

static void set_clck(struct Clock *clck, long time, long date) {
  unsigned long roll;
  roll = atomic_fetch_add_explicit(&clck->roll, 1, memory_order_relaxed);
  time &= 0xffffffffffff;
  date &= 0xffffffffffff;
  time |= roll << 48;
  date |= roll << 48;
  atomic_store_explicit(&clck->time, time, memory_order_relaxed);
  atomic_store_explicit(&clck->date, date, memory_order_relaxed);
}

static void get_clck(struct Clock *clck, long *out_time, long *out_date) {
  long time, date;
  do {
    time = atomic_load_explicit(&clck->time, memory_order_relaxed);
    date = atomic_load_explicit(&clck->date, memory_order_relaxed);
  } while ((time >> 48) != (date >> 48));
  *out_date = date & 0xffffffffffff;
  *out_time = time & 0xffffffffffff;
}

static long encode_date(const struct tm *tm) {
  long date;
  date = tm->tm_year;
  date <<= 4;
  date |= tm->tm_isdst == 1;
  date <<= 1;
  date |= tm->tm_mon;
  date <<= 5;
  date |= tm->tm_mday;
  date <<= 3;
  date |= tm->tm_wday;
  date <<= 5;
  date |= tm->tm_hour;
  date <<= 6;
  date |= tm->tm_min;
  date <<= 6;
  date |= tm->tm_sec;
  return date;
}

static void decode_date(long date, struct tm *tm) {
  tm->tm_sec = date & 63;
  date >>= 6;
  tm->tm_min = date & 63;
  date >>= 6;
  tm->tm_hour = date & 31;
  date >>= 5;
  tm->tm_wday = date & 7;
  date >>= 3;
  tm->tm_mday = date & 31;
  date >>= 5;
  tm->tm_mon = date & 15;
  date >>= 4;
  tm->tm_isdst = date & 1;
  date >>= 1;
  tm->tm_year = date;
  tm->tm_gmtoff = 0;  // unsupported
  tm->tm_zone = 0;    // unsupported
  tm->tm_yday = 0;    // unsupported
}

static void update_time() {
  struct tm tm;
  struct timespec ts;
  clock_gettime(0, &ts);
  gmtime_r(&ts.tv_sec, &tm);
  set_clck(&g_clck[0], ts.tv_sec, encode_date(&tm));
  localtime_r(&ts.tv_sec, &tm);
  set_clck(&g_clck[1], ts.tv_sec, encode_date(&tm));
}

static void *time_worker(void *arg) {
  sigset_t ss;
  sigemptyset(&ss);
  sigaddset(&ss, SIGHUP);
  sigaddset(&ss, SIGINT);
  sigaddset(&ss, SIGQUIT);
  sigaddset(&ss, SIGTERM);
  sigaddset(&ss, SIGUSR1);
  sigaddset(&ss, SIGALRM);
  pthread_sigmask(SIG_SETMASK, &ss, 0);
  pthread_setname_np(pthread_self(), "localtime");
  for (;;) {
    sleep(10);
    update_time();
  }
  return nullptr;
}

void time_init() {
  update_time();
  if (pthread_create(&g_time_thread, 0, time_worker, 0))
    __builtin_trap();
}

void time_destroy() {
  pthread_cancel(g_time_thread);
  if (pthread_join(g_time_thread, 0))
    __builtin_trap();
}

static const char kMonDays[2][12] = {
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
};

static void time_lockless(struct Clock *clck, long now, struct tm *tm) {
  long time, date, since;
  get_clck(clck, &time, &date);
  decode_date(date, tm);
  since = now - time;
  since = since < 60 ? since : 60;
  for (; since > 0; --since) {
    if (++tm->tm_sec >= 60) {
      tm->tm_sec = 0;
      if (++tm->tm_min >= 60) {
        tm->tm_min = 0;
        if (++tm->tm_hour >= 24) {
          tm->tm_hour = 0;
          if (++tm->tm_mday >= 7)
            tm->tm_mday = 0;
          if (++tm->tm_mday > kMonDays[!!tm->tm_isdst][tm->tm_mon]) {
            tm->tm_mday = 1;
            if (++tm->tm_mon >= 12) {
              tm->tm_mon = 0;
              ++tm->tm_year;
            }
          }
        }
      }
    }
  }
}

void gmtime_lockless(long now, struct tm *tm) {
  time_lockless(&g_clck[0], now, tm);
}

void localtime_lockless(long now, struct tm *tm) {
  time_lockless(&g_clck[1], now, tm);
}

// turn unix timestamp into string the easy way
char *FormatUnixHttpDateTime(char *s, int64_t t) {
  struct tm tm;
  gmtime_lockless(t, &tm);
  return FormatHttpDateTime(s, &tm);
}

// the standard strftime() function is dismally slow
// this function is non-generalized for just http so
// it needs 25 cycles rather than 709 cycles so cool
char *FormatDate(char *p) {
  return FormatUnixHttpDateTime(p, timespec_real().tv_sec);
}

void unlock_mutex(void *arg) {
  pthread_mutex_t *lock = arg;
  if (lock)
    pthread_mutex_unlock(lock);
}

// inserts ip:name claim into blocking message queue
// may be interrupted by absolute deadline
// may be cancelled by server shutdown
bool AddClaim(struct Claims *q, const struct Claim *v) {
  bool wake = false;
  bool added = false;
  pthread_mutex_lock(&q->mu);
  pthread_cleanup_push(unlock_mutex, &q->mu);
  if (q->count != ARRAYLEN(q->data)) {
    int i = q->pos + q->count;
    if (ARRAYLEN(q->data) <= i)
      i -= ARRAYLEN(q->data);
    memcpy(q->data + i, v, sizeof(*v));
    if (!q->count)
      wake = true;
    q->count++;
    added = true;
  }
  pthread_cleanup_pop(true);
  if (wake)
    pthread_cond_broadcast(&q->non_empty);
  return added;
}

// removes batch of ip:name claims from blocking message queue
// has no deadline or cancellation; enqueued must be processed
int GetClaims(struct Claims *q, struct Claim *out, int len) {
  int got = 0;
  pthread_mutex_lock(&q->mu);
  pthread_cleanup_push(unlock_mutex, &q->mu);
  pthread_setcancelstate(PTHREAD_CANCEL_MASKED, 0);
  while (!q->count && !is_shutting_down)
    if (pthread_cond_timedwait(&q->non_empty, &q->mu, 0))
      break;  // must be ECANCELED
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0);
  while (got < len && q->count) {
    memcpy(out + got, q->data + q->pos, sizeof(*out));
    if (q->count == ARRAYLEN(q->data))
      pthread_cond_broadcast(&q->non_full);
    ++got;
    q->pos++;
    q->count--;
    if (q->pos == ARRAYLEN(q->data))
      q->pos = 0;
  }
  pthread_cleanup_pop(true);
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
  if (!found &&                          //
      url.path.p &&                      //
      url.path.n > strlen("/claim/") &&  //
      IsValidNick(url.path.p + strlen("/claim/"),
                  url.path.n - strlen("/claim/"))) {
    memcpy(v->name, url.path.p + strlen("/claim/"),
           url.path.n - strlen("/claim/"));
    found = true;
  }
  free(f[1]);
  free(f[0]);
  return found;
}

// allocates memory with hardware-accelerated buffer overflow detection
// so if it gets hacked it'll at least crash instead of get compromised
void *NewSafeBuffer(void) {
  char *p;
  long pagesize = sysconf(_SC_PAGESIZE);
  npassert((p = _mapanon(BUF_SIZE + pagesize)));
  if (mprotect(p + BUF_SIZE, pagesize, PROT_NONE)) {
    kprintf("NewSafeBuffer mprotect() failed %m\n");
    _Exit(1);
  }
  return p;
}

// frees memory with hardware-accelerated buffer overflow detection
void FreeSafeBuffer(void *p) {
  npassert(!munmap(p, BUF_SIZE + sysconf(_SC_PAGESIZE)));
}

// signals by default get delivered to any random thread
// solution is to block every signal possible in threads
void BlockSignals(void) {
  sigset_t mask;
  sigfillset(&mask);
  sigdelset(&mask, SIGABRT);
  sigdelset(&mask, SIGTRAP);
  sigdelset(&mask, SIGFPE);
  sigdelset(&mask, SIGBUS);
  sigdelset(&mask, SIGSEGV);
  sigdelset(&mask, SIGILL);
  sigdelset(&mask, SIGXCPU);
  sigdelset(&mask, SIGXFSZ);
  sigprocmask(SIG_SETMASK, &mask, 0);
}

void UnblockSignals(void) {
  sigset_t mask;
  sigemptyset(&mask);
  sigprocmask(SIG_SETMASK, &mask, 0);
}

char *Statusz(char *p, const char *s, long x) {
  p = stpcpy(p, s);
  p = stpcpy(p, ": ");
  p = FormatInt64(p, x);
  p = stpcpy(p, "\n");
  return p;
}

void Listen(void) {
  int yes = 1;
  struct timeval timeo = {g_keepalive / 1000, g_keepalive % 1000};
  struct sockaddr_in addr = {.sin_family = AF_INET, .sin_port = htons(g_port)};
  npassert((g_server = socket(AF_INET, SOCK_STREAM, 0)) != -1);
  setsockopt(g_server, SOL_SOCKET, SO_RCVTIMEO, &timeo, sizeof(timeo));
  setsockopt(g_server, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo));
  setsockopt(g_server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
  setsockopt(g_server, SOL_TCP, TCP_NODELAY, &yes, sizeof(yes));
  if (bind(g_server, (struct sockaddr *)&addr, sizeof(addr))) {
    kprintf("error: failed to bind to port %d\n", g_port);
    exit(1);
  }
  if (listen(g_server, 1)) {
    kprintf("error: failed to listen on port %d\n", g_port);
    exit(1);
  }
}

void OnHttpWorkerCancel(void *arg) {
  struct Worker *w = arg;
  DEBUG("OnHttpWorkerCancel\n");
  if (w->sock != -1)
    close(w->sock);
  FreeSafeBuffer(w->outbuf);
  FreeSafeBuffer(w->inbuf);
  DestroyHttpMessage(w->msg);
  mbedtls_ssl_free(&w->ssl);
  mbedtls_ctr_drbg_free(&w->sslrng);
  mbedtls_ssl_config_free(&w->sslconf);
  CertsDestroy(&w->sslcerts);
  free(w->preload);
  free(w->msgbuf);
  --g_worker_threads;
  w->dead = true;
}

ssize_t OnRead(ssize_t rc) {
  DEBUG("read %zd bytes\n", rc);
  switch (rc) {
    default:
      break;
    case 0:
      IncrementCounter(&g_readeof);
      break;
    case -1:
      IncrementCounter(&g_readfail);
      switch (errno) {
        case EINTR:
          ++g_readfaileintr;
          break;
        case ECONNRESET:
          ++g_readfailreset;
          break;
        case ENETRESET:
          ++g_readfailnetreset;
          break;
        case EAGAIN:
          ++g_readfailtimeout;
          break;
        default:
          ++g_readfailmisc;
          break;
      }
      break;
  }
  return rc;
}

ssize_t OnWrite(ssize_t rc) {
  DEBUG("wrote %zd bytes\n", rc);
  switch (rc) {
    default:
      break;
    case -1:
      IncrementCounter(&g_writefail);
      switch (errno) {
        case EINTR:
          ++g_writefaileintr;
          break;
        case ECONNRESET:
          ++g_writefailreset;
          break;
        case ENETRESET:
          ++g_writefailnetreset;
          break;
        case EPIPE:
          ++g_writefailpipe;
          break;
        case EAGAIN:
          ++g_writefailtimeout;
          break;
        default:
          ++g_writefailmisc;
          break;
      }
      break;
  }
  return rc;
}

int TlsSend(void *c, const unsigned char *p, size_t n) {
  ssize_t rc;
  struct Worker *w = c;
  if ((rc = OnWrite(write(w->sock, p, n))) == -1) {
    switch (errno) {
      case EINTR:
        return MBEDTLS_ERR_SSL_WANT_WRITE;
      case EAGAIN:
        return MBEDTLS_ERR_SSL_TIMEOUT;
      case EPIPE:
      case ECONNRESET:
      case ENETRESET:
        return MBEDTLS_ERR_NET_CONN_RESET;
      default:
        return MBEDTLS_ERR_NET_SEND_FAILED;
    }
  }
  return rc;
}

int TlsRecv(void *c, unsigned char *p, size_t n, uint32_t waitms) {
  ssize_t rc;
  struct Worker *w = c;
  if (w->preloadread < w->preloadsize) {
    rc = MIN(w->preloadsize - w->preloadread, n);
    memcpy(p, w->preload + w->preloadread, rc);
    w->preloadread += rc;
    return rc;
  }
  if ((rc = OnRead(read(w->sock, p, n))) == -1) {
    switch (errno) {
      case EINTR:
        return MBEDTLS_ERR_SSL_WANT_READ;
      case EAGAIN:
        return MBEDTLS_ERR_SSL_TIMEOUT;
      case EPIPE:
      case ECONNRESET:
      case ENETRESET:
        return MBEDTLS_ERR_NET_CONN_RESET;
      default:
        return MBEDTLS_ERR_NET_RECV_FAILED;
    }
  }
  return rc;
}

ssize_t SslRead(mbedtls_ssl_context *ssl, void *buf, size_t size) {
  int rc;
  if (!(rc = mbedtls_ssl_read(ssl, buf, size))) {
    IncrementCounter(&g_sslreadeof);
    errno = ECONNRESET;
    rc = -1;
  } else if (rc < 0) {
    IncrementCounter(&g_sslreadfail);
    if (rc == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
      ++g_sslreadfailclosenotify;
      rc = 0;
    } else if (rc == MBEDTLS_ERR_NET_CONN_RESET) {
      ++g_sslreadfailreset;
      errno = ECONNRESET;
      rc = -1;
    } else if (rc == MBEDTLS_ERR_SSL_TIMEOUT) {
      ++g_sslreadfailtimeout;
      errno = ECONNRESET;
      rc = -1;
    } else if (rc == MBEDTLS_ERR_SSL_WANT_READ) {
      ++g_sslreadfailwantread;
      errno = EAGAIN;
      rc = -1;
    } else if (rc == MBEDTLS_ERR_SSL_FATAL_ALERT_MESSAGE) {
      ++g_sslreadfailfatalalert;
      errno = EIO;
      rc = -1;
    } else if (rc == MBEDTLS_ERR_SSL_INVALID_RECORD) {
      ++g_sslreadfailinvalidrecord;
      errno = EIO;
      rc = -1;
    } else if (rc == MBEDTLS_ERR_SSL_INVALID_MAC) {
      ++g_sslreadfailinvalidmac;
      errno = EIO;
      rc = -1;
    } else {
      errno = EIO;
      rc = -1;
    }
  }
  return rc;
}

ssize_t SslWrite(mbedtls_ssl_context *ssl, struct iovec *iov, int iovlen) {
  int i;
  size_t n;
  ssize_t rc;
  const unsigned char *p;
  for (i = 0; i < iovlen; ++i) {
    p = iov[i].iov_base;
    n = iov[i].iov_len;
    while (n) {
      if ((rc = mbedtls_ssl_write(ssl, p, n)) > 0) {
        p += rc;
        n -= rc;
      } else {
        IncrementCounter(&g_sslwritefail);
        if (rc == MBEDTLS_ERR_NET_CONN_RESET) {
          ++g_sslwritefailreset;
          errno = ECONNRESET;
          return -1;
        } else if (rc == MBEDTLS_ERR_SSL_TIMEOUT) {
          ++g_sslwritefailtimeout;
          errno = ETIMEDOUT;
          return -1;
        } else {
          errno = EIO;
          return -1;
        }
      }
    }
  }
  return 0;
}

ssize_t Read(struct Worker *w, char *buf, size_t size, uint32_t ip) {
  int err;
  ssize_t rc;
  if (w->ssl_activated)
    return SslRead(&w->ssl, buf, size);
  if ((rc = OnRead(read(w->sock, buf, size))) <= 0) {
    if (!w->got_first_recv &&             //
        (rc == -1 && errno == EAGAIN) &&  //
        !IsLoopbackIp(ip) &&              //
        !ContainsInt(&g_whitelisted, ip)) {
      Blackhole(ip);
      IncrementCounter(&g_banned);
      IncrementCounter(&g_evil);
      return -1;
    }
    return rc;
  }
  if (w->got_first_recv)
    return rc;
  w->got_first_recv = true;
  if (buf[0] != 22)
    return rc;
  w->ssl_activated = true;
  w->preloadread = 0;
  w->preloadsize = rc;
  w->preload = realloc(w->preload, w->preloadsize);
  memcpy(w->preload, buf, w->preloadsize);
  if ((err = mbedtls_ssl_handshake(&w->ssl))) {
    IncrementCounter(&g_sslshakefail);
    switch (err) {
      case MBEDTLS_ERR_SSL_CONN_EOF:
        ++g_sslshakefaileof;
        break;
      case MBEDTLS_ERR_NET_CONN_RESET:
        ++g_sslshakefailreset;
        break;
      case MBEDTLS_ERR_SSL_TIMEOUT:
        ++g_sslshakefailtimeout;
        break;
      case MBEDTLS_ERR_SSL_NO_CIPHER_CHOSEN:
        ++g_sslshakefailnocipher;
        break;
      case MBEDTLS_ERR_SSL_NO_USABLE_CIPHERSUITE:
        ++g_sslshakefailcantcipher;
        break;
      case MBEDTLS_ERR_SSL_BAD_HS_PROTOCOL_VERSION:
        ++g_sslshakefailnoversion;
        break;
      case MBEDTLS_ERR_SSL_INVALID_MAC:
        ++g_sslshakefailinvalidmac;
        break;
      case MBEDTLS_ERR_SSL_NO_CLIENT_CERTIFICATE:
        ++g_sslshakefailnoclientcert;
        break;
      case MBEDTLS_ERR_X509_CERT_VERIFY_FAILED:
        ++g_sslshakefailverifyfailed;
        break;
      case MBEDTLS_ERR_SSL_BAD_HS_CLIENT_HELLO:
        ++g_sslshakefailbadhello;
        break;
      case MBEDTLS_ERR_SSL_FATAL_ALERT_MESSAGE:
        switch (w->ssl.fatal_alert) {
          case MBEDTLS_SSL_ALERT_MSG_CERT_UNKNOWN:
            ++g_sslshakefailunknowncert;
            break;
          case MBEDTLS_SSL_ALERT_MSG_UNKNOWN_CA:
            ++g_sslshakefailunknownca;
            break;
          case MBEDTLS_SSL_ALERT_MSG_BAD_CERT:
            ++g_sslshakefailbadcert;
            break;
          default:
            dprintf(2, "unknown ssl handshake fatal alert %d (%s)\n",
                    w->ssl.fatal_alert,
                    GetAlertDescription(w->ssl.fatal_alert));
            dprintf(g_crash_fd, "unknown ssl handshake fatal alert %d (%s)\n",
                    w->ssl.fatal_alert,
                    GetAlertDescription(w->ssl.fatal_alert));
            ++g_sslshakefailfatalalert;
            break;
        }
        break;
      default:
        ++g_sslshakefailmisc;
        dprintf(2, "unknown ssl handshake failure -0x%04x\n", -err);
        dprintf(g_crash_fd, "unknown ssl handshake failure -0x%04x\n", -err);
        break;
    }
    errno = EIO;
    return -1;
  }
  w->ssl_established = true;
  IncrementCounter(&g_sslshake);
  return SslRead(&w->ssl, buf, size);
}

ssize_t Writev(struct Worker *w, struct iovec *iov, int iovlen) {
  if (w->ssl_activated)
    return SslWrite(&w->ssl, iov, iovlen);
  return OnWrite(writev(w->sock, iov, iovlen));
}

ssize_t Write(struct Worker *w, const void *buf, size_t size) {
  if (w->ssl_activated)
    return SslWrite(&w->ssl, &(struct iovec){(void *)buf, size}, 1);
  return OnWrite(write(w->sock, buf, size));
}

// easy string sender
ssize_t WriteStr(struct Worker *w, const char *s) {
  return Write(w, s, strlen(s));
}

// public /statusz endpoint for monitoring server internals
void ServeStatusz(struct Worker *w, char *outbuf, uint32_t ip) {
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
  p = Statusz(
      p, "qps",
      GetCounter(&g_messages) / MAX(1, timespec_sub(now, g_started).tv_sec));
  p = Statusz(p, "started", g_started.tv_sec);
  p = Statusz(p, "now", now.tv_sec);
  p = Statusz(p, "messages", GetCounter(&g_messages));
  p = Statusz(p, "connections", GetCounter(&g_connections));
  p = Statusz(p, "worker_threads", g_worker_threads);
  p = Statusz(p, "yourtokens", CountTokens(g_tok.b, ip, TB_CIDR));
  p = Statusz(p, "banned", GetCounter(&g_banned));
  p = Statusz(p, "evil", GetCounter(&g_evil));
  p = Statusz(p, "workers", g_workers);
  p = Statusz(p, "batches", g_batches);
  p = Statusz(p, "checkpoints", g_checkpoint_count);
  p = Statusz(p, "lastbatchsize", g_lastbatchsize);
  p = Statusz(p, "walpages", g_walpages);
  p = Statusz(p, "walprocessed", g_walprocessed);
  p = Statusz(p, "accepts", GetCounter(&g_accepts));
  p = Statusz(p, "dbfails", GetCounter(&g_dbfails));
  p = Statusz(p, "proxied", GetCounter(&g_proxied));
  p = Statusz(p, "memfails", GetCounter(&g_memfails));
  p = Statusz(p, "sysfails", GetCounter(&g_sysfails));
  p = Statusz(p, "rejected", GetCounter(&g_rejected));
  p = Statusz(p, "readeof", GetCounter(&g_readeof));
  p = Statusz(p, "readfail", GetCounter(&g_readfail));
  p = Statusz(p, "readfaileintr", g_readfaileintr);
  p = Statusz(p, "readfailreset", g_readfailreset);
  p = Statusz(p, "readfailnetreset", g_readfailnetreset);
  p = Statusz(p, "readfailtimeout", g_readfailtimeout);
  p = Statusz(p, "readfailmisc", g_readfailmisc);
  p = Statusz(p, "writeeof", GetCounter(&g_writeeof));
  p = Statusz(p, "writefail", GetCounter(&g_writefail));
  p = Statusz(p, "writefaileintr", g_writefaileintr);
  p = Statusz(p, "writefailreset", g_writefailreset);
  p = Statusz(p, "writefailnetreset", g_writefailnetreset);
  p = Statusz(p, "writefailpipe", g_writefailpipe);
  p = Statusz(p, "writefailtimeout", g_writefailtimeout);
  p = Statusz(p, "writefailmisc", g_writefailmisc);
  p = Statusz(p, "unproxied", GetCounter(&g_unproxied));
  p = Statusz(p, "notfounds", GetCounter(&g_notfounds));
  p = Statusz(p, "meltdowns", GetCounter(&g_meltdowns));
  p = Statusz(p, "parsefails", GetCounter(&g_parsefails));
  p = Statusz(p, "iprequests", GetCounter(&g_iprequests));
  p = Statusz(p, "queuefulls", GetCounter(&g_queuefulls));
  p = Statusz(p, "htmlclaims", GetCounter(&g_htmlclaims));
  p = Statusz(p, "ratelimits", GetCounter(&g_ratelimits));
  p = Statusz(p, "emptyclaims", GetCounter(&g_emptyclaims));
  p = Statusz(p, "acceptfails", GetCounter(&g_acceptfails));
  p = Statusz(p, "badversions", GetCounter(&g_badversions));
  p = Statusz(p, "plainclaims", GetCounter(&g_plainclaims));
  p = Statusz(p, "imageclaims", GetCounter(&g_imageclaims));
  p = Statusz(p, "invalidnames", GetCounter(&g_invalidnames));
  p = Statusz(p, "assetrequests", GetCounter(&g_assetrequests));
  p = Statusz(p, "claimrequests", GetCounter(&g_claimrequests));
  p = Statusz(p, "claimsenqueued", GetCounter(&g_claimsenqueued));
  p = Statusz(p, "claimsprocessed", GetCounter(&g_claimsprocessed));
  p = Statusz(p, "statuszrequests", GetCounter(&g_statuszrequests));
  p = Statusz(p, "checkpointfails", GetCounter(&g_checkpointfails));
  p = Statusz(p, "sslshake", GetCounter(&g_sslshake));
  p = Statusz(p, "sslshakefail", GetCounter(&g_sslshakefail));
  p = Statusz(p, "sslshakefaileof", g_sslshakefaileof);
  p = Statusz(p, "sslshakefailreset", g_sslshakefailreset);
  p = Statusz(p, "sslshakefailtimeout", g_sslshakefailtimeout);
  p = Statusz(p, "sslshakefailnocipher", g_sslshakefailnocipher);
  p = Statusz(p, "sslshakefailbadhello", g_sslshakefailbadhello);
  p = Statusz(p, "sslshakefailcantcipher", g_sslshakefailcantcipher);
  p = Statusz(p, "sslshakefailnoversion", g_sslshakefailnoversion);
  p = Statusz(p, "sslshakefailinvalidmac", g_sslshakefailinvalidmac);
  p = Statusz(p, "sslshakefailnoclientcert", g_sslshakefailnoclientcert);
  p = Statusz(p, "sslshakefailverifyfailed", g_sslshakefailverifyfailed);
  p = Statusz(p, "sslshakefailunknowncert", g_sslshakefailunknowncert);
  p = Statusz(p, "sslshakefailfatalalert", g_sslshakefailfatalalert);
  p = Statusz(p, "sslshakefailunknownca", g_sslshakefailunknownca);
  p = Statusz(p, "sslshakefailbadcert", g_sslshakefailbadcert);
  p = Statusz(p, "sslshakefailmisc", g_sslshakefailmisc);
  p = Statusz(p, "sslreadeof", GetCounter(&g_sslreadeof));
  p = Statusz(p, "sslreadfail", GetCounter(&g_sslreadfail));
  p = Statusz(p, "sslreadfailreset", g_sslreadfailreset);
  p = Statusz(p, "sslreadfailtimeout", g_sslreadfailtimeout);
  p = Statusz(p, "sslreadfailwantread", g_sslreadfailwantread);
  p = Statusz(p, "sslreadfailinvalidmac", g_sslreadfailinvalidmac);
  p = Statusz(p, "sslreadfailfatalalert", g_sslreadfailfatalalert);
  p = Statusz(p, "sslreadfailclosenotify", g_sslreadfailclosenotify);
  p = Statusz(p, "sslreadfailinvalidrecord", g_sslreadfailinvalidrecord);
  p = Statusz(p, "sslwritefail", GetCounter(&g_sslwritefail));
  p = Statusz(p, "sslwritefailreset", g_sslwritefailreset);
  p = Statusz(p, "sslwritefailtimeout", g_sslwritefailtimeout);
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
  Write(w, outbuf, p - outbuf);
}

// make thousands of http client handler threads
// load balance incoming connections for port 8080 across all threads
// hangup on any browser clients that lag for more than a few seconds
void *HttpWorker(void *arg) {
  struct Worker *w = arg;
  char *msgbuf = malloc(MSG_BUF);
  char *inbuf = NewSafeBuffer();
  char *outbuf = NewSafeBuffer();
  struct HttpMessage msg[1];

  BlockSignals();
  InitHttpMessage(msg, kHttpRequest);
  InitializeRng(&w->sslrng);
  ProgramCertificate(&w->sslcerts, ecdsacert.data, ecdsacert.size);
  ProgramPrivateKey(&w->sslcerts, ecdsakey.data, ecdsakey.size);
  /* ProgramCertificate(&w->sslcerts, rsacert.data, rsacert.size); */
  /* ProgramPrivateKey(&w->sslcerts, rsakey.data, rsakey.size); */
  mbedtls_ssl_config_defaults(&w->sslconf, MBEDTLS_SSL_IS_SERVER,
                              MBEDTLS_SSL_TRANSPORT_STREAM,
                              MBEDTLS_SSL_PRESET_DEFAULT);
  mbedtls_ssl_conf_sni(&w->sslconf, TlsRoute, &w->sslcerts);
  mbedtls_ssl_conf_rng(&w->sslconf, mbedtls_ctr_drbg_random, &w->sslrng);
  npassert(!mbedtls_ssl_conf_alpn_protocols(&w->sslconf, (void *)kAlpn));
  w->sslconf.disable_compression = true;
  mbedtls_ssl_init(&w->ssl);
  mbedtls_ssl_set_bio(&w->ssl, w, TlsSend, 0, TlsRecv);
  npassert(!mbedtls_ssl_setup(&w->ssl, &w->sslconf));

  w->msgbuf = msgbuf;
  w->inbuf = inbuf;
  w->outbuf = outbuf;
  w->msg = msg;
  pthread_cleanup_push(OnHttpWorkerCancel, w);

  char name[32];
  sprintf(name, "HTTP%d", w->id);
  pthread_setname_np(pthread_self(), name);
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
  ++g_worker_threads;

  // connection loop
  for (;;) {
    uint32_t ip;
    struct Data d;
    ssize_t got, sent;
    int tok, inmsglen, outmsglen;
    char ipbuf[32], *p, *q, cashbuf[64];

    w->addrsize = sizeof(w->addr);
    w->sock = accept(g_server, (struct sockaddr *)&w->addr, &w->addrsize);
    if (w->sock == -1) {
      if (errno != EAGAIN) {  // spinning on SO_RCVTIMEO
        IncrementCounter(&g_acceptfails);
        usleep(20000);
      }
      continue;
    }
    IncrementCounter(&g_accepts);
    w->connected = true;

    // acquire token for connection. we need this early rate limiter so
    // someone can't flood the server with bad ssl handshakes.
    ip = ntohl(w->addr.sin_addr.s_addr);
    if (!IsLoopbackIp(ip) &&  //
        !ContainsInt(&g_whitelisted, ip) &&
        (tok = AcquireToken(g_tok.b, ip, TB_CIDR)) < 4) {
      Blackhole(ip);
      IncrementCounter(&g_banned);
      IncrementCounter(&g_ratelimits);
      close(w->sock);
      w->connected = false;
      w->sock = -1;
      continue;
    }

    w->msgcount = 0;
    IncrementCounter(&g_connections);

    // simple http/1.1 message loop
    // let's assume we're behind a well-behaved frontend
    // each read() should give us just *one* HTTP message
    // if we get less than one message, we drop connection
    // if we get more than one message, we Connection: close
    // let's not bother with cray proto stuff like 100-expect
    bool must_close = false;
    do {
      bool comp;
      struct Asset *a;

      // wait for http message
      ResetHttpMessage(msg, kHttpRequest);
      w->startread = timespec_real();
      got = Read(w, inbuf, BUF_SIZE - 1, ip);
      if (got <= 0)
        break;

      // parse http message
      // we're only doing one-shot parsing right now
      if ((inmsglen = ParseHttpMessage(msg, inbuf, got, BUF_SIZE)) <= 0) {
        IncrementCounter(&g_parsefails);
        break;
      }
      IncrementCounter(&g_messages);
      ++w->msgcount;

      ksnprintf(ipbuf, sizeof(ipbuf), "%hhu.%hhu.%hhu.%hhu", ip >> 24, ip >> 16,
                ip >> 8, ip);

      // acquire token for each additional message
      if (w->msgcount > 1 &&    //
          !IsLoopbackIp(ip) &&  //
          !ContainsInt(&g_whitelisted, ip) &&
          (tok = AcquireToken(g_tok.b, ip, TB_CIDR)) < 32) {
        if (tok > 4) {
          LOG("%s rate limiting client\n", ipbuf, msg->version);
          WriteStr(w, "HTTP/1.1 429 Too Many Requests\r\n"
                      "Content-Type: text/plain\r\n"
                      "Connection: close\r\n"
                      "\r\n"
                      "429 Too Many Requests\n"
                      "Slow down or your IP will be blocked\n");
        } else {
          Blackhole(ip);
          IncrementCounter(&g_banned);
        }
        IncrementCounter(&g_ratelimits);
        break;
      }

      // we don't support http/1.0 and http/0.9 right now
      if (msg->version != 11) {
        LOG("%s used unsupported http/%d version\n", ipbuf, msg->version);
        WriteStr(w, "HTTP/1.1 505 HTTP Version Not Supported\r\n"
                    "Content-Type: text/plain\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "HTTP Version Not Supported\n");
        IncrementCounter(&g_badversions);
        break;
      }

      if (UrlStartsWith("http://ipv4.games/")) {
        msg->uri.a += strlen("http://ipv4.games");
      } else if (UrlStartsWith("https://ipv4.games/")) {
        msg->uri.a += strlen("https://ipv4.games");
      }

#if 0
      // access log
      char method[9] = {0};
      WRITE64LE(method, msg->method);
      LOG("%6P %16s %s %.*s %.*s %.*s %#.*s\n", ipbuf, method,
          msg->uri.b - msg->uri.a, inbuf + msg->uri.a,
          HeaderLength(kHttpCfIpcountry), HeaderData(kHttpCfIpcountry),
          HeaderLength(kHttpSecChUaPlatform), HeaderData(kHttpSecChUaPlatform),
          HeaderLength(kHttpReferer), HeaderData(kHttpReferer));
#endif

      must_close = 1. / g_workers * GetCounter(&g_connections) >= CONCERN_LOAD;

      // export monitoring data
      if (UrlEqual("/statusz")) {
        ServeStatusz(w, outbuf, ip);
        IncrementCounter(&g_statuszrequests);
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
      } else {
        a = 0;
      }

      // wait for server initialization
      while (a)
        if (a->ready)
          break;

      // assert serving
      if (a) {
        struct iovec iov[2];
        IncrementCounter(&g_assetrequests);
        comp = a->gzip.n < a->data.n &&
               HeaderHas(msg, inbuf, kHttpAcceptEncoding, "gzip", 4);
        ////////////////////////////////////////
        pthread_rwlock_rdlock(&a->lock);
        if (HasHeader(kHttpIfModifiedSince) &&
            a->mtim.tv_sec <=
                ParseHttpDateTime(HeaderData(kHttpIfModifiedSince),
                                  HeaderLength(kHttpIfModifiedSince))) {
          p = stpcpy(outbuf,
                     "HTTP/1.1 304 Not Modified\r\n" STANDARD_RESPONSE_HEADERS
                     "Vary: Accept-Encoding\r\n"
                     "Date: ");
          p = FormatDate(p);
          if (must_close)
            p = stpcpy(p, "\r\nConnection: close");
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
          sent = Write(w, outbuf, outmsglen);
        } else {
          p = stpcpy(outbuf, "HTTP/1.1 200 OK\r\n" STANDARD_RESPONSE_HEADERS
                             "Vary: Accept-Encoding\r\n"
                             "Date: ");
          p = FormatDate(p);
          if (must_close)
            p = stpcpy(p, "\r\nConnection: close");
          p = stpcpy(p, "\r\nLast-Modified: ");
          p = stpcpy(p, a->lastmodified);
          p = stpcpy(p, "\r\nContent-Type: ");
          p = stpcpy(p, a->type);
          p = stpcpy(p, "\r\nCache-Control: ");
          ksnprintf(cashbuf, sizeof(cashbuf), "max-age=%d, must-revalidate",
                    a->cash);
          p = stpcpy(p, cashbuf);
          if (comp)
            p = stpcpy(p, "\r\nContent-Encoding: gzip");
          p = stpcpy(p, "\r\nContent-Length: ");
          d = comp ? a->gzip : a->data;
          p = FormatInt32(p, d.n);
          p = stpcpy(p, "\r\n\r\n");
          iov[0].iov_base = outbuf;
          iov[0].iov_len = p - outbuf;
          iov[1].iov_base = d.p;
          iov[1].iov_len = msg->method == kHttpHead ? 0 : d.n;
          outmsglen = iov[0].iov_len + iov[1].iov_len;
          sent = Writev(w, iov, 2);
        }
        pthread_rwlock_unlock(&a->lock);
        ////////////////////////////////////////

      } else if (UrlStartsWith("/ip")) {
        // what is my ip endpoint
        IncrementCounter(&g_iprequests);
        p = stpcpy(outbuf, "HTTP/1.1 200 OK\r\n" STANDARD_RESPONSE_HEADERS
                           "Vary: Accept\r\n"
                           "Content-Type: text/plain\r\n"
                           "Cache-Control: max-age=3600, private\r\n"
                           "Date: ");
        p = FormatDate(p);
        if (must_close)
          p = stpcpy(p, "\r\nConnection: close");
        p = stpcpy(p, "\r\nContent-Length: ");
        p = FormatInt32(p, strlen(ipbuf));
        p = stpcpy(p, "\r\n\r\n");
        p = stpcpy(p, ipbuf);
        outmsglen = p - outbuf;
        sent = Write(w, outbuf, outmsglen);

      } else if (UrlStartsWith("/claim")) {
        // ip:name registration endpoint
        IncrementCounter(&g_claimrequests);
        struct Claim v = {.ip = ip, .created = timespec_real().tv_sec};
        if (GetNick(inbuf, msg, &v)) {
          if (AddClaim(&g_claims, &v)) {
            IncrementCounter(&g_claimsenqueued);
            DEBUG("%s claimed by %s\n", ipbuf, v.name);
            if (HasHeader(kHttpAccept) &&
                (HeaderHas(msg, inbuf, kHttpAccept, "image/*", 7) ||
                 HeaderHas(msg, inbuf, kHttpAccept, "image/gif", 9))) {
              IncrementCounter(&g_imageclaims);
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
              IncrementCounter(&g_plainclaims);
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
              IncrementCounter(&g_htmlclaims);
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
              IncrementCounter(&g_emptyclaims);
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
            sent = Write(w, outbuf, p - outbuf);
          } else {
            LOG("%s: 503 Claims Queue Full\n", ipbuf);
            WriteStr(w, "HTTP/1.1 503 Claims Queue Full\r\n"
                        "Content-Type: text/plain\r\n"
                        "Connection: close\r\n"
                        "\r\n"
                        "Claims Queue Full\n");
            IncrementCounter(&g_queuefulls);
          }
        } else {
          IncrementCounter(&g_invalidnames);
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
          sent = Write(w, outbuf, p - outbuf);
        }
        break;

      } else {
        // default endpoint
        IncrementCounter(&g_notfounds);
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
        p = stpcpy(p, "\r\nConnection: close");
        p = stpcpy(p, "\r\nContent-Length: ");
        p = FormatInt32(p, strlen(q));
        p = stpcpy(p, "\r\n\r\n");
        p = stpcpy(p, q);
        outmsglen = p - outbuf;
        sent = Write(w, outbuf, p - outbuf);
        break;
      }

      // if the client isn't pipelining and write() wrote the full
      // amount, then since we sent the content length and checked
      // that the client didn't attach a payload, we are so synced
      // thus we can safely process more messages
    } while (!must_close &&                                 //
             got == inmsglen &&                             //
             sent == outmsglen &&                           //
             !HasHeader(kHttpContentLength) &&              //
             !HasHeader(kHttpTransferEncoding) &&           //
             !HeaderEqualCase(kHttpConnection, "close") &&  //
             (msg->method == kHttpGet ||                    //
              msg->method == kHttpHead));
    if (w->ssl_established)
      mbedtls_ssl_close_notify(&w->ssl);
    if (w->ssl_activated)
      mbedtls_ssl_session_reset(&w->ssl);
    DestroyHttpMessage(msg);
    close(w->sock);
    w->sock = -1;
    w->connected = false;
    w->ssl_activated = false;
    w->got_first_recv = false;
    w->ssl_established = false;
    DecrementCounter(&g_connections);
  }

  pthread_cleanup_pop(true);
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
  npassert(Z_STREAM_END == deflate(&zs, Z_FINISH));
  npassert(Z_OK == deflateEnd(&zs));
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
  pthread_rwlock_init(&a.lock, 0);
  npassert(!stat(path, &st));
  npassert((a.data.p = xslurp(path, &a.data.n)));
  a.type = type;
  a.cash = cash;
  npassert((a.path = strdup(path)));
  a.mtim = st.st_mtim;
  npassert((a.gzip = Gzip(a.data)).p);
  FormatUnixHttpDateTime(a.lastmodified, a.mtim.tv_sec);
  a.ready = true;
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
    if (rc != st.st_size)
      goto OnError;
    CHECK_MEM((gzip = Gzip(data)).p);
    //!//!//!//!//!//!//!//!//!//!//!//!//!/
    pthread_rwlock_wrlock(&a->lock);
    f[0] = a->data.p;
    f[1] = a->gzip.p;
    a->data = data;
    a->gzip = gzip;
    a->mtim = st.st_mtim;
    memcpy(a->lastmodified, lastmodified, 32);
    pthread_rwlock_unlock(&a->lock);
    //!//!//!//!//!//!//!//!//!//!//!//!//!/
    a->ready = true;
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
  pthread_rwlock_destroy(&a->lock);
  free(a->path);
  free(a->data.p);
  free(a->gzip.p);
}

void OnCtrlC(int sig) {
  is_shutting_down = 1;
}

// parses cli arguments
void GetOpts(int argc, char *argv[]) {
  int opt;
  int64_t ip;
  while ((opt = getopt(argc, argv, GETOPTS)) != -1) {
    switch (opt) {
      case 'U':
        g_setuid = optarg;
        break;
      case 'G':
        g_setgid = optarg;
        break;
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
            LOG("whitelisted %s\n", optarg);
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
  struct Asset t;

  // serialize report operations
  if (g_online)
    pthread_mutex_lock(&report_lock);
  pthread_cleanup_push(unlock_mutex, g_online ? &report_lock : 0);

  if (g_online) {
    // wait until after the next checkpoint
    long cc = g_checkpoint_count;
    pthread_mutex_lock(&checkpoint_lock);
    pthread_cleanup_push(unlock_mutex, &checkpoint_lock);
    while (g_checkpoint_count == cc)
      pthread_cond_wait(&checkpoint_cond, &checkpoint_lock);
    pthread_cleanup_pop(true);
  }

  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0);
  if (gen(&t, x, y)) {
    void *f[2];
    //!//!//!//!//!//!//!//!//!//!//!//!//!/
    pthread_rwlock_wrlock(&a->lock);
    f[0] = a->data.p;
    f[1] = a->gzip.p;
    a->data = t.data;
    a->gzip = t.gzip;
    a->mtim = t.mtim;
    a->type = t.type;
    a->cash = t.cash;
    memcpy(a->lastmodified, t.lastmodified, 32);
    pthread_rwlock_unlock(&a->lock);
    //!//!//!//!//!//!//!//!//!//!//!//!//!/
    a->ready = true;
    free(f[0]);
    free(f[1]);
  }
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);

  // release report lock
  pthread_cleanup_pop(true);
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
    if (rc != SQLITE_ROW)
      CHECK_DB(rc);
    strlcpy(name2, (void *)sqlite3_column_text(stmt, 0), sizeof(name2));
    if (!IsValidNick(name2, -1))
      continue;
    if (strcmp(name1, name2)) {
      // name changed
      if (namestate)
        CHECK_SYS(appends(&a.data.p, "],\n"));
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
  if (namestate)
    CHECK_SYS(appends(&a.data.p, "]\n"));
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

void *GenerateScoreAllTime(void *arg) {
  LOG("%H regenerating score...\n");
  Update(&g_asset.score, GenerateScore, -1, MS2CASH(SCORE_UPDATE_MS));
  return 0;
}

// single thread for regenerating the user scores json
void *ScoreWorker(void *arg) {
  pthread_setname_np(pthread_self(), "ScoreAll");
  for (;;) {
    usleep(SCORE_UPDATE_MS * 1000);
    GenerateScoreAllTime(0);
  }
}

void *GenerateScoreHour(void *arg) {
  LOG("%H regenerating hour score...\n");
  Update(&g_asset.score_hour, GenerateScore, 60L * 60,
         MS2CASH(SCORE_H_UPDATE_MS));
  return 0;
}

// single thread for regenerating the user scores json
void *ScoreHourWorker(void *arg) {
  pthread_setname_np(pthread_self(), "ScoreHour");
  for (;;) {
    usleep(SCORE_H_UPDATE_MS * 1000);
    GenerateScoreHour(0);
  }
}

void *GenerateScoreDay(void *arg) {
  LOG("%H regenerating day score...\n");
  Update(&g_asset.score_day, GenerateScore, 60L * 60 * 24,
         MS2CASH(SCORE_D_UPDATE_MS));
  return 0;
}

// single thread for regenerating the user scores json
void *ScoreDayWorker(void *arg) {
  pthread_setname_np(pthread_self(), "ScoreDay");
  for (;;) {
    usleep(SCORE_D_UPDATE_MS * 1000);
    GenerateScoreDay(0);
  }
}

void *GenerateScoreWeek(void *arg) {
  LOG("%H regenerating week score...\n");
  Update(&g_asset.score_week, GenerateScore, 60L * 60 * 24 * 7,
         MS2CASH(SCORE_W_UPDATE_MS));
  return 0;
}

// single thread for regenerating the user scores json
void *ScoreWeekWorker(void *arg) {
  pthread_setname_np(pthread_self(), "ScoreWeek");
  for (;;) {
    usleep(SCORE_W_UPDATE_MS * 1000);
    GenerateScoreWeek(0);
  }
}

void *GenerateScoreMonth(void *arg) {
  LOG("%H regenerating month score...\n");
  Update(&g_asset.score_month, GenerateScore, 60L * 60 * 24 * 30,
         MS2CASH(SCORE_M_UPDATE_MS));
  return 0;
}

// single thread for regenerating the user scores json
void *ScoreMonthWorker(void *arg) {
  pthread_setname_np(pthread_self(), "ScoreMonth");
  for (;;) {
    usleep(SCORE_M_UPDATE_MS * 1000);
    GenerateScoreMonth(0);
  }
}

void InitRecent(void) {
  int rc;
  bool once;
  char *sb = 0;
  sqlite3 *db = 0;
  size_t sblen = 0;
  const char *text;
  sqlite3_stmt *stmt = 0;
  struct Asset *a, t = {0};
  CHECK_SQL(DbOpen("db.sqlite3", &db));
  CHECK_DB(DbPrepare(db, &stmt,
                     "SELECT ip, nick, created\n"
                     "FROM land\n"
                     "WHERE created NOT NULL\n"
                     "ORDER BY created DESC\n"
                     "LIMIT 50"));
  t.mtim = timespec_real();
  FormatUnixHttpDateTime(t.lastmodified, t.mtim.tv_sec);
  CHECK_SYS(appends(&t.data.p, "{\n"));
  CHECK_SYS(appendf(&t.data.p, "\"now\":[%ld,%ld],\n", t.mtim.tv_sec,
                    t.mtim.tv_nsec));
  CHECK_SYS(appends(&t.data.p, "\"recent\":[\n"));
  CHECK_SQL(sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, 0));
  for (once = false; (rc = DbStep(stmt)) != SQLITE_DONE; once = true) {
    if (rc != SQLITE_ROW)
      CHECK_SQL(rc);
    if ((text = (const char *)sqlite3_column_text(stmt, 1))) {
      if (once)
        CHECK_SYS(appends(&t.data.p, ",\n"));
      CHECK_SYS(appendf(&t.data.p, "[%ld,\"%s\",%ld]",
                        sqlite3_column_int64(stmt, 0),
                        EscapeJsStringLiteral(&sb, &sblen, text, -1, 0),
                        sqlite3_column_int64(stmt, 2)));
    }
  }
  CHECK_SQL(sqlite3_reset(stmt));
  CHECK_SQL(sqlite3_exec(db, "END TRANSACTION", 0, 0, 0));
  CHECK_SYS(appends(&t.data.p, "]}\n"));
  t.data.n = appendz(t.data.p).i;
  CHECK_MEM((t.gzip = Gzip(t.data)).p);
  // deploy json
  a = &g_asset.recent;
  //!//!//!//!//!//!//!//!//!//!//!//!//!/
  pthread_rwlock_wrlock(&a->lock);
  a->data = t.data;
  a->gzip = t.gzip;
  a->mtim = t.mtim;
  a->type = "application/json";
  a->cash = 0;
  memcpy(a->lastmodified, t.lastmodified, 32);
  pthread_rwlock_unlock(&a->lock);
  //!//!//!//!//!//!//!//!//!//!//!//!//!/
  a->ready = true;
  sqlite3_finalize(stmt);
  sqlite3_close(db);
  free(sb);
  return;
OnError:
  abort();
}

// single thread for inserting batched claims into the database
// this helps us avoid over 9000 threads having fcntl bloodbath
void *ClaimWorker(void *arg) {
  bool once;
  void *f[2];
  sqlite3 *db;
  int i, n, rc;
  char *sb = 0;
  size_t sblen = 0;
  const char *text;
  sqlite3_stmt *stmt;
  struct Asset *a, t = {0};
  sqlite3_stmt *recent_stmt;
  struct timespec last_checkpoint = timespec_real();
  struct Claim *v = gc(calloc(BATCH_MAX, sizeof(struct Claim)));
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0);
  pthread_setname_np(pthread_self(), "ClaimWorker");
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
  CHECK_DB(DbPrepare(db, &recent_stmt,
                     "SELECT ip, nick, created\n"
                     "FROM land\n"
                     "WHERE created NOT NULL\n"
                     "ORDER BY created DESC\n"
                     "LIMIT 50"));
  while ((n = GetClaims(&g_claims, v, BATCH_MAX))) {
    CHECK_SQL(sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, 0));
    for (i = 0; i < n; ++i) {
      CHECK_DB(sqlite3_bind_int64(stmt, 1, v[i].ip));
      CHECK_DB(sqlite3_bind_text(stmt, 2, v[i].name, -1, SQLITE_TRANSIENT));
      CHECK_DB(sqlite3_bind_int64(stmt, 3, v[i].created));
      CHECK_DB((rc = DbStep(stmt)) == SQLITE_DONE ? SQLITE_OK : rc);
      CHECK_DB(sqlite3_reset(stmt));
    }
    CHECK_SQL(sqlite3_exec(db, "COMMIT TRANSACTION", 0, 0, 0));
    DEBUG("Committed %d claims\n", n);
    AddCounter(&g_claimsprocessed, n);
    g_lastbatchsize = n;
    ++g_batches;

    // regenerate /recent json
    t.mtim = timespec_real();
    FormatUnixHttpDateTime(t.lastmodified, t.mtim.tv_sec);
    CHECK_SYS(appends(&t.data.p, "{\n"));
    CHECK_SYS(appendf(&t.data.p, "\"now\":[%ld,%ld],\n", t.mtim.tv_sec,
                      t.mtim.tv_nsec));
    CHECK_SYS(appends(&t.data.p, "\"recent\":[\n"));
    CHECK_SQL(sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, 0));
    for (once = false; (rc = DbStep(recent_stmt)) != SQLITE_DONE; once = true) {
      if (rc != SQLITE_ROW)
        CHECK_SQL(rc);
      if ((text = (const char *)sqlite3_column_text(recent_stmt, 1))) {
        if (once)
          CHECK_SYS(appends(&t.data.p, ",\n"));
        CHECK_SYS(appendf(&t.data.p, "[%ld,\"%s\",%ld]",
                          sqlite3_column_int64(recent_stmt, 0),
                          EscapeJsStringLiteral(&sb, &sblen, text, -1, 0),
                          sqlite3_column_int64(recent_stmt, 2)));
      }
    }
    CHECK_SQL(sqlite3_reset(recent_stmt));
    CHECK_SQL(sqlite3_exec(db, "END TRANSACTION", 0, 0, 0));
    CHECK_SYS(appends(&t.data.p, "]}\n"));
    t.data.n = appendz(t.data.p).i;
    CHECK_MEM((t.gzip = Gzip(t.data)).p);
    // deploy json
    a = &g_asset.recent;
    //!//!//!//!//!//!//!//!//!//!//!//!//!/
    pthread_rwlock_wrlock(&a->lock);
    f[0] = a->data.p;
    f[1] = a->gzip.p;
    a->data = t.data;
    a->gzip = t.gzip;
    a->mtim = t.mtim;
    a->type = "application/json";
    a->cash = 0;
    memcpy(a->lastmodified, t.lastmodified, 32);
    pthread_rwlock_unlock(&a->lock);
    //!//!//!//!//!//!//!//!//!//!//!//!//!/
    a->ready = true;
    bzero(&t, sizeof(t));
    free(f[0]);
    free(f[1]);

    // force database checkpoint
    struct timespec now = timespec_real();
    if (timespec_cmp(timespec_sub(now, last_checkpoint),
                     timespec_frommillis(CHECKPOINT_MS)) >= 0) {
      int ckpt_log, ckpt_ckpt;
      int rc = sqlite3_wal_checkpoint_v2(db, NULL, SQLITE_CHECKPOINT_PASSIVE,
                                         &ckpt_log, &ckpt_ckpt);
      if (rc == SQLITE_OK) {
        g_walpages = ckpt_log;
        g_walprocessed = ckpt_ckpt;
        if (ckpt_log == ckpt_ckpt) {
          // if the write ahead log successfully flushed then the
          // background scoreboard generating threads may proceed
          pthread_mutex_lock(&checkpoint_lock);
          ++g_checkpoint_count;
          pthread_cond_signal(&checkpoint_cond);
          pthread_mutex_unlock(&checkpoint_lock);
        }
      } else {
        IncrementCounter(&g_checkpointfails);
      }
      last_checkpoint = timespec_real();
    }

    // don't go too fast
    if (!is_shutting_down) {
      pthread_setcancelstate(PTHREAD_CANCEL_MASKED, 0);
      usleep(CLAIM_BATCH_MS * 1000);
      pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0);
    }
  }
  CHECK_DB(sqlite3_finalize(recent_stmt));
  CHECK_DB(sqlite3_finalize(stmt));
  CHECK_SQL(sqlite3_close(db));
  free(sb);
  return 0;
OnError:
  sqlite3_finalize(recent_stmt);
  sqlite3_finalize(stmt);
  sqlite3_close(db);
  goto StartOver;
}

// worker for refilling token buckets
void *ReplenishWorker(void *arg) {
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
  pthread_setname_np(pthread_self(), "Replenisher");
  for (struct timespec ts = timespec_real();;
       ts = timespec_add(ts, timespec_frommillis(TB_INTERVAL))) {
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &ts, 0);
    ReplenishTokens(g_tok.w, TB_WORDS);
  }
}

void SpawnWorker(intptr_t i) {
  sigset_t thmask;
  pthread_attr_t attr;
  sigfillset(&thmask);
  sigdelset(&thmask, SIGABRT);
  sigdelset(&thmask, SIGTRAP);
  sigdelset(&thmask, SIGFPE);
  sigdelset(&thmask, SIGBUS);
  sigdelset(&thmask, SIGSEGV);
  sigdelset(&thmask, SIGILL);
  sigdelset(&thmask, SIGXCPU);
  sigdelset(&thmask, SIGXFSZ);
  pthread_attr_init(&attr);
  pthread_attr_setsigmask_np(&attr, &thmask);
  pthread_attr_setstacksize(&attr, 128 * 1024);
  pthread_attr_setguardsize(&attr, sysconf(_SC_PAGESIZE));
  pthread_attr_setsigaltstacksize_np(&attr, sysconf(_SC_MINSIGSTKSZ) + 32768);
  pthread_create(&g_worker[i].th, &attr, HttpWorker, (void *)i);
  pthread_attr_destroy(&attr);
}

// we're permissive in allowing http connection keepalive until the
// moment worker resources start becoming scarce. when that happens
// we'll (1) cancel read operations that have not sent us a message
// in a while; (2) cancel clients who are sending lots of messages.
void Meltdown(void) {
  /* int i, marks; */
  /* struct timespec now; */
  IncrementCounter(&g_meltdowns);
  /* LOG("%H panicking because %d out of %d workers is connected\n", */
  /*     GetCounter(&g_connections), g_workers); */
  /* now = timespec_real(); */
  /* for (marks = i = 0; i < g_workers; ++i) { */
  /*   if (g_worker[i].connected && */
  /*       (g_worker[i].msgcount > PANIC_MSGS || */
  /*        timespec_cmp(timespec_sub(now, g_worker[i].startread), */
  /*                     timespec_frommillis(MELTALIVE_MS)) >= 0)) { */
  /*     pthread_cancel(g_worker[i].th); */
  /*     pthread_join(g_worker[i].th, 0); */
  /*     SpawnWorker(i); */
  /*     ++marks; */
  /*   } */
  /* } */
  /* LOG("Melted down %d connections\n", marks); */
}

// main thread worker
void *Supervisor(void *arg) {
  while (!is_shutting_down) {

    // check for updates to web assets on disk
    ReloadAsset(&g_asset.index);
    ReloadAsset(&g_asset.about);
    ReloadAsset(&g_asset.user);
    ReloadAsset(&g_asset.favicon);

    // check if server is about to explode
    if (g_workers > 1 &&
        1. / g_workers * GetCounter(&g_connections) > PANIC_LOAD)
      Meltdown();

    // spawn replacements for crashed workers
    for (int i = 0; i < g_workers; ++i) {
      if (g_worker[i].dead) {
        pthread_join(g_worker[i].th, 0);
        SpawnWorker(i);
      }
    }

    // wait a little bit
    if (!is_shutting_down)
      usleep(SUPERVISE_MS * 1000);
  }
  return 0;
}

void CheckDatabase(void) {
  int x, y;
  sqlite3 *db;
  CHECK_SQL(DbOpen("db.sqlite3", &db));
  if (g_integrity) {
    LOG("%H Checking database integrity...\n");
    CHECK_SQL(sqlite3_exec(db, "PRAGMA integrity_check", 0, 0, 0));
    LOG("%H Vacuuming database...\n");
    CHECK_SQL(sqlite3_exec(db, "VACUUM", 0, 0, 0));
  }
  LOG("%H checkpointing database...\n");
  sqlite3_wal_checkpoint_v2(db, NULL, SQLITE_CHECKPOINT_TRUNCATE, &x, &y);
  CHECK_SQL(sqlite3_close(db));
  return;
OnError:
  exit(1);
}

char *hexcpy(char *p, unsigned long x) {
  int k = x ? (__builtin_clzl(x) ^ 63) + 1 : 1;
  k = (k + 3) & -4;
  while (k > 0)
    *p++ = "0123456789abcdef"[(x >> (k -= 4)) & 15];
  *p = '\0';
  return p;
}

char *describe_backtrace(char *p, size_t len, const struct StackFrame *sf) {
  char *pe = p + len;
  bool gotsome = false;

  // show address of each function
  while (sf) {
    if (kisdangerous(sf)) {
      if (p + 1 + 9 + 1 < pe) {
        if (gotsome)
          *p++ = ' ';
        p = stpcpy(p, "DANGEROUS");
        if (p + 16 + 1 < pe) {
          *p++ = ' ';
          p = hexcpy(p, (long)sf);
        }
      }
      break;
    }
    if (p + 16 + 1 < pe) {
      unsigned char *ip = (unsigned char *)sf->addr;
#ifdef __x86_64__
      // x86 advances the progrem counter before an instruction
      // begins executing. return addresses in backtraces shall
      // point to code after the call, which means addr2line is
      // going to print unrelated code unless we fixup the addr
      if (!kisdangerous(ip))
        ip -= __is_call(ip);
#endif
      if (gotsome)
        *p++ = ' ';
      else
        gotsome = true;
      p = hexcpy(p, (long)ip);
    } else {
      break;
    }
    sf = sf->next;
  }

  // terminate string
  if (p < pe)
    *p = '\0';
  return p;
}

//                         abashed the devil stood
//                      and felt how awful goodness is
char *describe_crash(char *buf, size_t len, int sig, siginfo_t *si, void *arg) {
  char *p = buf;

  // check minimum length
  if (len < 64)
    return p;

  // describe crash
  p = stpcpy(p, strsignal(sig));
  if (si &&               //
      (sig == SIGFPE ||   //
       sig == SIGILL ||   //
       sig == SIGBUS ||   //
       sig == SIGSEGV ||  //
       sig == SIGTRAP)) {
    p = stpcpy(p, " at ");
    p = hexcpy(p, (long)si->si_addr);
  }

  // get stack frame daisy chain
  struct StackFrame pc;
  struct StackFrame *sf;
  ucontext_t *ctx;
  if ((ctx = (ucontext_t *)arg)) {
    pc.addr = ctx->uc_mcontext.PC;
    pc.next = (struct StackFrame *)ctx->uc_mcontext.BP;
    sf = &pc;
  } else {
    sf = (struct StackFrame *)__builtin_frame_address(0);
  }

  // describe backtrace
  p = stpcpy(p, " bt ");
  p = describe_backtrace(p, len - (p - buf), sf);

  return p;
}

void on_crash_signal(int sig, siginfo_t *si, void *arg) {
  char *p;
  char message[512];
  p = describe_crash(message, sizeof(message), sig, si, arg);
  write(g_crash_fd, "crash: ", 7);
  write(g_crash_fd, message, p - message);
  write(g_crash_fd, "\n", 1);
  write(2, "crash: ", 7);
  write(2, message, p - message);
  write(2, "\n", 1);
  _Exit(1);
  // pthread_exit(PTHREAD_CANCELED);
}

void make_server_crash_resistant(void) {
  const char *path = "crash.log";
  if ((g_crash_fd = open(path, O_CREAT | O_WRONLY | O_APPEND, 0644)) == -1) {
    fprintf(stderr, "%s: %s\n", path, strerror(errno));
    exit(1);
  }

  struct sigaction sa;
  sa.sa_flags = SA_SIGINFO;
  sigemptyset(&sa.sa_mask);

  sa.sa_sigaction = on_crash_signal;
  sigaddset(&sa.sa_mask, SIGABRT);
  sigaddset(&sa.sa_mask, SIGTRAP);
  sigaddset(&sa.sa_mask, SIGFPE);
  sigaddset(&sa.sa_mask, SIGBUS);
  sigaddset(&sa.sa_mask, SIGSEGV);
  sigaddset(&sa.sa_mask, SIGILL);
  sigaddset(&sa.sa_mask, SIGXCPU);
  sigaddset(&sa.sa_mask, SIGXFSZ);

  sigaction(SIGABRT, &sa, 0);
  sigaction(SIGTRAP, &sa, 0);
  sigaction(SIGFPE, &sa, 0);
  sigaction(SIGILL, &sa, 0);
  sigaction(SIGXCPU, &sa, 0);
  sigaction(SIGXFSZ, &sa, 0);

  sa.sa_flags |= SA_ONSTACK;
  sigaction(SIGBUS, &sa, 0);
  sigaction(SIGSEGV, &sa, 0);
}

#if DETECT_LEAKS
static pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;
void *malloc(size_t n) {
  pthread_mutex_lock(&mu);
  void *r = cosmo_leak_track(__dlmalloc(n));
  pthread_mutex_unlock(&mu);
  return r;
}
void *calloc(size_t n, size_t z) {
  pthread_mutex_lock(&mu);
  void *r = cosmo_leak_track(__dlcalloc(n, z));
  pthread_mutex_unlock(&mu);
  return r;
}
void *realloc(void *p, size_t n) {
  pthread_mutex_lock(&mu);
  void *t = cosmo_leak_track(__dlrealloc(p, n));
  if (t && t != p)
    cosmo_leak_untrack(p);
  pthread_mutex_unlock(&mu);
  return t;
}
void *memalign(size_t a, size_t n) {
  pthread_mutex_lock(&mu);
  void *r = cosmo_leak_track(__dlmemalign(a, n));
  pthread_mutex_unlock(&mu);
  return r;
}
void free(void *p) {
  pthread_mutex_lock(&mu);
  __dlfree(cosmo_leak_untrack(p));
  pthread_mutex_unlock(&mu);
}
#endif

struct File LoadFile(const char *path) {
  struct File f;
  if (!(f.data = xslurp(path, &f.size))) {
    perror(path);
    exit(1);
  }
  return f;
}

int main(int argc, char *argv[]) {
  signal(SIGPIPE, SIG_IGN);
  make_server_crash_resistant();

  if (!IsQemuUser()) {
    if (pledge(0, 0)) {
      fprintf(stderr, "%s: this OS doesn't support pledge() security\n",
              argv[0]);
      exit(1);
    }
    if (unveil("", 0) < 2) {
      fprintf(stderr, "%s: need OpenBSD or Landlock LSM v3+\n", argv[0]);
      exit(1);
    }
  }

  if (IsLinux())
    system("sudo sh -c 'echo 1 >/proc/sys/net/ipv4/tcp_fastopen'");

  // we don't have proper futexes on these platforms
  // we'll be somewhat less aggressive about workers
  if (IsXnu() || IsNetbsd())
    g_workers = MIN(g_workers, (unsigned)cosmo_cpu_count());

  // load flags
  GetOpts(argc, argv);

  // user interface
  kprintf("\
 |               _|                    \n\
 __| |   |  __| | \\ \\  \\   / _` |  __|\n\
 |   |   | |    __|\\ \\  \\ / (   | |\n\
\\__|\\__,_|_|   _|   \\_/\\_/ \\__,_|_|\n");
  npassert(!chdir("/opt/turfwar"));
  putenv("TMPDIR=/opt/turfwar/tmp");

  // load serving certificates
  rsacert = LoadFile(CERT_PATH "/fullchain.pem");
  rsakey = LoadFile(CERT_PATH "/privkey.pem");
  ecdsacert = LoadFile(CERT_PATH "-ecdsa/fullchain.pem");
  ecdsakey = LoadFile(CERT_PATH "-ecdsa/privkey.pem");

  // setup blackholed
  if ((g_blackhole.fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
    kprintf("error: socket(AF_UNIX) failed: %s\n", strerror(errno));
    _Exit(3);
  }
  if (!Blackhole(0)) {
    kprintf("turfwar isn't able to protect your kernel from level 4 ddos\n");
    kprintf("please run the blackholed program, see https://justine.lol/\n");
  }

  // sql init
  sqlite3_initialize();
  CheckDatabase();

  // configure threads
  sigset_t thmask;
  sigfillset(&thmask);
  sigdelset(&thmask, SIGABRT);
  sigdelset(&thmask, SIGTRAP);
  sigdelset(&thmask, SIGFPE);
  sigdelset(&thmask, SIGBUS);
  sigdelset(&thmask, SIGSEGV);
  sigdelset(&thmask, SIGILL);
  sigdelset(&thmask, SIGXCPU);
  sigdelset(&thmask, SIGXFSZ);
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setsigmask_np(&attr, &thmask);
  pthread_attr_setstacksize(&attr, 128 * 1024);
  pthread_attr_setguardsize(&attr, sysconf(_SC_PAGESIZE));
  pthread_attr_setsigaltstacksize_np(&attr, sysconf(_SC_MINSIGSTKSZ) + 32768);

  // generate scoreboards
  npassert(!pthread_create(&scorer, &attr, GenerateScoreAllTime, 0));
  npassert(!pthread_create(&scorer_hour, &attr, GenerateScoreHour, 0));
  npassert(!pthread_create(&scorer_day, &attr, GenerateScoreDay, 0));
  npassert(!pthread_create(&scorer_week, &attr, GenerateScoreWeek, 0));
  npassert(!pthread_create(&scorer_month, &attr, GenerateScoreMonth, 0));
  npassert(!pthread_join(scorer, 0));
  npassert(!pthread_join(scorer_hour, 0));
  npassert(!pthread_join(scorer_day, 0));
  npassert(!pthread_join(scorer_week, 0));
  npassert(!pthread_join(scorer_month, 0));

  // create server sockets
  Listen();
  g_online = true;

  // reduce privileges
  if (g_setgid) {
    struct group *p;
    if (!(p = getgrnam(g_setgid))) {
      kprintf("%s: group not found\n", g_setgid);
      _Exit(1);
    }
    npassert(!setgid(p->gr_gid));
  }
  if (g_setuid) {
    struct passwd *p;
    if (!(p = getpwnam(g_setuid))) {
      kprintf("%s: user not found\n", g_setuid);
      _Exit(1);
    }
    npassert(!setuid(p->pw_uid));
  }

  // the power to serve
  if (g_daemonize) {
    if (fork() > 0)
      _Exit(0);
    setsid();
    if (fork() > 0)
      _Exit(0);
    umask(0);
    closefrom(0);
    npassert(0 == open(_PATH_DEVNULL, O_RDWR));
    npassert(1 == dup(0));
    npassert(2 == open("turfwar.log", O_CREAT | O_WRONLY | O_APPEND, 0644));
  }

  // fill token buckets
  g_tok.b = malloc(TB_BYTES);
  memset(g_tok.b, 127, TB_BYTES);

  // server lifecycle locks
  g_started = timespec_real();

  // load static assets into memory and pre-zip them
  update_time();
  InitRecent();
  g_asset.index = LoadAsset("index.html", "text/html; charset=utf-8", 900);
  g_asset.about = LoadAsset("about.html", "text/html; charset=utf-8", 900);
  g_asset.user = LoadAsset("user.html", "text/html; charset=utf-8", 900);
  g_asset.favicon = LoadAsset("favicon.ico", "image/vnd.microsoft.icon", 86400);

  // sandbox ourselves
  __pledge_mode = PLEDGE_PENALTY_RETURN_EPERM;
  npassert(!unveil("/opt/turfwar", "rwc"));
  npassert(!unveil(0, 0));
  if (!IsOpenbsd())
    npassert(!pledge("stdio flock rpath wpath cpath anet", 0));

  // shutdown signals
  struct sigaction sa;
  sa.sa_flags = 0;
  sa.sa_handler = OnCtrlC;
  sigfillset(&sa.sa_mask);
  sigaction(SIGHUP, &sa, 0);
  sigaction(SIGINT, &sa, 0);
  sigaction(SIGTERM, &sa, 0);

  time_init();

  npassert(!pthread_create(&scorer, &attr, ScoreWorker, 0));
  npassert(!pthread_create(&scorer_hour, &attr, ScoreHourWorker, 0));
  npassert(!pthread_create(&scorer_day, &attr, ScoreDayWorker, 0));
  npassert(!pthread_create(&scorer_week, &attr, ScoreWeekWorker, 0));
  npassert(!pthread_create(&scorer_month, &attr, ScoreMonthWorker, 0));
  npassert(!pthread_create(&replenisher, &attr, ReplenishWorker, 0));
  npassert(!pthread_create(&claimer, &attr, ClaimWorker, 0));
  unassert((g_worker = calloc(g_workers, sizeof(*g_worker))));
  for (int id = 0, si = 0; id < g_workers; ++id, ++si) {
    g_worker[id].id = id;
    g_worker[id].sock = -1;
    npassert(
        !pthread_create(&g_worker[id].th, &attr, HttpWorker, &g_worker[id]));
  }

  // time to serve
  LOG("%H ready\n");
  Supervisor(0);

  // cancel listen()
  LOG("%H interrupting services...\n");
  pthread_cancel(scorer);
  pthread_cancel(scorer_day);
  pthread_cancel(scorer_hour);
  pthread_cancel(scorer_week);
  pthread_cancel(scorer_month);
  pthread_cancel(replenisher);

  LOG("%H joining services...\n");
  npassert(!pthread_join(scorer, 0));
  npassert(!pthread_join(scorer_day, 0));
  npassert(!pthread_join(scorer_hour, 0));
  npassert(!pthread_join(scorer_week, 0));
  npassert(!pthread_join(scorer_month, 0));
  npassert(!pthread_join(replenisher, 0));

  // cancel read() so that keepalive clients finish faster
  LOG("%H interrupting workers...\n");
  for (int i = 0; i < g_workers; ++i)
    if (!g_worker[i].dead)
      pthread_cancel(g_worker[i].th);

  // wait for producers to finish
  LOG("%H joining workers...\n");
  for (int i = 0; i < g_workers; ++i)
    unassert(!pthread_join(g_worker[i].th, 0));

  // now that all workers have terminated, the claims queue must be
  // empty, therefore, it is now safe to send a cancellation to the
  // claims worker thread which waits forever for new claims.
  pthread_cancel(claimer);
  LOG("%H waiting for claims worker...\n");
  unassert(!pthread_join(claimer, 0));
  unassert(!g_claims.count);

  // perform some sanity checks
  unassert(GetCounter(&g_claimsprocessed) == GetCounter(&g_claimsenqueued));

  // free memory
  LOG("%H freeing memory...\n");
  pthread_attr_destroy(&attr);
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
  free(rsacert.data);
  free(rsakey.data);
  free(ecdsacert.data);
  free(ecdsakey.data);
  free(g_worker);
  free(g_tok.b);

  time_destroy();

  LOG("%H goodbye\n");
#if DETECT_LEAKS
  cosmo_leak_finalize();
  if (cosmo_leak_print(0) > 0)
    _Exit(73);
#endif
}
