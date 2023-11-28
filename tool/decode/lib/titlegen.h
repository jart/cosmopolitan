#ifndef COSMOPOLITAN_TOOL_DECODE_LIB_TITLEGEN_H_
#define COSMOPOLITAN_TOOL_DECODE_LIB_TITLEGEN_H_
COSMOPOLITAN_C_START_

struct Modeline {
  const char *emacs;
  const char *vim;
};

extern const struct Modeline kModelineAsm;

void showtitle(const char *brand, const char *tool, const char *title,
               const char *description, const struct Modeline *modeline);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_DECODE_LIB_TITLEGEN_H_ */
