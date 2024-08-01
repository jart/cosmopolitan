// -*- mode:c++; indent-tabs-mode:nil; c-basic-offset:4; coding:utf-8 -*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
//
// Copyright 2024 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include "ctl/set.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/timespec.h"
#include "libc/mem/leaks.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/rusage.h"
#include "libc/testlib/benchmark.h"

// #include <set>
// #define ctl std
// #define check() size()

int
rand32(void)
{
    /* Knuth, D.E., "The Art of Computer Programming," Vol 2,
       Seminumerical Algorithms, Third Edition, Addison-Wesley, 1998,
       p. 106 (line 26) & p. 108 */
    static unsigned long long lcg = 1;
    lcg *= 6364136223846793005;
    lcg += 1442695040888963407;
    return lcg >> 32;
}

void
eat(int x)
{
}

void (*pEat)(int) = eat;

int
main()
{

    {
        long x = 0;
        ctl::set<long> s;
        BENCHMARK(1000000, 1, s.insert(rand32() % 1000000));
        // s.check();
        BENCHMARK(1000000, 1, {
            auto i = s.find(rand32() % 1000000);
            if (i != s.end())
                x += *i;
        });
        BENCHMARK(1000000, 1, {
            auto i = s.lower_bound(rand32() % 1000000);
            if (i != s.end())
                x += *i;
        });
        BENCHMARK(1000000, 1, s.erase(rand32() % 1000000));
        eat(x);
    }

    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    printf("%,10d kb peak rss\n", ru.ru_maxrss);

    CheckForMemoryLeaks();
}
