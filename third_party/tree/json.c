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
extern bool Dflag, inodeflag, devflag, Rflag, cflag, hflag, siflag, duflag;
extern bool noindent, force_color, xdev, nolinks, noreport;

extern const int ifmt[];
extern const char fmt[], *ftype[];

extern FILE *outfile;
extern int Level, *dirs, maxdirs, errors;

extern char *endcode;

/*  JSON code courtesy of Florian Sesser <fs@it-agenten.com>
[
  {"type": "directory", "name": "name", "mode": "0777", "user": "user", "group": "group", "inode": ###, "dev": ####, "time": "00:00 00-00-0000", "info": "<file comment>", "contents": [
    {"type": "link", "name": "name", "target": "name", "info": "...", "contents": [... if link is followed, otherwise this is empty.]}
    {"type": "file", "name": "name", "mode": "0777", "size": ###, "group": "group", "inode": ###, "dev": ###, "time": "00:00 00-00-0000", "info": "..."}
    {"type": "socket", "name": "", "info": "...", "error": "some error" ...}
    {"type": "block", "name": "" ...},
    {"type": "char", "name": "" ...},
    {"type": "fifo", "name": "" ...},
    {"type": "door", "name": "" ...},
    {"type": "port", "name": "" ...}
  ]},
  {"type": "report", "size": ###, "files": ###, "directories": ###}
]
*/

/**
 * JSON encoded strings are not HTML/XML strings:
 * https://tools.ietf.org/html/rfc8259#section-7
 * FIXME: Still not UTF-8
 */
void json_encode(FILE *fd, char *s)
{
  char *ctrl = "0-------btn-fr------------------";
  
  for(;*s;s++) {
    if ((unsigned char)*s < 32) {
      if (ctrl[(unsigned char)*s] != '-') fprintf(fd, "\\%c", ctrl[(unsigned char)*s]);
      else fprintf(fd, "\\u%04x", (unsigned char)*s);
    } else if (*s == '"' || *s == '\\') fprintf(fd, "\\%c", *s);
    else fprintf(fd, "%c", *s);
  }
}

void json_indent(int maxlevel)
{
  int i;

  fprintf(outfile, "  ");
  for(i=0; i<maxlevel; i++)
    fprintf(outfile, "  ");
}

void json_fillinfo(struct _info *ent)
{
  #ifdef __USE_FILE_OFFSET64
  if (inodeflag) fprintf(outfile,",\"inode\":%lld",(long long)ent->inode);
  #else
  if (inodeflag) fprintf(outfile,",\"inode\":%ld",(long int)ent->inode);
  #endif
  if (devflag) fprintf(outfile, ",\"dev\":%d", (int)ent->dev);
  #ifdef __EMX__
  if (pflag) fprintf(outfile, ",\"mode\":\"%04o\",\"prot\":\"%s\"",ent->attr, prot(ent->attr));
  #else
  if (pflag) fprintf(outfile, ",\"mode\":\"%04o\",\"prot\":\"%s\"", ent->mode & (S_IRWXU|S_IRWXG|S_IRWXO|S_ISUID|S_ISGID|S_ISVTX), prot(ent->mode));
  #endif
  if (uflag) fprintf(outfile, ",\"user\":\"%s\"", uidtoname(ent->uid));
  if (gflag) fprintf(outfile, ",\"group\":\"%s\"", gidtoname(ent->gid));
  if (sflag) {
    if (hflag || siflag) {
      char nbuf[64];
      int i;
      psize(nbuf,ent->size);
      for(i=0; isspace(nbuf[i]); i++);	/* trim() hack */
      fprintf(outfile, ",\"size\":\"%s\"", nbuf+i);
    } else
      fprintf(outfile, ",\"size\":%lld", (long long int)ent->size);
  }
  if (Dflag) fprintf(outfile, ",\"time\":\"%s\"", do_date(cflag? ent->ctime : ent->mtime));
}


void json_intro(void)
{
  extern char *_nl;
  fprintf(outfile, "[%s", noindent? "" : _nl);
}

void json_outtro(void)
{
  extern char *_nl;
  fprintf(outfile, "%s]\n", noindent? "" : _nl);
}

int json_printinfo(char *dirname, struct _info *file, int level)
{
  mode_t mt;
  int t;

  if (!noindent) json_indent(level);

  if (file != NULL) {
    if (file->lnk) mt = file->mode & S_IFMT;
    else mt = file->mode & S_IFMT;
  } else mt = 0;

  for(t=0;ifmt[t];t++)
    if (ifmt[t] == mt) break;
  fprintf(outfile,"{\"type\":\"%s\"", ftype[t]);

  return 0;
}

int json_printfile(char *dirname, char *filename, struct _info *file, int descend)
{
  int i;

  fprintf(outfile, ",\"name\":\"");
  json_encode(outfile, filename);
  fputc('"',outfile);

  if (file && file->comment) {
    fprintf(outfile, ",\"info\":\"");
    for(i=0; file->comment[i]; i++) {
      json_encode(outfile, file->comment[i]);
      if (file->comment[i+1]) fprintf(outfile, "\\n");
    }
    fprintf(outfile, "\"");
  }

  if (file && file->lnk) {
    fprintf(outfile, ",\"target\":\"");
    json_encode(outfile, file->lnk);
    fputc('"',outfile);
  }
  if (file) json_fillinfo(file);

  if (file && file->err) fprintf(outfile, ",\"error\": \"%s\"", file->err);
  if (!descend) fputc('}',outfile);
  else fprintf(outfile, ",\"contents\":[");

  return descend;
}

int json_error(char *error)
{
  fprintf(outfile,"{\"error\": \"%s\"}%s",error, noindent?"":"\n");
  return 0;
}

void json_newline(struct _info *file, int level, int postdir, int needcomma)
{
  extern char *_nl;

  fprintf(outfile, "%s%s", needcomma? "," : "", _nl);
}

void json_close(struct _info *file, int level, int needcomma)
{
  if (!noindent) json_indent(level);
  fprintf(outfile,"]}%s%s", needcomma? ",":"", noindent? "":"\n");
}

void json_report(struct totals tot)
{
  fprintf(outfile, ",%s{\"type\":\"report\"",noindent?"":"\n  ");
  if (duflag) fprintf(outfile,",\"size\":%lld", (long long int)tot.size);
  fprintf(outfile,",\"directories\":%ld", tot.dirs);
  if (!dflag) fprintf(outfile,",\"files\":%ld", tot.files);
  fprintf(outfile, "}");
}
