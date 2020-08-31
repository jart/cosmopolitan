#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_CASE_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_CASE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

#define CASE(OP, CODE) \
  case OP:             \
    CODE;              \
    break

#define CASR(OP, CODE) \
  case OP:             \
    CODE;              \
    return

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_CASE_H_ */
