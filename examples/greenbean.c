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
#include "libc/bits/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/sigbits.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/dce.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/kprintf.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/goodsocket.internal.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/clone.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"
#include "libc/sysv/consts/tcp.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"
#include "net/http/http.h"
#include "net/http/url.h"

/**
 * @fileoverview greenbean lightweighht threaded web server
 *
 *     $ make -j8 o//tool/net/greenbean.com
 *     $ o//tool/net/greenbean.com &
 *     $ printf 'GET /\n\n' | nc 127.0.0.1 8080
 *     HTTP/1.1 200 OK
 *     Server: greenbean/1.o
 *     Referrer-Policy: origin
 *     Cache-Control: private; max-age=0
 *     Content-Type: text/html; charset=utf-8
 *     Date: Sat, 14 May 2022 14:13:07 GMT
 *     Content-Length: 118
 *
 *     <!doctype html>
 *     <title>hello world</title>
 *     <h1>hello world</h1>
 *     <p>this is a fun webpage
 *     <p>hosted by greenbean
 *
 * Like redbean, greenbean has superior performance too, with an
 * advantage on benchmarks biased towards high connection counts
 *
 *     $ sudo wrk -c 300 -t 32 --latency http://10.10.10.124:8080/
 *     Running 10s test @ http://10.10.10.124:8080/
 *       32 threads and 300 connections
 *         Thread Stats   Avg      Stdev     Max   +/- Stdev
 *         Latency     1.07ms    8.27ms 138.55ms   98.58%
 *         Req/Sec    37.98k    12.61k  117.65k    80.11%
 *       Latency Distribution
 *          50%  200.00us
 *          75%  227.00us
 *          90%  303.00us
 *          99%   32.46ms
 *       10033090 requests in 8.31s, 2.96GB read
 *     Requests/sec: 1207983.58
 *     Transfer/sec:    365.19MB
 *
 */

#define THREADS   512
#define HEARTBEAT 100
#define KEEPALIVE 5000
#define LOGGING   0

#define STANDARD_RESPONSE_HEADERS \
  "Server: greenbean/1.o\r\n"     \
  "Referrer-Policy: origin\r\n"   \
  "Cache-Control: private; max-age=0\r\n"

////////////////////////////////////////////////////////////////////////////////
// BEGIN: Chris Wellons's Public Domain GNU Atomics Library

#define BARRIER_INC(x)     __atomic_add_fetch(x, 1, __ATOMIC_SEQ_CST)
#define BARRIER_GET(x)     __atomic_load_n(x, __ATOMIC_SEQ_CST)
#define ATOMIC_LOAD(q)     __atomic_load_n(q, __ATOMIC_ACQUIRE)
#define ATOMIC_RLOAD(q)    __atomic_load_n(q, __ATOMIC_RELAXED)
#define ATOMIC_STORE(q, v) __atomic_store_n(q, v, __ATOMIC_RELEASE)
#define ATOMIC_ADD(q, c)   __atomic_add_fetch(q, c, __ATOMIC_RELEASE)
#define ATOMIC_AND(q, m)   __atomic_and_fetch(q, m, __ATOMIC_RELEASE)
#define ATOMIC_CAS(q, e, d) \
  __atomic_compare_exchange_n(q, e, d, 0, __ATOMIC_RELEASE, __ATOMIC_RELAXED)

// Return the array index for then next value to be pushed. The size of this
// array must be (1 << exp) elements. Write the value into this array index,
// then commit it. With a single-consumer queue, this element store need not
// be atomic. The value will appear in the queue after the commit. Returns
// -1 if the queue is full.
static int queue_push(uint32_t *q, int exp) {
  uint32_t r = ATOMIC_LOAD(q);
  int mask = (1u << exp) - 1;
  int head = r & mask;
  int tail = r >> 16 & mask;
  int next = (head + 1u) & mask;
  if (r & 0x8000) {  // avoid overflow on commit
    ATOMIC_AND(q, ~0x8000);
  }
  return next == tail ? -1 : head;
}

// Commits and completes the push operation. Do this after storing into the
// array. This operation cannot fail.
static void queue_push_commit(uint32_t *q) {
  ATOMIC_ADD(q, 1);
}

