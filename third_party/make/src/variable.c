/* Internals of variables for GNU Make.
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
#include "commands.h"
#include "variable.h"
#include "rule.h"
#ifdef WINDOWS32
#include "pathstuff.h"
#endif
#include "hash.h"

/* Incremented every time we add or remove a global variable.  */
static unsigned long variable_changenum;

/* Chain of all pattern-specific variables.  */

static struct pattern_var *pattern_vars;

/* Pointer to the last struct in the pack of a specific size, from 1 to 255.*/

static struct pattern_var *last_pattern_vars[256];

/* Create a new pattern-specific variable struct. The new variable is
   inserted into the PATTERN_VARS list in the shortest patterns first
   order to support the shortest stem matching (the variables are
   matched in the reverse order so the ones with the longest pattern
   will be considered first). Variables with the same pattern length
   are inserted in the definition order. */

struct pattern_var *
create_pattern_var (const char *target, const char *suffix)
{
  size_t len = strlen (target);
  struct pattern_var *p = xcalloc (sizeof (struct pattern_var));

  if (pattern_vars != 0)
    {
      if (len < 256 && last_pattern_vars[len] != 0)
        {
          p->next = last_pattern_vars[len]->next;
          last_pattern_vars[len]->next = p;
        }
      else
        {
          /* Find the position where we can insert this variable. */
          struct pattern_var **v;

          for (v = &pattern_vars; ; v = &(*v)->next)
            {
              /* Insert at the end of the pack so that patterns with the
                 same length appear in the order they were defined .*/

              if (*v == 0 || (*v)->len > len)
                {
                  p->next = *v;
                  *v = p;
                  break;
                }
            }
        }
    }
  else
    {
      pattern_vars = p;
      p->next = 0;
    }

  p->target = target;
  p->len = len;
  p->suffix = suffix + 1;

  if (len < 256)
    last_pattern_vars[len] = p;

  return p;
}

/* Look up a target in the pattern-specific variable list.  */

static struct pattern_var *
lookup_pattern_var (struct pattern_var *start, const char *target)
{
  struct pattern_var *p;
  size_t targlen = strlen (target);

  for (p = start ? start->next : pattern_vars; p != 0; p = p->next)
    {
      const char *stem;
      size_t stemlen;

      if (p->len > targlen)
        /* It can't possibly match.  */
        continue;

      /* From the lengths of the filename and the pattern parts,
         find the stem: the part of the filename that matches the %.  */
      stem = target + (p->suffix - p->target - 1);
      stemlen = targlen - p->len + 1;

      /* Compare the text in the pattern before the stem, if any.  */
      if (stem > target && !strneq (p->target, target, stem - target))
        continue;

      /* Compare the text in the pattern after the stem, if any.
         We could test simply using streq, but this way we compare the
         first two characters immediately.  This saves time in the very
         common case where the first character matches because it is a
         period.  */
      if (*p->suffix == stem[stemlen]
          && (*p->suffix == '\0' || streq (&p->suffix[1], &stem[stemlen+1])))
        break;
    }

  return p;
}

/* Hash table of all global variable definitions.  */

static unsigned long
variable_hash_1 (const void *keyv)
{
  struct variable const *key = (struct variable const *) keyv;
  return_STRING_N_HASH_1 (key->name, key->length);
}

static unsigned long
variable_hash_2 (const void *keyv)
{
  struct variable const *key = (struct variable const *) keyv;
  return_STRING_N_HASH_2 (key->name, key->length);
}

static int
variable_hash_cmp (const void *xv, const void *yv)
{
  struct variable const *x = (struct variable const *) xv;
  struct variable const *y = (struct variable const *) yv;
  int result = x->length - y->length;
  if (result)
    return result;
  return_STRING_N_COMPARE (x->name, y->name, x->length);
}

#ifndef VARIABLE_BUCKETS
#define VARIABLE_BUCKETS                523
#endif
#ifndef PERFILE_VARIABLE_BUCKETS
#define PERFILE_VARIABLE_BUCKETS        23
#endif
#ifndef SMALL_SCOPE_VARIABLE_BUCKETS
#define SMALL_SCOPE_VARIABLE_BUCKETS    13
#endif

static struct variable_set global_variable_set;
static struct variable_set_list global_setlist
  = { 0, &global_variable_set, 0 };
struct variable_set_list *current_variable_set_list = &global_setlist;

/* Implement variables.  */

void
init_hash_global_variable_set (void)
{
  hash_init (&global_variable_set.table, VARIABLE_BUCKETS,
             variable_hash_1, variable_hash_2, variable_hash_cmp);
}

/* Define variable named NAME with value VALUE in SET.  VALUE is copied.
   LENGTH is the length of NAME, which does not need to be null-terminated.
   ORIGIN specifies the origin of the variable (makefile, command line
   or environment).
   If RECURSIVE is nonzero a flag is set in the variable saying
   that it should be recursively re-expanded.  */

struct variable *
define_variable_in_set (const char *name, size_t length,
                        const char *value, enum variable_origin origin,
                        int recursive, struct variable_set *set,
                        const floc *flocp)
{
  struct variable *v;
  struct variable **var_slot;
  struct variable var_key;

  if (set == NULL)
    set = &global_variable_set;

  var_key.name = (char *) name;
  var_key.length = (unsigned int) length;
  var_slot = (struct variable **) hash_find_slot (&set->table, &var_key);
  v = *var_slot;

#ifdef VMS
  /* VMS does not populate envp[] with DCL symbols and logical names which
     historically are mapped to environent variables.
     If the variable is not yet defined, then we need to check if getenv()
     can find it.  Do not do this for origin == o_env to avoid infinte
     recursion */
  if (HASH_VACANT (v) && (origin != o_env))
    {
      struct variable * vms_variable;
      char * vname = alloca (length + 1);
      char * vvalue;

      strncpy (vname, name, length);
      vvalue = getenv(vname);

      /* Values starting with '$' are probably foreign commands.
         We want to treat them as Shell aliases and not look them up here */
      if ((vvalue != NULL) && (vvalue[0] != '$'))
        {
          vms_variable =  lookup_variable(name, length);
          /* Refresh the slot */
          var_slot = (struct variable **) hash_find_slot (&set->table,
                                                          &var_key);
          v = *var_slot;
        }
    }
#endif

  if (env_overrides && origin == o_env)
    origin = o_env_override;

  if (! HASH_VACANT (v))
    {
      if (env_overrides && v->origin == o_env)
        /* V came from in the environment.  Since it was defined
           before the switches were parsed, it wasn't affected by -e.  */
        v->origin = o_env_override;

      /* A variable of this name is already defined.
         If the old definition is from a stronger source
         than this one, don't redefine it.  */
      if ((int) origin >= (int) v->origin)
        {
          free (v->value);
          v->value = xstrdup (value);
          if (flocp != 0)
            v->fileinfo = *flocp;
          else
            v->fileinfo.filenm = 0;
          v->origin = origin;
          v->recursive = recursive;
        }
      return v;
    }

  /* Create a new variable definition and add it to the hash table.  */

  v = xcalloc (sizeof (struct variable));
  v->name = xstrndup (name, length);
  v->length = (unsigned int) length;
  hash_insert_at (&set->table, v, var_slot);
  if (set == &global_variable_set)
    ++variable_changenum;

  v->value = xstrdup (value);
  if (flocp != 0)
    v->fileinfo = *flocp;
  v->origin = origin;
  v->recursive = recursive;

  v->export = v_default;
  v->exportable = 1;
  if (*name != '_' && (*name < 'A' || *name > 'Z')
      && (*name < 'a' || *name > 'z'))
    v->exportable = 0;
  else
    {
      for (++name; *name != '\0'; ++name)
        if (*name != '_' && (*name < 'a' || *name > 'z')
            && (*name < 'A' || *name > 'Z') && !ISDIGIT(*name))
          break;

      if (*name != '\0')
        v->exportable = 0;
    }

  return v;
}


