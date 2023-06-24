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

/** Before making any changes in the implementation, please emulate algorithmic changes
    with SPIN tool using <TBB directory>/tools/spin_models/ReaderWriterMutex.pml.
    There could be some code looking as "can be restructured" but its structure does matter! */

#include "third_party/tbb/queuing_rw_mutex.h"
#include "third_party/tbb/detail/_assert.h"
#include "third_party/tbb/detail/_utils.h"
#include "third_party/tbb/itt_notify.h"

namespace tbb {
namespace detail {
namespace r1 {

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
    // Workaround for overzealous compiler warnings
    #pragma warning (push)
    #pragma warning (disable: 4311 4312)
#endif

//! A view of a T* with additional functionality for twiddling low-order bits.
template<typename T>
class tricky_atomic_pointer {
public:
    using word = uintptr_t;

    static T* fetch_add( std::atomic<word>& location, word addend, std::memory_order memory_order ) {
        return reinterpret_cast<T*>(location.fetch_add(addend, memory_order));
    }

    static T* exchange( std::atomic<word>& location, T* value, std::memory_order memory_order ) {
        return reinterpret_cast<T*>(location.exchange(reinterpret_cast<word>(value), memory_order));
    }

    static T* compare_exchange_strong( std::atomic<word>& obj, const T* expected, const T* desired, std::memory_order memory_order ) {
        word expd = reinterpret_cast<word>(expected);
        obj.compare_exchange_strong(expd, reinterpret_cast<word>(desired), memory_order);
        return reinterpret_cast<T*>(expd);
    }

    static void store( std::atomic<word>& location, const T* value, std::memory_order memory_order ) {
        location.store(reinterpret_cast<word>(value), memory_order);
    }

    static T* load( std::atomic<word>& location, std::memory_order memory_order ) {
        return reinterpret_cast<T*>(location.load(memory_order));
    }

    static void spin_wait_while_eq(const std::atomic<word>& location, const T* value) {
        tbb::detail::d0::spin_wait_while_eq(location, reinterpret_cast<word>(value) );
    }

    T* & ref;
    tricky_atomic_pointer( T*& original ) : ref(original) {};
    tricky_atomic_pointer(const tricky_atomic_pointer&) = delete;
    tricky_atomic_pointer& operator=(const tricky_atomic_pointer&) = delete;
    T* operator&( const word operand2 ) const {
        return reinterpret_cast<T*>( reinterpret_cast<word>(ref) & operand2 );
    }
    T* operator|( const word operand2 ) const {
        return reinterpret_cast<T*>( reinterpret_cast<word>(ref) | operand2 );
    }
};

using tricky_pointer = tricky_atomic_pointer<queuing_rw_mutex::scoped_lock>;

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
    // Workaround for overzealous compiler warnings
    #pragma warning (pop)
#endif

//! Flag bits in a state_t that specify information about a locking request.
enum state_t_flags : unsigned char {
    STATE_NONE                   = 0,
    STATE_WRITER                 = 1<<0,
    STATE_READER                 = 1<<1,
    STATE_READER_UNBLOCKNEXT     = 1<<2,
    STATE_ACTIVEREADER           = 1<<3,
    STATE_UPGRADE_REQUESTED      = 1<<4,
    STATE_UPGRADE_WAITING        = 1<<5,
    STATE_UPGRADE_LOSER          = 1<<6,
    STATE_COMBINED_WAITINGREADER = STATE_READER | STATE_READER_UNBLOCKNEXT,
    STATE_COMBINED_READER        = STATE_COMBINED_WAITINGREADER | STATE_ACTIVEREADER,
    STATE_COMBINED_UPGRADING     = STATE_UPGRADE_WAITING | STATE_UPGRADE_LOSER
};

static const unsigned char RELEASED = 0;
static const unsigned char ACQUIRED = 1;

struct queuing_rw_mutex_impl {
    //! Try to acquire the internal lock
    /** Returns true if lock was successfully acquired. */
    static bool try_acquire_internal_lock(d1::queuing_rw_mutex::scoped_lock& s)
    {
        auto expected = RELEASED;
        return s.my_internal_lock.compare_exchange_strong(expected, ACQUIRED);
    }

