/*
  Copyright (c) 1990-2002 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/* Low-level Amiga routines shared between Zip and UnZip.
 *
 * Contains:  FileDate()
 *            getenv()          [replaces inadequate standard library version]
 *            setenv()          [SAS/C only, replaces standard library version]
 *            set_TZ()          [SAS/C only]
 *            GetPlatformLocalTimezone() [callback from timezone.c tzset()]
 *            time()
 *            sendpkt()
 *            Agetch()
 *
 * The first five are used by most Info-ZIP programs except fUnZip.
 * The last two are used by all except the non-CRYPT version of fUnZip.
 * Probably some of the stuff in here is unused by ZipNote and ZipSplit too...
 * sendpkt() is used by Agetch() and FileDate(), and by screensize() in
 * amiga/amiga.c (UnZip); time() is used only by Zip.
 */


/* HISTORY/CHANGES
 *  2 Sep 92, Greg Roelofs, Original coding.
 *  6 Sep 92, John Bush, Incorporated into UnZip 5.1
 *  6 Sep 92, John Bush, Interlude "FileDate()" defined, which calls or
 *            redefines SetFileDate() depending upon AMIGADOS2 definition.
 * 11 Oct 92, John Bush, Eliminated AMIGADOS2 switch by determining
 *            revision via OpenLibrary() call.  Now only one version of
 *            the program runs on both platforms (1.3.x vs. 2.x)
 * 11 Oct 92, John Bush, Merged with Zip version and changed arg passing
 *            to take time_t input instead of struct DateStamp.
 *            Arg passing made to conform with utime().
 * 22 Nov 92, Paul Kienitz, fixed includes for Aztec and cleaned up some
 *            lint-ish errors; simplified test for AmigaDOS version.
 * 11 Nov 95, Paul Kienitz, added Agetch() for crypt password input and
 *            UnZip's "More" prompt -- simplifies crypt.h and avoids
 *            use of library code redundant with sendpkt().  Made it
 *            available to fUnZip, which does not use FileDate().
 * 22 Nov 95, Paul Kienitz, created a new tzset() that gets the current
 *            timezone from the Locale preferences.  These exist only under
 *            AmigaDOS 2.1 and up, but it is probably correctly set on more
 *            Amigas than the TZ environment variable is.  We check that
 *            only if TZ is not validly set.  We do not parse daylight
 *            savings syntax except to check for presence vs. absence of a
 *            DST part; United States rules are assumed.  This is better
 *            than the tzset()s in the Amiga compilers' libraries do.
 * 15 Jan 96, Chr. Spieler, corrected the logic when to select low level
 *            sendpkt() (when FileDate(), Agetch() or windowheight() is used),
 *            and AMIGA's Agetch() (CRYPT, and UnZip(SFX)'s UzpMorePause()).
 * 10 Feb 96, Paul Kienitz, re-fiddled that selection logic again, moved
 *            stuff around for clarity.
 * 16 Mar 96, Paul Kienitz, created a replacement localtime() to go with the
 *            new tzset(), because Aztec's is hopelessly broken.  Also
 *            gmtime(), which localtime() calls.
 * 12 Apr 96, Paul Kienitz, daylight savings was being handled incorrectly.
 * 21 Apr 96, Paul Kienitz, had to replace time() as well, Aztec's returns
 *            local time instead of GMT.  That's why their localtime() was bad,
 *            because it assumed time_t was already local, and gmtime() was
 *            the one that checked TZ.
 * 23 Apr 96, Chr. Spieler, deactivated time() replacement for UnZip stuff.
 *            Currently, the UnZip sources do not make use of time() (and do
 *            not supply the working mktime() replacement, either!).
 * 29 Apr 96, Paul Kienitz, created a replacement getenv() out of code that
 *            was previously embedded in tzset(), for reliable global test
 *            of whether TZ is set or not.
 * 19 Jun 96, Haidinger Walter, re-adapted for current SAS/C compiler.
 *  7 Jul 96, Paul Kienitz, smoothed together compiler-related changes.
 *  4 Feb 97, Haidinger Walter, added set_TZ() for SAS/C.
 * 23 Apr 97, Paul Kienitz, corrected Unix->Amiga DST error by adding
 *            mkgmtime() so localtime() could be used.
 * 28 Apr 97, Christian Spieler, deactivated mkgmtime() definition for ZIP;
 *            the Zip sources supply this function as part of util.c.
 * 24 May 97, Haidinger Walter, added time_lib support for SAS/C and moved
 *            set_TZ() to time_lib.c.
 * 12 Jul 97, Paul Kienitz, adapted time_lib stuff for Aztec.
 * 26 Jul 97, Chr. Spieler, old mkgmtime() fixed (ydays[] def, sign vs unsign).
 * 30 Dec 97, Haidinger Walter, adaptation for SAS/C using z-stat.h functions.
 * 19 Feb 98, Haidinger Walter, removed alloc_remember, more SAS.C fixes.
 * 23 Apr 98, Chr. Spieler, removed mkgmtime(), changed FileDate to convert to
 *            Amiga file-time directly.
 * 24 Apr 98, Paul Kienitz, clip Unix dates earlier than 1978 in FileDate().
 * 02 Sep 98, Paul Kienitz, C. Spieler, always include zip.h to get a defined
 *            header inclusion sequence that resolves all header dependencies.
 * 06 Jun 00, Paul Kienitz, removed time_lib.c due to its incompatible license,
 *            moved set_TZ() back here, replaced minimal tzset() and localtime()
 *            with new versions derived from GNU glibc source.  Gave locale_TZ()
 *            reasonable European defaults for daylight savings.
 * 17 Jun 00, Paul Kienitz, threw out GNU code because of objections to the GPL
 *            virus, replaced with similar functions based on the public domain
 *            timezone code at ftp://elsie.nci.nih.gov/pub.  As with the GNU
 *            stuff, support for timezone files and leap seconds was removed.
 * 23 Aug 00, Paul Kienitz, moved timezone code out from here into separate
 *            platform-independent module 'timezone.c'.
 * 31 Dec 00, Christian Spieler, moved system-specific timezone help funcions
 *            back in here, from 'timezone.c'.
 * 07 Jan 01, Paul Kienitz, Chr. Spieler, added missing #include "timezone.h"
 *            and "symbolic" preprocessor constants for time calculations.
 * 15 Jan 02, Paul Kienitz, excluded all time handling code from compilation
 *            for Zip utilities (when "defined(UTIL)")
 */

