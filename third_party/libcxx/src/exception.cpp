//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <exception>
#include <new>
#include <typeinfo>

#if defined(LIBCXXRT) || defined(LIBCXX_BUILDING_LIBCXXABI)
  #include <cxxabi.h>
  using namespace __cxxabiv1;
  #define HAVE_DEPENDENT_EH_ABI 1
#endif

#if defined(_LIBCPP_ABI_MICROSOFT)
#include "third_party/libcxx/src/support/runtime/exception_msvc.ipp"
#include "third_party/libcxx/src/support/runtime/exception_pointer_msvc.ipp"
#elif defined(_LIBCPPABI_VERSION)
#include "third_party/libcxx/src/support/runtime/exception_libcxxabi.ipp"
#include "third_party/libcxx/src/support/runtime/exception_pointer_cxxabi.ipp"
#elif defined(LIBCXXRT)
#include "third_party/libcxx/src/support/runtime/exception_libcxxrt.ipp"
#include "third_party/libcxx/src/support/runtime/exception_pointer_cxxabi.ipp"
#elif defined(__GLIBCXX__)
#include "third_party/libcxx/src/support/runtime/exception_glibcxx.ipp"
#include "third_party/libcxx/src/support/runtime/exception_pointer_glibcxx.ipp"
#else
#include "third_party/libcxx/src/include/atomic_support.h"
#include "third_party/libcxx/src/support/runtime/exception_fallback.ipp"
#include "third_party/libcxx/src/support/runtime/exception_pointer_unimplemented.ipp"
#endif
