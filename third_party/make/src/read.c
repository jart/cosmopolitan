/* Reading and parsing of makefiles for GNU Make.
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

#include "makeint.h"

#include <assert.h>

#include "filedef.h"
#include "dep.h"
#include "job.h"
#include "os.h"
#include "commands.h"
#include "variable.h"
#include "rule.h"
#include "debug.h"
#include "hash.h"


#ifdef WINDOWS32
#include <windows.h>
#include "sub_proc.h"
#else  /* !WINDOWS32 */
#ifndef _AMIGA
#ifndef VMS
#include <pwd.h>
#else
struct passwd *getpwnam (char *name);
#endif
#endif
#endif /* !WINDOWS32 */

/* A 'struct ebuffer' controls the origin of the makefile we are currently
   eval'ing.
*/

struct ebuffer
  {
    char *buffer;       /* Start of the current line in the buffer.  */
    char *bufnext;      /* Start of the next line in the buffer.  */
    char *bufstart;     /* Start of the entire buffer.  */
    size_t size;        /* Malloc'd size of buffer. */
    FILE *fp;           /* File, or NULL if this is an internal buffer.  */
    floc floc;          /* Info on the file in fp (if any).  */
  };

/* Track the modifiers we can have on variable assignments */

struct vmodifiers
  {
    unsigned int assign_v:1;
    unsigned int define_v:1;
    unsigned int undefine_v:1;
    unsigned int export_v:1;
    unsigned int override_v:1;
    unsigned int private_v:1;
  };

/* Types of "words" that can be read in a makefile.  */
enum make_word_type
  {
     w_bogus, w_eol, w_static, w_variable, w_colon, w_dcolon, w_semicolon,
     w_varassign, w_ampcolon, w_ampdcolon
  };


/* A 'struct conditionals' contains the information describing
   all the active conditionals in a makefile.

   The global variable 'conditionals' contains the conditionals
   information for the current makefile.  It is initialized from
   the static structure 'toplevel_conditionals' and is later changed
   to new structures for included makefiles.  */

struct conditionals
  {
    unsigned int if_cmds;       /* Depth of conditional nesting.  */
    unsigned int allocated;     /* Elts allocated in following arrays.  */
    char *ignoring;             /* Are we ignoring or interpreting?
                                   0=interpreting, 1=not yet interpreted,
                                   2=already interpreted */
    char *seen_else;            /* Have we already seen an 'else'?  */
  };

static struct conditionals toplevel_conditionals;
static struct conditionals *conditionals = &toplevel_conditionals;


/* Default directories to search for include files in  */

static const char *default_include_directories[] =
  {
#if defined(WINDOWS32) && !defined(INCLUDEDIR)
/* This completely up to the user when they install MSVC or other packages.
   This is defined as a placeholder.  */
# define INCLUDEDIR "."
#endif
    INCLUDEDIR,
#ifndef _AMIGA
    "/usr/gnu/include",
    "/usr/local/include",
    "/usr/include",
#endif
    0
  };

/* List of directories to search for include files in  */

static const char **include_directories;

/* Maximum length of an element of the above.  */

static size_t max_incl_len;

/* The filename and pointer to line number of the
   makefile currently being read in.  */

const floc *reading_file = 0;

/* The chain of files read by read_all_makefiles.  */

static struct goaldep *read_files = 0;

static struct goaldep *eval_makefile (const char *filename, unsigned short flags);
static void eval (struct ebuffer *buffer, int flags);

static long readline (struct ebuffer *ebuf);
static void do_undefine (char *name, enum variable_origin origin,
                         struct ebuffer *ebuf);
static struct variable *do_define (char *name, enum variable_origin origin,
                                   struct ebuffer *ebuf);
static int conditional_line (char *line, size_t len, const floc *flocp);
static void record_files (struct nameseq *filenames, int are_also_makes,
                          const char *pattern,
                          const char *pattern_percent, char *depstr,
                          unsigned int cmds_started, char *commands,
                          size_t commands_idx, int two_colon,
                          char prefix, const floc *flocp);
static void record_target_var (struct nameseq *filenames, char *defn,
                               enum variable_origin origin,
                               struct vmodifiers *vmod,
                               const floc *flocp);
static enum make_word_type get_next_mword (char *buffer,
                                           char **startp, size_t *length);
static void remove_comments (char *line);
static char *find_map_unquote (char *string, int map);
static char *find_char_unquote (char *string, int stop);
static char *unescape_char (char *string, int c);


/* Compare a word, both length and contents.
   P must point to the word to be tested, and WLEN must be the length.
*/
#define word1eq(s)      (wlen == CSTRLEN (s) && strneq (s, p, CSTRLEN (s)))


/* Read in all the makefiles and return a chain of targets to rebuild.  */

struct goaldep *
read_all_makefiles (const char **makefiles)
{
  unsigned int num_makefiles = 0;

  /* Create *_LIST variables, to hold the makefiles, targets, and variables
     we will be reading. */

  define_variable_cname ("MAKEFILE_LIST", "", o_file, 0);

  DB (DB_BASIC, (_("Reading makefiles...\n")));

  /* If there's a non-null variable MAKEFILES, its value is a list of
     files to read first thing.  But don't let it prevent reading the
     default makefiles and don't let the default goal come from there.  */

  {
    char *value;
    char *name, *p;
    size_t length;

    {
      /* Turn off --warn-undefined-variables while we expand MAKEFILES.  */
      int save = warn_undefined_variables_flag;
      warn_undefined_variables_flag = 0;

      value = allocated_variable_expand ("$(MAKEFILES)");

      warn_undefined_variables_flag = save;
    }

    /* Set NAME to the start of next token and LENGTH to its length.
       MAKEFILES is updated for finding remaining tokens.  */
    p = value;

    while ((name = find_next_token ((const char **)&p, &length)) != 0)
      {
        if (*p != '\0')
          *p++ = '\0';
        eval_makefile (strcache_add (name), RM_NO_DEFAULT_GOAL|RM_INCLUDED|RM_DONTCARE);
      }

    free (value);
  }

  /* Read makefiles specified with -f switches.  */

  if (makefiles != 0)
    while (*makefiles != 0)
      {
        struct goaldep *d = eval_makefile (*makefiles, 0);

        if (errno)
          perror_with_name ("", *makefiles);

        /* Reuse the storage allocated for the read_file.  */
        *makefiles = dep_name (d);
        ++num_makefiles;
        ++makefiles;
      }

  /* If there were no -f switches, try the default names.  */

  if (num_makefiles == 0)
    {
      static const char *default_makefiles[] =
#ifdef VMS
        /* all lower case since readdir() (the vms version) 'lowercasifies' */
        /* TODO: Above is not always true, this needs more work */
        { "makefile.vms", "gnumakefile", "makefile", 0 };
#else
#ifdef _AMIGA
        { "GNUmakefile", "Makefile", "SMakefile", 0 };
#else /* !Amiga && !VMS */
#ifdef WINDOWS32
        { "GNUmakefile", "makefile", "Makefile", "makefile.mak", 0 };
#else /* !Amiga && !VMS && !WINDOWS32 */
        { "GNUmakefile", "makefile", "Makefile", 0 };
#endif /* !Amiga && !VMS && !WINDOWS32 */
#endif /* AMIGA */
#endif /* VMS */
      const char **p = default_makefiles;
      while (*p != 0 && !file_exists_p (*p))
        ++p;

      if (*p != 0)
        {
          eval_makefile (*p, 0);
          if (errno)
            perror_with_name ("", *p);
        }
      else
        {
          /* No default makefile was found.  Add the default makefiles to the
             'read_files' chain so they will be updated if possible.  */
          struct goaldep *tail = read_files;
          /* Add them to the tail, after any MAKEFILES variable makefiles.  */
          while (tail != 0 && tail->next != 0)
            tail = tail->next;
          for (p = default_makefiles; *p != 0; ++p)
            {
              struct goaldep *d = alloc_goaldep ();
              d->file = enter_file (strcache_add (*p));
              /* Tell update_goal_chain to bail out as soon as this file is
                 made, and main not to die if we can't make this file.  */
              d->flags = RM_DONTCARE;
              if (tail == 0)
                read_files = d;
              else
                tail->next = d;
              tail = d;
            }
          if (tail != 0)
            tail->next = 0;
        }
    }

  return read_files;
}

/* Install a new conditional and return the previous one.  */

static struct conditionals *
install_conditionals (struct conditionals *new)
{
  struct conditionals *save = conditionals;

  memset (new, '\0', sizeof (*new));
  conditionals = new;

  return save;
}

/* Free the current conditionals and reinstate a saved one.  */

static void
restore_conditionals (struct conditionals *saved)
{
  /* Free any space allocated by conditional_line.  */
  free (conditionals->ignoring);
  free (conditionals->seen_else);

  /* Restore state.  */
  conditionals = saved;
}

static struct goaldep *
eval_makefile (const char *filename, unsigned short flags)
{
  struct goaldep *deps;
  struct ebuffer ebuf;
  const floc *curfile;
  char *expanded = 0;

  /* Create a new goaldep entry.  */
  deps = alloc_goaldep ();
  deps->next = read_files;
  read_files = deps;

  ebuf.floc.filenm = filename; /* Use the original file name.  */
  ebuf.floc.lineno = 1;
  ebuf.floc.offset = 0;

  if (ISDB (DB_VERBOSE))
    {
      printf (_("Reading makefile '%s'"), filename);
      if (flags & RM_NO_DEFAULT_GOAL)
        printf (_(" (no default goal)"));
      if (flags & RM_INCLUDED)
        printf (_(" (search path)"));
      if (flags & RM_DONTCARE)
        printf (_(" (don't care)"));
      if (flags & RM_NO_TILDE)
        printf (_(" (no ~ expansion)"));
      puts ("...");
    }

  /* First, get a stream to read.  */

  /* Expand ~ in FILENAME unless it came from 'include',
     in which case it was already done.  */
  if (!(flags & RM_NO_TILDE) && filename[0] == '~')
    {
      expanded = tilde_expand (filename);
      if (expanded != 0)
        filename = expanded;
    }

  errno = 0;
  ENULLLOOP (ebuf.fp, fopen (filename, "r"));
  deps->error = errno;

  /* Check for unrecoverable errors: out of mem or FILE slots.  */
  switch (deps->error)
    {
#ifdef EMFILE
    case EMFILE:
#endif
#ifdef ENFILE
    case ENFILE:
#endif
    case ENOMEM:
      {
        const char *err = strerror (deps->error);
        OS (fatal, reading_file, "%s", err);
      }
    }

  /* If the makefile wasn't found and it's either a makefile from
     the 'MAKEFILES' variable or an included makefile,
     search the included makefile search path for this makefile.  */
  if (ebuf.fp == 0 && (flags & RM_INCLUDED) && *filename != '/')
    {
      unsigned int i;
      for (i = 0; include_directories[i] != 0; ++i)
        {
          const char *included = concat (3, include_directories[i],
                                         "/", filename);
          ebuf.fp = fopen (included, "r");
          if (ebuf.fp)
            {
              filename = included;
              break;
            }
        }
    }

  /* Enter the final name for this makefile as a goaldep.  */
  filename = strcache_add (filename);
  deps->file = lookup_file (filename);
  if (deps->file == 0)
    deps->file = enter_file (filename);
  filename = deps->file->name;
  deps->flags = flags;

  free (expanded);

  if (ebuf.fp == 0)
    {
      /* The makefile can't be read at all, give up entirely.
         If we did some searching errno has the error from the last attempt,
         rather from FILENAME itself: recover the more accurate one.  */
      errno = deps->error;
      deps->file->last_mtime = NONEXISTENT_MTIME;
      return deps;
    }

  /* Success; clear errno.  */
  deps->error = 0;

  /* Avoid leaking the makefile to children.  */
  fd_noinherit (fileno (ebuf.fp));

  /* Add this makefile to the list. */
  do_variable_definition (&ebuf.floc, "MAKEFILE_LIST", filename, o_file,
                          f_append_value, 0);

  /* Evaluate the makefile */

  ebuf.size = 200;
  ebuf.buffer = ebuf.bufnext = ebuf.bufstart = xmalloc (ebuf.size);

  curfile = reading_file;
  reading_file = &ebuf.floc;

  eval (&ebuf, !(flags & RM_NO_DEFAULT_GOAL));

  reading_file = curfile;

  fclose (ebuf.fp);

  free (ebuf.bufstart);
  alloca (0);

  errno = 0;
  return deps;
}

