/*
  Copyright (c) 1990-2000 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/* Here we have a handmade stat() function because Aztec's c.lib stat() */
/* does not support an st_mode field, which we need... also a chmod().  */

/* This stat() is by Paul Wells, modified by Paul Kienitz. */
/* Originally for use with Aztec C >= 5.0 and Lattice C <= 4.01  */
/* Adapted for SAS/C 6.5x by Haidinger Walter */

/* POLICY DECISION: We will not attempt to remove global variables from */
/* this source file for Aztec C.  These routines are essentially just   */
/* augmentations of Aztec's c.lib, which is itself not reentrant.  If   */
/* we want to produce a fully reentrant UnZip, we will have to use a    */
/* suitable startup module, such as purify.a for Aztec by Paul Kienitz. */

#ifndef __amiga_stat_c
#define __amiga_stat_c

#include <exec/types.h>
#include <exec/memory.h>
#include "amiga/z-stat.h"             /* fake version of stat.h */
#include <string.h>

#ifdef AZTEC_C
#  include <libraries/dos.h>
#  include <libraries/dosextens.h>
#  include <clib/exec_protos.h>
#  include <clib/dos_protos.h>
#  include <pragmas/exec_lib.h>
#  include <pragmas/dos_lib.h>
#endif
#ifdef __SASC
#  include <sys/dir.h>               /* SAS/C dir function prototypes */
#  include <sys/types.h>
#  include <proto/exec.h>
#  include <proto/dos.h>
#endif

#ifndef SUCCESS
#  define SUCCESS (-1)
#  define FAILURE (0)
#endif


void close_leftover_open_dirs(void);    /* prototype */

static DIR *dir_cleanup_list = NULL;    /* for resource tracking */

/* CALL THIS WHEN HANDLING CTRL-C OR OTHER UNEXPECTED EXIT! */
void close_leftover_open_dirs(void)
{
    while (dir_cleanup_list)
        closedir(dir_cleanup_list);
}


unsigned short disk_not_mounted;

extern int stat(const char *file, struct stat *buf);

stat(file,buf)
const char *file;
struct stat *buf;
{

        struct FileInfoBlock *inf;
        BPTR lock;
        time_t ftime;
        struct tm local_tm;

        if( (lock = Lock((char *)file,SHARED_LOCK))==0 )
                /* file not found */
                return(-1);

        if( !(inf = (struct FileInfoBlock *)AllocMem(
                (long)sizeof(struct FileInfoBlock),MEMF_PUBLIC|MEMF_CLEAR)) )
        {
                UnLock(lock);
                return(-1);
        }

        if( Examine(lock,inf)==FAILURE )
        {
                FreeMem((char *)inf,(long)sizeof(*inf));
                UnLock(lock);
                return(-1);
        }

        /* fill in buf */
        buf->st_dev         =
        buf->st_nlink       =
        buf->st_uid         =
        buf->st_gid         =
        buf->st_rdev        = 0;
        buf->st_ino         = inf->fib_DiskKey;
        buf->st_blocks      = inf->fib_NumBlocks;
        buf->st_size        = inf->fib_Size;

        /* now the date.  AmigaDOS has weird datestamps---
         *      ds_Days is the number of days since 1-1-1978;
         *      however, as Unix wants date since 1-1-1970...
         */

        ftime =
                (inf->fib_Date.ds_Days * 86400 )                +
                (inf->fib_Date.ds_Minute * 60 )                 +
                (inf->fib_Date.ds_Tick / TICKS_PER_SECOND )     +
                (86400 * 8 * 365 )                              +
                (86400 * 2 );  /* two leap years */

        /* tzset(); */  /* this should be handled by mktime(), instead */
        /* ftime += timezone; */
        local_tm = *gmtime(&ftime);
        local_tm.tm_isdst = -1;
        ftime = mktime(&local_tm);

        buf->st_ctime =
        buf->st_atime =
        buf->st_mtime = ftime;

        buf->st_mode = (inf->fib_DirEntryType < 0 ? S_IFREG : S_IFDIR);

        /* lastly, throw in the protection bits */
        buf->st_mode |= ((inf->fib_Protection ^ 0xF) & 0xFF);

        FreeMem((char *)inf, (long)sizeof(*inf));
        UnLock((BPTR)lock);

        return(0);

}

int fstat(int handle, struct stat *buf)
{
    /* fake some reasonable values for stdin */
    buf->st_mode = (S_IREAD|S_IWRITE|S_IFREG);
    buf->st_size = -1;
    buf->st_mtime = time(&buf->st_mtime);
    return 0;
}


/* opendir(), readdir(), closedir(), rmdir(), and chmod() by Paul Kienitz. */

