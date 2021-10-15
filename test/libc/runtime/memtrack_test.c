/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/mem/mem.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

static bool AreMemoryIntervalsEqual(const struct MemoryIntervals *mm1,
                                    const struct MemoryIntervals *mm2) {
  if (mm1->i != mm2->i) return false;
  if (memcmp(mm1->p, mm2->p, mm1->i * sizeof(*mm2->p)) != 0) return false;
  return true;
}

static void PrintMemoryInterval(const struct MemoryIntervals *mm) {
  int i;
  for (i = 0; i < mm->i; ++i) {
    if (i) fprintf(stderr, ",");
    fprintf(stderr, "{%d,%d}", mm->p[i].x, mm->p[i].y);
  }
  fprintf(stderr, "\n");
}

static void CheckMemoryIntervalsEqual(const struct MemoryIntervals *mm1,
                                      const struct MemoryIntervals *mm2) {
  if (!AreMemoryIntervalsEqual(mm1, mm2)) {
    PrintMemoryInterval(mm1);
    PrintMemoryInterval(mm2);
    CHECK(!"memory intervals not equal");
    exit(1);
  }
}

static void CheckMemoryIntervalsAreOk(const struct MemoryIntervals *mm) {
  if (!AreMemoryIntervalsOk(mm)) {
    PrintMemoryInterval(mm);
    CHECK(!"memory intervals not ok");
    exit(1);
  }
}

static void RunTrackMemoryIntervalTest(const struct MemoryIntervals t[2], int x,
                                       int y, long h) {
  struct MemoryIntervals *mm;
  mm = memcpy(malloc(sizeof(*t)), t, sizeof(*t));
  CheckMemoryIntervalsAreOk(mm);
  CHECK_NE(-1, TrackMemoryInterval(mm, x, y, h, 0, 0));
  CheckMemoryIntervalsAreOk(mm);
  CheckMemoryIntervalsEqual(mm, t + 1);
  free(mm);
}

static int RunReleaseMemoryIntervalsTest(const struct MemoryIntervals t[2],
                                         int x, int y) {
  int rc;
  struct MemoryIntervals *mm;
  mm = memcpy(malloc(sizeof(*t)), t, sizeof(*t));
  CheckMemoryIntervalsAreOk(mm);
  if ((rc = ReleaseMemoryIntervals(mm, x, y, NULL)) != -1) {
    CheckMemoryIntervalsAreOk(mm);
    CheckMemoryIntervalsEqual(t + 1, mm);
  }
  free(mm);
  return rc;
}

TEST(TrackMemoryInterval, TestEmpty) {
  static struct MemoryIntervals mm[2] = {
      {0, OPEN_MAX, 0, {}},
      {1, OPEN_MAX, 0, {{2, 2, 0}}},
  };
  mm[0].p = mm[0].s;
  mm[1].p = mm[1].s;
  RunTrackMemoryIntervalTest(mm, 2, 2, 0);
}

TEST(TrackMemoryInterval, TestFull) {
#if 0  // TODO(jart): Find way to re-enable
  int i;
  struct MemoryIntervals *mm;
  mm = calloc(1, sizeof(struct MemoryIntervals));
  for (i = 0; i < mm->n; ++i) {
    CheckMemoryIntervalsAreOk(mm);
    CHECK_NE(-1, TrackMemoryInterval(mm, i, i, i, 0, 0));
    CheckMemoryIntervalsAreOk(mm);
  }
  CHECK_EQ(-1, TrackMemoryInterval(mm, i, i, i, 0, 0));
  CHECK_EQ(ENOMEM, errno);
  CheckMemoryIntervalsAreOk(mm);
  free(mm);
#endif
}

TEST(TrackMemoryInterval, TestAppend) {
  static struct MemoryIntervals mm[2] = {
      {1, OPEN_MAX, 0, {{2, 2}}},
      {1, OPEN_MAX, 0, {{2, 3}}},
  };
  mm[0].p = mm[0].s;
  mm[1].p = mm[1].s;
  RunTrackMemoryIntervalTest(mm, 3, 3, 0);
}

TEST(TrackMemoryInterval, TestPrepend) {
  static struct MemoryIntervals mm[2] = {
      {1, OPEN_MAX, 0, {{2, 2}}},
      {1, OPEN_MAX, 0, {{1, 2}}},
  };
  mm[0].p = mm[0].s;
  mm[1].p = mm[1].s;
  RunTrackMemoryIntervalTest(mm, 1, 1, 0);
}

