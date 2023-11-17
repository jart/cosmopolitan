/* Tcl in ~ 500 lines of code.
 *
 * Copyright (c) 2007-2016, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Original from http://oldblog.antirez.com/page/picol.html
 * Changes on 2021-12-27 by octetta :
 * . Use Cosmopolitan's headers.
 * . Formatted as per Cosmopolitan's standards.
 */

#include "libc/fmt/conv.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

enum { PICOL_OK, PICOL_ERR, PICOL_RETURN, PICOL_BREAK, PICOL_CONTINUE };
enum { PT_ESC, PT_STR, PT_CMD, PT_VAR, PT_SEP, PT_EOL, PT_EOF };

struct picolParser {
  char *text;
  char *p;         /* current text position */
  int len;         /* remaining length */
  char *start;     /* token start */
  char *end;       /* token end */
  int type;        /* token type, PT_... */
  int insidequote; /* True if inside " " */
};

struct picolVar {
  char *name, *val;
  struct picolVar *next;
};

struct picolInterp; /* forward declaration */
typedef int (*picolCmdFunc)(struct picolInterp *i, int argc, char **argv,
                            void *privdata);

struct picolCmd {
  char *name;
  picolCmdFunc func;
  void *privdata;
  struct picolCmd *next;
};

struct picolCallFrame {
  struct picolVar *vars;
  struct picolCallFrame *parent; /* parent is NULL at top level */
};

struct picolInterp {
  int level; /* Level of nesting */
  struct picolCallFrame *callframe;
  struct picolCmd *commands;
  char *result;
};

void picolInitParser(struct picolParser *p, char *text) {
  p->text = p->p = text;
  p->len = strlen(text);
  p->start = 0;
  p->end = 0;
  p->insidequote = 0;
  p->type = PT_EOL;
}

int picolParseSep(struct picolParser *p) {
  p->start = p->p;
  while (*p->p == ' ' || *p->p == '\t' || *p->p == '\n' || *p->p == '\r') {
    p->p++;
    p->len--;
  }
  p->end = p->p - 1;
  p->type = PT_SEP;
  return PICOL_OK;
}

int picolParseEol(struct picolParser *p) {
  p->start = p->p;
  while (*p->p == ' ' || *p->p == '\t' || *p->p == '\n' || *p->p == '\r' ||
         *p->p == ';') {
    p->p++;
    p->len--;
  }
  p->end = p->p - 1;
  p->type = PT_EOL;
  return PICOL_OK;
}

int picolParseCommand(struct picolParser *p) {
  int level = 1;
  int blevel = 0;
  p->start = ++p->p;
  p->len--;
  while (1) {
    if (p->len == 0) {
      break;
    } else if (*p->p == '[' && blevel == 0) {
      level++;
    } else if (*p->p == ']' && blevel == 0) {
      if (!--level) break;
    } else if (*p->p == '\\') {
      p->p++;
      p->len--;
    } else if (*p->p == '{') {
      blevel++;
    } else if (*p->p == '}') {
      if (blevel != 0) blevel--;
    }
    p->p++;
    p->len--;
  }
  p->end = p->p - 1;
  p->type = PT_CMD;
  if (*p->p == ']') {
    p->p++;
    p->len--;
  }
  return PICOL_OK;
}

int picolParseVar(struct picolParser *p) {
  p->start = ++p->p;
  p->len--; /* skip the $ */
  while (1) {
    if ((*p->p >= 'a' && *p->p <= 'z') || (*p->p >= 'A' && *p->p <= 'Z') ||
        (*p->p >= '0' && *p->p <= '9') || *p->p == '_') {
      p->p++;
      p->len--;
      continue;
    }
    break;
  }
  if (p->start == p->p) { /* It's just a single char string "$" */
    p->start = p->end = p->p - 1;
    p->type = PT_STR;
  } else {
    p->end = p->p - 1;
    p->type = PT_VAR;
  }
  return PICOL_OK;
}

