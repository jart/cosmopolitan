/* $Copyright: $
 * Copyright (c) 1996 - 2023 by Steve Baker (ice@mama.indstate.edu)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "libc/limits.h"
#include "libc/sysv/consts/s.h"
#include "libc/runtime/runtime.h"
#include "libc/str/locale.h"
#include "libc/str/locale.h"
#include "libc/str/langinfo.h"
#include "libc/mem/mem.h"
#include "libc/calls/calls.h"
#include "libc/sysv/consts/f.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/stat.macros.h"
#include "libc/calls/struct/dirent.h"
#include "libc/mem/alg.h"
#include "third_party/tree/tree.h"

char *version = "$Version: $ tree v2.1.1 %s 1996 - 2023 by Steve Baker, Thomas Moore, Francesc Rocher, Florian Sesser, Kyosuke Tokoro $";
char *hversion= "\t\t tree v2.1.1 %s 1996 - 2023 by Steve Baker and Thomas Moore <br>\n"
		"\t\t HTML output hacked and copyleft %s 1998 by Francesc Rocher <br>\n"
		"\t\t JSON output hacked and copyleft %s 2014 by Florian Sesser <br>\n"
		"\t\t Charsets / OS/2 support %s 2001 by Kyosuke Tokoro\n";

/* Globals */
bool dflag, lflag, pflag, sflag, Fflag, aflag, fflag, uflag, gflag;
bool qflag, Nflag, Qflag, Dflag, inodeflag, devflag, hflag, Rflag;
bool Hflag, siflag, cflag, Xflag, Jflag, duflag, pruneflag;
bool noindent, force_color, nocolor, xdev, noreport, nolinks;
bool ignorecase, matchdirs, fromfile, metafirst, gitignore, showinfo;
bool reverse, fflinks;
int flimit;

struct listingcalls lc;

int pattern = 0, maxpattern = 0, ipattern = 0, maxipattern = 0;
char **patterns = NULL, **ipatterns = NULL;

char *host = NULL, *title = "Directory Tree", *sp = " ", *_nl = "\n";
char *Hintro = NULL, *Houtro = NULL;
char *file_comment = "#", *file_pathsep = "/";
char *timefmt = NULL;
const char *charset = NULL;

struct _info **(*getfulltree)(char *d, u_long lev, dev_t dev, off_t *size, char **err) = unix_getfulltree;
/* off_t (*listdir)(char *, int *, int *, u_long, dev_t) = unix_listdir; */
int (*basesort)() = alnumsort;
int (*topsort)() = NULL;

char *sLevel, *curdir;
FILE *outfile = NULL;
int Level, *dirs, maxdirs;
int errors;

char xpattern[PATH_MAX];

int mb_cur_max;

#ifdef __EMX__
const u_short ifmt[]={ FILE_ARCHIVED, FILE_DIRECTORY, FILE_SYSTEM, FILE_HIDDEN, FILE_READONLY, 0};
#else
  #ifdef S_IFPORT
  const u_int ifmt[] = {S_IFREG, S_IFDIR, S_IFLNK, S_IFCHR, S_IFBLK, S_IFSOCK, S_IFIFO, S_IFDOOR, S_IFPORT, 0};
  const char fmt[] = "-dlcbspDP?";
  const char *ftype[] = {"file", "directory", "link", "char", "block", "socket", "fifo", "door", "port", "unknown", NULL};
  #else
  const u_int ifmt[] = {S_IFREG, S_IFDIR, S_IFLNK, S_IFCHR, S_IFBLK, S_IFSOCK, S_IFIFO, 0};
  const char fmt[] = "-dlcbsp?";
  const char *ftype[] = {"file", "directory", "link", "char", "block", "socket", "fifo", "unknown", NULL};
  #endif
#endif

struct sorts {
  char *name;
  int (*cmpfunc)();
} sorts[] = {
  {"name", alnumsort},
  {"version", versort},
  {"size", fsizesort},
  {"mtime", mtimesort},
  {"ctime", ctimesort},
  {NULL, NULL}
};

/* Externs */
/* hash.c */
extern struct xtable *gtable[256], *utable[256];
extern struct inotable *itable[256];

/* color.c */
extern bool colorize, ansilines, linktargetcolor;
extern char *leftcode, *rightcode, *endcode;
extern const struct linedraw *linedraw;

/* Time to switch to getopt()? */
char *long_arg(char *argv[], int i, int *j, int *n, char *prefix) {
  char *ret = NULL;
  int len = strlen(prefix);

  if (!strncmp(prefix,argv[i],len)) {
    *j = len;
    if (*(argv[i]+(*j)) == '=') {
      if (*(argv[i]+ (++(*j)))) {
	ret=(argv[i] + (*j));
	*j = strlen(argv[i])-1;
      } else {
	fprintf(stderr,"tree: Missing argument to %s=\n", prefix);
	exit(1);
      }
    } else if (argv[*n] != NULL) {
      ret = argv[*n];
      (*n)++;
      *j = strlen(argv[i])-1;
    } else {
      fprintf(stderr,"tree: Missing argument to %s\n", prefix);
      exit(1);
    }
  }
  return ret;
}

