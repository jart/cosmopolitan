#include "libc/limits.h"
#include "libc/mem/mem.h"
#include "third_party/dlmalloc/dlmalloc.internal.h"

/**
 * Sets memory allocation parameter.
 *
 * The format is to provide a (parameter-number, parameter-value) pair.
 * mallopt then sets the corresponding parameter to the argument value
 * if it can (i.e., so long as the value is meaningful), and returns 1
 * if successful else 0. SVID/XPG/ANSI defines four standard param
 * numbers for mallopt, normally defined in malloc.h. None of these are
 * use in this malloc, so setting them has no effect. But this malloc
 * also supports other options in mallopt:
 *
 *     Symbol            param #  default    allowed param values
 *     M_TRIM_THRESHOLD     -1   2*1024*1024   any   (-1U disables trimming)
 *     M_GRANULARITY        -2     page size   any power of 2 >= page size
 *     M_MMAP_THRESHOLD     -3      256*1024   any   (or 0 if no MMAP support)
 */
bool32 mallopt(int param_number, int value) {
  size_t val;
  ensure_initialization();
  val = (value == -1) ? SIZE_MAX : (size_t)value;
  switch (param_number) {
    case M_TRIM_THRESHOLD:
      g_mparams.trim_threshold = val;
      return true;
    case M_GRANULARITY:
      if (val >= g_mparams.page_size && ((val & (val - 1)) == 0)) {
        g_mparams.granularity = val;
        return true;
      } else {
        return false;
      }
    case M_MMAP_THRESHOLD:
      g_mparams.mmap_threshold = val;
      return true;
    default:
      return false;
  }
}
