#ifndef __TBB_FAKE_H
#define __TBB_FAKE_H

namespace tbb {

    template <typename T>
    using concurrent_vector = std::vector<T>;

template <
    class Key,
    class T,
    class Hash = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>,
    class Allocator = std::allocator< std::pair<const Key, T> > >
    using concurrent_hash_map = std::unordered_map<Key, T, Hash, KeyEqual, Allocator>;

    using spin_mutex = std::mutex;

    template<typename InputIterator, typename Function>
    void parallel_for_each(InputIterator first, InputIterator last, const Function& f) {
    }

    template<typename Range, typename Body>
    void parallel_for_each(Range& rng, const Body& body) {
    }

    template<typename Range, typename Body>
    void parallel_for( const Range& range, const Body& body ) {
    }

    template <typename Index, typename Function>
    void parallel_for(Index first, Index last, const Function& f) {
    }

    enum task_group_status {
        not_complete,
        complete,
        canceled
    };

    class task_group {
    public:
        task_group() {};
        ~task_group() {};

        template<typename Func>
        void run( Func&& f ) {

        };

        template<typename Func>
        task_group_status run_and_wait( const Func& f ) {
            return task_group_status::complete;
        };

        task_group_status wait() {
            return task_group_status::complete;
        };
        
        void cancel() {

        };
    };

}
#endif
