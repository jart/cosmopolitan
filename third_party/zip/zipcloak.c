/*
  zipcloak.c - Zip 3

  Copyright (c) 1990-2008 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*
   This code was originally written in Europe and could be freely distributed
   from any country except the U.S.A. If this code was imported into the U.S.A,
   it could not be re-exported from the U.S.A to another country. (This
   restriction might seem curious but this is what US law required.)

   Now this code can be freely exported and imported.  See README.CR.
 */
#define __ZIPCLOAK_C

#ifndef UTIL
# define UTIL
#endif
#include "zip.h"
#define DEFCPYRT        /* main module: enable copyright string defines! */
#include "revision.h"
#include "crc32.h"
#include "crypt.h"
#include "ttyio.h"
#include <signal.h>
#ifndef NO_STDLIB_H
#  include <stdlib.h>
#endif

#if CRYPT       /* defined (as TRUE or FALSE) in crypt.h */

int main OF((int argc, char **argv));

local void handler OF((int sig));
local void license OF((void));
local void help OF((void));
local void version_info OF((void));

/* Temporary zip file pointer */
local FILE *tempzf;

/* Pointer to CRC-32 table (used for decryption/encryption) */
#if (!defined(USE_ZLIB) || defined(USE_OWN_CRCTAB))
ZCONST ulg near *crc_32_tab;
#else
ZCONST uLongf *crc_32_tab;
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
   If nl true, print and add new line. */
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
/* Print a message to mesg and flush.  Write new line first
   if current line has output already. */
{
  if (noisy) {
    if (mesg_line_started)
      fprintf(mesg, "\n");
    fprintf(mesg, "%s%s\n", a, b);
    mesg_line_started = 0;
    fflush(mesg);
  }
}

/***********************************************************************
 * Issue a message for the error, clean up files and memory, and exit.
 */
void ziperr(code, msg)
    int code;               /* error code from the ZE_ class */
    ZCONST char *msg;       /* message about how it happened */
{
    if (PERR(code)) perror("zipcloak error");
    fprintf(mesg, "zipcloak error: %s (%s)\n", ZIPERRORS(code), msg);
    if (tempzf != NULL) fclose(tempzf);
    if (tempzip != NULL) {
        destroy(tempzip);
        free((zvoid *)tempzip);
    }
    if (zipfile != NULL) free((zvoid *)zipfile);
    EXIT(code);
}

/***********************************************************************
 * Print a warning message to mesg (usually stderr) and return.
 */
void zipwarn(msg1, msg2)
    ZCONST char *msg1, *msg2;   /* message strings juxtaposed in output */
{
    fprintf(mesg, "zipcloak warning: %s%s\n", msg1, msg2);
}


/***********************************************************************
 * Upon getting a user interrupt, turn echo back on for tty and abort
 * cleanly using ziperr().
 */
local void handler(sig)
    int sig;                  /* signal number (ignored) */
{
#if (!defined(MSDOS) && !defined(__human68k__) && !defined(RISCOS))
    echon();
    putc('\n', mesg);
#endif
    ziperr(ZE_ABORT +sig-sig, "aborting");
    /* dummy usage of sig to avoid compiler warnings */
}


static ZCONST char *public[] = {
"The encryption code of this program is not copyrighted and is",
"put in the public domain. It was originally written in Europe",
"and can be freely distributed in both source and object forms",
"from any country, including the USA under License Exception",
"TSU of the U.S. Export Administration Regulations (section",
"740.13(e)) of 6 June 2002.  (Prior to January 2000, re-export",
"from the US was a violation of US law.)"
};

/***********************************************************************
 * Print license information to stdout.
 */
local void license()
{
    extent i;             /* counter for copyright array */

    for (i = 0; i < sizeof(swlicense)/sizeof(char *); i++) {
        puts(swlicense[i]);
    }
    putchar('\n');
    printf("Export notice:\n");
    for (i = 0; i < sizeof(public)/sizeof(char *); i++) {
        puts(public[i]);
    }
}


