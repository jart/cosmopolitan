/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/libgen.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/kprintf.h"
#include "libc/log/appendresourcereport.internal.h"
#include "libc/log/check.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.h"
#include "libc/mem/gc.internal.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/stdio/append.h"
#include "libc/stdio/posix_spawn.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/inaddr.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/itimer.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/posix.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"
#include "libc/sysv/consts/w.h"
#include "libc/temp.h"
#include "libc/thread/thread.h"
#include "libc/thread/thread2.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "libc/x/xsigaction.h"
#include "net/http/escape.h"
#include "net/https/https.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/mbedtls/ssl.h"
#include "third_party/zlib/zlib.h"
#include "tool/build/lib/eztls.h"
#include "tool/build/lib/psk.h"
#include "tool/build/runit.h"

/**
 * @fileoverview Remote test runner daemon.
 * Delivers 10x latency improvement over SSH (100x if Debian defaults)
 *
 * Here's how it handles connections:
 *
 * 1. Receives atomically-written request header, comprised of:
 *
 *   - 4 byte nbo magic = 0xFEEDABEEu
 *   - 1 byte command = kRunitExecute
 *   - 4 byte nbo name length in bytes, e.g. "test1"
 *   - 4 byte nbo executable file length in bytes
 *   - <name bytes> (no NUL terminator)
 *   - <file bytes> (it's binary data)
 *
 * 2. Runs program, after verifying it came from the IP that spawned
 *    this program via SSH. Be sure to only run this over a trusted
 *    physically-wired network. To use this software on untrustworthy
 *    networks, wrap it with stunnel and use your own CA.
 *
 * 3. Sends stdout/stderr fragments, potentially multiple times:
 *
 *   - 4 byte nbo magic = 0xFEEDABEEu
 *   - 1 byte command = kRunitStdout/Stderr
 *   - 4 byte nbo byte length
 *   - <chunk bytes>
 *
 * 4. Sends process exit code:
 *
 *   - 4 byte nbo magic = 0xFEEDABEEu
 *   - 1 byte command = kRunitExit
 *   - 1 byte exit status
 */

#define DEATH_CLOCK_SECONDS 300

#define kLogFile     "o/runitd.log"
#define kLogMaxBytes (2 * 1000 * 1000)

#define LOG_LEVEL_WARN 0
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_VERB 3
#define LOG_LEVEL_DEBU 3

#define DEBUF(FMT, ...) LOGF(DEBU, FMT, ##__VA_ARGS__)
#define VERBF(FMT, ...) LOGF(VERB, FMT, ##__VA_ARGS__)
#define INFOF(FMT, ...) LOGF(INFO, FMT, ##__VA_ARGS__)
#define WARNF(FMT, ...) LOGF(WARN, FMT, ##__VA_ARGS__)

