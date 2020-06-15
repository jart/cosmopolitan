#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/log/log.h"
#include "libc/stdio/stdio.h"
#include "third_party/f2c/f2c.h"
#include "third_party/f2c/fio.h"
#include "third_party/f2c/fmt.h"

extern char *f__r_mode[], *f__w_mode[];

/*global definitions*/
unit f__units[MXUNIT]; /*unit table*/
flag f__init;          /*0 on entry, 1 after initializations*/
cilist *f__elist;      /*active external io list*/
icilist *f__svic;      /*active internal io list*/
flag f__reading;       /*1 if reading, 0 if writing*/
flag f__cplus, f__cblank;
const char *f__fmtbuf;
flag f__external;   /*1 if external io, 0 if internal */
flag f__sequential; /*1 if sequential io, 0 if direct*/
flag f__formatted;  /*1 if formatted io, 0 if unformatted*/
FILE *f__cf;        /*current file*/
unit *f__curunit;   /*current unit*/
int f__recpos;      /*place in current record*/
OFF_T f__cursor, f__hiwater;
int f__scale;
char *f__icptr;
int (*f__getn)(void); /* for formatted input */
void (*f__putn)(int); /* for formatted output */
int (*f__doed)(struct syl *, char *, ftnlen), (*f__doned)(struct syl *);
int (*f__dorevert)(void), (*f__donewrec)(void), (*f__doend)(void);

/*error messages*/
const char *F_err[] = {
    "error in format",               /* 100 */
    "illegal unit number",           /* 101 */
    "formatted io not allowed",      /* 102 */
    "unformatted io not allowed",    /* 103 */
    "direct io not allowed",         /* 104 */
    "sequential io not allowed",     /* 105 */
    "can't backspace file",          /* 106 */
    "null file name",                /* 107 */
    "can't stat file",               /* 108 */
    "unit not connected",            /* 109 */
    "off end of record",             /* 110 */
    "truncation failed in endfile",  /* 111 */
    "incomprehensible list input",   /* 112 */
    "out of free space",             /* 113 */
    "unit not connected",            /* 114 */
    "read unexpected character",     /* 115 */
    "bad logical input field",       /* 116 */
    "bad variable type",             /* 117 */
    "bad namelist name",             /* 118 */
    "variable not in namelist",      /* 119 */
    "no end record",                 /* 120 */
    "variable count incorrect",      /* 121 */
    "subscript for scalar variable", /* 122 */
    "invalid array section",         /* 123 */
    "substring out of bounds",       /* 124 */
    "subscript out of bounds",       /* 125 */
    "can't read file",               /* 126 */
    "can't write file",              /* 127 */
    "'new' file exists",             /* 128 */
    "can't append to file",          /* 129 */
    "non-positive record number",    /* 130 */
    "nmLbuf overflow"                /* 131 */
};

#define MAXERR (sizeof(F_err) / sizeof(char *) + 100)

int f__canseek(FILE *f) /*SYSDEP*/
{
#ifdef NON_UNIX_STDIO
  return !isatty(fileno(f));
#else
  struct stat x;

  if (fstat(fileno(f), &x) < 0) return (0);
#ifdef S_IFMT
  switch (x.st_mode & S_IFMT) {
    case S_IFDIR:
    case S_IFREG:
      if (x.st_nlink > 0) /* !pipe */
        return (1);
      else
        return (0);
    case S_IFCHR:
      if (isatty(fileno(f))) return (0);
      return (1);
#ifdef S_IFBLK
    case S_IFBLK:
      return (1);
#endif
  }
#else
#ifdef S_ISDIR
  /* POSIX version */
  if (S_ISREG(x.st_mode) || S_ISDIR(x.st_mode)) {
    if (x.st_nlink > 0) /* !pipe */
      return (1);
    else
      return (0);
  }
  if (S_ISCHR(x.st_mode)) {
    if (isatty(fileno(f))) return (0);
    return (1);
  }
  if (S_ISBLK(x.st_mode)) return (1);
#else
  Help !How does fstat work on this system
      ?
#endif
#endif
  return (0); /* who knows what it is? */
#endif
}

