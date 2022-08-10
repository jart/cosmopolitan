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
#include "libc/fmt/conv.h"
#include "libc/runtime/gc.internal.h"
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

STATIC_YOINK("zip_uri_support");
STATIC_YOINK("o/" MODE "/tool/net/redbean.com");

char testlib_enable_tmp_setup_teardown_once;
int port;

void SetUpOnce(void) {
  if (IsWindows()) return;
  ssize_t n;
  char buf[1024];
  int fdin, fdout;
  ASSERT_NE(-1, mkdir("bin", 0755));
  ASSERT_NE(-1,
            (fdin = open("/zip/o/" MODE "/tool/net/redbean.com", O_RDONLY)));
  ASSERT_NE(-1, (fdout = creat("bin/redbean.com", 0755)));
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
  EXPECT_NE(-1, connect(fd, &addr, sizeof(addr)));
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
    dup(0);
    close(2);
    open("log", O_CREAT | O_TRUNC | O_WRONLY | O_APPEND, 0644);
    close(pipefds[0]);
    dup2(pipefds[1], 1);
    sigprocmask(SIG_SETMASK, &savemask, NULL);
    execv("bin/redbean.com",
          (char *const[]){"bin/redbean.com", "-vvszp0", "-l127.0.0.1", 0});
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
  if (g_testlib_failed) fputs(gc(xslurp("log", 0)), stderr);
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
    close(2);
    open("log", O_CREAT | O_TRUNC | O_WRONLY | O_APPEND, 0644);
    close(pipefds[0]);
    dup2(pipefds[1], 1);
    sigprocmask(SIG_SETMASK, &savemask, NULL);
    execv("bin/redbean.com",
          (char *const[]){"bin/redbean.com", "-vvszp0", "-l127.0.0.1", 0});
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
  if (g_testlib_failed) fputs(gc(xslurp("log", 0)), stderr);
}
