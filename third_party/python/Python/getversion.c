#include "third_party/python/Include/patchlevel.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pylifecycle.h"
/* clang-format off */

/* Return the full version string. */

const char *
Py_GetVersion(void)
{
	static char version[250];
	PyOS_snprintf(version, sizeof(version), "%.80s (%.80s) %.80s",
		      PY_VERSION, Py_GetBuildInfo(), Py_GetCompiler());
	return version;
}
