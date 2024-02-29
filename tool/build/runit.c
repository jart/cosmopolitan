/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "tool/build/runit.h"
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timespec.h"
#include "libc/errno.h"
#include "libc/fmt/libgen.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/serialize.h"
#include "libc/sock/ipclassify.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/itimer.h"
#include "libc/sysv/consts/limits.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/sock.h"
#include "libc/temp.h"
#include "libc/x/xasprintf.h"
#include "net/https/https.h"
#include "third_party/mbedtls/net_sockets.h"
#include "third_party/mbedtls/ssl.h"
#include "third_party/musl/netdb.h"
#include "third_party/zlib/zlib.h"
#include "tool/build/lib/eztls.h"
#include "tool/build/lib/psk.h"

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
 *         o/default/test/libc/mem/qsort_test.com \
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
int connect_latency;
int execute_latency;
int handshake_latency;
char g_hostname[128];
uint16_t g_runitdport;
volatile bool alarmed;

int __sys_execve(const char *, char *const[], char *const[]);

static void OnAlarm(int sig) {
  alarmed = true;
}

wontreturn void ShowUsage(FILE *f, int rc) {
  fprintf(f, "Usage: %s RUNITD PROGRAM HOSTNAME[:RUNITDPORT[:SSHPORT]]...\n",
          program_invocation_name);
  exit(rc);
  __builtin_unreachable();
}

void CheckExists(const char *path) {
  if (!isregularfile(path)) {
    fprintf(stderr, "error: %s: not found or irregular\n", path);
    ShowUsage(stderr, EX_USAGE);
    __builtin_unreachable();
  }
}

void Connect(void) {
  const char *ip4;
  int rc, err, expo;
  struct addrinfo *ai;
  struct timespec deadline;
  if ((rc = getaddrinfo(g_hostname, gc(xasprintf("%hu", g_runitdport)),
                        &kResolvHints, &ai)) != 0) {
    FATALF("%s:%hu: DNS lookup failed: %s", g_hostname, g_runitdport,
           gai_strerror(rc));
    __builtin_unreachable();
  }
  ip4 = (const char *)&((struct sockaddr_in *)ai->ai_addr)->sin_addr;
  DEBUGF("connecting to %d.%d.%d.%d port %d", ip4[0], ip4[1], ip4[2], ip4[3],
         ntohs(((struct sockaddr_in *)ai->ai_addr)->sin_port));
  CHECK_NE(-1,
           (g_sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)));
  expo = INITIAL_CONNECT_TIMEOUT;
  deadline = timespec_add(timespec_real(),
                          timespec_fromseconds(MAX_WAIT_CONNECT_SECONDS));
  LOGIFNEG1(sigaction(SIGALRM, &(struct sigaction){.sa_handler = OnAlarm}, 0));
  DEBUGF("connecting to %s (%hhu.%hhu.%hhu.%hhu) to run %s", g_hostname, ip4[0],
         ip4[1], ip4[2], ip4[3], g_prog);
  struct timespec start = timespec_real();
TryAgain:
  alarmed = false;
  LOGIFNEG1(setitimer(
      ITIMER_REAL,
      &(const struct itimerval){{0, 0}, {expo / 1000000, expo % 1000000}},
      NULL));
  rc = connect(g_sock, ai->ai_addr, ai->ai_addrlen);
  err = errno;
  if (rc == -1) {
    if (err == EINTR) {
      expo *= 1.5;
      if (timespec_cmp(timespec_real(), deadline) >= 0) {
        FATALF("timeout connecting to %s (%hhu.%hhu.%hhu.%hhu:%d)", g_hostname,
               ip4[0], ip4[1], ip4[2], ip4[3],
               ntohs(((struct sockaddr_in *)ai->ai_addr)->sin_port));
        __builtin_unreachable();
      }
      goto TryAgain;
    }
    FATALF("%s(%s:%hu): %s", "connect", g_hostname, g_runitdport,
           strerror(err));
  } else {
    DEBUGF("connected to %s", g_hostname);
  }
  setitimer(ITIMER_REAL, &(const struct itimerval){0}, 0);
  freeaddrinfo(ai);
  connect_latency = timespec_tomicros(timespec_sub(timespec_real(), start));
}

