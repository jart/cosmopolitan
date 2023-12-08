/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/serialize.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/mem/mem.h"
#include "libc/mem/sortedints.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/timer.h"
#include "libc/time/struct/tm.h"
#include "net/http/http.h"
#include "net/http/ip.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/musl/passwd.h"

#define LOG(FMT, ...)                                                \
  kprintf("%s %s:%d] " FMT "\n", GetTimestamp(), __FILE__, __LINE__, \
          ##__VA_ARGS__)

#define DEFAULT_CHAIN    "PREROUTING"
#define DEFAULT_LOGNAME  "/var/log/blackhole.log"
#define DEFAULT_PIDNAME  "/var/run/blackhole.pid"
#define DEFAULT_SOCKNAME "/var/run/blackhole.sock"
#define GETOPTS          "C:L:S:P:M:G:W:dh"
#define USAGE \
  "\
Usage: blackholed [-hdLPSMGW]\n\
  -h            help\n\
  -d            daemonize\n\
  -C CHAIN      change iptables chain to insert rules into\n\
  -W IP         whitelist ip address\n\
  -L PATH       log file name (default: " DEFAULT_LOGNAME ")\n\
  -P PATH       pid file name (default: " DEFAULT_PIDNAME ")\n\
  -S PATH       socket file name (default: " DEFAULT_SOCKNAME ")\n\
  -M MODE       socket mode bits (default: 0777)\n\
  -G GROUP      socket group name or gid (default: n/a)\n\
  --assimilate  change executable header to native format\n\
  --ftrace      function call tracing\n\
  --strace      system call tracing\n\
\n\
Usage:\n\
  sudo blackholed -d   # run daemon\n\
  blackhole 1.2.3.4    # anyone can securely ban ips\n\
\n\
Protocol:\n\
  Send a 4 byte datagram to the unix socket file containing\n\
  the IPv4 address you want banned encoded using big endian\n\
  a.k.a. network byte order. We ignore these ips: 0.0.0.0/8\n\
  and 127.0.0.0/8 so sending 0 to the socket is a good test\n"

#define LINUX_DOCS \
  "\n\
Linux Requirements:\n\
  sudo modprobe ip_tables\n\
  sudo echo ip_tables >>/etc/modules\n\
\n\
Administration Notes:\n\
  This program inserts IP bans into the prerouting chain in iptables raw\n\
  so that the kernel won't track the TCP connections of threat actors.\n\
  If you restart this program, then you should run\n\
    sudo iptables -t raw -F \n\
  to clear the IP blocks. It's a good idea to have a cron job\n\
  restart this daemon and clear the raw table daily. Use the\n\
    sudo iptables -t raw -L -vn\n\
  command to list the IP addresses that have been blocked.\n\
  If -C is specified, a chain needs to be created on every system startup\n\
  with the following commands:\n\
    sudo iptables -t raw -N blackholed\n\
    sudo iptables -t raw -A blackholed -j RETURN\n\
    sudo iptables -t raw -I PREROUTING -j blackholed\n\
\n"

#define BSD_DOCS \
  "\n\
BSD Requirements:\n\
  kldload pf\n\
  echo 'table <badhosts> persist' >>/etc/pf.conf\n\
  echo 'block on em0 from <badhosts> to any' >>/etc/pf.conf\n\
  echo 'pf_enable=\"YES\"' >>/etc/rc.conf\n\
  echo 'pf_rules=\"/etc/pf.conf\"' >>/etc/rc.conf\n\
  /etc/rc.d/pf start\n\
  pfctl -t badhosts -T add 1.2.3.4\n\
  pfctl -t badhosts -T show\n\
\n\
Administration Notes:\n\
  If you restart this program, then you should run\n\
    pfctl -t badhosts -T flush\n\
  to clear the IP blocks. It's a good idea to have a cron job\n\
  restart this daemon and clear the raw table daily. Use the\n\
    pfctl -t badhosts -T show\n\
  command to list the IP addresses that have been blocked.\n\
\n\
"

int g_logfd;
int g_sockmode;
bool g_daemonize;
uint32_t *g_myips;
const char *g_chain;
const char *g_group;
const char *g_pfctl;
const char *g_logname;
const char *g_pidname;
const char *g_sockname;
const char *g_iptables;
sig_atomic_t g_shutdown;
struct SortedInts g_blocked;
struct SortedInts g_whitelisted;

static wontreturn void ShowUsage(int fd, int rc) {
  write(fd, USAGE, sizeof(USAGE) - 1);
  if (IsLinux()) write(fd, LINUX_DOCS, sizeof(LINUX_DOCS) - 1);
  if (IsBsd()) write(fd, BSD_DOCS, sizeof(BSD_DOCS) - 1);
  _Exit(rc);
}

char *GetTimestamp(void) {
  struct timespec ts;
  static struct tm tm;
  static int64_t last;
  static char str[27];
  clock_gettime(0, &ts);
  if (ts.tv_sec != last) {
    localtime_r(&ts.tv_sec, &tm);
    last = ts.tv_sec;
  }
  iso8601us(str, &tm, ts.tv_nsec);
  return str;
}

void GetOpts(int argc, char *argv[]) {
  int opt;
  int64_t ip;
  g_sockmode = 0777;
  g_chain = DEFAULT_CHAIN;
  g_pidname = DEFAULT_PIDNAME;
  g_logname = DEFAULT_LOGNAME;
  g_sockname = DEFAULT_SOCKNAME;
  while ((opt = getopt(argc, argv, GETOPTS)) != -1) {
    switch (opt) {
      case 'd':
        g_daemonize = true;
        break;
      case 'C':
        g_chain = optarg;
        break;
      case 'S':
        g_sockname = optarg;
        break;
      case 'L':
        g_logname = emptytonull(optarg);
        break;
      case 'P':
        g_pidname = emptytonull(optarg);
        break;
      case 'G':
        g_group = emptytonull(optarg);
        break;
      case 'M':
        g_sockmode = strtol(optarg, 0, 8) & 0777;
        break;
      case 'W':
        if ((ip = ParseIp(optarg, -1)) != -1) {
          if (InsertInt(&g_whitelisted, ip, true)) {
            LOG("whitelisted %s", optarg);
          }
        } else {
          kprintf("error: could not parse -W %#s IP address\n", optarg);
          _Exit(1);
        }
        break;
      case 'h':
        ShowUsage(1, 0);
      default:
        ShowUsage(2, 64);
    }
  }
}

void OnTerm(int sig) {
  char tmp[21];
  LOG("got %s", strsignal_r(sig, tmp));
  g_shutdown = sig;
}

char *FormatIp(uint32_t ip) {
  static char ipbuf[16];
  ksnprintf(ipbuf, sizeof(ipbuf), "%hhu.%hhu.%hhu.%hhu", ip >> 24, ip >> 16,
            ip >> 8, ip);
  return ipbuf;
}

void BlockIp(uint32_t ip) {
  if (!vfork()) {
    if (g_iptables) {
      execve(g_iptables,
             (char *const[]){
                 "iptables",             //
                 "-t", "raw",            //
                 "-I", (char *)g_chain,  //
                 "-s", FormatIp(ip),     //
                 "-j", "DROP",           //
                 0,                      //
             },
             (char *const[]){0});
    } else if (g_pfctl) {
      execve(g_pfctl,
             (char *const[]){
                 "pfctl",           //
                 "-t", "badhosts",  //
                 "-T", "add",       //
                 FormatIp(ip),      //
                 0,                 //
             },
             (char *const[]){0});
    }
    _Exit(127);
  }
}

void RequireRoot(void) {
  if (geteuid()) {
    kprintf("error: need root privileges\n");
    ShowUsage(2, 2);
  }
}

void ListenForTerm(void) {
  struct sigaction sa = {.sa_handler = OnTerm};
  npassert(!sigaction(SIGTERM, &sa, 0));
  npassert(!sigaction(SIGHUP, &sa, 0));
  npassert(!sigaction(SIGINT, &sa, 0));
}

void AutomaticallyHarvestZombies(void) {
  struct sigaction sa = {.sa_handler = SIG_IGN, .sa_flags = SA_NOCLDWAIT};
  npassert(!sigaction(SIGCHLD, &sa, 0));
}

void FindFirewall(void) {
  if (!access("/sbin/iptables", X_OK)) {
    g_iptables = "/sbin/iptables";
  } else if (!access("/usr/sbin/iptables", X_OK)) {
    g_iptables = "/usr/sbin/iptables";
  } else if (!access("/sbin/pfctl", X_OK)) {
    g_pfctl = "/sbin/pfctl";
  } else {
    kprintf("error: could not find `iptables` or `pfctl` command\n");
    ShowUsage(2, 3);
  }
  errno = 0;
}

void OpenLog(void) {
  if (!g_logname) return;
  if (!g_daemonize) return;
  if ((g_logfd = open(g_logname, O_WRONLY | O_APPEND | O_CREAT, 0644)) == -1) {
    kprintf("error: open(%#s) failed: %s\n", g_logname, strerror(errno));
    ShowUsage(2, 5);
  }
}

void Daemonize(void) {
  if (g_daemonize && daemon(false, false)) {
    kprintf("error: daemon() failed: %s\n", strerror(errno));
    ShowUsage(2, 4);
  }
}

void UseLog(void) {
  if (g_logfd > 0) {
    npassert(dup2(g_logfd, 2) == 2);
    if (g_logfd != 2) {
      npassert(!close(g_logfd));
    }
  }
}

void UninterruptibleSleep(int ms) {
  struct timespec ts = timespec_add(timespec_real(), timespec_frommillis(ms));
  while (clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &ts, 0)) errno = 0;
}

