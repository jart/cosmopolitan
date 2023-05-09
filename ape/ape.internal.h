#ifndef COSMOPOLITAN_APE_APE_INTERNAL_H_
#define COSMOPOLITAN_APE_APE_INTERNAL_H_
#include "libc/dce.h"

#if SupportsWindows() || SupportsMetal() || SupportsXnu()
#define APE_IS_SHELL_SCRIPT
#endif

#endif /* COSMOPOLITAN_APE_APE_INTERNAL_H_ */
