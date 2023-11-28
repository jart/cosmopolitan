#ifndef NSYNC_ONCE_H_
#define NSYNC_ONCE_H_
#include "third_party/nsync/atomic.h"
COSMOPOLITAN_C_START_

/* An nsync_once allows a function to be called exactly once, when first
   referenced. */
typedef nsync_atomic_uint32_ nsync_once;

/* An initializer for nsync_once; it is guaranteed to be all zeroes. */
#define NSYNC_ONCE_INIT NSYNC_ATOMIC_UINT32_INIT_

/* The first time nsync_run_once() or nsync_run_once_arg() is applied to
   *once, the supplied function is run (with argument, in the case of
   nsync_run_once_arg()). Other callers will wait until the run of the
   function is complete, and then return without running the function
   again. */
void nsync_run_once(nsync_once *once, void (*f)(void));
void nsync_run_once_arg(nsync_once *once, void (*farg)(void *arg), void *arg);

/* Same as nsync_run_once()/nsync_run_once_arg() but uses a spinloop.
   Can be used on the same nsync_once as
   nsync_run_once/nsync_run_once_arg().

   These *_spin variants should be used only in contexts where normal
   blocking is disallowed, such as within user-space schedulers, when
   the runtime is not fully initialized, etc. They provide no
   significant performance benefit, and they should be avoided in normal
   code. */
void nsync_run_once_spin(nsync_once *once, void (*f)(void));
void nsync_run_once_arg_spin(nsync_once *once, void (*farg)(void *arg),
                             void *arg);

COSMOPOLITAN_C_END_
#endif /* NSYNC_ONCE_H_ */
