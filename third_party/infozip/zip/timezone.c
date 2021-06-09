/*
  timezone.c - Zip 3

  Copyright (c) 1990-2004 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2003-May-08 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/* Replacement time library functions, based on platform independent public
 * domain timezone code from ftp://elsie.nci.nih.gov/pub, with mktime and
 * mkgmtime from our own mktime.c in Zip.
 *
 * Contains:  tzset()
 *            __tzset()
 *            gmtime()
 *            localtime()
 *            mktime()
 *            mkgmtime()
 *            GetPlatformLocalTimezone()  [different versions]
 */

/* HISTORY/CHANGES
 * 17 Jun 00, Paul Kienitz, added the PD-based tzset(), localtime(), and so on
 *            to amiga/filedate.c, replacing GNU-based functions which had
 *            replaced time_lib.c, both having been rejected for licensing
 *            reasons.  Support for timezone files and leap seconds was removed.
 *
 * 23 Aug 00, Paul Kienitz, split into separate timezone.c file, made platform
 *            independent, copied in mktime() and mkgmtime() from Zip, renamed
 *            locale_TZ as GetPlatformLocalTimezone(), for use as a generic
 *            hook by other platforms.
 */

#ifndef __timezone_c
#define __timezone_c


#include "zip.h"
#include "timezone.h"
#include <ctype.h>
#include <errno.h>

#ifdef IZTZ_DEFINESTDGLOBALS
long timezone = 0;
int daylight = 0;
char *tzname[2];
#endif

#ifndef IZTZ_GETLOCALETZINFO
#  define IZTZ_GETLOCALETZINFO(ptzstruct, pgenrulefunct) (FALSE)
#endif

int real_timezone_is_set = FALSE;       /* set by tzset() */


#define TZDEFRULESTRING ",M4.1.0,M10.5.0"
#define TZDEFAULT       "EST5EDT"

#define SECSPERMIN      60
#define MINSPERHOUR     60
#define HOURSPERDAY     24
#define DAYSPERWEEK     7
#define DAYSPERNYEAR    365
#define DAYSPERLYEAR    366
#define SECSPERHOUR     (SECSPERMIN * MINSPERHOUR)
#define SECSPERDAY      ((long) SECSPERHOUR * HOURSPERDAY)
#define MONSPERYEAR 12

#define EPOCH_WDAY      4     /* Jan 1, 1970 was thursday */
#define EPOCH_YEAR      1970
#define TM_YEAR_BASE    1900
#define FIRST_GOOD_YEAR ((time_t) -1 < (time_t) 1 ? EPOCH_YEAR-68 : EPOCH_YEAR)
#define LAST_GOOD_YEAR  (EPOCH_YEAR + ((time_t) -1 < (time_t) 1 ? 67 : 135))

#define YDAYS(month, year) yr_days[leap(year)][month]

/* Nonzero if `y' is a leap year, else zero. */
#define leap(y)  (((y) % 4 == 0 && (y) % 100 != 0) || (y) % 400 == 0)

/* Number of leap years from EPOCH_YEAR  to `y' (not including `y' itself). */
#define _P4      ((EPOCH_YEAR / 4) * 4 + 1)
#define _P100    ((EPOCH_YEAR / 100) * 100 + 1)
#define _P400    ((EPOCH_YEAR / 400) * 400 + 1)
#define nleap(y) (((y) - _P4) / 4 - ((y) - _P100) / 100 + ((y) - _P400) / 400)

/* Length of month `m' (0 .. 11) */
#define monthlen(m, y) (yr_days[0][(m)+1] - yr_days[0][m] + \
                        ((m) == 1 && leap(y)))

