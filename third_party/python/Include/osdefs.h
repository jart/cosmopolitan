#ifndef Py_OSDEFS_H
#define Py_OSDEFS_H
COSMOPOLITAN_C_START_

#ifdef MS_WINDOWS
#define SEP        L'\\'
#define ALTSEP     L'/'
#define MAXPATHLEN 256
#define DELIM      L';'
#endif

/* Filename separator */
#ifndef SEP
#define SEP L'/'
#endif

#ifndef MAXPATHLEN
#if defined(PATH_MAX) && PATH_MAX > 1024
#define MAXPATHLEN PATH_MAX
#else
#define MAXPATHLEN 1024
#endif
#endif

/* Search path entry delimiter */
#ifndef DELIM
#define DELIM L':'
#endif

COSMOPOLITAN_C_END_
#endif /* !Py_OSDEFS_H */
