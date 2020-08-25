#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_BUFFER_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_BUFFER_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct Buffer {
  size_t i, n;
  char *p;
};

void AppendChar(struct Buffer *, char);
void AppendData(struct Buffer *, char *, size_t);
void AppendStr(struct Buffer *, const char *);
void AppendWide(struct Buffer *, wint_t);
void AppendFmt(struct Buffer *, const char *, ...);
ssize_t WriteBuffer(struct Buffer *, int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_BUFFER_H_ */
