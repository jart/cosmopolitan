/* Basic dependency engine for GNU Make.
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
#include "filedef.h"
#include "job.h"
#include "commands.h"
#include "dep.h"
#include "variable.h"
#include "debug.h"

#include <assert.h>

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#else
#include <sys/file.h>
#endif

#ifdef VMS
#include <starlet.h>
#endif
#ifdef WINDOWS32
#include <io.h>
#endif


/* The test for circular dependencies is based on the 'updating' bit in
   'struct file'.  However, double colon targets have separate 'struct
   file's; make sure we always use the base of the double colon chain. */

#define start_updating(_f)  (((_f)->double_colon ? (_f)->double_colon : (_f))\
                             ->updating = 1)
#define finish_updating(_f) (((_f)->double_colon ? (_f)->double_colon : (_f))\
                             ->updating = 0)
#define is_updating(_f)     (((_f)->double_colon ? (_f)->double_colon : (_f))\
                             ->updating)


/* Incremented when a command is started (under -n, when one would be).  */
unsigned int commands_started = 0;

/* Set to the goal dependency.  Mostly needed for remaking makefiles.  */
static struct goaldep *goal_list;
static struct dep *goal_dep;

/* Current value for pruning the scan of the goal chain.
   All files start with considered == 0.  */
static unsigned int considered = 0;

static enum update_status update_file (struct file *file, unsigned int depth);
static enum update_status update_file_1 (struct file *file, unsigned int depth);
static enum update_status check_dep (struct file *file, unsigned int depth,
                                     FILE_TIMESTAMP this_mtime, int *must_make);
static enum update_status touch_file (struct file *file);
static void remake_file (struct file *file);
static FILE_TIMESTAMP name_mtime (const char *name);
static const char *library_search (const char *lib, FILE_TIMESTAMP *mtime_ptr);


/* Remake all the goals in the 'struct dep' chain GOALS.  Return -1 if nothing
   was done, 0 if all goals were updated successfully, or 1 if a goal failed.

   If rebuilding_makefiles is nonzero, these goals are makefiles, so -t, -q,
   and -n should be disabled for them unless they were also command-line
   targets, and we should only make one goal at a time and return as soon as
   one goal whose 'changed' member is nonzero is successfully made.  */

enum update_status
update_goal_chain (struct goaldep *goaldeps)
{
  int t = touch_flag, q = question_flag, n = just_print_flag;
  enum update_status status = us_none;

  /* Duplicate the chain so we can remove things from it.  */

  struct dep *goals = copy_dep_chain ((struct dep *)goaldeps);

  goal_list = rebuilding_makefiles ? goaldeps : NULL;

#define MTIME(file) (rebuilding_makefiles ? file_mtime_no_search (file) \
                     : file_mtime (file))

  /* Start a fresh batch of consideration.  */
  ++considered;

  /* Update all the goals until they are all finished.  */

  while (goals != 0)
    {
      struct dep *g, *lastgoal;

      /* Start jobs that are waiting for the load to go down.  */

      start_waiting_jobs ();

      /* Wait for a child to die.  */

      reap_children (1, 0);

      lastgoal = 0;
      g = goals;
      while (g != 0)
        {
          /* Iterate over all double-colon entries for this file.  */
          struct file *file;
          int stop = 0, any_not_updated = 0;

          goal_dep = g;

          for (file = g->file->double_colon ? g->file->double_colon : g->file;
               file != NULL;
               file = file->prev)
            {
              unsigned int ocommands_started;
              enum update_status fail;

              file->dontcare = ANY_SET (g->flags, RM_DONTCARE);

              check_renamed (file);
              if (rebuilding_makefiles)
                {
                  if (file->cmd_target)
                    {
                      touch_flag = t;
                      question_flag = q;
                      just_print_flag = n;
                    }
                  else
                    touch_flag = question_flag = just_print_flag = 0;
                }

              /* Save the old value of 'commands_started' so we can compare
                 later.  It will be incremented when any commands are
                 actually run.  */
              ocommands_started = commands_started;

              fail = update_file (file, rebuilding_makefiles ? 1 : 0);
              check_renamed (file);

              /* Set the goal's 'changed' flag if any commands were started
                 by calling update_file above.  We check this flag below to
                 decide when to give an "up to date" diagnostic.  */
              if (commands_started > ocommands_started)
                g->changed = 1;

              stop = 0;
              if ((fail || file->updated) && status < us_question)
                {
                  /* We updated this goal.  Update STATUS and decide whether
                     to stop.  */
                  if (file->update_status)
                    {
                      /* Updating failed, or -q triggered.  The STATUS value
                         tells our caller which.  */
                      status = file->update_status;
                      /* If -q just triggered, stop immediately.  It doesn't
                         matter how much more we run, since we already know
                         the answer to return.  */
                      stop = (question_flag && !keep_going_flag
                              && !rebuilding_makefiles);
                    }
                  else
                    {
                      FILE_TIMESTAMP mtime = MTIME (file);
                      check_renamed (file);

                      if (file->updated && g->changed &&
                           mtime != file->mtime_before_update)
                        {
                          /* Updating was done.  If this is a makefile and
                             just_print_flag or question_flag is set (meaning
                             -n or -q was given and this file was specified
                             as a command-line target), don't change STATUS.
                             If STATUS is changed, we will get re-exec'd, and
                             enter an infinite loop.  */
                          if (!rebuilding_makefiles
                              || (!just_print_flag && !question_flag))
                            status = us_success;
                          if (rebuilding_makefiles && file->dontcare)
                            /* This is a default makefile; stop remaking.  */
                            stop = 1;
                        }
                    }
                }

              /* Keep track if any double-colon entry is not finished.
                 When they are all finished, the goal is finished.  */
              any_not_updated |= !file->updated;

              file->dontcare = 0;

              if (stop)
                break;
            }

          /* Reset FILE since it is null at the end of the loop.  */
          file = g->file;

          if (stop || !any_not_updated)
            {
              /* If we have found nothing whatever to do for the goal,
                 print a message saying nothing needs doing.  */

              if (!rebuilding_makefiles
                  /* If the update_status is success, we updated successfully
                     or not at all.  G->changed will have been set above if
                     any commands were actually started for this goal.  */
                  && file->update_status == us_success && !g->changed
                  /* Never give a message under -s or -q.  */
                  && !run_silent && !question_flag)
                OS (message, 1, ((file->phony || file->cmds == 0)
                                 ? _("Nothing to be done for '%s'.")
                                 : _("'%s' is up to date.")),
                    file->name);

              /* This goal is finished.  Remove it from the chain.  */
              if (lastgoal == 0)
                goals = g->next;
              else
                lastgoal->next = g->next;

              /* Free the storage.  */
              free (g);

              g = lastgoal == 0 ? goals : lastgoal->next;

              if (stop)
                break;
            }
          else
            {
              lastgoal = g;
              g = g->next;
            }
        }

      /* If we reached the end of the dependency graph update CONSIDERED
         for the next pass.  */
      if (g == 0)
        ++considered;
    }

  if (rebuilding_makefiles)
    {
      touch_flag = t;
      question_flag = q;
      just_print_flag = n;
    }

  return status;
}

