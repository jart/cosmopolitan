// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_VECTOR_H_
#define CTL_VECTOR_H_
#include "allocator.h"
#include "allocator_traits.h"
#include "copy.h"
#include "distance.h"
#include "equal.h"
#include "fill_n.h"
#include "initializer_list.h"
#include "iterator_traits.h"
#include "lexicographical_compare.h"
#include "max.h"
#include "move_backward.h"
#include "move_iterator.h"
#include "out_of_range.h"
#include "require_input_iterator.h"
#include "reverse_iterator.h"
#include "uninitialized_fill.h"
#include "uninitialized_fill_n.h"
#include "uninitialized_move_n.h"

namespace ctl {

template<typename T, typename Allocator = ctl::allocator<T>>
class vector
{
  public:
    using value_type = T;
    using allocator_type = Allocator;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename ctl::allocator_traits<Allocator>::pointer;
    using const_pointer =
      typename ctl::allocator_traits<Allocator>::const_pointer;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = ctl::reverse_iterator<iterator>;
    using const_reverse_iterator = ctl::reverse_iterator<const_iterator>;

  public:
    vector() noexcept(noexcept(Allocator()))
      : alloc_(), data_(nullptr), size_(0), capacity_(0)
    {
    }

    explicit vector(const Allocator& alloc) noexcept
      : alloc_(alloc), data_(nullptr), size_(0), capacity_(0)
    {
    }

    vector(size_type count,
           const T& value,
           const Allocator& alloc = Allocator())
      : alloc_(alloc), data_(nullptr), size_(0), capacity_(0)
    {
        assign(count, value);
    }

    explicit vector(size_type count, const Allocator& alloc = Allocator())
      : alloc_(alloc), data_(nullptr), size_(0), capacity_(0)
    {
        resize(count);
    }

    template<class InputIt, typename = ctl::require_input_iterator<InputIt>>
    vector(InputIt first, InputIt last, const Allocator& alloc = Allocator())
      : alloc_(alloc), data_(nullptr), size_(0), capacity_(0)
    {
        assign(first, last);
    }

    vector(const vector& other)
      : alloc_(ctl::allocator_traits<
               Allocator>::select_on_container_copy_construction(other.alloc_))
      , data_(nullptr)
      , size_(0)
      , capacity_(0)
    {
        assign(other.begin(), other.end());
    }

    vector(const vector& other, const Allocator& alloc)
      : alloc_(alloc), data_(nullptr), size_(0), capacity_(0)
    {
        assign(other.begin(), other.end());
    }