    //! Acquire the internal lock
    static void acquire_internal_lock(d1::queuing_rw_mutex::scoped_lock& s)
    {
        // Usually, we would use the test-test-and-set idiom here, with exponential backoff.
        // But so far, experiments indicate there is no value in doing so here.
        while( !try_acquire_internal_lock(s) ) {
            machine_pause(1);
        }
    }

    //! Release the internal lock
    static void release_internal_lock(d1::queuing_rw_mutex::scoped_lock& s)
    {
        s.my_internal_lock.store(RELEASED, std::memory_order_release);
    }

    //! Wait for internal lock to be released
    static void wait_for_release_of_internal_lock(d1::queuing_rw_mutex::scoped_lock& s)
    {
        spin_wait_until_eq(s.my_internal_lock, RELEASED);
    }

    //! A helper function
    static void unblock_or_wait_on_internal_lock(d1::queuing_rw_mutex::scoped_lock& s, uintptr_t flag ) {
        if( flag ) {
            wait_for_release_of_internal_lock(s);
        }
        else {
            release_internal_lock(s);
        }
    }

    //! Mask for low order bit of a pointer.
    static const tricky_pointer::word FLAG = 0x1;

    static uintptr_t get_flag( d1::queuing_rw_mutex::scoped_lock* ptr ) {
        return reinterpret_cast<uintptr_t>(ptr) & FLAG;
    }

    //------------------------------------------------------------------------
    // Methods of queuing_rw_mutex::scoped_lock
    //------------------------------------------------------------------------

