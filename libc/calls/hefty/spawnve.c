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
#include "libc/bits/pushpop.h"
#include "libc/calls/hefty/internal.h"
#include "libc/calls/hefty/spawn.h"
#include "libc/dce.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/errfuns.h"

/**
 * Launches program, e.g.
 *
 *     char buf[2];
 *     int ws, pid, fds[3] = {-1, -1, STDERR_FILENO};
 *     CHECK_NE(-1, (pid = spawnve(0, fds, commandv("ssh"),
 *                                 (char *const[]){"ssh", hostname, "cat", 0},
 *                                 environ)));
 *     CHECK_EQ(+2, write(fds[0], "hi", 2));
 *     CHECK_NE(-1, close(fds[0]));
 *     CHECK_EQ(+2, read(fds[1], buf, 2)));
 *     CHECK_NE(-1, close(fds[1]));
 *     CHECK_EQ(+0, memcmp(buf, "hi", 2)));
 *     CHECK_NE(-1, waitpid(pid, &ws, 0));
 *     CHECK_EQ(+0, WEXITSTATUS(ws));
 *
 * @param stdiofds may optionally be passed to customize standard i/o
 * @param stdiofds[𝑖] may be -1 to receive a pipe() fd
 * @param program will not be PATH searched, see commandv()
 * @param argv[0] is the name of the program to run
 * @param argv[1,n-2] optionally specify program arguments
 * @param argv[n-1] is NULL
 * @param envp[0,n-2] specifies "foo=bar" environment variables
 * @param envp[n-1] is NULL
 * @return pid of child, or -1 w/ errno
 * @deprecated just use vfork() and execve()
 */
int spawnve(unsigned flags, int stdiofds[3], const char *program,
            char *const argv[], char *const envp[]) {
  if (!argv[0]) return einval();
  int defaultfds[3] = {pushpop(STDIN_FILENO), pushpop(STDOUT_FILENO),
                       pushpop(STDERR_FILENO)};
  if (!IsWindows()) {
    return spawnve$sysv(flags, stdiofds ?: defaultfds, program, argv, envp);
  } else {
    return spawnve$nt(flags, stdiofds ?: defaultfds, program, argv, envp);
  }
}
