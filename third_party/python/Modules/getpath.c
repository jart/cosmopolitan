/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/cxxabi.h"
#include "libc/errno.h"
#include "libc/intrin/cmpxchg.h"
#include "libc/log/log.h"
#include "libc/mem/alloca.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/s.h"
#include "libc/x/x.h"
#include "third_party/python/Include/fileutils.h"
#include "third_party/python/Include/osdefs.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pymem.h"
/* Return the initial module search path. */

#pragma GCC diagnostic ignored "-Wunused-function" // search_for_exec_prefix
#pragma GCC diagnostic ignored "-Wunused-but-set-variable" // separator

/* Search in some common locations for the associated Python libraries.
 *
 * Two directories must be found, the platform independent directory
 * (prefix), containing the common .py and .pyc files, and the platform
 * dependent directory (exec_prefix), containing the shared library
 * modules.  Note that prefix and exec_prefix can be the same directory,
 * but for some installations, they are different.
 *
 * Py_GetPath() carries out separate searches for prefix and exec_prefix.
 * Each search tries a number of different locations until a ``landmark''
 * file or directory is found.  If no prefix or exec_prefix is found, a
 * warning message is issued and the preprocessor defined PREFIX and
 * EXEC_PREFIX are used (even though they will not work); python carries on
 * as best as is possible, but most imports will fail.
 *
 * Before any searches are done, the location of the executable is
 * determined.  If argv[0] has one or more slashes in it, it is used
 * unchanged.  Otherwise, it must have been invoked from the shell's path,
 * so we search $PATH for the named executable and use that.  If the
 * executable was not found on $PATH (or there was no $PATH environment
 * variable), the original argv[0] string is used.
 *
 * Next, the executable location is examined to see if it is a symbolic
 * link.  If so, the link is chased (correctly interpreting a relative
 * pathname if one is found) and the directory of the link target is used.
 *
 * Finally, argv0_path is set to the directory containing the executable
 * (i.e. the last component is stripped).
 *
 * With argv0_path in hand, we perform a number of steps.  The same steps
 * are performed for prefix and for exec_prefix, but with a different
 * landmark.
 *
 * Step 1. Are we running python out of the build directory?  This is
 * checked by looking for a different kind of landmark relative to
 * argv0_path.  For prefix, the landmark's path is derived from the VPATH
 * preprocessor variable (taking into account that its value is almost, but
 * not quite, what we need).  For exec_prefix, the landmark is
 * pybuilddir.txt.  If the landmark is found, we're done.
 *
 * For the remaining steps, the prefix landmark will always be
 * lib/python$VERSION/os.py and the exec_prefix will always be
 * lib/python$VERSION/lib-dynload, where $VERSION is Python's version
 * number as supplied by the Makefile.  Note that this means that no more
 * build directory checking is performed; if the first step did not find
 * the landmarks, the assumption is that python is running from an
 * installed setup.
 *
 * Step 2. See if the $PYTHONHOME environment variable points to the
 * installed location of the Python libraries.  If $PYTHONHOME is set, then
 * it points to prefix and exec_prefix.  $PYTHONHOME can be a single
 * directory, which is used for both, or the prefix and exec_prefix
 * directories separated by a colon.
 *
 * Step 3. Try to find prefix and exec_prefix relative to argv0_path,
 * backtracking up the path until it is exhausted.  This is the most common
 * step to succeed.  Note that if prefix and exec_prefix are different,
 * exec_prefix is more likely to be found; however if exec_prefix is a
 * subdirectory of prefix, both will be found.
 *
 * Step 4. Search the directories pointed to by the preprocessor variables
 * PREFIX and EXEC_PREFIX.  These are supplied by the Makefile but can be
 * passed in as options to the configure script.
 *
 * That's it!
 *
 * Well, almost.  Once we have determined prefix and exec_prefix, the
 * preprocessor variable PYTHONPATH is used to construct a path.  Each
 * relative path on PYTHONPATH is prefixed with prefix.  Then the directory
 * containing the shared library modules is appended.  The environment
 * variable $PYTHONPATH is inserted in front of it all.  Finally, the
 * prefix and exec_prefix globals are tweaked so they reflect the values
 * expected by other code, by stripping the "lib/python$VERSION/..." stuff
 * off.  If either points to the build directory, the globals are reset to
 * the corresponding preprocessor variables (so sys.prefix will reflect the
 * installation location, even though sys.path points into the build
 * directory).  This seems to make more sense given that currently the only
 * known use of sys.prefix and sys.exec_prefix is for the ILU installation
 * process to find the installed Python tree.
 *
 * An embedding application can use Py_SetPath() to override all of
 * these authomatic path computations.
 *
 * NOTE: Windows MSVC builds use PC/getpathp.c instead!
 */

