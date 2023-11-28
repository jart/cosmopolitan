#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_CASE_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_CASE_H_

#define CASE(OP, CODE) \
  case OP:             \
    CODE;              \
    break

#define CASR(OP, CODE) \
  case OP:             \
    CODE;              \
    return

#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_CASE_H_ */
