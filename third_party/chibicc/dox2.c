/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/alg/alg.h"
#include "libc/bits/bits.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "third_party/chibicc/chibicc.h"

#define APPEND(L) L.p = realloc(L.p, ++L.n * sizeof(*L.p))

struct Dox {
  unsigned char *p;
  struct Freelist {
    size_t n;
    void **p;
  } freelist;
  struct Set {
    size_t n;
    struct SetEntry {
      unsigned h;
      char *s;
    } * p;
  } names;
  struct DoxObjects {
    size_t n;
    struct DoxObject {
      bool ignore;
      char *type;
      char *name;
      char *path;
      int line;
      bool is_function;
      bool is_variadic;
      bool is_weak;
      bool is_inline;
      bool is_noreturn;
      bool is_destructor;
      bool is_constructor;
      bool is_force_align_arg_pointer;
      bool is_no_caller_saved_registers;
      char *visibility;
      struct Javadown *javadown;
      struct DoxObjectParams {
        size_t n;
        struct DoxObjectParam {
          char *type;
          char *name;
        } * p;
      } params;
    } * p;
  } objects;
  struct DoxMacros {
    size_t n;
    struct DoxMacro {
      bool ignore;
      char *name;
      char *path;
      int line;
      bool is_objlike;
      char *va_args_name;
      struct Javadown *javadown;
      struct DoxMacroParams {
        size_t n;
        struct DoxMacroParam {
          char *name;
        } * p;
      } params;
    } * p;
  } macros;
  struct DoxIndex {
    size_t n;
    struct DoxIndexEntry {
      enum DoxIndexType {
        kObject,
        kMacro,
      } t;
      int i;
    } * p;
  } index;
};

static unsigned Hash(const void *p, unsigned long n) {
  unsigned h, i;
  for (h = i = 0; i < n; i++) {
    h += ((unsigned char *)p)[i];
    h *= 0x9e3779b1;
  }
  return MAX(1, h);
}

static struct Dox *NewDox(void) {
  return calloc(1, sizeof(struct Dox));
}

static void FreeDox(struct Dox *dox) {
  int i;
  if (dox) {
    for (i = 0; i < dox->freelist.n; ++i) {
      free(dox->freelist.p[i]);
    }
    free(dox->names.p);
    free(dox->freelist.p);
    free(dox->objects.p);
    free(dox->macros.p);
    free(dox->index.p);
    free(dox);
  }
}

static void *FreeLater(struct Dox *dox, void *p) {
  APPEND(dox->freelist);
  dox->freelist.p[dox->freelist.n - 1] = p;
  return p;
}

static int DeserializeInt(struct Dox *dox) {
  int x;
  x = (unsigned)dox->p[0] << 000 | (unsigned)dox->p[1] << 010 |
      (unsigned)dox->p[2] << 020 | (unsigned)dox->p[3] << 030;
  dox->p += 4;
  return x;
}

static char *DeserializeStr(struct Dox *dox) {
  char *s;
  size_t n;
  n = DeserializeInt(dox);
  s = malloc(n + 1);
  memcpy(s, dox->p, n);
  s[n] = '\0';
  dox->p += n;
  return FreeLater(dox, s);
}

static struct Javadown *DeserializeJavadown(struct Dox *dox) {
  int i;
  bool present;
  struct Javadown *jd;
  if (DeserializeInt(dox)) {
    jd = FreeLater(dox, calloc(1, sizeof(struct Javadown)));
    jd->isfileoverview = DeserializeInt(dox);
    jd->title = DeserializeStr(dox);
    jd->text = DeserializeStr(dox);
    jd->tags.n = DeserializeInt(dox);
    jd->tags.p = FreeLater(dox, malloc(jd->tags.n * sizeof(*jd->tags.p)));
    for (i = 0; i < jd->tags.n; ++i) {
      jd->tags.p[i].tag = DeserializeStr(dox);
      jd->tags.p[i].text = DeserializeStr(dox);
    }
    return jd;
  } else {
    return NULL;
  }
}

