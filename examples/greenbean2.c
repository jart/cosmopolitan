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
#include "libc/macros.internal.h"
#include "libc/mem/gc.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"
#include "libc/sysv/consts/tcp.h"
#include "libc/thread/thread.h"
#include "libc/thread/thread2.h"
#include "net/http/http.h"
#include "third_party/nsync/cv.h"
#include "third_party/nsync/mu.h"
#include "third_party/nsync/time.h"

/**
 * @fileoverview greenbean lightweight threaded web server no. 2
 *
 * This web server is the same as greenbean.c except it supports having
 * more than one thread on Windows. To do that we have to make the code
 * more complicated by not using SO_REUSEPORT. The approach we take, is
 * creating a single listener thread which adds accepted sockets into a
 * queue that worker threads consume. This way, if you like Windows you
 * can easily have a web server with 10,000+ connections.
 */

#define PORT      8080
#define KEEPALIVE 30000
#define LOGGING   1

#define STANDARD_RESPONSE_HEADERS \
  "Server: greenbean/1.o\r\n"     \
  "Referrer-Policy: origin\r\n"   \
  "Cache-Control: private; max-age=0\r\n"

int server;
int threads;
pthread_t listener;
atomic_int a_termsig;
atomic_int a_workers;
atomic_int a_messages;
atomic_int a_connections;
pthread_cond_t statuscond;
pthread_mutex_t statuslock;
const char *volatile status = "";

struct Clients {
  int pos;
  int count;
  pthread_mutex_t mu;
  pthread_cond_t non_full;
  pthread_cond_t non_empty;
  struct Client {
    int sock;
    uint32_t size;
    struct sockaddr_in addr;
  } data[100];
} g_clients;

ssize_t Write(int fd, const char *s) {
  return write(fd, s, strlen(s));
}

void SomethingHappened(void) {
  unassert(!pthread_cond_signal(&statuscond));
}

void SomethingImportantHappened(void) {
  unassert(!pthread_mutex_lock(&statuslock));
  unassert(!pthread_cond_signal(&statuscond));
  unassert(!pthread_mutex_unlock(&statuslock));
}

bool AddClient(struct Clients *q, const struct Client *v,
               struct timespec *deadline) {
  bool wake = false;
  bool added = false;
  pthread_mutex_lock(&q->mu);
  while (q->count == ARRAYLEN(q->data)) {
    if (pthread_cond_timedwait(&q->non_full, &q->mu, deadline)) {
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
  pthread_mutex_unlock(&q->mu);
  if (wake) pthread_cond_signal(&q->non_empty);
  return added;
}

int GetClient(struct Clients *q, struct Client *out) {
  int got = 0, len = 1;
  pthread_mutex_lock(&q->mu);
  while (!q->count) {
    errno_t err;
    unassert(!pthread_setcancelstate(PTHREAD_CANCEL_MASKED, 0));
    err = pthread_cond_wait(&q->non_empty, &q->mu);
    unassert(!pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0));
    if (err) {
      unassert(err == ECANCELED);
      break;
    }
  }
  while (got < len && q->count) {
    memcpy(out + got, q->data + q->pos, sizeof(*out));
    if (q->count == ARRAYLEN(q->data)) {
      pthread_cond_broadcast(&q->non_full);
    }
    ++got;
    q->pos++;
    q->count--;
    if (q->pos == ARRAYLEN(q->data)) q->pos = 0;
  }
  pthread_mutex_unlock(&q->mu);
  return got;
}

void *ListenWorker(void *arg) {
  int yes = 1;
  pthread_setname_np(pthread_self(), "Listener");

  // load balance incoming connections for port 8080 across all threads
  // hangup on any browser clients that lag for more than a few seconds
  struct timeval timeo = {KEEPALIVE / 1000, KEEPALIVE % 1000};
  struct sockaddr_in addr = {.sin_family = AF_INET, .sin_port = htons(PORT)};

  server = socket(AF_INET, SOCK_STREAM, 0);
  if (server == -1) {
    kprintf("\r\e[Ksocket() failed %m\n");
    SomethingHappened();
    return 0;
  }

  // we don't bother checking for errors here since OS support for the
  // advanced features tends to be a bit spotty and harmless to ignore
  setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, &timeo, sizeof(timeo));
  setsockopt(server, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo));
  setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
  setsockopt(server, SOL_TCP, TCP_FASTOPEN, &yes, sizeof(yes));
  setsockopt(server, SOL_TCP, TCP_QUICKACK, &yes, sizeof(yes));
  errno = 0;

  // open our ears to incoming connections; so_reuseport makes it
  // possible for our many threads to bind to the same interface!
  // otherwise we'd need to create a complex multi-threaded queue
  if (bind(server, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    kprintf("\r\e[Kbind() returned %m\n");
    SomethingHappened();
    goto CloseServer;
  }
  unassert(!listen(server, 1));

  while (!a_termsig) {
    struct Client client;

    // musl libc and cosmopolitan libc support a posix thread extension
    // that makes thread cancelation work much better your i/o routines
    // will just raise ECANCELED, so you can check for cancelation with
    // normal logic rather than needing to push and pop cleanup handler
    // functions onto the stack, or worse dealing with async interrupts
    unassert(!pthread_setcancelstate(PTHREAD_CANCEL_MASKED, 0));

    // wait for client connection
    client.size = sizeof(client.addr);
    client.sock = accept(server, (struct sockaddr *)&client.addr, &client.size);

    // turn cancel off, so we don't need to check write() for ecanceled
    unassert(!pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0));

    if (client.sock == -1) {
      // accept() errors are generally ephemeral or recoverable
      if (errno == EAGAIN) continue;     // SO_RCVTIMEO interval churns
      if (errno == ECANCELED) continue;  // pthread_cancel() was called
      kprintf("\r\e[Kaccept() returned %m\n");
      SomethingHappened();
      usleep(10000);
      errno = 0;
      continue;
    }

#if LOGGING
    // log the incoming http message
    unsigned clientip = ntohl(client.addr.sin_addr.s_addr);
    kprintf("\r\e[K%6P accepted connection from %hhu.%hhu.%hhu.%hhu:%hu\n",
            clientip >> 24, clientip >> 16, clientip >> 8, clientip,
            ntohs(client.addr.sin_port));
    SomethingHappened();
#endif

    ++a_connections;
    SomethingHappened();
    struct timespec deadline =
        timespec_add(timespec_real(), timespec_frommillis(100));
    if (!AddClient(&g_clients, &client, &deadline)) {
      Write(client.sock, "HTTP/1.1 503 Accept Queue Full\r\n"
                         "Content-Type: text/plain\r\n"
                         "Connection: close\r\n"
                         "\r\n"
                         "Accept Queue Full\n");
      close(client.sock);
    }
  }

