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

#include "third_party/tbb/parallel_pipeline.h"
#include "third_party/tbb/spin_mutex.h"
#include "third_party/tbb/tbb_allocator.h"
#include "third_party/tbb/cache_aligned_allocator.h"
#include "third_party/tbb/itt_notify.h"
#include "third_party/tbb/tls.h"
#include "third_party/tbb/detail/_exception.h"
#include "third_party/tbb/detail/_small_object_pool.h"

namespace tbb {
namespace detail {
namespace r1 {

void handle_perror(int error_code, const char* aux_info);

using Token = unsigned long;

//! A processing pipeline that applies filters to items.
/** @ingroup algorithms */
class pipeline {
    friend void parallel_pipeline(d1::task_group_context&, std::size_t, const d1::filter_node&);
public:

    //! Construct empty pipeline.
    pipeline(d1::task_group_context& cxt, std::size_t max_token) :
        my_context(cxt),
        first_filter(nullptr),
        last_filter(nullptr),
        input_tokens(Token(max_token)),
        end_of_input(false),
        wait_ctx(0) {
            __TBB_ASSERT( max_token>0, "pipeline::run must have at least one token" );
        }

    ~pipeline();

    //! Add filter to end of pipeline.
    void add_filter( d1::base_filter& );

    //! Traverse tree of fitler-node in-order and add filter for each leaf
    void fill_pipeline(const d1::filter_node& root) {
        if( root.left && root.right ) {
            fill_pipeline(*root.left);
            fill_pipeline(*root.right);
        }
        else {
            __TBB_ASSERT(!root.left && !root.right, "tree should be full");
            add_filter(*root.create_filter());
        }
    }

private:
    friend class stage_task;
    friend class base_filter;
    friend void set_end_of_input(d1::base_filter& bf);

    task_group_context& my_context;

    //! Pointer to first filter in the pipeline.
    d1::base_filter* first_filter;

    //! Pointer to last filter in the pipeline.
    d1::base_filter* last_filter;

    //! Number of idle tokens waiting for input stage.
    std::atomic<Token> input_tokens;

    //! False until flow_control::stop() is called.
    std::atomic<bool> end_of_input;

    d1::wait_context wait_ctx;
};

//! This structure is used to store task information in an input buffer
struct task_info {
    void* my_object = nullptr;
    //! Invalid unless a task went through an ordered stage.
    Token my_token = 0;
    //! False until my_token is set.
    bool my_token_ready  = false;
    //! True if my_object is valid.
    bool is_valid = false;
    //! Set to initial state (no object, no token)
    void reset() {
        my_object = nullptr;
        my_token = 0;
        my_token_ready = false;
        is_valid = false;
    }
};

//! A buffer of input items for a filter.
/** Each item is a task_info, inserted into a position in the buffer corresponding to a Token. */
class input_buffer {
    friend class base_filter;
    friend class stage_task;
    friend class pipeline;
    friend void set_end_of_input(d1::base_filter& bf);

    using size_type = Token;

    //! Array of deferred tasks that cannot yet start executing.
    task_info* array;

    //! Size of array
    /** Always 0 or a power of 2 */
    size_type array_size;

    //! Lowest token that can start executing.
    /** All prior Token have already been seen. */
    Token low_token;

    //! Serializes updates.
    spin_mutex array_mutex;

    //! Resize "array".
    /** Caller is responsible to acquiring a lock on "array_mutex". */
    void grow( size_type minimum_size );

    //! Initial size for "array"
    /** Must be a power of 2 */
    static const size_type initial_buffer_size = 4;

    //! Used for out of order buffer, and for assigning my_token if is_ordered and my_token not already assigned
    Token high_token;

    //! True for ordered filter, false otherwise.
    const bool is_ordered;