int main(int argc, char **argv)
{
  struct ignorefile *ig;
  struct infofile *inf;
  char **dirname = NULL;
  int i, j=0, k, n, optf, p = 0, q = 0;
  char *stmp, *outfilename = NULL, *arg;
  char *stddata_fd;
  bool needfulltree, showversion = FALSE;

  aflag = dflag = fflag = lflag = pflag = sflag = Fflag = uflag = gflag = FALSE;
  Dflag = qflag = Nflag = Qflag = Rflag = hflag = Hflag = siflag = cflag = FALSE;
  noindent = force_color = nocolor = xdev = noreport = nolinks = reverse = FALSE;
  ignorecase = matchdirs = inodeflag = devflag = Xflag = Jflag = fflinks = FALSE;
  duflag = pruneflag = metafirst = gitignore = FALSE;

  flimit = 0;
  dirs = xmalloc(sizeof(int) * (maxdirs=PATH_MAX));
  memset(dirs, 0, sizeof(int) * maxdirs);
  dirs[0] = 0;
  Level = -1;

  setlocale(LC_CTYPE, "");
  setlocale(LC_COLLATE, "");

  charset = getcharset();
  if (charset == NULL && 
       (strcmp(nl_langinfo(CODESET), "UTF-8") == 0 ||
        strcmp(nl_langinfo(CODESET), "utf8") == 0)) {
    charset = "UTF-8";
  }

  lc = (struct listingcalls){
    null_intro, null_outtro, unix_printinfo, unix_printfile, unix_error, unix_newline,
    null_close, unix_report
  };

/* Still a hack, but assume that if the macro is defined, we can use it: */
#ifdef MB_CUR_MAX
  mb_cur_max = (int)MB_CUR_MAX;
#else
  mb_cur_max = 1;
#endif

  /* Output JSON automatically to "stddata" if present: */
  stddata_fd = getenv(ENV_STDDATA_FD);
  if (stddata_fd != NULL) {
    int std_fd = atoi(stddata_fd);
    if (std_fd <= 0) std_fd = STDDATA_FILENO;
    if (fcntl(std_fd, F_GETFD) >= 0) {
      Jflag = noindent = TRUE;
      _nl = "";
      lc = (struct listingcalls){
	json_intro, json_outtro, json_printinfo, json_printfile, json_error, json_newline,
	json_close, json_report
      };
      outfile = fdopen(std_fd, "w");
    }
  }

  memset(utable,0,sizeof(utable));
  memset(gtable,0,sizeof(gtable));
  memset(itable,0,sizeof(itable));

  optf = TRUE;
  for(n=i=1;i<argc;i=n) {
    n++;
    if (optf && argv[i][0] == '-' && argv[i][1]) {
      for(j=1;argv[i][j];j++) {
	switch(argv[i][j]) {
	case 'N':
	  Nflag = TRUE;
	  break;
	case 'q':
	  qflag = TRUE;
	  break;
	case 'Q':
	  Qflag = TRUE;
	  break;
	case 'd':
	  dflag = TRUE;
	  break;
	case 'l':
	  lflag = TRUE;
	  break;
	case 's':
	  sflag = TRUE;
	  break;
	case 'h':
	  hflag = TRUE;
	  sflag = TRUE; /* Assume they also want -s */
	  break;
	case 'u':
	  uflag = TRUE;
	  break;
	case 'g':
	  gflag = TRUE;
	  break;
	case 'f':
	  fflag = TRUE;
	  break;
	case 'F':
	  Fflag = TRUE;
	  break;
	case 'a':
	  aflag = TRUE;
	  break;
	case 'p':
	  pflag = TRUE;
	  break;
	case 'i':
	  noindent = TRUE;
	  _nl = "";
	  break;
	case 'C':
	  force_color = TRUE;
	  break;
	case 'n':
	  nocolor = TRUE;
	  break;
	case 'x':
	  xdev = TRUE;
	  break;
	case 'P':
	  if (argv[n] == NULL) {
	    fprintf(stderr,"tree: Missing argument to -P option.\n");
	    exit(1);
	  }
	  if (pattern >= maxpattern-1) patterns = xrealloc(patterns, sizeof(char *) * (maxpattern += 10));
	  patterns[pattern++] = argv[n++];
	  patterns[pattern] = NULL;
	  break;
	case 'I':
	  if (argv[n] == NULL) {
	    fprintf(stderr,"tree: Missing argument to -I option.\n");
	    exit(1);
	  }
	  if (ipattern >= maxipattern-1) ipatterns = xrealloc(ipatterns, sizeof(char *) * (maxipattern += 10));
	  ipatterns[ipattern++] = argv[n++];
	  ipatterns[ipattern] = NULL;
	  break;
	case 'A':
	  ansilines = TRUE;
	  break;
	case 'S':
	  charset = "IBM437";
	  break;
	case 'D':
	  Dflag = TRUE;
	  break;
	case 't':
	  basesort = mtimesort;
	  break;
	case 'c':
	  basesort = ctimesort;
	  cflag = TRUE;
	  break;
	case 'r':
	  reverse = TRUE;
	  break;
	case 'v':
	  basesort = versort;
	  break;
	case 'U':
	  basesort = NULL;
	  break;
	case 'X':
	  Xflag = TRUE;
	  Hflag = Jflag = FALSE;
	  lc = (struct listingcalls){
	    xml_intro, xml_outtro, xml_printinfo, xml_printfile, xml_error, xml_newline,
	    xml_close, xml_report
	  };
	  break;
	case 'J':
	  Jflag = TRUE;
	  Xflag = Hflag = FALSE;
	  lc = (struct listingcalls){
	    json_intro, json_outtro, json_printinfo, json_printfile, json_error, json_newline,
	    json_close, json_report
	  };
	  break;
	case 'H':
	  Hflag = TRUE;
	  Xflag = Jflag = FALSE;
	  lc = (struct listingcalls){
	    html_intro, html_outtro, html_printinfo, html_printfile, html_error, html_newline,
	    html_close, html_report
	  };
	  if (argv[n] == NULL) {
	    fprintf(stderr,"tree: Missing argument to -H option.\n");
	    exit(1);
	  }
	  host = argv[n++];
	  k = strlen(host)-1;
	  /* Allows a / if that is the only character as the 'host': */
	  if (k && host[k] == '/') host[k] = '\0';
	  sp = "&nbsp;";
	  break;
	case 'T':
	  if (argv[n] == NULL) {
	    fprintf(stderr,"tree: Missing argument to -T option.\n");
	    exit(1);
	  }
	  title = argv[n++];
	  break;
	case 'R':
	  Rflag = TRUE;
	  break;
	case 'L':
	  if ((sLevel = argv[n++]) == NULL) {
	    fprintf(stderr,"tree: Missing argument to -L option.\n");
	    exit(1);
	  }
	  Level = strtoul(sLevel,NULL,0)-1;
	  if (Level < 0) {
	    fprintf(stderr,"tree: Invalid level, must be greater than 0.\n");
	    exit(1);
	  }
	  break;
	case 'o':
	  if (argv[n] == NULL) {
	    fprintf(stderr,"tree: Missing argument to -o option.\n");
	    exit(1);
	  }
	  outfilename = argv[n++];
	  break;
	case '-':
	  if (j == 1) {
	    if (!strcmp("--", argv[i])) {
	      optf = FALSE;
	      break;
	    }
	    /* Long options that don't take parameters should just use strcmp: */
	    if (!strcmp("--help",argv[i])) {
	      usage(2);
	      exit(0);
	    }
	    if (!strcmp("--version",argv[i])) {
	      j = strlen(argv[i])-1;
	      showversion = TRUE;
	      break;
	    }
	    if (!strcmp("--inodes",argv[i])) {
	      j = strlen(argv[i])-1;
	      inodeflag=TRUE;
	      break;
	    }
	    if (!strcmp("--device",argv[i])) {
	      j = strlen(argv[i])-1;
	      devflag=TRUE;
	      break;
	    }
	    if (!strcmp("--noreport",argv[i])) {
	      j = strlen(argv[i])-1;
	      noreport = TRUE;
	      break;
	    }
	    if (!strcmp("--nolinks",argv[i])) {
	      j = strlen(argv[i])-1;
	      nolinks = TRUE;
	      break;
	    }
	    if (!strcmp("--dirsfirst",argv[i])) {
	      j = strlen(argv[i])-1;
	      topsort = dirsfirst;
	      break;
	    }
	    if (!strcmp("--filesfirst",argv[i])) {
	      j = strlen(argv[i])-1;
	      topsort = filesfirst;
	      break;
	    }
	    if ((arg = long_arg(argv, i, &j, &n, "--filelimit")) != NULL) {
	      flimit = atoi(arg);
	      break;
	    }
	    if ((arg = long_arg(argv, i, &j, &n, "--charset")) != NULL) {
	      charset = arg;
	      break;
	    }
	    if (!strcmp("--si", argv[i])) {
	      j = strlen(argv[i])-1;
	      sflag = TRUE;
	      hflag = TRUE;
	      siflag = TRUE;
	      break;
	    }
	    if (!strcmp("--du",argv[i])) {
	      j = strlen(argv[i])-1;
	      sflag = TRUE;
	      duflag = TRUE;
	      break;
	    }
	    if (!strcmp("--prune",argv[i])) {
	      j = strlen(argv[i])-1;
	      pruneflag = TRUE;
	      break;
	    }
	    if ((arg = long_arg(argv, i, &j, &n, "--timefmt")) != NULL) {
	      timefmt = scopy(arg);
	      Dflag = TRUE;
	      break;
	    }
	    if (!strcmp("--ignore-case",argv[i])) {
	      j = strlen(argv[i])-1;
	      ignorecase = TRUE;
	      break;
	    }
	    if (!strcmp("--matchdirs",argv[i])) {
	      j = strlen(argv[i])-1;
	      matchdirs = TRUE;
	      break;
	    }
	    if ((arg = long_arg(argv, i, &j, &n, "--sort")) != NULL) {
	      basesort = NULL;
	      for(k=0;sorts[k].name;k++) {
		if (strcasecmp(sorts[k].name,arg) == 0) {
		  basesort = sorts[k].cmpfunc;
		  break;
		}
	      }
	      if (basesort == NULL) {
		fprintf(stderr,"tree: Sort type '%s' not valid, should be one of: ", arg);
		for(k=0; sorts[k].name; k++)
		  printf("%s%c", sorts[k].name, sorts[k+1].name? ',': '\n');
		exit(1);
	      }
	      break;
	    }
	    if (!strcmp("--fromtabfile", argv[i])) {
	      j = strlen(argv[i])-1;
	      fromfile=TRUE;
	      getfulltree = tabedfile_getfulltree;
	      break;
	    }
	    if (!strcmp("--fromfile",argv[i])) {
	      j = strlen(argv[i])-1;
	      fromfile=TRUE;
	      getfulltree = file_getfulltree;
	      break;
	    }
	    if (!strcmp("--metafirst",argv[i])) {
	      j = strlen(argv[i])-1;
	      metafirst=TRUE;
	      break;
	    }
	    if ((arg = long_arg(argv, i, &j, &n, "--gitfile")) != NULL) {
	      gitignore=TRUE;
	      ig = new_ignorefile(arg, FALSE);
	      if (ig != NULL) push_filterstack(ig);
	      else {
		fprintf(stderr,"tree: Could not load gitignore file\n");
		exit(1);
	      }
	      break;
	    }
	    if (!strcmp("--gitignore",argv[i])) {
	      j = strlen(argv[i])-1;
	      gitignore=TRUE;
	      break;
	    }
	    if (!strcmp("--info",argv[i])) {
	      j = strlen(argv[i])-1;
	      showinfo=TRUE;
	      break;
	    }
	    if ((arg = long_arg(argv, i, &j, &n, "--infofile")) != NULL) {
	      showinfo = TRUE;
	      inf = new_infofile(arg, FALSE);
	      if (inf != NULL) push_infostack(inf);
	      else {
		fprintf(stderr,"tree: Could not load infofile\n");
		exit(1);
	      }
	      break;
	    }
	    if ((arg = long_arg(argv, i, &j, &n, "--hintro")) != NULL) {
	      Hintro = scopy(arg);
	      break;
	    }
	    if ((arg = long_arg(argv, i, &j, &n, "--houtro")) != NULL) {
	      Houtro = scopy(arg);
	      break;
	    }
	    if (!strcmp("--fflinks",argv[i])) {
	      j = strlen(argv[i])-1;
	      fflinks=TRUE;
	      break;
	    }

	    fprintf(stderr,"tree: Invalid argument `%s'.\n",argv[i]);
	    usage(1);
	    exit(1);
	  }
	default:
	  /* printf("here i = %d, n = %d\n", i, n); */
	  fprintf(stderr,"tree: Invalid argument -`%c'.\n",argv[i][j]);
	  usage(1);
	  exit(1);
	  break;
	}
      }
    } else {
      if (!dirname) dirname = (char **)xmalloc(sizeof(char *) * (q=MINIT));
      else if (p == (q-2)) dirname = (char **)xrealloc(dirname,sizeof(char *) * (q+=MINC));
      dirname[p++] = scopy(argv[i]);
    }
  }
  if (p) dirname[p] = NULL;

  setoutput(outfilename);

  parse_dir_colors();
  initlinedraw(0);
  
  if (showversion) {
    print_version(TRUE);
    exit(0);
  }

  /* Insure sensible defaults and sanity check options: */
  if (dirname == NULL) {
    dirname = xmalloc(sizeof(char *) * 2);
    dirname[0] = scopy(".");
    dirname[1] = NULL;
  }
  if (topsort == NULL) topsort = basesort;
  if (timefmt) setlocale(LC_TIME,"");
  if (dflag) pruneflag = FALSE;  /* You'll just get nothing otherwise. */
  if (Rflag && (Level == -1)) Rflag = FALSE;

  /* Not going to implement git configs so no core.excludesFile support. */
  if (gitignore && (stmp = getenv("GIT_DIR"))) {
    char *path = xmalloc(PATH_MAX);
    snprintf(path, PATH_MAX, "%s/info/exclude", stmp);
    push_filterstack(new_ignorefile(path, FALSE));
    free(path);
  }
  if (showinfo) {
    push_infostack(new_infofile(INFO_PATH, FALSE));
  }

  needfulltree = duflag || pruneflag || matchdirs || fromfile;

  emit_tree(dirname, needfulltree);

  if (outfilename != NULL) fclose(outfile);

  return errors ? 2 : 0;
}