void Unlink(const char *path) {
  if (!path) return;
  if (!unlink(path)) {
    LOG("deleted %s", path);
  } else {
    if (errno != ENOENT) {
      LOG("error: unlink(%#s) failed: %s", path, strerror(errno));
    }
    errno = 0;
  }
}

void WritePid(void) {
  ssize_t rc;
  int fd, pid;
  char buf[12] = {0};
  if (!g_pidname) return;
  if ((fd = open(g_pidname, O_RDWR | O_CREAT, 0644)) == -1) {
    LOG("error: open(%#s) failed: %s", g_pidname, strerror(errno));
    _Exit(4);
  }
  npassert((rc = pread(fd, buf, 11, 0)) != -1);
  if (rc) {
    pid = atoi(buf);
    LOG("killing old blackholed process %d", pid);
    if (!kill(pid, SIGTERM)) {
      UninterruptibleSleep(100);
      if (kill(pid, SIGKILL)) {
        if (errno != ESRCH) {
          LOG("kill -KILL %s failed: %s", pid, strerror(errno));
        }
        errno = 0;
      }
    } else {
      if (errno != ESRCH) {
        LOG("kill -TERM %d failed: %s", pid, strerror(errno));
      }
      errno = 0;
    }
  }
  FormatInt32(buf, getpid());
  npassert(!ftruncate(fd, 0));
  npassert((rc = pwrite(fd, buf, strlen(buf), 0)) == strlen(buf));
  npassert(!close(fd));
}

