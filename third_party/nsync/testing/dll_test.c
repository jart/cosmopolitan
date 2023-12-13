/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
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
#include "libc/intrin/dll.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "third_party/nsync/array.internal.h"
#include "third_party/nsync/testing/smprintf.h"
#include "third_party/nsync/testing/testing.h"

/* This tests internal abstractions. */

typedef A_TYPE (int) a_int;  /* an array of 32-bit integers */
static const a_int a_int_empty = A_EMPTY;  /* the empty array */

/* Append the integers in the argument list to *a, until the first negative one is found. */
static a_int *a_set (a_int *a, ...) {
	va_list ap;
	int x;
	A_SET_LEN (a, 0);
	va_start (ap, a);
	for (x = va_arg (ap, int); x >= 0; x = va_arg (ap, int)) {
		A_PUSH (a) = x;
	}
	va_end (ap);
	return (a);
}

/* Remove the first element from *a. Requires that *a be non-empty. */
static void a_remove_first (a_int *a) {
	int len = A_LEN (a);
	if (len == 0) {
		*(volatile int *)0 = 0;
	} else {
		memmove (&A (a, 0), &A (a, 1), sizeof (A (a, 0)) * (len - 1));
		A_SET_LEN (a, len-1);
	}
}


/* Return a malloced, nul-terminated string representation of the elements of *a. */
static char *a_string (const a_int *a) {
	int m = A_LEN (a) * 3 + 3;
	int n = 0;
	char *buf = (char *) malloc (m);
	char single[32];
	int i;
	snprintf (buf+n, m-n, "[");
	n = strlen (buf);
	for (i = 0; i != A_LEN (a); i++) {
		int len;
		snprintf (single, sizeof (single), "%s%lu", i == 0? "": " ",
			  (unsigned long)A (a, i));
		len = strlen (single);
		if (m < n + len + 2) {
			buf = (char *) realloc (buf, m *= 2);
		}
		snprintf (buf + n, m-n, "%s", single);
		n += len;
	}
	snprintf (buf+n, m-n, "]");
	return (buf);
}

/* A list item for use in the tests below */
struct list_item_s {
	struct Dll e;
	int i;
};
/* Return a pointer to the struct list_item_s containing struct Dll *e_. */
#define LIST_ITEM(e_)  DLL_CONTAINER(struct list_item_s, e, e_)


/* Check that list l contains elements containing the values in
   expected, by scanning both forwards and backwards through the list.  Also
   verify that dll_first() and dll_last() return the first and last element
   found during those iterations, and that dll_is_empty() yields the right value. */
static void verify_list (testing t, const char *label, struct Dll *l,
			 const a_int *expected, const char *file, int line) {
	struct Dll *first;
	struct Dll *last = NULL;
	struct Dll *p;
	int i = 0;
	char *expected_str = a_string (expected);
	for (p = dll_first (l); p != NULL; p = dll_next (l, p)) {
		if (A (expected, i) != LIST_ITEM (p)->i) {
			TEST_ERROR (t, ("%s:%d; %s:expected=%s: expected %d as "
				   "value %d in list, but found %d\n",
				   file, line, label, expected_str,
				   A (expected, i), i, LIST_ITEM (p)->i));
		}
		last = p;
		i++;
	}
	if (last != dll_last (l)) {
		TEST_ERROR (t, ("%s:%d: %s:expected=%s:  expected %p as "
			   "last item in list, but found %p\n",
			   file, line, label, expected_str, last, dll_last (l)));
	}
	if (i != A_LEN (expected)) {
		TEST_ERROR (t, ("%s:%d: %s:expected=%s:  expected %d items in "
			   "list, but found %d\n",
			   file, line, label, expected_str, A_LEN (expected), i));
	}

	first = NULL;
	for (p = dll_last (l); p != NULL; p = dll_prev (l, p)) {
		i--;
		if (A (expected, i) != LIST_ITEM (p)->i) {
			TEST_ERROR (t, ("%s:%d: %s:expected=%s:  expected %d as "
				   "value %d in reverse list, but found %d\n",
				   file, line, label, expected_str,
				   A (expected, i), i, LIST_ITEM (p)->i));
		}
		first = p;
	}
	if (first != dll_first (l)) {
		TEST_ERROR (t, ("%s:%d: %s:expected=%s:  expected %p as "
			   "first item in list, but found %p\n",
			   file, line, label, expected_str, first, dll_last (l)));
	}
	if (i != 0) {
		TEST_ERROR (t, ("%s:%d: %s:expected=%s:  expected %d items "
			   "in reverse list, but found %d\n",
			   file, line, label, expected_str,
			   A_LEN (expected), A_LEN (expected)-i));
	}

	if ((A_LEN (expected) == 0) != dll_is_empty (l)) {
		TEST_ERROR (t, ("%s:%d: %s:expected=%s:  expected dll_is_empty() "
			   "to yield %d but got %d\n",
			   file, line, label, expected_str,
			   (A_LEN (expected) == 0), dll_is_empty (l)));
	}
	free (expected_str);
}