/* If we're rebuilding an included makefile that failed, and we care
   about errors, show an error message the first time.  */

void
show_goal_error (void)
{
  struct goaldep *goal;

  if ((goal_dep->flags & (RM_INCLUDED|RM_DONTCARE)) != RM_INCLUDED)
    return;

  for (goal = goal_list; goal; goal = goal->next)
    if (goal_dep->file == goal->file)
      {
        if (goal->error)
          {
            OSS (error, &goal->floc, "%s: %s",
                 goal->file->name, strerror ((int)goal->error));
            goal->error = 0;
          }
        return;
      }
}

/* If FILE is not up to date, execute the commands for it.
   Return 0 if successful, non-0 if unsuccessful;
   but with some flag settings, just call 'exit' if unsuccessful.

   DEPTH is the depth in recursions of this function.
   We increment it during the consideration of our dependencies,
   then decrement it again after finding out whether this file
   is out of date.

   If there are multiple double-colon entries for FILE,
   each is considered in turn.  */

static enum update_status
update_file (struct file *file, unsigned int depth)
{
  enum update_status status = us_success;
  struct file *f;

  f = file->double_colon ? file->double_colon : file;

  /* Prune the dependency graph: if we've already been here on _this_
     pass through the dependency graph, we don't have to go any further.
     We won't reap_children until we start the next pass, so no state
     change is possible below here until then.  */
  if (f->considered == considered)
    {
      /* Check for the case where a target has been tried and failed but
         the diagnostics haven't been issued. If we need the diagnostics
         then we will have to continue. */
      if (!(f->updated && f->update_status > us_none
            && !f->dontcare && f->no_diag))
        {
          DBF (DB_VERBOSE, _("Pruning file '%s'.\n"));
          return f->command_state == cs_finished ? f->update_status : us_success;
        }
    }

  /* This loop runs until we start commands for a double colon rule, or until
     the chain is exhausted. */
  for (; f != 0; f = f->prev)
    {
      enum update_status new;

      f->considered = considered;

      new = update_file_1 (f, depth);
      check_renamed (f);

      /* Clean up any alloca() used during the update.  */
      alloca (0);

      /* If we got an error, don't bother with double_colon etc.  */
      if (new && !keep_going_flag)
        return new;

      if (f->command_state == cs_running
          || f->command_state == cs_deps_running)
        /* Don't run other :: rules for this target until
           this rule is finished.  */
        return us_success;

      if (new > status)
        status = new;
    }

  return status;
}

/* Show a message stating the target failed to build.  */

static void
complain (struct file *file)
{
  /* If this file has no_diag set then it means we tried to update it
     before in the dontcare mode and failed. The target that actually
     failed is not necessarily this file but could be one of its direct
     or indirect dependencies. So traverse this file's dependencies and
     find the one that actually caused the failure. */

  struct dep *d;

  for (d = file->deps; d != 0; d = d->next)
    {
      if (d->file->updated && d->file->update_status > us_none && file->no_diag)
        {
          complain (d->file);
          break;
        }
    }

  if (d == 0)
    {
      show_goal_error ();

      /* Didn't find any dependencies to complain about. */
      if (file->parent)
        {
          size_t l = strlen (file->name) + strlen (file->parent->name) + 4;
          const char *m = _("%sNo rule to make target '%s', needed by '%s'%s");

          if (!keep_going_flag)
            fatal (NILF, l, m, "", file->name, file->parent->name, "");

          error (NILF, l, m, "*** ", file->name, file->parent->name, ".");
        }
      else
        {
          size_t l = strlen (file->name) + 4;
          const char *m = _("%sNo rule to make target '%s'%s");

          if (!keep_going_flag)
            fatal (NILF, l, m, "", file->name, "");

          error (NILF, l, m, "*** ", file->name, ".");
        }

      file->no_diag = 0;
    }
}

/* Consider a single 'struct file' and update it as appropriate.
   Return 0 on success, or non-0 on failure.  */

