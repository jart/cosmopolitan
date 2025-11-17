/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/runtime/clktck.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/sysparam.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "third_party/sqlite3/sqlite3.h"

#define THREADS   3
#define PROCESSES 2
#define RETRIES   9999999

bool wal;
bool lockstep;
pthread_barrier_t *bar;

FIXTURE(sqlite, wal) {
  wal = true;
  lockstep = false;
}

FIXTURE(sqlite, wal_lockstep) {
  wal = true;
  lockstep = true;
}

FIXTURE(sqlite, normal_lockstep) {
  wal = false;
  lockstep = true;
}

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
  sqlite3_initialize();
}

void Sleep(long us) {
  usleep(MIN(us, 1000000 / CLK_TCK));
}

void WaitForAllThreadsAndProcesses(void) {
  if (lockstep) {
    int rc = pthread_barrier_wait(bar);
    unassert(!rc || rc == PTHREAD_BARRIER_SERIAL_THREAD);
  }
}

int DbOpen(const char *path, sqlite3 **db) {
  if (wal) {
    // if we try to open a WAL database at the same time from multiple
    // threads then it's likely we'll get a SQLITE_BUSY conflict since
    // WAL mode does a complicated dance to initialize itself thus all
    // we need to do is wait a little bit, and use exponential backoff
    int rc = sqlite3_open(path, db);
    if (rc != SQLITE_OK)
      return rc;
    for (int i = 0; i < RETRIES; ++i) {
      rc = sqlite3_exec(*db, "PRAGMA journal_mode=WAL", 0, 0, 0);
      if (rc == SQLITE_OK)
        break;
      if (rc != SQLITE_BUSY)
        return rc;
      Sleep(100L << MIN(i, 50));
    }
    // "WAL mode is safe from corruption with synchronous=NORMAL, [...]"
    // —Quoth SQLite (https://sqlite.org/pragma.html#pragma_synchronous)
    return sqlite3_exec(*db, "PRAGMA synchronous=NORMAL", 0, 0, 0);
  } else {
    return sqlite3_open(path, db);
  }
}

int DbStep(sqlite3_stmt *stmt) {
  int i, rc;
  for (i = 0; i < RETRIES; ++i) {
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
      break;
    if (rc == SQLITE_DONE)
      break;
    if (rc != SQLITE_BUSY)
      return rc;
    Sleep(100L << MIN(i, 50));
  }
  return rc;
}

int DbExec(sqlite3 *db, const char *sql) {
  int i, rc;
  for (i = 0; i < RETRIES; ++i) {
    rc = sqlite3_exec(db, sql, 0, 0, 0);
    if (rc == SQLITE_OK)
      break;
    if (rc != SQLITE_BUSY)
      return rc;
    Sleep(100L << MIN(i, 50));
  }
  return rc;
}

int DbPrepare(sqlite3 *db, sqlite3_stmt **stmt, const char *sql) {
  int i, rc;
  for (i = 0; i < RETRIES; ++i) {
    rc = sqlite3_prepare_v2(db, sql, -1, stmt, 0);
    if (rc == SQLITE_OK)
      break;
    if (rc != SQLITE_BUSY)
      return rc;
    Sleep(100L << MIN(i, 50));
  }
  return rc;
}

// test code needn't worry about power outages
int SetSynchronous(sqlite3 *db) {
  if (IsModeDbg())
    return DbExec(db, "PRAGMA synchronous=EXTRA");
  return DbExec(db, "PRAGMA synchronous=OFF");
}

