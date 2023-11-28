/*
  Copyright (c) 1990-2009 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2009-Jan-02 or later
  (the contents of which are also included in unzip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*---------------------------------------------------------------------------

  api.c

  This module supplies an UnZip engine for use directly from C/C++
  programs.  The functions are:

    ZCONST UzpVer *UzpVersion(void);
    unsigned UzpVersion2(UzpVer2 *version)
    int UzpMain(int argc, char *argv[]);
    int UzpAltMain(int argc, char *argv[], UzpInit *init);
    int UzpValidate(char *archive, int AllCodes);
    void UzpFreeMemBuffer(UzpBuffer *retstr);
    int UzpUnzipToMemory(char *zip, char *file, UzpOpts *optflgs,
                         UzpCB *UsrFuncts, UzpBuffer *retstr);

  non-WINDLL only (a special WINDLL variant is defined in windll/windll.c):
    int UzpGrep(char *archive, char *file, char *pattern, int cmd, int SkipBin,
                UzpCB *UsrFuncts);

  OS/2 only (for now):
    int UzpFileTree(char *name, cbList(callBack), char *cpInclude[],
          char *cpExclude[]);

  You must define `DLL' in order to include the API extensions.

  ---------------------------------------------------------------------------*/


#ifdef OS2
#  define  INCL_DOSMEMMGR
#endif

#define UNZIP_INTERNAL
#include "third_party/unzip/unzip.h"
#include "third_party/unzip/unzvers.h"

#ifdef DLL      /* This source file supplies DLL-only interface code. */

#ifndef POCKET_UNZIP    /* WinCE pUnZip defines this elsewhere. */
jmp_buf dll_error_return;
#endif

/*---------------------------------------------------------------------------
    Documented API entry points
  ---------------------------------------------------------------------------*/


ZCONST UzpVer * UZ_EXP UzpVersion()     /* returns pointer to const struct */
{
    static ZCONST UzpVer version = {    /* doesn't change between calls */
        /* structure size */
        UZPVER_LEN,
        /* version flags */
#ifdef BETA
# ifdef ZLIB_VERSION
        3,
# else
        1,
# endif
#else
# ifdef ZLIB_VERSION
        2,
# else
        0,
# endif
#endif
        /* betalevel and date strings */
        UZ_BETALEVEL, UZ_VERSION_DATE,
        /* zlib_version string */
#ifdef ZLIB_VERSION
        ZLIB_VERSION,
#else
        NULL,
#endif
        /*== someday each of these may have a separate patchlevel: ==*/
        /* unzip version */
        {UZ_MAJORVER, UZ_MINORVER, UZ_PATCHLEVEL, 0},
        /* zipinfo version */
        {ZI_MAJORVER, ZI_MINORVER, UZ_PATCHLEVEL, 0},
        /* os2dll version (retained for backward compatibility) */
        {UZ_MAJORVER, UZ_MINORVER, UZ_PATCHLEVEL, 0},
        /* windll version (retained for backward compatibility)*/
        {UZ_MAJORVER, UZ_MINORVER, UZ_PATCHLEVEL, 0},
#ifdef OS2DLL
        /* os2dll API minimum compatible version*/
        {UZ_OS2API_COMP_MAJOR, UZ_OS2API_COMP_MINOR, UZ_OS2API_COMP_REVIS, 0}
#else /* !OS2DLL */
#ifdef WINDLL
        /* windll API minimum compatible version*/
        {UZ_WINAPI_COMP_MAJOR, UZ_WINAPI_COMP_MINOR, UZ_WINAPI_COMP_REVIS, 0}
#else /* !WINDLL */
        /* generic DLL API minimum compatible version*/
        {UZ_GENAPI_COMP_MAJOR, UZ_GENAPI_COMP_MINOR, UZ_GENAPI_COMP_REVIS, 0}
#endif /* ?WINDLL */
#endif /* ?OS2DLL */
    };

    return &version;
}

