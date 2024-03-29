#ifndef Py_FILEUTILS_H
#define Py_FILEUTILS_H
#include "libc/calls/struct/stat.h"
#include "third_party/python/Include/object.h"
COSMOPOLITAN_C_START_

#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03050000
wchar_t * Py_DecodeLocale(
    const char *arg,
    size_t *size);

char* Py_EncodeLocale(
    const wchar_t *text,
    size_t *error_pos);
#endif

#ifndef Py_LIMITED_API

wchar_t * _Py_DecodeLocaleEx(
    const char *arg,
    size_t *size,
    int current_locale);

char* _Py_EncodeLocaleEx(
    const wchar_t *text,
    size_t *error_pos,
    int current_locale);

PyObject * _Py_device_encoding(int);

#if defined(MS_WINDOWS) || defined(__APPLE__)
    /* On Windows, the count parameter of read() is an int (bpo-9015, bpo-9611).
       On macOS 10.13, read() and write() with more than INT_MAX bytes
       fail with EINVAL (bpo-24658). */
#   define _PY_READ_MAX  INT_MAX
#   define _PY_WRITE_MAX INT_MAX
#else
    /* write() should truncate the input to PY_SSIZE_T_MAX bytes,
       but it's safer to do it ourself to have a portable behaviour */
#   define _PY_READ_MAX  PY_SSIZE_T_MAX
#   define _PY_WRITE_MAX PY_SSIZE_T_MAX
#endif

#ifdef MS_WINDOWS
struct _Py_stat_struct {
    unsigned long st_dev;
    uint64_t st_ino;
    unsigned short st_mode;
    int st_nlink;
    int st_uid;
    int st_gid;
    unsigned long st_rdev;
    __int64 st_size;
    time_t st_atime;
    int st_atime_nsec;
    time_t st_mtime;
    int st_mtime_nsec;
    time_t st_ctime;
    int st_ctime_nsec;
    unsigned long st_file_attributes;
};
#else
#  define _Py_stat_struct stat
#endif

int _Py_fstat(
    int fd,
    struct _Py_stat_struct *status);

int _Py_fstat_noraise(
    int fd,
    struct _Py_stat_struct *status);

int _Py_stat(
    PyObject *path,
    struct stat *status);

int _Py_open(
    const char *pathname,
    int flags);

int _Py_open_noraise(
    const char *pathname,
    int flags);

FILE * _Py_wfopen(
    const wchar_t *path,
    const wchar_t *mode);

FILE* _Py_fopen(
    const char *pathname,
    const char *mode);

FILE* _Py_fopen_obj(
    PyObject *path,
    const char *mode);

Py_ssize_t _Py_read(
    int fd,
    void *buf,
    size_t count);

Py_ssize_t _Py_write(
    int fd,
    const void *buf,
    size_t count);

Py_ssize_t _Py_write_noraise(
    int fd,
    const void *buf,
    size_t count);

#ifdef HAVE_READLINK
int _Py_wreadlink(
    const wchar_t *path,
    wchar_t *buf,
    size_t bufsiz);
#endif

#ifdef HAVE_REALPATH
wchar_t* _Py_wrealpath(
    const wchar_t *path,
    wchar_t *resolved_path,
    size_t resolved_path_size);
#endif

wchar_t* _Py_wgetcwd(
    wchar_t *buf,
    size_t size);

int _Py_get_inheritable(int fd);

int _Py_set_inheritable(int fd, int inheritable,
                                    int *atomic_flag_works);

int _Py_set_inheritable_async_safe(int fd, int inheritable,
                                               int *atomic_flag_works);

int _Py_dup(int fd);

#ifndef MS_WINDOWS
int _Py_get_blocking(int fd);

int _Py_set_blocking(int fd, int blocking);
#endif   /* !MS_WINDOWS */

int _Py_GetLocaleconvNumeric(
    PyObject **decimal_point,
    PyObject **thousands_sep,
    const char **grouping);

#endif   /* Py_LIMITED_API */

COSMOPOLITAN_C_END_
#endif /* !Py_FILEUTILS_H */
