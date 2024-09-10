#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

// this program is used by jart for manually testing teletype interrupts
// and canonical mode line editing. this file documents the hidden depth
// of 1960's era computer usage, that's entrenched in primitive i/o apis
//
// manual testing checklist:
//
// - "hello" enter echos "got: hello^J"
//
// - "hello" ctrl-d echos "got: hello"
//
// - "hello" ctrl-r echos "^R\nhello"
//
// - "hello" ctrl-u enter echos "got: ^J"
//
// - ctrl-d during i/o task prints "got eof" and exits
//
// - ctrl-d during cpu task gets delayed until read() is called
//
// - ctrl-c during cpu task echos ^C, then calls SignalHandler()
//   asynchronously, and program exits
//
// - ctrl-c during i/o task echos ^C, then calls SignalHandler()
//   asynchronously, read() raises EINTR, and program exits
//
// - ctrl-v ctrl-c should echo "^\b" then echo "^C" and insert "\3"
//
// - ctrl-v ctrl-d should echo "^\b" then echo "^D" and insert "\4"
//

volatile bool gotsig;

void SignalHandler(int sig) {
  // we don't need to do anything in our signal handler since the signal
  // delivery itself causes a visible state change, saying what happened
  const char *s = "SignalHandler() called\n";
  write(1, s, strlen(s));  // notice both functions are @asyncsignalsafe
  // however this will help if delivered asynchronously in cpubound code
  // it's also necessary to discern spurious interrupts from real signal
  gotsig = true;
}

// this is the easiest way to write a string literal to standard output,
// without formatting. printf() has an enormous binary footprint so it's
// nice to avoid linking that when it is not needed.
#define WRITE(sliteral) write(1, sliteral, sizeof(sliteral) - 1)

int main(int argc, char *argv[]) {

  WRITE("echoing stdin until ctrl+c is pressed\n");

  // when you type ctrl-c, by default it'll kill the process, unless you
  // define a SIGINT handler. there's multiple ways to do it. the common
  // way is to say signal(SIGINT, func) which is normally defined to put
  // the signal handler in Berkeley-style SA_RESTART mode. that means if
  // a signal handler is called while inside a function like read() then
  // the read operation will keep going afterwards like nothing happened
  // which can make it difficult to break your event loop. to avoid this
  // we can use sigaction() without specifying SA_RESTART in sa_flag and
  // that'll put the signal in system v mode. this means that whenever a
  // signal handler function in your program is called during an i/o op,
  // that i/o op will return an EINTR error, so you can churn your loop.
  // don't take that error too seriously though since SIGINT can also be
  // delivered asynchronously, during the times you're crunching numbers
  // rather than performing i/o which means you get no EINTR to warn you
  sigaction(SIGINT, &(struct sigaction){.sa_handler = SignalHandler}, 0);

  for (;;) {

    // asynchronous signals are needed to interrupt math, which we shall
    // simulate here. signals can happen any time any place. that's only
    // not the case when you use sigprocmask() to block signals which is
    // useful for kicking the can down the road.
    WRITE("doing cpu task...\n");
    for (volatile int i = 0; i < INT_MAX / 3; ++i) {
      if (gotsig) {
        WRITE("\rgot ctrl+c asynchronously\n");
        exit(0);
      }
    }

    // posix guarantees atomic i/o if you use pipe_buf sized buffers
    // that way we don't need to worry about things like looping and
    // we can also be assured that multiple actors wont have tearing
    // 512 is the minimum permissible value for PIPE_BUF for all the
    // platforms. when stdin is a terminal there are more guarantees
    // about exactly how many bytes will be returned. in ICANON mode
    // which is the default you can count on it returning one single
    // line, including its \n (or VEOL, or VEOL2) per read. if using
    // non-canonical raw mode, then a single logical keystroke shall
    // be returned per read, so long as has VMIN characters or more,
    // and the default VMIN is 1. you can also set VMIN w/ tcsetattr
    // to 0 for a special kind of non-blocking mode.
    char buf[512];

    // read data from standard input
    //
    // assuming you started this program in your terminal standard input
    // will be plugged into your termios driver, which cosmpolitan codes
    // in libc/calls/read-nt.c on windows. your read() function includes
    // a primitive version of readline/linenoise called "canonical mode"
    // which lets you edit the data that'll be returned by read() before
    // it's actually returned. for example, if you type hello and enter,
    // then "hello\n" will be returned. if you type hello and then ^D or
    // ctrl-d, then "hello" will be returned. the ctrl-d keystroke is in
    // fact an ascii control code whose special behavior can be bypassed
    // if you type ctrl-v ctrl-d and then enter, in which case "\3\n" is
    // returned, also known as ^D^J.
    WRITE("doing read i/o task...\n");
    int got = read(0, buf, sizeof(buf));

    // check if the read operation failed
    // negative one is the *only* return value to indicate errors
    if (got == -1) {
      if (errno == EINTR) {
        // a signal handler was invoked during the read operation
        // since we have only one such signal handler it's sigint
        // if we didn't have any signal handlers in our app, then
        // we wouldn't need to check this errno. using SA_RESTART
        // is another great way to avoid having to worry about it
        // however EINTR is very useful, when we choose to use it
        // the \r character is needed so when the line is printed
        // it'll overwrite the ^C that got echo'd with the ctrl-c
        if (gotsig) {
          WRITE("\rgot ctrl+c via i/o eintr\n");
          exit(0);
        } else {
          WRITE("\rgot spurious eintr\n");
          continue;
        }
      } else {
        // log it in the unlikely event something else went wrong
        perror("<stdin>");
        exit(1);
      }
    }

    // check if the user typed ctrl-d which closes the input handle
    if (!got) {
      WRITE("got eof\n");
      exit(0);
    }

    // visualize line data returned by canonical mode to standard output
    //
    // it's usually safe to ignore the return code of write; your system
    // will send SIGPIPE if there's any problem, which kills by default.
    //
    // it's possible to use keyboard shortcuts to embed control codes in
    // the line. so we visualize them using the classic tty notation. it
    // is also possible to type the ascii representation, so we use bold
    // to visually distinguish ascii codes. see also o//examples/ttyinfo
    write(1, "got: ", 5);
    for (int i = 0; i < got; ++i) {
      if (isascii(buf[i])) {
        if (iscntrl(buf[i])) {
          char ctl[2];
          ctl[0] = '^';
          ctl[1] = buf[i] ^ 0100;
          WRITE("\033[1m");
          write(1, ctl, 2);
          WRITE("\033[0m");
        } else {
          write(1, &buf[i], 1);
        }
      }
    }
    WRITE("\n");
  }
}
