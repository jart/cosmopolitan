/* Copyright 2024 Justine Alexandra Roberts Tunney

   Permission to use, copy, modify, and/or distribute this software for
   any purpose with or without fee is hereby granted, provided that the
   above copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
   WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
   AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
   DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
   PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
   TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
   PERFORMANCE OF THIS SOFTWARE.  */

#include "timelog.h"

#include <time.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>

struct timelog
  {
    struct timespec started;
    size_t command_length;
    char command[4000];
  };

char *timelog_path;
int timelog_fd = -1;

void timelog_init (void)
{
  if (!timelog_path)
    return;
  if ((timelog_fd = open (timelog_path, O_APPEND | O_WRONLY | O_CREAT, 0644)) == -1)
    {
      perror (timelog_path);
      exit (1);
    }
}

static int is_shell_safe (int c)
{
  if (c > 127)
    return true;
  if (isalnum(c))
    return true;
  switch (c)
    {
    case '+':
    case '-':
    case '.':
    case '/':
    case '_':
    case '=':
    case ':':
      return true;
    default:
      return false;
    }
}

static int needs_quotes (const char *s)
{
  int c;
  if (!*s)
    return true;
  while ((c = *s++ & 255))
    if (!is_shell_safe (c))
      return true;
  return false;
}

static long long get_microseconds (struct timespec beg, struct timespec end)
{
  end.tv_sec -= beg.tv_sec;
  if (end.tv_nsec < beg.tv_nsec)
    {
      end.tv_nsec += 1000000000;
      end.tv_sec--;
    }
  end.tv_nsec -= beg.tv_nsec;
  return end.tv_sec * 1000000ll + end.tv_nsec / 1000ll;
}

struct timelog *timelog_begin (char **argv)
{
  int i, j, q;
  char *p, *pe;

  /* don't bother if disabled */
  if (timelog_fd == -1)
    return NULL;

  /* allocate object */
  struct timelog *tl;
  if (!(tl = malloc (sizeof (struct timelog))))
    return NULL;

  /* stringify command */
  p = tl->command;
  pe = tl->command + sizeof (tl->command) - 1;
  for (i = 0; argv[i]; ++i)
    {
      if (i && p < pe)
        *p++ = ' ';
      q = needs_quotes (argv[i]);
      if (q && p < pe)
        *p++ = '\'';
      for (j = 0; argv[i][j]; ++j)
        {
          if (isspace(argv[i][j]) ||
              iscntrl(argv[i][j]))
            {
              if (p < pe)
                *p++ = ' ';
            }
          else if (argv[i][j] == '\'')
            {
              if (p < pe)
                *p++ = '\'';
              if (p < pe)
                *p++ = '"';
              if (p < pe)
                *p++ = '\'';
              if (p < pe)
                *p++ = '"';
              if (p < pe)
                *p++ = '\'';
            }
          else
            {
              if (p < pe)
                *p++ = argv[i][j];
            }
        }
      if (q && p < pe)
        *p++ = '\'';
    }
  if (p == pe)
    {
      p[-3] = '.';
      p[-2] = '.';
      p[-1] = '.';
    }
  *p++ = '\n';
  tl->command_length = p - tl->command;

  /* record starting timestamp */
  clock_gettime (CLOCK_REALTIME, &tl->started);

  /* return object */
  return tl;
}

void timelog_end (struct timelog *tl)
{
  long long us;
  char ibuf[22];
  struct iovec iov[2];
  struct timespec ended;

  /* don't bother if disabled */
  if (tl == NULL)
    return;

  /* get elapsed microseconds string */
  clock_gettime (CLOCK_REALTIME, &ended);
  us = get_microseconds (tl->started, ended);
  snprintf (ibuf, sizeof(ibuf), "% 20lld ", us);

  // write to log
  iov[0].iov_base = ibuf;
  iov[0].iov_len = 21;
  iov[1].iov_base = tl->command;
  iov[1].iov_len = tl->command_length;
  writev (timelog_fd, iov, 2);

  // free object
  free (tl);
}
