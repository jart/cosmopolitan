/* version information

  (c) 2007-2015 (W3C) MIT, ERCIM, Keio University
  See tidy.h for the copyright notice.

*/

#ifdef RELEASE_DATE
static const char TY_(release_date)[] = RELEASE_DATE;
#else
static const char TY_(release_date)[] = "2015/01/22";
#endif
#ifdef LIBTIDY_VERSION
#ifdef RC_NUMBER
static const char TY_(library_version)[] = LIBTIDY_VERSION "." RC_NUMBER;
#else
static const char TY_(library_version)[] = LIBTIDY_VERSION;
#endif
#else
static const char TY_(library_version)[] = "5.0.0";
#endif

/* eof */
