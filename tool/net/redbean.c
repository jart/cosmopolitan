/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/conv/conv.h"
#include "libc/conv/itoa.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/inaddr.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/shut.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "net/http/http.h"
#include "third_party/getopt/getopt.h"

#define STPCPY(p, s) mempcpy(p, s, strlen(s))

bool daemonize;
bool terminated;
int server, client;
const int yes = true;
char serverdate[128];
struct HttpRequest req;
uint32_t clientaddrsize;
struct sockaddr_in serveraddr;
struct sockaddr_in clientaddr;
char inbuf[PAGESIZE] aligned(PAGESIZE);
char outbuf[PAGESIZE] aligned(PAGESIZE);
char serveraddrstr[32], clientaddrstr[32];

void OnTerminate(void) {
  terminated = true;
}

noreturn void ShowUsage(FILE *f, int rc) {
  fprintf(f, "%s: %s %s\n", "Usage", program_invocation_name,
          "[-?drv] [-l LISTENIP] [-p PORT] [-t TIMEOUTMS]");
  exit(rc);
}

char *DescribeAddress(char buf[32], const struct sockaddr_in *addr) {
  char ip4buf[16];
  sprintf(buf, "%s:%hu",
          inet_ntop(addr->sin_family, &addr->sin_addr.s_addr, ip4buf,
                    sizeof(ip4buf)),
          ntohs(addr->sin_port));
  return buf;
}

void GetOpts(int argc, char *argv[]) {
  int opt;
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(8080);
  serveraddr.sin_addr.s_addr = INADDR_ANY;
  while ((opt = getopt(argc, argv, "?dvl:p:w:")) != -1) {
    switch (opt) {
      case 'd':
        daemonize = true;
        break;
      case 'v':
        g_loglevel++;
        break;
      case 'p':
        CHECK_NE(0xFFFF, (serveraddr.sin_port = htons(parseport(optarg))));
        break;
      case 'l':
        CHECK_EQ(1, inet_pton(AF_INET, optarg, &serveraddr.sin_addr));
        break;
      case '?':
        ShowUsage(stdout, EXIT_SUCCESS);
      default:
        ShowUsage(stderr, EX_USAGE);
    }
  }
}

void GenerateHttpDate(char buf[128]) {
  int64_t now;
  struct tm tm;
  time(&now);
  gmtime_r(&now, &tm);
  strftime(buf, 128, "%a, %d %b %Y %H:%M:%S GMT", &tm);
}

int CompareHeaderValue(int h, const char *s) {
  return strncmp(s, inbuf + req.headers[h].a,
                 req.headers[h].b - req.headers[h].a);
}

ssize_t EasyWrite(int fd, const void *data, size_t size) {
  char *p;
  ssize_t rc;
  size_t wrote, n;
  p = data;
  n = size;
  do {
    if ((rc = write(fd, p, n)) != -1) {
      wrote = rc;
      p += wrote;
      n -= wrote;
    } else {
      return -1;
    }
  } while (n);
  return 0;
}

void SendResponse(const char *data, size_t size) {
  ssize_t rc;
  if ((rc = EasyWrite(client, data, size)) == -1) {
    LOGF("send error %s %s", clientaddrstr, strerror(errno));
  }
}

void HandleRequest(void) {
  int contentlength;
  char *p, ibuf[21];
  const char *content = "\
<h1>Hello World</h1>\r\n\
";
  CHECK_NE(-1, shutdown(client, SHUT_RD));
  contentlength = strlen(content);
  p = outbuf;
  p = STPCPY(p, "HTTP/1.1 200 OK\r\n\
Connection: close\r\n\
Content-Type: text/html; charset=utf-8\r\n\
Date: ");
  p = stpcpy(p, serverdate);
  p = STPCPY(p, "\r\n\
Content-Length: ");
  p = mempcpy(p, ibuf, int64toarray_radix10(contentlength, ibuf));
  p = STPCPY(p, "\r\n\
\r\n");
  p = mempcpy(p, content, contentlength);
  SendResponse(outbuf, p - outbuf);
}

void ProcessRequest(void) {
  size_t got;
  ssize_t rc;
  clientaddrsize = sizeof(clientaddr);
  CHECK_NE(-1, (client = accept4(server, &clientaddr, &clientaddrsize,
                                 SOCK_CLOEXEC)));
  VERBOSEF("accepted %s", DescribeAddress(clientaddrstr, &clientaddr));
  if ((rc = read(client, inbuf, sizeof(inbuf))) != -1) {
    if ((got = rc)) {
      if (ParseHttpRequest(&req, inbuf, got) != -1) {
        HandleRequest();
      } else {
        LOGF("parse error %s %s", DescribeAddress(clientaddrstr, &clientaddr),
             strerror(errno));
      }
    }
  } else {
    LOGF("recv error %s %s", DescribeAddress(clientaddrstr, &clientaddr),
         strerror(errno));
  }
  VERBOSEF("closing %s", DescribeAddress(clientaddrstr, &clientaddr));
  LOGIFNEG1(close(client));
}

int main(int argc, char *argv[]) {
  showcrashreports();
  GetOpts(argc, argv);
  GenerateHttpDate(serverdate);
  xsigaction(SIGINT, OnTerminate, 0, 0, 0);
  xsigaction(SIGTERM, OnTerminate, 0, 0, 0);
  CHECK_NE(-1, (server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)));
  LOGIFNEG1(setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)));
  LOGIFNEG1(setsockopt(server, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes)));
  CHECK_NE(-1, bind(server, &serveraddr, sizeof(serveraddr)));
  CHECK_NE(-1, listen(server, 10));
  LOGIFNEG1(fcntl(server, F_SETFD, FD_CLOEXEC));
  DescribeAddress(serveraddrstr, &serveraddr);
  LOGF("listening on tcp %s", serveraddrstr);
  while (!terminated) {
    ProcessRequest();
  }
  LOGIFNEG1(close(server));
  return 0;
}
