/* dispose_command.c -- dispose of a COMMAND structure. */

/* Copyright (C) 1987-2009 Free Software Foundation, Inc.

   This file is part of GNU Bash, the Bourne Again SHell.

   Bash is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Bash is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Bash.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "config.h"

#include "bashtypes.h"

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include "bashansi.h"
#include "shell.h"

extern sh_obj_cache_t wdcache, wlcache;

/* Dispose of the command structure passed. */
void
dispose_command (command)
     COMMAND *command;
{
  if (command == 0)
    return;

  if (command->redirects)
    dispose_redirects (command->redirects);

  switch (command->type)
    {
    case cm_for:
#if defined (SELECT_COMMAND)
    case cm_select:
#endif
      {
	register FOR_COM *c;
#if defined (SELECT_COMMAND)
	if (command->type == cm_select)
	  c = (FOR_COM *)command->value.Select;
	else
#endif
	c = command->value.For;
	dispose_word (c->name);
	dispose_words (c->map_list);
	dispose_command (c->action);
	free (c);
	break;
      }

#if defined (ARITH_FOR_COMMAND)
    case cm_arith_for:
      {
	register ARITH_FOR_COM *c;

	c = command->value.ArithFor;
	dispose_words (c->init);
	dispose_words (c->test);
	dispose_words (c->step);
	dispose_command (c->action);
	free (c);
	break;
      }
#endif /* ARITH_FOR_COMMAND */

    case cm_group:
      {
	dispose_command (command->value.Group->command);
	free (command->value.Group);
	break;
      }

    case cm_subshell:
      {
	dispose_command (command->value.Subshell->command);
	free (command->value.Subshell);
	break;
      }

    case cm_coproc:
      {
	free (command->value.Coproc->name);
	dispose_command (command->value.Coproc->command);
	free (command->value.Coproc);
	break;
      }

    case cm_case:
      {
	register CASE_COM *c;
	PATTERN_LIST *t, *p;

	c = command->value.Case;
	dispose_word (c->word);

	for (p = c->clauses; p; )
	  {
	    dispose_words (p->patterns);
	    dispose_command (p->action);
	    t = p;
	    p = p->next;
	    free (t);
	  }
	free (c);
	break;
      }

    case cm_until:
    case cm_while:
      {
	register WHILE_COM *c;

	c = command->value.While;
	dispose_command (c->test);
	dispose_command (c->action);
	free (c);
	break;
      }

    case cm_if:
      {
	register IF_COM *c;

	c = command->value.If;
	dispose_command (c->test);
	dispose_command (c->true_case);
	dispose_command (c->false_case);
	free (c);
	break;
      }

    case cm_simple:
      {
	register SIMPLE_COM *c;

	c = command->value.Simple;
	dispose_words (c->words);
	dispose_redirects (c->redirects);
	free (c);
	break;
      }

    case cm_connection:
      {
	register CONNECTION *c;

	c = command->value.Connection;
	dispose_command (c->first);
	dispose_command (c->second);
	free (c);
	break;
      }

#if defined (DPAREN_ARITHMETIC)
    case cm_arith:
      {
	register ARITH_COM *c;

	c = command->value.Arith;
	dispose_words (c->exp);
	free (c);
	break;
      }
#endif /* DPAREN_ARITHMETIC */

#if defined (COND_COMMAND)
    case cm_cond:
      {
	register COND_COM *c;

	c = command->value.Cond;
	dispose_cond_node (c);
	break;
      }
#endif /* COND_COMMAND */

    case cm_function_def:
      {
	register FUNCTION_DEF *c;

	c = command->value.Function_def;
	dispose_function_def (c);
	break;
      }

    default:
      command_error ("dispose_command", CMDERR_BADTYPE, command->type, 0);
      break;
    }
  free (command);
}

#if defined (COND_COMMAND)
/* How to free a node in a conditional command. */
void
dispose_cond_node (cond)
     COND_COM *cond;
{
  if (cond)
    {
      if (cond->left)
	dispose_cond_node (cond->left);
      if (cond->right)
	dispose_cond_node (cond->right);
      if (cond->op)
	dispose_word (cond->op);
      free (cond);
    }
}
#endif /* COND_COMMAND */

void
dispose_function_def_contents (c)
     FUNCTION_DEF *c;
{
  dispose_word (c->name);
  dispose_command (c->command);
  FREE (c->source_file);
}

void
dispose_function_def (c)
     FUNCTION_DEF *c;
{
  dispose_function_def_contents (c);
  free (c);
}

/* How to free a WORD_DESC. */
void
dispose_word (w)
     WORD_DESC *w;
{
  FREE (w->word);
  ocache_free (wdcache, WORD_DESC, w);
}

/* Free a WORD_DESC, but not the word contained within. */
void
dispose_word_desc (w)
     WORD_DESC *w;
{
  w->word = 0;
  ocache_free (wdcache, WORD_DESC, w);
}

/* How to get rid of a linked list of words.  A WORD_LIST. */
void
dispose_words (list)
     WORD_LIST *list;
{
  WORD_LIST *t;

  while (list)
    {
      t = list;
      list = list->next;
      dispose_word (t->word);
#if 0
      free (t);
#else
      ocache_free (wlcache, WORD_LIST, t);
#endif
    }
}

#ifdef INCLUDE_UNUSED
/* How to dispose of an array of pointers to char.  This is identical to
   free_array in stringlib.c. */
void
dispose_word_array (array)
     char **array;
{
  register int count;

  if (array == 0)
    return;

  for (count = 0; array[count]; count++)
    free (array[count]);

  free (array);
}
#endif

/* How to dispose of an list of redirections.  A REDIRECT. */
void
dispose_redirects (list)
     REDIRECT *list;
{
  register REDIRECT *t;

  while (list)
    {
      t = list;
      list = list->next;

      if (t->rflags & REDIR_VARASSIGN)
	dispose_word (t->redirector.filename);

      switch (t->instruction)
	{
	case r_reading_until:
	case r_deblank_reading_until:
	  free (t->here_doc_eof);
	/*FALLTHROUGH*/
	case r_reading_string:
	case r_output_direction:
	case r_input_direction:
	case r_inputa_direction:
	case r_appending_to:
	case r_err_and_out:
	case r_append_err_and_out:
	case r_input_output:
	case r_output_force:
	case r_duplicating_input_word:
	case r_duplicating_output_word:
	case r_move_input_word:
	case r_move_output_word:
	  dispose_word (t->redirectee.filename);
	  /* FALLTHROUGH */
	default:
	  break;
	}
      free (t);
    }
}
