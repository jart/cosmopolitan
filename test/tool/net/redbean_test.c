/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/struct/sigset.h"
#include "libc/dce.h"
#include "libc/fmt/conv.h"
#include "libc/mem/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/goodsocket.internal.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/inaddr.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/shut.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/tcp.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "third_party/regex/regex.h"
#ifdef __x86_64__

__static_yoink("zipos");
__static_yoink("o/" MODE "/test/tool/net/redbean-tester.com");

int port;

void SetUpOnce(void) {
  ssize_t n;
  char buf[1024];
  int fdin, fdout;
  if (IsWindows()) return;
  testlib_enable_tmp_setup_teardown_once();
  ASSERT_NE(-1, mkdir("bin", 0755));
  ASSERT_NE(-1, (fdin = open("/zip/o/" MODE "/test/tool/net/redbean-tester.com",
                             O_RDONLY)));
  ASSERT_NE(-1, (fdout = creat("bin/redbean-tester.com", 0755)));
  for (;;) {
    ASSERT_NE(-1, (n = read(fdin, buf, sizeof(buf))));
    if (!n) break;
    ASSERT_EQ(n, write(fdout, buf, n));
  }
  close(fdout);
  close(fdin);
}

int Socket(void) {
  return GoodSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, false, 0);
}

char *SendHttpRequest(const char *s) {
  int fd;
  char *p;
  size_t n;
  ssize_t rc;
  struct sockaddr_in addr = {AF_INET, htons(port), {htonl(INADDR_LOOPBACK)}};
  EXPECT_NE(-1, (fd = Socket()));
  EXPECT_NE(-1, connect(fd, (struct sockaddr *)&addr, sizeof(addr)));
  n = strlen(s);
  EXPECT_EQ(n, write(fd, s, n));
  shutdown(fd, SHUT_WR);
  for (p = 0, n = 0;; n += rc) {
    p = xrealloc(p, n + 512);
    EXPECT_NE(-1, (rc = read(fd, p + n, 512)));
    if (rc <= 0) break;
  }
  p = xrealloc(p, n + 1);
  p[n] = 0;
  close(fd);
  return p;
}

bool Matches(const char *regex, const char *str) {
  bool r;
  regex_t re;
  printf("%s\n", str);
  EXPECT_EQ(REG_OK, regcomp(&re, regex, 0));
  r = regexec(&re, str, 0, 0, 0) == REG_OK;
  regfree(&re);
  return r;
}

TEST(redbean, testOptions) {
  if (IsWindows()) return;
  char portbuf[16];
  int pid, pipefds[2];
  sigset_t chldmask, savemask;
  sigaddset(&chldmask, SIGCHLD);
  EXPECT_NE(-1, sigprocmask(SIG_BLOCK, &chldmask, &savemask));
  ASSERT_NE(-1, pipe(pipefds));
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    setpgrp();
    close(0);
    open("/dev/null", O_RDWR);
    close(1);
    close(pipefds[0]);
    dup2(pipefds[1], 1);
    sigprocmask(SIG_SETMASK, &savemask, NULL);
    execv("bin/redbean-tester.com",
          (char *const[]){"bin/redbean-tester.com", "-vvszXp0", "-l127.0.0.1",
                          __strace > 0 ? "--strace" : 0, 0});
    _exit(127);
  }
  EXPECT_NE(-1, close(pipefds[1]));
  EXPECT_NE(-1, read(pipefds[0], portbuf, sizeof(portbuf)));
  port = atoi(portbuf);
  EXPECT_TRUE(Matches("HTTP/1\\.1 200 OK\r\n"
                      "Accept: \\*/\\*\r\n"
                      "Accept-Charset: utf-8,ISO-8859-1;q=0\\.7,\\*;q=0\\.5\r\n"
                      "Allow: GET, HEAD, POST, PUT, DELETE, OPTIONS\r\n"
                      "Date: .*\r\n"
                      "Server: redbean/.*\r\n"
                      "Content-Length: 0\r\n"
                      "\r\n",
                      gc(SendHttpRequest("OPTIONS * HTTP/1.1\n\n"))));
  EXPECT_EQ(0, close(pipefds[0]));
  EXPECT_NE(-1, kill(pid, SIGTERM));
  EXPECT_NE(-1, wait(0));
  EXPECT_NE(-1, sigprocmask(SIG_SETMASK, &savemask, 0));
}