TEST(TrackMemoryInterval, TestFillHole) {
  static struct MemoryIntervals mm[2] = {
      {4, OPEN_MAX, 0, {{1, 1}, {3, 4}, {5, 5, 1}, {6, 8}}},
      {3, OPEN_MAX, 0, {{1, 4}, {5, 5, 1}, {6, 8}}},
  };
  mm[0].p = mm[0].s;
  mm[1].p = mm[1].s;
  RunTrackMemoryIntervalTest(mm, 2, 2, 0);
}

TEST(TrackMemoryInterval, TestAppend2) {
  static struct MemoryIntervals mm[2] = {
      {1, OPEN_MAX, 0, {{2, 2}}},
      {2, OPEN_MAX, 0, {{2, 2}, {3, 3, 1}}},
  };
  mm[0].p = mm[0].s;
  mm[1].p = mm[1].s;
  RunTrackMemoryIntervalTest(mm, 3, 3, 1);
}

TEST(TrackMemoryInterval, TestPrepend2) {
  static struct MemoryIntervals mm[2] = {
      {1, OPEN_MAX, 0, {{2, 2}}},
      {2, OPEN_MAX, 0, {{1, 1, 1}, {2, 2}}},
  };
  mm[0].p = mm[0].s;
  mm[1].p = mm[1].s;
  RunTrackMemoryIntervalTest(mm, 1, 1, 1);
}

TEST(TrackMemoryInterval, TestFillHole2) {
  static struct MemoryIntervals mm[2] = {
      {4, OPEN_MAX, 0, {{1, 1}, {3, 4}, {5, 5, 1}, {6, 8}}},
      {5, OPEN_MAX, 0, {{1, 1}, {2, 2, 1}, {3, 4}, {5, 5, 1}, {6, 8}}},
  };
  mm[0].p = mm[0].s;
  mm[1].p = mm[1].s;
  RunTrackMemoryIntervalTest(mm, 2, 2, 1);
}

TEST(FindMemoryInterval, Test) {
  static struct MemoryIntervals mm[1] = {
      {
          4,
          OPEN_MAX,
          0,
          {
              [0] = {1, 1},
              [1] = {3, 4},
              [2] = {5, 5, 1},
              [3] = {6, 8},
          },
      },
  };
  mm[0].p = mm[0].s;
  EXPECT_EQ(0, FindMemoryInterval(mm, 0));
  EXPECT_EQ(0, FindMemoryInterval(mm, 1));
  EXPECT_EQ(1, FindMemoryInterval(mm, 2));
  EXPECT_EQ(1, FindMemoryInterval(mm, 3));
  EXPECT_EQ(1, FindMemoryInterval(mm, 4));
  EXPECT_EQ(2, FindMemoryInterval(mm, 5));
  EXPECT_EQ(3, FindMemoryInterval(mm, 6));
  EXPECT_EQ(3, FindMemoryInterval(mm, 7));
  EXPECT_EQ(3, FindMemoryInterval(mm, 8));
  EXPECT_EQ(4, FindMemoryInterval(mm, 9));
}

TEST(ReleaseMemoryIntervals, TestEmpty) {
  static struct MemoryIntervals mm[2] = {
      {0, OPEN_MAX, 0, {}},
      {0, OPEN_MAX, 0, {}},
  };
  mm[0].p = mm[0].s;
  mm[1].p = mm[1].s;
  EXPECT_NE(-1, RunReleaseMemoryIntervalsTest(mm, 2, 2));
}

TEST(ReleaseMemoryIntervals, TestRemoveElement_UsesInclusiveRange) {
  static struct MemoryIntervals mm[2] = {
      {3, OPEN_MAX, 0, {{0, 0}, {2, 2}, {4, 4}}},
      {2, OPEN_MAX, 0, {{0, 0}, {4, 4}}},
  };
  mm[0].p = mm[0].s;
  mm[1].p = mm[1].s;
  EXPECT_NE(-1, RunReleaseMemoryIntervalsTest(mm, 2, 2));
}

TEST(ReleaseMemoryIntervals, TestPunchHole) {
  static struct MemoryIntervals mm[2] = {
      {1, OPEN_MAX, 0, {{0, 9}}},
      {2, OPEN_MAX, 0, {{0, 3}, {6, 9}}},
  };
  mm[0].p = mm[0].s;
  mm[1].p = mm[1].s;
  EXPECT_NE(-1, RunReleaseMemoryIntervalsTest(mm, 4, 5));
}