    //! for parallel filters that accepts nullptrs, thread-local flag for reaching end_of_input
    using end_of_input_tls_t = basic_tls<input_buffer*>;
    end_of_input_tls_t end_of_input_tls;
    bool end_of_input_tls_allocated; // no way to test pthread creation of TLS

public:
    input_buffer(const input_buffer&) = delete;
    input_buffer& operator=(const input_buffer&) = delete;

    //! Construct empty buffer.
    input_buffer( bool ordered) :
            array(nullptr),
            array_size(0),
            low_token(0),
            high_token(0),
            is_ordered(ordered),
            end_of_input_tls(),
            end_of_input_tls_allocated(false) {
        grow(initial_buffer_size);
        __TBB_ASSERT( array, nullptr );
    }

    //! Destroy the buffer.
    ~input_buffer() {
        __TBB_ASSERT( array, nullptr );
        cache_aligned_allocator<task_info>().deallocate(array,array_size);
        poison_pointer( array );
        if( end_of_input_tls_allocated ) {
            destroy_my_tls();
        }
    }

    //! Define order when the first filter is serial_in_order.
    Token get_ordered_token(){
        return high_token++;
    }

    //! Put a token into the buffer.
    /** If task information was placed into buffer, returns true;
        otherwise returns false, informing the caller to create and spawn a task.
    */
    bool try_put_token( task_info& info ) {
        info.is_valid = true;
        spin_mutex::scoped_lock lock( array_mutex );
        Token token;
        if( is_ordered ) {
            if( !info.my_token_ready ) {
                info.my_token = high_token++;
                info.my_token_ready = true;
            }
            token = info.my_token;
        } else
            token = high_token++;
        __TBB_ASSERT( (long)(token-low_token)>=0, nullptr );
        if( token!=low_token ) {
            // Trying to put token that is beyond low_token.
            // Need to wait until low_token catches up before dispatching.
            if( token-low_token>=array_size )
                grow( token-low_token+1 );
            ITT_NOTIFY( sync_releasing, this );
            array[token&(array_size-1)] = info;
            return true;
        }
        return false;
    }

    //! Note that processing of a token is finished.
    /** Fires up processing of the next token, if processing was deferred. */
    // Uses template to avoid explicit dependency on stage_task.
    template<typename StageTask>
    void try_to_spawn_task_for_next_token(StageTask& spawner, d1::execution_data& ed) {
        task_info wakee;
        {
            spin_mutex::scoped_lock lock( array_mutex );
            // Wake the next task
            task_info& item = array[++low_token & (array_size-1)];
            ITT_NOTIFY( sync_acquired, this );
            wakee = item;
            item.is_valid = false;
        }
        if( wakee.is_valid )
            spawner.spawn_stage_task(wakee, ed);
    }

    // end_of_input signal for parallel_pipeline, parallel input filters with 0 tokens allowed.
    void create_my_tls() {
        int status = end_of_input_tls.create();
        if(status)
            handle_perror(status, "TLS not allocated for filter");
        end_of_input_tls_allocated = true;
    }
    void destroy_my_tls() {
        int status = end_of_input_tls.destroy();
        if(status)
            handle_perror(status, "Failed to destroy filter TLS");
    }
    bool my_tls_end_of_input() {
        return end_of_input_tls.get() != nullptr;
    }
    void set_my_tls_end_of_input() {
        end_of_input_tls.set(this);
    }
};

void input_buffer::grow( size_type minimum_size ) {
    size_type old_size = array_size;
    size_type new_size = old_size ? 2*old_size : initial_buffer_size;
    while( new_size<minimum_size )
        new_size*=2;
    task_info* new_array = cache_aligned_allocator<task_info>().allocate(new_size);
    task_info* old_array = array;
    for( size_type i=0; i<new_size; ++i )
        new_array[i].is_valid = false;
    Token t=low_token;
    for( size_type i=0; i<old_size; ++i, ++t )
        new_array[t&(new_size-1)] = old_array[t&(old_size-1)];
    array = new_array;
    array_size = new_size;
    if( old_array )
        cache_aligned_allocator<task_info>().deallocate(old_array,old_size);
}

class stage_task : public d1::task, public task_info {
private:
    friend class pipeline;
    pipeline& my_pipeline;
    d1::base_filter* my_filter;
    d1::small_object_allocator m_allocator;
    //! True if this task has not yet read the input.
    bool my_at_start;

