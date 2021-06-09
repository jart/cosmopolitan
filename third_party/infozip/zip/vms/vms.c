/*
  Copyright (c) 1990-2007 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*
 *  vms.c (zip) by Igor Mandrichenko    Version 2.2-2
 *
 *  Revision history:
 *  ...
 *  2.2-2       18-jan-1993     I.Mandrichenko
 *      vms_stat() added - version of stat() that handles special
 *      case when end-of-file-block == 0
 *
 *  3.0         11-oct-2004     SMS
 *      It would be nice to know why vms_stat() is needed.  If EOF can't
 *      be trusted for a zero-length file, why trust it for any file?
 *      Anyway, I removed the (int) cast on ->st_size, which may now be
 *      bigger than an int, just in case this code ever does get used.
 *      (A true zero-length file should still report zero length, even
 *      after the long fight with RMS.)
 *      Moved the VMS_PK_EXTRA test(s) into VMS_IM.C and VMS_PK.C to
 *      allow more general automatic dependency generation.
 */

#ifdef VMS                      /* For VMS only ! */

#define NO_ZIPUP_H              /* Prevent full inclusion of vms/zipup.h. */

#include "zip.h"
#include "zipup.h"              /* Only partial. */

#include <stdio.h>
#include <string.h>

#include <jpidef.h>
#include <fab.h>                /* Needed only in old environments. */
#include <nam.h>                /* Needed only in old environments. */
#include <starlet.h>
#include <ssdef.h>
#include <stsdef.h>

/* On VAX, define Goofy VAX Type-Cast to obviate /standard = vaxc.
   Otherwise, lame system headers on VAX cause compiler warnings.
   (GNU C may define vax but not __VAX.)
*/
#ifdef vax
# define __VAX 1
#endif /* def vax */

#ifdef __VAX
# define GVTC (unsigned int)
#else /* def __VAX */
# define GVTC
#endif /* def __VAX */


#ifdef UTIL

/* For utilities, include only vms.h, as either of the vms_XX.c files
 * would do.
 */

# include "vms.h"

#else /* not UTIL */

/* Include the `VMS attributes' preserving file-io code. We distinguish
   between two incompatible flavours of storing VMS attributes in the
   Zip archive:
   a) The "PKware" style follows the extra field specification for
      PKware's VMS Zip.
   b) The "IM (Info-ZIP)" flavour was defined from scratch by
      Igor Mandrichenko. This version has be used in official Info-ZIP
      releases for several years and is known to work well.
 */

/* Note that only one of these #include directives will include any
 * active code, depending on VMS_PK_EXTRA.  Both are included here (and
 * tested there) to allow more general automatic dependency generation.
 */

#include "vms_pk.c"
#include "vms_im.c"

#endif /* not UTIL [else] */

#ifndef ERR
#define ERR(x) (((x)&1)==0)
#endif

#ifndef NULL
#define NULL (void*)(0L)
#endif

int vms_stat( char *file, stat_t *s)
{
    int status;
    int staterr;
    struct FAB fab;
    struct NAM_STRUCT nam;
    struct XABFHC fhc;

    /*
     *  In simplest case when stat() returns "ok" and file size is
     *  nonzero or this is directory, finish with this
     */

    if( (staterr=stat(file,s)) == 0
        && ( s->st_size >= 0                      /* Size - ok */
             || (s->st_mode & S_IFREG) == 0       /* Not a plain file */
           )
    ) return staterr;

    /*
     *  Get here to handle the special case when stat() returns
     *  invalid file size. Use RMS to compute the size.
     *  When EOF block is zero, set file size to its physical size.
     *  One more case to get here is when this is remote file accessed
     *  via DECnet.
     */

    fab = cc$rms_fab;
    nam = CC_RMS_NAM;
    fhc = cc$rms_xabfhc;
    fab.FAB_NAM = &nam;
    fab.fab$l_xab = (char*)(&fhc);

#ifdef NAML$C_MAXRSS

    fab.fab$l_dna = (char *) -1;    /* Using NAML for default name. */
    fab.fab$l_fna = (char *) -1;    /* Using NAML for file name. */

#endif /* def NAML$C_MAXRSS */

    FAB_OR_NAML( fab, nam).FAB_OR_NAML_FNA = file;
    FAB_OR_NAML( fab, nam).FAB_OR_NAML_FNS = strlen( file);

    fab.fab$b_fac = FAB$M_GET;

    status = sys$open(&fab);
    fab.fab$l_xab = (char*)0L;
    sys$close(&fab);

    if( !ERR(status) )
    {
        if( fhc.xab$l_ebk > 0 )
            s->st_size = ( fhc.xab$l_ebk-1 ) * 512 + fhc.xab$w_ffb;
        else if( fab.fab$b_org == FAB$C_IDX
                 || fab.fab$b_org == FAB$C_REL
                 || fab.fab$b_org == FAB$C_HSH )
                /* Special case, when ebk=0: save entire allocated space */
                    s->st_size = fhc.xab$l_hbk * 512;
        else
            s->st_size = fhc.xab$w_ffb;
        return 0; /* stat() success code */
    }
    else
        return status;
}