void
eval_buffer (char *buffer, const floc *flocp)
{
  struct ebuffer ebuf;
  struct conditionals *saved;
  struct conditionals new;
  const floc *curfile;

  /* Evaluate the buffer */

  ebuf.size = strlen (buffer);
  ebuf.buffer = ebuf.bufnext = ebuf.bufstart = buffer;
  ebuf.fp = NULL;

  if (flocp)
    ebuf.floc = *flocp;
  else if (reading_file)
    ebuf.floc = *reading_file;
  else
    {
      ebuf.floc.filenm = NULL;
      ebuf.floc.lineno = 1;
      ebuf.floc.offset = 0;
    }

  curfile = reading_file;
  reading_file = &ebuf.floc;

  saved = install_conditionals (&new);

  eval (&ebuf, 1);

  restore_conditionals (saved);

  reading_file = curfile;

  alloca (0);
}

/* Check LINE to see if it's a variable assignment or undefine.

   It might use one of the modifiers "export", "override", "private", or it
   might be one of the conditional tokens like "ifdef", "include", etc.

   If it's not a variable assignment or undefine, VMOD.V_ASSIGN is 0.
   Returns LINE.

   Returns a pointer to the first non-modifier character, and sets VMOD
   based on the modifiers found if any, plus V_ASSIGN is 1.
 */
static char *
parse_var_assignment (const char *line, struct vmodifiers *vmod)
{
  const char *p;
  memset (vmod, '\0', sizeof (*vmod));

  /* Find the start of the next token.  If there isn't one we're done.  */
  NEXT_TOKEN (line);
  if (*line == '\0')
    return (char *) line;

  p = line;
  while (1)
    {
      size_t wlen;
      const char *p2;
      struct variable v;

      p2 = parse_variable_definition (p, &v);

      /* If this is a variable assignment, we're done.  */
      if (p2)
        break;

      /* It's not a variable; see if it's a modifier.  */
      p2 = end_of_token (p);
      wlen = p2 - p;

      if (word1eq ("export"))
        vmod->export_v = 1;
      else if (word1eq ("override"))
        vmod->override_v = 1;
      else if (word1eq ("private"))
        vmod->private_v = 1;
      else if (word1eq ("define"))
        {
          /* We can't have modifiers after 'define' */
          vmod->define_v = 1;
          p = next_token (p2);
          break;
        }
      else if (word1eq ("undefine"))
        {
          /* We can't have modifiers after 'undefine' */
          vmod->undefine_v = 1;
          p = next_token (p2);
          break;
        }
      else
        /* Not a variable or modifier: this is not a variable assignment.  */
        return (char *) line;

      /* It was a modifier.  Try the next word.  */
      p = next_token (p2);
      if (*p == '\0')
        return (char *) line;
    }

  /* Found a variable assignment or undefine.  */
  vmod->assign_v = 1;
  return (char *)p;
}


/* Read file FILENAME as a makefile and add its contents to the data base.

   SET_DEFAULT is true if we are allowed to set the default goal.  */

