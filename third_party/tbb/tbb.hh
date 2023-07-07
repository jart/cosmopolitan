// clang-format off
/*
    Copyright (c) 2005-2021 Intel Corporation

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef __TBB_tbb_H
#define __TBB_tbb_H

/**
    This header bulk-includes declarations or definitions of all the functionality
    provided by TBB (save for tbbmalloc and 3rd party dependent headers).

    If you use only a few TBB constructs, consider including specific headers only.
    Any header listed below can be included independently of others.
**/

// MISSING #include "oneapi/tbb/blocked_range.h"
// MISSING #include "oneapi/tbb/blocked_range2d.h"
// MISSING #include "oneapi/tbb/blocked_range3d.h"
#if TBB_PREVIEW_BLOCKED_RANGE_ND
// MISSING #include "tbb/blocked_rangeNd.h"
#endif
// MISSING #include "oneapi/tbb/cache_aligned_allocator.h"
// MISSING #include "oneapi/tbb/combinable.h"
// MISSING #include "oneapi/tbb/concurrent_hash_map.h"
#if TBB_PREVIEW_CONCURRENT_LRU_CACHE
// MISSING #include "tbb/concurrent_lru_cache.h"
#endif
// MISSING #include "oneapi/tbb/collaborative_call_once.h"
// MISSING #include "oneapi/tbb/concurrent_priority_queue.h"
// MISSING #include "oneapi/tbb/concurrent_queue.h"
// MISSING #include "oneapi/tbb/concurrent_unordered_map.h"
// MISSING #include "oneapi/tbb/concurrent_unordered_set.h"
// MISSING #include "oneapi/tbb/concurrent_map.h"
// MISSING #include "oneapi/tbb/concurrent_set.h"
// MISSING #include "oneapi/tbb/concurrent_vector.h"
// MISSING #include "oneapi/tbb/enumerable_thread_specific.h"
// MISSING #include "oneapi/tbb/flow_graph.h"
// MISSING #include "oneapi/tbb/global_control.h"
// MISSING #include "oneapi/tbb/info.h"
// MISSING #include "oneapi/tbb/null_mutex.h"
// MISSING #include "oneapi/tbb/null_rw_mutex.h"
// MISSING #include "oneapi/tbb/parallel_for.h"
// MISSING #include "oneapi/tbb/parallel_for_each.h"
// MISSING #include "oneapi/tbb/parallel_invoke.h"
// MISSING #include "oneapi/tbb/parallel_pipeline.h"
// MISSING #include "oneapi/tbb/parallel_reduce.h"
// MISSING #include "oneapi/tbb/parallel_scan.h"
// MISSING #include "oneapi/tbb/parallel_sort.h"
// MISSING #include "oneapi/tbb/partitioner.h"
// MISSING #include "oneapi/tbb/queuing_mutex.h"
// MISSING #include "oneapi/tbb/queuing_rw_mutex.h"
// MISSING #include "oneapi/tbb/spin_mutex.h"
// MISSING #include "oneapi/tbb/spin_rw_mutex.h"
// MISSING #include "oneapi/tbb/task.h"
// MISSING #include "oneapi/tbb/task_arena.h"
// MISSING #include "oneapi/tbb/task_group.h"
// MISSING #include "oneapi/tbb/task_scheduler_observer.h"
// MISSING #include "oneapi/tbb/tbb_allocator.h"
// MISSING #include "oneapi/tbb/tick_count.h"
// MISSING #include "oneapi/tbb/version.h"

#endif /* __TBB_tbb_H */
