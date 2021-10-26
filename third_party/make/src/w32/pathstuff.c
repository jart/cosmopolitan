/* Path conversion for Windows pathnames.
Copyright (C) 1996-2020 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "makeint.h"
#include <string.h>
#include <stdlib.h>
#include "pathstuff.h"

/*
 * Convert delimiter separated vpath to Canonical format.
 */
char *
convert_vpath_to_windows32(char *Path, char to_delim)
{
    char *etok;            /* token separator for old Path */

        /*
         * Convert all spaces to delimiters. Note that pathnames which
         * contain blanks get trounced here. Use 8.3 format as a workaround.
         */
        for (etok = Path; etok && *etok; etok++)
                if (ISBLANK ((unsigned char) *etok))
                        *etok = to_delim;

        return (convert_Path_to_windows32(Path, to_delim));
}

/*
 * Convert delimiter separated path to Canonical format.
 */
char *
convert_Path_to_windows32(char *Path, char to_delim)
{
    char *etok;            /* token separator for old Path */
    char *p;            /* points to element of old Path */

    /* is this a multi-element Path ? */
    /* FIXME: Perhaps use ":;\"" in strpbrk to convert all quotes to
       delimiters as well, as a way to handle quoted directories in
       PATH?  */
    for (p = Path, etok = strpbrk(p, ":;");
         etok;
         etok = strpbrk(p, ":;"))
        if ((etok - p) == 1) {
            if (*(etok - 1) == ';' ||
                *(etok - 1) == ':') {
                etok[-1] = to_delim;
                etok[0] = to_delim;
                p = ++etok;
                continue;    /* ignore empty bucket */
            } else if (!isalpha ((unsigned char) *p)) {
                /* found one to count, handle things like '.' */
                *etok = to_delim;
                p = ++etok;
            } else if ((*etok == ':') && ((etok = strpbrk(etok+1, ":;")) != NULL)) {
                /* found one to count, handle drive letter */
                *etok = to_delim;
                p = ++etok;
            } else
                /* all finished, force abort */
                p += strlen(p);
        } else if (*p == '"') { /* a quoted directory */
            for (p++; *p && *p != '"'; p++) /* skip quoted part */
                ;
            etok = strpbrk(p, ":;");        /* find next delimiter */
            if (etok) {
                *etok = to_delim;
                p = ++etok;
            } else
                p += strlen(p);
        } else {
            /* found another one, no drive letter */
            *etok = to_delim;
            p = ++etok;
        }

    return Path;
}

/*
 * Convert to forward slashes. Resolve to full pathname optionally
 */
char *
w32ify(const char *filename, int resolve)
{
    static char w32_path[FILENAME_MAX];
    char *p;

    if (resolve)
        _fullpath(w32_path, filename, sizeof (w32_path));
    else
        strncpy(w32_path, filename, sizeof (w32_path));

    for (p = w32_path; p && *p; p++)
        if (*p == '\\')
            *p = '/';

    return w32_path;
}

char *
getcwd_fs(char* buf, int len)
{
        char *p = getcwd(buf, len);

        if (p) {
                char *q = w32ify(buf, 0);
                strncpy(buf, q, len);
        }

        return p;
}

#ifdef unused
/*
 * Convert delimiter separated pathnames (e.g. PATH) or single file pathname
 * (e.g. c:/foo, c:\bar) to NutC format. If we are handed a string that
 * _NutPathToNutc() fails to convert, just return the path we were handed
 * and assume the caller will know what to do with it (It was probably
 * a mistake to try and convert it anyway due to some of the bizarre things
 * that might look like pathnames in makefiles).
 */
char *
convert_path_to_nutc(char *path)
{
    int  count;            /* count of path elements */
    char *nutc_path;     /* new NutC path */
    int  nutc_path_len;    /* length of buffer to allocate for new path */
    char *pathp;        /* pointer to nutc_path used to build it */
    char *etok;            /* token separator for old path */
    char *p;            /* points to element of old path */
    char sep;            /* what flavor of separator used in old path */
    char *rval;

    /* is this a multi-element path ? */
    for (p = path, etok = strpbrk(p, ":;"), count = 0;
         etok;
         etok = strpbrk(p, ":;"))
        if ((etok - p) == 1) {
            if (*(etok - 1) == ';' ||
                *(etok - 1) == ':') {
                p = ++etok;
                continue;    /* ignore empty bucket */
            } else if (etok = strpbrk(etok+1, ":;"))
                /* found one to count, handle drive letter */
                p = ++etok, count++;
            else
                /* all finished, force abort */
                p += strlen(p);
        } else
            /* found another one, no drive letter */
            p = ++etok, count++;

    if (count) {
        count++;    /* x1;x2;x3 <- need to count x3 */

        /*
         * Hazard a guess on how big the buffer needs to be.
         * We have to convert things like c:/foo to /c=/foo.
         */
        nutc_path_len = strlen(path) + (count*2) + 1;
        nutc_path = xmalloc(nutc_path_len);
        pathp = nutc_path;
        *pathp = '\0';

        /*
         * Loop through PATH and convert one elemnt of the path at at
         * a time. Single file pathnames will fail this and fall
         * to the logic below loop.
         */
        for (p = path, etok = strpbrk(p, ":;");
             etok;
             etok = strpbrk(p, ":;")) {

            /* don't trip up on device specifiers or empty path slots */
            if ((etok - p) == 1)
                if (*(etok - 1) == ';' ||
                    *(etok - 1) == ':') {
                    p = ++etok;
                    continue;
                } else if ((etok = strpbrk(etok+1, ":;")) == NULL)
                    break;    /* thing found was a WINDOWS32 pathname */

            /* save separator */
            sep = *etok;

            /* terminate the current path element -- temporarily */
            *etok = '\0';

#ifdef __NUTC__
            /* convert to NutC format */
            if (_NutPathToNutc(p, pathp, 0) == FALSE) {
                free(nutc_path);
                rval = savestring(path, strlen(path));
                return rval;
            }
#else
            *pathp++ = '/';
            *pathp++ = p[0];
            *pathp++ = '=';
            *pathp++ = '/';
            strcpy(pathp, &p[2]);
#endif

            pathp += strlen(pathp);
            *pathp++ = ':';     /* use Unix style path separtor for new path */
            *pathp   = '\0'; /* make sure we are null terminaed */

            /* restore path separator */
            *etok = sep;

            /* point p to first char of next path element */
            p = ++etok;

        }
    } else {
        nutc_path_len = strlen(path) + 3;
        nutc_path = xmalloc(nutc_path_len);
        pathp = nutc_path;
        *pathp = '\0';
        p = path;
    }

    /*
      * OK, here we handle the last element in PATH (e.g. c of a;b;c)
     * or the path was a single filename and will be converted
     * here. Note, testing p here assures that we don't trip up
     * on paths like a;b; which have trailing delimiter followed by
     * nothing.
     */
    if (*p != '\0') {
#ifdef __NUTC__
        if (_NutPathToNutc(p, pathp, 0) == FALSE) {
            free(nutc_path);
            rval = savestring(path, strlen(path));
            return rval;
        }
#else
        *pathp++ = '/';
        *pathp++ = p[0];
        *pathp++ = '=';
        *pathp++ = '/';
        strcpy(pathp, &p[2]);
#endif
    } else
        *(pathp-1) = '\0'; /* we're already done, don't leave trailing : */

    rval = savestring(nutc_path, strlen(nutc_path));
    free(nutc_path);
    return rval;
}

#endif
