#ifndef Py_DTRACE_H
#define Py_DTRACE_H
COSMOPOLITAN_C_START_

#define PyDTrace_LINE(arg0, arg1, arg2)
#define PyDTrace_FUNCTION_ENTRY(arg0, arg1, arg2)
#define PyDTrace_FUNCTION_RETURN(arg0, arg1, arg2)
#define PyDTrace_GC_START(arg0)
#define PyDTrace_GC_DONE(arg0)
#define PyDTrace_INSTANCE_NEW_START(arg0)
#define PyDTrace_INSTANCE_NEW_DONE(arg0)
#define PyDTrace_INSTANCE_DELETE_START(arg0)
#define PyDTrace_INSTANCE_DELETE_DONE(arg0)

#define PyDTrace_LINE_ENABLED()                  0
#define PyDTrace_FUNCTION_ENTRY_ENABLED()        0
#define PyDTrace_FUNCTION_RETURN_ENABLED()       0
#define PyDTrace_GC_START_ENABLED()              0
#define PyDTrace_GC_DONE_ENABLED()               0
#define PyDTrace_INSTANCE_NEW_START_ENABLED()    0
#define PyDTrace_INSTANCE_NEW_DONE_ENABLED()     0
#define PyDTrace_INSTANCE_DELETE_START_ENABLED() 0
#define PyDTrace_INSTANCE_DELETE_DONE_ENABLED()  0

COSMOPOLITAN_C_END_
#endif /* !Py_DTRACE_H */
