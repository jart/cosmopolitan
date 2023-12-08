/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/str/str.h"
#include "third_party/python/Include/osdefs.h"
#include "third_party/python/Include/pydebug.h"
#include "third_party/python/Include/pylifecycle.h"
#include "third_party/python/Include/pymacro.h"

static wchar_t *default_home = NULL;
static wchar_t env_home[MAXPATHLEN+1];

void
Py_SetPythonHome(wchar_t *home)
{
    default_home = home;
}

wchar_t *
Py_GetPythonHome(void)
{
    wchar_t *home = default_home;
    if (home == NULL && !Py_IgnoreEnvironmentFlag) {
        char* chome = Py_GETENV("PYTHONHOME");
        if (chome) {
            size_t size = Py_ARRAY_LENGTH(env_home);
            size_t r = mbstowcs(env_home, chome, size);
            if (r != (size_t)-1 && r < size)
                home = env_home;
        }
    }
    return home;
}
