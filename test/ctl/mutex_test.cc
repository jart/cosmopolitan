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

#include "ctl/mutex.h"
#include "libc/mem/leaks.h"

// #include <mutex>
// #define ctl std

ctl::mutex mtx;
int shared_resource = 0;
const int NUM_THREADS = 5;
const int ITERATIONS = 100000;

void*
increment_resource(void* arg)
{
    for (int i = 0; i < ITERATIONS; ++i) {
        mtx.lock();
        ++shared_resource;
        mtx.unlock();
    }
    return nullptr;
}

void*
decrement_resource(void* arg)
{
    for (int i = 0; i < ITERATIONS; ++i) {
        mtx.lock();
        --shared_resource;
        mtx.unlock();
    }
    return nullptr;
}

int
test_basic_locking()
{
    shared_resource = 0;
    pthread_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; ++i)
        if (pthread_create(&threads[i], 0, increment_resource, 0) != 0)
            return 1;
    for (int i = 0; i < NUM_THREADS; ++i)
        if (pthread_join(threads[i], 0) != 0)
            return 2;
    return (shared_resource == NUM_THREADS * ITERATIONS) ? 0 : 3;
}

int
test_lock_contention()
{
    shared_resource = 0;
    pthread_t threads[NUM_THREADS * 2];
    for (int i = 0; i < NUM_THREADS; ++i)
        if (pthread_create(&threads[i], 0, increment_resource, 0) != 0 ||
            pthread_create(&threads[i + NUM_THREADS],
                           nullptr,
                           decrement_resource,
                           nullptr) != 0)
            return 4;
    for (int i = 0; i < NUM_THREADS * 2; ++i)
        if (pthread_join(threads[i], 0) != 0)
            return 5;
    return (shared_resource == 0) ? 0 : 6;
}

int
test_try_lock()
{
    ctl::mutex try_mtx;
    if (!try_mtx.try_lock())
        return 7;
    if (try_mtx.try_lock())
        return 8;
    try_mtx.unlock();
    return 0;
}

int
main()
{
    int result;

    result = test_basic_locking();
    if (result != 0)
        return result;

    result = test_lock_contention();
    if (result != 0)
        return result;

    result = test_try_lock();
    if (result != 0)
        return result;

    CheckForMemoryLeaks();
}