void print_version(int nl)
{
  char buf[PATH_MAX], *v;
  v = version+12;
  sprintf(buf, "%.*s%s", (int)strlen(v)-2, v, nl?"\n":"");
  fprintf(outfile, buf, linedraw->copy);
}

void setoutput(char *filename)
{
  if (filename == NULL) {
#ifdef __EMX__
    _fsetmode(outfile=stdout,Hflag?"b":"t");
#else
    if (outfile == NULL) outfile = stdout;
#endif
  } else {
#ifdef __EMX__
    outfile = fopen(filename, Hflag? "wb":"wt");
#else
    outfile = fopen(filename, "w");
#endif
    if (outfile == NULL) {
      fprintf(stderr,"tree: invalid filename '%s'\n", filename);
      exit(1);
    }
  }
}

void usage(int n)
{
  /*     123456789!123456789!123456789!123456789!123456789!123456789!123456789!123456789! */
  /*     \t9!123456789!123456789!123456789!123456789!123456789!123456789!123456789! */
  fprintf(n < 2? stderr: stdout,
	"usage: tree [-acdfghilnpqrstuvxACDFJQNSUX] [-L level [-R]] [-H  baseHREF]\n"
	"\t[-T title] [-o filename] [-P pattern] [-I pattern] [--gitignore]\n"
	"\t[--gitfile[=]file] [--matchdirs] [--metafirst] [--ignore-case]\n"
	"\t[--nolinks] [--hintro[=]file] [--houtro[=]file] [--inodes] [--device]\n"
	"\t[--sort[=]<name>] [--dirsfirst] [--filesfirst] [--filelimit #] [--si]\n"
	"\t[--du] [--prune] [--charset[=]X] [--timefmt[=]format] [--fromfile]\n"
	"\t[--fromtabfile] [--fflinks] [--info] [--infofile[=]file] [--noreport]\n"
	"\t[--version] [--help] [--] [directory ...]\n");

  if (n < 2) return;
  fprintf(stdout,
	"  ------- Listing options -------\n"
	"  -a            All files are listed.\n"
	"  -d            List directories only.\n"
	"  -l            Follow symbolic links like directories.\n"
	"  -f            Print the full path prefix for each file.\n"
	"  -x            Stay on current filesystem only.\n"
	"  -L level      Descend only level directories deep.\n"
	"  -R            Rerun tree when max dir level reached.\n"
	"  -P pattern    List only those files that match the pattern given.\n"
	"  -I pattern    Do not list files that match the given pattern.\n"
	"  --gitignore   Filter by using .gitignore files.\n"
	"  --gitfile X   Explicitly read gitignore file.\n"
	"  --ignore-case Ignore case when pattern matching.\n"
	"  --matchdirs   Include directory names in -P pattern matching.\n"
	"  --metafirst   Print meta-data at the beginning of each line.\n"
	"  --prune       Prune empty directories from the output.\n"
	"  --info        Print information about files found in .info files.\n"
	"  --infofile X  Explicitly read info file.\n"
	"  --noreport    Turn off file/directory count at end of tree listing.\n"
	"  --charset X   Use charset X for terminal/HTML and indentation line output.\n"
	"  --filelimit # Do not descend dirs with more than # files in them.\n"
	"  -o filename   Output to file instead of stdout.\n"
	"  ------- File options -------\n"
	"  -q            Print non-printable characters as '?'.\n"
	"  -N            Print non-printable characters as is.\n"
	"  -Q            Quote filenames with double quotes.\n"
	"  -p            Print the protections for each file.\n"
	"  -u            Displays file owner or UID number.\n"
	"  -g            Displays file group owner or GID number.\n"
	"  -s            Print the size in bytes of each file.\n"
	"  -h            Print the size in a more human readable way.\n"
	"  --si          Like -h, but use in SI units (powers of 1000).\n"
	"  --du          Compute size of directories by their contents.\n"
	"  -D            Print the date of last modification or (-c) status change.\n"
	"  --timefmt <f> Print and format time according to the format <f>.\n"
	"  -F            Appends '/', '=', '*', '@', '|' or '>' as per ls -F.\n"
	"  --inodes      Print inode number of each file.\n"
	"  --device      Print device ID number to which each file belongs.\n"
	"  ------- Sorting options -------\n"
	"  -v            Sort files alphanumerically by version.\n"
	"  -t            Sort files by last modification time.\n"
	"  -c            Sort files by last status change time.\n"
	"  -U            Leave files unsorted.\n"
	"  -r            Reverse the order of the sort.\n"
	"  --dirsfirst   List directories before files (-U disables).\n"
	"  --filesfirst  List files before directories (-U disables).\n"
	"  --sort X      Select sort: name,version,size,mtime,ctime.\n"
	"  ------- Graphics options -------\n"
	"  -i            Don't print indentation lines.\n"
	"  -A            Print ANSI lines graphic indentation lines.\n"
	"  -S            Print with CP437 (console) graphics indentation lines.\n"
	"  -n            Turn colorization off always (-C overrides).\n"
	"  -C            Turn colorization on always.\n"
	"  ------- XML/HTML/JSON options -------\n"
	"  -X            Prints out an XML representation of the tree.\n"
	"  -J            Prints out an JSON representation of the tree.\n"
	"  -H baseHREF   Prints out HTML format with baseHREF as top directory.\n"
	"  -T string     Replace the default HTML title and H1 header with string.\n"
	"  --nolinks     Turn off hyperlinks in HTML output.\n"
	"  --hintro X    Use file X as the HTML intro.\n"
	"  --houtro X    Use file X as the HTML outro.\n"
	"  ------- Input options -------\n"
	"  --fromfile    Reads paths from files (.=stdin)\n"
	"  --fromtabfile Reads trees from tab indented files (.=stdin)\n"
	"  --fflinks     Process link information when using --fromfile.\n"
	"  ------- Miscellaneous options -------\n"
	"  --version     Print version and exit.\n"
	"  --help        Print usage and this help message and exit.\n"
	"  --            Options processing terminator.\n");
  exit(0);
}

