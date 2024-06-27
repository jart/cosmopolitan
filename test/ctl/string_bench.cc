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

#include "ctl/string.h"

#include <__utility/move.h>

#include "libc/calls/struct/timespec.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

#define BENCH(ITERATIONS, WORK_PER_RUN, CODE) \
    do { \
        struct timespec start = timespec_real(); \
        for (int __i = 0; __i < ITERATIONS; ++__i) { \
            asm volatile("" ::: "memory"); \
            CODE; \
        } \
        long long work = (WORK_PER_RUN) * (ITERATIONS); \
        double nanos = \
          (timespec_tonanos(timespec_sub(timespec_real(), start)) + work - \
           1) / \
          (double)work; \
        printf("%10g ns %2dx %s\n", nanos, (ITERATIONS), #CODE); \
    } while (0)

const char* big_c = "aaaaaaaaaaaaaaaaaaaaaaaa";
const char* small_c = "aaaaaaaaaaaaaaaaaaaaaaa";

int
main()
{
    const ctl::string_view big(big_c), small(small_c);

    BENCH(10000000, 1, {
        ctl::string s;
        s.append("hello ");
        s.append("world");
    });

    BENCH(1000000, 8, {
        ctl::string s;
        for (int i = 0; i < 8; ++i) {
            s.append('a');
        }
    });

    BENCH(1000000, 16, {
        ctl::string s;
        for (int i = 0; i < 16; ++i) {
            s.append('a');
        }
    });

    BENCH(1000000, 23, {
        ctl::string s;
        for (int i = 0; i < 23; ++i) {
            s.append('a');
        }
    });

    BENCH(1000000, 24, {
        ctl::string s;
        for (int i = 0; i < 24; ++i) {
            s.append('a');
        }
    });

    BENCH(1000000, 32, {
        ctl::string s;
        for (int i = 0; i < 32; ++i) {
            s.append('a');
        }
    });

    BENCH(1000000, 1, { ctl::string s(small_c); });

    BENCH(1000000, 1, { ctl::string s(small); });

    {
        ctl::string small_copy("hello world");
        BENCH(1000000, 1, { ctl::string s2(small_copy); });
    }

    BENCH(1000000, 1, {
        ctl::string s(small);
        ctl::string s2(std::move(s));
    });

    BENCH(1000000, 1, {
        ctl::string s(small);
        ctl::string s2(s);
    });

    BENCH(1000000, 1, { ctl::string s(big_c); });

    BENCH(1000000, 1, { ctl::string s(big); });

    {
        ctl::string big_copy(big);
        BENCH(1000000, 1, { ctl::string s2(big_copy); });
    }

    BENCH(1000000, 1, {
        ctl::string s(big);
        ctl::string s2(std::move(s));
    });

    BENCH(1000000, 1, {
        ctl::string s(big);
        ctl::string s2(s);
    });

    BENCH(1000000, 1, { ctl::string s(23, 'a'); });

    BENCH(1000000, 1, { ctl::string s(24, 'a'); });

    {
        ctl::string s(5, 'a');
        BENCH(1000000, 1, { ctl::string_view s2(s); });
    }

    {
        ctl::string big_trunc(48, 'a');
        big_trunc.resize(4);
        BENCH(1000000, 1, { ctl::string s(big_trunc); });
    }

    return 0;
}
