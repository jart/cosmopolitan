// -*-mode:c++;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8-*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
#ifndef CTL_SET_H_
#define CTL_SET_H_
#include "initializer_list.h"
#include "less.h"
#include "pair.h"

namespace ctl {

template<typename Key, typename Compare = ctl::less<Key>>
class set
{
    struct rbtree
    {
        uintptr_t left_;
        rbtree* right;
        rbtree* parent;
        Key value;

        rbtree* left() const
        {
            return (rbtree*)(left_ & -2);
        }

        void left(rbtree* val)
        {
            left_ = (uintptr_t)val | (left_ & 1);
        }

        bool is_red() const
        {
            return left_ & 1;
        }

        void is_red(bool val)
        {
            left_ &= -2;
            left_ |= val;
        }

        rbtree(const Key& val)
          : left_(1), right(nullptr), parent(nullptr), value(val)
        {
        }
    };

  public:
    using key_type = Key;
    using value_type = Key;
    using size_type = size_t;
    using node_type = rbtree;
    using key_compare = Compare;
    using value_compare = Compare;
    using difference_type = ptrdiff_t;
    using reference = value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using const_reference = const value_type&;

    class iterator
    {
      public:
        using value_type = Key;
        using difference_type = ptrdiff_t;
        using pointer = Key*;
        using reference = Key&;

        iterator() : node_(nullptr)
        {
        }

        reference operator*() const
        {
            return node_->value;
        }

        pointer operator->() const
        {
            return &(node_->value);
        }

        iterator& operator++()
        {
            if (node_ == nullptr)
                return *this;
            if (node_->right) {
                node_ = leftmost(node_->right);
            } else {
                node_type* parent = node_->parent;
                while (parent && node_ == parent->right) {
                    node_ = parent;
                    parent = parent->parent;
                }
                node_ = parent;
            }
            return *this;
        }

        iterator& operator--()
        {
            if (node_ == nullptr)
                __builtin_trap();
            if (node_->left()) {
                node_ = rightmost(node_->left());
            } else {
                node_type* parent = node_->parent;
                for (;;) {
                    if (parent == nullptr)
                        break;
                    if (node_ == parent->left()) {
                        node_ = parent;
                        parent = parent->parent;
                    } else {
                        break;
                    }
                }
                node_ = parent;
            }
            return *this;
        }

        iterator operator++(int)
        {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        iterator operator--(int)
        {
            iterator tmp = *this;
            --(*this);
            return tmp;
        }

        bool operator==(const iterator& other) const
        {
            return node_ == other.node_;
        }

        bool operator!=(const iterator& other) const
        {
            return !(*this == other);
        }

      private:
        friend class set;
        node_type* node_;

        explicit iterator(node_type* node) : node_(node)
        {
        }
    };

    class reverse_iterator
    {
      public:
        using value_type = Key;
        using difference_type = ptrdiff_t;
        using pointer = Key*;
        using reference = Key&;

        reverse_iterator() : node_(nullptr)
        {
        }

        reference operator*() const
        {
            return node_->value;
        }

        pointer operator->() const
        {
            return &(node_->value);
        }

        reverse_iterator& operator++()
        {
            if (node_->left()) {
                node_ = rightmost(node_->left());
            } else {
                node_type* parent = node_->parent;
                while (parent && node_ == parent->left()) {
                    node_ = parent;
                    parent = parent->parent;
                }
                node_ = parent;
            }
            return *this;
        }

        reverse_iterator operator++(int)
        {
            reverse_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        reverse_iterator& operator--()
        {
            if (node_->right) {
                node_ = leftmost(node_->right);
            } else {
                node_type* parent = node_->parent;
                while (parent && node_ == parent->right) {
                    node_ = parent;
                    parent = parent->parent;
                }
                node_ = parent;
            }
            return *this;
        }

        reverse_iterator operator--(int)
        {
            reverse_iterator tmp = *this;
            --(*this);
            return tmp;
        }

        bool operator==(const reverse_iterator& other) const
        {
            return node_ == other.node_;
        }

        bool operator!=(const reverse_iterator& other) const
        {
            return !(*this == other);
        }

        iterator base() const
        {
            if (node_ == nullptr)
                return iterator(leftmost(root_));
            reverse_iterator tmp = *this;
            ++tmp;
            return iterator(tmp.node_);
        }

      private:
        friend class set;
        node_type* node_;
        node_type* root_;

        explicit reverse_iterator(node_type* node, node_type* root) : node_(node), root_(root)
        {
        }
    };

