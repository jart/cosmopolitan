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

#  include <cxxabi.h>
using namespace __cxxabiv1;
#  define HAVE_DEPENDENT_EH_ABI 1

#include "exception_libcxxabi.h"
#include "exception_pointer_cxxabi.h"