static void
eval (struct ebuffer *ebuf, int set_default)
{
  char *collapsed = 0;
  size_t collapsed_length = 0;
  size_t commands_len = 200;
  char *commands;
  size_t commands_idx = 0;
  unsigned int cmds_started, tgts_started;
  int ignoring = 0, in_ignored_define = 0;
  int no_targets = 0;           /* Set when reading a rule without targets.  */
  int also_make_targets = 0;    /* Set when reading grouped targets. */
  struct nameseq *filenames = 0;
  char *depstr = 0;
  long nlines = 0;
  int two_colon = 0;
  char prefix = cmd_prefix;
  const char *pattern = 0;
  const char *pattern_percent;
  floc *fstart;
  floc fi;

#define record_waiting_files()                                                \
  do                                                                          \
    {                                                                         \
      if (filenames != 0)                                                     \
        {                                                                     \
          fi.lineno = tgts_started;                                           \
          fi.offset = 0;                                                      \
          record_files (filenames, also_make_targets, pattern,                \
                        pattern_percent, depstr,                              \
                        cmds_started, commands, commands_idx, two_colon,      \
                        prefix, &fi);                                         \
          filenames = 0;                                                      \
        }                                                                     \
      commands_idx = 0;                                                       \
      no_targets = 0;                                                         \
      pattern = 0;                                                            \
      also_make_targets = 0;                                                  \
    } while (0)

  pattern_percent = 0;
  cmds_started = tgts_started = 1;

  fstart = &ebuf->floc;
  fi.filenm = ebuf->floc.filenm;

  /* Loop over lines in the file.
     The strategy is to accumulate target names in FILENAMES, dependencies
     in DEPS and commands in COMMANDS.  These are used to define a rule
     when the start of the next rule (or eof) is encountered.

     When you see a "continue" in the loop below, that means we are moving on
     to the next line.  If you see record_waiting_files(), then the statement
     we are parsing also finishes the previous rule.  */

  commands = xmalloc (200);

  while (1)
    {
      size_t linelen;
      char *line;
      size_t wlen;
      char *p;
      char *p2;
      struct vmodifiers vmod;

      /* At the top of this loop, we are starting a brand new line.  */
      /* Grab the next line to be evaluated */
      ebuf->floc.lineno += nlines;
      nlines = readline (ebuf);

      /* If there is nothing left to eval, we're done.  */
      if (nlines < 0)
        break;

      line = ebuf->buffer;

      /* If this is the first line, check for a UTF-8 BOM and skip it.  */
      if (ebuf->floc.lineno == 1)
        {
          unsigned char *ul = (unsigned char *) line;
          if (ul[0] == 0xEF && ul[1] == 0xBB && ul[2] == 0xBF)
            {
              line += 3;
              if (ISDB(DB_BASIC))
                {
                  if (ebuf->floc.filenm)
                    printf (_("Skipping UTF-8 BOM in makefile '%s'\n"),
                            ebuf->floc.filenm);
                  else
                    printf (_("Skipping UTF-8 BOM in makefile buffer\n"));
                }
            }
        }
      /* If this line is empty, skip it.  */
      if (line[0] == '\0')
        continue;

      linelen = strlen (line);

      /* Check for a shell command line first.
         If it is not one, we can stop treating cmd_prefix specially.  */
      if (line[0] == cmd_prefix)
        {
          if (no_targets)
            /* Ignore the commands in a rule with no targets.  */
            continue;

          /* If there is no preceding rule line, don't treat this line
             as a command, even though it begins with a recipe prefix.
             SunOS 4 make appears to behave this way.  */

          if (filenames != 0)
            {
              if (ignoring)
                /* Yep, this is a shell command, and we don't care.  */
                continue;

              if (commands_idx == 0)
                cmds_started = ebuf->floc.lineno;

              /* Append this command line to the line being accumulated.
                 Skip the initial command prefix character.  */
              if (linelen + commands_idx > commands_len)
                {
                  commands_len = (linelen + commands_idx) * 2;
                  commands = xrealloc (commands, commands_len);
                }
              memcpy (&commands[commands_idx], line + 1, linelen - 1);
              commands_idx += linelen - 1;
              commands[commands_idx++] = '\n';
              continue;
            }
        }

      /* This line is not a shell command line.  Don't worry about whitespace.
         Get more space if we need it; we don't need to preserve the current
         contents of the buffer.  */

      if (collapsed_length < linelen+1)
        {
          collapsed_length = linelen+1;
          free (collapsed);
          /* Don't need xrealloc: we don't need to preserve the content.  */
          collapsed = xmalloc (collapsed_length);
        }
      strcpy (collapsed, line);
      /* Collapse continuation lines.  */
      collapse_continuations (collapsed);
      remove_comments (collapsed);

      /* Get rid if starting space (including formfeed, vtab, etc.)  */
      p = collapsed;
      NEXT_TOKEN (p);

      /* See if this is a variable assignment.  We need to do this early, to
         allow variables with names like 'ifdef', 'export', 'private', etc.  */
      p = parse_var_assignment (p, &vmod);
      if (vmod.assign_v)
        {
          struct variable *v;
          enum variable_origin origin = vmod.override_v ? o_override : o_file;

          /* If we're ignoring then we're done now.  */
          if (ignoring)
            {
              if (vmod.define_v)
                in_ignored_define = 1;
              continue;
            }

          /* Variable assignment ends the previous rule.  */
          record_waiting_files ();

          if (vmod.undefine_v)
          {
            do_undefine (p, origin, ebuf);
            continue;
          }
          else if (vmod.define_v)
            v = do_define (p, origin, ebuf);
          else
            v = try_variable_definition (fstart, p, origin, 0);

          assert (v != NULL);

          if (vmod.export_v)
            v->export = v_export;
          if (vmod.private_v)
            v->private_var = 1;

          /* This line has been dealt with.  */
          continue;
        }

      /* If this line is completely empty, ignore it.  */
      if (*p == '\0')
        continue;

      p2 = end_of_token (p);
      wlen = p2 - p;
      NEXT_TOKEN (p2);

      /* If we're in an ignored define, skip this line (but maybe get out).  */
      if (in_ignored_define)
        {
          /* See if this is an endef line (plus optional comment).  */
          if (word1eq ("endef") && STOP_SET (*p2, MAP_COMMENT|MAP_NUL))
            in_ignored_define = 0;

          continue;
        }

      /* Check for conditional state changes.  */
      {
        int i = conditional_line (p, wlen, fstart);
        if (i != -2)
          {
            if (i == -1)
              O (fatal, fstart, _("invalid syntax in conditional"));

            ignoring = i;
            continue;
          }
      }

      /* Nothing to see here... move along.  */
      if (ignoring)
        continue;

      /* Manage the "export" keyword used outside of variable assignment
         as well as "unexport".  */
      if (word1eq ("export") || word1eq ("unexport"))
        {
          int exporting = *p == 'u' ? 0 : 1;

          /* Export/unexport ends the previous rule.  */
          record_waiting_files ();

          /* (un)export by itself causes everything to be (un)exported. */
          if (*p2 == '\0')
            export_all_variables = exporting;
          else
            {
              size_t l;
              const char *cp;
              char *ap;

              /* Expand the line so we can use indirect and constructed
                 variable names in an (un)export command.  */
              cp = ap = allocated_variable_expand (p2);

              for (p = find_next_token (&cp, &l); p != 0;
                   p = find_next_token (&cp, &l))
                {
                  struct variable *v = lookup_variable (p, l);
                  if (v == 0)
                    v = define_variable_global (p, l, "", o_file, 0, fstart);
                  v->export = exporting ? v_export : v_noexport;
                }

              free (ap);
            }
          continue;
        }

      /* Handle the special syntax for vpath.  */
      if (word1eq ("vpath"))
        {
          const char *cp;
          char *vpat;
          size_t l;

          /* vpath ends the previous rule.  */
          record_waiting_files ();

          cp = variable_expand (p2);
          p = find_next_token (&cp, &l);
          if (p != 0)
            {
              vpat = xstrndup (p, l);
              p = find_next_token (&cp, &l);
              /* No searchpath means remove all previous
                 selective VPATH's with the same pattern.  */
            }
          else
            /* No pattern means remove all previous selective VPATH's.  */
            vpat = 0;
          construct_vpath_list (vpat, p);
          free (vpat);

          continue;
        }

      /* Handle include and variants.  */
      if (word1eq ("include") || word1eq ("-include") || word1eq ("sinclude"))
        {
          /* We have found an 'include' line specifying a nested
             makefile to be read at this point.  */
          struct conditionals *save;
          struct conditionals new_conditionals;
          struct nameseq *files;
          /* "-include" (vs "include") says no error if the file does not
             exist.  "sinclude" is an alias for this from SGI.  */
          int noerror = (p[0] != 'i');

          /* Include ends the previous rule.  */
          record_waiting_files ();

          p = allocated_variable_expand (p2);

          /* If no filenames, it's a no-op.  */
          if (*p == '\0')
            {
              free (p);
              continue;
            }

          /* Parse the list of file names.  Don't expand archive references!  */
          p2 = p;
          files = PARSE_FILE_SEQ (&p2, struct nameseq, MAP_NUL, NULL,
                                  PARSEFS_NOAR);
          free (p);

          /* Save the state of conditionals and start
             the included makefile with a clean slate.  */
          save = install_conditionals (&new_conditionals);

          /* Record the rules that are waiting so they will determine
             the default goal before those in the included makefile.  */
          record_waiting_files ();

          /* Read each included makefile.  */
          while (files != 0)
            {
              struct nameseq *next = files->next;
              unsigned short flags = (RM_INCLUDED | RM_NO_TILDE
                                      | (noerror ? RM_DONTCARE : 0)
                                      | (set_default ? 0 : RM_NO_DEFAULT_GOAL));

              struct goaldep *d = eval_makefile (files->name, flags);

              if (errno)
                d->floc = *fstart;

              free_ns (files);
              files = next;
            }

          /* Restore conditional state.  */
          restore_conditionals (save);

          continue;
        }

      /* Handle the load operations.  */
      if (word1eq ("load") || word1eq ("-load"))
        {
          /* A 'load' line specifies a dynamic object to load.  */
          struct nameseq *files;
          int noerror = (p[0] == '-');

          /* Load ends the previous rule.  */
          record_waiting_files ();

          p = allocated_variable_expand (p2);

          /* If no filenames, it's a no-op.  */
          if (*p == '\0')
            {
              free (p);
              continue;
            }

          /* Parse the list of file names.
             Don't expand archive references or strip "./"  */
          p2 = p;
          files = PARSE_FILE_SEQ (&p2, struct nameseq, MAP_NUL, NULL,
                                  PARSEFS_NOAR);
          free (p);

          /* Load each file.  */
          while (files != 0)
            {
              struct nameseq *next = files->next;
              const char *name = files->name;
              struct goaldep *deps;
              int r;

              /* Load the file.  0 means failure.  */
              r = load_file (&ebuf->floc, &name, noerror);
              if (! r && ! noerror)
                OS (fatal, &ebuf->floc, _("%s: failed to load"), name);

              free_ns (files);
              files = next;

              /* Return of -1 means a special load: don't rebuild it.  */
              if (r == -1)
                continue;

              /* It succeeded, so add it to the list "to be rebuilt".  */
              deps = alloc_goaldep ();
              deps->next = read_files;
              read_files = deps;
              deps->file = lookup_file (name);
              if (deps->file == 0)
                deps->file = enter_file (name);
              deps->file->loaded = 1;
            }

          continue;
        }

      /* This line starts with a tab but was not caught above because there
         was no preceding target, and the line might have been usable as a
         variable definition.  But now we know it is definitely lossage.  */
      if (line[0] == cmd_prefix)
        O (fatal, fstart, _("recipe commences before first target"));

      /* This line describes some target files.  This is complicated by
         the existence of target-specific variables, because we can't
         expand the entire line until we know if we have one or not.  So
         we expand the line word by word until we find the first ':',
         then check to see if it's a target-specific variable.

         In this algorithm, 'lb_next' will point to the beginning of the
         unexpanded parts of the input buffer, while 'p2' points to the
         parts of the expanded buffer we haven't searched yet. */

      {
        enum make_word_type wtype;
        char *cmdleft, *semip, *lb_next;
        size_t plen = 0;
        char *colonp;
        const char *end, *beg; /* Helpers for whitespace stripping. */

        /* Record the previous rule.  */

        record_waiting_files ();
        tgts_started = fstart->lineno;

        /* Search the line for an unquoted ; that is not after an
           unquoted #.  */
        cmdleft = find_map_unquote (line, MAP_SEMI|MAP_COMMENT|MAP_VARIABLE);
        if (cmdleft != 0 && *cmdleft == '#')
          {
            /* We found a comment before a semicolon.  */
            *cmdleft = '\0';
            cmdleft = 0;
          }
        else if (cmdleft != 0)
          /* Found one.  Cut the line short there before expanding it.  */
          *(cmdleft++) = '\0';
        semip = cmdleft;

        collapse_continuations (line);

        /* We can't expand the entire line, since if it's a per-target
           variable we don't want to expand it.  So, walk from the
           beginning, expanding as we go, and looking for "interesting"
           chars.  The first word is always expandable.  */
        wtype = get_next_mword (line, &lb_next, &wlen);
        switch (wtype)
          {
          case w_eol:
            if (cmdleft != 0)
              O (fatal, fstart, _("missing rule before recipe"));
            /* This line contained something but turned out to be nothing
               but whitespace (a comment?).  */
            continue;

          case w_colon:
          case w_dcolon:
          case w_ampcolon:
          case w_ampdcolon:
            /* We accept and ignore rules without targets for
               compatibility with SunOS 4 make.  */
            no_targets = 1;
            continue;

          default:
            break;
          }

        p2 = variable_expand_string (NULL, lb_next, wlen);

        while (1)
          {
            lb_next += wlen;
            if (cmdleft == 0)
              {
                /* Look for a semicolon in the expanded line.  */
                cmdleft = find_char_unquote (p2, ';');

                if (cmdleft != 0)
                  {
                    size_t p2_off = p2 - variable_buffer;
                    size_t cmd_off = cmdleft - variable_buffer;
                    char *pend = p2 + strlen (p2);

                    /* Append any remnants of lb, then cut the line short
                       at the semicolon.  */
                    *cmdleft = '\0';

                    /* One school of thought says that you shouldn't expand
                       here, but merely copy, since now you're beyond a ";"
                       and into a command script.  However, the old parser
                       expanded the whole line, so we continue that for
                       backwards-compatibility.  Also, it wouldn't be
                       entirely consistent, since we do an unconditional
                       expand below once we know we don't have a
                       target-specific variable. */
                    variable_expand_string (pend, lb_next, SIZE_MAX);
                    lb_next += strlen (lb_next);
                    p2 = variable_buffer + p2_off;
                    cmdleft = variable_buffer + cmd_off + 1;
                  }
              }

            colonp = find_char_unquote (p2, ':');

#ifdef HAVE_DOS_PATHS
            if (colonp > p2)
              /* The drive spec brain-damage strikes again...
                 Note that the only separators of targets in this context are
                 whitespace and a left paren.  If others are possible, add them
                 to the string in the call to strchr.  */
              while (colonp && (colonp[1] == '/' || colonp[1] == '\\') &&
                     isalpha ((unsigned char) colonp[-1]) &&
                     (colonp == p2 + 1 || strchr (" \t(", colonp[-2]) != 0))
                colonp = find_char_unquote (colonp + 1, ':');
#endif

            if (colonp)
              {
                /* If the previous character is '&', back up before '&:' */
                if (colonp > p2 && colonp[-1] == '&')
                  --colonp;

                break;
              }

            wtype = get_next_mword (lb_next, &lb_next, &wlen);
            if (wtype == w_eol)
              break;

            p2 += strlen (p2);
            *(p2++) = ' ';
            p2 = variable_expand_string (p2, lb_next, wlen);
            /* We don't need to worry about cmdleft here, because if it was
               found in the variable_buffer the entire buffer has already
               been expanded... we'll never get here.  */
          }

        p2 = next_token (variable_buffer);

        /* If the word we're looking at is EOL, see if there's _anything_
           on the line.  If not, a variable expanded to nothing, so ignore
           it.  If so, we can't parse this line so punt.  */
        if (wtype == w_eol)
          {
            if (*p2 == '\0')
              continue;

            /* There's no need to be ivory-tower about this: check for
               one of the most common bugs found in makefiles...  */
            if (cmd_prefix == '\t' && strneq (line, "        ", 8))
              O (fatal, fstart, _("missing separator (did you mean TAB instead of 8 spaces?)"));
            else
              O (fatal, fstart, _("missing separator"));
          }

        {
          char save = *colonp;

          /* If we have &:, it specifies that the targets are understood to be
             updated/created together by a single invocation of the recipe. */
          if (save == '&')
            also_make_targets = 1;

          /* Make the colon the end-of-string so we know where to stop
             looking for targets.  Start there again once we're done.  */
          *colonp = '\0';
          filenames = PARSE_SIMPLE_SEQ (&p2, struct nameseq);
          *colonp = save;
          p2 = colonp + (save == '&');
        }

        if (!filenames)
          {
            /* We accept and ignore rules without targets for
               compatibility with SunOS 4 make.  */
            no_targets = 1;
            continue;
          }
        /* This should never be possible; we handled it above.  */
        assert (*p2 != '\0');
        ++p2;

        /* Is this a one-colon or two-colon entry?  */
        two_colon = *p2 == ':';
        if (two_colon)
          p2++;

        /* Test to see if it's a target-specific variable.  Copy the rest
           of the buffer over, possibly temporarily (we'll expand it later
           if it's not a target-specific variable).  PLEN saves the length
           of the unparsed section of p2, for later.  */
        if (*lb_next != '\0')
          {
            size_t l = p2 - variable_buffer;
            plen = strlen (p2);
            variable_buffer_output (p2+plen, lb_next, strlen (lb_next)+1);
            p2 = variable_buffer + l;
          }

        p2 = parse_var_assignment (p2, &vmod);
        if (vmod.assign_v)
          {
            /* If there was a semicolon found, add it back, plus anything
               after it.  */
            if (semip)
              {
                size_t l = p2 - variable_buffer;
                *(--semip) = ';';
                collapse_continuations (semip);
                variable_buffer_output (p2 + strlen (p2),
                                        semip, strlen (semip)+1);
                p2 = variable_buffer + l;
              }
            record_target_var (filenames, p2,
                               vmod.override_v ? o_override : o_file,
                               &vmod, fstart);
            filenames = 0;
            continue;
          }

        /* This is a normal target, _not_ a target-specific variable.
           Unquote any = in the dependency list.  */
        find_char_unquote (lb_next, '=');

        /* Remember the command prefix for this target.  */
        prefix = cmd_prefix;

        /* We have some targets, so don't ignore the following commands.  */
        no_targets = 0;

        /* Expand the dependencies, etc.  */
        if (*lb_next != '\0')
          {
            size_t l = p2 - variable_buffer;
            variable_expand_string (p2 + plen, lb_next, SIZE_MAX);
            p2 = variable_buffer + l;

            /* Look for a semicolon in the expanded line.  */
            if (cmdleft == 0)
              {
                cmdleft = find_char_unquote (p2, ';');
                if (cmdleft != 0)
                  *(cmdleft++) = '\0';
              }
          }

        /* Is this a static pattern rule: 'target: %targ: %dep; ...'?  */
        p = strchr (p2, ':');
        while (p != 0 && p[-1] == '\\')
          {
            char *q = &p[-1];
            int backslash = 0;
            while (*q-- == '\\')
              backslash = !backslash;
            if (backslash)
              p = strchr (p + 1, ':');
            else
              break;
          }
#ifdef _AMIGA
        /* Here, the situation is quite complicated. Let's have a look
           at a couple of targets:

           install: dev:make

           dev:make: make

           dev:make:: xyz

           The rule is that it's only a target, if there are TWO :'s
           OR a space around the :.
        */
        if (p && !(ISSPACE (p[1]) || !p[1] || ISSPACE (p[-1])))
          p = 0;
#endif
#ifdef HAVE_DOS_PATHS
        {
          int check_again;
          do {
            check_again = 0;
            /* For DOS-style paths, skip a "C:\..." or a "C:/..." */
            if (p != 0 && (p[1] == '\\' || p[1] == '/') &&
                isalpha ((unsigned char)p[-1]) &&
                (p == p2 + 1 || strchr (" \t:(", p[-2]) != 0)) {
              p = strchr (p + 1, ':');
              check_again = 1;
            }
          } while (check_again);
        }
#endif
        if (p != 0)
          {
            struct nameseq *target;
            target = PARSE_FILE_SEQ (&p2, struct nameseq, MAP_COLON, NULL,
                                     PARSEFS_NOGLOB);
            ++p2;
            if (target == 0)
              O (fatal, fstart, _("missing target pattern"));
            else if (target->next != 0)
              O (fatal, fstart, _("multiple target patterns"));
            pattern_percent = find_percent_cached (&target->name);
            pattern = target->name;
            if (pattern_percent == 0)
              O (fatal, fstart, _("target pattern contains no '%%'"));
            free_ns (target);
          }
        else
          pattern = 0;

        /* Strip leading and trailing whitespaces. */
        beg = p2;
        end = beg + strlen (beg) - 1;
        strip_whitespace (&beg, &end);

        /* Put all the prerequisites here; they'll be parsed later.  */
        if (beg <= end && *beg != '\0')
          depstr = xstrndup (beg, end - beg + 1);
        else
          depstr = 0;

        commands_idx = 0;
        if (cmdleft != 0)
          {
            /* Semicolon means rest of line is a command.  */
            size_t l = strlen (cmdleft);

            cmds_started = fstart->lineno;

            /* Add this command line to the buffer.  */
            if (l + 2 > commands_len)
              {
                commands_len = (l + 2) * 2;
                commands = xrealloc (commands, commands_len);
              }
            memcpy (commands, cmdleft, l);
            commands_idx += l;
            commands[commands_idx++] = '\n';
          }

        /* Determine if this target should be made default. We used to do
           this in record_files() but because of the delayed target recording
           and because preprocessor directives are legal in target's commands
           it is too late. Consider this fragment for example:

           foo:

           ifeq ($(.DEFAULT_GOAL),foo)
              ...
           endif

           Because the target is not recorded until after ifeq directive is
           evaluated the .DEFAULT_GOAL does not contain foo yet as one
           would expect. Because of this we have to move the logic here.  */

        if (set_default && default_goal_var->value[0] == '\0')
          {
            struct dep *d;
            struct nameseq *t = filenames;

            for (; t != 0; t = t->next)
              {
                int reject = 0;
                const char *name = t->name;

                /* We have nothing to do if this is an implicit rule. */
                if (strchr (name, '%') != 0)
                  break;

                /* See if this target's name does not start with a '.',
                   unless it contains a slash.  */
                if (*name == '.' && strchr (name, '/') == 0
#ifdef HAVE_DOS_PATHS
                    && strchr (name, '\\') == 0
#endif
                    )
                  continue;


                /* If this file is a suffix, don't let it be
                   the default goal file.  */
                for (d = suffix_file->deps; d != 0; d = d->next)
                  {
                    struct dep *d2;
                    if (*dep_name (d) != '.' && streq (name, dep_name (d)))
                      {
                        reject = 1;
                        break;
                      }
                    for (d2 = suffix_file->deps; d2 != 0; d2 = d2->next)
                      {
                        size_t l = strlen (dep_name (d2));
                        if (!strneq (name, dep_name (d2), l))
                          continue;
                        if (streq (name + l, dep_name (d)))
                          {
                            reject = 1;
                            break;
                          }
                      }

                    if (reject)
                      break;
                  }

                if (!reject)
                  {
                    define_variable_global (".DEFAULT_GOAL", 13, t->name,
                                            o_file, 0, NILF);
                    break;
                  }
              }
          }
      }
    }

#undef word1eq

  if (conditionals->if_cmds)
    O (fatal, fstart, _("missing 'endif'"));

  /* At eof, record the last rule.  */
  record_waiting_files ();

  free (collapsed);
  free (commands);
}


