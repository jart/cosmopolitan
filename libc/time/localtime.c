/*-*- mode:c; indent-tabs-mode:t; tab-width:8; coding:utf-8                 -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
╚─────────────────────────────────────────────────────────────────────────────*/
#define LOCALTIME_IMPLEMENTATION
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/calls.h"
#include "libc/cxxabi.h"
#include "libc/serialize.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"
#include "libc/time/tz.internal.h"
#include "libc/time/tzfile.internal.h"

__static_yoink("zipos");
__static_yoink("usr/share/zoneinfo/");
__static_yoink("usr/share/zoneinfo/Anchorage");
__static_yoink("usr/share/zoneinfo/Beijing");
__static_yoink("usr/share/zoneinfo/Berlin");
__static_yoink("usr/share/zoneinfo/Boulder");
__static_yoink("usr/share/zoneinfo/Chicago");
__static_yoink("usr/share/zoneinfo/GMT");
__static_yoink("usr/share/zoneinfo/GST");
__static_yoink("usr/share/zoneinfo/Honolulu");
__static_yoink("usr/share/zoneinfo/Israel");
__static_yoink("usr/share/zoneinfo/Japan");
__static_yoink("usr/share/zoneinfo/London");
__static_yoink("usr/share/zoneinfo/Melbourne");
__static_yoink("usr/share/zoneinfo/New_York");
__static_yoink("usr/share/zoneinfo/UTC");

// clang-format off
/* Convert timestamp from time_t to struct tm.  */
/*
** This file is in the public domain, so clarified as of
** 1996-06-05 by Arthur David Olson.
*/

/*
** Leap second handling from Bradley White.
** POSIX-style TZ environment variable handling from Guy Harris.
*/

static pthread_mutex_t locallock;

void localtime_wipe(void) {
	pthread_mutex_init(&locallock, 0);
}

void localtime_lock(void) {
	pthread_mutex_lock(&locallock);
}

void localtime_unlock(void) {
	pthread_mutex_unlock(&locallock);
}

__attribute__((__constructor__)) static void localtime_init(void) {
	localtime_wipe();
	pthread_atfork(localtime_lock,
		       localtime_unlock,
		       localtime_wipe);
}

#ifndef TZ_ABBR_MAX_LEN
#define TZ_ABBR_MAX_LEN	16
#endif /* !defined TZ_ABBR_MAX_LEN */

#ifndef TZ_ABBR_CHAR_SET
#define TZ_ABBR_CHAR_SET \
	"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 :+-._"
#endif /* !defined TZ_ABBR_CHAR_SET */

#ifndef TZ_ABBR_ERR_CHAR
#define TZ_ABBR_ERR_CHAR	'_'
#endif /* !defined TZ_ABBR_ERR_CHAR */

#ifndef WILDABBR
/*
** Someone might make incorrect use of a time zone abbreviation:
**	1.	They might reference tzname[0] before calling tzset (explicitly
**		or implicitly).
**	2.	They might reference tzname[1] before calling tzset (explicitly
**		or implicitly).
**	3.	They might reference tzname[1] after setting to a time zone
**		in which Daylight Saving Time is never observed.
**	4.	They might reference tzname[0] after setting to a time zone
**		in which Standard Time is never observed.
**	5.	They might reference tm.TM_ZONE after calling offtime.
** What's best to do in the above cases is open to debate;
** for now, we just set things up so that in any of the five cases
** WILDABBR is used. Another possibility: initialize tzname[0] to the
** string "tzname[0] used before set", and similarly for the other cases.
** And another: initialize tzname[0] to "ERA", with an explanation in the
** manual page of what this "time zone abbreviation" means (doing this so
** that tzname[0] has the "normal" length of three characters).
*/
#define WILDABBR	"   "
#endif /* !defined WILDABBR */

static const char	wildabbr[] = WILDABBR;

static const char	gmt[] = "GMT";

/*
** The DST rules to use if TZ has no rules and we can't load TZDEFRULES.
** Default to US rules as of 2017-05-07.
** POSIX does not specify the default DST rules;
** for historical reasons, US rules are a common default.
*/
#ifndef TZDEFRULESTRING
#define TZDEFRULESTRING ",M3.2.0,M11.1.0"
#endif

struct ttinfo {				/* time type information */
	int32_t		tt_utoff;	/* UT offset in seconds */
	bool		tt_isdst;	/* used to set tm_isdst */
	int		tt_desigidx;	/* abbreviation list index */
	bool		tt_ttisstd;	/* transition is std time */
	bool		tt_ttisut;	/* transition is UT */
};

struct lsinfo {				/* leap second information */
	time_t		ls_trans;	/* transition time */
	int32_t		ls_corr;	/* correction to apply */
};

#define SMALLEST(a, b)	(((a) < (b)) ? (a) : (b))
#define BIGGEST(a, b)	(((a) > (b)) ? (a) : (b))

/* This abbreviation means local time is unspecified.  */
static char const UNSPEC[] = "-00";

/* How many extra bytes are needed at the end of struct state's chars array.
   This needs to be at least 1 for null termination in case the input
   data isn't properly terminated, and it also needs to be big enough
   for ttunspecified to work without crashing.  */
enum { CHARS_EXTRA = BIGGEST(sizeof UNSPEC, 2) - 1 };

#ifdef TZNAME_MAX
#define MY_TZNAME_MAX	TZNAME_MAX
#endif /* defined TZNAME_MAX */
#ifndef TZNAME_MAX
#define MY_TZNAME_MAX	255
#endif /* !defined TZNAME_MAX */

struct state {
	int		leapcnt;
	int		timecnt;
	int		typecnt;
	int		charcnt;
	bool		goback;
	bool		goahead;
	time_t		ats[TZ_MAX_TIMES];
	unsigned char	types[TZ_MAX_TIMES];
	struct ttinfo	ttis[TZ_MAX_TYPES];
	char		chars[BIGGEST(BIGGEST(TZ_MAX_CHARS + CHARS_EXTRA,
					      sizeof gmt),
				(2 * (MY_TZNAME_MAX + 1)))];
	struct lsinfo	lsis[TZ_MAX_LEAPS];

	/* The time type to use for early times or if no transitions.
	   It is always zero for recent tzdb releases.
	   It might be nonzero for data from tzdb 2018e or earlier.  */
	int defaulttype;
};

enum r_type {
  JULIAN_DAY,		/* Jn = Julian day */
  DAY_OF_YEAR,		/* n = day of year */
  MONTH_NTH_DAY_OF_WEEK	/* Mm.n.d = month, week, day of week */
};

struct rule {
	enum r_type	r_type;		/* type of rule */
	int		r_day;		/* day number of rule */
	int		r_week;		/* week number of rule */
	int		r_mon;		/* month number of rule */
	int32_t	r_time;		/* transition time of rule */
};

static struct tm *gmtsub(struct state const *, time_t const *, int32_t,
			 struct tm *);
static bool increment_overflow(int *, int);
static bool increment_overflow_time(time_t *, int32_t);
static int32_t leapcorr(struct state const *, time_t);
static bool normalize_overflow32(int32_t *, int *, int);
static struct tm *localtime_timesub(time_t const *, int32_t,
				    struct state const *, struct tm *);
static bool localtime_typesequiv(struct state const *, int, int);
static bool localtime_tzparse(char const *, struct state *, struct state *);

static struct state *	lclptr;
static struct state *	gmtptr;

#ifndef TZ_STRLEN_MAX
#define TZ_STRLEN_MAX 255
#endif /* !defined TZ_STRLEN_MAX */

static char		lcl_TZname[TZ_STRLEN_MAX + 1];
static int		lcl_is_set;

/*
** Section 4.12.3 of X3.159-1989 requires that
**	Except for the strftime function, these functions [asctime,
**	ctime, gmtime, localtime] return values in one of two static
**	objects: a broken-down time structure and an array of char.
** Thanks to Paul Eggert for noting this.
*/

static struct tm	tm;

#if 2 <= HAVE_TZNAME + TZ_TIME_T
char *			tzname[2] = {
	(char *) wildabbr,
	(char *) wildabbr
};
#endif
#if 2 <= USG_COMPAT + TZ_TIME_T
long			timezone;
int			daylight;
#endif
#if 2 <= ALTZONE + TZ_TIME_T
long			altzone;
#endif

/* Initialize *S to a value based on UTOFF, ISDST, and DESIGIDX.  */
static void
init_ttinfo(struct ttinfo *s, int32_t utoff, bool isdst, int desigidx)
{
	s->tt_utoff = utoff;
	s->tt_isdst = isdst;
	s->tt_desigidx = desigidx;
	s->tt_ttisstd = false;
	s->tt_ttisut = false;
}

/* Return true if SP's time type I does not specify local time.  */
static bool
ttunspecified(struct state const *sp, int i)
{
	char const *abbr = &sp->chars[sp->ttis[i].tt_desigidx];
	/* memcmp is likely faster than strcmp, and is safe due to CHARS_EXTRA.  */
	return memcmp(abbr, UNSPEC, sizeof UNSPEC) == 0;
}

