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
#include "libc/sysv/consts/s.h"
#include "third_party/tree/tree.h"

extern bool dflag, lflag, pflag, sflag, Fflag, aflag, fflag, uflag, gflag;
extern bool Dflag, inodeflag, devflag, Rflag, cflag, duflag, siflag;
extern bool noindent, force_color, xdev, nolinks, noreport;
extern const char *charset;

extern const int ifmt[];
extern const char fmt[], *ftype[];

extern FILE *outfile;
extern int Level, *dirs, maxdirs, errors;

extern char *endcode;

extern struct listingcalls lc;

/*
<tree>
  <directory name="name" mode=0777 size=### user="user" group="group" inode=### dev=### time="00:00 00-00-0000">
    <link name="name" target="name" ...>
      ... if link is followed, otherwise this is empty.
    </link>
    <file name="name" mode=0777 size=### user="user" group="group" inode=### dev=### time="00:00 00-00-0000"></file>
    <socket name="" ...><error>some error</error></socket>
    <block name="" ...></block>
    <char name="" ...></char>
    <fifo name="" ...></fifo>
    <door name="" ...></door>
    <port name="" ...></port>
    ...
  </directory>
  <report>
    <size>#</size>
    <files>#</files>
    <directories>#</directories>
  </report>
</tree>
*/

void xml_indent(int maxlevel)
{
  int i;
  
  fprintf(outfile, "  ");
  for(i=0; i<maxlevel; i++)
    fprintf(outfile, "  ");
}

void xml_fillinfo(struct _info *ent)
{
  #ifdef __USE_FILE_OFFSET64
  if (inodeflag) fprintf(outfile," inode=\"%lld\"",(long long)ent->inode);
  #else
  if (inodeflag) fprintf(outfile," inode=\"%ld\"",(long int)ent->inode);
  #endif
  if (devflag) fprintf(outfile, " dev=\"%d\"", (int)ent->dev);
  #ifdef __EMX__
  if (pflag) fprintf(outfile, " mode=\"%04o\" prot=\"%s\"",ent->attr, prot(ent->attr));
  #else
  if (pflag) fprintf(outfile, " mode=\"%04o\" prot=\"%s\"", ent->mode & (S_IRWXU|S_IRWXG|S_IRWXO|S_ISUID|S_ISGID|S_ISVTX), prot(ent->mode));
  #endif
  if (uflag) fprintf(outfile, " user=\"%s\"", uidtoname(ent->uid));
  if (gflag) fprintf(outfile, " group=\"%s\"", gidtoname(ent->gid));
  if (sflag) fprintf(outfile, " size=\"%lld\"", (long long int)(ent->size));
  if (Dflag) fprintf(outfile, " time=\"%s\"", do_date(cflag? ent->ctime : ent->mtime));
}

void xml_intro(void)
{
  extern char *_nl;

  fprintf(outfile,"<?xml version=\"1.0\"");
  if (charset) fprintf(outfile," encoding=\"%s\"",charset);
  fprintf(outfile,"?>%s<tree>%s",_nl,_nl);
}

void xml_outtro(void)
{
  fprintf(outfile,"</tree>\n");
}

int xml_printinfo(char *dirname, struct _info *file, int level)
{
  mode_t mt;
  int t;

  if (!noindent) xml_indent(level);

  if (file != NULL) {
    if (file->lnk) mt = file->mode & S_IFMT;
    else mt = file->mode & S_IFMT;
  } else mt = 0;

  for(t=0;ifmt[t];t++)
    if (ifmt[t] == mt) break;
  fprintf(outfile,"<%s", (file->tag = ftype[t]));

  return 0;
}

int xml_printfile(char *dirname, char *filename, struct _info *file, int descend)
{
  int i;

  fprintf(outfile, " name=\"");
  html_encode(outfile, filename);
  fputc('"',outfile);

  if (file && file->comment) {
    fprintf(outfile, " info=\"");
    for(i=0; file->comment[i]; i++) {
      html_encode(outfile, file->comment[i]);
      if (file->comment[i+1]) fprintf(outfile, "\n");
    }
    fputc('"', outfile);
  }

  if (file && file->lnk) {
    fprintf(outfile, " target=\"");
    html_encode(outfile,file->lnk);
    fputc('"',outfile);
  }
  if (file) xml_fillinfo(file);
  fputc('>',outfile);

  return 1;
}

int xml_error(char *error)
{
  fprintf(outfile,"<error>%s</error>", error);

  return 0;
}

void xml_newline(struct _info *file, int level, int postdir, int needcomma)
{
  if (postdir >= 0) fprintf(outfile, "\n");
}

void xml_close(struct _info *file, int level, int needcomma)
{
  if (!noindent && level >= 0) xml_indent(level);
  fprintf(outfile,"</%s>%s", file? file->tag : "unknown", noindent? "" : "\n");
}


void xml_report(struct totals tot)
{
  extern char *_nl;

  fprintf(outfile,"%s<report>%s",noindent?"":"  ", _nl);
  if (duflag) fprintf(outfile,"%s<size>%lld</size>%s", noindent?"":"    ", (long long int)tot.size, _nl);
  fprintf(outfile,"%s<directories>%ld</directories>%s", noindent?"":"    ", tot.dirs, _nl);
  if (!dflag) fprintf(outfile,"%s<files>%ld</files>%s", noindent?"":"    ", tot.files, _nl);
  fprintf(outfile,"%s</report>%s",noindent?"":"  ", _nl);
}
