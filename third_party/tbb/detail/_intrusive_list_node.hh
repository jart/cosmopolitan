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

#ifndef _TBB_detail__intrusive_list_node_H
#define _TBB_detail__intrusive_list_node_H

namespace tbb {
namespace detail {
namespace d1 {

//! Data structure to be inherited by the types that can form intrusive lists.
/** Intrusive list is formed by means of the member_intrusive_list<T> template class.
    Note that type T must derive from intrusive_list_node either publicly or
    declare instantiation member_intrusive_list<T> as a friend.
    This class implements a limited subset of std::list interface. **/
struct intrusive_list_node {
    intrusive_list_node* my_prev_node{};
    intrusive_list_node* my_next_node{};
#if TBB_USE_ASSERT
    intrusive_list_node() { my_prev_node = my_next_node = this; }
#endif /* TBB_USE_ASSERT */
};

} // namespace d1
} // namespace detail
} // namespace tbb

#endif // _TBB_detail__intrusive_list_node_H