/* Remove comments from LINE.
   This will also remove backslashes that escape things.
   It ignores comment characters that appear inside variable references.  */

static void
remove_comments (char *line)
{
  char *comment;

  comment = find_map_unquote (line, MAP_COMMENT|MAP_VARIABLE);

  if (comment != 0)
    /* Cut off the line at the #.  */
    *comment = '\0';
}

/* Execute a 'undefine' directive.
   The undefine line has already been read, and NAME is the name of
   the variable to be undefined. */

static void
do_undefine (char *name, enum variable_origin origin, struct ebuffer *ebuf)
{
  char *p, *var;

  /* Expand the variable name and find the beginning (NAME) and end.  */
  var = allocated_variable_expand (name);
  name = next_token (var);
  if (*name == '\0')
    O (fatal, &ebuf->floc, _("empty variable name"));
  p = name + strlen (name) - 1;
  while (p > name && ISBLANK (*p))
    --p;
  p[1] = '\0';

  undefine_variable_global (name, p - name + 1, origin);
  free (var);
}

/* Execute a 'define' directive.
   The first line has already been read, and NAME is the name of
   the variable to be defined.  The following lines remain to be read.  */

static struct variable *
do_define (char *name, enum variable_origin origin, struct ebuffer *ebuf)
{
  struct variable *v;
  struct variable var;
  floc defstart;
  int nlevels = 1;
  size_t length = 100;
  char *definition = xmalloc (length);
  size_t idx = 0;
  char *p, *n;

  defstart = ebuf->floc;

  p = parse_variable_definition (name, &var);
  if (p == NULL)
    /* No assignment token, so assume recursive.  */
    var.flavor = f_recursive;
  else
    {
      if (var.value[0] != '\0')
        O (error, &defstart, _("extraneous text after 'define' directive"));

      /* Chop the string before the assignment token to get the name.  */
      var.name[var.length] = '\0';
    }

  /* Expand the variable name and find the beginning (NAME) and end.  */
  n = allocated_variable_expand (name);
  name = next_token (n);
  if (name[0] == '\0')
    O (fatal, &defstart, _("empty variable name"));
  p = name + strlen (name) - 1;
  while (p > name && ISBLANK (*p))
    --p;
  p[1] = '\0';

  /* Now read the value of the variable.  */
  while (1)
    {
      size_t len;
      char *line;
      long nlines = readline (ebuf);

      /* If there is nothing left to be eval'd, there's no 'endef'!!  */
      if (nlines < 0)
        O (fatal, &defstart, _("missing 'endef', unterminated 'define'"));

      ebuf->floc.lineno += nlines;
      line = ebuf->buffer;

      collapse_continuations (line);

      /* If the line doesn't begin with a tab, test to see if it introduces
         another define, or ends one.  Stop if we find an 'endef' */
      if (line[0] != cmd_prefix)
        {
          p = next_token (line);
          len = strlen (p);

          /* If this is another 'define', increment the level count.  */
          if ((len == 6 || (len > 6 && ISBLANK (p[6])))
              && strneq (p, "define", 6))
            ++nlevels;

          /* If this is an 'endef', decrement the count.  If it's now 0,
             we've found the last one.  */
          else if ((len == 5 || (len > 5 && ISBLANK (p[5])))
                   && strneq (p, "endef", 5))
            {
              p += 5;
              remove_comments (p);
              if (*(next_token (p)) != '\0')
                O (error, &ebuf->floc,
                   _("extraneous text after 'endef' directive"));

              if (--nlevels == 0)
                break;
            }
        }

      /* Add this line to the variable definition.  */
      len = strlen (line);
      if (idx + len + 1 > length)
        {
          length = (idx + len) * 2;
          definition = xrealloc (definition, length + 1);
        }

      memcpy (&definition[idx], line, len);
      idx += len;
      /* Separate lines with a newline.  */
      definition[idx++] = '\n';
    }

  /* We've got what we need; define the variable.  */
  if (idx == 0)
    definition[0] = '\0';
  else
    definition[idx - 1] = '\0';

  v = do_variable_definition (&defstart, name,
                              definition, origin, var.flavor, 0);
  free (definition);
  free (n);
  return (v);
}

/* Interpret conditional commands "ifdef", "ifndef", "ifeq",
   "ifneq", "else" and "endif".
   LINE is the input line, with the command as its first word.

   FILENAME and LINENO are the filename and line number in the
   current makefile.  They are used for error messages.

   Value is -2 if the line is not a conditional at all,
   -1 if the line is an invalid conditional,
   0 if following text should be interpreted,
   1 if following text should be ignored.  */