DIR *opendir(const char *path)
{
    DIR *dd = AllocMem(sizeof(DIR), MEMF_PUBLIC);
    if (!dd) return NULL;
    if (!(dd->d_parentlock = Lock((char *)path, MODE_OLDFILE))) {
        disk_not_mounted = IoErr() == ERROR_DEVICE_NOT_MOUNTED;
        FreeMem(dd, sizeof(DIR));
        return NULL;
    } else
        disk_not_mounted = 0;
    if (!Examine(dd->d_parentlock, &dd->d_fib) || dd->d_fib.fib_EntryType < 0) {
        UnLock(dd->d_parentlock);
        FreeMem(dd, sizeof(DIR));
        return NULL;
    }
    dd->d_cleanuplink = dir_cleanup_list;       /* track them resources */
    if (dir_cleanup_list)
        dir_cleanup_list->d_cleanupparent = &dd->d_cleanuplink;
    dd->d_cleanupparent = &dir_cleanup_list;
    dir_cleanup_list = dd;
    return dd;
}

void closedir(DIR *dd)
{
    if (dd) {
        if (dd->d_cleanuplink)
            dd->d_cleanuplink->d_cleanupparent = dd->d_cleanupparent;
        *(dd->d_cleanupparent) = dd->d_cleanuplink;
        if (dd->d_parentlock)
            UnLock(dd->d_parentlock);
        FreeMem(dd, sizeof(DIR));
    }
}

struct dirent *readdir(DIR *dd)
{
    return (ExNext(dd->d_parentlock, &dd->d_fib) ? (struct dirent *)dd : NULL);
}


#ifdef AZTEC_C

int rmdir(const char *path)
{
    return (DeleteFile((char *)path) ? 0 : IoErr());
}

int chmod(const char *filename, int bits)       /* bits are as for st_mode */
{
    long protmask = (bits & 0xFF) ^ 0xF;
    return !SetProtection((char *)filename, protmask);
}


/* This here removes unnecessary bulk from the executable with Aztec: */
void _wb_parse(void)  { }

/* fake a unix function that does not apply to amigados: */
int umask(void)  { return 0; }


#  include <signal.h>

/* C library signal() messes up debugging yet adds no actual usefulness */
typedef void (*__signal_return_type)(int);
__signal_return_type signal()  { return SIG_ERR; }


/* The following replaces Aztec's argv-parsing function for compatibility with
Unix-like syntax used on other platforms.  It also fixes the problem the
standard _cli_parse() has of accepting only lower-ascii characters. */

int _argc, _arg_len;
char **_argv, *_arg_lin;

void _cli_parse(struct Process *pp, long alen, register UBYTE *aptr)
{
    register UBYTE *cp;
    register struct CommandLineInterface *cli;
    register short c;
    register short starred = 0;
#  ifdef PRESTART_HOOK
    void Prestart_Hook(void);
#  endif

    cli = (struct CommandLineInterface *) (pp->pr_CLI << 2);
    cp = (UBYTE *) (cli->cli_CommandName << 2);
    _arg_len = cp[0] + alen + 2;
    if (!(_arg_lin = AllocMem((long) _arg_len, 0L)))
        return;
    c = cp[0];
    strncpy(_arg_lin, cp + 1, c);
    _arg_lin[c] = 0;
    for (cp = _arg_lin + c + 1; alen && (*aptr < '\n' || *aptr > '\r'); alen--)
        *cp++ = *aptr++;
    *cp = 0;
    aptr = cp = _arg_lin + c + 1;
    for (_argc = 1; ; _argc++) {
        while (*cp == ' ' || *cp == '\t')
            cp++;
        if (!*cp)
            break;
        if (*cp == '"') {
            cp++;
            while (c = *cp++) {
                if (c == '"' && !starred) {
                    *aptr++ = 0;
                    starred = 0;
                    break;
                } else if (c == '\\' && !starred)
                    starred = 1;
                else {
                    *aptr++ = c;
                    starred = 0;
                }
            }
        } else {
            while ((c = *cp++) && c != ' ' && c != '\t')
                *aptr++ = c;
            *aptr++ = 0;
        }
        if (c == 0)
            --cp;
    }
    *aptr = 0;
    if (!(_argv = AllocMem((_argc + 1) * sizeof(*_argv), 0L))) {
        _argc = 0;
        return;
    }
    for (c = 0, cp = _arg_lin; c < _argc; c++) {
        _argv[c] = cp;
        cp += strlen(cp) + 1;
    }
    _argv[c] = NULL;
#  ifdef PRESTART_HOOK
    Prestart_Hook();
#  endif
}

#endif /* AZTEC_C */

#endif /* __amiga_stat_c */
