/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/fmt/conv.h"
#include "libc/fmt/libgen.h"
#include "libc/serialize.h"
#include "libc/log/libfatal.internal.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/stdio/append.h"
#include "libc/str/str.h"
#include "libc/x/xasprintf.h"
#include "third_party/chibicc/chibicc.h"

static void AppendStringLiteral(char **b, const char *s, const char *indent) {
  int c, w, l, o;
  for (o = l = 0;; l = c) {
    switch ((c = *s++ & 255)) {
      case 0:
        return;
      case '\r':
        continue;
      case '\t':
        w = READ16LE("\\t");
        break;
      case '\n':
        w = READ32LE("\\n\\\n");
        break;
      case '"':
        w = READ16LE("\\\"");
        break;
      case '\\':
        w = READ16LE("\\\\");
        break;
      case '`':
        /* convert markdown <code> to restructured text */
        if (o) {
          o = 0;
          w = READ16LE("''");
        } else if (*s == '`') {
          w = '`';
          ++s;
        } else {
          o = 1;
          w = READ16LE("``");
        }
        break;
      default:
        if ((0x00 <= c && c <= 0x1F) || c == 0x7F || (c == '?' && l == '?')) {
          w = '\\';
          w |= ('0' + ((c & 0300) >> 6)) << 010;
          w |= ('0' + ((c & 0070) >> 3)) << 020;
          w |= ('0' + ((c & 0007) >> 0)) << 030;
        } else {
          w = c;
        }
        break;
    }
    appendw(b, w);
    if (c == '\n' && indent) {
      appends(b, indent);
    }
  }
}

static void AppendJavadown(char **b, const struct Javadown *j) {
  size_t i;
  const char *s, *s2;
  if (j->title && *j->title) {
    AppendStringLiteral(b, j->title, 0);
    if (j->text && *j->text) {
      appendw(b, READ32LE("\\n\\\n"));
      appendw(b, READ32LE("\\n\\\n"));
    }
  }
  if (j->text && *j->text) {
    AppendStringLiteral(b, j->text, 0);
  }
  if (j->tags.n) {
    appendw(b, READ32LE("\\n\\\n"));
    for (i = 0; i < j->tags.n; ++i) {
      appendw(b, READ64LE("\\n\\\n:\0\0"));
      AppendStringLiteral(b, j->tags.p[i].tag, 0);
      s = j->tags.p[i].text;
      if (!strcmp(j->tags.p[i].tag, "param") && s && (s2 = strchr(s, ' '))) {
        appendw(b, ' ');
        appendd(b, s, s2 - s);
        s = s2 + 1;
      }
      appendw(b, ':');
      if (s && *s) {
        appendw(b, ' ');
        AppendStringLiteral(b, s, "    ");
      }
    }
  }
}

static void AppendScalar(char **b, struct Type *ty, char *name, int i) {
  if (ty->is_atomic) appendw(b, READ64LE("_Atomic "));
  if (i && ty->is_const) appendw(b, READ64LE("const \0"));
  if (ty->is_unsigned) appends(b, "unsigned ");
  appends(b, name);
}

static void AppendType(char **b, struct Type *ty, int i) {
  switch (ty->kind) {
    case TY_VOID:
      appends(b, "void");
      break;
    case TY_BOOL:
      appends(b, "_Bool");
      break;
    case TY_CHAR:
      AppendScalar(b, ty, "char", i);
      break;
    case TY_SHORT:
      AppendScalar(b, ty, "short", i);
      break;
    case TY_INT:
    case TY_ENUM:
      AppendScalar(b, ty, "int", i);
      break;
    case TY_LONG:
      AppendScalar(b, ty, "long", i);
      break;
    case TY_INT128:
      AppendScalar(b, ty, "__int128", i);
      break;
    case TY_FLOAT:
      AppendScalar(b, ty, "float", i);
      break;
    case TY_DOUBLE:
      AppendScalar(b, ty, "double", i);
      break;
    case TY_LDOUBLE:
      AppendScalar(b, ty, "long double", i);
      break;
    case TY_FUNC:
      AppendType(b, ty->return_ty, i);
      appends(b, " (*)()");
      break;
    case TY_PTR:
      if (!ty->array_len) {
        AppendType(b, ty->base, i + 1);
        if (ty->base->kind != TY_FUNC) {
          appendw(b, '*');
          if (i && ty->is_const) appendw(b, READ64LE(" const\0"));
          if (ty->is_restrict) appends(b, " restrict");
        }
        break;
      }
      /* undecay */
    case TY_ARRAY:
      AppendType(b, ty->base, i + 1);
      appendw(b, '[');
      if (!i && ty->is_static) appendw(b, READ64LE("static "));
      if (!i && ty->is_restrict) appends(b, "restrict ");
      appendf(b, "%lu", ty->array_len);
      appendw(b, ']');
      break;
    default:
      assert(0);
  }
}

