#ifndef _STDBOOL_H
#define _STDBOOL_H
#ifndef __cplusplus
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ <= 201710L
#define bool  _Bool
#define true  1
#define false 0
#endif
#else
#define _Bool bool
#endif
#define __bool_true_false_are_defined 1
#endif
