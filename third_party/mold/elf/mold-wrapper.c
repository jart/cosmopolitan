// clang-format off
#define _GNU_SOURCE 1

#if !defined(__OpenBSD__) && !defined(__FreeBSD__)
#include "libc/mem/alloca.h"
#endif
#include "libc/runtime/dlfcn.h"
#include "libc/calls/weirdtypes.h"
#include "libc/stdio/posix_spawn.h"


#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"
#include "libc/calls/calls.h"
#include "libc/calls/termios.h"
#include "libc/fmt/conv.h"
#include "libc/limits.h"
#include "libc/mem/alg.h"
#include "libc/mem/alloca.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/temp.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/exit.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/musl/crypt.h"
#include "third_party/musl/rand48.h"
#include "libc/mem/alg.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/runtime/pathconf.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/sysconf.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/ok.h"
#include "libc/time/time.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/musl/crypt.h"
#include "third_party/musl/lockf.h"

extern char **environ;

static char *get_mold_path() {
  char *path = getenv("MOLD_PATH");
  if (path)
    return path;
  fprintf(stderr, "MOLD_PATH is not set\n");
  exit(1);
}

static void debug_print(const char *fmt, ...) {
  if (!getenv("MOLD_WRAPPER_DEBUG"))
    return;

  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, "mold-wrapper.so: ");
  vfprintf(stderr, fmt, ap);
  fflush(stderr);
  va_end(ap);
}

static int count_args(va_list *ap) {
  va_list aq;
  va_copy(aq, *ap);

  int i = 0;
  while (va_arg(aq, char *))
    i++;
  va_end(aq);
  return i;
}

static void copy_args(char **argv, const char *arg0, va_list *ap) {
  int i = 1;
  char *arg;
  while ((arg = va_arg(*ap, char *)))
    argv[i++] = arg;

  ((const char **)argv)[0] = arg0;
  ((const char **)argv)[i] = NULL;
}

static bool is_ld(const char *path) {
  const char *ptr = path + strlen(path);
  while (path < ptr && ptr[-1] != '/')
    ptr--;

  return !strcmp(ptr, "ld") || !strcmp(ptr, "ld.lld") ||
         !strcmp(ptr, "ld.gold") || !strcmp(ptr, "ld.bfd") ||
         !strcmp(ptr, "ld.mold");
}

int execvpe(const char *file, char *const *argv, char *const *envp) {
  debug_print("execvpe %s\n", file);

  if (!strcmp(file, "ld") || is_ld(file))
    file = get_mold_path();

  for (int i = 0; envp[i]; i++)
    putenv(envp[i]);

  typeof(execvpe) *real = dlsym(RTLD_NEXT, "execvp");
  return real(file, argv, environ);
}

int execve(const char *path, char *const *argv, char *const *envp) {
  debug_print("execve %s\n", path);
  if (is_ld(path))
    path = get_mold_path();
  typeof(execve) *real = dlsym(RTLD_NEXT, "execve");
  return real(path, argv, envp);
}

int execl(const char *path, const char *arg0, ...) {
  va_list ap;
  va_start(ap, arg0);
  char **argv = alloca((count_args(&ap) + 2) * sizeof(char *));
  copy_args(argv, arg0, &ap);
  va_end(ap);
  return execve(path, argv, environ);
}

int execlp(const char *file, const char *arg0, ...) {
  va_list ap;
  va_start(ap, arg0);
  char **argv = alloca((count_args(&ap) + 2) * sizeof(char *));
  copy_args(argv, arg0, &ap);
  va_end(ap);
  return execvpe(file, argv, environ);
}

int execle(const char *path, const char *arg0, ...) {
  va_list ap;
  va_start(ap, arg0);
  char **argv = alloca((count_args(&ap) + 2) * sizeof(char *));
  copy_args(argv, arg0, &ap);
  char **env = va_arg(ap, char **);
  va_end(ap);
  return execve(path, argv, env);
}

int execv(const char *path, char *const *argv) {
  return execve(path, argv, environ);
}

int execvp(const char *file, char *const *argv) {
  return execvpe(file, argv, environ);
}

int posix_spawn(pid_t *pid, const char *path,
                const posix_spawn_file_actions_t *file_actions,
                const posix_spawnattr_t *attrp,
                char *const *argv, char *const *envp) {
  debug_print("posix_spawn %s\n", path);
  if (is_ld(path))
    path = get_mold_path();
  typeof(posix_spawn) *real = dlsym(RTLD_NEXT, "posix_spawn");
  return real(pid, path, file_actions, attrp, argv, envp);
}
