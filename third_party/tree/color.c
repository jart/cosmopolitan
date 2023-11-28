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
#include "libc/runtime/runtime.h"
#include "libc/calls/calls.h"
#include "libc/mem/mem.h"
#include "libc/sysv/consts/s.h"
#include "libc/intrin/kprintf.h"
#include "third_party/tree/tree.h"

/*
 * Hacked in DIR_COLORS support for linux. ------------------------------
 */
/*
 *  If someone asked me, I'd extend dircolors, to provide more generic
 * color support so that more programs could take advantage of it.  This
 * is really just hacked in support.  The dircolors program should:
 * 1) Put the valid terms in a environment var, like:
 *    COLOR_TERMS=linux:console:xterm:vt100...
 * 2) Put the COLOR and OPTIONS directives in a env var too.
 * 3) Have an option to dircolors to silently ignore directives that it
 *    doesn't understand (directives that other programs would
 *    understand).
 * 4) Perhaps even make those unknown directives environment variables.
 *
 * The environment is the place for cryptic crap no one looks at, but
 * programs.  No one is going to care if it takes 30 variables to do
 * something.
 */
enum {
  ERROR = -1, CMD_COLOR = 0, CMD_OPTIONS, CMD_TERM, CMD_EIGHTBIT, COL_RESET,
  COL_NORMAL, COL_FILE, COL_DIR, COL_LINK, COL_FIFO, COL_DOOR, COL_BLK, COL_CHR,
  COL_ORPHAN, COL_SOCK, COL_SETUID, COL_SETGID, COL_STICKY_OTHER_WRITABLE,
  COL_OTHER_WRITABLE, COL_STICKY, COL_EXEC, COL_MISSING,
  COL_LEFTCODE, COL_RIGHTCODE, COL_ENDCODE,
/* Keep this one last, sets the size of the color_code array: */
  DOT_EXTENSION
};

enum {
  MCOL_INODE, MCOL_PERMS, MCOL_USER, MCOL_GROUP, MCOL_SIZE, MCOL_DATE,
  MCOL_INDENTLINES
};

bool colorize = FALSE, ansilines = FALSE, linktargetcolor = FALSE;
char *term, termmatch = FALSE, istty;

char *color_code[DOT_EXTENSION+1] = {NULL};

char *vgacolor[] = {
  "black", "red", "green", "yellow", "blue", "fuchsia", "aqua", "white",
  NULL, NULL,
  "transparent", "red", "green", "yellow", "blue", "fuchsia", "aqua", "black"
};

struct colortable colortable[11];
struct extensions *ext = NULL;
const struct linedraw *linedraw;

char **split(char *str, char *delim, int *nwrds);
int cmd(char *s);

extern FILE *outfile;
extern bool Hflag, force_color, nocolor;
extern const char *charset;