    //! A method to acquire queuing_rw_mutex lock
    static void acquire(d1::queuing_rw_mutex& m, d1::queuing_rw_mutex::scoped_lock& s, bool write)
    {
        __TBB_ASSERT( !s.my_mutex, "scoped_lock is already holding a mutex");

        // Must set all fields before the exchange, because once the
        // exchange executes, *this becomes accessible to other threads.
        s.my_mutex = &m;
        s.my_prev.store(0U, std::memory_order_relaxed);
        s.my_next.store(0U, std::memory_order_relaxed);
        s.my_going.store(0U, std::memory_order_relaxed);
        s.my_state.store(d1::queuing_rw_mutex::scoped_lock::state_t(write ? STATE_WRITER : STATE_READER), std::memory_order_relaxed);
        s.my_internal_lock.store(RELEASED, std::memory_order_relaxed);


        // The CAS must have release semantics, because we are
        // "sending" the fields initialized above to other actors.
        // We need acquire semantics, because we are acquiring the predecessor (or mutex if no predecessor)
        queuing_rw_mutex::scoped_lock* predecessor = m.q_tail.exchange(&s, std::memory_order_acq_rel);

        if( write ) {       // Acquiring for write

            if( predecessor ) {
                ITT_NOTIFY(sync_prepare, s.my_mutex);
                predecessor = tricky_pointer(predecessor) & ~FLAG;
                __TBB_ASSERT( !predecessor->my_next, "the predecessor has another successor!");
                tricky_pointer::store(predecessor->my_next, &s, std::memory_order_release);
                // We are acquiring the mutex
                spin_wait_until_eq(s.my_going, 1U, std::memory_order_acquire);
            }

        } else {            // Acquiring for read
    #if __TBB_USE_ITT_NOTIFY
            bool sync_prepare_done = false;
    #endif
            if( predecessor ) {
                unsigned char pred_state{};
                __TBB_ASSERT( !s.my_prev.load(std::memory_order_relaxed), "the predecessor is already set" );
                if( tricky_pointer(predecessor) & FLAG ) {
                    /* this is only possible if predecessor is an upgrading reader and it signals us to wait */
                    pred_state = STATE_UPGRADE_WAITING;
                    predecessor = tricky_pointer(predecessor) & ~FLAG;
                } else {
                    // Load predecessor->my_state now, because once predecessor->my_next becomes
                    // non-null, we must assume that *predecessor might be destroyed.
                    pred_state = predecessor->my_state.load(std::memory_order_relaxed);
                    if (pred_state == STATE_READER) {
                        // Notify the previous reader to unblock us.
                        predecessor->my_state.compare_exchange_strong(pred_state, STATE_READER_UNBLOCKNEXT, std::memory_order_relaxed);
                    }
                    if (pred_state == STATE_ACTIVEREADER)  { // either we initially read it or CAS failed
                        // Active reader means that the predecessor already acquired the mutex and cannot notify us.
                        // Therefore, we need to acquire the mutex ourselves by re-reading predecessor state.
                        (void)predecessor->my_state.load(std::memory_order_acquire);
                    }
                }
                tricky_pointer::store(s.my_prev, predecessor, std::memory_order_relaxed);
                __TBB_ASSERT( !( tricky_pointer(predecessor) & FLAG ), "use of corrupted pointer!" );
                __TBB_ASSERT( !predecessor->my_next.load(std::memory_order_relaxed), "the predecessor has another successor!");
                tricky_pointer::store(predecessor->my_next, &s, std::memory_order_release);
                if( pred_state != STATE_ACTIVEREADER ) {
    #if __TBB_USE_ITT_NOTIFY
                    sync_prepare_done = true;
                    ITT_NOTIFY(sync_prepare, s.my_mutex);
    #endif
                    // We are acquiring the mutex
                    spin_wait_until_eq(s.my_going, 1U, std::memory_order_acquire);
                }
            }

            // The protected state must have been acquired here before it can be further released to any other reader(s):
            unsigned char old_state = STATE_READER;
            // When this reader is signaled by previous actor it acquires the mutex.
            // We need to build happens-before relation with all other coming readers that will read our ACTIVEREADER
            // without blocking on my_going. Therefore, we need to publish ACTIVEREADER with release semantics.
            // On fail it is relaxed, because we will build happens-before on my_going.
            s.my_state.compare_exchange_strong(old_state, STATE_ACTIVEREADER, std::memory_order_release, std::memory_order_relaxed);
            if( old_state!=STATE_READER ) {
#if __TBB_USE_ITT_NOTIFY
                if( !sync_prepare_done )
                    ITT_NOTIFY(sync_prepare, s.my_mutex);
#endif
                // Failed to become active reader -> need to unblock the next waiting reader first
                __TBB_ASSERT( s.my_state.load(std::memory_order_relaxed)==STATE_READER_UNBLOCKNEXT, "unexpected state" );
                spin_wait_while_eq(s.my_next, 0U, std::memory_order_acquire);
                /* my_state should be changed before unblocking the next otherwise it might finish
                   and another thread can get our old state and left blocked */
                s.my_state.store(STATE_ACTIVEREADER, std::memory_order_relaxed);
                tricky_pointer::load(s.my_next, std::memory_order_relaxed)->my_going.store(1U, std::memory_order_release);
            }
            __TBB_ASSERT(s.my_state.load(std::memory_order_relaxed) == STATE_ACTIVEREADER, "unlocked reader is active reader");
        }

        ITT_NOTIFY(sync_acquired, s.my_mutex);
    }

    //! A method to acquire queuing_rw_mutex if it is free
    static bool try_acquire(d1::queuing_rw_mutex& m, d1::queuing_rw_mutex::scoped_lock& s, bool write)
    {
        __TBB_ASSERT( !s.my_mutex, "scoped_lock is already holding a mutex");

        if( m.q_tail.load(std::memory_order_relaxed) )
            return false; // Someone already took the lock

        // Must set all fields before the exchange, because once the
        // exchange executes, *this becomes accessible to other threads.
        s.my_prev.store(0U, std::memory_order_relaxed);
        s.my_next.store(0U, std::memory_order_relaxed);
        s.my_going.store(0U, std::memory_order_relaxed); // TODO: remove dead assignment?
        s.my_state.store(d1::queuing_rw_mutex::scoped_lock::state_t(write ? STATE_WRITER : STATE_ACTIVEREADER), std::memory_order_relaxed);
        s.my_internal_lock.store(RELEASED, std::memory_order_relaxed);

        // The CAS must have release semantics, because we are
        // "sending" the fields initialized above to other actors.
        // We need acquire semantics, because we are acquiring the mutex
        d1::queuing_rw_mutex::scoped_lock* expected = nullptr;
        if (!m.q_tail.compare_exchange_strong(expected, &s, std::memory_order_acq_rel))
            return false; // Someone already took the lock
        s.my_mutex = &m;
        ITT_NOTIFY(sync_acquired, s.my_mutex);
        return true;
    }

