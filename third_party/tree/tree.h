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
#include "libc/calls/weirdtypes.h"
#include "libc/calls/typedef/u.h"
#include "libc/calls/struct/stat.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/macros.internal.h"

#ifdef __ANDROID
#define mbstowcs(w,m,x) mbsrtowcs(w,(const char**)(& #m),x,NULL)
#endif

/* Start using PATH_MAX instead of the magic number 4096 everywhere. */
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifndef INFO_PATH
#define INFO_PATH "/usr/share/finfo/global_info"
#endif

#define ENV_STDDATA_FD "STDDATA_FD"
#define STDDATA_FILENO 3

/* Should probably use strdup(), but we like our xmalloc() */
#define scopy(x)	strcpy(xmalloc(strlen(x)+1),(x))
#define MINIT		30	/* number of dir entries to initially allocate */
#define MINC		20	/* allocation increment */

struct _info {
  char *name;
  char *lnk;
  bool isdir;
  bool issok;
  bool isfifo;
  bool isexe;
  bool orphan;
  mode_t mode, lnkmode;
  uid_t uid;
  gid_t gid;
  off_t size;
  time_t atime, ctime, mtime;
  dev_t dev, ldev;
  ino_t inode, linode;
  #ifdef __EMX__
  long attr;
  #endif
  char *err;
  const char *tag;
  char **comment;
  struct _info **child, *next, *tchild;
};

/* list.c */
struct totals {
  u_long files, dirs;
  off_t size;
};

struct listingcalls {
  void (*intro)(void);
  void (*outtro)(void);
  int (*printinfo)(char *dirname, struct _info *file, int level);
  int (*printfile)(char *dirname, char *filename, struct _info *file, int descend);
  int (*error)(char *error);
  void (*newline)(struct _info *file, int level, int postdir, int needcomma);
  void (*close)(struct _info *file, int level, int needcomma);
  void (*report)(struct totals tot);
};


/* hash.c */
struct xtable {
  unsigned int xid;
  char *name;
  struct xtable *nxt;
};
struct inotable {
  ino_t inode;
  dev_t device;
  struct inotable *nxt;
};

/* color.c */
struct colortable {
  char *term_flg, *CSS_name, *font_fg, *font_bg;
};
struct extensions {
  char *ext;
  char *term_flg, *CSS_name, *web_fg, *web_bg, *web_extattr;
  struct extensions *nxt;
};
struct linedraw {
  const char **name, *vert, *vert_left, *corner, *copy;
  const char *ctop, *cbot, *cmid, *cext, *csingle;
};
struct meta_ids {
  char *name;
  char *term_flg;
};

/* filter.c */
struct pattern {
  char *pattern;
  int relative;
  struct pattern *next;
};

struct ignorefile {
  char *path;
  struct pattern *remove, *reverse;
  struct ignorefile *next;
};

/* info.c */
struct comment {
  struct pattern *pattern;
  char **desc;
  struct comment *next;
};

struct infofile {
  char *path;
  struct comment *comments;
  struct infofile *next;
};


/* Function prototypes: */
/* tree.c */
void setoutput(char *filename);
void print_version(int nl);
void usage(int);
void push_files(char *dir, struct ignorefile **ig, struct infofile **inf, bool top);
int patignore(char *name, int isdir);
int patinclude(char *name, int isdir);
struct _info **unix_getfulltree(char *d, u_long lev, dev_t dev, off_t *size, char **err);
struct _info **read_dir(char *dir, int *n, int infotop);

int filesfirst(struct _info **, struct _info **);
int dirsfirst(struct _info **, struct _info **);
int alnumsort(struct _info **, struct _info **);
int versort(struct _info **a, struct _info **b);
int reversealnumsort(struct _info **, struct _info **);
int mtimesort(struct _info **, struct _info **);
int ctimesort(struct _info **, struct _info **);
int sizecmp(off_t a, off_t b);
int fsizesort(struct _info **a, struct _info **b);

void *xmalloc(size_t), *xrealloc(void *, size_t);
char *gnu_getcwd();
int patmatch(char *, char *, int);
void indent(int maxlevel);
void free_dir(struct _info **);
#ifdef __EMX__
char *prot(long);
#else
char *prot(mode_t);
#endif
char *do_date(time_t);
void printit(char *);
int psize(char *buf, off_t size);
char Ftype(mode_t mode);
struct _info *stat2info(struct stat *st);
char *fillinfo(char *buf, struct _info *ent);

/* list.c */
void null_intro(void);
void null_outtro(void);
void null_close(struct _info *file, int level, int needcomma);
void emit_tree(char **dirname, bool needfulltree);
struct totals listdir(char *dirname, struct _info **dir, int lev, dev_t dev, bool hasfulltree);

/* unix.c */
int unix_printinfo(char *dirname, struct _info *file, int level);
int unix_printfile(char *dirname, char *filename, struct _info *file, int descend);
int unix_error(char *error);
void unix_newline(struct _info *file, int level, int postdir, int needcomma);
void unix_report(struct totals tot);

/* html.c */
void html_intro(void);
void html_outtro(void);
int html_printinfo(char *dirname, struct _info *file, int level);
int html_printfile(char *dirname, char *filename, struct _info *file, int descend);
int html_error(char *error);
void html_newline(struct _info *file, int level, int postdir, int needcomma);
void html_close(struct _info *file, int level, int needcomma);
void html_report(struct totals tot);
void html_encode(FILE *fd, char *s);

/* xml.c */
void xml_intro(void);
void xml_outtro(void);
int xml_printinfo(char *dirname, struct _info *file, int level);
int xml_printfile(char *dirname, char *filename, struct _info *file, int descend);
int xml_error(char *error);
void xml_newline(struct _info *file, int level, int postdir, int needcomma);
void xml_close(struct _info *file, int level, int needcomma);
void xml_report(struct totals tot);

/* json.c */
void json_indent(int maxlevel);
void json_fillinfo(struct _info *ent);
void json_intro(void);
void json_outtro(void);
int json_printinfo(char *dirname, struct _info *file, int level);
int json_printfile(char *dirname, char *filename, struct _info *file, int descend);
int json_error(char *error);
void json_newline(struct _info *file, int level, int postdir, int needcomma);
void json_close(struct _info *file, int level, int needcomma);
void json_report(struct totals tot);

/* color.c */
void parse_dir_colors();
int color(u_short mode, char *name, bool orphan, bool islink);
void endcolor(void);
const char *getcharset(void);
void initlinedraw(int);

/* hash.c */
char *uidtoname(uid_t uid);
char *gidtoname(gid_t gid);
int findino(ino_t, dev_t);
void saveino(ino_t, dev_t);

/* file.c */
struct _info **file_getfulltree(char *d, u_long lev, dev_t dev, off_t *size, char **err);
struct _info **tabedfile_getfulltree(char *d, u_long lev, dev_t dev, off_t *size, char **err);

/* filter.c */
void gittrim(char *s);
struct pattern *new_pattern(char *pattern);
int filtercheck(char *path, char *name, int isdir);
struct ignorefile *new_ignorefile(char *path, bool checkparents);
void push_filterstack(struct ignorefile *ig);
struct ignorefile *pop_filterstack(void);

/* info.c */
struct infofile *new_infofile(char *path, bool checkparents);
void push_infostack(struct infofile *inf);
struct infofile *pop_infostack(void);
struct comment *infocheck(char *path, char *name, int top, int isdir);
void printcomment(int line, int lines, char *s);

/* list.c */
void new_emit_unix(char **dirname, bool needfulltree);
