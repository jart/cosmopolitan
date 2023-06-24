// clang-format off
/*
    Copyright (c) 2005-2022 Intel Corporation

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

#ifndef _TBB_task_stream_H
#define _TBB_task_stream_H

//! This file is a possible future replacement for the task_stream class implemented in
//! task_stream.h. It refactors the code and extends task_stream capabilities by moving lane
//! management during operations on caller side. Despite the fact that new implementation should not
//! affect performance of the original task stream, analysis on this subject was not made at the
//! time it was developed. In addition, it is not clearly seen at the moment that this container
//! would be suitable for critical tasks due to linear time complexity on its operations.

#include "third_party/tbb/detail/_utils.hh"
#include "third_party/tbb/cache_aligned_allocator.hh"
#include "third_party/tbb/mutex.hh"

#include "third_party/tbb/scheduler_common.hh"
#include "third_party/tbb/misc.hh" // for FastRandom

#include "third_party/libcxx/deque"
#include "third_party/libcxx/climits"
#include "third_party/libcxx/atomic"

namespace tbb {
namespace detail {
namespace r1 {

//! Essentially, this is just a pair of a queue and a mutex to protect the queue.
/** The reason std::pair is not used is that the code would look less clean
    if field names were replaced with 'first' and 'second'. **/
template< typename T, typename mutex_t >
struct alignas(max_nfs_size) queue_and_mutex {
    typedef std::deque< T, cache_aligned_allocator<T> > queue_base_t;

    queue_base_t my_queue{};
    mutex_t      my_mutex{};
};

using population_t = uintptr_t;
const population_t one = 1;

inline void set_one_bit( std::atomic<population_t>& dest, int pos ) {
    __TBB_ASSERT( pos>=0, nullptr);
    __TBB_ASSERT( pos<int(sizeof(population_t)*CHAR_BIT), nullptr);
    dest.fetch_or( one<<pos );
}

inline void clear_one_bit( std::atomic<population_t>& dest, int pos ) {
    __TBB_ASSERT( pos>=0, nullptr);
    __TBB_ASSERT( pos<int(sizeof(population_t)*CHAR_BIT), nullptr);
    dest.fetch_and( ~(one<<pos) );
}

inline bool is_bit_set( population_t val, int pos ) {
    __TBB_ASSERT( pos>=0, nullptr);
    __TBB_ASSERT( pos<int(sizeof(population_t)*CHAR_BIT), nullptr);
    return (val & (one<<pos)) != 0;
}

struct random_lane_selector :
#if __INTEL_COMPILER == 1110 || __INTEL_COMPILER == 1500
        no_assign
#else
        no_copy
#endif
{
    random_lane_selector( FastRandom& random ) : my_random( random ) {}
    unsigned operator()( unsigned out_of ) const {
        __TBB_ASSERT( ((out_of-1) & out_of) == 0, "number of lanes is not power of two." );
        return my_random.get() & (out_of-1);
    }
private:
    FastRandom& my_random;
};

struct lane_selector_base :
#if __INTEL_COMPILER == 1110 || __INTEL_COMPILER == 1500
        no_assign
#else
        no_copy
#endif
{
    unsigned& my_previous;
    lane_selector_base( unsigned& previous ) : my_previous( previous ) {}
};

struct subsequent_lane_selector : lane_selector_base {
    subsequent_lane_selector( unsigned& previous ) : lane_selector_base( previous ) {}
    unsigned operator()( unsigned out_of ) const {
        __TBB_ASSERT( ((out_of-1) & out_of) == 0, "number of lanes is not power of two." );
        return (++my_previous &= out_of-1);
    }
};

struct preceding_lane_selector : lane_selector_base {
    preceding_lane_selector( unsigned& previous ) : lane_selector_base( previous ) {}
    unsigned operator()( unsigned out_of ) const {
        __TBB_ASSERT( ((out_of-1) & out_of) == 0, "number of lanes is not power of two." );
        return (--my_previous &= (out_of-1));
    }
};

//! Specializes from which side of the underlying container elements are retrieved. Method must be
//! called under corresponding mutex locked.
template<task_stream_accessor_type accessor>
class task_stream_accessor : no_copy {
protected:
    using lane_t = queue_and_mutex <d1::task*, mutex>;
    d1::task* get_item( lane_t::queue_base_t& queue ) {
        d1::task* result = queue.front();
        queue.pop_front();
        return result;
    }
};

template<>
class task_stream_accessor< back_nonnull_accessor > : no_copy {
protected:
    using lane_t = queue_and_mutex <d1::task*, mutex>;
    d1::task* get_item( lane_t::queue_base_t& queue ) {
        d1::task* result = nullptr;
        __TBB_ASSERT(!queue.empty(), nullptr);
        // Isolated task can put zeros in queue see look_specific
        do {
            result = queue.back();
            queue.pop_back();
        } while ( !result && !queue.empty() );
        return result;
    }
};

//! The container for "fairness-oriented" aka "enqueued" tasks.
template<task_stream_accessor_type accessor>
class task_stream : public task_stream_accessor< accessor > {
    using lane_t = typename task_stream_accessor<accessor>::lane_t;
    std::atomic<population_t> population{};
    lane_t* lanes{nullptr};
    unsigned N{};

public:
    task_stream() = default;