/*
 * 2007-01-29 SMS.
 *
 *  VMS Status Code Summary  (See STSDEF.H for details.)
 *
 *      Bits:   31:28    27:16     15:3     2:0
 *      Field:  Control  Facility  Message  Severity
 *
 *  In the Control field, bits 31:29 are reserved.  Bit 28 inhibits
 *  printing the message.  In the Facility field, bit 27 means
 *  customer-defined (not HP-assigned, like us).  In the Message field,
 *  bit 15 means facility-specific (which our messages are).  The
 *  Severity codes are 0 = Warning, 1 = Success, 2 = Error, 3 = Info,
 *  4 = Severe (fatal).
 *
 *  Previous versions of Info-ZIP programs used a generic ("chosen (by
 *  experimentation)") Control+Facility code of 0x7FFF, which included
 *  some reserved control bits, the inhibit-printing bit, and the
 *  customer-defined bit.
 *
 *  HP has now assigned official Facility names and corresponding
 *  Facility codes for the Info-ZIP products:
 *
 *      Facility Name    Facility Code
 *      IZ_UNZIP         1954 = 0x7A2
 *      IZ_ZIP           1955 = 0x7A3
 *
 *  Now, unless the CTL_FAC_IZ_ZIP macro is defined at build-time, we
 *  will use the official Facility code.
 *
 */

/* Official HP-assigned Info-ZIP Zip Facility code. */
#define FAC_IZ_ZIP 1955   /* 0x7A3 */

#ifndef CTL_FAC_IZ_ZIP
   /*
    * Default is inhibit-printing with the official Facility code.
    */
#  define CTL_FAC_IZ_ZIP ((0x1 << 12)| FAC_IZ_ZIP)
#  define MSG_FAC_SPEC 0x8000   /* Facility-specific code. */
#else /* ndef CTL_FAC_IZ_ZIP */
   /* Use the user-supplied Control+Facility code for err or warn. */
#  define OLD_STATUS
#  ifndef MSG_FAC_SPEC          /* Old default is not Facility-specific. */
#    define MSG_FAC_SPEC 0x0    /* Facility-specific code.  Or 0x8000. */
#  endif /* ndef MSG_FAC_SPEC */
#endif /* ndef CTL_FAC_IZ_ZIP [else] */


/* Return an intelligent status/severity code. */

void vms_exit(e)
   int e;
{
  {
#ifndef OLD_STATUS

    /*
     * Exit with code comprising Control, Facility, (facility-specific)
     * Message, and Severity.
     */
    exit( (CTL_FAC_IZ_ZIP << 16) |              /* Facility                */
          MSG_FAC_SPEC |                        /* Facility-specific       */
          (e << 4) |                            /* Message code            */
          (ziperrors[ e].severity & 0x07)       /* Severity                */
        );

#else /* ndef OLD_STATUS */

    /* 2007-01-17 SMS.
     * Defining OLD_STATUS provides the same behavior as in Zip versions
     * before an official VMS Facility code had been assigned, which
     * means that Success (ZE_OK) gives a status value of 1 (SS$_NORMAL)
     * with no Facility code, while any error or warning gives a status
     * value which includes a Facility code.  (Curiously, under the old
     * scheme, message codes were left-shifted by 4 instead of 3,
     * resulting in all-even message codes.)  I don't like this, but I
     * was afraid to remove it, as someone, somewhere may be depending
     * on it.  Define CTL_FAC_IZ_ZIP as 0x7FFF to get the old behavior.
     * Define only OLD_STATUS to get the old behavior for Success
     * (ZE_OK), but using the official HP-assigned Facility code for an
     * error or warning.  Define MSG_FAC_SPEC to get the desired
     * behavior.
     *
     * Exit with simple SS$_NORMAL for ZE_OK.  Otherwise, exit with code
     * comprising Control, Facility, Message, and Severity.
     */
    exit(
         (e == ZE_OK) ? SS$_NORMAL :            /* Success (others below)  */
         ((CTL_FAC_IZ_ZIP << 16) |              /* Facility                */
          MSG_FAC_SPEC |                        /* Facility-specific (?)   */
          (e << 4) |                            /* Message code            */
          (ziperrors[ e].severity & 0x07)       /* Severity                */
         )
        );

#endif /* ndef OLD_STATUS */
   }
}