void parse_dir_colors()
{
  char buf[1025], **arg, **c, *colors, *s;
  int i, n, col, cc;
  struct extensions *e;

  if (Hflag) return;

  s = getenv("NO_COLOR");
  if (s && s[0]) nocolor = TRUE;

  if (getenv("TERM") == NULL) {
    colorize = FALSE;
    return;
  }

  // [jart] we're biased in favor of color
  cc = true; // getenv("CLICOLOR") != NULL;
  if (getenv("CLICOLOR_FORCE") != NULL && !nocolor) force_color=TRUE;
  s = getenv("TREE_COLORS");
  if (s == NULL) s = getenv("LS_COLORS");
  if ((s == NULL || strlen(s) == 0) && (force_color || cc)) s = ":no=00:rs=0:fi=00:di=01;34:ln=01;36:pi=40;33:so=01;35:bd=40;33;01:cd=40;33;01:or=40;31;01:ex=01;32:*.bat=01;32:*.BAT=01;32:*.btm=01;32:*.BTM=01;32:*.cmd=01;32:*.CMD=01;32:*.com=01;32:*.COM=01;32:*.dll=01;32:*.DLL=01;32:*.exe=01;32:*.EXE=01;32:*.arj=01;31:*.bz2=01;31:*.deb=01;31:*.gz=01;31:*.lzh=01;31:*.rpm=01;31:*.tar=01;31:*.taz=01;31:*.tb2=01;31:*.tbz2=01;31:*.tbz=01;31:*.tgz=01;31:*.tz2=01;31:*.z=01;31:*.Z=01;31:*.zip=01;31:*.ZIP=01;31:*.zoo=01;31:*.asf=01;35:*.ASF=01;35:*.avi=01;35:*.AVI=01;35:*.bmp=01;35:*.BMP=01;35:*.flac=01;35:*.FLAC=01;35:*.gif=01;35:*.GIF=01;35:*.jpg=01;35:*.JPG=01;35:*.jpeg=01;35:*.JPEG=01;35:*.m2a=01;35:*.M2a=01;35:*.m2v=01;35:*.M2V=01;35:*.mov=01;35:*.MOV=01;35:*.mp3=01;35:*.MP3=01;35:*.mpeg=01;35:*.MPEG=01;35:*.mpg=01;35:*.MPG=01;35:*.ogg=01;35:*.OGG=01;35:*.ppm=01;35:*.rm=01;35:*.RM=01;35:*.tga=01;35:*.TGA=01;35:*.tif=01;35:*.TIF=01;35:*.wav=01;35:*.WAV=01;35:*.wmv=01;35:*.WMV=01;35:*.xbm=01;35:*.xpm=01;35:";

  if (s == NULL || (!force_color && (nocolor || !isatty(1)))) {
    colorize = FALSE;
    return;
  }

  colorize = TRUE;

  for(i=0; i < DOT_EXTENSION; i++) color_code[i] = NULL;

  colors = scopy(s);

  arg = split(colors,":",&n);

  for(i=0;arg[i];i++) {
    c = split(arg[i],"=",&n);

    switch(col = cmd(c[0])) {
      case ERROR:
	break;
      case DOT_EXTENSION:
	if (c[1]) {
	  e = xmalloc(sizeof(struct extensions));
	  e->ext = scopy(c[0]+1);
	  e->term_flg = scopy(c[1]);
	  e->nxt = ext;
	  ext = e;
	}
	break;
      case COL_LINK:
	if (c[1] && strcasecmp("target",c[1]) == 0) {
	  linktargetcolor = TRUE;
	  color_code[COL_LINK] = "01;36"; /* Should never actually be used */
	  break;
	}
      default:
	if (c[1]) color_code[col] = scopy(c[1]);
	break;
    }

    free(c);
  }
  free(arg);

  /**
   * Make sure at least reset (not normal) is defined.  We're going to assume
   * ANSI/vt100 support:
   */
  if (!color_code[COL_LEFTCODE]) color_code[COL_LEFTCODE] = scopy("\033[");
  if (!color_code[COL_RIGHTCODE]) color_code[COL_RIGHTCODE] = scopy("m");
  if (!color_code[COL_RESET]) color_code[COL_RESET] = scopy("0");
  if (!color_code[COL_ENDCODE]) {
    sprintf(buf,"%s%s%s",color_code[COL_LEFTCODE],color_code[COL_RESET],color_code[COL_RIGHTCODE]);
    color_code[COL_ENDCODE] = scopy(buf);
  }

  free(colors);
}

/*
 * You must free the pointer that is allocated by split() after you
 * are done using the array.
 */
char **split(char *str, char *delim, int *nwrds)
{
  int n = 128;
  char **w = xmalloc(sizeof(char *) * n);

  w[*nwrds = 0] = strtok(str,delim);

  while (w[*nwrds]) {
    if (*nwrds == (n-2)) w = xrealloc(w,sizeof(char *) * (n+=256));
    w[++(*nwrds)] = strtok(NULL,delim);
  }

  w[*nwrds] = NULL;
  return w;
}

