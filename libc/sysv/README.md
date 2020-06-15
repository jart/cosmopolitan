SYNOPSIS

  System Five Import Libraries

OVERVIEW

  Bell System Five is the umbrella term we use to describe Linux,
  FreeBSD, OpenBSD, and Mac OS X which all have nearly-identical
  application binary interfaces that stood the test of time, having
  definitions nearly the same as those of AT&T back in the 1980's.

  Cosmopolitan aims to help you build apps that can endure over the
  course of decades, just like these systems have: without needing to
  lift a finger for maintenance churn, broken builds, broken hearts.

  The challenge to System V binary compatibility basically boils down
  to numbers. All these systems agree on what services are provided,
  but tend to grant them wildly different numbers.

  We address this by putting all the numbers in a couple big shell
  scripts, ask the GNU Assembler to encode them into binaries using an
  efficient LEB128 encoding, unpacked by _init(), and ref'd via extern
  const. It gives us good debuggability, and any costs are gained back
  by fewer branches in wrapper functions.z