/******************************/
/*  Function version_local()  */
/******************************/

void version_local()
{
    static ZCONST char CompiledWith[] = "Compiled with %s%s for %s%s%s%s.\n\n";
#ifdef VMS_VERSION
    char *chrp1;
    char *chrp2;
    char buf[40];
    char vms_vers[ 16];
    int ver_maj;
#endif
#ifdef __DECC_VER
    char buf2[40];
    int  vtyp;
#endif

#ifdef VMS_VERSION
    /* Truncate the version string at the first (trailing) space. */
    strncpy( vms_vers, VMS_VERSION, sizeof( vms_vers));
    chrp1 = strchr( vms_vers, ' ');
    if (chrp1 != NULL)
        *chrp1 = '\0';

    /* Determine the major version number. */
    ver_maj = 0;
    chrp1 = strchr( &vms_vers[ 1], '.');
    for (chrp2 = &vms_vers[ 1];
     chrp2 < chrp1;
     ver_maj = ver_maj* 10+ *(chrp2++)- '0');

#endif /* def VMS_VERSION */

/*  DEC C in ANSI mode does not like "#ifdef MACRO" inside another
    macro when MACRO is equated to a value (by "#define MACRO 1").   */

    printf(CompiledWith,

#ifdef __GNUC__
      "gcc ", __VERSION__,
#else
#  if defined(DECC) || defined(__DECC) || defined (__DECC__)
      "DEC C",
#    ifdef __DECC_VER
      (sprintf(buf2, " %c%d.%d-%03d",
               ((vtyp = (__DECC_VER / 10000) % 10) == 6 ? 'T' :
                (vtyp == 8 ? 'S' : 'V')),
               __DECC_VER / 10000000,
               (__DECC_VER % 10000000) / 100000, __DECC_VER % 1000), buf2),
#    else
      "",
#    endif
#  else
#  ifdef VAXC
      "VAX C", "",
#  else
      "unknown compiler", "",
#  endif
#  endif
#endif

#ifdef VMS_VERSION
#  if defined( __alpha)
      "OpenVMS",
      (sprintf( buf, " (%s Alpha)", vms_vers), buf),
#  elif defined( __ia64) /* defined( __alpha) */
      "OpenVMS",
      (sprintf( buf, " (%s IA64)", vms_vers), buf),
#  else /* defined( __alpha) */
      (ver_maj >= 6) ? "OpenVMS" : "VMS",
      (sprintf( buf, " (%s VAX)", vms_vers), buf),
#  endif /* defined( __alpha) */
#else
      "VMS",
      "",
#endif /* def VMS_VERSION */

#ifdef __DATE__
      " on ", __DATE__
#else
      "", ""
#endif
      );

} /* end function version_local() */

/* 2004-10-08 SMS.
 *
 *       tempname() for VMS.
 *
 *    Generate a temporary Zip archive file name, near the actual
 *    destination Zip archive file, or at "tempath", if specified.
 *
 *    Using sys$parse() is probably more work than it's worth, but it
 *    should also be ODS5-safe.
 *
 *    Note that the generic method using tmpnam() (in FILEIO.C)
 *    produces "ziXXXXXX", where "XXXXXX" is the low six digits of the
 *    decimal representation of the process ID.  This method produces
 *    "ZIxxxxxxxx", where "xxxxxxxx" is the (whole) eight-digit
 *    hexadecimal representation of the process ID.  More important, it
 *    actually uses the directory part of the argument or "tempath".
 */


