/*
  Copyright (c) 1990-2000 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/* -----------------------------------------------------------------------------

The original functions (Metrowerks Codewarrior pro 3.0) gmtime, localtime,
mktime and time do not work correctly. The supplied link library mactime.c
contains replacement functions for them.

 *     Caveat: On a Mac, we only know the GMT and DST offsets for
 *     the current time, not for the time in question.
 *     Mac has no support for DST handling.
 *     DST changeover is all manually set by the user.


------------------------------------------------------------------------------*/

/*****************************************************************************/
/*  Includes                                                                 */
/*****************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <OSUtils.h>

#include "mactime.h"


/*
The MacOS function GetDateTime returns  the
number of seconds elapsed since midnight, January 1, 1904.
*/
const unsigned long MacOS_2_Unix = 2082844800L;


/*****************************************************************************/
/*  Macros, typedefs                                                         */
/*****************************************************************************/


#ifndef TEST_TIME_LIB
#define my_gmtime    gmtime
#define my_localtime localtime
#define my_mktime    mktime
#define my_time      time
#endif


/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/
/* internal prototypes */
static void clear_tm(struct tm * tm);
static long GMTDelta(void);
static Boolean DaylightSaving(void);
static time_t GetTimeMac(void);
static time_t Mactime(time_t *timer);
static void   normalize(int *i,int *j,int norm);
static struct tm *time2tm(const time_t *timer);
static time_t tm2time(struct tm *tp);

/* Because serial port and SLIP conflict with ReadXPram calls,
   we cache the call here so we don't hang on calling ReadLocation()  */
static void myReadLocation(MachineLocation * loc);


/* prototypes for STD lib replacement functions */
struct tm *my_gmtime(const time_t *t);
struct tm *my_localtime(const time_t *t);
time_t my_mktime(struct tm *tp);
time_t my_time(time_t *t);


/*****************************************************************************/
/*  Functions                                                                */
/*****************************************************************************/

 /*
 *  Mac file times are based on 1904 Jan 1 00:00 local time,
 *  not 1970 Jan 1 00:00 UTC.
 *  So we have to convert the time stamps into UNIX UTC
 *  compatible values.
 */
time_t MacFtime2UnixFtime(unsigned long macftime)
{
    long UTCoffset;

    GetGMToffsetMac(macftime, &UTCoffset);
    MACOS_TO_UNIX(macftime);
    macftime -= UTCoffset;

    return macftime;
}


 /*
 *  Mac file times are based on 1904 Jan 1 00:00 local time,
 *  not 1970 Jan 1 00:00 UTC.
 *  So we have to convert the time stamps into MacOS local
 *  compatible values.
 */
unsigned long UnixFtime2MacFtime(time_t unxftime)
{
    long UTCoffset;
    unsigned long macftime = unxftime;

    UNIX_TO_MACOS(macftime);
    GetGMToffsetMac(macftime, &UTCoffset);
    macftime += UTCoffset;

    return macftime;
}





/*
* This function convert a file-localtime to an another
* file-localtime.
*/
time_t AdjustForTZmoveMac(unsigned long macloctim, long s_gmtoffs)
{
    time_t MacGMTTime;
    long UTCoffset;

    /* convert macloctim into corresponding UTC value */
    MacGMTTime = macloctim - s_gmtoffs;
    GetGMToffsetMac(macloctim, &UTCoffset);

    return (MacGMTTime + UTCoffset);
} /* AdjustForTZmove() */




/*
 * This function calculates the difference between the supplied Mac
 * ftime value (local time) and the corresponding UTC time in seconds.
 */
Boolean GetGMToffsetMac(unsigned long mactime, long *UTCoffset)
{

mactime = mactime;
/*
 *     Caveat: On a Mac, we only know the GMT and DST offsets for
 *     the current time, not for the time in question.
 *     Mac has no support for DST handling.
 *     DST changeover is all manually set by the user.

 May be later I can include a support of GMT offset calculation for the
 time in question here.
*/
    *UTCoffset = GMTDelta();

    return true;
}







/*****************************************************************************
 *  Standard Library Replacement Functions
 *  gmtime(), mktime(), localtime(), time()
 *
 *  The unix epoch is used here.
 *  These functions gmtime(), mktime(), localtime() and time()
 *  expects and returns unix times.
 *
 * At midnight Jan. 1, 1970 GMT, the local time was
 *    midnight Jan. 1, 1970 + GMTDelta().
 *
 *
 *****************************************************************************/


struct tm *my_gmtime(const time_t *timer)
{
    return time2tm(timer);
}




struct tm *my_localtime(const time_t *timer)
{
    time_t maclocal;

    maclocal = *timer;
    maclocal += GMTDelta();

    return time2tm(&maclocal);
}




time_t my_mktime(struct tm *tp)
{
    time_t maclocal;

    maclocal = tm2time(tp);
    maclocal -= GMTDelta();

    return maclocal;
}






time_t my_time(time_t *time)
{
time_t tmp_time;

GetDateTime(&tmp_time);

MACOS_TO_UNIX(tmp_time);

if (time)
    {
    *time = tmp_time;
    }

return tmp_time;
}



/*****************************************************************************/
/*  static module level functions
/*****************************************************************************/


/*
 * The geographic location and time zone information of a Mac
 * are stored in extended parameter RAM.  The ReadLocation
 * produdure uses the geographic location record, MachineLocation,
 * to read the geographic location and time zone information in
 * extended parameter RAM.
 *
 * Because serial port and SLIP conflict with ReadXPram calls,
 * we cache the call here.
 *
 * Caveat: this caching will give the wrong result if a session
 * extend across the DST changeover time, but
 * this function resets itself every 2 hours.
 */