    //! True if this can be executed again.
    bool execute_filter(d1::execution_data& ed);

    //! Spawn task if token is available.
    void try_spawn_stage_task(d1::execution_data& ed) {
        ITT_NOTIFY( sync_releasing, &my_pipeline.input_tokens );
        if( (my_pipeline.input_tokens.fetch_sub(1, std::memory_order_release)) > 1 ) {
            d1::small_object_allocator alloc{};
            r1::spawn( *alloc.new_object<stage_task>(ed, my_pipeline, alloc ), my_pipeline.my_context );
        }
    }

public:

    //! Construct stage_task for first stage in a pipeline.
    /** Such a stage has not read any input yet. */
    stage_task(pipeline& pipeline, d1::small_object_allocator& alloc ) :
        my_pipeline(pipeline),
        my_filter(pipeline.first_filter),
        m_allocator(alloc),
        my_at_start(true)
    {
        task_info::reset();
        my_pipeline.wait_ctx.reserve();
    }
    //! Construct stage_task for a subsequent stage in a pipeline.
    stage_task(pipeline& pipeline, d1::base_filter* filter, const task_info& info, d1::small_object_allocator& alloc) :
        task_info(info),
        my_pipeline(pipeline),
        my_filter(filter),
        m_allocator(alloc),
        my_at_start(false)
    {
        my_pipeline.wait_ctx.reserve();
    }
    //! Roughly equivalent to the constructor of input stage task
    void reset() {
        task_info::reset();
        my_filter = my_pipeline.first_filter;
        my_at_start = true;
    }
    void finalize(d1::execution_data& ed) {
        m_allocator.delete_object(this, ed);
    }
    //! The virtual task execution method
    task* execute(d1::execution_data& ed) override {
        if(!execute_filter(ed)) {
            finalize(ed);
            return nullptr;
        }
        return this;
    }
    task* cancel(d1::execution_data& ed) override {
        finalize(ed);
        return nullptr;
    }

