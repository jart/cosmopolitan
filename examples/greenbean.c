#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/assert.h"
#include "libc/bits/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/sigbits.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/dce.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/kprintf.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/threaded.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/sysconf.h"
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

#define PORT      8080
#define HEARTBEAT 100
#define KEEPALIVE 5000
#define LOGGING   0

#define STANDARD_RESPONSE_HEADERS \
  "Server: greenbean/1.o\r\n"     \
  "Referrer-Policy: origin\r\n"   \
  "Cache-Control: private; max-age=0\r\n"

int workers;
int messages;
int connections;
int closingtime;
const char *status;

int Worker(void *id) {
  int server, yes = 1;

  // load balance incoming connections for port 8080 across all threads
  // hangup on any browser clients that lag for more than a few seconds
  struct timeval timeo = {KEEPALIVE / 1000, KEEPALIVE % 1000};
  struct sockaddr_in addr = {.sin_family = AF_INET, .sin_port = htons(PORT)};

  server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (server == -1) {
    if (LOGGING) kprintf("%s() failed %m\n", "socket");
    goto WorkerFinished;
  }

  setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, &timeo, sizeof(timeo));
  setsockopt(server, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo));
  setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
  setsockopt(server, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes));
  setsockopt(server, SOL_TCP, TCP_FASTOPEN, &yes, sizeof(yes));
  setsockopt(server, SOL_TCP, TCP_QUICKACK, &yes, sizeof(yes));
  errno = 0;

  if (bind(server, &addr, sizeof(addr)) == -1) {
    if (LOGGING) kprintf("%s() failed %m\n", "socket");
    goto WorkerFinished;
  }

  listen(server, 1);

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

    // this slows the server down a lot but is needed on non-Linux to
    // react to keyboard ctrl-c
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
      // the ctrl-c shutdowne event; otherwise, we retry the accept call
      continue;
    }

    asm volatile("lock incl\t%0" : "+m"(connections));

    // message loop
    do {
      // parse the incoming http message
      InitHttpMessage(&msg, kHttpRequest);
      // we're not terrible concerned when errors happen here
      if ((got = read(client, inbuf, sizeof(inbuf))) <= 0) break;
      // check that client message wasn't fragmented into more reads
      if (!(inmsglen = ParseHttpMessage(&msg, inbuf, got))) break;
      asm volatile("lock incl\t%0" : "+m"(messages));

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
    asm volatile("lock decl\t%0" : "+m"(connections));
  }

  // inform the parent that this clone has finished
WorkerFinished:
  close(server);
  asm volatile("lock decl\t%0" : "+m"(workers));
  return 0;
}

void OnCtrlC(int sig) {
  closingtime = true;
  status = " shutting down...";
}

int main(int argc, char *argv[]) {
  int i, threads;
  uint32_t *hostips;
  // ShowCrashReports();
  sigaction(SIGINT, &(struct sigaction){.sa_handler = OnCtrlC}, 0);
  for (hostips = GetHostIps(), i = 0; hostips[i]; ++i) {
    kprintf("listening on http://%d.%d.%d.%d:%d\n",
            (hostips[i] & 0xff000000) >> 030, (hostips[i] & 0x00ff0000) >> 020,
            (hostips[i] & 0x0000ff00) >> 010, (hostips[i] & 0x000000ff) >> 000,
            PORT);
  }
  threads = argc > 1 ? atoi(argv[1]) : 0;
  if (!threads) threads = GetCpuCount();
  workers = threads;
  for (i = 0; i < threads; ++i) {
    char *tls = __initialize_tls(malloc(64));
    void *stack = mmap(0, GetStackSize(), PROT_READ | PROT_WRITE,
                       MAP_STACK | MAP_ANONYMOUS, -1, 0);
    CHECK_NE(-1, clone(Worker, stack, GetStackSize(),
                       CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES |
                           CLONE_SIGHAND | CLONE_SETTLS,
                       (void *)(intptr_t)i, 0, tls, 64, 0));
  }
  status = "";
  while (workers) {
    kprintf(
        "\r\e[K\e[32mgreenbean\e[0m workers=%d connections=%d messages=%d%s ",
        workers, connections, messages, status);
    usleep(HEARTBEAT * 1000);
  }
  kprintf("\r\e[K");
}
