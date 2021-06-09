/*
  Copyright (c) 1990-2007 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/

/* 2004-12-13 SMS.
 * Disabled the module name macro to accommodate old GNU C which didn't
 * obey the directive, and thus confused MMS/MMK where the object
 * library dependencies need to have the correct module name.
 */
#if 0
# define module_name     VMSMUNCH
# define module_version  "V1.3-4"
#endif /* 0 */

/*
 *  Modified by:
 *
 *    v1.3.1        O.v.d.Linden, C. Spieler            04-JUL-1998 14:35
 *            Modified check that decides on the type of definitions for
 *            FIB$W_FID etc. to support GNU C.
 *
 *    v1.3          Hunter Goatley                      14-SEP-1992 08:51
 *            Added definitions of FIB$W_FID, FIB$W_DID, and
 *            FIB$L_ACCTL to allow for the fact that fibdef
 *            contains variant_unions under Alpha.
 */
/*---------------------------------------------------------------------------

  vmsmunch.c                    version 1.2                     28 Apr 1992

  This routine is a blatant and unrepentent appropriation of all the nasty
  and difficult-to-do and complicated VMS shenanigans which Joe Meadows has
  so magnificently captured in his FILE utility.  Not only that, it's even
  allowed! (see below).  But let it be clear at the outset that Joe did all
  the work; yea, verily, he is truly a godlike unit.

  The appropriations and modifications herein were performed primarily by
  him known as "Cave Newt," although the Info-ZIP working group probably had
  their fingers in it somewhere along the line.  The idea is to put the raw
  power of Joe's original routine at the disposal of various routines used
  by UnZip (and Zip, possibly), not least among them the utime() function.
  Read on for details...

        01-SEP-1994     Richard Levitte <levitte@e.kth.se>
                        If one of the fields given to VMSmunch are NULL,
                        do not update the corresponding daytime.

        18-JUL-1994     Hunter Goatley <goathunter@WKU.EDU>
                        Fixed IO$_ACCESS call.

        18-Jul-1994     Richard Levitte levitte@e.kth.se
                        Changed VMSmunch() to deassign the channel before
                        returning when an error has occured.

        02-Apr-1994     Jamie Hanrahan  jeh@cmkrnl.com
                        Moved definition of VMStimbuf struct from here
                        to vmsmunch.h
  ---------------------------------------------------------------------------

  Usage (i.e., "interface," in geek-speak):

     int VMSmunch( char *filename, int action, char *ptr );

     filename   the name of the file on which to be operated, obviously
     action     an integer which specifies what action to take
     ptr        pointer to any extra item which may be needed (else NULL)

  The possible values for the action argument are as follows:

     GET_TIMES      get the creation and revision dates of filename; ptr
                    must point to an empty VMStimbuf struct, as defined
                    in vmsmunch.h
                    (with room for at least 24 characters, including term.)
     SET_TIMES      set the creation and revision dates of filename (utime
                    option); ptr must point to a valid VMStimbuf struct,
                    as defined in vmsmunch.h
     GET_RTYPE      get the record type of filename; ptr must point to an
                    integer which, on return, is set to the type (as defined
                    in vmsdefs.h:  FAT$C_* defines)
     CHANGE_RTYPE   change the record type to that specified by the integer
                    to which ptr points; save the old record type (later
                    saves overwrite earlier ones)
     RESTORE_RTYPE  restore the record type to the previously saved value;
                    or, if none, set it to "fixed-length, 512-byte" record
                    format (ptr not used)

  ---------------------------------------------------------------------------

  Comments from FILE.C, a utility to modify file characteristics:

     Written by Joe Meadows Jr, at the Fred Hutchinson Cancer Research Center
     BITNET: JOE@FHCRCVAX
     PHONE: (206) 467-4970

     There are no restrictions on this code, you may sell it, include it
     with any commercial package, or feed it to a whale.. However, I would
     appreciate it if you kept this comment in the source code so that anyone
     receiving this code knows who to contact in case of problems. Note that
     I do not demand this condition..

  ---------------------------------------------------------------------------*/