static enum update_status
update_file_1 (struct file *file, unsigned int depth)
{
  enum update_status dep_status = us_success;
  FILE_TIMESTAMP this_mtime;
  int noexist, must_make, deps_changed;
  struct file *ofile;
  struct dep *d, *ad;
  struct dep amake;
  int running = 0;

  DBF (DB_VERBOSE, _("Considering target file '%s'.\n"));

  if (file->updated)
    {
      if (file->update_status > us_none)
        {
          DBF (DB_VERBOSE,
               _("Recently tried and failed to update file '%s'.\n"));

          /* If the file we tried to make is marked no_diag then no message
             was printed about it when it failed during the makefile rebuild.
             If we're trying to build it again in the normal rebuild, print a
             message now.  */
          if (file->no_diag && !file->dontcare)
              complain (file);

          return file->update_status;
        }

      DBF (DB_VERBOSE, _("File '%s' was considered already.\n"));
      return 0;
    }

  switch (file->command_state)
    {
    case cs_not_started:
    case cs_deps_running:
      break;
    case cs_running:
      DBF (DB_VERBOSE, _("Still updating file '%s'.\n"));
      return 0;
    case cs_finished:
      DBF (DB_VERBOSE, _("Finished updating file '%s'.\n"));
      return file->update_status;
    default:
      abort ();
    }

  /* Determine whether the diagnostics will be issued should this update
     fail. */
  file->no_diag = file->dontcare;

  ++depth;

  /* Notice recursive update of the same file.  */
  start_updating (file);

  /* We might change file if we find a different one via vpath;
     remember this one to turn off updating.  */
  ofile = file;

  /* Looking at the file's modtime beforehand allows the possibility
     that its name may be changed by a VPATH search, and thus it may
     not need an implicit rule.  If this were not done, the file
     might get implicit commands that apply to its initial name, only
     to have that name replaced with another found by VPATH search.  */

  this_mtime = file_mtime (file);
  check_renamed (file);
  noexist = this_mtime == NONEXISTENT_MTIME;
  if (noexist)
    DBF (DB_BASIC, _("File '%s' does not exist.\n"));
  else if (ORDINARY_MTIME_MIN <= this_mtime && this_mtime <= ORDINARY_MTIME_MAX
           && file->low_resolution_time)
    {
      /* Avoid spurious rebuilds due to low resolution time stamps.  */
      int ns = FILE_TIMESTAMP_NS (this_mtime);
      if (ns != 0)
        OS (error, NILF,
            _("*** Warning: .LOW_RESOLUTION_TIME file '%s' has a high resolution time stamp"),
            file->name);
      this_mtime += FILE_TIMESTAMPS_PER_S - 1 - ns;
    }

  must_make = noexist;

  /* If file was specified as a target with no commands,
     come up with some default commands.  */

  if (!file->phony && file->cmds == 0 && !file->tried_implicit)
    {
      if (try_implicit_rule (file, depth))
        DBF (DB_IMPLICIT, _("Found an implicit rule for '%s'.\n"));
      else
        DBF (DB_IMPLICIT, _("No implicit rule found for '%s'.\n"));
      file->tried_implicit = 1;
    }
  if (file->cmds == 0 && !file->is_target
      && default_file != 0 && default_file->cmds != 0)
    {
      DBF (DB_IMPLICIT, _("Using default recipe for '%s'.\n"));
      file->cmds = default_file->cmds;
    }

  /* Update all non-intermediate files we depend on, if necessary, and see
     whether any of them is more recent than this file.  We need to walk our
     deps, AND the deps of any also_make targets to ensure everything happens
     in the correct order.  */

  amake.file = file;
  amake.next = file->also_make;
  ad = &amake;
  while (ad)
    {
      struct dep *lastd = 0;

      /* Find the deps we're scanning */
      d = ad->file->deps;
      ad = ad->next;

      while (d)
        {
          enum update_status new;
          FILE_TIMESTAMP mtime;
          int maybe_make;
          int dontcare = 0;

          check_renamed (d->file);

          mtime = file_mtime (d->file);
          check_renamed (d->file);

          if (is_updating (d->file))
            {
              OSS (error, NILF, _("Circular %s <- %s dependency dropped."),
                   file->name, d->file->name);
              /* We cannot free D here because our the caller will still have
                 a reference to it when we were called recursively via
                 check_dep below.  */
              if (lastd == 0)
                file->deps = d->next;
              else
                lastd->next = d->next;
              d = d->next;
              continue;
            }

          d->file->parent = file;
          maybe_make = must_make;

          /* Inherit dontcare flag from our parent. */
          if (rebuilding_makefiles)
            {
              dontcare = d->file->dontcare;
              d->file->dontcare = file->dontcare;
            }

          new = check_dep (d->file, depth, this_mtime, &maybe_make);
          if (new > dep_status)
            dep_status = new;

          /* Restore original dontcare flag. */
          if (rebuilding_makefiles)
            d->file->dontcare = dontcare;

          if (! d->ignore_mtime)
            must_make = maybe_make;

          check_renamed (d->file);

          {
            struct file *f = d->file;
            if (f->double_colon)
              f = f->double_colon;
            do
              {
                running |= (f->command_state == cs_running
                            || f->command_state == cs_deps_running);
                f = f->prev;
              }
            while (f != 0);
          }

          if (dep_status && !keep_going_flag)
            break;

          if (!running)
            /* The prereq is considered changed if the timestamp has changed
               while it was built, OR it doesn't exist.  */
            d->changed = ((file_mtime (d->file) != mtime)
                          || (mtime == NONEXISTENT_MTIME));

          lastd = d;
          d = d->next;
        }
    }

  /* Now we know whether this target needs updating.
     If it does, update all the intermediate files we depend on.  */

  if (must_make || always_make_flag)
    {
      for (d = file->deps; d != 0; d = d->next)
        if (d->file->intermediate)
          {
            enum update_status new;
            int dontcare = 0;

            FILE_TIMESTAMP mtime = file_mtime (d->file);
            check_renamed (d->file);
            d->file->parent = file;

            /* Inherit dontcare flag from our parent. */
            if (rebuilding_makefiles)
              {
                dontcare = d->file->dontcare;
                d->file->dontcare = file->dontcare;
              }

            /* We may have already considered this file, when we didn't know
               we'd need to update it.  Force update_file() to consider it and
               not prune it.  */
            d->file->considered = 0;

            new = update_file (d->file, depth);
            if (new > dep_status)
              dep_status = new;

            /* Restore original dontcare flag. */
            if (rebuilding_makefiles)
              d->file->dontcare = dontcare;

            check_renamed (d->file);

            {
              struct file *f = d->file;
              if (f->double_colon)
                f = f->double_colon;
              do
                {
                  running |= (f->command_state == cs_running
                              || f->command_state == cs_deps_running);
                  f = f->prev;
                }
              while (f != 0);
            }

            if (dep_status && !keep_going_flag)
              break;

            if (!running)
              d->changed = ((file->phony && file->cmds != 0)
                            || file_mtime (d->file) != mtime);
          }
    }

  finish_updating (file);
  finish_updating (ofile);

  DBF (DB_VERBOSE, _("Finished prerequisites of target file '%s'.\n"));

  if (running)
    {
      set_command_state (file, cs_deps_running);
      --depth;
      DBF (DB_VERBOSE, _("The prerequisites of '%s' are being made.\n"));
      return 0;
    }

  /* If any dependency failed, give up now.  */

  if (dep_status)
    {
      /* I'm not sure if we can't just assign dep_status...  */
      file->update_status = dep_status == us_none ? us_failed : dep_status;
      notice_finished_file (file);

      --depth;

      DBF (DB_VERBOSE, _("Giving up on target file '%s'.\n"));

      if (depth == 0 && keep_going_flag
          && !just_print_flag && !question_flag)
        OS (error, NILF,
            _("Target '%s' not remade because of errors."), file->name);

      return dep_status;
    }

  if (file->command_state == cs_deps_running)
    /* The commands for some deps were running on the last iteration, but
       they have finished now.  Reset the command_state to not_started to
       simplify later bookkeeping.  It is important that we do this only
       when the prior state was cs_deps_running, because that prior state
       was definitely propagated to FILE's also_make's by set_command_state
       (called above), but in another state an also_make may have
       independently changed to finished state, and we would confuse that
       file's bookkeeping (updated, but not_started is bogus state).  */
    set_command_state (file, cs_not_started);

  /* Now record which prerequisites are more
     recent than this file, so we can define $?.  */

  deps_changed = 0;
  for (d = file->deps; d != 0; d = d->next)
    {
      FILE_TIMESTAMP d_mtime = file_mtime (d->file);
      check_renamed (d->file);

      if (! d->ignore_mtime)
        {
#if 1
          /* %%% In version 4, remove this code completely to
           implement not remaking deps if their deps are newer
           than their parents.  */
          if (d_mtime == NONEXISTENT_MTIME && !d->file->intermediate)
            /* We must remake if this dep does not
               exist and is not intermediate.  */
            must_make = 1;
#endif

          /* Set DEPS_CHANGED if this dep actually changed.  */
          deps_changed |= d->changed;
        }

      /* Set D->changed if either this dep actually changed,
         or its dependent, FILE, is older or does not exist.  */
      d->changed |= noexist || d_mtime > this_mtime;

      if (!noexist && ISDB (DB_BASIC|DB_VERBOSE))
        {
          const char *fmt = 0;

          if (d->ignore_mtime)
            {
              if (ISDB (DB_VERBOSE))
                fmt = _("Prerequisite '%s' is order-only for target '%s'.\n");
            }
          else if (d_mtime == NONEXISTENT_MTIME)
            {
              if (ISDB (DB_BASIC))
                fmt = _("Prerequisite '%s' of target '%s' does not exist.\n");
            }
          else if (d->changed)
            {
              if (ISDB (DB_BASIC))
                fmt = _("Prerequisite '%s' is newer than target '%s'.\n");
            }
          else if (ISDB (DB_VERBOSE))
            fmt = _("Prerequisite '%s' is older than target '%s'.\n");

          if (fmt)
            {
              print_spaces (depth);
              printf (fmt, dep_name (d), file->name);
              fflush (stdout);
            }
        }
    }

  /* Here depth returns to the value it had when we were called.  */
  depth--;

  if (file->double_colon && file->deps == 0)
    {
      must_make = 1;
      DBF (DB_BASIC,
           _("Target '%s' is double-colon and has no prerequisites.\n"));
    }
  else if (!noexist && file->is_target && !deps_changed && file->cmds == 0
           && !always_make_flag)
    {
      must_make = 0;
      DBF (DB_VERBOSE,
           _("No recipe for '%s' and no prerequisites actually changed.\n"));
    }
  else if (!must_make && file->cmds != 0 && always_make_flag)
    {
      must_make = 1;
      DBF (DB_VERBOSE, _("Making '%s' due to always-make flag.\n"));
    }

  if (!must_make)
    {
      if (ISDB (DB_VERBOSE))
        {
          print_spaces (depth);
          printf (_("No need to remake target '%s'"), file->name);
          if (!streq (file->name, file->hname))
              printf (_("; using VPATH name '%s'"), file->hname);
          puts (".");
          fflush (stdout);
        }

      notice_finished_file (file);

      /* Since we don't need to remake the file, convert it to use the
         VPATH filename if we found one.  hfile will be either the
         local name if no VPATH or the VPATH name if one was found.  */

      while (file)
        {
          file->name = file->hname;
          file = file->prev;
        }

      return 0;
    }

  DBF (DB_BASIC, _("Must remake target '%s'.\n"));

  /* It needs to be remade.  If it's VPATH and not reset via GPATH, toss the
     VPATH.  */
  if (!streq (file->name, file->hname))
    {
      DB (DB_BASIC, (_("  Ignoring VPATH name '%s'.\n"), file->hname));
      file->ignore_vpath = 1;
    }

  /* Now, take appropriate actions to remake the file.  */
  remake_file (file);

  if (file->command_state != cs_finished)
    {
      DBF (DB_VERBOSE, _("Recipe of '%s' is being run.\n"));
      return 0;
    }

  switch (file->update_status)
    {
    case us_failed:
      DBF (DB_BASIC, _("Failed to remake target file '%s'.\n"));
      break;
    case us_success:
      DBF (DB_BASIC, _("Successfully remade target file '%s'.\n"));
      break;
    case us_question:
      DBF (DB_BASIC, _("Target file '%s' needs to be remade under -q.\n"));
      break;
    case us_none:
      break;
    }

  file->updated = 1;
  return file->update_status;
}

