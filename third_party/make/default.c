/* Data base of default implicit rules for GNU Make.
Copyright (C) 1988-2020 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* clang-format off */
#include "third_party/make/makeint.inc"
/**/
#include "third_party/make/filedef.h"
#include "third_party/make/variable.h"
#include "third_party/make/rule.h"
#include "third_party/make/dep.h"
#include "third_party/make/job.h"
#include "libc/assert.h"
#include "third_party/make/commands.h"

/* This is the default list of suffixes for suffix rules.
   '.s' must come last, so that a '.o' file will be made from
   a '.c' or '.p' or ... file rather than from a .s file.  */

static char default_suffixes[]
  = ".out .a .ln .o .c .cc .C .cpp .p .f .F .m .r .y .l .ym .yl .s .S \
.mod .sym .def .h .info .dvi .tex .texinfo .texi .txinfo \
.w .ch .web .sh .elc .el";

static struct pspec default_pattern_rules[] =
  {
    { "(%)", "%",
        "$(AR) $(ARFLAGS) $@ $<" },
    /* The X.out rules are only in BSD's default set because
       BSD Make has no null-suffix rules, so 'foo.out' and
       'foo' are the same thing.  */
#ifdef __COSMOPOLITAN__
    { "%.exe", "%",
        "$(CP) $< $@" },
#endif
    { "%.out", "%",
        "@rm -f $@ \n cp $< $@" },

    /* Syntax is "ctangle foo.w foo.ch foo.c".  */
    { "%.c", "%.w %.ch",
        "$(CTANGLE) $^ $@" },
    { "%.tex", "%.w %.ch",
        "$(CWEAVE) $^ $@" },

    { 0, 0, 0 }
  };

static struct pspec default_terminal_rules[] =
  {
    /* RCS.  */
    { "%", "%,v",
        "$(CHECKOUT,v)" },
    { "%", "RCS/%,v",
        "$(CHECKOUT,v)" },
    { "%", "RCS/%",
        "$(CHECKOUT,v)" },

    /* SCCS.  */
    { "%", "s.%",
        "$(GET) $(GFLAGS) $(SCCS_OUTPUT_OPTION) $<" },
    { "%", "SCCS/s.%",
        "$(GET) $(GFLAGS) $(SCCS_OUTPUT_OPTION) $<" },

    { 0, 0, 0 }
  };

static const char *default_suffix_rules[] =
  {
    ".o",
    "$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@",
    ".s",
    "$(LINK.s) $^ $(LOADLIBES) $(LDLIBS) -o $@",
    ".S",
    "$(LINK.S) $^ $(LOADLIBES) $(LDLIBS) -o $@",
    ".c",
    "$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@",
    ".cc",
    "$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@",
    ".C",
    "$(LINK.C) $^ $(LOADLIBES) $(LDLIBS) -o $@",
    ".cpp",
    "$(LINK.cpp) $^ $(LOADLIBES) $(LDLIBS) -o $@",
    ".f",
    "$(LINK.f) $^ $(LOADLIBES) $(LDLIBS) -o $@",
    ".m",
    "$(LINK.m) $^ $(LOADLIBES) $(LDLIBS) -o $@",
    ".p",
    "$(LINK.p) $^ $(LOADLIBES) $(LDLIBS) -o $@",
    ".F",
    "$(LINK.F) $^ $(LOADLIBES) $(LDLIBS) -o $@",
    ".r",
    "$(LINK.r) $^ $(LOADLIBES) $(LDLIBS) -o $@",
    ".mod",
    "$(COMPILE.mod) -o $@ -e $@ $^",

    ".def.sym",
    "$(COMPILE.def) -o $@ $<",

    ".sh",
    "cat $< >$@ \n chmod a+x $@",

    ".s.o",
    "$(COMPILE.s) -o $@ $<",
    ".S.o",
    "$(COMPILE.S) -o $@ $<",
    ".c.o",
    "$(COMPILE.c) $(OUTPUT_OPTION) $<",
    ".cc.o",
    "$(COMPILE.cc) $(OUTPUT_OPTION) $<",
    ".C.o",
    "$(COMPILE.C) $(OUTPUT_OPTION) $<",
    ".cpp.o",
    "$(COMPILE.cpp) $(OUTPUT_OPTION) $<",
    ".f.o",
    "$(COMPILE.f) $(OUTPUT_OPTION) $<",
    ".m.o",
    "$(COMPILE.m) $(OUTPUT_OPTION) $<",
    ".p.o",
    "$(COMPILE.p) $(OUTPUT_OPTION) $<",
    ".F.o",
    "$(COMPILE.F) $(OUTPUT_OPTION) $<",
    ".r.o",
    "$(COMPILE.r) $(OUTPUT_OPTION) $<",
    ".mod.o",
    "$(COMPILE.mod) -o $@ $<",

    ".c.ln",
    "$(LINT.c) -C$* $<",
    ".y.ln",
    "$(YACC.y) $< \n $(LINT.c) -C$* y.tab.c \n $(RM) y.tab.c",
    ".l.ln",
    "@$(RM) $*.c\n $(LEX.l) $< > $*.c\n$(LINT.c) -i $*.c -o $@\n $(RM) $*.c",

    ".y.c",
    "$(YACC.y) $< \n mv -f y.tab.c $@",
    ".l.c",
    "@$(RM) $@ \n $(LEX.l) $< > $@",
    ".ym.m",
    "$(YACC.m) $< \n mv -f y.tab.c $@",
    ".lm.m",
    "@$(RM) $@ \n $(LEX.m) $< > $@",

    ".F.f",
    "$(PREPROCESS.F) $(OUTPUT_OPTION) $<",
    ".r.f",
    "$(PREPROCESS.r) $(OUTPUT_OPTION) $<",

    /* This might actually make lex.yy.c if there's no %R% directive in $*.l,
       but in that case why were you trying to make $*.r anyway?  */
    ".l.r",
    "$(LEX.l) $< > $@ \n mv -f lex.yy.r $@",

    ".S.s",
    "$(PREPROCESS.S) $< > $@",

    ".texinfo.info",
    "$(MAKEINFO) $(MAKEINFO_FLAGS) $< -o $@",

    ".texi.info",
    "$(MAKEINFO) $(MAKEINFO_FLAGS) $< -o $@",

    ".txinfo.info",
    "$(MAKEINFO) $(MAKEINFO_FLAGS) $< -o $@",

    ".tex.dvi",
    "$(TEX) $<",

    ".texinfo.dvi",
    "$(TEXI2DVI) $(TEXI2DVI_FLAGS) $<",

    ".texi.dvi",
    "$(TEXI2DVI) $(TEXI2DVI_FLAGS) $<",

    ".txinfo.dvi",
    "$(TEXI2DVI) $(TEXI2DVI_FLAGS) $<",

    ".w.c",
    "$(CTANGLE) $< - $@",       /* The '-' says there is no '.ch' file.  */

    ".web.p",
    "$(TANGLE) $<",

    ".w.tex",
    "$(CWEAVE) $< - $@",        /* The '-' says there is no '.ch' file.  */

    ".web.tex",
    "$(WEAVE) $<",

    0, 0,
  };

