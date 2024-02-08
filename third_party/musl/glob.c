/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│ vi: set et ft=c ts=8 sw=8 fenc=utf-8                                     :vi │
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  Musl Libc                                                                   │
│  Copyright © 2005-2014 Rich Felker, et al.                                   │
│                                                                              │
│  Permission is hereby granted, free of charge, to any person obtaining       │
│  a copy of this software and associated documentation files (the             │
│  "Software"), to deal in the Software without restriction, including         │
│  without limitation the rights to use, copy, modify, merge, publish,         │
│  distribute, sublicense, and/or sell copies of the Software, and to          │
│  permit persons to whom the Software is furnished to do so, subject to       │
│  the following conditions:                                                   │
│                                                                              │
│  The above copyright notice and this permission notice shall be              │
│  included in all copies or substantial portions of the Software.             │
│                                                                              │
│  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,             │
│  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF          │
│  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.      │
│  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY        │
│  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,        │
│  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE           │
│  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                      │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/musl/glob.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/dirent.h"
#include "libc/calls/struct/stat.h"
#include "libc/errno.h"
#include "libc/mem/alg.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/dt.h"
#include "libc/sysv/consts/s.h"
#include "third_party/musl/fnmatch.h"

#define MAXPATH 1024

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");

struct GlobList {
  struct GlobList *next;
  char name[];
};

static int AppendGlob(struct GlobList **tail, const char *name, size_t len,
                      int mark) {
  struct GlobList *new;
  if ((new = malloc(sizeof(struct GlobList) + len + 2))) {
    (*tail)->next = new;
    new->next = NULL;
    memcpy(new->name, name, len + 1);
    if (mark && len && name[len - 1] != '/') {
      new->name[len] = '/';
      new->name[len + 1] = 0;
    }
    *tail = new;
    return 0;
  } else {
    return -1;
  }
}

static int PerformGlob(char *buf, size_t pos, int type, char *pat, int flags,
                       int (*errfunc)(const char *path, int err),
                       struct GlobList **tail) {
  DIR *dir;
  size_t l;
  char *p, *p2;
  char saved_sep;
  ptrdiff_t i, j;
  struct stat st;
  struct dirent *de;
  int r, readerr, in_bracket, overflow, old_errno, fnm_flags;
  /* If GLOB_MARK is unused, we don't care about type. */
  if (!type && !(flags & GLOB_MARK)) type = DT_REG;
  /* Special-case the remaining pattern being all slashes, in
   * which case we can use caller-passed type if it's a dir. */
  if (*pat && type != DT_DIR) type = 0;
  while (pos + 1 < MAXPATH && *pat == '/') {
    buf[pos++] = *pat++;
  }
  /* Consume maximal [escaped-]literal prefix of pattern, copying
   * and un-escaping it to the running buffer as we go. */
  i = 0;
  j = 0;
  overflow = 0;
  in_bracket = 0;
  for (; pat[i] != '*' && pat[i] != '?' && (!in_bracket || pat[i] != ']');
       i++) {
    if (!pat[i]) {
      if (overflow) return 0;
      pat += i;
      pos += j;
      i = j = 0;
      break;
    } else if (pat[i] == '[') {
      in_bracket = 1;
    } else if (pat[i] == '\\' && !(flags & GLOB_NOESCAPE)) {
      /* Backslashes inside a bracket are (at least by
       * our interpretation) non-special, so if next
       * char is ']' we have a complete expression. */
      if (in_bracket && pat[i + 1] == ']') break;
      /* Unpaired final backslash never matches. */
      if (!pat[i + 1]) return 0;
      i++;
    }
    if (pat[i] == '/') {
      if (overflow) return 0;
      in_bracket = 0;
      pat += i + 1;
      i = -1;
      pos += j + 1;
      j = -1;
    }
    /* Only store a character if it fits in the buffer, but if
     * a potential bracket expression is open, the overflow
     * must be remembered and handled later only if the bracket
     * is unterminated (and thereby a literal), so as not to
     * disallow long bracket expressions with short matches. */
    if (pos + (j + 1) < MAXPATH) {
      buf[pos + j++] = pat[i];
    } else if (in_bracket) {
      overflow = 1;
    } else {
      return 0;
    }
    /* If we consume any new components, the caller-passed type
     * or dummy type from above is no longer valid. */
    type = 0;
  }
  buf[pos] = 0;
  if (!*pat) {
    /* If we consumed any components above, or if GLOB_MARK is
     * requested and we don't yet know if the match is a dir,
     * we must call stat to confirm the file exists and/or
     * determine its type. */
    if ((flags & GLOB_MARK) && type == DT_LNK) type = 0;
    if (!type && stat(buf, &st)) {
      if (errno != ENOENT && (errfunc(buf, errno) || (flags & GLOB_ERR))) {
        return GLOB_ABORTED;
      }
      return 0;
    }
    if (!type && S_ISDIR(st.st_mode)) type = DT_DIR;
    if (AppendGlob(tail, buf, pos, (flags & GLOB_MARK) && type == DT_DIR)) {
      return GLOB_NOSPACE;
    }
    return 0;
  }
  p2 = strchr(pat, '/');
  saved_sep = '/';
  /* Check if the '/' was escaped and, if so, remove the escape char
   * so that it will not be unpaired when passed to fnmatch. */
  if (p2 && !(flags & GLOB_NOESCAPE)) {
    for (p = p2; p > pat && p[-1] == '\\'; p--)
      ;
    if ((p2 - p) % 2) {
      p2--;
      saved_sep = '\\';
    }
  }
  dir = opendir(pos ? buf : ".");
  if (!dir) {
    if (errfunc(buf, errno) || (flags & GLOB_ERR)) return GLOB_ABORTED;
    return 0;
  }
  old_errno = errno;
  while (errno = 0, de = readdir(dir)) {
    /* Quickly skip non-directories when there's pattern left. */
    if (p2 && de->d_type && de->d_type != DT_DIR && de->d_type != DT_LNK) {
      continue;
    }
    l = strlen(de->d_name);
    if (l >= MAXPATH - pos) continue;
    if (p2) *p2 = 0;
    fnm_flags = ((flags & GLOB_NOESCAPE) ? FNM_NOESCAPE : 0) |
                ((!(flags & GLOB_PERIOD)) ? FNM_PERIOD : 0);
    if (fnmatch(pat, de->d_name, fnm_flags)) continue;
    /* With GLOB_PERIOD don't allow matching . or .. unless fnmatch()
     * would match them with FNM_PERIOD rules in effect. */
    if (p2 && (flags & GLOB_PERIOD) && de->d_name[0] == '.' &&
        (!de->d_name[1] || (de->d_name[1] == '.' && !de->d_name[2])) &&
        fnmatch(pat, de->d_name, fnm_flags | FNM_PERIOD)) {
      continue;
    }
    memcpy(buf + pos, de->d_name, l + 1);
    if (p2) *p2 = saved_sep;
    r = PerformGlob(buf, pos + l, de->d_type, p2 ? p2 : "", flags, errfunc,
                    tail);
    if (r) {
      closedir(dir);
      return r;
    }
  }
  readerr = errno;
  if (p2) *p2 = saved_sep;
  closedir(dir);
  if (readerr && (errfunc(buf, errno) || (flags & GLOB_ERR))) {
    return GLOB_ABORTED;
  }
  errno = old_errno;
  return 0;
}