#ifndef __amiga_filedate_c
#define __amiga_filedate_c


#include "zip.h"
#include <ctype.h>
#include <errno.h>

#include <exec/types.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <dos/dosextens.h>

#ifdef AZTEC_C
#  include <libraries/dos.h>
#  include <libraries/dosextens.h>
#  include <clib/exec_protos.h>
#  include <clib/dos_protos.h>
#  include <clib/locale_protos.h>
#  include <pragmas/exec_lib.h>
#  include <pragmas/dos_lib.h>
#  include <pragmas/locale_lib.h>
#  define ESRCH  ENOENT
#  define EOSERR EIO
#endif

#ifdef __SASC
#  include <stdlib.h>
#  if (defined(_M68020) && (!defined(__USE_SYSBASE)))
                            /* on 68020 or higher processors it is faster   */
#    define __USE_SYSBASE   /* to use the pragma libcall instead of syscall */
#  endif                    /* to access functions of the exec.library      */
#  include <proto/exec.h>   /* see SAS/C manual:part 2,chapter 2,pages 6-7  */
#  include <proto/dos.h>
#  include <proto/locale.h>
#  ifdef DEBUG
#     include <sprof.h>
#  endif
#  ifdef MWDEBUG
#    include <stdio.h>      /* include both before memwatch.h again just */
#    include <stdlib.h>     /* to be safe */
#    include "memwatch.h"
#  endif /* MWDEBUG */
#endif /* __SASC */

#include "crypt.h"            /* just so we can tell if CRYPT is supported */


#if (!defined(FUNZIP) && !defined(UTIL))

#include "timezone.h"         /* for AMIGA-specific timezone callbacks */

#ifndef SUCCESS
#  define SUCCESS (-1L)
#  define FAILURE 0L
#endif

