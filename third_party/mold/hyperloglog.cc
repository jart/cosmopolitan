// clang-format off
// This file implements HyperLogLog algorithm, which estimates
// the number of unique items in a given multiset.
//
// For more info, read
// https://engineering.fb.com/2018/12/13/data-infrastructure/hyperloglog

// TODO(fzakaria): changed from libcxx because pow symbol wasn't present.
#include "third_party/mold/common.h"

#include "libc/math.h"

namespace mold {

i64 HyperLogLog::get_cardinality() const {
  double z = 0;
  for (i64 val : buckets)
    z += pow(2, -val);
  return ALPHA * NBUCKETS * NBUCKETS / z;
}

} // namespace mold
