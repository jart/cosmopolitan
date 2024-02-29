// -*- C++ -*-
//===-------------------------- errno.h -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP_ERRNO_H
#define _LIBCPP_ERRNO_H

/*
    errno.h synopsis

Macros:

    EDOM
    EILSEQ  // C99
    ERANGE
    errno

*/

#include "third_party/libcxx/__config"

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

#include "libc/isystem/errno.h"

#ifdef __cplusplus

#if !defined(EOWNERDEAD) || !defined(ENOTRECOVERABLE)

#ifdef ELAST

static const int __elast1 = ELAST + 1;
static const int __elast2 = ELAST + 2;

#else

static const int __elast1 = 104;
static const int __elast2 = 105;

#endif

#ifdef ENOTRECOVERABLE

#define EOWNERDEAD __elast1

#ifdef ELAST
#undef ELAST
#define ELAST EOWNERDEAD
#endif

#elif defined(EOWNERDEAD)

#define ENOTRECOVERABLE __elast1
#ifdef ELAST
#undef ELAST
#define ELAST ENOTRECOVERABLE
#endif

#else // defined(EOWNERDEAD)

#define EOWNERDEAD __elast1
#define ENOTRECOVERABLE __elast2
#ifdef ELAST
#undef ELAST
#define ELAST ENOTRECOVERABLE
#endif

#endif // defined(EOWNERDEAD)

#endif // !defined(EOWNERDEAD) || !defined(ENOTRECOVERABLE)

//  supply errno values likely to be missing, particularly on Windows

#endif // __cplusplus

#endif // _LIBCPP_ERRNO_H