bool Send(int tmpfd, const void *output, size_t outputsize) {
  bool ok;
  char *zbuf;
  size_t zsize;
  int rc, have;
  static bool once;
  static z_stream zs;
  zsize = 32768;
  zbuf = gc(malloc(zsize));
  if (!once) {
    CHECK_EQ(Z_OK, deflateInit2(&zs, 4, Z_DEFLATED, MAX_WBITS, DEF_MEM_LEVEL,
                                Z_DEFAULT_STRATEGY));
    once = true;
  }
  zs.next_in = output;
  zs.avail_in = outputsize;
  ok = true;
  do {
    zs.avail_out = zsize;
    zs.next_out = (unsigned char *)zbuf;
    rc = deflate(&zs, Z_SYNC_FLUSH);
    CHECK_NE(Z_STREAM_ERROR, rc);
    have = zsize - zs.avail_out;
    rc = write(tmpfd, zbuf, have);
    if (rc != have) {
      DEBUGF("write(%d, %d) → %d", tmpfd, have, rc);
      ok = false;
      break;
    }
  } while (!zs.avail_out);
  return ok;
}

bool SendRequest(int tmpfd) {
  int fd;
  char *p;
  bool okall;
  uint32_t crc;
  struct stat st;
  const char *name;
  unsigned char *hdr, *q;
  size_t progsize, namesize, hdrsize;
  CHECK_NE(-1, (fd = open(g_prog, O_RDONLY)));
  CHECK_NE(-1, fstat(fd, &st));
  CHECK_NE(MAP_FAILED, (p = mmap(0, st.st_size, PROT_READ, MAP_SHARED, fd, 0)));
  CHECK_LE((namesize = strlen((name = basename(g_prog)))), PATH_MAX);
  CHECK_LE((progsize = st.st_size), INT_MAX);
  CHECK_NOTNULL((hdr = gc(calloc(1, (hdrsize = 17 + namesize)))));
  crc = crc32_z(0, (unsigned char *)p, st.st_size);
  q = hdr;
  q = WRITE32BE(q, RUNITD_MAGIC);
  *q++ = kRunitExecute;
  q = WRITE32BE(q, namesize);
  q = WRITE32BE(q, progsize);
  q = WRITE32BE(q, crc);
  q = mempcpy(q, name, namesize);
  assert(hdrsize == q - hdr);
  okall = true;
  okall &= Send(tmpfd, hdr, hdrsize);
  okall &= Send(tmpfd, p, progsize);
  CHECK_NE(-1, munmap(p, st.st_size));
  CHECK_NE(-1, close(fd));
  return okall;
}

void RelayRequest(void) {
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
        EzTlsDie("relay request failed", rc);
      }
    }
  }
  CHECK_NE(0, transferred);
  rc = EzTlsFlush(&ezbio, 0, 0);
  if (rc < 0) {
    EzTlsDie("relay request failed to flush", rc);
  }
  close(13);
}

bool Recv(char *p, int n) {
  int i, rc;
  for (i = 0; i < n; i += rc) {
    do rc = mbedtls_ssl_read(&ezssl, p + i, n - i);
    while (rc == MBEDTLS_ERR_SSL_WANT_READ);
    if (!rc) return false;
    if (rc < 0) {
      if (rc == MBEDTLS_ERR_NET_CONN_RESET) {
        EzTlsDie("connection reset", rc);
      } else {
        EzTlsDie("read response failed", rc);
      }
    }
  }
  return true;
}

int ReadResponse(void) {
  int exitcode;
  struct timespec start = timespec_real();
  for (;;) {
    char msg[5];
    if (!Recv(msg, 5)) {
      WARNF("%s didn't report status of %s", g_hostname, g_prog);
      exitcode = 200;
      break;
    }
    if (READ32BE(msg) != RUNITD_MAGIC) {
      WARNF("%s sent corrupted data stream after running %s", g_hostname,
            g_prog);
      exitcode = 201;
      break;
    }
    if (msg[4] == kRunitExit) {
      if (!Recv(msg, 1)) {
      TruncatedMessage:
        WARNF("%s sent truncated message running %s", g_hostname, g_prog);
        exitcode = 202;
        break;
      }
      exitcode = *msg & 255;
      if (exitcode) {
        WARNF("%s says %s exited with %d", g_hostname, g_prog, exitcode);
      } else {
        VERBOSEF("%s says %s exited with %d", g_hostname, g_prog, exitcode);
      }
      mbedtls_ssl_close_notify(&ezssl);
      break;
    } else if (msg[4] == kRunitStdout || msg[4] == kRunitStderr) {
      if (!Recv(msg, 4)) goto TruncatedMessage;
      int n = READ32BE(msg);
      char *s = malloc(n);
      if (!Recv(s, n)) goto TruncatedMessage;
      write(2, s, n);
      free(s);
    } else {
      WARNF("%s sent message with unknown command %d after running %s",
            g_hostname, msg[4], g_prog);
      exitcode = 203;
      break;
    }
  }
  execute_latency = timespec_tomicros(timespec_sub(timespec_real(), start));
  close(g_sock);
  return exitcode;
}

