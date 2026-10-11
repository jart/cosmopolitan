#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justin Tunney has waived       │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "plinko.h"
#include "print.h"

int PrintArgs(int fd, int keys, int vals, int n) {
  if (!keys)
    return n;
  if (keys > 0) {
    if (!(vals < 0 && Car(vals) == kClosure)) {
      if (n) {
        n += PrintChar(fd, L';');
        n += PrintChar(fd, L' ');
      }
      n += Print(fd, keys);
      n += PrintChar(fd, L'=');
      n += Print(fd, vals);
    }
    return n;
  }
  if (vals > 0) {
    if (n) {
      n += PrintChar(fd, L';');
      n += PrintChar(fd, L' ');
    }
    n += Print(fd, Car(keys));
    n += PrintChar(fd, L'=');
    n += PrintChar(fd, L'!');
    n += Print(fd, vals);
    vals = 0;
  } else {
    n += PrintArgs(fd, Car(keys), Car(vals), n);
  }
  if (!Cdr(keys))
    return n;
  return PrintArgs(fd, Cdr(keys), Cdr(vals), n);
}
