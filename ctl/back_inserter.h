// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_BACK_INSERTER_H_
#define CTL_BACK_INSERTER_H_
#include "iterator.h"
#include "utility.h"

namespace ctl {

template<class Container>
class back_insert_iterator
{
  protected:
    Container* container;

  public:
    using iterator_category = ctl::output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;

    explicit back_insert_iterator(Container& c) : container(&c)
    {
    }

    back_insert_iterator& operator=(const typename Container::value_type& value)
    {
        container->push_back(value);
        return *this;
    }

    back_insert_iterator& operator=(typename Container::value_type&& value)
    {
        container->push_back(ctl::move(value));
        return *this;
    }

    back_insert_iterator& operator*()
    {
        return *this;
    }

    back_insert_iterator& operator++()
    {
        return *this;
    }

    back_insert_iterator operator++(int)
    {
        return *this;
    }
};

template<class Container>
back_insert_iterator<Container>
back_inserter(Container& c)
{
    return back_insert_iterator<Container>(c);
}

} // namespace ctl

#endif // CTL_BACK_INSERTER_H_
