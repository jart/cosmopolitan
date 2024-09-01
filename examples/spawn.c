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
#include <fcntl.h>
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define PIPE_READ  0
#define PIPE_WRITE 1

int main() {
  pid_t pid;
  int status, ret;
  posix_spawnattr_t attr;
  posix_spawn_file_actions_t actions;
  char *const argv[] = {"ls", "--dired", NULL};
  int pipe_stdout[2], pipe_stderr[2];

  // Initialize file actions
  ret = posix_spawnattr_init(&attr);
  if (ret != 0) {
    fprintf(stderr, "posix_spawnattr_init failed: %s\n", strerror(ret));
    return 1;
  }

  // Explicitly request vfork() from posix_spawn() implementation
  //
  // This is currently the default for Cosmopolitan Libc, however you
  // may want to set this anyway, for portability with other platforms.
  // Please note that vfork() isn't officially specified by POSIX, so
  // portable code may want to omit this and just use the default.
  ret = posix_spawnattr_setflags(&attr, POSIX_SPAWN_USEVFORK);
  if (ret != 0) {
    fprintf(stderr, "posix_spawnattr_setflags failed: %s\n", strerror(ret));
    return 1;
  }

  // Initialize file actions
  ret = posix_spawn_file_actions_init(&actions);
  if (ret != 0) {
    fprintf(stderr, "posix_spawn_file_actions_init failed: %s\n",
            strerror(ret));
    return 1;
  }

  // Change directory to $HOME
  ret = posix_spawn_file_actions_addchdir_np(&actions, getenv("HOME"));
  if (ret != 0) {
    fprintf(stderr, "posix_spawn_file_actions_addchdir_np failed: %s\n",
            strerror(ret));
    return 1;
  }

  // Create pipes for stdout and stderr
  if (pipe(pipe_stdout) == -1 || pipe(pipe_stderr) == -1) {
    perror("pipe");
    return 1;
  }

  // Redirect child's stdout to pipe
  ret = posix_spawn_file_actions_adddup2(&actions, pipe_stdout[PIPE_WRITE],
                                         STDOUT_FILENO);
  if (ret != 0) {
    fprintf(stderr, "posix_spawn_file_actions_adddup2 (stdout) failed: %s\n",
            strerror(ret));
    return 1;
  }

  // Redirect child's stderr to pipe
  ret = posix_spawn_file_actions_adddup2(&actions, pipe_stderr[PIPE_WRITE],
                                         STDERR_FILENO);
  if (ret != 0) {
    fprintf(stderr, "posix_spawn_file_actions_adddup2 (stderr) failed: %s\n",
            strerror(ret));
    return 1;
  }

  // Close unused write ends of pipes in the child process
  ret = posix_spawn_file_actions_addclose(&actions, pipe_stdout[PIPE_READ]);
  if (ret != 0) {
    fprintf(stderr,
            "posix_spawn_file_actions_addclose (stdout read) failed: %s\n",
            strerror(ret));
    return 1;
  }
  ret = posix_spawn_file_actions_addclose(&actions, pipe_stderr[PIPE_READ]);
  if (ret != 0) {
    fprintf(stderr,
            "posix_spawn_file_actions_addclose (stderr read) failed: %s\n",
            strerror(ret));
    return 1;
  }

  // Spawn the child process
  ret = posix_spawnp(&pid, "ls", &actions, NULL, argv, NULL);
  if (ret != 0) {
    fprintf(stderr, "posix_spawn failed: %s\n", strerror(ret));
    return 1;
  }

  // Close unused write ends of pipes in the parent process
  close(pipe_stdout[PIPE_WRITE]);
  close(pipe_stderr[PIPE_WRITE]);

  // Read and print output from child process
  char buffer[4096];
  ssize_t bytes_read;

  printf("Stdout from child process:\n");
  while ((bytes_read = read(pipe_stdout[PIPE_READ], buffer, sizeof(buffer))) >
         0) {
    write(STDOUT_FILENO, buffer, bytes_read);
  }

  printf("\nStderr from child process:\n");
  while ((bytes_read = read(pipe_stderr[PIPE_READ], buffer, sizeof(buffer))) >
         0) {
    write(STDERR_FILENO, buffer, bytes_read);
  }

  // Wait for the child process to complete
  if (waitpid(pid, &status, 0) == -1) {
    perror("waitpid");
    return 1;
  }

  // Clean up
  posix_spawn_file_actions_destroy(&actions);
  posix_spawnattr_destroy(&attr);
  close(pipe_stdout[PIPE_READ]);
  close(pipe_stderr[PIPE_READ]);

  if (WIFEXITED(status)) {
    printf("Child process exited with status %d\n", WEXITSTATUS(status));
  } else if (WIFSIGNALED(status)) {
    printf("Child process terminated with signal %s\n",
           strsignal(WTERMSIG(status)));
  } else {
    printf("Child process did not exit normally\n");
  }

  return 0;
}
