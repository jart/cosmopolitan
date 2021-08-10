#ifndef Py_PYTHON_H
#define Py_PYTHON_H
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/fmt.h"
#include "libc/limits.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/rand/rand.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "third_party/python/Include/patchlevel.h"
#include "third_party/python/Include/pymacconfig.h"
#include "third_party/python/pyconfig.h"
/* clang-format off */

#include "third_party/python/Include/pyport.h"
#include "third_party/python/Include/pymacro.h"

/* A convenient way for code to know if clang's memory sanitizer is enabled. */
#if defined(__has_feature)
#  if __has_feature(memory_sanitizer)
#    if !defined(_Py_MEMORY_SANITIZER)
#      define _Py_MEMORY_SANITIZER
#    endif
#  endif
#endif

#include "third_party/python/Include/pyatomic.h"

/* Debug-mode build with pymalloc implies PYMALLOC_DEBUG.
 *  PYMALLOC_DEBUG is in error if pymalloc is not in use.
 */
#if defined(Py_DEBUG) && defined(WITH_PYMALLOC) && !defined(PYMALLOC_DEBUG)
#define PYMALLOC_DEBUG
#endif
#if defined(PYMALLOC_DEBUG) && !defined(WITH_PYMALLOC)
#error "PYMALLOC_DEBUG requires WITH_PYMALLOC"
#endif

#include "third_party/python/Include/pymath.h"
#include "third_party/python/Include/pytime.h"
#include "third_party/python/Include/pymem.h"

#include "third_party/python/Include/object.h"
#include "third_party/python/Include/objimpl.h"
#include "third_party/python/Include/typeslots.h"
#include "third_party/python/Include/pyhash.h"

#include "third_party/python/Include/pydebug.h"

#include "third_party/python/Include/bytearrayobject.h"
#include "third_party/python/Include/bytesobject.h"
#include "third_party/python/Include/unicodeobject.h"
#include "third_party/python/Include/longobject.h"
#include "third_party/python/Include/longintrepr.h"
#include "third_party/python/Include/boolobject.h"
#include "third_party/python/Include/floatobject.h"
#include "third_party/python/Include/complexobject.h"
#include "third_party/python/Include/rangeobject.h"
#include "third_party/python/Include/memoryobject.h"
#include "third_party/python/Include/tupleobject.h"
#include "third_party/python/Include/listobject.h"
#include "third_party/python/Include/dictobject.h"
#include "third_party/python/Include/odictobject.h"
#include "third_party/python/Include/enumobject.h"
#include "third_party/python/Include/setobject.h"
#include "third_party/python/Include/methodobject.h"
#include "third_party/python/Include/moduleobject.h"
#include "third_party/python/Include/funcobject.h"
#include "third_party/python/Include/classobject.h"
#include "third_party/python/Include/fileobject.h"
#include "third_party/python/Include/pycapsule.h"
#include "third_party/python/Include/traceback.h"
#include "third_party/python/Include/sliceobject.h"
#include "third_party/python/Include/cellobject.h"
#include "third_party/python/Include/iterobject.h"
#include "third_party/python/Include/genobject.h"
#include "third_party/python/Include/descrobject.h"
#include "third_party/python/Include/warnings.h"
#include "third_party/python/Include/weakrefobject.h"
#include "third_party/python/Include/structseq.h"
#include "third_party/python/Include/namespaceobject.h"

#include "third_party/python/Include/codecs.h"
#include "third_party/python/Include/pyerrors.h"

#include "third_party/python/Include/pystate.h"

#include "third_party/python/Include/pyarena.h"
#include "third_party/python/Include/modsupport.h"
#include "third_party/python/Include/pythonrun.h"
#include "third_party/python/Include/pylifecycle.h"
#include "third_party/python/Include/ceval.h"
#include "third_party/python/Include/sysmodule.h"
#include "third_party/python/Include/osmodule.h"
#include "third_party/python/Include/intrcheck.h"
#include "third_party/python/Include/import.h"

#include "third_party/python/Include/abstract.h"
#include "third_party/python/Include/bltinmodule.h"

#include "third_party/python/Include/compile.h"
#include "third_party/python/Include/eval.h"

#include "third_party/python/Include/pyctype.h"
#include "third_party/python/Include/pystrtod.h"
#include "third_party/python/Include/pystrcmp.h"
#include "third_party/python/Include/dtoa.h"
#include "third_party/python/Include/fileutils.h"
#include "third_party/python/Include/pyfpe.h"

#endif /* !Py_PYTHON_H */