static const char *default_variables[] =
  {
    "AR", "ar",
    "ARFLAGS", "rv",
    "AS", "as",
#ifdef GCC_IS_NATIVE
    "CC", "gcc",
    "CXX", "gcc",
    "OBJC", "gcc",
#else
    "CC", "cc",
    "CXX", "g++",
    "OBJC", "cc",
#endif

    /* This expands to $(CO) $(COFLAGS) $< $@ if $@ does not exist,
       and to the empty string if $@ does exist.  */
    "CHECKOUT,v", "+$(if $(wildcard $@),,$(CO) $(COFLAGS) $< $@)",
    "CO", "co",
    "COFLAGS", "",

    "CPP", "$(CC) -E",
    "FC", "f77",
    /* System V uses these, so explicit rules using them should work.
       However, there is no way to make implicit rules use them and FC.  */
    "F77", "$(FC)",
    "F77FLAGS", "$(FFLAGS)",
    "GET", SCCS_GET,
    "LD", "ld",
#ifdef GCC_IS_NATIVE
    "LEX", "flex",
#else
    "LEX", "lex",
#endif
    "LINT", "lint",
    "M2C", "m2c",
#ifdef  pyr
    "PC", "pascal",
#else
    "PC", "pc",
#endif  /* pyr.  */
#ifdef GCC_IS_NATIVE
    "YACC", "bison -y",
#else
    "YACC", "yacc",     /* Or "bison -y"  */
#endif
    "MAKEINFO", "makeinfo",
    "TEX", "tex",
    "TEXI2DVI", "texi2dvi",
    "WEAVE", "weave",
    "CWEAVE", "cweave",
    "TANGLE", "tangle",
    "CTANGLE", "ctangle",

    "RM", "rm -f",

    "LINK.o", "$(CC) $(LDFLAGS) $(TARGET_ARCH)",
    "COMPILE.c", "$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c",
    "LINK.c", "$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)",
    "COMPILE.m", "$(OBJC) $(OBJCFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c",
    "LINK.m", "$(OBJC) $(OBJCFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)",
    "COMPILE.cc", "$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c",
#ifndef HAVE_CASE_INSENSITIVE_FS
    /* On case-insensitive filesystems, treat *.C files as *.c files,
       to avoid erroneously compiling C sources as C++, which will
       probably fail.  */
    "COMPILE.C", "$(COMPILE.cc)",
#else
    "COMPILE.C", "$(COMPILE.c)",
#endif
    "COMPILE.cpp", "$(COMPILE.cc)",
    "LINK.cc", "$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)",
#ifndef HAVE_CASE_INSENSITIVE_FS
    "LINK.C", "$(LINK.cc)",
#else
    "LINK.C", "$(LINK.c)",
#endif
    "LINK.cpp", "$(LINK.cc)",
    "YACC.y", "$(YACC) $(YFLAGS)",
    "LEX.l", "$(LEX) $(LFLAGS) -t",
    "YACC.m", "$(YACC) $(YFLAGS)",
    "LEX.m", "$(LEX) $(LFLAGS) -t",
    "COMPILE.f", "$(FC) $(FFLAGS) $(TARGET_ARCH) -c",
    "LINK.f", "$(FC) $(FFLAGS) $(LDFLAGS) $(TARGET_ARCH)",
    "COMPILE.F", "$(FC) $(FFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c",
    "LINK.F", "$(FC) $(FFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)",
    "COMPILE.r", "$(FC) $(FFLAGS) $(RFLAGS) $(TARGET_ARCH) -c",
    "LINK.r", "$(FC) $(FFLAGS) $(RFLAGS) $(LDFLAGS) $(TARGET_ARCH)",
    "COMPILE.def", "$(M2C) $(M2FLAGS) $(DEFFLAGS) $(TARGET_ARCH)",
    "COMPILE.mod", "$(M2C) $(M2FLAGS) $(MODFLAGS) $(TARGET_ARCH)",
    "COMPILE.p", "$(PC) $(PFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c",
    "LINK.p", "$(PC) $(PFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)",
    "LINK.s", "$(CC) $(ASFLAGS) $(LDFLAGS) $(TARGET_MACH)",
    "COMPILE.s", "$(AS) $(ASFLAGS) $(TARGET_MACH)",
    "LINK.S", "$(CC) $(ASFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_MACH)",
    "COMPILE.S", "$(CC) $(ASFLAGS) $(CPPFLAGS) $(TARGET_MACH) -c",
    "PREPROCESS.S", "$(CC) -E $(CPPFLAGS)",
    "PREPROCESS.F", "$(FC) $(FFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -F",
    "PREPROCESS.r", "$(FC) $(FFLAGS) $(RFLAGS) $(TARGET_ARCH) -F",
    "LINT.c", "$(LINT) $(LINTFLAGS) $(CPPFLAGS) $(TARGET_ARCH)",
    "OUTPUT_OPTION", "-o $@",
    ".LIBPATTERNS", "lib%.so lib%.a",
    /* Make this assignment to avoid undefined variable warnings.  */
    "GNUMAKEFLAGS", "",
    0, 0
  };