    //! A method to release queuing_rw_mutex lock
    static void release(d1::queuing_rw_mutex::scoped_lock& s) {
        __TBB_ASSERT(s.my_mutex!=nullptr, "no lock acquired");

        ITT_NOTIFY(sync_releasing, s.my_mutex);

        if( s.my_state.load(std::memory_order_relaxed) == STATE_WRITER ) { // Acquired for write

            // The logic below is the same as "writerUnlock", but elides
            // "return" from the middle of the routine.
            // In the statement below, acquire semantics of reading my_next is required
            // so that following operations with fields of my_next are safe.
            d1::queuing_rw_mutex::scoped_lock* next = tricky_pointer::load(s.my_next, std::memory_order_acquire);
            if( !next ) {
                d1::queuing_rw_mutex::scoped_lock* expected = &s;
                // Release mutex on success otherwise wait for successor publication
                if( s.my_mutex->q_tail.compare_exchange_strong(expected, nullptr,
                    std::memory_order_release, std::memory_order_relaxed) )
                {
                    // this was the only item in the queue, and the queue is now empty.
                    goto done;
                }
                spin_wait_while_eq(s.my_next, 0U, std::memory_order_relaxed);
                next = tricky_pointer::load(s.my_next, std::memory_order_acquire);
            }
            next->my_going.store(2U, std::memory_order_relaxed); // protect next queue node from being destroyed too early
            // If the next is STATE_UPGRADE_WAITING, it is expected to acquire all other released readers via release
            // sequence in next->my_state. In that case, we need to preserve release sequence in next->my_state
            // contributed by other reader. So, there are two approaches not to break the release sequence:
            //   1. Use read-modify-write (exchange) operation to store with release the UPGRADE_LOSER state;
            //   2. Acquire the release sequence and store the sequence and UPGRADE_LOSER state.
            // The second approach seems better on x86 because it does not involve interlocked operations.
            // Therefore, we read next->my_state with acquire while it is not required for else branch to get the 
            // release sequence.
            if( next->my_state.load(std::memory_order_acquire)==STATE_UPGRADE_WAITING ) {
                // the next waiting for upgrade means this writer was upgraded before.
                acquire_internal_lock(s);
                // Responsibility transition, the one who reads uncorrupted my_prev will do release.
                // Guarantee that above store of 2 into next->my_going happens-before resetting of next->my_prev
                d1::queuing_rw_mutex::scoped_lock* tmp = tricky_pointer::exchange(next->my_prev, nullptr, std::memory_order_release);
                // Pass the release sequence that we acquired with the above load of next->my_state.
                next->my_state.store(STATE_UPGRADE_LOSER, std::memory_order_release);
                // We are releasing the mutex
                next->my_going.store(1U, std::memory_order_release);
                unblock_or_wait_on_internal_lock(s, get_flag(tmp));
            } else {
                // next->state cannot be STATE_UPGRADE_REQUESTED
                __TBB_ASSERT( next->my_state.load(std::memory_order_relaxed) & (STATE_COMBINED_WAITINGREADER | STATE_WRITER), "unexpected state" );
                __TBB_ASSERT( !( next->my_prev.load(std::memory_order_relaxed) & FLAG ), "use of corrupted pointer!" );
                // Guarantee that above store of 2 into next->my_going happens-before resetting of next->my_prev
                tricky_pointer::store(next->my_prev, nullptr, std::memory_order_release);
                // We are releasing the mutex
                next->my_going.store(1U, std::memory_order_release);
            }

        } else { // Acquired for read
            // The basic idea it to build happens-before relation with left and right readers via prev and next. In addition,
            // the first reader should acquire the left (prev) signal and propagate to right (next). To simplify, we always
            // build happens-before relation between left and right (left is happened before right).
            queuing_rw_mutex::scoped_lock *tmp = nullptr;
    retry:
            // Addition to the original paper: Mark my_prev as in use
            queuing_rw_mutex::scoped_lock *predecessor = tricky_pointer::fetch_add(s.my_prev, FLAG, std::memory_order_acquire);

            if( predecessor ) {
                if( !(try_acquire_internal_lock(*predecessor)) )
                {
                    // Failed to acquire the lock on predecessor. The predecessor either unlinks or upgrades.
                    // In the second case, it could or could not know my "in use" flag - need to check
                    // Responsibility transition, the one who reads uncorrupted my_prev will do release.
                    tmp = tricky_pointer::compare_exchange_strong(s.my_prev, tricky_pointer(predecessor) | FLAG, predecessor, std::memory_order_acquire);
                    if( !(tricky_pointer(tmp) & FLAG) ) {
                        __TBB_ASSERT(tricky_pointer::load(s.my_prev, std::memory_order_relaxed) != (tricky_pointer(predecessor) | FLAG), nullptr);
                        // Now owner of predecessor is waiting for _us_ to release its lock
                        release_internal_lock(*predecessor);
                    }
                    // else the "in use" flag is back -> the predecessor didn't get it and will release itself; nothing to do

                    tmp = nullptr;
                    goto retry;
                }
                __TBB_ASSERT(predecessor && predecessor->my_internal_lock.load(std::memory_order_relaxed)==ACQUIRED, "predecessor's lock is not acquired");
                tricky_pointer::store(s.my_prev, predecessor, std::memory_order_relaxed);
                acquire_internal_lock(s);

                tricky_pointer::store(predecessor->my_next, nullptr, std::memory_order_release);

                d1::queuing_rw_mutex::scoped_lock* expected = &s;
                if( !tricky_pointer::load(s.my_next, std::memory_order_acquire) && !s.my_mutex->q_tail.compare_exchange_strong(expected, predecessor, std::memory_order_release) ) {
                    spin_wait_while_eq( s.my_next, 0U, std::memory_order_acquire );
                }
                __TBB_ASSERT( !(s.my_next.load(std::memory_order_relaxed) & FLAG), "use of corrupted pointer" );

                // my_next is acquired either with load or spin_wait.
                if(d1::queuing_rw_mutex::scoped_lock *const l_next = tricky_pointer::load(s.my_next, std::memory_order_relaxed) ) { // I->next != nil, TODO: rename to next after clearing up and adapting the n in the comment two lines below
                    // Equivalent to I->next->prev = I->prev but protected against (prev[n]&FLAG)!=0
                    tmp = tricky_pointer::exchange(l_next->my_prev, predecessor, std::memory_order_release);
                    // I->prev->next = I->next;
                    __TBB_ASSERT(tricky_pointer::load(s.my_prev, std::memory_order_relaxed)==predecessor, nullptr);
                    predecessor->my_next.store(s.my_next.load(std::memory_order_relaxed), std::memory_order_release);
                }
                // Safe to release in the order opposite to acquiring which makes the code simpler
                release_internal_lock(*predecessor);

            } else { // No predecessor when we looked
                acquire_internal_lock(s);  // "exclusiveLock(&I->EL)"
                d1::queuing_rw_mutex::scoped_lock* next = tricky_pointer::load(s.my_next, std::memory_order_acquire);
                if( !next ) {
                    d1::queuing_rw_mutex::scoped_lock* expected = &s;
                    // Release mutex on success otherwise wait for successor publication
                    if( !s.my_mutex->q_tail.compare_exchange_strong(expected, nullptr,
                        std::memory_order_release, std::memory_order_relaxed) )
                    {
                        spin_wait_while_eq( s.my_next, 0U, std::memory_order_relaxed );
                        next = tricky_pointer::load(s.my_next, std::memory_order_acquire);
                    } else {
                        goto unlock_self;
                    }
                }
                next->my_going.store(2U, std::memory_order_relaxed);
                // Responsibility transition, the one who reads uncorrupted my_prev will do release.
                tmp = tricky_pointer::exchange(next->my_prev, nullptr, std::memory_order_release);
                next->my_going.store(1U, std::memory_order_release);
            }
    unlock_self:
            unblock_or_wait_on_internal_lock(s, get_flag(tmp));
        }
    done:
        // Lifetime synchronization, no need to build happens-before relation
        spin_wait_while_eq( s.my_going, 2U, std::memory_order_relaxed );

        s.initialize();
    }

