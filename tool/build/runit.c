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
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/sigbits.h"
#include "libc/calls/struct/flock.h"
#include "libc/dns/dns.h"
#include "libc/fmt/conv.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/runtime/gc.internal.h"
#include "libc/sock/ipclassify.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/itimer.h"
#include "libc/sysv/consts/lock.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sock.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "net/https/https.h"
#include "third_party/mbedtls/ssl.h"
#include "tool/build/lib/eztls.h"
#include "tool/build/lib/psk.h"
#include "tool/build/runit.h"

/**
 * @fileoverview Remote test runner.
 *
 * We want to scp .com binaries to remote machines and run them. The
 * problem is that SSH is the slowest thing imaginable, taking about
 * 300ms to connect to a host that's merely half a millisecond away.
 *
 * This program takes 17ms using elliptic curve diffie hellman exchange
 * where we favor a 32-byte binary preshared key (~/.runit.psk) instead
 * of certificates. It's how long it takes to connect, copy the binary,
 * and run it. The remote daemon is deployed via SSH if it's not there.
 *
 *     o/default/tool/build/runit.com             \
 *         o/default/tool/build/runitd.com        \
 *         o/default/test/libc/alg/qsort_test.com \
 *         freebsd.test.:31337:22
 *
 * APE binaries are hermetic and embed dependent files within their zip
 * structure, which is why all we need is this simple test runner tool.
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
 * This tool may be used in zero trust environments.
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
  char *p;
  size_t i;
  ssize_t rc;
  uint32_t crc;
  struct stat st;
  const char *name;
  unsigned char *hdr, *q;
  size_t progsize, namesize, hdrsize;
  DEBUGF("running %s on %s", g_prog, g_hostname);
  CHECK_NE(-1, (fd = open(g_prog, O_RDONLY)));
  CHECK_NE(-1, fstat(fd, &st));
  CHECK_NE(MAP_FAILED, (p = mmap(0, st.st_size, PROT_READ, MAP_SHARED, fd, 0)));
  CHECK_LE((namesize = strlen((name = basename(g_prog)))), PATH_MAX);
  CHECK_LE((progsize = st.st_size), INT_MAX);
  CHECK_NOTNULL((hdr = gc(calloc(1, (hdrsize = 17 + namesize)))));
  crc = crc32_z(0, p, st.st_size);
  q = hdr;
  q = WRITE32BE(q, RUNITD_MAGIC);
  *q++ = kRunitExecute;
  q = WRITE32BE(q, namesize);
  q = WRITE32BE(q, progsize);
  q = WRITE32BE(q, crc);
  q = mempcpy(q, name, namesize);
  assert(hdrsize == q - hdr);
  CHECK_EQ(hdrsize, mbedtls_ssl_write(&ezssl, hdr, hdrsize));
  for (i = 0; i < progsize; i += rc) {
    CHECK_GT((rc = mbedtls_ssl_write(&ezssl, p + i, progsize - i)), 0);
  }
  CHECK_EQ(0, EzTlsFlush(&ezbio, 0, 0));
  CHECK_NE(-1, munmap(p, st.st_size));
  CHECK_NE(-1, close(fd));
}

bool Recv(unsigned char *p, size_t n) {
  size_t i, rc;
  static long backoff;
  for (i = 0; i < n; i += rc) {
    do {
      rc = mbedtls_ssl_read(&ezssl, p + i, n - i);
    } while (rc == MBEDTLS_ERR_SSL_WANT_READ);
    if (!rc || rc == MBEDTLS_ERR_NET_CONN_RESET) {
      usleep((backoff = (backoff + 1000) * 2));
      return false;
    } else if (rc < 0) {
      TlsDie("read response failed", rc);
    }
  }
  return true;
}

int ReadResponse(void) {
  int res;
  ssize_t rc;
  size_t n, m;
  uint32_t size;
  unsigned char b[512];
  for (res = -1; res == -1;) {
    if (!Recv(b, 5)) break;
    CHECK_EQ(RUNITD_MAGIC, READ32BE(b), "%#.5s", b);
    switch (b[4]) {
      case kRunitExit:
        if (!Recv(b, 1)) break;
        if ((res = *b)) {
          WARNF("%s on %s exited with %d", g_prog, g_hostname, res);
        }
        break;
      case kRunitStderr:
        if (!Recv(b, 4)) break;
        size = READ32BE(b);
        for (; size; size -= n) {
          n = MIN(size, sizeof(b));
          if (!Recv(b, n)) goto drop;
          CHECK_EQ(n, write(2, b, n));
        }
        break;
      default:
        fprintf(stderr, "error: received invalid runit command\n");
        _exit(1);
    }
  }
drop:
  close(g_sock);
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
    EzFd(g_sock);
    EzHandshake(); /* TODO(jart): Backoff on MBEDTLS_ERR_NET_CONN_RESET */
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
  ShowCrashReports();
  SetupPresharedKeySsl(MBEDTLS_SSL_IS_CLIENT, GetRunitPsk());
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