/* Set FILE's 'updated' flag and re-check its mtime and the mtime's of all
   files listed in its 'also_make' member.  Under -t, this function also
   touches FILE.

   On return, FILE->update_status will no longer be us_none if it was.  */

void
notice_finished_file (struct file *file)
{
  struct dep *d;
  int ran = file->command_state == cs_running;
  int touched = 0;

  file->command_state = cs_finished;
  file->updated = 1;

  if (touch_flag
      /* The update status will be:
           us_success   if 0 or more commands (+ or ${MAKE}) were run and won;
           us_none      if this target was not remade;
           >us_none     if some commands were run and lost.
         We touch the target if it has commands which either were not run
         or won when they ran (i.e. status is 0).  */
      && file->update_status == us_success)
    {
      if (file->cmds != 0 && file->cmds->any_recurse)
        {
          /* If all the command lines were recursive,
             we don't want to do the touching.  */
          unsigned int i;
          for (i = 0; i < file->cmds->ncommand_lines; ++i)
            if (!(file->cmds->lines_flags[i] & COMMANDS_RECURSE))
              goto have_nonrecursing;
        }
      else
        {
        have_nonrecursing:
          if (file->phony)
            file->update_status = us_success;
          /* According to POSIX, -t doesn't affect targets with no cmds.  */
          else if (file->cmds != 0)
            {
              /* Should set file's modification date and do nothing else.  */
              file->update_status = touch_file (file);

              /* Pretend we ran a real touch command, to suppress the
                 "'foo' is up to date" message.  */
              commands_started++;

              /* Request for the timestamp to be updated (and distributed
                 to the double-colon entries). Simply setting ran=1 would
                 almost have done the trick, but messes up with the also_make
                 updating logic below.  */
              touched = 1;
            }
        }
    }

  if (file->mtime_before_update == UNKNOWN_MTIME)
    file->mtime_before_update = file->last_mtime;

  if ((ran && !file->phony) || touched)
    {
      int i = 0;

      /* If -n, -t, or -q and all the commands are recursive, we ran them so
         really check the target's mtime again.  Otherwise, assume the target
         would have been updated. */

      if ((question_flag || just_print_flag || touch_flag) && file->cmds)
        {
          for (i = file->cmds->ncommand_lines; i > 0; --i)
            if (! (file->cmds->lines_flags[i-1] & COMMANDS_RECURSE))
              break;
        }

      /* If there were no commands at all, it's always new. */

      else if (file->is_target && file->cmds == 0)
        i = 1;

      file->last_mtime = i == 0 ? UNKNOWN_MTIME : NEW_MTIME;
    }

  if (file->double_colon)
    {
      /* If this is a double colon rule and it is the last one to be
         updated, propagate the change of modification time to all the
         double-colon entries for this file.

         We do it on the last update because it is important to handle
         individual entries as separate rules with separate timestamps
         while they are treated as targets and then as one rule with the
         unified timestamp when they are considered as a prerequisite
         of some target.  */

      struct file *f;
      FILE_TIMESTAMP max_mtime = file->last_mtime;

      /* Check that all rules were updated and at the same time find
         the max timestamp.  We assume UNKNOWN_MTIME is newer then
         any other value.  */
      for (f = file->double_colon; f != 0 && f->updated; f = f->prev)
        if (max_mtime != UNKNOWN_MTIME
            && (f->last_mtime == UNKNOWN_MTIME || f->last_mtime > max_mtime))
          max_mtime = f->last_mtime;

      if (f == 0)
        for (f = file->double_colon; f != 0; f = f->prev)
          f->last_mtime = max_mtime;
    }

  if (ran && file->update_status != us_none)
    /* We actually tried to update FILE, which has
       updated its also_make's as well (if it worked).
       If it didn't work, it wouldn't work again for them.
       So mark them as updated with the same status.  */
    for (d = file->also_make; d != 0; d = d->next)
      {
        d->file->command_state = cs_finished;
        d->file->updated = 1;
        d->file->update_status = file->update_status;

        if (ran && !d->file->phony)
          /* Fetch the new modification time.
             We do this instead of just invalidating the cached time
             so that a vpath_search can happen.  Otherwise, it would
             never be done because the target is already updated.  */
          f_mtime (d->file, 0);
      }
  else if (file->update_status == us_none)
    /* Nothing was done for FILE, but it needed nothing done.
       So mark it now as "succeeded".  */
    file->update_status = us_success;
}