bool IsMyIp(uint32_t ip) {
  uint32_t *p;
  for (p = g_myips; *p; ++p) {
    if (ip == *p && !IsTestnetIp(ip)) {
      return true;
    }
  }
  return false;
}

int main(int argc, char *argv[]) {

  if (closefrom(3))
    for (int i = 3; i < 256; ++i)  //
      close(i);

  GetOpts(argc, argv);
  RequireRoot();
  FindFirewall();
  OpenLog();
  Daemonize();
  UseLog();
  WritePid();
  Unlink(g_sockname);

  if (!(g_myips = GetHostIps())) {
    LOG("failed to get host network interface addresses: %s", strerror(errno));
  }

  int server;
  struct sockaddr_un addr = {AF_UNIX};
  strlcpy(addr.sun_path, g_sockname, sizeof(addr.sun_path));
  if ((server = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
    LOG("error: socket(AF_UNIX) failed: %s", strerror(errno));
    _Exit(3);
  }
  if (bind(server, (struct sockaddr *)&addr, sizeof(addr))) {
    LOG("error: bind(%s) failed: %s", g_sockname, strerror(errno));
    _Exit(4);
  }
  if (chmod(g_sockname, g_sockmode)) {
    LOG("error: chmod(%s, %o) failed: %s", g_sockname, g_sockmode,
        strerror(errno));
    _Exit(5);
  }
  if (g_group) {
    int gid;
    struct group *g;
    if (isdigit(*g_group)) {
      gid = atoi(g_group);
    } else if ((g = getgrnam(g_group))) {
      gid = g->gr_gid;
    } else {
      LOG("error: group %s not found: %s", g_group, strerror(errno));
      _Exit(6);
    }
    if (chown(g_sockname, -1, gid)) {
      LOG("error: chmod(%s, -1, %o) failed: %s", g_sockname, g_sockmode,
          strerror(errno));
      _Exit(7);
    }
  }

  AutomaticallyHarvestZombies();
  ListenForTerm();

  while (!g_shutdown) {
    ssize_t rc;
    uint32_t ip;
    char msg[16];

    if (!(rc = read(server, msg, sizeof(msg)))) {
      LOG("error: impossible eof", strerror(errno));
      _Exit(6);
    } else if (rc == -1) {
      if (errno == EINTR) {
        errno = 0;
        continue;
      }
      LOG("error: read failed: %s", strerror(errno));
      continue;
    } else if (rc != 4) {
      LOG("error: read unexpected size of %ld: %s", rc, strerror(errno));
      continue;
    }

    BLOCK_SIGNALS;

    if ((ip = READ32BE(msg))) {
      if (IsMyIp(ip) ||                       // nics
          ContainsInt(&g_whitelisted, ip) ||  // protected
          (ip & 0xff000000) == 0x00000000 ||  // 0.0.0.0/8
          (ip & 0xff000000) == 0x7f000000) {  // 127.0.0.0/8
        LOG("won't block %s", FormatIp(ip));
      } else if (InsertInt(&g_blocked, ip, true)) {
        BlockIp(ip);
        LOG("blocked %s", FormatIp(ip));
      } else {
        LOG("already blocked %s", FormatIp(ip));
      }
    }

    ALLOW_SIGNALS;
  }

  if (g_shutdown == SIGINT ||  //
      g_shutdown == SIGHUP) {
    Unlink(g_sockname);
    Unlink(g_pidname);
  }
}
