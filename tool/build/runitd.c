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
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timeval.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/bits.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/inaddr.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/itimer.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"
#include "libc/sysv/consts/w.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "libc/x/xasprintf.h"
#include "net/https/https.h"
#include "third_party/getopt/getopt.h"
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

#define DEATH_CLOCK_SECONDS 128

#define kLogFile     "o/runitd.log"
#define kLogMaxBytes (2 * 1000 * 1000)

bool use_ftrace;
bool use_strace;
char *g_exepath;
volatile bool g_interrupted;
struct sockaddr_in g_servaddr;
unsigned char g_buf[PAGESIZE];
bool g_daemonize, g_sendready;
int g_timeout, g_bogusfd, g_servfd, g_clifd, g_exefd;

void OnInterrupt(int sig) {
  g_interrupted = true;
}

void OnChildTerminated(int sig) {
  int ws, pid;
  sigset_t ss, oldss;
  sigfillset(&ss);
  sigdelset(&ss, SIGTERM);
  sigprocmask(SIG_BLOCK, &ss, &oldss);
  for (;;) {
    INFOF("waitpid");
    if ((pid = waitpid(-1, &ws, WNOHANG)) != -1) {
      if (pid) {
        if (WIFEXITED(ws)) {
          DEBUGF("worker %d exited with %d", pid, WEXITSTATUS(ws));
        } else {
          DEBUGF("worker %d terminated with %s", pid, strsignal(WTERMSIG(ws)));
        }
      } else {
        break;
      }
    } else {
      if (errno == EINTR) continue;
      if (errno == ECHILD) break;
      FATALF("waitpid failed in sigchld");
    }
  }
  sigprocmask(SIG_SETMASK, &oldss, 0);
}

wontreturn void ShowUsage(FILE *f, int rc) {
  fprintf(f, "%s: %s %s\n", "Usage", program_invocation_name,
          "[-d] [-r] [-l LISTENIP] [-p PORT] [-t TIMEOUTMS]");
  exit(rc);
}

void GetOpts(int argc, char *argv[]) {
  int opt;
  g_timeout = RUNITD_TIMEOUT_MS;
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
        --__log_level;
        break;
      case 'v':
        ++__log_level;
        break;
      case 'd':
        g_daemonize = true;
        break;
      case 'r':
        g_sendready = true;
        break;
      case 't':
        g_timeout = atoi(optarg);
        break;
      case 'p':
        CHECK_NE(0xFFFF, (g_servaddr.sin_port = htons(parseport(optarg))));
        break;
      case 'l':
        CHECK_EQ(1, inet_pton(AF_INET, optarg, &g_servaddr.sin_addr));
        break;
      case 'h':
        ShowUsage(stdout, EXIT_SUCCESS);
        unreachable;
      default:
        ShowUsage(stderr, EX_USAGE);
        unreachable;
    }
  }
}

dontdiscard char *DescribeAddress(struct sockaddr_in *addr) {
  char ip4buf[16];
  return xasprintf("%s:%hu",
                   inet_ntop(addr->sin_family, &addr->sin_addr.s_addr, ip4buf,
                             sizeof(ip4buf)),
                   ntohs(addr->sin_port));
}

void StartTcpServer(void) {
  int yes = true;
  uint32_t asize;

  /*
   * TODO: How can we make close(serversocket) on Windows go fast?
   *       That way we can put back SOCK_CLOEXEC.
   */
  CHECK_NE(-1, (g_servfd =
                    socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, IPPROTO_TCP)));

  struct timeval timeo = {30};
  setsockopt(g_servfd, SOL_SOCKET, SO_RCVTIMEO, &timeo, sizeof(timeo));
  setsockopt(g_servfd, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo));

  LOGIFNEG1(setsockopt(g_servfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)));
  if (bind(g_servfd, (struct sockaddr *)&g_servaddr, sizeof(g_servaddr)) ==
      -1) {
    FATALF("bind failed %m");
  }
  CHECK_NE(-1, listen(g_servfd, 10));
  asize = sizeof(g_servaddr);
  CHECK_NE(-1, getsockname(g_servfd, (struct sockaddr *)&g_servaddr, &asize));
  INFOF("%s:%s", "listening on tcp", _gc(DescribeAddress(&g_servaddr)));
  if (g_sendready) {
    printf("ready %hu\n", ntohs(g_servaddr.sin_port));
    fflush(stdout);
    fclose(stdout);
    dup2(g_bogusfd, stdout->fd);
  }
}