#define ReqVers 36L        /* required library version for SetFileDate() */
#define ENVSIZE 100        /* max space allowed for an environment var   */

extern struct ExecBase *SysBase;

#ifndef min
#  define min(a, b)  ((a) < (b) ? (a) : (b))
#  define max(a, b)  ((a) < (b) ? (b) : (a))
#endif

#if defined(ZIP) || defined(HAVE_MKTIME)
static const unsigned short ydays[] =
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
#else
extern const unsigned short ydays[];  /* in unzip's fileio.c */
#endif

#define LEAP(y)     (((y) % 4 == 0 && (y) % 100 != 0) || (y) % 400 == 0)
#define YDAYS(m, y) (ydays[m] + (m > 1 && LEAP(y)))
/* Number of leap years from 1978 to `y' (not including `y' itself). */
#define ANLEAP(y)   (((y) - 1977) / 4 - ((y) - 1901) / 100 + ((y) - 1601) / 400)
#define SECSPERMIN  60
#define MINSPERHOUR 60
#define SECSPERHOUR (SECSPERMIN * MINSPERHOUR)
#define SECSPERDAY  86400L

/* prototypes */
char *getenv(const char *var);
#ifdef __SASC
/*  XXX !!  We have really got to find a way to operate without these. */
int setenv(const char *var, const char *value, int overwrite);
void set_TZ(long time_zone, int day_light);
#endif

LONG FileDate(char *filename, time_t u[]);
LONG sendpkt(struct MsgPort *pid, LONG action, LONG *args, LONG nargs);
int Agetch(void);

/* =============================================================== */

/***********************/
/* Function filedate() */
/***********************/

/*  FileDate() (originally utime.c), by Paul Wells.  Modified by John Bush
 *  and others (see also sendpkt() comments, below); NewtWare SetFileDate()
 *  clone cheaply ripped off from utime().
 */

/* DESCRIPTION
 * This routine chooses between 2 methods to set the file date on AMIGA.
 * Since AmigaDOS 2.x came out, SetFileDate() was available in ROM (v.36
 * and higher).  Under AmigaDOS 1.3.x (less than v.36 ROM), SetFileDate()
 * must be accomplished by constructing a message packet and sending it
 * to the file system handler of the file to be stamped.
 *
 * The system's ROM version is extracted from the external system Library
 * base.
 *
 * NOTE:  although argument passing conforms with utime(), note the
 *        following differences:
 *          - Return value is boolean success/failure.
 *          - If a structure or array is passed, only the first value
 *            is used, which *may* correspond to date accessed and not
 *            date modified.
 */

