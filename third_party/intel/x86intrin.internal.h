#ifndef _X86INTRIN_H_INCLUDED
#define _X86INTRIN_H_INCLUDED
#include "third_party/intel/ia32intrin.internal.h"

#ifndef __iamcu__
/* clang-format off */
#include "third_party/intel/immintrin.internal.h"
#include "third_party/intel/mm3dnow.internal.h"
#include "third_party/intel/fma4intrin.internal.h"
#include "third_party/intel/xopintrin.internal.h"
#include "third_party/intel/lwpintrin.internal.h"
#include "third_party/intel/tbmintrin.internal.h"
#include "third_party/intel/popcntintrin.internal.h"
#include "third_party/intel/mwaitxintrin.internal.h"
#include "third_party/intel/clzerointrin.internal.h"
/* clang-format on */
#endif /* __iamcu__ */

#endif /* _X86INTRIN_H_INCLUDED */