int RunOnHost(char *spec) {
  int err;
  char *p;
  for (p = spec; *p; ++p) {
    if (*p == ':') *p = ' ';
  }
  int got =
      sscanf(spec, "%100s %hu %hu", g_hostname, &g_runitdport, &g_sshport);
  if (got < 1) {
    kprintf("what on earth %#s -> %d\n", spec, got);
    fprintf(stderr, "what on earth %#s -> %d\n", spec, got);
    exit(1);
  }
  if (!strchr(g_hostname, '.')) strcat(g_hostname, ".test.");
  DEBUGF("connecting to %s port %d", g_hostname, g_runitdport);
  for (;;) {
    Connect();
    EzFd(g_sock);
    struct timespec start = timespec_real();
    err = EzHandshake2();
    handshake_latency = timespec_tomicros(timespec_sub(timespec_real(), start));
    if (!err) break;
    WARNF("handshake with %s:%d failed -0x%04x (%s)",  //
          g_hostname, g_runitdport, err, GetTlsError(err));
    close(g_sock);
    return 1;
  }
  RelayRequest();
  int rc = ReadResponse();
  kprintf("%s on %-16s %'8ld µs %'8ld µs %'11d µs\n", basename(g_prog),
          g_hostname, connect_latency, handshake_latency, execute_latency);
  return rc;
}

bool IsParallelBuild(void) {
  const char *makeflags;
  return (makeflags = getenv("MAKEFLAGS")) && strstr(makeflags, "-j");
}

bool ShouldRunInParallel(void) {
  return !IsWindows() && IsParallelBuild();
}

int SpawnSubprocesses(int argc, char *argv[]) {
  sigset_t chldmask, savemask;
  int i, ws, pid, *pids, exitcode;
  struct sigaction ignore, saveint, savequit;
  char *args[5] = {argv[0], argv[1], argv[2]};

  // create compressed network request ahead of time
  const char *tmpdir = firstnonnull(getenv("TMPDIR"), "/tmp");
  char *tpath = gc(xasprintf("%s/runit.XXXXXX", tmpdir));
  int tmpfd = mkstemp(tpath);
  CHECK_NE(-1, tmpfd);
  CHECK(SendRequest(tmpfd));
  CHECK_NE(-1, close(tmpfd));

  // fork off 𝑛 subprocesses for each host on which we run binary.
  // what's important here is htop in tree mode will report like:
  //
  //     runit.com xnu freebsd netbsd
  //     ├─runit.com xnu
  //     ├─runit.com freebsd
  //     └─runit.com netbsd
  //
  // That way when one hangs, it's easy to know what o/s it is.
  argc -= 3;
  argv += 3;
  exitcode = 0;
  pids = calloc(argc, sizeof(int));
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
    CHECK_NE(-1, (pids[i] = vfork()));
    if (!pids[i]) {
      dup2(open(tpath, O_RDONLY | O_CLOEXEC), 13);
      sigaction(SIGINT, &(struct sigaction){0}, 0);
      sigaction(SIGQUIT, &(struct sigaction){0}, 0);
      sigprocmask(SIG_SETMASK, &savemask, 0);
      execve(args[0], args, environ);  // for htop
      _Exit(127);
    }
  }

  // wait for children to terminate
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
  unlink(tpath);
  sigprocmask(SIG_SETMASK, &savemask, 0);
  sigaction(SIGQUIT, &savequit, 0);
  sigaction(SIGINT, &saveint, 0);
  free(pids);
  return exitcode;
}

int main(int argc, char *argv[]) {
  ShowCrashReports();
  signal(SIGPIPE, SIG_IGN);
  // mbedtls_debug_threshold = 3;
  if (getenv("DEBUG")) {
    __log_level = kLogDebug;
  }
  if (argc > 1 &&
      (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
    ShowUsage(stdout, 0);
    __builtin_unreachable();
  }
  if (argc < 3) {
    ShowUsage(stderr, EX_USAGE);
    __builtin_unreachable();
  }
  CheckExists((g_runitd = argv[1]));
  CheckExists((g_prog = argv[2]));
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