/* Undefine variable named NAME in SET. LENGTH is the length of NAME, which
   does not need to be null-terminated. ORIGIN specifies the origin of the
   variable (makefile, command line or environment). */

static void
free_variable_name_and_value (const void *item)
{
  struct variable *v = (struct variable *) item;
  free (v->name);
  free (v->value);
}

void
free_variable_set (struct variable_set_list *list)
{
  hash_map (&list->set->table, free_variable_name_and_value);
  hash_free (&list->set->table, 1);
  free (list->set);
  free (list);
}

void
undefine_variable_in_set (const char *name, size_t length,
                          enum variable_origin origin,
                          struct variable_set *set)
{
  struct variable *v;
  struct variable **var_slot;
  struct variable var_key;

  if (set == NULL)
    set = &global_variable_set;

  var_key.name = (char *) name;
  var_key.length = (unsigned int) length;
  var_slot = (struct variable **) hash_find_slot (&set->table, &var_key);

  if (env_overrides && origin == o_env)
    origin = o_env_override;

  v = *var_slot;
  if (! HASH_VACANT (v))
    {
      if (env_overrides && v->origin == o_env)
        /* V came from in the environment.  Since it was defined
           before the switches were parsed, it wasn't affected by -e.  */
        v->origin = o_env_override;

      /* Undefine only if this undefinition is from an equal or stronger
         source than the variable definition.  */
      if ((int) origin >= (int) v->origin)
        {
          hash_delete_at (&set->table, var_slot);
          free_variable_name_and_value (v);
          free (v);
          if (set == &global_variable_set)
            ++variable_changenum;
        }
    }
}

/* If the variable passed in is "special", handle its special nature.
   Currently there are two such variables, both used for introspection:
   .VARIABLES expands to a list of all the variables defined in this instance
   of make.
   .TARGETS expands to a list of all the targets defined in this
   instance of make.
   Returns the variable reference passed in.  */

#define EXPANSION_INCREMENT(_l)  ((((_l) / 500) + 1) * 500)

static struct variable *
lookup_special_var (struct variable *var)
{
  static unsigned long last_changenum = 0;


  /* This one actually turns out to be very hard, due to the way the parser
     records targets.  The way it works is that target information is collected
     internally until make knows the target is completely specified.  It unitl
     it sees that some new construct (a new target or variable) is defined that
     it knows the previous one is done.  In short, this means that if you do
     this:

       all:

       TARGS := $(.TARGETS)

     then $(TARGS) won't contain "all", because it's not until after the
     variable is created that the previous target is completed.

     Changing this would be a major pain.  I think a less complex way to do it
     would be to pre-define the target files as soon as the first line is
     parsed, then come back and do the rest of the definition as now.  That
     would allow $(.TARGETS) to be correct without a major change to the way
     the parser works.

  if (streq (var->name, ".TARGETS"))
    var->value = build_target_list (var->value);
  else
  */

  if (variable_changenum != last_changenum && streq (var->name, ".VARIABLES"))
    {
      size_t max = EXPANSION_INCREMENT (strlen (var->value));
      size_t len;
      char *p;
      struct variable **vp = (struct variable **) global_variable_set.table.ht_vec;
      struct variable **end = &vp[global_variable_set.table.ht_size];

      /* Make sure we have at least MAX bytes in the allocated buffer.  */
      var->value = xrealloc (var->value, max);

      /* Walk through the hash of variables, constructing a list of names.  */
      p = var->value;
      len = 0;
      for (; vp < end; ++vp)
        if (!HASH_VACANT (*vp))
          {
            struct variable *v = *vp;
            int l = v->length;

            len += l + 1;
            if (len > max)
              {
                size_t off = p - var->value;

                max += EXPANSION_INCREMENT (l + 1);
                var->value = xrealloc (var->value, max);
                p = &var->value[off];
              }

            memcpy (p, v->name, l);
            p += l;
            *(p++) = ' ';
          }
      *(p-1) = '\0';

      /* Remember the current variable change number.  */
      last_changenum = variable_changenum;
    }

  return var;
}


/* Lookup a variable whose name is a string starting at NAME
   and with LENGTH chars.  NAME need not be null-terminated.
   Returns address of the 'struct variable' containing all info
   on the variable, or nil if no such variable is defined.  */

