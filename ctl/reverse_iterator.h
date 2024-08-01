// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_REVERSE_ITERATOR_H_
#define CTL_REVERSE_ITERATOR_H_
#include "iterator_traits.h"
#include "utility.h"

namespace ctl {

template<class Iterator>
class reverse_iterator
{
  public:
    using iterator_type = Iterator;
    using iterator_category =
      typename ctl::iterator_traits<Iterator>::iterator_category;
    using value_type = typename ctl::iterator_traits<Iterator>::value_type;
    using difference_type =
      typename ctl::iterator_traits<Iterator>::difference_type;
    using pointer = typename ctl::iterator_traits<Iterator>::pointer;
    using reference = typename ctl::iterator_traits<Iterator>::reference;

    constexpr reverse_iterator() : current()
    {
    }

    constexpr explicit reverse_iterator(Iterator x) : current(x)
    {
    }

    template<class U>
    constexpr reverse_iterator(const reverse_iterator<U>& other)
      : current(other.base())
    {
    }

    template<class U>
    constexpr reverse_iterator& operator=(const reverse_iterator<U>& other)
    {
        current = other.base();
        return *this;
    }

    constexpr Iterator base() const
    {
        return current;
    }

    constexpr reference operator*() const
    {
        Iterator tmp = current;
        return *--tmp;
    }

    constexpr pointer operator->() const
    {
        return &(operator*());
    }

    constexpr reverse_iterator& operator++()
    {
        --current;
        return *this;
    }

    constexpr reverse_iterator operator++(int)
    {
        reverse_iterator tmp = *this;
        --current;
        return tmp;
    }

    constexpr reverse_iterator& operator--()
    {
        ++current;
        return *this;
    }

    constexpr reverse_iterator operator--(int)
    {
        reverse_iterator tmp = *this;
        ++current;
        return tmp;
    }

    constexpr reverse_iterator operator+(difference_type n) const
    {
        return reverse_iterator(current - n);
    }

    constexpr reverse_iterator& operator+=(difference_type n)
    {
        current -= n;
        return *this;
    }

    constexpr reverse_iterator operator-(difference_type n) const
    {
        return reverse_iterator(current + n);
    }

    constexpr reverse_iterator& operator-=(difference_type n)
    {
        current += n;
        return *this;
    }

    constexpr reference operator[](difference_type n) const
    {
        return *(*this + n);
    }

  protected:
    Iterator current;
};

template<class Iterator1, class Iterator2>
constexpr bool
operator==(const reverse_iterator<Iterator1>& lhs,
           const reverse_iterator<Iterator2>& rhs)
{
    return lhs.base() == rhs.base();
}

template<class Iterator1, class Iterator2>
constexpr bool
operator!=(const reverse_iterator<Iterator1>& lhs,
           const reverse_iterator<Iterator2>& rhs)
{
    return lhs.base() != rhs.base();
}

template<class Iterator1, class Iterator2>
constexpr bool
operator<(const reverse_iterator<Iterator1>& lhs,
          const reverse_iterator<Iterator2>& rhs)
{
    return rhs.base() < lhs.base();
}

template<class Iterator1, class Iterator2>
constexpr bool
operator<=(const reverse_iterator<Iterator1>& lhs,
           const reverse_iterator<Iterator2>& rhs)
{
    return !(rhs < lhs);
}

template<class Iterator1, class Iterator2>
constexpr bool
operator>(const reverse_iterator<Iterator1>& lhs,
          const reverse_iterator<Iterator2>& rhs)
{
    return rhs < lhs;
}

template<class Iterator1, class Iterator2>
constexpr bool
operator>=(const reverse_iterator<Iterator1>& lhs,
           const reverse_iterator<Iterator2>& rhs)
{
    return !(lhs < rhs);
}

template<class Iterator>
constexpr reverse_iterator<Iterator>
operator+(typename reverse_iterator<Iterator>::difference_type n,
          const reverse_iterator<Iterator>& it)
{
    return reverse_iterator<Iterator>(it.base() - n);
}

template<class Iterator1, class Iterator2>
constexpr auto
operator-(const reverse_iterator<Iterator1>& lhs,
          const reverse_iterator<Iterator2>& rhs) -> decltype(rhs.base() -
                                                              lhs.base())
{
    return rhs.base() - lhs.base();
}

} // namespace ctl

#endif // CTL_REVERSE_ITERATOR_H_