/* 2004-12-13 SMS.
 * Disabled the module name macro to accommodate old GNU C which didn't
 * obey the directive, and thus confused MMS/MMK where the object
 * library dependencies need to have the correct module name.
 */
#if 0
# if defined(__DECC) || defined(__GNUC__)
#  pragma module module_name module_version
# else
#  module module_name module_version
# endif
#endif /* 0 */

/*****************************/
/*  Includes, Defines, etc.  */
/*****************************/

/* Accomodation for /NAMES = AS_IS with old header files. */

#define sys$asctim SYS$ASCTIM
#define sys$assign SYS$ASSIGN
#define sys$bintim SYS$BINTIM
#define sys$dassgn SYS$DASSGN
#define sys$parse SYS$PARSE
#define sys$qiow SYS$QIOW
#define sys$search SYS$SEARCH

#include "zip.h"

#include <stdio.h>
#include <string.h>
#include <iodef.h>
#include <starlet.h>
#include <fibdef.h>   /* this gets created with the c3.0 compiler */

/*
 *  Under Alpha (DEC C in VAXC mode) and under `good old' VAXC, the FIB unions
 *  are declared as variant_unions.  DEC C (Alpha) in ANSI modes and third
 *  party compilers which do not support `variant_union' define preprocessor
 *  symbols to `hide' the "intermediate union/struct" names from the
 *  programmer's API.
 *  We check the presence of these defines and for DEC's FIBDEF.H defining
 *  __union as variant_union to make sure we access the structure correctly.
 */
#if defined(fib$w_did) || (defined(__union) && (__union == variant_union))
#  define FIB$W_DID     fib$w_did
#  define FIB$W_FID     fib$w_fid
#  define FIB$L_ACCTL   fib$l_acctl
#else
#  define FIB$W_DID     fib$r_did_overlay.fib$w_did
#  define FIB$W_FID     fib$r_fid_overlay.fib$w_fid
#  define FIB$L_ACCTL   fib$r_acctl_overlay.fib$l_acctl
#endif

#include "vms.h"
#include "vmsmunch.h"  /* GET/SET_TIMES, RTYPE, etc. */
#include "vmsdefs.h"   /* fatdef.h, etc. */

static void asctim(char *time, long int binval[2]);
static void bintim(char *time, long int binval[2]);

/* from <ssdef.h> */
#ifndef SS$_NORMAL
#  define SS$_NORMAL    1
#  define SS$_BADPARAM  20
#endif


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


/*************************/
/*  Function VMSmunch()  */
/*************************/

