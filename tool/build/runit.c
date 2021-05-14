/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/alg/alg.h"
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/sigbits.h"
#include "libc/calls/struct/flock.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timeval.h"
#include "libc/dce.h"
#include "libc/dns/dns.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/fmt.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/ipclassify.internal.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/itimer.h"
#include "libc/sysv/consts/lock.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/pr.h"
#include "libc/sysv/consts/shut.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/sock.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "tool/build/runit.h"

/**
 * @fileoverview Remote test runner.
 *
 * This is able to upload and run test binaries on remote operating
 * systems with about 30 milliseconds of latency. It requires zero ops
 * work too, since it deploys the ephemeral runit daemon via SSH upon
 * ECONNREFUSED. That takes 10x longer (300 milliseconds). Further note
 * there's no make -j race conditions here, thanks to SO_REUSEPORT.
 *
 *     o/default/tool/build/runit.com             \
 *         o/default/tool/build/runitd.com        \
 *         o/default/test/libc/alg/qsort_test.com \
 *         freebsd.test.:31337:22
 *
 * The only thing that needs to be configured is /etc/hosts or Bind, to
 * assign numbers to the officially reserved canned names. For example:
 *
 *     192.168.0.10 windows.test. windows
 *     192.168.0.11 freebsd.test. freebsd
 *     192.168.0.12 openbsd.test. openbsd
 *
 * Life is easiest if SSH public key authentication is configured too.
 * It can be tuned as follows in ~/.ssh/config:
 *
 *     host windows.test.
 *       user testacct
 *     host freebsd.test.
 *       user testacct
 *     host openbsd.test.
 *       user testacct
 *
 * Firewalls may need to be configured as well, to allow port tcp:31337
 * from the local subnet. For example:
 *
 *     iptables -L -vn
 *     iptables -I INPUT 1 -s 10.0.0.0/8 -p tcp --dport 31337 -j ACCEPT
 *     iptables -I INPUT 1 -s 192.168.0.0/16 -p tcp --dport 31337 -j ACCEPT
 *
 * If your system administrator blocks all ICMP, you'll likely encounter
 * difficulties. Consider offering feedback to his/her manager and grand
 * manager.
 *
 * Finally note this tool isn't designed for untrustworthy environments.
 * It also isn't designed to process untrustworthy inputs.
 */

static const struct addrinfo kResolvHints = {.ai_family = AF_INET,
                                             .ai_socktype = SOCK_STREAM,
                                             .ai_protocol = IPPROTO_TCP};

int g_sock;
char *g_prog;
char *g_runitd;
jmp_buf g_jmpbuf;
uint16_t g_sshport;
char g_ssh[PATH_MAX];
char g_hostname[128];
uint16_t g_runitdport;
volatile bool alarmed;

static void OnAlarm(int sig) {
  alarmed = true;
}

forceinline pureconst size_t GreatestTwoDivisor(size_t x) {
  return x & (~x + 1);
}

wontreturn void ShowUsage(FILE *f, int rc) {
  fprintf(f, "Usage: %s RUNITD PROGRAM HOSTNAME[:RUNITDPORT[:SSHPORT]]...\n",
          program_invocation_name);
  exit(rc);
  unreachable;
}

void CheckExists(const char *path) {
  if (!isregularfile(path)) {
    fprintf(stderr, "error: %s: not found or irregular\n", path);
    ShowUsage(stderr, EX_USAGE);
    unreachable;
  }
}

nodiscard char *MakeDeployScript(struct addrinfo *remotenic, size_t combytes) {
  const char *ip4 = (const char *)&remotenic->ai_addr4->sin_addr;
  return xasprintf("mkdir -p o/ && "
                   "dd bs=%zu count=%zu of=o/runitd.$$.com 2>/dev/null && "
                   "exec <&- && "
                   "chmod +x o/runitd.$$.com && "
                   "o/runitd.$$.com -rdl%hhu.%hhu.%hhu.%hhu -p %hu && "
                   "rm -f o/runitd.$$.com",
                   GreatestTwoDivisor(combytes),
                   combytes ? combytes / GreatestTwoDivisor(combytes) : 0,
                   ip4[0], ip4[1], ip4[2], ip4[3], g_runitdport);
}

void Upload(int pipe, int fd, struct stat *st) {
  int64_t i;
  for (i = 0; i < st->st_size;) {
    CHECK_GT(splice(fd, &i, pipe, NULL, st->st_size - i, 0), 0);
  }
  CHECK_NE(-1, close(fd));
}

