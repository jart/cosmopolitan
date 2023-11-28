#ifndef COSMOPOLITAN_LIBC_TESTLIB_ASPECT_INTERNAL_H_
#define COSMOPOLITAN_LIBC_TESTLIB_ASPECT_INTERNAL_H_
#include "libc/intrin/dll.h"
#include "libc/testlib/testlib.h"
COSMOPOLITAN_C_START_

#define TESTASPECT_CONTAINER(e) DLL_CONTAINER(struct TestAspect, elem, e)

struct TestAspect {
  bool once;
  void (*setup)(const testfn_t *);
  void (*teardown)(const testfn_t *);
  struct Dll elem;
};

extern struct Dll *testlib_aspects;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_TESTLIB_ASPECT_INTERNAL_H_ */
