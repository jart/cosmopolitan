// clang-format off
#include "third_party/mold/elf/mold.h"

#ifndef _WIN32
#include "libc/calls/calls.h"
#include "libc/calls/struct/flock.h"
#include "libc/calls/weirdtypes.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/posix.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/consts/splice.h"
#include "third_party/musl/passwd.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/stat.macros.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/weirdtypes.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/consts/utime.h"
#include "libc/time/time.h"
#include "libc/calls/makedev.h"
#include "libc/calls/weirdtypes.h"
#include "libc/thread/thread.h"
#include "libc/calls/typedef/u.h"
#include "libc/calls/weirdtypes.h"
#include "libc/intrin/newbie.h"
#include "libc/sock/select.h"
#include "libc/sysv/consts/endian.h"
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
#endif

namespace mold::elf {

template <typename E>
void acquire_global_lock(Context<E> &ctx) {
#ifndef _WIN32
  char *jobs = getenv("MOLD_JOBS");
  if (!jobs || std::string(jobs) != "1")
    return;

  char *home = getenv("HOME");
  if (!home)
    home = getpwuid(getuid())->pw_dir;

  std::string path = std::string(home) + "/.mold-lock";
  int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_CLOEXEC, 0600);
  if (fd == -1)
    return;

  if (lockf(fd, F_LOCK, 0) == -1)
    return;

  ctx.global_lock_fd = fd;
#endif
}

template <typename E>
void release_global_lock(Context<E> &ctx) {
#ifndef _WIN32
  if (ctx.global_lock_fd)
    close(*ctx.global_lock_fd);
#endif
}

using E = MOLD_TARGET;

template void acquire_global_lock(Context<E> &);
template void release_global_lock(Context<E> &);

} // namespace mold::elf