static ZCONST char *help_info[] = {
"",
"ZipCloak %s (%s)",
#ifdef VM_CMS
"Usage:  zipcloak [-dq] [-b fm] zipfile",
#else
"Usage:  zipcloak [-dq] [-b path] zipfile",
#endif
"  the default action is to encrypt all unencrypted entries in the zip file",
"",
"  -d  --decrypt      decrypt encrypted entries (copy if given wrong password)",
#ifdef VM_CMS
"  -b  --temp-mode    use \"fm\" as the filemode for the temporary zip file",
#else
"  -b  --temp-path    use \"path\" for the temporary zip file",
#endif
"  -O  --output-file  write output to new zip file",
"  -q  --quiet        quiet operation, suppress some informational messages",
"  -h  --help         show this help",
"  -v  --version      show version info",
"  -L  --license      show software license"
  };

/***********************************************************************
 * Print help (along with license info) to stdout.
 */
local void help()
{
    extent i;             /* counter for help array */

    for (i = 0; i < sizeof(help_info)/sizeof(char *); i++) {
        printf(help_info[i], VERSION, REVDATE);
        putchar('\n');
    }
}


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
#if CRYPT && defined(PASSWD_FROM_STDIN)
    "PASSWD_FROM_STDIN",
#endif /* CRYPT && PASSWD_FROM_STDIN */
    NULL
  };

  for (i = 0; i < sizeof(copyright)/sizeof(char *); i++)
  {
    printf(copyright[i], "zipcloak");
    putchar('\n');
  }
  putchar('\n');

  for (i = 0; i < sizeof(versinfolines)/sizeof(char *); i++)
  {
    printf(versinfolines[i], "ZipCloak", VERSION, REVDATE);
    putchar('\n');
  }

  version_local();

  puts("ZipCloak special compilation options:");
  for (i = 0; (int)i < (int)(sizeof(comp_opts)/sizeof(char *) - 1); i++)
  {
    printf("\t%s\n",comp_opts[i]);
  }
  printf("\t[encryption, version %d.%d%s of %s]\n",
            CR_MAJORVER, CR_MINORVER, CR_BETA_VER, CR_VERSION_DATE);
}

/* options for zipcloak - 3/5/2004 EG */
struct option_struct far options[] = {
  /* short longopt        value_type        negatable        ID    name */
#ifdef VM_CMS
    {"b",  "temp-mode",   o_REQUIRED_VALUE, o_NOT_NEGATABLE, 'b',  "temp file mode"},
#else
    {"b",  "temp-path",   o_REQUIRED_VALUE, o_NOT_NEGATABLE, 'b',  "path for temp file"},
#endif
    {"d",  "decrypt",     o_NO_VALUE,       o_NOT_NEGATABLE, 'd',  "decrypt"},
    {"h",  "help",        o_NO_VALUE,       o_NOT_NEGATABLE, 'h',  "help"},
    {"L",  "license",     o_NO_VALUE,       o_NOT_NEGATABLE, 'L',  "license"},
    {"l",  "",            o_NO_VALUE,       o_NOT_NEGATABLE, 'L',  "license"},
    {"O",  "output-file", o_REQUIRED_VALUE, o_NOT_NEGATABLE, 'O',  "output to new archive"},
    {"v",  "version",     o_NO_VALUE,       o_NOT_NEGATABLE, 'v',  "version"},
    /* the end of the list */
    {NULL, NULL,          o_NO_VALUE,       o_NOT_NEGATABLE, 0,    NULL} /* end has option_ID = 0 */
  };


/***********************************************************************
 * Encrypt or decrypt all of the entries in a zip file.  See the command
 * help in help() above.
 */