char *tempname( char *zip)
/* char *zip; */                /* Path name of Zip archive. */
{
    char *temp_name;            /* Return value. */
    int sts;                    /* System service status. */

    static int pid;             /* Process ID. */
    static int pid_len;         /* Returned size of process ID. */

    struct                      /* Item list for GETJPIW. */
    {
        short buf_len;          /* Buffer length. */
        short itm_cod;          /* Item code. */
        int *buf;               /* Buffer address. */
        int *ret_len;           /* Returned length. */
        int term;               /* Item list terminator. */
    } jpi_itm_lst = { sizeof( pid), JPI$_PID, &pid, &pid_len };

    /* ZI<UNIQUE> name storage. */
    static char zip_tmp_nam[ 16] = "ZI<unique>.;";

    struct FAB fab;             /* FAB structure. */
    struct NAM_STRUCT nam;      /* NAM[L] structure. */

    char exp_str[ NAM_MAXRSS+ 1];   /* Expanded name storage. */

#ifdef VMS_UNIQUE_TEMP_BY_TIME

    /* Use alternate time-based scheme to generate a unique temporary name. */
    sprintf( &zip_tmp_nam[ 2], "%08X", time( NULL));

#else /* def VMS_UNIQUE_TEMP_BY_TIME */

    /* Use the process ID to generate a unique temporary name. */
    sts = sys$getjpiw( 0, 0, 0, &jpi_itm_lst, 0, 0, 0);
    sprintf( &zip_tmp_nam[ 2], "%08X", pid);

#endif /* def VMS_UNIQUE_TEMP_BY_TIME */

    /* Smoosh the unique temporary name against the actual Zip archive
       name (or "tempath") to create the full temporary path name.
       (Truncate it at the file type to remove any file type.)
    */
    if (tempath != NULL)        /* Use "tempath", if it's been specified. */
        zip = tempath;

    /* Initialize the FAB and NAM[L], and link the NAM[L] to the FAB. */
    fab = cc$rms_fab;
    nam = CC_RMS_NAM;
    fab.FAB_NAM = &nam;

    /* Point the FAB/NAM[L] fields to the actual name and default name. */

#ifdef NAML$C_MAXRSS

    fab.fab$l_dna = (char *) -1;    /* Using NAML for default name. */
    fab.fab$l_fna = (char *) -1;    /* Using NAML for file name. */

#endif /* def NAML$C_MAXRSS */

    /* Default name = Zip archive name. */
    FAB_OR_NAML( fab, nam).FAB_OR_NAML_DNA = zip;
    FAB_OR_NAML( fab, nam).FAB_OR_NAML_DNS = strlen( zip);

    /* File name = "ZI<unique>,;". */
    FAB_OR_NAML( fab, nam).FAB_OR_NAML_FNA = zip_tmp_nam;
    FAB_OR_NAML( fab, nam).FAB_OR_NAML_FNS = strlen( zip_tmp_nam);

    nam.NAM_ESA = exp_str;      /* Expanded name (result) storage. */
    nam.NAM_ESS = NAM_MAXRSS;   /* Size of expanded name storage. */

    nam.NAM_NOP = NAM_M_SYNCHK; /* Syntax-only analysis. */

    temp_name = NULL;           /* Prepare for failure (unlikely). */
    sts = sys$parse( &fab, 0, 0);       /* Parse the name(s). */

    if ((sts& STS$M_SEVERITY) == STS$M_SUCCESS)
    {
        /* Overlay any resulting file type (typically ".ZIP") with none. */
        strcpy( nam.NAM_L_TYPE, ".;");

        /* Allocate temp name storage (as caller expects), and copy the
           (truncated) temp name into the new location.
        */
        temp_name = malloc( strlen( nam.NAM_ESA)+ 1);

        if (temp_name != NULL)
        {
            strcpy( temp_name, nam.NAM_ESA);
        }
    }
    return temp_name;
} /* tempname() for VMS. */