wchar_t *Py_GetProgramName(void);

#if !defined(PREFIX) || !defined(EXEC_PREFIX) || !defined(VERSION) || !defined(VPATH)
#define PREFIX L"Lib"
#define EXEC_PREFIX L"build"
#define VERSION L"3.6"
#define VPATH ""
// #error "PREFIX, EXEC_PREFIX, VERSION, and VPATH must be constant defined"
#endif

#ifndef LANDMARK
#define LANDMARK L"os.py"
#endif

#define LIMITED_SEARCH_PATH L"/zip/.python"

static wchar_t *progpath;
static wchar_t *prefix = LIMITED_SEARCH_PATH;
static wchar_t *exec_prefix = LIMITED_SEARCH_PATH;
static wchar_t *module_search_path = LIMITED_SEARCH_PATH;

/* Get file status. Encode the path to the locale encoding. */

static int
_Py_wstat(const wchar_t* path, struct stat *buf)
{
    int err;
    char *fname;
    fname = Py_EncodeLocale(path, NULL);
    if (fname == NULL) {
        errno = EINVAL;
        return -1;
    }
    err = stat(fname, buf);
    PyMem_Free(fname);
    return err;
}

static void
reduce(wchar_t *dir)
{
    size_t i = wcslen(dir);
    while (i > 0 && dir[i] != SEP)
        --i;
    dir[i] = '\0';
}

static int
isfile(wchar_t *filename)          /* Is file, not directory */
{
    struct stat buf;
    if (_Py_wstat(filename, &buf) != 0)
        return 0;
    if (!S_ISREG(buf.st_mode))
        return 0;
    return 1;
}


static int
ismodule(wchar_t *filename)        /* Is module -- check for .pyc too */
{
    if (isfile(filename))
        return 1;

    /* Check for the compiled version of prefix. */
    if (wcslen(filename) < MAXPATHLEN) {
        wcscat(filename, L"c");
        if (isfile(filename))
            return 1;
    }
    return 0;
}


static int
isxfile(wchar_t *filename)         /* Is executable file */
{
    struct stat buf;
    if (_Py_wstat(filename, &buf) != 0)
        return 0;
    if (!S_ISREG(buf.st_mode))
        return 0;
    if ((buf.st_mode & 0111) == 0)
        return 0;
    return 1;
}


static int
isdir(wchar_t *filename)                   /* Is directory */
{
    struct stat buf;
    if (_Py_wstat(filename, &buf) != 0)
        return 0;
    if (!S_ISDIR(buf.st_mode))
        return 0;
    return 1;
}


/* Add a path component, by appending stuff to buffer.
   buffer must have at least MAXPATHLEN + 1 bytes allocated, and contain a
   NUL-terminated string with no more than MAXPATHLEN characters (not counting
   the trailing NUL).  It's a fatal error if it contains a string longer than
   that (callers must be careful!).  If these requirements are met, it's
   guaranteed that buffer will still be a NUL-terminated string with no more
   than MAXPATHLEN characters at exit.  If stuff is too long, only as much of
   stuff as fits will be appended.
*/
static void
joinpath(wchar_t *buffer, wchar_t *stuff)
{
    size_t n, k;
    if (stuff[0] == SEP)
        n = 0;
    else {
        n = wcslen(buffer);
        if (n > 0 && buffer[n-1] != SEP && n < MAXPATHLEN)
            buffer[n++] = SEP;
    }
    if (n > MAXPATHLEN)
        Py_FatalError("buffer overflow in getpath.c's joinpath()");
    k = wcslen(stuff);
    if (n + k > MAXPATHLEN)
        k = MAXPATHLEN - n;
    wcsncpy(buffer+n, stuff, k);
    buffer[n+k] = '\0';
}