    void initialize( unsigned n_lanes ) {
        const unsigned max_lanes = sizeof(population_t) * CHAR_BIT;

        N = n_lanes >= max_lanes ? max_lanes : n_lanes > 2 ? 1 << (tbb::detail::log2(n_lanes - 1) + 1) : 2;
        __TBB_ASSERT( N == max_lanes || (N >= n_lanes && ((N - 1) & N) == 0), "number of lanes miscalculated" );
        __TBB_ASSERT( N <= sizeof(population_t) * CHAR_BIT, nullptr);
        lanes = static_cast<lane_t*>(cache_aligned_allocate(sizeof(lane_t) * N));
        for (unsigned i = 0; i < N; ++i) {
            new (lanes + i) lane_t;
        }
        __TBB_ASSERT( !population.load(std::memory_order_relaxed), nullptr);
    }

    ~task_stream() {
        if (lanes) {
            for (unsigned i = 0; i < N; ++i) {
                lanes[i].~lane_t();
            }
            cache_aligned_deallocate(lanes);
        }
    }

    //! Push a task into a lane. Lane selection is performed by passed functor.
    template<typename lane_selector_t>
    void push(d1::task* source, const lane_selector_t& next_lane ) {
        bool succeed = false;
        unsigned lane = 0;
        do {
            lane = next_lane( /*out_of=*/N );
            __TBB_ASSERT( lane < N, "Incorrect lane index." );
        } while( ! (succeed = try_push( source, lane )) );
    }

    //! Try finding and popping a task using passed functor for lane selection. Last used lane is
    //! updated inside lane selector.
    template<typename lane_selector_t>
    d1::task* pop( const lane_selector_t& next_lane ) {
        d1::task* popped = nullptr;
        unsigned lane = 0;
        for (atomic_backoff b; !empty() && !popped; b.pause()) {
            lane = next_lane( /*out_of=*/N);
            __TBB_ASSERT(lane < N, "Incorrect lane index.");
            popped = try_pop(lane);
        }
        return popped;
    }

    //! Try finding and popping a related task.
    d1::task* pop_specific( unsigned& last_used_lane, isolation_type isolation ) {
        d1::task* result = nullptr;
        // Lane selection is round-robin in backward direction.
        unsigned idx = last_used_lane & (N-1);
        do {
            if( is_bit_set( population.load(std::memory_order_relaxed), idx ) ) {
                lane_t& lane = lanes[idx];
                mutex::scoped_lock lock;
                if( lock.try_acquire(lane.my_mutex) && !lane.my_queue.empty() ) {
                    result = look_specific( lane.my_queue, isolation );
                    if( lane.my_queue.empty() )
                        clear_one_bit( population, idx );
                    if( result )
                        break;
                }
            }
            idx=(idx-1)&(N-1);
        } while( !empty() && idx != last_used_lane );
        last_used_lane = idx;
        return result;
    }

    //! Checks existence of a task.
    bool empty() {
        return !population.load(std::memory_order_relaxed);
    }

private:
    //! Returns true on successful push, otherwise - false.
    bool try_push(d1::task* source, unsigned lane_idx ) {
        mutex::scoped_lock lock;
        if( lock.try_acquire( lanes[lane_idx].my_mutex ) ) {
            lanes[lane_idx].my_queue.push_back( source );
            set_one_bit( population, lane_idx ); // TODO: avoid atomic op if the bit is already set
            return true;
        }
        return false;
    }

    //! Returns pointer to task on successful pop, otherwise - nullptr.
    d1::task* try_pop( unsigned lane_idx ) {
        if( !is_bit_set( population.load(std::memory_order_relaxed), lane_idx ) )
            return nullptr;
        d1::task* result = nullptr;
        lane_t& lane = lanes[lane_idx];
        mutex::scoped_lock lock;
        if( lock.try_acquire( lane.my_mutex ) && !lane.my_queue.empty() ) {
            result = this->get_item( lane.my_queue );
            if( lane.my_queue.empty() )
                clear_one_bit( population, lane_idx );
        }
        return result;
    }

    // TODO: unify '*_specific' logic with 'pop' methods above
    d1::task* look_specific( typename lane_t::queue_base_t& queue, isolation_type isolation ) {
        __TBB_ASSERT( !queue.empty(), nullptr);
        // TODO: add a worst-case performance test and consider an alternative container with better
        // performance for isolation search.
        typename lane_t::queue_base_t::iterator curr = queue.end();
        do {
            // TODO: consider logic from get_task to simplify the code.
            d1::task* result = *--curr;
            if( result && task_accessor::isolation(*result) == isolation ) {
                if( queue.end() - curr == 1 )
                    queue.pop_back(); // a little of housekeeping along the way
                else
                    *curr = nullptr;      // grabbing task with the same isolation
                // TODO: move one of the container's ends instead if the task has been found there
                return result;
            }
        } while( curr != queue.begin() );
        return nullptr;
    }

}; // task_stream

} // namespace r1
} // namespace detail
} // namespace tbb

#endif /* _TBB_task_stream_H */