    static bool downgrade_to_reader(d1::queuing_rw_mutex::scoped_lock& s) {
        if ( s.my_state.load(std::memory_order_relaxed) == STATE_ACTIVEREADER ) return true; // Already a reader

        ITT_NOTIFY(sync_releasing, s.my_mutex);
        d1::queuing_rw_mutex::scoped_lock* next = tricky_pointer::load(s.my_next, std::memory_order_acquire);
        if( !next ) {
            s.my_state.store(STATE_READER, std::memory_order_seq_cst);
            // the following load of q_tail must not be reordered with setting STATE_READER above
            if( &s == s.my_mutex->q_tail.load(std::memory_order_seq_cst) ) {
                unsigned char old_state = STATE_READER;
                // When this reader is signaled by previous actor it acquires the mutex.
                // We need to build happens-before relation with all other coming readers that will read our ACTIVEREADER
                // without blocking on my_going. Therefore, we need to publish ACTIVEREADER with release semantics.
                // On fail it is relaxed, because we will build happens-before on my_going.
                s.my_state.compare_exchange_strong(old_state, STATE_ACTIVEREADER, std::memory_order_release, std::memory_order_relaxed);
                if( old_state==STATE_READER )
                    return true; // Downgrade completed
            }
            /* wait for the next to register */
            spin_wait_while_eq(s.my_next, 0U, std::memory_order_relaxed);
            next = tricky_pointer::load(s.my_next, std::memory_order_acquire);
        }

        __TBB_ASSERT( next, "still no successor at this point!" );
        if( next->my_state.load(std::memory_order_relaxed) & STATE_COMBINED_WAITINGREADER )
            next->my_going.store(1U, std::memory_order_release);
        // If the next is STATE_UPGRADE_WAITING, it is expected to acquire all other released readers via release
        // sequence in next->my_state. In that case, we need to preserve release sequence in next->my_state
        // contributed by other reader. So, there are two approaches not to break the release sequence:
        //   1. Use read-modify-write (exchange) operation to store with release the UPGRADE_LOSER state;
        //   2. Acquire the release sequence and store the sequence and UPGRADE_LOSER state.
        // The second approach seems better on x86 because it does not involve interlocked operations.
        // Therefore, we read next->my_state with acquire while it is not required for else branch to get the 
        // release sequence.
        else if( next->my_state.load(std::memory_order_acquire)==STATE_UPGRADE_WAITING )
            // the next waiting for upgrade means this writer was upgraded before.
            // To safe release sequence on next->my_state read it with acquire
            next->my_state.store(STATE_UPGRADE_LOSER, std::memory_order_release);
        s.my_state.store(STATE_ACTIVEREADER, std::memory_order_release);
        return true;
    }