LONG FileDate(filename, u)
    char *filename;
    time_t u[];
{
    LONG SetFileDate(UBYTE *filename, struct DateStamp *pDate);
    LONG sendpkt(struct MsgPort *pid, LONG action, LONG *args, LONG nargs);
    struct MsgPort *taskport;
    BPTR dirlock, lock;
    struct FileInfoBlock *fib;
    LONG pktargs[4];
    UBYTE *ptr;
    long ret;

    struct DateStamp pDate;
    struct tm *ltm;
    int years;

    tzset();
    /* Amiga file date is based on 01-Jan-1978 00:00:00 (local time):
     * 8 years and 2 leapdays difference from Unix time.
     */
    ltm = localtime(&u[0]);
    years = ltm->tm_year + 1900;
    if (years < 1978)
        pDate.ds_Days = pDate.ds_Minute = pDate.ds_Tick = 0;
    else {
        pDate.ds_Days = (years - 1978) * 365L + (ANLEAP(years)) +
                        YDAYS(ltm->tm_mon, years) + (ltm->tm_mday - 1);
        pDate.ds_Minute = ltm->tm_hour * 60 + ltm->tm_min;
        pDate.ds_Tick = ltm->tm_sec * TICKS_PER_SECOND;
    }

    if (SysBase->LibNode.lib_Version >= ReqVers)
    {
        return (SetFileDate(filename,&pDate));  /* native routine at 2.0+ */
    }
    else  /* !(SysBase->lib_Version >=ReqVers) */
    {
        if( !(taskport = (struct MsgPort *)DeviceProc(filename)) )
        {
            errno = ESRCH;          /* no such process */
            return FAILURE;
        }

        if( !(lock = Lock(filename,SHARED_LOCK)) )
        {
            errno = ENOENT;         /* no such file */
            return FAILURE;
        }

        if( !(fib = (struct FileInfoBlock *)AllocMem(
            (long)sizeof(struct FileInfoBlock),MEMF_PUBLIC|MEMF_CLEAR)) )
        {
            errno = ENOMEM;         /* insufficient memory */
            UnLock(lock);
            return FAILURE;
        }

        if( Examine(lock,fib)==FAILURE )
        {
            errno = EOSERR;         /* operating system error */
            UnLock(lock);
            FreeMem(fib,(long)sizeof(*fib));
            return FAILURE;
        }

        dirlock = ParentDir(lock);
        ptr = (UBYTE *)AllocMem(64L,MEMF_PUBLIC);
        strcpy((ptr+1),fib->fib_FileName);
        *ptr = strlen(fib->fib_FileName);
        FreeMem(fib,(long)sizeof(*fib));
        UnLock(lock);

        /* now fill in argument array */

        pktargs[0] = 0;
        pktargs[1] = (LONG)dirlock;
        pktargs[2] = (LONG)&ptr[0] >> 2;
        pktargs[3] = (LONG)&pDate;

        errno = ret = sendpkt(taskport,ACTION_SET_DATE,pktargs,4L);

        FreeMem(ptr,64L);
        UnLock(dirlock);

        return SUCCESS;
    }  /* ?(SysBase->lib_Version >= ReqVers) */
} /* FileDate() */


char *getenv(const char *var)         /* not reentrant! */
{
    static char space[ENVSIZE];
    struct Process *me = (void *) FindTask(NULL);
    void *old_window = me->pr_WindowPtr;
    char *ret = NULL;

    me->pr_WindowPtr = (void *) -1;   /* suppress any "Please insert" popups */
    if (SysBase->LibNode.lib_Version >= ReqVers) {
        if (GetVar((char *) var, space, ENVSIZE - 1, /*GVF_GLOBAL_ONLY*/ 0) > 0)
            ret = space;
    } else {                    /* early AmigaDOS, get env var the crude way */
        BPTR hand, foot, spine;
        int z = 0;
        if (foot = Lock("ENV:", ACCESS_READ)) {
            spine = CurrentDir(foot);
            if (hand = Open((char *) var, MODE_OLDFILE)) {
                z = Read(hand, space, ENVSIZE - 1);
                Close(hand);
            }
            UnLock(CurrentDir(spine));
        }
        if (z > 0) {
            space[z] = '\0';
            ret = space;
        }
    }
    me->pr_WindowPtr = old_window;
    return ret;
}

#ifdef __SASC
int setenv(const char *var, const char *value, int overwrite)
{
    struct Process *me = (void *) FindTask(NULL);
    void *old_window = me->pr_WindowPtr;
    int ret = -1;

    me->pr_WindowPtr = (void *) -1;   /* suppress any "Please insert" popups */
    if (SysBase->LibNode.lib_Version >= ReqVers)
        ret = !SetVar((char *)var, (char *)value, -1, GVF_GLOBAL_ONLY | LV_VAR);
    else {
        BPTR hand, foot, spine;
        long len = value ? strlen(value) : 0;
        if (foot = Lock("ENV:", ACCESS_READ)) {
            spine = CurrentDir(foot);
            if (len) {
                if (hand = Open((char *) var, MODE_NEWFILE)) {
                    ret = Write(hand, (char *) value, len + 1) >= len;
                    Close(hand);
                }
            } else
                ret = DeleteFile((char *) var);
            UnLock(CurrentDir(spine));
        }
    }
    me->pr_WindowPtr = old_window;
    return ret;
}

