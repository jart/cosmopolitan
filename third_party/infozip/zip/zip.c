/*
  zip.c - Zip 3

  Copyright (c) 1990-2008 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*
 *  zip.c by Mark Adler.
 */
#define __ZIP_C

#include "zip.h"
#include <time.h>       /* for tzset() declaration */
#if defined(WIN32) || defined(WINDLL)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif
#ifdef WINDLL
#  include <setjmp.h>
#  include "windll/windll.h"
#endif
#define DEFCPYRT        /* main module: enable copyright string defines! */
#include "revision.h"
#include "crc32.h"
#include "crypt.h"
#include "ttyio.h"
#include <ctype.h>
#include <errno.h>
#ifdef VMS
#  include <stsdef.h>
#  include "vms/vmsmunch.h"
#  include "vms/vms.h"
#endif

#ifdef MACOS
#  include "macglob.h"
   extern MacZipGlobals MacZip;
   extern int error_level;
#endif

#if (defined(MSDOS) && !defined(__GO32__)) || defined(__human68k__)
#  include <process.h>
#  if (!defined(P_WAIT) && defined(_P_WAIT))
#    define P_WAIT _P_WAIT
#  endif
#endif

#include <signal.h>
#include <stdio.h>

#ifdef UNICODE_TEST
# ifdef WIN32
#  include <direct.h>
# endif
#endif

#ifdef BZIP2_SUPPORT
  /* If IZ_BZIP2 is defined as the location of the bzip2 files then
     assume the location has been added to include path.  For Unix
     this is done by the configure script. */
  /* Also do not need path for bzip2 include if OS includes support
     for bzip2 library. */
# include "bzlib.h"
#endif

#define MAXCOM 256      /* Maximum one-line comment size */


/* Local option flags */
#ifndef DELETE
#define DELETE  0
#endif
#define ADD     1
#define UPDATE  2
#define FRESHEN 3
#define ARCHIVE 4
local int action = ADD; /* one of ADD, UPDATE, FRESHEN, DELETE, or ARCHIVE */
local int comadd = 0;   /* 1=add comments for new files */
local int zipedit = 0;  /* 1=edit zip comment and all file comments */
local int latest = 0;   /* 1=set zip file time to time of latest file */
local int test = 0;     /* 1=test zip file with unzip -t */
local char *unzip_path = NULL; /* where to find unzip */
local int tempdir = 0;  /* 1=use temp directory (-b) */
local int junk_sfx = 0; /* 1=junk the sfx prefix */
#if defined(AMIGA) || defined(MACOS)
local int filenotes = 0; /* 1=take comments from AmigaDOS/MACOS filenotes */
#endif

#ifdef EBCDIC
int aflag = __EBCDIC;   /* Convert EBCDIC to ASCII or stay EBCDIC ? */
#endif
#ifdef CMS_MVS
int bflag = 0;          /* Use text mode as default */
#endif

#ifdef QDOS
char _version[] = VERSION;
#endif

#ifdef WINDLL
jmp_buf zipdll_error_return;
#ifdef ZIP64_SUPPORT
  unsigned long low, high; /* returning 64 bit values for systems without an _int64 */
  uzoff_t filesize64;
#endif
#endif

#if CRYPT
/* Pointer to crc_table, needed in crypt.c */
# if (!defined(USE_ZLIB) || defined(USE_OWN_CRCTAB))
ZCONST ulg near *crc_32_tab;
# else
ZCONST uLongf *crc_32_tab;
# endif
#endif /* CRYPT */

/* Local functions */

local void freeup  OF((void));
local int  finish  OF((int));
#if (!defined(MACOS) && !defined(WINDLL))
local void handler OF((int));
local void license OF((void));
#ifndef VMSCLI
local void help    OF((void));
local void help_extended OF((void));
#endif /* !VMSCLI */
#endif /* !MACOS && !WINDLL */

/* prereading of arguments is not supported in new command
   line interpreter get_option() so read filters as arguments
   are processed and convert to expected array later */
local int add_filter OF((int flag, char *pattern));
local int filterlist_to_patterns OF((void));
/* not used
 local int get_filters OF((int argc, char **argv));
*/

/* list to store file arguments */
local long add_name OF((char *filearg));


local int DisplayRunningStats OF((void));
local int BlankRunningStats OF((void));

#if !defined(WINDLL)
local void version_info OF((void));
# if !defined(MACOS)
local void zipstdout OF((void));
# endif /* !MACOS */
local int check_unzip_version OF((char *unzippath));
local void check_zipfile OF((char *zipname, char *zippath));
#endif /* !WINDLL */

/* structure used by add_filter to store filters */
struct filterlist_struct {
  char flag;
  char *pattern;
  struct filterlist_struct *next;
};
struct filterlist_struct *filterlist = NULL;  /* start of list */
struct filterlist_struct *lastfilter = NULL;  /* last filter in list */

/* structure used by add_filearg to store file arguments */
struct filelist_struct {
  char *name;
  struct filelist_struct *next;
};
long filearg_count = 0;
struct filelist_struct *filelist = NULL;  /* start of list */
struct filelist_struct *lastfile = NULL;  /* last file in list */

local void freeup()
/* Free all allocations in the 'found' list, the 'zfiles' list and
   the 'patterns' list. */
{
  struct flist far *f;  /* steps through found list */
  struct zlist far *z;  /* pointer to next entry in zfiles list */

  for (f = found; f != NULL; f = fexpel(f))
    ;
  while (zfiles != NULL)
  {
    z = zfiles->nxt;
    if (zfiles->zname && zfiles->zname != zfiles->name)
      free((zvoid *)(zfiles->zname));
    if (zfiles->name)
      free((zvoid *)(zfiles->name));
    if (zfiles->iname)
      free((zvoid *)(zfiles->iname));
    if (zfiles->cext && zfiles->cextra && zfiles->cextra != zfiles->extra)
      free((zvoid *)(zfiles->cextra));
    if (zfiles->ext && zfiles->extra)
      free((zvoid *)(zfiles->extra));
    if (zfiles->com && zfiles->comment)
      free((zvoid *)(zfiles->comment));
    if (zfiles->oname)
      free((zvoid *)(zfiles->oname));
#ifdef UNICODE_SUPPORT
    if (zfiles->uname)
      free((zvoid *)(zfiles->uname));
    if (zfiles->zuname)
      free((zvoid *)(zfiles->zuname));
    if (zfiles->ouname)
      free((zvoid *)(zfiles->ouname));
# ifdef WIN32
    if (zfiles->namew)
      free((zvoid *)(zfiles->namew));
    if (zfiles->inamew)
      free((zvoid *)(zfiles->inamew));
    if (zfiles->znamew)
      free((zvoid *)(zfiles->znamew));
# endif
#endif
    farfree((zvoid far *)zfiles);
    zfiles = z;
    zcount--;
  }

  if (patterns != NULL) {
    while (pcount-- > 0) {
      if (patterns[pcount].zname != NULL)
        free((zvoid *)(patterns[pcount].zname));
    }
    free((zvoid *)patterns);
    patterns = NULL;
  }

  /* close logfile */
  if (logfile) {
    fclose(logfile);
  }
}

local int finish(e)
int e;                  /* exit code */
/* Process -o and -m options (if specified), free up malloc'ed stuff, and
   exit with the code e. */
{
  int r;                /* return value from trash() */
  ulg t;                /* latest time in zip file */
  struct zlist far *z;  /* pointer into zfile list */

  /* If latest, set time to zip file to latest file in zip file */
  if (latest && zipfile && strcmp(zipfile, "-"))
  {
    diag("changing time of zip file to time of latest file in it");
    /* find latest time in zip file */
    if (zfiles == NULL)
       zipwarn("zip file is empty, can't make it as old as latest entry", "");
    else {
      t = 0;
      for (z = zfiles; z != NULL; z = z->nxt)
        /* Ignore directories in time comparisons */
#ifdef USE_EF_UT_TIME
        if (z->iname[z->nam-1] != (char)0x2f)   /* ascii '/' */
        {
          iztimes z_utim;
          ulg z_tim;

          z_tim = ((get_ef_ut_ztime(z, &z_utim) & EB_UT_FL_MTIME) ?
                   unix2dostime(&z_utim.mtime) : z->tim);
          if (t < z_tim)
            t = z_tim;
        }
#else /* !USE_EF_UT_TIME */
        if (z->iname[z->nam-1] != (char)0x2f    /* ascii '/' */
            && t < z->tim)
          t = z->tim;
#endif /* ?USE_EF_UT_TIME */
      /* set modified time of zip file to that time */
      if (t != 0)
        stamp(zipfile, t);
      else
        zipwarn(
         "zip file has only directories, can't make it as old as latest entry",
         "");
    }
  }
  if (tempath != NULL)
  {
    free((zvoid *)tempath);
    tempath = NULL;
  }
  if (zipfile != NULL)
  {
    free((zvoid *)zipfile);
    zipfile = NULL;
  }
  if (in_file != NULL)
  {
    fclose(in_file);
    in_file = NULL;
  }
  if (in_path != NULL)
  {
    free((zvoid *)in_path);
    in_path = NULL;
  }
  if (out_path != NULL)
  {
    free((zvoid *)out_path);
    out_path = NULL;
  }
  if (zcomment != NULL)
  {
    free((zvoid *)zcomment);
    zcomment = NULL;
  }


  /* If dispose, delete all files in the zfiles list that are marked */
  if (dispose)
  {
    diag("deleting files that were added to zip file");
    if ((r = trash()) != ZE_OK)
      ZIPERR(r, "was deleting moved files and directories");
  }


  /* Done! */
  freeup();
  return e;
}

void ziperr(c, h)
int c;                  /* error code from the ZE_ class */
ZCONST char *h;         /* message about how it happened */
/* Issue a message for the error, clean up files and memory, and exit. */
{
#ifndef WINDLL
#ifndef MACOS
  static int error_level = 0;
#endif

  if (error_level++ > 0)
     /* avoid recursive ziperr() printouts (his should never happen) */
     EXIT(ZE_LOGIC);  /* ziperr recursion is an internal logic error! */
#endif /* !WINDLL */

  if (mesg_line_started) {
    fprintf(mesg, "\n");
    mesg_line_started = 0;
  }
  if (logfile && logfile_line_started) {
    fprintf(logfile, "\n");
    logfile_line_started = 0;
  }
  if (h != NULL) {
    if (PERR(c))
      fprintf(mesg, "zip I/O error: %s", strerror(errno));
      /* perror("zip I/O error"); */
    fflush(mesg);
    fprintf(mesg, "\nzip error: %s (%s)\n", ZIPERRORS(c), h);
#ifdef DOS
    check_for_windows("Zip");
#endif
    if (logfile) {
      if (PERR(c))
        fprintf(logfile, "zip I/O error: %s\n", strerror(errno));
      fprintf(logfile, "\nzip error: %s (%s)\n", ZIPERRORS(c), h);
      logfile_line_started = 0;
    }
  }
  if (tempzip != NULL)
  {
    if (tempzip != zipfile) {
      if (current_local_file)
        fclose(current_local_file);
      if (y != current_local_file && y != NULL)
        fclose(y);
#ifndef DEBUG
      destroy(tempzip);
#endif
      free((zvoid *)tempzip);
    } else {
      /* -g option, attempt to restore the old file */

      /* zip64 support 09/05/2003 R.Nausedat */
      uzoff_t k = 0;                        /* keep count for end header */
      uzoff_t cb = cenbeg;                  /* get start of central */

      struct zlist far *z;  /* steps through zfiles linked list */

      fprintf(mesg, "attempting to restore %s to its previous state\n",
         zipfile);
      if (logfile)
        fprintf(logfile, "attempting to restore %s to its previous state\n",
           zipfile);

      zfseeko(y, cenbeg, SEEK_SET);

      tempzn = cenbeg;
      for (z = zfiles; z != NULL; z = z->nxt)
      {
        putcentral(z);
        tempzn += 4 + CENHEAD + z->nam + z->cext + z->com;
        k++;
      }
      putend(k, tempzn - cb, cb, zcomlen, zcomment);
      fclose(y);
      y = NULL;
    }
  }

  if (key != NULL) {
    free((zvoid *)key);
    key = NULL;
  }
  if (tempath != NULL) {
    free((zvoid *)tempath);
    tempath = NULL;
  }
  if (zipfile != NULL) {
    free((zvoid *)zipfile);
    zipfile = NULL;
  }
  if (out_path != NULL) {
    free((zvoid *)out_path);
    out_path = NULL;
  }
  if (zcomment != NULL) {
    free((zvoid *)zcomment);
    zcomment = NULL;
  }

  freeup();
#ifndef WINDLL
  EXIT(c);
#else
  longjmp(zipdll_error_return, c);
#endif
}


void error(h)
  ZCONST char *h;
/* Internal error, should never happen */
{
  ziperr(ZE_LOGIC, h);
}

#if (!defined(MACOS) && !defined(WINDLL))
local void handler(s)
int s;                  /* signal number (ignored) */
/* Upon getting a user interrupt, turn echo back on for tty and abort
   cleanly using ziperr(). */
{
#if defined(AMIGA) && defined(__SASC)
   _abort();
#else
#if !defined(MSDOS) && !defined(__human68k__) && !defined(RISCOS)
  echon();
  putc('\n', mesg);
#endif /* !MSDOS */
#endif /* AMIGA && __SASC */
  ziperr(ZE_ABORT, "aborting");
  s++;                                  /* keep some compilers happy */
}
#endif /* !MACOS && !WINDLL */

void zipmessage_nl(a, nl)
ZCONST char *a;     /* message string to output */
int nl;             /* 1 = add nl to end */
/* If nl false, print a message to mesg without new line.
   If nl true, print and add new line.  If logfile is
   open then also write message to log file. */
{
  if (noisy) {
    if (a && strlen(a)) {
      fprintf(mesg, "%s", a);
      mesg_line_started = 1;
    }
    if (nl) {
      if (mesg_line_started) {
        fprintf(mesg, "\n");
        mesg_line_started = 0;
      }
    } else if (a && strlen(a)) {
      mesg_line_started = 1;
    }
    fflush(mesg);
  }
  if (logfile) {
    if (a && strlen(a)) {
      fprintf(logfile, "%s", a);
      logfile_line_started = 1;
    }
    if (nl) {
      if (logfile_line_started) {
        fprintf(logfile, "\n");
        logfile_line_started = 0;
      }
    } else if (a && strlen(a)) {
      logfile_line_started = 1;
    }
    fflush(logfile);
  }
}

void zipmessage(a, b)
ZCONST char *a, *b;     /* message strings juxtaposed in output */
/* Print a message to mesg and flush.  Also write to log file if
   open.  Write new line first if current line has output already. */
{
  if (noisy) {
    if (mesg_line_started)
      fprintf(mesg, "\n");
    fprintf(mesg, "%s%s\n", a, b);
    mesg_line_started = 0;
    fflush(mesg);
  }
  if (logfile) {
    if (logfile_line_started)
      fprintf(logfile, "\n");
    fprintf(logfile, "%s%s\n", a, b);
    logfile_line_started = 0;
    fflush(logfile);
  }
}

void zipwarn(a, b)
ZCONST char *a, *b;     /* message strings juxtaposed in output */
/* Print a warning message to mesg (usually stderr) and return. */
{
  if (noisy) {
    if (mesg_line_started)
      fprintf(mesg, "\n");
    fprintf(mesg, "\tzip warning: %s%s\n", a, b);
    mesg_line_started = 0;
    fflush(mesg);
  }
  if (logfile) {
    if (logfile_line_started)
      fprintf(logfile, "\n");
    fprintf(logfile, "\tzip warning: %s%s\n", a, b);
    logfile_line_started = 0;
    fflush(logfile);
  }
}

#ifndef WINDLL
local void license()
/* Print license information to stdout. */
{
  extent i;             /* counter for copyright array */

  for (i = 0; i < sizeof(swlicense)/sizeof(char *); i++)
    puts(swlicense[i]);
}

#ifdef VMSCLI
void help()
#else
local void help()
#endif
/* Print help (along with license info) to stdout. */
{
  extent i;             /* counter for help array */

  /* help array */
  static ZCONST char *text[] = {
#ifdef VMS
"Zip %s (%s). Usage: zip == \"$ disk:[dir]zip.exe\"",
#else
"Zip %s (%s). Usage:",
#endif
#ifdef MACOS
"zip [-options] [-b fm] [-t mmddyyyy] [-n suffixes] [zipfile list] [-xi list]",
"  The default action is to add or replace zipfile entries from list.",
" ",
"  -f   freshen: only changed files  -u   update: only changed or new files",
"  -d   delete entries in zipfile    -m   move into zipfile (delete OS files)",
"  -r   recurse into directories     -j   junk (don't record) directory names",
"  -0   store only                   -l   convert LF to CR LF (-ll CR LF to LF)",
"  -1   compress faster              -9   compress better",
"  -q   quiet operation              -v   verbose operation/print version info",
"  -c   add one-line comments        -z   add zipfile comment",
"                                    -o   make zipfile as old as latest entry",
"  -F   fix zipfile (-FF try harder) -D   do not add directory entries",
"  -T   test zipfile integrity       -X   eXclude eXtra file attributes",
#  if CRYPT
"  -e   encrypt                      -n   don't compress these suffixes"
#  else
"  -h   show this help               -n   don't compress these suffixes"
#  endif
," -h2  show more help",
"  Macintosh specific:",
"  -jj  record Fullpath (+ Volname)  -N store finder-comments as comments",
"  -df  zip only datafork of a file  -S include finder invisible/system files"
#else /* !MACOS */
#ifdef VM_CMS
"zip [-options] [-b fm] [-t mmddyyyy] [-n suffixes] [zipfile list] [-xi list]",
#else  /* !VM_CMS */
"zip [-options] [-b path] [-t mmddyyyy] [-n suffixes] [zipfile list] [-xi list]",
#endif /* ?VM_CMS */
"  The default action is to add or replace zipfile entries from list, which",
"  can include the special name - to compress standard input.",
"  If zipfile and list are omitted, zip compresses stdin to stdout.",
"  -f   freshen: only changed files  -u   update: only changed or new files",
"  -d   delete entries in zipfile    -m   move into zipfile (delete OS files)",
"  -r   recurse into directories     -j   junk (don't record) directory names",
#ifdef THEOS
"  -0   store only                   -l   convert CR to CR LF (-ll CR LF to CR)",
#else
"  -0   store only                   -l   convert LF to CR LF (-ll CR LF to LF)",
#endif
"  -1   compress faster              -9   compress better",
"  -q   quiet operation              -v   verbose operation/print version info",
"  -c   add one-line comments        -z   add zipfile comment",
"  -@   read names from stdin        -o   make zipfile as old as latest entry",
"  -x   exclude the following names  -i   include only the following names",
#ifdef EBCDIC
#ifdef CMS_MVS
"  -a   translate to ASCII           -B   force binary read (text is default)",
#else  /* !CMS_MVS */
"  -a   translate to ASCII",
#endif /* ?CMS_MVS */
#endif /* EBCDIC */
#ifdef TANDEM
"                                    -Bn  set Enscribe formatting options",
#endif
"  -F   fix zipfile (-FF try harder) -D   do not add directory entries",
"  -A   adjust self-extracting exe   -J   junk zipfile prefix (unzipsfx)",
"  -T   test zipfile integrity       -X   eXclude eXtra file attributes",
#ifdef VMS
"  -C   preserve case of file names  -C-  down-case all file names",
"  -C2  preserve case of ODS2 names  -C2- down-case ODS2 file names* (*=default)",
"  -C5  preserve case of ODS5 names* -C5- down-case ODS5 file names",
"  -V   save VMS file attributes (-VV also save allocated blocks past EOF)",
"  -w   store file version numbers\
   -ww  store file version numbers as \".nnn\"",
#endif /* def VMS */
#ifdef NTSD_EAS
"  -!   use privileges (if granted) to obtain all aspects of WinNT security",
#endif /* NTSD_EAS */
#ifdef OS2
"  -E   use the .LONGNAME Extended attribute (if found) as filename",
#endif /* OS2 */
#ifdef S_IFLNK
"  -y   store symbolic links as the link instead of the referenced file",
#endif /* !S_IFLNK */
/*
"  -R   PKZIP recursion (see manual)",
*/
#if defined(MSDOS) || defined(OS2)
"  -$   include volume label         -S   include system and hidden files",
#endif
#ifdef AMIGA
#  if CRYPT
"  -N   store filenotes as comments  -e   encrypt",
"  -h   show this help               -n   don't compress these suffixes"
#  else
"  -N   store filenotes as comments  -n   don't compress these suffixes"
#  endif
#else /* !AMIGA */
#  if CRYPT
"  -e   encrypt                      -n   don't compress these suffixes"
#  else
"  -h   show this help               -n   don't compress these suffixes"
#  endif
#endif /* ?AMIGA */
#ifdef RISCOS
,"  -h2  show more help               -I   don't scan thru Image files"
#else
,"  -h2  show more help"
#endif
#endif /* ?MACOS */
#ifdef VMS
,"  (Must quote upper-case options, like \"-V\", unless SET PROC/PARSE=EXTEND)"
#endif /* def VMS */
,"  "
  };

  for (i = 0; i < sizeof(copyright)/sizeof(char *); i++)
  {
    printf(copyright[i], "zip");
    putchar('\n');
  }
  for (i = 0; i < sizeof(text)/sizeof(char *); i++)
  {
    printf(text[i], VERSION, REVDATE);
    putchar('\n');
  }
}

