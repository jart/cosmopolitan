#ifndef COSMOPOLITAN_TOOL_DECODE_LIB_FLAGGER_H_
#define COSMOPOLITAN_TOOL_DECODE_LIB_FLAGGER_H_
#include "tool/decode/lib/idname.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

char *RecreateFlags(const struct IdName *, unsigned long) nodiscard;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_DECODE_LIB_FLAGGER_H_ */
