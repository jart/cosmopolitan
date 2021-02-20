#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_LINES_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_LINES_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct Lines {
  size_t n;
  char **p;
};

struct Lines *NewLines(void);
void FreeLines(struct Lines *);
void AppendLine(struct Lines *, const char *, size_t);
void AppendLines(struct Lines *, const char *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_LINES_H_ */
