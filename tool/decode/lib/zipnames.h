#ifndef COSMOPOLITAN_TOOL_DECODE_LIB_ZIPNAMES_H_
#define COSMOPOLITAN_TOOL_DECODE_LIB_ZIPNAMES_H_
#include "tool/decode/lib/idname.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const struct IdName kZipCompressionNames[];
extern const struct IdName kZipExtraNames[];
extern const struct IdName kZipIattrNames[];
extern const struct IdName kZipOsNames[];
extern const struct IdName kZipEraNames[];

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_DECODE_LIB_ZIPNAMES_H_ */
