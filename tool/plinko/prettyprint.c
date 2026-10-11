#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "plinko.h"
#include "print.h"
#include "tree.h"

static void PrettyPrintList(int fd, int x, int n) {
  int y, once, func, mode, argwidth, funcwidth, forcedot;
  assert(n >= 0);
  assert(x <= 0);
  if (x < cx) {
    n += PrintChar(fd, L'!');
    n += PrintInt(fd, x, 0, 0, 0, 10, true);
  } else {
    if (ShouldConcealClosure(x)) {
      x = Car(Cdr(x));
    }
    PrintChar(fd, L'(');
    if (x < 0) {
      func = Car(x);
      funcwidth = PrettyPrint(fd, func, ++n);
      if (func == kDefine) {
        PrintSpace(fd);
        PrettyPrint(fd, Car(Cdr(x)), n);
        forcedot = 0;
        x = Cdr(x);
        mode = Cdr(x) < 0;
        once = 1;
        n += 1;
      } else if ((func == kLambda || func == kMacro) &&
                 (Cdr(x) < 0 && Cdr(Cdr(x)) < 0)) {
        PrintSpace(fd);
        if (!Car(Cdr(x))) {
          PrintChar(fd, L'(');
          PrintChar(fd, L')');
        } else {
          PrettyPrint(fd, Car(Cdr(x)), n);
        }
        x = Cdr(x);
        mode = 1;
        forcedot = 0;
        once = 1;
        n += 1;
      } else {
        if (func >= 0) {
          n += funcwidth + 1;
        }
        mode = func < 0 && Car(func) != kQuote;
        once = mode;
        forcedot = 0;
      }
      if (!forcedot && ShouldForceDot(x)) {
        forcedot = true;
        n += 2;
      }
      while ((x = Cdr(x))) {
        y = x;
        argwidth = 0;
        if (y < 0 && !forcedot) {
          y = Car(y);
        } else {
          argwidth += PrintSpace(fd);
          argwidth += PrintDot(fd);
          mode = y < 0;
          x = 0;
        }
        if (y >= 0) {
          argwidth += PrintSpace(fd);
          argwidth += PrintAtom(fd, y);
          if (!once)
            n += argwidth;
        } else {
          if (once && (y < 0 || mode)) {
            mode = 1;
            PrintNewline(fd);
            if (depth >= 0)
              PrintDepth(fd, depth);
            PrintIndent(fd, n);
          } else {
            if (y < 0)
              mode = 1;
            PrintSpace(fd);
          }
          once = 1;
          PrettyPrint(fd, y, n);
        }
        forcedot = 0;
      }
    }
    PrintChar(fd, L')');
  }
}

/**
 * Prints LISP data structure with style.
 *
 * @param fd is where i/o goes
 * @param x is thing to print
 * @param n is indent level
 */
int PrettyPrint(int fd, int x, int n) {
  assert(n >= 0);
  if (!noname) {
    GetName(&x);
  }
  x = EnterPrint(x);
  if (1. / x < 0) {
    PrettyPrintList(fd, x, n);
    n = 0;
  } else {
    n = PrintAtom(fd, x);
  }
  LeavePrint(x);
  return n;
}