static void myReadLocation(MachineLocation * loc)
{
    static MachineLocation storedLoc;   /* InsideMac, OSUtilities, page 4-20  */
    static time_t first_call = 0, last_call = 86400;

    if ((last_call - first_call) > 7200)
        {
        GetDateTime(&first_call);
        ReadLocation(&storedLoc);
        }

    GetDateTime(&last_call);
    *loc = storedLoc;
}




static Boolean DaylightSaving(void)
{
    MachineLocation loc;
    unsigned char dlsDelta;

    myReadLocation(&loc);
    dlsDelta =  loc.u.dlsDelta;

    return (dlsDelta != 0);
}




/* current local time = GMTDelta() + GMT
   GMT = local time - GMTDelta()    */
static long GMTDelta(void)
{
    MachineLocation loc;
    long gmtDelta;

    myReadLocation(&loc);

    /*
     * On a Mac, the GMT value is in seconds east of GMT.  For example,
     * San Francisco is at -28,800 seconds (8 hours * 3600 seconds per hour)
     * east of GMT.  The gmtDelta field is a 3-byte value contained in a
     * long word, so you must take care to get it properly.
     */
    gmtDelta = loc.u.gmtDelta & 0x00FFFFFF;
    if ((gmtDelta & 0x00800000) != 0)
        {
        gmtDelta |= 0xFF000000;
        }

    return gmtDelta;
}



/* This routine simulates stdclib time(), time in seconds since 1.1.1970
   The time is in GMT  */
static time_t GetTimeMac(void)
{
    unsigned long maclocal;


    /*
     * Get the current time expressed as the number of seconds
     * elapsed since the Mac epoch, midnight, Jan. 1, 1904 (local time).
     * On a Mac, current time accuracy is up to a second.
     */

    GetDateTime(&maclocal);     /* Get Mac local time  */
    maclocal -= GMTDelta();     /* Get Mac GMT  */
    MACOS_TO_UNIX(maclocal);

    return maclocal;            /* return unix GMT  */
}




/*
 *  clear_tm - sets a broken-down time to the equivalent of 1970/1/1 00:00:00
 */

static void clear_tm(struct tm * tm)
{
    tm->tm_sec   =  0;
    tm->tm_min   =  0;
    tm->tm_hour  =  0;
    tm->tm_mday  =  1;
    tm->tm_mon   =  0;
    tm->tm_year  =  0;
    tm->tm_wday  =  1;
    tm->tm_yday  =  0;
    tm->tm_isdst = -1;
}


static void normalize(int *i,int *j,int norm)
{
  while(*i < 0)
    {
    *i += norm;
    (*j)--;
    }

  while(*i >= norm)
    {
    *i -= norm;
    (*j)++;
    }
}



/*  Returns the GMT times  */
static time_t Mactime(time_t *timer)
{
    time_t t = GetTimeMac();

    if (timer != NULL)
        *timer = t;

    return t;
}




static struct tm *time2tm(const time_t *timer)
{
    DateTimeRec dtr;
    MachineLocation loc;
    time_t macLocal = *timer;

    static struct tm statictime;
    static const short monthday[12] =
        {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

    UNIX_TO_MACOS(macLocal);
    SecondsToDate(macLocal, &dtr);

    statictime.tm_sec  = dtr.second;         /* second, from 0 to 59 */
    statictime.tm_min  = dtr.minute;         /* minute, from 0 to 59 */
    statictime.tm_hour = dtr.hour;           /* hour, from 0 to 23 */
    statictime.tm_mday = dtr.day;            /* day of the month, from 1 to 31 */
    statictime.tm_mon  = dtr.month     - 1;  /* month, 1= January and 12 = December */
    statictime.tm_year = dtr.year   - 1900;  /* year, ranging from 1904 to 2040 */
    statictime.tm_wday = dtr.dayOfWeek - 1;  /* day of the week, 1 = Sun, 7 = Sat */

    statictime.tm_yday = monthday[statictime.tm_mon]
                         + statictime.tm_mday - 1;

    if (2 < statictime.tm_mon && !(statictime.tm_year & 3))
        {
        ++statictime.tm_yday;
        }

    myReadLocation(&loc);
    statictime.tm_isdst = DaylightSaving();

    return(&statictime);
}





static time_t tm2time(struct tm *tp)
{
time_t intMacTime;
DateTimeRec  dtr;

 normalize(&tp->tm_sec, &tp->tm_min, 60);
 normalize(&tp->tm_min, &tp->tm_hour,60);
 normalize(&tp->tm_hour,&tp->tm_mday,24);
 normalize(&tp->tm_mon, &tp->tm_year,12);

 dtr.year    = tp->tm_year + 1900;  /* years since 1900 */
 dtr.month   = tp->tm_mon  +    1;  /* month, 0 = January and 11 = December */
 dtr.day     = tp->tm_mday;         /* day of the month, from 1 to 31 */
 dtr.hour    = tp->tm_hour;         /* hour, from 0 to 23 */
 dtr.minute  = tp->tm_min;          /* minute, from 0 to 59 */
 dtr.second  = tp->tm_sec;          /* second, from 0 to 59 */

 DateToSeconds(&dtr, &intMacTime);

 MACOS_TO_UNIX(intMacTime);

 return intMacTime;
}
