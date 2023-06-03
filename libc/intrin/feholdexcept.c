#include "libc/runtime/fenv.h"

/**
 * Saves floating-point environment and clears current exceptions.
 */
int feholdexcept(fenv_t *envp) {
  fegetenv(envp);
  feclearexcept(FE_ALL_EXCEPT);
  return 0;
}