int main(argc, argv)
    int argc;                   /* number of tokens in command line */
    char **argv;                /* command line tokens */
{
    int attr;                   /* attributes of zip file */
    zoff_t start_offset;        /* start of central directory */
    int decrypt;                /* decryption flag */
    int temp_path;              /* 1 if next argument is path for temp files */
    char passwd[IZ_PWLEN+1];    /* password for encryption or decryption */
    char verify[IZ_PWLEN+1];    /* password for encryption or decryption */
#if 0
    char *q;                    /* steps through option arguments */
    int r;                      /* arg counter */
#endif
    int res;                    /* result code */
    zoff_t length;              /* length of central directory */
    FILE *inzip, *outzip;       /* input and output zip files */
    struct zlist far *z;        /* steps through zfiles linked list */
    /* used by get_option */
    unsigned long option; /* option ID returned by get_option */
    int argcnt = 0;       /* current argcnt in args */
    int argnum = 0;       /* arg number */
    int optchar = 0;      /* option state */
    char *value = NULL;   /* non-option arg, option value or NULL */
    int negated = 0;      /* 1 = option negated */
    int fna = 0;          /* current first non-opt arg */
    int optnum = 0;       /* index in table */

    char **args;               /* copy of argv that can be freed */

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
    if (argc == 1) {
        help();
        EXIT(ZE_OK);
    }

    /* Informational messages are written to stdout. */
    mesg = stdout;

    init_upper();               /* build case map table */

    crc_32_tab = get_crc_table();
                                /* initialize crc table for crypt */

    /* Go through args */
    zipfile = tempzip = NULL;
    tempzf = NULL;
#ifdef SIGINT
    signal(SIGINT, handler);
#endif
#ifdef SIGTERM                  /* Some don't have SIGTERM */
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
    temp_path = decrypt = 0;
#if 0
    /* old command line */
    for (r = 1; r < argc; r++) {
        if (*argv[r] == '-') {
            if (!argv[r][1]) ziperr(ZE_PARMS, "zip file cannot be stdin");
            for (q = argv[r]+1; *q; q++) {
                switch (*q) {
                case 'b':   /* Specify path for temporary file */
                    if (temp_path) {
                        ziperr(ZE_PARMS, "use -b before zip file name");
                    }
                    temp_path = 1;          /* Next non-option is path */
                    break;
                case 'd':
                    decrypt = 1;  break;
                case 'h':   /* Show help */
                    help();
                    EXIT(ZE_OK);
                case 'l': case 'L':  /* Show copyright and disclaimer */
                    license();
                    EXIT(ZE_OK);
                case 'q':   /* Quiet operation, suppress info messages */
                    noisy = 0;  break;
                case 'v':   /* Show version info */
                    version_info();
                    EXIT(ZE_OK);
                default:
                    ziperr(ZE_PARMS, "unknown option");
                } /* switch */
            } /* for */

        } else if (temp_path == 0) {
            if (zipfile != NULL) {
                ziperr(ZE_PARMS, "can only specify one zip file");

            } else if ((zipfile = ziptyp(argv[r])) == NULL) {
                ziperr(ZE_MEM, "was processing arguments");
            }
        } else {
            tempath = argv[r];
            temp_path = 0;
        } /* if */
    } /* for */

#else

    /* new command line */

    zipfile = NULL;
    out_path = NULL;

    /* make copy of args that can use with insert_arg() */
    args = copy_args(argv, 0);

    /*
    -------------------------------------------
    Process command line using get_option
    -------------------------------------------

    Each call to get_option() returns either a command
    line option and possible value or a non-option argument.
    Arguments are permuted so that all options (-r, -b temp)
    are returned before non-option arguments (zipfile).
    Returns 0 when nothing left to read.
    */

    /* set argnum = 0 on first call to init get_option */
    argnum = 0;

    /* get_option returns the option ID and updates parameters:
           args    - usually same as argv if no argument file support
           argcnt  - current argc for args
           value   - char* to value (free() when done with it) or NULL if no value
           negated - option was negated with trailing -
    */

    while ((option = get_option(&args, &argcnt, &argnum,
                                &optchar, &value, &negated,
                                &fna, &optnum, 0)))
    {
      switch (option)
      {
        case 'b':   /* Specify path for temporary file */
          if (temp_path) {
            ziperr(ZE_PARMS, "more than one temp_path");
          }
          temp_path = 1;
          tempath = value;
          break;
        case 'd':
          decrypt = 1;  break;
        case 'h':   /* Show help */
          help();
          EXIT(ZE_OK);
        case 'l': case 'L':  /* Show copyright and disclaimer */
          license();
          EXIT(ZE_OK);
        case 'O':   /* Output to new zip file instead of updating original zip file */
          if ((out_path = ziptyp(value)) == NULL) {
            ziperr(ZE_MEM, "was processing arguments");
          }
          free(value);
          break;
        case 'q':   /* Quiet operation, suppress info messages */
          noisy = 0;  break;
        case 'v':   /* Show version info */
          version_info();
          EXIT(ZE_OK);
        case o_NON_OPTION_ARG:
          /* not an option */
          /* no more options as permuting */
          /* just dash also ends up here */

          if (strcmp(value, "-") == 0) {
            ziperr(ZE_PARMS, "zip file cannot be stdin");
          } else if (zipfile != NULL) {
            ziperr(ZE_PARMS, "can only specify one zip file");
          }

          if ((zipfile = ziptyp(value)) == NULL) {
            ziperr(ZE_MEM, "was processing arguments");
          }
          free(value);
          break;

        default:
          ziperr(ZE_PARMS, "unknown option");
      }
    }

    free_args(args);

#endif

    if (zipfile == NULL) ziperr(ZE_PARMS, "need to specify zip file");

    /* in_path is the input zip file */
    if ((in_path = malloc(strlen(zipfile) + 1)) == NULL) {
      ziperr(ZE_MEM, "input");
    }
    strcpy(in_path, zipfile);

    /* out_path defaults to in_path */
    if (out_path == NULL) {
      if ((out_path = malloc(strlen(zipfile) + 1)) == NULL) {
        ziperr(ZE_MEM, "output");
      }
      strcpy(out_path, zipfile);
    }

    /* Read zip file */
    if ((res = readzipfile()) != ZE_OK) ziperr(res, zipfile);
    if (zfiles == NULL) ziperr(ZE_NAME, zipfile);

    /* Check for something to do */
    for (z = zfiles; z != NULL; z = z->nxt) {
        if (decrypt ? z->flg & 1 : !(z->flg & 1)) break;
    }
    if (z == NULL) {
        ziperr(ZE_NONE, decrypt ? "no encrypted files"
                       : "all files encrypted already");
    }

    /* Before we get carried away, make sure zip file is writeable */
    if ((inzip = fopen(zipfile, "a")) == NULL) ziperr(ZE_CREAT, zipfile);
    fclose(inzip);
    attr = getfileattr(zipfile);

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
      if ((y = tempzf = outzip = fdopen(yd, FOPW_TMP)) == NULL) {
        ZIPERR(ZE_TEMP, tempzip);
      }
    }