#define LOGF(LVL, FMT, ...)                                               \
  do {                                                                    \
    if (g_log_level >= LOG_LEVEL_##LVL) {                                 \
      kprintf("%r" #LVL " %6P %'18T %s:%d " FMT "\n", __FILE__, __LINE__, \
              ##__VA_ARGS__);                                             \
    }                                                                     \
  } while (0)

struct Client {
  int fd;
  int pid;
  int pipe[2];
  pthread_t th;
  uint32_t addrsize;
  struct sockaddr_in addr;
  bool once;
  int zstatus;
  z_stream zs;
  struct {
    size_t off;
    size_t len;
    size_t cap;
    char *data;
  } rbuf;
  char *output;
  char exepath[128];
  char buf[32768];
};

char *g_psk;
int g_log_level;
bool use_ftrace;
bool use_strace;
char g_hostname[256];
int g_bogusfd, g_servfd;
atomic_bool g_interrupted;
struct sockaddr_in g_servaddr;
bool g_daemonize, g_sendready;

void OnInterrupt(int sig) {
  g_interrupted = true;
}

void Close(int *fd) {
  if (*fd > 0) {
    close(*fd);
    *fd = -1;  // poll ignores -1
  }
}

wontreturn void ShowUsage(FILE *f, int rc) {
  fprintf(f, "%s: %s %s\n", "Usage", program_invocation_name,
          "[-d] [-r] [-l LISTENIP] [-p PORT] [-t TIMEOUTMS]");
  exit(rc);
}

char *DescribeAddress(struct sockaddr_in *addr) {
  static _Thread_local char res[64];
  char ip4buf[64];
  sprintf(res, "%s:%hu",
          inet_ntop(addr->sin_family, &addr->sin_addr.s_addr, ip4buf,
                    sizeof(ip4buf)),
          ntohs(addr->sin_port));
  return res;
}

void GetOpts(int argc, char *argv[]) {
  int opt;
  g_servaddr.sin_family = AF_INET;
  g_servaddr.sin_port = htons(RUNITD_PORT);
  g_servaddr.sin_addr.s_addr = INADDR_ANY;
  while ((opt = getopt(argc, argv, "fqhvsdrl:p:t:w:")) != -1) {
    switch (opt) {
      case 'f':
        use_ftrace = true;
        break;
      case 's':
        use_strace = true;
        break;
      case 'q':
        --g_log_level;
        break;
      case 'v':
        ++g_log_level;
        break;
      case 'd':
        g_daemonize = true;
        break;
      case 'r':
        g_sendready = true;
        break;
      case 't':
        break;
      case 'p':
        g_servaddr.sin_port = htons(parseport(optarg));
        break;
      case 'l':
        inet_pton(AF_INET, optarg, &g_servaddr.sin_addr);
        break;
      case 'h':
        ShowUsage(stdout, EXIT_SUCCESS);
      default:
        ShowUsage(stderr, EX_USAGE);
    }
  }
}

void StartTcpServer(void) {
  int yes = true;
  uint32_t asize;
  g_servfd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, IPPROTO_TCP);
  if (g_servfd == -1) {
    fprintf(stderr, program_invocation_short_name,
            ": socket failed: ", strerror(errno), "\n", NULL);
    exit(1);
  }
  struct timeval timeo = {DEATH_CLOCK_SECONDS / 10};
  setsockopt(g_servfd, SOL_SOCKET, SO_RCVTIMEO, &timeo, sizeof(timeo));
  setsockopt(g_servfd, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo));
  setsockopt(g_servfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
  if (bind(g_servfd, (struct sockaddr *)&g_servaddr, sizeof(g_servaddr)) ==
      -1) {
    fprintf(stderr, program_invocation_short_name,
            ": bind failed: ", strerror(errno), "\n", NULL);
    exit(1);
  }
  unassert(!listen(g_servfd, 10));
  asize = sizeof(g_servaddr);
  unassert(!getsockname(g_servfd, (struct sockaddr *)&g_servaddr, &asize));
  INFOF("listening on tcp:%s", DescribeAddress(&g_servaddr));
  if (g_sendready) {
    printf("ready %hu\n", ntohs(g_servaddr.sin_port));
    fflush(stdout);
    fclose(stdout);
    dup2(g_bogusfd, fileno(stdout));
  }
}

void SendExitMessage(int rc) {
  EzSanity();
  int res;
  unsigned char msg[4 + 1 + 1];
  DEBUF("SendExitMessage");
  msg[0 + 0] = (RUNITD_MAGIC & 0xff000000) >> 030;
  msg[0 + 1] = (RUNITD_MAGIC & 0x00ff0000) >> 020;
  msg[0 + 2] = (RUNITD_MAGIC & 0x0000ff00) >> 010;
  msg[0 + 3] = (RUNITD_MAGIC & 0x000000ff) >> 000;
  msg[4] = kRunitExit;
  msg[5] = rc;
  DEBUF("mbedtls_ssl_write");
  if (sizeof(msg) != (res = mbedtls_ssl_write(&ezssl, msg, sizeof(msg)))) {
    EzTlsDie("SendExitMessage mbedtls_ssl_write failed", res);
  }
  if ((res = EzTlsFlush(&ezbio, 0, 0))) {
    EzTlsDie("SendExitMessage EzTlsFlush failed", res);
  }
}