forceinline int32_t detzcode(const char *const codep) {
	return READ32BE(codep);
}

forceinline int64_t detzcode64(const char *const codep) {
	return READ64BE(codep);
}

static void
update_tzname_etc(struct state const *sp, struct ttinfo const *ttisp)
{
#if HAVE_TZNAME
	tzname[ttisp->tt_isdst] = (char *) &sp->chars[ttisp->tt_desigidx];
#endif
	if (!ttisp->tt_isdst)
		timezone = - ttisp->tt_utoff;
#if ALTZONE
	if (ttisp->tt_isdst)
		altzone = - ttisp->tt_utoff;
#endif
}

static void
settzname(void)
{
	register struct state * const	sp = lclptr;
	register int			i;

#if HAVE_TZNAME
	tzname[0] = tzname[1] = (char *) (sp ? wildabbr : gmt);
#endif
	daylight = 0;
	timezone = 0;
#if ALTZONE
	altzone = 0;
#endif
	if (sp == NULL) {
		return;
	}
	/*
	** And to get the latest time zone abbreviations into tzname. . .
	*/
	for (i = 0; i < sp->typecnt; ++i) {
		register const struct ttinfo * const	ttisp = &sp->ttis[i];
		update_tzname_etc(sp, ttisp);
	}
	for (i = 0; i < sp->timecnt; ++i) {
		register const struct ttinfo * const	ttisp =
							&sp->ttis[
								sp->types[i]];
		update_tzname_etc(sp, ttisp);
		if (ttisp->tt_isdst)
			daylight = 1;
	}
}

static void
scrub_abbrs(struct state *sp)
{
	int i;
	/*
	** First, replace bogus characters.
	*/
	for (i = 0; i < sp->charcnt; ++i)
		if (strchr(TZ_ABBR_CHAR_SET, sp->chars[i]) == NULL)
			sp->chars[i] = TZ_ABBR_ERR_CHAR;
	/*
	** Second, truncate long abbreviations.
	*/
	for (i = 0; i < sp->typecnt; ++i) {
		register const struct ttinfo * const	ttisp = &sp->ttis[i];
		char *cp = &sp->chars[ttisp->tt_desigidx];

		if (strlen(cp) > TZ_ABBR_MAX_LEN &&
			strcmp(cp, GRANDPARENTED) != 0)
				*(cp + TZ_ABBR_MAX_LEN) = '\0';
	}
}

/* Input buffer for data read from a compiled tz file.  */
union input_buffer {
  /* The first part of the buffer, interpreted as a header.  */
  struct tzhead tzhead;

  /* The entire buffer.  */
  char buf[2 * sizeof(struct tzhead) + 2 * sizeof(struct state)
	   + 4 * TZ_MAX_TIMES];
};

/* TZDIR with a trailing '/' rather than a trailing '\0'.  */
static char const tzdirslash[sizeof TZDIR] = TZDIR "/";

/* Local storage needed for 'tzloadbody'.  */
union local_storage {
  /* The results of analyzing the file's contents after it is opened.  */
  struct file_analysis {
    /* The input buffer.  */
    union input_buffer u;

    /* A temporary state used for parsing a TZ string in the file.  */
    struct state st;
  } u;

  /* The file name to be opened.  */
  char fullname[BIGGEST(sizeof(struct file_analysis),
			sizeof tzdirslash + 1024)];
};

/* Load tz data from the file named NAME into *SP.  Read extended
   format if DOEXTEND.  Use *LSP for temporary storage.  Return 0 on
   success, an errno value on failure.  */
static int
localtime_tzloadbody_(char const *name, struct state *sp, bool doextend,
		      union local_storage *lsp)
{
	register int			i;
	register int			fid;
	register int			stored;
	register ssize_t		nread;
	register bool doaccess;
	register union input_buffer *up = &lsp->u.u;
	register int tzheadsize = sizeof(struct tzhead);

	sp->goback = sp->goahead = false;

	if (! name) {
		name = TZDEFAULT;
		if (! name)
		  return EINVAL;
	}

	if (name[0] == ':')
		++name;
#ifdef SUPPRESS_TZDIR
	/* Do not prepend TZDIR.  This is intended for specialized
	   applications only, due to its security implications.  */
	doaccess = true;
#else
	doaccess = name[0] == '/';
#endif
	if (!doaccess) {
		char const *dot;
		size_t namelen = strlen(name);
		if (sizeof lsp->fullname - sizeof tzdirslash <= namelen)
		  return ENAMETOOLONG;

		/* Create a string "TZDIR/NAME".  Using sprintf here
		   would pull in stdio (and would fail if the
		   resulting string length exceeded INT_MAX!).  */
		memcpy(lsp->fullname, tzdirslash, sizeof tzdirslash);
		strcpy(lsp->fullname + sizeof tzdirslash, name);

		/* Set doaccess if NAME contains a ".." file name
		   component, as such a name could read a file outside
		   the TZDIR virtual subtree.  */
		for (dot = name; (dot = strchr(dot, '.')); dot++)
		  if ((dot == name || dot[-1] == '/') && dot[1] == '.'
		      && (dot[2] == '/' || !dot[2])) {
		    doaccess = true;
		    break;
		  }

		name = lsp->fullname;
	}
	if (doaccess && access(name, R_OK) != 0)
	  return errno;
	fid = open(name, O_RDONLY);
	if (fid < 0)
	  return errno;