static int
conditional_line (char *line, size_t len, const floc *flocp)
{
  const char *cmdname;
  enum { c_ifdef, c_ifndef, c_ifeq, c_ifneq, c_else, c_endif } cmdtype;
  unsigned int i;
  unsigned int o;

  /* Compare a word, both length and contents. */
#define word1eq(s)      (len == CSTRLEN (s) && strneq (s, line, CSTRLEN (s)))
#define chkword(s, t)   if (word1eq (s)) { cmdtype = (t); cmdname = (s); }

  /* Make sure this line is a conditional.  */
  chkword ("ifdef", c_ifdef)
  else chkword ("ifndef", c_ifndef)
  else chkword ("ifeq", c_ifeq)
  else chkword ("ifneq", c_ifneq)
  else chkword ("else", c_else)
  else chkword ("endif", c_endif)
  else
    return -2;

  /* Found one: skip past it and any whitespace after it.  */
  line += len;
  NEXT_TOKEN (line);

#define EXTRATEXT() OS (error, flocp, _("extraneous text after '%s' directive"), cmdname)
#define EXTRACMD()  OS (fatal, flocp, _("extraneous '%s'"), cmdname)

  /* An 'endif' cannot contain extra text, and reduces the if-depth by 1  */
  if (cmdtype == c_endif)
    {
      if (*line != '\0')
        EXTRATEXT ();

      if (!conditionals->if_cmds)
        EXTRACMD ();

      --conditionals->if_cmds;

      goto DONE;
    }

  /* An 'else' statement can either be simple, or it can have another
     conditional after it.  */
  if (cmdtype == c_else)
    {
      const char *p;

      if (!conditionals->if_cmds)
        EXTRACMD ();

      o = conditionals->if_cmds - 1;

      if (conditionals->seen_else[o])
        O (fatal, flocp, _("only one 'else' per conditional"));

      /* Change the state of ignorance.  */
      switch (conditionals->ignoring[o])
        {
          case 0:
            /* We've just been interpreting.  Never do it again.  */
            conditionals->ignoring[o] = 2;
            break;
          case 1:
            /* We've never interpreted yet.  Maybe this time!  */
            conditionals->ignoring[o] = 0;
            break;
        }

      /* It's a simple 'else'.  */
      if (*line == '\0')
        {
          conditionals->seen_else[o] = 1;
          goto DONE;
        }

      /* The 'else' has extra text.  That text must be another conditional
         and cannot be an 'else' or 'endif'.  */

      /* Find the length of the next word.  */
      for (p = line+1; ! STOP_SET (*p, MAP_SPACE|MAP_NUL); ++p)
        ;
      len = p - line;

      /* If it's 'else' or 'endif' or an illegal conditional, fail.  */
      if (word1eq ("else") || word1eq ("endif")
          || conditional_line (line, len, flocp) < 0)
        EXTRATEXT ();
      else
        {
          /* conditional_line() created a new level of conditional.
             Raise it back to this level.  */
          if (conditionals->ignoring[o] < 2)
            conditionals->ignoring[o] = conditionals->ignoring[o+1];
          --conditionals->if_cmds;
        }

      goto DONE;
    }

  if (conditionals->allocated == 0)
    {
      conditionals->allocated = 5;
      conditionals->ignoring = xmalloc (conditionals->allocated);
      conditionals->seen_else = xmalloc (conditionals->allocated);
    }

  o = conditionals->if_cmds++;
  if (conditionals->if_cmds > conditionals->allocated)
    {
      conditionals->allocated += 5;
      conditionals->ignoring = xrealloc (conditionals->ignoring,
                                         conditionals->allocated);
      conditionals->seen_else = xrealloc (conditionals->seen_else,
                                          conditionals->allocated);
    }

  /* Record that we have seen an 'if...' but no 'else' so far.  */
  conditionals->seen_else[o] = 0;

  /* Search through the stack to see if we're already ignoring.  */
  for (i = 0; i < o; ++i)
    if (conditionals->ignoring[i])
      {
        /* We are already ignoring, so just push a level to match the next
           "else" or "endif", and keep ignoring.  We don't want to expand
           variables in the condition.  */
        conditionals->ignoring[o] = 1;
        return 1;
      }

  if (cmdtype == c_ifdef || cmdtype == c_ifndef)
    {
      size_t l;
      char *var;
      struct variable *v;
      char *p;

      /* Expand the thing we're looking up, so we can use indirect and
         constructed variable names.  */
      var = allocated_variable_expand (line);

      /* Make sure there's only one variable name to test.  */
      p = end_of_token (var);
      l = p - var;
      NEXT_TOKEN (p);
      if (*p != '\0')
        return -1;

      var[l] = '\0';
      v = lookup_variable (var, l);

      conditionals->ignoring[o] =
        ((v != 0 && *v->value != '\0') == (cmdtype == c_ifndef));

      free (var);
    }
  else
    {
      /* "ifeq" or "ifneq".  */
      char *s1, *s2;
      size_t l;
      char termin = *line == '(' ? ',' : *line;

      if (termin != ',' && termin != '"' && termin != '\'')
        return -1;

      s1 = ++line;
      /* Find the end of the first string.  */
      if (termin == ',')
        {
          int count = 0;
          for (; *line != '\0'; ++line)
            if (*line == '(')
              ++count;
            else if (*line == ')')
              --count;
            else if (*line == ',' && count <= 0)
              break;
        }
      else
        while (*line != '\0' && *line != termin)
          ++line;

      if (*line == '\0')
        return -1;

      if (termin == ',')
        {
          /* Strip blanks after the first string.  */
          char *p = line++;
          while (ISBLANK (p[-1]))
            --p;
          *p = '\0';
        }
      else
        *line++ = '\0';

      s2 = variable_expand (s1);
      /* We must allocate a new copy of the expanded string because
         variable_expand re-uses the same buffer.  */
      l = strlen (s2);
      s1 = alloca (l + 1);
      memcpy (s1, s2, l + 1);

      if (termin != ',')
        /* Find the start of the second string.  */
        NEXT_TOKEN (line);

      termin = termin == ',' ? ')' : *line;
      if (termin != ')' && termin != '"' && termin != '\'')
        return -1;

      /* Find the end of the second string.  */
      if (termin == ')')
        {
          int count = 0;
          s2 = next_token (line);
          for (line = s2; *line != '\0'; ++line)
            {
              if (*line == '(')
                ++count;
              else if (*line == ')')
                {
                  if (count <= 0)
                    break;
                  else
                    --count;
                }
            }
        }
      else
        {
          ++line;
          s2 = line;
          while (*line != '\0' && *line != termin)
            ++line;
        }

      if (*line == '\0')
        return -1;

      *(line++) = '\0';
      NEXT_TOKEN (line);
      if (*line != '\0')
        EXTRATEXT ();

      s2 = variable_expand (s2);
      conditionals->ignoring[o] = (streq (s1, s2) == (cmdtype == c_ifneq));
    }

 DONE:
  /* Search through the stack to see if we're ignoring.  */
  for (i = 0; i < conditionals->if_cmds; ++i)
    if (conditionals->ignoring[i])
      return 1;
  return 0;
}


/* Record target-specific variable values for files FILENAMES.
   TWO_COLON is nonzero if a double colon was used.

   The links of FILENAMES are freed, and so are any names in it
   that are not incorporated into other data structures.

   If the target is a pattern, add the variable to the pattern-specific
   variable value list.  */

static void
record_target_var (struct nameseq *filenames, char *defn,
                   enum variable_origin origin, struct vmodifiers *vmod,
                   const floc *flocp)
{
  struct nameseq *nextf;
  struct variable_set_list *global;

  global = current_variable_set_list;

  /* If the variable is an append version, store that but treat it as a
     normal recursive variable.  */

  for (; filenames != 0; filenames = nextf)
    {
      struct variable *v;
      const char *name = filenames->name;
      const char *percent;
      struct pattern_var *p;

      nextf = filenames->next;
      free_ns (filenames);

      /* If it's a pattern target, then add it to the pattern-specific
         variable list.  */
      percent = find_percent_cached (&name);
      if (percent)
        {
          /* Get a reference for this pattern-specific variable struct.  */
          p = create_pattern_var (name, percent);
          p->variable.fileinfo = *flocp;
          /* I don't think this can fail since we already determined it was a
             variable definition.  */
          v = assign_variable_definition (&p->variable, defn);
          assert (v != 0);

          v->origin = origin;
          if (v->flavor == f_simple)
            v->value = allocated_variable_expand (v->value);
          else
            v->value = xstrdup (v->value);
        }
      else
        {
          struct file *f;

          /* Get a file reference for this file, and initialize it.
             We don't want to just call enter_file() because that allocates a
             new entry if the file is a double-colon, which we don't want in
             this situation.  */
          f = lookup_file (name);
          if (!f)
            f = enter_file (strcache_add (name));
          else if (f->double_colon)
            f = f->double_colon;

          initialize_file_variables (f, 1);

          current_variable_set_list = f->variables;
          v = try_variable_definition (flocp, defn, origin, 1);
          if (!v)
            O (fatal, flocp, _("Malformed target-specific variable definition"));
          current_variable_set_list = global;
        }

      /* Set up the variable to be *-specific.  */
      v->per_target = 1;
      v->private_var = vmod->private_v;
      v->export = vmod->export_v ? v_export : v_default;

      /* If it's not an override, check to see if there was a command-line
         setting.  If so, reset the value.  */
      if (v->origin != o_override)
        {
          struct variable *gv;
          size_t len = strlen (v->name);

          gv = lookup_variable (v->name, len);
          if (gv && v != gv
              && (gv->origin == o_env_override || gv->origin == o_command))
            {
              free (v->value);
              v->value = xstrdup (gv->value);
              v->origin = gv->origin;
              v->recursive = gv->recursive;
              v->append = 0;
            }
        }
    }
}

/* Record a description line for files FILENAMES,
   with dependencies DEPS, commands to execute described
   by COMMANDS and COMMANDS_IDX, coming from FILENAME:COMMANDS_STARTED.
   TWO_COLON is nonzero if a double colon was used.
   If not nil, PATTERN is the '%' pattern to make this
   a static pattern rule, and PATTERN_PERCENT is a pointer
   to the '%' within it.

   The links of FILENAMES are freed, and so are any names in it
   that are not incorporated into other data structures.  */

