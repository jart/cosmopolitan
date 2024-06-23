// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef _LIBCPP_INITIALIZER_LIST
#define _LIBCPP_INITIALIZER_LIST

namespace std {

template<class T>
class initializer_list
{
    constexpr initializer_list(const T* b, size_t s) noexcept
      : begin_(b), size_(s)
    {
    }

  public:
    typedef T value_type;
    typedef const T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef const T* iterator;
    typedef const T* const_iterator;

    constexpr initializer_list() noexcept : begin_(nullptr), size_(0)
    {
    }

    constexpr size_t size() const noexcept
    {
        return size_;
    }

    constexpr const T* begin() const noexcept
    {
        return begin_;
    }

    constexpr const T* end() const noexcept
    {
        return begin_ + size_;
    }

  private:
    const T* begin_;
    size_t size_;
};

template<class T>
inline constexpr const T*
begin(initializer_list<T> i) noexcept
{
    return i.begin();
}

template<class T>
inline constexpr const T*
end(initializer_list<T> i) noexcept
{
    return i.end();
}

} // namespace std

#endif // _LIBCPP_INITIALIZER_LIST