/* copy_absolute requires that path be allocated at least
   MAXPATHLEN + 1 bytes and that p be no more than MAXPATHLEN bytes. */
static void
copy_absolute(wchar_t *path, wchar_t *p, size_t pathlen)
{
    if (p[0] == SEP)
        wcscpy(path, p);
    else {
        if (!_Py_wgetcwd(path, pathlen)) {
            /* unable to get the current directory */
            wcscpy(path, p);
            return;
        }
        if (p[0] == '.' && p[1] == SEP)
            p += 2;
        joinpath(path, p);
    }
}

/* absolutize() requires that path be allocated at least MAXPATHLEN+1 bytes. */
static void
absolutize(wchar_t *path)
{
    wchar_t *buffer = gc(calloc(MAXPATHLEN+1, sizeof(wchar_t)));
    if (path[0] == SEP) return;
    copy_absolute(buffer, path, MAXPATHLEN+1);
    wcscpy(path, buffer);
}

/* search for a prefix value in an environment file. If found, copy it
   to the provided buffer, which is expected to be no more than MAXPATHLEN
   bytes long.
*/

static int
find_env_config_value(FILE * env_file, const wchar_t * key, wchar_t * value)
{
    int result = 0; /* meaning not found */
    char buffer[MAXPATHLEN*2+1];  /* allow extra for key, '=', etc. */

    fseek(env_file, 0, SEEK_SET);
    while (!feof(env_file)) {
        char * p = fgets(buffer, MAXPATHLEN*2, env_file);
        wchar_t tmpbuffer[MAXPATHLEN*2+1];
        PyObject * decoded;
        int n;

        if (p == NULL)
            break;
        n = strlen(p);
        if (p[n - 1] != '\n') {
            /* line has overflowed - bail */
            break;
        }
        if (p[0] == '#')    /* Comment - skip */
            continue;
        decoded = PyUnicode_DecodeUTF8(buffer, n, "surrogateescape");
        if (decoded != NULL) {
            Py_ssize_t k;
            wchar_t * state;
            k = PyUnicode_AsWideChar(decoded,
                                     tmpbuffer, MAXPATHLEN * 2);
            Py_DECREF(decoded);
            if (k >= 0) {
                wchar_t * tok = wcstok(tmpbuffer, L" \t\r\n", &state);
                if ((tok != NULL) && !wcscmp(tok, key)) {
                    tok = wcstok(NULL, L" \t", &state);
                    if ((tok != NULL) && !wcscmp(tok, L"=")) {
                        tok = wcstok(NULL, L"\r\n", &state);
                        if (tok != NULL) {
                            wcsncpy(value, tok, MAXPATHLEN);
                            result = 1;
                            break;
                        }
                    }
                }
            }
        }
    }
    return result;
}

/* search_for_prefix requires that argv0_path be no more than MAXPATHLEN
   bytes long.
*/
static int
search_for_prefix(wchar_t *argv0_path, wchar_t *home, wchar_t *_prefix,
                  wchar_t *lib_python)
{
    size_t n;
    wchar_t *vpath;

    /* If PYTHONHOME is set, we believe it unconditionally */
    if (home) {
        wchar_t *delim;
        wcsncpy(prefix, home, MAXPATHLEN);
        prefix[MAXPATHLEN] = L'\0';
        delim = wcschr(prefix, DELIM);
        if (delim)
            *delim = L'\0';
        joinpath(prefix, lib_python);
        joinpath(prefix, LANDMARK);
        return 1;
    }

    /* Check to see if argv[0] is in the build directory */
    wcsncpy(prefix, argv0_path, MAXPATHLEN);
    prefix[MAXPATHLEN] = L'\0';
    joinpath(prefix, L"Modules/Setup");
    if (isfile(prefix)) {
        /* Check VPATH to see if argv0_path is in the build directory. */
        vpath = Py_DecodeLocale(VPATH, NULL);
        if (vpath != NULL) {
            wcsncpy(prefix, argv0_path, MAXPATHLEN);
            prefix[MAXPATHLEN] = L'\0';
            joinpath(prefix, vpath);
            PyMem_RawFree(vpath);
            joinpath(prefix, L"Lib");
            joinpath(prefix, LANDMARK);
            if (ismodule(prefix))
                return -1;
        }
    }

    /* Search from argv0_path, until root is found */
    copy_absolute(prefix, argv0_path, MAXPATHLEN+1);
    do {
        n = wcslen(prefix);
        joinpath(prefix, lib_python);
        joinpath(prefix, LANDMARK);
        if (ismodule(prefix))
            return 1;
        prefix[n] = L'\0';
        reduce(prefix);
    } while (prefix[0]);

    /* Look at configure's PREFIX */
    wcsncpy(prefix, _prefix, MAXPATHLEN);
    prefix[MAXPATHLEN] = L'\0';
    joinpath(prefix, lib_python);
    joinpath(prefix, LANDMARK);
    if (ismodule(prefix))
        return 1;

    /* Fail */
    return 0;
}


