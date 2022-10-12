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
#include "libc/stdio/posix_spawn.h"
#include "libc/stdio/posix_spawn.internal.h"
#include "libc/str/str.h"

static int AddFileAction(posix_spawn_file_actions_t *l,
                         struct _posix_faction a) {
  struct _posix_faction *ap;
  if (!(ap = malloc(sizeof(*ap)))) return ENOMEM;
  a.next = *l;
  *ap = a;
  *l = ap;
  return 0;
}

/**
 * Initializes posix_spawn() file actions list.
 *
 * @param file_actions will need posix_spawn_file_actions_destroy()
 * @return 0 on success, or errno on error
 */
int posix_spawn_file_actions_init(posix_spawn_file_actions_t *file_actions) {
  *file_actions = 0;
  return 0;
}

/**
 * Destroys posix_spawn() file actions list.
 *
 * This function is safe to call multiple times.
 *
 * @param file_actions was initialized by posix_spawn_file_actions_init()
 * @return 0 on success, or errno on error
 */
int posix_spawn_file_actions_destroy(posix_spawn_file_actions_t *file_actions) {
  if (*file_actions) {
    posix_spawn_file_actions_destroy(&(*file_actions)->next);
    free((*file_actions)->path);
    free(*file_actions);
    *file_actions = 0;
  }
  return 0;
}

/**
 * Add a close action to object.
 *
 * @param file_actions was initialized by posix_spawn_file_actions_init()
 * @return 0 on success, or errno on error
 * @raise ENOMEM if we require more vespene gas
 */
int posix_spawn_file_actions_addclose(posix_spawn_file_actions_t *file_actions,
                                      int fildes) {
  return AddFileAction(file_actions, (struct _posix_faction){
                                         .action = _POSIX_SPAWN_CLOSE,
                                         .fildes = fildes,
                                     });
}

/**
 * Add a dup2 action to object.
 *
 * @param file_actions was initialized by posix_spawn_file_actions_init()
 * @return 0 on success, or errno on error
 * @raise ENOMEM if we require more vespene gas
 */
int posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t *file_actions,
                                     int fildes, int newfildes) {
  return AddFileAction(file_actions, (struct _posix_faction){
                                         .action = _POSIX_SPAWN_DUP2,
                                         .fildes = fildes,
                                         .newfildes = newfildes,
                                     });
}

/**
 * Add an open action to object.
 *
 * @param file_actions was initialized by posix_spawn_file_actions_init()
 * @param filedes is what open() result gets duplicated to
 * @param path will be safely copied
 * @return 0 on success, or errno on error
 * @raise ENOMEM if we require more vespene gas
 */
int posix_spawn_file_actions_addopen(posix_spawn_file_actions_t *file_actions,
                                     int fildes, const char *path, int oflag,
                                     unsigned mode) {
  if (!(path = strdup(path))) return ENOMEM;
  return AddFileAction(file_actions, (struct _posix_faction){
                                         .action = _POSIX_SPAWN_OPEN,
                                         .fildes = fildes,
                                         .path = path,
                                         .oflag = oflag,
                                         .mode = mode,
                                     });
}