int cmd(char *s)
{
  static struct {
    char *cmd;
    char cmdnum;
  } cmds[] = {
    {"rs", COL_RESET}, {"no", COL_NORMAL}, {"fi", COL_FILE}, {"di", COL_DIR},
    {"ln", COL_LINK}, {"pi", COL_FIFO}, {"do", COL_DOOR}, {"bd", COL_BLK},
    {"cd", COL_CHR}, {"or", COL_ORPHAN}, {"so", COL_SOCK}, {"su", COL_SETUID},
    {"sg", COL_SETGID}, {"tw", COL_STICKY_OTHER_WRITABLE},
    {"ow", COL_OTHER_WRITABLE}, {"st", COL_STICKY}, {"ex", COL_EXEC},
    {"mi", COL_MISSING}, {"lc", COL_LEFTCODE}, {"rc", COL_RIGHTCODE},
    {"ec", COL_ENDCODE}, {NULL, 0}
  };
  int i;

  if (s == NULL) return ERROR;  /* Probably can't happen */

  if (s[0] == '*') return DOT_EXTENSION;
  for(i=0;cmds[i].cmdnum;i++) {
    if (!strcmp(cmds[i].cmd,s)) return cmds[i].cmdnum;
  }
  return ERROR;
}

int print_color(int color)
{
  if (!color_code[color]) return FALSE;

  fputs(color_code[COL_LEFTCODE],outfile);
  fputs(color_code[color],outfile);
  fputs(color_code[COL_RIGHTCODE],outfile);
  return TRUE;
}

void endcolor(void)
{
  if (color_code[COL_ENDCODE])
    fputs(color_code[COL_ENDCODE],outfile);
}

int color(u_short mode, char *name, bool orphan, bool islink)
{
  struct extensions *e;
  int l, xl;

  if (orphan) {
    if (islink) {
      if (print_color(COL_MISSING)) return TRUE;
    } else {
      if (print_color(COL_ORPHAN)) return TRUE;
    }
  }

  /* It's probably safe to assume short-circuit evaluation, but we'll do it this way: */
  switch(mode & S_IFMT) {
    case S_IFIFO:
      return print_color(COL_FIFO);
    case S_IFCHR:
      return print_color(COL_CHR);
    case S_IFDIR:
      if (mode & S_ISVTX) {
	if ((mode & S_IWOTH))
	  if (print_color(COL_STICKY_OTHER_WRITABLE)) return TRUE;
	if (!(mode & S_IWOTH))
	  if (print_color(COL_STICKY)) return TRUE;
      }
      if ((mode & S_IWOTH))
	if (print_color(COL_OTHER_WRITABLE)) return TRUE;
      return print_color(COL_DIR);
#ifndef __EMX__
    case S_IFBLK:
      return print_color(COL_BLK);
    case S_IFLNK:
      return print_color(COL_LINK);
  #ifdef S_IFDOOR
    case S_IFDOOR:
      return print_color(COL_DOOR);
  #endif
#endif
    case S_IFSOCK:
      return print_color(COL_SOCK);
    case S_IFREG:
      if ((mode & S_ISUID))
	if (print_color(COL_SETUID)) return TRUE;
      if ((mode & S_ISGID))
	if (print_color(COL_SETGID)) return TRUE;
      if (mode & (S_IXUSR | S_IXGRP | S_IXOTH))
	if (print_color(COL_EXEC)) return TRUE;

      /* not a directory, link, special device, etc, so check for extension match */
      l = strlen(name);
      for(e=ext;e;e=e->nxt) {
	xl = strlen(e->ext);
	if (!strcmp((l>xl)?name+(l-xl):name,e->ext)) {
	  fputs(color_code[COL_LEFTCODE], outfile);
	  fputs(e->term_flg, outfile);
	  fputs(color_code[COL_RIGHTCODE], outfile);
	  return TRUE;
	}
      }
      /* colorize just normal files too */
      return print_color(COL_FILE);
  }
  return print_color(COL_NORMAL);
}

/*
 * Charsets provided by Kyosuke Tokoro (NBG01720@nifty.ne.jp)
 */