int picolParseBrace(struct picolParser *p) {
  int level = 1;
  p->start = ++p->p;
  p->len--;
  while (1) {
    if (p->len >= 2 && *p->p == '\\') {
      p->p++;
      p->len--;
    } else if (p->len == 0 || *p->p == '}') {
      level--;
      if (level == 0 || p->len == 0) {
        p->end = p->p - 1;
        if (p->len) {
          p->p++;
          p->len--; /* Skip final closed brace */
        }
        p->type = PT_STR;
        return PICOL_OK;
      }
    } else if (*p->p == '{')
      level++;
    p->p++;
    p->len--;
  }
  return PICOL_OK; /* unreached */
}

int picolParseString(struct picolParser *p) {
  int newword = (p->type == PT_SEP || p->type == PT_EOL || p->type == PT_STR);
  if (newword && *p->p == '{')
    return picolParseBrace(p);
  else if (newword && *p->p == '"') {
    p->insidequote = 1;
    p->p++;
    p->len--;
  }
  p->start = p->p;
  while (1) {
    if (p->len == 0) {
      p->end = p->p - 1;
      p->type = PT_ESC;
      return PICOL_OK;
    }
    switch (*p->p) {
      case '\\':
        if (p->len >= 2) {
          p->p++;
          p->len--;
        }
        break;
      case '$':
      case '[':
        p->end = p->p - 1;
        p->type = PT_ESC;
        return PICOL_OK;
      case ' ':
      case '\t':
      case '\n':
      case '\r':
      case ';':
        if (!p->insidequote) {
          p->end = p->p - 1;
          p->type = PT_ESC;
          return PICOL_OK;
        }
        break;
      case '"':
        if (p->insidequote) {
          p->end = p->p - 1;
          p->type = PT_ESC;
          p->p++;
          p->len--;
          p->insidequote = 0;
          return PICOL_OK;
        }
        break;
    }
    p->p++;
    p->len--;
  }
  return PICOL_OK; /* unreached */
}

int picolParseComment(struct picolParser *p) {
  while (p->len && *p->p != '\n') {
    p->p++;
    p->len--;
  }
  return PICOL_OK;
}

int picolGetToken(struct picolParser *p) {
  while (1) {
    if (!p->len) {
      if (p->type != PT_EOL && p->type != PT_EOF)
        p->type = PT_EOL;
      else
        p->type = PT_EOF;
      return PICOL_OK;
    }
    switch (*p->p) {
      case ' ':
      case '\t':
      case '\r':
        if (p->insidequote) return picolParseString(p);
        return picolParseSep(p);
      case '\n':
      case ';':
        if (p->insidequote) return picolParseString(p);
        return picolParseEol(p);
      case '[':
        return picolParseCommand(p);
      case '$':
        return picolParseVar(p);
      case '#':
        if (p->type == PT_EOL) {
          picolParseComment(p);
          continue;
        }
        return picolParseString(p);
      default:
        return picolParseString(p);
    }
  }
  return PICOL_OK; /* unreached */
}

void picolInitInterp(struct picolInterp *i) {
  i->level = 0;
  i->callframe = malloc(sizeof(struct picolCallFrame));
  i->callframe->vars = NULL;
  i->callframe->parent = NULL;
  i->commands = NULL;
  i->result = strdup("");
}

void picolSetResult(struct picolInterp *i, char *s) {
  free(i->result);
  i->result = strdup(s);
}

struct picolVar *picolGetVar(struct picolInterp *i, char *name) {
  struct picolVar *v = i->callframe->vars;
  while (v) {
    if (strcmp(v->name, name) == 0) return v;
    v = v->next;
  }
  return NULL;
}

