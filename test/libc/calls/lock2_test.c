/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/assert.h"
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/flock.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(lock, wholeFile) {
  ASSERT_SYS(0, 3, open("db", O_RDWR | O_CREAT | O_EXCL, 0644));
  ASSERT_SYS(0, 0, fcntl(3, F_SETLK, &(struct flock){.l_type = F_RDLCK}));
  ASSERT_SYS(0, 0, fcntl(3, F_SETLK, &(struct flock){.l_type = F_UNLCK}));
  ASSERT_SYS(0, 0, close(3));
}

TEST(lock, testUpgradeFromReadToWriteLock) {
  ASSERT_SYS(0, 3, open("db", O_RDWR | O_CREAT | O_EXCL, 0644));
  ASSERT_SYS(0, 0,
             fcntl(3, F_SETLK,
                   &(struct flock){
                       .l_type = F_RDLCK,
                       .l_start = 0x40000000,
                       .l_len = 1,
                   }));
  ASSERT_SYS(0, 0,
             fcntl(3, F_SETLK,
                   &(struct flock){
                       .l_type = F_WRLCK,
                       .l_start = 0x40000000,
                       .l_len = 1,
                   }));
  ASSERT_SYS(0, 0, close(3));
}

TEST(lock, testUpgradeWriteToWriteLock) {
  ASSERT_SYS(0, 3, open("db", O_RDWR | O_CREAT | O_EXCL, 0644));
  ASSERT_SYS(0, 0,
             fcntl(3, F_SETLK,
                   &(struct flock){
                       .l_type = F_WRLCK,
                       .l_start = 0x40000000,
                       .l_len = 1,
                   }));
  ASSERT_SYS(0, 0,
             fcntl(3, F_SETLK,
                   &(struct flock){
                       .l_type = F_WRLCK,
                       .l_start = 0x40000000,
                       .l_len = 1,
                   }));
  ASSERT_SYS(0, 0, close(3));
}

TEST(lock, unlockEverything_unlocksSmallerRanges) {
  int fd, pi[2];
  char buf[8] = {0};
  ASSERT_SYS(0, 3, creat("db", 0644));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 0, pipe(pi));
  SPAWN(fork);
  ASSERT_SYS(0, 5, open("db", O_RDWR));
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 8, read(3, buf, 8));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 0,
             fcntl(5, F_SETLK,
                   &(struct flock){
                       .l_type = F_WRLCK,
                       .l_start = 0x40000000,
                       .l_len = 1,
                   }));
  ASSERT_SYS(0, 0,
             fcntl(5, F_SETLK,
                   &(struct flock){
                       .l_type = F_WRLCK,
                       .l_start = 0x40000005,
                       .l_len = 1,
                   }));
  ASSERT_SYS(0, 0, close(5));
  PARENT();
  ASSERT_SYS(0, 0, close(3));
  ASSERT_NE(-1, (fd = open("db", O_RDWR)));
  ASSERT_SYS(0, 0,
             fcntl(fd, F_SETLK,
                   &(struct flock){
                       .l_type = F_WRLCK,
                       .l_start = 0x40000000,
                       .l_len = 1,
                   }));
  ASSERT_SYS(0, 0,
             fcntl(fd, F_SETLK,
                   &(struct flock){
                       .l_type = F_WRLCK,
                       .l_start = 0x40000005,
                       .l_len = 1,
                   }));
  ASSERT_SYS(0, 0,
             fcntl(fd, F_SETLK,
                   &(struct flock){
                       .l_type = F_UNLCK,
                   }));
  ASSERT_SYS(0, 8, write(4, buf, 8));
  ASSERT_SYS(0, 0, close(4));
  WAIT(exit, 0);
  ASSERT_SYS(0, 0, close(fd));
}

TEST(lock, close_releasesLocks) {
  int fd, pi[2];
  char buf[8] = {0};
  ASSERT_SYS(0, 3, creat("db", 0644));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 0, pipe(pi));
  SPAWN(fork);
  ASSERT_SYS(0, 5, open("db", O_RDWR));
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 8, read(3, buf, 8));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 0,
             fcntl(5, F_SETLK,
                   &(struct flock){
                       .l_type = F_WRLCK,
                       .l_start = 0x40000000,
                       .l_len = 1,
                   }));
  ASSERT_SYS(0, 0, close(5));
  PARENT();
  ASSERT_SYS(0, 0, close(3));
  ASSERT_NE(-1, (fd = open("db", O_RDWR)));
  ASSERT_SYS(0, 0,
             fcntl(fd, F_SETLK,
                   &(struct flock){
                       .l_type = F_WRLCK,
                       .l_start = 0x40000000,
                       .l_len = 1,
                   }));
  ASSERT_SYS(0, 0, close(fd));
  ASSERT_SYS(0, 8, write(4, buf, 8));
  ASSERT_SYS(0, 0, close(4));
  WAIT(exit, 0);
}