/**
 * True if file matches an -I pattern
 */
int patignore(char *name, int isdir)
{
  int i;
  for(i=0; i < ipattern; i++)
    if (patmatch(name, ipatterns[i], isdir)) return 1;
  return 0;
}

/**
 * True if name matches a -P pattern
 */
int patinclude(char *name, int isdir)
{
  int i;
  for(i=0; i < pattern; i++) {
    if (patmatch(name, patterns[i], isdir)) {
      return 1;
    }
  }
  return 0;
}

/**
 * Split out stat portion from read_dir as prelude to just using stat structure directly.
 */
struct _info *getinfo(char *name, char *path)
{
  static char *lbuf = NULL;
  static int lbufsize = 0;
  struct _info *ent;
  struct stat st, lst;
  int len, rs, isdir;

  if (lbuf == NULL) lbuf = xmalloc(lbufsize = PATH_MAX);

  if (lstat(path,&lst) < 0) return NULL;

  if ((lst.st_mode & S_IFMT) == S_IFLNK) {
    if ((rs = stat(path,&st)) < 0) memset(&st, 0, sizeof(st));
  } else {
    rs = 0;
    st.st_mode = lst.st_mode;
    st.st_dev = lst.st_dev;
    st.st_ino = lst.st_ino;
  }

  isdir = (st.st_mode & S_IFMT) == S_IFDIR;

#ifndef __EMX__
  if (gitignore && filtercheck(path, name, isdir)) return NULL;

