/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Cadence Ember                                                 │
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
#include "libc/isystem/errno.h"
#include "libc/isystem/signal.h"
#include "libc/isystem/stddef.h"
#include "libc/isystem/unistd.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/testlib.h"

#define MY_TEST_STRING_1 "He"
#define MY_TEST_STRING_2 "llo world!"

char buf[20] = {0};
int pipes[2];
int pid;
int got_sigusr1 = 0;

void sigusr1_handler(int) {
  got_sigusr1 = 1;
}

void write_pipe(int send_signal_before_end) {
  // Set up pipe for writing
  close(pipes[0]);
  FILE *stream = fdopen(pipes[1], "w");

  // Start writing the first part of the stream
  fputs(MY_TEST_STRING_1, stream);

  // Send SIGUSR1 to parent (if we're currently testing that)
  if (send_signal_before_end) {
    kill(getppid(), SIGUSR1);
  }

  // Send rest of stream
  fputs(MY_TEST_STRING_2, stream);

  // Close stream - this will cause the parent's fgets to end
  fclose(stream);
}

void read_pipe() {
  // Set up pipe for reading
  close(pipes[1]);
  FILE *stream = fdopen(pipes[0], "r");

  // Read with fgets
  fgets(buf, 20, stream);

  // Tidy up
  fclose(stream);
}

void setup_signal_and_pipe(uint64_t sa_flags) {
  // Set up SIGUSR1 handler
  struct sigaction sa = {.sa_handler = sigusr1_handler, .sa_flags = sa_flags};
  if (sigaction(SIGUSR1, &sa, NULL) == -1) {
    perror("sigaction");
    _exit(1);
  }

  // Set up pipe between parent and child
  if (pipe(pipes) == -1) {
    perror("pipe");
    _exit(1);
  }
}

TEST(fgets_eintr, testThatFgetsReadsFromPipeNormally) {
  setup_signal_and_pipe(0);  // 0 = no SA_RESTART
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    write_pipe(0);  // 0 = no signal
    _exit(0);
  }
  read_pipe();
  EXPECT_STREQ(MY_TEST_STRING_1 MY_TEST_STRING_2, buf);
}

TEST(fgets_eintr, testThatTheSignalInterruptsFgets) {
  setup_signal_and_pipe(0);  // 0 = no SA_RESTART
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    write_pipe(1);  // 1 = signal
    _exit(0);
  }
  read_pipe();
  EXPECT_STRNE(MY_TEST_STRING_1 MY_TEST_STRING_2, buf);
  EXPECT_EQ(EINTR, errno);
}

TEST(fgets_eintr, testThatFgetsRestartsWhenSaRestartIsSet) {
  setup_signal_and_pipe(SA_RESTART);  // SA_RESTART
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    write_pipe(1);  // 1 = signal
    _exit(0);
  }
  read_pipe();
  EXPECT_STREQ(MY_TEST_STRING_1 MY_TEST_STRING_2, buf);
  EXPECT_NE(EINTR, errno);
  EXPECT_EQ(1, got_sigusr1);
}
