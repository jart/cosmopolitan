/*
  Copyright (c) 1990-2005 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in unzip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*----------------------------------------------------------------*
 | envargs - add default options from environment to command line
 |----------------------------------------------------------------
 | Author: Bill Davidsen, original 10/13/91, revised 23 Oct 1991.
 | This program is in the public domain.
 |----------------------------------------------------------------
 | Minor program notes:
 |  1. Yes, the indirection is a tad complex
 |  2. Parentheses were added where not needed in some cases
 |     to make the action of the code less obscure.
 |----------------------------------------------------------------
 | UnZip notes: 24 May 92 ("v1.4"):
#include "libc/runtime/runtime.h"
#include "libc/runtime/runtime.h"
 |  1. #include "third_party/unzip/unzip.h" for prototypes (24 May 92)
 |  2. changed ch to type char (24 May 92)
 |  3. added an ifdef to avoid Borland warnings (24 May 92)
 |  4. included Rich Wales' mksargs() routine (for MS-DOS, maybe
 |     OS/2? NT?) (4 Dec 93)
 |  5. added alternate-variable string envstr2 (21 Apr 94)
 |  6. added support for quoted arguments (6 Jul 96)
 *----------------------------------------------------------------*/


#define __ENVARGS_C     /* identifies this source module */
#define UNZIP_INTERNAL
#include "third_party/unzip/unzip.h"

#ifdef __EMX__          /* emx isspace() returns TRUE on extended ASCII !! */
#  define ISspace(c) ((c) & 0x80 ? 0 : isspace((unsigned)c))
#else
#  define ISspace(c) isspace((unsigned)c)
#endif /* ?__EMX__ */

#if (!defined(RISCOS) && (!defined(MODERN) || defined(NO_STDLIB_H)))
extern char *getenv();
#endif
static int count_args OF((ZCONST char *));


/* envargs() returns PK-style error code */

int envargs(Pargc, Pargv, envstr, envstr2)
    int *Pargc;
    char ***Pargv;
    ZCONST char *envstr, *envstr2;
{
    char *envptr;       /* value returned by getenv */
    char *bufptr;       /* copy of env info */
    int argc = 0;       /* internal arg count */
    register int ch;    /* spare temp value */
    char **argv;        /* internal arg vector */
    char **argvect;     /* copy of vector address */

    /* see if anything in the environment */
    if ((envptr = getenv(envstr)) != (char *)NULL)        /* usual var */
        while (ISspace(*envptr))        /* must discard leading spaces */
            envptr++;
    if (envptr == (char *)NULL || *envptr == '\0')
        if ((envptr = getenv(envstr2)) != (char *)NULL)   /* alternate var */
            while (ISspace(*envptr))
                envptr++;
    if (envptr == (char *)NULL || *envptr == '\0')
        return PK_OK;

    bufptr = malloc(1 + strlen(envptr));
    if (bufptr == (char *)NULL)
        return PK_MEM;
#if ((defined(WIN32) || defined(WINDLL)) && !defined(_WIN32_WCE))
# ifdef WIN32
    if (IsWinNT()) {
        /* SPC: don't know codepage of 'real' WinNT console */
        strcpy(bufptr, envptr);
    } else {
        /* Win95 environment is DOS and uses OEM character coding */
        OEM_TO_INTERN(envptr, bufptr);
    }
# else /* !WIN32 */
    /* DOS (Win 3.x) environment uses OEM codepage */
    OEM_TO_INTERN(envptr, bufptr);
# endif
#else /* !((WIN32 || WINDLL) && !_WIN32_WCE) */
    strcpy(bufptr, envptr);
#endif /* ?((WIN32 || WINDLL) && !_WIN32_WCE) */

    /* count the args so we can allocate room for them */
    argc = count_args(bufptr);
    /* allocate a vector large enough for all args */
    argv = (char **)malloc((argc + *Pargc + 1) * sizeof(char *));
    if (argv == (char **)NULL) {
        free(bufptr);
        return PK_MEM;
    }
    argvect = argv;

    /* copy the program name first, that's always true */
    *(argv++) = *((*Pargv)++);

    /* copy the environment args next, may be changed */
    do {
#if defined(AMIGA) || defined(UNIX)
        if (*bufptr == '"') {
            char *argstart = ++bufptr;

            *(argv++) = argstart;
            for (ch = *bufptr; ch != '\0' && ch != '\"';
                 ch = *PREINCSTR(bufptr))
                if (ch == '\\' && bufptr[1] != '\0')
                    ++bufptr;           /* advance to char after backslash */
            if (ch != '\0')
                *(bufptr++) = '\0';     /* overwrite trailing " */

            /* remove escape characters */
            while ((argstart = MBSCHR(argstart, '\\')) != (char *)NULL) {
                strcpy(argstart, argstart + 1);
                if (*argstart)
                    ++argstart;
            }
        } else {
            *(argv++) = bufptr;
            while ((ch = *bufptr) != '\0' && !ISspace(ch))
                INCSTR(bufptr);
            if (ch != '\0')
                *(bufptr++) = '\0';
        }
#else
#ifdef DOS_FLX_NLM_OS2_W32
        /* we do not support backslash-quoting of quotes in quoted
         * strings under DOS_FLX_NLM_OS2_W32, because backslashes are
         * directory separators and double quotes are illegal in filenames */
        if (*bufptr == '"') {
            *(argv++) = ++bufptr;
            while ((ch = *bufptr) != '\0' && ch != '\"')
                INCSTR(bufptr);
            if (ch != '\0')
                *(bufptr++) = '\0';
        } else {
            *(argv++) = bufptr;
            while ((ch = *bufptr) != '\0' && !ISspace(ch))
                INCSTR(bufptr);
            if (ch != '\0')
                *(bufptr++) = '\0';
        }
#else
        *(argv++) = bufptr;
        while ((ch = *bufptr) != '\0' && !ISspace(ch))
            INCSTR(bufptr);
        if (ch != '\0')
            *(bufptr++) = '\0';
#endif /* ?DOS_FLX_NLM_OS2_W32 */
#endif /* ?(AMIGA || UNIX) */
        while ((ch = *bufptr) != '\0' && ISspace(ch))
            INCSTR(bufptr);
    } while (ch);

    /* now save old argc and copy in the old args */
    argc += *Pargc;
    while (--(*Pargc))
        *(argv++) = *((*Pargv)++);

    /* finally, add a NULL after the last arg, like Unix */
    *argv = (char *)NULL;

    /* save the values and return, indicating succes */
    *Pargv = argvect;
    *Pargc = argc;

    return PK_OK;
}