/* search_for_exec_prefix requires that argv0_path be no more than
   MAXPATHLEN bytes long.
*/
static int
search_for_exec_prefix(wchar_t *argv0_path, wchar_t *home,
                       wchar_t *_exec_prefix, wchar_t *lib_python)
{
    size_t n;

    /* If PYTHONHOME is set, we believe it unconditionally */
    if (home) {
        wchar_t *delim;
        delim = wcschr(home, DELIM);
        if (delim)
            wcsncpy(exec_prefix, delim+1, MAXPATHLEN);
        else
            wcsncpy(exec_prefix, home, MAXPATHLEN);
        exec_prefix[MAXPATHLEN] = L'\0';
        joinpath(exec_prefix, lib_python);
        joinpath(exec_prefix, L"lib-dynload");
        return 1;
    }

    /* Check to see if argv[0] is in the build directory. "pybuilddir.txt"
       is written by setup.py and contains the relative path to the location
       of shared library modules. */
    wcsncpy(exec_prefix, argv0_path, MAXPATHLEN);
    exec_prefix[MAXPATHLEN] = L'\0';
    joinpath(exec_prefix, L"pybuilddir.txt");
    if (isfile(exec_prefix)) {
        FILE *f = _Py_wfopen(exec_prefix, L"rb");
        if (f == NULL)
            errno = 0;
        else {
            char buf[MAXPATHLEN+1];
            PyObject *decoded;
            wchar_t rel_builddir_path[MAXPATHLEN+1];
            n = fread(buf, 1, MAXPATHLEN, f);
            buf[n] = '\0';
            fclose(f);
            decoded = PyUnicode_DecodeUTF8(buf, n, "surrogateescape");
            if (decoded != NULL) {
                Py_ssize_t k;
                k = PyUnicode_AsWideChar(decoded,
                                         rel_builddir_path, MAXPATHLEN);
                Py_DECREF(decoded);
                if (k >= 0) {
                    rel_builddir_path[k] = L'\0';
                    wcsncpy(exec_prefix, argv0_path, MAXPATHLEN);
                    exec_prefix[MAXPATHLEN] = L'\0';
                    joinpath(exec_prefix, rel_builddir_path);
                    return -1;
                }
            }
        }
    }

    /* Search from argv0_path, until root is found */
    copy_absolute(exec_prefix, argv0_path, MAXPATHLEN+1);
    do {
        n = wcslen(exec_prefix);
        joinpath(exec_prefix, lib_python);
        joinpath(exec_prefix, L"lib-dynload");
        if (isdir(exec_prefix))
            return 1;
        exec_prefix[n] = L'\0';
        reduce(exec_prefix);
    } while (exec_prefix[0]);

    /* Look at configure's EXEC_PREFIX */
    wcsncpy(exec_prefix, _exec_prefix, MAXPATHLEN);
    exec_prefix[MAXPATHLEN] = L'\0';
    joinpath(exec_prefix, lib_python);
    joinpath(exec_prefix, L"lib-dynload");
    if (isdir(exec_prefix))
        return 1;

    /* Fail */
    return 0;
}

