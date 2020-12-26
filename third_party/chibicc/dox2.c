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
  struct {
    size_t n;
    int *p;
  } objectindex;
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
  struct Javadown *jd;
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
}

static void DeserializeObject(struct Dox *dox, struct DoxObject *o) {
  int i;
  o->ignore = false;
  o->type = DeserializeStr(dox);
  o->name = DeserializeStr(dox);
  o->path = DeserializeStr(dox);
  o->line = DeserializeInt(dox);
  o->is_function = DeserializeInt(dox);
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

static void DeserializeDox(struct Dox *dox) {
  int i, j, n;
  i = dox->objects.n;
  n = DeserializeInt(dox);
  dox->objects.p =
      realloc(dox->objects.p, (dox->objects.n + n) * sizeof(*dox->objects.p));
  for (j = 0; j < n; ++j) {
    DeserializeObject(dox, dox->objects.p + i + j);
  }
  dox->objects.n += n;
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
      DeserializeDox(dox);
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

static int CompareObjectNames(const void *a, const void *b, void *arg) {
  int *i1, *i2;
  struct Dox *dox;
  i1 = a, i2 = b, dox = arg;
  return strcmp(dox->objects.p[*i1].name, dox->objects.p[*i2].name);
}

static void IndexDox(struct Dox *dox) {
  size_t i, j, n;
  dox->names.n = roundup2pow(dox->objects.n) << 1;
  dox->names.p = calloc(dox->names.n, sizeof(*dox->names.p));
  for (n = i = 0; i < dox->objects.n; ++i) {
    if (AddSet(&dox->names, dox->objects.p[i].name)) {
      ++n;
    } else {
      dox->objects.p[i].ignore = true;
    }
  }
  dox->objectindex.n = n;
  dox->objectindex.p = malloc(n * sizeof(*dox->objectindex.p));
  for (j = i = 0; i < dox->objects.n; ++i) {
    if (dox->objects.p[i].ignore) continue;
    dox->objectindex.p[j++] = i;
  }
  qsort_r(dox->objectindex.p, dox->objectindex.n, sizeof(*dox->objectindex.p),
          CompareObjectNames, dox);
}

static void PrintText(FILE *f, const char *s) {
  int c;
  bool bol, pre;
  for (pre = false, bol = true;;) {
    switch ((c = *s++)) {
      case '\0':
        if (pre) {
          fprintf(f, "</pre>");
        }
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
      case '\n':
        if (!pre && *s == '\n') {
          ++s;
          fprintf(f, "\n<p>");
        } else if (pre &&
                   (s[0] != ' ' || s[1] != ' ' || s[2] != ' ' || s[3] != ' ')) {
          fprintf(f, "</pre>\n");
          pre = false;
        } else {
          fprintf(f, "\n");
        }
        bol = true;
        break;
      case ' ':
        if (bol && !pre && s[0] == ' ' && s[1] == ' ' && s[2] == ' ') {
          pre = true;
          fprintf(f, "<pre>");
        }
        fprintf(f, " ");
        bol = false;
        break;
      default:
        fprintf(f, "%c", c);
        bol = false;
        break;
    }
  }
}

static void PrintDox(struct Dox *dox, FILE *f) {
  int i, j, k;
  char *prefix;
  struct DoxObject *o;
  fprintf(f, "\
<!doctype html>\n\
<meta charset=\"utf-8\">\n\
<style>\n\
.indent {\n\
  padding-left: 1em;\n\
}\n\
</style>\n\
\n\
<table width=\"100%%\"><tr><td width=\"33%%\" valign=\"top\">\n\
<p class=\"toc\">\n\
");
  for (i = 0; i < dox->objectindex.n; ++i) {
    o = dox->objects.p + dox->objectindex.p[i];
    if (o->ignore || !o->is_function) continue;
    fprintf(f, "<a href=\"#%s\">%s</a><br>\n", o->name, o->name);
    fprintf(f, "<br>\n");
  }
  fprintf(f, "<td width=\"67%%\" valign=\"top\">\n");
  for (i = 0; i < dox->objectindex.n; ++i) {
    o = dox->objects.p + dox->objectindex.p[i];
    if (o->ignore || !o->is_function) continue;
    fprintf(f, "\n<div id=\"%s\" class=\"func\">\n", o->name, o->name);
    fprintf(f, "<h3><a href=\"#%s\">", o->name);
    fprintf(f, "<strong class=\"name\">%s</strong></a></h3>", o->name);
    fprintf(f, "<p>");
    PrintText(f, o->javadown->title);
    fprintf(f, "\n");
    if (*o->javadown->text) {
      fprintf(f, "<p>");
      PrintText(f, o->javadown->text);
      fprintf(f, "\n");
    }
    fprintf(f, "<p><strong>@param</strong>\n");
    fprintf(f, "<div class=\"params indent\">\n");
    if (o->params.n) {
      fprintf(f, "<dl>\n");
      for (j = 0; j < o->params.n; ++j) {
        fprintf(f, "<dt>");
        PrintText(f, o->params.p[j].type);
        fprintf(f, " <em>");
        PrintText(f, o->params.p[j].name);
        fprintf(f, "</em>\n");
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
      fprintf(f, "</dl>\n");
    } else {
      fprintf(f, "<p>None.\n");
    }
    fprintf(f, "</div>\n");
    for (k = 0; k < o->javadown->tags.n; ++k) {
      if (!strcmp(o->javadown->tags.p[k].tag, "param")) continue;
      fprintf(f, "<p><strong>@");
      PrintText(f, o->javadown->tags.p[k].tag);
      fprintf(f, "</strong>\n");
      if (*o->javadown->tags.p[k].text) {
        PrintText(f, o->javadown->tags.p[k].text);
        fprintf(f, "\n");
      }
    }
    fprintf(f, "</div>\n");
  }
  fprintf(f, "</table>\n");
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