void f__fatal(int n, const char *s) {
  if (n < 100 && n >= 0)
    fprintf(stderr, "error: %s: %m\n", s); /*SYSDEP*/
  else if (n >= (int)MAXERR || n < -1) {
    fprintf(stderr, "%s: illegal error number %d\n", s, n);
  } else if (n == -1)
    fprintf(stderr, "%s: end of file\n", s);
  else
    fprintf(stderr, "%s: %s\n", s, F_err[n - 100]);
  if (f__curunit) {
    fprintf(stderr, "apparent state: unit %d ", (int)(f__curunit - f__units));
    fprintf(stderr, f__curunit->ufnm ? "named %s\n" : "(unnamed)\n",
            f__curunit->ufnm);
  } else
    fprintf(stderr, "apparent state: internal I/O\n");
  if (f__fmtbuf) fprintf(stderr, "last format: %s\n", f__fmtbuf);
  fprintf(stderr, "lately %s %s %s %s", f__reading ? "reading" : "writing",
          f__sequential ? "sequential" : "direct",
          f__formatted ? "formatted" : "unformatted",
          f__external ? "external" : "internal");
  sig_die(" IO", 1);
}

/*initialization routine*/
VOID f_init(Void) {
  unit *p;

  f__init = 1;
  p = &f__units[0];
  p->ufd = stderr;
  p->useek = f__canseek(stderr);
  p->ufmt = 1;
  p->uwrt = 1;
  p = &f__units[5];
  p->ufd = stdin;
  p->useek = f__canseek(stdin);
  p->ufmt = 1;
  p->uwrt = 0;
  p = &f__units[6];
  p->ufd = stdout;
  p->useek = f__canseek(stdout);
  p->ufmt = 1;
  p->uwrt = 1;
}

int f__nowreading(unit *x) {
  OFF_T loc;
  int ufmt, urw;

  if (x->urw & 1) goto done;
  if (!x->ufnm) goto cantread;
  ufmt = x->url ? 0 : x->ufmt;
  loc = ftell(x->ufd);
  urw = 3;
  if (!freopen(x->ufnm, f__w_mode[ufmt | 2], x->ufd)) {
    urw = 1;
    if (!freopen(x->ufnm, f__r_mode[ufmt], x->ufd)) {
    cantread:
      errno = 126;
      return 1;
    }
  }
  fseek(x->ufd, loc, SEEK_SET);
  x->urw = urw;
done:
  x->uwrt = 0;
  return 0;
}

int f__nowwriting(unit *x) {
  OFF_T loc;
  int ufmt;

  if (x->urw & 2) {
    if (x->urw & 1) fseek(x->ufd, (OFF_T)0, SEEK_CUR);
    goto done;
  }
  if (!x->ufnm) goto cantwrite;
  ufmt = x->url ? 0 : x->ufmt;
  if (x->uwrt == 3) { /* just did write, rewind */
    if (!(f__cf = x->ufd = freopen(x->ufnm, f__w_mode[ufmt], x->ufd)))
      goto cantwrite;
    x->urw = 2;
  } else {
    loc = ftell(x->ufd);
    if (!(f__cf = x->ufd = freopen(x->ufnm, f__w_mode[ufmt | 2], x->ufd))) {
      x->ufd = NULL;
    cantwrite:
      errno = 127;
      return (1);
    }
    x->urw = 3;
    fseek(x->ufd, loc, SEEK_SET);
  }
done:
  x->uwrt = 1;
  return 0;
}

int err__fl(int f, int m, const char *s) {
  if (!f) f__fatal(m, s);
  if (f__doend) (*f__doend)();
  return errno = m;
}
