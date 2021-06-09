/*
  zipnote.c - Zip 3

  Copyright (c) 1990-2008 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*
 *  zipnote.c by Mark Adler.
 */
#define __ZIPNOTE_C

#ifndef UTIL
#define UTIL
#endif
#include "zip.h"
#define DEFCPYRT        /* main module: enable copyright string defines! */
#include "revision.h"
#include <signal.h>

/* Calculate size of static line buffer used in write (-w) mode. */
#define WRBUFSIZ 2047
/* The line buffer size should be at least as large as FNMAX. */
#if FNMAX > WRBUFSIZ
#  undef WRBUFSIZ
#  define WRBUFSIZ FNMAX
#endif

/* Character to mark zip entry names in the comment file */
#define MARK '@'
#define MARKE " (comment above this line)"
#define MARKZ " (zip file comment below this line)"

/* Temporary zip file pointer */
local FILE *tempzf;


/* Local functions */
local void handler OF((int));
local void license OF((void));
local void help OF((void));
local void version_info OF((void));
local void putclean OF((char *, extent));
/* getline name conflicts with GNU getline() function */
local char *zgetline OF((char *, extent));
local int catalloc OF((char * far *, char *));
int main OF((int, char **));

/* keep compiler happy until implement long options - 11/4/2003 EG */
struct option_struct far options[] = {
  /* short longopt        value_type        negatable        ID    name */
    {"h",  "help",        o_NO_VALUE,       o_NOT_NEGATABLE, 'h',  "help"},
    /* the end of the list */
    {NULL, NULL,          o_NO_VALUE,       o_NOT_NEGATABLE, 0,    NULL} /* end has option_ID = 0 */
  };

#ifdef MACOS
#define ziperr(c, h)    zipnoteerr(c, h)
#define zipwarn(a, b)   zipnotewarn(a, b)

void zipnoteerr(int c, ZCONST char *h);
void zipnotewarn(ZCONST char *a, ZCONST char *b);
#endif

#ifdef QDOS
#define exit(p1) QDOSexit()
#endif

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

