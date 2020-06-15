/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/alg/alg.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/dirent.h"
#include "libc/fmt/fmt.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "third_party/editline/editline.h"
#include "third_party/editline/internal.h"

asm(".ident\t\"\\n\\n\
Cosmopolitan Linenoise (BSD-like license)\\n\
Copyright 2019 Justine Alexandra Roberts Tunney\\n\
Copyright 1992,1993 Simmule Turner and Rich Salz\\n\
All rights reserved.\\n\
\\n\
This software is not subject to any license of the American Telephone\\n\
and Telegraph Company or of the Regents of the University of California.\\n\
\\n\
Permission is granted to anyone to use this software for any purpose on\\n\
any computer system, and to alter it and redistribute it freely, subject\\n\
to the following restrictions:\\n\
1. The authors are not responsible for the consequences of use of this\\n\
   software, no matter how awful, even if they arise from flaws in it.\\n\
2. The origin of this software must not be misrepresented, either by\\n\
   explicit claim or by omission.  Since few users ever read sources,\\n\
   credits must appear in the documentation.\\n\
3. Altered versions must be plainly marked as such, and must not be\\n\
   misrepresented as being the original software.  Since few users\\n\
   ever read sources, credits must appear in the documentation.\\n\
4. This notice may not be removed or altered.\"");
asm(".include \"libc/disclaimer.inc\"");

#define MAX_TOTAL_MATCHES (256 << sizeof(char *))

int rl_attempted_completion_over = 0;
rl_completion_func_t *rl_attempted_completion_function = NULL;
rl_compentry_func_t *rl_completion_entry_function = NULL;

/* Wrap strcmp() for qsort() -- weird construct to pass -Wcast-qual */
static int compare(const void *p1, const void *p2) {
  char *const *v1 = (char *const *)p1;
  char *const *v2 = (char *const *)p2;
  return strcmp(*v1, *v2);
}

/* Fill in *avp with an array of names that match file, up to its length.
 * Ignore . and .. . */
static int FindMatches(char *dir, char *file, char ***avp) {
  char **av;
  char **word;
  char *p;
  DIR *dp;
  struct dirent *ep;
  size_t ac;
  size_t len;
  size_t choices;
  size_t total;
  if ((dp = opendir(dir)) == NULL) return 0;
  av = NULL;
  ac = 0;
  len = strlen(file);
  choices = 0;
  total = 0;
  while ((ep = readdir(dp)) != NULL) {
    p = ep->d_name;
    if (p[0] == '.' && (p[1] == '\0' || (p[1] == '.' && p[2] == '\0')))
      continue;
    if (len && strncmp(p, file, len) != 0) continue;
    choices++;
    if ((total += strlen(p)) > MAX_TOTAL_MATCHES) {
      /* This is a bit too much. */
      while (ac > 0) free(av[--ac]);
      continue;
    }
    if ((ac % MEM_INC) == 0) {
      word = malloc(sizeof(char *) * (ac + MEM_INC));
      if (!word) {
        total = 0;
        break;
      }
      if (ac) {
        memcpy(word, av, ac * sizeof(char *));
        free(av);
      }
      *avp = av = word;
    }
    if ((av[ac] = strdup(p)) == NULL) {
      if (ac == 0) free(av);
      total = 0;
      break;
    }
    ac++;
  }
  /* Clean up and return. */
  closedir(dp);
  if (total > MAX_TOTAL_MATCHES) {
    char many[sizeof(total) * 3];
    p = many + sizeof(many);
    *--p = '\0';
    while (choices > 0) {
      *--p = '0' + choices % 10;
      choices /= 10;
    }
    while (p > many + sizeof(many) - 8) *--p = ' ';
    if ((p = strdup(p)) != NULL) av[ac++] = p;
    if ((p = strdup("choices")) != NULL) av[ac++] = p;
  } else {
    if (ac) qsort(av, ac, sizeof(char *), compare);
  }
  return ac;
}

/* Split a pathname into allocated directory and trailing filename parts. */
static int SplitPath(const char *path, char **dirpart, char **filepart) {
  static char DOT[] = ".";
  char *dpart;
  char *fpart;
  if ((fpart = strrchr(path, '/')) == NULL) {
    if ((dpart = strdup(DOT)) == NULL) return -1;
    if ((fpart = strdup(path)) == NULL) {
      free(dpart);
      return -1;
    }
  } else {
    if ((dpart = strdup(path)) == NULL) return -1;
    dpart[fpart - path + 1] = '\0';
    if ((fpart = strdup(fpart + 1)) == NULL) {
      free(dpart);
      return -1;
    }
  }
  *dirpart = dpart;
  *filepart = fpart;
  return 0;
}

static rl_complete_func_t *el_complete_func = NULL;

/* For compatibility with the Heimdal project. */
rl_complete_func_t *rl_set_complete_func(rl_complete_func_t *func) {
  rl_complete_func_t *old = el_complete_func;
  el_complete_func = func;
  return old;
}

/* Attempt to complete the pathname, returning an allocated copy.
 * Fill in *match if we completed it, or set it to 0 if ambiguous. */
