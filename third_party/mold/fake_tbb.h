#ifndef __TBB_FAKE_H
#define __TBB_FAKE_H

namespace tbb {

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

}
#endif
