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
#include "libc/calls/struct/flock.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/stat.h"
#include "libc/dce.h"
#include "libc/dns/dns.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/kprintf.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/runtime/gc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/ipclassify.internal.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/itimer.h"
#include "libc/sysv/consts/limits.h"
#include "libc/sysv/consts/lock.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/sock.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "net/https/https.h"
#include "third_party/mbedtls/net_sockets.h"
#include "third_party/mbedtls/ssl.h"
#include "third_party/zlib/zlib.h"
#include "tool/build/lib/eztls.h"
#include "tool/build/lib/psk.h"
#include "tool/build/runit.h"

#define MAX_WAIT_CONNECT_SECONDS 12
#define INITIAL_CONNECT_TIMEOUT  100000

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
long g_backoff;
char *g_runitd;
jmp_buf g_jmpbuf;
uint16_t g_sshport;
char g_hostname[128];
uint16_t g_runitdport;
volatile bool alarmed;
char g_ssh[PATH_MAX];

int __sys_execve(const char *, char *const[], char *const[]) hidden;

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

dontdiscard char *MakeDeployScript(struct addrinfo *remotenic,
                                   size_t combytes) {
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
  sigset_t chldmask, savemask;
  int sshpid, wstatus, binfd, pipefds[2][2];
  struct sigaction ignore, saveint, savequit;
  ignore.sa_flags = 0;
  ignore.sa_handler = SIG_IGN;
  sigemptyset(&ignore.sa_mask);
  sigaction(SIGINT, &ignore, &saveint);
  sigaction(SIGQUIT, &ignore, &savequit);
  mkdir("o", 0755);
  CHECK_NE(-1, (lock = open(gc(xasprintf("o/lock.%s", g_hostname)),
                            O_RDWR | O_CREAT, 0644)));
  CHECK_NE(-1, fcntl(lock, F_SETLKW, &(struct flock){F_WRLCK}));
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
  sigemptyset(&chldmask);
  sigaddset(&chldmask, SIGCHLD);
  sigprocmask(SIG_BLOCK, &chldmask, &savemask);
  CHECK_NE(-1, pipe2(pipefds[0], O_CLOEXEC));
  CHECK_NE(-1, pipe2(pipefds[1], O_CLOEXEC));
  CHECK_NE(-1, (sshpid = fork()));
  if (!sshpid) {
    sigaction(SIGINT, &(struct sigaction){0}, 0);
    sigaction(SIGQUIT, &(struct sigaction){0}, 0);
    sigprocmask(SIG_SETMASK, &savemask, 0);
    dup2(pipefds[0][0], 0);
    dup2(pipefds[1][1], 1);
    execv(g_ssh, args);
    _exit(127);
  }
  close(pipefds[0][0]);
  close(pipefds[1][1]);
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
  LOGIFNEG1(sigprocmask(SIG_SETMASK, &savemask, NULL));
  if (WIFEXITED(wstatus)) {
    DEBUGF("ssh %s exited with %d", g_hostname, WEXITSTATUS(wstatus));
  } else {
    DEBUGF("ssh %s terminated with %s", g_hostname,
           strsignal(WTERMSIG(wstatus)));
  }
  CHECK(WIFEXITED(wstatus) && !WEXITSTATUS(wstatus), "wstatus=%#x", wstatus);
  LOGIFNEG1(fcntl(lock, F_SETLK, &(struct flock){F_UNLCK}));
  sigaction(SIGINT, &saveint, 0);
  sigaction(SIGQUIT, &savequit, 0);
  close(lock);
}

void Connect(void) {
  const char *ip4;
  int rc, err, expo;
  long double t1, t2;
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
  DEBUGF("connecting to %d.%d.%d.%d port %d", ip4[0], ip4[1], ip4[2], ip4[3],
         ntohs(ai->ai_addr4->sin_port));
  CHECK_NE(-1,
           (g_sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)));
  expo = INITIAL_CONNECT_TIMEOUT;
  t1 = nowl();
  LOGIFNEG1(sigaction(SIGALRM, &(struct sigaction){.sa_handler = OnAlarm}, 0));
Reconnect:
  DEBUGF("connecting to %s (%hhu.%hhu.%hhu.%hhu) to run %s", g_hostname, ip4[0],
         ip4[1], ip4[2], ip4[3], g_prog);