void SendOutputFragmentMessage(enum RunitCommand kind, char *buf, size_t size) {
  EzSanity();
  ssize_t rc;
  unsigned char msg[4 + 1 + 4];
  msg[0 + 0] = (RUNITD_MAGIC & 0xff000000) >> 030;
  msg[0 + 1] = (RUNITD_MAGIC & 0x00ff0000) >> 020;
  msg[0 + 2] = (RUNITD_MAGIC & 0x0000ff00) >> 010;
  msg[0 + 3] = (RUNITD_MAGIC & 0x000000ff) >> 000;
  msg[4 + 0] = kind;
  msg[5 + 0] = (size & 0xff000000) >> 030;
  msg[5 + 1] = (size & 0x00ff0000) >> 020;
  msg[5 + 2] = (size & 0x0000ff00) >> 010;
  msg[5 + 3] = (size & 0x000000ff) >> 000;
  DEBUF("mbedtls_ssl_write");
  if (sizeof(msg) != (rc = mbedtls_ssl_write(&ezssl, msg, sizeof(msg)))) {
    EzTlsDie("SendOutputFragmentMessage mbedtls_ssl_write failed", rc);
  }
  while (size) {
    if ((rc = mbedtls_ssl_write(&ezssl, buf, size)) <= 0) {
      EzTlsDie("SendOutputFragmentMessage mbedtls_ssl_write #2 failed", rc);
    }
    size -= rc;
    buf += rc;
  }
  if ((rc = EzTlsFlush(&ezbio, 0, 0))) {
    EzTlsDie("SendOutputFragmentMessage EzTlsFlush failed", rc);
  }
}

void Recv(struct Client *client, void *output, size_t outputsize) {
  EzSanity();
  ssize_t chunk, received, totalgot;
  if (!client->once) {
    unassert(Z_OK == inflateInit(&client->zs));
    client->once = true;
  }
  totalgot = 0;
  for (;;) {
    if (client->rbuf.len >= outputsize) {
      memcpy(output, client->rbuf.data + client->rbuf.off, outputsize);
      client->rbuf.len -= outputsize;
      client->rbuf.off += outputsize;
      // trim dymanic buffer once it empties
      if (!client->rbuf.len) {
        client->rbuf.off = 0;
        client->rbuf.cap = 4096;
        client->rbuf.data = realloc(client->rbuf.data, client->rbuf.cap);
      }
      return;
    }
    if (client->zstatus == Z_STREAM_END) {
      WARNF("recv zlib unexpected eof");
      pthread_exit(0);
    }
    // get another fixed-size data packet from network
    // pass along error conditions to caller
    // pass along eof condition to zlib
    received = mbedtls_ssl_read(&ezssl, client->buf, sizeof(client->buf));
    if (!received) {
      EzTlsDie("got unexpected eof", received);
    }
    if (received < 0) {
      EzTlsDie("read failed", received);
    }
    totalgot += received;
    // decompress packet completely
    // into a dynamical size buffer
    client->zs.avail_in = received;
    client->zs.next_in = (unsigned char *)client->buf;
    unassert(Z_OK == client->zstatus);
    do {
      // make sure we have a reasonable capacity for zlib output
      if (client->rbuf.cap - (client->rbuf.off + client->rbuf.len) <
          sizeof(client->buf)) {
        client->rbuf.cap += sizeof(client->buf);
        client->rbuf.data = realloc(client->rbuf.data, client->rbuf.cap);
      }
      // inflate packet, which naturally can be much larger
      // permit zlib no delay flushes that come from sender
      client->zs.next_out = (unsigned char *)client->rbuf.data +
                            (client->rbuf.off + client->rbuf.len);
      client->zs.avail_out = chunk =
          client->rbuf.cap - (client->rbuf.off + client->rbuf.len);
      client->zstatus = inflate(&client->zs, Z_SYNC_FLUSH);
      unassert(Z_STREAM_ERROR != client->zstatus);
      switch (client->zstatus) {
        case Z_NEED_DICT:
          WARNF("tls recv Z_NEED_DICT %ld total %ld", received, totalgot);
          pthread_exit(0);
        case Z_DATA_ERROR:
          WARNF("tls recv Z_DATA_ERROR %ld total %ld", received, totalgot);
          pthread_exit(0);
        case Z_MEM_ERROR:
          WARNF("tls recv Z_MEM_ERROR %ld total %ld", received, totalgot);
          pthread_exit(0);
        case Z_BUF_ERROR:
          client->zstatus = Z_OK;  // harmless? nothing for inflate to do
          break;                   // it probably just our wraparound eof
        default:
          client->rbuf.len += chunk - client->zs.avail_out;
          break;
      }
    } while (!client->zs.avail_out);
  }
}