static void
record_files (struct nameseq *filenames, int are_also_makes,
              const char *pattern,
              const char *pattern_percent, char *depstr,
              unsigned int cmds_started, char *commands,
              size_t commands_idx, int two_colon,
              char prefix, const floc *flocp)
{
  struct commands *cmds;
  struct dep *deps;
  struct dep *also_make = NULL;
  const char *implicit_percent;
  const char *name;

  /* If we've already snapped deps, that means we're in an eval being
     resolved after the makefiles have been read in.  We can't add more rules
     at this time, since they won't get snapped and we'll get core dumps.
     See Savannah bug # 12124.  */
  if (snapped_deps)
    O (fatal, flocp, _("prerequisites cannot be defined in recipes"));

  /* Determine if this is a pattern rule or not.  */
  name = filenames->name;
  implicit_percent = find_percent_cached (&name);

  /* If there's a recipe, set up a struct for it.  */
  if (commands_idx > 0)
    {
      cmds = xmalloc (sizeof (struct commands));
      cmds->fileinfo.filenm = flocp->filenm;
      cmds->fileinfo.lineno = cmds_started;
      cmds->fileinfo.offset = 0;
      cmds->commands = xstrndup (commands, commands_idx);
      cmds->command_lines = 0;
      cmds->recipe_prefix = prefix;
    }
  else if (are_also_makes)
    O (fatal, flocp, _("grouped targets must provide a recipe"));
  else
     cmds = NULL;

  /* If there's a prereq string then parse it--unless it's eligible for 2nd
     expansion: if so, snap_deps() will do it.  */
  if (depstr == 0)
    deps = 0;
  else
    {
      depstr = unescape_char (depstr, ':');
      if (second_expansion && strchr (depstr, '$'))
        {
          deps = alloc_dep ();
          deps->name = depstr;
          deps->need_2nd_expansion = 1;
          deps->staticpattern = pattern != 0;
        }
      else
        {
          deps = split_prereqs (depstr);
          free (depstr);

          /* We'll enter static pattern prereqs later when we have the stem.
             We don't want to enter pattern rules at all so that we don't
             think that they ought to exist (make manual "Implicit Rule Search
             Algorithm", item 5c).  */
          if (! pattern && ! implicit_percent)
            deps = enter_prereqs (deps, NULL);
        }
    }

  /* For implicit rules, _all_ the targets must have a pattern.  That means we
     can test the first one to see if we're working with an implicit rule; if
     so we handle it specially. */

  if (implicit_percent)
    {
      struct nameseq *nextf;
      const char **targets, **target_pats;
      unsigned short c;

      if (pattern != 0)
        O (fatal, flocp, _("mixed implicit and static pattern rules"));

      /* Count the targets to create an array of target names.
         We already have the first one.  */
      nextf = filenames->next;
      free_ns (filenames);
      filenames = nextf;

      for (c = 1; nextf; ++c, nextf = nextf->next)
        ;
      targets = xmalloc (c * sizeof (const char *));
      target_pats = xmalloc (c * sizeof (const char *));

      targets[0] = name;
      target_pats[0] = implicit_percent;

      c = 1;
      while (filenames)
        {
          name = filenames->name;
          implicit_percent = find_percent_cached (&name);

          if (implicit_percent == 0)
            O (fatal, flocp, _("mixed implicit and normal rules"));

          targets[c] = name;
          target_pats[c] = implicit_percent;
          ++c;

          nextf = filenames->next;
          free_ns (filenames);
          filenames = nextf;
        }

      create_pattern_rule (targets, target_pats, c, two_colon, deps, cmds, 1);

      return;
    }


  /* Walk through each target and create it in the database.
     We already set up the first target, above.  */
  while (1)
    {
      struct nameseq *nextf = filenames->next;
      struct file *f;
      struct dep *this = 0;

      free_ns (filenames);

      /* Check for special targets.  Do it here instead of, say, snap_deps()
         so that we can immediately use the value.  */
      if (!posix_pedantic && streq (name, ".POSIX"))
        {
          posix_pedantic = 1;
          define_variable_cname (".SHELLFLAGS", "-ec", o_default, 0);
          /* These default values are based on IEEE Std 1003.1-2008.
             It requires '-O 1' for [CF]FLAGS, but GCC doesn't allow space
             between -O and the number so omit it here.  */
          define_variable_cname ("ARFLAGS", "-rv", o_default, 0);
          define_variable_cname ("CC", "c99", o_default, 0);
          define_variable_cname ("CFLAGS", "-O1", o_default, 0);
          define_variable_cname ("FC", "fort77", o_default, 0);
          define_variable_cname ("FFLAGS", "-O1", o_default, 0);
          define_variable_cname ("SCCSGETFLAGS", "-s", o_default, 0);
        }
      else if (!second_expansion && streq (name, ".SECONDEXPANSION"))
        second_expansion = 1;
#if !defined (__MSDOS__) && !defined (__EMX__)
      else if (!one_shell && streq (name, ".ONESHELL"))
        one_shell = 1;
#endif

      /* If this is a static pattern rule:
         'targets: target%pattern: prereq%pattern; recipe',
         make sure the pattern matches this target name.  */
      if (pattern && !pattern_matches (pattern, pattern_percent, name))
        OS (error, flocp,
            _("target '%s' doesn't match the target pattern"), name);
      else if (deps)
        /* If there are multiple targets, copy the chain DEPS for all but the
           last one.  It is not safe for the same deps to go in more than one
           place in the database.  */
        this = nextf != 0 ? copy_dep_chain (deps) : deps;

      /* Find or create an entry in the file database for this target.  */
      if (!two_colon)
        {
          /* Single-colon.  Combine this rule with the file's existing record,
             if any.  */
          f = enter_file (strcache_add (name));
          if (f->double_colon)
            OS (fatal, flocp,
                _("target file '%s' has both : and :: entries"), f->name);

          /* If CMDS == F->CMDS, this target was listed in this rule
             more than once.  Just give a warning since this is harmless.  */
          if (cmds != 0 && cmds == f->cmds)
            OS (error, flocp,
                _("target '%s' given more than once in the same rule"),
                f->name);

          /* Check for two single-colon entries both with commands.
             Check is_target so that we don't lose on files such as .c.o
             whose commands were preinitialized.  */
          else if (cmds != 0 && f->cmds != 0 && f->is_target)
            {
              size_t l = strlen (f->name);
              error (&cmds->fileinfo, l,
                     _("warning: overriding recipe for target '%s'"),
                     f->name);
              error (&f->cmds->fileinfo, l,
                     _("warning: ignoring old recipe for target '%s'"),
                     f->name);
            }

          /* Defining .DEFAULT with no deps or cmds clears it.  */
          if (f == default_file && this == 0 && cmds == 0)
            f->cmds = 0;
          if (cmds != 0)
            f->cmds = cmds;

          /* Defining .SUFFIXES with no dependencies clears out the list of
             suffixes.  */
          if (f == suffix_file && this == 0)
            {
              free_dep_chain (f->deps);
              f->deps = 0;
            }
        }
      else
        {
          /* Double-colon.  Make a new record even if there already is one.  */
          f = lookup_file (name);

          /* Check for both : and :: rules.  Check is_target so we don't lose
             on default suffix rules or makefiles.  */
          if (f != 0 && f->is_target && !f->double_colon)
            OS (fatal, flocp,
                _("target file '%s' has both : and :: entries"), f->name);

          f = enter_file (strcache_add (name));
          /* If there was an existing entry and it was a double-colon entry,
             enter_file will have returned a new one, making it the prev
             pointer of the old one, and setting its double_colon pointer to
             the first one.  */
          if (f->double_colon == 0)
            /* This is the first entry for this name, so we must set its
               double_colon pointer to itself.  */
            f->double_colon = f;

          f->cmds = cmds;
        }

      if (are_also_makes)
        {
          struct dep *also = alloc_dep();
          also->name = f->name;
          also->file = f;
          also->next = also_make;
          also_make = also;
        }

      f->is_target = 1;

      /* If this is a static pattern rule, set the stem to the part of its
         name that matched the '%' in the pattern, so you can use $* in the
         commands.  If we didn't do it before, enter the prereqs now.  */
      if (pattern)
        {
          static const char *percent = "%";
          char *buffer = variable_expand ("");
          char *o = patsubst_expand_pat (buffer, name, pattern, percent,
                                         pattern_percent+1, percent+1);
          f->stem = strcache_add_len (buffer, o - buffer);
          if (this)
            {
              if (! this->need_2nd_expansion)
                this = enter_prereqs (this, f->stem);
              else
                this->stem = f->stem;
            }
        }

      /* Add the dependencies to this file entry.  */
      if (this != 0)
        {
          /* Add the file's old deps and the new ones in THIS together.  */
          if (f->deps == 0)
            f->deps = this;
          else if (cmds != 0)
            {
              struct dep *d = this;

              /* If this rule has commands, put these deps first.  */
              while (d->next != 0)
                d = d->next;

              d->next = f->deps;
              f->deps = this;
            }
          else
            {
              struct dep *d = f->deps;

              /* A rule without commands: put its prereqs at the end.  */
              while (d->next != 0)
                d = d->next;

              d->next = this;
            }
        }

      name = f->name;

      /* All done!  Set up for the next one.  */
      if (nextf == 0)
        break;

      filenames = nextf;

      /* Reduce escaped percents.  If there are any unescaped it's an error  */
      name = filenames->name;
      if (find_percent_cached (&name))
        O (error, flocp,
           _("*** mixed implicit and normal rules: deprecated syntax"));
    }

  /* If there are also-makes, then populate a copy of the also-make list into
     each one. For the last file, we take our original also_make list instead
     wastefully copying it one more time and freeing it.  */
  {
    struct dep *i;

    for (i = also_make; i != NULL; i = i->next)
      {
        struct file *f = i->file;
        struct dep *cpy = i->next ? copy_dep_chain (also_make) : also_make;

        if (f->also_make)
          {
            OS (error, &cmds->fileinfo,
                _("warning: overriding group membership for target '%s'"),
                f->name);
            free_dep_chain (f->also_make);
          }

        f->also_make = cpy;
      }
    }
}

/* Search STRING for an unquoted STOPMAP.
   Backslashes quote elements from STOPMAP and backslash.
   Quoting backslashes are removed from STRING by compacting it into itself.
   Returns a pointer to the first unquoted STOPCHAR if there is one, or nil if
   there are none.

   If MAP_VARIABLE is set, then the complete contents of variable references
   are skipped, even if the contain STOPMAP characters.  */

static char *
find_map_unquote (char *string, int stopmap)
{
  size_t string_len = 0;
  char *p = string;

  /* Always stop on NUL.  */
  stopmap |= MAP_NUL;

  while (1)
    {
      while (! STOP_SET (*p, stopmap))
        ++p;

      if (*p == '\0')
        break;

      /* If we stopped due to a variable reference, skip over its contents.  */
      if (*p == '$')
        {
          char openparen = p[1];

          /* Check if '$' is the last character in the string.  */
          if (openparen == '\0')
            break;

          p += 2;

          /* Skip the contents of a non-quoted, multi-char variable ref.  */
          if (openparen == '(' || openparen == '{')
            {
              unsigned int pcount = 1;
              char closeparen = (openparen == '(' ? ')' : '}');

              while (*p)
                {
                  if (*p == openparen)
                    ++pcount;
                  else if (*p == closeparen)
                    if (--pcount == 0)
                      {
                        ++p;
                        break;
                      }
                  ++p;
                }
            }

          /* Skipped the variable reference: look for STOPCHARS again.  */
          continue;
        }

      if (p > string && p[-1] == '\\')
        {
          /* Search for more backslashes.  */
          int i = -2;
          while (&p[i] >= string && p[i] == '\\')
            --i;
          ++i;
          /* Only compute the length if really needed.  */
          if (string_len == 0)
            string_len = strlen (string);
          /* The number of backslashes is now -I.
             Copy P over itself to swallow half of them.  */
          memmove (&p[i], &p[i/2], (string_len - (p - string)) - (i/2) + 1);
          p += i/2;
          if (i % 2 == 0)
            /* All the backslashes quoted each other; the STOPCHAR was
               unquoted.  */
            return p;

          /* The STOPCHAR was quoted by a backslash.  Look for another.  */
        }
      else
        /* No backslash in sight.  */
        return p;
    }

  /* Never hit a STOPCHAR or blank (with BLANK nonzero).  */
  return 0;
}

static char *
find_char_unquote (char *string, int stop)
{
  size_t string_len = 0;
  char *p = string;

  while (1)
    {
      p = strchr(p, stop);

      if (!p)
        return NULL;

      if (p > string && p[-1] == '\\')
        {
          /* Search for more backslashes.  */
          int i = -2;
          while (&p[i] >= string && p[i] == '\\')
            --i;
          ++i;
          /* Only compute the length if really needed.  */
          if (string_len == 0)
            string_len = strlen (string);
          /* The number of backslashes is now -I.
             Copy P over itself to swallow half of them.  */
          memmove (&p[i], &p[i/2], (string_len - (p - string)) - (i/2) + 1);
          p += i/2;
          if (i % 2 == 0)
            /* All the backslashes quoted each other; the STOPCHAR was
               unquoted.  */
            return p;

          /* The STOPCHAR was quoted by a backslash.  Look for another.  */
        }
      else
        /* No backslash in sight.  */
        return p;
    }
}

/* Unescape a character in a string.  The string is compressed onto itself.  */

static char *
unescape_char (char *string, int c)
{
  char *p = string;
  char *s = string;

  while (*s != '\0')
    {
      if (*s == '\\')
        {
          char *e = s;
          size_t l;

          /* We found a backslash.  See if it's escaping our character.  */
          while (*e == '\\')
            ++e;
          l = e - s;

          if (*e != c || l%2 == 0)
            {
              /* It's not; just take it all without unescaping.  */
              memmove (p, s, l);
              p += l;

              /* If we hit the end of the string, we're done.  */
              if (*e == '\0')
                break;
            }
          else if (l > 1)
            {
              /* It is, and there's >1 backslash.  Take half of them.  */
              l /= 2;
              memmove (p, s, l);
              p += l;
            }

          s = e;
        }

      *(p++) = *(s++);
    }

  *p = '\0';
  return string;
}

/* Search PATTERN for an unquoted % and handle quoting.  */

char *
find_percent (char *pattern)
{
  return find_char_unquote (pattern, '%');
}

/* Search STRING for an unquoted % and handle quoting.  Returns a pointer to
   the % or NULL if no % was found.
   This version is used with strings in the string cache: if there's a need to
   modify the string a new version will be added to the string cache and
   *STRING will be set to that.  */

const char *
find_percent_cached (const char **string)
{
  const char *p = *string;
  char *new = 0;
  size_t slen = 0;

  /* If the first char is a % return now.  This lets us avoid extra tests
     inside the loop.  */
  if (*p == '%')
    return p;

  while (1)
    {
      p = strchr(p, '%');

      if (!p)
        break;

      /* See if this % is escaped with a backslash; if not we're done.  */
      if (p[-1] != '\\')
        break;

      {
        /* Search for more backslashes.  */
        char *pv;
        int i = -2;

        while (&p[i] >= *string && p[i] == '\\')
          --i;
        ++i;

        /* At this point we know we'll need to allocate a new string.
           Make a copy if we haven't yet done so.  */
        if (! new)
          {
            slen = strlen (*string);
            new = alloca (slen + 1);
            memcpy (new, *string, slen + 1);
            p = new + (p - *string);
            *string = new;
          }

        /* At this point *string, p, and new all point into the same string.
           Get a non-const version of p so we can modify new.  */
        pv = new + (p - *string);

        /* The number of backslashes is now -I.
           Copy P over itself to swallow half of them.  */
        memmove (&pv[i], &pv[i/2], (slen - (pv - new)) - (i/2) + 1);
        p += i/2;

        /* If the backslashes quoted each other; the % was unquoted.  */
        if (i % 2 == 0)
          break;
      }
    }

  /* If we had to change STRING, add it to the strcache.  */
  if (new)
    {
      *string = strcache_add (*string);
      if (p)
        p = *string + (p - new);
    }

  /* If we didn't find a %, return NULL.  Otherwise return a ptr to it.  */
  return p;
}

/* Find the next line of text in an eval buffer, combining continuation lines
   into one line.
   Return the number of actual lines read (> 1 if continuation lines).
   Returns -1 if there's nothing left in the buffer.

   After this function, ebuf->buffer points to the first character of the
   line we just found.
 */

/* Read a line of text from a STRING.
   Since we aren't really reading from a file, don't bother with linenumbers.
 */

