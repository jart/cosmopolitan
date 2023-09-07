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
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/pledge.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/kprintf.h"
#include "libc/log/log.h"
#include "libc/mem/gc.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"
#include "libc/sysv/consts/tcp.h"
#include "libc/thread/thread.h"
#include "libc/thread/thread2.h"
#include "net/http/http.h"

/**
 * @fileoverview greenbean lightweight threaded web server
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
 *     $ wrk -c 300 -t 32 --latency http://127.0.0.1:8080/
 *     Running 10s test @ http://127.0.0.1:8080/
 *       32 threads and 300 connections
 *         Thread Stats   Avg      Stdev     Max   +/- Stdev
 *         Latency   661.06us    5.11ms  96.22ms   98.85%
 *         Req/Sec    42.38k     8.90k   90.47k    84.65%
 *       Latency Distribution
 *          50%  184.00us
 *          75%  201.00us
 *          90%  224.00us
 *          99%   11.99ms
 *       10221978 requests in 7.60s, 3.02GB read
 *     Requests/sec: 1345015.69
 *     Transfer/sec:    406.62MB
 *
 */

#define PORT      8080
#define KEEPALIVE 30000
#define LOGGING   1

#define STANDARD_RESPONSE_HEADERS \
  "Server: greenbean/1.o\r\n"     \
  "Referrer-Policy: origin\r\n"   \
  "Cache-Control: private; max-age=0\r\n"

int threads;
int alwaysclose;
atomic_int a_termsig;
atomic_int a_workers;
atomic_int a_messages;
atomic_int a_listening;
atomic_int a_connections;
pthread_cond_t statuscond;
pthread_mutex_t statuslock;
const char *volatile status = "";

void SomethingHappened(void) {
  unassert(!pthread_cond_signal(&statuscond));
}

void SomethingImportantHappened(void) {
  unassert(!pthread_mutex_lock(&statuslock));
  unassert(!pthread_cond_signal(&statuscond));
  unassert(!pthread_mutex_unlock(&statuslock));
}

void *Worker(void *id) {
  int server, yes = 1;

  // load balance incoming connections for port 8080 across all threads
  // hangup on any browser clients that lag for more than a few seconds
  struct timeval timeo = {KEEPALIVE / 1000, KEEPALIVE % 1000};
  struct sockaddr_in addr = {.sin_family = AF_INET, .sin_port = htons(PORT)};

  server = socket(AF_INET, SOCK_STREAM, 0);
  if (server == -1) {
    kprintf("socket() failed %m\n"
            "  try running: sudo prlimit --pid=$$ --nofile=%d\n",
            threads * 2);
    goto WorkerFinished;
  }

  // we don't bother checking for errors here since OS support for the
  // advanced features tends to be a bit spotty and harmless to ignore
  setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, &timeo, sizeof(timeo));
  setsockopt(server, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo));
  setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
  setsockopt(server, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes));
  setsockopt(server, SOL_TCP, TCP_FASTOPEN, &yes, sizeof(yes));
  setsockopt(server, SOL_TCP, TCP_QUICKACK, &yes, sizeof(yes));
  errno = 0;

  // open our ears to incoming connections; so_reuseport makes it
  // possible for our many threads to bind to the same interface!
  // otherwise we'd need to create a complex multi-threaded queue
  if (bind(server, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    kprintf("%s() failed %m\n", "socket");
    goto CloseWorker;
  }
  unassert(!listen(server, 1));

  // connection loop
  ++a_listening;
  SomethingImportantHappened();
  while (!a_termsig) {
    uint32_t clientaddrsize;
    struct sockaddr_in clientaddr;
    int client, inmsglen, outmsglen;
    char inbuf[1500], outbuf[512], *p, *q;

    // musl libc and cosmopolitan libc support a posix thread extension
    // that makes thread cancellation work much better your io routines
    // will just raise ECANCELED so you can check for cancellation with
    // normal logic rather than needing to push and pop cleanup handler
    // functions onto the stack, or worse dealing with async interrupts
    unassert(!pthread_setcancelstate(PTHREAD_CANCEL_MASKED, 0));

    // wait for client connection
    // we don't bother with poll() because this is actually very speedy
    clientaddrsize = sizeof(clientaddr);
    client = accept(server, (struct sockaddr *)&clientaddr, &clientaddrsize);

    // turns cancellation off so we don't interrupt active http clients
    unassert(!pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0));

    // accept() can raise a very diverse number of errors but none of
    // them are really true showstoppers that would necessitate us to
    // panic and abort the entire server, so we can just ignore these
    if (client == -1) {
      // we used SO_RCVTIMEO and SO_SNDTIMEO because those settings are
      // inherited by the accepted sockets, but using them also has the
      // side-effect that the listening socket fails with EAGAIN errors
      // which are harmless, and so are most other errors accept raises
      // e.g. ECANCELED, which lets us check closingtime without delay!
      continue;
    }

    ++a_connections;
    SomethingHappened();

    // message loop
    ssize_t got, sent;
    struct HttpMessage msg;
    do {
      // parse the incoming http message
      InitHttpMessage(&msg, kHttpRequest);
      // wait for http message (non-fragmented required)
      // we're not terrible concerned when errors happen here
      unassert(!pthread_setcancelstate(PTHREAD_CANCEL_MASKED, 0));
      if ((got = read(client, inbuf, sizeof(inbuf))) <= 0) break;
      unassert(!pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0));
      // check that client message wasn't fragmented into more reads
      if ((inmsglen = ParseHttpMessage(&msg, inbuf, got)) <= 0) break;
      ++a_messages;
      SomethingHappened();

#if LOGGING
      // log the incoming http message
      unsigned clientip = ntohl(clientaddr.sin_addr.s_addr);
      kprintf("\r\e[K%6P get some %hhu.%hhu.%hhu.%hhu:%hu %#.*s\n",
              clientip >> 24, clientip >> 16, clientip >> 8, clientip,
              ntohs(clientaddr.sin_port), msg.uri.b - msg.uri.a,
              inbuf + msg.uri.a);
      SomethingHappened();
#endif

      // display hello world html page for http://127.0.0.1:8080/
      struct tm tm;
      int64_t unixts;
      struct timespec ts;
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
        clock_gettime(0, &ts), unixts = ts.tv_sec;
        p = FormatHttpDateTime(p, gmtime_r(&unixts, &tm));
        p = stpcpy(p, "\r\nContent-Length: ");
        p = FormatInt32(p, strlen(q));
        if (alwaysclose) {
          p = stpcpy(p, "\r\nConnection: close");
        }
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
        clock_gettime(0, &ts), unixts = ts.tv_sec;
        p = FormatHttpDateTime(p, gmtime_r(&unixts, &tm));
        p = stpcpy(p, "\r\nContent-Length: ");
        p = FormatInt32(p, strlen(q));
        if (alwaysclose) {
          p = stpcpy(p, "\r\nConnection: close");
        }
        p = stpcpy(p, "\r\n\r\n");
        p = stpcpy(p, q);
        outmsglen = p - outbuf;
        sent = write(client, outbuf, p - outbuf);
      }

      // if the client isn't pipelining and write() wrote the full
      // amount, then since we sent the content length and checked
      // that the client didn't attach a payload, we are so synced
      // thus we can safely process more messages
    } while (!alwaysclose &&       //
             got == inmsglen &&    //
             sent == outmsglen &&  //
             !msg.headers[kHttpContentLength].a &&
             !msg.headers[kHttpTransferEncoding].a &&
             (msg.method == kHttpGet || msg.method == kHttpHead));
    DestroyHttpMessage(&msg);
    close(client);
    --a_connections;
    SomethingHappened();
  }
  --a_listening;

  // inform the parent that this clone has finished
