#ifndef COSMOPOLITAN_TOOL_DECODE_LIB_ELFIDNAMES_H_
#define COSMOPOLITAN_TOOL_DECODE_LIB_ELFIDNAMES_H_
#include "tool/decode/lib/idname.h"
COSMOPOLITAN_C_START_

extern const struct IdName kElfTypeNames[];
extern const struct IdName kElfOsabiNames[];
extern const struct IdName kElfClassNames[];
extern const struct IdName kElfDataNames[];
extern const struct IdName kElfMachineNames[];
extern const struct IdName kElfSegmentTypeNames[];
extern const struct IdName kElfSectionTypeNames[];
extern const struct IdName kElfSegmentFlagNames[];
extern const struct IdName kElfSectionFlagNames[];
extern const struct IdName kElfSymbolTypeNames[];
extern const struct IdName kElfSymbolBindNames[];
extern const struct IdName kElfSymbolVisibilityNames[];
extern const struct IdName kElfSpecialSectionNames[];
extern const struct IdName kElfNexgen32eRelocationNames[];

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_DECODE_LIB_ELFIDNAMES_H_ */
