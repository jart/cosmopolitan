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
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/limits.h"
#include "libc/thread/thread.h"
#include "third_party/musl/passwd.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");

static unsigned atou(char **s) {
  unsigned x;
  for (x = 0; **s - '0' < 10U; ++*s) x = 10 * x + (**s - '0');
  return x;
}

int __getgrent_a(FILE *f, struct group *gr, char **line, size_t *size,
		 char ***mem, size_t *nmem, struct group **res) {
  ssize_t l;
  char *s, *mems;
  size_t i;
  int rv = 0;
  int cs;
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);
  for (;;) {
    if ((l = getline(line, size, f)) < 0) {
      rv = ferror(f) ? errno : 0;
      free(*line);
      *line = 0;
      gr = 0;
      goto end;
    }
    line[0][l - 1] = 0;
    s = line[0];
    gr->gr_name = s++;
    if (!(s = strchr(s, ':'))) continue;
    *s++ = 0;
    gr->gr_passwd = s;
    if (!(s = strchr(s, ':'))) continue;
    *s++ = 0;
    gr->gr_gid = atou(&s);
    if (*s != ':') continue;
    *s++ = 0;
    mems = s;
    break;
  }
  for (*nmem = !!*s; *s; s++)
    if (*s == ',') ++*nmem;
  free(*mem);
  *mem = calloc(sizeof(char *), *nmem + 1);
  if (!*mem) {
    rv = errno;
    free(*line);
    *line = 0;
    gr = 0;
    goto end;
  }
  if (*mems) {
    mem[0][0] = mems;
    for (s = mems, i = 0; *s; s++)
      if (*s == ',') *s++ = 0, mem[0][++i] = s;
    mem[0][++i] = 0;
  } else {
    mem[0][0] = 0;
  }
  gr->gr_mem = *mem;
end:
  pthread_setcancelstate(cs, 0);
  *res = gr;
  if (rv) errno = rv;
  return rv;
}

int __getgr_a(const char *name, gid_t gid, struct group *gr, char **buf,
	      size_t *size, char ***mem, size_t *nmem,
	      struct group **res) {
  FILE *f;
  int rv = 0;
  int cs;
  *res = 0;
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);
  f = fopen("/etc/group", "rbe");
  if (!f) {
    rv = errno;
    goto done;
  }
  while (!(rv = __getgrent_a(f, gr, buf, size, mem, nmem, res)) && *res) {
    if ((name && !strcmp(name, (*res)->gr_name)) ||
        (!name && (*res)->gr_gid == gid)) {
      break;
    }
  }
done:
  pthread_setcancelstate(cs, 0);
  if (rv) errno = rv;
  return rv;
}

static int getgr_r(const char *name, gid_t gid, struct group *gr, char *buf,
                   size_t size, struct group **res) {
#define FIX(x) (gr->gr_##x = gr->gr_##x - line + buf)
  char *line = 0;
  size_t len = 0;
  char **mem = 0;
  size_t nmem = 0;
  int rv = 0;
  size_t i;
  int cs;
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);
  rv = __getgr_a(name, gid, gr, &line, &len, &mem, &nmem, res);
  if (*res && size < len + (nmem + 1) * sizeof(char *) + 32) {
    *res = 0;
    rv = ERANGE;
  }
  if (*res) {
    buf += (16 - (uintptr_t)buf) % 16;
    gr->gr_mem = (void *)buf;
    buf += (nmem + 1) * sizeof(char *);
    memcpy(buf, line, len);
    FIX(name);
    FIX(passwd);
    for (i = 0; mem[i]; i++) gr->gr_mem[i] = mem[i] - line + buf;
    gr->gr_mem[i] = 0;
  }
  free(mem);
  free(line);
  pthread_setcancelstate(cs, 0);
  if (rv) errno = rv;
  return rv;
#undef FIX
}

int getgrouplist(const char *user, gid_t gid, gid_t *groups, int *ngroups) {
  int rv, nlim, ret = -1;
  ssize_t i, n = 1;
  struct group gr;
  struct group *res;
  FILE *f;
  char *buf = 0;
  char **mem = 0;
  size_t nmem = 0;
  size_t size;
  nlim = *ngroups;
  if (nlim >= 1) *groups++ = gid;
  f = fopen("/etc/group", "rbe");
  if (!f && errno != ENOENT && errno != ENOTDIR) goto cleanup;
  if (f) {
    while (!(rv = __getgrent_a(f, &gr, &buf, &size, &mem, &nmem, &res)) &&
           res) {
      for (i = 0; gr.gr_mem[i] && strcmp(user, gr.gr_mem[i]); i++)
        ;
      if (!gr.gr_mem[i]) continue;
      if (++n <= nlim) *groups++ = gr.gr_gid;
    }
    if (rv) {
      errno = rv;
      goto cleanup;
    }
  }
  ret = n > nlim ? -1 : n;
  *ngroups = n;
cleanup:
  if (f) fclose(f);
  free(buf);
  free(mem);
  return ret;
}

int getgrnam_r(const char *name, struct group *gr, char *buf, size_t size,
               struct group **res) {
  return getgr_r(name, 0, gr, buf, size, res);
}

int getgrgid_r(gid_t gid, struct group *gr, char *buf, size_t size,
               struct group **res) {
  return getgr_r(0, gid, gr, buf, size, res);
}

static struct GetgrentState {
  FILE *f;
  char *line;
  char **mem;
  struct group gr;
} g_getgrent[1];

/**
 * Closes group database.
 * @threadunsafe
 */
void endgrent() {
  setgrent();
}

/**
 * Rewinds to beginning of group database.
 * @threadunsafe
 */
void setgrent() {
  if (g_getgrent->f) fclose(g_getgrent->f);
  g_getgrent->f = 0;
}

/**
 * Returns successive entries in /etc/group database.
 * @threadunsafe
 */
struct group *getgrent() {
  struct group *res;
  size_t size = 0, nmem = 0;
  if (!g_getgrent->f) g_getgrent->f = fopen("/etc/group", "rbe");
  if (!g_getgrent->f) return 0;
  __getgrent_a(g_getgrent->f, &g_getgrent->gr, &g_getgrent->line, &size,
               &g_getgrent->mem, &nmem, &res);
  return res;
}

struct group *getgrgid(gid_t gid) {
  struct group *res;
  size_t size = 0, nmem = 0;
  __getgr_a(0, gid, &g_getgrent->gr, &g_getgrent->line, &size, &g_getgrent->mem,
            &nmem, &res);
  return res;
}

struct group *getgrnam(const char *name) {
  struct group *res;
  size_t size = 0, nmem = 0;
  __getgr_a(name, 0, &g_getgrent->gr, &g_getgrent->line, &size,
            &g_getgrent->mem, &nmem, &res);
  return res;
}

int initgroups(const char *user, gid_t gid) {
  gid_t groups[NGROUPS_MAX];
  int count = NGROUPS_MAX;
  if (getgrouplist(user, gid, groups, &count) < 0) return -1;
  return setgroups(count, groups);
}
