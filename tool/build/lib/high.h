#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_HIGH_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_HIGH_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct High {
  bool active;
  uint8_t keyword;
  uint8_t reg;
  uint8_t literal;
  uint8_t label;
  uint8_t comment;
  uint8_t quote;
};

extern struct High g_high;

char *HighStart(char *, int);
char *HighEnd(char *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_HIGH_H_ */