    ~stage_task() override {
        if ( my_filter && my_object ) {
            my_filter->finalize(my_object);
            my_object = nullptr;
        }
        my_pipeline.wait_ctx.release();
    }
    //! Creates and spawns stage_task from task_info
    void spawn_stage_task(const task_info& info, d1::execution_data& ed) {
        d1::small_object_allocator alloc{};
        stage_task* clone = alloc.new_object<stage_task>(ed, my_pipeline, my_filter, info, alloc);
        r1::spawn(*clone, my_pipeline.my_context);
    }
};

bool stage_task::execute_filter(d1::execution_data& ed) {
    __TBB_ASSERT( !my_at_start || !my_object, "invalid state of task" );
    if( my_at_start ) {
        if( my_filter->is_serial() ) {
            my_object = (*my_filter)(my_object);
            if( my_object || ( my_filter->object_may_be_null() && !my_pipeline.end_of_input.load(std::memory_order_relaxed)) ) {
                if( my_filter->is_ordered() ) {
                    my_token = my_filter->my_input_buffer->get_ordered_token();
                    my_token_ready = true;
                }
                if( !my_filter->next_filter_in_pipeline ) { // we're only filter in pipeline
                    reset();
                    return true;
                } else {
                    try_spawn_stage_task(ed);
                }
            } else {
                my_pipeline.end_of_input.store(true, std::memory_order_relaxed);
                return false;
            }
        } else /*not is_serial*/ {
            if ( my_pipeline.end_of_input.load(std::memory_order_relaxed) ) {
                return false;
            }

            try_spawn_stage_task(ed);

            my_object = (*my_filter)(my_object);
            if( !my_object && (!my_filter->object_may_be_null() || my_filter->my_input_buffer->my_tls_end_of_input()) ){
                my_pipeline.end_of_input.store(true, std::memory_order_relaxed);
                return false;
            }
        }
        my_at_start = false;
    } else {
        my_object = (*my_filter)(my_object);
        if( my_filter->is_serial() )
            my_filter->my_input_buffer->try_to_spawn_task_for_next_token(*this, ed);
    }
    my_filter = my_filter->next_filter_in_pipeline;
    if( my_filter ) {
        // There is another filter to execute.
        if( my_filter->is_serial() ) {
            // The next filter must execute tokens when they are available (in order for serial_in_order)
            if( my_filter->my_input_buffer->try_put_token(*this) ){
                my_filter = nullptr; // To prevent deleting my_object twice if exception occurs
                return false;
            }
        }
    } else {
        // Reached end of the pipe.
        std::size_t ntokens_avail = my_pipeline.input_tokens.fetch_add(1, std::memory_order_acquire);

        if( ntokens_avail>0  // Only recycle if there is one available token
                || my_pipeline.end_of_input.load(std::memory_order_relaxed) ) {
            return false; // No need to recycle for new input
        }
        ITT_NOTIFY( sync_acquired, &my_pipeline.input_tokens );
        // Recycle as an input stage task.
        reset();
    }
    return true;
}

pipeline::~pipeline() {
    while( first_filter ) {
        d1::base_filter* f = first_filter;
        if( input_buffer* b = f->my_input_buffer ) {
            b->~input_buffer();
            deallocate_memory(b);
        }
        first_filter = f->next_filter_in_pipeline;
        f->~base_filter();
        deallocate_memory(f);
    }
}

void pipeline::add_filter( d1::base_filter& new_fitler ) {
    __TBB_ASSERT( new_fitler.next_filter_in_pipeline==d1::base_filter::not_in_pipeline(), "filter already part of pipeline?" );
    new_fitler.my_pipeline = this;
    if ( first_filter == nullptr )
        first_filter = &new_fitler;
    else
        last_filter->next_filter_in_pipeline = &new_fitler;
    new_fitler.next_filter_in_pipeline = nullptr;
    last_filter = &new_fitler;
    if( new_fitler.is_serial() ) {
        new_fitler.my_input_buffer = new (allocate_memory(sizeof(input_buffer))) input_buffer( new_fitler.is_ordered() );
    } else {
        if( first_filter == &new_fitler && new_fitler.object_may_be_null() ) {
            //TODO: buffer only needed to hold TLS; could improve
            new_fitler.my_input_buffer = new (allocate_memory(sizeof(input_buffer))) input_buffer( /*is_ordered*/false );
            new_fitler.my_input_buffer->create_my_tls();
        }
    }
}

void __TBB_EXPORTED_FUNC parallel_pipeline(d1::task_group_context& cxt, std::size_t max_token, const d1::filter_node& fn) {
    pipeline pipe(cxt, max_token);

    pipe.fill_pipeline(fn);

    d1::small_object_allocator alloc{};
    stage_task& st = *alloc.new_object<stage_task>(pipe, alloc);

    // Start execution of tasks
    r1::execute_and_wait(st, cxt, pipe.wait_ctx, cxt);
}

void __TBB_EXPORTED_FUNC set_end_of_input(d1::base_filter& bf) {
    __TBB_ASSERT(bf.my_input_buffer, nullptr);
    __TBB_ASSERT(bf.object_may_be_null(), nullptr);
    if(bf.is_serial() ) {
        bf.my_pipeline->end_of_input.store(true, std::memory_order_relaxed);
    } else {
        __TBB_ASSERT(bf.my_input_buffer->end_of_input_tls_allocated, nullptr);
        bf.my_input_buffer->set_my_tls_end_of_input();
    }
}

} // namespace r1
} // namespace detail
} // namespace tbb
