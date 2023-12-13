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
#include "third_party/nsync/atomic.h"
#include "third_party/nsync/common.internal.h"
#include "third_party/nsync/mu_semaphore.h"
#include "third_party/nsync/races.internal.h"
#include "third_party/nsync/wait_s.internal.h"

asm(".ident\t\"\\n\\n\
*NSYNC (Apache 2.0)\\n\
Copyright 2016 Google, Inc.\\n\
https://github.com/google/nsync\"");

/* Routines for debugging. */

/* An emit_buf represents a buffer into which debug information can
   be written.  */
struct emit_buf {
        char *start;  /* start of buffer */
        int len;      /* pength of buffer */
        int pos;      /* position of next character to bve written */
        int overflow; /* non-zero iff buffer overflow has occurred */
};

/* Initialize *b to point to start[0, .., len-1], and return b.
   of to an internal static buffer if buf==NULL. */
static struct emit_buf *emit_init (struct emit_buf *b, char *start, int len) {
        b->start = start;
        b->len = len;
        b->pos = 0;
        b->overflow = 0;
        return (b);
}


/* Write character c to buffer *b. */
static void emit_c (struct emit_buf *b, int c) {
        if (b->pos < b->len) {
                b->start[b->pos++] = c;
        } else if (!b->overflow) {
                static const char suffix[] = "...";
                const char *s = &suffix[sizeof (suffix)]; /* past nul */
                char *p = &b->start[b->len];  /* past end */
                while (s > suffix && p > b->start) {
                        *--p = *--s;
                }
                b->overflow = 1;
        }
}

/* A printf-like function that writes to an emit_buf.
   It understands only the format specifiers %s (const char *), and %i
   (uintptr_t in hex), with no modifiers. */
static void emit_print (struct emit_buf *b, const char *fmt, ...) {
        va_list ap;
        va_start (ap, fmt);
        while (*fmt != 0) {
                int c = *fmt++;
                if (c != '%') {
                        emit_c (b, c);
                } else {
                        c = *fmt++;
                        if (c == 's') {
                                const char *s = va_arg (ap, const char *);
                                while (*s != 0) {
                                        emit_c (b, *s++);
                                }
                        } else if (c == 'i') {
                                uintptr_t n = va_arg (ap, uintptr_t);
                                int i;
                                for (i = 0; (n >> i) >= 0x10; i += 4) {
                                }
                                for (; i >= 0; i -= 4) {
                                        emit_c (b, "0123456789abcdef"[(n >> i) & 0xf]);
                                }
                        } else {
                                ASSERT (0);
                        }
                }
        }
        va_end (ap);
}

/* Map a bit in a uint32_t to a human-readable name. */
struct bit_name {
        uint32_t mask;
        const char *name;
};

/* names for bits in a mu word */
static const struct bit_name mu_bit[] = {
        { MU_WLOCK,          "wlock" },
        { MU_SPINLOCK,       "spin" },
        { MU_WAITING,        "wait" },
        { MU_DESIG_WAKER,    "desig" },
        { MU_CONDITION,      "cond" },
        { MU_WRITER_WAITING, "writer" },
        { MU_LONG_WAIT,      "long" },
        { MU_ALL_FALSE,      "false" },
        { 0,                 "" }  /* sentinel */
};

/* names for bits in a cv word */
static const struct bit_name cv_bit[] = {
        { CV_SPINLOCK,       "spin" },
        { CV_NON_EMPTY,      "wait" },
        { 0,                 "" }  /* sentinel */
};

/* names for bits in a waiter flags word */
static const struct bit_name waiter_flags_bit[] = {
        { WAITER_RESERVED,   "rsrvd" },
        { WAITER_IN_USE,     "in_use" },
        { 0,                 "" }  /* sentinel */
};

/* Emit the names of bits in word to buffer *b using names[] */
static void emit_word (struct emit_buf *b, const struct bit_name *name, uint32_t word) {
      int i;
      for (i = 0; name[i].mask != 0; i++) {
              if ((word & name[i].mask) != 0) {
                      emit_print (b, " %s", name[i].name);
              }
      }
}

/* Emit the waiter queue *q to *b. */
static void emit_waiters (struct emit_buf *b, struct Dll *list) {
        struct Dll *p = dll_first (list);
        struct Dll *next;
        if (p != NULL) {
                emit_print (b, "\nwaiters =\n");
        }
        for (; p != NULL && !b->overflow; p = next) {
                struct nsync_waiter_s *nw = DLL_NSYNC_WAITER (p);
                waiter *w = DLL_WAITER (p);
                next = NULL;
                emit_print (b, "   %i", (uintptr_t) w);
                if (w->tag != WAITER_TAG) {
                        emit_print (b, "bad WAITER_TAG %i",
                                    (uintptr_t) w->tag);
                } else {
                        next = dll_next (list, p);
                        if (nw->tag != NSYNC_WAITER_TAG) {
                                emit_print (b, " bad WAITER_TAG %i",
                                            (uintptr_t) nw->tag);
                        } else {
                                emit_print (b, " embedded=%i waiting=%i",
                                            (uintptr_t) (w->flags & NSYNC_WAITER_FLAG_MUCV),
                                            (uintptr_t) ATM_LOAD (&nw->waiting));
                        }
                        emit_word (b, waiter_flags_bit, w->flags);
                        emit_print (b, " %s removes=%i cond=(%i %i %i)",
                                    w->l_type == nsync_writer_type_? "writer" :
                                    w->l_type == nsync_reader_type_? "reader" :
                                                                     "??????",
                                    (uintptr_t) ATM_LOAD (&w->remove_count),
                                    (uintptr_t) w->cond.f,
                                    (uintptr_t) w->cond.v,
                                    (uintptr_t) w->cond.eq);
                        if (w->same_condition.next != &w->same_condition) {
                                emit_print (b, " same_as %i",
                                            (uintptr_t) DLL_WAITER_SAMECOND (
                                                w->same_condition.next));
                        }
                }
                emit_c (b, '\n');
        }
}