static int IgnoreGlobError(const char *path, int err) {
  return 0;
}

static void FreeGlobList(struct GlobList *head) {
  struct GlobList *match, *next;
  for (match = head->next; match; match = next) {
    next = match->next;
    free(match);
  }
}

static int GlobPredicate(const void *a, const void *b) {
  return strcmp(*(const char **)a, *(const char **)b);
}

/**
 * Finds pathnames matching pattern.
 *
 * For example:
 *
 *     glob_t g = {.gl_offs = 2};
 *     glob("*.*", GLOB_DOOFFS, NULL, &g);
 *     glob("../.*", GLOB_DOOFFS | GLOB_APPEND, NULL, &g);
 *     g.gl_pathv[0] = "ls";
 *     g.gl_pathv[1] = "-l";
 *     execvp("ls", &g.gl_pathv[0]);
 *     globfree(g);
 *
 * @param pat can have star wildcard see fnmatch()
 * @param g will receive matching entries and needs globfree()
 * @return 0 on success or GLOB_NOMATCH, GLOB_NOSPACE on OOM, or
 *     GLOB_ABORTED on read error
 */
int glob(const char *pat, int flags, int errfunc(const char *path, int err),
         glob_t *g) {
  int error = 0;
  size_t cnt, i;
  char **pathv, buf[MAXPATH];
  struct GlobList head = {.next = NULL}, *tail = &head;
  size_t offs = (flags & GLOB_DOOFFS) ? g->gl_offs : 0;
  if (!errfunc) errfunc = IgnoreGlobError;
  if (!(flags & GLOB_APPEND)) {
    g->gl_offs = offs;
    g->gl_pathc = 0;
    g->gl_pathv = NULL;
  }
  if (*pat) {
    char *p = strdup(pat);
    if (!p) return GLOB_NOSPACE;
    buf[0] = 0;
    error = PerformGlob(buf, 0, 0, p, flags, errfunc, &tail);
    free(p);
  }
  if (error == GLOB_NOSPACE) {
    FreeGlobList(&head);
    return error;
  }
  for (cnt = 0, tail = head.next; tail; tail = tail->next, cnt++)
    ;
  if (!cnt) {
    if (flags & GLOB_NOCHECK) {
      tail = &head;
      if (AppendGlob(&tail, pat, strlen(pat), 0)) {
        return GLOB_NOSPACE;
      }
      cnt++;
    } else
      return GLOB_NOMATCH;
  }
  if (flags & GLOB_APPEND) {
    pathv =
        realloc(g->gl_pathv, (offs + g->gl_pathc + cnt + 1) * sizeof(char *));
    if (!pathv) {
      FreeGlobList(&head);
      return GLOB_NOSPACE;
    }
    g->gl_pathv = pathv;
    offs += g->gl_pathc;
  } else {
    g->gl_pathv = malloc((offs + cnt + 1) * sizeof(char *));
    if (!g->gl_pathv) {
      FreeGlobList(&head);
      return GLOB_NOSPACE;
    }
    for (i = 0; i < offs; i++) {
      g->gl_pathv[i] = NULL;
    }
  }
  for (i = 0, tail = head.next; i < cnt; tail = tail->next, i++) {
    g->gl_pathv[offs + i] = tail->name;
  }
  g->gl_pathv[offs + i] = NULL;
  g->gl_pathc += cnt;
  if (!(flags & GLOB_NOSORT)) {
    qsort(g->gl_pathv + offs, cnt, sizeof(char *), GlobPredicate);
  }
  return error;
}

/**
 * Frees entries allocated by glob().
 */
void globfree(glob_t *g) {
  size_t i;
  for (i = 0; i < g->gl_pathc; i++) {
    free(g->gl_pathv[g->gl_offs + i] - offsetof(struct GlobList, name));
  }
  free(g->gl_pathv);
  g->gl_pathc = 0;
  g->gl_pathv = NULL;
}
