/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/mem/mem.h"
#include "libc/stdio/spawn.h"
#include "libc/str/str.h"

/**
 * Creates object with no actions.
 */
int posix_spawn_file_actions_init(posix_spawn_file_actions_t *file_actions) {
  *file_actions = malloc(sizeof(char));
  if (!*file_actions) return ENOMEM;
  strcpy(*file_actions, "");
  return 0;
}

/**
 * Frees object storage and make invalid.
 */
int posix_spawn_file_actions_destroy(posix_spawn_file_actions_t *file_actions) {
  free(*file_actions);
  *file_actions = NULL;
  return 0;
}

/**
 * Adds new action string to object.
 */
static int add_to_file_actions(posix_spawn_file_actions_t *file_actions,
                               char *new_action) {
  *file_actions =
      realloc(*file_actions, strlen(*file_actions) + strlen(new_action) + 1);
  if (!*file_actions) return ENOMEM;
  strcat(*file_actions, new_action);
  return 0;
}

/**
 * Add a close action to object.
 */
int posix_spawn_file_actions_addclose(posix_spawn_file_actions_t *file_actions,
                                      int fildes) {
  char temp[100];
  (sprintf)(temp, "close(%d)", fildes);
  return add_to_file_actions(file_actions, temp);
}

/**
 * Add a dup2 action to object.
 */
int posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t *file_actions,
                                     int fildes, int newfildes) {
  char temp[100];
  (sprintf)(temp, "dup2(%d,%d)", fildes, newfildes);
  return add_to_file_actions(file_actions, temp);
}

/**
 * Add an open action to object.
 */
int posix_spawn_file_actions_addopen(posix_spawn_file_actions_t *file_actions,
                                     int fildes, const char *path, int oflag,
                                     unsigned mode) {
  char temp[100];
  (sprintf)(temp, "open(%d,%s*%o,%o)", fildes, path, oflag, mode);
  return add_to_file_actions(file_actions, temp);
}