unsigned UZ_EXP UzpVersion2(UzpVer2 *version)
{

    if (version->structlen != sizeof(UzpVer2))
        return sizeof(UzpVer2);

#ifdef BETA
    version->flag = 1;
#else
    version->flag = 0;
#endif
    strcpy(version->betalevel, UZ_BETALEVEL);
    strcpy(version->date, UZ_VERSION_DATE);

#ifdef ZLIB_VERSION
    /* Although ZLIB_VERSION is a compile-time constant, we implement an
       "overrun-safe" copy because its actual value is not under our control.
     */
    strncpy(version->zlib_version, ZLIB_VERSION,
            sizeof(version->zlib_version) - 1);
    version->zlib_version[sizeof(version->zlib_version) - 1] = '\0';
    version->flag |= 2;
#else
    version->zlib_version[0] = '\0';
#endif

    /* someday each of these may have a separate patchlevel: */
    version->unzip.major = UZ_MAJORVER;
    version->unzip.minor = UZ_MINORVER;
    version->unzip.patchlevel = UZ_PATCHLEVEL;

    version->zipinfo.major = ZI_MAJORVER;
    version->zipinfo.minor = ZI_MINORVER;
    version->zipinfo.patchlevel = UZ_PATCHLEVEL;

    /* these are retained for backward compatibility only: */
    version->os2dll.major = UZ_MAJORVER;
    version->os2dll.minor = UZ_MINORVER;
    version->os2dll.patchlevel = UZ_PATCHLEVEL;

    version->windll.major = UZ_MAJORVER;
    version->windll.minor = UZ_MINORVER;
    version->windll.patchlevel = UZ_PATCHLEVEL;

#ifdef OS2DLL
    /* os2dll API minimum compatible version*/
    version->dllapimin.major = UZ_OS2API_COMP_MAJOR;
    version->dllapimin.minor = UZ_OS2API_COMP_MINOR;
    version->dllapimin.patchlevel = UZ_OS2API_COMP_REVIS;
#else /* !OS2DLL */
#ifdef WINDLL
    /* windll API minimum compatible version*/
    version->dllapimin.major = UZ_WINAPI_COMP_MAJOR;
    version->dllapimin.minor = UZ_WINAPI_COMP_MINOR;
    version->dllapimin.patchlevel = UZ_WINAPI_COMP_REVIS;
#else /* !WINDLL */
    /* generic DLL API minimum compatible version*/
    version->dllapimin.major = UZ_GENAPI_COMP_MAJOR;
    version->dllapimin.minor = UZ_GENAPI_COMP_MINOR;
    version->dllapimin.patchlevel = UZ_GENAPI_COMP_REVIS;
#endif /* ?WINDLL */
#endif /* ?OS2DLL */
    return 0;
}





#ifndef SFX
#ifndef WINDLL

int UZ_EXP UzpAltMain(int argc, char *argv[], UzpInit *init)
{
    int r, (*dummyfn)();


    CONSTRUCTGLOBALS();

    if (init->structlen >= (sizeof(ulg) + sizeof(dummyfn)) && init->msgfn)
        G.message = init->msgfn;

    if (init->structlen >= (sizeof(ulg) + 2*sizeof(dummyfn)) && init->inputfn)
        G.input = init->inputfn;

    if (init->structlen >= (sizeof(ulg) + 3*sizeof(dummyfn)) && init->pausefn)
        G.mpause = init->pausefn;

    if (init->structlen >= (sizeof(ulg) + 4*sizeof(dummyfn)) && init->userfn)
        (*init->userfn)();    /* allow void* arg? */

    r = unzip(__G__ argc, argv);
    DESTROYGLOBALS();
    RETURN(r);
}

#endif /* !WINDLL */




#ifndef __16BIT__

void UZ_EXP UzpFreeMemBuffer(UzpBuffer *retstr)
{
    if (retstr != NULL && retstr->strptr != NULL) {
        free(retstr->strptr);
        retstr->strptr = NULL;
        retstr->strlength = 0;
    }
}




#ifndef WINDLL

static int UzpDLL_Init OF((zvoid *pG, UzpCB *UsrFuncts));

static int UzpDLL_Init(pG, UsrFuncts)
zvoid *pG;
UzpCB *UsrFuncts;
{
    int (*dummyfn)();

    if (UsrFuncts->structlen >= (sizeof(ulg) + sizeof(dummyfn)) &&
        UsrFuncts->msgfn)
        ((Uz_Globs *)pG)->message = UsrFuncts->msgfn;
    else
        return FALSE;

    if (UsrFuncts->structlen >= (sizeof(ulg) + 2*sizeof(dummyfn)) &&
        UsrFuncts->inputfn)
        ((Uz_Globs *)pG)->input = UsrFuncts->inputfn;

    if (UsrFuncts->structlen >= (sizeof(ulg) + 3*sizeof(dummyfn)) &&
        UsrFuncts->pausefn)
        ((Uz_Globs *)pG)->mpause = UsrFuncts->pausefn;

    if (UsrFuncts->structlen >= (sizeof(ulg) + 4*sizeof(dummyfn)) &&
        UsrFuncts->passwdfn)
        ((Uz_Globs *)pG)->decr_passwd = UsrFuncts->passwdfn;

    if (UsrFuncts->structlen >= (sizeof(ulg) + 5*sizeof(dummyfn)) &&
        UsrFuncts->statrepfn)
        ((Uz_Globs *)pG)->statreportcb = UsrFuncts->statrepfn;

    return TRUE;
}


