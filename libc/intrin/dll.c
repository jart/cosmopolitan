/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/dll.h"

/**
 * @fileoverview Doubly Linked List Library
 * Based on designs used by Mike Burrows and Linus Torvalds.
 */

/**
 * Makes `succ` and its successors come after `elem`.
 *
 * It's required that `elem` and `succ` aren't part of the same list.
 */
void dll_splice_after(struct Dll *elem, struct Dll *succ) {
  struct Dll *tmp1, *tmp2;
  tmp1 = elem->next;
  tmp2 = succ->prev;
  elem->next = succ;
  succ->prev = elem;
  tmp2->next = tmp1;
  tmp1->prev = tmp2;
}

/**
 * Removes item from doubly-linked list.
 *
 * @param list is a doubly-linked list, where `!*list` means empty
 */
void dll_remove(struct Dll **list, struct Dll *elem) {
  if (*list == elem) {
    if ((*list)->prev == *list) {
      *list = 0;
    } else {
      *list = (*list)->prev;
    }
  }
  elem->next->prev = elem->prev;
  elem->prev->next = elem->next;
  elem->next = elem;
  elem->prev = elem;
}

/**
 * Inserts items into list, at the beginning.
 *
 * The resulting list will start with `elem`, followed by other items in
 * `elem`, followed by the items previously in `*list`.
 *
 * @param list is a doubly-linked list, where `!*list` means empty
 * @param elem must not be a member of `list`, or null for no-op
 */
void dll_make_first(struct Dll **list, struct Dll *elem) {
  if (elem) {
    if (!*list) {
      *list = elem->prev;
    } else {
      dll_splice_after(*list, elem);
    }
  }
}

/**
 * Inserts items into list, at the end.
 *
 * The resulting `*list` will end with `elem`, preceded by the other
 * items in `elem`, preceded by the items previously in `*list`.
 *
 * @param list is a doubly-linked list, where `!*list` means empty
 * @param elem must not be a member of `list`, or null for no-op
 */
void dll_make_last(struct Dll **list, struct Dll *elem) {
  if (elem) {
    dll_make_first(list, elem->next);
    *list = elem;
  }
}
