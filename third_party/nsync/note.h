#ifndef NSYNC_NOTE_H_
#define NSYNC_NOTE_H_
#include "third_party/nsync/time.h"
COSMOPOLITAN_C_START_

/* An nsync_note represents a single bit that can transition from 0 to 1
   at most once. When 1, the note is said to be notified. There are
   operations to wait for the transition, which can be triggered either
   by an explicit call, or timer expiry. Notes can have parent notes; a
   note becomes notified if its parent becomes notified. */
typedef struct nsync_note_s_ *nsync_note;

/* Return a freshly allocated nsync_note, or NULL if an nsync_note
   cannot be created.

   If parent!=NULL, the allocated nsync_note's parent will be parent.
   The newaly allocated note will be automatically notified at
   abs_deadline, and is notified at initialization if
   abs_deadline==nsync_zero_time.

   nsync_notes should be passed to nsync_note_free() when no longer needed. */
nsync_note nsync_note_new(nsync_note parent, nsync_time abs_deadline);

/* Free resources associated with n. Requires that n was allocated by
   nsync_note_new(), and no concurrent or future operations are applied
   to n directly.

   It is legal to call nsync_note_free() on a node even if it has a
   parent or children that are in use; if n has both a parent and
   children, n's parent adopts its children. */
void nsync_note_free(nsync_note n);

/* Notify n and all its descendants. */
void nsync_note_notify(nsync_note n);

/* Return whether n has been notified.  */
int nsync_note_is_notified(nsync_note n);

/* Wait until n has been notified or abs_deadline is reached, and return
   whether n has been notified. If abs_deadline==nsync_time_no_deadline,
   the deadline is far in the future. */
int nsync_note_wait(nsync_note n, nsync_time abs_deadline);

/* Return the expiry time associated with n. This is the minimum of the
   abs_deadline passed on creation and that of any of its ancestors. */
nsync_time nsync_note_expiry(nsync_note n);

COSMOPOLITAN_C_END_
#endif /* NSYNC_NOTE_H_ */