// Return the array index for the next value to be popped. The size of this
// array must be (1 << exp) elements. Read from this array index, then
// commit the pop. This element load need not be atomic. The value will be
// removed from the queue after the commit. Returns -1 if the queue is
// empty.
static int queue_pop(uint32_t *q, int exp) {
  uint32_t r = ATOMIC_LOAD(q);
  int mask = (1u << exp) - 1;
  int head = r & mask;
  int tail = r >> 16 & mask;
  return head == tail ? -1 : tail;
}

// Commits and completes the pop operation. Do this after loading from the
// array. This operation cannot fail.
static void queue_pop_commit(uint32_t *q) {
  ATOMIC_ADD(q, 0x10000);
}

// Like queue_pop() but for multiple-consumer queues. The element load must
// be atomic since it is concurrent with the producer's push, though it can
// use a relaxed memory order. The loaded value must not be used unless the
// commit is successful. Stores a temporary "save" to be used at commit.
static int queue_mpop(uint32_t *q, int exp, uint32_t *save) {
  uint32_t r = *save = ATOMIC_LOAD(q);
  int mask = (1u << exp) - 1;
  int head = r & mask;
  int tail = r >> 16 & mask;
  return head == tail ? -1 : tail;
}

// Like queue_pop_commit() but for multiple-consumer queues. It may fail if
// another consumer pops concurrently, in which case the pop must be retried
// from the beginning.
static bool queue_mpop_commit(uint32_t *q, uint32_t save) {
  return ATOMIC_CAS(q, &save, save + 0x10000);
}

// Spin-lock barrier for n threads, where n is a power of two.
// Initialize *barrier to zero.
static void barrier_waitn(int *barrier, int n) {
  int v = BARRIER_INC(barrier);
  if (v & (n - 1)) {
    for (v &= n; (BARRIER_GET(barrier) & n) == v;) {
      donothing;
    }
  }
}

// END: Chris Wellons's Public Domain GNU Atomics Library
////////////////////////////////////////////////////////////////////////////////

int barrier1;
int itsbegun;
int closingtime;
int barrier2;
int itsdone;

