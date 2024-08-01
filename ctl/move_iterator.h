#ifndef CTL_MOVE_ITERATOR_H_
#define CTL_MOVE_ITERATOR_H_
#include "iterator_traits.h"

namespace ctl {

template<typename Iterator>
class move_iterator
{
  public:
    using iterator_type = Iterator;
    using iterator_category =
      typename ctl::iterator_traits<Iterator>::iterator_category;
    using value_type = typename ctl::iterator_traits<Iterator>::value_type;
    using difference_type =
      typename ctl::iterator_traits<Iterator>::difference_type;
    using pointer = Iterator;
    using reference = value_type&&;

    constexpr move_iterator() : current()
    {
    }

    explicit constexpr move_iterator(Iterator i) : current(i)
    {
    }

    template<class U>
    constexpr move_iterator(const move_iterator<U>& u) : current(u.base())
    {
    }

    constexpr Iterator base() const
    {
        return current;
    }

    constexpr reference operator*() const
    {
        return static_cast<reference>(*current);
    }

    constexpr pointer operator->() const
    {
        return current;
    }

    constexpr move_iterator& operator++()
    {
        ++current;
        return *this;
    }

    constexpr move_iterator operator++(int)
    {
        move_iterator tmp = *this;
        ++current;
        return tmp;
    }

    constexpr move_iterator& operator--()
    {
        --current;
        return *this;
    }

    constexpr move_iterator operator--(int)
    {
        move_iterator tmp = *this;
        --current;
        return tmp;
    }

    constexpr move_iterator operator+(difference_type n) const
    {
        return move_iterator(current + n);
    }

    constexpr move_iterator& operator+=(difference_type n)
    {
        current += n;
        return *this;
    }

    constexpr move_iterator operator-(difference_type n) const
    {
        return move_iterator(current - n);
    }

    constexpr move_iterator& operator-=(difference_type n)
    {
        current -= n;
        return *this;
    }

    constexpr reference operator[](difference_type n) const
    {
        return ctl::move(current[n]);
    }

  private:
    Iterator current;
};

template<typename Iterator>
__attribute__((__always_inline__)) constexpr move_iterator<Iterator>
make_move_iterator(Iterator i)
{
    return move_iterator<Iterator>(i);
}

template<typename Iterator1, typename Iterator2>
constexpr bool
operator==(const move_iterator<Iterator1>& x, const move_iterator<Iterator2>& y)
{
    return x.base() == y.base();
}

template<typename Iterator1, typename Iterator2>
constexpr bool
operator!=(const move_iterator<Iterator1>& x, const move_iterator<Iterator2>& y)
{
    return !(x == y);
}

template<typename Iterator1, typename Iterator2>
constexpr bool
operator<(const move_iterator<Iterator1>& x, const move_iterator<Iterator2>& y)
{
    return x.base() < y.base();
}

template<typename Iterator1, typename Iterator2>
constexpr bool
operator<=(const move_iterator<Iterator1>& x, const move_iterator<Iterator2>& y)
{
    return !(y < x);
}

template<typename Iterator1, typename Iterator2>
constexpr bool
operator>(const move_iterator<Iterator1>& x, const move_iterator<Iterator2>& y)
{
    return y < x;
}

template<typename Iterator1, typename Iterator2>
constexpr bool
operator>=(const move_iterator<Iterator1>& x, const move_iterator<Iterator2>& y)
{
    return !(x < y);
}

template<typename Iterator>
constexpr move_iterator<Iterator>
operator+(typename move_iterator<Iterator>::difference_type n,
          const move_iterator<Iterator>& x)
{
    return x + n;
}

template<typename Iterator1, typename Iterator2>
constexpr auto
operator-(const move_iterator<Iterator1>& x,
          const move_iterator<Iterator2>& y) -> decltype(x.base() - y.base())
{
    return x.base() - y.base();
}

} // namespace ctl

#endif // CTL_MOVE_ITERATOR_H_
