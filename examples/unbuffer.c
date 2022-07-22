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
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/termios.h"
#include "libc/calls/struct/winsize.h"
#include "libc/calls/termios.h"
#include "libc/calls/ttydefaults.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/consts/w.h"

#define STR(s) s, strlen(s)

struct termios tio;
struct winsize wsz;
sigset_t chldmask, savemask;
struct sigaction ignore, saveint, savequit;
char databuf[512];
char pathbuf[PATH_MAX];

int main(int argc, char *argv[]) {
  ssize_t rc;
  const char *prog;
  size_t i, got, wrote;
  int ws, mfd, sfd, pid;

  if (argc < 2) {
    write(2, STR("usage: "));
    if (argc > 0) write(2, argv[0], strlen(argv[0]));
    write(2, STR(" prog [argv₁...]\n"));
    return 1;
  }

  if (!(prog = commandv(argv[1], pathbuf, sizeof(pathbuf)))) {
    write(2, STR("error: "));
    write(2, prog, strlen(prog));
    write(2, STR(" not found\n"));
    return 2;
  }

  bzero(&wsz, sizeof(wsz));
  wsz.ws_col = 80;
  wsz.ws_row = 45;

  bzero(&tio, sizeof(tio));
  tio.c_iflag = ICRNL | IXON | IUTF8;
  tio.c_oflag = OPOST | ONLCR;
  tio.c_cflag = CREAD | CS8;
  tio.c_lflag =
      ISIG | ICANON | ECHO | ECHOE | ECHOK | IEXTEN | ECHOCTL | ECHOKE;
  tio.c_cc[VMIN] = 1;
  tio.c_cc[VINTR] = CTRL('C');
  tio.c_cc[VQUIT] = CTRL('\\');
  tio.c_cc[VERASE] = CTRL('?');
  tio.c_cc[VKILL] = CTRL('U');
  tio.c_cc[VEOF] = CTRL('D');
  tio.c_cc[VSTART] = CTRL('Q');
  tio.c_cc[VSTOP] = CTRL('S');
  tio.c_cc[VSUSP] = CTRL('Z');
  tio.c_cc[VREPRINT] = CTRL('R');
  tio.c_cc[VDISCARD] = CTRL('O');
  tio.c_cc[VWERASE] = CTRL('W');
  tio.c_cc[VLNEXT] = CTRL('V');

  if (openpty(&mfd, &sfd, 0, &tio, &wsz)) {
    write(2, STR("error: openpty() failed\n"));
    return 3;
  }

  ignore.sa_flags = 0;
  ignore.sa_handler = SIG_IGN;
  sigemptyset(&ignore.sa_mask);
  sigaction(SIGINT, &ignore, &saveint);
  sigaction(SIGQUIT, &ignore, &savequit);
  sigemptyset(&chldmask);
  sigaddset(&chldmask, SIGCHLD);
  sigprocmask(SIG_BLOCK, &chldmask, &savemask);

  if ((pid = vfork()) == -1) {
    write(2, STR("error: vfork() failed\n"));
    return 4;
  }

  if (!pid) {
    close(0);
    dup(sfd);
    close(1);
    dup(sfd);
    close(2);
    dup(sfd);
    close(mfd);
    sigaction(SIGINT, &saveint, 0);
    sigaction(SIGQUIT, &savequit, 0);
    sigprocmask(SIG_SETMASK, &savemask, 0);
    execv(prog, argv + 1);
    return 127;
  }
  close(sfd);

  for (;;) {
    if (waitpid(pid, &ws, WNOHANG) == pid) {
      break;
    }
    rc = read(mfd, databuf, sizeof(databuf));
    if (rc == -1) {
      perror("read");
      return 5;
    }
    if (!(got = rc)) {
      continue;
    }
    for (i = 0; i < got; i += wrote) {
      rc = write(1, databuf + i, got - i);
      if (rc != -1) {
        wrote = rc;
      } else {
        write(2, STR("error: read() failed\n"));
        return 5;
      }
    }
  }

  sigaction(SIGINT, &saveint, 0);
  sigaction(SIGQUIT, &savequit, 0);
  sigprocmask(SIG_SETMASK, &savemask, 0);

  if (WIFEXITED(ws)) {
    return WEXITSTATUS(ws);
  } else {
    return 128 + WTERMSIG(ws);
  }
}