static void DeserializeObject(struct Dox *dox, struct DoxObject *o) {
  int i;
  o->ignore = false;
  o->type = DeserializeStr(dox);
  o->name = DeserializeStr(dox);
  o->path = DeserializeStr(dox);
  o->line = DeserializeInt(dox);
  o->is_function = DeserializeInt(dox);
  o->is_variadic = DeserializeInt(dox);
  o->is_weak = DeserializeInt(dox);
  o->is_inline = DeserializeInt(dox);
  o->is_noreturn = DeserializeInt(dox);
  o->is_destructor = DeserializeInt(dox);
  o->is_constructor = DeserializeInt(dox);
  o->is_force_align_arg_pointer = DeserializeInt(dox);
  o->is_no_caller_saved_registers = DeserializeInt(dox);
  o->visibility = DeserializeStr(dox);
  o->javadown = DeserializeJavadown(dox);
  o->params.n = DeserializeInt(dox);
  o->params.p = FreeLater(dox, malloc(o->params.n * sizeof(*o->params.p)));
  for (i = 0; i < o->params.n; ++i) {
    o->params.p[i].type = DeserializeStr(dox);
    o->params.p[i].name = DeserializeStr(dox);
  }
}

static void DeserializeMacro(struct Dox *dox, struct DoxMacro *m) {
  int i;
  m->ignore = false;
  m->name = DeserializeStr(dox);
  m->path = DeserializeStr(dox);
  m->line = DeserializeInt(dox);
  m->is_objlike = DeserializeInt(dox);
  m->va_args_name = DeserializeStr(dox);
  m->javadown = DeserializeJavadown(dox);
  m->params.n = DeserializeInt(dox);
  m->params.p = FreeLater(dox, malloc(m->params.n * sizeof(*m->params.p)));
  for (i = 0; i < m->params.n; ++i) {
    m->params.p[i].name = DeserializeStr(dox);
  }
}

static void DeserializeDox(struct Dox *dox, const char *path) {
  int i, j, n;
  i = dox->objects.n;
  n = DeserializeInt(dox);
  dox->objects.p =
      realloc(dox->objects.p, (dox->objects.n + n) * sizeof(*dox->objects.p));
  for (j = 0; j < n; ++j) {
    DeserializeObject(dox, dox->objects.p + i + j);
  }
  i = dox->macros.n;
  dox->objects.n += n;
  n = DeserializeInt(dox);
  dox->macros.p =
      realloc(dox->macros.p, (dox->macros.n + n) * sizeof(*dox->macros.p));
  for (j = 0; j < n; ++j) {
    DeserializeMacro(dox, dox->macros.p + i + j);
  }
  dox->macros.n += n;
  CHECK_EQ(31337, DeserializeInt(dox));
}

static void ReadDox(struct Dox *dox, const StringArray *files) {
  int i, fd;
  void *map;
  struct stat st;
  for (i = 0; i < files->len; ++i) {
    CHECK_NE(-1, (fd = open(files->data[i], O_RDONLY)));
    CHECK_NE(-1, fstat(fd, &st));
    if (st.st_size) {
      CHECK_NE(MAP_FAILED,
               (map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0)));
      dox->p = map;
      DeserializeDox(dox, files->data[i]);
      munmap(map, st.st_size);
    }
    close(fd);
  }
}

static bool AddSet(struct Set *set, char *s) {
  unsigned i, h, k;
  h = Hash(s, strlen(s));
  k = 0;
  for (k = 0;; ++k) {
    i = (h + k + ((k + 1) >> 1)) & (set->n - 1);
    if (!set->p[i].h) {
      set->p[i].h = h;
      set->p[i].s = s;
      return true;
    }
    if (h == set->p[i].h && !strcmp(s, set->p[i].s)) {
      return false;
    }
  }
}

static int CompareDoxIndexEntry(const void *p1, const void *p2, void *arg) {
  struct Dox *dox;
  const char *s1, *s2;
  struct DoxIndexEntry *a, *b;
  dox = arg, a = p1, b = p2;
  s1 = a->t == kObject ? dox->objects.p[a->i].name : dox->macros.p[a->i].name;
  s2 = b->t == kObject ? dox->objects.p[b->i].name : dox->macros.p[b->i].name;
  while (*s1 == '_') ++s1;
  while (*s2 == '_') ++s2;
  return strcasecmp(s1, s2);
}

