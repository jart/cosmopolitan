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
#include "libc/runtime/gc.h"
#include "third_party/chibicc/chibicc.h"

#define APPEND(L) L.p = realloc(L.p, ++L.n * sizeof(*L.p))

struct DoxWriter {
  struct Buffer {
    size_t n;
    char *p;
  } buf;
  struct Macros {
    size_t n;
    Macro **p;
  } macros;
  struct Objects {
    size_t n;
    Obj **p;
  } objects;
};

static void SerializeData(struct Buffer *buf, const void *p, unsigned long n) {
  struct Slice *s;
  buf->p = realloc(buf->p, buf->n + n);
  memcpy(buf->p + buf->n, p, n);
  buf->n += n;
}

static int SerializeInt(struct Buffer *buf, int x) {
  unsigned char b[4];
  b[0] = x >> 000;
  b[1] = x >> 010;
  b[2] = x >> 020;
  b[3] = x >> 030;
  SerializeData(buf, b, 4);
  return x;
}

static void SerializeStr(struct Buffer *buf, const char *s) {
  size_t n;
  if (!s) s = "";
  n = strlen(s);
  n = MIN(INT_MAX, n);
  SerializeInt(buf, n);
  SerializeData(buf, s, n);
}

static void SerializeJavadown(struct Buffer *buf, struct Javadown *jd) {
  int i;
  SerializeInt(buf, jd->isfileoverview);
  SerializeStr(buf, jd->title);
  SerializeStr(buf, jd->text);
  SerializeInt(buf, jd->tags.n);
  for (i = 0; i < jd->tags.n; ++i) {
    SerializeStr(buf, jd->tags.p[i].tag);
    SerializeStr(buf, jd->tags.p[i].text);
  }
}

static char *DescribeScalar(struct Type *ty, char *name) {
  return xasprintf("%s%s%s", ty->is_atomic ? "_Atomic " : "",
                   ty->is_unsigned ? "unsigned " : "", name);
}

static char *DescribeType(struct Type *ty) {
  switch (ty->kind) {
    case TY_VOID:
      return strdup("void");
    case TY_BOOL:
      return strdup("_Bool");
    case TY_CHAR:
      return DescribeScalar(ty, "char");
    case TY_SHORT:
      return DescribeScalar(ty, "short");
    case TY_INT:
      return DescribeScalar(ty, "int");
    case TY_LONG:
      return DescribeScalar(ty, "long");
    case TY_INT128:
      return DescribeScalar(ty, "__int128");
    case TY_FLOAT:
      return DescribeScalar(ty, "float");
    case TY_DOUBLE:
      return DescribeScalar(ty, "double");
    case TY_LDOUBLE:
      return DescribeScalar(ty, "long double");
    case TY_PTR:
      return xasprintf("%s*", gc(DescribeType(ty->base)));
    case TY_ARRAY:
      return xasprintf("%s[%d]", gc(DescribeType(ty->base)), ty->array_len);
    case TY_ENUM:
      if (ty->name_pos) {
        return xasprintf("enum %.*s", ty->name_pos->len, ty->name_pos->loc);
      } else {
        return strdup("ANONYMOUS-ENUM");
      }
    case TY_STRUCT:
      if (ty->name_pos) {
        return xasprintf("struct %.*s", ty->name_pos->len, ty->name_pos->loc);
      } else {
        return strdup("ANONYMOUS-STRUCT");
      }
    case TY_UNION:
      if (ty->name_pos) {
        return xasprintf("union %.*s", ty->name_pos->len, ty->name_pos->loc);
      } else {
        return strdup("ANONYMOUS-UNION");
      }
    case TY_FUNC:
      return xasprintf("%s(*)()", gc(DescribeType(ty->return_ty)));
    default:
      return "UNKNOWN";
  }
}

static int CountParams(Obj *params) {
  int n;
  for (n = 0; params; params = params->next) ++n;
  return n;
}

static const char *GetFileName(Obj *obj) {
  if (obj->javadown && obj->javadown->file) return obj->javadown->file->name;
  if (obj->tok && obj->tok->file) return obj->tok->file->name;
  return "missingno.c";
}

static int GetLine(Obj *obj) {
  if (obj->javadown && obj->javadown->file) return obj->javadown->line_no;
  if (obj->tok && obj->tok->file) return obj->tok->line_no;
  return 0;
}

