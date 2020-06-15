#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_PERSIST_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_PERSIST_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct ObjectParam {
  size_t size;
  void *p;
  uint32_t *magic;
  int32_t *abi;
  struct ObjectArrayParam {
    size_t len;
    size_t size;
    void *pp;
  } * arrays;
};

void PersistObject(const char *, size_t, const struct ObjectParam *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_PERSIST_H_ */