	nread = read(fid, up->buf, sizeof up->buf);
	if (nread < tzheadsize) {
	  int err = nread < 0 ? errno : EINVAL;
	  close(fid);
	  return err;
	}
	if (close(fid) < 0)
	  return errno;
	for (stored = 4; stored <= 8; stored *= 2) {
	    char version = up->tzhead.tzh_version[0];
	    bool skip_datablock = stored == 4 && version;
	    int32_t datablock_size;
	    int32_t ttisstdcnt = detzcode(up->tzhead.tzh_ttisstdcnt);
	    int32_t ttisutcnt = detzcode(up->tzhead.tzh_ttisutcnt);
	    int64_t prevtr = -1;
	    int32_t prevcorr = 0;
	    int32_t leapcnt = detzcode(up->tzhead.tzh_leapcnt);
	    int32_t timecnt = detzcode(up->tzhead.tzh_timecnt);
	    int32_t typecnt = detzcode(up->tzhead.tzh_typecnt);
	    int32_t charcnt = detzcode(up->tzhead.tzh_charcnt);
	    char const *p = up->buf + tzheadsize;
	    /* Although tzfile(5) currently requires typecnt to be nonzero,
	       support future formats that may allow zero typecnt
	       in files that have a TZ string and no transitions.  */
	    if (! (0 <= leapcnt && leapcnt < TZ_MAX_LEAPS
		   && 0 <= typecnt && typecnt < TZ_MAX_TYPES
		   && 0 <= timecnt && timecnt < TZ_MAX_TIMES
		   && 0 <= charcnt && charcnt < TZ_MAX_CHARS
		   && 0 <= ttisstdcnt && ttisstdcnt < TZ_MAX_TYPES
		   && 0 <= ttisutcnt && ttisutcnt < TZ_MAX_TYPES))
	      return EINVAL;
	    datablock_size
		    = (timecnt * stored		/* ats */
		       + timecnt		/* types */
		       + typecnt * 6		/* ttinfos */
		       + charcnt		/* chars */
		       + leapcnt * (stored + 4)	/* lsinfos */
		       + ttisstdcnt		/* ttisstds */
		       + ttisutcnt);		/* ttisuts */
	    if (nread < tzheadsize + datablock_size)
	      return EINVAL;
	    if (skip_datablock)
		p += datablock_size;
	    else {
		if (! ((ttisstdcnt == typecnt || ttisstdcnt == 0)
		       && (ttisutcnt == typecnt || ttisutcnt == 0)))
		  return EINVAL;

		sp->leapcnt = leapcnt;
		sp->timecnt = timecnt;
		sp->typecnt = typecnt;
		sp->charcnt = charcnt;

		/* Read transitions, discarding those out of time_t range.
		   But pretend the last transition before TIME_T_MIN
		   occurred at TIME_T_MIN.  */
		timecnt = 0;
		for (i = 0; i < sp->timecnt; ++i) {
			int64_t at
			  = stored == 4 ? detzcode(p) : detzcode64(p);
			sp->types[i] = at <= TIME_T_MAX;
			if (sp->types[i]) {
			  time_t attime
			    = ((TYPE_SIGNED(time_t) ? at < TIME_T_MIN : at < 0)
			       ? TIME_T_MIN : at);
			  if (timecnt && attime <= sp->ats[timecnt - 1]) {
			    if (attime < sp->ats[timecnt - 1])
			      return EINVAL;
			    sp->types[i - 1] = 0;
			    timecnt--;
			  }
			  sp->ats[timecnt++] = attime;
			}
			p += stored;
		}

		timecnt = 0;
		for (i = 0; i < sp->timecnt; ++i) {
			unsigned char typ = *p++;
			if (sp->typecnt <= typ)
			  return EINVAL;
			if (sp->types[i])
				sp->types[timecnt++] = typ;
		}
		sp->timecnt = timecnt;
		for (i = 0; i < sp->typecnt; ++i) {
			register struct ttinfo *	ttisp;
			unsigned char isdst, desigidx;

			ttisp = &sp->ttis[i];
			ttisp->tt_utoff = detzcode(p);
			p += 4;
			isdst = *p++;
			if (! (isdst < 2))
			  return EINVAL;
			ttisp->tt_isdst = isdst;
			desigidx = *p++;
			if (! (desigidx < sp->charcnt))
			  return EINVAL;
			ttisp->tt_desigidx = desigidx;
		}
		for (i = 0; i < sp->charcnt; ++i)
			sp->chars[i] = *p++;
		/* Ensure '\0'-terminated, and make it safe to call
		   ttunspecified later.  */
		bzero(&sp->chars[i], CHARS_EXTRA);

		/* Read leap seconds, discarding those out of time_t range.  */
		leapcnt = 0;
		for (i = 0; i < sp->leapcnt; ++i) {
		  int64_t tr = stored == 4 ? detzcode(p) : detzcode64(p);
		  int32_t corr = detzcode(p + stored);
		  p += stored + 4;

		  /* Leap seconds cannot occur before the Epoch,
		     or out of order.  */
		  if (tr <= prevtr)
		    return EINVAL;

		  /* To avoid other botches in this code, each leap second's
		     correction must differ from the previous one's by 1
		     second or less, except that the first correction can be
		     any value; these requirements are more generous than
		     RFC 8536, to allow future RFC extensions.  */
		  if (! (i == 0
			 || (prevcorr < corr
			     ? corr == prevcorr + 1
			     : (corr == prevcorr
				|| corr == prevcorr - 1))))
		    return EINVAL;
		  prevtr = tr;
		  prevcorr = corr;

		  if (tr <= TIME_T_MAX) {
		    sp->lsis[leapcnt].ls_trans = tr;
		    sp->lsis[leapcnt].ls_corr = corr;
		    leapcnt++;
		  }
		}
		sp->leapcnt = leapcnt;

		for (i = 0; i < sp->typecnt; ++i) {
			register struct ttinfo *	ttisp;

			ttisp = &sp->ttis[i];
			if (ttisstdcnt == 0)
				ttisp->tt_ttisstd = false;
			else {
				if (*p != true && *p != false)
				  return EINVAL;
				ttisp->tt_ttisstd = *p++;
			}
		}
		for (i = 0; i < sp->typecnt; ++i) {
			register struct ttinfo *	ttisp;

			ttisp = &sp->ttis[i];
			if (ttisutcnt == 0)
				ttisp->tt_ttisut = false;
			else {
				if (*p != true && *p != false)
						return EINVAL;
				ttisp->tt_ttisut = *p++;
			}
		}
	    }

	    nread -= p - up->buf;
	    memmove(up->buf, p, nread);

	    /* If this is an old file, we're done.  */
	    if (!version)
	      break;
	}
	if (doextend && nread > 2 &&
		up->buf[0] == '\n' && up->buf[nread - 1] == '\n' &&
		sp->typecnt + 2 <= TZ_MAX_TYPES) {
			struct state	*ts = &lsp->u.st;

			up->buf[nread - 1] = '\0';
			if (localtime_tzparse(&up->buf[1], ts, sp)) {

			  /* Attempt to reuse existing abbreviations.
			     Without this, America/Anchorage would be right on
			     the edge after 2037 when TZ_MAX_CHARS is 50, as
			     sp->charcnt equals 40 (for LMT AST AWT APT AHST
			     AHDT YST AKDT AKST) and ts->charcnt equals 10
			     (for AKST AKDT).  Reusing means sp->charcnt can
			     stay 40 in this example.  */
			  int gotabbr = 0;
			  int charcnt = sp->charcnt;
			  for (i = 0; i < ts->typecnt; i++) {
			    char *tsabbr = ts->chars + ts->ttis[i].tt_desigidx;
			    int j;
			    for (j = 0; j < charcnt; j++)
			      if (strcmp(sp->chars + j, tsabbr) == 0) {
				ts->ttis[i].tt_desigidx = j;
				gotabbr++;
				break;
			      }
			    if (! (j < charcnt)) {
			      int tsabbrlen = strlen(tsabbr);
			      if (j + tsabbrlen < TZ_MAX_CHARS) {
				strcpy(sp->chars + j, tsabbr);
				charcnt = j + tsabbrlen + 1;
				ts->ttis[i].tt_desigidx = j;
				gotabbr++;
			      }
			    }
			  }
			  if (gotabbr == ts->typecnt) {
			    sp->charcnt = charcnt;

			    /* Ignore any trailing, no-op transitions generated
			       by zic as they don't help here and can run afoul
			       of bugs in zic 2016j or earlier.  */
			    while (1 < sp->timecnt
				   && (sp->types[sp->timecnt - 1]
				       == sp->types[sp->timecnt - 2]))
			      sp->timecnt--;

			    for (i = 0;
				 i < ts->timecnt && sp->timecnt < TZ_MAX_TIMES;
				 i++) {
			      time_t t = ts->ats[i];
			      if (increment_overflow_time(&t, leapcorr(sp, t))
				  || (0 < sp->timecnt
				      && t <= sp->ats[sp->timecnt - 1]))
				continue;
			      sp->ats[sp->timecnt] = t;
			      sp->types[sp->timecnt] = (sp->typecnt
							+ ts->types[i]);
			      sp->timecnt++;
			    }
			    for (i = 0; i < ts->typecnt; i++)
			      sp->ttis[sp->typecnt++] = ts->ttis[i];
			  }
			}
	}
	if (sp->typecnt == 0)
	  return EINVAL;
	if (sp->timecnt > 1) {
	    if (sp->ats[0] <= TIME_T_MAX - SECSPERREPEAT) {
		time_t repeatat = sp->ats[0] + SECSPERREPEAT;
		int repeattype = sp->types[0];
		for (i = 1; i < sp->timecnt; ++i)
		  if (sp->ats[i] == repeatat
		      && localtime_typesequiv(sp, sp->types[i], repeattype)) {
			  sp->goback = true;
			  break;
		  }
	    }
	    if (TIME_T_MIN + SECSPERREPEAT <= sp->ats[sp->timecnt - 1]) {
		time_t repeatat = sp->ats[sp->timecnt - 1] - SECSPERREPEAT;
		int repeattype = sp->types[sp->timecnt - 1];
		for (i = sp->timecnt - 2; i >= 0; --i)
		  if (sp->ats[i] == repeatat
		      && localtime_typesequiv(sp, sp->types[i], repeattype)) {
			  sp->goahead = true;
			  break;
		  }
	    }
	}

	/* Infer sp->defaulttype from the data.  Although this default
	   type is always zero for data from recent tzdb releases,
	   things are trickier for data from tzdb 2018e or earlier.

	   The first set of heuristics work around bugs in 32-bit data
	   generated by tzdb 2013c or earlier.  The workaround is for
	   zones like Australia/Macquarie where timestamps before the
	   first transition have a time type that is not the earliest
	   standard-time type.  See:
	   https://mm.icann.org/pipermail/tz/2013-May/019368.html */
	/*
	** If type 0 does not specify local time, or is unused in transitions,
	** it's the type to use for early times.
	*/
	for (i = 0; i < sp->timecnt; ++i)
		if (sp->types[i] == 0)
			break;
	i = i < sp->timecnt && ! ttunspecified(sp, 0) ? -1 : 0;
	/*
	** Absent the above,
	** if there are transition times
	** and the first transition is to a daylight time
	** find the standard type less than and closest to
	** the type of the first transition.
	*/
	if (i < 0 && sp->timecnt > 0 && sp->ttis[sp->types[0]].tt_isdst) {
		i = sp->types[0];
		while (--i >= 0)
			if (!sp->ttis[i].tt_isdst)
				break;
	}
	/* The next heuristics are for data generated by tzdb 2018e or
	   earlier, for zones like EST5EDT where the first transition
	   is to DST.  */
	/*
	** If no result yet, find the first standard type.
	** If there is none, punt to type zero.
	*/
	if (i < 0) {
		i = 0;
		while (sp->ttis[i].tt_isdst)
			if (++i >= sp->typecnt) {
				i = 0;
				break;
			}
	}
	/* A simple 'sp->defaulttype = 0;' would suffice here if we
	   didn't have to worry about 2018e-or-earlier data.  Even
	   simpler would be to remove the defaulttype member and just
	   use 0 in its place.  */
	sp->defaulttype = i;

	return 0;
}

static int /* [jart] pthread cancelation safe */
localtime_tzloadbody(char const *name, struct state *sp, bool doextend,
		     union local_storage *lsp)
{
	int rc;
	BLOCK_CANCELATION;
	rc = localtime_tzloadbody_(name, sp, doextend, lsp);
	ALLOW_CANCELATION;
	return rc;
}

/* Load tz data from the file named NAME into *SP.  Read extended
   format if DOEXTEND.  Return 0 on success, an errno value on failure.  */
