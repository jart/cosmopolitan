#ifndef NSYNC_DEBUG_H_
#define NSYNC_DEBUG_H_
#include "third_party/nsync/cv.h"
#include "third_party/nsync/mu.h"
COSMOPOLITAN_C_START_

/* Debugging operations for mutexes and condition variables.

   These operations should not be relied upon for normal functionality.
   The implementation may be slow, output formats may change, and the
   implementation is free to yield the empty string. */

/* Place in buf[0,..,n-1] a nul-terminated, human readable string
   indicative of some of the internal state of the mutex or condition
   variable, and return buf. If n>=4, buffer overflow is indicated by
   placing the characters "..." at the end of the string.

   The *_and_waiters() variants attempt to output the waiter lists in
   addition to the basic state. These variants may acquire internal
   locks and follow internal pointers. Thus, they are riskier if invoked
   in an address space whose overall health is uncertain. */
char *nsync_mu_debug_state(nsync_mu *mu, char *buf, int n);
char *nsync_cv_debug_state(nsync_cv *cv, char *buf, int n);
char *nsync_mu_debug_state_and_waiters(nsync_mu *mu, char *buf, int n);
char *nsync_cv_debug_state_and_waiters(nsync_cv *cv, char *buf, int n);

/* Like nsync_*_debug_state_and_waiters(), but ignoring all locking and
   safety considerations, and using an internal, possibly static buffer
   that may be overwritten by subsequent or concurrent calls to these
   routines. These variants should be used only from an interactive
   debugger, when all other threads are stopped; the debugger is
   expected to recover from errors. */
char *nsync_mu_debugger(nsync_mu *mu);
char *nsync_cv_debugger(nsync_cv *cv);

COSMOPOLITAN_C_END_
#endif /* NSYNC_DEBUG_H_ */