/* 2005-02-17 SMS.
 *
 *       ziptyp() for VMS.
 *
 *    Generate a real Zip archive file name (exact, if it exists), using
 *    a default file name.
 *
 *    2005-02-17 SMS.  Moved to here from [-]ZIPFILE.C, to segregate
 *    better the RMS stuff.
 *
 *    Before 2005-02-17, if sys$parse() failed, ziptyp() returned a null
 *    string ("&zero", where "static char zero = '\0';").  This
 *    typically caused Zip to proceed, but then the final rename() of
 *    the temporary archive would (silently) fail (null file name, after
 *    all), leaving only the temporary archive file, and providing no
 *    warning message to the victim.  Now, when sys$parse() fails,
 *    ziptyp() returns the original string, so a later open() fails, and
 *    a relatively informative message is provided.  (A VMS-specific
 *    message could also be provided here, if desired.)
 *
 *    2005-09-16 SMS.
 *    Changed name parsing in ziptyp() to solve a problem with a
 *    search-list logical name device-directory spec for the zipfile.
 *    Previously, when the zipfile did not exist (so sys$search()
 *    failed), the expanded name was used, but as it was
 *    post-sys$search(), it was based on the _last_ member of the search
 *    list instead of the first.  Now, the expanded name from the
 *    original sys$parse() (pre-sys$search()) is retained, and it is
 *    used if sys$search() fails.  This name is based on the first
 *    member of the search list, as a user might expect.
 */

/* Default Zip archive file spec. */
#define DEF_DEVDIRNAM "SYS$DISK:[].zip"

char *ziptyp( char *s)
{
    int status;
    int exp_len;
    struct FAB fab;
    struct NAM_STRUCT nam;
    char result[ NAM_MAXRSS+ 1];
    char exp[ NAM_MAXRSS+ 1];
    char *p;

    fab = cc$rms_fab;                           /* Initialize FAB. */
    nam = CC_RMS_NAM;                           /* Initialize NAM[L]. */
    fab.FAB_NAM = &nam;                         /* FAB -> NAM[L] */

#ifdef NAML$C_MAXRSS

    fab.fab$l_dna =(char *) -1;         /* Using NAML for default name. */
    fab.fab$l_fna = (char *) -1;        /* Using NAML for file name. */

#endif /* def NAML$C_MAXRSS */

    /* Argument file name and length. */
    FAB_OR_NAML( fab, nam).FAB_OR_NAML_FNA = s;
    FAB_OR_NAML( fab, nam).FAB_OR_NAML_FNS = strlen( s);

    /* Default file spec and length. */
    FAB_OR_NAML( fab, nam).FAB_OR_NAML_DNA = DEF_DEVDIRNAM;
    FAB_OR_NAML( fab, nam).FAB_OR_NAML_DNS = sizeof( DEF_DEVDIRNAM)- 1;

    nam.NAM_ESA = exp;                 /* Expanded name, */
    nam.NAM_ESS = NAM_MAXRSS;          /* storage size. */
    nam.NAM_RSA = result;              /* Resultant name, */
    nam.NAM_RSS = NAM_MAXRSS;          /* storage size. */

    status = sys$parse(&fab);
    if ((status & 1) == 0)
    {
        /* Invalid file name.  Return (re-allocated) original, and hope
           for a later error message.
        */
        if ((p = malloc( strlen( s)+ 1)) != NULL )
        {
            strcpy( p, s);
        }
        return p;
    }

    /* Save expanded name length from sys$parse(). */
    exp_len = nam.NAM_ESL;

    /* Leave expanded name as-is, in case of search failure. */
    nam.NAM_ESA = NULL;                 /* Expanded name, */
    nam.NAM_ESS = 0;                    /* storage size. */

    status = sys$search(&fab);
    if (status & 1)
    {   /* Zip file exists.  Use resultant (complete, exact) name. */
        if ((p = malloc( nam.NAM_RSL+ 1)) != NULL )
        {
            result[ nam.NAM_RSL] = '\0';
            strcpy( p, result);
        }
    }
    else
    {   /* New Zip file.  Use pre-search expanded name. */
        if ((p = malloc( exp_len+ 1)) != NULL )
        {
            exp[ exp_len] = '\0';
            strcpy( p, exp);
        }
    }
    return p;
} /* ziptyp() for VMS. */


/* 2005-12-30 SMS.
 *
 *       vms_file_version().
 *
 *    Return the ";version" part of a VMS file specification.
 */