    vector(vector&& other) noexcept
      : alloc_(ctl::move(other.alloc_))
      , data_(other.data_)
      , size_(other.size_)
      , capacity_(other.capacity_)
    {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    vector(vector&& other, const Allocator& alloc)
      : alloc_(alloc), data_(nullptr), size_(0), capacity_(0)
    {
        if (alloc_ == other.alloc_) {
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        } else {
            assign(ctl::make_move_iterator(other.begin()),
                   ctl::make_move_iterator(other.end()));
        }
    }

    vector(std::initializer_list<T> init, const Allocator& alloc = Allocator())
      : alloc_(alloc), data_(nullptr), size_(0), capacity_(0)
    {
        assign(init.begin(), init.end());
    }

    ~vector()
    {
        clear();
        ctl::allocator_traits<Allocator>::deallocate(alloc_, data_, capacity_);
    }

    vector& operator=(const vector& other)
    {
        if (this != &other) {
            if (ctl::allocator_traits<
                  Allocator>::propagate_on_container_copy_assignment::value)
                alloc_ = other.alloc_;
            assign(other.begin(), other.end());
        }
        return *this;
    }

    vector& operator=(vector&& other) noexcept(
      ctl::allocator_traits<Allocator>::is_always_equal::value)
    {
        if (this != &other) {
            clear();
            ctl::allocator_traits<Allocator>::deallocate(
              alloc_, data_, capacity_);
            if (ctl::allocator_traits<
                  Allocator>::propagate_on_container_move_assignment::value)
                alloc_ = ctl::move(other.alloc_);
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    vector& operator=(std::initializer_list<T> ilist)
    {
        assign(ilist.begin(), ilist.end());
        return *this;
    }

    void assign(size_type count, const T& value)
    {
        clear();
        if (count > capacity_)
            reallocate(count);
        ctl::uninitialized_fill_n(data_, count, value);
        size_ = count;
    }

    template<class InputIt, typename = ctl::require_input_iterator<InputIt>>
    void assign(InputIt first, InputIt last)
    {
        clear();
        for (; first != last; ++first)
            push_back(*first);
    }

    void assign(std::initializer_list<T> ilist)
    {
        assign(ilist.begin(), ilist.end());
    }

    reference at(size_type pos)
    {
        if (pos >= size_)
            throw ctl::out_of_range();
        return data_[pos];
    }

    const_reference at(size_type pos) const
    {
        if (pos >= size_)
            throw ctl::out_of_range();
        return data_[pos];
    }

    reference operator[](size_type pos)
    {
        if (pos >= size_)
            __builtin_trap();
        return data_[pos];
    }

    const_reference operator[](size_type pos) const
    {
        if (pos >= size_)
            __builtin_trap();
        return data_[pos];
    }

    reference front()
    {
        return data_[0];
    }

    const_reference front() const
    {
        return data_[0];
    }

    reference back()
    {
        return data_[size_ - 1];
    }

    const_reference back() const
    {
        return data_[size_ - 1];
    }

    T* data() noexcept
    {
        return data_;
    }

    const T* data() const noexcept
    {
        return data_;
    }

    iterator begin() noexcept
    {
        return data_;
    }

    const_iterator begin() const noexcept
    {
        return data_;
    }

    const_iterator cbegin() const noexcept
    {
        return data_;
    }

    iterator end() noexcept
    {
        return data_ + size_;
    }

    const_iterator end() const noexcept
    {
        return data_ + size_;
    }

    const_iterator cend() const noexcept
    {
        return data_ + size_;
    }

    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    bool empty() const noexcept
    {
        return size_ == 0;
    }

    size_type size() const noexcept
    {
        return size_;
    }

    size_type max_size() const noexcept
    {
        return __PTRDIFF_MAX__;
    }

    size_type capacity() const noexcept
    {
        return capacity_;
    }

    void reserve(size_type new_cap)
    {
        if (new_cap > capacity_)
            reallocate(new_cap);
    }

    void shrink_to_fit()
    {
        if (size_ < capacity_)
            reallocate(size_);
    }

    void clear() noexcept
    {
        for (size_type i = 0; i < size_; ++i)
            ctl::allocator_traits<Allocator>::destroy(alloc_, data_ + i);
        size_ = 0;
    }

    iterator insert(const_iterator pos, const T& value)
    {
        return insert(pos, 1, value);
    }

    iterator insert(const_iterator pos, T&& value)
    {
        difference_type index = pos - begin();
        if (size_ == capacity_)
            grow();
        iterator it = begin() + index;
        ctl::move_backward(it, end(), end() + 1);
        *it = ctl::move(value);
        ++size_;
        return it;
    }

    iterator insert(const_iterator pos, size_type count, const T& value)
    {
        difference_type index = pos - begin();
        if (size_ + count > capacity_)
            reallocate(ctl::max(size_ + count, capacity_ * 2));
        iterator it = begin() + index;
        ctl::move_backward(it, end(), end() + count);
        ctl::fill_n(it, count, value);
        size_ += count;
        return it;
    }

    template<class InputIt, typename = ctl::require_input_iterator<InputIt>>
    iterator insert(const_iterator pos, InputIt first, InputIt last)
    {
        difference_type count = ctl::distance(first, last);
        difference_type index = pos - begin();
        if (size_ + count > capacity_)
            reallocate(ctl::max(size_ + count, capacity_ * 2));
        iterator it = begin() + index;
        ctl::move_backward(it, end(), end() + count);
        ctl::copy(first, last, it);
        size_ += count;
        return it;
    }

    iterator insert(const_iterator pos, std::initializer_list<T> ilist)
    {
        return insert(pos, ilist.begin(), ilist.end());
    }

    template<class... Args>
    iterator emplace(const_iterator pos, Args&&... args)
    {
        difference_type index = pos - begin();
        if (size_ == capacity_)
            grow();
        iterator it = begin() + index;
        ctl::move_backward(it, end(), end() + 1);
        ctl::allocator_traits<Allocator>::construct(
          alloc_, it, ctl::forward<Args>(args)...);
        ++size_;
        return it;
    }

    iterator erase(const_iterator pos)
    {
        return erase(pos, pos + 1);
    }

    constexpr iterator erase(const_iterator first, const_iterator last)
    {
        difference_type index = first - begin();
        difference_type count = last - first;
        iterator it = begin() + index;
        for (iterator move_it = it + count; move_it != end(); ++move_it, ++it)
            *it = ctl::move(*move_it);
        for (difference_type i = 0; i < count; ++i)
            ctl::allocator_traits<Allocator>::destroy(alloc_, end() - i - 1);
        size_ -= count;
        return begin() + index;
    }

    void push_back(const T& value)
    {
        if (size_ == capacity_)
            grow();
        ctl::allocator_traits<Allocator>::construct(
          alloc_, data_ + size_, value);
        ++size_;
    }

    void push_back(T&& value)
    {
        if (size_ == capacity_)
            grow();
        ctl::allocator_traits<Allocator>::construct(
          alloc_, data_ + size_, ctl::move(value));
        ++size_;
    }

    template<class... Args>
    reference emplace_back(Args&&... args)
    {
        if (size_ == capacity_)
            grow();
        ctl::allocator_traits<Allocator>::construct(
          alloc_, data_ + size_, ctl::forward<Args>(args)...);
        return data_[size_++];
    }

    void pop_back()
    {
        if (!empty()) {
            ctl::allocator_traits<Allocator>::destroy(alloc_,
                                                      data_ + size_ - 1);
            --size_;
        }
    }

    void resize(size_type count)
    {
        resize(count, T());
    }

    void resize(size_type count, const value_type& value)
    {
        if (count > size_) {
            if (count > capacity_)
                reallocate(count);
            ctl::uninitialized_fill(data_ + size_, data_ + count, value);
        } else if (count < size_) {
            for (size_type i = count; i < size_; ++i)
                ctl::allocator_traits<Allocator>::destroy(alloc_, data_ + i);
        }
        size_ = count;
    }

    void swap(vector& other) noexcept(
      ctl::allocator_traits<Allocator>::is_always_equal::value)
    {
        using ctl::swap;
        swap(alloc_, other.alloc_);
        swap(data_, other.data_);
        swap(size_, other.size_);
        swap(capacity_, other.capacity_);
    }

    allocator_type get_allocator() const noexcept
    {
        return alloc_;
    }

  private:
    void grow()
    {
        size_type c2;
        c2 = capacity_;
        if (c2 < 4)
            c2 = 4;
        c2 += c2 >> 1;
        reallocate(c2);
    }

    void reallocate(size_type new_capacity)
    {
        pointer new_data =
          ctl::allocator_traits<Allocator>::allocate(alloc_, new_capacity);
        size_type new_size = size_ < new_capacity ? size_ : new_capacity;
        try {
            ctl::uninitialized_move_n(data_, new_size, new_data);
        } catch (...) {
            ctl::allocator_traits<Allocator>::deallocate(
              alloc_, new_data, new_capacity);
            throw;
        }
        for (size_type i = 0; i < size_; ++i)
            ctl::allocator_traits<Allocator>::destroy(alloc_, data_ + i);
        ctl::allocator_traits<Allocator>::deallocate(alloc_, data_, capacity_);
        data_ = new_data;
        size_ = new_size;
        capacity_ = new_capacity;
    }

    [[no_unique_address]] Allocator alloc_;
    pointer data_;
    size_type size_;
    size_type capacity_;
};

template<class T, class Alloc>
bool
operator==(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs)
{
    return lhs.size() == rhs.size() &&
           ctl::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<class T, class Alloc>
bool
operator!=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs)
{
    return !(lhs == rhs);
}

template<class T, class Alloc>
bool
operator<(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs)
{
    return ctl::lexicographical_compare(
      lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<class T, class Alloc>
bool
operator<=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs)
{
    return !(rhs < lhs);
}

template<class T, class Alloc>
bool
operator>(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs)
{
    return rhs < lhs;
}

template<class T, class Alloc>
bool
operator>=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs)
{
    return !(lhs < rhs);
}

template<class T, class Alloc>
void
swap(vector<T, Alloc>& lhs,
     vector<T, Alloc>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}

template<class InputIt,
         class Alloc =
           ctl::allocator<typename ctl::iterator_traits<InputIt>::value_type>>
vector(InputIt, InputIt, Alloc = Alloc())
  -> vector<typename ctl::iterator_traits<InputIt>::value_type, Alloc>;

template<class Alloc>
vector(size_t,
       const typename ctl::allocator_traits<Alloc>::value_type&,
       Alloc = Alloc())
  -> vector<typename ctl::allocator_traits<Alloc>::value_type, Alloc>;

} // namespace ctl

#endif // CTL_VECTOR_H_