TEST(lock, getWriteLockSameProcess_canBeUnlocked) {
  struct flock lock;
  ASSERT_SYS(0, 3, open("db", O_RDWR | O_CREAT | O_EXCL, 0644));
  ASSERT_SYS(0, 0, ftruncate(3, 1));
  ASSERT_SYS(0, 0,
             fcntl(3, F_SETLK,
                   &(struct flock){
                       .l_type = F_WRLCK,
                       .l_start = 0,
                       .l_len = 1,
                   }));
  lock.l_type = F_RDLCK;
  lock.l_whence = SEEK_END;
  lock.l_start = -1;
  lock.l_len = 1;
  lock.l_pid = 123;
  ASSERT_SYS(0, 0, fcntl(3, F_GETLK, &lock));
  EXPECT_EQ(F_UNLCK, lock.l_type);
  EXPECT_EQ(SEEK_END, lock.l_whence);
  EXPECT_EQ(-1, lock.l_start);
  EXPECT_EQ(1, lock.l_len);
  EXPECT_EQ(123, lock.l_pid);
  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 1;
  lock.l_pid = 123;
  ASSERT_SYS(0, 0, fcntl(3, F_GETLK, &lock));
  EXPECT_EQ(F_UNLCK, lock.l_type);
  EXPECT_EQ(SEEK_SET, lock.l_whence);
  EXPECT_EQ(0, lock.l_start);
  EXPECT_EQ(1, lock.l_len);
  EXPECT_EQ(123, lock.l_pid);
  ASSERT_SYS(0, 0, close(3));
}

TEST(lock, getReadLockSameProcess_canBeUnlocked) {
  struct flock lock;
  ASSERT_SYS(0, 3, open("db", O_RDWR | O_CREAT | O_EXCL, 0644));
  ASSERT_SYS(0, 0, ftruncate(3, 1));
  ASSERT_SYS(0, 0,
             fcntl(3, F_SETLK,
                   &(struct flock){
                       .l_type = F_RDLCK,
                       .l_start = 0,
                       .l_len = 1,
                   }));
  lock.l_type = F_RDLCK;
  lock.l_whence = SEEK_END;
  lock.l_start = -1;
  lock.l_len = 1;
  lock.l_pid = 123;
  ASSERT_SYS(0, 0, fcntl(3, F_GETLK, &lock));
  EXPECT_EQ(F_UNLCK, lock.l_type);
  EXPECT_EQ(SEEK_END, lock.l_whence);
  EXPECT_EQ(-1, lock.l_start);
  EXPECT_EQ(1, lock.l_len);
  EXPECT_EQ(123, lock.l_pid);
  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 1;
  lock.l_pid = 123;
  ASSERT_SYS(0, 0, fcntl(3, F_GETLK, &lock));
  EXPECT_EQ(F_UNLCK, lock.l_type);
  EXPECT_EQ(SEEK_SET, lock.l_whence);
  EXPECT_EQ(0, lock.l_start);
  EXPECT_EQ(1, lock.l_len);
  EXPECT_EQ(123, lock.l_pid);
  ASSERT_SYS(0, 0, close(3));
}

TEST(lock, getLock_sharedLockAcrossProcesses_isUnlocked) {
  struct flock lock;
  ASSERT_SYS(0, 3, open("db", O_RDWR | O_CREAT | O_EXCL, 0644));
  ASSERT_SYS(0, 0, ftruncate(3, 1));
  ASSERT_SYS(0, 0,
             fcntl(3, F_SETLK,
                   &(struct flock){
                       .l_type = F_RDLCK,
                       .l_start = 0,
                       .l_len = 1,
                   }));
  SPAWN(fork);
  lock.l_type = F_RDLCK;
  lock.l_whence = SEEK_END;
  lock.l_start = -1;
  lock.l_len = 1;
  lock.l_pid = 666;
  ASSERT_SYS(0, 0, fcntl(3, F_GETLK, &lock));
  ASSERT_EQ(F_UNLCK, lock.l_type);
  ASSERT_EQ(SEEK_END, lock.l_whence);
  ASSERT_EQ(-1, lock.l_start);
  ASSERT_EQ(1, lock.l_len);
  ASSERT_EQ(666, lock.l_pid);  // l_pid isn't modified
  EXITS(0);
  ASSERT_SYS(0, 0, close(3));
}

