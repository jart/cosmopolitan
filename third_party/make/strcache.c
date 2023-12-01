/* Constant string caching for GNU Make.
Copyright (C) 2006-2023 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include "makeint.h"

#include <stddef.h>
#include <assert.h>

#include "hash.h"

/* A string cached here will never be freed, so we don't need to worry about
   reference counting.  We just store the string, and then remember it in a
   hash so it can be looked up again. */

typedef unsigned short int sc_buflen_t;

struct strcache {
  struct strcache *next;    /* The next block of strings.  Must be first!  */
  sc_buflen_t end;          /* Offset to the beginning of free space.  */
  sc_buflen_t bytesfree;    /* Free space left in this buffer.  */
  sc_buflen_t count;        /* # of strings in this buffer (for stats).  */
  char buffer[1];           /* The buffer comes after this.  */
};

/* The size (in bytes) of each cache buffer.
   Try to pick something that will map well into the heap.
   This must be able to be represented by a short int (<=65535).  */
#define CACHE_BUFFER_BASE       (8192)
#define CACHE_BUFFER_ALLOC(_s)  ((_s) - (2 * sizeof (size_t)))
#define CACHE_BUFFER_OFFSET     (offsetof (struct strcache, buffer))
#define CACHE_BUFFER_SIZE(_s)   (CACHE_BUFFER_ALLOC(_s) - CACHE_BUFFER_OFFSET)
#define BUFSIZE                 CACHE_BUFFER_SIZE (CACHE_BUFFER_BASE)

static struct strcache *strcache = NULL;
static struct strcache *fullcache = NULL;

static unsigned long total_buffers = 0;
static unsigned long total_strings = 0;
static unsigned long total_size = 0;

/* Add a new buffer to the cache.  Add it at the front to reduce search time.
   This can also increase the overhead, since it's less likely that older
   buffers will be filled in.  However, GNU Make has so many smaller strings
   that this doesn't seem to be much of an issue in practice.
 */
static struct strcache *
new_cache (struct strcache **head, sc_buflen_t buflen)
{
  struct strcache *new = xmalloc (buflen + CACHE_BUFFER_OFFSET);
  new->end = 0;
  new->count = 0;
  new->bytesfree = buflen;

  new->next = *head;
  *head = new;

  ++total_buffers;
  return new;
}

static const char *
copy_string (struct strcache *sp, const char *str, sc_buflen_t len)
{
  /* Add the string to this cache.  */
  char *res = &sp->buffer[sp->end];

  memmove (res, str, len);
  res[len++] = '\0';
  sp->end += len;
  sp->bytesfree -= len;
  ++sp->count;

  return res;
}

static const char *
add_string (const char *str, sc_buflen_t len)
{
  const char *res;
  struct strcache *sp;
  struct strcache **spp = &strcache;
  /* We need space for the nul char.  */
  sc_buflen_t sz = len + 1;

  ++total_strings;
  total_size += sz;

  /* If the string we want is too large to fit into a single buffer, then
     no existing cache is large enough.  Add it directly to the fullcache.  */
  if (sz > BUFSIZE)
    {
      sp = new_cache (&fullcache, sz);
      return copy_string (sp, str, len);
    }

  /* Find the first cache with enough free space.  */
  for (; *spp != NULL; spp = &(*spp)->next)
    if ((*spp)->bytesfree > sz)
      break;
  sp = *spp;

  /* If nothing is big enough, make a new cache at the front.  */
  if (sp == NULL)
    {
      sp = new_cache (&strcache, BUFSIZE);
      spp = &strcache;
    }

  /* Add the string to this cache.  */
  res = copy_string (sp, str, len);

  /* If the amount free in this cache is less than the average string size,
     consider it full and move it to the full list.  */
  if (total_strings > 20 && sp->bytesfree < (total_size / total_strings) + 1)
    {
      *spp = sp->next;
      sp->next = fullcache;
      fullcache = sp;
    }

  return res;
}

/* For strings too large for the strcache, we just save them in a list.  */
struct hugestring {
  struct hugestring *next;  /* The next string.  */
  char buffer[1];           /* The string.  */
};

static struct hugestring *hugestrings = NULL;

static const char *
add_hugestring (const char *str, size_t len)
{
  struct hugestring *new = xmalloc (sizeof (struct hugestring) + len);
  memcpy (new->buffer, str, len);
  new->buffer[len] = '\0';

  new->next = hugestrings;
  hugestrings = new;

  return new->buffer;
}

/* Hash table of strings in the cache.  */

static unsigned long
str_hash_1 (const void *key)
{
  return_ISTRING_HASH_1 ((const char *) key);
}

static unsigned long
str_hash_2 (const void *key)
{
  return_ISTRING_HASH_2 ((const char *) key);
}