static bool IsSupportedReturnType(struct Type *ty) {
  switch (ty->kind) {
    case TY_VOID:
    case TY_BOOL:
    case TY_CHAR:
    case TY_SHORT:
    case TY_INT:
    case TY_LONG:
    case TY_FLOAT:
    case TY_DOUBLE:
      return true;
    case TY_PTR:
      if (ty->base->kind == TY_CHAR) {
        return !ty->base->is_unsigned;
      } else {
        return false;
      }
    default:
      return false;
  }
}

static bool IsSupportedParameterType(struct Type *ty) {
  switch (ty->kind) {
    case TY_BOOL:
    case TY_CHAR:
    case TY_SHORT:
    case TY_INT:
    case TY_LONG:
    case TY_FLOAT:
    case TY_DOUBLE:
      return true;
    case TY_PTR:
      if (ty->base->kind == TY_CHAR) {
        return true;
      } else {
        return false;
      }
    default:
      return false;
  }
}

static bool Reject(char **b, struct Obj *obj, struct Type *ty,
                   const char *reason) {
  appendf(b, "\n/* %s: %s: ", obj->name, reason);
  AppendType(b, ty, 0);
  appendw(b, READ32LE(" */\n"));
  return false;
}

static bool IsFunctionSupported(char **b, struct Obj *func) {
  Obj *param;
  if (!IsSupportedReturnType(func->ty->return_ty)) {
    return Reject(b, func, func->ty->return_ty, "unsupported return type");
  }
  for (param = func->params; param; param = param->next) {
    if (!IsSupportedParameterType(param->ty)) {
      return Reject(b, func, param->ty, "unsupported parameter type");
    }
  }
  return true;
}

static int GetParamDirective(struct Obj **param) {
  bool is_unsigned;
  is_unsigned = (*param)->ty->is_unsigned;
  switch ((*param)->ty->kind) {
    case TY_BOOL:
      return 'p';
    case TY_CHAR:
      return is_unsigned ? 'B' : 'b';
    case TY_SHORT:
      return is_unsigned ? 'H' : 'h';
    case TY_INT:
      return is_unsigned ? 'I' : 'i';
    case TY_LONG:
      return is_unsigned ? 'L' : 'l';
    case TY_FLOAT:
      return 'f';
    case TY_DOUBLE:
      return 'd';
    case TY_PTR:
      if ((*param)->ty->base->kind == TY_CHAR) {
        if ((*param)->ty->base->is_unsigned &&
            ((*param)->next && ((*param)->next->ty->kind == TY_LONG &&
                                (*param)->next->ty->is_unsigned))) {
          *param = (*param)->next;
          return READ16LE("y*");
        } else {
          return READ16LE("s*");
        }
      } else {
        UNREACHABLE();
      }
    default:
      UNREACHABLE();
  }
}

static char *GetParamIntermediate(struct Obj **param) {
  bool is_unsigned;
  is_unsigned = (*param)->ty->is_unsigned;
  switch ((*param)->ty->kind) {
    case TY_BOOL:
      return "int";
    case TY_CHAR:
      return is_unsigned ? "unsigned char" : "signed char";
    case TY_SHORT:
      return is_unsigned ? "unsigned short" : "short";
    case TY_INT:
      return is_unsigned ? "unsigned" : "int";
    case TY_LONG:
      return is_unsigned ? "unsigned long" : "long";
    case TY_FLOAT:
      return "float";
    case TY_DOUBLE:
      return "float";
    case TY_PTR:
      if ((*param)->ty->base->kind == TY_CHAR) {
        *param = (*param)->next;
        return "Py_buffer";
      } else {
        UNREACHABLE();
      }
    default:
      UNREACHABLE();
  }
}