const char *getcharset(void)
{
  char *cs;
  static char buffer[256];

  cs = getenv("TREE_CHARSET");
  if (cs) return strncpy(buffer,cs,255);

#ifndef __EMX__
  return NULL;
#else
  ULONG aulCpList[3],ulListSize,codepage=0;

  if(!getenv("WINDOWID"))
    if(!DosQueryCp(sizeof aulCpList,aulCpList,&ulListSize))
      if(ulListSize>=sizeof*aulCpList)
	codepage=*aulCpList;

  switch(codepage) {
    case 437: case 775: case 850: case 851: case 852: case 855:
    case 857: case 860: case 861: case 862: case 863: case 864:
    case 865: case 866: case 868: case 869: case 891: case 903:
    case 904:
      sprintf(buffer,"IBM%03lu",codepage);
      break;
    case 367:
      return"US-ASCII";
    case 813:
      return"ISO-8859-7";
    case 819:
      return"ISO-8859-1";
    case 881: case 882: case 883: case 884: case 885:
      sprintf(buffer,"ISO-8859-%lu",codepage-880);
      break;
    case  858: case  924:
      sprintf(buffer,"IBM%05lu",codepage);
      break;
    case 874:
      return"TIS-620";
    case 897: case 932: case 942: case 943:
      return"Shift_JIS";
    case 912:
      return"ISO-8859-2";
    case 915:
      return"ISO-8859-5";
    case 916:
      return"ISO-8859-8";
    case 949: case 970:
      return"EUC-KR";
    case 950:
      return"Big5";
    case 954:
      return"EUC-JP";
    case 1051:
      return"hp-roman8";
    case 1089:
      return"ISO-8859-6";
    case 1250: case 1251: case 1253: case 1254: case 1255: case 1256:
    case 1257: case 1258:
      sprintf(buffer,"windows-%lu",codepage);
      break;
    case 1252:
      return"ISO-8859-1-Windows-3.1-Latin-1";
    default:
      return NULL;
  }
  return buffer;
#endif
}