void SendExitMessage(int rc) {
  unsigned char msg[4 + 1 + 1];
  msg[0 + 0] = (RUNITD_MAGIC & 0xff000000) >> 030;
  msg[0 + 1] = (RUNITD_MAGIC & 0x00ff0000) >> 020;
  msg[0 + 2] = (RUNITD_MAGIC & 0x0000ff00) >> 010;
  msg[0 + 3] = (RUNITD_MAGIC & 0x000000ff) >> 000;
  msg[4] = kRunitExit;
  msg[5] = rc;
  INFOF("mbedtls_ssl_write");
  CHECK_EQ(sizeof(msg), mbedtls_ssl_write(&ezssl, msg, sizeof(msg)));
  CHECK_EQ(0, EzTlsFlush(&ezbio, 0, 0));
}

void SendOutputFragmentMessage(enum RunitCommand kind, unsigned char *buf,
                               size_t size) {
  ssize_t rc;
  size_t sent;
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
  INFOF("mbedtls_ssl_write");
  CHECK_EQ(sizeof(msg), mbedtls_ssl_write(&ezssl, msg, sizeof(msg)));
  while (size) {
    CHECK_NE(-1, (rc = mbedtls_ssl_write(&ezssl, buf, size)));
    CHECK_LE((sent = (size_t)rc), size);
    size -= sent;
    buf += sent;
  }
  CHECK_EQ(0, EzTlsFlush(&ezbio, 0, 0));
}

void Recv(void *output, size_t outputsize) {
  int rc;
  ssize_t tx, chunk, received, totalgot;
  static bool once;
  static int zstatus;
  static char buf[32768];
  static z_stream zs;
  static struct {
    size_t off;
    size_t len;
    size_t cap;
    char *data;
  } rbuf;
  if (!once) {
    CHECK_EQ(Z_OK, inflateInit(&zs));
    once = true;
  }
  totalgot = 0;
  for (;;) {
    if (rbuf.len >= outputsize) {
      tx = MIN(outputsize, rbuf.len);
      memcpy(output, rbuf.data + rbuf.off, outputsize);
      rbuf.len -= outputsize;
      rbuf.off += outputsize;
      // trim dymanic buffer once it empties
      if (!rbuf.len) {
        rbuf.off = 0;
        rbuf.cap = 4096;
        rbuf.data = realloc(rbuf.data, rbuf.cap);
      }
      return;
    }
    if (zstatus == Z_STREAM_END) {
      close(g_clifd);
      FATALF("recv zlib unexpected eof");
    }
    // get another fixed-size data packet from network
    // pass along error conditions to caller
    // pass along eof condition to zlib
    received = mbedtls_ssl_read(&ezssl, buf, sizeof(buf));
    if (!received) {
      close(g_clifd);
      TlsDie("got unexpected eof", received);
    }
    if (received < 0) {
      close(g_clifd);
      TlsDie("read failed", received);
    }
    totalgot += received;
    // decompress packet completely
    // into a dynamical size buffer
    zs.avail_in = received;
    zs.next_in = (unsigned char *)buf;
    CHECK_EQ(Z_OK, zstatus);
    do {
      // make sure we have a reasonable capacity for zlib output
      if (rbuf.cap - (rbuf.off + rbuf.len) < sizeof(buf)) {
        rbuf.cap += sizeof(buf);
        rbuf.data = realloc(rbuf.data, rbuf.cap);
      }
      // inflate packet, which naturally can be much larger
      // permit zlib no delay flushes that come from sender
      zs.next_out = (unsigned char *)rbuf.data + (rbuf.off + rbuf.len);
      zs.avail_out = chunk = rbuf.cap - (rbuf.off + rbuf.len);
      zstatus = inflate(&zs, Z_SYNC_FLUSH);
      CHECK_NE(Z_STREAM_ERROR, zstatus);
      switch (zstatus) {
        case Z_NEED_DICT:
          WARNF("tls recv Z_NEED_DICT %ld total %ld", received, totalgot);
          exit(1);
        case Z_DATA_ERROR:
          WARNF("tls recv Z_DATA_ERROR %ld total %ld", received, totalgot);
          exit(1);
        case Z_MEM_ERROR:
          WARNF("tls recv Z_MEM_ERROR %ld total %ld", received, totalgot);
          exit(1);
        case Z_BUF_ERROR:
          zstatus = Z_OK;  // harmless? nothing for inflate to do
          break;           // it probably just our wraparound eof
        default:
          rbuf.len += chunk - zs.avail_out;
          break;
      }
    } while (!zs.avail_out);
  }
}

