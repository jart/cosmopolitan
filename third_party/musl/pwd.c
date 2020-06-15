/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:2;tab-width:8;coding:utf-8   -*-│
│vi: set et ft=c ts=2 tw=8 fenc=utf-8                                       :vi│
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
#include "libc/calls/weirdtypes.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "third_party/musl/passwd.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");

#define PTHREAD_CANCEL_DISABLE       0
#define pthread_setcancelstate(x, y) (void)y

static unsigned atou(char **s) {
  unsigned x;
  for (x = 0; **s - '0' < 10U; ++*s) x = 10 * x + (**s - '0');
  return x;
}

static int __getpwent_a(FILE *f, struct passwd *pw, char **line, size_t *size,
                        struct passwd **res) {
  ssize_t l;
  char *s;
  int rv = 0;
  int cs;
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);
  for (;;) {
    if ((l = getline(line, size, f)) < 0) {
      rv = ferror(f) ? errno : 0;
      free(*line);
      *line = 0;
      pw = 0;
      break;
    }
    line[0][l - 1] = 0;
    s = line[0];
    pw->pw_name = s++;
    if (!(s = strchr(s, ':'))) continue;
    *s++ = 0;
    pw->pw_passwd = s;
    if (!(s = strchr(s, ':'))) continue;
    *s++ = 0;
    pw->pw_uid = atou(&s);
    if (*s != ':') continue;
    *s++ = 0;
    pw->pw_gid = atou(&s);
    if (*s != ':') continue;
    *s++ = 0;
    pw->pw_gecos = s;
    if (!(s = strchr(s, ':'))) continue;
    *s++ = 0;
    pw->pw_dir = s;
    if (!(s = strchr(s, ':'))) continue;
    *s++ = 0;
    pw->pw_shell = s;
    break;
  }
  pthread_setcancelstate(cs, 0);
  *res = pw;
  if (rv) errno = rv;
  return rv;
}

static int __getpw_a(const char *name, uid_t uid, struct passwd *pw, char **buf,
                     size_t *size, struct passwd **res) {
  FILE *f;
  int cs;
  int rv = 0;
  *res = 0;
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);
  if ((f = fopen("/etc/passwd", "rbe"))) {
    while (!(rv = __getpwent_a(f, pw, buf, size, res)) && *res) {
      if ((name && !strcmp(name, (*res)->pw_name)) ||
          (!name && (*res)->pw_uid == uid)) {
        break;
      }
    }
    fclose(f);
  }
  pthread_setcancelstate(cs, 0);
  if (rv) errno = rv;
  return rv;
}

static int getpw_r(const char *name, uid_t uid, struct passwd *pw, char *buf,
                   size_t size, struct passwd **res) {
#define FIX(x) (pw->pw_##x = pw->pw_##x - line + buf)
  char *line = 0;
  size_t len = 0;
  int rv = 0;
  int cs;
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);
  rv = __getpw_a(name, uid, pw, &line, &len, res);
  if (*res && size < len) {
    *res = 0;
    rv = ERANGE;
  }
  if (*res) {
    memcpy(buf, line, len);
    FIX(name);
    FIX(passwd);
    FIX(gecos);
    FIX(dir);
    FIX(shell);
  }
  free(line);
  pthread_setcancelstate(cs, 0);
  if (rv) errno = rv;
  return rv;
#undef FIX
}

int getpwnam_r(const char *name, struct passwd *pw, char *buf, size_t size,
               struct passwd **res) {
  return getpw_r(name, 0, pw, buf, size, res);
}

int getpwuid_r(uid_t uid, struct passwd *pw, char *buf, size_t size,
               struct passwd **res) {
  return getpw_r(0, uid, pw, buf, size, res);
}

static struct GetpwentState {
  FILE *f;
  char *line;
  struct passwd pw;
  size_t size;
} g_getpwent[1];

void endpwent() {
  setpwent();
}
void setpwent() {
  if (g_getpwent->f) fclose(g_getpwent->f);
  g_getpwent->f = 0;
}

struct passwd *getpwent() {
  struct passwd *res;
  if (!g_getpwent->f) g_getpwent->f = fopen("/etc/passwd", "rbe");
  if (!g_getpwent->f) return 0;
  __getpwent_a(g_getpwent->f, &g_getpwent->pw, &g_getpwent->line,
               &g_getpwent->size, &res);
  return res;
}

struct passwd *getpwuid(uid_t uid) {
  struct passwd *res;
  __getpw_a(0, uid, &g_getpwent->pw, &g_getpwent->line, &g_getpwent->size,
            &res);
  return res;
}

struct passwd *getpwnam(const char *name) {
  struct passwd *res;
  __getpw_a(name, 0, &g_getpwent->pw, &g_getpwent->line, &g_getpwent->size,
            &res);
  return res;
}