static int
localtime_tzload(char const *name, struct state *sp, bool doextend)
{
	union local_storage *lsp = malloc(sizeof *lsp);
	if (!lsp) {
		return HAVE_MALLOC_ERRNO ? errno : ENOMEM;
	} else {
		int err = localtime_tzloadbody(name, sp, doextend, lsp);
		free(lsp);
		return err;
	}
}

static bool
localtime_typesequiv(const struct state *sp, int a, int b)
{
	register bool result;

	if (sp == NULL ||
		a < 0 || a >= sp->typecnt ||
		b < 0 || b >= sp->typecnt)
			result = false;
	else {
		register const struct ttinfo *	ap = &sp->ttis[a];
		register const struct ttinfo *	bp = &sp->ttis[b];
		result = (ap->tt_utoff == bp->tt_utoff
			  && ap->tt_isdst == bp->tt_isdst
			  && ap->tt_ttisstd == bp->tt_ttisstd
			  && ap->tt_ttisut == bp->tt_ttisut
			  && (strcmp(&sp->chars[ap->tt_desigidx],
				     &sp->chars[bp->tt_desigidx])
			      == 0));
	}
	return result;
}

static const int	mon_lengths[2][MONSPERYEAR] = {
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

static const int	year_lengths[2] = {
	DAYSPERNYEAR, DAYSPERLYEAR
};

/* Is C an ASCII digit?  */
static inline bool
is_digit(char c)
{
  return '0' <= c && c <= '9';
}

/*
** Given a pointer into a timezone string, scan until a character that is not
** a valid character in a time zone abbreviation is found.
** Return a pointer to that character.
*/

static nosideeffect const char *
getzname(register const char *strp)
{
	register char	c;

	while ((c = *strp) != '\0' && !is_digit(c) && c != ',' && c != '-' &&
		c != '+')
			++strp;
	return strp;
}

/*
** Given a pointer into an extended timezone string, scan until the ending
** delimiter of the time zone abbreviation is located.
** Return a pointer to the delimiter.
**
** As with getzname above, the legal character set is actually quite
** restricted, with other characters producing undefined results.
** We don't do any checking here; checking is done later in common-case code.
*/

static nosideeffect const char *
getqzname(register const char *strp, const int delim)
{
	register int	c;

	while ((c = *strp) != '\0' && c != delim)
		++strp;
	return strp;
}

/*
** Given a pointer into a timezone string, extract a number from that string.
** Check that the number is within a specified range; if it is not, return
** NULL.
** Otherwise, return a pointer to the first character not part of the number.
*/

static const char *
getnum(register const char *strp, int *const nump, const int min, const int max)
{
	register char	c;
	register int	num;

	if (strp == NULL || !is_digit(c = *strp))
		return NULL;
	num = 0;
	do {
		num = num * 10 + (c - '0');
		if (num > max)
			return NULL;	/* illegal value */
		c = *++strp;
	} while (is_digit(c));
	if (num < min)
		return NULL;		/* illegal value */
	*nump = num;
	return strp;
}

/*
** Given a pointer into a timezone string, extract a number of seconds,
** in hh[:mm[:ss]] form, from the string.
** If any error occurs, return NULL.
** Otherwise, return a pointer to the first character not part of the number
** of seconds.
*/

static const char *
getsecs(register const char *strp, int32_t *const secsp)
{
	int	num;
	int32_t secsperhour = SECSPERHOUR;

	/*
	** 'HOURSPERDAY * DAYSPERWEEK - 1' allows quasi-Posix rules like
	** "M10.4.6/26", which does not conform to Posix,
	** but which specifies the equivalent of
	** "02:00 on the first Sunday on or after 23 Oct".
	*/
	strp = getnum(strp, &num, 0, HOURSPERDAY * DAYSPERWEEK - 1);
	if (strp == NULL)
		return NULL;
	*secsp = num * secsperhour;
	if (*strp == ':') {
		++strp;
		strp = getnum(strp, &num, 0, MINSPERHOUR - 1);
		if (strp == NULL)
			return NULL;
		*secsp += num * SECSPERMIN;
		if (*strp == ':') {
			++strp;
			/* 'SECSPERMIN' allows for leap seconds.  */
			strp = getnum(strp, &num, 0, SECSPERMIN);
			if (strp == NULL)
				return NULL;
			*secsp += num;
		}
	}
	return strp;
}

/*
** Given a pointer into a timezone string, extract an offset, in
** [+-]hh[:mm[:ss]] form, from the string.
** If any error occurs, return NULL.
** Otherwise, return a pointer to the first character not part of the time.
*/

static const char *
getoffset(register const char *strp, int32_t *const offsetp)
{
	register bool neg = false;

	if (*strp == '-') {
		neg = true;
		++strp;
	} else if (*strp == '+')
		++strp;
	strp = getsecs(strp, offsetp);
	if (strp == NULL)
		return NULL;		/* illegal time */
	if (neg)
		*offsetp = -*offsetp;
	return strp;
}

/*
** Given a pointer into a timezone string, extract a rule in the form
** date[/time]. See POSIX section 8 for the format of "date" and "time".
** If a valid rule is not found, return NULL.
** Otherwise, return a pointer to the first character not part of the rule.
*/

static const char *
getrule(const char *strp, register struct rule *const rulep)
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
	} else if (is_digit(*strp)) {
		/*
		** Day of year.
		*/
		rulep->r_type = DAY_OF_YEAR;
		strp = getnum(strp, &rulep->r_day, 0, DAYSPERLYEAR - 1);
	} else	return NULL;		/* invalid format */
	if (strp == NULL)
		return NULL;
	if (*strp == '/') {
		/*
		** Time specified.
		*/
		++strp;
		strp = getoffset(strp, &rulep->r_time);
	} else	rulep->r_time = 2 * SECSPERHOUR;	/* default = 2:00:00 */
	return strp;
}

/*
** Given a year, a rule, and the offset from UT at the time that rule takes
** effect, calculate the year-relative time that rule takes effect.
*/

static int32_t
transtime(const int year, register const struct rule *const rulep,
	  const int32_t offset)
{
	register bool	leapyear;
	register int32_t value;
	register int	i;
	int		d, m1, yy0, yy1, yy2, dow;

	leapyear = isleap(year);
	switch (rulep->r_type) {

	case JULIAN_DAY:
		/*
		** Jn - Julian day, 1 == January 1, 60 == March 1 even in leap
		** years.
		** In non-leap years, or if the day number is 59 or less, just
		** add SECSPERDAY times the day number-1 to the time of
		** January 1, midnight, to get the day.
		*/
		value = (rulep->r_day - 1) * SECSPERDAY;
		if (leapyear && rulep->r_day >= 60)
			value += SECSPERDAY;
		break;

	case DAY_OF_YEAR:
		/*
		** n - day of year.
		** Just add SECSPERDAY times the day number to the time of
		** January 1, midnight, to get the day.
		*/
		value = rulep->r_day * SECSPERDAY;
		break;

	case MONTH_NTH_DAY_OF_WEEK:
		/*
		** Mm.n.d - nth "dth day" of month m.
		*/

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
		** "dow" is the day-of-week of the first day of the month. Get
		** the day-of-month (zero-origin) of the first "dow" day of the
		** month.
		*/
		d = rulep->r_day - dow;
		if (d < 0)
			d += DAYSPERWEEK;
		for (i = 1; i < rulep->r_week; ++i) {
			if (d + DAYSPERWEEK >=
				mon_lengths[leapyear][rulep->r_mon - 1])
					break;
			d += DAYSPERWEEK;
		}

		/*
		** "d" is the day-of-month (zero-origin) of the day we want.
		*/
		value = d * SECSPERDAY;
		for (i = 0; i < rulep->r_mon - 1; ++i)
			value += mon_lengths[leapyear][i] * SECSPERDAY;
		break;

	default: UNREACHABLE();
	}

	/*
	** "value" is the year-relative time of 00:00:00 UT on the day in
	** question. To get the year-relative time of the specified local
	** time on that day, add the transition time and the current offset
	** from UT.
	*/
	return value + rulep->r_time + offset;
}

/*
** Given a POSIX section 8-style TZ string, fill in the rule tables as
** appropriate.
*/

