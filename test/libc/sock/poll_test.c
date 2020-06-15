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
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/macros.h"
#include "libc/runtime/gc.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/limits.h"
#include "libc/sysv/consts/poll.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "tool/decode/lib/flagger.h"
#include "tool/decode/lib/pollnames.h"

#if 0 /* todo(jart): fix me */

#define POLL(FDS, TIMEOUT) \
  poll(((struct pollfd[])FDS), ARRAYLEN(((struct pollfd[])FDS)), TIMOUT)

nodiscard char *FormatPollFd(struct pollfd *pol) {
  return xasprintf("fd:%d revents:%s", pol->fd,
                   gc(recreateflags(kPollNames, pol->revents)));
}

TEST(poll, testNegativeOneFd_completelyIgnored) {
  struct pollfd fds[] = {{-1}};
  EXPECT_EQ(0, poll(fds, ARRAYLEN(fds), 0));
  EXPECT_STREQ("fd:-1 revents:0", gc(FormatPollFd(&fds[0])));
}

TEST(poll, demo) {
  int rw[2];
  char buf[2] = "hi";
  ASSERT_NE(-1, pipe(rw));
  ASSERT_EQ(2, write(rw[1], buf, sizeof(buf))); /* produce */
  {
    struct pollfd fds[] = {{rw[0], POLLIN}, {rw[1], POLLOUT}};
    EXPECT_EQ(2, poll(fds, ARRAYLEN(fds), 0));
    system(gc(xasprintf("ls -l /proc/%d/fd", getpid())));
    EXPECT_STREQ("fd:3 revents:POLLIN", gc(FormatPollFd(fds + 0)));
    EXPECT_STREQ("fd:4 revents:POLLOUT", gc(FormatPollFd(&fds[1])));
  }
  ASSERT_EQ(2, read(rw[0], buf, sizeof(buf))); /* consume */
  {
    struct pollfd fds[] = {{rw[0], POLLIN}, {rw[1], POLLOUT}};
    EXPECT_EQ(1, poll(fds, ARRAYLEN(fds), 0));
    EXPECT_STREQ("fd:3 revents:0", gc(FormatPollFd(&fds[0])));
    EXPECT_STREQ("fd:4 revents:POLLOUT", gc(FormatPollFd(&fds[1])));
  }
  ASSERT_NE(-1, close(rw[1])); /* close producer */
  {
    struct pollfd fds[] = {{rw[0], POLLIN}, {rw[1], POLLOUT}};
    EXPECT_EQ(2, poll(fds, ARRAYLEN(fds), 0));
    EXPECT_STREQ("fd:3 revents:POLLHUP", gc(FormatPollFd(&fds[0])));
    EXPECT_STREQ("fd:4 revents:POLLNVAL", gc(FormatPollFd(&fds[1])));
  }
  ASSERT_NE(-1, close(rw[0])); /* close consumer */
}

#endif
