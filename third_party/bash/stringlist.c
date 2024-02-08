/* stringlist.c - functions to handle a generic `list of strings' structure */

/* Copyright (C) 2000-2019 Free Software Foundation, Inc.

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

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include <stdio.h>
#include "bashansi.h"

#include "shell.h"

#ifdef STRDUP
#  undef STRDUP
#endif
#define STRDUP(x)	((x) ? savestring (x) : (char *)NULL)

/* Allocate a new STRINGLIST, with room for N strings. */

STRINGLIST *
strlist_create (n)
     int n;
{
  STRINGLIST *ret;
  register int i;

  ret = (STRINGLIST *)xmalloc (sizeof (STRINGLIST));
  if (n)
    {
      ret->list = strvec_create (n+1);
      ret->list_size = n;
      for (i = 0; i < n; i++)
	ret->list[i] = (char *)NULL;
    }
  else
    {
      ret->list = (char **)NULL;
      ret->list_size = 0;
    }
  ret->list_len = 0;
  return ret;
}

STRINGLIST *
strlist_resize (sl, n)
     STRINGLIST *sl;
     int n;
{
  register int i;

  if (sl == 0)
    return (sl = strlist_create (n));

  if (n > sl->list_size)
    {
      sl->list = strvec_resize (sl->list, n + 1);
      for (i = sl->list_size; i <= n; i++)
	sl->list[i] = (char *)NULL;
      sl->list_size = n;
    }
  return sl;
}

void
strlist_flush (sl)
     STRINGLIST *sl;
{
  if (sl == 0 || sl->list == 0)
    return;
  strvec_flush (sl->list);
  sl->list_len = 0;
}
  
void
strlist_dispose (sl)
     STRINGLIST *sl;
{
  if (sl == 0)
    return;
  if (sl->list)
    strvec_dispose (sl->list);
  free (sl);
}

int
strlist_remove (sl, s)
     STRINGLIST *sl;
     char *s;
{
  int r;

  if (sl == 0 || sl->list == 0 || sl->list_len == 0)
    return 0;

  r = strvec_remove (sl->list, s);
  if (r)
    sl->list_len--;
  return r;
}

STRINGLIST *
strlist_copy (sl)
     STRINGLIST *sl;
{
  STRINGLIST *new;
  register int i;

  if (sl == 0)
    return ((STRINGLIST *)0);
  new = strlist_create (sl->list_size);
  /* I'd like to use strvec_copy, but that doesn't copy everything. */
  if (sl->list)
    {
      for (i = 0; i < sl->list_size; i++)
	new->list[i] = STRDUP (sl->list[i]);
    }
  new->list_size = sl->list_size;
  new->list_len = sl->list_len;
  /* just being careful */
  if (new->list)
    new->list[new->list_len] = (char *)NULL;
  return new;
}

/* Return a new STRINGLIST with everything from M1 and M2. */

STRINGLIST *
strlist_merge (m1, m2)
     STRINGLIST *m1, *m2;
{
  STRINGLIST *sl;
  int i, n, l1, l2;

  l1 = m1 ? m1->list_len : 0;
  l2 = m2 ? m2->list_len : 0;

  sl = strlist_create (l1 + l2 + 1);
  for (i = n = 0; i < l1; i++, n++)
    sl->list[n] = STRDUP (m1->list[i]);
  for (i = 0; i < l2; i++, n++)
    sl->list[n] = STRDUP (m2->list[i]);
  sl->list_len = n;
  sl->list[n] = (char *)NULL;
  return (sl);
}

/* Make STRINGLIST M1 contain everything in M1 and M2. */
STRINGLIST *
strlist_append (m1, m2)
     STRINGLIST *m1, *m2;
{
  register int i, n, len1, len2;

  if (m1 == 0)
    return (m2 ? strlist_copy (m2) : (STRINGLIST *)0);

  len1 = m1->list_len;
  len2 = m2 ? m2->list_len : 0;

  if (len2)
    {
      m1 = strlist_resize (m1, len1 + len2 + 1);
      for (i = 0, n = len1; i < len2; i++, n++)
	m1->list[n] = STRDUP (m2->list[i]);
      m1->list[n] = (char *)NULL;
      m1->list_len = n;
    }

  return m1;
}

STRINGLIST *
strlist_prefix_suffix (sl, prefix, suffix)
     STRINGLIST *sl;
     char *prefix, *suffix;
{
  int plen, slen, tlen, llen, i;
  char *t;

  if (sl == 0 || sl->list == 0 || sl->list_len == 0)
    return sl;

  plen = STRLEN (prefix);
  slen = STRLEN (suffix);

  if (plen == 0 && slen == 0)
    return (sl);

  for (i = 0; i < sl->list_len; i++)
    {
      llen = STRLEN (sl->list[i]);
      tlen = plen + llen + slen + 1;
      t = (char *)xmalloc (tlen + 1);
      if (plen)
	strcpy (t, prefix);
      strcpy (t + plen, sl->list[i]);
      if (slen)
	strcpy (t + plen + llen, suffix);
      free (sl->list[i]);
      sl->list[i] = t;
    }

  return (sl);	 
}
   
void
strlist_print (sl, prefix)
     STRINGLIST *sl;
     char *prefix;
{
  register int i;

  if (sl == 0)
    return;
  for (i = 0; i < sl->list_len; i++)
    printf ("%s%s\n", prefix ? prefix : "", sl->list[i]);
}

void
strlist_walk (sl, func)
     STRINGLIST *sl;
     sh_strlist_map_func_t *func;
{
  register int i;

  if (sl == 0)
    return;
  for (i = 0; i < sl->list_len; i++)
    if ((*func)(sl->list[i]) < 0)
      break;
} 
     
void
strlist_sort (sl)
     STRINGLIST *sl;
{
  if (sl == 0 || sl->list_len == 0 || sl->list == 0)
    return;
  strvec_sort (sl->list, 0);
}

STRINGLIST *
strlist_from_word_list (list, alloc, starting_index, ip)
     WORD_LIST *list;
     int alloc, starting_index, *ip;
{
  STRINGLIST *ret;
  int slen, len;

  if (list == 0)
    {
      if (ip)
        *ip = 0;
      return ((STRINGLIST *)0);
    }
  slen = list_length (list);
  ret = (STRINGLIST *)xmalloc (sizeof (STRINGLIST));
  ret->list = strvec_from_word_list (list, alloc, starting_index, &len);
  ret->list_size = slen + starting_index;
  ret->list_len = len;
  if (ip)
    *ip = len;
  return ret;
}

WORD_LIST *
strlist_to_word_list (sl, alloc, starting_index)
     STRINGLIST *sl;
     int alloc, starting_index;
{
  WORD_LIST *list;

  if (sl == 0 || sl->list == 0)
    return ((WORD_LIST *)NULL);

  list = strvec_to_word_list (sl->list, alloc, starting_index);
  return list;
}
