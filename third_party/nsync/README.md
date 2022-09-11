# *NSYNC

The `THIRD_PARTY_NSYNC` and `LIBC_THREAD` packages include source code
from *NSYNC. Here's the latest upstream synchronization point:

    git@github.com:google/nsync
    ac5489682760393fe21bd2a8e038b528442412a7 (1.25.0)
    Author: Mike Burrows <m3b@google.com>
    Date:   Wed Jun 1 16:47:52 2022 -0700

NSYNC uses the Apache 2.0 license. We made the following local changes:

  - Write custom `nsync_malloc_()` so `malloc()` can use *NSYNC.

  - Rewrite `futex()` wrapper to support old Linux kernels and OpenBSD.

  - Normalize sources to Cosmopolitan style conventions; *NSYNC upstream
    supports dozens of compilers and operating systems, at compile-time.
    Since Cosmo solves portability at runtime instead, most of the build
    config toil has been removed, in order to help the NSYNC source code
    be more readable and hackable.