static void
calculate_path(void)
{
    static wchar_t delimiter[2] = {DELIM, '\0'};
    static wchar_t separator[2] = {SEP, '\0'};
    /* ignore PYTHONPATH/PYTHONHOME for now */
    // char *_rtpypath = Py_GETENV("PYTHONPATH");
    /* XXX use wide version on Windows */
    // wchar_t *rtpypath = NULL;
    // wchar_t *home = Py_GetPythonHome();
    char *_path = getenv("PATH");
    wchar_t *path_buffer = NULL;
    wchar_t *path = NULL;
    wchar_t *prog = Py_GetProgramName();
    /* wont need zip_path because embedded stdlib inside executable */
    /* wchar_t zip_path[MAXPATHLEN+1]; */
    wchar_t *buf;
    size_t bufsz;
    size_t ape_length;
    wchar_t *ape_path = gc(calloc(MAXPATHLEN+1, sizeof(wchar_t)));
    wchar_t *argv0_path = gc(calloc(MAXPATHLEN+1, sizeof(wchar_t)));
    wchar_t *ape_lib_path = gc(calloc(MAXPATHLEN+1, sizeof(wchar_t)));
    wchar_t *ape_exec_path = gc(calloc(MAXPATHLEN+1, sizeof(wchar_t)));
    wchar_t *package_path = gc(calloc(MAXPATHLEN+1, sizeof(wchar_t)));
    wchar_t *ape_package_path = gc(calloc(MAXPATHLEN+1, sizeof(wchar_t)));
    if (IsWindows()) {
        delimiter[0] = L';';
        separator[0] = L'\\';
    }
    if (_path) {
        path_buffer = Py_DecodeLocale(_path, NULL);
        path = path_buffer;
    }
    /* If there is no slash in the argv0 path, then we have to
     * assume python is on the user's $PATH, since there's no
     * other way to find a directory to start the search from.  If
     * $PATH isn't exported, you lose.
     */
    if (wcschr(prog, SEP)) {
        wcsncpy(progpath, prog, MAXPATHLEN);
    } else if (path) {
        while (1) {
            wchar_t *delim = wcschr(path, DELIM);
            if (delim) {
                size_t len = delim - path;
                if (len > MAXPATHLEN)
                    len = MAXPATHLEN;
                wcsncpy(progpath, path, len);
                *(progpath + len) = '\0';
            }
            else
                wcsncpy(progpath, path, MAXPATHLEN);
            joinpath(progpath, prog);
            if (isxfile(progpath))
                break;
            if (!delim) {
                progpath[0] = L'\0';
                break;
            }
            path = delim + 1;
        }
    } else {
        progpath[0] = '\0';
    }
    PyMem_RawFree(path_buffer);
    if (progpath[0] != SEP && progpath[0] != '\0')
        absolutize(progpath);
    wcsncpy(argv0_path, progpath, MAXPATHLEN);
    argv0_path[MAXPATHLEN] = '\0';
    reduce(argv0_path);
    /* At this point, argv0_path is guaranteed to be less than
       MAXPATHLEN bytes long.
    */
    /* not searching for pyvenv.cfg */
    /* Avoid absolute path for prefix */
    wcsncpy(prefix, 
            L"third_party/python/Lib", 
            MAXPATHLEN);
    /* wcsncpy(prefix, */
    /*         L"/zip/.python", */
    /*         MAXPATHLEN); */
    /* Avoid absolute path for exec_prefix */
    wcsncpy(exec_prefix, L"build/lib.linux-x86_64-3.6", MAXPATHLEN);
    wcsncpy(package_path, L"Lib/site-packages", MAXPATHLEN);
    /* add paths for the internal store of the APE */
    if (wcslen(progpath) > 0 && wcslen(progpath) + 1 < MAXPATHLEN)
        wcsncpy(ape_path, progpath, MAXPATHLEN);
    else
        wcsncpy(ape_path, prog, MAXPATHLEN);
    ape_length = wcslen(ape_path);
    wcsncpy(ape_lib_path, ape_path, MAXPATHLEN);
    // extra 1 at the start for the slash
    if(ape_length + 1 + wcslen(prefix) + 1 < MAXPATHLEN)
    {
        ape_lib_path[ape_length] = L'/';
        wcscat(ape_lib_path + ape_length + 1, prefix);
    }
    wcsncpy(ape_exec_path, ape_path, MAXPATHLEN);
    if(ape_length + 1 + wcslen(exec_prefix) + 1 < MAXPATHLEN)
    {
        ape_exec_path[ape_length] = L'/';
        wcscat(ape_exec_path + ape_length + 1, exec_prefix);
    }
    wcsncpy(ape_package_path, ape_path, MAXPATHLEN);
    if(ape_length + 1 + wcslen(package_path) + 1 < MAXPATHLEN)
    {
        ape_package_path[ape_length] = L'/';
        wcscat(ape_package_path + ape_length + 1, package_path);
    }
    /* Calculate size of return buffer */
    bufsz = 0;
    bufsz += wcslen(ape_lib_path) + 1;
    bufsz += wcslen(ape_exec_path) + 1;
    bufsz += wcslen(ape_package_path) + 1;
    bufsz += wcslen(ape_path) + 1;
    bufsz += wcslen(prefix) + 1;
    bufsz += wcslen(exec_prefix) + 1;
    bufsz += wcslen(package_path) + 1;
    /* This is the only malloc call in this file */
    buf = PyMem_RawMalloc(bufsz * sizeof(wchar_t));
    if (buf == NULL) {
        Py_FatalError(
            "Not enough memory for dynamic PYTHONPATH");
    }
    buf[0] = L'\0';
    wcscat(buf, prefix);
    wcscat(buf, delimiter);
    wcscat(buf, package_path);
    wcscat(buf, delimiter);
    wcscat(buf, ape_lib_path);
    wcscat(buf, delimiter);
    wcscat(buf, ape_package_path);
    wcscat(buf, delimiter);
    wcscat(buf, ape_exec_path);
    wcscat(buf, delimiter);
    wcscat(buf, ape_path);
    wcscat(buf, delimiter);
    /* Finally, on goes the directory for dynamic-load modules */
    wcscat(buf, exec_prefix);
    /* And publish the results */
    module_search_path = buf;
    // printf("%ls\n", buf);
}