static void SerializeDox(struct DoxWriter *dox, Obj *prog) {
  int i;
  char *s;
  Obj *param;
  MacroParam *mparam;
  SerializeInt(&dox->buf, dox->objects.n);
  for (i = 0; i < dox->objects.n; ++i) {
    s = DescribeType(dox->objects.p[i]->ty);
    SerializeStr(&dox->buf, s);
    free(s);
    SerializeStr(&dox->buf, dox->objects.p[i]->name);
    SerializeStr(&dox->buf, GetFileName(dox->objects.p[i]));
    SerializeInt(&dox->buf, GetLine(dox->objects.p[i]));
    SerializeInt(&dox->buf, dox->objects.p[i]->is_function);
    SerializeInt(&dox->buf, dox->objects.p[i]->is_weak);
    SerializeInt(&dox->buf, dox->objects.p[i]->is_inline);
    SerializeInt(&dox->buf, dox->objects.p[i]->is_noreturn);
    SerializeInt(&dox->buf, dox->objects.p[i]->is_destructor);
    SerializeInt(&dox->buf, dox->objects.p[i]->is_constructor);
    SerializeInt(&dox->buf, dox->objects.p[i]->is_force_align_arg_pointer);
    SerializeInt(&dox->buf, dox->objects.p[i]->is_no_caller_saved_registers);
    SerializeStr(&dox->buf, dox->objects.p[i]->visibility);
    SerializeJavadown(&dox->buf, dox->objects.p[i]->javadown->javadown);
    SerializeInt(&dox->buf, CountParams(dox->objects.p[i]->params));
    for (param = dox->objects.p[i]->params; param; param = param->next) {
      s = DescribeType(param->ty);
      SerializeStr(&dox->buf, s);
      free(s);
      SerializeStr(&dox->buf, param->name);
    }
  }
  SerializeInt(&dox->buf, dox->macros.n);
  for (i = 0; i < dox->macros.n; ++i) {
    SerializeStr(&dox->buf, dox->macros.p[i]->name);
    SerializeStr(&dox->buf, dox->macros.p[i]->javadown->file->name);
    SerializeInt(&dox->buf, dox->macros.p[i]->javadown->line_no);
    SerializeJavadown(&dox->buf, dox->macros.p[i]->javadown->javadown);
  }
  SerializeInt(&dox->buf, 31337);
}

static void LoadPublicDefinitions(struct DoxWriter *dox, Obj *prog) {
  int i;
  Obj *obj;
  Macro *macro;
  for (obj = prog; obj; obj = obj->next) {
    if (obj->is_static) continue;
    if (*obj->name == '_') continue;
    if (!obj->javadown) continue;
    if (obj->is_string_literal) continue;
    if (obj->visibility && !strcmp(obj->visibility, "hidden")) continue;
    if (strchr(obj->name, '$')) continue;
    APPEND(dox->objects);
    dox->objects.p[dox->objects.n - 1] = obj;
  }
  for (i = 0; i < macros.capacity; ++i) {
    if (!macros.buckets[i].key) continue;
    if (macros.buckets[i].key == (char *)-1) continue;
    macro = macros.buckets[i].val;
    if (!macro->javadown) continue;
    if (!macro->javadown->javadown) continue;
    if (*macro->name == '_') continue;
    if (strchr(macro->name, '$')) continue;
    APPEND(dox->macros);
    dox->macros.p[dox->macros.n - 1] = macro;
  }
}

static struct DoxWriter *NewDoxWriter(void) {
  return calloc(1, sizeof(struct DoxWriter));
}

static void FreeDoxWriter(struct DoxWriter *dox) {
  if (dox) {
    free(dox->buf.p);
    free(dox->macros.p);
    free(dox->objects.p);
    free(dox);
  }
}

static void WriteDox(struct DoxWriter *dox, const char *path) {
  int fd;
  CHECK_NE(-1, (fd = creat(path, 0644)));
  CHECK_EQ(dox->buf.n, write(fd, dox->buf.p, dox->buf.n));
  close(fd);
}

/**
 * Emits documentation datum for compilation unit just parsed.
 */
void output_javadown(const char *path, Obj *prog) {
  struct DoxWriter *dox = NewDoxWriter();
  LoadPublicDefinitions(dox, prog);
  SerializeDox(dox, prog);
  WriteDox(dox, path);
  FreeDoxWriter(dox);
}
