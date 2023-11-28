#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_JAVADOWN_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_JAVADOWN_H_
COSMOPOLITAN_C_START_

struct JavadownTag {
  char *tag;
  char *text;
};

struct JavadownTags {
  size_t n;
  struct JavadownTag * p;
};

struct Javadown {
  bool isfileoverview;
  char *title;
  char *text;
  struct JavadownTags tags;
};

struct Javadown *ParseJavadown(const char *, size_t);
void FreeJavadown(struct Javadown *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_JAVADOWN_H_ */