void HandleClient(void) {
  const size_t kMaxNameSize = 128;
  const size_t kMaxFileSize = 10 * 1024 * 1024;
  uint32_t crc;
  ssize_t got, wrote;
  struct sockaddr_in addr;
  long double now, deadline;
  sigset_t chldmask, savemask;
  char *addrstr, *exename, *exe;
  unsigned char msg[4 + 1 + 4 + 4 + 4];
  struct sigaction ignore, saveint, savequit;
  uint32_t addrsize, namesize, filesize, remaining;
  int rc, events, exitcode, wstatus, child, pipefds[2];

  /* read request to run program */
  addrsize = sizeof(addr);
  INFOF("accept");
  do {
    g_clifd =
        accept4(g_servfd, (struct sockaddr *)&addr, &addrsize, SOCK_CLOEXEC);
  } while (g_clifd == -1 && errno == EAGAIN);
  CHECK_NE(-1, g_clifd);
  if (fork()) {
    close(g_clifd);
    return;
  }
  EzFd(g_clifd);
  INFOF("EzHandshake");
  EzHandshake();
  addrstr = _gc(DescribeAddress(&addr));
  DEBUGF("%s %s %s", _gc(DescribeAddress(&g_servaddr)), "accepted", addrstr);

  Recv(msg, sizeof(msg));
  CHECK_EQ(RUNITD_MAGIC, READ32BE(msg));
  CHECK_EQ(kRunitExecute, msg[4]);
  namesize = READ32BE(msg + 5);
  filesize = READ32BE(msg + 9);
  crc = READ32BE(msg + 13);
  exename = _gc(calloc(1, namesize + 1));
  Recv(exename, namesize);
  g_exepath = _gc(xasprintf("o/%d.%s", getpid(), basename(exename)));
  INFOF("%s asked we run %`'s (%,u bytes @ %`'s)", addrstr, exename, filesize,
        g_exepath);

  exe = malloc(filesize);
  Recv(exe, filesize);
  if (crc32_z(0, exe, filesize) != crc) {
    FATALF("%s crc mismatch! %`'s", addrstr, exename);
  }
  CHECK_NE(-1, (g_exefd = creat(g_exepath, 0700)));
  LOGIFNEG1(ftruncate(g_exefd, filesize));
  INFOF("xwrite");
  CHECK_NE(-1, xwrite(g_exefd, exe, filesize));
  LOGIFNEG1(close(g_exefd));

  /* run program, tee'ing stderr to both log and client */
  DEBUGF("spawning %s", exename);
  ignore.sa_flags = 0;
  ignore.sa_handler = SIG_IGN;
  sigemptyset(&ignore.sa_mask);
  sigaction(SIGINT, &ignore, &saveint);
  sigaction(SIGQUIT, &ignore, &savequit);
  sigemptyset(&chldmask);
  sigaddset(&chldmask, SIGCHLD);
  sigprocmask(SIG_BLOCK, &chldmask, &savemask);
  CHECK_NE(-1, pipe2(pipefds, O_CLOEXEC));
  CHECK_NE(-1, (child = fork()));
  if (!child) {
    dup2(g_bogusfd, 0);
    dup2(pipefds[1], 1);
    dup2(pipefds[1], 2);
    sigaction(SIGINT, &(struct sigaction){0}, 0);
    sigaction(SIGQUIT, &(struct sigaction){0}, 0);
    sigprocmask(SIG_SETMASK, &savemask, 0);
    int i = 0;
    char *args[4] = {0};
    args[i++] = g_exepath;
    if (use_strace) args[i++] = "--strace";
    if (use_ftrace) args[i++] = "--ftrace";
    execv(g_exepath, args);
    _Exit(127);
  }
  close(pipefds[1]);
  DEBUGF("communicating %s[%d]", exename, child);
  deadline = nowl() + DEATH_CLOCK_SECONDS;
  for (;;) {
    now = nowl();
    if (now >= deadline) {
      WARNF("%s worker timed out", exename);
      LOGIFNEG1(kill(child, 9));
      LOGIFNEG1(waitpid(child, 0, 0));
      LOGIFNEG1(close(g_clifd));
      LOGIFNEG1(close(pipefds[0]));
      LOGIFNEG1(unlink(g_exepath));
      _exit(1);
    }
    struct pollfd fds[2];
    fds[0].fd = g_clifd;
    fds[0].events = POLLIN;
    fds[1].fd = pipefds[0];
    fds[1].events = POLLIN;
    INFOF("poll");
    events = poll(fds, ARRAYLEN(fds), (deadline - now) * 1000);
    CHECK_NE(-1, events);  // EINTR shouldn't be possible
    if (fds[0].revents) {
      if (!(fds[0].revents & POLLHUP)) {
        WARNF("%s got unexpected input event from client %#x", exename,
              fds[0].revents);
      }
      WARNF("%s client disconnected so killing worker %d", exename, child);
      LOGIFNEG1(kill(child, 9));
      LOGIFNEG1(waitpid(child, 0, 0));
      LOGIFNEG1(close(g_clifd));
      LOGIFNEG1(close(pipefds[0]));
      LOGIFNEG1(unlink(g_exepath));
      _exit(1);
    }
    INFOF("read");
    got = read(pipefds[0], g_buf, sizeof(g_buf));
    CHECK_NE(-1, got);  // EINTR shouldn't be possible
    if (!got) {
      LOGIFNEG1(close(pipefds[0]));
      break;
    }
    fwrite(g_buf, got, 1, stderr);
    SendOutputFragmentMessage(kRunitStderr, g_buf, got);
  }
  INFOF("waitpid");
  CHECK_NE(-1, waitpid(child, &wstatus, 0));  // EINTR shouldn't be possible
  if (WIFEXITED(wstatus)) {
    if (WEXITSTATUS(wstatus)) {
      WARNF("%s exited with %d", exename, WEXITSTATUS(wstatus));
    } else {
      VERBOSEF("%s exited with %d", exename, WEXITSTATUS(wstatus));
    }
    exitcode = WEXITSTATUS(wstatus);
  } else {
    WARNF("%s terminated with %s", exename, strsignal(WTERMSIG(wstatus)));
    exitcode = 128 + WTERMSIG(wstatus);
  }
  LOGIFNEG1(unlink(g_exepath));
  SendExitMessage(exitcode);
  INFOF("mbedtls_ssl_close_notify");
  mbedtls_ssl_close_notify(&ezssl);
  LOGIFNEG1(close(g_clifd));
  _exit(0);
}