static void IndexDox(struct Dox *dox) {
  size_t i, j, n;
  dox->names.n = roundup2pow(dox->objects.n + dox->macros.n) << 1;
  dox->names.p = calloc(dox->names.n, sizeof(*dox->names.p));
  n = 0;
  for (i = 0; i < dox->objects.n; ++i) {
    if (AddSet(&dox->names, dox->objects.p[i].name)) {
      ++n;
    } else {
      dox->objects.p[i].ignore = true;
    }
  }
  for (i = 0; i < dox->macros.n; ++i) {
    if (AddSet(&dox->names, dox->macros.p[i].name)) {
      ++n;
    } else {
      dox->macros.p[i].ignore = true;
    }
  }
  dox->index.n = n;
  dox->index.p = malloc(n * sizeof(*dox->index.p));
  j = 0;
  for (i = 0; i < dox->objects.n; ++i) {
    if (dox->objects.p[i].ignore) continue;
    dox->index.p[j].t = kObject;
    dox->index.p[j].i = i;
    ++j;
  }
  for (i = 0; i < dox->macros.n; ++i) {
    if (dox->macros.p[i].ignore) continue;
    dox->index.p[j].t = kMacro;
    dox->index.p[j].i = i;
    ++j;
  }
  CHECK_EQ(n, j);
  qsort_r(dox->index.p, dox->index.n, sizeof(*dox->index.p),
          CompareDoxIndexEntry, dox);
}

/**
 * Escapes HTML entities and interprets basic Markdown syntax.
 */
static void PrintText(FILE *f, const char *s) {
  int c;
  bool bol, pre, ul0, ul2, ol0, ol2, bt1, bt2;
  for (bt1 = bt2 = ul2 = ul0 = ol2 = ol0 = pre = false, bol = true;;) {
    switch ((c = *s++)) {
      case '\0':
        if (bt1 || bt2) fprintf(f, "</code>");
        if (pre) fprintf(f, "</pre>");
        if (ul0 || ul2) fprintf(f, "</ul>");
        if (ol0 || ol2) fprintf(f, "</ol>");
        return;
      case '&':
        fprintf(f, "&amp;");
        bol = false;
        break;
      case '<':
        fprintf(f, "&lt;");
        bol = false;
        break;
      case '>':
        fprintf(f, "&gt;");
        bol = false;
        break;
      case '"':
        fprintf(f, "&quot;");
        bol = false;
        break;
      case '\'':
        fprintf(f, "&apos;");
        bol = false;
        break;
      case '`':
        if (!pre && !bt1 && !bt2 && *s != '`') {
          fprintf(f, "<code>");
          bt1 = true;
        } else if (!pre && !bt1 && !bt2 && *s == '`') {
          fprintf(f, "<code>");
          bt2 = true;
          ++s;
        } else if (bt1) {
          fprintf(f, "</code>");
          bt1 = false;
        } else if (bt2 && *s == '`') {
          fprintf(f, "</code>");
          bt2 = false;
          ++s;
        } else {
          fprintf(f, "`");
        }
        bol = false;
        break;
      case '\n':
        if (!pre && !ul0 && !ul2 && !ol0 && !ol2 && *s == '\n') {
          fprintf(f, "\n<p>");
          bol = true;
        } else if (pre && s[0] != '\n') {
          if (s[0] != ' ' || s[1] != ' ' || s[2] != ' ' || s[3] != ' ') {
            fprintf(f, "</pre>\n");
            pre = false;
            bol = true;
          } else {
            fprintf(f, "\n");
            bol = false;
            s += 4;
          }
        } else if (ul0 && s[0] == '-' && s[1] == ' ') {
          fprintf(f, "\n<li>");
          s += 2;
          bol = false;
        } else if (ul2 && s[0] == ' ' && s[1] == ' ' && s[2] == '-' &&
                   s[3] == ' ') {
          fprintf(f, "\n<li>");
          s += 4;
          bol = false;
        } else if (ul0 && s[0] != '\n' && (s[0] != ' ' || s[1] != ' ')) {
          fprintf(f, "\n</ul>\n");
          bol = true;
          ul0 = false;
        } else if (ul2 && s[0] != '\n' &&
                   (s[0] != ' ' || s[1] != ' ' || s[2] != ' ' || s[3] != ' ')) {
          fprintf(f, "\n</ul>\n");
          bol = true;
          ul2 = false;
        } else if (ol0 && ('0' <= s[0] && s[0] <= '9') && s[1] == '.' &&
                   s[2] == ' ') {
          fprintf(f, "\n<li>");
          s += 3;
          bol = false;
        } else if (ol2 && s[0] == ' ' && s[1] == ' ' &&
                   ('0' <= s[2] && s[2] <= '9') && s[3] == '.' && s[3] == ' ') {
          fprintf(f, "\n<li>");
          s += 5;
          bol = false;
        } else if (ol0 && s[0] != '\n' && (s[0] != ' ' || s[1] != ' ')) {
          fprintf(f, "\n</ol>\n");
          bol = true;
          ol0 = false;
        } else if (ol2 && s[0] != '\n' &&
                   (s[0] != ' ' || s[1] != ' ' || s[2] != ' ' || s[3] != ' ')) {
          fprintf(f, "\n</ol>\n");
          bol = true;
          ol2 = false;
        } else {
          fprintf(f, "\n");
          bol = true;
        }
        break;
      case '-':
        if (bol && !ul0 && !ul2 && !ol0 && !ol2 && s[0] == ' ') {
          ul0 = true;
          fprintf(f, "<ul><li>");
          ++s;
        } else {
          fprintf(f, "-");
        }
        bol = false;
        break;
      case '1':
        if (bol && !ol0 && !ol2 && !ul0 && !ul2 && s[0] == '.' && s[1] == ' ') {
          ol0 = true;
          fprintf(f, "<ol><li>");
          s += 2;
        } else {
          fprintf(f, "1");
        }
        bol = false;
        break;
      case ' ':
        if (bol && !pre && s[0] == ' ' && s[1] == ' ' && s[2] == ' ') {
          pre = true;
          fprintf(f, "<pre>");
          s += 3;
        } else if (bol && !ul0 && !ul2 && !ol0 && !ol2 && s[0] == ' ' &&
                   s[1] == '-' && s[2] == ' ') {
          ul2 = true;
          fprintf(f, "<ul><li>");
          s += 3;
        } else if (bol && !ul0 && !ul2 && !ol0 && !ol2 && s[0] == ' ' &&
                   ('0' <= s[1] && s[1] <= '9') && s[2] == '.' && s[3] == ' ') {
          ol2 = true;
          fprintf(f, "<ol><li>");
          s += 4;
        } else {
          fprintf(f, " ");
        }
        bol = false;
        break;
      default:
        fprintf(f, "%c", c);
        bol = false;
        break;
    }
  }
}

