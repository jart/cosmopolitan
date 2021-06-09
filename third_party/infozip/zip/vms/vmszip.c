/*
  Copyright (c) 1990-2007 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/

/* 2005-02-14 SMS.
   Added some ODS5 support.
      Use longer name structures in NAML, where available.
      Locate special characters mindful of "^" escapes.
      Replaced compile-time case preservation (VMS_PRESERVE_CASE macro)
      with command-line-specified case preservation (vms_case_x
      variables).
   Prototyped all functions.
   Removed "#ifndef UTIL", as no one should be compiling it that way.
*/

#include "zip.h"
#include "vmsmunch.h"
#include "vms.h"

#include <ctype.h>
#include <time.h>
#include <unixlib.h>

/* Judge availability of str[n]casecmp() in C RTL.
   (Note: This must follow a "#include <decc$types.h>" in something to
   ensure that __CRTL_VER is as defined as it will ever be.  DEC C on
   VAX may not define it itself.)
*/
#ifdef __CRTL_VER
#if __CRTL_VER >= 70000000
#define HAVE_STRCASECMP
#endif /* __CRTL_VER >= 70000000 */
#endif /* def __CRTL_VER */

#ifdef HAVE_STRCASECMP
#include <strings.h>    /* str[n]casecmp() */
#endif /* def HAVE_STRCASECMP */

#include <dvidef.h>
#include <lib$routines.h>
#include <ssdef.h>
#include <stsdef.h>
#include <starlet.h>

/* Directory file type with version, and its strlen(). */
#define DIR_TYPE_VER ".DIR;1"
#define DIR_TYPE_VER_LEN (sizeof( DIR_TYPE_VER)- 1)

/* Extra malloc() space in names for cutpath().  (May have to change
   ".FOO]" to "]FOO.DIR;1".)
*/
#define DIR_PAD (DIR_TYPE_VER_LEN- 1)

/* Hex digit table. */