/* Check whether another file (whose mtime is THIS_MTIME) needs updating on
   account of a dependency which is file FILE.  If it does, store 1 in
   *MUST_MAKE_PTR.  In the process, update any non-intermediate files that
   FILE depends on (including FILE itself).  Return nonzero if any updating
   failed.  */

static enum update_status
check_dep (struct file *file, unsigned int depth,
           FILE_TIMESTAMP this_mtime, int *must_make_ptr)
{
  struct file *ofile;
  struct dep *d;
  enum update_status dep_status = us_success;

  ++depth;
  start_updating (file);

  /* We might change file if we find a different one via vpath;
     remember this one to turn off updating.  */
  ofile = file;

  if (file->phony || !file->intermediate)
    {
      /* If this is a non-intermediate file, update it and record whether it
         is newer than THIS_MTIME.  */
      FILE_TIMESTAMP mtime;
      dep_status = update_file (file, depth);
      check_renamed (file);
      mtime = file_mtime (file);
      check_renamed (file);
      if (mtime == NONEXISTENT_MTIME || mtime > this_mtime)
        *must_make_ptr = 1;
    }
  else
    {
      /* FILE is an intermediate file.  */
      FILE_TIMESTAMP mtime;

      if (!file->phony && file->cmds == 0 && !file->tried_implicit)
        {
          if (try_implicit_rule (file, depth))
            DBF (DB_IMPLICIT, _("Found an implicit rule for '%s'.\n"));
          else
            DBF (DB_IMPLICIT, _("No implicit rule found for '%s'.\n"));
          file->tried_implicit = 1;
        }
      if (file->cmds == 0 && !file->is_target
          && default_file != 0 && default_file->cmds != 0)
        {
          DBF (DB_IMPLICIT, _("Using default commands for '%s'.\n"));
          file->cmds = default_file->cmds;
        }

      check_renamed (file);
      mtime = file_mtime (file);
      check_renamed (file);
      if (mtime != NONEXISTENT_MTIME && mtime > this_mtime)
        /* If the intermediate file actually exists and is newer, then we
           should remake from it.  */
        *must_make_ptr = 1;
      else
        {
          /* Otherwise, update all non-intermediate files we depend on, if
             necessary, and see whether any of them is more recent than the
             file on whose behalf we are checking.  */
          struct dep *ld;
          int deps_running = 0;

          /* If this target is not running, set it's state so that we check it
             fresh.  It could be it was checked as part of an order-only
             prerequisite and so wasn't rebuilt then, but should be now.  */
          if (file->command_state != cs_running)
            {
              /* If the target was waiting for a dependency it has to be
                 reconsidered, as that dependency might have finished.  */
              if (file->command_state == cs_deps_running)
                file->considered = 0;

              set_command_state (file, cs_not_started);
            }

          ld = 0;
          d = file->deps;
          while (d != 0)
            {
              enum update_status new;
              int maybe_make;

              if (is_updating (d->file))
                {
                  OSS (error, NILF, _("Circular %s <- %s dependency dropped."),
                       file->name, d->file->name);
                  if (ld == 0)
                    {
                      file->deps = d->next;
                      free_dep (d);
                      d = file->deps;
                    }
                  else
                    {
                      ld->next = d->next;
                      free_dep (d);
                      d = ld->next;
                    }
                  continue;
                }

              d->file->parent = file;
              maybe_make = *must_make_ptr;
              new = check_dep (d->file, depth, this_mtime, &maybe_make);
              if (new > dep_status)
                dep_status = new;

              if (! d->ignore_mtime)
                *must_make_ptr = maybe_make;
              check_renamed (d->file);
              if (dep_status && !keep_going_flag)
                break;

              if (d->file->command_state == cs_running
                  || d->file->command_state == cs_deps_running)
                deps_running = 1;

              ld = d;
              d = d->next;
            }

          if (deps_running)
            /* Record that some of FILE's deps are still being made.
               This tells the upper levels to wait on processing it until the
               commands are finished.  */
            set_command_state (file, cs_deps_running);
        }
    }

  finish_updating (file);
  finish_updating (ofile);

  return dep_status;
}