TryAgain:
  alarmed = false;
  LOGIFNEG1(setitimer(
      ITIMER_REAL,
      &(const struct itimerval){{0, 0}, {expo / 1000000, expo % 1000000}},
      NULL));
  rc = connect(g_sock, ai->ai_addr, ai->ai_addrlen);
  err = errno;
  t2 = nowl();
  if (rc == -1) {
    if (err == EINTR) {
      expo *= 1.5;
      if (t2 > t1 + MAX_WAIT_CONNECT_SECONDS) {
        FATALF("timeout connecting to %s (%hhu.%hhu.%hhu.%hhu:%d)", g_hostname,
               ip4[0], ip4[1], ip4[2], ip4[3], ntohs(ai->ai_addr4->sin_port));
        unreachable;
      }
      goto TryAgain;
    }
    if (err == ECONNREFUSED || err == EHOSTUNREACH || err == ECONNRESET) {
      DEBUGF("got %s from %s (%hhu.%hhu.%hhu.%hhu)", strerror(err), g_hostname,
             ip4[0], ip4[1], ip4[2], ip4[3]);
      setitimer(ITIMER_REAL, &(const struct itimerval){0}, 0);
      DeployEphemeralRunItDaemonRemotelyViaSsh(ai);
      if (t2 > t1 + MAX_WAIT_CONNECT_SECONDS) {
        FATALF("timeout connecting to %s (%hhu.%hhu.%hhu.%hhu:%d)", g_hostname,
               ip4[0], ip4[1], ip4[2], ip4[3], ntohs(ai->ai_addr4->sin_port));
        unreachable;
      }
      usleep((expo *= 2));
      goto Reconnect;
    } else {
      FATALF("%s(%s:%hu): %s", "connect", g_hostname, g_runitdport,
             strerror(err));
      unreachable;
    }
  } else {
    DEBUGF("connected to %s", g_hostname);
  }
  setitimer(ITIMER_REAL, &(const struct itimerval){0}, 0);
  freeaddrinfo(ai);
}

bool Send(int *pipes, int pipescount, const void *output, size_t outputsize) {
  bool okall;
  int rc, i, have;
  static bool once;
  static z_stream zs;
  char *zbuf = gc(malloc(PIPE_BUF));
  if (!once) {
    CHECK_EQ(Z_OK, deflateInit2(&zs, 4, Z_DEFLATED, MAX_WBITS, DEF_MEM_LEVEL,
                                Z_DEFAULT_STRATEGY));
    once = true;
  }
  zs.next_in = output;
  zs.avail_in = outputsize;
  okall = true;
  do {
    zs.avail_out = PIPE_BUF;
    zs.next_out = (unsigned char *)zbuf;
    rc = deflate(&zs, Z_SYNC_FLUSH);
    CHECK_NE(Z_STREAM_ERROR, rc);
    have = PIPE_BUF - zs.avail_out;
    for (i = 0; i < pipescount; ++i) {
      rc = write(pipes[i * 2 + 1], zbuf, have);
      if (rc != have) {
        DEBUGF("write(%d, %d) → %d", pipes[i * 2 + 1], have, rc);
        okall = false;
      }
    }
  } while (!zs.avail_out);
  return okall;
}

bool SendRequest(int *pipes, int pipescount) {
  int fd;
  char *p;
  size_t i;
  bool okall;
  ssize_t rc;
  uint32_t crc;
  struct stat st;
  const char *name;
  unsigned char *hdr, *q;
  size_t progsize, namesize, hdrsize;
  unsigned have;
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
  okall = true;
  okall &= Send(pipes, pipescount, hdr, hdrsize);
  okall &= Send(pipes, pipescount, p, progsize);
  CHECK_NE(-1, munmap(p, st.st_size));
  CHECK_NE(-1, close(fd));
  for (i = 0; i < pipescount; ++i) {
    okall &= !close(pipes[i * 2 + 1]);
  }
  return okall;
}

bool RelayRequest(void) {
  int i, rc, have, transferred;
  char *buf = gc(malloc(PIPE_BUF));
  for (transferred = 0;;) {
    rc = read(13, buf, PIPE_BUF);
    CHECK_NE(-1, rc);
    have = rc;
    if (!rc) break;
    transferred += have;
    for (i = 0; i < have; i += rc) {
      rc = mbedtls_ssl_write(&ezssl, buf + i, have - i);
      if (rc <= 0) {
        TlsDie("relay request failed", rc);
      }
    }
  }
  CHECK_NE(0, transferred);
  rc = EzTlsFlush(&ezbio, 0, 0);
  if (rc < 0) {
    TlsDie("relay request failed to flush", rc);
  }
  close(13);
  return true;
}

