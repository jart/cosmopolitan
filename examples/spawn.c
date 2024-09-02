#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif

// posix_spawn() example
//
// This program demonstrates the use of posix_spawn() to run the command
// `ls --dired` and capture its output. It teaches several key features:
//
// - Changing the working directory for the child process
// - Redirecting stdout and stderr to pipes
// - Handling the output from the child process
//
// The primary advantage of using posix_spawn() instead of the
// traditional fork()/execve() combination for launching processes is
// safety, efficiency, and cross-platform compatibility.
//
// 1. On Linux, FreeBSD, and NetBSD:
//
//    Cosmopolitan Libc's posix_spawn() uses vfork() under the hood on
//    these platforms automatically, since it's faster than fork(). It's
//    because vfork() creates a child process without needing to copy
//    the parent's page tables, making it more efficient, especially for
//    large processes. Furthermore, vfork() avoids the need to acquire
//    every single mutex (see pthread_atfork() for more details) which
//    makes it scalable in multi-threaded apps, since the other threads
//    in your app can keep going while the spawning thread waits for the
//    subprocess to call execve(). Normally vfork() is error-prone since
//    there exists few functions that are @vforksafe. the posix_spawn()
//    API is designed to offer maximum assurance that you can't shoot
//    yourself in the foot. If you do, then file a bug with Cosmo.
//
// 2. On Windows:
//
//    posix_spawn() avoids fork() entirely. Windows doesn't natively
//    support fork(), and emulating it can be slow and memory-intensive.
//    By using posix_spawn(), we get a much faster process creation on
//    Windows systems, because it only needs to call CreateProcess().
//    Your file actions are replayed beforehand in a simulated way. Only
//    Cosmopolitan Libc offers this level of quality. With Cygwin you'd
//    have to use its proprietary APIs to achieve the same performance.
//
// 3. Simplified error handling:
//
//    posix_spawn() combines process creation and program execution in a
//    single call, reducing the points of failure and simplifying error
//    handling. One important thing that happens with Cosmopolitan's
//    posix_spawn() implementation is that the error code of execve()
//    inside your subprocess, should it fail, will be propagated to your
//    parent process. This will happen efficiently via vfork() shared
//    memory in the event your Linux environment supports this. If it
//    doesn't, then Cosmopolitan will fall back to a throwaway pipe().
//    The pipe is needed on platforms like XNU and OpenBSD which do not
//    support vfork(). It's also needed under QEMU User.
//
// 4. Signal safety:
//
//    posix_spawn() guarantees your signal handler callback functions
//    won't be executed in the child process. By default, it'll remove
//    sigaction() callbacks atomically. This ensures that if something
//    like a SIGTERM or SIGHUP is sent to the child process before it's
//    had a chance to call execve(), then the child process will simply
//    be terminated (like the spawned process would) instead of running
//    whatever signal handlers the spawning process has installed. If
//    you've set some signals to SIG_IGN, then that'll be preserved for
//    the child process by posix_spawn(), unless you explicitly call
//    posix_spawnattr_setsigdefault() to reset them.
//
// 5. Portability:
//
//    posix_spawn() is part of the POSIX standard, making it more
//    portable across different UNIX-like systems and Windows (with
//    appropriate libraries). Even the non-POSIX APIs we use here are
//    portable; e.g. posix_spawn_file_actions_addchdir_np() is supported
//    by glibc, musl, freebsd, and apple too.
//
// These benefits make posix_spawn() a preferred choice for efficient
// and portable process creation in many scenarios, especially when
// launching many processes or on systems where process creation
// performance is critical.

#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <unistd.h>

#define max(X, Y) ((Y) < (X) ? (X) : (Y))

#define USE_SELECT 0  // want poll() or select()? they both work great

#define PIPE_READ  0
#define PIPE_WRITE 1

