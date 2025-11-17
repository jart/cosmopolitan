// Copyright 2025 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include "libc/assert.h"
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/flock.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/weaken.h"
#include "libc/nt/enum/filelockflags.h"
#include "libc/nt/errors.h"
#include "libc/nt/events.h"
#include "libc/nt/files.h"
#include "libc/nt/memory.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/stdckdint.h"
#include "libc/stdio/sysparam.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/errfuns.h"
#include "libc/sysv/errno.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"

/**
 * @fileoverview POSIX Advisory Locks for Windows
 *
 * This implementation has the following quirks:
 *
 * - When `F_GETLK` finds a conflicting lock owned by another process,
 *   the `l_pid` field is always set to -1.
 *
 * - When `F_GETLK` finds a conflicting lock owned by another process,
 *   the `l_len` and `l_start` fields aren't changed to reflect a more
 *   specific interval for a conflicting lock.
 *
 * - It's not possible for `F_GETLK` to accurately report `F_UNLCK` in
 *   all cases, when the current process owns locked regions that only
 *   partially overlap the requested region.
 *
 * - When a process transitions its own lock from `F_RDLCK` to `F_WRLCK`
 *   or vice versa, this does not happen atomically; the lock needs to
 *   be deleted and then recreated. So another thread or process could
 *   obtain the lock on the requested region during that time.
 *
 * - When a process only partially unlocks a locked interval it owns,
 *   possibly due to setting a lock that overlaps an existing one, then
 *   the operation can not happen atomically. The entire region that the
 *   process originally locked needs to be unlocked, and then the
 *   unrelated righthand and/or lefthand portions that were blown away
 *   need to be recreated, before the requested lock operation can
 *   proceed. This resurrection process is always blocking (even if
 *   `F_SETLKW` isn't used) and it can't be interrupted by signals.
 *   Thankfully we've yet to find software that actually does this.
 */

#define PROTECTED_BYTES 0x7fffffff00000000

#define FILELOCK_CONTAINER(e) DLL_CONTAINER(struct FileLock, elem, e)

__msabi extern typeof(WaitForMultipleObjects)
    *const __imp_WaitForMultipleObjects;

struct FileLock {
  struct Dll elem;
  intptr_t handle;
  int64_t dev;
  int64_t ino;
  int64_t off;
  int64_t len;
  int type;
};

struct FileLocks {
  pthread_mutex_t mu;
  struct Dll *list;
  atomic_bool used;
};

alignas(64) static struct FileLocks __flocks = {
    .mu = PTHREAD_MUTEX_INITIALIZER,
};

textwindows static void __flocks_lock(void) {
  pthread_mutex_lock(&__flocks.mu);
}

textwindows static void __flocks_unlock(void) {
  pthread_mutex_unlock(&__flocks.mu);
}

textwindows static void *__flocks_malloc(size_t size) {
  return HeapAlloc(GetProcessHeap(), 0, size);
}

textwindows static void __flocks_free(void *ptr) {
  HeapFree(GetProcessHeap(), 0, ptr);
}

textwindows static struct FileLock *__flocks_newlock(void) {
  struct FileLock *fl;
  if (!(fl = __flocks_malloc(sizeof(struct FileLock))))
    return 0;
  bzero(fl, sizeof(*fl));
  dll_init(&fl->elem);
  return fl;
}

textwindows static void __flocks_freelock(struct FileLock *fl) {
  __flocks_free(fl);
}

textwindows static void __flocks_add(struct Dll *list) {
  if (!dll_is_empty(list)) {
    dll_make_first(&__flocks.list, list);
    if (!atomic_load(&__flocks.used))
      atomic_store(&__flocks.used, true);
  }
}

textwindows static void __flocks_remove(struct FileLock *fl) {
  dll_remove(&__flocks.list, &fl->elem);
  if (dll_is_empty(__flocks.list))
    atomic_store(&__flocks.used, false);
}

textwindows static void __flocks_freelocks(struct Dll *locks) {
  struct Dll *e, *e2;
  for (e = dll_first(locks); e; e = e2) {
    e2 = dll_next(locks, e);
    __flocks_freelock(FILELOCK_CONTAINER(e));
  }
}