static int
str_hash_cmp (const void *x, const void *y)
{
  return_ISTRING_COMPARE ((const char *) x, (const char *) y);
}

static struct hash_table strings;
static unsigned long total_adds = 0;

static const char *
add_hash (const char *str, size_t len)
{
  char *const *slot;
  const char *key;

  /* If it's too large for the string cache, just copy it.
     We don't bother trying to match these.  */
  if (len > USHRT_MAX - 1)
    return add_hugestring (str, len);

  /* Look up the string in the hash.  If it's there, return it.  */
  slot = (char *const *) hash_find_slot (&strings, str);
  key = *slot;

  /* Count the total number of add operations we performed.  */
  ++total_adds;

  if (!HASH_VACANT (key))
    return key;

  /* Not there yet so add it to a buffer, then into the hash table.  */
  key = add_string (str, (sc_buflen_t)len);
  hash_insert_at (&strings, key, slot);
  return key;
}

/* Returns true if the string is in the cache; false if not.  */
int
strcache_iscached (const char *str)
{
  struct strcache *sp;

  for (sp = strcache; sp != 0; sp = sp->next)
    if (str >= sp->buffer && str < sp->buffer + sp->end)
      return 1;
  for (sp = fullcache; sp != 0; sp = sp->next)
    if (str >= sp->buffer && str < sp->buffer + sp->end)
      return 1;

  {
    struct hugestring *hp;
    for (hp = hugestrings; hp != 0; hp = hp->next)
      if (str == hp->buffer)
        return 1;
  }

  return 0;
}

/* If the string is already in the cache, return a pointer to the cached
   version.  If not, add it then return a pointer to the cached version.
   Note we do NOT take control of the string passed in.  */
const char *
strcache_add (const char *str)
{
  return add_hash (str, strlen (str));
}

const char *
strcache_add_len (const char *str, size_t len)
{
  /* If we're not given a nul-terminated string we have to create one, because
     the hashing functions expect it.  */
  if (str[len] != '\0')
    {
      char *key = alloca (len + 1);
      memcpy (key, str, len);
      key[len] = '\0';
      str = key;
    }

  return add_hash (str, len);
}

void
strcache_init (void)
{
  hash_init (&strings, 8000, str_hash_1, str_hash_2, str_hash_cmp);
}


/* Generate some stats output.  */

void
strcache_print_stats (const char *prefix)
{
  const struct strcache *sp;
  unsigned long numbuffs = 0, fullbuffs = 0;
  unsigned long totfree = 0, maxfree = 0, minfree = BUFSIZE;

  if (! strcache)
    {
      printf (_("\n%s No strcache buffers\n"), prefix);
      return;
    }

  /* Count the first buffer separately since it's not full.  */
  for (sp = strcache->next; sp != NULL; sp = sp->next)
    {
      sc_buflen_t bf = sp->bytesfree;

      totfree += bf;
      maxfree = (bf > maxfree ? bf : maxfree);
      minfree = (bf < minfree ? bf : minfree);

      ++numbuffs;
    }
  for (sp = fullcache; sp != NULL; sp = sp->next)
    {
      sc_buflen_t bf = sp->bytesfree;

      totfree += bf;
      maxfree = (bf > maxfree ? bf : maxfree);
      minfree = (bf < minfree ? bf : minfree);

      ++numbuffs;
      ++fullbuffs;
    }

  /* Make sure we didn't lose any buffers.  */
  assert (total_buffers == numbuffs + 1);

  printf (_("\n%s strcache buffers: %lu (%lu) / strings = %lu / storage = %lu B / avg = %lu B\n"),
          prefix, numbuffs + 1, fullbuffs, total_strings, total_size,
          (total_size / total_strings));

  printf (_("%s current buf: size = %hu B / used = %hu B / count = %hu / avg = %u B\n"),
          prefix, (sc_buflen_t)BUFSIZE, strcache->end, strcache->count,
          (unsigned int) (strcache->end / strcache->count));

  if (numbuffs)
    {
      /* Show information about non-current buffers.  */
      unsigned long sz = total_size - strcache->end;
      unsigned long cnt = total_strings - strcache->count;
      sc_buflen_t avgfree = (sc_buflen_t) (totfree / numbuffs);

      printf (_("%s other used: total = %lu B / count = %lu / avg = %lu B\n"),
              prefix, sz, cnt, sz / cnt);

      printf (_("%s other free: total = %lu B / max = %lu B / min = %lu B / avg = %hu B\n"),
              prefix, totfree, maxfree, minfree, avgfree);
    }

  printf (_("\n%s strcache performance: lookups = %lu / hit rate = %lu%%\n"),
          prefix, total_adds, (long unsigned)(100.0 * (total_adds - total_strings) / total_adds));
  fputs (_("# hash-table stats:\n# "), stdout);
  hash_print_stats (&strings, stdout);
}
