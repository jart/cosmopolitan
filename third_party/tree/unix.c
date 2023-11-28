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

extern FILE *outfile;
extern bool dflag, Fflag, duflag, metafirst, hflag, siflag, noindent;
extern bool colorize, linktargetcolor;
extern const struct linedraw *linedraw;
extern int *dirs;

static char info[512] = {0};

int unix_printinfo(char *dirname, struct _info *file, int level)
{
  fillinfo(info, file);
  if (metafirst) {
    if (info[0] == '[') fprintf(outfile, "%s  ",info);
    if (!noindent) indent(level);
  } else {
    if (!noindent) indent(level);
    if (info[0] == '[') fprintf(outfile, "%s  ",info);
  }
  return 0;
}

int unix_printfile(char *dirname, char *filename, struct _info *file, int descend)
{
  int colored = FALSE, c;

  if (file && colorize) {
    if (file->lnk && linktargetcolor) colored = color(file->lnkmode,file->name,file->orphan,FALSE);
    else colored = color(file->mode,file->name,file->orphan,FALSE);
  }

  printit(filename);

  if (colored) endcolor();

  if (file) {
    if (Fflag && !file->lnk) {
      if ((c = Ftype(file->mode))) fputc(c, outfile);
    }

    if (file->lnk) {
      fprintf(outfile," -> ");
      if (colorize) colored = color(file->lnkmode,file->lnk,file->orphan,TRUE);
      printit(file->lnk);
      if (colored) endcolor();
      if (Fflag) {
	if ((c = Ftype(file->lnkmode))) fputc(c, outfile);
      }
    }
  }
  return 0;
}

int unix_error(char *error)
{
  fprintf(outfile, "  [%s]", error);
  return 0;
}

void unix_newline(struct _info *file, int level, int postdir, int needcomma)
{
  if (postdir <= 0) fprintf(outfile, "\n");
  if (file && file->comment) {
    int infosize = 0, line, lines;
    if (metafirst) infosize = info[0] == '['? strlen(info)+2 : 0;

    for(lines = 0; file->comment[lines]; lines++);
    dirs[level+1] = 1;
    for(line = 0; line < lines; line++) {
      if (metafirst) {
	printf("%*s", infosize, "");
      }
      indent(level);
      printcomment(line, lines, file->comment[line]);
    }
    dirs[level+1] = 0;
  }
}

void unix_report(struct totals tot)
{
  char buf[256];

  fputc('\n', outfile);
  if (duflag) {
    psize(buf, tot.size);
    fprintf(outfile,"%s%s used in ", buf, hflag || siflag? "" : " bytes");
  }
  if (dflag)
    fprintf(outfile,"%ld director%s\n",tot.dirs,(tot.dirs==1? "y":"ies"));
  else
    fprintf(outfile,"%ld director%s, %ld file%s\n",tot.dirs,(tot.dirs==1? "y":"ies"),tot.files,(tot.files==1? "":"s"));
}