int UZ_EXP UzpUnzipToMemory(char *zip, char *file, UzpOpts *optflgs,
    UzpCB *UsrFuncts, UzpBuffer *retstr)
{
    int r;
#if (defined(WINDLL) && !defined(CRTL_CP_IS_ISO))
    char *intern_zip, *intern_file;
#endif

    CONSTRUCTGLOBALS();
#if (defined(WINDLL) && !defined(CRTL_CP_IS_ISO))
    intern_zip = (char *)malloc(strlen(zip)+1);
    if (intern_zip == NULL) {
       DESTROYGLOBALS();
       return PK_MEM;
    }
    intern_file = (char *)malloc(strlen(file)+1);
    if (intern_file == NULL) {
       DESTROYGLOBALS();
       free(intern_zip);
       return PK_MEM;
    }
    ISO_TO_INTERN(zip, intern_zip);
    ISO_TO_INTERN(file, intern_file);
#   define zip intern_zip
#   define file intern_file
#endif
    /* Copy those options that are meaningful for UzpUnzipToMemory, instead of
     * a simple "memcpy(G.UzO, optflgs, sizeof(UzpOpts));"
     */
    uO.pwdarg = optflgs->pwdarg;
    uO.aflag = optflgs->aflag;
    uO.C_flag = optflgs->C_flag;
    uO.qflag = optflgs->qflag;  /* currently,  overridden in unzipToMemory */

    if (!UzpDLL_Init((zvoid *)&G, UsrFuncts)) {
       DESTROYGLOBALS();
       return PK_BADERR;
    }
    G.redirect_data = 1;

    r = (unzipToMemory(__G__ zip, file, retstr) <= PK_WARN);

    DESTROYGLOBALS();
#if (defined(WINDLL) && !defined(CRTL_CP_IS_ISO))
#  undef file
#  undef zip
    free(intern_file);
    free(intern_zip);
#endif
    if (!r && retstr->strlength) {
       free(retstr->strptr);
       retstr->strptr = NULL;
    }
    return r;
}
#endif /* !WINDLL */
#endif /* !__16BIT__ */





#ifdef OS2DLL

int UZ_EXP UzpFileTree(char *name, cbList(callBack), char *cpInclude[],
                char *cpExclude[])
{
    int r;

    CONSTRUCTGLOBALS();
    uO.qflag = 2;
    uO.vflag = 1;
    uO.C_flag = 1;
    G.wildzipfn = name;
    G.process_all_files = TRUE;
    if (cpInclude) {
        char **ptr = cpInclude;

        while (*ptr != NULL) ptr++;
        G.filespecs = ptr - cpInclude;
        G.pfnames = cpInclude, G.process_all_files = FALSE;
    }
    if (cpExclude) {
        char **ptr = cpExclude;

        while (*ptr != NULL) ptr++;
        G.xfilespecs = ptr - cpExclude;
        G.pxnames = cpExclude, G.process_all_files = FALSE;
    }

    G.processExternally = callBack;
    r = process_zipfiles(__G)==0;
    DESTROYGLOBALS();
    return r;
}

#endif /* OS2DLL */
#endif /* !SFX */




/*---------------------------------------------------------------------------
    Helper functions
  ---------------------------------------------------------------------------*/


void setFileNotFound(__G)
    __GDEF
{
    G.filenotfound++;
}


#ifndef SFX

int unzipToMemory(__GPRO__ char *zip, char *file, UzpBuffer *retstr)
{
    int r;
    char *incname[2];

    if ((zip == NULL) || (strlen(zip) > ((WSIZE>>2) - 160)))
        return PK_PARAM;
    if ((file == NULL) || (strlen(file) > ((WSIZE>>2) - 160)))
        return PK_PARAM;

    G.process_all_files = FALSE;
    G.extract_flag = TRUE;
    uO.qflag = 2;
    G.wildzipfn = zip;

    G.pfnames = incname;
    incname[0] = file;
    incname[1] = NULL;
    G.filespecs = 1;

    r = process_zipfiles(__G);
    if (retstr) {
        retstr->strptr = (char *)G.redirect_buffer;
        retstr->strlength = G.redirect_size;
    }
    return r;                   /* returns `PK_???' error values */
}

