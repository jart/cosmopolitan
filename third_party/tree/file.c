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
#include "libc/mem/mem.h"
#include "libc/mem/alg.h"
#include "libc/sysv/consts/s.h"
#include "third_party/tree/tree.h"

extern bool dflag, Fflag, aflag, fflag, pruneflag, gitignore, showinfo;
extern bool noindent, force_color, matchdirs, fflinks;
extern bool reverse;
extern int pattern, ipattern;

extern int (*topsort)();
extern FILE *outfile;
extern int Level, *dirs, maxdirs;

extern bool colorize;
extern char *endcode;

extern char *file_comment, *file_pathsep;

/* 64K paths maximum */
#define MAXPATH	64*1024

enum ftok { T_PATHSEP, T_DIR, T_FILE, T_EOP };

char *nextpc(char **p, int *tok)
{
  static char prev = 0;
  char *s = *p;
  if (!**p) {
    *tok = T_EOP;	/* Shouldn't happen. */
    return NULL;
  }
  if (prev) {
    prev = 0;
    *tok = T_PATHSEP;
    return NULL;
  }
  if (strchr(file_pathsep, **p) != NULL) {
    (*p)++;
    *tok = T_PATHSEP;
    return NULL;
  }
  while(**p && strchr(file_pathsep,**p) == NULL) (*p)++;

  if (**p) {
    *tok = T_DIR;
    prev = **p;
    *(*p)++ = '\0';
  } else *tok = T_FILE;
  return s;
}

struct _info *newent(char *name) {
  struct _info *n = xmalloc(sizeof(struct _info));
  memset(n,0,sizeof(struct _info));
  n->name = scopy(name);
  n->child = NULL;
  n->tchild = n->next = NULL;
  return n;
}

/* Should replace this with a Red-Black tree implementation or the like */
struct _info *search(struct _info **dir, char *name)
{
  struct _info *ptr, *prev, *n;
  int cmp;

  if (*dir == NULL) return (*dir = newent(name));

  for(prev = ptr = *dir; ptr != NULL; ptr=ptr->next) {
    cmp = strcmp(ptr->name,name);
    if (cmp == 0) return ptr;
    if (cmp > 0) break;
    prev = ptr;
  }
  n = newent(name);
  n->next = ptr;
  if (prev == ptr) *dir = n;
  else prev->next = n;
  return n;
}

void freefiletree(struct _info *ent)
{
  struct _info *ptr = ent, *t;

  while (ptr != NULL) {
    if (ptr->tchild) freefiletree(ptr->tchild);
    t = ptr;
    ptr = ptr->next;
    free(t);
  }
}

/**
 * Recursively prune (unset show flag) files/directories of matches/ignored
 * patterns:
 */
struct _info **fprune(struct _info *head, char *path, bool matched, bool root)
{
  struct _info **dir, *new = NULL, *end = NULL, *ent, *t;
  struct comment *com;
  struct ignorefile *ig = NULL;
  struct infofile *inf = NULL;
  char *cur, *fpath = xmalloc(sizeof(char) * MAXPATH);
  int i, show, count = 0;

  strcpy(fpath, path);
  cur = fpath + strlen(fpath);
  *(cur++) = '/';

  push_files(path, &ig, &inf, root);

  for(ent = head; ent != NULL;) {
    strcpy(cur, ent->name);
    if (ent->tchild) ent->isdir = 1;

    show = 1;
    if (dflag && !ent->isdir) show = 0;
    if (!aflag && !root && ent->name[0] == '.') show = 0;
    if (show && !matched) {
      if (!ent->isdir) {
	if (pattern && !patinclude(ent->name, 0)) show = 0;
	if (ipattern && patignore(ent->name, 0)) show = 0;
      }
      if (ent->isdir && show && matchdirs && pattern) {
	if (patinclude(ent->name, 1)) matched = TRUE;
      }
    }
    if (pruneflag && !matched && ent->isdir && ent->tchild == NULL) show = 0;
    if (gitignore && filtercheck(path, ent->name, ent->isdir)) show = 0;
    if (show && showinfo && (com = infocheck(path, ent->name, inf != NULL, ent->isdir))) {
      for(i = 0; com->desc[i] != NULL; i++);
      ent->comment = xmalloc(sizeof(char *) * (i+1));
      for(i = 0; com->desc[i] != NULL; i++) ent->comment[i] = scopy(com->desc[i]);
      ent->comment[i] = NULL;
    }
    if (show && ent->tchild != NULL) ent->child = fprune(ent->tchild, fpath, matched, FALSE);


    t = ent;
    ent = ent->next;
    if (show) {
      if (end) end = end->next = t;
      else new = end = t;
      count++;
    } else {
      t->next = NULL;
      freefiletree(t);
    }
  }
  if (end) end->next = NULL;