TEST(redbean, testPipeline) {
  if (IsWindows()) return;
  char portbuf[16];
  int pid, pipefds[2];
  sigset_t chldmask, savemask;
  sigaddset(&chldmask, SIGCHLD);
  EXPECT_NE(-1, sigprocmask(SIG_BLOCK, &chldmask, &savemask));
  ASSERT_NE(-1, pipe(pipefds));
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    setpgrp();
    close(0);
    open("/dev/null", O_RDWR);
    close(pipefds[0]);
    dup2(pipefds[1], 1);
    sigprocmask(SIG_SETMASK, &savemask, NULL);
    execv("bin/redbean-tester.com",
          (char *const[]){"bin/redbean-tester.com", "-vvszXp0", "-l127.0.0.1",
                          __strace > 0 ? "--strace" : 0, 0});
    _exit(127);
  }
  EXPECT_NE(-1, close(pipefds[1]));
  EXPECT_NE(-1, read(pipefds[0], portbuf, sizeof(portbuf)));
  port = atoi(portbuf);
  EXPECT_TRUE(Matches("HTTP/1\\.1 200 OK\r\n"
                      "Accept: \\*/\\*\r\n"
                      "Accept-Charset: utf-8,ISO-8859-1;q=0\\.7,\\*;q=0\\.5\r\n"
                      "Allow: GET, HEAD, POST, PUT, DELETE, OPTIONS\r\n"
                      "Date: .*\r\n"
                      "Server: redbean/.*\r\n"
                      "Content-Length: 0\r\n"
                      "\r\n"
                      "HTTP/1\\.1 200 OK\r\n"
                      "Accept: \\*/\\*\r\n"
                      "Accept-Charset: utf-8,ISO-8859-1;q=0\\.7,\\*;q=0\\.5\r\n"
                      "Allow: GET, HEAD, POST, PUT, DELETE, OPTIONS\r\n"
                      "Date: .*\r\n"
                      "Server: redbean/.*\r\n"
                      "Content-Length: 0\r\n"
                      "\r\n",
                      gc(SendHttpRequest("OPTIONS * HTTP/1.1\n\n"
                                         "OPTIONS * HTTP/1.1\n\n"))));
  EXPECT_EQ(0, close(pipefds[0]));
  EXPECT_NE(-1, kill(pid, SIGTERM));
  EXPECT_NE(-1, wait(0));
  EXPECT_NE(-1, sigprocmask(SIG_SETMASK, &savemask, 0));
}

TEST(redbean, testContentRange) {
  if (IsWindows()) return;
  char portbuf[16];
  int pid, pipefds[2];
  sigset_t chldmask, savemask;
  sigaddset(&chldmask, SIGCHLD);
  EXPECT_NE(-1, sigprocmask(SIG_BLOCK, &chldmask, &savemask));
  ASSERT_NE(-1, pipe(pipefds));
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    setpgrp();
    close(0);
    open("/dev/null", O_RDWR);
    close(pipefds[0]);
    dup2(pipefds[1], 1);
    sigprocmask(SIG_SETMASK, &savemask, NULL);
    execv("bin/redbean-tester.com",
          (char *const[]){"bin/redbean-tester.com", "-vvszXp0", "-l127.0.0.1",
                          __strace > 0 ? "--strace" : 0, 0});
    _exit(127);
  }
  EXPECT_NE(-1, close(pipefds[1]));
  EXPECT_NE(-1, read(pipefds[0], portbuf, sizeof(portbuf)));
  port = atoi(portbuf);

  EXPECT_TRUE(Matches("\
HTTP/1.1 206 Partial Content\r\n\
Content-Range: bytes 18-21/52\r\n\
Content-Type: text/plain; charset=utf-8\r\n\
Vary: Accept-Encoding\r\n\
Last-Modified: .*\r\n\
Accept-Ranges: bytes\r\n\
X-Content-Type-Options: nosniff\r\n\
Date: .*\r\n\
Server: redbean/.*\r\n\
Content-Length: 4\r\n\
\r\n\
J\n\
K\n",
                      gc(SendHttpRequest("GET /seekable.txt HTTP/1.1\r\n"
                                         "Range: bytes=18-21\r\n"
                                         "\r\n"))));

  EXPECT_TRUE(Matches("\
HTTP/1.1 416 Range Not Satisfiable\r\n\
Content-Range: bytes \\*/52\r\n\
Content-Type: text/plain; charset=utf-8\r\n\
Vary: Accept-Encoding\r\n\
Last-Modified: .*\r\n\
Accept-Ranges: bytes\r\n\
X-Content-Type-Options: nosniff\r\n\
Date: .*\r\n\
Server: redbean/.*\r\n\
Content-Length: 0\r\n\
\r\n",
                      gc(SendHttpRequest("GET /seekable.txt HTTP/1.1\r\n"
                                         "Range: bytes=-18-21\r\n"
                                         "\r\n"))));

  EXPECT_TRUE(Matches("\
HTTP/1.1 206 Partial Content\r\n\
Content-Range: bytes 18-51/52\r\n\
Content-Type: text/plain; charset=utf-8\r\n\
Vary: Accept-Encoding\r\n\
Last-Modified: .*\r\n\
Accept-Ranges: bytes\r\n\
X-Content-Type-Options: nosniff\r\n\
Date: .*\r\n\
Server: redbean/2.2.0\r\n\
Content-Length: 34\r\n\
\r\n\
J\n\
K\n\
L\n\
M\n\
N\n\
O\n\
P\n\
Q\n\
R\n\
S\n\
T\n\
U\n\
V\n\
W\n\
X\n\
Y\n\
Z\n",
                      gc(SendHttpRequest("GET /seekable.txt HTTP/1.1\r\n"
                                         "Range: bytes=18-60\r\n"
                                         "\r\n"))));

  EXPECT_EQ(0, close(pipefds[0]));
  EXPECT_NE(-1, kill(pid, SIGTERM));
  EXPECT_NE(-1, wait(0));
  EXPECT_NE(-1, sigprocmask(SIG_SETMASK, &savemask, 0));
}

#endif /* __x86_64__ */
