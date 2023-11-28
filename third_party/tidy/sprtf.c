/* sprtf.c
 * SPRTF - Log output utility - part of the HTML Tidy project
 *
 * Copyright (c) 1998-2017 Geoff R. McLane and HTACG
 *
 * See tidy.h for the copyright notice.
 */

#ifdef _MSC_VER
#  pragma warning( disable : 4995 )
#endif


#ifdef _MSC_VER
#  if (defined(UNICODE) || defined(_UNICODE))
#  endif
#else /* !_MSC_VER */
#endif /* _MSC_VER y/n */

#include "third_party/tidy/sprtf.h"

#ifdef ENABLE_DEBUG_LOG

#ifdef _MSC_VER
#  ifndef _CRT_SECURE_NO_DEPRECATE
#    define _CRT_SECURE_NO_DEPRECATE
#  endif /* #ifndef _CRT_SECURE_NO_DEPRECATE */
#  pragma warning( disable:4996 )
#else
#  define strcmpi strcasecmp
#endif

#ifndef MX_ONE_BUF
#  define MX_ONE_BUF 1024
#endif
#ifndef MX_BUFFERS
#  define MX_BUFFERS 1024
#endif

static char _s_strbufs[MX_ONE_BUF * MX_BUFFERS];
static int iNextBuf = 0;

char *GetNxtBuf()
{
   iNextBuf++;
   if(iNextBuf >= MX_BUFFERS)
      iNextBuf = 0;
   return &_s_strbufs[MX_ONE_BUF * iNextBuf];
}

#define  MXIO     512

static char def_log[] = "temptidy.txt"; /* use local log */
static char logfile[264] = "\0";
static FILE * outfile = NULL;
static int addsystime = 0;
static int addsysdate = 0;
static int addstdout = 1;
static int addflush = 1;
static int add2screen = 0;
static int add2listview = 0;
static int append_to_log = 0;

#ifndef VFP
#  define VFP(a) ( a && ( a != (FILE *)-1 ) )
#endif

int   add_list_out( int val )
{
   int i = add2listview;
   add2listview = val;
   return i;
}

int   add_std_out( int val )
{
   int i = addstdout;
   addstdout = val;
   return i;
}

int   add_screen_out( int val )
{
   int i = add2screen;
   add2screen = val;
   return i;
}


int   add_sys_time( int val )
{
   int   i = addsystime;
   addsystime = val;
   return i;
}

int   add_sys_date( int val )
{
   int   i = addsysdate;
   addsysdate = val;
   return i;
}


int   add_append_log( int val )
{
   int   i = append_to_log;
   append_to_log = val;
   return i;
}


#ifdef _MSC_VER
static const char *mode = "wb"; /* in window sprtf looks after the line endings */
#else
static const char *mode = "w";
#endif

int   open_log_file( void )
{
   if (logfile[0] == 0)
      strcpy(logfile,def_log);
   if (append_to_log) {
#ifdef _MSC_VER
        mode = "ab"; /* in window sprtf looks after the line endings */
#else
        mode = "a";
#endif
   }
   outfile = fopen(logfile, mode);
   if( outfile == 0 ) {
      outfile = (FILE *)-1;
      sprtf("ERROR: Failed to open log file [%s] ...\n", logfile);
      /* exit(1); failed */
      return 0;   /* failed */
   }
   return 1; /* success */
}

void close_log_file( void )
{
   if( VFP(outfile) ) {
      fclose(outfile);
   }
   outfile = NULL;
}

char * get_log_file( void )
{
   if (logfile[0] == 0)
      strcpy(logfile,def_log);
   if (outfile == (FILE *)-1) /* disable the log file */
       return (char *)"none";
   return logfile;
}

void   set_log_file( char * nf, int open )
{
   if (logfile[0] == 0)
      strcpy(logfile,def_log);
   if ( nf && *nf && strcmpi(nf,logfile) ) {
      close_log_file(); /* remove any previous */
      strcpy(logfile,nf); /* set new name */
      if (strcmp(logfile,"none") == 0) { /* if equal 'none' */
          outfile = (FILE *)-1; /* disable the log file */
      } else if (open) {
          open_log_file();  /* and open it ... anything previous written is 'lost' */
      } else
          outfile = 0; /* else set 0 to open on first write */
   }
}

#ifdef _MSC_VER
int _gettimeofday(struct timeval *tp, void *tzp)
{
#ifdef WIN32
    struct _timeb timebuffer;
    _ftime(&timebuffer);
    tp->tv_sec = (long)timebuffer.time;
    tp->tv_usec = timebuffer.millitm * 1000;
#else
    tp->tv_sec = time(NULL);
    tp->tv_usec = 0;
#endif
    return 0;
}

#endif /* _MSC_VER */

void add_date_stg( char *ps, struct timeval *ptv )
{
    time_t curtime;
    struct tm * ptm;
    curtime = (ptv->tv_sec & 0xffffffff);
    ptm = localtime(&curtime);
    if (ptm) {
        strftime(EndBuf(ps),128,"%Y/%m/%d",ptm);
    }
}

void add_time_stg( char *ps, struct timeval *ptv )
{
    time_t curtime;
    struct tm * ptm;
    curtime = (ptv->tv_sec & 0xffffffff);
    ptm = localtime(&curtime);
    if (ptm) {
        strftime(EndBuf(ps),128,"%H:%M:%S",ptm);
    }
}

char *get_date_stg()
{
    char *ps;
    struct timeval tv;
    gettimeofday( (struct timeval *)&tv, (struct timezone *)0 );
    ps = GetNxtBuf();
    *ps = 0;
    add_date_stg( ps, &tv );
    return ps;
}