#else
    if ((y = tempzf = outzip = fopen(tempzip = tempname(zipfile), FOPW)) == NULL) {
        ziperr(ZE_TEMP, tempzip);
    }
#endif

    /* Get password */
    if (getp("Enter password: ", passwd, IZ_PWLEN+1) == NULL)
        ziperr(ZE_PARMS,
               "stderr is not a tty (you may never see this message!)");

    if (decrypt == 0) {
        if (getp("Verify password: ", verify, IZ_PWLEN+1) == NULL)
               ziperr(ZE_PARMS,
                      "stderr is not a tty (you may never see this message!)");

        if (strcmp(passwd, verify))
               ziperr(ZE_PARMS, "password verification failed");

        if (*passwd == '\0')
               ziperr(ZE_PARMS, "zero length password not allowed");
    }

    /* Open input zip file again, copy preamble if any */
    if ((in_file = fopen(zipfile, FOPR)) == NULL) ziperr(ZE_NAME, zipfile);

    if (zipbeg && (res = bfcopy(zipbeg)) != ZE_OK)
    {
        ziperr(res, res == ZE_TEMP ? tempzip : zipfile);
    }
    tempzn = zipbeg;

    /* Go through local entries, copying, encrypting, or decrypting */
    for (z = zfiles; z != NULL; z = z->nxt) {
        if (decrypt && (z->flg & 1)) {
            printf("decrypting: %s", z->zname);
            fflush(stdout);
            if ((res = zipbare(z, passwd)) != ZE_OK)
            {
                if (res != ZE_MISS) ziperr(res, "was decrypting an entry");
                printf(" (wrong password--just copying)");
                fflush(stdout);
            }
            putchar('\n');

        } else if ((!decrypt) && !(z->flg & 1)) {
            printf("encrypting: %s\n", z->zname);
            fflush(stdout);
            if ((res = zipcloak(z, passwd)) != ZE_OK)
            {
                ziperr(res, "was encrypting an entry");
            }
        } else {
            printf("   copying: %s\n", z->zname);
            fflush(stdout);
            if ((res = zipcopy(z)) != ZE_OK)
            {
                ziperr(res, "was copying an entry");
            }
        } /* if */
    } /* for */

    fclose(in_file);


    /* Write central directory and end of central directory */

    /* get start of central */
    if ((start_offset = zftello(outzip)) == (zoff_t)-1)
        ziperr(ZE_TEMP, tempzip);

    for (z = zfiles; z != NULL; z = z->nxt) {
        if ((res = putcentral(z)) != ZE_OK) ziperr(res, tempzip);
    }

    /* get end of central */
    if ((length = zftello(outzip)) == (zoff_t)-1)
        ziperr(ZE_TEMP, tempzip);

    length -= start_offset;               /* compute length of central */
    if ((res = putend((zoff_t)zcount, length, start_offset, zcomlen,
                      zcomment)) != ZE_OK) {
        ziperr(res, tempzip);
    }
    tempzf = NULL;
    if (fclose(outzip)) ziperr(ZE_TEMP, tempzip);
    if ((res = replace(out_path, tempzip)) != ZE_OK) {
        zipwarn("new zip file left as: ", tempzip);
        free((zvoid *)tempzip);
        tempzip = NULL;
        ziperr(res, "was replacing the original zip file");
    }
    free((zvoid *)tempzip);
    tempzip = NULL;
    setfileattr(zipfile, attr);
