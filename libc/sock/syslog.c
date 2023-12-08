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
#include "libc/sock/syslog.h"
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/weirdtypes.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/log/internal.h"
#include "libc/macros.internal.h"
#include "libc/nt/events.h"
#include "libc/nt/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/log.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sock.h"
#include "libc/time/struct/tm.h"

/* Note: log_facility should be initialized with LOG_USER by default,
 * but since LOG_USER is not a constant value, we cannot initialize it
 * here. Set it to -1 to force the public functions to call __initlog()
 * for the first time.
 */
static int log_facility = -1;
static char16_t log_ident[32];
static int log_opt;
static int log_mask;
static uint16_t log_id; /* Used for Windows EvtID */
static int64_t log_fd = -1;

static const char *const kLogPaths[] = {
    "/dev/log",
    // "/var/run/log", // TODO: Help with XNU and FreeBSD.
};

static struct sockaddr_un log_addr = {AF_UNIX, "/dev/log"};

static int64_t Time(int64_t *tp) {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  if (tp) *tp = ts.tv_sec;
  return ts.tv_sec;
}

static void __initlog() {
  log_ident[0] = '\0';
  log_opt = LOG_ODELAY;
  log_facility = LOG_USER;
  log_mask = LOG_PRI(0xff);  // Initially use max verbosity
  log_fd = -1;
}

forceinline int is_lost_conn(int e) {
  return (e == ECONNREFUSED) || (e == ECONNRESET) || (e == ENOTCONN) ||
         (e == EPIPE);
}

static void __openlog() {
  int i;
  if (IsWindows()) {
    log_fd = RegisterEventSource(NULL, log_ident);
  } else {
    log_fd = socket(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, 0);
    if (log_fd >= 0) {
      for (i = 0; i < ARRAYLEN(kLogPaths); ++i) {
        strcpy(log_addr.sun_path, kLogPaths[i]);
        if (!connect(log_fd, (void *)&log_addr, sizeof(log_addr))) {
          return;
        }
      }
      printf("ERR: connect(openlog) failed: %s (errno=%d)\n", strerror(errno),
             errno);
    }
  }
}

/**
 * Generates a log message which will be distributed by syslogd
 *
 * Note: no errors are reported if an error occurred while logging
 *       the message.
 *
 * @param priority is a bitmask containing the facility value and
 *        the level value. If no facility value is ORed into priority,
 *        then the default value set by openlog() is used.
 *        it set to NULL, the program name is used.
 *        Level is one of LOG_EMERG, LOG_ALERT, LOG_CRIT, LOG_ERR,
 *        LOG_WARNING, LOG_NOTICE, LOG_INFO, LOG_DEBUG
 * @param message the format of the message to be processed by vprintf()
 * @param ap the va_list of arguments to be applied to the message
 * @asyncsignalsafe
 */
void vsyslog(int priority, const char *message, va_list ap) {
  char timebuf[16]; /* Store formatted time */
  time_t now;
  struct tm tm;
  char buf[1024];
  int errno_save = errno; /* Save current errno */
  int pid;
  int l, l2;
  int hlen; /* If LOG_CONS is specified, use to store the point in
             * the header message after the timestamp */
  BLOCK_CANCELATION;
  if (log_fd < 0) __openlog();
  if (!(priority & LOG_FACMASK)) priority |= log_facility;
  /* Build the time string */
  now = Time(NULL);
  gmtime_r(&now, &tm);
  strftime(timebuf, sizeof(timebuf), "%b %e %T", &tm);
  pid = (log_opt & LOG_PID) ? getpid() : 0;
  /* This is a clever trick to optionally include "[<pid>]"
   * only if pid != 0. When pid==0, the while "[%.0d]" is skipped:
   *   %s%.0d%s -> String, pidValue, String
   * Each of those %s:
   *  - if pid == 0 -> !pid is true (1), so "[" + 1 points to the
   *    NULL terminator after the "[".
   *  - if pid != 0 -> !pid is false (0), so the string printed is
   *    the "[".
   */
  l = snprintf(buf, sizeof(buf), "<%d>%s ", priority, timebuf);
  hlen = l;
  l += snprintf(buf + l, sizeof(buf) - l, "%hs%s%.0d%s: ", log_ident,
                "[" + !pid, pid, "]" + !pid);
  errno = errno_save;
  /* Append user message */
  l2 = vsnprintf(buf + l, sizeof(buf) - l, message, ap);
  if (l2 >= 0) {
    if (l2 >= sizeof(buf) - l) {
      l = sizeof(buf) - 1;
    } else {
      l += l2;
    }
    if (buf[l - 1] != '\n') {
      buf[l++] = '\n';
    }
    if (!IsWindows()) {
      /* Unix:
       * - First try to send it to syslogd
       * - If fails and LOG_CONS is provided, writes to /dev/console
       */
      if (send(log_fd, buf, l, 0) < 0 &&
          (!is_lost_conn(errno) ||
           connect(log_fd, (void *)&log_addr, sizeof(log_addr)) < 0 ||
           send(log_fd, buf, l, 0) < 0) &&
          (log_opt & LOG_CONS)) {
        int fd = open("/dev/console", O_WRONLY | O_NOCTTY);
        if (fd >= 0) {
          dprintf(fd, "%.*s", l - hlen, buf + hlen);
          close(fd);
        }
      }
    } else {
      uint16_t evtType;
      uint32_t evtID;
      const char *bufArr[] = {
          &buf[hlen]}; /* Only print the message without time*/
      /* Windows only have 3 usable event types
       * Event ID are not supported
       * For more information on message types and event IDs, see:
       *     https://docs.microsoft.com/en-us/windows/win32/eventlog/event-identifiers
       */
      priority &= LOG_PRIMASK;  // Remove facility from the priority field
      if (priority == LOG_EMERG || priority == LOG_ALERT ||
          priority == LOG_CRIT) {
        evtType = EVENTLOG_ERROR_TYPE;
        evtID = 0xe << 28 | (LOG_USER) << 16;
      } else if (priority == LOG_WARNING) {
        evtType = EVENTLOG_WARNING_TYPE;
        evtID = 0xa << 28 | (LOG_USER) << 16;
      } else {
        /* LOG_NOTICE, LOG_INFO, LOG_DEBUG */
        evtType = EVENTLOG_INFORMATION_TYPE;
        evtID = 0x6 << 28 | (LOG_USER) << 16;
      }
      ReportEventA(log_fd, evtType, /* Derived from priority */
                   0 /* Category unsupported */, evtID, /* Unsupported */
                   NULL,                                /* User SID */
                   1,                                   /* Number of strings */
                   0,                                   /* Raw data size */
                   bufArr,                              /* String(s) */
                   NULL /* Arguments */);
      ++log_id;
    }
    if (log_opt & LOG_PERROR) {
      dprintf(2, "%.*s", l - hlen, buf + hlen);
    }
  }
  ALLOW_CANCELATION;
}