  if ((lst.st_mode & S_IFMT) != S_IFDIR && !(lflag && ((st.st_mode & S_IFMT) == S_IFDIR))) {
    if (pattern && !patinclude(name, isdir)) return NULL;
  }
  if (ipattern && patignore(name, isdir)) return NULL;
#endif

  if (dflag && ((st.st_mode & S_IFMT) != S_IFDIR)) return NULL;

#ifndef __EMX__
/*    if (pattern && ((lst.st_mode & S_IFMT) == S_IFLNK) && !lflag) continue; */
#endif

  ent = (struct _info *)xmalloc(sizeof(struct _info));
  memset(ent, 0, sizeof(struct _info));

  ent->name = scopy(name);
  /* We should just incorporate struct stat into _info, and eliminate this unnecessary copying.
   * Made sense long ago when we had fewer options and didn't need half of stat.
   */
  ent->mode   = lst.st_mode;
  ent->uid    = lst.st_uid;
  ent->gid    = lst.st_gid;
  ent->size   = lst.st_size;
  ent->dev    = st.st_dev;
  ent->inode  = st.st_ino;
  ent->ldev   = lst.st_dev;
  ent->linode = lst.st_ino;
  ent->lnk    = NULL;
  ent->orphan = FALSE;
  ent->err    = NULL;
  ent->child  = NULL;

  ent->atime  = lst.st_atime;
  ent->ctime  = lst.st_ctime;
  ent->mtime  = lst.st_mtime;

#ifdef __EMX__
  ent->attr   = lst.st_attr;
#else

  /* These should be eliminated, as they're barely used: */
  ent->isdir  = isdir;
  ent->issok  = ((st.st_mode & S_IFMT) == S_IFSOCK);
  ent->isfifo = ((st.st_mode & S_IFMT) == S_IFIFO);
  ent->isexe  = (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) ? 1 : 0;

  if ((lst.st_mode & S_IFMT) == S_IFLNK) {
    if (lst.st_size+1 > lbufsize) lbuf = xrealloc(lbuf,lbufsize=(lst.st_size+8192));
    if ((len=readlink(path,lbuf,lbufsize-1)) < 0) {
      ent->lnk = scopy("[Error reading symbolic link information]");
      ent->isdir = FALSE;
      ent->lnkmode = st.st_mode;
    } else {
      lbuf[len] = 0;
      ent->lnk = scopy(lbuf);
      if (rs < 0) ent->orphan = TRUE;
      ent->lnkmode = st.st_mode;
    }
  }
#endif

  ent->comment = NULL;

  return ent;
}

struct _info **read_dir(char *dir, int *n, int infotop)
{
  struct comment *com;
  static char *path = NULL;
  static long pathsize;
  struct _info **dl, *info;
  struct dirent *ent;
  DIR *d;
  int ne, p = 0, i;
  int es = (dir[strlen(dir)-1] == '/');

  if (path == NULL) {
    path=xmalloc(pathsize = strlen(dir)+PATH_MAX);
  }

  *n = -1;
  if ((d=opendir(dir)) == NULL) return NULL;

  dl = (struct _info **)xmalloc(sizeof(struct _info *) * (ne = MINIT));

  while((ent = (struct dirent *)readdir(d))) {
    if (!strcmp("..",ent->d_name) || !strcmp(".",ent->d_name)) continue;
    if (Hflag && !strcmp(ent->d_name,"00Tree.html")) continue;
    if (!aflag && ent->d_name[0] == '.') continue;

    if (strlen(dir)+strlen(ent->d_name)+2 > pathsize) path = xrealloc(path,pathsize=(strlen(dir)+strlen(ent->d_name)+PATH_MAX));
    if (es) sprintf(path, "%s%s", dir, ent->d_name);
    else sprintf(path,"%s/%s",dir,ent->d_name);

    info = getinfo(ent->d_name, path);
    if (info) {
      if (showinfo && (com = infocheck(path, ent->d_name, infotop, info->isdir))) {
	for(i = 0; com->desc[i] != NULL; i++);
	info->comment = xmalloc(sizeof(char *) * (i+1));
	for(i = 0; com->desc[i] != NULL; i++) info->comment[i] = scopy(com->desc[i]);
	info->comment[i] = NULL;
      }
      if (p == (ne-1)) dl = (struct _info **)xrealloc(dl,sizeof(struct _info *) * (ne += MINC));
      dl[p++] = info;
    }
  }
  closedir(d);

  if ((*n = p) == 0) {
    free(dl);
    return NULL;
  }