textwindows static bool __flocks_overlaps(struct FileLock *fl, int64_t off,
                                          int64_t len) {
  return MAX(off, fl->off) < MIN(off + len, fl->off + fl->len);
}

// called by fork() from child so flocks aren't inherited
textwindows void __flocks_wipe(void) {
  atomic_init(&__flocks.used, false);
  pthread_mutex_wipe_np(&__flocks.mu);
  // this memory is all managed by win32 so fork() and mmap() have no
  // awareness of its existence therefore it can't exist in the child
  __flocks.list = 0;
}

textwindows static void __flocks_unlockfile(int64_t hFile, int64_t off,
                                            int64_t len) {
  // if this thread previously called LockFileEx(), read(), or write(),
  // etc. then an event object is guaranteed to be cached in its thread
  // local storage. even if that somehow isn't the case, UnlockFileEx()
  // will simply use hFile as the event object, which is not the end of
  // the world therefore it's safe to ignore the failure condition here
  intptr_t event = CreateEventTls();
  struct NtOverlapped overlap = {.hEvent = event, .Pointer = off};
  // we don't believe it's possible for this to fail. we have seen this
  // raise kNtErrorNotLocked which isn't considered an error by fcntl()
  // but we would consider it a bug in this module if it's raised here.
  unassert(UnlockFileEx(hFile, 0, len, len >> 32, &overlap));
  CloseEventTls(event);
}

// called by close(fd) to unlock every lock owned by process on file. we
// didn't start the fire. it's *famously* one of the worst unix designs.
// yes it actually does apply even if the unrelated fd isn't using locks
textwindows void __flocks_close(struct Fd *f) {
  if (atomic_load(&__flocks.used)) {
    int64_t dev = f->dev;
    int64_t ino = f->ino;
    if (!dev || !ino) {
      struct NtByHandleFileInformation wst;
      if (!GetFileInformationByHandle(f->handle, &wst))
        return;
      dev = wst.dwVolumeSerialNumber;
      ino = (wst.nFileIndexHigh + 0ull) << 32 | wst.nFileIndexLow;
    }
    __flocks_lock();
    struct Dll *e, *e2;
    for (e = dll_first(__flocks.list); e; e = e2) {
      e2 = dll_next(__flocks.list, e);
      struct FileLock *fl = FILELOCK_CONTAINER(e);
      if (fl->dev == dev && fl->ino == ino) {
        __flocks_remove(fl);
        __flocks_unlockfile(fl->handle, fl->off, fl->len);
        __flocks_freelock(fl);
      }
    }
    __flocks_unlock();
  }
}

textwindows static bool32 __flocks_lockfileu(intptr_t hand, uint32_t flags,
                                             int64_t off, int64_t len) {
  intptr_t event = CreateEventTls();
  struct NtOverlapped overlap = {.hEvent = event, .Pointer = off};
  bool32 ok = LockFileEx(hand, flags, 0, len, len >> 32, &overlap);
  if (!ok && GetLastError() == kNtErrorIoPending)
    ok = true;
  if (ok) {
    uint32_t exchanged;
    ok = GetOverlappedResult(hand, &overlap, &exchanged, true);
  }
  CloseEventTls(event);
  return ok;
}

