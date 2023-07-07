// clang-format off
/*
    Copyright (c) 2005-2021 Intel Corporation

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

#ifndef _TBB_intrusive_list_H
#define _TBB_intrusive_list_H

#include "third_party/tbb/detail/_intrusive_list_node.hh"

namespace tbb {
namespace detail {
namespace r1 {

using d1::intrusive_list_node;

//! List of element of type T, where T is derived from intrusive_list_node
/** The class is not thread safe. **/
template <class List, class T>
class intrusive_list_base {
    //! Pointer to the head node
    intrusive_list_node my_head;

    //! Number of list elements
    std::size_t my_size;

    static intrusive_list_node& node ( T& item ) { return List::node(item); }

    static T& item ( intrusive_list_node* node ) { return List::item(node); }

    static const T& item( const intrusive_list_node* node ) { return List::item(node); }

    template <typename DereferenceType>
    class iterator_impl {
        static_assert(std::is_same<DereferenceType, T>::value ||
                      std::is_same<DereferenceType, const T>::value,
                      "Incorrect DereferenceType in iterator_impl");

        using pointer_type = typename std::conditional<std::is_same<DereferenceType, T>::value,
                                                       intrusive_list_node*,
                                                       const intrusive_list_node*>::type;

    public:
        iterator_impl() : my_pos(nullptr) {}

        iterator_impl( pointer_type pos ) : my_pos(pos) {}

        iterator_impl& operator++() {
            my_pos = my_pos->my_next_node;
            return *this;
        }

        iterator_impl operator++( int ) {
            iterator_impl it(*this);
            ++*this;
            return it;
        }

        iterator_impl& operator--() {
            my_pos = my_pos->my_prev_node;
            return *this;
        }

        iterator_impl operator--( int ) {
            iterator_impl it(*this);
            --*this;
            return it;
        }

        bool operator==( const iterator_impl& rhs ) const {
            return my_pos == rhs.my_pos;
        }

        bool operator!=( const iterator_impl& rhs ) const {
            return my_pos != rhs.my_pos;
        }

        DereferenceType& operator*() const {
            return intrusive_list_base::item(my_pos);
        }

        DereferenceType* operator->() const {
            return &intrusive_list_base::item(my_pos);
        }
    private:
        // Node the iterator points to at the moment
        pointer_type my_pos;
    }; // class iterator_impl

    void assert_ok () const {
        __TBB_ASSERT( (my_head.my_prev_node == &my_head && !my_size) ||
                      (my_head.my_next_node != &my_head && my_size >0), "intrusive_list_base corrupted" );
#if TBB_USE_ASSERT >= 2
        std::size_t i = 0;
        for ( intrusive_list_node *n = my_head.my_next_node; n != &my_head; n = n->my_next_node )
            ++i;
        __TBB_ASSERT( my_size == i, "Wrong size" );
#endif /* TBB_USE_ASSERT >= 2 */
    }

public:
    using iterator = iterator_impl<T>;
    using const_iterator = iterator_impl<const T>;

    intrusive_list_base () : my_size(0) {
        my_head.my_prev_node = &my_head;
        my_head.my_next_node = &my_head;
    }

    bool empty () const { return my_head.my_next_node == &my_head; }

    std::size_t size () const { return my_size; }

    iterator begin () { return iterator(my_head.my_next_node); }

    iterator end () { return iterator(&my_head); }

    const_iterator begin () const { return const_iterator(my_head.my_next_node); }

    const_iterator end () const { return const_iterator(&my_head); }

    void push_front ( T& val ) {
        __TBB_ASSERT( node(val).my_prev_node == &node(val) && node(val).my_next_node == &node(val),
                    "Object with intrusive list node can be part of only one intrusive list simultaneously" );
        // An object can be part of only one intrusive list at the given moment via the given node member
        node(val).my_prev_node = &my_head;
        node(val).my_next_node = my_head.my_next_node;
        my_head.my_next_node->my_prev_node = &node(val);
        my_head.my_next_node = &node(val);
        ++my_size;
        assert_ok();
    }