int picolSetVar(struct picolInterp *i, char *name, char *val) {
  struct picolVar *v = picolGetVar(i, name);
  if (v) {
    free(v->val);
    v->val = strdup(val);
  } else {
    v = malloc(sizeof(*v));
    v->name = strdup(name);
    v->val = strdup(val);
    v->next = i->callframe->vars;
    i->callframe->vars = v;
  }
  return PICOL_OK;
}

struct picolCmd *picolGetCommand(struct picolInterp *i, char *name) {
  struct picolCmd *c = i->commands;
  while (c) {
    if (strcmp(c->name, name) == 0) return c;
    c = c->next;
  }
  return NULL;
}

int picolRegisterCommand(struct picolInterp *i, char *name, picolCmdFunc f,
                         void *privdata) {
  struct picolCmd *c = picolGetCommand(i, name);
  char errbuf[1024];
  if (c) {
    snprintf(errbuf, 1024, "Command '%s' already defined", name);
    picolSetResult(i, errbuf);
    return PICOL_ERR;
  }
  c = malloc(sizeof(*c));
  c->name = strdup(name);
  c->func = f;
  c->privdata = privdata;
  c->next = i->commands;
  i->commands = c;
  return PICOL_OK;
}

/* EVAL! */
int picolEval(struct picolInterp *i, char *t) {
  struct picolParser p;
  int argc = 0, j;
  char **argv = NULL;
  char errbuf[1024];
  int retcode = PICOL_OK;
  picolSetResult(i, "");
  picolInitParser(&p, t);
  while (1) {
    char *t;
    int tlen;
    int prevtype = p.type;
    picolGetToken(&p);
    if (p.type == PT_EOF) break;
    tlen = p.end - p.start + 1;
    if (tlen < 0) tlen = 0;
    t = malloc(tlen + 1);
    memcpy(t, p.start, tlen);
    t[tlen] = '\0';
    if (p.type == PT_VAR) {
      struct picolVar *v = picolGetVar(i, t);
      if (!v) {
        snprintf(errbuf, 1024, "No such variable '%s'", t);
        free(t);
        picolSetResult(i, errbuf);
        retcode = PICOL_ERR;
        goto err;
      }
      free(t);
      t = strdup(v->val);
    } else if (p.type == PT_CMD) {
      retcode = picolEval(i, t);
      free(t);
      if (retcode != PICOL_OK) goto err;
      t = strdup(i->result);
    } else if (p.type == PT_ESC) {
      /* XXX: escape handling missing! */
    } else if (p.type == PT_SEP) {
      prevtype = p.type;
      free(t);
      continue;
    }
    /* We have a complete command + args. Call it! */
    if (p.type == PT_EOL) {
      struct picolCmd *c;
      free(t);
      prevtype = p.type;
      if (argc) {
        if ((c = picolGetCommand(i, argv[0])) == NULL) {
          snprintf(errbuf, 1024, "No such command '%s'", argv[0]);
          picolSetResult(i, errbuf);
          retcode = PICOL_ERR;
          goto err;
        }
        retcode = c->func(i, argc, argv, c->privdata);
        if (retcode != PICOL_OK) goto err;
      }
      /* Prepare for the next command */
      for (j = 0; j < argc; j++) free(argv[j]);
      free(argv);
      argv = NULL;
      argc = 0;
      continue;
    }
    /* We have a new token, append to the previous or as new arg? */
    if (prevtype == PT_SEP || prevtype == PT_EOL) {
      argv = realloc(argv, sizeof(char *) * (argc + 1));
      argv[argc] = t;
      argc++;
    } else { /* Interpolation */
      int oldlen = strlen(argv[argc - 1]), tlen = strlen(t);
      argv[argc - 1] = realloc(argv[argc - 1], oldlen + tlen + 1);
      memcpy(argv[argc - 1] + oldlen, t, tlen);
      argv[argc - 1][oldlen + tlen] = '\0';
      free(t);
    }
    prevtype = p.type;
  }
err:
  for (j = 0; j < argc; j++) free(argv[j]);
  free(argv);
  return retcode;
}