textwindows static int __flocks_lockfile(int64_t hand, uint32_t flags,
                                         int64_t off, int64_t len,
                                         sigset_t waitmask) {
  for (;;) {
    int got_sig = 0;
    uint32_t other_error = 0;

    intptr_t event = CreateEventTls();
    if (!event)
      return __winerr();

    struct NtOverlapped overlap = {.hEvent = event, .Pointer = off};
    bool32 ok = LockFileEx(hand, flags, 0, len, len >> 32, &overlap);
    if (!ok && GetLastError() == kNtErrorIoPending) {
      intptr_t sigev;
      if ((sigev = __interruptible_start(waitmask))) {
        if (_is_canceled()) {
          CancelIoEx(hand, &overlap);
        } else if (_weaken(__sig_get) &&
                   (got_sig = _weaken(__sig_get)(waitmask))) {
          CancelIoEx(hand, &overlap);
        } else {
          intptr_t hands[] = {event, sigev};
          uint32_t wi = __imp_WaitForMultipleObjects(2, hands, 0, -1u);
          if (wi == 1) {  // event was signaled by signal enqueue
            CancelIoEx(hand, &overlap);
            if (_weaken(__sig_get))
              got_sig = _weaken(__sig_get)(waitmask);
          } else if (wi == -1u) {
            other_error = GetLastError();
            CancelIoEx(hand, &overlap);
          }
        }
        __interruptible_end();
      } else {
        other_error = GetLastError();
        CancelIoEx(hand, &overlap);
      }
      ok = true;
    }
    if (ok) {
      uint32_t exchanged;
      ok = GetOverlappedResult(hand, &overlap, &exchanged, true);
    }
    uint32_t io_error = GetLastError();
    CloseEventTls(event);

    // check if i/o completed
    // this could forseeably happen even if CancelIoEx was called
    if (ok) {
      if (got_sig)  // swallow dequeued signal
        _weaken(__sig_relay)(got_sig, SI_KERNEL, waitmask);
      return 0;
    }

    // check if i/o failed
    if (io_error != kNtErrorOperationAborted) {
      if (got_sig)  // swallow dequeued signal
        _weaken(__sig_relay)(got_sig, SI_KERNEL, waitmask);
      errno = __errno_windows2linux(io_error);
      return -1;
    }

    // it's now reasonable to report semaphore creation error
    if (other_error) {
      errno = __errno_windows2linux(other_error);
      return -1;
    }

    // check for thread cancelation and acknowledge
    if (_check_cancel() == -1)
      return -1;

    // if signal module has been linked, then
    if (_weaken(__sig_get)) {

      // gobble up all unmasked pending signals
      // it's now safe to recurse into signal handlers
      int handler_was_called = 0;
      do {
        if (got_sig)
          handler_was_called |=
              _weaken(__sig_relay)(got_sig, SI_KERNEL, waitmask);
      } while ((got_sig = _weaken(__sig_get)(waitmask)));

      // check if SIGTHR handler was called
      if (_check_cancel() == -1)
        return -1;

      // check if signal handler without SA_RESTART was called
      if (handler_was_called & SIG_HANDLED_NO_RESTART)
        return eintr();
    }

    // otherwise try the i/o operation again
  }
}

textwindows static int __flocks_getlk(struct Fd *f, struct flock *l, int type,
                                      int64_t off, int64_t len, int64_t dev,
                                      int64_t ino) {
  // to "get" is to check if a lock is immediately obtainable. if the
  // requested region can be locked without blocking, then F_UNLCK is
  // returned; it is always be the case if it is owned by the current
  // process. if another process holds a lock that conflicts with our
  // request, then this function should ideally return information on
  // one of the conflicting locks.
  __flocks_lock();
  for (struct Dll *e = dll_first(__flocks.list); e;
       e = dll_next(__flocks.list, e)) {
    struct FileLock *fl = FILELOCK_CONTAINER(e);
    if (fl->dev != dev || fl->ino != ino)
      continue;
    // here's a lazy solution. if there's a lock owned by this process
    // which envelops the requested region then we're pretty darn sure
    // the lock would be immediately obtainable; type does not matter.
    if (fl->off <= off && fl->off + fl->len >= off + len) {
      l->l_type = F_UNLCK;
      __flocks_unlock();
      return 0;
    }
  }
  __flocks_unlock();
  // now that we know the current process doesn't obviously own the
  // entire requested region, we shall attempt to acquire the lock to
  // determine if it's owned by another process (or possibly this one)
  // we sadly can't get the pid or true range of the conflicting lock.
  if (type == F_RDLCK) {
    // test if we can set a read lock
    if (__flocks_lockfileu(f->handle, kNtLockfileFailImmediately, off, len)) {
      l->l_type = F_UNLCK;
      __flocks_unlockfile(f->handle, off, len);
    } else {
      unassert(GetLastError() == kNtErrorLockViolation);
      l->l_type = F_WRLCK;
      l->l_pid = -1;
    }
  } else {
    // test if we can set a write lock
    if (__flocks_lockfileu(
            f->handle, kNtLockfileFailImmediately | kNtLockfileExclusiveLock,
            off, len)) {
      l->l_type = F_UNLCK;
      __flocks_unlockfile(f->handle, off, len);
    } else {
      unassert(GetLastError() == kNtErrorLockViolation);
      if (__flocks_lockfileu(f->handle, kNtLockfileFailImmediately, off, len)) {
        l->l_type = F_RDLCK;
        l->l_pid = -1;
        __flocks_unlockfile(f->handle, off, len);
      } else {
        unassert(GetLastError() == kNtErrorLockViolation);
        l->l_type = F_WRLCK;
        l->l_pid = -1;
      }
    }
  }
  return 0;
}