#ifdef RISCOS
    /* Set the filetype of the zipfile to &DDC */
    setfiletype(zipfile, 0xDDC);
#endif
    free((zvoid *)in_path);
    free((zvoid *)out_path);

    free((zvoid *)zipfile);
    zipfile = NULL;

    /* Done! */
    RETURN(0);
}
#else /* !CRYPT */


/* below is only used if crypt is not enabled */

struct option_struct far options[] = {
  /* short longopt        value_type        negatable        ID    name */
    {"h",  "help",        o_NO_VALUE,       o_NOT_NEGATABLE, 'h',  "help"},
    /* the end of the list */
    {NULL, NULL,          o_NO_VALUE,       o_NOT_NEGATABLE, 0,    NULL} /* end has option_ID = 0 */
  };


int main OF((void));

void zipwarn(msg1, msg2)
ZCONST char  *msg1, *msg2;
{
    /* Tell picky compilers to shut up about unused variables */
    msg1 = msg1; msg2 = msg2;
}

void ziperr(c, h)
int  c;
ZCONST char *h;
{
    /* Tell picky compilers to shut up about unused variables */
    c = c; h = h;
}

int main()
{
    fprintf(mesg, "\
This version of ZipCloak does not support encryption.  Get the current Zip\n\
source distribution and recompile ZipCloak after you have added an option to\n\
define the symbol USE_CRYPT to the C compiler's command arguments.\n");
    RETURN(1);
}

#endif /* ?CRYPT */
