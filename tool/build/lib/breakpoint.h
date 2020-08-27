#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_BREAKPOINT_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_BREAKPOINT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct Breakpoints {
  size_t i, n;
  struct Breakpoint {
    int64_t addr;
    const char *symbol;
    bool disable;
    bool oneshot;
  } * p;
};

ssize_t IsAtBreakpoint(struct Breakpoints *, int64_t);
ssize_t PushBreakpoint(struct Breakpoints *, struct Breakpoint *);
void PopBreakpoint(struct Breakpoints *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_BREAKPOINT_H_ */