/* Touch FILE.  Return us_success if successful, us_failed if not.  */

#define TOUCH_ERROR(call) do{ perror_with_name ((call), file->name);    \
                              return us_failed; }while(0)

static enum update_status
touch_file (struct file *file)
{
  if (!run_silent)
    OS (message, 0, "touch %s", file->name);

  /* Print-only (-n) takes precedence over touch (-t).  */
  if (just_print_flag)
    return us_success;

#ifndef NO_ARCHIVES
  if (ar_name (file->name))
    return ar_touch (file->name) ? us_failed : us_success;
  else
#endif
    {
      int fd;

      EINTRLOOP (fd, open (file->name, O_RDWR | O_CREAT, 0666));
      if (fd < 0)
        TOUCH_ERROR ("touch: open: ");
      else
        {
          struct stat statbuf;
          char buf = 'x';
          int e;

          EINTRLOOP (e, fstat (fd, &statbuf));
          if (e < 0)
            TOUCH_ERROR ("touch: fstat: ");
          /* Rewrite character 0 same as it already is.  */
          EINTRLOOP (e, read (fd, &buf, 1));
          if (e < 0)
            TOUCH_ERROR ("touch: read: ");
          {
            off_t o;
            EINTRLOOP (o, lseek (fd, 0L, 0));
            if (o < 0L)
              TOUCH_ERROR ("touch: lseek: ");
          }
          EINTRLOOP (e, write (fd, &buf, 1));
          if (e < 0)
            TOUCH_ERROR ("touch: write: ");

          /* If file length was 0, we just changed it, so change it back.  */
          if (statbuf.st_size == 0)
            {
              (void) close (fd);
              EINTRLOOP (fd, open (file->name, O_RDWR | O_TRUNC, 0666));
              if (fd < 0)
                TOUCH_ERROR ("touch: open: ");
            }
          (void) close (fd);
        }
    }

  return us_success;
}

/* Having checked and updated the dependencies of FILE,
   do whatever is appropriate to remake FILE itself.
   Return the status from executing FILE's commands.  */

static void
remake_file (struct file *file)
{
  if (file->cmds == 0)
    {
      if (file->phony)
        /* Phony target.  Pretend it succeeded.  */
        file->update_status = us_success;
      else if (file->is_target)
        /* This is a nonexistent target file we cannot make.
           Pretend it was successfully remade.  */
        file->update_status = us_success;
      else
        {
          /* This is a dependency file we cannot remake.  Fail.  */
          if (!rebuilding_makefiles || !file->dontcare)
            complain (file);
          file->update_status = us_failed;
        }
    }
  else
    {
      chop_commands (file->cmds);

      /* The normal case: start some commands.  */
      if (!touch_flag || file->cmds->any_recurse)
        {
          execute_file_commands (file);
          return;
        }

      /* This tells notice_finished_file it is ok to touch the file.  */
      file->update_status = us_success;
    }

  /* This does the touching under -t.  */
  notice_finished_file (file);
}

/* Return the mtime of a file, given a 'struct file'.
   Caches the time in the struct file to avoid excess stat calls.

   If the file is not found, and SEARCH is nonzero, VPATH searching and
   replacement is done.  If that fails, a library (-lLIBNAME) is tried and
   the library's actual name (/lib/libLIBNAME.a, etc.) is substituted into
   FILE.  */