    using const_iterator = iterator;
    using const_reverse_iterator = reverse_iterator;

    set() : root_(nullptr), size_(0), comp_(Compare())
    {
    }

    explicit set(const Compare& comp) : root_(nullptr), size_(0), comp_(comp)
    {
    }

    template<class InputIt>
    set(InputIt first, InputIt last, const Compare& comp = Compare())
      : root_(nullptr), size_(0), comp_(comp)
    {
        for (; first != last; ++first)
            insert(*first);
    }

    set(const set& other) : root_(nullptr), size_(0)
    {
        if (other.root_) {
            root_ = copier(other.root_);
            size_ = other.size_;
        }
    }

    set(set&& other) noexcept : root_(other.root_), size_(other.size_)
    {
        other.root_ = nullptr;
        other.size_ = 0;
    }

    set(std::initializer_list<value_type> init, const Compare& comp = Compare())
      : root_(nullptr), size_(0), comp_(comp)
    {
        for (const auto& value : init)
            insert(value);
    }

    ~set()
    {
        clear();
    }

    set& operator=(const set& other)
    {
        if (this != &other) {
            clear();
            if (other.root_) {
                root_ = copier(other.root_);
                size_ = other.size_;
            }
        }
        return *this;
    }

    set& operator=(set&& other) noexcept
    {
        if (this != &other) {
            clear();
            root_ = other.root_;
            size_ = other.size_;
            other.root_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    bool empty() const noexcept
    {
        return size_ == 0;
    }

    size_type size() const noexcept
    {
        return size_;
    }

    iterator begin() noexcept
    {
        if (root_)
            return iterator(leftmost(root_));
        return iterator(nullptr);
    }

    const_iterator begin() const noexcept
    {
        if (root_)
            return const_iterator(leftmost(root_));
        return const_iterator(nullptr);
    }

    const_iterator cbegin() const noexcept
    {
        return begin();
    }

    reverse_iterator rbegin()
    {
        return reverse_iterator(rightmost(root_), root_);
    }

    const_reverse_iterator rbegin() const
    {
        return const_reverse_iterator(rightmost(root_), root_);
    }

    const_reverse_iterator crbegin() const
    {
        return const_reverse_iterator(rightmost(root_), root_);
    }

    iterator end() noexcept
    {
        return iterator(nullptr);
    }

    const_iterator end() const noexcept
    {
        return const_iterator(nullptr);
    }

    const_iterator cend() const noexcept
    {
        return end();
    }

    reverse_iterator rend()
    {
        return reverse_iterator(nullptr, root_);
    }

    const_reverse_iterator rend() const
    {
        return const_reverse_iterator(nullptr, root_);
    }

    const_reverse_iterator crend() const
    {
        return const_reverse_iterator(nullptr, root_);
    }

    void clear() noexcept
    {
        clearer(root_);
        root_ = nullptr;
        size_ = 0;
    }

    ctl::pair<iterator, bool> insert(value_type&& value)
    {
        return insert_node(new node_type(ctl::move(value)));
    }

    ctl::pair<iterator, bool> insert(const value_type& value)
    {
        return insert_node(new node_type(value));
    }

    iterator insert(const_iterator hint, const value_type& value)
    {
        return insert(value).first;
    }

    iterator insert(const_iterator hint, value_type&& value)
    {
        return insert(ctl::move(value)).first;
    }

    template<class InputIt>
    void insert(InputIt first, InputIt last)
    {
        for (; first != last; ++first)
            insert(*first);
    }

    void insert(std::initializer_list<value_type> ilist)
    {
        for (const auto& value : ilist)
            insert(value);
    }

    template<class... Args>
    ctl::pair<iterator, bool> emplace(Args&&... args)
    {
        value_type value(ctl::forward<Args>(args)...);
        return insert(ctl::move(value));
    }

    iterator erase(iterator pos)
    {
        node_type* node = pos.node_;
        iterator res = ++pos;
        erase_node(node);
        return res;
    }

    size_type erase(const key_type& key)
    {
        node_type* node = get_element(key);
        if (node == nullptr)
            return 0;
        erase_node(node);
        return 1;
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        while (first != last)
            first = erase(first);
        return iterator(const_cast<node_type*>(last.node_));
    }

    void swap(set& other) noexcept
    {
        ctl::swap(root_, other.root_);
        ctl::swap(size_, other.size_);
    }

    ctl::pair<iterator, iterator> equal_range(const key_type& key)
    {
        return { iterator(get_floor(key)), iterator(get_ceiling(key)) };
    }

    ctl::pair<const_iterator, const_iterator> equal_range(
      const key_type& key) const
    {
        return { const_iterator(get_floor(key)),
                 const_iterator(get_ceiling(key)) };
    }

    iterator lower_bound(const key_type& key)
    {
        return iterator(get_floor(key));
    }

    const_iterator lower_bound(const key_type& key) const
    {
        return const_iterator(get_floor(key));
    }

    iterator upper_bound(const key_type& key)
    {
        return iterator(get_ceiling(key));
    }

    const_iterator upper_bound(const key_type& key) const
    {
        return const_iterator(get_ceiling(key));
    }

    size_type count(const key_type& key) const
    {
        return !!get_element(key);
    }

    iterator find(const key_type& key)
    {
        return iterator(get_element(key));
    }

    const_iterator find(const key_type& key) const
    {
        return const_iterator(get_element(key));
    }

    template<class... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args)
    {
        return emplace(ctl::forward<Args>(args)...).first;
    }

    void check() const
    {
        size_type count = 0;
        if (root_ != nullptr) {
            if (root_->is_red())
                // ILLEGAL TREE: root node must be black
                __builtin_trap();
            int black_height = -1;
            checker(root_, nullptr, 0, black_height);
            count = tally(root_);
        }
        if (count != size_)
            // ILLEGAL TREE: unexpected number of nodes
            __builtin_trap();
    }

  private:
    static node_type* leftmost(node_type* node) noexcept
    {
        while (node && node->left())
            node = node->left();
        return node;
    }

    static node_type* rightmost(node_type* node) noexcept
    {
        while (node && node->right)
            node = node->right;
        return node;
    }

    static optimizesize void clearer(node_type* node) noexcept
    {
        node_type* right;
        for (; node; node = right) {
            right = node->right;
            clearer(node->left());
            delete node;
        }
    }

    static optimizesize node_type* copier(const node_type* node)
    {
        if (node == nullptr)
            return nullptr;
        node_type* new_node = new node_type(node->value);
        new_node->left(copier(node->left()));
        new_node->right = copier(node->right);
        if (new_node->left())
            new_node->left()->parent = new_node;
        if (new_node->right)
            new_node->right->parent = new_node;
        return new_node;
    }

    static optimizesize size_type tally(const node_type* node)
    {
        if (node == nullptr)
            return 0;
        return 1 + tally(node->left()) + tally(node->right);
    }

    template<typename K>
    node_type* get_element(const K& key) const
    {
        node_type* current = root_;
        while (current != nullptr) {
            if (comp_(key, current->value)) {
                current = current->left();
            } else if (comp_(current->value, key)) {
                current = current->right;
            } else {
                return current;
            }
        }
        return nullptr;
    }

    template<typename K>
    node_type* get_floor(const K& key) const
    {
        node_type* result = nullptr;
        node_type* current = root_;
        while (current != nullptr) {
            if (!comp_(current->value, key)) {
                result = current;
                current = current->left();
            } else {
                current = current->right;
            }
        }
        return result;
    }

    template<typename K>
    node_type* get_ceiling(const K& key) const
    {
        node_type* result = nullptr;
        node_type* current = root_;
        while (current != nullptr) {
            if (comp_(key, current->value)) {
                result = current;
                current = current->left();
            } else {
                current = current->right;
            }
        }
        return result;
    }

    optimizesize ctl::pair<iterator, bool> insert_node(node_type* node)
    {
        if (root_ == nullptr) {
            root_ = node;
            root_->is_red(false);
            size_++;
            return { iterator(root_), true };
        }
        node_type* current = root_;
        node_type* parent = nullptr;
        while (current != nullptr) {
            parent = current;
            if (comp_(node->value, current->value)) {
                current = current->left();
            } else if (comp_(current->value, node->value)) {
                current = current->right;
            } else {
                delete node; // already exists
                return { iterator(current), false };
            }
        }
        if (comp_(node->value, parent->value)) {
            parent->left(node);
        } else {
            parent->right = node;
        }
        node->parent = parent;
        size_++;
        rebalance_after_insert(node);
        return { iterator(node), true };
    }

    optimizesize void erase_node(node_type* node)
    {
        node_type* y = node;
        node_type* x = nullptr;
        node_type* x_parent = nullptr;
        bool y_original_color = y->is_red();
        if (node->left() == nullptr) {
            x = node->right;
            transplant(node, node->right);
            x_parent = node->parent;
        } else if (node->right == nullptr) {
            x = node->left();
            transplant(node, node->left());
            x_parent = node->parent;
        } else {
            y = leftmost(node->right);
            y_original_color = y->is_red();
            x = y->right;
            if (y->parent == node) {
                if (x)
                    x->parent = y;
                x_parent = y;
            } else {
                transplant(y, y->right);
                y->right = node->right;
                y->right->parent = y;
                x_parent = y->parent;
            }
            transplant(node, y);
            y->left(node->left());
            y->left()->parent = y;
            y->is_red(node->is_red());
        }
        if (!y_original_color)
            rebalance_after_erase(x, x_parent);
        delete node;
        --size_;
    }

    optimizesize void left_rotate(node_type* x)
    {
        node_type* y = x->right;
        x->right = y->left();
        if (y->left() != nullptr)
            y->left()->parent = x;
        y->parent = x->parent;
        if (x->parent == nullptr) {
            root_ = y;
        } else if (x == x->parent->left()) {
            x->parent->left(y);
        } else {
            x->parent->right = y;
        }
        y->left(x);
        x->parent = y;
    }

    optimizesize void right_rotate(node_type* y)
    {
        node_type* x = y->left();
        y->left(x->right);
        if (x->right != nullptr)
            x->right->parent = y;
        x->parent = y->parent;
        if (y->parent == nullptr) {
            root_ = x;
        } else if (y == y->parent->right) {
            y->parent->right = x;
        } else {
            y->parent->left(x);
        }
        x->right = y;
        y->parent = x;
    }

    optimizesize void transplant(node_type* u, node_type* v)
    {
        if (u->parent == nullptr) {
            root_ = v;
        } else if (u == u->parent->left()) {
            u->parent->left(v);
        } else {
            u->parent->right = v;
        }
        if (v != nullptr)
            v->parent = u->parent;
    }

    optimizesize void checker(const node_type* node,
                              const node_type* parent,
                              int black_count,
                              int& black_height) const
    {
        if (node == nullptr) {
            // Leaf nodes are considered black
            if (black_height == -1) {
                black_height = black_count;
            } else if (black_count != black_height) {
                // ILLEGAL TREE: Black height mismatch
                __builtin_trap();
            }
            return;
        }
        if (node->parent != parent)
            // ILLEGAL TREE: Parent link is incorrect
            __builtin_trap();
        if (parent) {
            if (parent->left() == node && !comp_(node->value, parent->value))
                // ILLEGAL TREE: Binary search property violated on left child
                __builtin_trap();
            if (parent->right == node && !comp_(parent->value, node->value))
                // ILLEGAL TREE: Binary search property violated on right child
                __builtin_trap();
        }
        if (!node->is_red()) {
            black_count++;
        } else if (parent != nullptr && parent->is_red()) {
            // ILLEGAL TREE: Red node has red child
            __builtin_trap();
        }
        checker(node->left(), node, black_count, black_height);
        checker(node->right, node, black_count, black_height);
    }

    optimizesize void rebalance_after_insert(node_type* node)
    {
        node->is_red(true);
        while (node != root_ && node->parent->is_red()) {
            if (node->parent == node->parent->parent->left()) {
                node_type* uncle = node->parent->parent->right;
                if (uncle && uncle->is_red()) {
                    node->parent->is_red(false);
                    uncle->is_red(false);
                    node->parent->parent->is_red(true);
                    node = node->parent->parent;
                } else {
                    if (node == node->parent->right) {
                        node = node->parent;
                        left_rotate(node);
                    }
                    node->parent->is_red(false);
                    node->parent->parent->is_red(true);
                    right_rotate(node->parent->parent);
                }
            } else {
                node_type* uncle = node->parent->parent->left();
                if (uncle && uncle->is_red()) {
                    node->parent->is_red(false);
                    uncle->is_red(false);
                    node->parent->parent->is_red(true);
                    node = node->parent->parent;
                } else {
                    if (node == node->parent->left()) {
                        node = node->parent;
                        right_rotate(node);
                    }
                    node->parent->is_red(false);
                    node->parent->parent->is_red(true);
                    left_rotate(node->parent->parent);
                }
            }
        }
        root_->is_red(false);
    }

    optimizesize void rebalance_after_erase(node_type* node, node_type* parent)
    {
        while (node != root_ && (node == nullptr || !node->is_red())) {
            if (node == parent->left()) {
                node_type* sibling = parent->right;
                if (sibling->is_red()) {
                    sibling->is_red(false);
                    parent->is_red(true);
                    left_rotate(parent);
                    sibling = parent->right;
                }
                if ((sibling->left() == nullptr ||
                     !sibling->left()->is_red()) &&
                    (sibling->right == nullptr || !sibling->right->is_red())) {
                    sibling->is_red(true);
                    node = parent;
                    parent = node->parent;
                } else {
                    if (sibling->right == nullptr ||
                        !sibling->right->is_red()) {
                        sibling->left()->is_red(false);
                        sibling->is_red(true);
                        right_rotate(sibling);
                        sibling = parent->right;
                    }
                    sibling->is_red(parent->is_red());
                    parent->is_red(false);
                    sibling->right->is_red(false);
                    left_rotate(parent);
                    node = root_;
                    break;
                }
            } else {
                node_type* sibling = parent->left();
                if (sibling->is_red()) {
                    sibling->is_red(false);
                    parent->is_red(true);
                    right_rotate(parent);
                    sibling = parent->left();
                }
                if ((sibling->right == nullptr || !sibling->right->is_red()) &&
                    (sibling->left() == nullptr ||
                     !sibling->left()->is_red())) {
                    sibling->is_red(true);
                    node = parent;
                    parent = node->parent;
                } else {
                    if (sibling->left() == nullptr ||
                        !sibling->left()->is_red()) {
                        sibling->right->is_red(false);
                        sibling->is_red(true);
                        left_rotate(sibling);
                        sibling = parent->left();
                    }
                    sibling->is_red(parent->is_red());
                    parent->is_red(false);
                    sibling->left()->is_red(false);
                    right_rotate(parent);
                    node = root_;
                    break;
                }
            }
        }
        if (node != nullptr)
            node->is_red(false);
    }

    node_type* root_;
    size_type size_;
    Compare comp_;
};

template<class Key, typename Compare = ctl::less<Key>>
bool
operator==(const set<Key, Compare>& lhs, const set<Key, Compare>& rhs)
{
    if (lhs.size() != rhs.size())
        return false;
    auto i = lhs.cbegin();
    auto j = rhs.cbegin();
    for (; i != lhs.end(); ++i, ++j) {
        if (!(*i == *j))
            return false;
    }
    return true;
}

template<class Key, typename Compare = ctl::less<Key>>
bool
operator<(const set<Key, Compare>& lhs, const set<Key, Compare>& rhs)
{
    auto i = lhs.cbegin();
    auto j = rhs.cbegin();
    for (; i != lhs.end() && j != rhs.end(); ++i, ++j) {
        if (Compare()(*i, *j))
            return true;
        if (Compare()(*j, *i))
            return false;
    }
    return i == lhs.end() && j != rhs.end();
}

template<class Key, typename Compare = ctl::less<Key>>
bool
operator!=(const set<Key, Compare>& lhs, const set<Key, Compare>& rhs)
{
    return !(lhs == rhs);
}

template<class Key, typename Compare = ctl::less<Key>>
bool
operator<=(const set<Key, Compare>& lhs, const set<Key, Compare>& rhs)
{
    return !(rhs < lhs);
}

template<class Key, typename Compare = ctl::less<Key>>
bool
operator>(const set<Key, Compare>& lhs, const set<Key, Compare>& rhs)
{
    return rhs < lhs;
}

template<class Key, typename Compare = ctl::less<Key>>
bool
operator>=(const set<Key, Compare>& lhs, const set<Key, Compare>& rhs)
{
    return !(lhs < rhs);
}

template<class Key, typename Compare = ctl::less<Key>>
void
swap(set<Key, Compare>& lhs, set<Key, Compare>& rhs) noexcept;

} // namespace ctl

#endif // CTL_SET_H_