static bool HasTag(struct Javadown *jd, const char *tag) {
  int k;
  if (jd) {
    for (k = 0; k < jd->tags.n; ++k) {
      if (!strcmp(jd->tags.p[k].tag, tag)) {
        return true;
      }
    }
  }
  return false;
}

static bool IsNoReturn(struct DoxObject *o) {
  return o->is_noreturn || HasTag(o->javadown, "noreturn");
}

static void PrintDox(struct Dox *dox, FILE *f) {
  int i, j, k;
  char *prefix;
  bool was_outputted;
  struct DoxMacro *m;
  struct DoxObject *o;

  // header
  fprintf(f, "\
<!doctype html>\n\
<html lang=\"en\">\n\
<meta charset=\"utf-8\">\n\
<script async src=\"https://www.googletagmanager.com/gtag/js?id=UA-43182592-5\"></script>\n\
<script>window.dataLayer = window.dataLayer || []; function gtag(){dataLayer.push(arguments);} gtag('js', new Date()); gtag('config', 'UA-43182592-5');</script>\n\
<title>Cosmopolitan C Library</title>\n\
<meta name=\"viewport\" content=\"width=1024\">\n\
<link rel=\"canonical\" href=\"https://justine.lol/cosmopolitan/documentation.html\">\n\
<link rel=\"stylesheet\" href=\"https://fonts.googleapis.com/css2?family=Roboto&display=swap\">\n\
<link rel=\"stylesheet\" href=\"style.css\">\n\
<style>\n\
  .nav {\n\
    margin-bottom: 0;\n\
  }\n\
  .toc {\n\
    overflow-x: auto;\n\
  }\n\
  .toc a {\n\
    text-decoration: none;\n\
  }\n\
  h3 a {\n\
    color: inherit;\n\
    text-decoration: none;\n\
  }\n\
  pre {\n\
    margin-left: 0;\n\
    padding: 12px;\n\
    background: #f6f6f6;\n\
    width: 100%;\n\
    overflow-x: auto;\n\
    border-radius: 5px;\n\
  }\n\
  code {\n\
    padding: 2px 4px;\n\
    background: #e4e6e8;\n\
    border-radius: 3px;\n\
  }\n\
  hr {\n\
    height: 1px;\n\
    margin-bottom: 16px;\n\
    color: #d6d9dc;\n\
    background: #d6d9dc;\n\
    border: 0;\n\
  }\n\
  .category {\n\
    font-weight: bold;\n\
  }\n\
  .tagname {\n\
    font-size: 12pt;\n\
    font-weight: bold;\n\
  }\n\
  .tag {\n\
    margin-top: .5em;\n\
  }\n\
  .tag dl {\n\
    margin-top: .5em;\n\
    margin-bottom: .5em;\n\
    margin-left: 1em;\n\
  }\n\
</style>\n\
\n\
<header>\n\
  <img width=\"196\" height=\"105\"\n\
       src=\"//storage.googleapis.com/justine/cosmopolitan/cosmopolitan.png\"\n\
       title=\"cosmopolitan honeybadger\"\n\
       alt=\"honeybadger\">\n\
  <h1>cosmopolitan libc</h1>\n\
  <span>build-once run-anywhere c without devops</span>\n\
</header>\n\
\n\
<nav class=\"nav\">\n\
  <ul>\n\
    <li><a href=\"index.html\">Intro</a>\n\
    <li><a href=\"download.html\">Download</a>\n\
    <li><a class=\"active\" href=\"documentation.html\">Documentation</a>\n\
    <li><a href=\"sources.html\">Sources</a>\n\
    <li><a href=\"https://github.com/jart/cosmopolitan\">GitHub</a>\n\
    <li><a href=\"license.html\">License</a>\n\
    <li class=\"right\"><a href=\"../index.html\">» jart's web page</a>\n\
  </ul>\n\
</nav>\n\
\n\
<table class=\"dox\" width=\"960\">\n\
<tr>\n\
<td width=\"283\" valign=\"top\" class=\"toc\">\n\
");

  /* // lefthand index: objects */
  /* fprintf(f, "<p><span class=\"category\">macro objects</span>\n"); */
  /* fprintf(f, "<p>\n"); */
  /* for (i = 0; i < dox->index.n; ++i) { */
  /*   if (dox->index.p[i].t != kMacro) continue; */
  /*   m = dox->macros.p + dox->index.p[i].i; */
  /*   if (m->ignore) continue; */
  /*   if (!m->is_objlike) continue; */
  /*   fprintf(f, "<a href=\"#%s\">%s</a><br>\n", m->name, m->name); */
  /* } */

  /* // lefthand index: functions */
  /* fprintf(f, "<p><span class=\"category\">macro functions</span>\n"); */
  /* fprintf(f, "<p>\n"); */
  /* for (i = 0; i < dox->index.n; ++i) { */
  /*   if (dox->index.p[i].t != kMacro) continue; */
  /*   m = dox->macros.p + dox->index.p[i].i; */
  /*   if (m->ignore) continue; */
  /*   if (m->is_objlike) continue; */
  /*   fprintf(f, "<a href=\"#%s\">%s</a><br>\n", m->name, m->name); */
  /* } */

  // lefthand index: objects
  fprintf(f, "<p><span class=\"category\">objects</span>\n");
  fprintf(f, "<p>\n");
  for (i = 0; i < dox->index.n; ++i) {
    if (dox->index.p[i].t != kObject) continue;
    o = dox->objects.p + dox->index.p[i].i;
    if (o->ignore) continue;
    if (o->is_function) continue;
    fprintf(f, "<a href=\"#%s\">%s</a><br>\n", o->name, o->name);
  }

  // lefthand index: functions
  fprintf(f, "<p><span class=\"category\">functions</span>\n");
  fprintf(f, "<p>\n");
  for (i = 0; i < dox->index.n; ++i) {
    if (dox->index.p[i].t != kObject) continue;
    o = dox->objects.p + dox->index.p[i].i;
    if (o->ignore) continue;
    if (!o->is_function) continue;
    fprintf(f, "<a href=\"#%s\">%s</a><br>\n", o->name, o->name);
  }

  // righthand contents
  fprintf(f, "<td width=\"667\" valign=\"top\">\n");
  for (i = 0; i < dox->index.n; ++i) {
    if (dox->index.p[i].t == kObject) {
      o = dox->objects.p + dox->index.p[i].i;
      if (o->ignore) continue;
      fprintf(f, "\n");
      if (i) fprintf(f, "<hr>");
      fprintf(f, "<div id=\"%s\" class=\"api\">\n", o->name);
      fprintf(f, "<h3><a href=\"#%s\">%s</a></h3>", o->name, o->name);

      // title
      if (o->javadown && *o->javadown->title) {
        fprintf(f, "<p>");
        PrintText(f, o->javadown->title);
        fprintf(f, "\n");
      }

      // text
      if (o->javadown && *o->javadown->text) {
        fprintf(f, "<p>");
        PrintText(f, o->javadown->text);
        fprintf(f, "\n");
      }

      // parameters
      if (o->is_function &&
          (o->is_variadic || o->params.n || HasTag(o->javadown, "param"))) {
        fprintf(f, "<div class=\"tag\">\n");
        fprintf(f, "<span class=\"tagname\">@param</span>\n");
        fprintf(f, "<dl>\n");
        if (o->params.n) {
          for (j = 0; j < o->params.n; ++j) {
            fprintf(f, "<dt>");
            PrintText(f, o->params.p[j].type);
            fprintf(f, " <em>");
            PrintText(f, o->params.p[j].name);
            fprintf(f, "</em>\n");
            if (o->javadown) {
              prefix = xasprintf("%s ", o->params.p[j].name);
              for (k = 0; k < o->javadown->tags.n; ++k) {
                if (!strcmp(o->javadown->tags.p[k].tag, "param") &&
                    startswith(o->javadown->tags.p[k].text, prefix)) {
                  fprintf(f, "<dd>");
                  PrintText(f, o->javadown->tags.p[k].text + strlen(prefix));
                  fprintf(f, "\n");
                  break;
                }
              }
              free(prefix);
            }
          }
        } else {
          for (k = 0; k < o->javadown->tags.n; ++k) {
            if (!strcmp(o->javadown->tags.p[k].tag, "param")) {
              fprintf(f, "<dd>");
              PrintText(f, o->javadown->tags.p[k].text);
              fprintf(f, "\n");
              break;
            }
          }
        }
        if (o->is_variadic) {
          fprintf(f, "<dt>...\n");
        }
        fprintf(f, "</dl>\n");
        fprintf(f, "</div>\n");  // .tag
      }

      // return
      if (o->is_function) {
        fprintf(f, "<div class=\"tag\">\n");
        if (IsNoReturn(o)) {
          fprintf(f, "<span class=\"tagname\">@noreturn</span>\n");
        } else {
          fprintf(f, "<span class=\"tagname\">@return</span>\n");
          was_outputted = false;
          fprintf(f, "<dl>\n");
          if (o->javadown) {
            for (k = 0; k < o->javadown->tags.n; ++k) {
              if (strcmp(o->javadown->tags.p[k].tag, "return")) continue;
              if (!was_outputted) {
                fprintf(f, "<dt>");
                PrintText(f, o->type);
                was_outputted = true;
              }
              fprintf(f, "\n<dd>");
              PrintText(f, o->javadown->tags.p[k].text);
              fprintf(f, "\n");
            }
          }
          if (!was_outputted) {
            fprintf(f, "<dt>");
            PrintText(f, o->type);
          }
          fprintf(f, "</dl>\n");
          fprintf(f, "</div>\n");  // .tag
        }
      }

      // type
      if (!o->is_function) {
        fprintf(f, "<div class=\"tag\">\n");
        fprintf(f, "<span class=\"tagname\">@type</span>\n");
        fprintf(f, "<dl>\n");
        fprintf(f, "<dt>");
        PrintText(f, o->type);
        fprintf(f, "</dl>\n");
        fprintf(f, "</div>\n");  // .tag
      }

      // tags
      if (o->javadown) {
        for (k = 0; k < o->javadown->tags.n; ++k) {
          if (!strcmp(o->javadown->tags.p[k].tag, "param")) continue;
          if (!strcmp(o->javadown->tags.p[k].tag, "return")) continue;
          if (!strcmp(o->javadown->tags.p[k].tag, "noreturn")) continue;
          fprintf(f, "<div class=\"tag\">\n");
          fprintf(f, "<span class=\"tagname\">@");
          PrintText(f, o->javadown->tags.p[k].tag);
          fprintf(f, "</span>\n");
          if (*o->javadown->tags.p[k].text) {
            PrintText(f, o->javadown->tags.p[k].text);
            fprintf(f, "\n");
          }
          fprintf(f, "</div>\n");  // .tag
        }
      }

      // sauce
      if (strcmp(o->path, "missingno.c")) {
        fprintf(f, "<div class=\"tag\">\n");
        fprintf(f,
                "<span class=\"tagname\">@see</span> <a "
                "href=\"https://github.com/jart/cosmopolitan/blob/master/"
                "%s#L%d\">%s</a>",
                o->path, o->line, o->path);
        fprintf(f, "</div>\n");  // .tag
      }

      fprintf(f, "</div>\n"); /* class=".api" */
    } else {
      continue;
      m = dox->macros.p + dox->index.p[i].i;
      if (m->ignore) continue;
      fprintf(f, "\n");
      if (i) fprintf(f, "<hr>");
      fprintf(f, "<div id=\"%s\" class=\"api\">\n", m->name);
      fprintf(f, "<h3><a href=\"#%s\">%s</a></h3>", m->name, m->name);
      // title
      if (m->javadown && *m->javadown->title) {
        fprintf(f, "<p>");
        PrintText(f, m->javadown->title);
        fprintf(f, "\n");
      }
      // text
      if (m->javadown && *m->javadown->text) {
        fprintf(f, "<p>");
        PrintText(f, m->javadown->text);
        fprintf(f, "\n");
      }
      // parameters
      if (!m->is_objlike && (m->params.n || HasTag(m->javadown, "param"))) {
        fprintf(f, "<div class=\"tag\">\n");
        fprintf(f, "<span class=\"tagname\">@param</span>\n");
        fprintf(f, "<dl>\n");
        if (m->params.n) {
          for (j = 0; j < m->params.n; ++j) {
            fprintf(f, "<dt>");
            fprintf(f, "<em>");
            PrintText(f, m->params.p[j].name);
            fprintf(f, "</em>\n");
            if (m->javadown) {
              prefix = xasprintf("%s ", m->params.p[j].name);
              for (k = 0; k < m->javadown->tags.n; ++k) {
                if (!strcmp(m->javadown->tags.p[k].tag, "param") &&
                    startswith(m->javadown->tags.p[k].text, prefix)) {
                  fprintf(f, "<dd>");
                  PrintText(f, m->javadown->tags.p[k].text + strlen(prefix));
                  fprintf(f, "\n");
                  break;
                }
              }
              free(prefix);
            }
          }
        } else {
          for (k = 0; k < m->javadown->tags.n; ++k) {
            if (!strcmp(m->javadown->tags.p[k].tag, "param")) {
              fprintf(f, "<dd>");
              PrintText(f, m->javadown->tags.p[k].text);
              fprintf(f, "\n");
              break;
            }
          }
        }
        fprintf(f, "</dl>\n");
        fprintf(f, "</div>\n");  // .tag
      }
      fprintf(f, "</div>\n"); /* class=".api" */
    }
  }
  fprintf(f, "</table>\n");

  // footer
  fprintf(f, "\
\n\
<footer>\n\
  <p>\n\
    <div style=\"float:right;text-align:right\">\n\
      Free Libre &amp; Open Source<br>\n\
      <a href=\"https://github.com/jart\">github.com/jart/cosmopolitan</a>\n\
    </div>\n\
    Feedback<br>\n\
    jtunney@gmail.com\n\
  </p>\n\
  <div style=\"clear:both\"></div>\n\
</footer>\n\
");
}

/**
 * Merges documentation data and outputs HTML.
 */
void drop_dox(const StringArray *files, const char *path) {
  FILE *f;
  struct Dox *dox;
  dox = NewDox();
  ReadDox(dox, files);
  IndexDox(dox);
  f = fopen(path, "w");
  PrintDox(dox, f);
  fclose(f);
  FreeDox(dox);
}