FILE_TIMESTAMP
f_mtime (struct file *file, int search)
{
  FILE_TIMESTAMP mtime;
  unsigned int propagate_timestamp;

  /* File's mtime is not known; must get it from the system.  */

#ifndef NO_ARCHIVES
  if (ar_name (file->name))
    {
      /* This file is an archive-member reference.  */

      char *arname, *memname;
      struct file *arfile;
      time_t member_date;

      /* Find the archive's name.  */
      ar_parse_name (file->name, &arname, &memname);

      /* Find the modification time of the archive itself.
         Also allow for its name to be changed via VPATH search.  */
      arfile = lookup_file (arname);
      if (arfile == 0)
        arfile = enter_file (strcache_add (arname));
      mtime = f_mtime (arfile, search);
      check_renamed (arfile);
      if (search && strcmp (arfile->hname, arname))
        {
          /* The archive's name has changed.
             Change the archive-member reference accordingly.  */

          char *name;
          size_t arlen, memlen;

          arlen = strlen (arfile->hname);
          memlen = strlen (memname);

          name = alloca (arlen + 1 + memlen + 2);
          memcpy (name, arfile->hname, arlen);
          name[arlen] = '(';
          memcpy (name + arlen + 1, memname, memlen);
          name[arlen + 1 + memlen] = ')';
          name[arlen + 1 + memlen + 1] = '\0';

          /* If the archive was found with GPATH, make the change permanent;
             otherwise defer it until later.  */
          if (arfile->name == arfile->hname)
            rename_file (file, strcache_add (name));
          else
            rehash_file (file, strcache_add (name));
          check_renamed (file);
        }

      free (arname);

      file->low_resolution_time = 1;

      if (mtime == NONEXISTENT_MTIME)
        /* The archive doesn't exist, so its members don't exist either.  */
        return NONEXISTENT_MTIME;

      member_date = ar_member_date (file->hname);
      mtime = (member_date == (time_t) -1
               ? NONEXISTENT_MTIME
               : file_timestamp_cons (file->hname, member_date, 0));
    }
  else
#endif
    {
      mtime = name_mtime (file->name);

      if (mtime == NONEXISTENT_MTIME && search && !file->ignore_vpath)
        {
          /* If name_mtime failed, search VPATH.  */
          const char *name = vpath_search (file->name, &mtime, NULL, NULL);
          if (name
              /* Last resort, is it a library (-lxxx)?  */
              || (file->name[0] == '-' && file->name[1] == 'l'
                  && (name = library_search (file->name, &mtime)) != 0))
            {
              size_t name_len;

              if (mtime != UNKNOWN_MTIME)
                /* vpath_search and library_search store UNKNOWN_MTIME
                   if they didn't need to do a stat call for their work.  */
                file->last_mtime = mtime;

              /* If we found it in VPATH, see if it's in GPATH too; if so,
                 change the name right now; if not, defer until after the
                 dependencies are updated. */
#ifndef VMS
              name_len = strlen (name) - strlen (file->name) - 1;
#else
              name_len = strlen (name) - strlen (file->name);
              if (name[name_len - 1] == '/')
                  name_len--;
#endif
              if (gpath_search (name, name_len))
                {
                  rename_file (file, name);
                  check_renamed (file);
                  return file_mtime (file);
                }

              rehash_file (file, name);
              check_renamed (file);
              /* If the result of a vpath search is -o or -W, preserve it.
                 Otherwise, find the mtime of the resulting file.  */
              if (mtime != OLD_MTIME && mtime != NEW_MTIME)
                mtime = name_mtime (name);
            }
        }
    }

  /* Files can have bogus timestamps that nothing newly made will be
     "newer" than.  Updating their dependents could just result in loops.
     So notify the user of the anomaly with a warning.

     We only need to do this once, for now. */

  if (!clock_skew_detected
      && mtime != NONEXISTENT_MTIME && mtime != NEW_MTIME
      && !file->updated)
    {
      static FILE_TIMESTAMP adjusted_now;

      FILE_TIMESTAMP adjusted_mtime = mtime;

#if defined(WINDOWS32) || defined(__MSDOS__)
      /* Experimentation has shown that FAT filesystems can set file times
         up to 3 seconds into the future!  Play it safe.  */

#define FAT_ADJ_OFFSET  (FILE_TIMESTAMP) 3

      FILE_TIMESTAMP adjustment = FAT_ADJ_OFFSET << FILE_TIMESTAMP_LO_BITS;
      if (ORDINARY_MTIME_MIN + adjustment <= adjusted_mtime)
        adjusted_mtime -= adjustment;
#elif defined(__EMX__)
      /* FAT filesystems round time to the nearest even second!
         Allow for any file (NTFS or FAT) to perhaps suffer from this
         brain damage.  */
      FILE_TIMESTAMP adjustment = (((FILE_TIMESTAMP_S (adjusted_mtime) & 1) == 0
                     && FILE_TIMESTAMP_NS (adjusted_mtime) == 0)
                    ? (FILE_TIMESTAMP) 1 << FILE_TIMESTAMP_LO_BITS
                    : 0);
#endif

      /* If the file's time appears to be in the future, update our
         concept of the present and try once more.  */
      if (adjusted_now < adjusted_mtime)
        {
          int resolution;
          FILE_TIMESTAMP now = file_timestamp_now (&resolution);
          adjusted_now = now + (resolution - 1);
          if (adjusted_now < adjusted_mtime)
            {
              double from_now =
                (FILE_TIMESTAMP_S (mtime) - FILE_TIMESTAMP_S (now)
                 + ((FILE_TIMESTAMP_NS (mtime) - FILE_TIMESTAMP_NS (now))
                    / 1e9));
              char from_now_string[100];

              if (from_now >= 99 && from_now <= ULONG_MAX)
                sprintf (from_now_string, "%lu", (unsigned long) from_now);
              else
                sprintf (from_now_string, "%.2g", from_now);
              OSS (error, NILF,
                   _("Warning: File '%s' has modification time %s s in the future"),
                   file->name, from_now_string);
              clock_skew_detected = 1;
            }
        }
    }

  /* Store the mtime into all the entries for this file for which it is safe
     to do so: avoid propagating timestamps to double-colon rules that haven't
     been examined so they're run or not based on the pre-update timestamp.  */
  if (file->double_colon)
    file = file->double_colon;

  propagate_timestamp = file->updated;
  do
    {
      /* If this file is not implicit but it is intermediate then it was
         made so by the .INTERMEDIATE target.  If this file has never
         been built by us but was found now, it existed before make
         started.  So, turn off the intermediate bit so make doesn't
         delete it, since it didn't create it.  */
      if (mtime != NONEXISTENT_MTIME && file->command_state == cs_not_started
          && !file->tried_implicit && file->intermediate)
        file->intermediate = 0;

      if (file->updated == propagate_timestamp)
        file->last_mtime = mtime;
      file = file->prev;
    }
  while (file != 0);

  return mtime;
}


/* Return the mtime of the file or archive-member reference NAME.  */

/* First, we check with stat().  If the file does not exist, then we return
   NONEXISTENT_MTIME.  If it does, and the symlink check flag is set, then
   examine each indirection of the symlink and find the newest mtime.
   This causes one duplicate stat() when -L is being used, but the code is
   much cleaner.  */

static FILE_TIMESTAMP
name_mtime (const char *name)
{
  FILE_TIMESTAMP mtime;
  struct stat st;
  int e;

#if defined(WINDOWS32)
  {
    char tem[MAXPATHLEN], *tstart, *tend;
    const char *p = name + strlen (name);

    /* Remove any trailing slashes and "."/"..".  MS-Windows stat
       fails on valid directories if NAME ends in a slash, and we need
       to emulate the Posix behavior where stat on "foo/" or "foo/."
       succeeds ONLY if "foo" is a directory. */
    if (p > name)
      {
        memcpy (tem, name, p - name + 1);
        tstart = tem;
        if (tstart[1] == ':')
          tstart += 2;
        tend = tem + (p - name - 1);
        if (*tend == '.' && tend > tstart)
          tend--;
        if (*tend == '.' && tend > tstart)
          tend--;
        for ( ; tend > tstart && (*tend == '/' || *tend == '\\'); tend--)
          *tend = '\0';
      }
    else
      {
        tem[0] = '\0';
        tend = &tem[0];
      }

    e = stat (tem, &st);
    if (e == 0 && !_S_ISDIR (st.st_mode) && tend < tem + (p - name - 1))
      {
        errno = ENOTDIR;
        e = -1;
      }
  }
#else
  EINTRLOOP (e, stat (name, &st));
#endif
  if (e == 0)
    mtime = FILE_TIMESTAMP_STAT_MODTIME (name, st);
  else if (errno == ENOENT || errno == ENOTDIR)
    mtime = NONEXISTENT_MTIME;
  else
    {
      perror_with_name ("stat: ", name);
      return NONEXISTENT_MTIME;
    }

  /* If we get here we either found it, or it doesn't exist.
     If it doesn't exist see if we can use a symlink mtime instead.  */

#ifdef MAKE_SYMLINKS
#ifndef S_ISLNK
# define S_ISLNK(_m)     (((_m)&S_IFMT)==S_IFLNK)
#endif
  if (check_symlink_flag && strlen (name) <= GET_PATH_MAX)
    {
      PATH_VAR (lpath);

      /* Check each symbolic link segment (if any).  Find the latest mtime
         amongst all of them (and the target file of course).
         Note that we have already successfully dereferenced all the links
         above.  So, if we run into any error trying to lstat(), or
         readlink(), or whatever, something bizarre-o happened.  Just give up
         and use whatever mtime we've already computed at that point.  */
      strcpy (lpath, name);
      while (1)
        {
          FILE_TIMESTAMP ltime;
          PATH_VAR (lbuf);
          long llen;
          char *p;

          EINTRLOOP (e, lstat (lpath, &st));
          if (e)
            {
              /* Just take what we have so far.  */
              if (errno != ENOENT && errno != ENOTDIR)
                perror_with_name ("lstat: ", lpath);
              break;
            }

          /* If this is not a symlink, we're done (we started with the real
             file's mtime so we don't need to test it again).  */
          if (!S_ISLNK (st.st_mode))
            break;

          /* If this mtime is newer than what we had, keep the new one.  */
          ltime = FILE_TIMESTAMP_STAT_MODTIME (lpath, st);
          if (ltime > mtime)
            mtime = ltime;

          /* Set up to check the file pointed to by this link.  */
          EINTRLOOP (llen, readlink (lpath, lbuf, GET_PATH_MAX));
          if (llen < 0)
            {
              /* Eh?  Just take what we have.  */
              perror_with_name ("readlink: ", lpath);
              break;
            }
          lbuf[llen] = '\0';

          /* If the target is fully-qualified or the source is just a
             filename, then the new path is the target.  Otherwise it's the
             source directory plus the target.  */
          if (lbuf[0] == '/' || (p = strrchr (lpath, '/')) == NULL)
            strcpy (lpath, lbuf);
          else if ((p - lpath) + llen + 2 > GET_PATH_MAX)
            /* Eh?  Path too long!  Again, just go with what we have.  */
            break;
          else
            /* Create the next step in the symlink chain.  */
            strcpy (p+1, lbuf);
        }
    }
#endif

  return mtime;
}