/* Stores data from timezone and daylight to ENV:TZ.                  */
/* ENV:TZ is required to exist by some other SAS/C library functions, */
/* like stat() or fstat().                                            */
void set_TZ(long time_zone, int day_light)
{
    char put_tz[MAXTIMEZONELEN];  /* string for putenv: "TZ=aaabbb:bb:bbccc" */
    int offset;
    void *exists;     /* dummy ptr to see if global envvar TZ already exists */
    exists = (void *)getenv(TZ_ENVVAR);
    /* see if there is already an envvar TZ_ENVVAR. If not, create it */
    if (exists == NULL) {
        /* create TZ string by pieces: */
        sprintf(put_tz, "GMT%+ld", time_zone / 3600L);
        if (time_zone % 3600L) {
            offset = (int) labs(time_zone % 3600L);
            sprintf(put_tz + strlen(put_tz), ":%02d", offset / 60);
            if (offset % 60)
                sprintf(put_tz + strlen(put_tz), ":%02d", offset % 60);
        }
        if (day_light)
            strcat(put_tz,"DST");
        setenv(TZ_ENVVAR, put_tz, 1);
    }
}
#endif /* __SASC */

/* set state as well as possible from settings found in locale.library */
int GetPlatformLocalTimezone(sp, fill_tzstate_from_rules)
     register struct state * ZCONST sp;
     void (*fill_tzstate_from_rules)(struct state * ZCONST sp_res,
                                     ZCONST struct rule * ZCONST start,
                                     ZCONST struct rule * ZCONST end);
{
    struct Library *LocaleBase;
    struct Locale *ll;
    struct Process *me = (void *) FindTask(NULL);
    void *old_window = me->pr_WindowPtr;
    BPTR eh;
    int z, valid = FALSE;

    /* read timezone from locale.library if TZ envvar missing */
    me->pr_WindowPtr = (void *) -1;   /* suppress any "Please insert" popups */
    if (LocaleBase = OpenLibrary("locale.library", 0)) {
        if (ll = OpenLocale(NULL)) {
            z = ll->loc_GMTOffset;    /* in minutes */
            if (z == -300) {
                if (eh = Lock("ENV:sys/locale.prefs", ACCESS_READ)) {
                    UnLock(eh);
                    valid = TRUE;
                } else
                    z = 300; /* bug: locale not initialized, default bogus! */
            } else
                valid = TRUE;
            if (valid) {
                struct rule startrule, stoprule;

                sp->timecnt = 0;
                sp->typecnt = 1;
                sp->charcnt = 2;
                sp->chars[0] = sp->chars[1] = '\0';
                sp->ttis[0].tt_abbrind = 0;
                sp->ttis[1].tt_abbrind = 1;
                sp->ttis[0].tt_gmtoff = -z * MINSPERHOUR;
                sp->ttis[1].tt_gmtoff = -z * MINSPERHOUR + SECSPERHOUR;
                sp->ttis[0].tt_isdst = 0;
                sp->ttis[1].tt_isdst = 1;
                stoprule.r_type = MONTH_NTH_DAY_OF_WEEK;
                stoprule.r_day = 0;
                stoprule.r_week = 5;
                stoprule.r_mon = 10;
                stoprule.r_time = 2 * SECSPERHOUR;
                startrule = stoprule;
                startrule.r_mon = 4;
                startrule.r_week = 1;
                if (z >= -180 && z < 150) {
                    /* At this point we make a really gratuitous assumption: */
                    /* if the time zone could be Europe, we use the European */
                    /* Union rules without checking what country we're in.   */
                    /* The AmigaDOS locale country codes do not, at least in */
                    /* 2.x versions of the OS, recognize very many countries */
                    /* outside of Europe and North America.                  */
                    sp->typecnt = 2;
                    startrule.r_mon = 3;   /* one week earlier than US DST */
                    startrule.r_week = 5;
                } else if (z >= 150 && z <= 480 &&
                           /* no DST in alaska, hawaii */
                           (ll->loc_CountryCode == 0x55534100 /*"USA"*/ ||
                            ll->loc_CountryCode == 0x43414E00 /*"CAN"*/))
                    sp->typecnt = 2;
                    /* We check the country code for U.S. or Canada because */
                    /* most of Latin America has no DST.  Even in these two */
                    /* countries there are some exceptions...               */
                /* else if...  Feel free to add more cases here! */

                if (sp->typecnt > 1)
                    (*fill_tzstate_from_rules)(sp, &startrule, &stoprule);
            }
            CloseLocale(ll);
        }
        CloseLibrary(LocaleBase);
    }
    me->pr_WindowPtr = old_window;
    return valid;
}

