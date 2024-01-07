//===------------------------ exception.cpp -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "third_party/libcxx/__config"
#include "third_party/libcxx/exception"
#include "third_party/libcxx/new"
#include "third_party/libcxx/typeinfo"

#if defined(LIBCXXRT) || defined(LIBCXX_BUILDING_LIBCXXABI)
  #include "third_party/libcxxabi/include/cxxabi.h"
  using namespace __cxxabiv1;
  #define HAVE_DEPENDENT_EH_ABI 1
#endif

#include "third_party/libcxx/exception_libcxxabi.hh"
#include "third_party/libcxx/exception_pointer_cxxabi.hh"