struct variable *
lookup_variable (const char *name, size_t length)
{
  const struct variable_set_list *setlist;
  struct variable var_key;
  int is_parent = 0;

  var_key.name = (char *) name;
  var_key.length = (unsigned int) length;

  for (setlist = current_variable_set_list;
       setlist != 0; setlist = setlist->next)
    {
      const struct variable_set *set = setlist->set;
      struct variable *v;

      v = (struct variable *) hash_find_item ((struct hash_table *) &set->table, &var_key);
      if (v && (!is_parent || !v->private_var))
        return v->special ? lookup_special_var (v) : v;

      is_parent |= setlist->next_is_parent;
    }

#ifdef VMS
  /* VMS does not populate envp[] with DCL symbols and logical names which
     historically are mapped to enviroment varables and returned by getenv() */
  {
    char *vname = alloca (length + 1);
    char *value;
    strncpy (vname, name, length);
    vname[length] = 0;
    value = getenv (vname);
    if (value != 0)
      {
        char *sptr;
        int scnt;

        sptr = value;
        scnt = 0;

        while ((sptr = strchr (sptr, '$')))
          {
            scnt++;
            sptr++;
          }

        if (scnt > 0)
          {
            char *nvalue;
            char *nptr;

            nvalue = alloca (strlen (value) + scnt + 1);
            sptr = value;
            nptr = nvalue;

            while (*sptr)
              {
                if (*sptr == '$')
                  {
                    *nptr++ = '$';
                    *nptr++ = '$';
                  }
                else
                  {
                    *nptr++ = *sptr;
                  }
                sptr++;
              }

            *nptr = '\0';
            return define_variable (vname, length, nvalue, o_env, 1);

          }

        return define_variable (vname, length, value, o_env, 1);
      }
  }
#endif /* VMS */

  return 0;
}

/* Lookup a variable whose name is a string starting at NAME
   and with LENGTH chars in set SET.  NAME need not be null-terminated.
   Returns address of the 'struct variable' containing all info
   on the variable, or nil if no such variable is defined.  */

struct variable *
lookup_variable_in_set (const char *name, size_t length,
                        const struct variable_set *set)
{
  struct variable var_key;

  var_key.name = (char *) name;
  var_key.length = (unsigned int) length;

  return (struct variable *) hash_find_item ((struct hash_table *) &set->table, &var_key);
}

/* Initialize FILE's variable set list.  If FILE already has a variable set
   list, the topmost variable set is left intact, but the the rest of the
   chain is replaced with FILE->parent's setlist.  If FILE is a double-colon
   rule, then we will use the "root" double-colon target's variable set as the
   parent of FILE's variable set.

   If we're READING a makefile, don't do the pattern variable search now,
   since the pattern variable might not have been defined yet.  */

void
initialize_file_variables (struct file *file, int reading)
{
  struct variable_set_list *l = file->variables;

  if (l == 0)
    {
      l = (struct variable_set_list *)
        xmalloc (sizeof (struct variable_set_list));
      l->set = xmalloc (sizeof (struct variable_set));
      hash_init (&l->set->table, PERFILE_VARIABLE_BUCKETS,
                 variable_hash_1, variable_hash_2, variable_hash_cmp);
      file->variables = l;
    }

  /* If this is a double-colon, then our "parent" is the "root" target for
     this double-colon rule.  Since that rule has the same name, parent,
     etc. we can just use its variables as the "next" for ours.  */

  if (file->double_colon && file->double_colon != file)
    {
      initialize_file_variables (file->double_colon, reading);
      l->next = file->double_colon->variables;
      l->next_is_parent = 0;
      return;
    }

  if (file->parent == 0)
    l->next = &global_setlist;
  else
    {
      initialize_file_variables (file->parent, reading);
      l->next = file->parent->variables;
    }
  l->next_is_parent = 1;

  /* If we're not reading makefiles and we haven't looked yet, see if
     we can find pattern variables for this target.  */

  if (!reading && !file->pat_searched)
    {
      struct pattern_var *p;

      p = lookup_pattern_var (0, file->name);
      if (p != 0)
        {
          struct variable_set_list *global = current_variable_set_list;

          /* We found at least one.  Set up a new variable set to accumulate
             all the pattern variables that match this target.  */

          file->pat_variables = create_new_variable_set ();
          current_variable_set_list = file->pat_variables;

          do
            {
              /* We found one, so insert it into the set.  */

              struct variable *v;

              if (p->variable.flavor == f_simple)
                {
                  v = define_variable_loc (
                    p->variable.name, strlen (p->variable.name),
                    p->variable.value, p->variable.origin,
                    0, &p->variable.fileinfo);

                  v->flavor = f_simple;
                }
              else
                {
                  v = do_variable_definition (
                    &p->variable.fileinfo, p->variable.name,
                    p->variable.value, p->variable.origin,
                    p->variable.flavor, 1);
                }

              /* Also mark it as a per-target and copy export status. */
              v->per_target = p->variable.per_target;
              v->export = p->variable.export;
              v->private_var = p->variable.private_var;
            }
          while ((p = lookup_pattern_var (p, file->name)) != 0);

          current_variable_set_list = global;
        }
      file->pat_searched = 1;
    }

  /* If we have a pattern variable match, set it up.  */

  if (file->pat_variables != 0)
    {
      file->pat_variables->next = l->next;
      file->pat_variables->next_is_parent = l->next_is_parent;
      l->next = file->pat_variables;
      l->next_is_parent = 0;
    }
}

/* Pop the top set off the current variable set list,
   and free all its storage.  */

struct variable_set_list *
create_new_variable_set (void)
{
  struct variable_set_list *setlist;
  struct variable_set *set;

  set = xmalloc (sizeof (struct variable_set));
  hash_init (&set->table, SMALL_SCOPE_VARIABLE_BUCKETS,
             variable_hash_1, variable_hash_2, variable_hash_cmp);

  setlist = (struct variable_set_list *)
    xmalloc (sizeof (struct variable_set_list));
  setlist->set = set;
  setlist->next = current_variable_set_list;
  setlist->next_is_parent = 0;

  return setlist;
}

/* Create a new variable set and push it on the current setlist.
   If we're pushing a global scope (that is, the current scope is the global
   scope) then we need to "push" it the other way: file variable sets point
   directly to the global_setlist so we need to replace that with the new one.
 */

struct variable_set_list *
push_new_variable_scope (void)
{
  current_variable_set_list = create_new_variable_set ();
  if (current_variable_set_list->next == &global_setlist)
    {
      /* It was the global, so instead of new -> &global we want to replace
         &global with the new one and have &global -> new, with current still
         pointing to &global  */
      struct variable_set *set = current_variable_set_list->set;
      current_variable_set_list->set = global_setlist.set;
      global_setlist.set = set;
      current_variable_set_list->next = global_setlist.next;
      global_setlist.next = current_variable_set_list;
      current_variable_set_list = &global_setlist;
    }
  return (current_variable_set_list);
}