/* ACTUAL COMMANDS! */
int picolArityErr(struct picolInterp *i, char *name) {
  char buf[1024];
  snprintf(buf, 1024, "Wrong number of args for %s", name);
  picolSetResult(i, buf);
  return PICOL_ERR;
}

int picolCommandMath(struct picolInterp *i, int argc, char **argv, void *pd) {
  char buf[64];
  int a, b, c;
  if (argc != 3) return picolArityErr(i, argv[0]);
  a = atoi(argv[1]);
  b = atoi(argv[2]);
  if (argv[0][0] == '+')
    c = a + b;
  else if (argv[0][0] == '-')
    c = a - b;
  else if (argv[0][0] == '*')
    c = a * b;
  else if (argv[0][0] == '/')
    c = a / b;
  else if (argv[0][0] == '>' && argv[0][1] == '\0')
    c = a > b;
  else if (argv[0][0] == '>' && argv[0][1] == '=')
    c = a >= b;
  else if (argv[0][0] == '<' && argv[0][1] == '\0')
    c = a < b;
  else if (argv[0][0] == '<' && argv[0][1] == '=')
    c = a <= b;
  else if (argv[0][0] == '=' && argv[0][1] == '=')
    c = a == b;
  else if (argv[0][0] == '!' && argv[0][1] == '=')
    c = a != b;
  else
    c = 0; /* I hate warnings */
  snprintf(buf, 64, "%d", c);
  picolSetResult(i, buf);
  return PICOL_OK;
}

int picolCommandSet(struct picolInterp *i, int argc, char **argv, void *pd) {
  if (argc != 3) return picolArityErr(i, argv[0]);
  picolSetVar(i, argv[1], argv[2]);
  picolSetResult(i, argv[2]);
  return PICOL_OK;
}

int picolCommandPuts(struct picolInterp *i, int argc, char **argv, void *pd) {
  if (argc != 2) return picolArityErr(i, argv[0]);
  printf("%s\n", argv[1]);
  return PICOL_OK;
}

int picolCommandIf(struct picolInterp *i, int argc, char **argv, void *pd) {
  int retcode;
  if (argc != 3 && argc != 5) return picolArityErr(i, argv[0]);
  if ((retcode = picolEval(i, argv[1])) != PICOL_OK) return retcode;
  if (atoi(i->result))
    return picolEval(i, argv[2]);
  else if (argc == 5)
    return picolEval(i, argv[4]);
  return PICOL_OK;
}

int picolCommandWhile(struct picolInterp *i, int argc, char **argv, void *pd) {
  if (argc != 3) return picolArityErr(i, argv[0]);
  while (1) {
    int retcode = picolEval(i, argv[1]);
    if (retcode != PICOL_OK) return retcode;
    if (atoi(i->result)) {
      if ((retcode = picolEval(i, argv[2])) == PICOL_CONTINUE)
        continue;
      else if (retcode == PICOL_OK)
        continue;
      else if (retcode == PICOL_BREAK)
        return PICOL_OK;
      else
        return retcode;
    } else {
      return PICOL_OK;
    }
  }
}

int picolCommandRetCodes(struct picolInterp *i, int argc, char **argv,
                         void *pd) {
  if (argc != 1) return picolArityErr(i, argv[0]);
  if (strcmp(argv[0], "break") == 0)
    return PICOL_BREAK;
  else if (strcmp(argv[0], "continue") == 0)
    return PICOL_CONTINUE;
  return PICOL_OK;
}

void picolDropCallFrame(struct picolInterp *i) {
  struct picolCallFrame *cf = i->callframe;
  struct picolVar *v = cf->vars, *t;
  while (v) {
    t = v->next;
    free(v->name);
    free(v->val);
    free(v);
    v = t;
  }
  i->callframe = cf->parent;
  free(cf);
}