char hex_digit[ 16] = {
 '0', '1', '2', '3', '4', '5', '6', '7',
 '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

/* Character property table for (re-)escaping ODS5 extended file names.
   Note that this table ignore Unicode, and does not identify invalid
   characters.

   ODS2 valid characters: 0-9 A-Z a-z $ - _

   ODS5 Invalid characters:
      C0 control codes (0x00 to 0x1F inclusive)
      Asterisk (*)
      Question mark (?)

   ODS5 Invalid characters only in VMS V7.2 (which no one runs, right?):
      Double quotation marks (")
      Backslash (\)
      Colon (:)
      Left angle bracket (<)
      Right angle bracket (>)
      Slash (/)
      Vertical bar (|)

   Characters escaped by "^":
      SP  !  #  %  &  '  (  )  +  ,  .  ;  =  @  [  ]  ^  `  {  }  ~

   Either "^_" or "^ " is accepted as a space.  Period (.) is a special
   case.  Note that un-escaped < and > can also confuse a directory
   spec.

   Characters put out as ^xx:
      7F (DEL)
      80-9F (C1 control characters)
      A0 (nonbreaking space)
      FF (Latin small letter y diaeresis)

   Other cases:
      Unicode: "^Uxxxx", where "xxxx" is four hex digits.

    Property table values:
      Normal escape:    1
      Space:            2
      Dot:              4
      Hex-hex escape:   8
      -------------------
      Hex digit:       64
*/

unsigned char char_prop[ 256] = {

/* NUL SOH STX ETX EOT ENQ ACK BEL   BS  HT  LF  VT  FF  CR  SO  SI */
    0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,

/* DLE DC1 DC2 DC3 DC4 NAK SYN ETB  CAN  EM SUB ESC  FS  GS  RS  US */
    0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,

/*  SP  !   "   #   $   %   &   '    (   )   *   +   ,   -   .   /  */
    2,  1,  0,  1,  0,  1,  1,  1,   1,  1,  0,  1,  1,  0,  4,  0,

/*  0   1   2   3   4   5   6   7    8   9   :   ;   <   =   >   ?  */
   64, 64, 64, 64, 64, 64, 64, 64,  64, 64,  0,  1,  1,  1,  1,  1,

/*  @   A   B   C   D   E   F   G    H   I   J   K   L   M   N   O  */
    1, 64, 64, 64, 64, 64, 64,  0,   0,  0,  0,  0,  0,  0,  0,  0,

/*  P   Q   R   S   T   U   V   W    X   Y   Z   [   \   ]   ^   _  */
    0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  1,  0,  1,  1,  0,

/*  `   a   b   c   d   e   f   g    h   i   j   k   l   m   n   o  */
    1, 64, 64, 64, 64, 64, 64,  0,   0,  0,  0,  0,  0,  0,  0,  0,

/*  p   q   r   s   t   u   v   w    x   y   z   {   |   }   ~  DEL */
    0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  1,  0,  1,  1,  8,

    8,  8,  8,  8,  8,  8,  8,  8,   8,  8,  8,  8,  8,  8,  8,  8,
    8,  8,  8,  8,  8,  8,  8,  8,   8,  8,  8,  8,  8,  8,  8,  8,
    8,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  8
};

/* The C RTL from OpenVMS 7.0 and newer supplies POSIX compatible versions of
 * opendir() et al. Thus, we have to use other names in our private code for
 * directory scanning to prevent symbol name conflicts at link time.
 * For now, we do not use the library supplied "dirent.h" functions, since
 * our private implementation provides some functionality which may not be
 * present in the library versions.  For example:
 * ==> zopendir("DISK:[DIR.SUB1]SUB2.DIR") scans "DISK:[DIR.SUB1.SUB2]".
 */

typedef struct zdirent {
  int d_wild;                /* flag for wildcard vs. non-wild */
  struct FAB fab;
  struct NAM_STRUCT nam;
  char d_qualwildname[ NAM_MAXRSS+ 1];
  char d_name[ NAM_MAXRSS+ 1];
} zDIR;

extern char *label;
local ulg label_time = 0;
local ulg label_mode = 0;
local time_t label_utim = 0;

local int relative_dir_s = 0;   /* Relative directory spec. */

/* Local functions */
local void vms_wild OF((char *, zDIR *));
local zDIR *zopendir OF((ZCONST char *));
local char *readd OF((zDIR *));
local char *strlower OF((char *));
local char *strupper OF((char *));

/* 2004-09-25 SMS.
   str[n]casecmp() replacement for old C RTL.
   Assumes a prehistorically incompetent toupper().
*/
#ifndef HAVE_STRCASECMP

int strncasecmp( char *s1, char *s2, size_t n)
{
  /* Initialization prepares for n == 0. */
  char c1 = '\0';
  char c2 = '\0';

  while (n-- > 0)
  {
    /* Set c1 and c2.  Convert lower-case characters to upper-case. */
    if (islower( c1 = *s1))
      c1 = toupper( c1);

    if (islower( c2 = *s2))
      c2 = toupper( c2);

    /* Quit at inequality or NUL. */
    if ((c1 != c2) || (c1 == '\0'))
      break;

    s1++;
    s2++;
  }
return ((unsigned int) c1- (unsigned int) c2);
}

#ifndef UINT_MAX
#define UINT_MAX 4294967295U
#endif

#define strcasecmp( s1, s2) strncasecmp( s1, s2, UINT_MAX)

#endif /* ndef HAVE_STRCASECMP */


/* 2004-09-27 SMS.
   eat_carets().

   Delete ODS5 extended file name escape characters ("^") in the
   original buffer.
   Note that the current scheme does not handle all EFN cases, but it
   could be made more complicated.
*/

local void eat_carets( char *str)
/* char *str;      Source pointer. */
{
  char *strd;   /* Destination pointer. */
  char hdgt;
  unsigned char uchr;
  unsigned char prop;

  /* Skip ahead to the first "^", if any. */
  while ((*str != '\0') && (*str != '^'))
     str++;

  /* If no caret was found, quit early. */
  if (*str != '\0')
  {
    /* Shift characters leftward as carets are found. */
    strd = str;
    while (*str != '\0')
    {
      uchr = *str;
      if (uchr == '^')
      {
        /* Found a caret.  Skip it, and check the next character. */
        uchr = *(++str);
        prop = char_prop[ uchr];
        if (prop& 64)
        {
          /* Hex digit.  Get char code from this and next hex digit. */
          if (uchr <= '9')
          {
            hdgt = uchr- '0';           /* '0' - '9' -> 0 - 9. */
          }
          else
          {
            hdgt = ((uchr- 'A')& 7)+ 10;    /* [Aa] - [Ff] -> 10 - 15. */
          }
          hdgt <<= 4;                   /* X16. */
          uchr = *(++str);              /* Next char must be hex digit. */
          if (uchr <= '9')
          {
            uchr = hdgt+ uchr- '0';
          }
          else
          {
            uchr = hdgt+ ((uchr- 'A')& 15)+ 10;
          }
        }
        else if (uchr == '_')
        {
          /* Convert escaped "_" to " ". */
          uchr = ' ';
        }
        else if (uchr == '/')
        {
          /* Convert escaped "/" (invalid Zip) to "?" (invalid VMS). */
          uchr = '?';
        }
        /* Else, not a hex digit.  Must be a simple escaped character
           (or Unicode, which is not yet handled here).
        */
      }
      /* Else, not a caret.  Use as-is. */
      *strd = uchr;

      /* Advance destination and source pointers. */
      strd++;
      str++;
    }
    /* Terminate the destination string. */
    *strd = '\0';
  }
}


/* 2007-05-22 SMS.
 * explicit_dev().
 *
 * Determine if an explicit device name is present in a (VMS) file
 * specification.
 */
local int explicit_dev( char *file_spec)
{
  int sts;
  struct FAB fab;               /* FAB. */
  struct NAM_STRUCT nam;        /* NAM[L]. */

  /* Initialize the FAB and NAM[L], and link the NAM[L] to the FAB. */
  nam = CC_RMS_NAM;
  fab = cc$rms_fab;
  fab.FAB_NAM = &nam;

  /* Point the FAB/NAM[L] fields to the actual name and default name. */

#ifdef NAML$C_MAXRSS

  fab.fab$l_dna = (char *) -1;  /* Using NAML for default name. */
  fab.fab$l_fna = (char *) -1;  /* Using NAML for file name. */

#endif /* def NAML$C_MAXRSS */

  /* File name. */
  FAB_OR_NAML( fab, nam).FAB_OR_NAML_FNA = file_spec;
  FAB_OR_NAML( fab, nam).FAB_OR_NAML_FNS = strlen( file_spec);

  nam.NAM_NOP = NAM_M_SYNCHK;   /* Syntax-only analysis. */
  sts = sys$parse( &fab, 0, 0); /* Parse the file spec. */

  /* Device found = $PARSE success and "device was explicit" flag. */
  return (((sts& STS$M_SEVERITY) == STS$M_SUCCESS) &&
   ((nam.NAM_FNB& NAM_M_EXP_DEV) != 0));
}


/* 2005-02-04 SMS.
   find_dir().

   Find directory boundaries in an ODS2 or ODS5 file spec.
   Returns length (zero if no directory, negative if error),
   and sets "start" argument to first character (typically "[") location.

   No one will care about the details, but the return values are:

       0  No dir.
      -2  [, no end.              -3  <, no end.
      -4  [, multiple start.      -5  <, multiple start.
      -8  ], no start.            -9  >, no start.
     -16  ], wrong end.          -17  >, wrong end.
     -32  ], multiple end.       -33  >, multiple end.

   Note that the current scheme handles only simple EFN cases, but it
   could be made more complicated.
*/
int find_dir( char *file_spec, char **start)
{
  char *cp;
  char chr;

  char *end_tmp = NULL;
  char *start_tmp = NULL;
  int lenth = 0;

  for (cp = file_spec; cp < file_spec+ strlen( file_spec); cp++)
  {
    chr = *cp;
    if (chr == '^')
    {
      /* Skip ODS5 extended name escaped characters. */
      cp++;
      /* If escaped char is a hex digit, skip the second hex digit, too. */
      if (char_prop[ (unsigned char) *cp]& 64)
        cp++;
    }
    else if (chr == '[')
    {
      /* Found start. */
      if (start_tmp == NULL)
      {
        /* First time.  Record start location. */
        start_tmp = cp;
        /* Error if no end. */
        lenth = -2;
      }
      else
      {
        /* Multiple start characters.  */
        lenth = -4;
        break;
      }
    }
    else if (chr == '<')
    {
      /* Found start. */
      if (start_tmp == NULL)
      {
        /* First time.  Record start location. */
        start_tmp = cp;
        /* Error if no end. */
        lenth = -3;
      }
      else
      {
        /* Multiple start characters.  */
        lenth = -5;
        break;
      }
    }
    else if (chr == ']')
    {
      /* Found end. */
      if (end_tmp == NULL)
      {
        /* First time. */
        if (lenth == 0)
        {
          /* End without start. */
          lenth = -8;
          break;
        }
        else if (lenth != -2)
        {
          /* Wrong kind of end. */
          lenth = -16;
          break;
        }
        /* End ok.  Record end location. */
        end_tmp = cp;
        lenth = end_tmp+ 1- start_tmp;
        /* Could break here, ignoring excessive end characters. */
      }
      else
      {
        /* Multiple end characters. */
        lenth = -32;
        break;
      }
    }
    else if (chr == '>')
    {
      /* Found end. */
      if (end_tmp == NULL)
      {
        /* First time. */
        if (lenth == 0)
        {
          /* End without start. */
          lenth = -9;
          break;
        }
        else if (lenth != -3)
        {
          /* Wrong kind of end. */
          lenth = -17;
          break;
        }
        /* End ok.  Record end location. */
        end_tmp = cp;
        lenth = end_tmp+ 1- start_tmp;
        /* Could break here, ignoring excessive end characters. */
      }
      else
      {
        /* Multiple end characters. */
        lenth = -33;
        break;
      }
    }
  }

  /* If both start and end were found,
     then set result pointer where safe.
  */
  if (lenth > 0)
  {
    if (start != NULL)
    {
      *start = start_tmp;
    }
  }
  return lenth;
}


/* 2005-02-08 SMS.
   file_sys_type().

   Determine the file system type for the (VMS) path name argument.
*/
local int file_sys_type( char *path)
{
  int acp_code;

#ifdef DVI$C_ACP_F11V5

/* Should know about ODS5 file system.  Do actual check.
   (This should be non-VAX with __CRTL_VER >= 70200000.)
*/

  int sts;

  struct dsc$descriptor_s dev_descr =
   { 0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0 };

  /* Load path argument into device descriptor. */
  dev_descr.dsc$a_pointer = path;
  dev_descr.dsc$w_length = strlen( dev_descr.dsc$a_pointer);

  /* Get filesystem type code.
     (Text results for this item code have been unreliable.)
  */
  sts = lib$getdvi( &((int) DVI$_ACPTYPE), 0, &dev_descr, &acp_code, 0, 0);

  if ((sts & STS$M_SUCCESS) != STS$K_SUCCESS)
  {
    acp_code = -1;
  }

#else /* def DVI$C_ACP_F11V5 */

/* Too old for ODS5 file system.  Must be ODS2. */

  acp_code = DVI$C_ACP_F11V2;

#endif /* def DVI$C_ACP_F11V5 */

  return acp_code;
}

/*---------------------------------------------------------------------------

    _vms_findfirst() and _vms_findnext(), based on public-domain DECUS C
    fwild() and fnext() routines (originally written by Martin Minow, poss-
    ibly modified by Jerry Leichter for bintnxvms.c), were written by Greg
    Roelofs and are still in the public domain.  Routines approximate the
    behavior of MS-DOS (MSC and Turbo C) findfirst and findnext functions.

    2005-01-04 SMS.
    Changed to use NAML instead of NAM, where available.

  ---------------------------------------------------------------------------*/

static char wild_version_part[10]="\0";

local void vms_wild( char *p, zDIR *d)
{
  /*
   * Do wildcard setup.
   */
  /* Set up the FAB and NAM[L] blocks. */
  d->fab = cc$rms_fab;                  /* Initialize FAB. */
  d->nam = CC_RMS_NAM;                  /* Initialize NAM[L]. */

  d->fab.FAB_NAM = &d->nam;             /* FAB -> NAM[L] */

#ifdef NAML$C_MAXRSS

  d->fab.fab$l_dna =(char *) -1;        /* Using NAML for default name. */
  d->fab.fab$l_fna = (char *) -1;       /* Using NAML for file name. */

#endif /* def NAML$C_MAXRSS */

  /* Argument file name and length. */
  d->FAB_OR_NAML( fab, nam).FAB_OR_NAML_FNA = p;
  d->FAB_OR_NAML( fab, nam).FAB_OR_NAML_FNS = strlen(p);

#define DEF_DEVDIR "SYS$DISK:[]"

  /* Default file spec and length. */
  d->FAB_OR_NAML( fab, nam).FAB_OR_NAML_DNA = DEF_DEVDIR;
  d->FAB_OR_NAML( fab, nam).FAB_OR_NAML_DNS = sizeof( DEF_DEVDIR)- 1;

  d->nam.NAM_ESA = d->d_qualwildname;   /* qualified wild name */
  d->nam.NAM_ESS = NAM_MAXRSS;          /* max length */
  d->nam.NAM_RSA = d->d_name;           /* matching file name */
  d->nam.NAM_RSS = NAM_MAXRSS;          /* max length */

  /* parse the file name */
  if (sys$parse(&d->fab) != RMS$_NORMAL)
    return;
  /* Does this replace d->fab.fab$l_fna with a new string in its own space?
     I sure hope so, since p is free'ed before this routine returns. */

  /* have qualified wild name (i.e., disk:[dir.subdir]*.*); null-terminate
   * and set wild-flag */
  d->d_qualwildname[d->nam.NAM_ESL] = '\0';
  d->d_wild = (d->nam.NAM_FNB & NAM$M_WILDCARD)? 1 : 0;   /* not used... */
#ifdef DEBUG
  fprintf(mesg, "  incoming wildname:  %s\n", p);
  fprintf(mesg, "  qualified wildname:  %s\n", d->d_qualwildname);
#endif /* DEBUG */
}

local zDIR *zopendir( ZCONST char *n)
/* ZCONST char *n;         directory to open */
/* Start searching for files in the VMS directory n */
{
  char *c;              /* scans VMS path */
  zDIR *d;              /* malloc'd return value */
  int m;                /* length of name */
  char *p;              /* malloc'd temporary string */

  if ((d = (zDIR *)malloc(sizeof(zDIR))) == NULL ||
      (p = malloc((m = strlen(n)) + 4)) == NULL) {
    if (d != NULL) free((zvoid *)d);
    return NULL;
  }
  /* Directory may be in form "[DIR.SUB1.SUB2]" or "[DIR.SUB1]SUB2.DIR;1".
     If latter, convert to former.
     2005-01-31 SMS.  Changed to require ";1", as VMS does, which
     simplified the code slightly, too.  Note that ODS5 allows ".DIR" in
     any case (upper, lower, mixed).
  */
  if ((m > 0) && (*(c = strcpy(p,n)+m-1) != ']'))
  {
    if ((c- p < DIR_TYPE_VER_LEN) ||
     strcasecmp((c+ 1- DIR_TYPE_VER_LEN), DIR_TYPE_VER))
    {
      free((zvoid *)d);  free((zvoid *)p);
      return NULL;
    }
    c -= 4;             /* The "D". */
    *c-- = '\0';        /* terminate at "DIR;1" */
    *c = ']';           /* "." --> "]" */

    /* Replace the formerly last "]" with ".".
       For ODS5, ignore "^]".
    */
    while ((c > p) && ((*--c != ']') || (*(c- 1) == '^')))
      ;
    *c = '.';           /* "]" --> "." */
  }
  strcat(p, "*.*");
  strcat(p, wild_version_part);
  vms_wild(p, d);       /* set up wildcard */
  free((zvoid *)p);
  return d;
}

local char *readd( zDIR *d)
/* zDIR *d;                directory stream to read from */
/* Return a pointer to the next name in the directory stream d, or NULL if
   no more entries or an error occurs. */
{
  int r;                /* return code */

  do {
    d->fab.fab$w_ifi = 0;       /* internal file index:  what does this do? */
/*
  2005-02-04 SMS.

  From the docs:

        Note that you must close the file before invoking the Search
        service (FAB$W_IFI must be 0). 

  The same is true for PARSE.  Most likely, it's cleared by setting
  "fab = cc$rms_fab", and left that way, so clearing it here may very
  well be pointless.  (I think it is, and I've never seen it explicitly
  cleared elsewhere, but I haven't tested it everywhere either.)
*/
    /* get next match to possible wildcard */
    if ((r = sys$search(&d->fab)) == RMS$_NORMAL)
    {
        d->d_name[d->nam.NAM_RSL] = '\0';   /* null terminate */
        return (char *)d->d_name;   /* OK */
    }
  } while (r == RMS$_PRV);
  return NULL;
}


int wild( char *p)
/* char *p;                path/pattern to match */
/* Expand the pattern based on the contents of the file system.
   Return an error code in the ZE_ class.
   Note that any command-line file argument may need wildcard expansion,
   so all user-specified constituent file names pass through here.
*/
{
  zDIR *d;              /* stream for reading directory */
  char *e;              /* name found in directory */
  int f;                /* true if there was a match */

  int dir_len;          /* Length of the directory part of the name. */
  char *dir_start;      /* First character of the directory part. */

  /* special handling of stdin request */
  if (strcmp(p, "-") == 0)   /* if compressing stdin */
    return newname(p, 0, 0);

  /* Determine whether this name has an absolute or relative directory
     spec.  It's relative if there is no directory, or if the directory
     has a leading dot ("[.").
  */
  dir_len = find_dir( p, &dir_start);
  relative_dir_s = ((dir_len <= 0)? 1 : (dir_start[ 1] == '.'));

  /* Search given pattern for matching names */
  if ((d = (zDIR *)malloc(sizeof(zDIR))) == NULL)
    return ZE_MEM;
  vms_wild(p, d);       /* pattern may be more than just directory name */

  /*
   * Save version specified by user to use in recursive drops into
   * subdirectories.
   */
  strncpy(wild_version_part, d->nam.NAM_L_VER, d->nam.NAM_B_VER);
  wild_version_part[d->nam.NAM_B_VER] = '\0';

  f = 0;
  while ((e = readd(d)) != NULL)        /* "dosmatch" is already built in */
    if (procname(e, 0) == ZE_OK)
      f = 1;
  free(d);

  /* Done */
  return f ? ZE_OK : ZE_MISS;
}

int procname( char *n, int caseflag)
/* char *n;                name to process */
/* int caseflag;           true to force case-sensitive match */
/* Process a name or sh expression to operate on (or exclude).  Return
   an error code in the ZE_ class. */
{
  zDIR *d;              /* directory stream from zopendir() */
  char *e;              /* pointer to name from readd() */
  int m;                /* matched flag */
  char *p;              /* path for recursion */
  struct stat s;        /* result of stat() */
  struct zlist far *z;  /* steps through zfiles list */

  if (strcmp(n, "-") == 0)   /* if compressing stdin */
    return newname(n, 0, caseflag);
  else if (LSSTAT(n, &s)
#if defined(__TURBOC__) || defined(VMS) || defined(__WATCOMC__)
           /* For these 3 compilers, stat() succeeds on wild card names! */
           || isshexp(n)
#endif
          )
  {
    /* Not a file or directory--search for shell expression in zip file */
    if (caseflag) {
      p = malloc(strlen(n) + 1);
      if (p != NULL)
        strcpy(p, n);
    } else
      p = ex2in(n, 0, (int *)NULL);     /* shouldn't affect matching chars */
    m = 1;
    for (z = zfiles; z != NULL; z = z->nxt) {
      if (MATCH(p, z->iname, caseflag))
      {
        z->mark = pcount ? filter(z->zname, caseflag) : 1;
        if (verbose)
            fprintf(mesg, "zip diagnostic: %scluding %s\n",
               z->mark ? "in" : "ex", z->name);
        m = 0;
      }
    }
    free((zvoid *)p);
    return m ? ZE_MISS : ZE_OK;
  }

  /* Live name--use if file, recurse if directory */
  if ((s.st_mode & S_IFDIR) == 0)
  {
    /* add or remove name of file */
    if ((m = newname(n, 0, caseflag)) != ZE_OK)
      return m;
  } else {
    if (dirnames && (m = newname(n, 1, caseflag)) != ZE_OK) {
      return m;
    }
    /* recurse into directory */
    if (recurse && (d = zopendir(n)) != NULL)
    {
      while ((e = readd(d)) != NULL) {
        if ((m = procname(e, caseflag)) != ZE_OK)     /* recurse on name */
        {
          free(d);
          return m;
        }
      }
      free(d);
    }
  } /* (s.st_mode & S_IFDIR) == 0) */
  return ZE_OK;
}

/* 2004-09-24 SMS.
   Cuter strlower() and strupper() functions.
*/

local char *strlower( char *s)
/* Convert all uppercase letters to lowercase in string s */
{
  for ( ; *s != '\0'; s++)
    if (isupper( *s))
      *s = tolower( *s);

  return s;
}

local char *strupper( char *s)
/* Convert all lowercase letters to uppercase in string s */
{
  for ( ; *s != '\0'; s++)
    if (islower( *s))
      *s = toupper( *s);

  return s;
}

char *ex2in( char *x, int isdir, int *pdosflag)
/* char *x;                external file name */
/* int isdir;              input: x is a directory */
/* int *pdosflag;          output: force MSDOS file attributes? */

/* Convert the external file name to a zip file name, returning the
   malloc'ed string or NULL if not enough memory.

   2005-02-09 SMS.
   Added some ODS5 support.

   Note that if we were really clever, we'd save the truncated original
   file name for later use as "iname", instead of running the de-escaped
   product back through in2ex() to recover it later.

   2005-11-13 SMS.
   Changed to translate "[..." into enough "/" characters to cause
   in2ex() to reconstruct it.  This should not be needed, however, as
   pattern matching really should avoid ex2in() and in2ex().
*/
{
  char *n;                      /* Internal file name (malloc'ed). */
  char *nn;                     /* Temporary "n"-like pointer. */
  char *ext_dir_and_name;       /* External dir]name (less "dev:["). */
  char chr;                     /* Temporary character storage. */
  int dosflag;
  int down_case;                /* Resultant down-case flag. */
  int dir_len;                  /* Directory spec length. */
  int ods_level;                /* File system type. */

  dosflag = dosify; /* default for non-DOS and non-OS/2 */

  /* Locate the directory part of the external name. */
  dir_len = find_dir( x, &ext_dir_and_name);
  if (dir_len <= 0)
  {
    /* Directory not found.  Use whole external name. */
    ext_dir_and_name = x;
  }
  else if (pathput)
  {
    /* Include directory. */
    if (ext_dir_and_name[ 1] == '.')
    {
      /* Relative path.  If not a directory-depth wildcard, then drop
         first "[." (or "<.").  If "[..." (or "<..."), then preserve all
         characters, including the first "[" (or "<") for special
         handling below.
      */
      if ((ext_dir_and_name[ 2] != '.') || (ext_dir_and_name[ 3] != '.'))
      {
        /* Normal relative path.  Drop first "[." (or "<."). */
        dir_len -= 2;
        ext_dir_and_name += 2;
      }
    }
    else
    {
      /* Absolute path.  Skip first "[" (or "<"). */
      dir_len -= 1;
      ext_dir_and_name += 1;

      /* 2007-04-26 SMS.
         Skip past "000000." or "000000]" (or "000000>"), which should
         not be stored in the archive.  This arises, for example, with
         "zip -r archive [000000]foo.dir"
      */
#define MFD "000000"

      if ((strncmp( ext_dir_and_name, MFD, strlen( MFD)) == 0) &&
       ((ext_dir_and_name[ 6] == '.') ||
       (ext_dir_and_name[ 6] == ']') ||
       (ext_dir_and_name[ 6] == '>')))
      {
        dir_len -= 7;
        ext_dir_and_name += 7;
      } 
    }
  }
  else
  {
    /* Junking paths.  Skip the whole directory spec. */
    ext_dir_and_name += dir_len;
    dir_len = 0;
  }

  /* Malloc space for internal name and copy it. */
  if ((n = malloc(strlen( ext_dir_and_name)+ 1)) == NULL)
    return NULL;
  strcpy( n, ext_dir_and_name);

  /* Convert VMS directory separators (".") to "/". */
  if (dir_len > 0)
  {
    for (nn = n; nn < n+ dir_len; nn++)
    {
      chr = *nn;
      if (chr == '^')
      {
        /* Skip ODS5 extended name escaped characters. */
        nn++;
        /* If escaped char is a hex digit, skip the second hex digit, too. */
        if (char_prop[ (unsigned char) *nn]& 64)
          nn++;
      }
      else if ((chr == '.') || ((nn == n) && ((chr == '[') || (chr == '<'))))
      {
        /* Convert VMS directory separator (".", or initial "[" or "<"
           of "[..." or "<...") to "/".
        */
        *nn = '/';
      }
    }
    /* Replace directory end character (typically "]") with "/". */
    n[ dir_len- 1] = '/';
  }

  /* If relative path, then strip off the current directory. */
  if (relative_dir_s)
  {
    char cwd[ NAM_MAXRSS+ 1];
    char *cwd_dir_only;
    char *q;
    int cwd_dir_only_len;

    q = getcwd( cwd, (sizeof( cwd)- 1));

    /* 2004-09-24 SMS.
       With SET PROCESSS /PARSE = EXTENDED, getcwd() can return a
       mixed-case result, confounding the comparisons below with an
       all-uppercase name in "n".  Always use a case-insensitive
       comparison around here.
    */

    /* Locate the directory part of the external name. */
    dir_len = find_dir( q, &cwd_dir_only);
    if (dir_len > 0)
    {
      /* Skip first "[" (or "<"). */
      cwd_dir_only++;
      /* Convert VMS directory separators (".") to "/". */
      for (q = cwd_dir_only; q < cwd_dir_only+ dir_len; q++)
      {
        chr = *q;
        if (chr == '^')
        {
          /* Skip ODS5 extended name escaped characters. */
          q++;
          /* If escaped char is a hex digit, skip the second hex digit, too. */
          if (char_prop[ (unsigned char) *q]& 64)
            q++;
        }
        else if (chr == '.')
        {
          /* Convert VMS directory separator (".") to "/". */
          *q = '/';
        }
      }
      /* Replace directory end character (typically "]") with "/". */
      cwd_dir_only[ dir_len- 2] = '/';
    }

    /* If the slash-converted cwd matches the front of the internal
       name, then shuffle the remainder of the internal name to the
       beginning of the internal name storage.

       Because we already know that the path is relative, this test may
       always succeed.
    */
    cwd_dir_only_len = strlen( cwd_dir_only);
    if (strncasecmp( n, cwd_dir_only, cwd_dir_only_len) == 0)
    {
       nn = n+ cwd_dir_only_len;
       q = n;
       while (*q++ = *nn++);
    }
  } /* (relative_dir_s) */

  /* 2007-05-22 SMS.
   * If a device name is present, assume that it's a real (VMS) file
   * specification, and do down-casing according to the ODS2 or ODS5
   * down-casing policy.  If no device name is present, assume that it's
   * a pattern ("-i", ...), and do no down-casing here.  (Case
   * sensitivity in patterns is handled elsewhere.)
   */
  if (explicit_dev( x))
  {
    /* If ODS5 is possible, do complicated down-case check.

       Note that the test for ODS2/ODS5 is misleading and over-broad. 
       Here, "ODS2" includes anything from DVI$C_ACP_F11V1 (=1, ODS1) up
       to (but not including) DVI$C_ACP_F11V5 (= 11, DVI$C_ACP_F11V5),
       while "ODS5" includes anything from DVI$C_ACP_F11V5 on up.  See
       DVIDEF.H.
    */

#if defined( DVI$C_ACP_F11V5) && defined( NAML$C_MAXRSS)

    /* Check options and/or ODS level for down-case or preserve case. */
    down_case = 0;      /* Assume preserve case. */
    if ((vms_case_2 <= 0) && (vms_case_5 < 0))
    {
      /* Always down-case. */
      down_case = 1;
    }
    else if ((vms_case_2 <= 0) || (vms_case_5 < 0))
    {
      /* Down-case depending on ODS level.  (Use (full) external name.) */
      ods_level = file_sys_type( x);

      if (ods_level > 0)
      {
        /* Valid ODS level.  (Name (full) contains device.)
         * Down-case accordingly.
         */
        if (((ods_level < DVI$C_ACP_F11V5) && (vms_case_2 <= 0)) ||
         ((ods_level >= DVI$C_ACP_F11V5) && (vms_case_5 < 0)))
        {
          /* Down-case for this ODS level. */
          down_case = 1;
        }
      }
    }

#else /* defined( DVI$C_ACP_F11V5) && defined( NAML$C_MAXRSS) */

/* No case-preserved names are possible (VAX).  Do simple down-case check. */

    down_case = (vms_case_2 <= 0);

#endif /* defined( DVI$C_ACP_F11V5) && defined( NAML$C_MAXRSS) [else] */

    /* If down-casing, convert to lower case. */
    if (down_case != 0)
    {
      strlower( n);
    }
  }

  /* Remove simple ODS5 extended file name escape characters. */
  eat_carets( n);

  if (isdir)
  {
    if (strcasecmp( (nn = n+ strlen( n)- DIR_TYPE_VER_LEN), DIR_TYPE_VER))
      error("directory not version 1");
    else
      if (pathput)
        strcpy( nn, "/");
      else
        *n = '\0';              /* directories are discarded with zip -rj */
  }
  else if (vmsver == 0)
  {
    /* If not keeping version numbers, truncate the name at the ";".
       (No escaped characters are expected in the version.)
    */
    if ((ext_dir_and_name = strrchr( n, ';')) != NULL)
      *ext_dir_and_name = '\0';
  }
  else if (vmsver > 1)
  {
    /* Keeping version numbers, but as ".nnn", not ";nnn". */
    if ((ext_dir_and_name = strrchr( n, ';')) != NULL)
      *ext_dir_and_name = '.';
  }

  /* Remove a type-less dot. */
  /* (Note that currently "name..ver" is not altered.) */
  if ((ext_dir_and_name = strrchr( n, '.')) != NULL)
  {
    if (ext_dir_and_name[ 1] == '\0')           /* "name." -> "name" */
      *ext_dir_and_name = '\0';
    else if (ext_dir_and_name[ 1] == ';')       /* "name.;ver" -> "name;ver" */
    {
      char *f = ext_dir_and_name+ 1;
      while (*ext_dir_and_name++ = *f++);
    }
  }

  if (dosify)
    msname(n);

  /* Returned malloc'ed name */
  if (pdosflag)
    *pdosflag = dosflag;

  return n;
}


char *in2ex( char *n)
/* char *n;                internal file name */
/* Convert the zip file name to an external file name, returning the malloc'ed
   string or NULL if not enough memory. */
{
  char *x;              /* external file name */
  char *t;              /* scans name */
  int i;
  char chr;
  char *endp;
  char *last_slash;
  char *versionp;

#ifdef NAML$C_MAXRSS

  char buf[ NAML$C_MAXRSS+ 1];
  unsigned char prop;
  unsigned char uchr;
  char *last_dot;

#endif /* def NAML$C_MAXRSS */

  /* Locate the last slash. */
  last_slash = strrchr( n, '/');

/* If ODS5 is possible, replace escape carets in name. */

#ifdef NAML$C_MAXRSS

  endp = n+ strlen( n);

  /* Locate the version delimiter, if one is expected. */
  if (vmsver == 0)
  { /* No version expected. */
    versionp = endp;
  }
  else
  {
    if (vmsver > 1)
    { /* Expect a dot-version, ".nnn".  Locate the version ".".
         Temporarily terminate at this dot to allow the last-dot search
         below to find the last non-version dot.
      */
      versionp = strrchr( n, '.');
      if (versionp != NULL)     /* Can't miss. */
      {
        *versionp = '\0';
      }
    }
    else
    { /* Expect a semi-colon-version, ";nnn".  Locate the ";".  */
      versionp = strrchr( n, ';');
    }
    if ((versionp == NULL) || (versionp < last_slash))
    { /* If confused, and the version delimiter was not in the name,
         then ignore it.
      */
      versionp = endp;
    }
  }

  /* No escape needed for the last dot, if it's part of the file name.
     All dots in a directory must be escaped.
  */
  last_dot = strrchr( n, '.');

  if ((last_dot != NULL) && (last_slash != NULL) && (last_dot < last_slash))
  {
    last_dot = last_slash;
  }

  /* Replace the version dot if necessary. */
  if ((vmsver > 1) && (versionp != NULL) && (versionp < endp))
  {
    *versionp = '.';
  }

  /* Add ODS5 escape sequences.  Leave "/" and "?" for later.
     The name here looks (roughly) like: dir1/dir2/a.b
  */
  t = n;
  x = buf;
  while (uchr = *t++)
  {
    /* Characters in the version do not need escaping. */
    if (t <= versionp)
    {
      prop = char_prop[ uchr]& 31;
      if (prop)
      {
        if (prop& 4)
        { /* Dot. */
          if (t < last_dot)
          {
            /* Dot which must be escaped. */
            *x++ = '^';
          }
        }
        else if (prop& 8)
        {
          /* Character needing hex-hex escape. */
          *x++ = '^';
          *x++ = hex_digit[ uchr>> 4];
          uchr = hex_digit[ uchr& 15];
        }
        else
        {
          /* Non-dot character which must be escaped (and simple works).
             "?" gains the caret but remains "?" until later.
             ("/" remains (unescaped) "/".)
          */
          *x++ = '^';
          if (prop& 2)
          {
            /* Escaped space (represented as "^_"). */
            uchr = '_';
          }
        }
      }
    }
    *x++ = uchr;
  }
  *x = '\0';

  /* Point "n" to altered name buffer, and re-find the last slash. */
  n = buf;
  last_slash = strrchr( n, '/');

#endif /* def NAML$C_MAXRSS */

  if ((t = last_slash) == NULL)
  {
    if ((x = malloc(strlen(n) + 1 + DIR_PAD)) == NULL)
      return NULL;
    strcpy(x, n);
  }
  else
  {
    if ((x = malloc(strlen(n) + 3 + DIR_PAD)) == NULL)
      return NULL;

    /* Begin with "[". */
    x[ 0] = '[';
    i = 1;
    if (*n != '/')
    {
      /* Relative path.  Add ".". */
      x[ i++] = '.';
    }
    else
    {
      /* Absolute path.  Skip leading "/". */
      n++;
    }
    strcpy( (x+ i), n);

    /* Place the final ']'.  Remember where the name starts. */
    *(t = x + i + (t - n)) = ']';
    last_slash = t;

    /* Replace "/" with ".", and "?" with (now escaped) "/", in the
       directory part of the name.
    */
    while (--t > x)
    {
      chr = *t;
      if (chr == '/')
      {
        *t = '.';
      }
      else if (chr == '?')
      {
        *t = '/';
      }
    }

    /* Replace "?" with (now escaped) "/", in the non-directory part of
       the name.
    */
    while ((chr = *(++last_slash)) != '\0')
    {
      if (chr == '?')
      {
        *last_slash = '/';
      }
    }
  }

/* If case preservation is impossible (VAX, say), and down-casing, then
   up-case.  If case preservation is possible and wasn't done, then
   there's no way to ensure proper restoration of original case, so
   don't try.  This may differ from pre-3.0 behavior.
*/
#ifndef NAML$C_MAXRSS

  if (vms_case_2 <= 0)
  {
    strupper( x);
  }

#endif /* ndef NAML$C_MAXRSS */

  return x;
}

void stamp( char *f, ulg d)
/* char *f;                name of file to change */
/* ulg d;                  dos-style time to change it to */
/* Set last updated and accessed time of file f to the DOS time d. */
{
  int tm_sec, tm_min, tm_hour, tm_mday, tm_mon, tm_year;
  char timbuf[24];
  static ZCONST char *month[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN",
                                 "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
  struct VMStimbuf {
      char *actime;           /* VMS revision date, ASCII format */
      char *modtime;          /* VMS creation date, ASCII format */
  } ascii_times;

  ascii_times.actime = ascii_times.modtime = timbuf;

  /* Convert DOS time to ASCII format for VMSmunch */
  tm_sec = (int)(d << 1) & 0x3e;
  tm_min = (int)(d >> 5) & 0x3f;
  tm_hour = (int)(d >> 11) & 0x1f;
  tm_mday = (int)(d >> 16) & 0x1f;
  tm_mon = ((int)(d >> 21) & 0xf) - 1;
  tm_year = ((int)(d >> 25) & 0x7f) + 1980;
  sprintf(timbuf, "%02d-%3s-%04d %02d:%02d:%02d.00", tm_mday, month[tm_mon],
    tm_year, tm_hour, tm_min, tm_sec);

  /* Set updated and accessed times of f */
  if (VMSmunch(f, SET_TIMES, (char *)&ascii_times) != RMS$_NMF)
    zipwarn("can't set zipfile time: ", f);
}

ulg filetime( char *f, ulg *a, zoff_t *n, iztimes *t)
/* char *f;                name of file to get info on */
/* ulg *a;                 return value: file attributes */
/* zoff_t *n;              return value: file size */
/* iztimes *t;             return value: access, modific. and creation times */
/* If file *f does not exist, return 0.  Else, return the file's last
   modified date and time as an MSDOS date and time.  The date and
   time is returned in a long with the date most significant to allow
   unsigned integer comparison of absolute times.  Also, if a is not
   a NULL pointer, store the file attributes there, with the high two
   bytes being the Unix attributes, and the low byte being a mapping
   of that to DOS attributes.  If n is not NULL, store the file size
   there.  If t is not NULL, the file's access, modification and creation
   times are stored there as UNIX time_t values.
   If f is "-", use standard input as the file. If f is a device, return
   a file size of -1 */
{
  struct stat s;        /* results of stat() */
  /* convert to a malloc string dump FNMAX - 11/8/04 EG */
  char *name;
  int len = strlen(f);

  if (f == label) {
    if (a != NULL)
      *a = label_mode;
    if (n != NULL)
      *n = -2; /* convention for a label name */
    if (t != NULL)
      t->atime = t->mtime = t->ctime = label_utim;
    return label_time;
  }
  if ((name = malloc(len + 1)) == NULL) {
    ZIPERR(ZE_MEM, "filetime");
  }
  strcpy(name, f);
  if (name[len - 1] == '/')
    name[len - 1] = '\0';
  /* not all systems allow stat'ing a file with / appended */

  if (strcmp(f, "-") == 0) {
    if (fstat(fileno(stdin), &s) != 0) {
      free(name);
      error("fstat(stdin)");
    }
  } else if (LSSTAT(name, &s) != 0) {
             /* Accept about any file kind including directories
              * (stored with trailing / with -r option)
              */
    free(name);
    return 0;
  }
  free(name);

  if (a != NULL) {
    *a = ((ulg)s.st_mode << 16) | !(s.st_mode & S_IWRITE);
    if ((s.st_mode & S_IFDIR) != 0) {
      *a |= MSDOS_DIR_ATTR;
    }
  }
  if (n != NULL)
    *n = (s.st_mode & S_IFMT) == S_IFREG ? s.st_size : -1;
  if (t != NULL) {
    t->atime = s.st_mtime;
#ifdef USE_MTIME
    t->mtime = s.st_mtime;            /* Use modification time in VMS */
#else
    t->mtime = s.st_ctime;            /* Use creation time in VMS */
#endif
    t->ctime = s.st_ctime;
  }

#ifdef USE_MTIME
  return unix2dostime((time_t *)&s.st_mtime); /* Use modification time in VMS */
#else
  return unix2dostime((time_t *)&s.st_ctime); /* Use creation time in VMS */
#endif
}

int deletedir( char *d)
/* char *d;                directory to delete */

/* Delete the directory *d if it is empty, do nothing otherwise.
   Return the result of rmdir(), delete(), or system().
   For VMS, d must be in format [x.y]z.dir;1  (not [x.y.z]).
 */
{
    /* code from Greg Roelofs, who horked it from Mark Edwards (unzip) */
    int r, len;
    char *s;              /* malloc'd string for system command */

    len = strlen(d);
    if ((s = malloc(len + 34)) == NULL)
      return 127;

    system(strcat(strcpy(s, "set prot=(o:rwed) "), d));
    r = delete(d);
    free(s);
    return r;
}