int Poll(void) {
  int i, wait, evcount;
  struct pollfd fds[1];
TryAgain:
  if (g_interrupted) return 0;
  fds[0].fd = g_servfd;
  fds[0].events = POLLIN | POLLERR | POLLHUP;
  wait = MIN(1000, g_timeout);
  evcount = poll(fds, ARRAYLEN(fds), wait);
  if (!evcount) g_timeout -= wait;
  if (evcount == -1 && errno == EINTR) goto TryAgain;
  CHECK_NE(-1, evcount);
  for (i = 0; i < evcount; ++i) {
    CHECK(fds[i].revents & POLLIN);
    HandleClient();
  }
  /* manually do this because of nt */
  while (waitpid(-1, NULL, WNOHANG) > 0) donothing;
  return evcount;
}

int Serve(void) {
  StartTcpServer();
  sigaction(SIGINT, (&(struct sigaction){.sa_handler = OnInterrupt}), 0);
  sigaction(SIGCHLD,
            (&(struct sigaction){.sa_handler = OnChildTerminated,
                                 .sa_flags = SA_RESTART}),
            0);
  for (;;) {
    if (!Poll() && (!g_timeout || g_interrupted)) break;
  }
  close(g_servfd);
  if (!g_timeout) {
    INFOF("timeout expired, shutting down");
  } else {
    INFOF("got ctrl-c, shutting down");
  }
  return 0;
}

void Daemonize(void) {
  struct stat st;
  if (fork() > 0) _exit(0);
  setsid();
  if (fork() > 0) _exit(0);
  dup2(g_bogusfd, stdin->fd);
  if (!g_sendready) dup2(g_bogusfd, stdout->fd);
  freopen(kLogFile, "ae", stderr);
  if (fstat(fileno(stderr), &st) != -1 && st.st_size > kLogMaxBytes) {
    ftruncate(fileno(stderr), 0);
  }
}

int main(int argc, char *argv[]) {
  int i;
  SetupPresharedKeySsl(MBEDTLS_SSL_IS_SERVER, GetRunitPsk());
  __log_level = kLogWarn;
  GetOpts(argc, argv);
  for (i = 3; i < 16; ++i) close(i);
  errno = 0;
  // poll()'ing /dev/null stdin file descriptor on xnu returns POLLNVAL?!
  if (IsWindows()) {
    CHECK_EQ(3, (g_bogusfd = open("/dev/null", O_RDONLY | O_CLOEXEC)));
  } else {
    CHECK_EQ(3, (g_bogusfd = open("/dev/zero", O_RDONLY | O_CLOEXEC)));
  }
  if (!isdirectory("o")) CHECK_NE(-1, mkdir("o", 0700));
  if (g_daemonize) Daemonize();
  return Serve();
}
