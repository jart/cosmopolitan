DESCRIPTION

  plinko is a simple lisp interpreter that takes advantage of advanced
  operating system features irrespective of their practicality such as
  using the nsa instruction popcount for mark sweep garbage collection
  overcommit memory, segment registers, and other dirty hacks that the
  popular interpreters cannot do; this lets plinko gain a considerable
  performance edge while retaining an event greater edge in simplicity

  We hope you find these sources informative, educational, and possibly
  useful too. Lisp source code, written in its dialect is included too
  under //tool/plinko/lib and unit tests which clarify their usage can
  be found in //test/tool/plinko.

BENCHMARK

  binary trees (n=21)

    - sbcl:     200 ms (native jit;  simulated arithmetic)
    - plinko:   400 ms (interpreted; simulated arithmetic)
    - python3:  800 ms (interpreted;    native arithmetic)
    - racket:  1200 ms (interpreted; simulated arithmetic)

AUTHOR

  Justine Alexandra Roberts Tunney <jtunney@gmail.com>

LICENSE

  ISC

SEE ALSO

  SectorLISP
  SectorLambda