void zipmessage_nl(a, nl)
ZCONST char *a;     /* message string to output */
int nl;             /* 1 = add nl to end */
/* If nl false, print a message to mesg without new line.
   If nl true, print and add new line.  If logfile is
   open then also write message to log file. */
{
  if (noisy) {
    fprintf(mesg, "%s", a);
    if (nl) {
      fprintf(mesg, "\n");
      mesg_line_started = 0;
    } else {
      mesg_line_started = 1;
    }
    fflush(mesg);
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
}

void ziperr(c, h)
int c;                  /* error code from the ZE_ class */
ZCONST char *h;         /* message about how it happened */
/* Issue a message for the error, clean up files and memory, and exit. */
{
  if (PERR(c))
    perror("zipnote error");
  fprintf(mesg, "zipnote error: %s (%s)\n", ZIPERRORS(c), h);
  if (tempzf != NULL)
    fclose(tempzf);
  if (tempzip != NULL)
  {
    destroy(tempzip);
    free((zvoid *)tempzip);
  }
  if (zipfile != NULL)
    free((zvoid *)zipfile);
  EXIT(c);
}


local void handler(s)
int s;                  /* signal number (ignored) */
/* Upon getting a user interrupt, abort cleanly using ziperr(). */
{
#ifndef MSDOS
  putc('\n', mesg);
#endif /* !MSDOS */
  ziperr(ZE_ABORT, "aborting");
  s++;                                  /* keep some compilers happy */
}


void zipwarn(a, b)
ZCONST char *a, *b;     /* message strings juxtaposed in output */
/* Print a warning message to mesg (usually stderr) and return. */
{
  fprintf(mesg, "zipnote warning: %s%s\n", a, b);
}


local void license()
/* Print license information to stdout. */
{
  extent i;             /* counter for copyright array */

  for (i = 0; i < sizeof(swlicense)/sizeof(char *); i++)
    puts(swlicense[i]);
}


local void help()
/* Print help (along with license info) to stdout. */
{
  extent i;             /* counter for help array */

  /* help array */
  static ZCONST char *text[] = {
"",
"ZipNote %s (%s)",
#ifdef VM_CMS
"Usage:  zipnote [-w] [-q] [-b fm] zipfile",
#else
"Usage:  zipnote [-w] [-q] [-b path] zipfile",
#endif
"  the default action is to write the comments in zipfile to stdout",
"  -w   write the zipfile comments from stdin",
#ifdef VM_CMS
"  -b   use \"fm\" as the filemode for the temporary zip file",
#else
"  -b   use \"path\" for the temporary zip file",
#endif
"  -q   quieter operation, suppress some informational messages",
"  -h   show this help    -v   show version info    -L   show software license",
"",
"Example:",
#ifdef VMS
"     define/user sys$output foo.tmp",
"     zipnote foo.zip",
"     edit foo.tmp",
"     ... then you edit the comments, save, and exit ...",
"     define/user sys$input foo.tmp",
"     zipnote -w foo.zip",
#else
#ifdef RISCOS
"     zipnote foo/zip > foo/tmp",
"     <!Edit> foo/tmp",
"     ... then you edit the comments, save, and exit ...",
"     zipnote -w foo/zip < foo/tmp",
#else
#ifdef VM_CMS
"     zipnote foo.zip > foo.tmp",
"     xedit foo tmp",
"     ... then you edit the comments, save, and exit ...",
"     zipnote -w foo.zip < foo.tmp",
#else
"     zipnote foo.zip > foo.tmp",
"     ed foo.tmp",
"     ... then you edit the comments, save, and exit ...",
"     zipnote -w foo.zip < foo.tmp",
#endif /* VM_CMS */
#endif /* RISCOS */
#endif /* VMS */
"",
"  \"@ name\" can be followed by an \"@=newname\" line to change the name"
  };

  for (i = 0; i < sizeof(copyright)/sizeof(char *); i++) {
    printf(copyright[i], "zipnote");
    putchar('\n');
  }
  for (i = 0; i < sizeof(text)/sizeof(char *); i++)
  {
    printf(text[i], VERSION, REVDATE);
    putchar('\n');
  }
}

/*
 * XXX put this in version.c
 */

local void version_info()
/* Print verbose info about program version and compile time options
   to stdout. */
{
  extent i;             /* counter in text arrays */

  /* Options info array */
  static ZCONST char *comp_opts[] = {
#ifdef DEBUG
    "DEBUG",
#endif
    NULL
  };

  for (i = 0; i < sizeof(copyright)/sizeof(char *); i++)
  {
    printf(copyright[i], "zipnote");
    putchar('\n');
  }

  for (i = 0; i < sizeof(versinfolines)/sizeof(char *); i++)
  {
    printf(versinfolines[i], "ZipNote", VERSION, REVDATE);
    putchar('\n');
  }

  version_local();

  puts("ZipNote special compilation options:");
  for (i = 0; (int)i < (int)(sizeof(comp_opts)/sizeof(char *) - 1); i++)
  {
    printf("\t%s\n",comp_opts[i]);
  }
  if (i == 0)
      puts("\t[none]");
}


local void putclean(s, n)
char *s;                /* string to write to stdout */
extent n;               /* length of string */
/* Write the string s to stdout, filtering out control characters that are
   not tab or newline (mainly to remove carriage returns), and prefix MARK's
   and backslashes with a backslash.  Also, terminate with a newline if
   needed. */
{
  int c;                /* next character in string */
  int e;                /* last character written */

  e = '\n';                     /* if empty, write nothing */
  while (n--)
  {
    c = *(uch *)s++;
    if (c == MARK || c == '\\')
      putchar('\\');
    if (c >= ' ' || c == '\t' || c == '\n')
      { e=c; putchar(e); }
  }
  if (e != '\n')
    putchar('\n');
}


local char *zgetline(buf, size)
char *buf;
extent size;
/* Read a line of text from stdin into string buffer 'buf' of size 'size'.
   In case of buffer overflow or EOF, a NULL pointer is returned. */
{
    char *line;
    unsigned len;

    line = fgets(buf, size, stdin);
    if (line != NULL && (len = strlen(line)) > 0) {
        if (len == size-1 && line[len-1] != '\n') {
            /* buffer is full and record delimiter not seen -> overflow */
            line = NULL;
        } else {
            /* delete trailing record delimiter */
            if (line[len-1] == '\n') line[len-1] = '\0';
        }
    }
    return line;
}


local int catalloc(a, s)
char * far *a;          /* pointer to a pointer to a malloc'ed string */
char *s;                /* string to concatenate on a */
/* Concatentate the string s to the malloc'ed string pointed to by a.
   Preprocess s by removing backslash escape characters. */
{
  char *p;              /* temporary pointer */
  char *q;              /* temporary pointer */

  for (p = q = s; *q; *p++ = *q++)
    if (*q == '\\' && *(q+1))
      q++;
  *p = 0;
  if ((p = malloc(strlen(*a) + strlen(s) + 3)) == NULL)
    return ZE_MEM;
  strcat(strcat(strcpy(p, *a), **a ? "\r\n" : ""), s);
  free((zvoid *)*a);
  *a = p;
  return ZE_OK;
}


#ifndef USE_ZIPNOTEMAIN
int main(argc, argv)
#else
int zipnotemain(argc, argv)
#endif
int argc;               /* number of tokens in command line */
char **argv;            /* command line tokens */
/* Write the comments in the zipfile to stdout, or read them from stdin. */
{
  char abf[WRBUFSIZ+1]; /* input line buffer */
  char *a;              /* pointer to line buffer or NULL */
  zoff_t c;             /* start of central directory */
  int k;                /* next argument type */
  char *q;              /* steps through option arguments */
  int r;                /* arg counter, temporary variable */
  zoff_t s;             /* length of central directory */
  int t;                /* attributes of zip file */
  int w;                /* true if updating zip file from stdin */
  FILE *x;              /* input file for testing if can write it */
  struct zlist far *z;  /* steps through zfiles linked list */

#ifdef THEOS
  setlocale(LC_CTYPE, "I");
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

  /* If no args, show help */
  if (argc == 1)
  {
    help();
    EXIT(ZE_OK);
  }

  /* Direct info messages to stderr; stdout is used for data output. */
  mesg = stderr;

  init_upper();           /* build case map table */

  /* Go through args */
  zipfile = tempzip = NULL;
  tempzf = NULL;
  signal(SIGINT, handler);
#ifdef SIGTERM              /* AMIGA has no SIGTERM */
  signal(SIGTERM, handler);
#endif
#ifdef SIGABRT
  signal(SIGABRT, handler);
#endif
#ifdef SIGBREAK
  signal(SIGBREAK, handler);
#endif
#ifdef SIGBUS
  signal(SIGBUS, handler);
#endif
#ifdef SIGILL
  signal(SIGILL, handler);
#endif
#ifdef SIGSEGV
  signal(SIGSEGV, handler);
#endif
  k = w = 0;
  for (r = 1; r < argc; r++)
    if (*argv[r] == '-') {
      if (argv[r][1])
        for (q = argv[r]+1; *q; q++)
          switch (*q)
          {
            case 'b':   /* Specify path for temporary file */
              if (k)
                ziperr(ZE_PARMS, "use -b before zip file name");
              else
                k = 1;          /* Next non-option is path */
              break;
            case 'h':   /* Show help */
              help();  EXIT(ZE_OK);
            case 'l':  case 'L':  /* Show copyright and disclaimer */
              license();  EXIT(ZE_OK);
            case 'q':   /* Quiet operation, suppress info messages */
              noisy = 0;  break;
            case 'v':   /* Show version info */
              version_info();  EXIT(ZE_OK);
            case 'w':
              w = 1;  break;
            default:
              ziperr(ZE_PARMS, "unknown option");
          }
      else
        ziperr(ZE_PARMS, "zip file cannot be stdin");
    } else
      if (k == 0)
      {
        if (zipfile == NULL)
        {
          if ((zipfile = ziptyp(argv[r])) == NULL)
            ziperr(ZE_MEM, "was processing arguments");
        }
        else
          ziperr(ZE_PARMS, "can only specify one zip file");
      }
      else
      {
        tempath = argv[r];
        k = 0;
      }
  if (zipfile == NULL)
    ziperr(ZE_PARMS, "need to specify zip file");

  if ((in_path = malloc(strlen(zipfile) + 1)) == NULL) {
    ziperr(ZE_MEM, "input");
  }
  strcpy(in_path, zipfile);

  /* Read zip file */
  if ((r = readzipfile()) != ZE_OK)
    ziperr(r, zipfile);
  if (zfiles == NULL)
    ziperr(ZE_NAME, zipfile);

  /* Put comments to stdout, if not -w */
  if (!w)
  {
    for (z = zfiles; z != NULL; z = z->nxt)
    {
      printf("%c %s\n", MARK, z->zname);
      putclean(z->comment, z->com);
      printf("%c%s\n", MARK, MARKE);
    }
    printf("%c%s\n", MARK, MARKZ);
    putclean(zcomment, zcomlen);
    EXIT(ZE_OK);
  }

  /* If updating comments, make sure zip file is writeable */
  if ((x = fopen(zipfile, "a")) == NULL)
    ziperr(ZE_CREAT, zipfile);
  fclose(x);
  t = getfileattr(zipfile);

  /* Process stdin, replacing comments */
  z = zfiles;
  while ((a = zgetline(abf, WRBUFSIZ+1)) != NULL &&
         (a[0] != MARK || strcmp(a + 1, MARKZ)))
  {                                     /* while input and not file comment */
    if (a[0] != MARK || a[1] != ' ')    /* better be "@ name" */
      ziperr(ZE_NOTE, "unexpected input");
    while (z != NULL && strcmp(a + 2, z->zname))
      z = z->nxt;                       /* allow missing entries in order */
    if (z == NULL)
      ziperr(ZE_NOTE, "unknown entry name");
    if ((a = zgetline(abf, WRBUFSIZ+1)) != NULL && a[0] == MARK && a[1] == '=')
    {
      if (z->name != z->iname)
        free((zvoid *)z->iname);
      if ((z->iname = malloc(strlen(a+1))) == NULL)
        ziperr(ZE_MEM, "was changing name");
#ifdef EBCDIC
      strtoasc(z->iname, a+2);
#else
      strcpy(z->iname, a+2);
#endif

/*
 * Don't update z->nam here, we need the old value a little later.....
 * The update is handled in zipcopy().
 */
      a = zgetline(abf, WRBUFSIZ+1);
    }
    if (z->com)                         /* change zip entry comment */
      free((zvoid *)z->comment);
    z->comment = malloc(1);  *(z->comment) = 0;
    while (a != NULL && *a != MARK)
    {
      if ((r = catalloc(&(z->comment), a)) != ZE_OK)
        ziperr(r, "was building new zipentry comments");
      a = zgetline(abf, WRBUFSIZ+1);
    }
    z->com = strlen(z->comment);
    z = z->nxt;                         /* point to next entry */
  }
  if (a != NULL)                        /* change zip file comment */
  {
    zcomment = malloc(1);  *zcomment = 0;
    while ((a = zgetline(abf, WRBUFSIZ+1)) != NULL)
      if ((r = catalloc(&zcomment, a)) != ZE_OK)
        ziperr(r, "was building new zipfile comment");
    zcomlen = strlen(zcomment);
  }

  /* Open output zip file for writing */
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
    if ((tempzf = y = fdopen(yd, FOPW)) == NULL) {
      ZIPERR(ZE_TEMP, tempzip);
    }
  }
#else
  if ((tempzf = y = fopen(tempzip = tempname(zipfile), FOPW)) == NULL)
    ziperr(ZE_TEMP, tempzip);
#endif

  /* Open input zip file again, copy preamble if any */
  if ((in_file = fopen(zipfile, FOPR)) == NULL)
    ziperr(ZE_NAME, zipfile);

  if (zipbeg && (r = bfcopy(zipbeg)) != ZE_OK)
    ziperr(r, r == ZE_TEMP ? tempzip : zipfile);
  tempzn = zipbeg;

  /* Go through local entries, copying them over as is */
  fix = 3; /* needed for zipcopy if name changed */
  for (z = zfiles; z != NULL; z = z->nxt) {
    if ((r = zipcopy(z)) != ZE_OK)
      ziperr(r, "was copying an entry");
  }
  fclose(x);

  /* Write central directory and end of central directory with new comments */
  if ((c = zftello(y)) == (zoff_t)-1)    /* get start of central */
    ziperr(ZE_TEMP, tempzip);
  for (z = zfiles; z != NULL; z = z->nxt)
    if ((r = putcentral(z)) != ZE_OK)
      ziperr(r, tempzip);
  if ((s = zftello(y)) == (zoff_t)-1)    /* get end of central */
    ziperr(ZE_TEMP, tempzip);
  s -= c;                       /* compute length of central */
  if ((r = putend((zoff_t)zcount, s, c, zcomlen, zcomment)) != ZE_OK)
    ziperr(r, tempzip);
  tempzf = NULL;
  if (fclose(y))
    ziperr(ZE_TEMP, tempzip);
  if ((r = replace(zipfile, tempzip)) != ZE_OK)
  {
    zipwarn("new zip file left as: ", tempzip);
    free((zvoid *)tempzip);
    tempzip = NULL;
    ziperr(r, "was replacing the original zip file");
  }
  free((zvoid *)tempzip);
  tempzip = NULL;
  setfileattr(zipfile, t);
#ifdef RISCOS
  /* Set the filetype of the zipfile to &DDC */
  setfiletype(zipfile,0xDDC);
#endif
  free((zvoid *)zipfile);
  zipfile = NULL;

  /* Done! */
  RETURN(0);
}