CloseWorker:
  close(server);
WorkerFinished:
  --a_workers;

  SomethingImportantHappened();
  return 0;
}

void PrintStatus(void) {
  kprintf("\r\e[K\e[32mgreenbean\e[0m "
          "workers=%d "
          "listening=%d "
          "connections=%d "
          "messages=%d%s ",
          a_workers, a_listening, a_connections, a_messages, status);
}

void OnTerm(int sig) {
  a_termsig = sig;
  status = " shutting down...";
  SomethingHappened();
}

int main(int argc, char *argv[]) {
  int i;

  // print cpu registers and backtrace on crash
  // note that pledge'll makes backtraces worse
  // you can press ctrl+\ to trigger your crash
  ShowCrashReports();

  // listen for ctrl-c, terminal close, and kill
  struct sigaction sa = {.sa_handler = OnTerm};
  unassert(!sigaction(SIGINT, &sa, 0));
  unassert(!sigaction(SIGHUP, &sa, 0));
  unassert(!sigaction(SIGTERM, &sa, 0));

  // print all the ips that 0.0.0.0 would bind
  // Cosmo's GetHostIps() API is much easier than ioctl(SIOCGIFCONF)
  uint32_t *hostips;
  for (hostips = gc(GetHostIps()), i = 0; hostips[i]; ++i) {
    kprintf("listening on http://%hhu.%hhu.%hhu.%hhu:%hu\n", hostips[i] >> 24,
            hostips[i] >> 16, hostips[i] >> 8, hostips[i], PORT);
  }

  // you can pass the number of threads you want as the first command arg
  threads = argc > 1 ? atoi(argv[1]) : __get_cpu_count();
  if (!(1 <= threads && threads <= 100000)) {
    kprintf("\r\e[Kerror: invalid number of threads: %d\n", threads);
    exit(1);
  }

  // caveat emptor microsofties
  if (IsWindows()) {
    kprintf("sorry but windows isn't supported by the greenbean demo yet\n"
            "because it doesn't support SO_REUSEPORT which is a nice for\n"
            "gaining great performance on UNIX systems, with simple code\n"
            "however windows will work fine if we limit it to one thread\n");
    threads = 1;      // we're going to make just one web server thread
    alwaysclose = 1;  // don't let client idle, since it'd block others
  }

  // secure the server
  //
  // pledge() and unveil() let us whitelist which system calls and files
  // the server will be allowed to use. this way if it gets hacked, they
  // won't be able to do much damage, like compromising the whole server
  //
  // we use an internal api to force threads to enable beforehand, since
  // cosmopolitan code morphs the binary to support tls across platforms
  // and doing that requires extra permissions we don't need for serving
  //
  // pledge violations on openbsd are logged nicely to the system logger
  // but on linux we need to use a cosmopolitan extension to get details
  // although doing that slightly weakens the security pledge() provides
  //
  // if your operating system doesn't support these security features or
  // is too old, then pledge() and unveil() don't consider this an error
  // so it works. if security is critical there's a special call to test
  // which is npassert(!pledge(0, 0)), and npassert(unveil("", 0) != -1)
  __enable_threads();
  __pledge_mode = PLEDGE_PENALTY_KILL_THREAD | PLEDGE_STDERR_LOGGING;
  unveil("/dev/null", "rw");
  unveil(0, 0);
  pledge("stdio inet", 0);

  // spawn over 9,000 worker threads
  //
  // you don't need weird i/o models, or event driven yoyo pattern code
  // to build a massively scalable server. the secret is to use threads
  // with tiny stacks. then you can write plain simple imperative code!
  //
  // we like pthread attributes since they generally make thread spawns
  // faster especially in cases where you need to make detached threads
  //
  // we block signals in our worker threads so we won't need messy code
  // to spin on eintr. operating systems also deliver signals to random
  // threads, and we'd have ctrl-c, etc. be handled by the main thread.
  //
  // alternatively you can just use signal() instead of sigaction(); it
  // uses SA_RESTART because all the syscalls the worker currently uses
  // are documented as @restartable which means no EINTR toil is needed
  unassert(!pthread_cond_init(&statuscond, 0));
  unassert(!pthread_mutex_init(&statuslock, 0));
  sigset_t block;
  sigfillset(&block);
  sigdelset(&block, SIGSEGV);  // invalid memory access
  sigdelset(&block, SIGBUS);   // another kind of bad memory access
  sigdelset(&block, SIGFPE);   // divide by zero, etc.
  sigdelset(&block, SIGSYS);   // pledge violations
  sigdelset(&block, SIGILL);   // bad cpu opcode
  pthread_attr_t attr;
  unassert(!pthread_attr_init(&attr));
  unassert(!pthread_attr_setsigmask_np(&attr, &block));
  pthread_t *th = gc(calloc(threads, sizeof(pthread_t)));
  for (i = 0; i < threads; ++i) {
    int rc;
    ++a_workers;
    if ((rc = pthread_create(th + i, &attr, Worker, (void *)(intptr_t)i))) {
      --a_workers;
      // rc will most likely be EAGAIN (we hit the process/thread limit)
      kprintf("\r\e[Kerror: pthread_create(%d) failed: %s\n"
              "       try increasing RLIMIT_NPROC\n",
              i, strerror(rc));
      if (!i) exit(1);
      threads = i;
      break;
    }
    if (!(i % 50)) {
      PrintStatus();
    }
  }
  unassert(!pthread_attr_destroy(&attr));

  // wait for workers to terminate
  unassert(!pthread_mutex_lock(&statuslock));
  while (!a_termsig) {
    PrintStatus();
    unassert(!pthread_cond_wait(&statuscond, &statuslock));
    usleep(10 * 1000);
  }
  unassert(!pthread_mutex_unlock(&statuslock));

  // cancel all the worker threads so they shut down asap
  // and it'll wait on active clients to gracefully close
  // you've never seen a production server close so fast!
  for (i = 0; i < threads; ++i) {
    pthread_cancel(th[i]);
  }

  // print status in terminal as the shutdown progresses
  unassert(!pthread_mutex_lock(&statuslock));
  while (a_workers) {
    unassert(!pthread_cond_wait(&statuscond, &statuslock));
    PrintStatus();
  }
  unassert(!pthread_mutex_unlock(&statuslock));

  // wait for final termination and free thread memory
  for (i = 0; i < threads; ++i) {
    unassert(!pthread_join(th[i], 0));
  }

  // clean up terminal line
  kprintf("\r\e[Kthank you for choosing \e[32mgreenbean\e[0m\n");

  // clean up more resources
  unassert(!pthread_mutex_destroy(&statuslock));
  unassert(!pthread_cond_destroy(&statuscond));

  // quality assurance
  if (IsModeDbg()) {
    CheckForMemoryLeaks();
  }

  // propagate termination signal
  signal(a_termsig, SIG_DFL);
  raise(a_termsig);
}