static bool
localtime_tzparse(const char *name, struct state *sp, struct state *basep)
{
	const char *			stdname;
	const char *			dstname;
	size_t				stdlen;
	size_t				dstlen;
	size_t				charcnt;
	int32_t				stdoffset;
	int32_t				dstoffset;
	register char *			cp;
	register bool			load_ok;
	time_t atlo = TIME_T_MIN, leaplo = TIME_T_MIN;

	stdname = name;
	if (*name == '<') {
	  name++;
	  stdname = name;
	  name = getqzname(name, '>');
	  if (*name != '>')
	    return false;
	  stdlen = name - stdname;
	  name++;
	} else {
	  name = getzname(name);
	  stdlen = name - stdname;
	}
	if (!stdlen)
	  return false;
	name = getoffset(name, &stdoffset);
	if (name == NULL)
	  return false;
	charcnt = stdlen + 1;
	if (sizeof sp->chars < charcnt)
	  return false;
	if (basep) {
	  if (0 < basep->timecnt)
	    atlo = basep->ats[basep->timecnt - 1];
	  load_ok = false;
	  sp->leapcnt = basep->leapcnt;
	  memcpy(sp->lsis, basep->lsis, sp->leapcnt * sizeof *sp->lsis);
	} else {
	  load_ok = localtime_tzload(TZDEFRULES, sp, false) == 0;
	  if (!load_ok)
	    sp->leapcnt = 0;	/* So, we're off a little.  */
	}
	if (0 < sp->leapcnt)
	  leaplo = sp->lsis[sp->leapcnt - 1].ls_trans;
	if (*name != '\0') {
		if (*name == '<') {
			dstname = ++name;
			name = getqzname(name, '>');
			if (*name != '>')
			  return false;
			dstlen = name - dstname;
			name++;
		} else {
			dstname = name;
			name = getzname(name);
			dstlen = name - dstname; /* length of DST abbr. */
		}
		if (!dstlen)
		  return false;
		charcnt += dstlen + 1;
		if (sizeof sp->chars < charcnt)
		  return false;
		if (*name != '\0' && *name != ',' && *name != ';') {
			name = getoffset(name, &dstoffset);
			if (name == NULL)
			  return false;
		} else	dstoffset = stdoffset - SECSPERHOUR;
		if (*name == '\0' && !load_ok)
			name = TZDEFRULESTRING;
		if (*name == ',' || *name == ';') {
			struct rule	start;
			struct rule	end;
			register int	year;
			register int	timecnt;
			time_t		janfirst;
			int32_t janoffset = 0;
			int yearbeg, yearlim;

			++name;
			if ((name = getrule(name, &start)) == NULL)
			  return false;
			if (*name++ != ',')
			  return false;
			if ((name = getrule(name, &end)) == NULL)
			  return false;
			if (*name != '\0')
			  return false;
			sp->typecnt = 2;	/* standard time and DST */
			/*
			** Two transitions per year, from EPOCH_YEAR forward.
			*/
			init_ttinfo(&sp->ttis[0], -stdoffset, false, 0);
			init_ttinfo(&sp->ttis[1], -dstoffset, true, stdlen + 1);
			sp->defaulttype = 0;
			timecnt = 0;
			janfirst = 0;
			yearbeg = EPOCH_YEAR;

			do {
			  int32_t yearsecs
			    = year_lengths[isleap(yearbeg - 1)] * SECSPERDAY;
			  yearbeg--;
			  if (increment_overflow_time(&janfirst, -yearsecs)) {
			    janoffset = -yearsecs;
			    break;
			  }
			} while (atlo < janfirst
				 && EPOCH_YEAR - YEARSPERREPEAT / 2 < yearbeg);

			while (true) {
			  int32_t yearsecs
			    = year_lengths[isleap(yearbeg)] * SECSPERDAY;
			  int yearbeg1 = yearbeg;
			  time_t janfirst1 = janfirst;
			  if (increment_overflow_time(&janfirst1, yearsecs)
			      || increment_overflow(&yearbeg1, 1)
			      || atlo <= janfirst1)
			    break;
			  yearbeg = yearbeg1;
			  janfirst = janfirst1;
			}

			yearlim = yearbeg;
			if (increment_overflow(&yearlim, YEARSPERREPEAT + 1))
			  yearlim = INT_MAX;
			for (year = yearbeg; year < yearlim; year++) {
				int32_t
				  starttime = transtime(year, &start, stdoffset),
				  endtime = transtime(year, &end, dstoffset);
				int32_t
				  yearsecs = (year_lengths[isleap(year)]
					      * SECSPERDAY);
				bool reversed = endtime < starttime;
				if (reversed) {
					int32_t swap = starttime;
					starttime = endtime;
					endtime = swap;
				}
				if (reversed
				    || (starttime < endtime
					&& endtime - starttime < yearsecs)) {
					if (TZ_MAX_TIMES - 2 < timecnt)
						break;
					sp->ats[timecnt] = janfirst;
					if (! increment_overflow_time
					    (&sp->ats[timecnt],
					     janoffset + starttime)
					    && atlo <= sp->ats[timecnt])
					  sp->types[timecnt++] = !reversed;
					sp->ats[timecnt] = janfirst;
					if (! increment_overflow_time
					    (&sp->ats[timecnt],
					     janoffset + endtime)
					    && atlo <= sp->ats[timecnt]) {
					  sp->types[timecnt++] = reversed;
					}
				}
				if (endtime < leaplo) {
				  yearlim = year;
				  if (increment_overflow(&yearlim,
							 YEARSPERREPEAT + 1))
				    yearlim = INT_MAX;
				}
				if (increment_overflow_time
				    (&janfirst, janoffset + yearsecs))
					break;
				janoffset = 0;
			}
			sp->timecnt = timecnt;
			if (! timecnt) {
				sp->ttis[0] = sp->ttis[1];
				sp->typecnt = 1;	/* Perpetual DST.  */
			} else if (YEARSPERREPEAT < year - yearbeg)
				sp->goback = sp->goahead = true;
		} else {
			register int32_t	theirstdoffset;
			register int32_t	theirdstoffset;
			register int32_t	theiroffset;
			register bool		isdst;
			register int		i;
			register int		j;

			if (*name != '\0')
			  return false;
			/*
			** Initial values of theirstdoffset and theirdstoffset.
			*/
			theirstdoffset = 0;
			for (i = 0; i < sp->timecnt; ++i) {
				j = sp->types[i];
				if (!sp->ttis[j].tt_isdst) {
					theirstdoffset =
						- sp->ttis[j].tt_utoff;
					break;
				}
			}
			theirdstoffset = 0;
			for (i = 0; i < sp->timecnt; ++i) {
				j = sp->types[i];
				if (sp->ttis[j].tt_isdst) {
					theirdstoffset =
						- sp->ttis[j].tt_utoff;
					break;
				}
			}
			/*
			** Initially we're assumed to be in standard time.
			*/
			isdst = false;
			/*
			** Now juggle transition times and types
			** tracking offsets as you do.
			*/
			for (i = 0; i < sp->timecnt; ++i) {
				j = sp->types[i];
				sp->types[i] = sp->ttis[j].tt_isdst;
				if (sp->ttis[j].tt_ttisut) {
					/* No adjustment to transition time */
				} else {
					/*
					** If daylight saving time is in
					** effect, and the transition time was
					** not specified as standard time, add
					** the daylight saving time offset to
					** the transition time; otherwise, add
					** the standard time offset to the
					** transition time.
					*/
					/*
					** Transitions from DST to DDST
					** will effectively disappear since
					** POSIX provides for only one DST
					** offset.
					*/
					if (isdst && !sp->ttis[j].tt_ttisstd) {
						sp->ats[i] += dstoffset -
							theirdstoffset;
					} else {
						sp->ats[i] += stdoffset -
							theirstdoffset;
					}
				}
				theiroffset = -sp->ttis[j].tt_utoff;
				if (sp->ttis[j].tt_isdst)
					theirdstoffset = theiroffset;
				else	theirstdoffset = theiroffset;
			}
			/*
			** Finally, fill in ttis.
			*/
			init_ttinfo(&sp->ttis[0], -stdoffset, false, 0);
			init_ttinfo(&sp->ttis[1], -dstoffset, true, stdlen + 1);
			sp->typecnt = 2;
			sp->defaulttype = 0;
		}
	} else {
		dstlen = 0;
		sp->typecnt = 1;		/* only standard time */
		sp->timecnt = 0;
		init_ttinfo(&sp->ttis[0], -stdoffset, false, 0);
		sp->defaulttype = 0;
	}
	sp->charcnt = charcnt;
	cp = sp->chars;
	memcpy(cp, stdname, stdlen);
	cp += stdlen;
	*cp++ = '\0';
	if (dstlen != 0) {
		memcpy(cp, dstname, dstlen);
		*(cp + dstlen) = '\0';
	}
	return true;
}

static void
gmtload(struct state *const sp)
{
	if (localtime_tzload(gmt, sp, true) != 0)
		localtime_tzparse("GMT0", sp, NULL);
}

/* Initialize *SP to a value appropriate for the TZ setting NAME.
   Return 0 on success, an errno value on failure.  */
static int
zoneinit(struct state *sp, char const *name)
{
	if (name && ! name[0]) {
		/*
		** User wants it fast rather than right.
		*/
		sp->leapcnt = 0;		/* so, we're off a little */
		sp->timecnt = 0;
		sp->typecnt = 0;
		sp->charcnt = 0;
		sp->goback = sp->goahead = false;
		init_ttinfo(&sp->ttis[0], 0, false, 0);
		strcpy(sp->chars, gmt);
		sp->defaulttype = 0;
		return 0;
	} else {
		int err = localtime_tzload(name, sp, true);
		if (err != 0 && name && name[0] != ':' &&
		    localtime_tzparse(name, sp, NULL))
			err = 0;
		if (err == 0)
			scrub_abbrs(sp);
		return err;
	}
}

static void
FreeLocaltime(void *p) {
	free(p);
}