/* Set up the default .SUFFIXES list.  */

void
set_default_suffixes (void)
{
  suffix_file = enter_file (strcache_add (".SUFFIXES"));
  suffix_file->builtin = 1;

  if (no_builtin_rules_flag)
    define_variable_cname ("SUFFIXES", "", o_default, 0);
  else
    {
      struct dep *d;
      const char *p = default_suffixes;
      suffix_file->deps = enter_prereqs (PARSE_SIMPLE_SEQ ((char **)&p, struct dep),
                                         NULL);
      for (d = suffix_file->deps; d; d = d->next)
        d->file->builtin = 1;

      define_variable_cname ("SUFFIXES", default_suffixes, o_default, 0);
    }
}

/* Enter the default suffix rules as file rules.  This used to be done in
   install_default_implicit_rules, but that loses because we want the
   suffix rules installed before reading makefiles, and the pattern rules
   installed after.  */

void
install_default_suffix_rules (void)
{
  const char **s;

  if (no_builtin_rules_flag)
    return;

  for (s = default_suffix_rules; *s != 0; s += 2)
    {
      struct file *f = enter_file (strcache_add (s[0]));
      /* This function should run before any makefile is parsed.  */
      assert (f->cmds == 0);
      f->cmds = xmalloc (sizeof (struct commands));
      f->cmds->fileinfo.filenm = 0;
      f->cmds->commands = xstrdup (s[1]);
      f->cmds->command_lines = 0;
      f->cmds->recipe_prefix = RECIPEPREFIX_DEFAULT;
      f->builtin = 1;
    }
}


/* Install the default pattern rules.  */

void
install_default_implicit_rules (void)
{
  struct pspec *p;

  if (no_builtin_rules_flag)
    return;

  for (p = default_pattern_rules; p->target != 0; ++p)
    install_pattern_rule (p, 0);

  for (p = default_terminal_rules; p->target != 0; ++p)
    install_pattern_rule (p, 1);
}

void
define_default_variables (void)
{
  const char **s;

  if (no_builtin_variables_flag)
    return;

  for (s = default_variables; *s != 0; s += 2)
    define_variable (s[0], strlen (s[0]), s[1], o_default, 1);
}

void
undefine_default_variables (void)
{
  const char **s;

  for (s = default_variables; *s != 0; s += 2)
    undefine_variable_global (s[0], strlen (s[0]), o_default);
}