static int count_args(s)
    ZCONST char *s;
{
    int count = 0;
    char ch;

    do {
        /* count and skip args */
        ++count;
#if defined(AMIGA) || defined(UNIX)
        if (*s == '\"') {
            for (ch = *PREINCSTR(s);  ch != '\0' && ch != '\"';
                 ch = *PREINCSTR(s))
                if (ch == '\\' && s[1] != '\0')
                    ++s;
            if (*s)
                ++s;        /* trailing quote */
        } else
#else
#ifdef DOS_FLX_NLM_OS2_W32
        if (*s == '\"') {
            ++s;                /* leading quote */
            while ((ch = *s) != '\0' && ch != '\"')
                INCSTR(s);
            if (*s)
                ++s;        /* trailing quote */
        } else
#endif /* DOS_FLX_NLM_OS2_W32 */
#endif /* ?(AMIGA || UNIX) */
        while ((ch = *s) != '\0' && !ISspace(ch))  /* note else-clauses above */
            INCSTR(s);
        while ((ch = *s) != '\0' && ISspace(ch))
            INCSTR(s);
    } while (ch);

    return count;
}



#ifdef TEST

int main(argc, argv)
    int argc;
    char **argv;
{
    int err;

    printf("Orig argv: %p\n", argv);
    dump_args(argc, argv);
    if ((err = envargs(&argc, &argv, "ENVTEST")) != PK_OK) {
        perror("envargs:  cannot get memory for arguments");
        EXIT(err);
    }
    printf(" New argv: %p\n", argv);
    dump_args(argc, argv);
}



void dump_args(argc, argv)
    int argc;
    char *argv[];
{
    int i;

    printf("\nDump %d args:\n", argc);
    for (i = 0; i < argc; ++i)
        printf("%3d %s\n", i, argv[i]);
}

#endif /* TEST */



#ifdef MSDOS   /* DOS_OS2?  DOS_OS2_W32? */

/*
 * void mksargs(int *argcp, char ***argvp)
 *
 *    Substitutes the extended command line argument list produced by
 *    the MKS Korn Shell in place of the command line info from DOS.
 *
 *    The MKS shell gets around DOS's 128-byte limit on the length of
 *    a command line by passing the "real" command line in the envi-
 *    ronment.  The "real" arguments are flagged by prepending a tilde
 *    (~) to each one.
 *
 *    This "mksargs" routine creates a new argument list by scanning
 *    the environment from the beginning, looking for strings begin-
 *    ning with a tilde character.  The new list replaces the original
 *    "argv" (pointed to by "argvp"), and the number of arguments
 *    in the new list replaces the original "argc" (pointed to by
 *    "argcp").
 *
 *    Rich Wales
 */
void mksargs(argcp, argvp)
    int *argcp;
    char ***argvp;
{
#ifndef MSC /* declared differently in MSC 7.0 headers, at least */
#ifndef __WATCOMC__
    extern char **environ;          /* environment */
#endif
#endif
    char        **envp;             /* pointer into environment */
    char        **newargv;          /* new argument list */
    char        **argp;             /* pointer into new arg list */
    int         newargc;            /* new argument count */

    /* sanity check */
    if (environ == NULL || argcp == NULL || argvp == NULL || *argvp == NULL)
        return;

    /* find out how many environment arguments there are */
    for (envp = environ, newargc = 0;
         *envp != NULL && (*envp)[0] == '~';
         envp++, newargc++)
        ;
    if (newargc == 0)
        return;     /* no environment arguments */

    /* set up new argument list */
    newargv = (char **) malloc(sizeof(char **) * (newargc+1));
    if (newargv == NULL)
        return;     /* malloc failed */

    for (argp = newargv, envp = environ; *envp != NULL && (*envp)[0] == '~';
         *argp++ = &(*envp++)[1])
        ;
    *argp = NULL;   /* null-terminate the list */

    /* substitute new argument list in place of old one */
    *argcp = newargc;
    *argvp = newargv;
}

#endif /* MSDOS */