static void
localtime_tzset_unlocked(void)
{
	char const *name = getenv("TZ");
	struct state *sp = lclptr;
	int lcl = name ? strlen(name) < sizeof lcl_TZname : -1;
	if (lcl < 0
	    ? lcl_is_set < 0
	    : 0 < lcl_is_set && strcmp(lcl_TZname, name) == 0)
		return;
	if (!sp) {
		lclptr = sp = malloc(sizeof *lclptr);
		__cxa_atexit(FreeLocaltime, sp, 0);
	}
	if (sp) {
		if (zoneinit(sp, name) != 0)
			zoneinit(sp, "");
		if (0 < lcl)
			strcpy(lcl_TZname, name);
	}
	settzname();
	lcl_is_set = lcl;
}

void
tzset(void)
{
	localtime_lock();
	localtime_tzset_unlocked();
	localtime_unlock();
}

static void
gmtcheck(void)
{
	static bool gmt_is_set;
	localtime_lock();
	if (! gmt_is_set) {
		gmtptr = malloc(sizeof *gmtptr);
		if (gmtptr)
			gmtload(gmtptr);
		gmt_is_set = true;
	}
	localtime_unlock();
}

static void
FreeGmt(void *p) {
	free(p);
}

static void
localtime_gmtcheck(void)
{
	static bool gmt_is_set;
	localtime_lock();
	if (! gmt_is_set) {
		gmtptr = malloc(sizeof *gmtptr);
		__cxa_atexit(FreeGmt, gmtptr, 0);
		if (gmtptr)
			gmtload(gmtptr);
		gmt_is_set = true;
	}
	localtime_unlock();
}

/*
** The easy way to behave "as if no library function calls" localtime
** is to not call it, so we drop its guts into "localsub", which can be
** freely called. (And no, the PANS doesn't require the above behavior,
** but it *is* desirable.)
**
** If successful and SETNAME is nonzero,
** set the applicable parts of tzname, timezone and altzone;
** however, it's OK to omit this step if the timezone is POSIX-compatible,
** since in that case tzset should have already done this step correctly.
** SETNAME's type is int32_t for compatibility with gmtsub,
** but it is actually a boolean and its value should be 0 or 1.
*/

/*ARGSUSED*/
static struct tm *
localsub(struct state const *sp, time_t const *timep, int32_t setname,
	 struct tm *const tmp)
{
	register const struct ttinfo *	ttisp;
	register int			i;
	register struct tm *		result;
	const time_t			t = *timep;

	if (sp == NULL) {
	  /* Don't bother to set tzname etc.; tzset has already done it.  */
	  return gmtsub(gmtptr, timep, 0, tmp);
	}
	if ((sp->goback && t < sp->ats[0]) ||
		(sp->goahead && t > sp->ats[sp->timecnt - 1])) {
			time_t newt;
			register time_t		seconds;
			register time_t		years;

			if (t < sp->ats[0])
				seconds = sp->ats[0] - t;
			else	seconds = t - sp->ats[sp->timecnt - 1];
			--seconds;

			/* Beware integer overflow, as SECONDS might
			   be close to the maximum time_t.  */
			years = seconds / SECSPERREPEAT * YEARSPERREPEAT;
			seconds = years * AVGSECSPERYEAR;
			years += YEARSPERREPEAT;
			if (t < sp->ats[0])
			  newt = t + seconds + SECSPERREPEAT;
			else
			  newt = t - seconds - SECSPERREPEAT;

			if (newt < sp->ats[0] ||
				newt > sp->ats[sp->timecnt - 1])
					return NULL;	/* "cannot happen" */
			result = localsub(sp, &newt, setname, tmp);
			if (result) {
				register int64_t newy;

				newy = result->tm_year;
				if (t < sp->ats[0])
					newy -= years;
				else	newy += years;
				if (! (INT_MIN <= newy && newy <= INT_MAX))
					return NULL;
				result->tm_year = newy;
			}
			return result;
	}
	if (sp->timecnt == 0 || t < sp->ats[0]) {
		i = sp->defaulttype;
	} else {
		register int	lo = 1;
		register int	hi = sp->timecnt;

		while (lo < hi) {
			register int	mid = (lo + hi) >> 1;

			if (t < sp->ats[mid])
				hi = mid;
			else	lo = mid + 1;
		}
		i = sp->types[lo - 1];
	}
	ttisp = &sp->ttis[i];
	/*
	** To get (wrong) behavior that's compatible with System V Release 2.0
	** you'd replace the statement below with
	**	t += ttisp->tt_utoff;
	**	timesub(&t, 0L, sp, tmp);
	*/
	result = localtime_timesub(&t, ttisp->tt_utoff, sp, tmp);
	if (result) {
	  result->tm_isdst = ttisp->tt_isdst;
	  result->tm_zone = (char *) &sp->chars[ttisp->tt_desigidx];
	  if (setname)
	    update_tzname_etc(sp, ttisp);
	}
	return result;
}

static struct tm *
localtime_tzset(time_t const *timep, struct tm *tmp, bool setname)
{
	localtime_lock();
	if (setname || !lcl_is_set)
		localtime_tzset_unlocked();
	tmp = localsub(lclptr, timep, setname, tmp);
	localtime_unlock();
	return tmp;
}

struct tm *
localtime(const time_t *timep)
{
	return localtime_tzset(timep, &tm, true);
}

struct tm *
localtime_r(const time_t *timep, struct tm *tmp)
{
	return localtime_tzset(timep, tmp, false);
}

/*
** gmtsub is to gmtime as localsub is to localtime.
*/

static struct tm *
gmtsub(struct state const *sp, time_t const *timep, int32_t offset,
       struct tm *tmp)
{
	register struct tm *	result;

	result = localtime_timesub(timep, offset, gmtptr, tmp);
	/*
	** Could get fancy here and deliver something such as
	** "+xx" or "-xx" if offset is non-zero,
	** but this is no time for a treasure hunt.
	*/
	tmp->tm_zone = ((char *)
			(offset ? wildabbr : gmtptr ? gmtptr->chars : gmt));
	return result;
}

/*
* Re-entrant version of gmtime.
*/

/**
 * Converts UNIX timestamp to broken-down representation.
 */
struct tm *
gmtime_r(const time_t *timep, struct tm *tmp)
{
	localtime_gmtcheck();
	return gmtsub(gmtptr, timep, 0, tmp);
}

/**
 * Converts UNIX timestamp to broken-down representation.
 * @threadunsafe (see gmtime_r)
 */
struct tm *
gmtime(const time_t *timep)
{
	return gmtime_r(timep, &tm);
}

/*
** Return the number of leap years through the end of the given year
** where, to make the math easy, the answer for year zero is defined as zero.
*/

static time_t
leaps_thru_end_of_nonneg(time_t y)
{
	return y / 4 - y / 100 + y / 400;
}

static time_t
leaps_thru_end_of(time_t y)
{
	return (y < 0
		? -1 - leaps_thru_end_of_nonneg(-1 - y)
		: leaps_thru_end_of_nonneg(y));
}