/* Emit to *b the state of *mu, and return a pointer to *b's buffer.
  
   If blocking!=0, print_waiters!=0, and *mu's waiter list is non-empty, the
   call will block until it can acquire the spinlock.
   If print_waiters!=0, the waiter list is printed.
   The spinlock is released before return if it was acquired.
   blocking==0 && print_waiters!=0 is unsafe and is intended for use within
   interactive debuggers.  */
static char *emit_mu_state (struct emit_buf *b, nsync_mu *mu,
			    int blocking, int print_waiters) {
        uintptr_t word;
        uintptr_t readers;
        int acquired = 0;
        IGNORE_RACES_START ();
        word = ATM_LOAD (&mu->word);
        if ((word & MU_WAITING) != 0 && print_waiters &&  /* can benefit from lock */
	    (blocking || (word & MU_SPINLOCK) == 0)) {  /* willing, or no need to wait */
                word = nsync_spin_test_and_set_ (&mu->word, MU_SPINLOCK, MU_SPINLOCK, 0);
                acquired = 1;
        }
        readers = word / MU_RLOCK;
        emit_print (b, "mu 0x%i -> 0x%i = {", (uintptr_t) mu, word);
        emit_word (b, mu_bit, word);
        if (readers != 0) {
                emit_print (b, " readers=0x%i", readers);
        }
        emit_print (b, " }");
        if (print_waiters) {
                emit_waiters (b, mu->waiters);
        }
        if (acquired) {
                ATM_STORE_REL (&mu->word, word); /* release store */
        }
        emit_c (b, 0);
        IGNORE_RACES_END ();
        return (b->start);
}

/* Emit to *b the state of *cv, and return a pointer to *b's buffer.

   If blocking!=0, print_waiters!=0, and *cv's waiter list is non-empty, the
   call will block until it can acquire the spinlock.
   If print_waiters!=0, the waiter list is printed.
   The spinlock is released before return if it was acquired.
   blocking==0 && print_waiters!=0 is unsafe and is intended for use within
   interactive debuggers.  */
static char *emit_cv_state (struct emit_buf *b, nsync_cv *cv,
			    int blocking, int print_waiters) {
        uintptr_t word;
        int acquired = 0;
        IGNORE_RACES_START ();
        word = ATM_LOAD (&cv->word);
        if ((word & CV_NON_EMPTY) != 0 && print_waiters &&  /* can benefit from lock */
	    (blocking || (word & CV_SPINLOCK) == 0)) {  /* willing, or no need to wait */
                word = nsync_spin_test_and_set_ (&cv->word, CV_SPINLOCK, CV_SPINLOCK, 0);
                acquired = 1;
        }
        emit_print (b, "cv 0x%i -> 0x%i = {", (uintptr_t) cv, word);
        emit_word (b, cv_bit, word);
        emit_print (b, " }");
        if (print_waiters) {
                emit_waiters (b, cv->waiters);
        }
        if (acquired) {
                ATM_STORE_REL (&cv->word, word); /* release store */
        }
        emit_c (b, 0);
        IGNORE_RACES_END ();
        return (b->start);
}

char *nsync_mu_debug_state (nsync_mu *mu, char *buf, int n) {
        struct emit_buf b;
        return (emit_mu_state (emit_init (&b, buf, n), mu, 0, 0));
}

char *nsync_cv_debug_state (nsync_cv *cv, char *buf, int n) {
        struct emit_buf b;
        return (emit_cv_state (emit_init (&b, buf, n), cv, 0, 0));
}

char *nsync_mu_debug_state_and_waiters (nsync_mu *mu, char *buf, int n) {
        struct emit_buf b;
        return (emit_mu_state (emit_init (&b, buf, n), mu, 1, 1));
}

char *nsync_cv_debug_state_and_waiters (nsync_cv *cv, char *buf, int n) {
        struct emit_buf b;
        return (emit_cv_state (emit_init (&b, buf, n), cv, 1, 1));
}

static char nsync_debug_buf[1024];

char *nsync_mu_debugger (nsync_mu *mu) {
        struct emit_buf b;
        return (emit_mu_state (emit_init (&b, nsync_debug_buf,
                                          (int) sizeof (nsync_debug_buf)),
                               mu, 0, 1));
}
char *nsync_cv_debugger (nsync_cv *cv) {
        struct emit_buf b;
        return (emit_cv_state (emit_init (&b, nsync_debug_buf,
                                          (int) sizeof (nsync_debug_buf)),
                               cv, 0, 1));
}