#ifdef VMSCLI
void help_extended()
#else
local void help_extended()
#endif
/* Print extended help to stdout. */
{
  extent i;             /* counter for help array */

  /* help array */
  static ZCONST char *text[] = {
"",
"Extended Help for Zip",
"",
"See the Zip Manual for more detailed help",
"",
"",
"Zip stores files in zip archives.  The default action is to add or replace",
"zipfile entries.",
"",
"Basic command line:",
"  zip options archive_name file file ...",
"",
"Some examples:",
"  Add file.txt to z.zip (create z if needed):      zip z file.txt",
"  Zip all files in current dir:                    zip z *",
"  Zip files in current dir and subdirs also:       zip -r z .",
"",
"Basic modes:",
" External modes (selects files from file system):",
"        add      - add new files/update existing files in archive (default)",
"  -u    update   - add new files/update existing files only if later date",
"  -f    freshen  - update existing files only (no files added)",
"  -FS   filesync - update if date or size changed, delete if no OS match",
" Internal modes (selects entries in archive):",
"  -d    delete   - delete files from archive (see below)",
"  -U    copy     - select files in archive to copy (use with --out)",
"",
"Basic options:",
"  -r        recurse into directories (see Recursion below)",
"  -m        after archive created, delete original files (move into archive)",
"  -j        junk directory names (store just file names)",
"  -q        quiet operation",
"  -v        verbose operation (just \"zip -v\" shows version information)",
"  -c        prompt for one-line comment for each entry",
"  -z        prompt for comment for archive (end with just \".\" line or EOF)",
"  -@        read names to zip from stdin (one path per line)",
"  -o        make zipfile as old as latest entry",
"",
"",
"Syntax:",
"  The full command line syntax is:",
"",
"    zip [-shortopts ...] [--longopt ...] [zipfile [path path ...]] [-xi list]",
"",
"  Any number of short option and long option arguments are allowed",
"  (within limits) as well as any number of path arguments for files",
"  to zip up.  If zipfile exists, the archive is read in.  If zipfile",
"  is \"-\", stream to stdout.  If any path is \"-\", zip stdin.",
"",
"Options and Values:",
"  For short options that take values, use -ovalue or -o value or -o=value",
"  For long option values, use either --longoption=value or --longoption value",
"  For example:",
"    zip -ds 10 --temp-dir=path zipfile path1 path2 --exclude pattern pattern",
"  Avoid -ovalue (no space between) to avoid confusion",
"  In particular, be aware of 2-character options.  For example:",
"    -d -s is (delete, split size) while -ds is (dot size)",
"  Usually better to break short options across multiple arguments by function",
"    zip -r -dbdcds 10m -lilalf logfile archive input_directory -ll",
"",
"  All args after just \"--\" arg are read verbatim as paths and not options.",
"    zip zipfile path path ... -- verbatimpath verbatimpath ...",
"  Use -nw to also disable wildcards, so paths are read literally:",
"    zip zipfile -nw -- \"-leadingdashpath\" \"a[path].c\" \"path*withwildcard\"",
"  You may still have to escape or quote arguments to avoid shell expansion",
"",
"Wildcards:",
"  Internally zip supports the following wildcards:",
"    ?       (or %% or #, depending on OS) matches any single character",
"    *       matches any number of characters, including zero",
"    [list]  matches char in list (regex), can do range [ac-f], all but [!bf]",
"  If port supports [], must escape [ as [[] or use -nw to turn off wildcards",
"  For shells that expand wildcards, escape (\\* or \"*\") so zip can recurse",
"    zip zipfile -r . -i \"*.h\"",
"",
"  Normally * crosses dir bounds in path, e.g. 'a*b' can match 'ac/db'.  If",
"   -ws option used, * does not cross dir bounds but ** does",
"",
"  For DOS and Windows, [list] is now disabled unless the new option",
"  -RE       enable [list] (regular expression) matching",
"  is used to avoid problems with file paths containing \"[\" and \"]\":",
"    zip files_ending_with_number -RE foo[0-9].c",
"",
"Include and Exclude:",
"  -i pattern pattern ...   include files that match a pattern",
"  -x pattern pattern ...   exclude files that match a pattern",
"  Patterns are paths with optional wildcards and match paths as stored in",
"  archive.  Exclude and include lists end at next option, @, or end of line.",
"    zip -x pattern pattern @ zipfile path path ...",
"",
"Case matching:",
"  On most OS the case of patterns must match the case in the archive, unless",
"  the -ic option is used.",
"  -ic       ignore case of archive entries",
"  This option not available on case-sensitive file systems.  On others, case",
"  ignored when matching files on file system but matching against archive",
"  entries remains case sensitive for modes -f (freshen), -U (archive copy),",
"  and -d (delete) because archive paths are always case sensitive.  With",
"  -ic, all matching ignores case, but it's then possible multiple archive",
"  entries that differ only in case will match.",
"",
"End Of Line Translation (text files only):",
"  -l        change CR or LF (depending on OS) line end to CR LF (Unix->Win)",
"  -ll       change CR LF to CR or LF (depending on OS) line end (Win->Unix)",
"  If first buffer read from file contains binary the translation is skipped",
"",
"Recursion:",
"  -r        recurse paths, include files in subdirs:  zip -r a path path ...",
"  -R        recurse current dir and match patterns:   zip -R a ptn ptn ...",
"  Use -i and -x with either to include or exclude paths",
"  Path root in archive starts at current dir, so if /a/b/c/file and",
"   current dir is /a/b, 'zip -r archive .' puts c/file in archive",
"",
"Date filtering:",
"  -t date   exclude before (include files modified on this date and later)",
"  -tt date  include before (include files modified before date)",
"  Can use both at same time to set a date range",
"  Dates are mmddyyyy or yyyy-mm-dd",
"",
"Deletion, File Sync:",
"  -d        delete files",
"  Delete archive entries matching internal archive paths in list",
"    zip archive -d pattern pattern ...",
"  Can use -t and -tt to select files in archive, but NOT -x or -i, so",
"    zip archive -d \"*\" -t 2005-12-27",
"  deletes all files from archive.zip with date of 27 Dec 2005 and later",
"  Note the * (escape as \"*\" on Unix) to select all files in archive",
"",
"  -FS       file sync",
"  Similar to update, but files updated if date or size of entry does not",
"  match file on OS.  Also deletes entry from archive if no matching file",
"  on OS.",
"    zip archive_to_update -FS -r dir_used_before",
"  Result generally same as creating new archive, but unchanged entries",
"  are copied instead of being read and compressed so can be faster.",
"      WARNING:  -FS deletes entries so make backup copy of archive first",
"",
"Compression:",
"  -0        store files (no compression)",
"  -1 to -9  compress fastest to compress best (default is 6)",
"  -Z cm     set compression method to cm:",
"              store   - store without compression, same as option -0",
"              deflate - original zip deflate, same as -1 to -9 (default)",
"            if bzip2 is enabled:",
"              bzip2 - use bzip2 compression (need modern unzip)",
"",
"Encryption:",
"  -e        use standard (weak) PKZip 2.0 encryption, prompt for password",
"  -P pswd   use standard encryption, password is pswd",
"",
"Splits (archives created as a set of split files):",
"  -s ssize  create split archive with splits of size ssize, where ssize nm",
"              n number and m multiplier (kmgt, default m), 100k -> 100 kB",
"  -sp       pause after each split closed to allow changing disks",
"      WARNING:  Archives created with -sp use data descriptors and should",
"                work with most unzips but may not work with some",
"  -sb       ring bell when pause",
"  -sv       be verbose about creating splits",
"      Split archives CANNOT be updated, but see --out and Copy Mode below",
"",
"Using --out (output to new archive):",
"  --out oa  output to new archive oa",
"  Instead of updating input archive, create new output archive oa.",
"  Result is same as without --out but in new archive.  Input archive",
"  unchanged.",
"      WARNING:  --out ALWAYS overwrites any existing output file",
"  For example, to create new_archive like old_archive but add newfile1",
"  and newfile2:",
"    zip old_archive newfile1 newfile2 --out new_archive",
"  Cannot update split archive, so use --out to out new archive:",
"    zip in_split_archive newfile1 newfile2 --out out_split_archive",
"  If input is split, output will default to same split size",
"  Use -s=0 or -s- to turn off splitting to convert split to single file:",
"    zip in_split_archive -s 0 --out out_single_file_archive",
"      WARNING:  If overwriting old split archive but need less splits,",
"                old splits not overwritten are not needed but remain",
"",
"Copy Mode (copying from archive to archive):",
"  -U        (also --copy) select entries in archive to copy (reverse delete)",
"  Copy Mode copies entries from old to new archive with --out and is used by",
"  zip when either no input files on command line or -U (--copy) used.",
"    zip inarchive --copy pattern pattern ... --out outarchive",
"  To copy only files matching *.c into new archive, excluding foo.c:",
"    zip old_archive --copy \"*.c\" --out new_archive -x foo.c",
"  If no input files and --out, copy all entries in old archive:",
"    zip old_archive --out new_archive",
"",
"Streaming and FIFOs:",
"  prog1 | zip -ll z -      zip output of prog1 to zipfile z, converting CR LF",
"  zip - -R \"*.c\" | prog2   zip *.c files in current dir and stream to prog2 ",
"  prog1 | zip | prog2      zip in pipe with no in or out acts like zip - -",
"  If Zip is Zip64 enabled, streaming stdin creates Zip64 archives by default",
"   that need PKZip 4.5 unzipper like UnZip 6.0",
"  WARNING:  Some archives created with streaming use data descriptors and",
"            should work with most unzips but may not work with some",
"  Can use -fz- to turn off Zip64 if input not large (< 4 GB):",
"    prog_with_small_output | zip archive -fz-",
"",
"  Zip now can read Unix FIFO (named pipes).  Off by default to prevent zip",
"  from stopping unexpectedly on unfed pipe, use -FI to enable:",
"    zip -FI archive fifo",
"",
"Dots, counts:",
"  -db       display running count of bytes processed and bytes to go",
"              (uncompressed size, except delete and copy show stored size)",
"  -dc       display running count of entries done and entries to go",
"  -dd       display dots every 10 MB (or dot size) while processing files",
"  -dg       display dots globally for archive instead of for each file",
"    zip -qdgds 10m   will turn off most output except dots every 10 MB",
"  -ds siz   each dot is siz processed where siz is nm as splits (0 no dots)",
"  -du       display original uncompressed size for each entry as added",
"  -dv       display volume (disk) number in format in_disk>out_disk",
"  Dot size is approximate, especially for dot sizes less than 1 MB",
"  Dot options don't apply to Scanning files dots (dot/2sec) (-q turns off)",
"",
"Logging:",
"  -lf path  open file at path as logfile (overwrite existing file)",
"  -la       append to existing logfile",
"  -li       include info messages (default just warnings and errors)",
"",
"Testing archives:",
"  -T        test completed temp archive with unzip before updating archive",
"  -TT cmd   use command cmd instead of 'unzip -tqq' to test archive",
"             On Unix, to use unzip in current directory, could use:",
"               zip archive file1 file2 -T -TT \"./unzip -tqq\"",
"             In cmd, {} replaced by temp archive path, else temp appended.",
"             The return code is checked for success (0 on Unix)",
"",
"Fixing archives:",
"  -F        attempt to fix a mostly intact archive (try this first)",
"  -FF       try to salvage what can (may get more but less reliable)",
"  Fix options copy entries from potentially bad archive to new archive.",
"  -F tries to read archive normally and copy only intact entries, while",
"  -FF tries to salvage what can and may result in incomplete entries.",
"  Must use --out option to specify output archive:",
"    zip -F bad.zip --out fixed.zip",
"  Use -v (verbose) with -FF to see details:",
"    zip reallybad.zip -FF -v --out fixed.zip",
"  Currently neither option fixes bad entries, as from text mode ftp get.",
"",
"Difference mode:",
"  -DF       (also --dif) only include files that have changed or are",
"             new as compared to the input archive",
"  Difference mode can be used to create incremental backups.  For example:",
"    zip --dif full_backup.zip -r somedir --out diff.zip",
"  will store all new files, as well as any files in full_backup.zip where",
"  either file time or size have changed from that in full_backup.zip,",
"  in new diff.zip.  Output archive not excluded automatically if exists,",
"  so either use -x to exclude it or put outside what is being zipped.",
"",
"DOS Archive bit (Windows only):",
"  -AS       include only files with the DOS Archive bit set",
"  -AC       after archive created, clear archive bit of included files",
"      WARNING: Once the archive bits are cleared they are cleared",
"               Use -T to test the archive before the bits are cleared",
"               Can also use -sf to save file list before zipping files",
"",
"Show files:",
"  -sf       show files to operate on and exit (-sf- logfile only)",
"  -su       as -sf but show escaped UTF-8 Unicode names also if exist",
"  -sU       as -sf but show escaped UTF-8 Unicode names instead",
"  Any character not in the current locale is escaped as #Uxxxx, where x",
"  is hex digit, if 16-bit code is sufficient, or #Lxxxxxx if 24-bits",
"  are needed.  If add -UN=e, Zip escapes all non-ASCII characters.",
"",
"Unicode:",
"  If compiled with Unicode support, Zip stores UTF-8 path of entries.",
"  This is backward compatible.  Unicode paths allow better conversion",
"  of entry names between different character sets.",
"",
"  New Unicode extra field includes checksum to verify Unicode path",
"  goes with standard path for that entry (as utilities like ZipNote",
"  can rename entries).  If these do not match, use below options to",
"  set what Zip does:",
"      -UN=Quit     - if mismatch, exit with error",
"      -UN=Warn     - if mismatch, warn, ignore UTF-8 (default)",
"      -UN=Ignore   - if mismatch, quietly ignore UTF-8",
"      -UN=No       - ignore any UTF-8 paths, use standard paths for all",
"  An exception to -UN=N are entries with new UTF-8 bit set (instead",
"  of using extra fields).  These are always handled as Unicode.",
"",
"  Normally Zip escapes all chars outside current char set, but leaves",
"  as is supported chars, which may not be OK in path names.  -UN=Escape",
"  escapes any character not ASCII:",
"    zip -sU -UN=e archive",
"  Can use either normal path or escaped Unicode path on command line",
"  to match files in archive.",
"",
"  Zip now stores UTF-8 in entry path and comment fields on systems",
"  where UTF-8 char set is default, such as most modern Unix, and",
"  and on other systems in new extra fields with escaped versions in",
"  entry path and comment fields for backward compatibility.",
"  Option -UN=UTF8 will force storing UTF-8 in entry path and comment",
"  fields:",
"      -UN=UTF8     - store UTF-8 in entry path and comment fields",
"  This option can be useful for multi-byte char sets on Windows where",
"  escaped paths and comments can be too long to be valid as the UTF-8",
"  versions tend to be shorter.",
"",
"  Only UTF-8 comments on UTF-8 native systems supported.  UTF-8 comments",
"  for other systems planned in next release.",
"",
"Self extractor:",
"  -A        Adjust offsets - a self extractor is created by prepending",
"             the extractor executable to archive, but internal offsets",
"             are then off.  Use -A to fix offsets.",
"  -J        Junk sfx - removes prepended extractor executable from",
"             self extractor, leaving a plain zip archive.",
"",
"More option highlights (see manual for additional options and details):",
"  -b dir    when creating or updating archive, create the temp archive in",
"             dir, which allows using seekable temp file when writing to a",
"             write once CD, such archives compatible with more unzips",
"             (could require additional file copy if on another device)",
"  -MM       input patterns must match at least one file and matched files",
"             must be readable or exit with OPEN error and abort archive",
"             (without -MM, both are warnings only, and if unreadable files",
"             are skipped OPEN error (18) returned after archive created)",
"  -nw       no wildcards (wildcards are like any other character)",
"  -sc       show command line arguments as processed and exit",
"  -sd       show debugging as Zip does each step",
"  -so       show all available options on this system",
"  -X        default=strip old extra fields, -X- keep old, -X strip most",
"  -ws       wildcards don't span directory boundaries in paths",
""
  };

  for (i = 0; i < sizeof(text)/sizeof(char *); i++)
  {
    printf(text[i]);
    putchar('\n');
  }
#ifdef DOS
  check_for_windows("Zip");
#endif
}

/*
 * XXX version_info() in a separate file
 */
local void version_info()
/* Print verbose info about program version and compile time options
   to stdout. */
{
  extent i;             /* counter in text arrays */
  char *envptr;

  /* Bzip2 option string storage (with version). */

#ifdef BZIP2_SUPPORT
  static char bz_opt_ver[81];
  static char bz_opt_ver2[81];
  static char bz_opt_ver3[81];
#endif

  /* Options info array */
  static ZCONST char *comp_opts[] = {
#ifdef ASM_CRC
    "ASM_CRC",
#endif
#ifdef ASMV
    "ASMV",
#endif
#ifdef DYN_ALLOC
    "DYN_ALLOC",
#endif
#ifdef MMAP
    "MMAP",
#endif
#ifdef BIG_MEM
    "BIG_MEM",
#endif
#ifdef MEDIUM_MEM
    "MEDIUM_MEM",
#endif
#ifdef SMALL_MEM
    "SMALL_MEM",
#endif
#ifdef DEBUG
    "DEBUG",
#endif
#ifdef USE_EF_UT_TIME
    "USE_EF_UT_TIME       (store Universal Time)",
#endif
#ifdef NTSD_EAS
    "NTSD_EAS             (store NT Security Descriptor)",
#endif
#if defined(WIN32) && defined(NO_W32TIMES_IZFIX)
    "NO_W32TIMES_IZFIX",
#endif
#ifdef VMS
#ifdef VMSCLI
    "VMSCLI",
#endif
#ifdef VMS_IM_EXTRA
    "VMS_IM_EXTRA",
#endif
#ifdef VMS_PK_EXTRA
    "VMS_PK_EXTRA",
#endif
#endif /* VMS */
#ifdef WILD_STOP_AT_DIR
    "WILD_STOP_AT_DIR     (wildcards do not cross directory boundaries)",
#endif
#ifdef WIN32_OEM
    "WIN32_OEM            (store file paths on Windows as OEM)",
#endif
#ifdef BZIP2_SUPPORT
    bz_opt_ver,
    bz_opt_ver2,
    bz_opt_ver3,
#endif
#ifdef S_IFLNK
# ifdef VMS
    "SYMLINK_SUPPORT      (symbolic links supported, if C RTL permits)",
# else
    "SYMLINK_SUPPORT      (symbolic links supported)",
# endif
#endif
#ifdef LARGE_FILE_SUPPORT
# ifdef USING_DEFAULT_LARGE_FILE_SUPPORT
    "LARGE_FILE_SUPPORT (default settings)",
# else
    "LARGE_FILE_SUPPORT   (can read and write large files on file system)",
# endif
#endif
#ifdef ZIP64_SUPPORT
    "ZIP64_SUPPORT        (use Zip64 to store large files in archives)",
#endif
#ifdef UNICODE_SUPPORT
    "UNICODE_SUPPORT      (store and read UTF-8 Unicode paths)",
#endif

#ifdef UNIX
    "STORE_UNIX_UIDs_GIDs (store UID/GID sizes/values using new extra field)",
# ifdef UIDGID_NOT_16BIT
    "UIDGID_NOT_16BIT     (old Unix 16-bit UID/GID extra field not used)",
# else
    "UIDGID_16BIT         (old Unix 16-bit UID/GID extra field also used)",
# endif
#endif

#if CRYPT && defined(PASSWD_FROM_STDIN)
    "PASSWD_FROM_STDIN",
#endif /* CRYPT & PASSWD_FROM_STDIN */
    NULL
  };

  static ZCONST char *zipenv_names[] = {
#ifndef VMS
#  ifndef RISCOS
    "ZIP"
#  else /* RISCOS */
    "Zip$Options"
#  endif /* ?RISCOS */
#else /* VMS */
    "ZIP_OPTS"
#endif /* ?VMS */
    ,"ZIPOPT"
#ifdef AZTEC_C
    ,     /* extremely lame compiler bug workaround */
#endif
#ifndef __RSXNT__
# ifdef __EMX__
    ,"EMX"
    ,"EMXOPT"
# endif
# if (defined(__GO32__) && (!defined(__DJGPP__) || __DJGPP__ < 2))
    ,"GO32"
    ,"GO32TMP"
# endif
# if (defined(__DJGPP__) && __DJGPP__ >= 2)
    ,"TMPDIR"
# endif
#endif /* !__RSXNT__ */
#ifdef RISCOS
    ,"Zip$Exts"
#endif
  };

  for (i = 0; i < sizeof(copyright)/sizeof(char *); i++)
  {
    printf(copyright[i], "zip");
    putchar('\n');
  }

  for (i = 0; i < sizeof(versinfolines)/sizeof(char *); i++)
  {
    printf(versinfolines[i], "Zip", VERSION, REVDATE);
    putchar('\n');
  }

  version_local();

  puts("Zip special compilation options:");
#if WSIZE != 0x8000
  printf("\tWSIZE=%u\n", WSIZE);
#endif

  /* Fill in bzip2 version.  (32-char limit valid as of bzip 1.0.3.) */
#ifdef BZIP2_SUPPORT
  sprintf( bz_opt_ver,
   "BZIP2_SUPPORT        (bzip2 library version %.32s)", BZ2_bzlibVersion());
  sprintf( bz_opt_ver2,
   "    bzip2 code and library copyright (c) Julian R Seward");
  sprintf( bz_opt_ver3,
   "    (See the bzip2 license for terms of use)");
#endif

  for (i = 0; (int)i < (int)(sizeof(comp_opts)/sizeof(char *) - 1); i++)
  {
    printf("\t%s\n",comp_opts[i]);
  }
#ifdef USE_ZLIB
  if (strcmp(ZLIB_VERSION, zlibVersion()) == 0)
    printf("\tUSE_ZLIB [zlib version %s]\n", ZLIB_VERSION);
  else
    printf("\tUSE_ZLIB [compiled with version %s, using version %s]\n",
      ZLIB_VERSION, zlibVersion());
  i++;  /* zlib use means there IS at least one compilation option */
#endif
#if CRYPT
  printf("\t[encryption, version %d.%d%s of %s] (modified for Zip 3)\n\n",
            CR_MAJORVER, CR_MINORVER, CR_BETA_VER, CR_VERSION_DATE);
  for (i = 0; i < sizeof(cryptnote)/sizeof(char *); i++)
  {
    printf(cryptnote[i]);
    putchar('\n');
  }
  ++i;  /* crypt support means there IS at least one compilation option */
#endif /* CRYPT */
  if (i == 0)
      puts("\t[none]");

  puts("\nZip environment options:");
  for (i = 0; i < sizeof(zipenv_names)/sizeof(char *); i++)
  {
    envptr = getenv(zipenv_names[i]);
    printf("%16s:  %s\n", zipenv_names[i],
           ((envptr == (char *)NULL || *envptr == 0) ? "[none]" : envptr));
  }
#ifdef DOS
  check_for_windows("Zip");
#endif
}
#endif /* !WINDLL */


#ifndef PROCNAME
/* Default to case-sensitive matching of archive entries for the modes
   that specifically operate on archive entries, as this archive may
   have come from a system that allows paths in the archive to differ
   only by case.  Except for adding ARCHIVE (copy mode), this is how it
   was done before.  Note that some case-insensitive ports (WIN32, VMS)
   define their own PROCNAME() in their respective osdep.h that use the
   filter_match_case flag set to FALSE by the -ic option to enable
   case-insensitive archive entry mathing. */
#  define PROCNAME(n) procname(n, (action == ARCHIVE || action == DELETE \
                                   || action == FRESHEN) \
                                  && filter_match_case)
#endif /* PROCNAME */

#ifndef WINDLL
#ifndef MACOS
local void zipstdout()
/* setup for writing zip file on stdout */
{
  mesg = stderr;
  if (isatty(1))
    ziperr(ZE_PARMS, "cannot write zip file to terminal");
  if ((zipfile = malloc(4)) == NULL)
    ziperr(ZE_MEM, "was processing arguments");
  strcpy(zipfile, "-");
  /*
  if ((r = readzipfile()) != ZE_OK)
    ziperr(r, zipfile);
  */
}
#endif /* !MACOS */

local int check_unzip_version(unzippath)
  char *unzippath;
{
#ifdef ZIP64_SUPPORT
  /* Here is where we need to check for the version of unzip the user
   * has.  If creating a Zip64 archive need UnZip 6 or may fail.
   */
    char cmd[4004];
    FILE *unzip_out = NULL;
    char buf[1001];
    float UnZip_Version = 0.0;

    cmd[0] = '\0';
    strncat(cmd, unzippath, 4000);
    strcat(cmd, " -v");

    if ((unzip_out = popen(cmd, "r")) == NULL) {
      perror("unzip pipe error");
    } else {
      if (fgets(buf, 1000, unzip_out) == NULL) {
        zipwarn("failed to get information from UnZip", "");
      } else {
        /* the first line should start with the version */
        if (sscanf(buf, "UnZip %f ", &UnZip_Version) < 1) {
          zipwarn("unexpected output of UnZip -v", "");
        } else {
          /* printf("UnZip %f\n", UnZip_Version); */

          while (fgets(buf, 1000, unzip_out)) {
          }
        }
      }
      pclose(unzip_out);
    }
    if (UnZip_Version < 6.0 && zip64_archive) {
      sprintf(buf, "Found UnZip version %4.2f", UnZip_Version);
      zipwarn(buf, "");
      zipwarn("Need UnZip 6.00 or later to test this Zip64 archive", "");
      return 0;
    }
#endif
  return 1;
}