int VMSmunch(
    char  *filename,
    int   action,
    char  *ptr )
{

    /* original file.c variables */

    static struct FAB Fab;
    static struct NAM_STRUCT Nam;
    static struct fibdef Fib; /* short fib */

    static struct dsc$descriptor FibDesc =
      {sizeof(Fib),DSC$K_DTYPE_Z,DSC$K_CLASS_S,(char *)&Fib};
    static struct dsc$descriptor_s DevDesc =
      {0,DSC$K_DTYPE_T,DSC$K_CLASS_S,&Nam.NAM_DVI[1]};
    static struct fatdef Fat;
    static union {
      struct fchdef fch;
      long int dummy;
    } uchar;
    static struct fjndef jnl;
    static long int Cdate[2],Rdate[2],Edate[2],Bdate[2];
    static short int revisions;
    static unsigned long uic;
#if defined(__DECC) || defined(__DECCXX)
#pragma __member_alignment __save
#pragma __nomember_alignment
#endif /* __DECC || __DECCXX */
    static union {
      unsigned short int value;
      struct {
        unsigned system : 4;
        unsigned owner : 4;
        unsigned group : 4;
        unsigned world : 4;
      } bits;
    } prot;
#if defined(__DECC) || defined(__DECCXX)
#pragma __member_alignment __restore
#endif /* __DECC || __DECCXX */

    static struct atrdef Atr[] = {
      {sizeof(Fat),ATR$C_RECATTR, GVTC &Fat},          /* record attributes */
      {sizeof(uchar),ATR$C_UCHAR, GVTC &uchar},    /* File characteristics */
      {sizeof(Cdate),ATR$C_CREDATE, GVTC &Cdate[0]},   /* Creation date */
      {sizeof(Rdate),ATR$C_REVDATE, GVTC &Rdate[0]},   /* Revision date */
      {sizeof(Edate),ATR$C_EXPDATE, GVTC &Edate[0]},   /* Expiration date */
      {sizeof(Bdate),ATR$C_BAKDATE, GVTC &Bdate[0]},   /* Backup date */
      {sizeof(revisions),ATR$C_ASCDATES, GVTC &revisions}, /* number of revs */
      {sizeof(prot),ATR$C_FPRO, GVTC &prot},           /* file protection  */
      {sizeof(uic),ATR$C_UIC, GVTC &uic},              /* file owner */
      {sizeof(jnl),ATR$C_JOURNAL, GVTC &jnl},          /* journal flags */
      {0,0,0}
    } ;

    static char EName[NAM_MAXRSS];
    static char RName[NAM_MAXRSS];
    static struct dsc$descriptor_s FileName =
      {0,DSC$K_DTYPE_T,DSC$K_CLASS_S,0};
    static struct dsc$descriptor_s string = {0,DSC$K_DTYPE_T,DSC$K_CLASS_S,0};
    static short int DevChan;
    static short int iosb[4];

    static long int i,status;
/*  static char *retval;  */


    /* new VMSmunch variables */

    static int  old_rtype=FAT$C_FIXED;   /* storage for record type */



/*---------------------------------------------------------------------------
    Initialize attribute blocks, parse filename, resolve any wildcards, and
    get the file info.
  ---------------------------------------------------------------------------*/

    /* Initialize RMS structures.  We need a NAM[L] to retrieve the FID. */
    Fab = cc$rms_fab;
    Fab.fab$l_fna = filename;
    Fab.fab$b_fns = strlen(filename);
    Fab.FAB_NAM = &Nam; /* FAB has an associated NAM[L]. */
    Nam = CC_RMS_NAM;
    Nam.NAM_ESA = EName; /* expanded filename */
    Nam.NAM_ESS = sizeof(EName);
    Nam.NAM_RSA = RName; /* resultant filename */
    Nam.NAM_RSS = sizeof(RName);

    /* do $PARSE and $SEARCH here */
    status = sys$parse(&Fab);
    if (!(status & 1)) return(status);

    /* search for the first file.. If none signal error */
    status = sys$search(&Fab);
    if (!(status & 1)) return(status);

    while (status & 1) {
        /* initialize Device name length, note that this points into the NAM[L]
           to get the device name filled in by the $PARSE, $SEARCH services */
        DevDesc.dsc$w_length = Nam.NAM_DVI[0];

        status = sys$assign(&DevDesc,&DevChan,0,0);
        if (!(status & 1)) return(status);

        FileName.dsc$a_pointer = Nam.NAM_L_NAME;
        FileName.dsc$w_length = Nam.NAM_B_NAME+Nam.NAM_B_TYPE+Nam.NAM_B_VER;

        /* Initialize the FIB */
        for (i=0;i<3;i++) {
            Fib.FIB$W_FID[i]=Nam.NAM_FID[i];
            Fib.FIB$W_DID[i]=Nam.NAM_DID[i];
        }

        /* Use the IO$_ACCESS function to return info about the file */
        /* Note, used this way, the file is not opened, and the expiration */
        /* and revision dates are not modified */
        status = sys$qiow(0,DevChan,IO$_ACCESS,&iosb,0,0,
                          &FibDesc,&FileName,0,0,&Atr,0);
        if (!(status & 1) || !((status = iosb[0]) & 1)) {
            sys$dassgn(DevChan);
            return(status);
        }

    /*-----------------------------------------------------------------------
        We have the current information from the file:  now see what user
        wants done with it.
      -----------------------------------------------------------------------*/

        switch (action) {

          case GET_TIMES:   /* non-modifying */
              asctim(((struct VMStimbuf *)ptr)->modtime, Cdate);
              asctim(((struct VMStimbuf *)ptr)->actime, Rdate);
              sys$dassgn(DevChan);
              return RMS$_NORMAL;     /* return to user */
              break;

          case SET_TIMES:
              if (((struct VMStimbuf *)ptr)->modtime != (char *)NULL)
                  bintim(((struct VMStimbuf *)ptr)->modtime, Cdate);
              if (((struct VMStimbuf *)ptr)->actime != (char *)NULL)
                  bintim(((struct VMStimbuf *)ptr)->actime, Rdate);
              break;

          case GET_RTYPE:   /* non-modifying */
              *(int *)ptr = Fat.fat$v_rtype;
              sys$dassgn(DevChan);
              return RMS$_NORMAL;     /* return to user */
              break;

          case CHANGE_RTYPE:
              old_rtype = Fat.fat$v_rtype;              /* save current one */
              if ((*(int *)ptr < FAT$C_UNDEFINED) ||
                  (*(int *)ptr > FAT$C_STREAMCR))
                  Fat.fat$v_rtype = FAT$C_STREAMLF;       /* Unix I/O happy */
              else
                  Fat.fat$v_rtype = *(int *)ptr;
              break;

          case RESTORE_RTYPE:
              Fat.fat$v_rtype = old_rtype;
              break;

          default:
              sys$dassgn(DevChan);
              return SS$_BADPARAM;   /* anything better? */
        }

    /*-----------------------------------------------------------------------
        Go back and write modified data to the file header.
      -----------------------------------------------------------------------*/

        /* note, part of the FIB was cleared by earlier QIOW, so reset it */
        Fib.FIB$L_ACCTL = FIB$M_NORECORD;
        for (i=0;i<3;i++) {
            Fib.FIB$W_FID[i]=Nam.NAM_FID[i];
            Fib.FIB$W_DID[i]=Nam.NAM_DID[i];
        }

        /* Use the IO$_MODIFY function to change info about the file */
        /* Note, used this way, the file is not opened, however this would */
        /* normally cause the expiration and revision dates to be modified. */
        /* Using FIB$M_NORECORD prohibits this from happening. */
        status = sys$qiow(0,DevChan,IO$_MODIFY,&iosb,0,0,
                          &FibDesc,&FileName,0,0,&Atr,0);
        if (!(status & 1) || !((status = iosb[0]) & 1)) {
            sys$dassgn(DevChan);
            return(status);
        }

        status = sys$dassgn(DevChan);
        if (!(status & 1)) return(status);

        /* look for next file, if none, no big deal.. */
        status = sys$search(&Fab);
    }
    return(status);
} /* end function VMSmunch() */





/***********************/
/*  Function asctim()  */
/***********************/

static void asctim(        /* convert 64-bit binval to string, put in time */
    char *time,
    long int binval[2] )
{
    static struct dsc$descriptor date_str={23,DSC$K_DTYPE_T,DSC$K_CLASS_S,0};
      /* dsc$w_length, dsc$b_dtype, dsc$b_class, dsc$a_pointer */

    date_str.dsc$a_pointer = time;
    sys$asctim(0, &date_str, binval, 0);
    time[23] = '\0';
}





/***********************/
/*  Function bintim()  */
/***********************/

static void bintim(        /* convert time string to 64 bits, put in binval */
    char *time,
    long int binval[2] )
{
    static struct dsc$descriptor date_str={0,DSC$K_DTYPE_T,DSC$K_CLASS_S,0};

    date_str.dsc$w_length = strlen(time);
    date_str.dsc$a_pointer = time;
    sys$bintim(&date_str, binval);
}