char *el_filename_complete(char *pathname, int *match) {
  char **av;
  char *dir;
  char *file;
  char *path;
  char *p;
  size_t ac;
  size_t end;
  size_t i;
  size_t j;
  size_t len;
  if (SplitPath((const char *)pathname, &dir, &file) < 0) return NULL;
  if ((ac = FindMatches(dir, file, &av)) == 0) {
    free(dir);
    free(file);
    return NULL;
  }
  p = NULL;
  len = strlen(file);
  if (ac == 1) {
    /* Exactly one match -- finish it off. */
    *match = 1;
    j = strlen(av[0]) - len + 2;
    p = malloc(sizeof(char) * (j + 1));
    if (p) {
      memcpy(p, av[0] + len, j);
      len = strlen(dir) + strlen(av[0]) + 2;
      path = malloc(sizeof(char) * len);
      if (path) {
        snprintf(path, len, "%s/%s", dir, av[0]);
        rl_add_slash(path, p);
        free(path);
      }
    }
  } else {
    *match = 0;
    if (len) {
      /* Find largest matching substring. */
      for (i = len, end = strlen(av[0]); i < end; i++) {
        for (j = 1; j < ac; j++) {
          if (av[0][i] != av[j][i]) goto breakout;
        }
      }
    breakout:
      if (i > len) {
        j = i - len + 1;
        p = malloc(sizeof(char) * j);
        if (p) {
          memcpy(p, av[0] + len, j);
          p[j - 1] = '\0';
        }
      }
    }
  }
  /* Clean up and return. */
  free(dir);
  free(file);
  for (i = 0; i < ac; i++) free(av[i]);
  free(av);
  return p;
}

char *rl_filename_completion_function(const char *text, int state) {
  char *dir;
  char *file;
  static char **av;
  static size_t i, ac;
  if (!state) {
    if (SplitPath(text, &dir, &file) < 0) return NULL;
    ac = FindMatches(dir, file, &av);
    free(dir);
    free(file);
    if (!ac) return NULL;
    i = 0;
  }
  if (i < ac) return av[i++];
  do {
    free(av[--i]);
  } while (i > 0);
  return NULL;
}

/* Similar to el_find_word(), but used by GNU Readline API */
static char *rl_find_token(size_t *len) {
  char *ptr;
  int pos;
  for (pos = rl_point; pos < rl_end; pos++) {
    if (isspace(rl_line_buffer[pos])) {
      if (pos > 0) pos--;
      break;
    }
  }
  ptr = &rl_line_buffer[pos];
  while (pos >= 0 && !isspace(rl_line_buffer[pos])) {
    if (pos == 0) break;
    pos--;
  }
  if (ptr != &rl_line_buffer[pos]) {
    *len = (size_t)(ptr - &rl_line_buffer[pos]);
    return &rl_line_buffer[pos];
  }
  return NULL;
}

/*
 * "uses an application-supplied generator function to generate the list
 * of possible matches, and then returns the array of these matches. The
 * caller should place the address of its generator function in
 * rl_completion_entry_function"
 */
char **rl_completion_matches(const char *token,
                             rl_compentry_func_t *generator) {
  int state = 0, num = 0;
  char **array, *entry;
  if (!generator) {
    generator = rl_completion_entry_function;
    if (!generator) generator = rl_filename_completion_function;
  }
  if (!generator) return NULL;
  array = malloc(512 * sizeof(char *));
  if (!array) return NULL;
  while (num < 511 && (entry = generator(token, state))) {
    state = 1;
    array[num++] = entry;
  }
  array[num] = NULL;
  if (!num) {
    free(array);
    return NULL;
  }
  return array;
}

static char *complete(char *token, int *match) {
  size_t len = 0;
  char *word, **words = NULL;
  int start, end;
  word = rl_find_token(&len);
  if (!word) goto fallback;
  start = word - rl_line_buffer;
  end = start + len;
  word = strndup(word, len);
  if (!word) goto fallback;
  rl_attempted_completion_over = 0;
  words = rl_attempted_completion_function(word, start, end);
  if (!rl_attempted_completion_over && !words)
    words = rl_completion_matches(word, NULL);
  if (words) {
    int i = 0;
    free(word);
    word = NULL;
    if (words[0]) word = strdup(words[0] + len);
    while (words[i]) free(words[i++]);
    free(words);
    if (word) return word;
  }
fallback:
  return el_filename_complete(token, match);
}

/*
 * First check for editline specific custom completion function, then
 * for any GNU Readline compat, then fallback to filename completion.
 */
char *rl_complete(char *token, int *match) {
  if (el_complete_func) return el_complete_func(token, match);
  if (rl_attempted_completion_function) return complete(token, match);
  return el_filename_complete(token, match);
}

static rl_list_possib_func_t *el_list_possib_func = NULL;

/* For compatibility with the Heimdal project. */
rl_list_possib_func_t *rl_set_list_possib_func(rl_list_possib_func_t *func) {
  rl_list_possib_func_t *old = el_list_possib_func;
  el_list_possib_func = func;
  return old;
}

/* Default possible completions. */
int el_filename_list_possib(char *pathname, char ***av) {
  char *dir;
  char *file;
  int ac;
  if (SplitPath(pathname, &dir, &file) < 0) return 0;
  ac = FindMatches(dir, file, av);
  free(dir);
  free(file);
  return ac;
}

/* Return all possible completions. */
int rl_list_possib(char *token, char ***av) {
  if (el_list_possib_func) return el_list_possib_func(token, av);
  return el_filename_list_possib(token, av);
}