void DeployEphemeralRunItDaemonRemotelyViaSsh(struct addrinfo *ai) {
  int lock;
  size_t got;
  char *args[7];
  struct stat st;
  char linebuf[32];
  struct timeval now, then;
  sigset_t chldmask, savemask;
  int sshpid, wstatus, binfd, pipefds[2][2];
  struct sigaction ignore, saveint, savequit;
  mkdir("o", 0755);
  CHECK_NE(-1, (lock = open(gc(xasprintf("o/lock.%s", g_hostname)),
                            O_RDWR | O_CREAT, 0644)));
  CHECK_NE(-1, flock(lock, LOCK_EX));
  CHECK_NE(-1, gettimeofday(&now, 0));
  if (!read(lock, &then, 16) || ((now.tv_sec * 1000 + now.tv_usec / 1000) -
                                 (then.tv_sec * 1000 + then.tv_usec / 1000)) >=
                                    (RUNITD_TIMEOUT_MS >> 1)) {
    DEBUGF("ssh %s:%hu to spawn %s", g_hostname, g_runitdport, g_runitd);
    CHECK_NE(-1, (binfd = open(g_runitd, O_RDONLY | O_CLOEXEC)));
    CHECK_NE(-1, fstat(binfd, &st));
    args[0] = "ssh";
    args[1] = "-C";
    args[2] = "-p";
    args[3] = gc(xasprintf("%hu", g_sshport));
    args[4] = g_hostname;
    args[5] = gc(MakeDeployScript(ai, st.st_size));
    args[6] = NULL;
    ignore.sa_flags = 0;
    ignore.sa_handler = SIG_IGN;
    LOGIFNEG1(sigemptyset(&ignore.sa_mask));
    LOGIFNEG1(sigaction(SIGINT, &ignore, &saveint));
    LOGIFNEG1(sigaction(SIGQUIT, &ignore, &savequit));
    LOGIFNEG1(sigemptyset(&chldmask));
    LOGIFNEG1(sigaddset(&chldmask, SIGCHLD));
    LOGIFNEG1(sigprocmask(SIG_BLOCK, &chldmask, &savemask));
    CHECK_NE(-1, pipe2(pipefds[0], O_CLOEXEC));
    CHECK_NE(-1, pipe2(pipefds[1], O_CLOEXEC));
    CHECK_NE(-1, (sshpid = fork()));
    if (!sshpid) {
      sigaction(SIGINT, &saveint, NULL);
      sigaction(SIGQUIT, &savequit, NULL);
      sigprocmask(SIG_SETMASK, &savemask, NULL);
      dup2(pipefds[0][0], 0);
      dup2(pipefds[1][1], 1);
      execv(g_ssh, args);
      _exit(127);
    }
    LOGIFNEG1(close(pipefds[0][0]));
    LOGIFNEG1(close(pipefds[1][1]));
    Upload(pipefds[0][1], binfd, &st);
    LOGIFNEG1(close(pipefds[0][1]));
    CHECK_NE(-1, (got = read(pipefds[1][0], linebuf, sizeof(linebuf))));
    CHECK_GT(got, 0, "on host %s", g_hostname);
    linebuf[sizeof(linebuf) - 1] = '\0';
    if (strncmp(linebuf, "ready ", 6) != 0) {
      FATALF("expected ready response but got %`'.*s", got, linebuf);
    } else {
      DEBUGF("got ready response");
    }
    g_runitdport = (uint16_t)atoi(&linebuf[6]);
    LOGIFNEG1(close(pipefds[1][0]));
    CHECK_NE(-1, waitpid(sshpid, &wstatus, 0));
    LOGIFNEG1(sigaction(SIGINT, &saveint, NULL));
    LOGIFNEG1(sigaction(SIGQUIT, &savequit, NULL));
    LOGIFNEG1(sigprocmask(SIG_SETMASK, &savemask, NULL));
    if (WIFEXITED(wstatus)) {
      DEBUGF("ssh %s exited with %d", g_hostname, WEXITSTATUS(wstatus));
    } else {
      DEBUGF("ssh %s terminated with %s", g_hostname,
             strsignal(WTERMSIG(wstatus)));
    }
    CHECK(WIFEXITED(wstatus) && !WEXITSTATUS(wstatus), "wstatus=%#x", wstatus);
    CHECK_NE(-1, gettimeofday(&now, 0));
    CHECK_NE(-1, lseek(lock, 0, SEEK_SET));
    CHECK_NE(-1, write(lock, &now, 16));
  } else {
    DEBUGF("nospawn %s on %s:%hu", g_runitd, g_hostname, g_runitdport);
  }
  LOGIFNEG1(close(lock));
}

void SetDeadline(int micros) {
  alarmed = false;
  LOGIFNEG1(
      sigaction(SIGALRM, &(struct sigaction){.sa_handler = OnAlarm}, NULL));
  LOGIFNEG1(setitimer(ITIMER_REAL,
                      &(const struct itimerval){{0, 0}, {0, micros}}, NULL));
}

