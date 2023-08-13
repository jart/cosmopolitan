#ifndef COSMOPOLITAN_LIBC_STDBOOL_H_
#define COSMOPOLITAN_LIBC_STDBOOL_H_

#ifndef __cplusplus
#if __STDC_VERSION__ + 0 >= 201112
#define bool _Bool
#else
#define bool unsigned char
#endif
#define true 1
#define false 0
#endif /* __cplusplus */

#define __bool_true_false_are_defined

#endif /* COSMOPOLITAN_LIBC_STDBOOL_H_ */
