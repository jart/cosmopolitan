/* xalloc-oversized.h -- memory allocation size checking

   Copyright (C) 1990-2000, 2003-2004, 2006-2020 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef XALLOC_OVERSIZED_H_
#define XALLOC_OVERSIZED_H_

#include <stddef.h>
#include <stdint.h>

/* True if N * S would overflow in a size_t calculation,
   or would generate a value larger than PTRDIFF_MAX.
   This expands to a constant expression if N and S are both constants.
   By gnulib convention, SIZE_MAX represents overflow in size
   calculations, so the conservative size_t-based dividend to use here
   is SIZE_MAX - 1.  */
#define __xalloc_oversized(n, s) \
  ((size_t) (PTRDIFF_MAX < SIZE_MAX ? PTRDIFF_MAX : SIZE_MAX - 1) / (s) < (n))

#if PTRDIFF_MAX < SIZE_MAX
typedef ptrdiff_t __xalloc_count_type;
#else
typedef size_t __xalloc_count_type;
#endif

/* Return 1 if an array of N objects, each of size S, cannot exist
   reliably due to size or ptrdiff_t arithmetic overflow.  S must be
   positive and N must be nonnegative.  This is a macro, not a
   function, so that it works correctly even when SIZE_MAX < N.  */

#if 7 <= __GNUC__
# define xalloc_oversized(n, s) \
   __builtin_mul_overflow_p (n, s, (__xalloc_count_type) 1)
#elif 5 <= __GNUC__ && !defined __ICC && !__STRICT_ANSI__
# define xalloc_oversized(n, s) \
   (__builtin_constant_p (n) && __builtin_constant_p (s) \
    ? __xalloc_oversized (n, s) \
    : ({ __xalloc_count_type __xalloc_count; \
         __builtin_mul_overflow (n, s, &__xalloc_count); }))

/* Other compilers use integer division; this may be slower but is
   more portable.  */
#else
# define xalloc_oversized(n, s) __xalloc_oversized (n, s)
#endif

#endif /* !XALLOC_OVERSIZED_H_ */