static const char *GetReturnIntermediate(struct Type *ty) {
  bool is_unsigned;
  is_unsigned = ty->is_unsigned;
  switch (ty->kind) {
    case TY_BOOL:
      return "int";
    case TY_CHAR:
      return is_unsigned ? "unsigned char" : "signed char";
    case TY_SHORT:
      return is_unsigned ? "unsigned short" : "short";
    case TY_INT:
      return is_unsigned ? "unsigned" : "int";
    case TY_LONG:
      return is_unsigned ? "unsigned long" : "long";
    case TY_FLOAT:
      return "float";
    case TY_DOUBLE:
      return "float";
    case TY_PTR:
      if (ty->base->kind == TY_CHAR) {
        if (ty->base->is_const) {
          return "const char*";
        } else {
          return "char*";
        }
      } else {
        UNREACHABLE();
      }
    default:
      UNREACHABLE();
  }
}

static void AppendFunction(char **b, Obj *func, const char *module) {
  Obj *param;
  const char *name;
  appendf(b, "\nPyDoc_STRVAR(pb_%s_%s_doc,\n\"%s($module", module, func->name,
          func->name);
  for (param = func->params; param; param = param->next) {
    appendw(b, READ16LE(", "));
    appends(b, param->name);
  }
  appends(b, ")");
  if (func->javadown) {
    appends(b, "\\n\\\n--\\n\\n\\\n");
    AppendJavadown(b, func->javadown->javadown);
  }
  appendw(b, READ32LE("\");\n\n"));
  AppendType(b, func->ty->return_ty, 0);
  appendw(b, ' ');
  appends(b, func->name);
  appendw(b, '(');
  if (func->params) {
    AppendType(b, func->params->ty, 0);
    for (param = func->params->next; param; param = param->next) {
      appendw(b, READ16LE(", "));
      AppendType(b, param->ty, 0);
    }
  } else {
    appendw(b, READ32LE("void"));
  }
  appendw(b, READ32LE(");\n\n"));
  appends(b, "static PyObject*\n");
  appendf(b, "pb_%s_%s(PyObject* self_, PyObject* args_)\n", module,
          func->name);
  appendw(b, READ16LE("{\n"));
  appendw(b, READ32LE("    "));
  appends(b, "PyObject* res_;\n");
  if (func->ty->return_ty->kind != TY_VOID) {
    appendw(b, READ32LE("    "));
    appends(b, GetReturnIntermediate(func->ty->return_ty));
    appendw(b, READ64LE(" ret_;\n"));
  }
  if (func->params) {
    for (param = func->params; param; param = param->next) {
      name = param->name;
      appendw(b, READ32LE("    "));
      appends(b, GetParamIntermediate(&param));
      appendw(b, ' ');
      appends(b, name);
      appendw(b, READ16LE(";\n"));
      if (!param) break;
    }
    appends(b, "    if (!PyArg_ParseTuple(args_, \"");
    for (param = func->params; param; param = param->next) {
      appendw(b, GetParamDirective(&param));
      if (!param) break;
    }
    appendf(b, ":%s\"", func->name);
    for (param = func->params; param; param = param->next) {
      appendf(b, ", &%s", param->name);
    }
    appends(b, ")) return 0;\n");
  }
  appendw(b, READ32LE("    "));
  if (func->ty->return_ty->kind != TY_VOID) {
    appendw(b, READ64LE("ret_ = "));
  }
  appends(b, func->name);
  appendw(b, '(');
  for (param = func->params; param; param = param->next) {
    if (param != func->params) {
      appendw(b, READ16LE(", "));
    }
    appends(b, param->name);
    if (param->ty->kind == TY_PTR && param->ty->base->kind == TY_CHAR) {
      appendw(b, READ32LE(".buf"));
      if (param->ty->base->is_unsigned &&
          (param->next && (param->next->ty->kind == TY_LONG &&
                           param->next->ty->is_unsigned))) {
        appendf(b, ", %s.len", param->name);
        param = param->next;
      }
    }
  }
  appends(b, ");\n");
  switch (func->ty->return_ty->kind) {
    case TY_VOID:
      appends(b, "    res_ = Py_None;\n");
      appends(b, "    Py_INCREF(res_);\n");
      break;
    case TY_BOOL:
      appends(b, "    res_ = ret_ ? Py_True : Py_False;\n");
      appends(b, "    Py_INCREF(res_);\n");
      break;
    case TY_CHAR:
    case TY_SHORT:
    case TY_INT:
      appends(b, "    res_ = PyLong_FromLong(ret_);\n");
      break;
    case TY_LONG:
      if (func->ty->return_ty->is_unsigned) {
        appends(b, "    res_ = PyLong_FromUnsignedLong(ret_);\n");
      } else {
        appends(b, "    res_ = PyLong_FromLong(ret_);\n");
      }
      break;
    case TY_FLOAT:
    case TY_DOUBLE:
      appends(b, "    res_ = PyFloat_FromDouble(ret_);\n");
      break;
    case TY_PTR:
      appends(b, "\
    if (ret_) {\n\
        res_ = PyUnicode_DecodeUTF8(ret_, strlen(ret_), 0);\n\
    } else {\n\
        res_ = Py_None;\n\
        Py_INCREF(res_);\n\
    }\n");
      if (!func->ty->return_ty->base->is_const) {
        appends(b, "    free(res_);\n");
      }
      break;
    default:
      assert(0);
  }
  for (param = func->params; param; param = param->next) {
    if (param->ty->kind == TY_PTR && param->ty->base->kind == TY_CHAR) {
      appendf(b, "    PyBuffer_Release(&%s);\n", param->name);
    }
  }
  appends(b, "    return res_;\n");
  appendw(b, READ16LE("}\n"));
}