CloseServer:
  SomethingHappened();
  close(server);
  return 0;
}

void *Worker(void *id) {
  pthread_setname_np(pthread_self(), "Worker");

  // connection loop
  while (!a_termsig) {
    struct Client client;
    int inmsglen, outmsglen;
    char inbuf[512], outbuf[512], *p, *q;

    // find a client to serve
    if (!GetClient(&g_clients, &client)) {
      continue;  // should be due to ecanceled
    }

    // message loop
    ssize_t got, sent;
    struct HttpMessage msg;
    do {
      // parse the incoming http message
      InitHttpMessage(&msg, kHttpRequest);
      // wait for http message (non-fragmented required)
      // we're not terribly concerned when errors happen here
      unassert(!pthread_setcancelstate(PTHREAD_CANCEL_MASKED, 0));
      if ((got = read(client.sock, inbuf, sizeof(inbuf))) <= 0) break;
      unassert(!pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0));
      // check that client message wasn't fragmented into more reads
      if ((inmsglen = ParseHttpMessage(&msg, inbuf, got)) <= 0) break;
      ++a_messages;
      SomethingHappened();

#if LOGGING
      // log the incoming http message
      unsigned clientip = ntohl(client.addr.sin_addr.s_addr);
      kprintf("\r\e[K%6P get some %hhu.%hhu.%hhu.%hhu:%hu %#.*s\n",
              clientip >> 24, clientip >> 16, clientip >> 8, clientip,
              ntohs(client.addr.sin_port), msg.uri.b - msg.uri.a,
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
        p = stpcpy(p, "\r\n\r\n");
        p = stpcpy(p, q);
        outmsglen = p - outbuf;
        sent = write(client.sock, outbuf, outmsglen);

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
        p = stpcpy(p, "\r\n\r\n");
        p = stpcpy(p, q);
        outmsglen = p - outbuf;
        sent = write(client.sock, outbuf, p - outbuf);
      }

      // if the client isn't pipelining and write() wrote the full
      // amount, then since we sent the content length and checked
      // that the client didn't attach a payload, we are so synced
      // thus we can safely process more messages
    } while (got == inmsglen &&    //
             sent == outmsglen &&  //
             !msg.headers[kHttpContentLength].a &&
             !msg.headers[kHttpTransferEncoding].a &&
             (msg.method == kHttpGet || msg.method == kHttpHead));
    DestroyHttpMessage(&msg);
    kprintf("\r\e[K%6P client disconnected\n");
    SomethingHappened();
    close(client.sock);
    --a_connections;
    SomethingHappened();
  }

  --a_workers;
  SomethingImportantHappened();
  return 0;
}