void
pop_variable_scope (void)
{
  struct variable_set_list *setlist;
  struct variable_set *set;

  /* Can't call this if there's no scope to pop!  */
  assert (current_variable_set_list->next != NULL);

  if (current_variable_set_list != &global_setlist)
    {
      /* We're not pointing to the global setlist, so pop this one.  */
      setlist = current_variable_set_list;
      set = setlist->set;
      current_variable_set_list = setlist->next;
    }
  else
    {
      /* This set is the one in the global_setlist, but there is another global
         set beyond that.  We want to copy that set to global_setlist, then
         delete what used to be in global_setlist.  */
      setlist = global_setlist.next;
      set = global_setlist.set;
      global_setlist.set = setlist->set;
      global_setlist.next = setlist->next;
      global_setlist.next_is_parent = setlist->next_is_parent;
    }

  /* Free the one we no longer need.  */
  free (setlist);
  hash_map (&set->table, free_variable_name_and_value);
  hash_free (&set->table, 1);
  free (set);
}

/* Merge FROM_SET into TO_SET, freeing unused storage in FROM_SET.  */

static void
merge_variable_sets (struct variable_set *to_set,
                     struct variable_set *from_set)
{
  struct variable **from_var_slot = (struct variable **) from_set->table.ht_vec;
  struct variable **from_var_end = from_var_slot + from_set->table.ht_size;

  int inc = to_set == &global_variable_set ? 1 : 0;

  for ( ; from_var_slot < from_var_end; from_var_slot++)
    if (! HASH_VACANT (*from_var_slot))
      {
        struct variable *from_var = *from_var_slot;
        struct variable **to_var_slot
          = (struct variable **) hash_find_slot (&to_set->table, *from_var_slot);
        if (HASH_VACANT (*to_var_slot))
          {
            hash_insert_at (&to_set->table, from_var, to_var_slot);
            variable_changenum += inc;
          }
        else
          {
            /* GKM FIXME: delete in from_set->table */
            free (from_var->value);
            free (from_var);
          }
      }
}

/* Merge SETLIST1 into SETLIST0, freeing unused storage in SETLIST1.  */

void
merge_variable_set_lists (struct variable_set_list **setlist0,
                          struct variable_set_list *setlist1)
{
  struct variable_set_list *to = *setlist0;
  struct variable_set_list *last0 = 0;

  /* If there's nothing to merge, stop now.  */
  if (!setlist1 || setlist1 == &global_setlist)
    return;

  if (to)
    {
      /* These loops rely on the fact that all setlists terminate with the
         global setlist (before NULL).  If not, arguably we SHOULD die.  */

      /* Make sure that setlist1 is not already a subset of setlist0.  */
      while (to != &global_setlist)
        {
          if (to == setlist1)
            return;
          to = to->next;
        }

      to = *setlist0;
      while (setlist1 != &global_setlist && to != &global_setlist)
        {
          struct variable_set_list *from = setlist1;
          setlist1 = setlist1->next;

          merge_variable_sets (to->set, from->set);

          last0 = to;
          to = to->next;
        }
    }

  if (setlist1 != &global_setlist)
    {
      if (last0 == 0)
        *setlist0 = setlist1;
      else
        last0->next = setlist1;
    }
}

/* Define the automatic variables, and record the addresses
   of their structures so we can change their values quickly.  */

void
define_automatic_variables (void)
{
  struct variable *v;
  char buf[200];

  sprintf (buf, "%u", makelevel);
  define_variable_cname (MAKELEVEL_NAME, buf, o_env, 0);

  sprintf (buf, "%s%s%s",
           version_string,
           (remote_description == 0 || remote_description[0] == '\0')
           ? "" : "-",
           (remote_description == 0 || remote_description[0] == '\0')
           ? "" : remote_description);
  define_variable_cname ("MAKE_VERSION", buf, o_default, 0);
  define_variable_cname ("MAKE_HOST", make_host, o_default, 0);

#ifdef  __MSDOS__
  /* Allow to specify a special shell just for Make,
     and use $COMSPEC as the default $SHELL when appropriate.  */
  {
    static char shell_str[] = "SHELL";
    const int shlen = sizeof (shell_str) - 1;
    struct variable *mshp = lookup_variable ("MAKESHELL", 9);
    struct variable *comp = lookup_variable ("COMSPEC", 7);

    /* $(MAKESHELL) overrides $(SHELL) even if -e is in effect.  */
    if (mshp)
      (void) define_variable (shell_str, shlen,
                              mshp->value, o_env_override, 0);
    else if (comp)
      {
        /* $(COMSPEC) shouldn't override $(SHELL).  */
        struct variable *shp = lookup_variable (shell_str, shlen);

        if (!shp)
          (void) define_variable (shell_str, shlen, comp->value, o_env, 0);
      }
  }
#elif defined(__EMX__)
  {
    static char shell_str[] = "SHELL";
    const int shlen = sizeof (shell_str) - 1;
    struct variable *shell = lookup_variable (shell_str, shlen);
    struct variable *replace = lookup_variable ("MAKESHELL", 9);

    /* if $MAKESHELL is defined in the environment assume o_env_override */
    if (replace && *replace->value && replace->origin == o_env)
      replace->origin = o_env_override;

    /* if $MAKESHELL is not defined use $SHELL but only if the variable
       did not come from the environment */
    if (!replace || !*replace->value)
      if (shell && *shell->value && (shell->origin == o_env
          || shell->origin == o_env_override))
        {
          /* overwrite whatever we got from the environment */
          free (shell->value);
          shell->value = xstrdup (default_shell);
          shell->origin = o_default;
        }

    /* Some people do not like cmd to be used as the default
       if $SHELL is not defined in the Makefile.
       With -DNO_CMD_DEFAULT you can turn off this behaviour */
# ifndef NO_CMD_DEFAULT
    /* otherwise use $COMSPEC */
    if (!replace || !*replace->value)
      replace = lookup_variable ("COMSPEC", 7);

    /* otherwise use $OS2_SHELL */
    if (!replace || !*replace->value)
      replace = lookup_variable ("OS2_SHELL", 9);
# else
#   warning NO_CMD_DEFAULT: GNU make will not use CMD.EXE as default shell
# endif

    if (replace && *replace->value)
      /* overwrite $SHELL */
      (void) define_variable (shell_str, shlen, replace->value,
                              replace->origin, 0);
    else
      /* provide a definition if there is none */
      (void) define_variable (shell_str, shlen, default_shell,
                              o_default, 0);
  }

#endif

  /* This won't override any definition, but it will provide one if there
     isn't one there.  */
  v = define_variable_cname ("SHELL", default_shell, o_default, 0);
#ifdef __MSDOS__
  v->export = v_export;  /*  Export always SHELL.  */
#endif

  /* On MSDOS we do use SHELL from environment, since it isn't a standard
     environment variable on MSDOS, so whoever sets it, does that on purpose.
     On OS/2 we do not use SHELL from environment but we have already handled
     that problem above. */
#if !defined(__MSDOS__) && !defined(__EMX__)
  /* Don't let SHELL come from the environment.  */
  if (*v->value == '\0' || v->origin == o_env || v->origin == o_env_override)
    {
      free (v->value);
      v->origin = o_file;
      v->value = xstrdup (default_shell);
    }
#endif

  /* Make sure MAKEFILES gets exported if it is set.  */
  v = define_variable_cname ("MAKEFILES", "", o_default, 0);
  v->export = v_ifset;

  /* Define the magic D and F variables in terms of
     the automatic variables they are variations of.  */

#if defined(__MSDOS__) || defined(WINDOWS32)
  /* For consistency, remove the trailing backslash as well as slash.  */
  define_variable_cname ("@D", "$(patsubst %/,%,$(patsubst %\\,%,$(dir $@)))",
                         o_automatic, 1);
  define_variable_cname ("%D", "$(patsubst %/,%,$(patsubst %\\,%,$(dir $%)))",
                         o_automatic, 1);
  define_variable_cname ("*D", "$(patsubst %/,%,$(patsubst %\\,%,$(dir $*)))",
                         o_automatic, 1);
  define_variable_cname ("<D", "$(patsubst %/,%,$(patsubst %\\,%,$(dir $<)))",
                         o_automatic, 1);
  define_variable_cname ("?D", "$(patsubst %/,%,$(patsubst %\\,%,$(dir $?)))",
                         o_automatic, 1);
  define_variable_cname ("^D", "$(patsubst %/,%,$(patsubst %\\,%,$(dir $^)))",
                         o_automatic, 1);
  define_variable_cname ("+D", "$(patsubst %/,%,$(patsubst %\\,%,$(dir $+)))",
                         o_automatic, 1);
#else  /* not __MSDOS__, not WINDOWS32 */
  define_variable_cname ("@D", "$(patsubst %/,%,$(dir $@))", o_automatic, 1);
  define_variable_cname ("%D", "$(patsubst %/,%,$(dir $%))", o_automatic, 1);
  define_variable_cname ("*D", "$(patsubst %/,%,$(dir $*))", o_automatic, 1);
  define_variable_cname ("<D", "$(patsubst %/,%,$(dir $<))", o_automatic, 1);
  define_variable_cname ("?D", "$(patsubst %/,%,$(dir $?))", o_automatic, 1);
  define_variable_cname ("^D", "$(patsubst %/,%,$(dir $^))", o_automatic, 1);
  define_variable_cname ("+D", "$(patsubst %/,%,$(dir $+))", o_automatic, 1);
#endif
  define_variable_cname ("@F", "$(notdir $@)", o_automatic, 1);
  define_variable_cname ("%F", "$(notdir $%)", o_automatic, 1);
  define_variable_cname ("*F", "$(notdir $*)", o_automatic, 1);
  define_variable_cname ("<F", "$(notdir $<)", o_automatic, 1);
  define_variable_cname ("?F", "$(notdir $?)", o_automatic, 1);
  define_variable_cname ("^F", "$(notdir $^)", o_automatic, 1);
  define_variable_cname ("+F", "$(notdir $+)", o_automatic, 1);
}