#endif /* !SFX */

/*
    With the advent of 64 bit support, for now I am assuming that
    if the size of the file is greater than an unsigned long, there
    will simply not be enough memory to handle it, and am returning
    FALSE.
*/
int redirect_outfile(__G)
     __GDEF
{
#ifdef ZIP64_SUPPORT
    __int64 check_conversion;
#endif

    if (G.redirect_size != 0 || G.redirect_buffer != NULL)
        return FALSE;

#ifndef NO_SLIDE_REDIR
    G.redirect_slide = !G.pInfo->textmode;
#endif
#if (lenEOL != 1)
    if (G.pInfo->textmode) {
        G.redirect_size = (ulg)(G.lrec.ucsize * lenEOL);
        if (G.redirect_size < G.lrec.ucsize)
            G.redirect_size = (ulg)((G.lrec.ucsize > (ulg)-2L) ?
                                    G.lrec.ucsize : -2L);
#ifdef ZIP64_SUPPORT
        check_conversion = G.lrec.ucsize * lenEOL;
#endif
    } else
#endif
    {
        G.redirect_size = (ulg)G.lrec.ucsize;
#ifdef ZIP64_SUPPORT
        check_conversion = (__int64)G.lrec.ucsize;
#endif
    }

#ifdef ZIP64_SUPPORT
    if ((__int64)G.redirect_size != check_conversion)
        return FALSE;
#endif

#ifdef __16BIT__
    if ((ulg)((extent)G.redirect_size) != G.redirect_size)
        return FALSE;
#endif
#ifdef OS2
    DosAllocMem((void **)&G.redirect_buffer, G.redirect_size+1,
      PAG_READ|PAG_WRITE|PAG_COMMIT);
    G.redirect_pointer = G.redirect_buffer;
#else
    G.redirect_pointer =
      G.redirect_buffer = malloc((extent)(G.redirect_size+1));
#endif
    if (!G.redirect_buffer)
        return FALSE;
    G.redirect_pointer[G.redirect_size] = '\0';
    return TRUE;
}



int writeToMemory(__GPRO__ ZCONST uch *rawbuf, extent size)
{
    int errflg = FALSE;

    if ((uch *)rawbuf != G.redirect_pointer) {
        extent redir_avail = (G.redirect_buffer + G.redirect_size) -
                             G.redirect_pointer;

        /* Check for output buffer overflow */
        if (size > redir_avail) {
           /* limit transfer data to available space, set error return flag */
           size = redir_avail;
           errflg = TRUE;
        }
        memcpy(G.redirect_pointer, rawbuf, size);
    }
    G.redirect_pointer += size;
    return errflg;
}




int close_redirect(__G)
     __GDEF
{
    if (G.pInfo->textmode) {
        *G.redirect_pointer = '\0';
        G.redirect_size = (ulg)(G.redirect_pointer - G.redirect_buffer);
        if ((G.redirect_buffer =
             realloc(G.redirect_buffer, G.redirect_size + 1)) == NULL) {
            G.redirect_size = 0;
            return EOF;
        }
    }
    return 0;
}




#ifndef SFX
#ifndef __16BIT__
#ifndef WINDLL

/* Purpose: Determine if file in archive contains the string szSearch

   Parameters: archive  = archive name
               file     = file contained in the archive. This cannot be
                          a wildcard to be meaningful
               pattern  = string to search for
               cmd      = 0 - case-insensitive search
                          1 - case-sensitve search
                          2 - case-insensitive, whole words only
                          3 - case-sensitive, whole words only
               SkipBin  = if true, skip any files that have control
                          characters other than CR, LF, or tab in the first
                          100 characters.

   Returns:    TRUE if a match is found
               FALSE if no match is found
               -1 on error

   Comments: This does not pretend to be as useful as the standard
             Unix grep, which returns the strings associated with a
             particular pattern, nor does it search past the first
             matching occurrence of the pattern.
 */