/**
 * Sets log priority mask
 *
 * Modifies the log priority mask that determines which calls to
 * syslog() may be logged.
 * Log priority values are LOG_EMERG, LOG_ALERT, LOG_CRIT, LOG_ERR,
 * LOG_WARNING, LOG_NOTICE, LOG_INFO, and LOG_DEBUG.
 *
 * @param mask the new priority mask to use by syslog()
 * @return the previous log priority mask
 * @asyncsignalsafe
 */
int setlogmask(int maskpri) {
  int ret;
  if (log_facility == -1) __initlog();
  ret = log_mask;
  if (maskpri) log_mask = LOG_PRI(maskpri);
  return ret;
}

/**
 * Opens a connection to the system logger
 *
 * Calling this function before calling syslog() is optional and
 * only allow customizing the identity, options and facility of
 * the messages logged.
 *
 * @param ident a string that prepends every logged message. If
 *        it set to NULL, the program name is used.
 * @param opt specifies flags which control the operation of openlog().
 *        Only the following flags are supported:
 *         LOG_CONS = Write directly to the system console if there is
 *                    an error while sending to the system logger.
 *         LOG_NDELAY = Open the connection with the system logger
 *                      immediately instead of waiting for the first
 *                      message to be logged.
 *         LOG_ODELAY = The converse of LOG_NDELAY.
 *         LOG_PERROR = Also log the message to stderr.
 *         LOG_PID = Include the caller's PID with each message
 * @param facility specifies the default facitlity value that defines
 *        what kind of program is logging the message.
 *        Possible values are: LOG_AUTH, LOG_AUTHPRIV,
 *        LOG_CRON, LOG_DAEMON, LOG_FTP, LOG_LOCAL0 through LOG_LOCAL7,
 *        LOG_LPR, LOG_MAIL, LOG_NEWS, LOG_SYSLOG, LOG_USER (default),
 *        LOG_UUCP.
 * @asyncsignalsafe
 */
void openlog(const char *ident, int opt, int facility) {
  BLOCK_CANCELATION;
  if (log_facility == -1) __initlog();
  if (!ident) ident = firstnonnull(program_invocation_short_name, "unknown");
  tprecode8to16(log_ident, ARRAYLEN(log_ident), ident);
  log_opt = opt;
  log_facility = facility;
  log_id = 0;
  if ((opt & LOG_NDELAY) && log_fd < 0) __openlog();
  ALLOW_CANCELATION;
}

/**
 * Generates a log message which will be distributed by syslogd
 *
 * Note: no errors are reported if an error occurred while logging
 *       the message.
 *
 * @param priority is a bitmask containing the facility value and
 *        the level value. If no facility value is ORed into priority,
 *        then the default value set by openlog() is used.
 *        it set to NULL, the program name is used.
 *        Level is one of LOG_EMERG, LOG_ALERT, LOG_CRIT, LOG_ERR,
 *        LOG_WARNING, LOG_NOTICE, LOG_INFO, LOG_DEBUG
 * @param message the message formatted using the same rules as printf()
 * @asyncsignalsafe
 */
void syslog(int priority, const char *message, ...) {
  va_list ap;
  if (log_facility == -1) {
    __initlog();
  }
  if (LOG_PRI(priority) <= log_mask) {
    va_start(ap, message);
    vsyslog(priority, message, ap);
    va_end(ap);
  }
}

/**
 * Closes the file descriptor being used to write to the system logger
 *
 * Use of closelog is optional
 * @asyncsignalsafe
 */
void closelog(void) {
  if (log_facility == -1) {
    __initlog();
  }
  if (log_fd != -1) {
    if (IsWindows()) {
      DeregisterEventSource(log_fd);
    } else {
      close(log_fd);
    }
    log_fd = -1;
  }
}
