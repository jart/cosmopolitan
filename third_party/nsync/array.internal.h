#ifndef NSYNC_ARRAY_H_
#define NSYNC_ARRAY_H_

/* Return the number of elements in a C array a.
   A constant expression if a is a constant expression. */
#define NELEM(a) ((int) (sizeof (a) / sizeof (a[0])))


/* A dynamic array */

/* internal routines */
void a_ensure_ (void *v, int delta, int sz);

/* The following struct prefixes all array values. */
struct a_hdr_ {
	int len_;  /* The number of elements considererf to be part of the array. */
	int max_;  /* abs(max_) is the number of elements that have been set aside
                      to store elements of the array.  If max_ < 0, the space
                      has been allocated statically.  If max_ > 0, the space
                      has been allocated dynamically.  */
};

/* A type declaration for an array of "type".  Example:  typedef A_TYPE (int) array_of_int; */
#define A_TYPE(type) struct { struct a_hdr_ h_; type *a_; }

/* The empty array initializer.   Empty arrays can also be initialized by zeroing. */
#define A_EMPTY { { 0, 0 }, NULL }

/* A static initializer using the contents of C array "data".
   Example:
       static int some_ints[] = { 7, 11, 13 };
       static array_of_int x = A_INIT (some_ints);    */
#define A_INIT(data) { { NELEM (data), -NELEM (data) }, data }

/* Element i of the array *a   (l-value or r-value) */
#define A(a,i) ((a)->a_[i])

/* Append an entry to array *a, and yield it as an l-value. */
#define A_PUSH(a) (*(a_ensure_ ((a), 1, sizeof ((a)->a_[0])), &(a)->a_[(a)->h_.len_++]))

/* Return the length of array *a. */
#define A_LEN(a) ((a)->h_.len_)

/* Set the length of array *a to l.  Requires that 0 <= l <= A_LEN (a). */
#define A_SET_LEN(a, l) do { if (0 <= (l) && (l) <= (a)->h_.len_) { \
				(a)->h_.len_ = (l); } else {  \
				*(volatile int *)0 = 0; } } while (0)

/* Reduce the length of array *a by n.  Requires that 0 <= n <= A_LEN (a). */
#define A_DISCARD(a, n) do { if (0 <= (n) && (n) <= (a)->h_.len_) { \
				(a)->h_.len_ -= (n); } else { \
			        *(volatile int *)0 = 0; } } while (0)

/* Deallocate and disassociate any storage associated with *a, and make *a
   empty.  */
#define A_FREE(a) do { if ((a)->h_.max_ > 0) { free ((void *) (a)->a_); } \
                       (a)->a_ = NULL; (a)->h_.max_ = 0; (a)->h_.len_ = 0; \
		  } while (0)

#endif /*NSYNC_ARRAY_H_*/
