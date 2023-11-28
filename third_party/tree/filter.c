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
#include "libc/calls/calls.h"
#include "libc/mem/mem.h"
#include "third_party/tree/tree.h"

extern char xpattern[PATH_MAX];

struct ignorefile *filterstack = NULL;

void gittrim(char *s)
{
  int i, e = strlen(s)-1;

  if (s[e] == '\n') e--;

  for(i = e; i >= 0; i--) {
    if (s[i] != ' ') break;
    if (i && s[i-1] != '\\') e--;
  }
  s[e+1] = '\0';
  for(i = e = 0; s[i] != '\0';) {
    if (s[i] == '\\') i++;
    s[e++] = s[i++];
  }
  s[e] = '\0';
}

struct pattern *new_pattern(char *pattern)
{
  struct pattern *p = xmalloc(sizeof(struct pattern));
  p->pattern = scopy(pattern + ((pattern[0] == '/')? 1 : 0));
  p->relative = (strchr(pattern,'/') == NULL);
  p->next = NULL;
  return p;
}

struct ignorefile *new_ignorefile(char *path, bool checkparents)
{
  struct stat st;
  char buf[PATH_MAX], rpath[PATH_MAX];
  struct ignorefile *ig;
  struct pattern *remove = NULL, *remend, *p;
  struct pattern *reverse = NULL, *revend;
  int rev;
  FILE *fp;

  rev = stat(path, &st);
  if (rev < 0 || !S_ISREG(st.st_mode)) {
    snprintf(buf, PATH_MAX, "%s/.gitignore", path);
    fp = fopen(buf, "r");

    if (fp == NULL && checkparents) {
      strcpy(rpath, path);
      while ((fp == NULL) && (strcmp(rpath, "/") != 0)) {
	snprintf(buf, PATH_MAX, "%.*s/..", PATH_MAX-4, rpath);
	if (realpath(buf, rpath) == NULL) break;
	snprintf(buf, PATH_MAX, "%.*s/.gitignore", PATH_MAX-12, rpath);
	fp = fopen(buf, "r");
      }
    }
  } else fp = fopen(path, "r");
  if (fp == NULL) return NULL;

  while (fgets(buf, PATH_MAX, fp) != NULL) {
    if (buf[0] == '#') continue;
    rev = (buf[0] == '!');
    gittrim(buf);
    if (strlen(buf) == 0) continue;
    p = new_pattern(buf + (rev? 1 : 0));
    if (rev) {
      if (reverse == NULL) reverse = revend = p;
      else {
	revend->next = p;
	revend = p;
      }
    } else {
      if (remove == NULL) remove = remend = p;
      else {
	remend->next = p;
	remend = p;
      }
    }
  }

  fclose(fp);

  ig = xmalloc(sizeof(struct ignorefile));
  ig->remove = remove;
  ig->reverse = reverse;
  ig->path = scopy(path);
  ig->next = NULL;

  return ig;
}

void push_filterstack(struct ignorefile *ig)
{
  if (ig == NULL) return;
  ig->next = filterstack;
  filterstack = ig;
}

struct ignorefile *pop_filterstack(void)
{
  struct ignorefile *ig;
  struct pattern *p, *c;

  ig = filterstack;
  if (ig == NULL) return NULL;

  filterstack = filterstack->next;

  for(p=c=ig->remove; p != NULL; c = p) {
    p=p->next;
    free(c->pattern);
  }
  for(p=c=ig->reverse; p != NULL; c = p) {
    p=p->next;
    free(c->pattern);
  }
  free(ig->path);
  free(ig);
  return NULL;
}

/**
 * true if remove filter matches and no reverse filter matches.
 */
int filtercheck(char *path, char *name, int isdir)
{
  int filter = 0;
  struct ignorefile *ig;
  struct pattern *p;

  for(ig = filterstack; !filter && ig; ig = ig->next) {
    int fpos = sprintf(xpattern, "%s/", ig->path);

    for(p = ig->remove; p != NULL; p = p->next) {
      if (p->relative) {
	if (patmatch(name, p->pattern, isdir) == 1) {
	  filter = 1;
	  break;
	}
      } else {
	sprintf(xpattern + fpos, "%s", p->pattern);
	if (patmatch(path, xpattern, isdir) == 1) {
	  filter = 1;
	  break;
	}
      }
     }
  }
  if (!filter) return 0;

  for(ig = filterstack; ig; ig = ig->next) {
    int fpos = sprintf(xpattern, "%s/", ig->path);

    for(p = ig->reverse; p != NULL; p = p->next) {
      if (p->relative) {
	if (patmatch(name, p->pattern, isdir) == 1) return 0;
      } else {
	sprintf(xpattern + fpos, "%s", p->pattern);
	if (patmatch(path, xpattern, isdir) == 1) return 0;
      }
    }
  }

  return 1;
}