  dl[p] = NULL;
  return dl;
}

void push_files(char *dir, struct ignorefile **ig, struct infofile **inf, bool top)
{
  if (gitignore) {
    *ig = new_ignorefile(dir, top);
    if (*ig != NULL) push_filterstack(*ig);
  }
  if (showinfo) {
    *inf = new_infofile(dir, top);
    if (*inf != NULL) push_infostack(*inf);
  }
}

/* This is for all the impossible things people wanted the old tree to do.
 * This can and will use a large amount of memory for large directory trees
 * and also take some time.
 */
struct _info **unix_getfulltree(char *d, u_long lev, dev_t dev, off_t *size, char **err)
{
  char *path;
  long pathsize = 0;
  struct ignorefile *ig = NULL;
  struct infofile *inf = NULL;
  struct _info **dir, **sav, **p, *sp;
  struct stat sb;
  int n;
  u_long lev_tmp;
  int tmp_pattern = 0;
  char *start_rel_path;

  *err = NULL;
  if (Level >= 0 && lev > Level) return NULL;
  if (xdev && lev == 0) {
    stat(d,&sb);
    dev = sb.st_dev;
  }
  /* if the directory name matches, turn off pattern matching for contents */
  if (matchdirs && pattern) {
    lev_tmp = lev;
    start_rel_path = d + strlen(d);
    for (start_rel_path = d + strlen(d); start_rel_path != d; --start_rel_path) {
      if (*start_rel_path == '/')
        --lev_tmp;
      if (lev_tmp <= 0) {
        if (*start_rel_path)
          ++start_rel_path;
        break;
      }
    }
    if (*start_rel_path && patinclude(start_rel_path, 1)) {
      tmp_pattern = pattern;
      pattern = 0;
    }
  }

  push_files(d, &ig, &inf, lev==0);

  sav = dir = read_dir(d, &n, inf != NULL);
  if (tmp_pattern) {
    pattern = tmp_pattern;
    tmp_pattern = 0;
  }
  if (dir == NULL && n) {
    *err = scopy("error opening dir");
    return NULL;
  }
  if (n == 0) {
    if (sav != NULL) free_dir(sav);
    return NULL;
  }
  path = xmalloc(pathsize=PATH_MAX);
  
  if (flimit > 0 && n > flimit) {
    sprintf(path,"%d entries exceeds filelimit, not opening dir",n);
    *err = scopy(path);
    free_dir(sav);
    free(path);
    return NULL;
  }

  if (lev >= maxdirs-1) {
    dirs = xrealloc(dirs,sizeof(int) * (maxdirs += 1024));
  }

  while (*dir) {
    if ((*dir)->isdir && !(xdev && dev != (*dir)->dev)) {
      if ((*dir)->lnk) {
	if (lflag) {
	  if (findino((*dir)->inode,(*dir)->dev)) {
	    (*dir)->err = scopy("recursive, not followed");
	  } else {
	    saveino((*dir)->inode, (*dir)->dev);
	    if (*(*dir)->lnk == '/')
	      (*dir)->child = unix_getfulltree((*dir)->lnk,lev+1,dev,&((*dir)->size),&((*dir)->err));
	    else {
	      if (strlen(d)+strlen((*dir)->lnk)+2 > pathsize) path=xrealloc(path,pathsize=(strlen(d)+strlen((*dir)->name)+1024));
	      if (fflag && !strcmp(d,"/")) sprintf(path,"%s%s",d,(*dir)->lnk);
	      else sprintf(path,"%s/%s",d,(*dir)->lnk);
	      (*dir)->child = unix_getfulltree(path,lev+1,dev,&((*dir)->size),&((*dir)->err));
	    }
	  }
	}
      } else {
	if (strlen(d)+strlen((*dir)->name)+2 > pathsize) path=xrealloc(path,pathsize=(strlen(d)+strlen((*dir)->name)+1024));
	if (fflag && !strcmp(d,"/")) sprintf(path,"%s%s",d,(*dir)->name);
	else sprintf(path,"%s/%s",d,(*dir)->name);
	saveino((*dir)->inode, (*dir)->dev);
	(*dir)->child = unix_getfulltree(path,lev+1,dev,&((*dir)->size),&((*dir)->err));
      }
      /* prune empty folders, unless they match the requested pattern */
      if (pruneflag && (*dir)->child == NULL &&
	  !(matchdirs && pattern && patinclude((*dir)->name, (*dir)->isdir))) {
	sp = *dir;
	for(p=dir;*p;p++) *p = *(p+1);
	n--;
	free(sp->name);
	if (sp->lnk) free(sp->lnk);
	free(sp);
	continue;
      }
    }
    if (duflag) *size += (*dir)->size;
    dir++;
  }

  /* sorting needs to be deferred for --du: */
  if (topsort) qsort(sav,n,sizeof(struct _info *),topsort);

  free(path);
  if (n == 0) {
    free_dir(sav);
    return NULL;
  }
  if (ig != NULL) pop_filterstack();
  if (inf != NULL) pop_infostack();
  return sav;
}

/**
 * filesfirst and dirsfirst are now top-level meta-sorts.
 */
int filesfirst(struct _info **a, struct _info **b)
{
  if ((*a)->isdir != (*b)->isdir) {
    return (*a)->isdir ? 1 : -1;
  }
  return basesort(a, b);
}

int dirsfirst(struct _info **a, struct _info **b)
{
  if ((*a)->isdir != (*b)->isdir) {
    return (*a)->isdir ? -1 : 1;
  }
  return basesort(a, b);
}

/* Sorting functions */
int alnumsort(struct _info **a, struct _info **b)
{
  int v = strcoll((*a)->name,(*b)->name);
  return reverse? -v : v;
}

int versort(struct _info **a, struct _info **b)
{
  int v = strverscmp((*a)->name,(*b)->name);
  return reverse? -v : v;
}

int mtimesort(struct _info **a, struct _info **b)
{
  int v;

  if ((*a)->mtime == (*b)->mtime) {
    v = strcoll((*a)->name,(*b)->name);
    return reverse? -v : v;
  }
  v =  (*a)->mtime == (*b)->mtime? 0 : ((*a)->mtime < (*b)->mtime ? -1 : 1);
  return reverse? -v : v;
}