#ifdef ZIP
time_t time(time_t *tp)
{
    time_t t;
    struct DateStamp ds;
    DateStamp(&ds);
    t = ds.ds_Tick / TICKS_PER_SECOND + ds.ds_Minute * 60
                                      + (ds.ds_Days + 2922) * SECSPERDAY;
    t = mktime(gmtime(&t));
    /* gmtime leaves ds in the local timezone, mktime converts it to GMT */
    if (tp) *tp = t;
    return t;
}
#endif /* ZIP */

#endif /* !FUNZIP && !UTIL */


#if CRYPT || !defined(FUNZIP)

/*  sendpkt.c
 *  by A. Finkel, P. Lindsay, C. Sheppner
 *  returns Res1 of the reply packet
 */
/*
#include <exec/types.h>
#include <exec/memory.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <proto/exec.h>
#include <proto/dos.h>
*/

LONG sendpkt(struct MsgPort *pid, LONG action, LONG *args, LONG nargs);

LONG sendpkt(pid,action,args,nargs)
struct MsgPort *pid;           /* process identifier (handler message port) */
LONG action,                   /* packet type (desired action)              */
     *args,                    /* a pointer to argument list                */
     nargs;                    /* number of arguments in list               */
{

    struct MsgPort *replyport, *CreatePort(UBYTE *, long);
    void DeletePort(struct MsgPort *);
    struct StandardPacket *packet;
    LONG count, *pargs, res1;

    replyport = CreatePort(NULL,0L);
    if( !replyport ) return(0);

    packet = (struct StandardPacket *)AllocMem(
            (long)sizeof(struct StandardPacket),MEMF_PUBLIC|MEMF_CLEAR);
    if( !packet )
    {
        DeletePort(replyport);
        return(0);
    }

    packet->sp_Msg.mn_Node.ln_Name  = (char *)&(packet->sp_Pkt);
    packet->sp_Pkt.dp_Link          = &(packet->sp_Msg);
    packet->sp_Pkt.dp_Port          = replyport;
    packet->sp_Pkt.dp_Type          = action;

    /* copy the args into the packet */
    pargs = &(packet->sp_Pkt.dp_Arg1);      /* address of 1st argument */
    for( count=0; count<nargs; count++ )
        pargs[count] = args[count];

    PutMsg(pid,(struct Message *)packet);   /* send packet */

    WaitPort(replyport);
    GetMsg(replyport);

    res1 = packet->sp_Pkt.dp_Res1;

    FreeMem((char *)packet,(long)sizeof(*packet));
    DeletePort(replyport);

    return(res1);

} /* sendpkt() */

#endif /* CRYPT || !FUNZIP */


#if CRYPT || (defined(UNZIP) && !defined(FUNZIP))

/* Agetch() reads one raw keystroke -- uses sendpkt() */

int Agetch(void)
{
    LONG sendpkt(struct MsgPort *pid, LONG action, LONG *args, LONG nargs);
    struct Task *me = FindTask(NULL);
    struct CommandLineInterface *cli = BADDR(((struct Process *) me)->pr_CLI);
    BPTR fh = cli->cli_StandardInput;   /* this is immune to < redirection */
    void *conp = ((struct FileHandle *) BADDR(fh))->fh_Type;
    char longspace[8];
    long *flag = (long *) ((ULONG) &longspace[4] & ~3); /* LONGWORD ALIGNED! */
    UBYTE c;

    *flag = 1;
    sendpkt(conp, ACTION_SCREEN_MODE, flag, 1);         /* assume success */
    Read(fh, &c, 1);
    *flag = 0;
    sendpkt(conp, ACTION_SCREEN_MODE, flag, 1);
    if (c == 3)                                         /* ^C in input */
        Signal(me, SIGBREAKF_CTRL_C);
    return c;
}

#endif /* CRYPT || (UNZIP && !FUNZIP) */

#endif /* __amiga_filedate_c*/