char *get_time_stg()
{
    char *ps;
    struct timeval tv;
    gettimeofday( (struct timeval *)&tv, (struct timezone *)0 );
    ps = GetNxtBuf();
    *ps = 0;
    add_time_stg( ps, &tv );
    return ps;
}

char *get_date_time_stg()
{
    char *ps;
    struct timeval tv;
    gettimeofday( (struct timeval *)&tv, (struct timezone *)0 );
    ps = GetNxtBuf();
    *ps = 0;
    add_date_stg( ps, &tv );
    strcat(ps," ");
    add_time_stg( ps, &tv );
    return ps;
}

static void oi( char * psin )
{
    int len, w;
    char * ps = psin;
    if (!ps)
        return;

   len = (int)strlen(ps);
   if (len) {

      if( outfile == 0 ) {
         open_log_file();
      }
      if( VFP(outfile) ) {
          char *tb;
          if (addsysdate) {
              tb = GetNxtBuf();
              len = sprintf( tb, "%s - %s", get_date_time_stg(), ps );
              ps = tb;
          } else if( addsystime ) {
              tb = GetNxtBuf();
              len = sprintf( tb, "%s - %s", get_time_stg(), ps );
              ps = tb;
          }

         w = (int)fwrite( ps, 1, len, outfile );
         if( w != len ) {
            fclose(outfile);
            outfile = (FILE *)-1;
            sprtf("WARNING: Failed write to log file [%s] ...\n", logfile);
            exit(1);
         } else if (addflush) {
            fflush( outfile );
         }
      }

      if( addstdout ) {
         fwrite( ps, 1, len, stderr );  /* 20170917 - Switch to using 'stderr' in place of 'stdout' */
      }
#ifdef ADD_LISTVIEW
       if (add2listview) {
           LVInsertItem(ps);
       }
#endif /* ADD_LISTVIEW */
#ifdef ADD_SCREENOUT
       if (add2screen) {
          Add_String(ps);    /* add string to screen list */
       }
#endif /* #ifdef ADD_SCREENOUT */
   }
}

#ifdef _MSC_VER
/* service to ensure line endings in windows only */
static void prt( char * ps )
{
    static char _s_buf[1024];
    char * pb = _s_buf;
    size_t i, j, k;
    char   c, d;
    i = strlen(ps);
    k = 0;
    d = 0;
    if(i) {
        k = 0;
        d = 0;
        for( j = 0; j < i; j++ ) {
            c = ps[j];
            if( c == 0x0d ) {
                if( (j+1) < i ) {
                    if( ps[j+1] != 0x0a ) {
                        pb[k++] = c;
                        c = 0x0a;
                    }
            } else {
                    pb[k++] = c;
                    c = 0x0a;
                }
            } else if( c == 0x0a ) {
                if( d != 0x0d ) {
                    pb[k++] = 0x0d;
                }
            }
            pb[k++] = c;
            d = c;
            if( k >= MXIO ) {
                pb[k] = 0;
                oi(pb);
                k = 0;
            }
        }   /* for length of string */
        if( k ) {
            pb[k] = 0;
            oi( pb );
        }
    }
}
#endif /* #ifdef _MSC_VER */

int direct_out_it( char *cp )
{
#ifdef _MSC_VER
    prt(cp);
#else
    oi(cp);
#endif
    return (int)strlen(cp);
}

/* STDAPI StringCchVPrintf( OUT LPTSTR  pszDest,
 *   IN  size_t  cchDest, IN  LPCTSTR pszFormat, IN  va_list argList ); */
int MCDECL sprtf( const char *pf, ... )
{
   static char _s_sprtfbuf[M_MAX_SPRTF+4];
   char * pb = _s_sprtfbuf;
   int   i;
   va_list arglist;
   va_start(arglist, pf);
   i = vsnprintf( pb, M_MAX_SPRTF, pf, arglist );
   va_end(arglist);
#ifdef _MSC_VER
   prt(pb); /* ensure CR/LF */
#else
   oi(pb);
#endif
   return i;
}

#ifdef UNICODE
/* WIDE VARIETY */
static void wprt( PTSTR ps )
{
   static char _s_woibuf[1024];
   char * cp = _s_woibuf;
   int len = (int)lstrlen(ps);
   if(len) {
      int ret = WideCharToMultiByte( CP_ACP, /* UINT CodePage, // code page */
         0, /* DWORD dwFlags,            // performance and mapping flags */
         ps,   /* LPCWSTR lpWideCharStr,    // wide-character string */
         len,     /* int cchWideChar,          // number of chars in string. */
         cp,      /* LPSTR lpMultiByteStr,     // buffer for new string */
         1024,    /* int cbMultiByte,          // size of buffer */
         NULL,    /* LPCSTR lpDefaultChar,     // default for unmappable chars */
         NULL );  /* LPBOOL lpUsedDefaultChar  // set when default char used */
      /* oi(cp); */
      prt(cp);
   }
}

int MCDECL wsprtf( PTSTR pf, ... )
{
   static WCHAR _s_sprtfwbuf[1024];
   PWSTR pb = _s_sprtfwbuf;
   int   i = 1;
   va_list arglist;
   va_start(arglist, pf);
   *pb = 0;
   StringCchVPrintf(pb,1024,pf,arglist);
   va_end(arglist);
   wprt(pb);
   return i;
}

#endif /* #ifdef UNICODE */

#endif /* #ifdef ENABLE_DEBUG_LOG */
/* eof - sprtf.c */
