#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/calls/calls.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/termios.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/append.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/time/struct/tm.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/musl/passwd.h"

/**
 * @fileoverview program for broadcasting message to all terminals, e.g.
 *
 *     o//examples/wall.com hello friends
 *
 * Will place some bold text into the top left corners of all terminals:
 *
 *     Broadcast message from jart@host (pts/4) (Tue Oct 11 03:52:24 2022):
 *     hello friends
 *
 * It's useful for having one terminal tab notify your other ones when
 * something like a long running job completes. In the old days back
 * when unix systems had multiple users, the wall command provided the
 * ultimate lulz especially if it's a setuid binary with root privs.
 */

#define GETOPTS "v"
#define USAGE \
  "\
Usage: wall.com [-v] [TEXTMSG] [<TEXTMSG]\n\
  -v          increase verbosity\n\
Examples:\n\
  wall.com 'hello my friends'\n\
  echo hello my friends | wall.com\n\
"

int g_verbose;

static void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, GETOPTS)) != -1) {
    switch (opt) {
      case 'v':
        g_verbose++;
        break;
      case '?':
        write(1, USAGE, sizeof(USAGE) - 1);
        exit(0);
      default:
        write(2, USAGE, sizeof(USAGE) - 1);
        exit(64);
    }
  }
}

char *GetHost(void) {
  static char host[254];
  if (gethostname(host, sizeof(host))) {
    strcpy(host, "localhost");
  }
  return host;
}

char *GetTime(void) {
  struct tm tm;
  struct timespec ts;
  clock_gettime(0, &ts);
  localtime_r(&ts.tv_sec, &tm);
  return chomp(asctime(&tm));
}

int main(int argc, char *argv[]) {
  GetOpts(argc, argv);

  // create broadcast banner
  // we use \e[1m so the text is strong & visually distinct
  // we use \e[K so it doesn't get lost in a forest of text
  // we map \n → \r\n because teletype might be in raw mode
  char *msg = 0;
  const char *s;
  appends(&msg, "\e7");    // save cursor position
  appends(&msg, "\e[H");   // goto top left corne
  appends(&msg, "\e[K");   // clear line forward
  appends(&msg, "\e[1m");  // bold text
  appendf(&msg, "Broadcast message from %s@%s", getpwuid(getuid())->pw_name,
          GetHost());
  if (isatty(0) && (s = ttyname(0))) appendf(&msg, " (%s)", s);
  appendf(&msg, " (%s):\r\n", GetTime());
  appends(&msg, "\e[K");

  // append broadcast message
  if (optind < argc) {
    // use cli arguments as message if they exist
    for (int i = 0; optind + i < argc; ++i) {
      if (i) appends(&msg, " ");
      for (s = argv[optind + i]; *s; ++s) {
        if (*s == '\n') {
          appends(&msg, "\r\n\e[K");
        } else {
          appendw(&msg, *s & 255);
        }
      }
    }
  } else {
    // otherwise use stdin as message
    ssize_t i, rc;
    char buf[512];
    while ((rc = read(0, buf, sizeof(buf))) > 0) {
      for (i = 0; i < rc; ++i) {
        if (buf[i] == '\n') {
          appends(&msg, "\r\n\e[K");  // clear line forward
        } else {
          appendw(&msg, buf[i] & 255);
        }
      }
    }
  }
  appends(&msg, "\r\n\e[K\e[0m\e8");  // restore

  // try to send message to all pseudoteletypewriters
  for (int i = 0; i < 100; ++i) {
    int fd;
    char pts[32];
    snprintf(pts, sizeof(pts), "/dev/pts/%d", i);
    if ((fd = open(pts, O_WRONLY | O_NOCTTY)) == -1) {
      if (errno == ENOENT) continue;
      if (g_verbose) perror(pts);
    }
    write(fd, msg, appendz(msg).i);
    close(fd);
  }

  return 0;
}