void SendProgramOutut(struct Client *client) {
  if (client->output) {
    SendOutputFragmentMessage(kRunitStderr, client->output,
                              appendz(client->output).i);
  }
}

void PrintProgramOutput(struct Client *client) {
  if (client->output) {
    char *p = client->output;
    size_t z = appendz(p).i;
    if ((p = IndentLines(p, z, &z, 2))) {
      fwrite(p, 1, z, stderr);
      free(p);
    }
  }
}

void FreeClient(struct Client *client) {
  DEBUF("FreeClient");
  Close(&client->pipe[1]);
  Close(&client->pipe[0]);
  if (client->pid) {
    kill(client->pid, SIGHUP);
    waitpid(client->pid, 0, 0);
  }
  Close(&client->fd);
  if (*client->exepath) {
    unlink(client->exepath);
  }
  if (client->once) {
    inflateEnd(&client->zs);
  }
  EzDestroy();
  free(client->rbuf.data);
  free(client->output);
  free(client);
  VERBF("---------------");
}

void *ClientWorker(void *arg) {
  uint32_t crc;
  sigset_t sigmask;
  int events, wstatus;
  struct Client *client = arg;
  uint32_t namesize, filesize;
  char *addrstr, *exename, *exe;
  unsigned char msg[4 + 1 + 4 + 4 + 4];

  SetupPresharedKeySsl(MBEDTLS_SSL_IS_SERVER, g_psk);
  defer(FreeClient, client);

  // read request to run program
  EzFd(client->fd);
  DEBUF("EzHandshake");
  EzHandshake();
  addrstr = DescribeAddress(&client->addr);
  DEBUF("%s %s %s", DescribeAddress(&g_servaddr), "accepted", addrstr);

  // get the executable
  Recv(client, msg, sizeof(msg));
  if (READ32BE(msg) != RUNITD_MAGIC) {
    WARNF("%s magic mismatch!", addrstr);
    pthread_exit(0);
  }
  if (msg[4] != kRunitExecute) {
    WARNF("%s unknown command!", addrstr);
    pthread_exit(0);
  }
  namesize = READ32BE(msg + 5);
  filesize = READ32BE(msg + 9);
  crc = READ32BE(msg + 13);
  exename = gc(calloc(1, namesize + 1));
  Recv(client, exename, namesize);
  INFOF("%s sent %#s (%'u bytes @ %#s)", addrstr, exename, filesize,
        client->exepath);
  exe = gc(malloc(filesize));
  Recv(client, exe, filesize);
  if (crc32_z(0, exe, filesize) != crc) {
    WARNF("%s crc mismatch! %#s", addrstr, exename);
    pthread_exit(0);
  }

  // create the executable file
  // if another thread vforks while we're writing it then a race
  // condition can happen, where etxtbsy is raised by our execve
  // we're using o_cloexec so it's guaranteed to fix itself fast
  // thus we use an optimistic approach to avoid expensive locks
  sprintf(client->exepath, "o/%s.XXXXXX.com", basename(exename));
  int exefd = openatemp(AT_FDCWD, client->exepath, 4, O_CLOEXEC, 0700);
  ftruncate(exefd, filesize);
  if (write(exefd, exe, filesize) != filesize) {
    WARNF("%s failed to write %#s", addrstr, exename);
    close(exefd);
    pthread_exit(0);
  }
  if (close(exefd)) {
    WARNF("%s failed to close %#s", addrstr, exename);
    pthread_exit(0);
  }

  // do the args
  int i = 0;
  char *args[8] = {0};
  if (!IsXnuSilicon()) {
    exe = client->exepath;
  } else {
    exe = "ape-m1.com";
    args[i++] = (char *)exe;
    args[i++] = "-";
    args[i++] = client->exepath;
  }
  args[i++] = client->exepath;
  if (use_strace) args[i++] = "--strace";
  if (use_ftrace) args[i++] = "--ftrace";

  // run program, tee'ing stderr to both log and client
  DEBUF("spawning %s", client->exepath);
  sigemptyset(&sigmask);
  sigaddset(&sigmask, SIGINT);
  sigaddset(&sigmask, SIGCHLD);
  sigprocmask(SIG_BLOCK, &sigmask, 0);

  // spawn the program
  int etxtbsy_tries = 0;
RetryOnEtxtbsyRaceCondition:
  Close(&client->pipe[1]);
  Close(&client->pipe[0]);
  if (etxtbsy_tries++) {
    if (etxtbsy_tries == 24) {  // ~30 seconds
      WARNF("%s failed to spawn on %s due because either (1) the ETXTBSY race "
            "condition kept happening or (2) the program in question actually "
            "is crashing with SIGVTALRM, without printing anything to out/err!",
            exename, g_hostname);
      pthread_exit(0);
    }
    if (usleep(1u << etxtbsy_tries)) {
      INFOF("interrupted exponential spawn backoff");
      pthread_exit(0);
    }
  }
  errno_t err;
  posix_spawnattr_t spawnattr;
  posix_spawn_file_actions_t spawnfila;
  sigemptyset(&sigmask);
  pipe2(client->pipe, O_CLOEXEC);
  posix_spawnattr_init(&spawnattr);
  posix_spawnattr_setflags(&spawnattr, POSIX_SPAWN_SETPGROUP);
  posix_spawnattr_setsigmask(&spawnattr, &sigmask);
  posix_spawn_file_actions_init(&spawnfila);
  posix_spawn_file_actions_adddup2(&spawnfila, g_bogusfd, 0);
  posix_spawn_file_actions_adddup2(&spawnfila, client->pipe[1], 1);
  posix_spawn_file_actions_adddup2(&spawnfila, client->pipe[1], 2);
  err = posix_spawn(&client->pid, exe, &spawnfila, &spawnattr, args, environ);
  if (err) {
    if (err == ETXTBSY) {
      goto RetryOnEtxtbsyRaceCondition;
    }
    WARNF("%s failed to spawn on %s due to %s", exename, g_hostname,
          strerror(err));
    pthread_exit(0);
  }
  posix_spawn_file_actions_destroy(&spawnfila);
  posix_spawnattr_destroy(&spawnattr);
  Close(&client->pipe[1]);

  DEBUF("communicating %s[%d]", exename, client->pid);
  struct timespec deadline =
      timespec_add(timespec_real(), timespec_fromseconds(DEATH_CLOCK_SECONDS));
  for (;;) {
    if (g_interrupted) {
      WARNF("killing %d %s and hanging up %d due to interrupt", client->fd,
            exename, client->pid);
    HangupClientAndTerminateJob:
      SendProgramOutut(client);
      mbedtls_ssl_close_notify(&ezssl);
    TerminateJob:
      PrintProgramOutput(client);
      pthread_exit(0);
    }
    struct timespec now = timespec_real();
    if (timespec_cmp(now, deadline) >= 0) {
      WARNF("killing %s (pid %d) which timed out after %d seconds", exename,
            client->pid, DEATH_CLOCK_SECONDS);
      goto HangupClientAndTerminateJob;
    }
    struct pollfd fds[2];
    fds[0].fd = client->fd;
    fds[0].events = POLLIN;
    fds[1].fd = client->pipe[0];
    fds[1].events = POLLIN;
    events = poll(fds, ARRAYLEN(fds),
                  timespec_tomillis(timespec_sub(deadline, now)));
    if (events == -1) {
      if (errno == EINTR) {
        INFOF("poll interrupted");
        continue;
      } else {
        WARNF("killing %d %s and hanging up %d because poll failed", client->fd,
              exename, client->pid);
        goto HangupClientAndTerminateJob;
      }
    }
    if (events) {
      if (fds[0].revents) {
        int received;
        char buf[512];
        received = mbedtls_ssl_read(&ezssl, buf, sizeof(buf));
        if (!received) {
          WARNF("%s client disconnected so killing worker %d", exename,
                client->pid);
          goto TerminateJob;
        }
        if (received > 0) {
          WARNF("%s client sent %d unexpected bytes so killing job", exename,
                received);
          goto HangupClientAndTerminateJob;
        }
        if (received == MBEDTLS_ERR_SSL_WANT_READ) {  // EAGAIN SO_RCVTIMEO
          WARNF("%s (pid %d) is taking a really long time", exename,
                client->pid);
          continue;
        }
        WARNF("client ssl read failed with -0x%04x (%s) so killing %s",
              -received, GetTlsError(received), exename);
        goto TerminateJob;
      }
      if (fds[1].revents) {
        char buf[512];
        ssize_t got = read(client->pipe[0], buf, sizeof(buf));
        if (got == -1) {
          WARNF("got %s reading %s output", strerror(errno), exename);
          goto HangupClientAndTerminateJob;
        }
        if (!got) {
          VERBF("got eof reading %s output", exename);
          Close(&client->pipe[0]);
          break;
        }
        DEBUF("got %ld bytes reading %s output", got, exename);
        appendd(&client->output, buf, got);
      }
    }
  }
WaitAgain:
  DEBUF("waitpid");
  struct rusage rusage;
  int wrc = wait4(client->pid, &wstatus, 0, &rusage);
  if (wrc == -1) {
    if (errno == EINTR) {
      WARNF("waitpid interrupted; killing %s pid %d", exename, client->pid);
      kill(client->pid, SIGINT);
      goto WaitAgain;
    }
    WARNF("waitpid failed %m");
    client->pid = 0;
    goto HangupClientAndTerminateJob;
  }
  client->pid = 0;
  int exitcode;
  if (WIFEXITED(wstatus)) {
    if (WEXITSTATUS(wstatus)) {
      WARNF("%s on %s exited with %d", exename, g_hostname,
            WEXITSTATUS(wstatus));
      appendf(&client->output, "------ %s %s $?=%d (0x%08x) ------\n",
              g_hostname, exename, WEXITSTATUS(wstatus), wstatus);
    } else {
      VERBF("%s on %s exited with %d", exename, g_hostname,
            WEXITSTATUS(wstatus));
    }
    exitcode = WEXITSTATUS(wstatus);
  } else if (WIFSIGNALED(wstatus)) {
    if (WTERMSIG(wstatus) == SIGVTALRM && !client->output) {
      free(client->output);
      client->output = 0;
      goto RetryOnEtxtbsyRaceCondition;
    }
    WARNF("%s on %s terminated with %s", exename, g_hostname,
          strsignal(WTERMSIG(wstatus)));
    exitcode = 128 + WTERMSIG(wstatus);
    appendf(&client->output, "------ %s %s $?=%s (0x%08x) ------\n", g_hostname,
            exename, strsignal(WTERMSIG(wstatus)), wstatus);
  } else {
    WARNF("%s on %s died with wait status 0x%08x", exename, g_hostname,
          wstatus);
    exitcode = 127;
  }
  if (wstatus) {
    AppendResourceReport(&client->output, &rusage, "\n");
    PrintProgramOutput(client);
  }
  SendProgramOutut(client);
  SendExitMessage(exitcode);
  mbedtls_ssl_close_notify(&ezssl);
  if (etxtbsy_tries) {
    VERBF("encountered %d ETXTBSY race conditions spawning %s", etxtbsy_tries,
          exename);
  }
  pthread_exit(0);
}