TEST(lock, getLock_exclusiveLockAcrossProcesses_isWriteLock) {
  struct flock lock;
  ASSERT_SYS(0, 3, open("db", O_RDWR | O_CREAT | O_EXCL, 0644));
  ASSERT_SYS(0, 0,
             fcntl(3, F_SETLK,
                   &(struct flock){
                       .l_type = F_WRLCK,
                       .l_start = 0,
                       .l_len = 1,
                   }));
  SPAWN(fork);
  lock.l_type = F_RDLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 1;
  ASSERT_SYS(0, 0, fcntl(3, F_GETLK, &lock));
  ASSERT_EQ(F_WRLCK, lock.l_type);
  ASSERT_EQ(SEEK_SET, lock.l_whence);
  ASSERT_EQ(0, lock.l_start);
  ASSERT_EQ(1, lock.l_len);
  ASSERT_EQ(IsWindows() ? -1 : getppid(), lock.l_pid);
  EXITS(0);
  ASSERT_SYS(0, 0, close(3));
}

TEST(lock, seekEndUnderflowsEof_isInvalid) {
  struct flock lock;
  ASSERT_SYS(0, 3, open("db", O_RDWR | O_CREAT | O_EXCL, 0644));
  ASSERT_SYS(0, 0,
             fcntl(3, F_SETLK,
                   &(struct flock){
                       .l_type = F_RDLCK,
                       .l_start = 0,
                       .l_len = 1,
                   }));
  lock.l_type = F_RDLCK;
  lock.l_whence = SEEK_END;
  lock.l_start = -1;
  lock.l_len = 1;
  ASSERT_SYS(EINVAL, -1, fcntl(3, F_GETLK, &lock));
  ASSERT_SYS(0, 0, close(3));
}

TEST(lock, getLockWithUnlck_isInvalid) {
  if (IsFreebsd() || IsNetbsd() || IsOpenbsd() || IsXnu())
    return;  // they don't fail it, who knows what they do
  struct flock lock;
  ASSERT_SYS(0, 3, open("db", O_RDWR | O_CREAT | O_EXCL, 0644));
  ASSERT_SYS(0, 0,
             fcntl(3, F_SETLK,
                   &(struct flock){
                       .l_type = F_WRLCK,
                       .l_start = 0,
                       .l_len = 1,
                   }));
  lock.l_type = F_UNLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 1;
  ASSERT_SYS(EINVAL, -1, fcntl(3, F_GETLK, &lock));
  ASSERT_SYS(0, 0, close(3));
}

void OtherProcess(atomic_bool *file_is_locked) {
  for (;;)
    if (atomic_load(file_is_locked))
      break;
  ASSERT_SYS(0, 3, open("db", O_RDWR));
  // here's our innocent process, patiently waiting for the lock. you
  // would think this would deadlock since the main thread calls wait
  // before releasing the lock...
  ASSERT_SYS(0, 0, fcntl(3, F_SETLKW, &(struct flock){.l_type = F_WRLCK}));
  ASSERT_SYS(0, 0, close(3));
  _Exit(0);
}

void *RogueThread(void *arg) {
  // unfortunately this other thread comes straight out of the cut in
  // the main process and opens up the database file without actually
  // doing anything with it, except calling close. that kills all the
  // locks made by other threads on that same file. it is because the
  // locks aren't actually owned by file descriptors. locks associate
  // with the device and inode.
  ASSERT_SYS(0, 4, open("db", O_RDWR));
  ASSERT_SYS(0, 0, close(4));
  return 0;
}

TEST(lock, closeKillsAllLocksOnDevInoEvenWhenItIsntUsingLocks) {
  atomic_bool *file_is_locked;
  unassert((file_is_locked = _mapshared(sizeof(atomic_bool))));
  int child = fork();
  ASSERT_NE(-1, child);
  if (!child)
    OtherProcess(file_is_locked);
  ASSERT_SYS(0, 3, open("db", O_RDWR | O_CREAT, 0644));
  ASSERT_SYS(0, 0, fcntl(3, F_SETLK, &(struct flock){.l_type = F_WRLCK}));
  atomic_store(file_is_locked, true);
  pthread_t th;
  ASSERT_EQ(0, pthread_create(&th, 0, RogueThread, 0));
  ASSERT_EQ(0, pthread_join(th, 0));
  int ws;
  ASSERT_SYS(0, child, wait(&ws));
  ASSERT_EQ(0, ws);
  ASSERT_SYS(0, 0, close(3));
  unassert(!munmap(file_is_locked, sizeof(atomic_bool)));
}
