#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_JAVADOWN_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_JAVADOWN_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct Javadown {
  bool isfileoverview;
  char *title;
  char *text;
  struct JavadownTags {
    size_t n;
    struct JavadownTag {
      char *tag;
      char *text;
    } * p;
  } tags;
};

struct Javadown *ParseJavadown(const char *, size_t);
void FreeJavadown(struct Javadown *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_JAVADOWN_H_ */
