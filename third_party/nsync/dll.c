/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│vi: set et ft=c ts=8 tw=8 fenc=utf-8                                       :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2016 Google Inc.                                                   │
│                                                                              │
│ Licensed under the Apache License, Version 2.0 (the "License");              │
│ you may not use this file except in compliance with the License.             │
│ You may obtain a copy of the License at                                      │
│                                                                              │
│     http://www.apache.org/licenses/LICENSE-2.0                               │
│                                                                              │
│ Unless required by applicable law or agreed to in writing, software          │
│ distributed under the License is distributed on an "AS IS" BASIS,            │
│ WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.     │
│ See the License for the specific language governing permissions and          │
│ limitations under the License.                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/nsync/dll.h"

asm(".ident\t\"\\n\\n\
*NSYNC (Apache 2.0)\\n\
Copyright 2016 Google, Inc.\\n\
https://github.com/google/nsync\"");
// clang-format off

/* Initialize *e. */
void nsync_dll_init_ (nsync_dll_element_ *e, void *container) {
	e->next = e;
	e->prev = e;
	e->container = container;
}

/* Return whether list is empty. */
int nsync_dll_is_empty_ (nsync_dll_list_ list) {
	return (list == NULL);
}

/* Remove *e from list, and returns the new list. */
nsync_dll_list_ nsync_dll_remove_ (nsync_dll_list_ list, nsync_dll_element_ *e) {
	if (list == e) { /* removing tail of list */
		if (list->prev == list) {
			list = NULL; /* removing only element of list */
		} else {
			list = list->prev;
		}
	}
	e->next->prev = e->prev;
	e->prev->next = e->next;
	e->next = e;
	e->prev = e;
	return (list);
}

/* Cause element *n and its successors to come after element *p.
   Requires n and p are non-NULL and do not point at elements of the same list.

   Unlike the other operations in this API, this operation acts on
   two circular lists of elements, rather than on a "head" location that points
   to such a circular list.

   If the two lists are p->p_2nd->p_mid->p_last->p and n->n_2nd->n_mid->n_last->n,
   then after nsync_dll_splice_after_ (p, n), the p list would be:
   p->n->n_2nd->n_mid->n_last->p_2nd->p_mid->p_last->p.  */
void nsync_dll_splice_after_ (nsync_dll_element_ *p, nsync_dll_element_ *n) {
        nsync_dll_element_ *p_2nd = p->next;
        nsync_dll_element_ *n_last = n->prev;
        p->next = n;  /* n follows p */
        n->prev = p;
	n_last->next = p_2nd;  /* remainder of p-list follows last of n-list */
	p_2nd->prev = n_last;
}

/* Make element *e the first element of list, and return
   the list.  The resulting list will have *e as its first element, followed by
   any elements in the same list as *e, followed by the elements that were
   previously in list.  Requires that *e not be in list.  If e==NULL, list is
   returned unchanged.

   Suppose the e list is e->e_2nd->e_mid->e_last->e.
   Recall that a head "list" points to the last element of its list.
   If list is initially null, then the outcome is:
	result = e_last->e->e_2nd->e_mid->e_last
   If list is  initially list->list_last->list_1st->list_mid->list_last,
   then the outcome is:
	result = list_last->e->e_2nd->e_mid->e_last->list_1st->list_mid->list_last
   */
nsync_dll_list_ nsync_dll_make_first_in_list_ (nsync_dll_list_ list, nsync_dll_element_ *e) {
	if (e != NULL) {
		if (list == NULL) {
			list = e->prev;   /*e->prev is e_last*/
		} else {
			nsync_dll_splice_after_ (list, e);
		}
	}
	return (list);
}

/* Make element *e the last element of list, and return
   the list.  The resulting list will have *e as its last element, preceded by
   any elements in the same list as *e, preceded by the elements that were
   previously in list.  Requires that *e not be in list.  If e==NULL, list is
   returned unchanged. */
nsync_dll_list_ nsync_dll_make_last_in_list_ (nsync_dll_list_ list, nsync_dll_element_ *e) {
	if (e != NULL) {
		nsync_dll_make_first_in_list_ (list, e->next);
		list = e;
	}
	return (list);
}

/* Return a pointer to the first element of list, or NULL if list is empty. */
nsync_dll_element_ *nsync_dll_first_ (nsync_dll_list_ list) {
	nsync_dll_element_ *first = NULL;
	if (list != NULL) {
		first = list->next;
	}
	return (first);
}

/* Return a pointer to the last element of list, or NULL if list is empty. */
nsync_dll_element_ *nsync_dll_last_ (nsync_dll_list_ list) {
	return (list);
}

/* Return a pointer to the next element of list following *e,
   or NULL if there is no such element. */
nsync_dll_element_ *nsync_dll_next_ (nsync_dll_list_ list, nsync_dll_element_ *e) {
	nsync_dll_element_ *next = NULL;
	if (e != list) {
		next = e->next;
	}
	return (next);
}

/* Return a pointer to the previous element of list following *e,
   or NULL if there is no such element. */
nsync_dll_element_ *nsync_dll_prev_ (nsync_dll_list_ list, nsync_dll_element_ *e) {
	nsync_dll_element_ *prev = NULL;
	if (e != list->next) {
		prev = e->prev;
	}
	return (prev);
}