char *vms_file_version( char *s)
{
    int status;
    struct FAB fab;
    struct NAM_STRUCT nam;
    char *p;

    static char exp[ NAM_MAXRSS+ 1];    /* Expanded name storage. */


    fab = cc$rms_fab;                   /* Initialize FAB. */
    nam = CC_RMS_NAM;                   /* Initialize NAM[L]. */
    fab.FAB_NAM = &nam;                 /* FAB -> NAM[L] */

#ifdef NAML$C_MAXRSS

    fab.fab$l_dna =(char *) -1;         /* Using NAML for default name. */
    fab.fab$l_fna = (char *) -1;        /* Using NAML for file name. */

#endif /* def NAML$C_MAXRSS */

    /* Argument file name and length. */
    FAB_OR_NAML( fab, nam).FAB_OR_NAML_FNA = s;
    FAB_OR_NAML( fab, nam).FAB_OR_NAML_FNS = strlen( s);

    nam.NAM_ESA = exp;                 /* Expanded name, */
    nam.NAM_ESS = NAM_MAXRSS;          /* storage size. */

    nam.NAM_NOP = NAM_M_SYNCHK;        /* Syntax-only analysis. */

    status = sys$parse(&fab);

    if ((status & 1) == 0)
    {
        /* Invalid file name.  Return "". */
        exp[ 0] = '\0';
        p = exp;
    }
    else
    {
        /* Success.  NUL-terminate, and return a pointer to the ";" in
           the expanded name storage buffer.
        */
        p = nam.NAM_L_VER;
        p[ nam.NAM_B_VER] = '\0';
    }
    return p;
} /* vms_file_version(). */


/* 2004-11-23 SMS.
 *
 *       get_rms_defaults().
 *
 *    Get user-specified values from (DCL) SET RMS_DEFAULT.  FAB/RAB
 *    items of particular interest are:
 *
 *       fab$w_deq         default extension quantity (blocks) (write).
 *       rab$b_mbc         multi-block count.
 *       rab$b_mbf         multi-buffer count (used with rah and wbh).
 */

#define DIAG_FLAG (verbose >= 2)

/* Default RMS parameter values. */

#define RMS_DEQ_DEFAULT 16384   /* About 1/4 the max (65535 blocks). */
#define RMS_MBC_DEFAULT 127     /* The max, */
#define RMS_MBF_DEFAULT 2       /* Enough to enable rah and wbh. */

/* GETJPI item descriptor structure. */
typedef struct
    {
    short buf_len;
    short itm_cod;
    void *buf;
    int *ret_len;
    } jpi_item_t;

/* Durable storage */

static int rms_defaults_known = 0;

/* JPI item buffers. */
static unsigned short rms_ext;
static char rms_mbc;
static unsigned char rms_mbf;

/* Active RMS item values. */
unsigned short rms_ext_active;
char rms_mbc_active;
unsigned char rms_mbf_active;

/* GETJPI item lengths. */
static int rms_ext_len;         /* Should come back 2. */
static int rms_mbc_len;         /* Should come back 1. */
static int rms_mbf_len;         /* Should come back 1. */

/* Desperation attempts to define unknown macros.  Probably doomed.
 * If these get used, expect sys$getjpiw() to return %x00000014 =
 * %SYSTEM-F-BADPARAM, bad parameter value.
 * They keep compilers with old header files quiet, though.
 */
#ifndef JPI$_RMS_EXTEND_SIZE
#  define JPI$_RMS_EXTEND_SIZE 542
#endif /* ndef JPI$_RMS_EXTEND_SIZE */

#ifndef JPI$_RMS_DFMBC
#  define JPI$_RMS_DFMBC 535
#endif /* ndef JPI$_RMS_DFMBC */

#ifndef JPI$_RMS_DFMBFSDK
#  define JPI$_RMS_DFMBFSDK 536
#endif /* ndef JPI$_RMS_DFMBFSDK */

/* GETJPI item descriptor set. */

struct
    {
    jpi_item_t rms_ext_itm;
    jpi_item_t rms_mbc_itm;
    jpi_item_t rms_mbf_itm;
    int term;
    } jpi_itm_lst =
     { { 2, JPI$_RMS_EXTEND_SIZE, &rms_ext, &rms_ext_len },
       { 1, JPI$_RMS_DFMBC, &rms_mbc, &rms_mbc_len },
       { 1, JPI$_RMS_DFMBFSDK, &rms_mbf, &rms_mbf_len },
       0
     };