static long
readstring (struct ebuffer *ebuf)
{
  char *eol;

  /* If there is nothing left in this buffer, return 0.  */
  if (ebuf->bufnext >= ebuf->bufstart + ebuf->size)
    return -1;

  /* Set up a new starting point for the buffer, and find the end of the
     next logical line (taking into account backslash/newline pairs).  */

  eol = ebuf->buffer = ebuf->bufnext;

  while (1)
    {
      int backslash = 0;
      const char *bol = eol;
      const char *p;

      /* Find the next newline.  At EOS, stop.  */
      p = eol = strchr (eol , '\n');
      if (!eol)
        {
          ebuf->bufnext = ebuf->bufstart + ebuf->size + 1;
          return 0;
        }

      /* Found a newline; if it's escaped continue; else we're done.  */
      while (p > bol && *(--p) == '\\')
        backslash = !backslash;
      if (!backslash)
        break;
      ++eol;
    }

  /* Overwrite the newline char.  */
  *eol = '\0';
  ebuf->bufnext = eol+1;

  return 0;
}

static long
readline (struct ebuffer *ebuf)
{
  char *p;
  char *end;
  char *start;
  long nlines = 0;

  /* The behaviors between string and stream buffers are different enough to
     warrant different functions.  Do the Right Thing.  */

  if (!ebuf->fp)
    return readstring (ebuf);

  /* When reading from a file, we always start over at the beginning of the
     buffer for each new line.  */

  p = start = ebuf->bufstart;
  end = p + ebuf->size;
  *p = '\0';

  while (fgets (p, (int) (end - p), ebuf->fp) != 0)
    {
      char *p2;
      size_t len;
      int backslash;

      len = strlen (p);
      if (len == 0)
        {
          /* This only happens when the first thing on the line is a '\0'.
             It is a pretty hopeless case, but (wonder of wonders) Athena
             lossage strikes again!  (xmkmf puts NULs in its makefiles.)
             There is nothing really to be done; we synthesize a newline so
             the following line doesn't appear to be part of this line.  */
          O (error, &ebuf->floc,
             _("warning: NUL character seen; rest of line ignored"));
          p[0] = '\n';
          len = 1;
        }

      /* Jump past the text we just read.  */
      p += len;

      /* If the last char isn't a newline, the whole line didn't fit into the
         buffer.  Get some more buffer and try again.  */
      if (p[-1] != '\n')
        goto more_buffer;

      /* We got a newline, so add one to the count of lines.  */
      ++nlines;

#if !defined(WINDOWS32) && !defined(__MSDOS__) && !defined(__EMX__)
      /* Check to see if the line was really ended with CRLF; if so ignore
         the CR.  */
      if ((p - start) > 1 && p[-2] == '\r')
        {
          --p;
          memmove (p-1, p, strlen (p) + 1);
        }
#endif

      backslash = 0;
      for (p2 = p - 2; p2 >= start; --p2)
        {
          if (*p2 != '\\')
            break;
          backslash = !backslash;
        }

      if (!backslash)
        {
          p[-1] = '\0';
          break;
        }

      /* It was a backslash/newline combo.  If we have more space, read
         another line.  */
      if (end - p >= 80)
        continue;

      /* We need more space at the end of our buffer, so realloc it.
         Make sure to preserve the current offset of p.  */
    more_buffer:
      {
        size_t off = p - start;
        ebuf->size *= 2;
        start = ebuf->buffer = ebuf->bufstart = xrealloc (start, ebuf->size);
        p = start + off;
        end = start + ebuf->size;
        *p = '\0';
      }
    }

  if (ferror (ebuf->fp))
    pfatal_with_name (ebuf->floc.filenm);

  /* If we found some lines, return how many.
     If we didn't, but we did find _something_, that indicates we read the last
     line of a file with no final newline; return 1.
     If we read nothing, we're at EOF; return -1.  */

  return nlines ? nlines : p == ebuf->bufstart ? -1 : 1;
}

/* Parse the next "makefile word" from the input buffer, and return info
   about it.

   A "makefile word" is one of:

     w_bogus        Should never happen
     w_eol          End of input
     w_static       A static word; cannot be expanded
     w_variable     A word containing one or more variables/functions
     w_colon        A colon
     w_dcolon       A double-colon
     w_ampcolon     An ampersand-colon (&:) token
     w_ampdcolon    An ampersand-double-colon (&::) token
     w_semicolon    A semicolon
     w_varassign    A variable assignment operator (=, :=, ::=, +=, ?=, or !=)

   Note that this function is only used when reading certain parts of the
   makefile.  Don't use it where special rules hold sway (RHS of a variable,
   in a command list, etc.)  */

static enum make_word_type
get_next_mword (char *buffer, char **startp, size_t *length)
{
  enum make_word_type wtype;
  char *p = buffer, *beg;
  char c;

  /* Skip any leading whitespace.  */
  while (ISBLANK (*p))
    ++p;

  beg = p;
  c = *(p++);

  /* Look at the start of the word to see if it's simple.  */
  switch (c)
    {
    case '\0':
      wtype = w_eol;
      goto done;

    case ';':
      wtype = w_semicolon;
      goto done;

    case '=':
      wtype = w_varassign;
      goto done;

    case ':':
      if (*p == '=')
        {
          ++p;
          wtype = w_varassign; /* := */
        }
      else if (*p == ':')
        {
          ++p;
          if (p[1] == '=')
            {
              ++p;
              wtype = w_varassign; /* ::= */
            }
          else
            wtype = w_dcolon;
        }
      else
        wtype = w_colon;
      goto done;

    case '&':
      if (*p == ':')
        {
          ++p;
          if (*p != ':')
            wtype = w_ampcolon; /* &: */
          else
            {
              ++p;
              wtype = w_ampdcolon; /* &:: */
            }
          goto done;
        }
      break;

    case '+':
    case '?':
    case '!':
      if (*p == '=')
        {
          ++p;
          wtype = w_varassign; /* += or ?= or != */
          goto done;
        }
      break;

    default:
      break;
    }

  /* This is some non-operator word.  A word consists of the longest
     string of characters that doesn't contain whitespace, one of [:=#],
     or [?+!]=, or &:.  */

  /* We start out assuming a static word; if we see a variable we'll
     adjust our assumptions then.  */
  wtype = w_static;

  /* We already found the first value of "c", above.  */
  while (1)
    {
      char closeparen;
      int count;

      switch (c)
        {
        case '\0':
        case ' ':
        case '\t':
        case '=':
          goto done_word;

        case ':':
#ifdef HAVE_DOS_PATHS
          /* A word CAN include a colon in its drive spec.  The drive
             spec is allowed either at the beginning of a word, or as part
             of the archive member name, like in "libfoo.a(d:/foo/bar.o)".  */
          if ((p - beg == 2 || (p - beg > 2 && p[-3] == '('))
              && isalpha ((unsigned char)p[-2]))
            break;
#endif
          goto done_word;

        case '$':
          c = *(p++);
          if (c == '$')
            break;
          if (c == '\0')
            goto done_word;

          /* This is a variable reference, so note that it's expandable.
             Then read it to the matching close paren.  */
          wtype = w_variable;

          if (c == '(')
            closeparen = ')';
          else if (c == '{')
            closeparen = '}';
          else
            /* This is a single-letter variable reference.  */
            break;

          for (count=0; *p != '\0'; ++p)
            {
              if (*p == c)
                ++count;
              else if (*p == closeparen && --count < 0)
                {
                  ++p;
                  break;
                }
            }
          break;

        case '?':
        case '+':
          if (*p == '=')
            goto done_word;
          break;

        case '\\':
          switch (*p)
            {
            case ':':
            case ';':
            case '=':
            case '\\':
              ++p;
              break;
            }
          break;

        case '&':
          if (*p == ':')
            goto done_word;
          break;

        default:
          break;
        }

      c = *(p++);
    }
 done_word:
  --p;

 done:
  if (startp)
    *startp = beg;
  if (length)
    *length = p - beg;
  return wtype;
}

/* Construct the list of include directories
   from the arguments and the default list.  */

void
construct_include_path (const char **arg_dirs)
{
#ifdef VAXC             /* just don't ask ... */
  stat_t stbuf;
#else
  struct stat stbuf;
#endif
  const char **dirs;
  const char **cpp;
  size_t idx;

  /* Compute the number of pointers we need in the table.  */
  idx = sizeof (default_include_directories) / sizeof (const char *);
  if (arg_dirs)
    for (cpp = arg_dirs; *cpp != 0; ++cpp)
      ++idx;

#ifdef  __MSDOS__
  /* Add one for $DJDIR.  */
  ++idx;
#endif

  dirs = xmalloc (idx * sizeof (const char *));

  idx = 0;
  max_incl_len = 0;

  /* First consider any dirs specified with -I switches.
     Ignore any that don't exist.  Remember the maximum string length.  */

  if (arg_dirs)
    while (*arg_dirs != 0)
      {
        const char *dir = *(arg_dirs++);
        char *expanded = 0;
        int e;

        if (dir[0] == '~')
          {
            expanded = tilde_expand (dir);
            if (expanded != 0)
              dir = expanded;
          }

        EINTRLOOP (e, stat (dir, &stbuf));
        if (e == 0 && S_ISDIR (stbuf.st_mode))
          {
            size_t len = strlen (dir);
            /* If dir name is written with trailing slashes, discard them.  */
            while (len > 1 && dir[len - 1] == '/')
              --len;
            if (len > max_incl_len)
              max_incl_len = len;
            dirs[idx++] = strcache_add_len (dir, len);
          }

        free (expanded);
      }

  /* Now add the standard default dirs at the end.  */

#ifdef  __MSDOS__
  {
    /* The environment variable $DJDIR holds the root of the DJGPP directory
       tree; add ${DJDIR}/include.  */
    struct variable *djdir = lookup_variable ("DJDIR", 5);

    if (djdir)
      {
        size_t len = strlen (djdir->value) + 8;
        char *defdir = alloca (len + 1);

        strcat (strcpy (defdir, djdir->value), "/include");
        dirs[idx++] = strcache_add (defdir);

        if (len > max_incl_len)
          max_incl_len = len;
      }
  }
#endif

  for (cpp = default_include_directories; *cpp != 0; ++cpp)
    {
      int e;

      EINTRLOOP (e, stat (*cpp, &stbuf));
      if (e == 0 && S_ISDIR (stbuf.st_mode))
        {
          size_t len = strlen (*cpp);
          /* If dir name is written with trailing slashes, discard them.  */
          while (len > 1 && (*cpp)[len - 1] == '/')
            --len;
          if (len > max_incl_len)
            max_incl_len = len;
          dirs[idx++] = strcache_add_len (*cpp, len);
        }
    }

  dirs[idx] = 0;

  /* Now add each dir to the .INCLUDE_DIRS variable.  */

  for (cpp = dirs; *cpp != 0; ++cpp)
    do_variable_definition (NILF, ".INCLUDE_DIRS", *cpp,
                            o_default, f_append, 0);

  include_directories = dirs;
}

/* Expand ~ or ~USER at the beginning of NAME.
   Return a newly malloc'd string or 0.  */

char *
tilde_expand (const char *name)
{
#ifndef VMS
  if (name[1] == '/' || name[1] == '\0')
    {
      char *home_dir;
      int is_variable;

      {
        /* Turn off --warn-undefined-variables while we expand HOME.  */
        int save = warn_undefined_variables_flag;
        warn_undefined_variables_flag = 0;

        home_dir = allocated_variable_expand ("$(HOME)");

        warn_undefined_variables_flag = save;
      }

      is_variable = home_dir[0] != '\0';
      if (!is_variable)
        {
          free (home_dir);
          home_dir = getenv ("HOME");
        }
# if !defined(_AMIGA) && !defined(WINDOWS32)
      if (home_dir == 0 || home_dir[0] == '\0')
        {
          char *logname = getlogin ();
          home_dir = 0;
          if (logname != 0)
            {
              struct passwd *p = getpwnam (logname);
              if (p != 0)
                home_dir = p->pw_dir;
            }
        }
# endif /* !AMIGA && !WINDOWS32 */
      if (home_dir != 0)
        {
          char *new = xstrdup (concat (2, home_dir, name + 1));
          if (is_variable)
            free (home_dir);
          return new;
        }
    }
# if !defined(_AMIGA) && !defined(WINDOWS32)
  else
    {
      struct passwd *pwent;
      char *userend = strchr (name + 1, '/');
      if (userend != 0)
        *userend = '\0';
      pwent = getpwnam (name + 1);
      if (pwent != 0)
        {
          if (userend == 0)
            return xstrdup (pwent->pw_dir);
          else
            return xstrdup (concat (3, pwent->pw_dir, "/", userend + 1));
        }
      else if (userend != 0)
        *userend = '/';
    }
# endif /* !AMIGA && !WINDOWS32 */
#endif /* !VMS */
  return 0;
}

