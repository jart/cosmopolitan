#ifndef COSMOPOLITAN_TOOL_DECODE_LIB_IDNAME_H_
#define COSMOPOLITAN_TOOL_DECODE_LIB_IDNAME_H_
COSMOPOLITAN_C_START_

struct IdName {
  unsigned long id;
  const char *const name;
};

const char *findnamebyid(const struct IdName *names, unsigned long id);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_DECODE_LIB_IDNAME_H_ */
