/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timeval.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/libgen.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/log/internal.h"
#include "libc/log/log.h"
#include "libc/math.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"

#define kNontrivialSize (8 * 1000 * 1000)

static struct timespec vflogf_ts;

/**
 * Takes corrective action if logging is on the fritz.
 */
static void vflogf_onfail(FILE *f) {
  errno_t err;
  struct stat st;
  if (IsTiny()) return;
  err = ferror_unlocked(f);
  if (fileno_unlocked(f) != -1 &&
      (err == ENOSPC || err == EDQUOT || err == EFBIG) &&
      (fstat(fileno_unlocked(f), &st) == -1 || st.st_size > kNontrivialSize)) {
    ftruncate(fileno_unlocked(f), 0);
    fseek_unlocked(f, SEEK_SET, 0);
    f->beg = f->end = 0;
    clearerr_unlocked(f);
    fprintf_unlocked(f, "performed emergency log truncation: %s\n",
                     strerror(err));
  }
}

/**
 * Writes formatted message w/ timestamp to log.
 *
 * Timestamps are hyphenated out when multiple events happen within the
 * same second in the same process. When timestamps are crossed out, it
 * will display microseconsd as a delta elapsed time. This is useful if
 * you do something like:
 *
 *     INFOF("connecting to foo");
 *     connect(...)
 *     INFOF("connected to foo");
 *
 * In that case, the second log entry will always display the amount of
 * time that it took to connect. This is great in forking applications.
 *
 * @asyncsignalsafe
 */
void(vflogf)(unsigned level, const char *file, int line, FILE *f,
             const char *fmt, va_list va) {
  int bufmode;
  int64_t dots;
  struct tm tm;
  char buf32[32];
  const char *prog;
  const char *sign;
  struct timespec t2;
  if (!f) f = __log_file;
  if (!f) return;
  flockfile(f);
  strace_enabled(-1);
  BLOCK_SIGNALS;
  BLOCK_CANCELATION;

  // We display TIMESTAMP.MICROS normally. However, when we log multiple
  // times in the same second, we display TIMESTAMP+DELTAMICROS instead.
  t2 = timespec_real();
  if (t2.tv_sec == vflogf_ts.tv_sec) {
    sign = "+";
    dots = t2.tv_nsec - vflogf_ts.tv_nsec;
  } else {
    sign = ".";
    dots = t2.tv_nsec;
  }
  vflogf_ts = t2;

  localtime_r(&t2.tv_sec, &tm);
  strcpy(iso8601(buf32, &tm), sign);
  prog = basename(firstnonnull(program_invocation_name, "unknown"));
  bufmode = f->bufmode;
  if (bufmode == _IOLBF) f->bufmode = _IOFBF;

  if ((fprintf_unlocked)(f, "%r%c%s%06ld:%s:%d:%.*s:%d] ",
                         "FEWIVDNT"[level & 7], buf32, dots / 1000, file, line,
                         strchrnul(prog, '.') - prog, prog, getpid()) <= 0) {
    vflogf_onfail(f);
  }
  (vfprintf_unlocked)(f, fmt, va);
  fputc_unlocked('\n', f);
  if (bufmode == _IOLBF) {
    f->bufmode = _IOLBF;
    fflush_unlocked(f);
  }

  if (level == kLogFatal) {
    __start_fatal(file, line);
    strcpy(buf32, "unknown");
    gethostname(buf32, sizeof(buf32));
    (dprintf)(STDERR_FILENO,
              "exiting due to aforementioned error (host %s pid %d tid %d)\n",
              buf32, getpid(), gettid());
    _Exit(22);
  }

  ALLOW_CANCELATION;
  ALLOW_SIGNALS;
  strace_enabled(+1);
  funlockfile(f);
}
