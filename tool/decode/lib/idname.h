#ifndef COSMOPOLITAN_TOOL_DECODE_LIB_IDNAME_H_
#define COSMOPOLITAN_TOOL_DECODE_LIB_IDNAME_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct IdName {
  unsigned long id;
  const char *const name;
};

const char *findnamebyid(const struct IdName *names, unsigned long id);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_DECODE_LIB_IDNAME_H_ */