bool Recv(unsigned char *p, size_t n) {
  size_t i, rc;
  for (i = 0; i < n; i += rc) {
    do {
      rc = mbedtls_ssl_read(&ezssl, p + i, n - i);
    } while (rc == MBEDTLS_ERR_SSL_WANT_READ);
    if (!rc) return false;
    if (rc < 0) {
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

static inline bool IsElf(const char *p, size_t n) {
  return n >= 4 && READ32LE(p) == READ32LE("\177ELF");
}

static inline bool IsMachO(const char *p, size_t n) {
  return n >= 4 && READ32LE(p) == 0xFEEDFACEu + 1;
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
  DEBUGF("connecting to %s port %d", g_hostname, g_runitdport);
  do {
    for (;;) {
      Connect();
      EzFd(g_sock);
      if (!EzHandshake2()) break;
      WARNF("warning: got connection reset in handshake");
      close(g_sock);
    }
  } while ((rc = RelayRequest()) == -1 || (rc = ReadResponse()) == -1);
  return rc;
}

bool IsParallelBuild(void) {
  const char *makeflags;
  return (makeflags = getenv("MAKEFLAGS")) && strstr(makeflags, "-j");
}

bool ShouldRunInParralel(void) {
  return !IsWindows() && IsParallelBuild();
}

int SpawnSubprocesses(int argc, char *argv[]) {
  sigset_t chldmask, savemask;
  struct sigaction ignore, saveint, savequit;
  int i, rc, ws, pid, *pids, *pipes, exitcode;
  char *args[5] = {argv[0], argv[1], argv[2]};
  argc -= 3;
  argv += 3;

  // fork off 𝑛 subprocesses for each host on which we run binary.
  // what's important here is htop in tree mode will report like:
  //
  //     runit.com xnu freebsd netbsd
  //     ├─runit.com xnu
  //     ├─runit.com freebsd
  //     └─runit.com netbsd
  //
  // That way when one hangs, it's easy to know what o/s it is.
  pids = calloc(argc, sizeof(int));
  pipes = calloc(argc, sizeof(int) * 2);
  ignore.sa_flags = 0;
  ignore.sa_handler = SIG_IGN;
  sigemptyset(&ignore.sa_mask);
  sigaction(SIGINT, &ignore, &saveint);
  sigaction(SIGQUIT, &ignore, &savequit);
  sigemptyset(&chldmask);
  sigaddset(&chldmask, SIGCHLD);
  sigprocmask(SIG_BLOCK, &chldmask, &savemask);
  for (i = 0; i < argc; ++i) {
    args[3] = argv[i];
    CHECK_NE(-1, pipe2(pipes + i * 2, O_CLOEXEC));
    CHECK_NE(-1, (pids[i] = vfork()));
    if (!pids[i]) {
      dup2(pipes[i * 2], 13);
      sigaction(SIGINT, &(struct sigaction){0}, 0);
      sigaction(SIGQUIT, &(struct sigaction){0}, 0);
      sigprocmask(SIG_SETMASK, &savemask, 0);
      execve(args[0], args, environ);  // for htop
      _Exit(127);
    }
    close(pipes[i * 2]);  // close reader
  }

  // the smart part is we only do the expensive deflate operation from
  // the main process we assume file descriptor 13 is safely inherited
  static z_stream zs;
  sigaction(SIGPIPE, &ignore, 0);
  exitcode = SendRequest(pipes, argc) ? 0 : 150;

  // now wait for the children to terminate
  for (;;) {
    if ((pid = wait(&ws)) == -1) {
      if (errno == EINTR) continue;
      if (errno == ECHILD) break;
      FATALF("wait failed");
    }
    for (i = 0; i < argc; ++i) {
      if (pids[i] != pid) continue;
      if (WIFEXITED(ws)) {
        if (WEXITSTATUS(ws)) {
          INFOF("%s exited with %d", argv[i], WEXITSTATUS(ws));
        } else {
          DEBUGF("%s exited with %d", argv[i], WEXITSTATUS(ws));
        }
        if (!exitcode) exitcode = WEXITSTATUS(ws);
      } else {
        INFOF("%s terminated with %s", argv[i], strsignal(WTERMSIG(ws)));
        if (!exitcode) exitcode = 128 + WTERMSIG(ws);
      }
      break;
    }
  }
  sigprocmask(SIG_SETMASK, &savemask, 0);
  sigaction(SIGQUIT, &savequit, 0);
  sigaction(SIGINT, &saveint, 0);
  free(pipes);
  free(pids);
  return exitcode;
}

int main(int argc, char *argv[]) {
  ShowCrashReports();
  if (getenv("DEBUG")) {
    __log_level = kLogDebug;
  }
  if (argc > 1 &&
      (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
    ShowUsage(stdout, 0);
    unreachable;
  }
  if (argc < 3) {
    ShowUsage(stderr, EX_USAGE);
    unreachable;
  }
  CheckExists((g_runitd = argv[1]));
  CheckExists((g_prog = argv[2]));
  CHECK_NOTNULL(
      commandv(firstnonnull(getenv("SSH"), "ssh"), g_ssh, sizeof(g_ssh)));
  if (argc == 3) {
    /* hosts list empty */
    return 0;
  } else if (argc == 4) {
    /* TODO(jart): this is broken */
    /* single host */
    SetupPresharedKeySsl(MBEDTLS_SSL_IS_CLIENT, GetRunitPsk());
    g_sshport = 22;
    g_runitdport = RUNITD_PORT;
    return RunOnHost(argv[3]);
  } else {
    /* multiple hosts */
    return SpawnSubprocesses(argc, argv);
  }
}