int ctimesort(struct _info **a, struct _info **b)
{
  int v;

  if ((*a)->ctime == (*b)->ctime) {
    v = strcoll((*a)->name,(*b)->name);
    return reverse? -v : v;
  }
  v = (*a)->ctime == (*b)->ctime? 0 : ((*a)->ctime < (*b)->ctime? -1 : 1);
  return reverse? -v : v;
}

int sizecmp(off_t a, off_t b)
{
  return (a == b)? 0 : ((a < b)? 1 : -1);
}

int fsizesort(struct _info **a, struct _info **b)
{
  int v = sizecmp((*a)->size, (*b)->size);
  if (v == 0) v = strcoll((*a)->name,(*b)->name);
  return reverse? -v : v;
}

void *xmalloc (size_t size)
{
  register void *value = malloc (size);
  if (value == NULL) {
    fprintf(stderr,"tree: virtual memory exhausted.\n");
    exit(1);
  }
  return value;
}

void *xrealloc (void *ptr, size_t size)
{
  register void *value = realloc (ptr,size);
  if (value == NULL) {
    fprintf(stderr,"tree: virtual memory exhausted.\n");
    exit(1);
  }
  return value;
}

void free_dir(struct _info **d)
{
  int i;

  for(i=0;d[i];i++) {
    free(d[i]->name);
    if (d[i]->lnk) free(d[i]->lnk);
    free(d[i]);
  }
  free(d);
}

char *gnu_getcwd()
{
  int size = 100;
  char *buffer = (char *) xmalloc (size);

  while (1) {
    char *value = getcwd (buffer, size);
    if (value != 0) return buffer;
    size *= 2;
    free (buffer);
    buffer = (char *) xmalloc (size);
  }
}

static char cond_lower(char c)
{
  return ignorecase ? tolower(c) : c;
}

/*
 * Patmatch() code courtesy of Thomas Moore (dark@mama.indstate.edu)
 * '|' support added by David MacMahon (davidm@astron.Berkeley.EDU)
 * Case insensitive support added by Jason A. Donenfeld (Jason@zx2c4.com)
 * returns:
 *    1 on a match
 *    0 on a mismatch
 *   -1 on a syntax error in the pattern
 */
int patmatch(char *buf, char *pat, int isdir)
{
  int match = 1,m,n;
  char *bar = strchr(pat, '|');
  char pprev = 0;

  /* If a bar is found, call patmatch recursively on the two sub-patterns */
  if (bar) {
    /* If the bar is the first or last character, it's a syntax error */
    if (bar == pat || !bar[1]) {
      return -1;
    }
    /* Break pattern into two sub-patterns */
    *bar = '\0';
    match = patmatch(buf, pat, isdir);
    if (!match) {
      match = patmatch(buf, bar+1, isdir);
    }
    /* Join sub-patterns back into one pattern */
    *bar = '|';
    return match;
  }

  while(*pat && match) {
    switch(*pat) {
    case '[':
      pat++;
      if(*pat != '^') {
	n = 1;
	match = 0;
      } else {
	pat++;
	n = 0;
      }
      while(*pat != ']'){
	if(*pat == '\\') pat++;
	if(!*pat /* || *pat == '/' */ ) return -1;
	if(pat[1] == '-'){
	  m = *pat;
	  pat += 2;
	  if(*pat == '\\' && *pat)
	    pat++;
	  if(cond_lower(*buf) >= cond_lower(m) && cond_lower(*buf) <= cond_lower(*pat))
	    match = n;
	  if(!*pat)
	    pat--;
	} else if(cond_lower(*buf) == cond_lower(*pat)) match = n;
	pat++;
      }
      buf++;
      break;
    case '*':
      pat++;
      if(!*pat) {
	int f = (strchr(buf, '/') == NULL);
        return f;
      }
      match = 0;
      /* "Support" ** for .gitignore support, mostly the same as *: */
      if (*pat == '*') {
	pat++;
	if(!*pat) return 1;

	while(*buf && !(match = patmatch(buf, pat, isdir))) {
	  /* ** between two /'s is allowed to match a null /: */
	  if (pprev == '/' && *pat == '/' && *(pat+1) && (match = patmatch(buf, pat+1, isdir))) return match;
	  buf++;
	  while(*buf && *buf != '/') buf++;
	}
      } else {
	while(*buf && !(match = patmatch(buf++, pat, isdir)))
	  if (*buf == '/') break;
      }
      if (!match && (!*buf || *buf == '/')) match = patmatch(buf, pat, isdir);
      return match;
    case '?':
      if(!*buf) return 0;
      buf++;
      break;
    case '/':
      if (!*(pat+1) && !*buf) return isdir;
      match = (*buf++ == *pat);
      break;
    case '\\':
      if(*pat)
	pat++;
    default:
      match = (cond_lower(*buf++) == cond_lower(*pat));
      break;
    }
    pprev = *pat++;
    if(match<1) return match;
  }
  if(!*buf) return match;
  return 0;
}


/**
 * They cried out for ANSI-lines (not really), but here they are, as an option
 * for the xterm and console capable among you, as a run-time option.
 */
void indent(int maxlevel)
{
  int i;

  if (ansilines) {
    if (dirs[1]) fprintf(outfile,"\033(0");
    for(i=1; (i <= maxlevel) && dirs[i]; i++) {
      if (dirs[i+1]) {
	if (dirs[i] == 1) fprintf(outfile,"\170   ");
	else printf("    ");
      } else {
	if (dirs[i] == 1) fprintf(outfile,"\164\161\161 ");
	else fprintf(outfile,"\155\161\161 ");
      }
    }
    if (dirs[1]) fprintf(outfile,"\033(B");
  } else {
    if (Hflag) fprintf(outfile,"\t");
    for(i=1; (i <= maxlevel) && dirs[i]; i++) {
      fprintf(outfile,"%s ",
	      dirs[i+1] ? (dirs[i]==1 ? linedraw->vert     : (Hflag? "&nbsp;&nbsp;&nbsp;" : "   ") )
			: (dirs[i]==1 ? linedraw->vert_left:linedraw->corner));
    }
  }
}