int main() {
  errno_t err;

  // Create spawn attributes object.
  posix_spawnattr_t attr;
  err = posix_spawnattr_init(&attr);
  if (err != 0) {
    fprintf(stderr, "posix_spawnattr_init failed: %s\n", strerror(err));
    exit(1);
  }

  // Explicitly request vfork() from posix_spawn() implementation.
  //
  // This is currently the default for Cosmopolitan Libc, however you
  // may want to set this anyway, for portability with other platforms.
  // Please note that vfork() isn't officially specified by POSIX, so
  // portable code may want to omit this and just use the default.
  err = posix_spawnattr_setflags(&attr, POSIX_SPAWN_USEVFORK);
  if (err != 0) {
    fprintf(stderr, "posix_spawnattr_setflags: %s\n", strerror(err));
    exit(2);
  }

  // Create file actions object.
  posix_spawn_file_actions_t actions;
  err = posix_spawn_file_actions_init(&actions);
  if (err != 0) {
    fprintf(stderr, "posix_spawn_file_actions_init: %s\n", strerror(err));
    exit(3);
  }

  // Change directory to root directory in child process.
  err = posix_spawn_file_actions_addchdir_np(&actions, "/");
  if (err != 0) {
    fprintf(stderr, "posix_spawn_file_actions_addchdir_np: %s\n",
            strerror(err));
    exit(4);
  }

  // Disable stdin in child process.
  //
  // By default, if you launch this example in your terminal, then child
  // processes can read from your teletypewriter's keyboard too. You can
  // avoid this by assigning /dev/null to standard input so if the child
  // tries to read input, read() will return zero, indicating eof.
  if ((err = posix_spawn_file_actions_addopen(&actions, STDIN_FILENO,
                                              "/dev/null", O_RDONLY, 0644))) {
    fprintf(stderr, "posix_spawn_file_actions_addopen: %s\n", strerror(err));
    exit(5);
  }

  // Create pipes for stdout and stderr.
  //
  // Using O_DIRECT puts the pipe in message mode. This way we have some
  // visibility into how the child process is using write(). It can also
  // help ensure that logged lines won't be chopped up here, which could
  // happen more frequently on platforms like Windows, which is somewhat
  // less sophisticated than Linux with how it performs buffering.
  //
  // You can also specify O_CLOEXEC, which is a nice touch that lets you
  // avoid needing to call posix_spawn_file_actions_addclose() later on.
  // That's because all file descriptors are inherited by child programs
  // by default. This is even the case with Cosmopolitan Libc on Windows
  //
  // XXX: We assume that stdin/stdout/stderr exist in this process. It's
  //      possible for a rogue parent process to launch this example, in
  //      a way where the following spawn logic will break.
  int pipe_stdout[2];
  int pipe_stderr[2];
  if (pipe2(pipe_stdout, O_DIRECT) == -1 ||
      pipe2(pipe_stderr, O_DIRECT) == -1) {
    perror("pipe");
    exit(6);
  }

  // Redirect child's stdout/stderr to pipes
  if ((err = posix_spawn_file_actions_adddup2(&actions, pipe_stdout[PIPE_WRITE],
                                              STDOUT_FILENO)) ||
      (err = posix_spawn_file_actions_adddup2(&actions, pipe_stderr[PIPE_WRITE],
                                              STDERR_FILENO))) {
    fprintf(stderr, "posix_spawn_file_actions_adddup2: %s\n", strerror(err));
    exit(7);
  }

  // Close unwanted write ends of pipes in the child process
  if ((err = posix_spawn_file_actions_addclose(&actions,
                                               pipe_stdout[PIPE_READ])) ||
      (err = posix_spawn_file_actions_addclose(&actions,
                                               pipe_stderr[PIPE_READ]))) {
    fprintf(stderr, "posix_spawn_file_actions_addclose: %s\n", strerror(err));
    exit(8);
  };

  // Asynchronously launch the child process.
  pid_t pid;
  char *const argv[] = {"ls", "--dired", NULL};
  printf("** Launching `ls --dired` in root directory\n");
  err = posix_spawnp(&pid, argv[0], &actions, NULL, argv, NULL);
  if (err) {
    fprintf(stderr, "posix_spawn: %s\n", strerror(err));
    exit(9);
  }

  // Close unused write ends of pipes in the parent process
  close(pipe_stdout[PIPE_WRITE]);
  close(pipe_stderr[PIPE_WRITE]);

  // we need poll() or select() because we're multiplexing output
  // both poll() and select() work across all supported platforms
#if USE_SELECT
  // Relay output from child process using select()
  char buffer[512];
  ssize_t got_stdout = 1;
  ssize_t got_stderr = 1;
  while (got_stdout > 0 || got_stderr > 0) {
    fd_set rfds;
    FD_ZERO(&rfds);
    if (got_stdout > 0)
      FD_SET(pipe_stdout[PIPE_READ], &rfds);
    if (got_stderr > 0)
      FD_SET(pipe_stderr[PIPE_READ], &rfds);
    int nfds = max(pipe_stdout[PIPE_READ], pipe_stderr[PIPE_READ]) + 1;
    if (select(nfds, &rfds, 0, 0, 0) == -1) {
      perror("select");
      exit(10);
    }
    if (FD_ISSET(pipe_stdout[PIPE_READ], &rfds)) {
      got_stdout = read(pipe_stdout[PIPE_READ], buffer, sizeof(buffer));
      printf("\n");
      if (got_stdout > 0) {
        printf("** Got stdout from child process:\n");
        fflush(stdout);
        write(STDOUT_FILENO, buffer, got_stdout);
      } else if (!got_stdout) {
        printf("** Got stdout EOF from child process\n");
      } else {
        printf("** Got stdout read() error from child process: %s\n",
               strerror(errno));
      }
    }
    if (FD_ISSET(pipe_stderr[PIPE_READ], &rfds)) {
      got_stderr = read(pipe_stderr[PIPE_READ], buffer, sizeof(buffer));
      printf("\n");
      if (got_stderr > 0) {
        printf("** Got stderr from child process:\n");
        fflush(stdout);
        write(STDOUT_FILENO, buffer, got_stderr);
      } else if (!got_stderr) {
        printf("** Got stderr EOF from child process\n");
      } else {
        printf("** Got stderr read() error from child process: %s\n",
               strerror(errno));
      }
    }
  }

#else
  // Relay output from child process using poll()
  char buffer[512];
  ssize_t got_stdout = 1;
  ssize_t got_stderr = 1;
  while (got_stdout > 0 || got_stderr > 0) {
    struct pollfd fds[2];
    fds[0].fd = got_stdout > 0 ? pipe_stdout[PIPE_READ] : -1;
    fds[0].events = POLLIN;  // POLLHUP, POLLNVAL, and POLLERR are implied
    fds[1].fd = got_stderr > 0 ? pipe_stderr[PIPE_READ] : -1;
    fds[1].events = POLLIN;  // POLLHUP, POLLNVAL, and POLLERR are implied
    if (poll(fds, 2, -1) == -1) {
      perror("select");
      exit(10);
    }
    if (fds[0].revents) {
      printf("\n");
      if (fds[0].revents & POLLIN)
        printf("** Got POLLIN on stdout from child process\n");
      if (fds[0].revents & POLLHUP)
        printf("** Got POLLHUP on stdout from child process\n");
      if (fds[0].revents & POLLERR)
        printf("** Got POLLERR on stdout from child process\n");
      if (fds[0].revents & POLLNVAL)
        printf("** Got POLLNVAL on stdout from child process\n");
      got_stdout = read(pipe_stdout[PIPE_READ], buffer, sizeof(buffer));
      if (got_stdout > 0) {
        printf("** Got stdout from child process:\n");
        fflush(stdout);
        write(STDOUT_FILENO, buffer, got_stdout);
      } else if (!got_stdout) {
        printf("** Got stdout EOF from child process\n");
      } else {
        printf("** Got stdout read() error from child process: %s\n",
               strerror(errno));
      }
    }
    if (fds[1].revents) {
      printf("\n");
      if (fds[1].revents & POLLIN)
        printf("** Got POLLIN on stderr from child process\n");
      if (fds[1].revents & POLLHUP)
        printf("** Got POLLHUP on stderr from child process\n");
      if (fds[1].revents & POLLERR)
        printf("** Got POLLERR on stderr from child process\n");
      if (fds[1].revents & POLLNVAL)
        printf("** Got POLLNVAL on stderr from child process\n");
      got_stderr = read(pipe_stderr[PIPE_READ], buffer, sizeof(buffer));
      if (got_stderr > 0) {
        printf("** Got stderr from child process:\n");
        fflush(stdout);
        write(STDOUT_FILENO, buffer, got_stderr);
      } else if (!got_stderr) {
        printf("** Got stderr EOF from child process\n");
      } else {
        printf("** Got stderr read() error from child process: %s\n",
               strerror(errno));
      }
    }
  }
#endif

  // Wait for child process to die.
  int wait_status;
  if (waitpid(pid, &wait_status, 0) == -1) {
    perror("waitpid");
    exit(11);
  }

  // Clean up resources.
  posix_spawn_file_actions_destroy(&actions);
  posix_spawnattr_destroy(&attr);
  close(pipe_stdout[PIPE_READ]);
  close(pipe_stderr[PIPE_READ]);

  // Report wait status.
  //
  // When a process dies, it's almost always due to calling _Exit() or
  // being killed due to an unhandled signal. On both UNIX and Windows
  // this information will be propagated to the parent. That status is
  // able to be propagated to the parent of this process too.
  printf("\n");
  if (WIFEXITED(wait_status)) {
    printf("** Child process exited with exit code %d\n",
           WEXITSTATUS(wait_status));
    exit(WEXITSTATUS(wait_status));
  } else if (WIFSIGNALED(wait_status)) {
    printf("** Child process terminated with signal %s\n",
           strsignal(WTERMSIG(wait_status)));
    fflush(stdout);
    sigset_t sm;
    sigemptyset(&sm);
    sigaddset(&sm, WTERMSIG(wait_status));
    sigprocmask(SIG_UNBLOCK, &sm, 0);
    signal(SIGABRT, SIG_DFL);
    raise(WTERMSIG(wait_status));
    exit(128 + WTERMSIG(wait_status));
  } else {
    printf("** Child process exited weirdly with wait status 0x%08x\n",
           wait_status);
    exit(12);
  }
}