local void check_zipfile(zipname, zippath)
  char *zipname;
  char *zippath;
  /* Invoke unzip -t on the given zip file */
{
#if (defined(MSDOS) && !defined(__GO32__)) || defined(__human68k__)
  int status, len;
  char *path, *p;
  char *zipnam;

  if ((zipnam = (char *)malloc(strlen(zipname) + 3)) == NULL)
    ziperr(ZE_MEM, "was creating unzip zipnam");

# ifdef MSDOS
  /* Add quotes for MSDOS.  8/11/04 */
  strcpy(zipnam, "\"");    /* accept spaces in name and path */
  strcat(zipnam, zipname);
  strcat(zipnam, "\"");
# else
  strcpy(zipnam, zipname);
# endif

  if (unzip_path) {
    /* if user gave us the unzip to use go with it */
    char *here;
    int len;
    char *cmd;

    /* Replace first {} with archive name.  If no {} append name to string. */
    here = strstr(unzip_path, "{}");

    if ((cmd = (char *)malloc(strlen(unzip_path) + strlen(zipnam) + 3)) == NULL)
      ziperr(ZE_MEM, "was creating unzip cmd");

    if (here) {
      /* have {} so replace with temp name */
      len = here - unzip_path;
      strcpy(cmd, unzip_path);
      cmd[len] = '\0';
      strcat(cmd, " ");
      strcat(cmd, zipnam);
      strcat(cmd, " ");
      strcat(cmd, here + 2);
    } else {
      /* No {} so append temp name to end */
      strcpy(cmd, unzip_path);
      strcat(cmd, " ");
      strcat(cmd, zipnam);
    }

    status = system(cmd);

    free(unzip_path);
    unzip_path = NULL;
    free(cmd);
  } else {
    /* Here is where we need to check for the version of unzip the user
     * has.  If creating a Zip64 archive need UnZip 6 or may fail.
     */
    if (check_unzip_version("unzip") == 0)
      ZIPERR(ZE_TEST, zipfile);

    status = spawnlp(P_WAIT, "unzip", "unzip", verbose ? "-t" : "-tqq",
                     zipnam, NULL);
# ifdef __human68k__
    if (status == -1)
      perror("unzip");
# else
/*
 * unzip isn't in PATH range, assume an absolute path to zip in argv[0]
 * and hope that unzip is in the same directory.
 */
    if (status == -1) {
      p = MBSRCHR(zippath, '\\');
      path = MBSRCHR((p == NULL ? zippath : p), '/');
      if (path != NULL)
        p = path;
      if (p != NULL) {
        len = (int)(p - zippath) + 1;
        if ((path = malloc(len + sizeof("unzip.exe"))) == NULL)
          ziperr(ZE_MEM, "was creating unzip path");
        memcpy(path, zippath, len);
        strcpy(&path[len], "unzip.exe");

        if (check_unzip_version(path) == 0)
          ZIPERR(ZE_TEST, zipfile);

        status = spawnlp(P_WAIT, path, "unzip", verbose ? "-t" : "-tqq",
                        zipnam, NULL);
        free(path);
      }
      if (status == -1)
        perror("unzip");
    }
  }
# endif /* ?__human68k__ */
  free(zipnam);
  if (status != 0) {

#else /* (MSDOS && !__GO32__) || __human68k__ */
  char *cmd;
  int result;

  /* Tell picky compilers to shut up about unused variables */
  zippath = zippath;

  if (unzip_path) {
    /* user gave us a path to some unzip (may not be UnZip) */
    char *here;
    int len;

    /* Replace first {} with archive name.  If no {} append name to string. */
    here = strstr(unzip_path, "{}");

    if ((cmd = malloc(strlen(unzip_path) + strlen(zipname) + 3)) == NULL) {
      ziperr(ZE_MEM, "building command string for testing archive");
    }

    if (here) {
      /* have {} so replace with temp name */
      len = here - unzip_path;
      strcpy(cmd, unzip_path);
      cmd[len] = '\0';
      strcat(cmd, " ");
# ifdef UNIX
      strcat(cmd, "'");    /* accept space or $ in name */
      strcat(cmd, zipname);
      strcat(cmd, "'");
# else
      strcat(cmd, zipname);
# endif
      strcat(cmd, " ");
      strcat(cmd, here + 2);
    } else {
      /* No {} so append temp name to end */
      strcpy(cmd, unzip_path);
      strcat(cmd, " ");
# ifdef UNIX
      strcat(cmd, "'");    /* accept space or $ in name */
      strcat(cmd, zipname);
      strcat(cmd, "'");
# else
      strcat(cmd, zipname);
# endif
    }
    free(unzip_path);
    unzip_path = NULL;

  } else {
    if ((cmd = malloc(20 + strlen(zipname))) == NULL) {
      ziperr(ZE_MEM, "building command string for testing archive");
    }

    strcpy(cmd, "unzip -t ");
# ifdef QDOS
    strcat(cmd, "-Q4 ");
# endif
    if (!verbose) strcat(cmd, "-qq ");
    if (check_unzip_version("unzip") == 0)
      ZIPERR(ZE_TEST, zipfile);

# ifdef UNIX
    strcat(cmd, "'");    /* accept space or $ in name */
    strcat(cmd, zipname);
    strcat(cmd, "'");
# else
    strcat(cmd, zipname);
# endif
  }

  result = system(cmd);
# ifdef VMS
  /* Convert success severity to 0, others to non-zero. */
  result = ((result & STS$M_SEVERITY) != STS$M_SUCCESS);
# endif /* def VMS */
  free(cmd);
  cmd = NULL;
  if (result) {
#endif /* ?((MSDOS && !__GO32__) || __human68k__) */

    fprintf(mesg, "test of %s FAILED\n", zipfile);
    ziperr(ZE_TEST, "original files unmodified");
  }
  if (noisy) {
    fprintf(mesg, "test of %s OK\n", zipfile);
    fflush(mesg);
  }
  if (logfile) {
    fprintf(logfile, "test of %s OK\n", zipfile);
    fflush(logfile);
  }
}
#endif /* !WINDLL */

/* get_filters() is replaced by the following
local int get_filters(argc, argv)
*/

/* The filter patterns for options -x, -i, and -R are
   returned by get_option() one at a time, so use a linked
   list to store until all args are processed.  Then convert
   to array for processing.
 */

/* add a filter to the linked list */
local int add_filter(flag, pattern)
  int flag;
  char *pattern;
{
  char *iname, *p = NULL;
  FILE *fp;
  struct filterlist_struct *filter = NULL;

  /* should never happen */
  if (flag != 'R' && flag != 'x' && flag != 'i') {
    ZIPERR(ZE_LOGIC, "bad flag to add_filter");
  }
  if (pattern == NULL) {
    ZIPERR(ZE_LOGIC, "null pattern to add_filter");
  }

  if (pattern[0] == '@') {
    /* read file with 1 pattern per line */
    if (pattern[1] == '\0') {
      ZIPERR(ZE_PARMS, "missing file after @");
    }
    fp = fopen(pattern + 1, "r");
    if (fp == NULL) {
      sprintf(errbuf, "%c pattern file '%s'", flag, pattern);
      ZIPERR(ZE_OPEN, errbuf);
    }
    while ((p = getnam(fp)) != NULL) {
      if ((filter = (struct filterlist_struct *) malloc(sizeof(struct filterlist_struct))) == NULL) {
        ZIPERR(ZE_MEM, "adding filter");
      }
      if (filterlist == NULL) {
        /* first filter */
        filterlist = filter;         /* start of list */
        lastfilter = filter;
      } else {
        lastfilter->next = filter;   /* link to last filter in list */
        lastfilter = filter;
      }
      iname = ex2in(p, 0, (int *)NULL);
      free(p);
      if (iname != NULL) {
        lastfilter->pattern = in2ex(iname);
        free(iname);
      } else {
        lastfilter->pattern = NULL;
      }
      lastfilter->flag = flag;
      pcount++;
      lastfilter->next = NULL;
    }
    fclose(fp);
  } else {
    /* single pattern */
    if ((filter = (struct filterlist_struct *) malloc(sizeof(struct filterlist_struct))) == NULL) {
      ZIPERR(ZE_MEM, "adding filter");
    }
    if (filterlist == NULL) {
      /* first pattern */
      filterlist = filter;         /* start of list */
      lastfilter = filter;
    } else {
      lastfilter->next = filter;   /* link to last filter in list */
      lastfilter = filter;
    }
    iname = ex2in(pattern, 0, (int *)NULL);
    if (iname != NULL) {
       lastfilter->pattern = in2ex(iname);
       free(iname);
    } else {
      lastfilter->pattern = NULL;
    }
    lastfilter->flag = flag;
    pcount++;
    lastfilter->next = NULL;
  }

  return pcount;
}

/* convert list to patterns array */
local int filterlist_to_patterns()
{
  unsigned i;
  struct filterlist_struct *next = NULL;

  if (pcount == 0) {
    patterns = NULL;
    return 0;
  }
  if ((patterns = (struct plist *) malloc((pcount + 1) * sizeof(struct plist)))
      == NULL) {
    ZIPERR(ZE_MEM, "was creating pattern list");
  }

  for (i = 0; i < pcount && filterlist != NULL; i++) {
    switch (filterlist->flag) {
      case 'i':
        icount++;
        break;
      case 'R':
        Rcount++;
        break;
    }
    patterns[i].select = filterlist->flag;
    patterns[i].zname = filterlist->pattern;
    next = filterlist->next;
    free(filterlist);
    filterlist = next;
  }

  return pcount;
}


/* add a file argument to linked list */
local long add_name(filearg)
  char *filearg;
{
  char *name = NULL;
  struct filelist_struct *fileentry = NULL;

  if ((fileentry = (struct filelist_struct *) malloc(sizeof(struct filelist_struct))) == NULL) {
    ZIPERR(ZE_MEM, "adding file");
  }
  if ((name = malloc(strlen(filearg) + 1)) == NULL) {
    ZIPERR(ZE_MEM, "adding file");
  }
  strcpy(name, filearg);
  fileentry->next = NULL;
  fileentry->name = name;
  if (filelist == NULL) {
    /* first file argument */
    filelist = fileentry;         /* start of list */
    lastfile = fileentry;
  } else {
    lastfile->next = fileentry;   /* link to last filter in list */
    lastfile = fileentry;
  }
  filearg_count++;

  return filearg_count;
}


/* Running Stats
   10/30/04 */

local int DisplayRunningStats()
{
  char tempstrg[100];

  if (mesg_line_started) {
    fprintf(mesg, "\n");
    mesg_line_started = 0;
  }
  if (logfile_line_started) {
    fprintf(logfile, "\n");
    logfile_line_started = 0;
  }
  if (display_volume) {
    if (noisy) {
      fprintf(mesg, "%lu>%lu: ", current_in_disk + 1, current_disk + 1);
      mesg_line_started = 1;
    }
    if (logall) {
      fprintf(logfile, "%lu>%lu: ", current_in_disk + 1, current_disk + 1);
      logfile_line_started = 1;
    }
  }
  if (display_counts) {
    if (noisy) {
      fprintf(mesg, "%3ld/%3ld ", files_so_far, files_total - files_so_far);
      mesg_line_started = 1;
    }
    if (logall) {
      fprintf(logfile, "%3ld/%3ld ", files_so_far, files_total - files_so_far);
      logfile_line_started = 1;
    }
  }
  if (display_bytes) {
    /* since file sizes can change as we go, use bytes_so_far from
       initial scan so all adds up */
    WriteNumString(bytes_so_far, tempstrg);
    if (noisy) {
      fprintf(mesg, "[%4s", tempstrg);
      mesg_line_started = 1;
    }
    if (logall) {
      fprintf(logfile, "[%4s", tempstrg);
      logfile_line_started = 1;
    }
    if (bytes_total >= bytes_so_far) {
      WriteNumString(bytes_total - bytes_so_far, tempstrg);
      if (noisy)
        fprintf(mesg, "/%4s] ", tempstrg);
      if (logall)
        fprintf(logfile, "/%4s] ", tempstrg);
    } else {
      WriteNumString(bytes_so_far - bytes_total, tempstrg);
      if (noisy)
        fprintf(mesg, "-%4s] ", tempstrg);
      if (logall)
        fprintf(logfile, "-%4s] ", tempstrg);
    }
  }
  if (noisy)
      fflush(mesg);
  if (logall)
      fflush(logfile);

  return 0;
}

local int BlankRunningStats()
{
  if (display_volume) {
    if (noisy) {
      fprintf(mesg, "%lu>%lu: ", current_in_disk + 1, current_disk + 1);
      mesg_line_started = 1;
    }
    if (logall) {
      fprintf(logfile, "%lu>%lu: ", current_in_disk + 1, current_disk + 1);
      logfile_line_started = 1;
    }
  }
  if (display_counts) {
    if (noisy) {
      fprintf(mesg, "   /    ");
      mesg_line_started = 1;
    }
    if (logall) {
      fprintf(logfile, "   /    ");
      logfile_line_started = 1;
    }
  }
  if (display_bytes) {
    if (noisy) {
      fprintf(mesg, "     /      ");
      mesg_line_started = 1;
    }
    if (logall) {
      fprintf(logfile, "     /      ");
      logfile_line_started = 1;
    }
  }
  if (noisy)
      fflush(mesg);
  if (logall)
      fflush(logfile);

  return 0;
}

#if CRYPT
#ifndef WINDLL
int encr_passwd(modeflag, pwbuf, size, zfn)
int modeflag;
char *pwbuf;
int size;
ZCONST char *zfn;
{
    char *prompt;

    /* Tell picky compilers to shut up about unused variables */
    zfn = zfn;

    prompt = (modeflag == ZP_PW_VERIFY) ?
              "Verify password: " : "Enter password: ";

    if (getp(prompt, pwbuf, size) == NULL) {
      ziperr(ZE_PARMS, "stderr is not a tty");
    }
    return IZ_PW_ENTERED;
}
#endif /* !WINDLL */
#else /* !CRYPT */
int encr_passwd(modeflag, pwbuf, size, zfn)
int modeflag;
char *pwbuf;
int size;
ZCONST char *zfn;
{
    /* Tell picky compilers to shut up about unused variables */
    modeflag = modeflag; pwbuf = pwbuf; size = size; zfn = zfn;

    return ZE_LOGIC;    /* This function should never be called! */
}
#endif /* CRYPT */


/* rename a split
 * A split has a tempfile name until it is closed, then
 * here rename it as out_path the final name for the split.
 */
int rename_split(temp_name, out_path)
  char *temp_name;
  char *out_path;
{
  int r;
  /* Replace old zip file with new zip file, leaving only the new one */
  if ((r = replace(out_path, temp_name)) != ZE_OK)
  {
    zipwarn("new zip file left as: ", temp_name);
    free((zvoid *)tempzip);
    tempzip = NULL;
    ZIPERR(r, "was replacing split file");
  }
  if (zip_attributes) {
    setfileattr(out_path, zip_attributes);
  }
  return ZE_OK;
}


int set_filetype(out_path)
  char *out_path;
{
#ifdef __BEOS__
  /* Set the filetype of the zipfile to "application/zip" */
  setfiletype( out_path, "application/zip" );
#endif

#ifdef __ATHEOS__
  /* Set the filetype of the zipfile to "application/x-zip" */
  setfiletype(out_path, "application/x-zip");
#endif

#ifdef MACOS
  /* Set the Creator/Type of the zipfile to 'IZip' and 'ZIP ' */
  setfiletype(out_path, 'IZip', 'ZIP ');
#endif

#ifdef RISCOS
  /* Set the filetype of the zipfile to &DDC */
  setfiletype(out_path, 0xDDC);
#endif
  return ZE_OK;
}


/*
  -------------------------------------------------------
  Command Line Options
  -------------------------------------------------------

  Valid command line options.

  The function get_option() uses this table to check if an
  option is valid and if it takes a value (also called an
  option argument).  To add an option to zip just add it
  to this table and add a case in the main switch to handle
  it.  If either shortopt or longopt not used set to "".

   The fields:
       shortopt     - short option name (1 or 2 chars)
       longopt      - long option name
       value_type   - see zip.h for constants
       negatable    - option is negatable with trailing -
       ID           - unsigned long int returned for option
       name         - short description of option which is
                        returned on some errors and when options
                        are listed with -so option, can be NULL
*/

/* Most option IDs are set to the shortopt char.  For
   multichar short options set to arbitrary unused constant. */
#define o_AC            0x101
#define o_AS            0x102
#define o_C2            0x103
#define o_C5            0x104
#define o_db            0x105
#define o_dc            0x106
#define o_dd            0x107
#define o_des           0x108
#define o_df            0x109
#define o_DF            0x110
#define o_dg            0x111
#define o_ds            0x112
#define o_du            0x113
#define o_dv            0x114
#define o_FF            0x115
#define o_FI            0x116
#define o_FS            0x117
#define o_h2            0x118
#define o_ic            0x119
#define o_jj            0x120
#define o_la            0x121
#define o_lf            0x122
#define o_li            0x123
#define o_ll            0x124
#define o_mm            0x125
#define o_MM            0x126
#define o_nw            0x127
#define o_RE            0x128
#define o_sb            0x129
#define o_sc            0x130
#define o_sd            0x131
#define o_sf            0x132
#define o_so            0x133
#define o_sp            0x134
#define o_su            0x135
#define o_sU            0x136
#define o_sv            0x137
#define o_tt            0x138
#define o_TT            0x139
#define o_UN            0x140
#define o_ve            0x141
#define o_VV            0x142
#define o_ws            0x143
#define o_ww            0x144
#define o_z64           0x145
#ifdef UNICODE_TEST
#define o_sC            0x146
#endif


/* the below is mainly from the old main command line
   switch with a few changes */
struct option_struct far options[] = {
  /* short longopt        value_type        negatable        ID    name */
#ifdef EBCDIC
    {"a",  "ascii",       o_NO_VALUE,       o_NOT_NEGATABLE, 'a',  "to ascii"},
#endif /* EBCDIC */
#ifdef CMS_MVS
    {"B",  "binary",      o_NO_VALUE,       o_NOT_NEGATABLE, 'B',  "binary"},
#endif /* CMS_MVS */
#ifdef TANDEM
    {"B",  "",            o_NUMBER_VALUE,   o_NOT_NEGATABLE, 'B',  "nsk"},
#endif
    {"0",  "store",       o_NO_VALUE,       o_NOT_NEGATABLE, '0',  "store"},
    {"1",  "compress-1",  o_NO_VALUE,       o_NOT_NEGATABLE, '1',  "compress 1"},
    {"2",  "compress-2",  o_NO_VALUE,       o_NOT_NEGATABLE, '2',  "compress 2"},
    {"3",  "compress-3",  o_NO_VALUE,       o_NOT_NEGATABLE, '3',  "compress 3"},
    {"4",  "compress-4",  o_NO_VALUE,       o_NOT_NEGATABLE, '4',  "compress 4"},
    {"5",  "compress-5",  o_NO_VALUE,       o_NOT_NEGATABLE, '5',  "compress 5"},
    {"6",  "compress-6",  o_NO_VALUE,       o_NOT_NEGATABLE, '6',  "compress 6"},
    {"7",  "compress-7",  o_NO_VALUE,       o_NOT_NEGATABLE, '7',  "compress 7"},
    {"8",  "compress-8",  o_NO_VALUE,       o_NOT_NEGATABLE, '8',  "compress 8"},
    {"9",  "compress-9",  o_NO_VALUE,       o_NOT_NEGATABLE, '9',  "compress 9"},
    {"A",  "adjust-sfx",  o_NO_VALUE,       o_NOT_NEGATABLE, 'A',  "adjust self extractor offsets"},
#if defined(WIN32)
    {"AC", "archive-clear", o_NO_VALUE,     o_NOT_NEGATABLE, o_AC, "clear DOS archive bit of included files"},
    {"AS", "archive-set", o_NO_VALUE,       o_NOT_NEGATABLE, o_AS, "include only files with archive bit set"},
#endif
    {"b",  "temp-path",   o_REQUIRED_VALUE, o_NOT_NEGATABLE, 'b',  "dir to use for temp archive"},
    {"c",  "entry-comments", o_NO_VALUE,    o_NOT_NEGATABLE, 'c',  "add comments for each entry"},
#ifdef VMS
    {"C",  "preserve-case", o_NO_VALUE,     o_NEGATABLE,     'C',  "Preserve (C-: down-) case all on VMS"},
    {"C2", "preserve-case-2", o_NO_VALUE,   o_NEGATABLE,     o_C2, "Preserve (C2-: down-) case ODS2 on VMS"},
    {"C5", "preserve-case-5", o_NO_VALUE,   o_NEGATABLE,     o_C5, "Preserve (C5-: down-) case ODS5 on VMS"},
#endif /* VMS */
    {"d",  "delete",      o_NO_VALUE,       o_NOT_NEGATABLE, 'd',  "delete entries from archive"},
    {"db", "display-bytes", o_NO_VALUE,     o_NEGATABLE,     o_db, "display running bytes"},
    {"dc", "display-counts", o_NO_VALUE,    o_NEGATABLE,     o_dc, "display running file count"},
    {"dd", "display-dots", o_NO_VALUE,      o_NEGATABLE,     o_dd, "display dots as process each file"},
    {"dg", "display-globaldots",o_NO_VALUE, o_NEGATABLE,     o_dg, "display dots for archive instead of files"},
    {"ds", "dot-size",     o_REQUIRED_VALUE, o_NOT_NEGATABLE, o_ds, "set progress dot size - default 10M bytes"},
    {"du", "display-usize", o_NO_VALUE,     o_NEGATABLE,     o_du, "display uncompressed size in bytes"},
    {"dv", "display-volume", o_NO_VALUE,    o_NEGATABLE,     o_dv, "display volume (disk) number"},
#ifdef MACOS
    {"df", "datafork",    o_NO_VALUE,       o_NOT_NEGATABLE, o_df, "save datafork"},
#endif /* MACOS */
    {"D",  "no-dir-entries", o_NO_VALUE,    o_NOT_NEGATABLE, 'D',  "no entries for dirs themselves (-x */)"},
    {"DF", "difference-archive",o_NO_VALUE, o_NOT_NEGATABLE, o_DF, "create diff archive with changed/new files"},
    {"e",  "encrypt",     o_NO_VALUE,       o_NOT_NEGATABLE, 'e',  "encrypt entries, ask for password"},
#ifdef OS2
    {"E",  "longnames",   o_NO_VALUE,       o_NOT_NEGATABLE, 'E',  "use OS2 longnames"},
#endif
    {"F",  "fix",         o_NO_VALUE,       o_NOT_NEGATABLE, 'F',  "fix mostly intact archive (try first)"},
    {"FF", "fixfix",      o_NO_VALUE,       o_NOT_NEGATABLE, o_FF, "try harder to fix archive (not as reliable)"},
    {"FI", "fifo",        o_NO_VALUE,       o_NEGATABLE,     o_FI, "read Unix FIFO (zip will wait on open pipe)"},
    {"FS", "filesync",    o_NO_VALUE,       o_NOT_NEGATABLE, o_FS, "add/delete entries to make archive match OS"},
    {"f",  "freshen",     o_NO_VALUE,       o_NOT_NEGATABLE, 'f',  "freshen existing archive entries"},
    {"fd", "force-descriptors", o_NO_VALUE, o_NOT_NEGATABLE, o_des,"force data descriptors as if streaming"},
#ifdef ZIP64_SUPPORT
    {"fz", "force-zip64", o_NO_VALUE,       o_NEGATABLE,     o_z64,"force use of Zip64 format, negate prevents"},
#endif
    {"g",  "grow",        o_NO_VALUE,       o_NOT_NEGATABLE, 'g',  "grow existing archive instead of replace"},
#ifndef WINDLL
    {"h",  "help",        o_NO_VALUE,       o_NOT_NEGATABLE, 'h',  "help"},
    {"H",  "",            o_NO_VALUE,       o_NOT_NEGATABLE, 'h',  "help"},
    {"?",  "",            o_NO_VALUE,       o_NOT_NEGATABLE, 'h',  "help"},
    {"h2", "more-help",   o_NO_VALUE,       o_NOT_NEGATABLE, o_h2, "extended help"},
#endif /* !WINDLL */
    {"i",  "include",     o_VALUE_LIST,     o_NOT_NEGATABLE, 'i',  "include only files matching patterns"},
#if defined(VMS) || defined(WIN32)
    {"ic", "ignore-case", o_NO_VALUE,       o_NEGATABLE,     o_ic, "ignore case when matching archive entries"},
#endif
#ifdef RISCOS
    {"I",  "no-image",    o_NO_VALUE,       o_NOT_NEGATABLE, 'I',  "no image"},
#endif
    {"j",  "junk-paths",  o_NO_VALUE,       o_NOT_NEGATABLE, 'j',  "strip paths and just store file names"},
#ifdef MACOS
    {"jj", "absolute-path", o_NO_VALUE,     o_NOT_NEGATABLE, o_jj, "MAC absolute path"},
#endif /* ?MACOS */
    {"J",  "junk-sfx",    o_NO_VALUE,       o_NOT_NEGATABLE, 'J',  "strip self extractor from archive"},
    {"k",  "DOS-names",   o_NO_VALUE,       o_NOT_NEGATABLE, 'k',  "force use of 8.3 DOS names"},
    {"l",  "to-crlf",     o_NO_VALUE,       o_NOT_NEGATABLE, 'l',  "convert text file line ends - LF->CRLF"},
    {"ll", "from-crlf",   o_NO_VALUE,       o_NOT_NEGATABLE, o_ll, "convert text file line ends - CRLF->LF"},
    {"lf", "logfile-path",o_REQUIRED_VALUE, o_NOT_NEGATABLE, o_lf, "log to log file at path (default overwrite)"},
    {"la", "log-append",  o_NO_VALUE,       o_NEGATABLE,     o_la, "append to existing log file"},
    {"li", "log-info",    o_NO_VALUE,       o_NEGATABLE,     o_li, "include informational messages in log"},
#ifndef WINDLL
    {"L",  "license",     o_NO_VALUE,       o_NOT_NEGATABLE, 'L',  "display license"},
#endif
    {"m",  "move",        o_NO_VALUE,       o_NOT_NEGATABLE, 'm',  "add files to archive then delete files"},
    {"mm", "",            o_NO_VALUE,       o_NOT_NEGATABLE, o_mm, "not used"},
    {"MM", "must-match",  o_NO_VALUE,       o_NOT_NEGATABLE, o_MM, "error if in file not matched/not readable"},
    {"n",  "suffixes",    o_REQUIRED_VALUE, o_NOT_NEGATABLE, 'n',  "suffixes to not compress: .gz:.zip"},
    {"nw", "no-wild",     o_NO_VALUE,       o_NOT_NEGATABLE, o_nw, "no wildcards during add or update"},
#if defined(AMIGA) || defined(MACOS)
    {"N",  "notes",       o_NO_VALUE,       o_NOT_NEGATABLE, 'N',  "add notes as entry comments"},
#endif
    {"o",  "latest-time", o_NO_VALUE,       o_NOT_NEGATABLE, 'o',  "use latest entry time as archive time"},
    {"O",  "output-file", o_REQUIRED_VALUE, o_NOT_NEGATABLE, 'O',  "set out zipfile different than in zipfile"},
    {"p",  "paths",       o_NO_VALUE,       o_NOT_NEGATABLE, 'p',  "store paths"},
    {"P",  "password",    o_REQUIRED_VALUE, o_NOT_NEGATABLE, 'P',  "encrypt entries, option value is password"},
#if defined(QDOS) || defined(QLZIP)
    {"Q",  "Q-flag",      o_NUMBER_VALUE,   o_NOT_NEGATABLE, 'Q',  "Q flag"},
#endif
    {"q",  "quiet",       o_NO_VALUE,       o_NOT_NEGATABLE, 'q',  "quiet"},
    {"r",  "recurse-paths", o_NO_VALUE,     o_NOT_NEGATABLE, 'r',  "recurse down listed paths"},
    {"R",  "recurse-patterns", o_NO_VALUE,  o_NOT_NEGATABLE, 'R',  "recurse current dir and match patterns"},
    {"RE", "regex",       o_NO_VALUE,       o_NOT_NEGATABLE, o_RE, "allow [list] matching (regex)"},
    {"s",  "split-size",  o_REQUIRED_VALUE, o_NOT_NEGATABLE, 's',  "do splits, set split size (-s=0 no splits)"},
    {"sp", "split-pause", o_NO_VALUE,       o_NOT_NEGATABLE, o_sp, "pause while splitting to select destination"},
    {"sv", "split-verbose", o_NO_VALUE,     o_NOT_NEGATABLE, o_sv, "be verbose about creating splits"},
    {"sb", "split-bell",  o_NO_VALUE,       o_NOT_NEGATABLE, o_sb, "when pause for next split ring bell"},
    {"sc", "show-command",o_NO_VALUE,       o_NOT_NEGATABLE, o_sc, "show command line"},
#ifdef UNICODE_TEST
    {"sC", "create-files",o_NO_VALUE,       o_NOT_NEGATABLE, o_sC, "create empty files using archive names"},
#endif
    {"sd", "show-debug",  o_NO_VALUE,       o_NOT_NEGATABLE, o_sd, "show debug"},
    {"sf", "show-files",  o_NO_VALUE,       o_NEGATABLE,     o_sf, "show files to operate on and exit"},
    {"so", "show-options",o_NO_VALUE,       o_NOT_NEGATABLE, o_so, "show options"},
#ifdef UNICODE_SUPPORT
    {"su", "show-unicode", o_NO_VALUE,      o_NEGATABLE,     o_su, "as -sf but also show escaped Unicode"},
    {"sU", "show-just-unicode", o_NO_VALUE, o_NEGATABLE,     o_sU, "as -sf but only show escaped Unicode"},
#endif
#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(ATARI)
    {"S",  "",            o_NO_VALUE,       o_NOT_NEGATABLE, 'S',  "include system and hidden"},
#endif /* MSDOS || OS2 || WIN32 || ATARI */
    {"t",  "from-date",   o_REQUIRED_VALUE, o_NOT_NEGATABLE, 't',  "exclude before date"},
    {"tt", "before-date", o_REQUIRED_VALUE, o_NOT_NEGATABLE, o_tt, "include before date"},
    {"T",  "test",        o_NO_VALUE,       o_NOT_NEGATABLE, 'T',  "test updates before replacing archive"},
    {"TT", "unzip-command", o_REQUIRED_VALUE,o_NOT_NEGATABLE,o_TT, "unzip command to use, name is added to end"},
    {"u",  "update",      o_NO_VALUE,       o_NOT_NEGATABLE, 'u',  "update existing entries and add new"},
    {"U",  "copy-entries", o_NO_VALUE,      o_NOT_NEGATABLE, 'U',  "select from archive instead of file system"},
#ifdef UNICODE_SUPPORT
    {"UN", "unicode",     o_REQUIRED_VALUE, o_NOT_NEGATABLE, o_UN, "UN=quit, warn, ignore, no, escape"},
#endif
    {"v",  "verbose",     o_NO_VALUE,       o_NOT_NEGATABLE, 'v',  "display additional information"},
    {"",   "version",     o_NO_VALUE,       o_NOT_NEGATABLE, o_ve, "(if no other args) show version information"},
#ifdef VMS
    {"V",  "VMS-portable", o_NO_VALUE,      o_NOT_NEGATABLE, 'V',  "Store VMS attributes, portable file format"},
    {"VV", "VMS-specific", o_NO_VALUE,      o_NOT_NEGATABLE, o_VV, "Store VMS attributes, VMS specific format"},
    {"w",  "VMS-versions", o_NO_VALUE,      o_NOT_NEGATABLE, 'w',  "store VMS versions"},
    {"ww", "VMS-dot-versions", o_NO_VALUE,  o_NOT_NEGATABLE, o_ww, "store VMS versions as \".nnn\""},
#endif /* VMS */
    {"ws", "wild-stop-dirs", o_NO_VALUE,    o_NOT_NEGATABLE, o_ws,  "* stops at /, ** includes any /"},
    {"x",  "exclude",     o_VALUE_LIST,     o_NOT_NEGATABLE, 'x',  "exclude files matching patterns"},
/*    {"X",  "no-extra",    o_NO_VALUE,       o_NOT_NEGATABLE, 'X',  "no extra"},
*/
    {"X",  "strip-extra", o_NO_VALUE,       o_NEGATABLE,     'X',  "-X- keep all ef, -X strip but critical ef"},
#ifdef S_IFLNK
    {"y",  "symlinks",    o_NO_VALUE,       o_NOT_NEGATABLE, 'y',  "store symbolic links"},
#endif /* S_IFLNK */
    {"z",  "archive-comment", o_NO_VALUE,   o_NOT_NEGATABLE, 'z',  "ask for archive comment"},
    {"Z",  "compression-method", o_REQUIRED_VALUE, o_NOT_NEGATABLE, 'Z', "compression method"},
#if defined(MSDOS) || defined(OS2)
    {"$",  "volume-label", o_NO_VALUE,      o_NOT_NEGATABLE, '$',  "store volume label"},
#endif
#ifndef MACOS
    {"@",  "names-stdin", o_NO_VALUE,       o_NOT_NEGATABLE, '@',  "get file names from stdin, one per line"},
#endif /* !MACOS */
#ifdef NTSD_EAS
    {"!",  "use-privileges", o_NO_VALUE,    o_NOT_NEGATABLE, '!',  "use privileges"},
#endif
#ifdef RISCOS
    {"/",  "exts-to-swap", o_REQUIRED_VALUE, o_NOT_NEGATABLE, '/',  "override Zip$Exts"},
#endif
    /* the end of the list */
    {NULL, NULL,          o_NO_VALUE,       o_NOT_NEGATABLE, 0,    NULL} /* end has option_ID = 0 */
  };



#ifndef USE_ZIPMAIN
int main(argc, argv)
#else
int zipmain(argc, argv)
#endif
int argc;               /* number of tokens in command line */
char **argv;            /* command line tokens */
/* Add, update, freshen, or delete zip entries in a zip file.  See the
   command help in help() above. */
{
  int d;                /* true if just adding to a zip file */
  char *e;              /* malloc'd comment buffer */
  struct flist far *f;  /* steps through found linked list */
  int i;                /* arg counter, root directory flag */
  int kk;               /* next arg type (formerly another re-use of "k") */

  /* zip64 support 09/05/2003 R.Nausedat */
  uzoff_t c;            /* start of central directory */
  uzoff_t t;            /* length of central directory */
  zoff_t k;             /* marked counter, comment size, entry count */
  uzoff_t n;            /* total of entry len's */

  int o;                /* true if there were any ZE_OPEN errors */
  char *p;              /* steps through option arguments */
  char *pp;             /* temporary pointer */
  int r;                /* temporary variable */
  int s;                /* flag to read names from stdin */
  uzoff_t csize;        /* compressed file size for stats */
  uzoff_t usize;        /* uncompressed file size for stats */
  ulg tf;               /* file time */
  int first_listarg = 0;/* index of first arg of "process these files" list */
  struct zlist far *v;  /* temporary variable */
  struct zlist far * far *w;    /* pointer to last link in zfiles list */
  FILE *x /*, *y */;    /* input and output zip files (y global) */
  struct zlist far *z;  /* steps through zfiles linked list */
  int bad_open_is_error = 0; /* if read fails, 0=warning, 1=error */
#if 0
  /* does not seem used */
#ifdef WINDLL
  int retcode;          /* return code for dll */
#endif /* WINDLL */
#endif
#if (!defined(VMS) && !defined(CMS_MVS))
  char *zipbuf;         /* stdio buffer for the zip file */
#endif /* !VMS && !CMS_MVS */
  FILE *comment_stream; /* set to stderr if anything is read from stdin */
  int all_current;      /* used by File Sync to determine if all entries are current */

  struct filelist_struct *filearg;

/* used by get_option */
  unsigned long option; /* option ID returned by get_option */
  int argcnt = 0;       /* current argcnt in args */
  int argnum = 0;       /* arg number */
  int optchar = 0;      /* option state */
  char *value = NULL;   /* non-option arg, option value or NULL */
  int negated = 0;      /* 1 = option negated */
  int fna = 0;          /* current first non-opt arg */
  int optnum = 0;       /* index in table */

  int show_options = 0; /* show options */
  int show_what_doing = 0; /* show what doing */
  int show_args = 0;    /* show command line */
  int seen_doubledash = 0; /* seen -- argument */
  int key_needed = 0;   /* prompt for encryption key */
  int have_out = 0;     /* if set in_path and out_path different archive */
#ifdef UNICODE_TEST
  int create_files = 0;
#endif

  char **args = NULL;  /* could be wide argv */


#ifdef THEOS
  /* the argument expansion from the standard library is full of bugs */
  /* use mine instead */
  _setargv(&argc, &argv);
  setlocale(LC_CTYPE, "I");
#else
  SETLOCALE(LC_CTYPE, "");
#endif

#ifdef UNICODE_SUPPORT
# ifdef UNIX
  /* For Unix, set the locale to UTF-8.  Any UTF-8 locale is
     OK and they should all be the same.  This allows seeing,
     writing, and displaying (if the fonts are loaded) all
     characters in UTF-8. */
  {
    char *loc;

    /*
      loc = setlocale(LC_CTYPE, NULL);
      printf("  Initial language locale = '%s'\n", loc);
    */

    loc = setlocale(LC_CTYPE, "en_US.UTF-8");

    /*
      printf("langinfo %s\n", nl_langinfo(CODESET));
    */

    if (loc != NULL) {
      /* using UTF-8 character set so can set UTF-8 GPBF bit 11 */
      using_utf8 = 1;
      /*
        printf("  Locale set to %s\n", loc);
      */
    } else {
      /*
        printf("  Could not set Unicode UTF-8 locale\n");
      */
    }
  }
# endif
#endif

#if defined(__IBMC__) && defined(__DEBUG_ALLOC__)
  {
    extern void DebugMalloc(void);
    atexit(DebugMalloc);
  }
#endif

#ifdef QDOS
  {
    extern void QDOSexit(void);
    atexit(QDOSexit);
  }
#endif

#ifdef NLM
  {
    extern void NLMexit(void);
    atexit(NLMexit);
  }
#endif

#ifdef RISCOS
  set_prefix();
#endif

#ifdef __human68k__
  fflush(stderr);
  setbuf(stderr, NULL);
#endif

/* Re-initialize global variables to make the zip dll re-entrant. It is
 * possible that we could get away with not re-initializing all of these
 * but better safe than sorry.
 */
#if defined(MACOS) || defined(WINDLL) || defined(USE_ZIPMAIN)
  action = ADD; /* one of ADD, UPDATE, FRESHEN, DELETE, or ARCHIVE */
  comadd = 0;   /* 1=add comments for new files */
  zipedit = 0;  /* 1=edit zip comment and all file comments */
  latest = 0;   /* 1=set zip file time to time of latest file */
  before = 0;   /* 0=ignore, else exclude files before this time */
  after = 0;    /* 0=ignore, else exclude files newer than this time */
  test = 0;     /* 1=test zip file with unzip -t */
  unzip_path = NULL; /* where to look for unzip command path */
  tempdir = 0;  /* 1=use temp directory (-b) */
  junk_sfx = 0; /* 1=junk the sfx prefix */
#if defined(AMIGA) || defined(MACOS)
  filenotes = 0;/* 1=take comments from AmigaDOS/MACOS filenotes */
#endif
#ifndef USE_ZIPMAIN
  zipstate = -1;
#endif
  tempzip = NULL;
  fcount = 0;
  recurse = 0;         /* 1=recurse into directories; 2=match filenames */
  dispose = 0;         /* 1=remove files after put in zip file */
  pathput = 1;         /* 1=store path with name */
  method = BEST;       /* one of BEST, DEFLATE (only), or STORE (only) */
  dosify = 0;          /* 1=make new entries look like MSDOS */
  verbose = 0;         /* 1=report oddities in zip file structure */
  fix = 0;             /* 1=fix the zip file */
  adjust = 0;          /* 1=adjust offsets for sfx'd file (keep preamble) */
  level = 6;           /* 0=fastest compression, 9=best compression */
  translate_eol = 0;   /* Translate end-of-line LF -> CR LF */
#if defined(OS2) || defined(WIN32)
  use_longname_ea = 0; /* 1=use the .LONGNAME EA as the file's name */
#endif
#ifdef NTSD_EAS
  use_privileges = 0;     /* 1=use security privileges overrides */
#endif
  no_wild = 0;            /* 1 = wildcards are disabled */
#ifdef WILD_STOP_AT_DIR
   wild_stop_at_dir = 1;  /* default wildcards do not include / in matches */
#else
   wild_stop_at_dir = 0;  /* default wildcards do include / in matches */
#endif

  skip_this_disk = 0;
  des_good = 0;           /* Good data descriptor found */
  des_crc = 0;            /* Data descriptor CRC */
  des_csize = 0;          /* Data descriptor csize */
  des_usize = 0;          /* Data descriptor usize */

  dot_size = 0;           /* buffers processed in deflate per dot, 0 = no dots */
  dot_count = 0;          /* buffers seen, recyles at dot_size */

  display_counts = 0;     /* display running file count */
  display_bytes = 0;      /* display running bytes remaining */
  display_globaldots = 0; /* display dots for archive instead of each file */
  display_volume = 0;     /* display current input and output volume (disk) numbers */
  display_usize = 0;      /* display uncompressed bytes */

  files_so_far = 0;       /* files processed so far */
  bad_files_so_far = 0;   /* bad files skipped so far */
  files_total = 0;        /* files total to process */
  bytes_so_far = 0;       /* bytes processed so far (from initial scan) */
  good_bytes_so_far = 0;  /* good bytes read so far */
  bad_bytes_so_far = 0;   /* bad bytes skipped so far */
  bytes_total = 0;        /* total bytes to process (from initial scan) */

  logall = 0;             /* 0 = warnings/errors, 1 = all */
  logfile = NULL;         /* pointer to open logfile or NULL */
  logfile_append = 0;     /* append to existing logfile */
  logfile_path = NULL;    /* pointer to path of logfile */

  hidden_files = 0;       /* process hidden and system files */
  volume_label = 0;       /* add volume label */
  dirnames = 1;           /* include directory entries by default */
#if defined(WIN32)
  only_archive_set = 0;   /* only include if DOS archive bit set */
  clear_archive_bits = 0; /* clear DOS archive bit of included files */
#endif
  linkput = 0;            /* 1=store symbolic links as such */
  noisy = 1;              /* 0=quiet operation */
  extra_fields = 1;       /* 0=create minimum, 1=don't copy old, 2=keep old */

  use_descriptors = 0;    /* 1=use data descriptors 12/29/04 */
  zip_to_stdout = 0;      /* output zipfile to stdout 12/30/04 */
  allow_empty_archive = 0;/* if no files, create empty archive anyway 12/28/05 */
  copy_only = 0;          /* 1=copying archive entries only */

  output_seekable = 1;    /* 1 = output seekable 3/13/05 EG */

#ifdef ZIP64_SUPPORT      /* zip64 support 10/4/03 */
  force_zip64 = -1;       /* if 1 force entries to be zip64 */
                          /* mainly for streaming from stdin */
  zip64_entry = 0;        /* current entry needs Zip64 */
  zip64_archive = 0;      /* if 1 then at least 1 entry needs zip64 */
#endif

#ifdef UNICODE_SUPPORT
  utf8_force = 0;         /* 1=force storing UTF-8 as standard per AppNote bit 11 */
#endif

  unicode_escape_all = 0; /* 1=escape all non-ASCII characters in paths */
  unicode_mismatch = 1;   /* unicode mismatch is 0=error, 1=warn, 2=ignore, 3=no */

  scan_delay = 5;         /* seconds before display Scanning files message */
  scan_dot_time = 2;      /* time in seconds between Scanning files dots */
  scan_start = 0;         /* start of scan */
  scan_last = 0;          /* time of last message */
  scan_started = 0;       /* scan has started */
  scan_count = 0;         /* Used for Scanning files ... message */

  before = 0;             /* 0=ignore, else exclude files before this time */
  after = 0;              /* 0=ignore, else exclude files newer than this time */

  special = ".Z:.zip:.zoo:.arc:.lzh:.arj"; /* List of special suffixes */
  key = NULL;             /* Scramble password if scrambling */
  key_needed = 0;         /* Need scramble password */
  tempath = NULL;         /* Path for temporary files */
  patterns = NULL;        /* List of patterns to be matched */
  pcount = 0;             /* number of patterns */
  icount = 0;             /* number of include only patterns */
  Rcount = 0;             /* number of -R include patterns */

  found = NULL;           /* List of names found, or new found entry */
  fnxt = &found;

  /* used by get_option */
  argcnt = 0;             /* size of args */
  argnum = 0;             /* current arg number */
  optchar = 0;            /* option state */
  value = NULL;           /* non-option arg, option value or NULL */
  negated = 0;            /* 1 = option negated */
  fna = 0;                /* current first nonopt arg */
  optnum = 0;             /* option index */

  show_options = 0;       /* 1 = show options */
  show_what_doing = 0;    /* 1 = show what zip doing */
  show_args = 0;          /* 1 = show command line */
  seen_doubledash = 0;    /* seen -- argument */

  zipfile = NULL;         /* path of usual in and out zipfile */
  tempzip = NULL;         /* name of temp file */
  y = NULL;               /* output file now global so can change in splits */
  in_file = NULL;         /* current input file for splits */
  in_split_path = NULL;   /* current in split path */
  in_path = NULL;         /* used by splits to track changing split locations */
  out_path = NULL;        /* if set, use -O out_path as output */
  have_out = 0;           /* if set, in_path and out_path not the same archive */

  total_disks = 0;        /* total disks in archive */
  current_in_disk = 0;    /* current read split disk */
  current_in_offset = 0;  /* current offset in current read disk */
  skip_current_disk = 0;  /* if != 0 and fix then skip entries on this disk */

  zip64_eocd_disk = 0;    /* disk with Zip64 End Of Central Directory Record */
  zip64_eocd_offset = 0;  /* offset for Zip64 EOCD Record */

  current_local_disk = 0; /* disk with current local header */

  current_disk = 0;           /* current disk number */
  cd_start_disk = (ulg)-1;    /* central directory start disk */
  cd_start_offset = 0;        /* offset of start of cd on cd start disk */
  cd_entries_this_disk = 0;   /* cd entries this disk */
  total_cd_entries = 0;       /* total cd entries in new/updated archive */

  /* for split method 1 (keep split with local header open and update) */
  current_local_tempname = NULL; /* name of temp file */
  current_local_file = NULL;  /* file pointer for current local header */
  current_local_offset = 0;   /* offset to start of current local header */

  /* global */
  bytes_this_split = 0;       /* bytes written to the current split */
  read_split_archive = 0;     /* 1=scanzipf_reg detected spanning signature */
  split_method = 0;           /* 0=no splits, 1=update LHs, 2=data descriptors */
  split_size = 0;             /* how big each split should be */
  split_bell = 0;             /* when pause for next split ring bell */
  bytes_prev_splits = 0;      /* total bytes written to all splits before this */
  bytes_this_entry = 0;       /* bytes written for this entry across all splits */
  noisy_splits = 0;           /* be verbose about creating splits */
  mesg_line_started = 0;      /* 1=started writing a line to mesg */
  logfile_line_started = 0;   /* 1=started writing a line to logfile */

  filelist = NULL;
  filearg_count = 0;
  allow_empty_archive = 0;    /* if no files, allow creation of empty archive anyway */
  bad_open_is_error = 0;      /* if read fails, 0=warning, 1=error */
  unicode_mismatch = 0;       /* unicode mismatch is 0=error, 1=warn, 2=ignore, 3=no */
  show_files = 0;             /* show files to operate on and exit */
  scan_delay = 5;             /* seconds before display Scanning files message */
  scan_dot_time = 2;          /* time in seconds between Scanning files dots */
  scan_started = 0;           /* space at start of scan has been displayed */
  scan_last = 0;              /* Time last dot displayed for Scanning files message */
  scan_start = 0;             /* Time scanning started for Scanning files message */
#ifdef UNICODE_SUPPORT
  use_wide_to_mb_default = 0;
#endif
  filter_match_case = 1;      /* default is to match case when matching archive entries */
  allow_fifo = 0;             /* 1=allow reading Unix FIFOs, waiting if pipe open */

#if !defined(MACOS) && !defined(USE_ZIPMAIN)
  retcode = setjmp(zipdll_error_return);
  if (retcode) {
    return retcode;
  }
#endif /* !MACOS */
#endif /* MACOS || WINDLL */

#if !defined(ALLOW_REGEX) && (defined(MSDOS) || defined(WIN32))
  allow_regex = 0;        /* 1 = allow [list] matching (regex) */
#else
  allow_regex = 1;
#endif

  mesg = (FILE *) stdout; /* cannot be made at link time for VMS */
  comment_stream = (FILE *)stdin;

  init_upper();           /* build case map table */

#ifdef LARGE_FILE_SUPPORT
  /* test if we can support large files - 9/29/04 */
  if (sizeof(zoff_t) < 8) {
    ZIPERR(ZE_COMPERR, "LARGE_FILE_SUPPORT enabled but OS not supporting it");
  }
#endif
  /* test if sizes are the same - 12/30/04 */
  if (sizeof(uzoff_t) != sizeof(zoff_t)){
    ZIPERR(ZE_COMPERR, "uzoff_t not same size as zoff_t");
  }

#if (defined(WIN32) && defined(USE_EF_UT_TIME))
  /* For the Win32 environment, we may have to "prepare" the environment
     prior to the tzset() call, to work around tzset() implementation bugs.
   */
  iz_w32_prepareTZenv();
#endif

#if (defined(IZ_CHECK_TZ) && defined(USE_EF_UT_TIME))
#  ifndef VALID_TIMEZONE
#     define VALID_TIMEZONE(tmp) \
             (((tmp = getenv("TZ")) != NULL) && (*tmp != '\0'))
#  endif
  zp_tz_is_valid = VALID_TIMEZONE(p);
#if (defined(AMIGA) || defined(DOS))
  if (!zp_tz_is_valid)
    extra_fields = 0;     /* disable storing "UT" time stamps */
#endif /* AMIGA || DOS */
#endif /* IZ_CHECK_TZ && USE_EF_UT_TIME */

/* For systems that do not have tzset() but supply this function using another
   name (_tzset() or something similar), an appropiate "#define tzset ..."
   should be added to the system specifc configuration section.  */
#if (!defined(TOPS20) && !defined(VMS))
#if (!defined(RISCOS) && !defined(MACOS) && !defined(QDOS))
#if (!defined(BSD) && !defined(MTS) && !defined(CMS_MVS) && !defined(TANDEM))
  tzset();
#endif
#endif
#endif

#ifdef VMSCLI
  {
      ulg status = vms_zip_cmdline(&argc, &argv);
      if (!(status & 1))
            return status;
  }
#endif /* VMSCLI */

  /*    Substitutes the extended command line argument list produced by
   *    the MKS Korn Shell in place of the command line info from DOS.
   */

  /* extract extended argument list from environment */
  expand_args(&argc, &argv);

#ifndef WINDLL
  /* Process arguments */
  diag("processing arguments");
  /* First, check if just the help or version screen should be displayed */
  if (argc == 1 && isatty(1))   /* no arguments, and output screen available */
  {                             /* show help screen */
# ifdef VMSCLI
    VMSCLI_help();
# else
    help();
# endif
    EXIT(ZE_OK);
  }
  /* Check -v here as env arg can change argc.  Handle --version in main switch. */
  else if (argc == 2 && strcmp(argv[1], "-v") == 0 &&
           /* only "-v" as argument, and */
           (isatty(1) || isatty(0)))
           /* stdout or stdin is connected to console device */
  {                             /* show diagnostic version info */
    version_info();
    EXIT(ZE_OK);
  }
# ifndef VMS
#   ifndef RISCOS
  envargs(&argc, &argv, "ZIPOPT", "ZIP");  /* get options from environment */
#   else /* RISCOS */
  envargs(&argc, &argv, "ZIPOPT", "Zip$Options");  /* get options from environment */
  getRISCOSexts("Zip$Exts");        /* get the extensions to swap from environment */
#   endif /* ? RISCOS */
# else /* VMS */
  envargs(&argc, &argv, "ZIPOPT", "ZIP_OPTS");  /* 4th arg for unzip compat. */
# endif /* ?VMS */
#endif /* !WINDLL */

  zipfile = tempzip = NULL;
  y = NULL;
  d = 0;                        /* disallow adding to a zip file */
#if (!defined(MACOS) && !defined(WINDLL) && !defined(NLM))
  signal(SIGINT, handler);
#ifdef SIGTERM                  /* AMIGADOS and others have no SIGTERM */
  signal(SIGTERM, handler);
#endif
# if defined(SIGABRT) && !(defined(AMIGA) && defined(__SASC))
   signal(SIGABRT, handler);
# endif
# ifdef SIGBREAK
   signal(SIGBREAK, handler);
# endif
# ifdef SIGBUS
   signal(SIGBUS, handler);
# endif
# ifdef SIGILL
   signal(SIGILL, handler);
# endif
# ifdef SIGSEGV
   signal(SIGSEGV, handler);
# endif
#endif /* !MACOS && !WINDLL && !NLM */
#ifdef NLM
  NLMsignals();
#endif


#if defined(UNICODE_SUPPORT) && defined(WIN32)
  /* check if this Win32 OS has support for wide character calls */
  has_win32_wide();
#endif

  /* make copy of args that can use with insert_arg() used by get_option() */
  args = copy_args(argv, 0);

  kk = 0;                       /* Next non-option argument type */
  s = 0;                        /* set by -@ */

  /*
  -------------------------------------------
  Process command line using get_option
  -------------------------------------------

  Each call to get_option() returns either a command
  line option and possible value or a non-option argument.
  Arguments are permuted so that all options (-r, -b temp)
  are returned before non-option arguments (zipfile).
  Returns 0 when nothing left to read.
  */

  /* set argnum = 0 on first call to init get_option */
  argnum = 0;

  /* get_option returns the option ID and updates parameters:
         args    - usually same as argv if no argument file support
         argcnt  - current argc for args
         value   - char* to value (free() when done with it) or NULL if no value
         negated - option was negated with trailing -
  */

  while ((option = get_option(&args, &argcnt, &argnum,
                              &optchar, &value, &negated,
                              &fna, &optnum, 0)))
  {
    switch (option)
    {
#ifdef EBCDIC
      case 'a':
        aflag = ASCII;
        printf("Translating to ASCII...\n");
        break;
#endif /* EBCDIC */
#ifdef CMS_MVS
        case 'B':
          bflag = 1;
          printf("Using binary mode...\n");
          break;
#endif /* CMS_MVS */
#ifdef TANDEM
        case 'B':
          nskformatopt(value);
          free(value);
          break;
#endif

        case '0':
          method = STORE; level = 0; break;
        case '1':  case '2':  case '3':  case '4':
        case '5':  case '6':  case '7':  case '8':  case '9':
          /* Set the compression efficacy */
          level = (int)option - '0';  break;
        case 'A':   /* Adjust unzipsfx'd zipfile:  adjust offsets only */
          adjust = 1; break;
#if defined(WIN32)
        case o_AC:
          clear_archive_bits = 1; break;
        case o_AS:
          /* Since some directories could be empty if no archive bits are
             set for files in a directory, don't add directory entries (-D).
             Just files with the archive bit set are added, including paths
             (unless paths are excluded).  All major unzips should create
             directories for the paths as needed. */
          dirnames = 0;
          only_archive_set = 1; break;
#endif /* MSDOS || OS2 || WIN32 */
        case 'b':   /* Specify path for temporary file */
          tempdir = 1;
          tempath = value;
          break;
        case 'c':   /* Add comments for new files in zip file */
          comadd = 1;  break;

        /* -C, -C2, and -C5 are with -V */

        case 'd':   /* Delete files from zip file */
          if (action != ADD) {
            ZIPERR(ZE_PARMS, "specify just one action");
          }
          action = DELETE;
          break;
#ifdef MACOS
        case o_df:
          MacZip.DataForkOnly = true;
          break;
#endif /* MACOS */
        case o_db:
          if (negated)
            display_bytes = 0;
          else
            display_bytes = 1;
          break;
        case o_dc:
          if (negated)
            display_counts = 0;
          else
            display_counts = 1;
          break;
        case o_dd:
          /* display dots */
          display_globaldots = 0;
          if (negated) {
            dot_count = 0;
          } else {
            /* set default dot size if dot_size not set (dot_count = 0) */
            if (dot_count == 0)
              /* default to 10 MB */
              dot_size = 10 * 0x100000;
            dot_count = -1;
          }
          break;
        case o_dg:
          /* display dots globally for archive instead of for each file */
          if (negated) {
            display_globaldots = 0;
          } else {
            display_globaldots = 1;
            /* set default dot size if dot_size not set (dot_count = 0) */
            if (dot_count == 0)
              dot_size = 10 * 0x100000;
            dot_count = -1;
          }
          break;
        case o_ds:
          /* input dot_size is now actual dot size to account for
             different buffer sizes */
          if (value == NULL)
            dot_size = 10 * 0x100000;
          else if (value[0] == '\0') {
            /* default to 10 MB */
            dot_size = 10 * 0x100000;
            free(value);
          } else {
            dot_size = ReadNumString(value);
            if (dot_size == (zoff_t)-1) {
              sprintf(errbuf, "option -ds (--dot-size) has bad size:  '%s'",
                      value);
              free(value);
              ZIPERR(ZE_PARMS, errbuf);
            }
            if (dot_size < 0x400) {
              /* < 1 KB so there is no multiplier, assume MB */
              dot_size *= 0x100000;

            } else if (dot_size < 0x400L * 32) {
              /* 1K <= dot_size < 32K */
              sprintf(errbuf, "dot size must be at least 32 KB:  '%s'", value);
              free(value);
              ZIPERR(ZE_PARMS, errbuf);

            } else {
              /* 32K <= dot_size */
            }
            free(value);
          }
          dot_count = -1;
          break;
        case o_du:
          if (negated)
            display_usize = 0;
          else
            display_usize = 1;
          break;
        case o_dv:
          if (negated)
            display_volume = 0;
          else
            display_volume = 1;
          break;
        case 'D':   /* Do not add directory entries */
          dirnames = 0; break;
        case o_DF:  /* Create a difference archive */
          diff_mode = 1;
          allow_empty_archive = 1;
          break;
        case 'e':   /* Encrypt */
#if !CRYPT
          ZIPERR(ZE_PARMS, "encryption not supported");
#else /* CRYPT */
          if (key)
            free(key);
          key_needed = 1;
#endif /* !CRYPT */
          break;
        case 'F':   /* fix the zip file */
          fix = 1; break;
        case o_FF:  /* try harder to fix file */
          fix = 2; break;
        case o_FI:
          if (negated)
            allow_fifo = 0;
          else
            allow_fifo = 1;
          break;
        case o_FS:  /* delete exiting entries in archive where there is
                       no matching file on file system */
          filesync = 1; break;
        case 'f':   /* Freshen zip file--overwrite only */
          if (action != ADD) {
            ZIPERR(ZE_PARMS, "specify just one action");
          }
          action = FRESHEN;
          break;
        case 'g':   /* Allow appending to a zip file */
          d = 1;  break;
#ifndef WINDLL
        case 'h': case 'H': case '?':  /* Help */
#ifdef VMSCLI
          VMSCLI_help();
#else
          help();
#endif
          RETURN(finish(ZE_OK));
#endif /* !WINDLL */

#ifndef WINDLL
        case o_h2:  /* Extended Help */
          help_extended();
          RETURN(finish(ZE_OK));
#endif /* !WINDLL */

        /* -i is with -x */
#if defined(VMS) || defined(WIN32)
        case o_ic:  /* Ignore case (case-insensitive matching of archive entries) */
          if (negated)
            filter_match_case = 1;
          else
            filter_match_case = 0;
          break;
#endif
#ifdef RISCOS
        case 'I':   /* Don't scan through Image files */
          scanimage = 0;
          break;
#endif
#ifdef MACOS
        case o_jj:   /* store absolute path including volname */
            MacZip.StoreFullPath = true;
            break;
#endif /* ?MACOS */
        case 'j':   /* Junk directory names */
          pathput = 0;  break;
        case 'J':   /* Junk sfx prefix */
          junk_sfx = 1;  break;
        case 'k':   /* Make entries using DOS names (k for Katz) */
          dosify = 1;  break;
        case 'l':   /* Translate end-of-line */
          translate_eol = 1; break;
        case o_ll:
          translate_eol = 2; break;
        case o_lf:
          /* open a logfile */
          /* allow multiple use of option but only last used */
          if (logfile_path) {
            free(logfile_path);
          }
          logfile_path = value;
          break;
        case o_la:
          /* append to existing logfile */
          if (negated)
            logfile_append = 0;
          else
            logfile_append = 1;
          break;
        case o_li:
          /* log all including informational messages */
          if (negated)
            logall = 0;
          else
            logall = 1;
          break;
#ifndef WINDLL
        case 'L':   /* Show license */
          license();
          RETURN(finish(ZE_OK));
#endif
        case 'm':   /* Delete files added or updated in zip file */
          dispose = 1;  break;
        case o_mm:  /* To prevent use of -mm for -MM */
          ZIPERR(ZE_PARMS, "-mm not supported, Must_Match is -MM");
          dispose = 1;  break;
        case o_MM:  /* Exit with error if input file can't be read */
          bad_open_is_error = 1; break;
        case 'n':   /* Don't compress files with a special suffix */
          special = value;
          /* special = NULL; */ /* will be set at next argument */
          break;
        case o_nw:  /* no wildcards - wildcards are handled like other characters */
          no_wild = 1;
          break;
#if defined(AMIGA) || defined(MACOS)
        case 'N':   /* Get zipfile comments from AmigaDOS/MACOS filenotes */
          filenotes = 1; break;
#endif
        case 'o':   /* Set zip file time to time of latest file in it */
          latest = 1;  break;
        case 'O':   /* Set output file different than input archive */
          out_path = ziptyp(value);
          free(value);
          have_out = 1;
          break;
        case 'p':   /* Store path with name */
          break;            /* (do nothing as annoyance avoidance) */
        case 'P':   /* password for encryption */
          if (key != NULL) {
            free(key);
          }
#if CRYPT
          key = value;
          key_needed = 0;
#else
          ZIPERR(ZE_PARMS, "encryption not supported");
#endif /* CRYPT */
          break;
#if defined(QDOS) || defined(QLZIP)
        case 'Q':
          qlflag  = strtol(value, NULL, 10);
       /* qlflag  = strtol((p+1), &p, 10); */
       /* p--; */
          if (qlflag == 0) qlflag = 4;
          free(value);
          break;
#endif
        case 'q':   /* Quiet operation */
          noisy = 0;
#ifdef MACOS
          MacZip.MacZip_Noisy = false;
#endif  /* MACOS */
          if (verbose) verbose--;
          break;
        case 'r':   /* Recurse into subdirectories, match full path */
          if (recurse == 2) {
            ZIPERR(ZE_PARMS, "do not specify both -r and -R");
          }
          recurse = 1;  break;
        case 'R':   /* Recurse into subdirectories, match filename */
          if (recurse == 1) {
            ZIPERR(ZE_PARMS, "do not specify both -r and -R");
          }
          recurse = 2;  break;

        case o_RE:   /* Allow [list] matching (regex) */
          allow_regex = 1; break;

        case o_sc:  /* show command line args */
          show_args = 1; break;
#ifdef UNICODE_TEST
        case o_sC:  /* create empty files from archive names */
          create_files = 1;
          show_files = 1; break;
#endif
        case o_sd:  /* show debugging */
          show_what_doing = 1; break;
        case o_sf:  /* show files to operate on */
          if (!negated)
            show_files = 1;
          else
            show_files = 2;
          break;
        case o_so:  /* show all options */
          show_options = 1; break;
#ifdef UNICODE_SUPPORT
        case o_su:  /* -sf but also show Unicode if exists */
          if (!negated)
            show_files = 3;
          else
            show_files = 4;
          break;
        case o_sU:  /* -sf but only show Unicode if exists or normal if not */
          if (!negated)
            show_files = 5;
          else
            show_files = 6;
          break;
#endif

        case 's':   /* enable split archives */
          /* get the split size from value */
          if (strcmp(value, "-") == 0) {
            /* -s- do not allow splits */
            split_method = -1;
          } else {
            split_size = ReadNumString(value);
            if (split_size == (uzoff_t)-1) {
              sprintf(errbuf, "bad split size:  '%s'", value);
              ZIPERR(ZE_PARMS, errbuf);
            }
            if (split_size == 0) {
              /* do not allow splits */
              split_method = -1;
            } else {
              if (split_method == 0) {
                split_method = 1;
              }
              if (split_size < 0x400) {
                /* < 1 KB there is no multiplier, assume MB */
                split_size *= 0x100000;
              }
              /* By setting the minimum split size to 64 KB we avoid
                 not having enough room to write a header unsplit
                 which is required */
              if (split_size < 0x400L * 64) {
                /* split_size < 64K */
                sprintf(errbuf, "minimum split size is 64 KB:  '%s'", value);
                free(value);
                ZIPERR(ZE_PARMS, errbuf);
              }
            }
          }
          free(value);
          break;
        case o_sb:  /* when pause for next split ring bell */
          split_bell = 1;
          break;
        case o_sp:  /* enable split select - pause splitting between splits */
          use_descriptors = 1;
          split_method = 2;
          break;
        case o_sv:  /* be verbose about creating splits */
          noisy_splits = 1;
          break;

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(ATARI)
        case 'S':
          hidden_files = 1; break;
#endif /* MSDOS || OS2 || WIN32 || ATARI */
#ifdef MACOS
        case 'S':
          MacZip.IncludeInvisible = true; break;
#endif /* MACOS */
        case 't':   /* Exclude files earlier than specified date */
          {
            int yyyy, mm, dd;       /* results of sscanf() */

            /* Support ISO 8601 & American dates */
            if ((sscanf(value, "%4d-%2d-%2d", &yyyy, &mm, &dd) != 3 &&
                 sscanf(value, "%2d%2d%4d", &mm, &dd, &yyyy) != 3) ||
                mm < 1 || mm > 12 || dd < 1 || dd > 31) {
              ZIPERR(ZE_PARMS,
                     "invalid date entered for -t option - use mmddyyyy or yyyy-mm-dd");
            }
            before = dostime(yyyy, mm, dd, 0, 0, 0);
          }
          free(value);
          break;
        case o_tt:  /* Exclude files at or after specified date */
          {
            int yyyy, mm, dd;       /* results of sscanf() */

            /* Support ISO 8601 & American dates */
            if ((sscanf(value, "%4d-%2d-%2d", &yyyy, &mm, &dd) != 3 &&
                 sscanf(value, "%2d%2d%4d", &mm, &dd, &yyyy) != 3) ||
                mm < 1 || mm > 12 || dd < 1 || dd > 31) {
              ZIPERR(ZE_PARMS,
                     "invalid date entered for -tt option - use mmddyyyy or yyyy-mm-dd");
            }
            after = dostime(yyyy, mm, dd, 0, 0, 0);
          }
          free(value);
          break;
        case 'T':   /* test zip file */
          test = 1; break;
        case o_TT:  /* command path to use instead of 'unzip -t ' */
          if (unzip_path)
            free(unzip_path);
          unzip_path = value;
          break;
        case 'U':   /* Select archive entries to keep or operate on */
          if (action != ADD) {
            ZIPERR(ZE_PARMS, "specify just one action");
          }
          action = ARCHIVE;
          break;
#ifdef UNICODE_SUPPORT
        case o_UN:   /* Unicode */
          if (abbrevmatch("quit", value, 0, 1)) {
            /* Unicode path mismatch is error */
            unicode_mismatch = 0;
          } else if (abbrevmatch("warn", value, 0, 1)) {
            /* warn of mismatches and continue */
            unicode_mismatch = 1;
          } else if (abbrevmatch("ignore", value, 0, 1)) {
            /* ignore mismatches and continue */
            unicode_mismatch = 2;
          } else if (abbrevmatch("no", value, 0, 1)) {
            /* no use Unicode path */
            unicode_mismatch = 3;
          } else if (abbrevmatch("escape", value, 0, 1)) {
            /* escape all non-ASCII characters */
            unicode_escape_all = 1;

          } else if (abbrevmatch("UTF8", value, 0, 1)) {
            /* force storing UTF-8 as standard per AppNote bit 11 */
            utf8_force = 1;

          } else {
            zipwarn("-UN must be Quit, Warn, Ignore, No, Escape, or UTF8: ", value);

            free(value);
            ZIPERR(ZE_PARMS, "-UN (unicode) bad value");
          }
          free(value);
          break;
#endif
        case 'u':   /* Update zip file--overwrite only if newer */
          if (action != ADD) {
            ZIPERR(ZE_PARMS, "specify just one action");
          }
          action = UPDATE;
          break;
        case 'v':        /* Either display version information or */
        case o_ve:       /* Mention oddities in zip file structure */
          if (option == o_ve ||      /* --version */
              (argcnt == 2 && strlen(args[1]) == 2)) { /* -v only */
            /* display version */
#ifndef WINDLL
            version_info();
#else
            zipwarn("version information not supported for dll", "");
#endif
            RETURN(finish(ZE_OK));
          } else {
            noisy = 1;
            verbose++;
          }
          break;
#ifdef VMS
        case 'C':  /* Preserve case (- = down-case) all. */
          if (negated)
          { /* Down-case all. */
            if ((vms_case_2 > 0) || (vms_case_5 > 0))
            {
              ZIPERR( ZE_PARMS, "Conflicting case directives (-C-)");
            }
            vms_case_2 = -1;
            vms_case_5 = -1;
          }
          else
          { /* Not negated.  Preserve all. */
            if ((vms_case_2 < 0) || (vms_case_5 < 0))
            {
              ZIPERR( ZE_PARMS, "Conflicting case directives (-C)");
            }
            vms_case_2 = 1;
            vms_case_5 = 1;
          }
          break;
        case o_C2:  /* Preserve case (- = down-case) ODS2. */
          if (negated)
          { /* Down-case ODS2. */
            if (vms_case_2 > 0)
            {
              ZIPERR( ZE_PARMS, "Conflicting case directives (-C2-)");
            }
            vms_case_2 = -1;
          }
          else
          { /* Not negated.  Preserve ODS2. */
            if (vms_case_2 < 0)
            {
              ZIPERR( ZE_PARMS, "Conflicting case directives (-C2)");
            }
            vms_case_2 = 1;
          }
          break;
        case o_C5:  /* Preserve case (- = down-case) ODS5. */
          if (negated)
          { /* Down-case ODS5. */
            if (vms_case_5 > 0)
            {
              ZIPERR( ZE_PARMS, "Conflicting case directives (-C5-)");
            }
            vms_case_5 = -1;
          }
          else
          { /* Not negated.  Preserve ODS5. */
            if (vms_case_5 < 0)
            {
              ZIPERR( ZE_PARMS, "Conflicting case directives (-C5)");
            }
            vms_case_5 = 1;
          }
          break;
        case 'V':   /* Store in VMS format.  (Record multiples.) */
          vms_native = 1; break;
          /* below does work with new parser but doesn't allow tracking
             -VV separately, like adding a separate description */
          /* vms_native++; break; */
        case o_VV:  /* Store in VMS specific format */
          vms_native = 2; break;
        case 'w':   /* Append the VMS version number */
          vmsver |= 1;  break;
        case o_ww:   /* Append the VMS version number as ".nnn". */
          vmsver |= 3;  break;
#endif /* VMS */
        case o_ws:  /* Wildcards do not include directory boundaries in matches */
          wild_stop_at_dir = 1;
          break;

        case 'i':   /* Include only the following files */
          /* if nothing matches include list then still create an empty archive */
          allow_empty_archive = 1;
        case 'x':   /* Exclude following files */
          add_filter((int) option, value);
          free(value);
          break;
#ifdef S_IFLNK
        case 'y':   /* Store symbolic links as such */
          linkput = 1;  break;
#endif /* S_IFLNK */
        case 'z':   /* Edit zip file comment */
          zipedit = 1;  break;
        case 'Z':   /* Compression method */
          if (abbrevmatch("deflate", value, 0, 1)) {
            /* deflate */
            method = DEFLATE;
          } else if (abbrevmatch("store", value, 0, 1)) {
            /* store */
            method = STORE;
          } else if (abbrevmatch("bzip2", value, 0, 1)) {
            /* bzip2 */
#ifdef BZIP2_SUPPORT
            method = BZIP2;
#else
            ZIPERR(ZE_COMPERR, "Compression method bzip2 not enabled");
#endif
          } else {
#ifdef BZIP2_SUPPORT
            zipwarn("valid compression methods are:  store, deflate, bzip2", "");
#else
            zipwarn("valid compression methods are:  store, deflate)", "");
#endif
            zipwarn("unknown compression method found:  ", value);
            free(value);
            ZIPERR(ZE_PARMS, "Option -Z (--compression-method):  unknown method");
          }
          free(value);
          break;
#if defined(MSDOS) || defined(OS2)
        case '$':   /* Include volume label */
          volume_label = 1; break;
#endif
#ifndef MACOS
        case '@':   /* read file names from stdin */
          comment_stream = NULL;
          s = 1;          /* defer -@ until have zipfile name */
          break;
#endif /* !MACOS */
        case 'X':
          if (negated)
            extra_fields = 2;
          else
            extra_fields = 0;
          break;
#ifdef OS2
        case 'E':
          /* use the .LONGNAME EA (if any) as the file's name. */
          use_longname_ea = 1;
          break;
#endif
#ifdef NTSD_EAS
        case '!':
          /* use security privilege overrides */
          use_privileges = 1;
          break;
#endif
#ifdef RISCOS
        case '/':
          exts2swap = value; /* override Zip$Exts */
          break;
#endif
        case o_des:
          use_descriptors = 1;
          break;

#ifdef ZIP64_SUPPORT
        case o_z64:   /* Force creation of Zip64 entries */
          if (negated) {
            force_zip64 = 0;
          } else {
            force_zip64 = 1;
          }
          break;
#endif

        case o_NON_OPTION_ARG:
          /* not an option */
          /* no more options as permuting */
          /* just dash also ends up here */

          if (recurse != 2 && kk == 0 && patterns == NULL) {
            /* have all filters so convert filterlist to patterns array
               as PROCNAME needs patterns array */
            filterlist_to_patterns();
          }

          /* "--" stops arg processing for remaining args */
          /* ignore only first -- */
          if (strcmp(value, "--") == 0 && seen_doubledash == 0) {
            /* -- */
            seen_doubledash = 1;
            if (kk == 0) {
              ZIPERR(ZE_PARMS, "can't use -- before archive name");
            }

            /* just ignore as just marks what follows as non-option arguments */

          } else if (kk == 6) {
            /* value is R pattern */
            add_filter((int)'R', value);
            free(value);
            if (first_listarg == 0) {
              first_listarg = argnum;
            }
          } else switch (kk)
          {
            case 0:
              /* first non-option arg is zipfile name */
#if (!defined(MACOS) && !defined(WINDLL))
              if (strcmp(value, "-") == 0) {  /* output zipfile is dash */
                /* just a dash */
                zipstdout();
              } else
#endif /* !MACOS && !WINDLL */
              {
                /* name of zipfile */
                if ((zipfile = ziptyp(value)) == NULL) {
                  ZIPERR(ZE_MEM, "was processing arguments");
                }
                /* read zipfile if exists */
                /*
                if ((r = readzipfile()) != ZE_OK) {
                  ZIPERR(r, zipfile);
                }
                */
                free(value);
              }
              if (show_what_doing) {
                fprintf(mesg, "sd: Zipfile name '%s'\n", zipfile);
                fflush(mesg);
              }
              /* if in_path not set, use zipfile path as usual for input */
              /* in_path is used as the base path to find splits */
              if (in_path == NULL) {
                if ((in_path = malloc(strlen(zipfile) + 1)) == NULL) {
                  ZIPERR(ZE_MEM, "was processing arguments");
                }
                strcpy(in_path, zipfile);
              }
              /* if out_path not set, use zipfile path as usual for output */
              /* out_path is where the output archive is written */
              if (out_path == NULL) {
                if ((out_path = malloc(strlen(zipfile) + 1)) == NULL) {
                  ZIPERR(ZE_MEM, "was processing arguments");
                }
                strcpy(out_path, zipfile);
              }
              kk = 3;
              if (s)
              {
                /* do -@ and get names from stdin */
                /* should be able to read names from
                   stdin and output to stdout, but
                   this was not allowed in old code.
                   This check moved to kk = 3 case to fix. */
                /* if (strcmp(zipfile, "-") == 0) {
                  ZIPERR(ZE_PARMS, "can't use - and -@ together");
                }
                */
                while ((pp = getnam(stdin)) != NULL)
                {
                  kk = 4;
                  if (recurse == 2) {
                    /* reading patterns from stdin */
                    add_filter((int)'R', pp);
                  } else {
                    /* file argument now processed later */
                    add_name(pp);
                  }
                  /*
                  if ((r = PROCNAME(pp)) != ZE_OK) {
                    if (r == ZE_MISS)
                      zipwarn("name not matched: ", pp);
                    else {
                      ZIPERR(r, pp);
                    }
                  }
                  */
                  free(pp);
                }
                s = 0;
              }
              if (recurse == 2) {
                /* rest are -R patterns */
                kk = 6;
              }
              break;

            case 3:  case 4:
              /* no recurse and -r file names */
              /* can't read filenames -@ and input - from stdin at
                 same time */
              if (s == 1 && strcmp(value, "-") == 0) {
                ZIPERR(ZE_PARMS, "can't read input (-) and filenames (-@) both from stdin");
              }
              /* add name to list for later processing */
              add_name(value);
              /*
              if ((r = PROCNAME(value)) != ZE_OK) {
                if (r == ZE_MISS)
                  zipwarn("name not matched: ", value);
                else {
                  ZIPERR(r, value);
                }
              }
              */
              if (kk == 3) {
                first_listarg = argnum;
                kk = 4;
              }
              break;

            } /* switch kk */
            break;

        default:
          /* should never get here as get_option will exit if not in table */
          sprintf(errbuf, "no such option ID: %ld", option);
          ZIPERR(ZE_PARMS, errbuf);

     }  /* switch */
  }


  /* do processing of command line and one-time tasks */

  /* Key not yet specified.  If needed, get/verify it now. */
  if (key_needed) {
    if ((key = malloc(IZ_PWLEN+1)) == NULL) {
      ZIPERR(ZE_MEM, "was getting encryption password");
    }
    r = encr_passwd(ZP_PW_ENTER, key, IZ_PWLEN+1, zipfile);
    if (r != IZ_PW_ENTERED) {
      if (r < IZ_PW_ENTERED)
        r = ZE_PARMS;
      ZIPERR(r, "was getting encryption password");
    }
    if (*key == '\0') {
      ZIPERR(ZE_PARMS, "zero length password not allowed");
    }
    if ((e = malloc(IZ_PWLEN+1)) == NULL) {
      ZIPERR(ZE_MEM, "was verifying encryption password");
    }
    r = encr_passwd(ZP_PW_VERIFY, e, IZ_PWLEN+1, zipfile);
    if (r != IZ_PW_ENTERED && r != IZ_PW_SKIPVERIFY) {
      free((zvoid *)e);
      if (r < ZE_OK) r = ZE_PARMS;
      ZIPERR(r, "was verifying encryption password");
    }
    r = ((r == IZ_PW_SKIPVERIFY) ? 0 : strcmp(key, e));
    free((zvoid *)e);
    if (r) {
      ZIPERR(ZE_PARMS, "password verification failed");
    }
  }
  if (key) {
    /* if -P "" could get here */
    if (*key == '\0') {
      ZIPERR(ZE_PARMS, "zero length password not allowed");
    }
  }

  if (show_what_doing) {
    fprintf(mesg, "sd: Command line read\n");
    fflush(mesg);
  }

  /* show command line args */
  if (show_args) {
    fprintf(mesg, "command line:\n");
    for (i = 0; args[i]; i++) {
      fprintf(mesg, "'%s'  ", args[i]);
    }
    fprintf(mesg, "\n");
    ZIPERR(ZE_ABORT, "show command line");
  }

  /* show all options */
  if (show_options) {
    printf("available options:\n");
    printf(" %-2s  %-18s %-4s %-3s %-30s\n", "sh", "long", "val", "neg", "description");
    printf(" %-2s  %-18s %-4s %-3s %-30s\n", "--", "----", "---", "---", "-----------");
    for (i = 0; options[i].option_ID; i++) {
      printf(" %-2s  %-18s ", options[i].shortopt, options[i].longopt);
      switch (options[i].value_type) {
        case o_NO_VALUE:
          printf("%-4s ", "");
          break;
        case o_REQUIRED_VALUE:
          printf("%-4s ", "req");
          break;
        case o_OPTIONAL_VALUE:
          printf("%-4s ", "opt");
          break;
        case o_VALUE_LIST:
          printf("%-4s ", "list");
          break;
        case o_ONE_CHAR_VALUE:
          printf("%-4s ", "char");
          break;
        case o_NUMBER_VALUE:
          printf("%-4s ", "num");
          break;
        default:
          printf("%-4s ", "unk");
      }
      switch (options[i].negatable) {
        case o_NEGATABLE:
          printf("%-3s ", "neg");
          break;
        case o_NOT_NEGATABLE:
          printf("%-3s ", "");
          break;
        default:
          printf("%-3s ", "unk");
      }
      if (options[i].name)
        printf("%-30s\n", options[i].name);
      else
        printf("\n");
    }
    RETURN(finish(ZE_OK));
  }


  /* open log file */
  if (logfile_path) {
    char mode[10];
    char *p;
    char *lastp;

    /* if no extension add .log */
    p = logfile_path;
    /* find last / */
    lastp = NULL;
    for (p = logfile_path; (p = MBSRCHR(p, '/')) != NULL; p++) {
      lastp = p;
    }
    if (lastp == NULL)
      lastp = logfile_path;
    if (MBSRCHR(lastp, '.') == NULL) {
      /* add .log */
      if ((p = malloc(strlen(logfile_path) + 5)) == NULL) {
        ZIPERR(ZE_MEM, "logpath");
      }
      strcpy(p, logfile_path);
      strcat(p, ".log");
      free(logfile_path);
      logfile_path = p;
    }

    if (logfile_append) {
      sprintf(mode, "a");
    } else {
      sprintf(mode, "w");
    }
    if ((logfile = zfopen(logfile_path, mode)) == NULL) {
      sprintf(errbuf, "could not open logfile '%s'", logfile_path);
      ZIPERR(ZE_PARMS, errbuf);
    }
    {
      /* At top put start time and command line */

      /* get current time */
      struct tm *now;
      time_t clocktime;

      time(&clocktime);
      now = localtime(&clocktime);

      fprintf(logfile, "---------\n");
      fprintf(logfile, "Zip log opened %s", asctime(now));
      fprintf(logfile, "command line arguments:\n ");
      for (i = 1; args[i]; i++) {
        size_t j;
        int has_space = 0;

        for (j = 0; j < strlen(args[i]); j++) {
          if (isspace(args[i][j])) {
            has_space = 1;
            break;
          }
        }
        if (has_space)
          fprintf(logfile, "\"%s\" ", args[i]);
        else
          fprintf(logfile, "%s ", args[i]);
      }
      fprintf(logfile, "\n\n");
      fflush(logfile);
    }
  } else {
    /* only set logall if logfile open */
    logall = 0;
  }


  if (split_method && out_path) {
    /* if splitting, the archive name must have .zip extension */
    int plen = strlen(out_path);
    char *out_path_ext;

#ifdef VMS
    /* On VMS, adjust plen (and out_path_ext) to avoid the file version. */
    plen -= strlen( vms_file_version( out_path));
#endif /* def VMS */
    out_path_ext = out_path+ plen- 4;

    if (plen < 4 ||
        out_path_ext[0] != '.' ||
        toupper(out_path_ext[1]) != 'Z' ||
        toupper(out_path_ext[2]) != 'I' ||
        toupper(out_path_ext[3]) != 'P') {
      ZIPERR(ZE_PARMS, "archive name must end in .zip for splits");
    }
  }


  if (verbose && (dot_size == 0) && (dot_count == 0)) {
    /* now default to default 10 MB dot size */
    dot_size = 10 * 0x100000;
    /* show all dots as before if verbose set and dot_size not set (dot_count = 0) */
    /* maybe should turn off dots in default verbose mode */
    /* dot_size = -1; */
  }

  /* done getting -R filters so convert filterlist if not done */
  if (pcount && patterns == NULL) {
    filterlist_to_patterns();
  }

#if (defined(MSDOS) || defined(OS2)) && !defined(WIN32)
  if ((kk == 3 || kk == 4) && volume_label == 1) {
    /* read volume label */
    PROCNAME(NULL);
    kk = 4;
  }
#endif

  if (have_out && kk == 3) {
    copy_only = 1;
    action = ARCHIVE;
  }

  if (have_out && namecmp(in_path, out_path) == 0) {
    sprintf(errbuf, "--out path must be different than in path: %s", out_path);
    ZIPERR(ZE_PARMS, errbuf);
  }

  if (fix && diff_mode) {
    ZIPERR(ZE_PARMS, "can't use --diff (-DF) with fix (-F or -FF)");
  }

  if (action == ARCHIVE && !have_out && !show_files) {
    ZIPERR(ZE_PARMS, "-U (--copy) requires -O (--out)");
  }

  if (fix && !have_out) {
    zipwarn("fix options -F and -FF require --out:\n",
            "                     zip -F indamagedarchive --out outfixedarchive");
    ZIPERR(ZE_PARMS, "fix options require --out");
  }

  if (fix && !copy_only) {
    ZIPERR(ZE_PARMS, "no other actions allowed when fixing archive (-F or -FF)");
  }

  if (!have_out && diff_mode) {
    ZIPERR(ZE_PARMS, "-DF (--diff) requires -O (--out)");
  }

  if (diff_mode && (action == ARCHIVE || action == DELETE)) {
    ZIPERR(ZE_PARMS, "can't use --diff (-DF) with -d or -U");
  }

  if (action != ARCHIVE && (recurse == 2 || pcount) && first_listarg == 0 &&
      !filelist && (kk < 3 || (action != UPDATE && action != FRESHEN))) {
    ZIPERR(ZE_PARMS, "nothing to select from");
  }

/*
  -------------------------------------
  end of new command line code
  -------------------------------------
*/

#if (!defined(MACOS) && !defined(WINDLL))
  if (kk < 3) {               /* zip used as filter */
    zipstdout();
    comment_stream = NULL;
    if ((r = procname("-", 0)) != ZE_OK) {
      if (r == ZE_MISS) {
        if (bad_open_is_error) {
          zipwarn("name not matched: ", "-");
          ZIPERR(ZE_OPEN, "-");
        } else {
          zipwarn("name not matched: ", "-");
        }
      } else {
        ZIPERR(r, "-");
      }
    }
    kk = 4;
    if (s) {
      ZIPERR(ZE_PARMS, "can't use - and -@ together");
    }
  }
#endif /* !MACOS && !WINDLL */

  if (zipfile && !strcmp(zipfile, "-")) {
    if (show_what_doing) {
      fprintf(mesg, "sd: Zipping to stdout\n");
      fflush(mesg);
    }
    zip_to_stdout = 1;
  }

  /* Check option combinations */
  if (special == NULL) {
    ZIPERR(ZE_PARMS, "missing suffix list");
  }
  if (level == 9 || !strcmp(special, ";") || !strcmp(special, ":"))
    special = NULL; /* compress everything */

  if (action == DELETE && (method != BEST || dispose || recurse ||
      key != NULL || comadd || zipedit)) {
    zipwarn("invalid option(s) used with -d; ignored.","");
    /* reset flags - needed? */
    method  = BEST;
    dispose = 0;
    recurse = 0;
    if (key != NULL) {
      free((zvoid *)key);
      key   = NULL;
    }
    comadd  = 0;
    zipedit = 0;
  }
  if (action == ARCHIVE && (method != BEST || dispose || recurse ||
      comadd || zipedit)) {
    zipwarn("can't set method, move, recurse, or comments with copy mode.","");
    /* reset flags - needed? */
    method  = BEST;
    dispose = 0;
    recurse = 0;
    comadd  = 0;
    zipedit = 0;
  }
  if (linkput && dosify)
    {
      zipwarn("can't use -y with -k, -y ignored", "");
      linkput = 0;
    }
  if (fix == 1 && adjust)
    {
      zipwarn("can't use -F with -A, -F ignored", "");
      fix = 0;
    }
  if (fix == 2 && adjust)
    {
      zipwarn("can't use -FF with -A, -FF ignored", "");
      fix = 0;
    }
  if (test && zip_to_stdout) {
    test = 0;
    zipwarn("can't use -T on stdout, -T ignored", "");
  }
  if (split_method && (fix || adjust)) {
    ZIPERR(ZE_PARMS, "can't create split archive while fixing or adjusting\n");
  }
  if (split_method && (d || zip_to_stdout)) {
    ZIPERR(ZE_PARMS, "can't create split archive with -d or -g or on stdout\n");
  }
  if ((action != ADD || d) && filesync) {
    ZIPERR(ZE_PARMS, "can't use -d, -f, -u, -U, or -g with filesync -FS\n");
  }
  if ((action != ADD || d) && zip_to_stdout) {
    ZIPERR(ZE_PARMS, "can't use -d, -f, -u, -U, or -g on stdout\n");
  }
#if defined(EBCDIC)  && !defined(OS390)
  if (aflag==ASCII && !translate_eol) {
    /* Translation to ASCII implies EOL translation!
     * (on OS390, consistent EOL translation is controlled separately)
     * The default translation mode is "UNIX" mode (single LF terminators).
     */
    translate_eol = 2;
  }
#endif
#ifdef CMS_MVS
  if (aflag==ASCII && bflag)
    ZIPERR(ZE_PARMS, "can't use -a with -B");
#endif
#ifdef VMS
  if (!extra_fields && vms_native)
    {
      zipwarn("can't use -V with -X, -V ignored", "");
      vms_native = 0;
    }
  if (vms_native && translate_eol)
    ZIPERR(ZE_PARMS, "can't use -V with -l or -ll");
#endif

  if (noisy) {
    if (fix == 1)
      zipmessage("Fix archive (-F) - assume mostly intact archive", "");
    else if (fix == 2)
      zipmessage("Fix archive (-FF) - salvage what can", "");
  }

  /* Read old archive */

  /* Now read the zip file here instead of when doing args above */
  /* Only read the central directory and build zlist */
  if (show_what_doing) {
    fprintf(mesg, "sd: Reading archive\n");
    fflush(mesg);
  }




  /* If -FF we do it all here */
  if (fix == 2) {

    /* Open zip file and temporary output file */
    if (show_what_doing) {
      fprintf(mesg, "sd: Open zip file and create temp file (-FF)\n");
      fflush(mesg);
    }
    diag("opening zip file and creating temporary zip file");
    x = NULL;
    tempzn = 0;
    if (show_what_doing) {
      fprintf(mesg, "sd: Creating new zip file (-FF)\n");
      fflush(mesg);
    }
#if defined(UNIX) && !defined(NO_MKSTEMP)
    {
      int yd;
      int i;

      /* use mkstemp to avoid race condition and compiler warning */

      if (tempath != NULL)
      {
        /* if -b used to set temp file dir use that for split temp */
        if ((tempzip = malloc(strlen(tempath) + 12)) == NULL) {
          ZIPERR(ZE_MEM, "allocating temp filename");
        }
        strcpy(tempzip, tempath);
        if (lastchar(tempzip) != '/')
          strcat(tempzip, "/");
      }
      else
      {
        /* create path by stripping name and appending template */
        if ((tempzip = malloc(strlen(zipfile) + 12)) == NULL) {
        ZIPERR(ZE_MEM, "allocating temp filename");
        }
        strcpy(tempzip, zipfile);
        for(i = strlen(tempzip); i > 0; i--) {
          if (tempzip[i - 1] == '/')
            break;
        }
        tempzip[i] = '\0';
      }
      strcat(tempzip, "ziXXXXXX");

      if ((yd = mkstemp(tempzip)) == EOF) {
        ZIPERR(ZE_TEMP, tempzip);
      }
      if ((y = fdopen(yd, FOPW_TMP)) == NULL) {
        ZIPERR(ZE_TEMP, tempzip);
      }
    }
#else
    if ((tempzip = tempname(zipfile)) == NULL) {
      ZIPERR(ZE_MEM, "allocating temp filename");
    }
    if ((y = zfopen(tempzip, FOPW_TMP)) == NULL) {
      ZIPERR(ZE_TEMP, tempzip);
    }
#endif

#if (!defined(VMS) && !defined(CMS_MVS))
    /* Use large buffer to speed up stdio: */
#if (defined(_IOFBF) || !defined(BUFSIZ))
    zipbuf = (char *)malloc(ZBSZ);
#else
    zipbuf = (char *)malloc(BUFSIZ);
#endif
    if (zipbuf == NULL) {
      ZIPERR(ZE_MEM, tempzip);
    }
# ifdef _IOFBF
    setvbuf(y, zipbuf, _IOFBF, ZBSZ);
# else
    setbuf(y, zipbuf);
# endif /* _IOBUF */
#endif /* !VMS  && !CMS_MVS */


    if ((r = readzipfile()) != ZE_OK) {
      ZIPERR(r, zipfile);
    }

    /* Write central directory and end header to temporary zip */
    if (show_what_doing) {
      fprintf(mesg, "sd: Writing central directory (-FF)\n");
      fflush(mesg);
    }
    diag("writing central directory");
    k = 0;                        /* keep count for end header */
    c = tempzn;                   /* get start of central */
    n = t = 0;
    for (z = zfiles; z != NULL; z = z->nxt)
    {
      if ((r = putcentral(z)) != ZE_OK) {
        ZIPERR(r, tempzip);
      }
      tempzn += 4 + CENHEAD + z->nam + z->cext + z->com;
      n += z->len;
      t += z->siz;
      k++;
    }
    if (zcount == 0)
      zipwarn("zip file empty", "");
    t = tempzn - c;               /* compute length of central */
    diag("writing end of central directory");
    if ((r = putend(k, t, c, zcomlen, zcomment)) != ZE_OK) {
      ZIPERR(r, tempzip);
    }
    if (fclose(y)) {
      ZIPERR(d ? ZE_WRITE : ZE_TEMP, tempzip);
    }
    if (in_file != NULL) {
      fclose(in_file);
      in_file = NULL;
    }

    /* Replace old zip file with new zip file, leaving only the new one */
    if (strcmp(zipfile, "-") && !d)
    {
      diag("replacing old zip file with new zip file");
      if ((r = replace(out_path, tempzip)) != ZE_OK)
      {
        zipwarn("new zip file left as: ", tempzip);
        free((zvoid *)tempzip);
        tempzip = NULL;
        ZIPERR(r, "was replacing the original zip file");
      }
      free((zvoid *)tempzip);
    }
    tempzip = NULL;
    if (zip_attributes && strcmp(zipfile, "-")) {
      setfileattr(out_path, zip_attributes);
#ifdef VMS
      /* If the zip file existed previously, restore its record format: */
      if (x != NULL)
        (void)VMSmunch(out_path, RESTORE_RTYPE, NULL);
#endif
    }

    set_filetype(out_path);

    /* finish logfile (it gets closed in freeup() called by finish()) */
    if (logfile) {
        struct tm *now;
        time_t clocktime;

        fprintf(logfile, "\nTotal %ld entries (", files_total);
        DisplayNumString(logfile, bytes_total);
        fprintf(logfile, " bytes)");

        /* get current time */
        time(&clocktime);
        now = localtime(&clocktime);
        fprintf(logfile, "\nDone %s", asctime(now));
        fflush(logfile);
    }

    RETURN(finish(ZE_OK));
  }



  /* read zipfile if exists */
  if ((r = readzipfile()) != ZE_OK) {
    ZIPERR(r, zipfile);
  }

#ifndef UTIL
  if (split_method == -1) {
    split_method = 0;
  } else if (!fix && split_method == 0 && total_disks > 1) {
    /* if input archive is multi-disk and splitting has not been
       enabled or disabled (split_method == -1), then automatically
       set split size to same as first input split */
    zoff_t size = 0;

    in_split_path = get_in_split_path(in_path, 0);

    if (filetime(in_split_path, NULL, &size, NULL) == 0) {
      zipwarn("Could not get info for input split: ", in_split_path);
      return ZE_OPEN;
    }
    split_method = 1;
    split_size = (uzoff_t) size;

    free(in_split_path);
    in_split_path = NULL;
  }

  if (noisy_splits && split_size > 0)
    zipmessage("splitsize = ", zip_fuzofft(split_size, NULL, NULL));
#endif

  /* so disk display starts at 1, will be updated when entries are read */
  current_in_disk = 0;

  /* no input zipfile and showing contents */
  if (!zipfile_exists && show_files && (kk == 3 || action == ARCHIVE)) {
    ZIPERR(ZE_OPEN, zipfile);
  }

  if (zcount == 0 && (action != ADD || d)) {
    zipwarn(zipfile, " not found or empty");
  }

  if (have_out && kk == 3) {
    /* no input paths so assume copy mode and match everything if --out */
    for (z = zfiles; z != NULL; z = z->nxt) {
      z->mark = pcount ? filter(z->zname, filter_match_case) : 1;
    }
  }

  /* Scan for new files */

  /* Process file arguments from command line */
  if (filelist) {
    if (action == ARCHIVE) {
      /* find in archive */
      if (show_what_doing) {
        fprintf(mesg, "sd: Scanning archive entries\n");
        fflush(mesg);
      }
      for (; filelist; ) {
        if ((r = proc_archive_name(filelist->name, filter_match_case)) != ZE_OK) {
          if (r == ZE_MISS) {
            char *n = NULL;
#ifdef WIN32
            /* Win9x console always uses OEM character coding, and
               WinNT console is set to OEM charset by default, too */
            if ((n = malloc(strlen(filelist->name) + 1)) == NULL)
              ZIPERR(ZE_MEM, "name not matched error");
            INTERN_TO_OEM(filelist->name, n);
#else
            n = filelist->name;
#endif
            zipwarn("not in archive: ", n);
#ifdef WIN32
            free(n);
#endif
          }
          else {
            ZIPERR(r, filelist->name);
          }
        }
        free(filelist->name);
        filearg = filelist;
        filelist = filelist->next;
        free(filearg);
      }
    } else {
      /* try find matching files on OS first then try find entries in archive */
      if (show_what_doing) {
        fprintf(mesg, "sd: Scanning files\n");
        fflush(mesg);
      }
      for (; filelist; ) {
        if ((r = PROCNAME(filelist->name)) != ZE_OK) {
          if (r == ZE_MISS) {
            if (bad_open_is_error) {
              zipwarn("name not matched: ", filelist->name);
              ZIPERR(ZE_OPEN, filelist->name);
            } else {
              zipwarn("name not matched: ", filelist->name);
            }
          } else {
            ZIPERR(r, filelist->name);
          }
        }
        free(filelist->name);
        filearg = filelist;
        filelist = filelist->next;
        free(filearg);
      }
    }
  }

  /* recurse from current directory for -R */
  if (recurse == 2) {
#ifdef AMIGA
    if ((r = PROCNAME("")) != ZE_OK)
#else
    if ((r = PROCNAME(".")) != ZE_OK)
#endif
    {
      if (r == ZE_MISS) {
        if (bad_open_is_error) {
          zipwarn("name not matched: ", "current directory for -R");
          ZIPERR(ZE_OPEN, "-R");
        } else {
          zipwarn("name not matched: ", "current directory for -R");
        }
      } else {
        ZIPERR(r, "-R");
      }
    }
  }


  if (show_what_doing) {
    fprintf(mesg, "sd: Applying filters\n");
    fflush(mesg);
  }
  /* Clean up selections ("3 <= kk <= 5" now) */
  if (kk != 4 && first_listarg == 0 &&
      (action == UPDATE || action == FRESHEN)) {
    /* if -u or -f with no args, do all, but, when present, apply filters */
    for (z = zfiles; z != NULL; z = z->nxt) {
      z->mark = pcount ? filter(z->zname, filter_match_case) : 1;
#ifdef DOS
      if (z->mark) z->dosflag = 1;      /* force DOS attribs for incl. names */
#endif
    }
  }
  if (show_what_doing) {
    fprintf(mesg, "sd: Checking dups\n");
    fflush(mesg);
  }
  if ((r = check_dup()) != ZE_OK) {     /* remove duplicates in found list */
    if (r == ZE_PARMS) {
      ZIPERR(r, "cannot repeat names in zip file");
    }
    else {
      ZIPERR(r, "was processing list of files");
    }
  }

  if (zcount)
    free((zvoid *)zsort);


/*
 * XXX make some kind of mktemppath() function for each OS.
 */

#ifndef VM_CMS
/* For CMS, leave tempath NULL.  A-disk will be used as default. */
  /* If -b not specified, make temporary path the same as the zip file */
#if defined(MSDOS) || defined(__human68k__) || defined(AMIGA)
  if (tempath == NULL && ((p = MBSRCHR(zipfile, '/')) != NULL ||
#  ifdef MSDOS
                          (p = MBSRCHR(zipfile, '\\')) != NULL ||
#  endif /* MSDOS */
                          (p = MBSRCHR(zipfile, ':')) != NULL))
  {
    if (*p == ':')
      p++;
#else
#ifdef RISCOS
  if (tempath == NULL && (p = MBSRCHR(zipfile, '.')) != NULL)
  {
#else
#ifdef QDOS
  if (tempath == NULL && (p = LastDir(zipfile)) != NULL)
  {
#else
  if (tempath == NULL && (p = MBSRCHR(zipfile, '/')) != NULL)
  {
#endif /* QDOS */
#endif /* RISCOS */
#endif /* MSDOS || __human68k__ || AMIGA */
    if ((tempath = (char *)malloc((int)(p - zipfile) + 1)) == NULL) {
      ZIPERR(ZE_MEM, "was processing arguments");
    }
    r = *p;  *p = 0;
    strcpy(tempath, zipfile);
    *p = (char)r;
  }
#endif /* VM_CMS */

#if (defined(IZ_CHECK_TZ) && defined(USE_EF_UT_TIME))
  if (!zp_tz_is_valid) {
    zipwarn("TZ environment variable not found, cannot use UTC times!!","");
  }
#endif /* IZ_CHECK_TZ && USE_EF_UT_TIME */

  /* For each marked entry, if not deleting, check if it exists, and if
     updating or freshening, compare date with entry in old zip file.
     Unmark if it doesn't exist or is too old, else update marked count. */
  if (show_what_doing) {
    fprintf(mesg, "sd: Scanning files to update\n");
    fflush(mesg);
  }
#ifdef MACOS
  PrintStatProgress("Getting file information ...");
#endif
  diag("stating marked entries");
  k = 0;                        /* Initialize marked count */
  scan_started = 0;
  scan_count = 0;
  all_current = 1;
  for (z = zfiles; z != NULL; z = z->nxt) {
    /* if already displayed Scanning files in newname() then continue dots */
    if (noisy && scan_last) {
      scan_count++;
      if (scan_count % 100 == 0) {
        time_t current = time(NULL);

        if (current - scan_last > scan_dot_time) {
          if (scan_started == 0) {
            scan_started = 1;
            fprintf(mesg, " ");
            fflush(mesg);
          }
          scan_last = current;
          fprintf(mesg, ".");
          fflush(mesg);
        }
      }
    }
    z->current = 0;
    if (!(z->mark)) {
      /* if something excluded run through the list to catch deletions */
      all_current = 0;
    }
    if (z->mark) {
#ifdef USE_EF_UT_TIME
      iztimes f_utim, z_utim;
      ulg z_tim;
#endif /* USE_EF_UT_TIME */
      Trace((stderr, "zip diagnostics: marked file=%s\n", z->oname));

      csize = z->siz;
      usize = z->len;
      if (action == DELETE) {
        /* only delete files in date range */
#ifdef USE_EF_UT_TIME
        z_tim = (get_ef_ut_ztime(z, &z_utim) & EB_UT_FL_MTIME) ?
                unix2dostime(&z_utim.mtime) : z->tim;
#else /* !USE_EF_UT_TIME */
#       define z_tim  z->tim
#endif /* ?USE_EF_UT_TIME */
        if (z_tim < before || (after && z_tim >= after)) {
          /* include in archive */
          z->mark = 0;
        } else {
          /* delete file */
          files_total++;
          /* ignore len in old archive and update to current size */
          z->len = usize;
          if (csize != (uzoff_t) -1 && csize != (uzoff_t) -2)
            bytes_total += csize;
          k++;
        }
      } else if (action == ARCHIVE) {
        /* only keep files in date range */
#ifdef USE_EF_UT_TIME
        z_tim = (get_ef_ut_ztime(z, &z_utim) & EB_UT_FL_MTIME) ?
                unix2dostime(&z_utim.mtime) : z->tim;
#else /* !USE_EF_UT_TIME */
#       define z_tim  z->tim
#endif /* ?USE_EF_UT_TIME */
        if (z_tim < before || (after && z_tim >= after)) {
          /* exclude from archive */
          z->mark = 0;
        } else {
          /* keep file */
          files_total++;
          /* ignore len in old archive and update to current size */
          z->len = usize;
          if (csize != (uzoff_t) -1 && csize != (uzoff_t) -2)
            bytes_total += csize;
          k++;
        }
      } else {
        int isdirname = 0;

        if (z->name && (z->name)[strlen(z->name) - 1] == '/') {
          isdirname = 1;
        }

# if defined(UNICODE_SUPPORT) && defined(WIN32)
        if (!no_win32_wide) {
          if (z->namew == NULL) {
            if (z->uname != NULL)
              z->namew = utf8_to_wchar_string(z->uname);
            else
              z->namew = local_to_wchar_string(z->name);
          }
        }
# endif

#ifdef USE_EF_UT_TIME
# if defined(UNICODE_SUPPORT) && defined(WIN32)
        if (!no_win32_wide)
          tf = filetimew(z->namew, (ulg *)NULL, (zoff_t *)&usize, &f_utim);
        else
          tf = filetime(z->name, (ulg *)NULL, (zoff_t *)&usize, &f_utim);
# else
        tf = filetime(z->name, (ulg *)NULL, (zoff_t *)&usize, &f_utim);
# endif
#else /* !USE_EF_UT_TIME */
# if defined(UNICODE_SUPPORT) && defined(WIN32)
        if (!no_win32_wide)
          tf = filetimew(z->namew, (ulg *)NULL, (zoff_t *)&usize, NULL);
        else
          tf = filetime(z->name, (ulg *)NULL, (zoff_t *)&usize, NULL);
# else
        tf = filetime(z->name, (ulg *)NULL, (zoff_t *)&usize, NULL);
# endif
#endif /* ?USE_EF_UT_TIME */
        if (tf == 0)
          /* entry that is not on OS */
          all_current = 0;
        if (tf == 0 ||
            tf < before || (after && tf >= after) ||
            ((action == UPDATE || action == FRESHEN) &&
#ifdef USE_EF_UT_TIME
             ((get_ef_ut_ztime(z, &z_utim) & EB_UT_FL_MTIME) ?
              f_utim.mtime <= ROUNDED_TIME(z_utim.mtime) : tf <= z->tim)
#else /* !USE_EF_UT_TIME */
             tf <= z->tim
#endif /* ?USE_EF_UT_TIME */
           ))
        {
          z->mark = comadd ? 2 : 0;
          z->trash = tf && tf >= before &&
                     (after ==0 || tf < after);   /* delete if -um or -fm */
          if (verbose)
            fprintf(mesg, "zip diagnostic: %s %s\n", z->oname,
                   z->trash ? "up to date" : "missing or early");
          if (logfile)
            fprintf(logfile, "zip diagnostic: %s %s\n", z->oname,
                   z->trash ? "up to date" : "missing or early");
        }
        else if (diff_mode && tf == z->tim &&
                 ((isdirname && (zoff_t)usize == -1) || (usize == z->len))) {
          /* if in diff mode only include if file time or size changed */
          /* usize is -1 for directories */
          z->mark = 0;
        }
        else {
          /* usize is -1 for directories and -2 for devices */
          if (tf == z->tim &&
              ((z->len == 0 && (zoff_t)usize == -1)
               || usize == z->len)) {
            /* FileSync uses the current flag */
            /* Consider an entry current if file time is the same
               and entry size is 0 and a directory on the OS
               or the entry size matches the OS size */
            z->current = 1;
          } else {
            all_current = 0;
          }
          files_total++;
          if (usize != (uzoff_t) -1 && usize != (uzoff_t) -2)
            /* ignore len in old archive and update to current size */
            z->len = usize;
          else
            z->len = 0;
          if (usize != (uzoff_t) -1 && usize != (uzoff_t) -2)
            bytes_total += usize;
          k++;
        }
      }
    }
  }

  /* Remove entries from found list that do not exist or are too old */
  if (show_what_doing) {
    fprintf(mesg, "sd: fcount = %u\n", (unsigned)fcount);
    fflush(mesg);
  }

  diag("stating new entries");
  scan_count = 0;
  scan_started = 0;
  Trace((stderr, "zip diagnostic: fcount=%u\n", (unsigned)fcount));
  for (f = found; f != NULL;) {
    Trace((stderr, "zip diagnostic: new file=%s\n", f->oname));

    if (noisy) {
      /* if updating archive and update was quick, scanning for new files
         can still take a long time */
      if (!zip_to_stdout && scan_last == 0 && scan_count % 100 == 0) {
        time_t current = time(NULL);

        if (current - scan_start > scan_delay) {
          fprintf(mesg, "Scanning files ");
          fflush(mesg);
          mesg_line_started = 1;
          scan_last = current;
        }
      }
      /* if already displayed Scanning files in newname() or above then continue dots */
      if (scan_last) {
        scan_count++;
        if (scan_count % 100 == 0) {
          time_t current = time(NULL);

          if (current - scan_last > scan_dot_time) {
            if (scan_started == 0) {
              scan_started = 1;
              fprintf(mesg, " ");
              fflush(mesg);
            }
            scan_last = current;
            fprintf(mesg, ".");
            fflush(mesg);
          }
        }
      }
    }
    tf = 0;
    if (action != DELETE && action != FRESHEN) {
#if defined(UNICODE_SUPPORT) && defined(WIN32)
      if (!no_win32_wide)
        tf = filetimew(f->namew, (ulg *)NULL, (zoff_t *)&usize, NULL);
      else
        tf = filetime(f->name, (ulg *)NULL, (zoff_t *)&usize, NULL);
#else
      tf = filetime(f->name, (ulg *)NULL, (zoff_t *)&usize, NULL);
#endif
    }

    if (action == DELETE || action == FRESHEN ||
        tf == 0 ||
        tf < before || (after && tf >= after) ||
        (namecmp(f->zname, zipfile) == 0 && !zip_to_stdout)
       )
      f = fexpel(f);
    else {
      /* ??? */
      files_total++;
      f->usize = 0;
      if (usize != (uzoff_t) -1 && usize != (uzoff_t) -2) {
        bytes_total += usize;
        f->usize = usize;
      }
      f = f->nxt;
    }
  }
  if (mesg_line_started) {
    fprintf(mesg, "\n");
    mesg_line_started = 0;
  }
#ifdef MACOS
  PrintStatProgress("done");
#endif

  if (show_files) {
    uzoff_t count = 0;
    uzoff_t bytes = 0;

    if (noisy) {
      fflush(mesg);
    }

    if (noisy && (show_files == 1 || show_files == 3 || show_files == 5)) {
      /* sf, su, sU */
      if (mesg_line_started) {
        fprintf(mesg, "\n");
        mesg_line_started = 0;
      }
      if (kk == 3)
        /* -sf alone */
        fprintf(mesg, "Archive contains:\n");
      else if (action == DELETE)
        fprintf(mesg, "Would Delete:\n");
      else if (action == FRESHEN)
        fprintf(mesg, "Would Freshen:\n");
      else if (action == ARCHIVE)
        fprintf(mesg, "Would Copy:\n");
      else
        fprintf(mesg, "Would Add/Update:\n");
      fflush(mesg);
    }

    if (logfile) {
      if (logfile_line_started) {
        fprintf(logfile, "\n");
        logfile_line_started = 0;
      }
      if (kk == 3)
        /* -sf alone */
        fprintf(logfile, "Archive contains:\n");
      else if (action == DELETE)
        fprintf(logfile, "Would Delete:\n");
      else if (action == FRESHEN)
        fprintf(logfile, "Would Freshen:\n");
      else if (action == ARCHIVE)
        fprintf(logfile, "Would Copy:\n");
      else
        fprintf(logfile, "Would Add/Update:\n");
      fflush(logfile);
    }

    for (z = zfiles; z != NULL; z = z->nxt) {
      if (z->mark || kk == 3) {
        count++;
        if ((zoff_t)z->len > 0)
          bytes += z->len;
        if (noisy && (show_files == 1 || show_files == 3))
          /* sf, su */
          fprintf(mesg, "  %s\n", z->oname);
        if (logfile && !(show_files == 5 || show_files == 6))
          /* not sU or sU- show normal name in log */
          fprintf(logfile, "  %s\n", z->oname);

#ifdef UNICODE_TEST
        if (create_files) {
          int r;
          int dir = 0;
          FILE *f;

#if defined(UNICODE_SUPPORT) && defined(WIN32)
          char *fn = NULL;
          wchar_t *fnw = NULL;

          if (!no_win32_wide) {
            if ((fnw = malloc((wcslen(z->znamew) + 120) * sizeof(wchar_t))) == NULL)
              ZIPERR(ZE_MEM, "sC");
            wcscpy(fnw, L"testdir/");
            wcscat(fnw, z->znamew);
            if (fnw[wcslen(fnw) - 1] == '/')
              dir = 1;
            if (dir)
              r = _wmkdir(fnw);
            else
              f = _wfopen(fnw, L"w");
          } else {
            if ((fn = malloc(strlen(z->zname) + 120)) == NULL)
              ZIPERR(ZE_MEM, "sC");
            strcpy(fn, "testdir/");
            strcat(fn, z->zname);
            if (fn[strlen(fn) - 1] == '/')
              dir = 1;
            if (dir)
              r = mkdir(fn);
            else
              f = fopen(fn, "w");
          }
#else
          char *fn = NULL;
          if ((fn = malloc(strlen(z->zname) + 120)) == NULL)
            ZIPERR(ZE_MEM, "sC");
          strcpy(fn, "testdir/");
          if (z->uname)
            strcat(fn, z->uname);
          else
            strcat(fn, z->zname);

          if (fn[strlen(fn) - 1] == '/')
            dir = 1;
          if (dir)
            r = mkdir(fn, 0777);
          else
            f = fopen(fn, "w");
#endif
          if (dir) {
            if (r) {
              if (errno != 17) {
                printf(" - could not create directory testdir/%s\n", z->oname);
                perror("    dir");
              }
            } else {
              printf(" - created directory testdir/%s\n", z->oname);
            }
          } else {
            if (f == NULL) {
              printf(" - could not open testdir/%s\n", z->oname);
              perror("    file");
            } else {
              fclose(f);
              printf(" - created testdir/%s\n", z->oname);
              if (z->uname)
                printf("   u - created testdir/%s\n", z->uname);
            }
          }
        }
#endif
#ifdef UNICODE_SUPPORT
        if (show_files == 3 || show_files == 4) {
          /* su, su- */
          /* Include escaped Unicode name if exists under standard name */
          if (z->ouname) {
            if (noisy && show_files == 3)
              fprintf(mesg, "     Escaped Unicode:  %s\n", z->ouname);
            if (logfile)
              fprintf(logfile, "     Escaped Unicode:  %s\n", z->ouname);
          }
        }
        if (show_files == 5 || show_files == 6) {
          /* sU, sU- */
          /* Display only escaped Unicode name if exists or standard name */
          if (z->ouname) {
            /* Unicode name */
            if (noisy && show_files == 5) {
              fprintf(mesg, "  %s\n", z->ouname);
            }
            if (logfile) {
              fprintf(logfile, "  %s\n", z->ouname);
            }
          } else {
            /* No Unicode name so use standard name */
            if (noisy && show_files == 5) {
              fprintf(mesg, "  %s\n", z->oname);
            }
            if (logfile) {
              fprintf(logfile, "  %s\n", z->oname);
            }
          }
        }
#endif
      }
    }
    for (f = found; f != NULL; f = f->nxt) {
      count++;
      if ((zoff_t)f->usize > 0)
        bytes += f->usize;
#ifdef UNICODE_SUPPORT
      if (unicode_escape_all) {
        char *escaped_unicode;
        escaped_unicode = local_to_escape_string(f->zname);
        if (noisy && (show_files == 1 || show_files == 3 || show_files == 5))
          /* sf, su, sU */
          fprintf(mesg, "  %s\n", escaped_unicode);
        if (logfile)
          fprintf(logfile, "  %s\n", escaped_unicode);
        free(escaped_unicode);
      } else {
#endif
        if (noisy && (show_files == 1 || show_files == 3 || show_files == 5))
          /* sf, su, sU */
          fprintf(mesg, "  %s\n", f->oname);
        if (logfile)
          fprintf(logfile, "  %s\n", f->oname);
#ifdef UNICODE_SUPPORT
      }
#endif
    }
    if (noisy || logfile == NULL)
      fprintf(mesg, "Total %s entries (%s bytes)\n",
                                          zip_fuzofft(count, NULL, NULL),
                                          zip_fuzofft(bytes, NULL, NULL));
    if (logfile)
      fprintf(logfile, "Total %s entries (%s bytes)\n",
                                          zip_fuzofft(count, NULL, NULL),
                                          zip_fuzofft(bytes, NULL, NULL));
    RETURN(finish(ZE_OK));
  }

  /* Make sure there's something left to do */
  if (k == 0 && found == NULL && !diff_mode &&
      !(zfiles == NULL && allow_empty_archive) &&
      !(zfiles != NULL &&
        (latest || fix || adjust || junk_sfx || comadd || zipedit))) {
    if (test && (zfiles != NULL || zipbeg != 0)) {
#ifndef WINDLL
      check_zipfile(zipfile, argv[0]);
#endif
      RETURN(finish(ZE_OK));
    }
    if (action == UPDATE || action == FRESHEN) {
      RETURN(finish(ZE_NONE));
    }
    else if (zfiles == NULL && (latest || fix || adjust || junk_sfx)) {
      ZIPERR(ZE_NAME, zipfile);
    }
#ifndef WINDLL
    else if (recurse && (pcount == 0) && (first_listarg > 0)) {
#ifdef VMS
      strcpy(errbuf, "try: zip \"");
      for (i = 1; i < (first_listarg - 1); i++)
        strcat(strcat(errbuf, args[i]), "\" ");
      strcat(strcat(errbuf, args[i]), " *.* -i");
#else /* !VMS */
      strcpy(errbuf, "try: zip");
      for (i = 1; i < first_listarg; i++)
        strcat(strcat(errbuf, " "), args[i]);
#  ifdef AMIGA
      strcat(errbuf, " \"\" -i");
#  else
      strcat(errbuf, " . -i");
#  endif
#endif /* ?VMS */
      for (i = first_listarg; i < argc; i++)
        strcat(strcat(errbuf, " "), args[i]);
      ZIPERR(ZE_NONE, errbuf);
    }
    else {
      ZIPERR(ZE_NONE, zipfile);
    }
#endif /* !WINDLL */
  }

  if (filesync && all_current && fcount == 0) {
    zipmessage("Archive is current", "");
    RETURN(finish(ZE_OK));
  }

  d = (d && k == 0 && (zipbeg || zfiles != NULL)); /* d true if appending */

#if CRYPT
  /* Initialize the crc_32_tab pointer, when encryption was requested. */
  if (key != NULL) {
    crc_32_tab = get_crc_table();
#ifdef EBCDIC
    /* convert encryption key to ASCII (ISO variant for 8-bit ASCII chars) */
    strtoasc(key, key);
#endif /* EBCDIC */
  }
#endif /* CRYPT */

  /* Just ignore the spanning signature if a multi-disk archive */
  if (zfiles && total_disks != 1 && zipbeg == 4) {
    zipbeg = 0;
  }

  /* Before we get carried away, make sure zip file is writeable. This
   * has the undesired side effect of leaving one empty junk file on a WORM,
   * so when the zipfile does not exist already and when -b is specified,
   * the writability check is made in replace().
   */
  if (strcmp(zipfile, "-"))
  {
    if (tempdir && zfiles == NULL && zipbeg == 0) {
      zip_attributes = 0;
    } else {
      x = (have_out || (zfiles == NULL && zipbeg == 0)) ? zfopen(out_path, FOPW) :
                                                          zfopen(out_path, FOPM);
      /* Note: FOPW and FOPM expand to several parameters for VMS */
      if (x == NULL) {
        ZIPERR(ZE_CREAT, out_path);
      }
      fclose(x);
      zip_attributes = getfileattr(out_path);
      if (zfiles == NULL && zipbeg == 0)
        destroy(out_path);
    }
  }
  else
    zip_attributes = 0;

  /* Throw away the garbage in front of the zip file for -J */
  if (junk_sfx) zipbeg = 0;

  /* Open zip file and temporary output file */
  if (show_what_doing) {
    fprintf(mesg, "sd: Open zip file and create temp file\n");
    fflush(mesg);
  }
  diag("opening zip file and creating temporary zip file");
  x = NULL;
  tempzn = 0;
  if (strcmp(zipfile, "-") == 0)
  {
#ifdef MSDOS
    /* It is nonsense to emit the binary data stream of a zipfile to
     * the (text mode) console.  This case should already have been caught
     * in a call to zipstdout() far above.  Therefore, if the following
     * failsafe check detects a console attached to stdout, zip is stopped
     * with an "internal logic error"!  */
    if (isatty(fileno(stdout)))
      ZIPERR(ZE_LOGIC, "tried to write binary zipfile data to console!");
    /* Set stdout mode to binary for MSDOS systems */
#  ifdef __HIGHC__
    setmode(stdout, _BINARY);
#  else
    setmode(fileno(stdout), O_BINARY);
#  endif
    y = zfdopen(fileno(stdout), FOPW);
#else
    y = stdout;
#endif
    /* tempzip must be malloced so a later free won't barf */
    tempzip = malloc(4);
    if (tempzip == NULL) {
      ZIPERR(ZE_MEM, "allocating temp filename");
    }
    strcpy(tempzip, "-");
  }
  else if (d) /* d true if just appending (-g) */
  {
    if (total_disks > 1) {
      ZIPERR(ZE_PARMS, "cannot grow split archive");
    }
    if ((y = zfopen(zipfile, FOPM)) == NULL) {
      ZIPERR(ZE_NAME, zipfile);
    }
    tempzip = zipfile;
    /*
    tempzf = y;
    */

    if (zfseeko(y, cenbeg, SEEK_SET)) {
      ZIPERR(ferror(y) ? ZE_READ : ZE_EOF, zipfile);
    }
    bytes_this_split = cenbeg;
    tempzn = cenbeg;
  }
  else
  {
    if (show_what_doing) {
      fprintf(mesg, "sd: Creating new zip file\n");
      fflush(mesg);
    }
    /* See if there is something at beginning of disk 1 to copy.
       If not, do nothing as zipcopy() will open files to read
       as needed. */
    if (zipbeg) {
      in_split_path = get_in_split_path(in_path, 0);

      while ((in_file = zfopen(in_split_path, FOPR_EX)) == NULL) {
        /* could not open split */

        /* Ask for directory with split.  Updates in_path */
        if (ask_for_split_read_path(0) != ZE_OK) {
          ZIPERR(ZE_ABORT, "could not open archive to read");
        }
        free(in_split_path);
        in_split_path = get_in_split_path(in_path, 1);
      }
    }
#if defined(UNIX) && !defined(NO_MKSTEMP)
    {
      int yd;
      int i;

      /* use mkstemp to avoid race condition and compiler warning */

      if (tempath != NULL)
      {
        /* if -b used to set temp file dir use that for split temp */
        if ((tempzip = malloc(strlen(tempath) + 12)) == NULL) {
          ZIPERR(ZE_MEM, "allocating temp filename");
        }
        strcpy(tempzip, tempath);
        if (lastchar(tempzip) != '/')
          strcat(tempzip, "/");
      }
      else
      {
        /* create path by stripping name and appending template */
        if ((tempzip = malloc(strlen(zipfile) + 12)) == NULL) {
        ZIPERR(ZE_MEM, "allocating temp filename");
        }
        strcpy(tempzip, zipfile);
        for(i = strlen(tempzip); i > 0; i--) {
          if (tempzip[i - 1] == '/')
            break;
        }
        tempzip[i] = '\0';
      }
      strcat(tempzip, "ziXXXXXX");

      if ((yd = mkstemp(tempzip)) == EOF) {
        ZIPERR(ZE_TEMP, tempzip);
      }
      if ((y = fdopen(yd, FOPW_TMP)) == NULL) {
        ZIPERR(ZE_TEMP, tempzip);
      }
    }
#else
    if ((tempzip = tempname(zipfile)) == NULL) {
      ZIPERR(ZE_MEM, "allocating temp filename");
    }
    if ((y = zfopen(tempzip, FOPW_TMP)) == NULL) {
      ZIPERR(ZE_TEMP, tempzip);
    }
#endif
  }

#if (!defined(VMS) && !defined(CMS_MVS))
  /* Use large buffer to speed up stdio: */
#if (defined(_IOFBF) || !defined(BUFSIZ))
  zipbuf = (char *)malloc(ZBSZ);
#else
  zipbuf = (char *)malloc(BUFSIZ);
#endif
  if (zipbuf == NULL) {
    ZIPERR(ZE_MEM, tempzip);
  }
# ifdef _IOFBF
  setvbuf(y, zipbuf, _IOFBF, ZBSZ);
# else
  setbuf(y, zipbuf);
# endif /* _IOBUF */
#endif /* !VMS  && !CMS_MVS */

  /* If not seekable set some flags 3/14/05 EG */
  output_seekable = 1;
  if (!is_seekable(y)) {
    output_seekable = 0;
    use_descriptors = 1;
  }

  /* Not needed.  Only need Zip64 when input file is larger than 2 GB or reading
     stdin and writing stdout.  This is set in putlocal() for each file. */
#if 0
  /* If using descriptors and Zip64 enabled force Zip64 3/13/05 EG */
# ifdef ZIP64_SUPPORT
  if (use_descriptors && force_zip64 != 0) {
    force_zip64 = 1;
  }
# endif
#endif

  /* if archive exists, not streaming and not deleting or growing, copy
     any bytes at beginning */
  if (strcmp(zipfile, "-") != 0 && !d)  /* this must go *after* set[v]buf */
  {
    /* copy anything before archive */
    if (in_file && zipbeg && (r = bfcopy(zipbeg)) != ZE_OK) {
      ZIPERR(r, r == ZE_TEMP ? tempzip : zipfile);
    }
    if (in_file) {
      fclose(in_file);
      in_file = NULL;
      free(in_split_path);
    }
    tempzn = zipbeg;
    if (split_method) {
      /* add spanning signature */
      if (show_what_doing) {
        fprintf(mesg, "sd: Adding spanning/splitting signature at top of archive\n");
        fflush(mesg);
      }
      /* write the spanning signature at the top of the archive */
      errbuf[0] = 0x50 /*'P' except for EBCDIC*/;
      errbuf[1] = 0x4b /*'K' except for EBCDIC*/;
      errbuf[2] = 7;
      errbuf[3] = 8;
      bfwrite(errbuf, 1, 4, BFWRITE_DATA);
      /* tempzn updated below */
      tempzn += 4;
    }
  }

  o = 0;                                /* no ZE_OPEN errors yet */


  /* Process zip file, updating marked files */
#ifdef DEBUG
  if (zfiles != NULL)
    diag("going through old zip file");
#endif
  if (zfiles != NULL && show_what_doing) {
    fprintf(mesg, "sd: Going through old zip file\n");
    fflush(mesg);
  }
  w = &zfiles;
  while ((z = *w) != NULL) {
    if (z->mark == 1)
    {
      uzoff_t len;
      if ((zoff_t)z->len == -1)
        /* device */
        len = 0;
      else
        len = z->len;

      /* if not deleting, zip it up */
      if (action != ARCHIVE && action != DELETE)
      {
        struct zlist far *localz; /* local header */

        if (verbose || !(filesync && z->current))
          DisplayRunningStats();
        if (noisy)
        {
          if (action == FRESHEN) {
            fprintf(mesg, "freshening: %s", z->oname);
            mesg_line_started = 1;
            fflush(mesg);
          } else if (filesync && z->current) {
            if (verbose) {
              fprintf(mesg, "      ok: %s", z->oname);
              mesg_line_started = 1;
              fflush(mesg);
            }
          } else if (!(filesync && z->current)) {
            fprintf(mesg, "updating: %s", z->oname);
            mesg_line_started = 1;
            fflush(mesg);
          }
        }
        if (logall)
        {
          if (action == FRESHEN) {
            fprintf(logfile, "freshening: %s", z->oname);
            logfile_line_started = 1;
            fflush(logfile);
          } else if (filesync && z->current) {
            if (verbose) {
              fprintf(logfile, " current: %s", z->oname);
              logfile_line_started = 1;
              fflush(logfile);
            }
          } else {
            fprintf(logfile, "updating: %s", z->oname);
            logfile_line_started = 1;
            fflush(logfile);
          }
        }

        /* Get local header flags and extra fields */
        if (readlocal(&localz, z) != ZE_OK) {
          zipwarn("could not read local entry information: ", z->oname);
          z->lflg = z->flg;
          z->ext = 0;
        } else {
          z->lflg = localz->lflg;
          z->ext = localz->ext;
          z->extra = localz->extra;
          if (localz->nam) free(localz->iname);
          if (localz->nam) free(localz->name);
#ifdef UNICODE_SUPPORT
          if (localz->uname) free(localz->uname);
#endif
          free(localz);
        }

        if (!(filesync && z->current) &&
             (r = zipup(z)) != ZE_OK && r != ZE_OPEN && r != ZE_MISS)
        {
          zipmessage_nl("", 1);
          /*
          if (noisy)
          {
            if (mesg_line_started) {
#if (!defined(MACOS) && !defined(WINDLL))
              putc('\n', mesg);
              fflush(mesg);
#else
              fprintf(stdout, "\n");
              fflush(stdout);
#endif
              mesg_line_started = 0;
            }
          }
          if (logall) {
            if (logfile_line_started) {
              fprintf(logfile, "\n");
              logfile_line_started = 0;
              fflush(logfile);
            }
          }
          */
          sprintf(errbuf, "was zipping %s", z->name);
          ZIPERR(r, errbuf);
        }
        if (filesync && z->current)
        {
          /* if filesync if entry matches OS just copy */
          if ((r = zipcopy(z)) != ZE_OK)
          {
            sprintf(errbuf, "was copying %s", z->oname);
            ZIPERR(r, errbuf);
          }
          zipmessage_nl("", 1);
          /*
          if (noisy)
          {
            if (mesg_line_started) {
#if (!defined(MACOS) && !defined(WINDLL))
              putc('\n', mesg);
              fflush(mesg);
#else
              fprintf(stdout, "\n");
              fflush(stdout);
#endif
              mesg_line_started = 0;
            }
          }
          if (logall) {
            if (logfile_line_started) {
              fprintf(logfile, "\n");
              logfile_line_started = 0;
              fflush(logfile);
            }
          }
          */
        }
        if (r == ZE_OPEN || r == ZE_MISS)
        {
          o = 1;
          zipmessage_nl("", 1);
          /*
          if (noisy)
          {
#if (!defined(MACOS) && !defined(WINDLL))
            putc('\n', mesg);
            fflush(mesg);
#else
            fprintf(stdout, "\n");
#endif
            mesg_line_started = 0;
          }
          if (logall) {
            fprintf(logfile, "\n");
            logfile_line_started = 0;
            fflush(logfile);
          }
          */
          if (r == ZE_OPEN) {
            perror(z->oname);
            zipwarn("could not open for reading: ", z->oname);
            if (bad_open_is_error) {
              sprintf(errbuf, "was zipping %s", z->name);
              ZIPERR(r, errbuf);
            }
          } else {
            zipwarn("file and directory with the same name: ", z->oname);
          }
          zipwarn("will just copy entry over: ", z->oname);
          if ((r = zipcopy(z)) != ZE_OK)
          {
            sprintf(errbuf, "was copying %s", z->oname);
            ZIPERR(r, errbuf);
          }
          z->mark = 0;
        }
        files_so_far++;
        good_bytes_so_far += z->len;
        bytes_so_far += len;
        w = &z->nxt;
      }
      else if (action == ARCHIVE)
      {
#ifdef DEBUG
        zoff_t here = zftello(y);
#endif

        DisplayRunningStats();
        if (skip_this_disk - 1 != z->dsk)
          /* moved to another disk so start copying again */
          skip_this_disk = 0;
        if (skip_this_disk - 1 == z->dsk) {
          /* skipping this disk */
          if (noisy) {
            fprintf(mesg, " skipping: %s", z->oname);
            mesg_line_started = 1;
            fflush(mesg);
          }
          if (logall) {
            fprintf(logfile, " skipping: %s", z->oname);
            logfile_line_started = 1;
            fflush(logfile);
          }
        } else {
          /* copying this entry */
          if (noisy) {
            fprintf(mesg, " copying: %s", z->oname);
            if (display_usize) {
              fprintf(mesg, " (");
              DisplayNumString(mesg, z->len );
              fprintf(mesg, ")");
            }
            mesg_line_started = 1;
            fflush(mesg);
          }
          if (logall)
          {
            fprintf(logfile, " copying: %s", z->oname);
            if (display_usize) {
              fprintf(logfile, " (");
              DisplayNumString(logfile, z->len );
              fprintf(logfile, ")");
            }
            logfile_line_started = 1;
            fflush(logfile);
          }
        }

        if (skip_this_disk - 1 == z->dsk)
          /* skip entries on this disk */
          z->mark = 0;
        else if ((r = zipcopy(z)) != ZE_OK)
        {
          if (r == ZE_ABORT) {
            ZIPERR(r, "user requested abort");
          } else if (fix != 1) {
            /* exit */
            sprintf(errbuf, "was copying %s", z->oname);
            zipwarn("(try -F to attempt to fix)", "");
            ZIPERR(r, errbuf);
          }
          else /* if (r == ZE_FORM) */ {
#ifdef DEBUG
            zoff_t here = zftello(y);
#endif

            /* seek back in output to start of this entry so can overwrite */
            if (zfseeko(y, current_local_offset, SEEK_SET) != 0){
              ZIPERR(r, "could not seek in output file");
            }
            zipwarn("bad - skipping: ", z->oname);
#ifdef DEBUG
            here = zftello(y);
#endif
            tempzn = current_local_offset;
            bytes_this_split = current_local_offset;
          }
        }
        if (skip_this_disk || !(fix == 1 && r != ZE_OK))
        {
          if (noisy && mesg_line_started) {
            fprintf(mesg, "\n");
            mesg_line_started = 0;
            fflush(mesg);
          }
          if (logall && logfile_line_started) {
            fprintf(logfile, "\n");
            logfile_line_started = 0;
            fflush(logfile);
          }
        }
        /* input counts */
        files_so_far++;
        if (r != ZE_OK)
          bad_bytes_so_far += z->siz;
        else
          good_bytes_so_far += z->siz;
        bytes_so_far += z->siz;

        if (r != ZE_OK && fix == 1) {
          /* remove bad entry from list */
          v = z->nxt;                     /* delete entry from list */
          free((zvoid *)(z->iname));
          free((zvoid *)(z->zname));
          free(z->oname);
#ifdef UNICODE_SUPPORT
          if (z->uname) free(z->uname);
#endif /* def UNICODE_SUPPORT */
          if (z->ext)
            /* don't have local extra until zipcopy reads it */
            if (z->extra) free((zvoid *)(z->extra));
          if (z->cext && z->cextra != z->extra)
            free((zvoid *)(z->cextra));
          if (z->com)
            free((zvoid *)(z->comment));
          farfree((zvoid far *)z);
          *w = v;
          zcount--;
        } else {
          w = &z->nxt;
        }

#ifdef WINDLL
#ifdef ZIP64_SUPPORT
        /* int64 support in caller */
        if (lpZipUserFunctions->ServiceApplication64 != NULL)
        {
          if ((*lpZipUserFunctions->ServiceApplication64)(z->zname, z->siz))
                    ZIPERR(ZE_ABORT, "User terminated operation");
        }
        else
        {
          /* no int64 support in caller */
          filesize64 = z->siz;
          low = (unsigned long)(filesize64 & 0x00000000FFFFFFFF);
          high = (unsigned long)((filesize64 >> 32) & 0x00000000FFFFFFFF);
          if (lpZipUserFunctions->ServiceApplication64_No_Int64 != NULL) {
            if ((*lpZipUserFunctions->ServiceApplication64_No_Int64)(z->zname, low, high))
                      ZIPERR(ZE_ABORT, "User terminated operation");
          }
        }
#else
        if (lpZipUserFunctions->ServiceApplication != NULL) {
          if ((*lpZipUserFunctions->ServiceApplication)(z->zname, z->siz))
            ZIPERR(ZE_ABORT, "User terminated operation");
        }
#endif /* ZIP64_SUPPORT - I added comments around // comments - does that help below? EG */
/* strange but true: if I delete this and put these two endifs adjacent to
   each other, the Aztec Amiga compiler never sees the second endif!  WTF?? PK */
#endif /* WINDLL */
      }
      else
      {
        DisplayRunningStats();
        if (noisy)
        {
          fprintf(mesg, "deleting: %s", z->oname);
          if (display_usize) {
            fprintf(mesg, " (");
            DisplayNumString(mesg, z->len );
            fprintf(mesg, ")");
          }
          fflush(mesg);
          fprintf(mesg, "\n");
        }
        if (logall)
        {
          fprintf(logfile, "deleting: %s", z->oname);
          if (display_usize) {
            fprintf(logfile, " (");
            DisplayNumString(logfile, z->len );
            fprintf(logfile, ")");
          }
          fprintf(logfile, "\n");
          fflush(logfile);
        }
        files_so_far++;
        good_bytes_so_far += z->siz;
        bytes_so_far += z->siz;
#ifdef WINDLL
#ifdef ZIP64_SUPPORT
        /* int64 support in caller */
        if (lpZipUserFunctions->ServiceApplication64 != NULL)
        {
          if ((*lpZipUserFunctions->ServiceApplication64)(z->zname, z->siz))
                    ZIPERR(ZE_ABORT, "User terminated operation");
        }
        else
        {
          /* no int64 support in caller */
          filesize64 = z->siz;
          low = (unsigned long)(filesize64 & 0x00000000FFFFFFFF);
          high = (unsigned long)((filesize64 >> 32) & 0x00000000FFFFFFFF);
          if (lpZipUserFunctions->ServiceApplication64_No_Int64 != NULL) {
            if ((*lpZipUserFunctions->ServiceApplication64_No_Int64)(z->zname, low, high))
                      ZIPERR(ZE_ABORT, "User terminated operation");
          }
        }
#else
        if (lpZipUserFunctions->ServiceApplication != NULL) {
          if ((*lpZipUserFunctions->ServiceApplication)(z->zname, z->siz))
            ZIPERR(ZE_ABORT, "User terminated operation");
        }
#endif /* ZIP64_SUPPORT - I added comments around // comments - does that help below? EG */
/* strange but true: if I delete this and put these two endifs adjacent to
   each other, the Aztec Amiga compiler never sees the second endif!  WTF?? PK */
#endif /* WINDLL */

        v = z->nxt;                     /* delete entry from list */
        free((zvoid *)(z->iname));
        free((zvoid *)(z->zname));
        free(z->oname);
#ifdef UNICODE_SUPPORT
        if (z->uname) free(z->uname);
#endif /* def UNICODE_SUPPORT */
        if (z->ext)
          /* don't have local extra until zipcopy reads it */
          if (z->extra) free((zvoid *)(z->extra));
        if (z->cext && z->cextra != z->extra)
          free((zvoid *)(z->cextra));
        if (z->com)
          free((zvoid *)(z->comment));
        farfree((zvoid far *)z);
        *w = v;
        zcount--;
      }
    }
    else
    {
      if (action == ARCHIVE) {
        v = z->nxt;                     /* delete entry from list */
        free((zvoid *)(z->iname));
        free((zvoid *)(z->zname));
        free(z->oname);
#ifdef UNICODE_SUPPORT
        if (z->uname) free(z->uname);
#endif /* def UNICODE_SUPPORT */
        if (z->ext)
          /* don't have local extra until zipcopy reads it */
          if (z->extra) free((zvoid *)(z->extra));
        if (z->cext && z->cextra != z->extra)
          free((zvoid *)(z->cextra));
        if (z->com)
          free((zvoid *)(z->comment));
        farfree((zvoid far *)z);
        *w = v;
        zcount--;
      }
      else
      {
        if (filesync) {
          /* Delete entries if don't match a file on OS */
          BlankRunningStats();
          if (noisy)
          {
            fprintf(mesg, "deleting: %s", z->oname);
            if (display_usize) {
              fprintf(mesg, " (");
              DisplayNumString(mesg, z->len );
              fprintf(mesg, ")");
            }
            fflush(mesg);
            fprintf(mesg, "\n");
            mesg_line_started = 0;
          }
          if (logall)
          {
            fprintf(logfile, "deleting: %s", z->oname);
            if (display_usize) {
              fprintf(logfile, " (");
              DisplayNumString(logfile, z->len );
              fprintf(logfile, ")");
            }
            fprintf(logfile, "\n");
            fflush(logfile);
            logfile_line_started = 0;
          }
        }
        /* copy the original entry */
        else if (!d && !diff_mode && (r = zipcopy(z)) != ZE_OK)
        {
          sprintf(errbuf, "was copying %s", z->oname);
          ZIPERR(r, errbuf);
        }
        w = &z->nxt;
      }
    }
  }


  /* Process the edited found list, adding them to the zip file */
  if (show_what_doing) {
    fprintf(mesg, "sd: Zipping up new entries\n");
    fflush(mesg);
  }
  diag("zipping up new entries, if any");
  Trace((stderr, "zip diagnostic: fcount=%u\n", (unsigned)fcount));
  for (f = found; f != NULL; f = fexpel(f))
  {
    uzoff_t len;
    /* add a new zfiles entry and set the name */
    if ((z = (struct zlist far *)farmalloc(sizeof(struct zlist))) == NULL) {
      ZIPERR(ZE_MEM, "was adding files to zip file");
    }
    z->nxt = NULL;
    z->name = f->name;
    f->name = NULL;
#ifdef UNICODE_SUPPORT
    z->uname = NULL;          /* UTF-8 name for extra field */
    z->zuname = NULL;         /* externalized UTF-8 name for matching */
    z->ouname = NULL;         /* display version of UTF-8 name with OEM */

#if 0
    /* New AppNote bit 11 allowing storing UTF-8 in path */
    if (utf8_force && f->uname) {
      if (f->iname)
        free(f->iname);
      if ((f->iname = malloc(strlen(f->uname) + 1)) == NULL)
        ZIPERR(ZE_MEM, "Unicode bit 11");
      strcpy(f->iname, f->uname);
# ifdef WIN32
      if (f->inamew)
        free(f->inamew);
      f->inamew = utf8_to_wchar_string(f->iname);
# endif
    }
#endif

    /* Only set z->uname if have a non-ASCII Unicode name */
    /* The Unicode path extra field is created if z->uname is not NULL,
       unless on a UTF-8 system, then instead of creating the extra field
       set bit 11 in the General Purpose Bit Flag */
    {
      int is_ascii = 0;

# ifdef WIN32
      if (!no_win32_wide)
        is_ascii = is_ascii_stringw(f->inamew);
      else
        is_ascii = is_ascii_string(f->uname);
# else
      is_ascii = is_ascii_string(f->uname);
# endif

      if (z->uname == NULL) {
        if (!is_ascii)
          z->uname = f->uname;
        else
          free(f->uname);
      } else {
        free(f->uname);
      }
    }
    f->uname = NULL;

#endif
    z->iname = f->iname;
    f->iname = NULL;
    z->zname = f->zname;
    f->zname = NULL;
    z->oname = f->oname;
    f->oname = NULL;
#if defined(UNICODE_SUPPORT) && defined(WIN32)
    z->namew = f->namew;
    f->namew = NULL;
    z->inamew = f->inamew;
    f->inamew = NULL;
    z->znamew = f->znamew;
    f->znamew = NULL;
#endif
    z->ext = z->cext = z->com = 0;
    z->extra = z->cextra = NULL;
    z->mark = 1;
    z->dosflag = f->dosflag;
    /* zip it up */
    DisplayRunningStats();
    if (noisy)
    {
      fprintf(mesg, "  adding: %s", z->oname);
      mesg_line_started = 1;
      fflush(mesg);
    }
    if (logall)
    {
      fprintf(logfile, "  adding: %s", z->oname);
      logfile_line_started = 1;
      fflush(logfile);
    }
    /* initial scan */
    len = f->usize;
    if ((r = zipup(z)) != ZE_OK  && r != ZE_OPEN && r != ZE_MISS)
    {
      zipmessage_nl("", 1);
      /*
      if (noisy)
      {
#if (!defined(MACOS) && !defined(WINDLL))
        putc('\n', mesg);
        fflush(mesg);
#else
        fprintf(stdout, "\n");
#endif
        mesg_line_started = 0;
        fflush(mesg);
      }
      if (logall) {
        fprintf(logfile, "\n");
        logfile_line_started = 0;
        fflush(logfile);
      }
      */
      sprintf(errbuf, "was zipping %s", z->oname);
      ZIPERR(r, errbuf);
    }
    if (r == ZE_OPEN || r == ZE_MISS)
    {
      o = 1;
      zipmessage_nl("", 1);
      /*
      if (noisy)
      {
#if (!defined(MACOS) && !defined(WINDLL))
        putc('\n', mesg);
        fflush(mesg);
#else
        fprintf(stdout, "\n");
#endif
        mesg_line_started = 0;
        fflush(mesg);
      }
      if (logall) {
        fprintf(logfile, "\n");
        logfile_line_started = 0;
        fflush(logfile);
      }
      */
      if (r == ZE_OPEN) {
        perror("zip warning");
        if (logfile)
          fprintf(logfile, "zip warning: %s\n", strerror(errno));
        zipwarn("could not open for reading: ", z->oname);
        if (bad_open_is_error) {
          sprintf(errbuf, "was zipping %s", z->name);
          ZIPERR(r, errbuf);
        }
      } else {
        zipwarn("file and directory with the same name: ", z->oname);
      }
      files_so_far++;
      bytes_so_far += len;
      bad_files_so_far++;
      bad_bytes_so_far += len;
      free((zvoid *)(z->name));
      free((zvoid *)(z->iname));
      free((zvoid *)(z->zname));
      free(z->oname);
#ifdef UNICODE_SUPPORT
      if (z->uname)
        free(z->uname);
# ifdef WIN32
      if (z->namew)
        free((zvoid *)(z->namew));
      if (z->inamew)
        free((zvoid *)(z->inamew));
      if (z->znamew)
        free((zvoid *)(z->znamew));
# endif
#endif
      farfree((zvoid far *)z);
    }
    else
    {
      files_so_far++;
      /* current size of file (just before reading) */
      good_bytes_so_far += z->len;
      /* size of file on initial scan */
      bytes_so_far += len;
      *w = z;
      w = &z->nxt;
      zcount++;
    }
  }
  if (key != NULL)
  {
    free((zvoid *)key);
    key = NULL;
  }

  /* final status 3/17/05 EG */
  if (noisy && bad_files_so_far)
  {
    char tempstrg[100];

    fprintf(mesg, "\nzip warning: Not all files were readable\n");
    fprintf(mesg, "  files/entries read:  %lu", files_total - bad_files_so_far);
    WriteNumString(good_bytes_so_far, tempstrg);
    fprintf(mesg, " (%s bytes)", tempstrg);
    fprintf(mesg, "  skipped:  %lu", bad_files_so_far);
    WriteNumString(bad_bytes_so_far, tempstrg);
    fprintf(mesg, " (%s bytes)\n", tempstrg);
    fflush(mesg);
  }
  if (logfile && bad_files_so_far)
  {
    char tempstrg[100];

    fprintf(logfile, "\nzip warning: Not all files were readable\n");
    fprintf(logfile, "  files/entries read:  %lu", files_total - bad_files_so_far);
    WriteNumString(good_bytes_so_far, tempstrg);
    fprintf(logfile, " (%s bytes)", tempstrg);
    fprintf(logfile, "  skipped:  %lu", bad_files_so_far);
    WriteNumString(bad_bytes_so_far, tempstrg);
    fprintf(logfile, " (%s bytes)", tempstrg);
  }

  /* Get one line comment for each new entry */
  if (show_what_doing) {
    fprintf(mesg, "sd: Get comment if any\n");
    fflush(mesg);
  }
#if defined(AMIGA) || defined(MACOS)
  if (comadd || filenotes)
  {
    if (comadd)
#else
  if (comadd)
  {
#endif
    {
      if (comment_stream == NULL) {
#ifndef RISCOS
        comment_stream = (FILE*)fdopen(fileno(stderr), "r");
#else
        comment_stream = stderr;
#endif
      }
      if ((e = malloc(MAXCOM + 1)) == NULL) {
        ZIPERR(ZE_MEM, "was reading comment lines");
      }
    }
#ifdef __human68k__
    setmode(fileno(comment_stream), O_TEXT);
#endif
#ifdef MACOS
    if (noisy) fprintf(mesg, "\nStart commenting files ...\n");
#endif
    for (z = zfiles; z != NULL; z = z->nxt)
      if (z->mark)
#if defined(AMIGA) || defined(MACOS)
        if (filenotes && (p = GetComment(z->zname)))
        {
          if (z->comment = malloc(k = strlen(p)+1))
          {
            z->com = k;
            strcpy(z->comment, p);
          }
          else
          {
            free((zvoid *)e);
            ZIPERR(ZE_MEM, "was reading filenotes");
          }
        }
        else if (comadd)
#endif /* AMIGA || MACOS */
        {
          if (noisy)
            fprintf(mesg, "Enter comment for %s:\n", z->oname);
          if (fgets(e, MAXCOM+1, comment_stream) != NULL)
          {
            if ((p = malloc((extent)(k = strlen(e))+1)) == NULL)
            {
              free((zvoid *)e);
              ZIPERR(ZE_MEM, "was reading comment lines");
            }
            strcpy(p, e);
            if (p[k-1] == '\n')
              p[--k] = 0;
            z->comment = p;
            /* zip64 support 09/05/2003 R.Nausedat */
            z->com = (extent)k;
          }
        }
#ifdef MACOS
    if (noisy) fprintf(mesg, "\n...done");
#endif
#if defined(AMIGA) || defined(MACOS)
    if (comadd)
      free((zvoid *)e);
    GetComment(NULL);           /* makes it free its internal storage */
#else
    free((zvoid *)e);
#endif
  }

  /* Get multi-line comment for the zip file */
  if (zipedit)
  {
#ifndef WINDLL
    if (comment_stream == NULL) {
#ifndef RISCOS
      comment_stream = (FILE*)fdopen(fileno(stderr), "r");
#else
      comment_stream = stderr;
#endif
    }
    if ((e = malloc(MAXCOM + 1)) == NULL) {
      ZIPERR(ZE_MEM, "was reading comment lines");
    }
    if (noisy && zcomlen)
    {
      fputs("current zip file comment is:\n", mesg);
      fwrite(zcomment, 1, zcomlen, mesg);
      if (zcomment[zcomlen-1] != '\n')
        putc('\n', mesg);
      free((zvoid *)zcomment);
    }
    if ((zcomment = malloc(1)) == NULL)
      ZIPERR(ZE_MEM, "was setting comments to null");
    zcomment[0] = '\0';
    if (noisy)
      fputs("enter new zip file comment (end with .):\n", mesg);
#if (defined(AMIGA) && (defined(LATTICE)||defined(__SASC)))
    flushall();  /* tty input/output is out of sync here */
#endif
#ifdef __human68k__
    setmode(fileno(comment_stream), O_TEXT);
#endif
#ifdef MACOS
    printf("\n enter new zip file comment \n");
    if (fgets(e, MAXCOM+1, comment_stream) != NULL) {
        if ((p = malloc((k = strlen(e))+1)) == NULL) {
            free((zvoid *)e);
            ZIPERR(ZE_MEM, "was reading comment lines");
        }
        strcpy(p, e);
        if (p[k-1] == '\n') p[--k] = 0;
        zcomment = p;
    }
#else /* !MACOS */
    while (fgets(e, MAXCOM+1, comment_stream) != NULL && strcmp(e, ".\n"))
    {
      if (e[(r = strlen(e)) - 1] == '\n')
        e[--r] = 0;
      if ((p = malloc((*zcomment ? strlen(zcomment) + 3 : 1) + r)) == NULL)
      {
        free((zvoid *)e);
        ZIPERR(ZE_MEM, "was reading comment lines");
      }
      if (*zcomment)
        strcat(strcat(strcpy(p, zcomment), "\r\n"), e);
      else
        strcpy(p, *e ? e : "\r\n");
      free((zvoid *)zcomment);
      zcomment = p;
    }
#endif /* ?MACOS */
    free((zvoid *)e);
#else /* WINDLL */
    comment(zcomlen);
    if ((p = malloc(strlen(szCommentBuf)+1)) == NULL) {
      ZIPERR(ZE_MEM, "was setting comments to null");
    }
    if (szCommentBuf[0] != '\0')
       lstrcpy(p, szCommentBuf);
    else
       p[0] = '\0';
    free((zvoid *)zcomment);
    GlobalUnlock(hStr);
    GlobalFree(hStr);
    zcomment = p;
#endif /* WINDLL */
    zcomlen = strlen(zcomment);
  }

  if (display_globaldots) {
#ifndef WINDLL
    putc('\n', mesg);
#else
    fprintf(stdout,"%c",'\n');
#endif
    mesg_line_started = 0;
  }

  /* Write central directory and end header to temporary zip */
  if (show_what_doing) {
    fprintf(mesg, "sd: Writing central directory\n");
    fflush(mesg);
  }
  diag("writing central directory");
  k = 0;                        /* keep count for end header */
  c = tempzn;                   /* get start of central */
  n = t = 0;
  for (z = zfiles; z != NULL; z = z->nxt)
  {
    if (z->mark || !(diff_mode || filesync)) {
      if ((r = putcentral(z)) != ZE_OK) {
        ZIPERR(r, tempzip);
      }
      tempzn += 4 + CENHEAD + z->nam + z->cext + z->com;
      n += z->len;
      t += z->siz;
      k++;
    }
  }

  if (k == 0)
    zipwarn("zip file empty", "");
  if (verbose) {
    fprintf(mesg, "total bytes=%s, compressed=%s -> %d%% savings\n",
            zip_fzofft(n, NULL, "u"), zip_fzofft(t, NULL, "u"), percent(n, t));
    fflush(mesg);
  }
  if (logall) {
    fprintf(logfile, "total bytes=%s, compressed=%s -> %d%% savings\n",
            zip_fzofft(n, NULL, "u"), zip_fzofft(t, NULL, "u"), percent(n, t));
    fflush(logfile);
  }
  t = tempzn - c;               /* compute length of central */
  diag("writing end of central directory");
  if (show_what_doing) {
    fprintf(mesg, "sd: Writing end of central directory\n");
    fflush(mesg);
  }

  if ((r = putend(k, t, c, zcomlen, zcomment)) != ZE_OK) {
    ZIPERR(r, tempzip);
  }

  /*
  tempzf = NULL;
  */
  if (fclose(y)) {
    ZIPERR(d ? ZE_WRITE : ZE_TEMP, tempzip);
  }
  y = NULL;
  if (in_file != NULL) {
    fclose(in_file);
    in_file = NULL;
  }
  /*
  if (x != NULL)
    fclose(x);
  */

  /* Free some memory before spawning unzip */
#ifdef USE_ZLIB
  zl_deflate_free();
#else
  lm_free();
#endif
#ifdef BZIP2_SUPPORT
  bz_compress_free();
#endif

#ifndef WINDLL
  /* Test new zip file before overwriting old one or removing input files */
  if (test)
    check_zipfile(tempzip, argv[0]);
#endif
  /* Replace old zip file with new zip file, leaving only the new one */
  if (strcmp(zipfile, "-") && !d)
  {
    diag("replacing old zip file with new zip file");
    if (show_what_doing) {
      fprintf(mesg, "sd: Replacing old zip file\n");
      fflush(mesg);
    }
    if ((r = replace(out_path, tempzip)) != ZE_OK)
    {
      zipwarn("new zip file left as: ", tempzip);
      free((zvoid *)tempzip);
      tempzip = NULL;
      ZIPERR(r, "was replacing the original zip file");
    }
    free((zvoid *)tempzip);
  }
  tempzip = NULL;
  if (zip_attributes && strcmp(zipfile, "-")) {
    setfileattr(out_path, zip_attributes);
#ifdef VMS
    /* If the zip file existed previously, restore its record format: */
    if (x != NULL)
      (void)VMSmunch(out_path, RESTORE_RTYPE, NULL);
#endif
  }
  if (strcmp(zipfile, "-")) {
    if (show_what_doing) {
      fprintf(mesg, "sd: Setting file type\n");
      fflush(mesg);
    }

    set_filetype(out_path);
  }

#if defined(WIN32)
  /* All looks good so, if requested, clear the DOS archive bits */
  if (clear_archive_bits) {
    if (noisy)
      zipmessage("Clearing archive bits...", "");
    for (z = zfiles; z != NULL; z = z->nxt)
    {
# ifdef UNICODE_SUPPORT
      if (z->mark) {
        if (!no_win32_wide) {
          if (!ClearArchiveBitW(z->namew)){
            zipwarn("Could not clear archive bit for: ", z->oname);
          }
        } else {
          if (!ClearArchiveBit(z->name)){
            zipwarn("Could not clear archive bit for: ", z->oname);
          }
        }
      }
# else
      if (!ClearArchiveBit(z->name)){
        zipwarn("Could not clear archive bit for: ", z->oname);
      }
# endif
    }
  }
#endif

  /* finish logfile (it gets closed in freeup() called by finish()) */
  if (logfile) {
      struct tm *now;
      time_t clocktime;

      fprintf(logfile, "\nTotal %ld entries (", files_total);
      if (good_bytes_so_far != bytes_total) {
        fprintf(logfile, "planned ");
        DisplayNumString(logfile, bytes_total);
        fprintf(logfile, " bytes, actual ");
        DisplayNumString(logfile, good_bytes_so_far);
        fprintf(logfile, " bytes)");
      } else {
        DisplayNumString(logfile, bytes_total);
        fprintf(logfile, " bytes)");
      }

      /* get current time */

      time(&clocktime);
      now = localtime(&clocktime);
      fprintf(logfile, "\nDone %s", asctime(now));
  }

  /* Finish up (process -o, -m, clean up).  Exit code depends on o. */
#if (!defined(VMS) && !defined(CMS_MVS))
  free((zvoid *) zipbuf);
#endif /* !VMS && !CMS_MVS */
  RETURN(finish(o ? ZE_OPEN : ZE_OK));
}