static struct tm *
localtime_timesub(const time_t *timep, int32_t offset,
		  const struct state *sp, struct tm *tmp)
{
	register const struct lsinfo *	lp;
	register time_t			tdays;
	register const int *		ip;
	register int32_t		corr;
	register int			i;
	int32_t idays, rem, dayoff, dayrem;
	time_t y;

	/* If less than SECSPERMIN, the number of seconds since the
	   most recent positive leap second; otherwise, do not add 1
	   to localtime tm_sec because of leap seconds.  */
	time_t secs_since_posleap = SECSPERMIN;

	corr = 0;
	i = (sp == NULL) ? 0 : sp->leapcnt;
	while (--i >= 0) {
		lp = &sp->lsis[i];
		if (*timep >= lp->ls_trans) {
			corr = lp->ls_corr;
			if ((i == 0 ? 0 : lp[-1].ls_corr) < corr)
			  secs_since_posleap = *timep - lp->ls_trans;
			break;
		}
	}

	/* Calculate the year, avoiding integer overflow even if
	   time_t is unsigned.  */
	tdays = *timep / SECSPERDAY;
	rem = *timep % SECSPERDAY;
	rem += offset % SECSPERDAY - corr % SECSPERDAY + 3 * SECSPERDAY;
	dayoff = offset / SECSPERDAY - corr / SECSPERDAY + rem / SECSPERDAY - 3;
	rem %= SECSPERDAY;
	/* y = (EPOCH_YEAR
	        + floor((tdays + dayoff) / DAYSPERREPEAT) * YEARSPERREPEAT),
	   sans overflow.  But calculate against 1570 (EPOCH_YEAR -
	   YEARSPERREPEAT) instead of against 1970 so that things work
	   for localtime values before 1970 when time_t is unsigned.  */
	dayrem = tdays % DAYSPERREPEAT;
	dayrem += dayoff % DAYSPERREPEAT;
	y = (EPOCH_YEAR - YEARSPERREPEAT
	     + ((1 + dayoff / DAYSPERREPEAT + dayrem / DAYSPERREPEAT
		 - ((dayrem % DAYSPERREPEAT) < 0)
		 + tdays / DAYSPERREPEAT)
		* YEARSPERREPEAT));
	/* idays = (tdays + dayoff) mod DAYSPERREPEAT, sans overflow.  */
	idays = tdays % DAYSPERREPEAT;
	idays += dayoff % DAYSPERREPEAT + 2 * DAYSPERREPEAT;
	idays %= DAYSPERREPEAT;
	/* Increase Y and decrease IDAYS until IDAYS is in range for Y.  */
	while (year_lengths[isleap(y)] <= idays) {
		int tdelta = idays / DAYSPERLYEAR;
		int32_t ydelta = tdelta + !tdelta;
		time_t newy = y + ydelta;
		register int	leapdays;
		leapdays = leaps_thru_end_of(newy - 1) -
			leaps_thru_end_of(y - 1);
		idays -= ydelta * DAYSPERNYEAR;
		idays -= leapdays;
		y = newy;
	}

	if (!TYPE_SIGNED(time_t) && y < TM_YEAR_BASE) {
	  int signed_y = y;
	  tmp->tm_year = signed_y - TM_YEAR_BASE;
	} else if ((!TYPE_SIGNED(time_t) || INT_MIN + TM_YEAR_BASE <= y)
		   && y - TM_YEAR_BASE <= INT_MAX)
	  tmp->tm_year = y - TM_YEAR_BASE;
	else {
	  errno = EOVERFLOW;
	  return NULL;
	}
	tmp->tm_yday = idays;
	/*
	** The "extra" mods below avoid overflow problems.
	*/
	tmp->tm_wday = (TM_WDAY_BASE
			+ ((tmp->tm_year % DAYSPERWEEK)
			   * (DAYSPERNYEAR % DAYSPERWEEK))
			+ leaps_thru_end_of(y - 1)
			- leaps_thru_end_of(TM_YEAR_BASE - 1)
			+ idays);
	tmp->tm_wday %= DAYSPERWEEK;
	if (tmp->tm_wday < 0)
		tmp->tm_wday += DAYSPERWEEK;
	tmp->tm_hour = rem / SECSPERHOUR;
	rem %= SECSPERHOUR;
	tmp->tm_min = rem / SECSPERMIN;
	tmp->tm_sec = rem % SECSPERMIN;

	/* Use "... ??:??:60" at the end of the localtime minute containing
	   the second just before the positive leap second.  */
	tmp->tm_sec += secs_since_posleap <= tmp->tm_sec;

	ip = mon_lengths[isleap(y)];
	for (tmp->tm_mon = 0; idays >= ip[tmp->tm_mon]; ++(tmp->tm_mon))
		idays -= ip[tmp->tm_mon];
	tmp->tm_mday = idays + 1;
	tmp->tm_isdst = 0;
	tmp->tm_gmtoff = offset;
	return tmp;
}

/*
** Adapted from code provided by Robert Elz, who writes:
**	The "best" way to do mktime I think is based on an idea of Bob
**	Kridle's (so its said...) from a long time ago.
**	It does a binary search of the time_t space. Since time_t's are
**	just 32 bits, its a max of 32 iterations (even at 64 bits it
**	would still be very reasonable).
*/

#ifndef WRONG
#define WRONG	(-1)
#endif /* !defined WRONG */

/*
** Normalize logic courtesy Paul Eggert.
*/

forceinline bool
increment_overflow(int *ip, int j)
{
#if defined(__GNUC__) && __GNUC__ >= 6
	int i = *ip;
	if (__builtin_add_overflow(i, j, &i)) return true;
	*ip = i;
	return false;
#else
	register int const	i = *ip;
	/*
	** If i >= 0 there can only be overflow if i + j > INT_MAX
	** or if j > INT_MAX - i; given i >= 0, INT_MAX - i cannot overflow.
	** If i < 0 there can only be overflow if i + j < INT_MIN
	** or if j < INT_MIN - i; given i < 0, INT_MIN - i cannot overflow.
	*/
	if ((i >= 0) ? (j > INT_MAX - i) : (j < INT_MIN - i))
		return true;
	*ip += j;
	return false;
#endif
}

forceinline bool
increment_overflow32(int32_t *const lp, int const m)
{
#if defined(__GNUC__) && __GNUC__ >= 6
	int32_t i = *lp;
	if (__builtin_add_overflow(i, m, &i)) return true;
	*lp = i;
	return false;
#else
	register int32_t const	l = *lp;
	if ((l >= 0) ? (m > INT32_MAX - l) : (m < INT32_MIN - l))
		return true;
	*lp += m;
	return false;
#endif
}

forceinline bool
increment_overflow_time(time_t *tp, int32_t j)
{
#if defined(__GNUC__) && __GNUC__ >= 6
	time_t i = *tp;
	if (__builtin_add_overflow(i, j, &i)) return true;
	*tp = i;
	return false;
#else
	/*
	** This is like
	** 'if (! (TIME_T_MIN <= *tp + j && *tp + j <= TIME_T_MAX)) ...',
	** except that it does the right thing even if *tp + j would overflow.
	*/
	if (! (j < 0
	       ? (TYPE_SIGNED(time_t) ? TIME_T_MIN - j <= *tp : -1 - j < *tp)
	       : *tp <= TIME_T_MAX - j))
		return true;
	*tp += j;
	return false;
#endif
}

static bool
normalize_overflow(int *const tensptr, int *const unitsptr, const int base)
{
	register int	tensdelta;

	tensdelta = (*unitsptr >= 0) ?
		(*unitsptr / base) :
		(-1 - (-1 - *unitsptr) / base);
	*unitsptr -= tensdelta * base;
	return increment_overflow(tensptr, tensdelta);
}

static bool
normalize_overflow32(int32_t *tensptr, int *unitsptr, int base)
{
	register int	tensdelta;

	tensdelta = (*unitsptr >= 0) ?
		(*unitsptr / base) :
		(-1 - (-1 - *unitsptr) / base);
	*unitsptr -= tensdelta * base;
	return increment_overflow32(tensptr, tensdelta);
}

static int
tmcomp(register const struct tm *const atmp,
       register const struct tm *const btmp)
{
	register int	result;

	if (atmp->tm_year != btmp->tm_year)
		return atmp->tm_year < btmp->tm_year ? -1 : 1;
	if ((result = (atmp->tm_mon - btmp->tm_mon)) == 0 &&
		(result = (atmp->tm_mday - btmp->tm_mday)) == 0 &&
		(result = (atmp->tm_hour - btmp->tm_hour)) == 0 &&
		(result = (atmp->tm_min - btmp->tm_min)) == 0)
			result = atmp->tm_sec - btmp->tm_sec;
	return result;
}