    static bool upgrade_to_writer(d1::queuing_rw_mutex::scoped_lock& s) {
        if (s.my_state.load(std::memory_order_relaxed) == STATE_WRITER) {
            // Already a writer
            return true;
        }

        __TBB_ASSERT(s.my_state.load(std::memory_order_relaxed) == STATE_ACTIVEREADER, "only active reader can be updated");

        queuing_rw_mutex::scoped_lock* tmp{};
        queuing_rw_mutex::scoped_lock* me = &s;

        ITT_NOTIFY(sync_releasing, s.my_mutex);
        // Publish ourselves into my_state that other UPGRADE_WAITING actors can acquire our state.
        s.my_state.store(STATE_UPGRADE_REQUESTED, std::memory_order_release);
    requested:
        __TBB_ASSERT( !(s.my_next.load(std::memory_order_relaxed) & FLAG), "use of corrupted pointer!" );
        acquire_internal_lock(s);
        d1::queuing_rw_mutex::scoped_lock* expected = &s;
        if( !s.my_mutex->q_tail.compare_exchange_strong(expected, tricky_pointer(me)|FLAG, std::memory_order_acq_rel) ) {
            spin_wait_while_eq( s.my_next, 0U, std::memory_order_relaxed );
            queuing_rw_mutex::scoped_lock * next;
            next = tricky_pointer::fetch_add(s.my_next, FLAG, std::memory_order_acquire);
            // While we were READER the next READER might reach STATE_UPGRADE_WAITING state.
            // Therefore, it did not build happens before relation with us and we need to acquire the 
            // next->my_state to build the happens before relation ourselves
            unsigned short n_state = next->my_state.load(std::memory_order_acquire);
            /* the next reader can be blocked by our state. the best thing to do is to unblock it */
            if( n_state & STATE_COMBINED_WAITINGREADER )
                next->my_going.store(1U, std::memory_order_release);
            // Responsibility transition, the one who reads uncorrupted my_prev will do release.
            tmp = tricky_pointer::exchange(next->my_prev, &s, std::memory_order_release);
            unblock_or_wait_on_internal_lock(s, get_flag(tmp));
            if( n_state & (STATE_COMBINED_READER | STATE_UPGRADE_REQUESTED) ) {
                // save next|FLAG for simplicity of following comparisons
                tmp = tricky_pointer(next)|FLAG;
                for( atomic_backoff b; tricky_pointer::load(s.my_next, std::memory_order_relaxed)==tmp; b.pause() ) {
                    if( s.my_state.load(std::memory_order_acquire) & STATE_COMBINED_UPGRADING ) {
                        if( tricky_pointer::load(s.my_next, std::memory_order_acquire)==tmp )
                            tricky_pointer::store(s.my_next, next, std::memory_order_relaxed);
                        goto waiting;
                    }
                }
                __TBB_ASSERT(tricky_pointer::load(s.my_next, std::memory_order_relaxed) != (tricky_pointer(next)|FLAG), nullptr);
                goto requested;
            } else {
                __TBB_ASSERT( n_state & (STATE_WRITER | STATE_UPGRADE_WAITING), "unexpected state");
                __TBB_ASSERT( (tricky_pointer(next)|FLAG) == tricky_pointer::load(s.my_next, std::memory_order_relaxed), nullptr);
                tricky_pointer::store(s.my_next, next, std::memory_order_relaxed);
            }
        } else {
            /* We are in the tail; whoever comes next is blocked by q_tail&FLAG */
            release_internal_lock(s);
        } // if( this != my_mutex->q_tail... )
        {
            unsigned char old_state = STATE_UPGRADE_REQUESTED;
            // If we reach STATE_UPGRADE_WAITING state we do not build happens-before relation with READER on
            // left. We delegate this responsibility to READER on left when it try upgrading. Therefore, we are releasing
            // on success.
            // Otherwise, on fail, we already acquired the next->my_state.
            s.my_state.compare_exchange_strong(old_state, STATE_UPGRADE_WAITING, std::memory_order_release, std::memory_order_relaxed);
        }
    waiting:
        __TBB_ASSERT( !( s.my_next.load(std::memory_order_relaxed) & FLAG ), "use of corrupted pointer!" );
        __TBB_ASSERT( s.my_state & STATE_COMBINED_UPGRADING, "wrong state at upgrade waiting_retry" );
        __TBB_ASSERT( me==&s, nullptr );
        ITT_NOTIFY(sync_prepare, s.my_mutex);
        /* if no one was blocked by the "corrupted" q_tail, turn it back */
        expected = tricky_pointer(me)|FLAG;
        s.my_mutex->q_tail.compare_exchange_strong(expected, &s, std::memory_order_release);
        queuing_rw_mutex::scoped_lock * predecessor;
        // Mark my_prev as 'in use' to prevent predecessor from releasing
        predecessor = tricky_pointer::fetch_add(s.my_prev, FLAG, std::memory_order_acquire);
        if( predecessor ) {
            bool success = try_acquire_internal_lock(*predecessor);
            {
                // While the predecessor pointer (my_prev) is in use (FLAG is set), we can safely update the node`s state.
                // Corrupted pointer transitions responsibility to release the predecessor`s node on us.
                unsigned char old_state = STATE_UPGRADE_REQUESTED;
                // Try to build happens before with the upgrading READER on left. If fail, the predecessor state is not
                // important for us because it will acquire our state.
                predecessor->my_state.compare_exchange_strong(old_state, STATE_UPGRADE_WAITING, std::memory_order_release,
                    std::memory_order_relaxed);
            }
            if( !success ) {
                // Responsibility transition, the one who reads uncorrupted my_prev will do release.
                tmp = tricky_pointer::compare_exchange_strong(s.my_prev, tricky_pointer(predecessor)|FLAG, predecessor, std::memory_order_acquire);
                if( tricky_pointer(tmp) & FLAG ) {
                    tricky_pointer::spin_wait_while_eq(s.my_prev, predecessor);
                    predecessor = tricky_pointer::load(s.my_prev, std::memory_order_relaxed);
                } else {
                    // TODO: spin_wait condition seems never reachable
                    tricky_pointer::spin_wait_while_eq(s.my_prev, tricky_pointer(predecessor)|FLAG);
                    release_internal_lock(*predecessor);
                }
            } else {
                tricky_pointer::store(s.my_prev, predecessor, std::memory_order_relaxed);
                release_internal_lock(*predecessor);
                tricky_pointer::spin_wait_while_eq(s.my_prev, predecessor);
                predecessor = tricky_pointer::load(s.my_prev, std::memory_order_relaxed);
            }
            if( predecessor )
                goto waiting;
        } else {
            tricky_pointer::store(s.my_prev, nullptr, std::memory_order_relaxed);
        }
        __TBB_ASSERT( !predecessor && !s.my_prev, nullptr );

        // additional lifetime issue prevention checks
        // wait for the successor to finish working with my fields
        wait_for_release_of_internal_lock(s);
        // now wait for the predecessor to finish working with my fields
        spin_wait_while_eq( s.my_going, 2U );

        bool result = ( s.my_state != STATE_UPGRADE_LOSER );
        s.my_state.store(STATE_WRITER, std::memory_order_relaxed);
        s.my_going.store(1U, std::memory_order_relaxed);

        ITT_NOTIFY(sync_acquired, s.my_mutex);
        return result;
    }