int UZ_EXP UzpGrep(char *archive, char *file, char *pattern, int cmd,
                   int SkipBin, UzpCB *UsrFuncts)
{
    int retcode = FALSE, compare;
    ulg i, j, patternLen, buflen;
    char * sz, *p;
    UzpOpts flgopts;
    UzpBuffer retstr;

    memzero(&flgopts, sizeof(UzpOpts));         /* no special options */

    if (!UzpUnzipToMemory(archive, file, &flgopts, UsrFuncts, &retstr)) {
       return -1;   /* not enough memory, file not found, or other error */
    }

    if (SkipBin) {
        if (retstr.strlength < 100)
            buflen = retstr.strlength;
        else
            buflen = 100;
        for (i = 0; i < buflen; i++) {
            if (iscntrl(retstr.strptr[i])) {
                if ((retstr.strptr[i] != 0x0A) &&
                    (retstr.strptr[i] != 0x0D) &&
                    (retstr.strptr[i] != 0x09))
                {
                    /* OK, we now think we have a binary file of some sort */
                    free(retstr.strptr);
                    return FALSE;
                }
            }
        }
    }

    patternLen = strlen(pattern);

    if (retstr.strlength < patternLen) {
        free(retstr.strptr);
        return FALSE;
    }

    sz = malloc(patternLen + 3); /* add two in case doing whole words only */
    if (cmd > 1) {
        strcpy(sz, " ");
        strcat(sz, pattern);
        strcat(sz, " ");
    } else
        strcpy(sz, pattern);

    if ((cmd == 0) || (cmd == 2)) {
        for (i = 0; i < strlen(sz); i++)
            sz[i] = toupper(sz[i]);
        for (i = 0; i < retstr.strlength; i++)
            retstr.strptr[i] = toupper(retstr.strptr[i]);
    }

    for (i = 0; i < (retstr.strlength - patternLen); i++) {
        p = &retstr.strptr[i];
        compare = TRUE;
        for (j = 0; j < patternLen; j++) {
            /* We cannot do strncmp here, as we may be dealing with a
             * "binary" file, such as a word processing file, or perhaps
             * even a true executable of some sort. */
            if (p[j] != sz[j]) {
                compare = FALSE;
                break;
            }
        }
        if (compare == TRUE) {
            retcode = TRUE;
            break;
        }
    }

    free(sz);
    free(retstr.strptr);

    return retcode;
}
#endif /* !WINDLL */
#endif /* !__16BIT__ */



int UZ_EXP UzpValidate(char *archive, int AllCodes)
{
    int retcode;
    CONSTRUCTGLOBALS();

    uO.jflag = 1;
    uO.tflag = 1;
    uO.overwrite_none = 0;
    G.extract_flag = (!uO.zipinfo_mode &&
                      !uO.cflag && !uO.tflag && !uO.vflag && !uO.zflag
#ifdef TIMESTAMP
                      && !uO.T_flag
#endif
                     );

    uO.qflag = 2;                        /* turn off all messages */
    G.fValidate = TRUE;
    G.pfnames = (char **)&fnames[0];    /* assign default filename vector */

    if (archive == NULL) {      /* something is screwed up:  no filename */
        DESTROYGLOBALS();
        retcode = PK_NOZIP;
        goto exit_retcode;
    }

    if (strlen(archive) >= FILNAMSIZ) {
       /* length of supplied archive name exceed the system's filename limit */
       DESTROYGLOBALS();
       retcode = PK_PARAM;
       goto exit_retcode;
    }

    G.wildzipfn = (char *)malloc(FILNAMSIZ);
    strcpy(G.wildzipfn, archive);
#if (defined(WINDLL) && !defined(CRTL_CP_IS_ISO))
    _ISO_INTERN(G.wildzipfn);
#endif

#ifdef WINDLL
    Wiz_NoPrinting(TRUE);
#endif

    G.process_all_files = TRUE;         /* for speed */

    if (setjmp(dll_error_return) != 0) {
#ifdef WINDLL
        Wiz_NoPrinting(FALSE);
#endif
        free(G.wildzipfn);
        DESTROYGLOBALS();
        retcode = PK_BADERR;
        goto exit_retcode;
    }

    retcode = process_zipfiles(__G);

    free(G.wildzipfn);
#ifdef WINDLL
    Wiz_NoPrinting(FALSE);
#endif
    DESTROYGLOBALS();

    /* PK_WARN == 1 and PK_FIND == 11. When we are just looking at an
       archive, we should still be able to see the files inside it,
       even if we can't decode them for some reason.

       We also still want to be able to get at files even if there is
       something odd about the zip archive, hence allow PK_WARN,
       PK_FIND, IZ_UNSUP as well as PK_ERR
     */

exit_retcode:
    if (AllCodes)
        return retcode;

    if ((retcode == PK_OK) || (retcode == PK_WARN) || (retcode == PK_ERR) ||
        (retcode == IZ_UNSUP) || (retcode == PK_FIND))
        return TRUE;
    else
        return FALSE;
}

#endif /* !SFX */
#endif /* DLL */
