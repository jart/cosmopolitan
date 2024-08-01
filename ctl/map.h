// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_MAP_H_
#define CTL_MAP_H_
#include "out_of_range.h"
#include "set.h"

namespace ctl {

template<typename Key, typename Value, typename Compare = ctl::less<Key>>
class map
{
    class EntryCompare
    {
      public:
        explicit EntryCompare(Compare comp = Compare()) : comp_(comp)
        {
        }

        bool operator()(const ctl::pair<const Key, Value>& lhs,
                        const ctl::pair<const Key, Value>& rhs) const
        {
            return comp_(lhs.first, rhs.first);
        }

      private:
        Compare comp_;
    };

    ctl::set<ctl::pair<const Key, Value>, EntryCompare> data_;

  public:
    using key_type = Key;
    using mapped_type = Value;
    using value_type = ctl::pair<const Key, Value>;
    using size_type = typename ctl::set<value_type, EntryCompare>::size_type;
    using difference_type =
      typename ctl::set<value_type, EntryCompare>::difference_type;
    using key_compare = Compare;
    using value_compare = EntryCompare;
    using iterator = typename ctl::set<value_type, EntryCompare>::iterator;
    using const_iterator =
      typename ctl::set<value_type, EntryCompare>::const_iterator;
    using reverse_iterator =
      typename ctl::set<value_type, EntryCompare>::reverse_iterator;
    using const_reverse_iterator =
      typename ctl::set<value_type, EntryCompare>::const_reverse_iterator;

    map() : data_(EntryCompare())
    {
    }

    explicit map(const Compare& comp) : data_(EntryCompare(comp))
    {
    }

    map(const map& other) = default;
    map(map&& other) noexcept = default;
    map(std::initializer_list<value_type> init, const Compare& comp = Compare())
      : data_(init, EntryCompare(comp))
    {
    }

    template<typename InputIt>
    map(InputIt first, InputIt last, const Compare& comp = Compare())
      : data_(first, last, EntryCompare(comp))
    {
    }

    map& operator=(const map& other) = default;
    map& operator=(map&& other) noexcept = default;
    map& operator=(std::initializer_list<value_type> ilist)
    {
        data_ = ilist;
        return *this;
    }

    iterator begin() noexcept
    {
        return data_.begin();
    }

    const_iterator begin() const noexcept
    {
        return data_.begin();
    }

    const_iterator cbegin() const noexcept
    {
        return data_.cbegin();
    }

    iterator end() noexcept
    {
        return data_.end();
    }

    const_iterator end() const noexcept
    {
        return data_.end();
    }

    const_iterator cend() const noexcept
    {
        return data_.cend();
    }

    reverse_iterator rbegin() noexcept
    {
        return data_.rbegin();
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return data_.rbegin();
    }

    const_reverse_iterator crbegin() const noexcept
    {
        return data_.crbegin();
    }

    reverse_iterator rend() noexcept
    {
        return data_.rend();
    }

    const_reverse_iterator rend() const noexcept
    {
        return data_.rend();
    }

    const_reverse_iterator crend() const noexcept
    {
        return data_.crend();
    }

    bool empty() const noexcept
    {
        return data_.empty();
    }

    size_type size() const noexcept
    {
        return data_.size();
    }

    size_type max_size() const noexcept
    {
        return data_.max_size();
    }

    Value& operator[](const Key& key)
    {
        return ((data_.insert(ctl::make_pair(key, Value()))).first)->second;
    }

    Value& operator[](Key&& key)
    {
        return ((data_.insert(ctl::make_pair(ctl::move(key), Value()))).first)
          ->second;
    }

    Value& at(const Key& key)
    {
        auto it = find(key);
        if (it == end())
            throw ctl::out_of_range();
        return it->second;
    }

    const Value& at(const Key& key) const
    {
        auto it = find(key);
        if (it == end())
            throw ctl::out_of_range();
        return it->second;
    }

