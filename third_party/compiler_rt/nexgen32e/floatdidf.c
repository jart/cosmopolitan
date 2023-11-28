/* This file is distributed under the University of Illinois Open Source
 * License. See LICENSE.TXT for details.
 */

__static_yoink("huge_compiler_rt_license");

/* double __floatdidf(di_int a); */

#if defined(__x86_64__) || defined(_M_X64)

#include "third_party/compiler_rt/int_lib.h"

double __floatdidf(int64_t a)
{
	return (double)a;
}

#endif /* __x86_64__ */