void Connect(void) {
  const char *ip4;
  int rc, err, expo;
  struct addrinfo *ai;
  if ((rc = getaddrinfo(g_hostname, gc(xasprintf("%hu", g_runitdport)),
                        &kResolvHints, &ai)) != 0) {
    FATALF("%s:%hu: EAI_%s %m", g_hostname, g_runitdport, gai_strerror(rc));
    unreachable;
  }
  ip4 = (const char *)&ai->ai_addr4->sin_addr;
  if (ispublicip(ai->ai_family, &ai->ai_addr4->sin_addr)) {
    FATALF("%s points to %hhu.%hhu.%hhu.%hhu"
           " which isn't part of a local/private/testing subnet",
           g_hostname, ip4[0], ip4[1], ip4[2], ip4[3]);
    unreachable;
  }
  CHECK_NE(-1,
           (g_sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)));
  expo = 1;
Reconnect:
  DEBUGF("connecting to %s (%hhu.%hhu.%hhu.%hhu) to run %s", g_hostname, ip4[0],
         ip4[1], ip4[2], ip4[3], g_prog);
  SetDeadline(100000);
TryAgain:
  rc = connect(g_sock, ai->ai_addr, ai->ai_addrlen);
  err = errno;
  SetDeadline(0);
  if (rc == -1) {
    if (err == EINTR) goto TryAgain;
    if (err == ECONNREFUSED || err == EHOSTUNREACH || err == ECONNRESET) {
      DEBUGF("got %s from %s (%hhu.%hhu.%hhu.%hhu)", strerror(err), g_hostname,
             ip4[0], ip4[1], ip4[2], ip4[3]);
      usleep((expo *= 2));
      DeployEphemeralRunItDaemonRemotelyViaSsh(ai);
      goto Reconnect;
    } else {
      FATALF("%s(%s:%hu): %s", "connect", g_hostname, g_runitdport,
             strerror(err));
      unreachable;
    }
  } else {
    DEBUGF("connected to %s", g_hostname);
  }
  freeaddrinfo(ai);
}

void SendRequest(void) {
  int fd;
  int64_t off;
  struct stat st;
  const char *name;
  unsigned char *hdr;
  size_t progsize, namesize, hdrsize;
  DEBUGF("running %s on %s", g_prog, g_hostname);
  CHECK_NE(-1, (fd = open(g_prog, O_RDONLY)));
  CHECK_NE(-1, fstat(fd, &st));
  CHECK_LE((namesize = strlen((name = basename(g_prog)))), PATH_MAX);
  CHECK_LE((progsize = st.st_size), INT_MAX);
  CHECK_NOTNULL((hdr = gc(calloc(1, (hdrsize = 4 + 1 + 4 + 4 + namesize)))));
  hdr[0 + 0] = (unsigned char)((unsigned)RUNITD_MAGIC >> 030);
  hdr[0 + 1] = (unsigned char)((unsigned)RUNITD_MAGIC >> 020);
  hdr[0 + 2] = (unsigned char)((unsigned)RUNITD_MAGIC >> 010);
  hdr[0 + 3] = (unsigned char)((unsigned)RUNITD_MAGIC >> 000);
  hdr[4 + 0] = kRunitExecute;
  hdr[5 + 0] = (unsigned char)((unsigned)namesize >> 030);
  hdr[5 + 1] = (unsigned char)((unsigned)namesize >> 020);
  hdr[5 + 2] = (unsigned char)((unsigned)namesize >> 010);
  hdr[5 + 3] = (unsigned char)((unsigned)namesize >> 000);
  hdr[9 + 0] = (unsigned char)((unsigned)progsize >> 030);
  hdr[9 + 1] = (unsigned char)((unsigned)progsize >> 020);
  hdr[9 + 2] = (unsigned char)((unsigned)progsize >> 010);
  hdr[9 + 3] = (unsigned char)((unsigned)progsize >> 000);
  memcpy(&hdr[4 + 1 + 4 + 4], name, namesize);
  CHECK_EQ(hdrsize, write(g_sock, hdr, hdrsize));
  for (off = 0; off < progsize;) {
    CHECK_GT(sendfile(g_sock, fd, &off, progsize - off), 0);
  }
  CHECK_NE(-1, shutdown(g_sock, SHUT_WR));
}