void *Worker(void *arg) {
  int rc;
  sqlite3 *db;
  sqlite3_stmt *stmt[3];
  static const char *const kNames[] = {"alice", "bob", "varu", "vader"};
  ASSERT_EQ(SQLITE_OK, DbOpen("foo.sqlite", &db));
  ASSERT_EQ(SQLITE_OK, SetSynchronous(db));
  WaitForAllThreadsAndProcesses();
  ASSERT_EQ(SQLITE_OK, DbPrepare(db, &stmt[0],
                                 "INSERT INTO t (id, name) VALUES (?1, ?2)"));
  WaitForAllThreadsAndProcesses();
  ASSERT_EQ(SQLITE_OK, DbPrepare(db, &stmt[1],
                                 "SELECT name, COUNT(*) FROM t GROUP BY name"));
  ASSERT_EQ(SQLITE_OK, DbPrepare(db, &stmt[2], "DELETE FROM t WHERE id = ?1"));
  for (int j = 0; j < 20; ++j) {
    WaitForAllThreadsAndProcesses();
    ASSERT_EQ(SQLITE_OK, DbExec(db, "BEGIN TRANSACTION"));
    for (int i = 0; i < 4; ++i) {
      ASSERT_EQ(SQLITE_OK, sqlite3_bind_int64(stmt[0], 1, lemur64() % 64));
      ASSERT_EQ(SQLITE_OK, sqlite3_bind_text(
                               stmt[0], 2, kNames[lemur64() % ARRAYLEN(kNames)],
                               -1, SQLITE_TRANSIENT));
      ASSERT_EQ(SQLITE_DONE, DbStep(stmt[0]));
      ASSERT_EQ(SQLITE_OK, sqlite3_reset(stmt[0]));
    }
    ASSERT_EQ(SQLITE_OK, DbExec(db, "COMMIT TRANSACTION"));
    WaitForAllThreadsAndProcesses();
    ASSERT_EQ(SQLITE_OK, DbExec(db, "BEGIN TRANSACTION"));
    for (;;) {
      rc = DbStep(stmt[1]);
      WaitForAllThreadsAndProcesses();
      if (rc == SQLITE_DONE)
        break;
      ASSERT_EQ(SQLITE_ROW, rc);
    }
    ASSERT_EQ(SQLITE_OK, sqlite3_reset(stmt[1]));
    ASSERT_EQ(SQLITE_OK, DbExec(db, "END TRANSACTION"));
    WaitForAllThreadsAndProcesses();
    ASSERT_EQ(SQLITE_OK, sqlite3_bind_int64(stmt[2], 1, lemur64() % 64));
    ASSERT_EQ(SQLITE_DONE, DbStep(stmt[2]));
    ASSERT_EQ(SQLITE_OK, sqlite3_reset(stmt[2]));
    WaitForAllThreadsAndProcesses();
    ASSERT_EQ(SQLITE_OK, DbExec(db, "PRAGMA integrity_check"));
    if (wal) {
      WaitForAllThreadsAndProcesses();
      ASSERT_EQ(SQLITE_OK, DbExec(db, "PRAGMA wal_checkpoint"));
    }
    WaitForAllThreadsAndProcesses();
    ASSERT_EQ(SQLITE_OK, DbExec(db, "PRAGMA integrity_check"));
  }
  ASSERT_EQ(SQLITE_OK, sqlite3_finalize(stmt[2]));
  ASSERT_EQ(SQLITE_OK, sqlite3_finalize(stmt[1]));
  ASSERT_EQ(SQLITE_OK, sqlite3_finalize(stmt[0]));
  ASSERT_EQ(SQLITE_OK, sqlite3_close(db));
  return 0;
}

TEST(sqlite, test) {

  // setup database
  sqlite3 *db;
  ASSERT_EQ(SQLITE_OK, DbOpen("foo.sqlite", &db));
  ASSERT_EQ(SQLITE_OK, SetSynchronous(db));
  ASSERT_EQ(SQLITE_OK, DbExec(db, "CREATE TABLE t (\n"
                                  "  id INTEGER,\n"
                                  "  name TEXT\n"
                                  ")"));
  ASSERT_EQ(SQLITE_OK, DbExec(db, "CREATE INDEX i1 ON t (id)"));
  ASSERT_EQ(SQLITE_OK, DbExec(db, "CREATE INDEX i2 ON t (name)"));
  ASSERT_EQ(SQLITE_OK, sqlite3_close(db));

  // setup phaser
  if (lockstep) {
    pthread_barrierattr_t barattr;
    ASSERT_NE(MAP_FAILED, (bar = mmap(0, sizeof(*bar), PROT_READ | PROT_WRITE,
                                      MAP_ANONYMOUS | MAP_SHARED, -1, 0)));
    unassert(!pthread_barrierattr_init(&barattr));
    unassert(!pthread_barrierattr_setpshared(&barattr, PTHREAD_PROCESS_SHARED));
    unassert(!pthread_barrier_init(bar, &barattr, PROCESSES * THREADS));
    unassert(!pthread_barrierattr_destroy(&barattr));
  }

  // spawn processes and threads
  for (int i = 0; i < PROCESSES; ++i) {
    int pid = fork();
    ASSERT_NE(-1, pid);
    if (!pid) {
      pthread_t t[THREADS];
      for (int i = 0; i < THREADS; ++i)
        unassert(!pthread_create(&t[i], 0, Worker, 0));
      for (int i = 0; i < THREADS; ++i)
        unassert(!pthread_join(t[i], 0));
      _Exit(0);
    }
  }

  // join processes and threads
  for (;;) {
    int ws;
    int rc = wait(&ws);
    if (rc == -1) {
      ASSERT_EQ(ECHILD, errno);
      break;
    }
    ASSERT_EQ(0, ws);
  }

  // destroy phaser
  if (lockstep) {
    unassert(!pthread_barrier_destroy(bar));
    unassert(!munmap(bar, sizeof(pthread_barrier_t)));
  }
}