    static bool is_writer(const d1::queuing_rw_mutex::scoped_lock& m) {
        return m.my_state.load(std::memory_order_relaxed) == STATE_WRITER;
    }

    static void construct(d1::queuing_rw_mutex& m) {
        suppress_unused_warning(m);
        ITT_SYNC_CREATE(&m, _T("tbb::queuing_rw_mutex"), _T(""));
    }
};

void __TBB_EXPORTED_FUNC acquire(d1::queuing_rw_mutex& m, d1::queuing_rw_mutex::scoped_lock& s, bool write) {
    queuing_rw_mutex_impl::acquire(m, s, write);
}

bool __TBB_EXPORTED_FUNC try_acquire(d1::queuing_rw_mutex& m, d1::queuing_rw_mutex::scoped_lock& s, bool write) {
    return queuing_rw_mutex_impl::try_acquire(m, s, write);
}

void __TBB_EXPORTED_FUNC release(d1::queuing_rw_mutex::scoped_lock& s) {
    queuing_rw_mutex_impl::release(s);
}

bool __TBB_EXPORTED_FUNC upgrade_to_writer(d1::queuing_rw_mutex::scoped_lock& s) {
    return queuing_rw_mutex_impl::upgrade_to_writer(s);
}

bool __TBB_EXPORTED_FUNC is_writer(const d1::queuing_rw_mutex::scoped_lock& s) {
    return queuing_rw_mutex_impl::is_writer(s);
}

bool __TBB_EXPORTED_FUNC downgrade_to_reader(d1::queuing_rw_mutex::scoped_lock& s) {
    return queuing_rw_mutex_impl::downgrade_to_reader(s);
}

void __TBB_EXPORTED_FUNC construct(d1::queuing_rw_mutex& m) {
    queuing_rw_mutex_impl::construct(m);
}

} // namespace r1
} // namespace detail
} // namespace tbb