static time_t
localtime_time2sub(
	struct tm *const tmp,
	struct tm *(*funcp)(struct state const *, time_t const *,
			    int32_t, struct tm *),
	struct state const *sp,
	const int32_t offset,
	bool *okayp,
	bool do_norm_secs)
{
	register int			dir;
	register int			i, j;
	register int			saved_seconds;
	register int32_t		li;
	register time_t			lo;
	register time_t			hi;
	int32_t			y;
	time_t				newt;
	time_t				t;
	struct tm			yourtm, mytm;

	*okayp = false;
	yourtm = *tmp;
	if (do_norm_secs) {
		if (normalize_overflow(&yourtm.tm_min, &yourtm.tm_sec,
			SECSPERMIN))
				return WRONG;
	}
	if (normalize_overflow(&yourtm.tm_hour, &yourtm.tm_min, MINSPERHOUR))
		return WRONG;
	if (normalize_overflow(&yourtm.tm_mday, &yourtm.tm_hour, HOURSPERDAY))
		return WRONG;
	y = yourtm.tm_year;
	if (normalize_overflow32(&y, &yourtm.tm_mon, MONSPERYEAR))
		return WRONG;
	/*
	** Turn y into an actual year number for now.
	** It is converted back to an offset from TM_YEAR_BASE later.
	*/
	if (increment_overflow32(&y, TM_YEAR_BASE))
		return WRONG;
	while (yourtm.tm_mday <= 0) {
		if (increment_overflow32(&y, -1))
			return WRONG;
		li = y + (1 < yourtm.tm_mon);
		yourtm.tm_mday += year_lengths[isleap(li)];
	}
	while (yourtm.tm_mday > DAYSPERLYEAR) {
		li = y + (1 < yourtm.tm_mon);
		yourtm.tm_mday -= year_lengths[isleap(li)];
		if (increment_overflow32(&y, 1))
			return WRONG;
	}
	for ( ; ; ) {
		i = mon_lengths[isleap(y)][yourtm.tm_mon];
		if (yourtm.tm_mday <= i)
			break;
		yourtm.tm_mday -= i;
		if (++yourtm.tm_mon >= MONSPERYEAR) {
			yourtm.tm_mon = 0;
			if (increment_overflow32(&y, 1))
				return WRONG;
		}
	}
	if (increment_overflow32(&y, -TM_YEAR_BASE))
		return WRONG;
	if (! (INT_MIN <= y && y <= INT_MAX))
		return WRONG;
	yourtm.tm_year = y;
	if (yourtm.tm_sec >= 0 && yourtm.tm_sec < SECSPERMIN)
		saved_seconds = 0;
	else if (y + TM_YEAR_BASE < EPOCH_YEAR) {
		/*
		** We can't set tm_sec to 0, because that might push the
		** time below the minimum representable time.
		** Set tm_sec to 59 instead.
		** This assumes that the minimum representable time is
		** not in the same minute that a leap second was deleted from,
		** which is a safer assumption than using 58 would be.
		*/
		if (increment_overflow(&yourtm.tm_sec, 1 - SECSPERMIN))
			return WRONG;
		saved_seconds = yourtm.tm_sec;
		yourtm.tm_sec = SECSPERMIN - 1;
	} else {
		saved_seconds = yourtm.tm_sec;
		yourtm.tm_sec = 0;
	}
	/*
	** Do a binary search (this works whatever time_t's type is).
	*/
	lo = TIME_T_MIN;
	hi = TIME_T_MAX;
	for ( ; ; ) {
		t = lo / 2 + hi / 2;
		if (t < lo)
			t = lo;
		else if (t > hi)
			t = hi;
		if (! funcp(sp, &t, offset, &mytm)) {
			/*
			** Assume that t is too extreme to be represented in
			** a struct tm; arrange things so that it is less
			** extreme on the next pass.
			*/
			dir = (t > 0) ? 1 : -1;
		} else	dir = tmcomp(&mytm, &yourtm);
		if (dir != 0) {
			if (t == lo) {
				if (t == TIME_T_MAX)
					return WRONG;
				++t;
				++lo;
			} else if (t == hi) {
				if (t == TIME_T_MIN)
					return WRONG;
				--t;
				--hi;
			}
			if (lo > hi)
				return WRONG;
			if (dir > 0)
				hi = t;
			else	lo = t;
			continue;
		}
#if defined TM_GMTOFF && ! UNINIT_TRAP
		if (mytm.TM_GMTOFF != yourtm.TM_GMTOFF
		    && (yourtm.TM_GMTOFF < 0
			? (-SECSPERDAY <= yourtm.TM_GMTOFF
			   && (mytm.TM_GMTOFF <=
			       (SMALLEST(INT32_MAX, LONG_MAX)
				+ yourtm.TM_GMTOFF)))
			: (yourtm.TM_GMTOFF <= SECSPERDAY
			   && ((BIGGEST(INT32_MIN, LONG_MIN)
				+ yourtm.TM_GMTOFF)
			       <= mytm.TM_GMTOFF)))) {
		  /* MYTM matches YOURTM except with the wrong UT offset.
		     YOURTM.TM_GMTOFF is plausible, so try it instead.
		     It's OK if YOURTM.TM_GMTOFF contains uninitialized data,
		     since the guess gets checked.  */
		  time_t altt = t;
		  int32_t diff = mytm.TM_GMTOFF - yourtm.TM_GMTOFF;
		  if (!increment_overflow_time(&altt, diff)) {
		    struct tm alttm;
		    if (funcp(sp, &altt, offset, &alttm)
			&& alttm.tm_isdst == mytm.tm_isdst
			&& alttm.TM_GMTOFF == yourtm.TM_GMTOFF
			&& tmcomp(&alttm, &yourtm) == 0) {
		      t = altt;
		      mytm = alttm;
		    }
		  }
		}
#endif
		if (yourtm.tm_isdst < 0 || mytm.tm_isdst == yourtm.tm_isdst)
			break;
		/*
		** Right time, wrong type.
		** Hunt for right time, right type.
		** It's okay to guess wrong since the guess
		** gets checked.
		*/
		if (sp == NULL)
			return WRONG;
		for (i = sp->typecnt - 1; i >= 0; --i) {
			if (sp->ttis[i].tt_isdst != yourtm.tm_isdst)
				continue;
			for (j = sp->typecnt - 1; j >= 0; --j) {
				if (sp->ttis[j].tt_isdst == yourtm.tm_isdst)
					continue;
				if (ttunspecified(sp, j))
				  continue;
				newt = (t + sp->ttis[j].tt_utoff
					- sp->ttis[i].tt_utoff);
				if (! funcp(sp, &newt, offset, &mytm))
					continue;
				if (tmcomp(&mytm, &yourtm) != 0)
					continue;
				if (mytm.tm_isdst != yourtm.tm_isdst)
					continue;
				/*
				** We have a match.
				*/
				t = newt;
				goto label;
			}
		}
		return WRONG;
	}
label:
	newt = t + saved_seconds;
	if ((newt < t) != (saved_seconds < 0))
		return WRONG;
	t = newt;
	if (funcp(sp, &t, offset, tmp))
		*okayp = true;
	return t;
}

static time_t
localtime_time2(
	struct tm * const tmp,
	struct tm *(*funcp)(struct state const *, time_t const *,
			    int32_t, struct tm *),
	struct state const *sp,
	const int32_t offset,
	bool *okayp)
{
	time_t	t;

	/*
	** First try without normalization of seconds
	** (in case tm_sec contains a value associated with a leap second).
	** If that fails, try with normalization of seconds.
	*/
	t = localtime_time2sub(tmp, funcp, sp, offset, okayp, false);
	return *okayp ? t : localtime_time2sub(tmp,funcp,sp,offset,okayp,true);
}

static time_t
localtime_time1(
	struct tm *const tmp,
	struct tm *(*funcp)(struct state const *, time_t const *,
			    int32_t, struct tm *),
	struct state const *sp,
	const int32_t offset)
{
	register time_t			t;
	register int			samei, otheri;
	register int			sameind, otherind;
	register int			i;
	register int			nseen;
	char				seen[TZ_MAX_TYPES];
	unsigned char			types[TZ_MAX_TYPES];
	bool				okay;

	if (tmp == NULL) {
		errno = EINVAL;
		return WRONG;
	}
	if (tmp->tm_isdst > 1)
		tmp->tm_isdst = 1;
	t = localtime_time2(tmp, funcp, sp, offset, &okay);
	if (okay)
		return t;
	if (tmp->tm_isdst < 0)
#ifdef PCTS
		/*
		** POSIX Conformance Test Suite code courtesy Grant Sullivan.
		*/
		tmp->tm_isdst = 0;	/* reset to std and try again */
#else
		return t;
#endif /* !defined PCTS */
	/*
	** We're supposed to assume that somebody took a time of one type
	** and did some math on it that yielded a "struct tm" that's bad.
	** We try to divine the type they started from and adjust to the
	** type they need.
	*/
	if (sp == NULL)
		return WRONG;
	for (i = 0; i < sp->typecnt; ++i)
		seen[i] = false;
	nseen = 0;
	for (i = sp->timecnt - 1; i >= 0; --i)
		if (!seen[sp->types[i]] && !ttunspecified(sp, sp->types[i])) {
			seen[sp->types[i]] = true;
			types[nseen++] = sp->types[i];
		}
	for (sameind = 0; sameind < nseen; ++sameind) {
		samei = types[sameind];
		if (sp->ttis[samei].tt_isdst != tmp->tm_isdst)
			continue;
		for (otherind = 0; otherind < nseen; ++otherind) {
			otheri = types[otherind];
			if (sp->ttis[otheri].tt_isdst == tmp->tm_isdst)
				continue;
			tmp->tm_sec += (sp->ttis[otheri].tt_utoff
					- sp->ttis[samei].tt_utoff);
			tmp->tm_isdst = !tmp->tm_isdst;
			t = localtime_time2(tmp, funcp, sp, offset, &okay);
			if (okay)
				return t;
			tmp->tm_sec -= (sp->ttis[otheri].tt_utoff
					- sp->ttis[samei].tt_utoff);
			tmp->tm_isdst = !tmp->tm_isdst;
		}
	}
	return WRONG;
}

static time_t
mktime_tzname(struct state *sp, struct tm *tmp, bool setname)
{
	if (sp)
		return localtime_time1(tmp, localsub, sp, setname);
	else {
		localtime_gmtcheck();
		return localtime_time1(tmp, gmtsub, gmtptr, 0);
	}
}

time_t
mktime(struct tm *tmp)
{
	time_t t;
	localtime_lock();
	localtime_tzset_unlocked();
	t = mktime_tzname(lclptr, tmp, true);
	localtime_unlock();
	return t;
}

time_t
timelocal(struct tm *tmp)
{
	if (tmp != NULL)
		tmp->tm_isdst = -1;	/* in case it wasn't initialized */
	return mktime(tmp);
}

time_t
timegm(struct tm *tmp)
{
	return timeoff(tmp, 0);
}

time_t
timeoff(struct tm *tmp, long offset)
{
	if (tmp)
		tmp->tm_isdst = 0;
	gmtcheck();
	return localtime_time1(tmp, gmtsub, gmtptr, offset);
}

static int32_t
leapcorr(struct state const *sp, time_t t)
{
	register struct lsinfo const *	lp;
	register int			i;

	i = sp->leapcnt;
	while (--i >= 0) {
		lp = &sp->lsis[i];
		if (t >= lp->ls_trans)
			return lp->ls_corr;
	}
	return 0;
}