/* External interface */
void
Py_SetPath(const wchar_t *path)
{
    if (module_search_path != NULL) {
        PyMem_RawFree(module_search_path);
        module_search_path = NULL;
    }
    if (path != NULL) {
        wchar_t *prog = Py_GetProgramName();
        wcsncpy(progpath, prog, MAXPATHLEN);
        exec_prefix[0] = prefix[0] = L'\0';
        module_search_path = PyMem_RawMalloc((wcslen(path) + 1) * sizeof(wchar_t));
        if (module_search_path != NULL)
            wcscpy(module_search_path, path);
    }
}

wchar_t *
Py_GetPath(void)
{
    if (!module_search_path)
        calculate_path();
    return module_search_path;
}

wchar_t *
Py_GetPrefix(void)
{
    if (!module_search_path)
        calculate_path();
    return prefix;
}

wchar_t *
Py_GetExecPrefix(void)
{
    if (!module_search_path)
        calculate_path();
    return exec_prefix;
}

wchar_t *
Py_GetProgramFullPath(void)
{
    static bool once;
    if (_cmpxchg(&once, false, true)) {
        progpath = utf8to32(GetProgramExecutableName(), -1, 0);
        __cxa_atexit(free, progpath, 0);
    }
    return progpath;
}

void
Py_LimitedPath(void)
{
    prefix = wcscpy(PyMem_RawMalloc((wcslen(LIMITED_SEARCH_PATH) + 1) * 4), LIMITED_SEARCH_PATH);
    exec_prefix = wcscpy(PyMem_RawMalloc((wcslen(LIMITED_SEARCH_PATH) + 1) * 4), LIMITED_SEARCH_PATH);
    module_search_path = wcscpy(PyMem_RawMalloc((wcslen(LIMITED_SEARCH_PATH) + 1) * 4), LIMITED_SEARCH_PATH);
}