/* Return a new list containing the count integers from start to
   start+count-1 by appending successive elements to the list.
   This exercises dll_make_last() using singleton elements. */
static struct Dll *make_list (int start, int count) {
	struct Dll *l = NULL;
	int i;
	for (i = start; i != start+count; i++) {
		struct list_item_s *item =
			(struct list_item_s *) malloc (sizeof (*item));
		dll_init (&item->e);
		item->i = i;
		dll_make_last (&l, &item->e);
	}
	return (l);
}

/* Return a new list containing the count integers from start to
   start+count-1 by prefixing the list with elements, starting with the last.
   It exercises dll_make_first() using singleton elements. */
static struct Dll *make_rlist (int start, int count) {
	struct Dll *l = NULL;
	int i;
	for (i = start + count - 1; i != start-1; i--) {
		struct list_item_s *item =
			(struct list_item_s *) malloc (sizeof (*item));
		dll_init (&item->e);
		item->i = i;
		dll_make_first (&l, &item->e);
	}
	return (l);
}

/* Test the functionality of the various doubly-linked list
   operations internal to the nsync_mu implementation. */
static void test_dll (testing t) {
	int i;
	a_int expected;
	struct list_item_s *item;

	struct Dll *empty = NULL;
	struct Dll *list = NULL;

	struct Dll *x10 = NULL;
	struct Dll *x20 = NULL;
	struct Dll *x30 = NULL;
	struct Dll *x40 = NULL;
	struct Dll *x50 = NULL;

	bzero (&expected, sizeof (expected));

	/* All lists are initially empty. */
	verify_list (t, "empty (0)", empty, &a_int_empty, __FILE__, __LINE__);
	verify_list (t, "list (0)", list, &a_int_empty, __FILE__, __LINE__);
	verify_list (t, "x10", x10, &a_int_empty, __FILE__, __LINE__);
	verify_list (t, "x20", x20, &a_int_empty, __FILE__, __LINE__);
	verify_list (t, "x30", x30, &a_int_empty, __FILE__, __LINE__);
	verify_list (t, "x40", x40, &a_int_empty, __FILE__, __LINE__);
	verify_list (t, "x50", x50, &a_int_empty, __FILE__, __LINE__);

	/* Make the xN list have the values N, N+1, N+2. */
	x10 = make_list (10, 3);
	verify_list (t, "x10", x10, a_set (&expected, 10, 11, 12, -1), __FILE__, __LINE__);
	x20 = make_rlist (20, 3);
	verify_list (t, "x20", x20, a_set (&expected, 20, 21, 22, -1), __FILE__, __LINE__);
	x30 = make_list (30, 3);
	verify_list (t, "x30", x30, a_set (&expected, 30, 31, 32, -1), __FILE__, __LINE__);
	x40 = make_list (40, 3);
	verify_list (t, "x40", x40, a_set (&expected, 40, 41, 42, -1), __FILE__, __LINE__);
	x50 = make_list (50, 3);
	verify_list (t, "x50", x50, a_set (&expected, 50, 51, 52, -1), __FILE__, __LINE__);

	/* Check that adding nothing to an empty list leaves it empty. */
	dll_make_first (&list, NULL);
	verify_list (t, "list(1)", list, &a_int_empty, __FILE__, __LINE__);
	dll_make_first (&list, dll_first (empty));
	verify_list (t, "list(2)", list, &a_int_empty, __FILE__, __LINE__);
	dll_make_first (&list, dll_last (empty));
	verify_list (t, "list(3)", list, &a_int_empty, __FILE__, __LINE__);

	/* Prefix an empty list with some elements. */
	dll_make_first (&list, dll_first (x10));
	verify_list (t, "list(4)", list, a_set (&expected, 10, 11, 12, -1),
		     __FILE__, __LINE__);

	/* Check that adding nothing no a non-empty list leaves it unchanged. */
	dll_make_first (&list, NULL);
	verify_list (t, "list(5)", list, a_set (&expected, 10, 11, 12, -1),
		     __FILE__, __LINE__);
	dll_make_first (&list, dll_first (empty));
	verify_list (t, "list(6)", list, a_set (&expected, 10, 11, 12, -1),
		     __FILE__, __LINE__);
	dll_make_first (&list, dll_last (empty));
	verify_list (t, "list(7)", list, a_set (&expected, 10, 11, 12, -1),
		     __FILE__, __LINE__);

	/* Check prefixing the list with some elements. */
	dll_make_first (&list, dll_first (x20));
	verify_list (t, "list(8)", list,
		     a_set (&expected, 20, 21, 22, 10, 11, 12, -1),
		     __FILE__, __LINE__);

	/* Check appending elements to list. */
	dll_make_last (&list, dll_last (x30));
	verify_list (t, "list(9)", list,
		     a_set (&expected, 20, 21, 22, 10, 11, 12, 30, 31, 32, -1),
		     __FILE__, __LINE__);

	/* Remove the first element. */
	item = (struct list_item_s *) LIST_ITEM (dll_first (list));
	dll_remove (&list, &item->e);
	verify_list (t, "list(10)", list,
		     a_set (&expected, 21, 22, 10, 11, 12, 30, 31, 32, -1),
		     __FILE__, __LINE__);
	free (item);

	/* Remove the last element. */
	item = (struct list_item_s *) LIST_ITEM (dll_last (list));
	dll_remove (&list, &item->e);
	verify_list (t, "list(11)", list,
		     a_set (&expected, 21, 22, 10, 11, 12, 30, 31, -1),
		     __FILE__, __LINE__);
	free (item);

	/* Remove the third element. */
	item = LIST_ITEM (dll_next (list, dll_next (list, dll_first (list))));
	dll_remove (&list, &item->e);
	verify_list (t, "list(12)",
		     list, a_set (&expected, 21, 22, 11, 12, 30, 31, -1),
		     __FILE__, __LINE__);
	free (item);

	/* Remove all elements. */
	a_set (&expected, 21, 22, 11, 12, 30, 31, -1);
	for (i = 0; !dll_is_empty (list); i++) {
		char buf[32];
		item = LIST_ITEM (dll_first (list));
		dll_remove (&list, &item->e);
		a_remove_first (&expected);
		snprintf (buf, sizeof (buf), "list(13.%d)", i);
		verify_list (t, buf, list, &expected, __FILE__, __LINE__);
		free (item);
	}
	verify_list (t, "list(14)", list, &a_int_empty, __FILE__, __LINE__);

	/* Append some elements to an empty list. */
	dll_make_last (&list, dll_last (x40));
	verify_list (t, "list(15)", list,
		     a_set (&expected, 40, 41, 42, -1), __FILE__, __LINE__);

	/* Use dll_splice_after() to put {50, 51, 52} just after 41, which is
	   next (first (list)). */
	dll_splice_after (dll_next (list, dll_first (list)), dll_first (x50));
	verify_list (t, "list(16)", list,
		     a_set (&expected, 40, 41, 50, 51, 52, 42, -1),
		     __FILE__, __LINE__);

	A_FREE (&expected);

	while (!dll_is_empty (list)) {
		item = LIST_ITEM (dll_first (list));
		dll_remove (&list, &item->e);
		free (item);
	}
}

int main (int argc, char *argv[]) {
	testing_base tb = testing_new (argc, argv, 0);
	TEST_RUN (tb, test_dll);
	return (testing_base_exit (tb));
}
