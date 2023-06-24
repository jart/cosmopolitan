// clang-format off
/*
    Copyright (c) 2005-2022 Intel Corporation

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

#ifndef __TBB_detail__assert_H
#define __TBB_detail__assert_H

#include "third_party/tbb/detail/_config.hh"

#if __TBBMALLOC_BUILD
namespace rml { namespace internal {
#else
namespace tbb {
namespace detail {
namespace r1 {
#endif
//! Process an assertion failure.
/** Normally called from __TBB_ASSERT macro.
  If assertion handler is null, print message for assertion failure and abort.
  Otherwise call the assertion handler. */
TBB_EXPORT void __TBB_EXPORTED_FUNC assertion_failure(const char* location, int line, const char* expression, const char* comment);
#if __TBBMALLOC_BUILD
}} // namespaces rml::internal
#else
} // namespace r1
} // namespace detail
} // namespace tbb
#endif

#if __TBBMALLOC_BUILD
//! Release version of assertions
#define __TBB_ASSERT_RELEASE(predicate,message) ((predicate)?((void)0) : rml::internal::assertion_failure(__func__,__LINE__,#predicate,message))
#else
#define __TBB_ASSERT_RELEASE(predicate,message) ((predicate)?((void)0) : tbb::detail::r1::assertion_failure(__func__,__LINE__,#predicate,message))
#endif

#if TBB_USE_ASSERT
    //! Assert that predicate is true.
    /** If predicate is false, print assertion failure message.
        If the comment argument is not nullptr, it is printed as part of the failure message.
        The comment argument has no other effect. */
    #define __TBB_ASSERT(predicate,message) __TBB_ASSERT_RELEASE(predicate,message)
    //! "Extended" version
    #define __TBB_ASSERT_EX __TBB_ASSERT
#else
    //! No-op version of __TBB_ASSERT.
    #define __TBB_ASSERT(predicate,comment) ((void)0)
    //! "Extended" version is useful to suppress warnings if a variable is only used with an assert
    #define __TBB_ASSERT_EX(predicate,comment) ((void)(1 && (predicate)))
#endif // TBB_USE_ASSERT

#endif // __TBB_detail__assert_H
