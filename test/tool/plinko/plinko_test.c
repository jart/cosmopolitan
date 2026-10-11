#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"

__static_yoink("zipos");
__static_yoink("plinko");
__static_yoink("library.lisp");
__static_yoink("library_test.lisp");
__static_yoink("plan_test.lisp");
__static_yoink("expand_test.lisp");
__static_yoink("binarytrees.lisp");
__static_yoink("algebra.lisp");
__static_yoink("algebra_test.lisp");
__static_yoink("infix.lisp");
__static_yoink("kanren.lisp");
__static_yoink("kanren_test.lisp");
__static_yoink("ok.lisp");

static const char *const kSauces[] = {
    "/zip/library.lisp",       //
    "/zip/library_test.lisp",  //
    "/zip/plan_test.lisp",     //
    "/zip/expand_test.lisp",   //
    "/zip/binarytrees.lisp",   //
    "/zip/algebra.lisp",       //
    "/zip/algebra_test.lisp",  //
    "/zip/infix.lisp",         //
    "/zip/kanren.lisp",        //
    "/zip/kanren_test.lisp",   //
    "/zip/ok.lisp",            //
};

void SetUpOnce(void) {
  int fdin, fdout;
  testlib_enable_tmp_setup_teardown_once();
  ASSERT_NE(-1, mkdir("bin", 0755));
  ASSERT_NE(-1, (fdin = open("/zip/plinko", O_RDONLY)));
  ASSERT_NE(-1, (fdout = creat("bin/plinko", 0755)));
  ASSERT_NE(-1, copyfd(fdin, fdout, -1));
  EXPECT_EQ(0, close(fdout));
  EXPECT_EQ(0, close(fdin));
}

static void RunPlinko(const char *flag) {
  ssize_t rc, got;
  char buf[1024], drain[64];
  sigset_t chldmask, savemask;
  int i, pid, fdin, wstatus, pfds[2][2];
  struct sigaction ignore, saveint, savequit, savepipe;
  ignore.sa_flags = 0;
  ignore.sa_handler = SIG_IGN;
  sigemptyset(&ignore.sa_mask);
  sigaction(SIGINT, &ignore, &saveint);
  sigaction(SIGQUIT, &ignore, &savequit);
  sigaction(SIGPIPE, &ignore, &savepipe);
  sigemptyset(&chldmask);
  sigaddset(&chldmask, SIGCHLD);
  sigprocmask(SIG_BLOCK, &chldmask, &savemask);
  ASSERT_NE(-1, pipe2(pfds[0], O_CLOEXEC));
  ASSERT_NE(-1, pipe2(pfds[1], O_CLOEXEC));
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    __strace = 0;
    __ftrace = 0;
    close(0), dup(pfds[0][0]);
    close(1), dup(pfds[1][1]);
    close(2), dup(pfds[1][1]);
    sigaction(SIGINT, &saveint, 0);
    sigaction(SIGQUIT, &savequit, 0);
    sigaction(SIGPIPE, &savepipe, 0);
    sigprocmask(SIG_SETMASK, &savemask, 0);
    execve("bin/plinko", (char *const[]){"bin/plinko", (char *)flag, 0},
           (char *const[]){0});
    _exit(127);
  }
  close(pfds[0][0]);
  close(pfds[1][1]);
  for (i = 0; i < ARRAYLEN(kSauces); ++i) {
    EXPECT_NE(-1, (fdin = open(kSauces[i], O_RDONLY)));
    rc = copyfd(fdin, pfds[0][1], -1);
    if (rc == -1)
      EXPECT_EQ(EPIPE, errno);
    EXPECT_NE(-1, close(fdin));
  }
  EXPECT_NE(-1, close(pfds[0][1]));
  bzero(buf, sizeof(buf));
  ASSERT_NE(-1, (got = read(pfds[1][0], buf, sizeof(buf) - 1)));
  EXPECT_NE(0, got);
  while (read(pfds[1][0], drain, sizeof(drain)) > 0)
    donothing;
  EXPECT_NE(-1, close(pfds[1][0]));
  EXPECT_NE(-1, waitpid(pid, &wstatus, 0));
  EXPECT_TRUE(WIFEXITED(wstatus));
  EXPECT_STREQ("OKCOMPUTER\n", buf);
  EXPECT_EQ(0, WEXITSTATUS(wstatus));
  sigaction(SIGINT, &saveint, 0);
  sigaction(SIGQUIT, &savequit, 0);
  sigaction(SIGPIPE, &savepipe, 0);
  sigprocmask(SIG_SETMASK, &savemask, 0);
  if (g_testlib_failed) {
    kprintf("note: got the following in pipe: %s%n", buf);
  }
}

TEST(plinko, works) {
  RunPlinko(0);
}

// -P disables the planner's optimizations, so the same tests passing in
// both modes shows the optimized dispatches behave like the plain ones
TEST(plinko, worksUnplanned) {
  RunPlinko("-P");
}