void HandleClient(void) {
  struct Client *client;
  client = calloc(1, sizeof(struct Client));
  client->addrsize = sizeof(client->addr);
  for (;;) {
    if (g_interrupted) {
      free(client);
      return;
    }
    // poll() because we use SA_RESTART and accept() is @restartable
    if (poll(&(struct pollfd){g_servfd, POLLIN}, 1, -1) > 0) {
      client->fd = accept4(g_servfd, (struct sockaddr *)&client->addr,
                           &client->addrsize, SOCK_CLOEXEC);
      if (client->fd != -1) {
        VERBF("accepted client fd %d", client->fd);
        break;
      } else if (errno != EINTR && errno != EAGAIN) {
        WARNF("accept4 failed %m");
      }
    } else if (errno != EINTR && errno != EAGAIN) {
      WARNF("poll failed %m");
    }
  }
  sigset_t mask;
  pthread_attr_t attr;
  sigfillset(&mask);
  pthread_attr_init(&attr);
  pthread_attr_setsigmask_np(&attr, &mask);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  pthread_create(&client->th, &attr, ClientWorker, client);
  pthread_attr_destroy(&attr);
}

int Serve(void) {
  sigset_t mask;
  StartTcpServer();
  sigemptyset(&mask);
  sigaddset(&mask, SIGCHLD);
  signal(SIGINT, OnInterrupt);
  sigprocmask(SIG_BLOCK, &mask, 0);
  while (!g_interrupted) {
    HandleClient();
  }
  if (g_interrupted) {
    WARNF("got ctrl-c, shutting down...");
  }
  WARNF("server exiting");
  close(g_servfd);
  return 0;
}