int export_all_variables;

/* Create a new environment for FILE's commands.
   If FILE is nil, this is for the 'shell' function.
   The child's MAKELEVEL variable is incremented.  */

char **
target_environment (struct file *file)
{
  struct variable_set_list *set_list;
  struct variable_set_list *s;
  struct hash_table table;
  struct variable **v_slot;
  struct variable **v_end;
  struct variable makelevel_key;
  char **result_0;
  char **result;

  if (file == 0)
    set_list = current_variable_set_list;
  else
    set_list = file->variables;

  hash_init (&table, VARIABLE_BUCKETS,
             variable_hash_1, variable_hash_2, variable_hash_cmp);

  /* Run through all the variable sets in the list,
     accumulating variables in TABLE.  */
  for (s = set_list; s != 0; s = s->next)
    {
      struct variable_set *set = s->set;
      v_slot = (struct variable **) set->table.ht_vec;
      v_end = v_slot + set->table.ht_size;
      for ( ; v_slot < v_end; v_slot++)
        if (! HASH_VACANT (*v_slot))
          {
            struct variable **new_slot;
            struct variable *v = *v_slot;

            /* If this is a per-target variable and it hasn't been touched
               already then look up the global version and take its export
               value.  */
            if (v->per_target && v->export == v_default)
              {
                struct variable *gv;

                gv = lookup_variable_in_set (v->name, strlen (v->name),
                                             &global_variable_set);
                if (gv)
                  v->export = gv->export;
              }

            switch (v->export)
              {
              case v_default:
                if (v->origin == o_default || v->origin == o_automatic)
                  /* Only export default variables by explicit request.  */
                  continue;

                /* The variable doesn't have a name that can be exported.  */
                if (! v->exportable)
                  continue;

                if (! export_all_variables
                    && v->origin != o_command
                    && v->origin != o_env && v->origin != o_env_override)
                  continue;
                break;

              case v_export:
                break;

              case v_noexport:
                {
                  /* If this is the SHELL variable and it's not exported,
                     then add the value from our original environment, if
                     the original environment defined a value for SHELL.  */
                  if (streq (v->name, "SHELL") && shell_var.value)
                    {
                      v = &shell_var;
                      break;
                    }
                  continue;
                }

              case v_ifset:
                if (v->origin == o_default)
                  continue;
                break;
              }

            new_slot = (struct variable **) hash_find_slot (&table, v);
            if (HASH_VACANT (*new_slot))
              hash_insert_at (&table, v, new_slot);
          }
    }

  makelevel_key.name = (char *)MAKELEVEL_NAME;
  makelevel_key.length = MAKELEVEL_LENGTH;
  hash_delete (&table, &makelevel_key);

  result = result_0 = xmalloc ((table.ht_fill + 2) * sizeof (char *));

  v_slot = (struct variable **) table.ht_vec;
  v_end = v_slot + table.ht_size;
  for ( ; v_slot < v_end; v_slot++)
    if (! HASH_VACANT (*v_slot))
      {
        struct variable *v = *v_slot;

        /* If V is recursively expanded and didn't come from the environment,
           expand its value.  If it came from the environment, it should
           go back into the environment unchanged.  */
        if (v->recursive
            && v->origin != o_env && v->origin != o_env_override)
          {
            char *value = recursively_expand_for_file (v, file);
#ifdef WINDOWS32
            if (strcmp (v->name, "Path") == 0 ||
                strcmp (v->name, "PATH") == 0)
              convert_Path_to_windows32 (value, ';');
#endif
            *result++ = xstrdup (concat (3, v->name, "=", value));
            free (value);
          }
        else
          {
#ifdef WINDOWS32
            if (strcmp (v->name, "Path") == 0 ||
                strcmp (v->name, "PATH") == 0)
              convert_Path_to_windows32 (v->value, ';');
#endif
            *result++ = xstrdup (concat (3, v->name, "=", v->value));
          }
      }

  *result = xmalloc (100);
  sprintf (*result, "%s=%u", MAKELEVEL_NAME, makelevel + 1);
  *++result = 0;

  hash_free (&table, 0);

  return result_0;
}