    ctl::pair<iterator, bool> insert(const value_type& value)
    {
        return data_.insert(value);
    }

    ctl::pair<iterator, bool> insert(value_type&& value)
    {
        return data_.insert(ctl::move(value));
    }

    template<typename P>
    ctl::pair<iterator, bool> insert(P&& value)
    {
        return data_.insert(value_type(ctl::forward<P>(value)));
    }

    iterator insert(const_iterator hint, const value_type& value)
    {
        return data_.insert(hint, value);
    }

    iterator insert(const_iterator hint, value_type&& value)
    {
        return data_.insert(hint, ctl::move(value));
    }

    template<typename P>
    iterator insert(const_iterator hint, P&& value)
    {
        return data_.insert(hint, value_type(ctl::forward<P>(value)));
    }

    template<typename InputIt>
    void insert(InputIt first, InputIt last)
    {
        data_.insert(first, last);
    }

    void insert(std::initializer_list<value_type> ilist)
    {
        data_.insert(ilist);
    }

    template<typename... Args>
    ctl::pair<iterator, bool> emplace(Args&&... args)
    {
        return data_.emplace(ctl::forward<Args>(args)...);
    }

    template<typename... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args)
    {
        return data_.emplace_hint(hint, ctl::forward<Args>(args)...);
    }

    iterator erase(const_iterator pos)
    {
        return data_.erase(pos);
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        return data_.erase(first, last);
    }

    size_type erase(const Key& key)
    {
        return data_.erase(ctl::make_pair(key, Value()));
    }

    void swap(map& other) noexcept
    {
        data_.swap(other.data_);
    }

    void clear() noexcept
    {
        data_.clear();
    }

    iterator find(const Key& key)
    {
        return data_.find(ctl::make_pair(key, Value()));
    }

    const_iterator find(const Key& key) const
    {
        return data_.find(ctl::make_pair(key, Value()));
    }

    size_type count(const Key& key) const
    {
        return data_.count(ctl::make_pair(key, Value()));
    }

    iterator lower_bound(const Key& key)
    {
        return data_.lower_bound(ctl::make_pair(key, Value()));
    }

    const_iterator lower_bound(const Key& key) const
    {
        return data_.lower_bound(ctl::make_pair(key, Value()));
    }

    iterator upper_bound(const Key& key)
    {
        return data_.upper_bound(ctl::make_pair(key, Value()));
    }

    const_iterator upper_bound(const Key& key) const
    {
        return data_.upper_bound(ctl::make_pair(key, Value()));
    }

    ctl::pair<iterator, iterator> equal_range(const Key& key)
    {
        return data_.equal_range(ctl::make_pair(key, Value()));
    }

    ctl::pair<const_iterator, const_iterator> equal_range(const Key& key) const
    {
        return data_.equal_range(ctl::make_pair(key, Value()));
    }

    key_compare key_comp() const
    {
        return data_.value_comp().comp;
    }

    value_compare value_comp() const
    {
        return data_.value_comp();
    }

    friend bool operator==(const map& lhs, const map& rhs)
    {
        return lhs.data_ == rhs.data_;
    }

    friend bool operator!=(const map& lhs, const map& rhs)
    {
        return !(lhs == rhs);
    }

    friend bool operator<(const map& lhs, const map& rhs)
    {
        return lhs.data_ < rhs.data_;
    }

    friend bool operator<=(const map& lhs, const map& rhs)
    {
        return !(rhs < lhs);
    }

    friend bool operator>(const map& lhs, const map& rhs)
    {
        return rhs < lhs;
    }

    friend bool operator>=(const map& lhs, const map& rhs)
    {
        return !(lhs < rhs);
    }

    friend void swap(map& lhs, map& rhs) noexcept
    {
        lhs.swap(rhs);
    }
};

} // namespace ctl

#endif // CTL_MAP_H_