void output_bindings_python(const char *path, Obj *prog, Token *tok) {
  int fd;
  Obj *obj;
  char *b = 0;
  char *bm = 0;
  const char *module;
  module = basename(stripexts(strdup(tok->file->name)));
  appends(&b, "\
#define PY_SSIZE_T_CLEAN\n\
#include \"third_party/python/Include/abstract.h\"\n\
#include \"third_party/python/Include/boolobject.h\"\n\
#include \"third_party/python/Include/floatobject.h\"\n\
#include \"third_party/python/Include/import.h\"\n\
#include \"third_party/python/Include/longobject.h\"\n\
#include \"third_party/python/Include/methodobject.h\"\n\
#include \"third_party/python/Include/modsupport.h\"\n\
#include \"third_party/python/Include/moduleobject.h\"\n\
#include \"third_party/python/Include/pymacro.h\"\n\
#include \"third_party/python/Include/pyport.h\"\n\
");
  if (tok->file->javadown) {
    appendf(&b, "\nPyDoc_STRVAR(pb_%s_doc,\n\"", module);
    AppendJavadown(&b, tok->file->javadown);
    appendw(&b, READ32LE("\");\n"));
  }
  for (obj = prog; obj; obj = obj->next) {
    if (obj->is_function) {
      if (obj->is_static) continue;
      if (!obj->is_definition) continue;
      if (*obj->name == '_') continue;
      if (strchr(obj->name, '$')) continue;
      if (!IsFunctionSupported(&b, obj)) continue;
      AppendFunction(&b, obj, module);
      appendf(&bm, "    {\"%s\", pb_%s_%s, %s, pb_%s_%s_doc},\n", obj->name,
              module, obj->name, obj->params ? "METH_VARARGS" : "METH_NOARGS",
              module, obj->name);
    }
  }
  appends(&bm, "    {0},\n");
  appendf(&b, "\nstatic PyMethodDef pb_%s_methods[] = {\n", module);
  appendd(&b, bm, appendz(bm).i);
  appends(&b, "};\n");
  appendf(&b, "\n\
static struct PyModuleDef pb_%s_module = {\n\
    PyModuleDef_HEAD_INIT,\n\
    \"%s\",\n\
    %s,\n\
    -1,\n\
    pb_%s_methods,\n\
};\n\
\n\
PyMODINIT_FUNC\n\
PyInit_%s(void)\n\
{\n\
    return PyModule_Create(&pb_%s_module);\n\
}\n\
\n\
__attribute__((__section__(\".rodata.pytab.1\")))\n\
const struct _inittab _PyImport_Inittab_%s = {\n\
    \"%s\",\n\
    PyInit_%s,\n\
};\n\
",
          module, module,
          tok->file->javadown ? gc(xasprintf("pb_%s_doc", module)) : "0",
          module, module, module, module, module, module);
  CHECK_NE(-1, (fd = creat(path, 0644)));
  CHECK_NE(-1, xwrite(fd, b, appendz(b).i));
  CHECK_NE(-1, close(fd));
  free(bm);
  free(b);
}
