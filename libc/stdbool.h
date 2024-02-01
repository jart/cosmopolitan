#ifndef COSMOPOLITAN_LIBC_STDBOOL_H_
#define COSMOPOLITAN_LIBC_STDBOOL_H_

#ifndef __cplusplus

#define bool _Bool
#if defined(__STDC_VERSION__) && __STDC_VERSION__ > 201710L
#define true  ((_Bool) + 1u)
#define false ((_Bool) + 0u)
#else
#define true  1
#define false 0
#endif

#else /* __cplusplus */

#define _Bool bool

#endif /* __cplusplus */

#define __bool_true_false_are_defined 1

#endif /* COSMOPOLITAN_LIBC_STDBOOL_H_ */
