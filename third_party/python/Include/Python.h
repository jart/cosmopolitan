#ifndef Py_PYTHON_H
#define Py_PYTHON_H

/* 
 * PYTHON DIAMOND DEPENDENCY HEADER 
 * 
 * If your editor isn't able to automatically insert #include lines,
 * then this header can make development easier. It's also great for
 * making beginner's tutorials simpler and more attractive.
 *
 * However it's sloppy to use something like this in the long term since
 * it's not a scalable dependency model. It makes builds slower, because
 * changing any single header will invalidate all the build artifacts.
 * 
 * So please consider doing the conscientious thing and avoid using it!
 */

#include "libc/errno.h"
#include "third_party/python/pyconfig.h"
#include "third_party/python/Include/patchlevel.h"
#include "third_party/python/pyconfig.h"
#include "third_party/python/Include/pyport.h"
#include "third_party/python/Include/pymacro.h"
#include "third_party/python/Include/pyatomic.h"
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
