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
#include "third_party/tree/tree.h"

extern char *version, *hversion;
extern bool dflag, lflag, pflag, sflag, Fflag, aflag, fflag, uflag, gflag;
extern bool Dflag, inodeflag, devflag, Rflag, duflag, hflag, siflag;
extern bool noindent, force_color, xdev, nolinks, metafirst, noreport;
extern char *host, *sp, *title, *Hintro, *Houtro;
extern const char *charset;

extern FILE *outfile;
extern int Level, *dirs, maxdirs;

extern bool colorize, linktargetcolor;
extern char *endcode;
extern const struct linedraw *linedraw;

int htmldirlen = 0;

char *class(struct _info *info)
{
  return
    info->isdir  ? "DIR"  :
    info->isexe  ? "EXEC" :
    info->isfifo ? "FIFO" :
    info->issok  ? "SOCK" : "NORM";
}

void html_encode(FILE *fd, char *s)
{
  for(;*s;s++) {
    switch(*s) {
      case '<':
	fputs("&lt;",fd);
	break;
      case '>':
	fputs("&gt;",fd);
	break;
      case '&':
	fputs("&amp;",fd);
	break;
      case '"':
	fputs("&quot;",fd);
	break;
      default:
	fputc(*s,fd);
	break;
    }
  }
}

void url_encode(FILE *fd, char *s)
{
  for(;*s;s++) {
    switch(*s) {
      case ' ':
      case '"':
      case '#':
      case '%':
      case '<':
      case '>':
      case '[':
      case ']':
      case '^':
      case '\\':
      case '?':
      case '+':
	fprintf(fd,"%%%02X",*s);
	break;
      case '&':
	fprintf(fd,"&amp;");
	break;
      default:
	fprintf(fd,isprint((u_int)*s)?"%c":"%%%02X",(u_char)*s);
	break;
    }
  }
}

void fcat(char *filename)
{
  FILE *fp;
  char buf[PATH_MAX];
  int n;

  if ((fp = fopen(filename, "r")) == NULL) return;
  while((n = fread(buf, sizeof(char), PATH_MAX, fp)) > 0) {
    fwrite(buf, sizeof(char), n, outfile);
  }
  fclose(fp);
}

void html_intro(void)
{
  if (Hintro) fcat(Hintro);
  else {
    fprintf(outfile,
	"<!DOCTYPE html>\n"
	"<html>\n"
	"<head>\n"
	" <meta http-equiv=\"Content-Type\" content=\"text/html; charset=%s\">\n"
	" <meta name=\"Author\" content=\"Made by 'tree'\">\n"
	" <meta name=\"GENERATOR\" content=\"", charset ? charset : "iso-8859-1");
    print_version(FALSE);
    fprintf(outfile, "\">\n"
	" <title>%s</title>\n"
	" <style type=\"text/css\">\n"
	"  BODY { font-family : monospace, sans-serif;  color: black;}\n"
	"  P { font-family : monospace, sans-serif; color: black; margin:0px; padding: 0px;}\n"
	"  A:visited { text-decoration : none; margin : 0px; padding : 0px;}\n"
	"  A:link    { text-decoration : none; margin : 0px; padding : 0px;}\n"
	"  A:hover   { text-decoration: underline; background-color : yellow; margin : 0px; padding : 0px;}\n"
	"  A:active  { margin : 0px; padding : 0px;}\n"
	"  .VERSION { font-size: small; font-family : arial, sans-serif; }\n"
	"  .NORM  { color: black;  }\n"
	"  .FIFO  { color: purple; }\n"
	"  .CHAR  { color: yellow; }\n"
	"  .DIR   { color: blue;   }\n"
	"  .BLOCK { color: yellow; }\n"
	"  .LINK  { color: aqua;   }\n"
	"  .SOCK  { color: fuchsia;}\n"
	"  .EXEC  { color: green;  }\n"
	" </style>\n"
	"</head>\n"
	"<body>\n"
	"\t<h1>%s</h1><p>\n", title, title);
  }
}

void html_outtro(void)
{
  if (Houtro) fcat(Houtro);
  else {
    fprintf(outfile,"\t<hr>\n");
    fprintf(outfile,"\t<p class=\"VERSION\">\n");
    fprintf(outfile,hversion,linedraw->copy, linedraw->copy, linedraw->copy, linedraw->copy);
    fprintf(outfile,"\t</p>\n");
    fprintf(outfile,"</body>\n");
    fprintf(outfile,"</html>\n");
  }
}

void html_print(char *s)
{
  int i;
  for(i=0; s[i]; i++) {
    if (s[i] == ' ') fprintf(outfile,"%s",sp);
    else fprintf(outfile,"%c", s[i]);
  }
  fprintf(outfile,"%s%s", sp, sp);
}

int html_printinfo(char *dirname, struct _info *file, int level)
{
  char info[512];

  fillinfo(info,file);
  if (metafirst) {
    if (info[0] == '[') {
      html_print(info);
      fprintf(outfile,"%s%s", sp, sp);
    }
    if (!noindent) indent(level);
  } else {
    if (!noindent) indent(level);
    if (info[0] == '[') {
      html_print(info);
      fprintf(outfile,"%s%s", sp, sp);
    }
  }

  return 0;
}

/* descend == add 00Tree.html to the link */
int html_printfile(char *dirname, char *filename, struct _info *file, int descend)
{
  int i;
  /* Switch to using 'a' elements only. Omit href attribute if not a link */
  fprintf(outfile,"<a");
  if (file) {
    if (force_color) fprintf(outfile," class=\"%s\"", class(file));
    if (file->comment) {
      fprintf(outfile," title=\"");
      for(i=0; file->comment[i]; i++) {
	html_encode(outfile, file->comment[i]);
	if (file->comment[i+1]) fprintf(outfile, "\n");
      }
      fprintf(outfile, "\"");
    }

    if (!nolinks) {
      fprintf(outfile," href=\"%s",host);
      if (dirname != NULL) {
	int len = strlen(dirname);
	int off = (len >= htmldirlen? htmldirlen : 0);
	url_encode(outfile, dirname + off);
	putc('/',outfile);
	url_encode(outfile, filename);
	fprintf(outfile,"%s%s\"",(descend > 1? "/00Tree.html" : ""), (file->isdir?"/":""));
      } else {
	fprintf(outfile,"%s/\"",(descend > 1? "/00Tree.html" : ""));
      }
    }
  }
  fprintf(outfile, ">");

  if (dirname) html_encode(outfile,filename);
  else html_encode(outfile, host);

  fprintf(outfile,"</a>");
  return 0;
}

int html_error(char *error)
{
  fprintf(outfile, "  [%s]", error);
  return 0;
}

void html_newline(struct _info *file, int level, int postdir, int needcomma)
{
  fprintf(outfile, "<br>\n");
}

void html_close(struct _info *file, int level, int needcomma)
{
  fprintf(outfile, "</%s><br>\n", file->tag);
}

void html_report(struct totals tot)
{
  char buf[256];

  fprintf(outfile,"<br><br><p>\n\n");

  if (duflag) {
    psize(buf, tot.size);
    fprintf(outfile,"%s%s used in ", buf, hflag || siflag? "" : " bytes");
  }
  if (dflag)
    fprintf(outfile,"%ld director%s\n",tot.dirs,(tot.dirs==1? "y":"ies"));
  else
    fprintf(outfile,"%ld director%s, %ld file%s\n",tot.dirs,(tot.dirs==1? "y":"ies"),tot.files,(tot.files==1? "":"s"));

  fprintf(outfile, "\n</p>\n");
}