TEST(ReleaseMemoryIntervals, TestShortenLeft) {
  if (IsWindows()) return;
  static struct MemoryIntervals mm[2] = {
      {1, OPEN_MAX, 0, {{0, 9}}},
      {1, OPEN_MAX, 0, {{0, 7}}},
  };
  mm[0].p = mm[0].s;
  mm[1].p = mm[1].s;
  EXPECT_NE(-1, RunReleaseMemoryIntervalsTest(mm, 8, 9));
}

TEST(ReleaseMemoryIntervals, TestShortenRight) {
  if (IsWindows()) return;
  static struct MemoryIntervals mm[2] = {
      {1, OPEN_MAX, 0, {{0, 9}}},
      {1, OPEN_MAX, 0, {{3, 9}}},
  };
  mm[0].p = mm[0].s;
  mm[1].p = mm[1].s;
  EXPECT_NE(-1, RunReleaseMemoryIntervalsTest(mm, 0, 2));
}

TEST(ReleaseMemoryIntervals, TestShortenLeft2) {
  if (IsWindows()) return;
  static struct MemoryIntervals mm[2] = {
      {1, OPEN_MAX, 0, {{0, 9}}},
      {1, OPEN_MAX, 0, {{0, 7}}},
  };
  mm[0].p = mm[0].s;
  mm[1].p = mm[1].s;
  EXPECT_NE(-1, RunReleaseMemoryIntervalsTest(mm, 8, 11));
}

TEST(ReleaseMemoryIntervals, TestShortenRight2) {
  if (IsWindows()) return;
  static struct MemoryIntervals mm[2] = {
      {1, OPEN_MAX, 0, {{0, 9}}},
      {1, OPEN_MAX, 0, {{3, 9}}},
  };
  mm[0].p = mm[0].s;
  mm[1].p = mm[1].s;
  EXPECT_NE(-1, RunReleaseMemoryIntervalsTest(mm, -3, 2));
}

TEST(ReleaseMemoryIntervals, TestZeroZero) {
  static struct MemoryIntervals mm[2] = {
      {1, OPEN_MAX, 0, {{3, 9}}},
      {1, OPEN_MAX, 0, {{3, 9}}},
  };
  mm[0].p = mm[0].s;
  mm[1].p = mm[1].s;
  EXPECT_NE(-1, RunReleaseMemoryIntervalsTest(mm, 0, 0));
}

TEST(ReleaseMemoryIntervals, TestNoopLeft) {
  static struct MemoryIntervals mm[2] = {
      {1, OPEN_MAX, 0, {{3, 9}}},
      {1, OPEN_MAX, 0, {{3, 9}}},
  };
  mm[0].p = mm[0].s;
  mm[1].p = mm[1].s;
  EXPECT_NE(-1, RunReleaseMemoryIntervalsTest(mm, 1, 2));
}

TEST(ReleaseMemoryIntervals, TestNoopRight) {
  static struct MemoryIntervals mm[2] = {
      {1, OPEN_MAX, 0, {{3, 9}}},
      {1, OPEN_MAX, 0, {{3, 9}}},
  };
  mm[0].p = mm[0].s;
  mm[1].p = mm[1].s;
  EXPECT_NE(-1, RunReleaseMemoryIntervalsTest(mm, 10, 10));
}

TEST(ReleaseMemoryIntervals, TestBigFree) {
  static struct MemoryIntervals mm[2] = {
      {2, OPEN_MAX, 0, {{0, 3}, {6, 9}}},
      {0, OPEN_MAX, 0, {}},
  };
  mm[0].p = mm[0].s;
  mm[1].p = mm[1].s;
  EXPECT_NE(-1, RunReleaseMemoryIntervalsTest(mm, INT_MIN, INT_MAX));
}

TEST(ReleaseMemoryIntervals, TestWeirdGap) {
  static struct MemoryIntervals mm[2] = {
      {3, OPEN_MAX, 0, {{10, 10}, {20, 20}, {30, 30}}},
      {2, OPEN_MAX, 0, {{10, 10}, {30, 30}}},
  };
  mm[0].p = mm[0].s;
  mm[1].p = mm[1].s;
  EXPECT_NE(-1, RunReleaseMemoryIntervalsTest(mm, 15, 25));
}
