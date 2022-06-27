#ifndef COSMOPOLITAN_LIBC_RUNTIME_UNWIND_H_
#define COSMOPOLITAN_LIBC_RUNTIME_UNWIND_H_

#define UNW_TDEP_CURSOR_LEN 127

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

typedef enum {
  _URC_NO_REASON = 0,
  _URC_FOREIGN_EXCEPTION_CAUGHT = 1,
  _URC_FATAL_PHASE2_ERROR = 2,
  _URC_FATAL_PHASE1_ERROR = 3,
  _URC_NORMAL_STOP = 4,
  _URC_END_OF_STACK = 5,
  _URC_HANDLER_FOUND = 6,
  _URC_INSTALL_CONTEXT = 7,
  _URC_CONTINUE_UNWIND = 8
} _Unwind_Reason_Code;

typedef uint64_t unw_word_t;

typedef struct unw_cursor {
  unw_word_t opaque[UNW_TDEP_CURSOR_LEN];
} unw_cursor_t;

struct _Unwind_Context {
  unw_cursor_t cursor;
  int end_of_stack;
};

typedef _Unwind_Reason_Code (*_Unwind_Trace_Fn)(struct _Unwind_Context *,
                                                void *);

uintptr_t _Unwind_GetCFA(struct _Unwind_Context *);
uintptr_t _Unwind_GetIP(struct _Unwind_Context *);
_Unwind_Reason_Code _Unwind_Backtrace(_Unwind_Trace_Fn, void *);
void *_Unwind_FindEnclosingFunction(void *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_UNWIND_H_ */
