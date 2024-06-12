//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <new>
#include <stdexcept>
#include <string>

#ifdef _LIBCPP_ABI_VCRUNTIME
#include "third_party/libcxx/src/support/runtime/stdexcept_vcruntime.ipp"
#else
#include "third_party/libcxx/src/support/runtime/stdexcept_default.ipp"
#endif