    void remove( T& val ) {
        __TBB_ASSERT( node(val).my_prev_node != &node(val) && node(val).my_next_node != &node(val), "Element to remove is not in the list" );
        __TBB_ASSERT( node(val).my_prev_node->my_next_node == &node(val) && node(val).my_next_node->my_prev_node == &node(val), "Element to remove is not in the list" );
        --my_size;
        node(val).my_next_node->my_prev_node = node(val).my_prev_node;
        node(val).my_prev_node->my_next_node = node(val).my_next_node;
#if TBB_USE_ASSERT
        node(val).my_prev_node = node(val).my_next_node = &node(val);
#endif
        assert_ok();
    }

    iterator erase ( iterator it ) {
        T& val = *it;
        ++it;
        remove( val );
        return it;
    }

}; // intrusive_list_base

#if __TBB_TODO
// With standard compliant compilers memptr_intrusive_list could be named simply intrusive_list,
// and inheritance based intrusive_list version would become its partial specialization.
// Here are the corresponding declarations:

struct dummy_intrusive_list_item { intrusive_list_node my_node; };

template <class T, class U = dummy_intrusive_list_item, intrusive_list_node U::*NodePtr = &dummy_intrusive_list_item::my_node>
class intrusive_list : public intrusive_list_base<intrusive_list<T, U, NodePtr>, T>;

template <class T>
class intrusive_list<T, dummy_intrusive_list_item, &dummy_intrusive_list_item::my_node>
    : public intrusive_list_base<intrusive_list<T>, T>;

#endif /* __TBB_TODO */

//! Double linked list of items of type T containing a member of type intrusive_list_node.
/** NodePtr is a member pointer to the node data field. Class U is either T or
    a base class of T containing the node member. Default values exist for the sake
    of a partial specialization working with inheritance case.

    The list does not have ownership of its items. Its purpose is to avoid dynamic
    memory allocation when forming lists of existing objects.

    The class is not thread safe. **/
template <class T, class U, intrusive_list_node U::*NodePtr>
class memptr_intrusive_list : public intrusive_list_base<memptr_intrusive_list<T, U, NodePtr>, T>
{
    friend class intrusive_list_base<memptr_intrusive_list<T, U, NodePtr>, T>;

    static intrusive_list_node& node ( T& val ) { return val.*NodePtr; }

    static T& item ( intrusive_list_node* node ) {
        // Cannot use __TBB_offsetof (and consequently __TBB_get_object_ref) macro
        // with *NodePtr argument because gcc refuses to interpret pasted "->" and "*"
        // as member pointer dereferencing operator, and explicit usage of ## in
        // __TBB_offsetof implementation breaks operations with normal member names.
        return *reinterpret_cast<T*>((char*)node - ((ptrdiff_t)&(reinterpret_cast<T*>(0x1000)->*NodePtr) - 0x1000));
    }

    static const T& item( const intrusive_list_node* node ) {
        return item(const_cast<intrusive_list_node*>(node));
    }

}; // intrusive_list<T, U, NodePtr>

//! Double linked list of items of type T that is derived from intrusive_list_node class.
/** The list does not have ownership of its items. Its purpose is to avoid dynamic
    memory allocation when forming lists of existing objects.

    The class is not thread safe. **/
template <class T>
class intrusive_list : public intrusive_list_base<intrusive_list<T>, T>
{
    friend class intrusive_list_base<intrusive_list<T>, T>;

    static intrusive_list_node& node ( T& val ) { return val; }

    static T& item ( intrusive_list_node* node ) { return *static_cast<T*>(node); }

    static const T& item( const intrusive_list_node* node ) { return *static_cast<const T*>(node); }
}; // intrusive_list<T>

} // namespace r1
} // namespace detail
} // namespace tbb

#endif /* _TBB_intrusive_list_H */