int get_rms_defaults()
{
int sts;

/* Get process RMS_DEFAULT values. */

sts = sys$getjpiw( 0, 0, 0, &jpi_itm_lst, 0, 0, 0);
if ((sts& STS$M_SEVERITY) != STS$M_SUCCESS)
    {
    /* Failed.  Don't try again. */
    rms_defaults_known = -1;
    }
else
    {
    /* Fine, but don't come back. */
    rms_defaults_known = 1;
    }

/* Limit the active values according to the RMS_DEFAULT values. */

if (rms_defaults_known > 0)
    {
    /* Set the default values. */

    rms_ext_active = RMS_DEQ_DEFAULT;
    rms_mbc_active = RMS_MBC_DEFAULT;
    rms_mbf_active = RMS_MBF_DEFAULT;

    /* Default extend quantity.  Use the user value, if set. */
    if (rms_ext > 0)
        {
        rms_ext_active = rms_ext;
        }

    /* Default multi-block count.  Use the user value, if set. */
    if (rms_mbc > 0)
        {
        rms_mbc_active = rms_mbc;
        }

    /* Default multi-buffer count.  Use the user value, if set. */
    if (rms_mbf > 0)
        {
        rms_mbf_active = rms_mbf;
        }
    }

if (DIAG_FLAG)
    {
    fprintf( stderr,
     "Get RMS defaults.  getjpi sts = %%x%08x.\n",
     sts);

    if (rms_defaults_known > 0)
        {
        fprintf( stderr,
         "               Default: deq = %6d, mbc = %3d, mbf = %3d.\n",
         rms_ext, rms_mbc, rms_mbf);
        }
    }
return sts;
}

#ifdef __DECC

/* 2004-11-23 SMS.
 *
 *       acc_cb(), access callback function for DEC C zfopen().
 *
 *    Set some RMS FAB/RAB items, with consideration of user-specified
 * values from (DCL) SET RMS_DEFAULT.  Items of particular interest are:
 *
 *       fab$w_deq         default extension quantity (blocks).
 *       rab$b_mbc         multi-block count.
 *       rab$b_mbf         multi-buffer count (used with rah and wbh).
 *
 *    See also the FOP* macros in OSDEP.H.  Currently, no notice is
 * taken of the caller-ID value, but options could be set differently
 * for read versus write access.  (I assume that specifying fab$w_deq,
 * for example, for a read-only file has no ill effects.)
 */

/* Global storage. */

int fopm_id = FOPM_ID;          /* Callback id storage, modify. */
int fopr_id = FOPR_ID;          /* Callback id storage, read. */
int fopw_id = FOPW_ID;          /* Callback id storage, write. */

int fhow_id = FHOW_ID;          /* Callback id storage, in read. */

/* acc_cb() */

int acc_cb( int *id_arg, struct FAB *fab, struct RAB *rab)
{
int sts;

/* Get process RMS_DEFAULT values, if not already done. */
if (rms_defaults_known == 0)
    {
    get_rms_defaults();
    }

/* If RMS_DEFAULT (and adjusted active) values are available, then set
 * the FAB/RAB parameters.  If RMS_DEFAULT values are not available,
 * suffer with the default parameters.
 */
if (rms_defaults_known > 0)
    {
    /* Set the FAB/RAB parameters accordingly. */
    fab-> fab$w_deq = rms_ext_active;
    rab-> rab$b_mbc = rms_mbc_active;
    rab-> rab$b_mbf = rms_mbf_active;

    /* Truncate at EOF on close, as we'll probably over-extend. */
    fab-> fab$v_tef = 1;

    /* If using multiple buffers, enable read-ahead and write-behind. */
    if (rms_mbf_active > 1)
        {
        rab-> rab$v_rah = 1;
        rab-> rab$v_wbh = 1;
        }

    if (DIAG_FLAG)
        {
        fprintf( mesg,
         "Open callback.  ID = %d, deq = %6d, mbc = %3d, mbf = %3d.\n",
         *id_arg, fab-> fab$w_deq, rab-> rab$b_mbc, rab-> rab$b_mbf);
        }
    }

/* Declare success. */
return 0;
}

#endif /* def __DECC */

