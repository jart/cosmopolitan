#include "libc/runtime/runtime.h"
#include "third_party/f2c/f2c.h"
#include "third_party/f2c/internal.h"

/**
 * This gives the effect of
 *
 *      subroutine exit(rc)
 *      integer*4 rc
 *      stop
 *      end
 *
 * with the added side effect of supplying rc as the program's exit code.
 */
void exit_(integer *rc) {
  f_exit();
  exit(*rc);
}