static struct variable *
set_special_var (struct variable *var)
{
  if (streq (var->name, RECIPEPREFIX_NAME))
    {
      /* The user is resetting the command introduction prefix.  This has to
         happen immediately, so that subsequent rules are interpreted
         properly.  */
      cmd_prefix = var->value[0]=='\0' ? RECIPEPREFIX_DEFAULT : var->value[0];
    }

  return var;
}

/* Given a string, shell-execute it and return a malloc'ed string of the
 * result. This removes only ONE newline (if any) at the end, for maximum
 * compatibility with the *BSD makes.  If it fails, returns NULL. */

static char *
shell_result (const char *p)
{
  char *buf;
  size_t len;
  char *args[2];
  char *result;

  install_variable_buffer (&buf, &len);

  args[0] = (char *) p;
  args[1] = NULL;
  variable_buffer_output (func_shell_base (variable_buffer, args, 0), "\0", 1);
  result = strdup (variable_buffer);

  restore_variable_buffer (buf, len);
  return result;
}

/* Given a variable, a value, and a flavor, define the variable.
   See the try_variable_definition() function for details on the parameters. */

struct variable *
do_variable_definition (const floc *flocp, const char *varname,
                        const char *value, enum variable_origin origin,
                        enum variable_flavor flavor, int target_var)
{
  const char *p;
  char *alloc_value = NULL;
  struct variable *v;
  int append = 0;
  int conditional = 0;

  /* Calculate the variable's new value in VALUE.  */

  switch (flavor)
    {
    default:
    case f_bogus:
      /* Should not be possible.  */
      abort ();
    case f_simple:
      /* A simple variable definition "var := value".  Expand the value.
         We have to allocate memory since otherwise it'll clobber the
         variable buffer, and we may still need that if we're looking at a
         target-specific variable.  */
      p = alloc_value = allocated_variable_expand (value);
      break;
    case f_shell:
      {
        /* A shell definition "var != value".  Expand value, pass it to
           the shell, and store the result in recursively-expanded var. */
        char *q = allocated_variable_expand (value);
        p = alloc_value = shell_result (q);
        free (q);
        flavor = f_recursive;
        break;
      }
    case f_conditional:
      /* A conditional variable definition "var ?= value".
         The value is set IFF the variable is not defined yet. */
      v = lookup_variable (varname, strlen (varname));
      if (v)
        goto done;

      conditional = 1;
      flavor = f_recursive;
      /* FALLTHROUGH */
    case f_recursive:
      /* A recursive variable definition "var = value".
         The value is used verbatim.  */
      p = value;
      break;
    case f_append:
    case f_append_value:
      {
        /* If we have += but we're in a target variable context, we want to
           append only with other variables in the context of this target.  */
        if (target_var)
          {
            append = 1;
            v = lookup_variable_in_set (varname, strlen (varname),
                                        current_variable_set_list->set);

            /* Don't append from the global set if a previous non-appending
               target-specific variable definition exists. */
            if (v && !v->append)
              append = 0;
          }
        else
          v = lookup_variable (varname, strlen (varname));

        if (v == 0)
          {
            /* There was no old value.
               This becomes a normal recursive definition.  */
            p = value;
            flavor = f_recursive;
          }
        else
          {
            /* Paste the old and new values together in VALUE.  */

            size_t oldlen, vallen;
            const char *val;
            char *tp = NULL;

            val = value;
            if (v->recursive)
              /* The previous definition of the variable was recursive.
                 The new value is the unexpanded old and new values.  */
              flavor = f_recursive;
            else if (flavor != f_append_value)
              /* The previous definition of the variable was simple.
                 The new value comes from the old value, which was expanded
                 when it was set; and from the expanded new value.  Allocate
                 memory for the expansion as we may still need the rest of the
                 buffer if we're looking at a target-specific variable.  */
              val = tp = allocated_variable_expand (val);

            /* If the new value is empty, nothing to do.  */
            vallen = strlen (val);
            if (!vallen)
              {
                alloc_value = tp;
                goto done;
              }

            oldlen = strlen (v->value);
            p = alloc_value = xmalloc (oldlen + 1 + vallen + 1);

            if (oldlen)
              {
                memcpy (alloc_value, v->value, oldlen);
                alloc_value[oldlen] = ' ';
                ++oldlen;
              }

            memcpy (&alloc_value[oldlen], val, vallen + 1);

            free (tp);
          }
        break;
      }
    }

#ifdef __MSDOS__
  /* Many Unix Makefiles include a line saying "SHELL=/bin/sh", but
     non-Unix systems don't conform to this default configuration (in
     fact, most of them don't even have '/bin').  On the other hand,
     $SHELL in the environment, if set, points to the real pathname of
     the shell.
     Therefore, we generally won't let lines like "SHELL=/bin/sh" from
     the Makefile override $SHELL from the environment.  But first, we
     look for the basename of the shell in the directory where SHELL=
     points, and along the $PATH; if it is found in any of these places,
     we define $SHELL to be the actual pathname of the shell.  Thus, if
     you have bash.exe installed as d:/unix/bash.exe, and d:/unix is on
     your $PATH, then SHELL=/usr/local/bin/bash will have the effect of
     defining SHELL to be "d:/unix/bash.exe".  */
  if ((origin == o_file || origin == o_override)
      && strcmp (varname, "SHELL") == 0)
    {
      PATH_VAR (shellpath);
      extern char * __dosexec_find_on_path (const char *, char *[], char *);

      /* See if we can find "/bin/sh.exe", "/bin/sh.com", etc.  */
      if (__dosexec_find_on_path (p, NULL, shellpath))
        {
          char *tp;

          for (tp = shellpath; *tp; tp++)
            if (*tp == '\\')
              *tp = '/';

          v = define_variable_loc (varname, strlen (varname),
                                   shellpath, origin, flavor == f_recursive,
                                   flocp);
        }
      else
        {
          const char *shellbase, *bslash;
          struct variable *pathv = lookup_variable ("PATH", 4);
          char *path_string;
          char *fake_env[2];
          size_t pathlen = 0;

          shellbase = strrchr (p, '/');
          bslash = strrchr (p, '\\');
          if (!shellbase || bslash > shellbase)
            shellbase = bslash;
          if (!shellbase && p[1] == ':')
            shellbase = p + 1;
          if (shellbase)
            shellbase++;
          else
            shellbase = p;

          /* Search for the basename of the shell (with standard
             executable extensions) along the $PATH.  */
          if (pathv)
            pathlen = strlen (pathv->value);
          path_string = xmalloc (5 + pathlen + 2 + 1);
          /* On MSDOS, current directory is considered as part of $PATH.  */
          sprintf (path_string, "PATH=.;%s", pathv ? pathv->value : "");
          fake_env[0] = path_string;
          fake_env[1] = 0;
          if (__dosexec_find_on_path (shellbase, fake_env, shellpath))
            {
              char *tp;

              for (tp = shellpath; *tp; tp++)
                if (*tp == '\\')
                  *tp = '/';

              v = define_variable_loc (varname, strlen (varname),
                                       shellpath, origin,
                                       flavor == f_recursive, flocp);
            }
          else
            v = lookup_variable (varname, strlen (varname));

          free (path_string);
        }
    }
  else
#endif /* __MSDOS__ */
#ifdef WINDOWS32
  if ((origin == o_file || origin == o_override || origin == o_command)
      && streq (varname, "SHELL"))
    {
      extern const char *default_shell;

      /* Call shell locator function. If it returns TRUE, then
         set no_default_sh_exe to indicate sh was found and
         set new value for SHELL variable.  */

      if (find_and_set_default_shell (p))
        {
          v = define_variable_in_set (varname, strlen (varname), default_shell,
                                      origin, flavor == f_recursive,
                                      (target_var
                                       ? current_variable_set_list->set
                                       : NULL),
                                      flocp);
          no_default_sh_exe = 0;
        }
      else
        {
          char *tp = alloc_value;

          alloc_value = allocated_variable_expand (p);

          if (find_and_set_default_shell (alloc_value))
            {
              v = define_variable_in_set (varname, strlen (varname), p,
                                          origin, flavor == f_recursive,
                                          (target_var
                                           ? current_variable_set_list->set
                                           : NULL),
                                          flocp);
              no_default_sh_exe = 0;
            }
          else
            v = lookup_variable (varname, strlen (varname));

          free (tp);
        }
    }
  else
    v = NULL;

