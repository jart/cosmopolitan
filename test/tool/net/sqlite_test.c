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
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "third_party/sqlite3/sqlite3.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
  sqlite3_initialize();
}

// if we try to open a WAL database at the same time from multiple
// threads then it's likely we'll get a SQLITE_BUSY conflict since
// WAL mode does a complicated dance to initialize itself thus all
// we need to do is wait a little bit, and use exponential backoff
int DbOpen(const char *path, sqlite3 **db) {
  int i, rc;
  rc = sqlite3_open(path, db);
  if (rc != SQLITE_OK) return rc;
  for (i = 0; i < 16; ++i) {
    rc = sqlite3_exec(*db, "PRAGMA journal_mode=WAL", 0, 0, 0);
    if (rc == SQLITE_OK) break;
    if (rc != SQLITE_BUSY) return rc;
    usleep(1000L << i);
  }
  return sqlite3_exec(*db, "PRAGMA synchronous=NORMAL", 0, 0, 0);
}

int DbStep(sqlite3_stmt *stmt) {
  int i, rc;
  for (i = 0; i < 16; ++i) {
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) break;
    if (rc == SQLITE_DONE) break;
    if (rc != SQLITE_BUSY) return rc;
    usleep(1000L << i);
  }
  return rc;
}

int DbExec(sqlite3 *db, const char *sql) {
  int i, rc;
  for (i = 0; i < 16; ++i) {
    rc = sqlite3_exec(db, sql, 0, 0, 0);
    if (rc == SQLITE_OK) break;
    if (rc != SQLITE_BUSY) return rc;
    usleep(1000L << i);
  }
  return rc;
}

int DbPrepare(sqlite3 *db, sqlite3_stmt **stmt, const char *sql) {
  return sqlite3_prepare_v2(db, sql, -1, stmt, 0);
}

void *Worker(void *arg) {
  int rc;
  sqlite3 *db;
  sqlite3_stmt *stmt[2];
  static const char *const kNames[] = {"alice", "bob", "varu", "vader"};
  ASSERT_EQ(SQLITE_OK, DbOpen("foo.sqlite", &db));
  ASSERT_EQ(SQLITE_OK, DbPrepare(db, &stmt[0],
                                 "INSERT INTO t (id, name) VALUES (?1, ?2)"));
  ASSERT_EQ(SQLITE_OK, DbPrepare(db, &stmt[1],
                                 "SELECT name, COUNT(*) FROM t GROUP BY name"));
  for (int j = 0; j < 50; ++j) {
    ASSERT_EQ(SQLITE_OK, DbExec(db, "BEGIN TRANSACTION"));
    for (int i = 0; i < 4; ++i) {
      ASSERT_EQ(SQLITE_OK, sqlite3_bind_int64(stmt[0], 1, _rand64()));
      ASSERT_EQ(SQLITE_OK, sqlite3_bind_text(
                               stmt[0], 2, kNames[_rand64() % ARRAYLEN(kNames)],
                               -1, SQLITE_TRANSIENT));
      ASSERT_EQ(SQLITE_DONE, DbStep(stmt[0]));
      ASSERT_EQ(SQLITE_OK, sqlite3_reset(stmt[0]));
    }
    ASSERT_EQ(SQLITE_OK, DbExec(db, "COMMIT TRANSACTION"));
    ASSERT_EQ(SQLITE_OK, DbExec(db, "BEGIN TRANSACTION"));
    for (;;) {
      rc = DbStep(stmt[1]);
      if (rc == SQLITE_DONE) break;
      ASSERT_EQ(SQLITE_ROW, rc);
    }
    ASSERT_EQ(SQLITE_OK, sqlite3_reset(stmt[1]));
    ASSERT_EQ(SQLITE_OK, DbExec(db, "END TRANSACTION"));
  }
  ASSERT_EQ(SQLITE_OK, sqlite3_finalize(stmt[1]));
  ASSERT_EQ(SQLITE_OK, sqlite3_finalize(stmt[0]));
  ASSERT_EQ(SQLITE_OK, sqlite3_close(db));
  return 0;
}

TEST(sqlite, test) {
  sqlite3 *db;
  ASSERT_EQ(SQLITE_OK, DbOpen("foo.sqlite", &db));
  ASSERT_EQ(SQLITE_OK, DbExec(db, "PRAGMA synchronous=0"));
  ASSERT_EQ(SQLITE_OK, DbExec(db, "CREATE TABLE t (\n"
                                  "  id INTEGER,\n"
                                  "  name TEXT\n"
                                  ")"));
  ASSERT_EQ(SQLITE_OK, sqlite3_close(db));
  int i, n = 4;
  pthread_t *t = gc(malloc(sizeof(pthread_t) * n));
  for (i = 0; i < n; ++i) {
    ASSERT_EQ(0, pthread_create(t + i, 0, Worker, 0));
  }
  for (i = 0; i < n; ++i) {
    ASSERT_EQ(0, pthread_join(t[i], 0));
  }
}
