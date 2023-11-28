#ifndef COSMOPOLITAN_TOOL_DECODE_LIB_MACHOIDNAMES_H_
#define COSMOPOLITAN_TOOL_DECODE_LIB_MACHOIDNAMES_H_
#include "tool/decode/lib/idname.h"
COSMOPOLITAN_C_START_

extern const struct IdName kMachoArchitectures[];
extern const struct IdName kMachoFileTypeNames[];
extern const struct IdName kMachoFlagNames[];
extern const struct IdName kMachoSegmentFlagNames[];
extern const struct IdName kMachoSectionTypeNames[];
extern const struct IdName kMachoSectionAttributeNames[];
extern const struct IdName kMachoLoadCommandNames[];
extern const struct IdName kMachoVmProtNames[];

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_DECODE_LIB_MACHOIDNAMES_H_ */