  /* If not $SHELL, or if $SHELL points to a program we didn't find,
     just process this variable "as usual".  */
  if (!v)
#endif

  /* If we are defining variables inside an $(eval ...), we might have a
     different variable context pushed, not the global context (maybe we're
     inside a $(call ...) or something.  Since this function is only ever
     invoked in places where we want to define globally visible variables,
     make sure we define this variable in the global set.  */

  v = define_variable_in_set (varname, strlen (varname), p,
                              origin, flavor == f_recursive,
                              (target_var
                               ? current_variable_set_list->set : NULL),
                              flocp);
  v->append = append;
  v->conditional = conditional;

 done:
  free (alloc_value);
  return v->special ? set_special_var (v) : v;
}

/* Parse P (a null-terminated string) as a variable definition.

   If it is not a variable definition, return NULL and the contents of *VAR
   are undefined, except NAME is set to the first non-space character or NIL.

   If it is a variable definition, return a pointer to the char after the
   assignment token and set the following fields (only) of *VAR:
    name   : name of the variable (ALWAYS SET) (NOT NUL-TERMINATED!)
    length : length of the variable name
    value  : value of the variable (nul-terminated)
    flavor : flavor of the variable
   Other values in *VAR are unchanged.
  */

char *
parse_variable_definition (const char *p, struct variable *var)
{
  int wspace = 0;
  const char *e = NULL;

  NEXT_TOKEN (p);
  var->name = (char *)p;
  var->length = 0;

  while (1)
    {
      int c = *p++;

      /* If we find a comment or EOS, it's not a variable definition.  */
      if (STOP_SET (c, MAP_COMMENT|MAP_NUL))
        return NULL;

      if (c == '$')
        {
          /* This begins a variable expansion reference.  Make sure we don't
             treat chars inside the reference as assignment tokens.  */
          char closeparen;
          unsigned int count;

          c = *p++;
          if (c == '(')
            closeparen = ')';
          else if (c == '{')
            closeparen = '}';
          else if (c == '\0')
            return NULL;
          else
            /* '$$' or '$X'.  Either way, nothing special to do here.  */
            continue;

          /* P now points past the opening paren or brace.
             Count parens or braces until it is matched.  */
          for (count = 1; *p != '\0'; ++p)
            {
              if (*p == closeparen && --count == 0)
                {
                  ++p;
                  break;
                }
              if (*p == c)
                ++count;
            }
          continue;
        }

      /* If we find whitespace skip it, and remember we found it.  */
      if (ISBLANK (c))
        {
          wspace = 1;
          e = p - 1;
          NEXT_TOKEN (p);
          c = *p;
          if (c == '\0')
            return NULL;
          ++p;
        }


      if (c == '=')
        {
          var->flavor = f_recursive;
          if (! e)
            e = p - 1;
          break;
        }

      /* Match assignment variants (:=, +=, ?=, !=)  */
      if (*p == '=')
        {
          switch (c)
            {
              case ':':
                var->flavor = f_simple;
                break;
              case '+':
                var->flavor = f_append;
                break;
              case '?':
                var->flavor = f_conditional;
                break;
              case '!':
                var->flavor = f_shell;
                break;
              default:
                /* If we skipped whitespace, non-assignments means no var.  */
                if (wspace)
                  return NULL;

                /* Might be assignment, or might be $= or #=.  Check.  */
                continue;
            }
          if (! e)
            e = p - 1;
          ++p;
          break;
        }

      /* Check for POSIX ::= syntax  */
      if (c == ':')
        {
          /* A colon other than :=/::= is not a variable defn.  */
          if (*p != ':' || p[1] != '=')
            return NULL;

          /* POSIX allows ::= to be the same as GNU make's := */
          var->flavor = f_simple;
          if (! e)
            e = p - 1;
          p += 2;
          break;
        }

      /* If we skipped whitespace, non-assignments means no var.  */
      if (wspace)
        return NULL;
    }

  var->length = (unsigned int) (e - var->name);
  var->value = next_token (p);
  return (char *)p;
}