void PrintStatus(void) {
  kprintf("\r\e[K\e[32mgreenbean\e[0m "
          "workers=%d "
          "connections=%d "
          "messages=%d%s ",
          a_workers, a_connections, a_messages, status);
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
  // you can press ctrl+\ to trigger backtraces
  // ShowCrashReports();

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

  // secure the server
  //
  // pledge() and unveil() let us whitelist which system calls and files
  // the server will be allowed to use. this way if it gets hacked, they
  // won't be able to do much damage, like compromising the whole server
  //
  // pledge violations on openbsd are logged nicely to the system logger
  // but on linux we need to use a cosmopolitan extension to get details
  // although doing that slightly weakens the security pledge() provides
  //
  // if your operating system doesn't support these security features or
  // is too old, then pledge() and unveil() don't consider this an error
  // so it works. if security is critical there's a special call to test
  // which is npassert(!pledge(0, 0)), and npassert(unveil("", 0) != -1)
  __pledge_mode = PLEDGE_PENALTY_RETURN_EPERM;  // c. greenbean --strace
  unveil("/dev/null", "rw");
  unveil(0, 0);
  pledge("stdio inet", 0);

  // initialize our synchronization data structures, which were written
  // by mike burrows in a library called *nsync we've tailored for libc
  unassert(!pthread_cond_init(&statuscond, 0));
  unassert(!pthread_mutex_init(&statuslock, 0));
  unassert(!pthread_mutex_init(&g_clients.mu, 0));
  unassert(!pthread_cond_init(&g_clients.non_full, 0));
  unassert(!pthread_cond_init(&g_clients.non_empty, 0));

  // spawn over 9000 worker threads
  //
  // you don't need weird i/o models, or event driven yoyo pattern code
  // to build a massively scalable server. the secret is to use threads
  // with tiny stacks. then you can write plain simple imperative code!
  //
  // we block signals in our worker threads so we won't need messy code
  // to spin on eintr. operating systems also deliver signals to random
  // threads, and we'd have ctrl-c, etc. be handled by the main thread.
  //
  // alternatively you can just use signal() instead of sigaction(); it
  // uses SA_RESTART because all the syscalls the worker currently uses
  // are documented as @restartable which means no EINTR toil is needed
  sigset_t block;
  sigemptyset(&block);
  sigaddset(&block, SIGINT);
  sigaddset(&block, SIGHUP);
  sigaddset(&block, SIGQUIT);
  pthread_attr_t attr;
  int pagesz = getauxval(AT_PAGESZ);
  unassert(!pthread_attr_init(&attr));
  unassert(!pthread_attr_setstacksize(&attr, 65536));
  unassert(!pthread_attr_setguardsize(&attr, pagesz));
  unassert(!pthread_attr_setsigmask_np(&attr, &block));
  unassert(!pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0));
  unassert(!pthread_create(&listener, &attr, ListenWorker, 0));
  pthread_t *th = gc(calloc(threads, sizeof(pthread_t)));
  for (i = 0; i < threads; ++i) {
    int rc;
    ++a_workers;
    if ((rc = pthread_create(th + i, &attr, Worker, (void *)(intptr_t)i))) {
      --a_workers;
      kprintf("\r\e[Kpthread_create failed: %s\n", strerror(rc));
      if (rc == EAGAIN) {
        kprintf("sudo prlimit --pid=$$ --nofile=%d\n", threads * 3);
        kprintf("sudo prlimit --pid=$$ --nproc=%d\n", threads * 2);
      }
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
  close(server);
  pthread_cancel(listener);
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
  unassert(!pthread_join(listener, 0));
  for (i = 0; i < threads; ++i) {
    unassert(!pthread_join(th[i], 0));
  }

  // clean up terminal line
  kprintf("\r\e[Kthank you for choosing \e[32mgreenbean\e[0m\n");

  // clean up more resources
  unassert(!pthread_cond_destroy(&statuscond));
  unassert(!pthread_mutex_destroy(&statuslock));
  unassert(!pthread_mutex_destroy(&g_clients.mu));
  unassert(!pthread_cond_destroy(&g_clients.non_full));
  unassert(!pthread_cond_destroy(&g_clients.non_empty));

  // quality assurance
  if (IsModeDbg()) {
    CheckForMemoryLeaks();
  }

  // propagate termination signal
  signal(a_termsig, SIG_DFL);
  raise(a_termsig);
}