/*
 * 2004-09-19 SMS.
 *
 *----------------------------------------------------------------------
 *
 *       decc_init()
 *
 *    On non-VAX systems, uses LIB$INITIALIZE to set a collection of C
 *    RTL features without using the DECC$* logical name method.
 *
 *----------------------------------------------------------------------
 */

#ifdef __DECC

#ifdef __CRTL_VER

#if !defined( __VAX) && (__CRTL_VER >= 70301000)

#include <unixlib.h>

/*--------------------------------------------------------------------*/

/* Global storage. */

/*    Flag to sense if decc_init() was called. */

int decc_init_done = -1;

/*--------------------------------------------------------------------*/

/* decc_init()

      Uses LIB$INITIALIZE to set a collection of C RTL features without
      requiring the user to define the corresponding logical names.
*/

/* Structure to hold a DECC$* feature name and its desired value. */

typedef struct
   {
   char *name;
   int value;
   } decc_feat_t;

/* Array of DECC$* feature names and their desired values. */

decc_feat_t decc_feat_array[] = {

   /* Preserve command-line case with SET PROCESS/PARSE_STYLE=EXTENDED */
 { "DECC$ARGV_PARSE_STYLE", 1 },

   /* Preserve case for file names on ODS5 disks. */
 { "DECC$EFS_CASE_PRESERVE", 1 },

   /* Enable multiple dots (and most characters) in ODS5 file names,
      while preserving VMS-ness of ";version". */
 { "DECC$EFS_CHARSET", 1 },

   /* List terminator. */
 { (char *)NULL, 0 } };

/* LIB$INITIALIZE initialization function. */

static void decc_init( void)
{
int feat_index;
int feat_value;
int feat_value_max;
int feat_value_min;
int i;
int sts;

/* Set the global flag to indicate that LIB$INITIALIZE worked. */

decc_init_done = 1;

/* Loop through all items in the decc_feat_array[]. */

for (i = 0; decc_feat_array[ i].name != NULL; i++)
   {
   /* Get the feature index. */
   feat_index = decc$feature_get_index( decc_feat_array[ i].name);
   if (feat_index >= 0)
      {
      /* Valid item.  Collect its properties. */
      feat_value = decc$feature_get_value( feat_index, 1);
      feat_value_min = decc$feature_get_value( feat_index, 2);
      feat_value_max = decc$feature_get_value( feat_index, 3);

      if ((decc_feat_array[ i].value >= feat_value_min) &&
       (decc_feat_array[ i].value <= feat_value_max))
         {
         /* Valid value.  Set it if necessary. */
         if (feat_value != decc_feat_array[ i].value)
            {
            sts = decc$feature_set_value( feat_index,
             1,
             decc_feat_array[ i].value);
            }
         }
      else
         {
         /* Invalid DECC feature value. */
         printf( " INVALID DECC FEATURE VALUE, %d: %d <= %s <= %d.\n",
          feat_value,
          feat_value_min, decc_feat_array[ i].name, feat_value_max);
         }
      }
   else
      {
      /* Invalid DECC feature name. */
      printf( " UNKNOWN DECC FEATURE: %s.\n", decc_feat_array[ i].name);
      }
   }
}

/* Get "decc_init()" into a valid, loaded LIB$INITIALIZE PSECT. */

#pragma nostandard

/* Establish the LIB$INITIALIZE PSECTs, with proper alignment and
   other attributes.  Note that "nopic" is significant only on VAX.
*/
#pragma extern_model save

#pragma extern_model strict_refdef "LIB$INITIALIZ" 2, nopic, nowrt
const int spare[ 8] = { 0 };

#pragma extern_model strict_refdef "LIB$INITIALIZE" 2, nopic, nowrt
void (*const x_decc_init)() = decc_init;

#pragma extern_model restore

/* Fake reference to ensure loading the LIB$INITIALIZE PSECT. */

#pragma extern_model save

int LIB$INITIALIZE( void);

#pragma extern_model strict_refdef
int dmy_lib$initialize = (int) LIB$INITIALIZE;

#pragma extern_model restore

#pragma standard

#endif /* !defined( __VAX) && (__CRTL_VER >= 70301000) */

#endif /* def __CRTL_VER */

#endif /* def __DECC */

#endif /* VMS */