  dir = xmalloc(sizeof(struct _info *) * (count+1));
  for(count = 0, ent = new; ent != NULL; ent = ent->next, count++) {
    dir[count] = ent;
  }
  dir[count] = NULL;

  if (topsort) qsort(dir,count,sizeof(struct _info *),topsort);

  if (ig != NULL) ig = pop_filterstack();
  if (inf != NULL) inf = pop_infostack();
  free(fpath);

  return dir;
}

struct _info **file_getfulltree(char *d, u_long lev, dev_t dev, off_t *size, char **err)
{
  FILE *fp = (strcmp(d,".")? fopen(d,"r") : stdin);
  char *path, *spath, *s, *link;
  struct _info *root = NULL, **cwd, *ent;
  int l, tok;

  *size = 0;
  if (fp == NULL) {
    fprintf(stderr,"tree: Error opening %s for reading.\n", d);
    return NULL;
  }
  path = xmalloc(sizeof(char) * MAXPATH);

  while(fgets(path, MAXPATH, fp) != NULL) {
    if (file_comment != NULL && strncmp(path,file_comment,strlen(file_comment)) == 0) continue;
    l = strlen(path);
    while(l && (path[l-1] == '\n' || path[l-1] == '\r')) path[--l] = '\0';
    if (l == 0) continue;

    spath = path;
    cwd = &root;

    link = fflinks? strstr(path, " -> ") : NULL;
    if (link) {
      *link = '\0';
      link += 4;
    }
    ent = NULL;
    do {
      s = nextpc(&spath, &tok);
      if (tok == T_PATHSEP) continue;
      switch(tok) {
	case T_PATHSEP: continue;
	case T_FILE:
	case T_DIR:
	  /* Should probably handle '.' and '..' entries here */
	  ent = search(cwd, s);
	  /* Might be empty, but should definitely be considered a directory: */
	  if (tok == T_DIR) {
	    ent->isdir = 1;
	    ent->mode = S_IFDIR;
	  } else {
	    ent->mode = S_IFREG;
	  }

	  cwd = &(ent->tchild);
	  break;
      }
    } while (tok != T_FILE && tok != T_EOP);

    if (link) {
      ent->isdir = 0;
      ent->mode = S_IFLNK;
      ent->lnk = scopy(link);
    }
  }
  if (fp != stdin) fclose(fp);

  free(path);

  /* Prune accumulated directory tree: */
  return fprune(root, "", FALSE, TRUE);
}

struct _info **tabedfile_getfulltree(char *d, u_long lev, dev_t dev, off_t *size, char **err)
{
  FILE *fp = (strcmp(d,".")? fopen(d,"r") : stdin);
  char *path, *spath, *link;
  struct _info *root = NULL, **istack, *ent;
  int line = 0, tabs, maxstack = 2048, top = 0, l;

  *size = 0;
  if (fp == NULL) {
    fprintf(stderr,"tree: Error opening %s for reading.\n", d);
    return NULL;
  }
  path = xmalloc(sizeof(char) * MAXPATH);
  istack = xmalloc(sizeof(struct _info *) * maxstack);
  memset(istack, 0, sizeof(struct _info *) * maxstack);

  while(fgets(path, MAXPATH, fp) != NULL) {
    line++;
    if (file_comment != NULL && strncmp(path,file_comment,strlen(file_comment)) == 0) continue;
    l = strlen(path);
    while(l && (path[l-1] == '\n' || path[l-1] == '\r')) path[--l] = '\0';
    if (l == 0) continue;

    for(tabs=0; path[tabs] == '\t'; tabs++);
    if (tabs >= maxstack) {
      fprintf(stderr, "tree: Tab depth exceeds maximum path depth (%d >= %d) on line %d\n", tabs, maxstack, line);
      continue;
    }

    spath = path+tabs;

    link = fflinks? strstr(spath, " -> ") : NULL;
    if (link) {
      *link = '\0';
      link += 4;
    }
    if (tabs-1 > top) {
	fprintf(stderr, "tree: Orphaned file [%s] on line %d, check tab depth in file.\n", spath, line);
	continue;
    }
    ent = istack[tabs] = search(tabs? &(istack[tabs-1]->tchild) : &root, spath);
    ent->mode = S_IFREG;
    if (tabs) {
      istack[tabs-1]->isdir = 1;
      istack[tabs-1]->mode = S_IFDIR;
    }
    if (link) {
      ent->isdir = 0;
      ent->mode = S_IFLNK;
      ent->lnk = scopy(link);
    }
    top = tabs;
  }
  if (fp != stdin) fclose(fp);

  free(path);
  free(istack);

  /* Prune accumulated directory tree: */
  return fprune(root, "", FALSE, TRUE);
}
