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
 *     $ sudo wrk -c 300 -t 32 --latency http://127.0.0.1:8080/
 *     Running 10s test @ http://127.0.0.1:8080/
 *       32 threads and 300 connections
 *         Thread Stats   Avg      Stdev     Max   +/- Stdev
 *         Latency    36.21us  133.39us   8.10ms   98.52%
 *         Req/Sec    73.24k    28.92k  131.06k    47.49%
 *       Latency Distribution
 *          50%   22.00us
 *          75%   29.00us
 *          90%   40.00us
 *          99%  333.00us
 *       4356560 requests in 4.62s, 1.29GB read
 *     Requests/sec: 942663.73
 *     Transfer/sec:    284.98MB
 *
 */

#define THREADS   32
#define HEARTBEAT 500
#define KEEPALIVE 5000
#define LOGGING   0

#define STANDARD_RESPONSE_HEADERS \
  "Server: greenbean/1.o\r\n"     \
  "Referrer-Policy: origin\r\n"   \
  "Cache-Control: private; max-age=0\r\n"

int workers;
int barrier;
int closingtime;

int Worker(void *id) {
  int server, itsover, ready, yes = 1;

  // announce to the main process this has spawned
  kprintf(" #%.2ld", (intptr_t)id);
  __atomic_add_fetch(&workers, 1, __ATOMIC_SEQ_CST);

  // wait for all threads to spawn before we proceed
  for (;;) {
    __atomic_load(&barrier, &ready, __ATOMIC_SEQ_CST);
    if (ready) break;
    __builtin_ia32_pause();
  }

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
  for (;;) {
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

    __atomic_load(&closingtime, &itsover, __ATOMIC_SEQ_CST);
    if (itsover) break;

    if (!IsLinux() &&
        poll(&(struct pollfd){server, POLLIN}, 1, HEARTBEAT) < 1) {
      continue;
    }

    // wait for client connection
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
      // the ctrl-c shutdown event; otherwise, we retry the accept call
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
      kprintf("#%.2ld get some %d.%d.%d.%d:%d %#.*s\n", (intptr_t)id,
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
  kprintf(" #%.2ld", (intptr_t)id);
  __atomic_sub_fetch(&workers, 1, __ATOMIC_SEQ_CST);
  return 0;
}

void OnCtrlC(int sig) {
  closingtime = true;
}

int main(int argc, char *argv[]) {
  int64_t loadtzdbearly;
  int i, gotsome, haveleft, ready = 1;

  ShowCrashReports();
  kprintf("welcome to greenbean\n");
  gmtime(&loadtzdbearly);

  // spawn a bunch of threads
  for (i = 0; i < THREADS; ++i) {
    void *stack = mmap(0, 65536, PROT_READ | PROT_WRITE,
                       MAP_STACK | MAP_ANONYMOUS, -1, 0);
    clone(Worker, stack, 65536,
          CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND,
          (void *)(intptr_t)i, 0, 0, 0, 0);
  }

  // wait for all threads to spawn
  for (;;) {
    __atomic_load(&workers, &gotsome, __ATOMIC_SEQ_CST);
    if (workers == THREADS) break;
    __builtin_ia32_pause();
  }

  // all threads are spawned so unleash the barrier
  kprintf("\ngreenbean is ready to go\n");
  sigaction(SIGINT, &(struct sigaction){.sa_handler = OnCtrlC}, 0);
  __atomic_store(&barrier, &ready, __ATOMIC_SEQ_CST);

  // main process does nothing until it's closing time
  for (;;) {
    __atomic_load(&workers, &haveleft, __ATOMIC_SEQ_CST);
    if (!haveleft) break;
    __builtin_ia32_pause();
    usleep(HEARTBEAT * 1000);
    if (closingtime) {
      kprintf("\rgreenbean is shutting down...\n");
    }
  }

  kprintf("\n");
  kprintf("thank you for flying greenbean\n");
}