int picolCommandCallProc(struct picolInterp *i, int argc, char **argv,
                         void *pd) {
  char **x = pd, *alist = x[0], *body = x[1], *p = strdup(alist), *tofree;
  struct picolCallFrame *cf = malloc(sizeof(*cf));
  int arity = 0, done = 0, errcode = PICOL_OK;
  char errbuf[1024];
  cf->vars = NULL;
  cf->parent = i->callframe;
  i->callframe = cf;
  tofree = p;
  while (1) {
    char *start = p;
    while (*p != ' ' && *p != '\0') p++;
    if (*p != '\0' && p == start) {
      p++;
      continue;
    }
    if (p == start) break;
    if (*p == '\0')
      done = 1;
    else
      *p = '\0';
    if (++arity > argc - 1) goto arityerr;
    picolSetVar(i, start, argv[arity]);
    p++;
    if (done) break;
  }
  free(tofree);
  if (arity != argc - 1) goto arityerr;
  errcode = picolEval(i, body);
  if (errcode == PICOL_RETURN) errcode = PICOL_OK;
  picolDropCallFrame(i); /* remove the called proc callframe */
  return errcode;
arityerr:
  snprintf(errbuf, 1024, "Proc '%s' called with wrong arg num", argv[0]);
  picolSetResult(i, errbuf);
  picolDropCallFrame(i); /* remove the called proc callframe */
  return PICOL_ERR;
}

int picolCommandProc(struct picolInterp *i, int argc, char **argv, void *pd) {
  char **procdata = malloc(sizeof(char *) * 2);
  if (argc != 4) return picolArityErr(i, argv[0]);
  procdata[0] = strdup(argv[2]); /* arguments list */
  procdata[1] = strdup(argv[3]); /* procedure body */
  return picolRegisterCommand(i, argv[1], picolCommandCallProc, procdata);
}

int picolCommandReturn(struct picolInterp *i, int argc, char **argv, void *pd) {
  if (argc != 1 && argc != 2) return picolArityErr(i, argv[0]);
  picolSetResult(i, (argc == 2) ? argv[1] : "");
  return PICOL_RETURN;
}

void picolRegisterCoreCommands(struct picolInterp *i) {
  int j;
  char *name[] = {"+", "-", "*", "/", ">", ">=", "<", "<=", "==", "!="};
  for (j = 0; j < (int)(sizeof(name) / sizeof(char *)); j++)
    picolRegisterCommand(i, name[j], picolCommandMath, NULL);
  picolRegisterCommand(i, "set", picolCommandSet, NULL);
  picolRegisterCommand(i, "puts", picolCommandPuts, NULL);
  picolRegisterCommand(i, "if", picolCommandIf, NULL);
  picolRegisterCommand(i, "while", picolCommandWhile, NULL);
  picolRegisterCommand(i, "break", picolCommandRetCodes, NULL);
  picolRegisterCommand(i, "continue", picolCommandRetCodes, NULL);
  picolRegisterCommand(i, "proc", picolCommandProc, NULL);
  picolRegisterCommand(i, "return", picolCommandReturn, NULL);
}

int main(int argc, char **argv) {
  struct picolInterp interp;
  picolInitInterp(&interp);
  picolRegisterCoreCommands(&interp);
  if (argc == 1) {
    while (1) {
      char clibuf[1024];
      int retcode;
      printf("picol> ");
      fflush(stdout);
      if (fgets(clibuf, 1024, stdin) == NULL) return 0;
      retcode = picolEval(&interp, clibuf);
      if (interp.result[0] != '\0') printf("[%d] %s\n", retcode, interp.result);
    }
  } else if (argc == 2) {
    char buf[1024 * 16];
    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
      perror("open");
      exit(1);
    }
    buf[fread(buf, 1, 1024 * 16, fp)] = '\0';
    fclose(fp);
    if (picolEval(&interp, buf) != PICOL_OK) printf("%s\n", interp.result);
  }
  return 0;
}
