#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "plinko.h"

bool MakesClosures(int x) {
  int h;
  if (x < 0 && (h = Car(x)) != kQuote && h != kClosure) {
    if (h == kMacro)
      return true;
    if (h == kLambda)
      return true;
    if (h == kCond) {
      while ((x = Cdr(x)) < 0) {
        if ((h = Car(x)) < 0) {
          if (MakesClosures(Car(h)))
            return true;
          if ((h = Cdr(h)) < 0) {
            if (MakesClosures(Car(h)))
              return true;
          }
        }
      }
    } else {
      while (x) {
        if (x < 0) {
          h = Car(x);
          x = Cdr(x);
        } else {
          h = x;
          x = 0;
        }
        MakesClosures(h);
      }
    }
  }
  return false;
}