void Daemonize(void) {
  VERBF("Daemonize");
  struct stat st;
  if (fork() > 0) _exit(0);
  setsid();
  if (fork() > 0) _exit(0);
  dup2(g_bogusfd, fileno(stdin));
  if (!g_sendready) dup2(g_bogusfd, fileno(stdout));
  freopen(kLogFile, "ae", stderr);
  if (fstat(fileno(stderr), &st) != -1 && st.st_size > kLogMaxBytes) {
    ftruncate(fileno(stderr), 0);
  }
}

int main(int argc, char *argv[]) {
#if IsModeDbg()
  ShowCrashReports();
#endif
  GetOpts(argc, argv);
  g_psk = GetRunitPsk();
  signal(SIGPIPE, SIG_IGN);
  setenv("TZ", "PST", true);
  gethostname(g_hostname, sizeof(g_hostname));
  for (int i = 3; i < 16; ++i) close(i);
  errno = 0;
  // poll()'ing /dev/null stdin file descriptor on xnu returns POLLNVAL?!
  if (IsWindows()) {
    g_bogusfd = open("/dev/null", O_RDONLY | O_CLOEXEC);
  } else {
    g_bogusfd = open("/dev/zero", O_RDONLY | O_CLOEXEC);
  }
  if (g_daemonize) Daemonize();
  mkdir("o", 0700);
  Serve();
  free(g_psk);
#if IsModeDbg()
  CheckForMemoryLeaks();
  CheckForFileLeaks();
#endif
  pthread_exit(0);
}