/* Search for a library file specified as -lLIBNAME, searching for a
   suitable library file in the system library directories and the VPATH
   directories.  */

static const char *
library_search (const char *lib, FILE_TIMESTAMP *mtime_ptr)
{
  static const char *dirs[] =
    {
#ifndef _AMIGA
      "/lib",
      "/usr/lib",
#endif
#if defined(WINDOWS32) && !defined(LIBDIR)
/*
 * This is completely up to the user at product install time. Just define
 * a placeholder.
 */
#define LIBDIR "."
#endif
      LIBDIR,                   /* Defined by configuration.  */
      0
    };

  const char *file = 0;
  char *libpatterns;
  FILE_TIMESTAMP mtime;

  /* Loop variables for the libpatterns value.  */
  char *p;
  const char *p2;
  size_t len;
  size_t liblen;

  /* Information about the earliest (in the vpath sequence) match.  */
  unsigned int best_vpath = 0, best_path = 0;

  const char **dp;

  libpatterns = xstrdup (variable_expand ("$(.LIBPATTERNS)"));

  /* Skip the '-l'.  */
  lib += 2;
  liblen = strlen (lib);

  /* Loop through all the patterns in .LIBPATTERNS, and search on each one.
     To implement the linker-compatible behavior we have to search through
     all entries in .LIBPATTERNS and choose the "earliest" one.  */
  p2 = libpatterns;
  while ((p = find_next_token (&p2, &len)) != 0)
    {
      static char *buf = NULL;
      static size_t buflen = 0;
      static size_t libdir_maxlen = 0;
      static unsigned int std_dirs = 0;
      char *libbuf = variable_expand ("");

      /* Expand the pattern using LIB as a replacement.  */
      {
        char c = p[len];
        char *p3, *p4;

        p[len] = '\0';
        p3 = find_percent (p);
        if (!p3)
          {
            /* Give a warning if there is no pattern.  */
            OS (error, NILF,
                _(".LIBPATTERNS element '%s' is not a pattern"), p);
            p[len] = c;
            continue;
          }
        p4 = variable_buffer_output (libbuf, p, p3-p);
        p4 = variable_buffer_output (p4, lib, liblen);
        p4 = variable_buffer_output (p4, p3+1, len - (p3-p));
        p[len] = c;
      }

      /* Look first for 'libNAME.a' in the current directory.  */
      mtime = name_mtime (libbuf);
      if (mtime != NONEXISTENT_MTIME)
        {
          if (mtime_ptr != 0)
            *mtime_ptr = mtime;
          file = strcache_add (libbuf);
          /* This by definition will have the best index, so stop now.  */
          break;
        }

      /* Now try VPATH search on that.  */

      {
        unsigned int vpath_index, path_index;
        const char* f = vpath_search (libbuf, mtime_ptr ? &mtime : NULL,
                                      &vpath_index, &path_index);
        if (f)
          {
            /* If we have a better match, record it.  */
            if (file == 0 ||
                vpath_index < best_vpath ||
                (vpath_index == best_vpath && path_index < best_path))
              {
                file = f;
                best_vpath = vpath_index;
                best_path = path_index;

                if (mtime_ptr != 0)
                  *mtime_ptr = mtime;
              }
          }
      }

      /* Now try the standard set of directories.  */

      if (!buflen)
        {
          for (dp = dirs; *dp != 0; ++dp)
            {
              size_t l = strlen (*dp);
              if (l > libdir_maxlen)
                libdir_maxlen = l;
              std_dirs++;
            }
          buflen = strlen (libbuf);
          buf = xmalloc (libdir_maxlen + buflen + 2);
        }
      else if (buflen < strlen (libbuf))
        {
          buflen = strlen (libbuf);
          buf = xrealloc (buf, libdir_maxlen + buflen + 2);
        }

      {
        /* Use the last std_dirs index for standard directories. This
           was it will always be greater than the VPATH index.  */
        unsigned int vpath_index = ~((unsigned int)0) - std_dirs;

        for (dp = dirs; *dp != 0; ++dp)
          {
            sprintf (buf, "%s/%s", *dp, libbuf);
            mtime = name_mtime (buf);
            if (mtime != NONEXISTENT_MTIME)
              {
                if (file == 0 || vpath_index < best_vpath)
                  {
                    file = strcache_add (buf);
                    best_vpath = vpath_index;

                    if (mtime_ptr != 0)
                      *mtime_ptr = mtime;
                  }
              }

            vpath_index++;
          }
      }

    }

  free (libpatterns);
  return file;
}