void initlinedraw(int flag)
{
  static const char*latin1_3[]={
    "ISO-8859-1", "ISO-8859-1:1987", "ISO_8859-1", "latin1", "l1", "IBM819",
    "CP819", "csISOLatin1", "ISO-8859-3", "ISO_8859-3:1988", "ISO_8859-3",
    "latin3", "ls", "csISOLatin3", NULL
  };
  static const char*iso8859_789[]={
    "ISO-8859-7", "ISO_8859-7:1987", "ISO_8859-7", "ELOT_928", "ECMA-118",
    "greek", "greek8", "csISOLatinGreek", "ISO-8859-8", "ISO_8859-8:1988",
    "iso-ir-138", "ISO_8859-8", "hebrew", "csISOLatinHebrew", "ISO-8859-9",
    "ISO_8859-9:1989", "iso-ir-148", "ISO_8859-9", "latin5", "l5",
    "csISOLatin5", NULL
  };
  static const char*shift_jis[]={
    "Shift_JIS", "MS_Kanji", "csShiftJIS", NULL
  };
  static const char*euc_jp[]={
    "EUC-JP", "Extended_UNIX_Code_Packed_Format_for_Japanese",
    "csEUCPkdFmtJapanese", NULL
  };
  static const char*euc_kr[]={
    "EUC-KR", "csEUCKR", NULL
  };
  static const char*iso2022jp[]={
    "ISO-2022-JP", "csISO2022JP", "ISO-2022-JP-2", "csISO2022JP2", NULL
  };
  static const char*ibm_pc[]={
    "IBM437", "cp437", "437", "csPC8CodePage437", "IBM852", "cp852", "852",
    "csPCp852", "IBM863", "cp863", "863", "csIBM863", "IBM855", "cp855",
    "855", "csIBM855", "IBM865", "cp865", "865", "csIBM865", "IBM866",
    "cp866", "866", "csIBM866", NULL
  };
  static const char*ibm_ps2[]={
    "IBM850", "cp850", "850", "csPC850Multilingual", "IBM00858", "CCSID00858",
    "CP00858", "PC-Multilingual-850+euro", NULL
  };
  static const char*ibm_gr[]={
    "IBM869", "cp869", "869", "cp-gr", "csIBM869", NULL
  };
  static const char*gb[]={
    "GB2312", "csGB2312", NULL
  };
  static const char*utf8[]={
    "UTF-8", "utf8", NULL
  };
  static const char*big5[]={
    "Big5", "csBig5", NULL
  };
  static const char*viscii[]={
    "VISCII", "csVISCII", NULL
  };
  static const char*koi8ru[]={
    "KOI8-R", "csKOI8R", "KOI8-U", NULL
  };
  static const char*windows[]={
    "ISO-8859-1-Windows-3.1-Latin-1", "csWindows31Latin1",
    "ISO-8859-2-Windows-Latin-2", "csWindows31Latin2", "windows-1250",
    "windows-1251", "windows-1253", "windows-1254", "windows-1255",
    "windows-1256", "windows-1256", "windows-1257", NULL
  };
  
  static const struct linedraw cstable[]={
    { latin1_3,    "|  ",              "|--",            "&middot;--",     "&copy;",
      " [",        " [",               " [",             " [",             " ["       },
    { iso8859_789, "|  ",              "|--",            "&middot;--",     "(c)",
      " [",        " [",               " [",             " [",             " ["       },
    { shift_jis,   "\204\240 ",        "\204\245",       "\204\244",       "(c)",
      " [",        " [",               " [",             " [",             " ["       },
    { euc_jp,      "\250\242 ",        "\250\247",       "\250\246",       "(c)",
      " [",        " [",               " [",             " [",             " ["       },
    { euc_kr,      "\246\242 ",        "\246\247",       "\246\246",       "(c)",
      " [",        " [",               " [",             " [",             " ["       },
    { iso2022jp,   "\033$B(\"\033(B ", "\033$B('\033(B", "\033$B(&\033(B", "(c)",
      " [",        " [",               " [",             " [",             " ["       },
    { ibm_pc,      "\263  ",           "\303\304\304",   "\300\304\304",   "(c)",
      " [",        " [",               " [",             " [",             " ["       },
    { ibm_ps2,     "\263  ",           "\303\304\304",   "\300\304\304",   "\227",
      " [",        " [",               " [",             " [",             " ["       },
    { ibm_gr,      "\263  ",           "\303\304\304",   "\300\304\304",   "\270",
      " [",        " [",               " [",             " [",             " ["       },
    { gb,          "\251\246 ",        "\251\300",       "\251\270",       "(c)",
      " [",        " [",               " [",             " [",             " ["       },
    { utf8,        "\342\224\202\302\240\302\240", "\342\224\234\342\224\200\342\224\200",
      "\342\224\224\342\224\200\342\224\200", "\302\251",
      " \342\216\247", " \342\216\251", " \342\216\250", " \342\216\252",  " {"       },
    { big5,        "\242x ",           "\242u",          "\242|",          "(c)",
      " [",        " [",               " [",             " [",             " ["       },
    { viscii,      "|  ",              "|--",            "`--",            "\371",
      " [",        " [",               " [",             " [",             " ["       },
    { koi8ru,      "\201  ",           "\206\200\200",   "\204\200\200",   "\277",
      " [",        " [",               " [",             " [",             " ["       },
    { windows,     "|  ",              "|--",            "`--",            "\251",
      " [",        " [",               " [",             " [",             " ["       },
    { NULL,        "|  ",              "|--",            "`--",            "(c)",
      " [",        " [",               " [",             " [",             " ["       },
  };
  const char**s;

  if (flag) {
    fprintf(stderr,"tree: missing argument to --charset, valid charsets include:\n");
    for(linedraw=cstable;linedraw->name;++linedraw) {
      for(s=linedraw->name;*s;++s) {
	fprintf(stderr,"  %s\n",*s);
      }
    }
    return;
  }
  if (charset) {
    for(linedraw=cstable;linedraw->name;++linedraw)
      for(s=linedraw->name;*s;++s)
	if(!strcasecmp(charset,*s)) return;
  }
  linedraw=cstable+sizeof cstable/sizeof*cstable-1;
}