#ifdef __EMX__
char *prot(long m)
#else
char *prot(mode_t m)
#endif
{
#ifdef __EMX__
  const u_short *p;
  static char buf[6];
  char*cp;

  for(p=ifmt,cp=strcpy(buf,"adshr");*cp;++p,++cp)
    if(!(m&*p))
      *cp='-';
#else
  static char buf[11], perms[] = "rwxrwxrwx";
  int i, b;

  for(i=0;ifmt[i] && (m&S_IFMT) != ifmt[i];i++);
  buf[0] = fmt[i];

  /**
   * Nice, but maybe not so portable, it is should be no less portable than the
   * old code.
   */
  for(b=S_IRUSR,i=0; i<9; b>>=1,i++)
    buf[i+1] = (m & (b)) ? perms[i] : '-';
  if (m & S_ISUID) buf[3] = (buf[3]=='-')? 'S' : 's';
  if (m & S_ISGID) buf[6] = (buf[6]=='-')? 'S' : 's';
  if (m & S_ISVTX) buf[9] = (buf[9]=='-')? 'T' : 't';

  buf[10] = 0;
#endif
  return buf;
}

#define SIXMONTHS (6*31*24*60*60)

char *do_date(time_t t)
{
  static char buf[256];
  struct tm *tm;

  tm = localtime(&t);

  if (timefmt) {
    strftime(buf,255,timefmt,tm);
    buf[255] = 0;
  } else {
    time_t c = time(0);
    /* Use strftime() so that locale is respected: */
    if (t > c || (t+SIXMONTHS) < c)
      strftime(buf,255,"%b %e  %Y",tm);
    else
      strftime(buf,255,"%b %e %R", tm);
  }
  return buf;
}

/**
 * Must fix this someday
 */
void printit(char *s)
{
  int c;

  if (Nflag) {
    if (Qflag) fprintf(outfile, "\"%s\"",s);
    else fprintf(outfile,"%s",s);
    return;
  }
  if (mb_cur_max > 1) {
    wchar_t *ws, *tp;
    ws = xmalloc(sizeof(wchar_t)* (c=(strlen(s)+1)));
    if (mbstowcs(ws,s,c) != (size_t)-1) {
      if (Qflag) putc('"',outfile);
      for(tp=ws;*tp && c > 1;tp++, c--) {
	if (iswprint(*tp)) fprintf(outfile,"%lc",(wint_t)*tp);
	else {
	  if (qflag) putc('?',outfile);
	  else fprintf(outfile,"\\%03o",(unsigned int)*tp);
	}
      }
      if (Qflag) putc('"',outfile);
      free(ws);
      return;
    }
    free(ws);
  }
  if (Qflag) putc('"',outfile);
  for(;*s;s++) {
    c = (unsigned char)*s;
#ifdef __EMX__
    if(_nls_is_dbcs_lead(*(unsigned char*)s)){
      putc(*s,outfile);
      putc(*++s,outfile);
      continue;
    }
#endif
    if((c >= 7 && c <= 13) || c == '\\' || (c == '"' && Qflag) || (c == ' ' && !Qflag)) {
      putc('\\',outfile);
      if (c > 13) putc(c, outfile);
      else putc("abtnvfr"[c-7], outfile);
    } else if (isprint(c)) putc(c,outfile);
    else {
      if (qflag) {
	if (mb_cur_max > 1 && c > 127) putc(c,outfile);
	else putc('?',outfile);
      } else fprintf(outfile,"\\%03o",c);
    }
  }
  if (Qflag) putc('"',outfile);
}

int psize(char *buf, off_t size)
{
  static char *iec_unit="BKMGTPEZY", *si_unit = "dkMGTPEZY";
  char *unit = siflag ? si_unit : iec_unit;
  int idx, usize = siflag ? 1000 : 1024;

  if (hflag || siflag) {
    for (idx=size<usize?0:1; size >= (usize*usize); idx++,size/=usize);
    if (!idx) return sprintf(buf, " %4d", (int)size);
    else return sprintf(buf, ((size/usize) >= 10)? " %3.0f%c" : " %3.1f%c" , (float)size/(float)usize,unit[idx]);
  } else return sprintf(buf, sizeof(off_t) == sizeof(long long)? " %11lld" : " %9lld", (long long int)size);
}

char Ftype(mode_t mode)
{
  int m = mode & S_IFMT;
  if (!dflag && m == S_IFDIR) return '/';
  else if (m == S_IFSOCK) return '=';
  else if (m == S_IFIFO) return '|';
  else if (m == S_IFLNK) return '@'; /* Here, but never actually used though. */
#ifdef S_IFDOOR
  else if (m == S_IFDOOR) return '>';
#endif
  else if ((m == S_IFREG) && (mode & (S_IXUSR | S_IXGRP | S_IXOTH))) return '*';
  return 0;
}

struct _info *stat2info(struct stat *st)
{
  static struct _info info;

  info.linode = st->st_ino;
  info.ldev = st->st_dev;
#ifdef __EMX__
  info.attr = st->st_attr
#endif
  info.mode = st->st_mode;
  info.uid = st->st_uid;
  info.gid = st->st_gid;
  info.size = st->st_size;
  info.atime = st->st_atime;
  info.ctime = st->st_ctime;
  info.mtime = st->st_mtime;

  info.isdir  = ((st->st_mode & S_IFMT) == S_IFDIR);
  info.issok  = ((st->st_mode & S_IFMT) == S_IFSOCK);
  info.isfifo = ((st->st_mode & S_IFMT) == S_IFIFO);
  info.isexe  = (st->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) ? 1 : 0;

  return &info;
}

char *fillinfo(char *buf, struct _info *ent)
{
  int n;
  buf[n=0] = 0;
  #ifdef __USE_FILE_OFFSET64
  if (inodeflag) n += sprintf(buf," %7lld",(long long)ent->linode);
  #else
  if (inodeflag) n += sprintf(buf," %7ld",(long int)ent->linode);
  #endif
  if (devflag) n += sprintf(buf+n, " %3d", (int)ent->ldev);
  #ifdef __EMX__
  if (pflag) n += sprintf(buf+n, " %s",prot(ent->attr));
  #else
  if (pflag) n += sprintf(buf+n, " %s", prot(ent->mode));
  #endif
  if (uflag) n += sprintf(buf+n, " %-8.32s", uidtoname(ent->uid));
  if (gflag) n += sprintf(buf+n, " %-8.32s", gidtoname(ent->gid));
  if (sflag) n += psize(buf+n,ent->size);
  if (Dflag) n += sprintf(buf+n, " %s", do_date(cflag? ent->ctime : ent->mtime));

  if (buf[0] == ' ') {
      buf[0] = '[';
      sprintf(buf+n, "]");
  }

  return buf;
}