int ReadResponse(void) {
  int res;
  uint32_t size;
  ssize_t rc;
  size_t n, m;
  unsigned char *p;
  enum RunitCommand cmd;
  static long backoff;
  static unsigned char msg[512];
  res = -1;
  for (;;) {
    if ((rc = recv(g_sock, msg, sizeof(msg), 0)) == -1) {
      CHECK_EQ(ECONNRESET, errno);
      usleep((backoff = (backoff + 1000) * 2));
      break;
    }
    p = &msg[0];
    n = (size_t)rc;
    if (!n) break;
    do {
      CHECK_GE(n, 4 + 1);
      CHECK_EQ(RUNITD_MAGIC, READ32BE(p));
      p += 4, n -= 4;
      cmd = *p++, n--;
      switch (cmd) {
        case kRunitExit:
          CHECK_GE(n, 1);
          if ((res = *p & 0xff)) {
            WARNF("%s on %s exited with %d", g_prog, g_hostname, res);
          }
          goto drop;
        case kRunitStderr:
          CHECK_GE(n, 4);
          size = READ32BE(p), p += 4, n -= 4;
          while (size) {
            if (n) {
              CHECK_NE(-1, (rc = write(STDERR_FILENO, p, min(n, size))));
              CHECK_NE(0, (m = (size_t)rc));
              p += m, n -= m, size -= m;
            } else {
              CHECK_NE(-1, (rc = recv(g_sock, msg, sizeof(msg), 0)));
              p = &msg[0];
              n = (size_t)rc;
              if (!n) goto drop;
            }
          }
          break;
        default:
          __die();
      }
    } while (n);
  }
drop:
  CHECK_NE(-1, close(g_sock));
  return res;
}

int RunOnHost(char *spec) {
  int rc;
  char *p;
  for (p = spec; *p; ++p) {
    if (*p == ':') *p = ' ';
  }
  CHECK_GE(sscanf(spec, "%100s %hu %hu", g_hostname, &g_runitdport, &g_sshport),
           1);
  if (!strchr(g_hostname, '.')) strcat(g_hostname, ".test.");
  do {
    Connect();
    SendRequest();
  } while ((rc = ReadResponse()) == -1);
  return rc;
}

bool IsParallelBuild(void) {
  const char *makeflags;
  return (makeflags = getenv("MAKEFLAGS")) && strstr(makeflags, "-j");
}

bool ShouldRunInParralel(void) {
  return !IsWindows() && IsParallelBuild();
}

int RunRemoteTestsInParallel(char *hosts[], int count) {
  sigset_t chldmask, savemask;
  int i, rc, ws, pid, *pids, exitcode;
  struct sigaction ignore, saveint, savequit;
  pids = calloc(count, sizeof(char *));
  ignore.sa_flags = 0;
  ignore.sa_handler = SIG_IGN;
  LOGIFNEG1(sigemptyset(&ignore.sa_mask));
  LOGIFNEG1(sigaction(SIGINT, &ignore, &saveint));
  LOGIFNEG1(sigaction(SIGQUIT, &ignore, &savequit));
  LOGIFNEG1(sigemptyset(&chldmask));
  LOGIFNEG1(sigaddset(&chldmask, SIGCHLD));
  LOGIFNEG1(sigprocmask(SIG_BLOCK, &chldmask, &savemask));
  for (i = 0; i < count; ++i) {
    CHECK_NE(-1, (pids[i] = fork()));
    if (!pids[i]) {
      sigaction(SIGINT, &saveint, NULL);
      sigaction(SIGQUIT, &savequit, NULL);
      sigprocmask(SIG_SETMASK, &savemask, NULL);
      _exit(RunOnHost(hosts[i]));
    }
  }
  for (exitcode = 0;;) {
    if ((pid = wait(&ws)) == -1) {
      if (errno == EINTR) continue;
      if (errno == ECHILD) break;
      FATALF("wait failed");
    }
    for (i = 0; i < count; ++i) {
      if (pids[i] != pid) continue;
      if (WIFEXITED(ws)) {
        DEBUGF("%s exited with %d", hosts[i], WEXITSTATUS(ws));
        if (!exitcode) exitcode = WEXITSTATUS(ws);
      } else {
        DEBUGF("%s terminated with %s", hosts[i], strsignal(WTERMSIG(ws)));
        if (!exitcode) exitcode = 128 + WTERMSIG(ws);
      }
      break;
    }
  }
  LOGIFNEG1(sigaction(SIGINT, &saveint, NULL));
  LOGIFNEG1(sigaction(SIGQUIT, &savequit, NULL));
  LOGIFNEG1(sigprocmask(SIG_SETMASK, &savemask, NULL));
  free(pids);
  return exitcode;
}

int main(int argc, char *argv[]) {
  showcrashreports();
  /* __log_level = kLogDebug; */
  if (argc > 1 &&
      (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
    ShowUsage(stdout, 0);
    unreachable;
  }
  if (argc < 1 + 2) ShowUsage(stderr, EX_USAGE);
  CHECK_NOTNULL(commandv(firstnonnull(getenv("SSH"), "ssh"), g_ssh));
  CheckExists((g_runitd = argv[1]));
  CheckExists((g_prog = argv[2]));
  if (argc == 1 + 2) return 0; /* hosts list empty */
  g_sshport = 22;
  g_runitdport = RUNITD_PORT;
  return RunRemoteTestsInParallel(&argv[3], argc - 3);
}