/* internal module-level constants */
#ifndef IZ_MKTIME_ONLY
static ZCONST char  gmt[] = "GMT";
static ZCONST int    mon_lengths[2][MONSPERYEAR] = {
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
    { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};
#endif /* !IZ_MKTIME_ONLY */
static ZCONST int    yr_days[2][MONSPERYEAR+1] = {
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};
#ifndef IZ_MKTIME_ONLY
static ZCONST int   year_lengths[2] = {
    DAYSPERNYEAR, DAYSPERLYEAR
};

/* internal variables */
static struct state statism;


/* prototypes of static functions */
static time_t transtime OF((ZCONST time_t janfirst, ZCONST int year,
                            ZCONST struct rule * ZCONST rulep,
                            ZCONST long offset));
static void generate_transitions OF((register struct state * ZCONST sp,
                                     ZCONST struct rule * ZCONST start,
                                     ZCONST struct rule * ZCONST end));
static ZCONST char *getzname OF((ZCONST char *strp));
static ZCONST char *getnum OF((ZCONST char *strp, int * ZCONST nump,
                               ZCONST int min, ZCONST int max));
static ZCONST char *getsecs OF((ZCONST char *strp, long * ZCONST secsp));
static ZCONST char *getoffset OF((ZCONST char *strp, long * ZCONST offsetp));
static ZCONST char *getrule OF((ZCONST char *strp, struct rule * ZCONST rulep));
static int Parse_TZ OF((ZCONST char *name, register struct state * ZCONST sp));


static time_t transtime(janfirst, year, rulep, offset)
     ZCONST time_t janfirst;
     ZCONST int year;
     ZCONST struct rule * ZCONST rulep;
     ZCONST long offset;
{
    register int    leapyear;
    register time_t value;
    register int    i;
    int             d, m1, yy0, yy1, yy2, dow;

    value = 0;
    leapyear = leap(year);
    switch (rulep->r_type) {

    case JULIAN_DAY:
        /*
        ** Jn - Julian day, 1 == January 1, 60 == March 1 even in leap
        ** years.
        ** In non-leap years, or if the day number is 59 or less, just
        ** add SECSPERDAY times the day number-1 to the time of
        ** January 1, midnight, to get the day.
        */
        value = janfirst + (rulep->r_day - 1) * SECSPERDAY;
        if (leapyear && rulep->r_day >= 60)
            value += SECSPERDAY;
        break;

    case DAY_OF_YEAR:
        /*
        ** n - day of year.
        ** Just add SECSPERDAY times the day number to the time of
        ** January 1, midnight, to get the day.
        */
        value = janfirst + rulep->r_day * SECSPERDAY;
        break;

    case MONTH_NTH_DAY_OF_WEEK:
        /*
        ** Mm.n.d - nth "dth day" of month m.
        */
        value = janfirst;
/*
        for (i = 0; i < rulep->r_mon - 1; ++i)
            value += mon_lengths[leapyear][i] * SECSPERDAY;
*/
        value += yr_days[leapyear][rulep->r_mon - 1] * SECSPERDAY;

        /*
        ** Use Zeller's Congruence to get day-of-week of first day of
        ** month.
        */
        m1 = (rulep->r_mon + 9) % 12 + 1;
        yy0 = (rulep->r_mon <= 2) ? (year - 1) : year;
        yy1 = yy0 / 100;
        yy2 = yy0 % 100;
        dow = ((26 * m1 - 2) / 10 +
            1 + yy2 + yy2 / 4 + yy1 / 4 - 2 * yy1) % 7;
        if (dow < 0)
            dow += DAYSPERWEEK;

        /*
        ** "dow" is the day-of-week of the first day of the month.  Get
        ** the day-of-month (zero-origin) of the first "dow" day of the
        ** month.
        */
        d = rulep->r_day - dow;
        if (d < 0)
            d += DAYSPERWEEK;
        for (i = 1; i < rulep->r_week; ++i) {
            if (d + DAYSPERWEEK >= mon_lengths[leapyear][rulep->r_mon - 1])
                break;
            d += DAYSPERWEEK;
        }

        /*
        ** "d" is the day-of-month (zero-origin) of the day we want.
        */
        value += d * SECSPERDAY;
        break;
    }

    /*
    ** "value" is the Epoch-relative time of 00:00:00 UTC on the day in
    ** question.  To get the Epoch-relative time of the specified local
    ** time on that day, add the transition time and the current offset
    ** from UTC.
    */
    return value + rulep->r_time + offset;
}

static void generate_transitions(sp, start, end)
     register struct state * ZCONST sp;
     ZCONST struct rule * ZCONST start;
     ZCONST struct rule * ZCONST end;
{
    register int             year;
    register time_t          janfirst;
    time_t                   starttime;
    time_t                   endtime;
    long                     stdoffset = -sp->ttis[0].tt_gmtoff;
    long                     dstoffset = -sp->ttis[1].tt_gmtoff;
    register time_t *        atp;
    register unsigned char * typep;

    /*
    ** Two transitions per year, from EPOCH_YEAR to LAST_GOOD_YEAR.
    */
    sp->timecnt = 2 * (LAST_GOOD_YEAR - EPOCH_YEAR + 1);
    atp = sp->ats;
    typep = sp->types;
    janfirst = 0;
    for (year = EPOCH_YEAR; year <= LAST_GOOD_YEAR; ++year) {
        starttime = transtime(janfirst, year, start, stdoffset);
        endtime = transtime(janfirst, year, end, dstoffset);
        if (starttime > endtime) {
            *atp++ = endtime;
            *typep++ = 0;   /* DST ends */
            *atp++ = starttime;
            *typep++ = 1;   /* DST begins */
        } else {
            *atp++ = starttime;
            *typep++ = 1;   /* DST begins */
            *atp++ = endtime;
            *typep++ = 0;   /* DST ends */
        }
        janfirst += year_lengths[leap(year)] * SECSPERDAY;
    }
}

static ZCONST char *getzname(strp)
     ZCONST char *strp;
{
    register char   c;

    while ((c = *strp) != '\0' && !isdigit(c) && c != ',' && c != '-' &&
        c != '+')
            ++strp;
    return strp;
}

static ZCONST char *getnum(strp, nump, min, max)
     ZCONST char *strp;
     int * ZCONST nump;
     ZCONST int min;
     ZCONST int max;
{
    register char   c;
    register int    num;

    if (strp == NULL || !isdigit(c = *strp))
        return NULL;
    num = 0;
    do {
        num = num * 10 + (c - '0');
        if (num > max)
            return NULL;    /* illegal value */
        c = *++strp;
    } while (isdigit(c));
    if (num < min)
        return NULL;        /* illegal value */
    *nump = num;
    return strp;
}

static ZCONST char *getsecs(strp, secsp)
     ZCONST char *strp;
     long * ZCONST secsp;
{
    int num;

    /*
    ** `HOURSPERDAY * DAYSPERWEEK - 1' allows quasi-Posix rules like
    ** "M10.4.6/26", which does not conform to Posix,
    ** but which specifies the equivalent of
    ** ``02:00 on the first Sunday on or after 23 Oct''.
    */
    strp = getnum(strp, &num, 0, HOURSPERDAY * DAYSPERWEEK - 1);
    if (strp == NULL)
        return NULL;
    *secsp = num * (long) SECSPERHOUR;
    if (*strp == ':') {
        ++strp;
        strp = getnum(strp, &num, 0, MINSPERHOUR - 1);
        if (strp == NULL)
            return NULL;
        *secsp += num * SECSPERMIN;
        if (*strp == ':') {
            ++strp;
            /* `SECSPERMIN' allows for leap seconds.  */
            strp = getnum(strp, &num, 0, SECSPERMIN);
            if (strp == NULL)
                return NULL;
            *secsp += num;
        }
    }
    return strp;
}

static ZCONST char *getoffset(strp, offsetp)
     ZCONST char *strp;
     long * ZCONST offsetp;
{
    register int    neg = 0;

    if (*strp == '-') {
        neg = 1;
        ++strp;
    } else if (*strp == '+')
        ++strp;
    strp = getsecs(strp, offsetp);
    if (strp == NULL)
        return NULL;        /* illegal time */
    if (neg)
        *offsetp = -*offsetp;
    return strp;
}

static ZCONST char *getrule(strp, rulep)
     ZCONST char *strp;
     struct rule * ZCONST rulep;
{
    if (*strp == 'J') {
        /*
        ** Julian day.
        */
        rulep->r_type = JULIAN_DAY;
        ++strp;
        strp = getnum(strp, &rulep->r_day, 1, DAYSPERNYEAR);
    } else if (*strp == 'M') {
        /*
        ** Month, week, day.
        */
        rulep->r_type = MONTH_NTH_DAY_OF_WEEK;
        ++strp;
        strp = getnum(strp, &rulep->r_mon, 1, MONSPERYEAR);
        if (strp == NULL)
            return NULL;
        if (*strp++ != '.')
            return NULL;
        strp = getnum(strp, &rulep->r_week, 1, 5);
        if (strp == NULL)
            return NULL;
        if (*strp++ != '.')
            return NULL;
        strp = getnum(strp, &rulep->r_day, 0, DAYSPERWEEK - 1);
    } else if (isdigit(*strp)) {
        /*
        ** Day of year.
        */
        rulep->r_type = DAY_OF_YEAR;
        strp = getnum(strp, &rulep->r_day, 0, DAYSPERLYEAR - 1);
    } else  return NULL;        /* invalid format */
    if (strp == NULL)
        return NULL;
    if (*strp == '/') {
        /*
        ** Time specified.
        */
        ++strp;
        strp = getsecs(strp, &rulep->r_time);
    } else
        rulep->r_time = 2 * SECSPERHOUR;    /* default = 2:00:00 */
    return strp;
}

static int Parse_TZ(name, sp)
     ZCONST char *name;
     register struct state * ZCONST sp;
{
    ZCONST char *            stdname;
    ZCONST char *            dstname;
    size_t                   stdlen;
    size_t                   dstlen;
    long                     stdoffset;
    long                     dstoffset;
    register char *          cp;

    dstname = NULL;
    stdname = name;
    name = getzname(name);
    stdlen = name - stdname;
    if (stdlen < 3)
        return -1;
    if (*name == '\0')
        return -1;
    name = getoffset(name, &stdoffset);
    if (name == NULL)
        return -1;
    if (*name != '\0') {
        dstname = name;
        name = getzname(name);
        dstlen = name - dstname;    /* length of DST zone name */
        if (dstlen < 3)
            return -1;
        if (*name != '\0' && *name != ',' && *name != ';') {
            name = getoffset(name, &dstoffset);
            if (name == NULL)
                return -1;
        } else
            dstoffset = stdoffset - SECSPERHOUR;
        if (*name == '\0')
            name = TZDEFRULESTRING;
        if (*name == ',' || *name == ';') {
            struct rule     start;
            struct rule     end;

            ++name;
            if ((name = getrule(name, &start)) == NULL)
                return -1;
            if (*name++ != ',')
                return -1;
            if ((name = getrule(name, &end)) == NULL)
                return -1;
            if (*name != '\0')
                return -1;
            sp->typecnt = 2;    /* standard time and DST */
            sp->ttis[0].tt_gmtoff = -stdoffset;
            sp->ttis[0].tt_isdst = 0;
            sp->ttis[0].tt_abbrind = 0;
            sp->ttis[1].tt_gmtoff = -dstoffset;
            sp->ttis[1].tt_isdst = 1;
            sp->ttis[1].tt_abbrind = stdlen + 1;
            generate_transitions(sp, &start, &end);
        }
    } else {
        dstlen = 0;
        sp->typecnt = 1;        /* only standard time */
        sp->timecnt = 0;
        sp->ttis[0].tt_gmtoff = -stdoffset;
        sp->ttis[0].tt_isdst = 0;
        sp->ttis[0].tt_abbrind = 0;
    }
    sp->charcnt = stdlen + 1;
    if (dstlen != 0)
        sp->charcnt += dstlen + 1;
    if ((size_t) sp->charcnt > sizeof(sp->chars))
        return -1;
    cp = sp->chars;
    (void) strncpy(cp, stdname, stdlen);
    cp += stdlen;
    *cp++ = '\0';
    if (dstlen != 0) {
        (void) strncpy(cp, dstname, dstlen);
        *(cp + dstlen) = '\0';
    }
    return 0;
}

void tzset()
{
    char *TZstring;
    int dstfirst;
    static char *old_TZstring = NULL;

    TZstring = getenv("TZ");    /* read TZ envvar */
    if (old_TZstring && TZstring && !strcmp(old_TZstring, TZstring))
        /* do not repeatedly parse an unchanged TZ specification */
        return;
    if ((TZstring && TZstring[0] && Parse_TZ(TZstring, &statism) == 0)
                || IZTZ_GETLOCALETZINFO(&statism, generate_transitions)
                || Parse_TZ(gmt, &statism) == 0) {
        daylight  = statism.typecnt > 1;
        dstfirst  = daylight && statism.ttis[0].tt_isdst && !statism.ttis[1].tt_isdst;
        timezone  = -statism.ttis[dstfirst].tt_gmtoff;
        tzname[0] = statism.chars + statism.ttis[dstfirst].tt_abbrind;
        tzname[1] = statism.chars + statism.ttis[!dstfirst].tt_abbrind;
        real_timezone_is_set = TRUE;
        if (TZstring) {
            if (old_TZstring)
                old_TZstring = realloc(old_TZstring, strlen(TZstring) + 1);
            else
                old_TZstring = malloc(strlen(TZstring) + 1);
            if (old_TZstring)
                strcpy(old_TZstring, TZstring);
        }
    } else {
        timezone = 0;   /* default is GMT0 which means no offsets */
        daylight = 0;   /* from local system time                 */
        real_timezone_is_set = FALSE;
        if (old_TZstring) {
            free(old_TZstring);
            old_TZstring = NULL;
        }
    }
#ifdef IZTZ_SETLOCALTZINFO
    /* Some SAS/C library functions, e.g. stat(), call library       */
    /* __tzset() themselves. So envvar TZ *must* exist in order to   */
    /* to get the right offset from GMT.  XXX  TRY HARD to fix this! */
    set_TZ(timezone, daylight);
#endif /* IZTZ_SETLOCALTZINFO */
}

/* XXX  Does this also help SAS/C library work? */
void __tzset()
{
    if (!real_timezone_is_set) tzset();
}

static struct tm _tmbuf;

struct tm *gmtime(when)
     ZCONST time_t *when;
{
    long days = *when / SECSPERDAY;
    long secs = *when % SECSPERDAY;
    int isleap;

    memset(&_tmbuf, 0, sizeof(_tmbuf));   /* get any nonstandard fields */
    _tmbuf.tm_wday = (days + EPOCH_WDAY) % 7;
    _tmbuf.tm_year = EPOCH_YEAR - TM_YEAR_BASE;
    isleap = leap(_tmbuf.tm_year + TM_YEAR_BASE);
    while (days >= year_lengths[isleap]) {
        days -= year_lengths[isleap];
        _tmbuf.tm_year++;
        isleap = leap(_tmbuf.tm_year + TM_YEAR_BASE);
    }
    _tmbuf.tm_mon = 0;
    _tmbuf.tm_yday = days;
    while (days >= mon_lengths[isleap][_tmbuf.tm_mon])
        days -= mon_lengths[isleap][_tmbuf.tm_mon++];
    _tmbuf.tm_mday = days + 1;
    _tmbuf.tm_isdst = 0;
    _tmbuf.tm_sec = secs % SECSPERMIN;
    _tmbuf.tm_min = (secs / SECSPERMIN) % SECSPERMIN;
    _tmbuf.tm_hour = secs / SECSPERHOUR;
    return &_tmbuf;
}

struct tm *localtime(when)
     ZCONST time_t *when;
{
    time_t     localwhen = *when;
    int        timetype;
    struct tm *ret;

    __tzset();
    if (statism.timecnt == 0 || localwhen < statism.ats[0])
        timetype = statism.ttis[0].tt_isdst && statism.typecnt > 1 &&
                   !statism.ttis[1].tt_isdst;
    else {
        for (timetype = 1; timetype < statism.timecnt; ++timetype)
            if (localwhen < statism.ats[timetype])
                break;
        timetype = statism.types[timetype - 1];
    }
    localwhen += statism.ttis[timetype].tt_gmtoff;
    ret = gmtime(&localwhen);
    ret->tm_isdst = statism.ttis[timetype].tt_isdst;
    return ret;
}

#ifdef NEED__ISINDST
int _isindst(tb)
    struct tm *tb;
{
    time_t     localt;          /* time_t equivalent of given tm struct */
    time_t     univt;           /* assumed UTC value of given time */
    long       tzoffset_adj;    /* timezone-adjustment `remainder' */
    int        bailout_cnt;     /* counter of tries for tz correction */
    int        timetype;

    __tzset();

    /* when DST is unsupported in current timezone, DST is always off */
    if (statism.typecnt <= 1) return FALSE;

    localt = mkgmtime(tb);
    if (localt == (time_t)-1)
        /* specified time is out-of-range, default to FALSE */
        return FALSE;

    univt = localt - statism.ttis[0].tt_gmtoff;
    bailout_cnt = 3;
    do {
        if (statism.timecnt == 0 || univt < statism.ats[0])
            timetype = statism.ttis[0].tt_isdst && statism.typecnt > 1 &&
                       !statism.ttis[1].tt_isdst;
        else {
            for (timetype = 1; timetype < statism.timecnt; ++timetype)
                if (univt < statism.ats[timetype])
                    break;
            timetype = statism.types[timetype - 1];
        }
        if ((tzoffset_adj = localt - univt - statism.ttis[timetype].tt_gmtoff)
            == 0L)
            break;
        univt += tzoffset_adj;
    } while (--bailout_cnt > 0);

    /* return TRUE when DST is active at given time */
    return (statism.ttis[timetype].tt_isdst);
}
#endif /* NEED__ISINDST */
#endif /* !IZ_MKTIME_ONLY */

/* Return the equivalent in seconds past 12:00:00 a.m. Jan 1, 1970 GMT
   of the local time and date in the exploded time structure `tm',
   adjust out of range fields in `tm' and set `tm->tm_yday', `tm->tm_wday'.
   If `tm->tm_isdst < 0' was passed to mktime(), the correct setting of
   tm_isdst is determined and returned. Otherwise, mktime() assumes this
   field as valid; its information is used when converting local time
   to UTC.
   Return -1 if time in `tm' cannot be represented as time_t value. */

time_t mktime(tm)
     struct tm *tm;
{
  struct tm *ltm;               /* Local time. */
  time_t loctime;               /* The time_t value of local time. */
  time_t then;                  /* The time to return. */
  long tzoffset_adj;            /* timezone-adjustment `remainder' */
  int bailout_cnt;              /* counter of tries for tz correction */
  int save_isdst;               /* Copy of the tm->isdst input value */

  save_isdst = tm->tm_isdst;
  loctime = mkgmtime(tm);
  if (loctime == -1) {
    tm->tm_isdst = save_isdst;
    return (time_t)-1;
  }

  /* Correct for the timezone and any daylight savings time.
     The correction is verified and repeated when not correct, to
     take into account the rare case that a change to or from daylight
     savings time occurs between when it is the time in `tm' locally
     and when it is that time in Greenwich. After the second correction,
     the "timezone & daylight" offset should be correct in all cases. To
     be sure, we allow a third try, but then the loop is stopped. */
  bailout_cnt = 3;
  then = loctime;
  do {
    ltm = localtime(&then);
    if (ltm == (struct tm *)NULL ||
        (tzoffset_adj = loctime - mkgmtime(ltm)) == 0L)
      break;
    then += tzoffset_adj;
  } while (--bailout_cnt > 0);

  if (ltm == (struct tm *)NULL || tzoffset_adj != 0L) {
    /* Signal failure if timezone adjustment did not converge. */
    tm->tm_isdst = save_isdst;
    return (time_t)-1;
  }

  if (save_isdst >= 0) {
    if (ltm->tm_isdst  && !save_isdst)
    {
      if (then + 3600 < then)
        then = (time_t)-1;
      else
        then += 3600;
    }
    else if (!ltm->tm_isdst && save_isdst)
    {
      if (then - 3600 > then)
        then = (time_t)-1;
      else
        then -= 3600;
    }
    ltm->tm_isdst = save_isdst;
  }

  if (tm != ltm)  /* `tm' may already point to localtime's internal storage */
    *tm = *ltm;

  return then;
}


#ifndef NO_TIME_T_MAX
   /* Provide default values for the upper limit of the time_t range.
      These are the result of the decomposition into a `struct tm' for
      the time value 0xFFFFFFFEL ( = (time_t)-2 ).
      Note: `(time_t)-1' is reserved for "invalid time"!  */
#  ifndef TM_YEAR_MAX
#    define TM_YEAR_MAX         2106
#  endif
#  ifndef TM_MON_MAX
#    define TM_MON_MAX          1       /* February */
#  endif
#  ifndef TM_MDAY_MAX
#    define TM_MDAY_MAX         7
#  endif
#  ifndef TM_HOUR_MAX
#    define TM_HOUR_MAX         6
#  endif
#  ifndef TM_MIN_MAX
#    define TM_MIN_MAX          28
#  endif
#  ifndef TM_SEC_MAX
#    define TM_SEC_MAX          14
#  endif
#endif /* NO_TIME_T_MAX */

/* Adjusts out-of-range values for `tm' field `tm_member'. */
#define ADJUST_TM(tm_member, tm_carry, modulus) \
  if ((tm_member) < 0) { \
    tm_carry -= (1 - ((tm_member)+1) / (modulus)); \
    tm_member = (modulus-1) + (((tm_member)+1) % (modulus)); \
  } else if ((tm_member) >= (modulus)) { \
    tm_carry += (tm_member) / (modulus); \
    tm_member = (tm_member) % (modulus); \
  }

/* Return the equivalent in seconds past 12:00:00 a.m. Jan 1, 1970 GMT
   of the Greenwich Mean time and date in the exploded time structure `tm'.
   This function does always put back normalized values into the `tm' struct,
   parameter, including the calculated numbers for `tm->tm_yday',
   `tm->tm_wday', and `tm->tm_isdst'.
   Returns -1 if the time in the `tm' parameter cannot be represented
   as valid `time_t' number. */

time_t mkgmtime(tm)
     struct tm *tm;
{
  int years, months, days, hours, minutes, seconds;

  years = tm->tm_year + TM_YEAR_BASE;   /* year - 1900 -> year */
  months = tm->tm_mon;                  /* 0..11 */
  days = tm->tm_mday - 1;               /* 1..31 -> 0..30 */
  hours = tm->tm_hour;                  /* 0..23 */
  minutes = tm->tm_min;                 /* 0..59 */
  seconds = tm->tm_sec;                 /* 0..61 in ANSI C. */

  ADJUST_TM(seconds, minutes, 60)
  ADJUST_TM(minutes, hours, 60)
  ADJUST_TM(hours, days, 24)
  ADJUST_TM(months, years, 12)
  if (days < 0)
    do {
      if (--months < 0) {
        --years;
        months = 11;
      }
      days += monthlen(months, years);
    } while (days < 0);
  else
    while (days >= monthlen(months, years)) {
      days -= monthlen(months, years);
      if (++months >= 12) {
        ++years;
        months = 0;
      }
    }

  /* Restore adjusted values in tm structure */
  tm->tm_year = years - TM_YEAR_BASE;
  tm->tm_mon = months;
  tm->tm_mday = days + 1;
  tm->tm_hour = hours;
  tm->tm_min = minutes;
  tm->tm_sec = seconds;

  /* Set `days' to the number of days into the year. */
  days += YDAYS(months, years);
  tm->tm_yday = days;

  /* Now calculate `days' to the number of days since Jan 1, 1970. */
  days = (unsigned)days + 365 * (unsigned)(years - EPOCH_YEAR) +
         (unsigned)(nleap (years));
  tm->tm_wday = ((unsigned)days + EPOCH_WDAY) % 7;
  tm->tm_isdst = 0;

  if (years < EPOCH_YEAR)
    return (time_t)-1;

#if (defined(TM_YEAR_MAX) && defined(TM_MON_MAX) && defined(TM_MDAY_MAX))
#if (defined(TM_HOUR_MAX) && defined(TM_MIN_MAX) && defined(TM_SEC_MAX))
  if (years > TM_YEAR_MAX ||
      (years == TM_YEAR_MAX &&
       (tm->tm_yday > (YDAYS(TM_MON_MAX, TM_YEAR_MAX) + (TM_MDAY_MAX - 1)) ||
        (tm->tm_yday == (YDAYS(TM_MON_MAX, TM_YEAR_MAX) + (TM_MDAY_MAX - 1)) &&
         (hours > TM_HOUR_MAX ||
          (hours == TM_HOUR_MAX &&
           (minutes > TM_MIN_MAX ||
            (minutes == TM_MIN_MAX && seconds > TM_SEC_MAX) )))))))
    return (time_t)-1;
#endif
#endif

  return (time_t)(SECSPERDAY * (unsigned long)(unsigned)days +
                  SECSPERHOUR * (unsigned long)hours +
                  (unsigned long)(SECSPERMIN * minutes + seconds));
}

#endif /* __timezone_c */