int Worker(void *id) {
  int server, yes = 1;

  kprintf(" %d", id);
  barrier_waitn(&barrier1, THREADS);
  itsbegun = true;

  // load balance incoming connections for port 8080 across all threads
  // hangup on any browser clients that lag for more than a few seconds
  struct timeval timeo = {KEEPALIVE / 1000, KEEPALIVE % 1000};
  struct sockaddr_in addr = {.sin_family = AF_INET, .sin_port = htons(8080)};
  CHECK_NE(-1, (server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)));
  setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, &timeo, sizeof(timeo));
  setsockopt(server, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo));
  setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
  setsockopt(server, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes));
  setsockopt(server, SOL_TCP, TCP_FASTOPEN, &yes, sizeof(yes));
  setsockopt(server, SOL_TCP, TCP_QUICKACK, &yes, sizeof(yes));
  CHECK_EQ(0, bind(server, &addr, sizeof(addr)));
  CHECK_EQ(0, listen(server, 10));

  // connection loop
  while (!closingtime) {
    struct tm tm;
    int64_t unixts;
    struct Url url;
    ssize_t got, sent;
    struct timespec ts;
    struct HttpMessage msg;
    uint32_t clientaddrsize;
    struct sockaddr_in clientaddr;
    char inbuf[1500], outbuf[512], *p, *q;
    int clientip, client, inmsglen, outmsglen;

    // wait for client connection
    if (poll(&(struct pollfd){server, POLLIN}, 1, HEARTBEAT) < 1) continue;
    clientaddrsize = sizeof(clientaddr);
    client = accept(server, &clientaddr, &clientaddrsize);

    // accept() can raise a very diverse number of errors but none of
    // them are really true showstoppers that would necessitate us to
    // panic and abort the entire server, so we can just ignore these
    if (client == -1) {
      // we used SO_RCVTIMEO and SO_SNDTIMEO because those settings are
      // inherited by the accepted sockets, but using them also has the
      // side-effect that the listening socket fails with EAGAIN, every
      // several seconds. we can use that to our advantage to check for
      // the ctrl-c shutdowne event; otherwise, we retry the accept call
      continue;
    }

    // message loop
    do {
      // parse the incoming http message
      InitHttpMessage(&msg, kHttpRequest);
      // we're not terrible concerned when errors happen here
      if ((got = read(client, inbuf, sizeof(inbuf))) <= 0) break;
      // check that client message wasn't fragmented into more reads
      if (!(inmsglen = ParseHttpMessage(&msg, inbuf, got))) break;

#if LOGGING
      // log the incoming http message
      clientip = ntohl(clientaddr.sin_addr.s_addr);
      kprintf("#%.4x get some %d.%d.%d.%d:%d %#.*s\n", (intptr_t)id,
              (clientip & 0xff000000) >> 030, (clientip & 0x00ff0000) >> 020,
              (clientip & 0x0000ff00) >> 010, (clientip & 0x000000ff) >> 000,
              ntohs(clientaddr.sin_port), msg.uri.b - msg.uri.a,
              inbuf + msg.uri.a);
#endif

      // display hello world html page for http://127.0.0.1:8080/
      if (msg.method == kHttpGet &&
          (msg.uri.b - msg.uri.a == 1 && inbuf[msg.uri.a + 0] == '/')) {
        q = "<!doctype html>\r\n"
            "<title>hello world</title>\r\n"
            "<h1>hello world</h1>\r\n"
            "<p>this is a fun webpage\r\n"
            "<p>hosted by greenbean\r\n";
        p = stpcpy(outbuf, "HTTP/1.1 200 OK\r\n" STANDARD_RESPONSE_HEADERS
                           "Content-Type: text/html; charset=utf-8\r\n"
                           "Date: ");
        clock_gettime(CLOCK_REALTIME, &ts), unixts = ts.tv_sec;
        p = FormatHttpDateTime(p, gmtime_r(&unixts, &tm));
        p = stpcpy(p, "\r\nContent-Length: ");
        p = FormatInt32(p, strlen(q));
        p = stpcpy(p, "\r\n\r\n");
        p = stpcpy(p, q);
        outmsglen = p - outbuf;
        sent = write(client, outbuf, outmsglen);

      } else {
        // display 404 not found error page for every thing else
        q = "<!doctype html>\r\n"
            "<title>404 not found</title>\r\n"
            "<h1>404 not found</h1>\r\n";
        p = stpcpy(outbuf,
                   "HTTP/1.1 404 Not Found\r\n" STANDARD_RESPONSE_HEADERS
                   "Content-Type: text/html; charset=utf-8\r\n"
                   "Date: ");
        clock_gettime(CLOCK_REALTIME, &ts), unixts = ts.tv_sec;
        p = FormatHttpDateTime(p, gmtime_r(&unixts, &tm));
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
    } while (got == inmsglen && sent == outmsglen &&
             !msg.headers[kHttpContentLength].a &&
             !msg.headers[kHttpTransferEncoding].a &&
             (msg.method == kHttpGet || msg.method == kHttpHead));
    DestroyHttpMessage(&msg);
    close(client);
  }

  // inform the parent that this clone has finished
  close(server);
  kprintf(" %d", id);
  barrier_waitn(&barrier2, THREADS);
  itsdone = true;
  return 0;
}

void OnCtrlC(int sig) {
  closingtime = true;
}

int main(int argc, char *argv[]) {
  /* ShowCrashReports(); */
  int64_t loadtzdbearly;
  kprintf("welcome to greenbean\n");
  gmtime(&loadtzdbearly);
  for (int i = 0; i < THREADS; ++i) {
    void *stack = mmap(0, 65536, PROT_READ | PROT_WRITE,
                       MAP_STACK | MAP_ANONYMOUS, -1, 0);
    clone(Worker, stack, 65536,
          CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND,
          (void *)(intptr_t)i, 0, 0, 0, 0);
  }
  while (!ATOMIC_LOAD(&itsbegun)) usleep(HEARTBEAT * 1000);
  sigaction(SIGINT, &(struct sigaction){.sa_handler = OnCtrlC}, 0);
  kprintf("\nit's begun\n");
  while (!ATOMIC_LOAD(&itsdone)) usleep(HEARTBEAT * 1000);
  kprintf("\nthank you for flying greenbean\n");
}
