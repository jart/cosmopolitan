/* clang-format off */
#include "third_party/python/Include/Python.h"

#ifndef PLATFORM
#define PLATFORM "unknown"
#endif

const char *
Py_GetPlatform(void)
{
	return PLATFORM;
}
