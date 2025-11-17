#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justine Tunney has waived      │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include <cosmo.h>
#include <stdlib.h>

// zero-overhead memory leak detector demo
//
//     $ make -j o//examples/memleak_zerocost
//     $ o//examples/memleak_zerocost
//     error: leaked 128 byte allocation at 0x7a1483eabf50
//     error: leaked 16 byte allocation at 0x7a1483eacfe0
//     error: leaked 1'008 byte allocation at 0x42671e0
//            you forgot to call free 3 times
//

void NoOp(void *p) {
}

void (*pNoOp)(void *) = NoOp;

int main(int argc, char *argv[]) {

  // run program, which leaks memory
  pNoOp(malloc(10));
  pNoOp(malloc(100));
  pNoOp(malloc(1000));

  // check for memory leaks. this has zero overhead, because it crawls
  // the data structures used by cosmopolitan's malloc implementation.
  // this is great thing to always use so you know when there's a leak
  // and if you have trouble tracking down where it came from then you
  // can swap this out with the code in examples/memleak_backtrace.c
  CheckForMemoryLeaks();
}