textwindows static int __flocks_setlk(struct Fd *f, struct flock *l, int cmd,
                                      int type, int64_t off, int64_t len,
                                      int64_t dev, int64_t ino, int fd,
                                      sigset_t waitmask) {
  struct Dll *e, *e2;

  // acquire the process-wide file lock gil
  __flocks_lock();

  // setting a read or write lock is a noop if the process currently
  // owns a lock of the same type, that envelops the requested range
  if (type != F_UNLCK) {
    for (e = dll_first(__flocks.list); e; e = dll_next(__flocks.list, e)) {
      struct FileLock *fl = FILELOCK_CONTAINER(e);
      if (fl->dev == dev &&    //
          fl->ino == ino &&    //
          fl->type == type &&  //
          fl->off <= off &&    //
          fl->off + fl->len >= off + len) {
        __flocks_unlock();
        return 0;
      }
    }
  }

  // plan operation w/o syscalls. we need to consider the set of locks
  // we know about, which are the ones owned by this process, and then
  // figure out how to carve a hole in them so the requested operation
  // can succeed. the issue we're fundamentally solving is that flocks
  // behave similarly to mmaps on windows. unix lets you slice them up
  // but windows requires you pass the same {off,len} each time, or it
  // fails with a lock conflict.
  struct Dll *delete_locks = 0;
  struct Dll *resurrect_locks = 0;
  for (e = dll_first(__flocks.list); e; e = e2) {
    e2 = dll_next(__flocks.list, e);
    struct FileLock *fl = FILELOCK_CONTAINER(e);
    if (fl->dev != dev || fl->ino != ino)
      continue;
    if (!__flocks_overlaps(fl, off, len))
      continue;
    __flocks_remove(fl);
    dll_make_first(&delete_locks, &fl->elem);
    if (fl->off < off) {
      struct FileLock *left;
      if (!(left = __flocks_newlock()))
        goto OnPlanningOom;
      dll_make_first(&resurrect_locks, &left->elem);
      left->handle = f->handle;
      left->type = fl->type;
      left->dev = fl->dev;
      left->ino = fl->ino;
      left->off = fl->off;
      left->len = off - fl->off;
    }
    if (fl->off + fl->len > off + len) {
      struct FileLock *right;
      if (!(right = __flocks_newlock()))
        goto OnPlanningOom;
      dll_make_first(&resurrect_locks, &right->elem);
      right->handle = f->handle;
      right->type = fl->type;
      right->dev = fl->dev;
      right->ino = fl->ino;
      right->off = off + len;
      right->len = (fl->off + fl->len) - (off + len);
    }
  }

  // allocate new object
  struct FileLock *fl = 0;
  if (!(fl = __flocks_newlock())) {
  OnPlanningOom:
    __flocks_add(delete_locks);
    __flocks_unlock();
    __flocks_freelocks(resurrect_locks);
    return enomem();
  }

  // delete conflicting locks
  for (e = dll_first(delete_locks); e; e = e2) {
    e2 = dll_next(delete_locks, e);
    struct FileLock *fl2 = FILELOCK_CONTAINER(e);
    __flocks_unlockfile(fl2->handle, fl2->off, fl2->len);
    __flocks_freelock(fl2);
  }

  // resurrect conflicting shards
  while ((e = dll_first(resurrect_locks))) {
    dll_remove(&resurrect_locks, e);
    __flocks_add(e);
    struct FileLock *fl2 = FILELOCK_CONTAINER(e);
    // hopefully this won't cause a deadlock if some other process snags
    // the shard before we've had a chance to bring it back because this
    // code will block indefinitely, even in non-blocking mode, and this
    // operation can't be interrupted by a signal handler. therefore its
    // probably not possible that this is going to fail. it is more of a
    // concern that we could cause a deadlock in the application's logic
    npassert(__flocks_lockfileu(
        fl2->handle, fl2->type == F_WRLCK ? kNtLockfileExclusiveLock : 0,
        fl2->off, fl2->len));
  }

  // release the gil while performing the main intended operation, which
  // might take a very long time. the lock operation could also call sig
  // handlers, which might even longjmp out of this code. so we must not
  // be in any kind of mutex or transaction or holding onto resources we
  // aren't afraid to leak. remember that fcntl() is async signal safe!!
  __flocks_unlock();

  // if the user requested unlocking, then we're done
  if (type == F_UNLCK) {
    __flocks_freelock(fl);
    return 0;
  }

  // perform the actual intended file lock i/o operation
  uint32_t flags = 0;
  if (cmd != F_SETLKW)
    flags |= kNtLockfileFailImmediately;
  if (type == F_WRLCK)
    flags |= kNtLockfileExclusiveLock;
  int rc = __flocks_lockfile(f->handle, flags, off, len, waitmask);

  // success
  if (!rc) {
    fl->dev = dev;
    fl->ino = ino;
    fl->off = off;
    fl->len = len;
    fl->type = type;
    fl->handle = f->handle;
    __flocks_lock();
    __flocks_add(&fl->elem);
    __flocks_unlock();
    return 0;
  }

  // oh snap. if we had to carve up locks, then this failure could leave
  // the file locks in a different state than they were before. we tried
  // having our own transaction byte and then rolling back the resurrect
  // and deleted lock changes above. but sqlite_test only passed when we
  // used this simpler faster approach.
  __flocks_freelock(fl);
  return -1;
}