/* Try to interpret LINE (a null-terminated string) as a variable definition.

   If LINE was recognized as a variable definition, a pointer to its 'struct
   variable' is returned.  If LINE is not a variable definition, NULL is
   returned.  */

struct variable *
assign_variable_definition (struct variable *v, const char *line)
{
  char *name;

  if (!parse_variable_definition (line, v))
    return NULL;

  /* Expand the name, so "$(foo)bar = baz" works.  */
  name = alloca (v->length + 1);
  memcpy (name, v->name, v->length);
  name[v->length] = '\0';
  v->name = allocated_variable_expand (name);

  if (v->name[0] == '\0')
    O (fatal, &v->fileinfo, _("empty variable name"));

  return v;
}

/* Try to interpret LINE (a null-terminated string) as a variable definition.

   ORIGIN may be o_file, o_override, o_env, o_env_override,
   or o_command specifying that the variable definition comes
   from a makefile, an override directive, the environment with
   or without the -e switch, or the command line.

   See the comments for assign_variable_definition().

   If LINE was recognized as a variable definition, a pointer to its 'struct
   variable' is returned.  If LINE is not a variable definition, NULL is
   returned.  */

struct variable *
try_variable_definition (const floc *flocp, const char *line,
                         enum variable_origin origin, int target_var)
{
  struct variable v;
  struct variable *vp;

  if (flocp != 0)
    v.fileinfo = *flocp;
  else
    v.fileinfo.filenm = 0;

  if (!assign_variable_definition (&v, line))
    return 0;

  vp = do_variable_definition (flocp, v.name, v.value,
                               origin, v.flavor, target_var);

  free (v.name);

  return vp;
}

/* Print information for variable V, prefixing it with PREFIX.  */

static void
print_variable (const void *item, void *arg)
{
  const struct variable *v = item;
  const char *prefix = arg;
  const char *origin;

  switch (v->origin)
    {
    case o_automatic:
      origin = _("automatic");
      break;
    case o_default:
      origin = _("default");
      break;
    case o_env:
      origin = _("environment");
      break;
    case o_file:
      origin = _("makefile");
      break;
    case o_env_override:
      origin = _("environment under -e");
      break;
    case o_command:
      origin = _("command line");
      break;
    case o_override:
      origin = _("'override' directive");
      break;
    case o_invalid:
    default:
      abort ();
    }
  fputs ("# ", stdout);
  fputs (origin, stdout);
  if (v->private_var)
    fputs (" private", stdout);
  if (v->fileinfo.filenm)
    printf (_(" (from '%s', line %lu)"),
            v->fileinfo.filenm, v->fileinfo.lineno + v->fileinfo.offset);
  putchar ('\n');
  fputs (prefix, stdout);

  /* Is this a 'define'?  */
  if (v->recursive && strchr (v->value, '\n') != 0)
    printf ("define %s\n%s\nendef\n", v->name, v->value);
  else
    {
      char *p;

      printf ("%s %s= ", v->name, v->recursive ? v->append ? "+" : "" : ":");

      /* Check if the value is just whitespace.  */
      p = next_token (v->value);
      if (p != v->value && *p == '\0')
        /* All whitespace.  */
        printf ("$(subst ,,%s)", v->value);
      else if (v->recursive)
        fputs (v->value, stdout);
      else
        /* Double up dollar signs.  */
        for (p = v->value; *p != '\0'; ++p)
          {
            if (*p == '$')
              putchar ('$');
            putchar (*p);
          }
      putchar ('\n');
    }
}


static void
print_auto_variable (const void *item, void *arg)
{
  const struct variable *v = item;

  if (v->origin == o_automatic)
    print_variable (item, arg);
}


static void
print_noauto_variable (const void *item, void *arg)
{
  const struct variable *v = item;

  if (v->origin != o_automatic)
    print_variable (item, arg);
}


/* Print all the variables in SET.  PREFIX is printed before
   the actual variable definitions (everything else is comments).  */

static void
print_variable_set (struct variable_set *set, const char *prefix, int pauto)
{
  hash_map_arg (&set->table, (pauto ? print_auto_variable : print_variable),
                (void *)prefix);

  fputs (_("# variable set hash-table stats:\n"), stdout);
  fputs ("# ", stdout);
  hash_print_stats (&set->table, stdout);
  putc ('\n', stdout);
}

/* Print the data base of variables.  */

void
print_variable_data_base (void)
{
  puts (_("\n# Variables\n"));

  print_variable_set (&global_variable_set, "", 0);

  puts (_("\n# Pattern-specific Variable Values"));

  {
    struct pattern_var *p;
    unsigned int rules = 0;

    for (p = pattern_vars; p != 0; p = p->next)
      {
        ++rules;
        printf ("\n%s :\n", p->target);
        print_variable (&p->variable, (void *)"# ");
      }

    if (rules == 0)
      puts (_("\n# No pattern-specific variable values."));
    else
      printf (_("\n# %u pattern-specific variable values"), rules);
  }
}


/* Print all the local variables of FILE.  */

void
print_file_variables (const struct file *file)
{
  if (file->variables != 0)
    print_variable_set (file->variables->set, "# ", 1);
}

void
print_target_variables (const struct file *file)
{
  if (file->variables != 0)
    {
      size_t l = strlen (file->name);
      char *t = alloca (l + 3);

      strcpy (t, file->name);
      t[l] = ':';
      t[l+1] = ' ';
      t[l+2] = '\0';

      hash_map_arg (&file->variables->set->table, print_noauto_variable, t);
    }
}

#ifdef WINDOWS32
void
sync_Path_environment (void)
{
  char *path = allocated_variable_expand ("$(PATH)");
  static char *environ_path = NULL;

  if (!path)
    return;

  /* If done this before, free the previous entry before allocating new one.  */
  free (environ_path);

  /* Create something WINDOWS32 world can grok.  */
  convert_Path_to_windows32 (path, ';');
  environ_path = xstrdup (concat (3, "PATH", "=", path));
  putenv (environ_path);
  free (path);
}
#endif