/* Parse a string into a sequence of filenames represented as a chain of
   struct nameseq's and return that chain.  Optionally expand the strings via
   glob().

   The string is passed as STRINGP, the address of a string pointer.
   The string pointer is updated to point at the first character
   not parsed, which either is a null char or equals STOPMAP.

   SIZE is how large (in bytes) each element in the new chain should be.
   This is useful if we want them actually to be other structures
   that have room for additional info.

   STOPMAP is a map of characters that tell us to stop parsing.

   PREFIX, if non-null, is added to the beginning of each filename.

   FLAGS allows one or more of the following bitflags to be set:
        PARSEFS_NOSTRIP - Do no strip './'s off the beginning
        PARSEFS_NOAR    - Do not check filenames for archive references
        PARSEFS_NOGLOB  - Do not expand globbing characters
        PARSEFS_EXISTS  - Only return globbed files that actually exist
                          (cannot also set NOGLOB)
        PARSEFS_NOCACHE - Do not add filenames to the strcache (caller frees)
  */

void *
parse_file_seq (char **stringp, size_t size, int stopmap,
                const char *prefix, int flags)
{
  /* tmp points to tmpbuf after the prefix, if any.
     tp is the end of the buffer. */
  static char *tmpbuf = NULL;

  int cachep = NONE_SET (flags, PARSEFS_NOCACHE);

  struct nameseq *new = 0;
  struct nameseq **newp = &new;
#define NEWELT(_n)  do { \
                        const char *__n = (_n); \
                        *newp = xcalloc (size); \
                        (*newp)->name = (cachep ? strcache_add (__n) : xstrdup (__n)); \
                        newp = &(*newp)->next; \
                    } while(0)

  char *p;
  glob_t gl;
  char *tp;
  int findmap = stopmap|MAP_VMSCOMMA|MAP_NUL;

  if (NONE_SET (flags, PARSEFS_ONEWORD))
    findmap |= MAP_BLANK;

  /* Always stop on NUL.  */
  stopmap |= MAP_NUL;

  if (size < sizeof (struct nameseq))
    size = sizeof (struct nameseq);

  if (NONE_SET (flags, PARSEFS_NOGLOB))
    dir_setup_glob (&gl);

  /* Get enough temporary space to construct the largest possible target.  */
  {
    static size_t tmpbuf_len = 0;
    size_t l = strlen (*stringp) + 1;
    if (l > tmpbuf_len)
      {
        tmpbuf = xrealloc (tmpbuf, l);
        tmpbuf_len = l;
      }
  }
  tp = tmpbuf;

  /* Parse STRING.  P will always point to the end of the parsed content.  */
  p = *stringp;
  while (1)
    {
      const char *name;
      const char **nlist = 0;
      char *tildep = 0;
      int globme = 1;
#ifndef NO_ARCHIVES
      char *arname = 0;
      char *memname = 0;
#endif
      char *s;
      size_t nlen;
      int tot, i;

      /* Skip whitespace; at the end of the string or STOPCHAR we're done.  */
      NEXT_TOKEN (p);
      if (STOP_SET (*p, stopmap))
        break;

      /* There are names left, so find the end of the next name.
         Throughout this iteration S points to the start.  */
      s = p;
      p = find_map_unquote (p, findmap);

#ifdef VMS
        /* convert comma separated list to space separated */
      if (p && *p == ',')
        *p =' ';
#endif
#ifdef _AMIGA
      /* If we stopped due to a device name, skip it.  */
      if (p && p != s+1 && p[0] == ':')
        p = find_map_unquote (p+1, findmap);
#endif
#ifdef HAVE_DOS_PATHS
      /* If we stopped due to a drive specifier, skip it.
         Tokens separated by spaces are treated as separate paths since make
         doesn't allow path names with spaces.  */
      if (p && p == s+1 && p[0] == ':'
          && isalpha ((unsigned char)s[0]) && STOP_SET (p[1], MAP_DIRSEP))
        p = find_map_unquote (p+1, findmap);
#endif

      if (!p)
        p = s + strlen (s);

      /* Strip leading "this directory" references.  */
      if (NONE_SET (flags, PARSEFS_NOSTRIP))
#ifdef VMS
        /* Skip leading '[]'s. should only be one set or bug somwhere else */
        if (p - s > 2 && s[0] == '[' && s[1] == ']')
            s += 2;
        /* Skip leading '<>'s. should only be one set or bug somwhere else */
        if (p - s > 2 && s[0] == '<' && s[1] == '>')
            s += 2;
#endif
        /* Skip leading './'s.  */
        while (p - s > 2 && s[0] == '.' && s[1] == '/')
          {
            /* Skip "./" and all following slashes.  */
            s += 2;
            while (*s == '/')
              ++s;
          }

      /* Extract the filename just found, and skip it.
         Set NAME to the string, and NLEN to its length.  */

      if (s == p)
        {
        /* The name was stripped to empty ("./"). */
#if defined(_AMIGA)
          /* PDS-- This cannot be right!! */
          tp[0] = '\0';
          nlen = 0;
#else
          tp[0] = '.';
          tp[1] = '/';
          tp[2] = '\0';
          nlen = 2;
#endif
        }
      else
        {
#ifdef VMS
/* VMS filenames can have a ':' in them but they have to be '\'ed but we need
 *  to remove this '\' before we can use the filename.
 * xstrdup called because S may be read-only string constant.
 */
          char *n = tp;
          while (s < p)
            {
              if (s[0] == '\\' && s[1] == ':')
                ++s;
              *(n++) = *(s++);
            }
          n[0] = '\0';
          nlen = strlen (tp);
#else
          nlen = p - s;
          memcpy (tp, s, nlen);
          tp[nlen] = '\0';
#endif
        }

      /* At this point, TP points to the element and NLEN is its length.  */

#ifndef NO_ARCHIVES
      /* If this is the start of an archive group that isn't complete, set up
         to add the archive prefix for future files.  A file list like:
         "libf.a(x.o y.o z.o)" needs to be expanded as:
         "libf.a(x.o) libf.a(y.o) libf.a(z.o)"

         TP == TMP means we're not already in an archive group.  Ignore
         something starting with '(', as that cannot actually be an
         archive-member reference (and treating it as such results in an empty
         file name, which causes much lossage).  Also if it ends in ")" then
         it's a complete reference so we don't need to treat it specially.

         Finally, note that archive groups must end with ')' as the last
         character, so ensure there's some word ending like that before
         considering this an archive group.  */
      if (NONE_SET (flags, PARSEFS_NOAR)
          && tp == tmpbuf && tp[0] != '(' && tp[nlen-1] != ')')
        {
          char *n = strchr (tp, '(');
          if (n)
            {
              /* This looks like the first element in an open archive group.
                 A valid group MUST have ')' as the last character.  */
              const char *e = p;
              do
                {
                  const char *o = e;
                  NEXT_TOKEN (e);
                  /* Find the end of this word.  We don't want to unquote and
                     we don't care about quoting since we're looking for the
                     last char in the word. */
                  while (! STOP_SET (*e, findmap))
                    ++e;
                  /* If we didn't move, we're done now.  */
                  if (e == o)
                    break;
                  if (e[-1] == ')')
                    {
                      /* Found the end, so this is the first element in an
                         open archive group.  It looks like "lib(mem".
                         Reset TP past the open paren.  */
                      nlen -= (n + 1) - tp;
                      tp = n + 1;

                      /* We can stop looking now.  */
                      break;
                    }
                }
              while (*e != '\0');

              /* If we have just "lib(", part of something like "lib( a b)",
                 go to the next item.  */
              if (! nlen)
                continue;
            }
        }

      /* If we are inside an archive group, make sure it has an end.  */
      if (tp > tmpbuf)
        {
          if (tp[nlen-1] == ')')
            {
              /* This is the natural end; reset TP.  */
              tp = tmpbuf;

              /* This is just ")", something like "lib(a b )": skip it.  */
              if (nlen == 1)
                continue;
            }
          else
            {
              /* Not the end, so add a "fake" end.  */
              tp[nlen++] = ')';
              tp[nlen] = '\0';
            }
        }
#endif

      /* If we're not globbing we're done: add it to the end of the chain.
         Go to the next item in the string.  */
      if (ANY_SET (flags, PARSEFS_NOGLOB))
        {
          NEWELT (concat (2, prefix, tmpbuf));
          continue;
        }

      /* If we get here we know we're doing glob expansion.
         TP is a string in tmpbuf.  NLEN is no longer used.
         We may need to do more work: after this NAME will be set.  */
      name = tmpbuf;

      /* Expand tilde if applicable.  */
      if (tmpbuf[0] == '~')
        {
          tildep = tilde_expand (tmpbuf);
          if (tildep != 0)
            name = tildep;
        }

#ifndef NO_ARCHIVES
      /* If NAME is an archive member reference replace it with the archive
         file name, and save the member name in MEMNAME.  We will glob on the
         archive name and then reattach MEMNAME later.  */
      if (NONE_SET (flags, PARSEFS_NOAR) && ar_name (name))
        {
          ar_parse_name (name, &arname, &memname);
          name = arname;
        }
#endif /* !NO_ARCHIVES */

      /* glob() is expensive: don't call it unless we need to.  */
      if (NONE_SET (flags, PARSEFS_EXISTS) && strpbrk (name, "?*[") == NULL)
        {
          globme = 0;
          tot = 1;
          nlist = &name;
        }
      else
        switch (glob (name, GLOB_ALTDIRFUNC, NULL, &gl))
          {
          case GLOB_NOSPACE:
            out_of_memory ();

          case 0:
            /* Success.  */
            tot = gl.gl_pathc;
            nlist = (const char **)gl.gl_pathv;
            break;

          case GLOB_NOMATCH:
            /* If we want only existing items, skip this one.  */
            if (ANY_SET (flags, PARSEFS_EXISTS))
              {
                tot = 0;
                break;
              }
            /* FALLTHROUGH */

          default:
            /* By default keep this name.  */
            tot = 1;
            nlist = &name;
            break;
          }

      /* For each matched element, add it to the list.  */
      for (i = 0; i < tot; ++i)
#ifndef NO_ARCHIVES
        if (memname != 0)
          {
            /* Try to glob on MEMNAME within the archive.  */
            struct nameseq *found = ar_glob (nlist[i], memname, size);
            if (! found)
              /* No matches.  Use MEMNAME as-is.  */
              NEWELT (concat (5, prefix, nlist[i], "(", memname, ")"));
            else
              {
                /* We got a chain of items.  Attach them.  */
                if (*newp)
                  (*newp)->next = found;
                else
                  *newp = found;

                /* Find and set the new end.  Massage names if necessary.  */
                while (1)
                  {
                    if (! cachep)
                      found->name = xstrdup (concat (2, prefix, name));
                    else if (prefix)
                      found->name = strcache_add (concat (2, prefix, name));

                    if (found->next == 0)
                      break;

                    found = found->next;
                  }
                newp = &found->next;
              }
          }
        else
#endif /* !NO_ARCHIVES */
          NEWELT (concat (2, prefix, nlist[i]));

      if (globme)
        globfree (&gl);

#ifndef NO_ARCHIVES
      free (arname);
#endif

      free (tildep);
    }

  *stringp = p;
  return new;
}