textwindows int __flocks_fcntl(struct Fd *f, int fd, int cmd, uintptr_t arg,
                               sigset_t waitmask) {

  struct flock *l = (struct flock *)arg;
  int64_t len = l->l_len;
  int64_t off = l->l_start;
  int type = l->l_type;

  // validate args
  if (cmd == F_GETLK) {
    if (type != F_WRLCK &&  //
        type != F_RDLCK)
      return einval();
  } else if (cmd == F_SETLK ||  //
             cmd == F_SETLKW) {
    if (type != F_WRLCK &&  //
        type != F_RDLCK &&  //
        type != F_UNLCK)
      return einval();
  } else {
    return einval();
  }

  // get unique id
  int64_t dev = f->dev;
  int64_t ino = f->ino;
  bool gotfileinfo = false;
  struct NtByHandleFileInformation fi;
  if (!dev || !ino) {
    // this takes 10µs or 10x longer than LockFileEx
    if (!GetFileInformationByHandle(f->handle, &fi))
      return ebadf();
    f->dev = dev = fi.dwVolumeSerialNumber;
    f->ino = ino = (fi.nFileIndexHigh + 0ull) << 32 | fi.nFileIndexLow;
    gotfileinfo = true;
  }

  // compute off
  int64_t bot;
  switch (l->l_whence) {
    case SEEK_SET:
      bot = 0;
      break;
    case SEEK_CUR:
      bot = f->cursor->shared->pointer;
      break;
    case SEEK_END:
      if (!gotfileinfo)
        if (!GetFileInformationByHandle(f->handle, &fi))
          return ebadf();
      bot = (fi.nFileSizeHigh + 0ull) << 32 | fi.nFileSizeLow;
      break;
    default:
      return einval();
  }
  if (ckd_add(&off, off, bot) || off < 0)
    return einval();

  // compute len
  if (!len)
    len = PROTECTED_BYTES - off;
  int64_t end;
  if (len < 0 || ckd_add(&end, off, len))
    return einval();

  // prevent user from locking bytes reserved for libc
  // just in case we ever decide to do transactions again
  if (end > PROTECTED_BYTES)
    return einval();

  // perform operation
  if (cmd == F_GETLK)
    return __flocks_getlk(f, l, type, off, len, dev, ino);
  return __flocks_setlk(f, l, cmd, type, off, len, dev, ino, fd, waitmask);
}
