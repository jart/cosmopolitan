#ifndef NSYNC_TESTING_CLOSURE_H_
#define NSYNC_TESTING_CLOSURE_H_
#include "libc/mem/mem.h"

/* A run-once, self-freeing closure. */
typedef struct closure_s {
	void (*f0) (void *);
} closure;

/* Run the closure *cl, and free it. */
void closure_run (closure *cl);

/* Fork a new thread running the closure *cl, which will be freed when the
   thread exits.  */
void closure_fork (closure *cl);

/* To create a closure, declare a closure constructor with the right function arguments.

   For functions taking no arguments, use
	CLOSURE_DECL_BODY0 (foo)
   to generate the static routine:
	static closure *closure_foo (void (*f) (void));
   that will return a closure for any function *f that takes no argument.

   For an 1-argument function, use
	CLOSURE_DECL_BODY1 (foo, type)
   to generate the static routine:
	static closure *closure_foo (void (*f) (type), type x);
   that will return a closure for any function taking a single argument of the
   specified type.

   For an 2-argument function, use
	CLOSURE_DECL_BODY2 (foo, type0, type1)
   to generate the static routine:
	static closure *closure_foo (void (*f) (type0, type1), type0 x0, type1 x1);
   that will return a closure for any function taking a "type0" argument, and
   a "type1" argument.

   And so on, up to 9 arguments.

   For example, to make closures out of qsort():

	// First, just once (per module) define:
	//      static closure *closure_qsort_args (
	//              void (*f) (void *, size_t, size_t, int (*)(const void *, const void *))
	//              void *x0, size_t x1, size_t x2, int (*x3)(const void *, const void *));
	// by writing:
	CLOSURE_DECL_BODY4 (qsort_args, void *, size_t, size_t, int (*)(const void *, const void *))

	// Second, for each closure to be created, write something like this:
	closure *cl = closure_qsort_args (array, n_elements, sizeof (array[0]), &elem_cmp);

	// Then to run (and free) each closure:
	closure_run (cl);
	// This is like calling
	//      qsort (array, n_elements, sizeof (array[0]), &elem_cmp);
	//      free (cl);
 */




/* ------------------------------------------------------------------ */
/* Internal macro details follow. */
#define CLOSURE_S0(x,e) e
#define CLOSURE_S1(x,e) x##0
#define CLOSURE_S2(x,e) x##0, x##1
#define CLOSURE_S3(x,e) x##0, x##1, x##2
#define CLOSURE_S4(x,e) x##0, x##1, x##2, x##3
#define CLOSURE_S5(x,e) x##0, x##1, x##2, x##3, x##4
#define CLOSURE_S6(x,e) x##0, x##1, x##2, x##3, x##4, x##5
#define CLOSURE_S7(x,e) x##0, x##1, x##2, x##3, x##4, x##5, x##6
#define CLOSURE_S8(x,e) x##0, x##1, x##2, x##3, x##4, x##5, x##6, x##7
#define CLOSURE_S9(x,e) x##0, x##1, x##2, x##3, x##4, x##5, x##6, x##7, x##8

#define CLOSURE_P0(x,y,p,s,t)
#define CLOSURE_P1(x,y,p,s,t) p x##0 y##0 t
#define CLOSURE_P2(x,y,p,s,t) p x##0 y##0 s x##1 y##1 t
#define CLOSURE_P3(x,y,p,s,t) p x##0 y##0 s x##1 y##1 s x##2 y##2 t
#define CLOSURE_P4(x,y,p,s,t) p x##0 y##0 s x##1 y##1 s x##2 y##2 s x##3 y##3 t
#define CLOSURE_P5(x,y,p,s,t) p x##0 y##0 s x##1 y##1 s x##2 y##2 s x##3 y##3 s x##4 y##4 t
#define CLOSURE_P6(x,y,p,s,t) p x##0 y##0 s x##1 y##1 s x##2 y##2 s x##3 y##3 s x##4 y##4 s \
				x##5 y##5 t
#define CLOSURE_P7(x,y,p,s,t) p x##0 y##0 s x##1 y##1 s x##2 y##2 s x##3 y##3 s x##4 y##4 s \
				x##5 y##5 s x##6 y##6 t
#define CLOSURE_P8(x,y,p,s,t) p x##0 y##0 s x##1 y##1 s x##2 y##2 s x##3 y##3 s x##4 y##4 s \
				x##5 y##5 s x##6 y##6 s x##7 y##7 t
#define CLOSURE_P9(x,y,p,s,t) p x##0 y##0 s x##1 y##1 s x##2 y##2 s x##3 y##3 s x##4 y##4 s \
				x##5 y##5 s x##6 y##6 s x##7 y##7 s x##8 y##8 t

#define CLOSURE_COMMA_ ,
#define CLOSURE_SEMI_ ;
#define CLOSURE_BLANK_

#define CLOSURE_DECL_BODY_N_(name, n) \
	struct closure_s_##name { \
		void (*f0) (void *); /* must be first; matches closure. */ \
		void (*f) (CLOSURE_S##n (closure_t_##name##_,void)); \
		CLOSURE_P##n (closure_t_##name##_, a, CLOSURE_BLANK_, \
			      CLOSURE_SEMI_, CLOSURE_SEMI_) \
	}; \
	static void closure_f0_##name (void *v) { \
		struct closure_s_##name *a = (struct closure_s_##name *) v; \
		(*a->f) (CLOSURE_S##n (a->a,CLOSURE_BLANK_)); \
		free (a); \
	} \
	static closure *closure_##name (void (*f) (CLOSURE_S##n (closure_t_##name##_,void)) \
					CLOSURE_P##n (closure_t_##name##_, a, CLOSURE_COMMA_, \
					CLOSURE_COMMA_, CLOSURE_BLANK_)) { \
		struct closure_s_##name *cl = (struct closure_s_##name *) malloc (sizeof (*cl)); \
		cl->f0 = &closure_f0_##name; \
		cl->f = f; \
		CLOSURE_P##n (cl->a, = a, CLOSURE_BLANK_, CLOSURE_SEMI_, CLOSURE_SEMI_) \
		return ((closure *) cl); \
	}


#define CLOSURE_DECL_BODY0(name) \
	CLOSURE_DECL_BODY_N_ (name, 0)
#define CLOSURE_DECL_BODY1(name, t0) \
	typedef t0 closure_t_##name##_0; \
	CLOSURE_DECL_BODY_N_ (name, 1)
#define CLOSURE_DECL_BODY2(name, t0, t1) \
	typedef t0 closure_t_##name##_0; \
	typedef t1 closure_t_##name##_1; \
	CLOSURE_DECL_BODY_N_ (name, 2)
#define CLOSURE_DECL_BODY3(name, t0, t1, t2) \
	typedef t0 closure_t_##name##_0; \
	typedef t1 closure_t_##name##_1; \
	typedef t2 closure_t_##name##_2; \
	CLOSURE_DECL_BODY_N_ (name, 3)
#define CLOSURE_DECL_BODY4(name, t0, t1, t2, t3) \
	typedef t0 closure_t_##name##_0; \
	typedef t1 closure_t_##name##_1; \
	typedef t2 closure_t_##name##_2; \
	typedef t3 closure_t_##name##_3; \
	CLOSURE_DECL_BODY_N_ (name, 4)
#define CLOSURE_DECL_BODY5(name, t0, t1, t2, t3, t4) \
	typedef t0 closure_t_##name##_0; \
	typedef t1 closure_t_##name##_1; \
	typedef t2 closure_t_##name##_2; \
	typedef t3 closure_t_##name##_3; \
	typedef t4 closure_t_##name##_4; \
	CLOSURE_DECL_BODY_N_ (name, 5)
#define CLOSURE_DECL_BODY6(name, t0, t1, t2, t3, t4, t5) \
	typedef t0 closure_t_##name##_0; \
	typedef t1 closure_t_##name##_1; \
	typedef t2 closure_t_##name##_2; \
	typedef t3 closure_t_##name##_3; \
	typedef t4 closure_t_##name##_4; \
	typedef t5 closure_t_##name##_5; \
	CLOSURE_DECL_BODY_N_ (name, 6)
#define CLOSURE_DECL_BODY7(name, t0, t1, t2, t3, t4, t5, t6) \
	typedef t0 closure_t_##name##_0; \
	typedef t1 closure_t_##name##_1; \
	typedef t2 closure_t_##name##_2; \
	typedef t3 closure_t_##name##_3; \
	typedef t4 closure_t_##name##_4; \
	typedef t5 closure_t_##name##_5; \
	typedef t6 closure_t_##name##_6; \
	CLOSURE_DECL_BODY_N_ (name, 7)
#define CLOSURE_DECL_BODY8(name, t0, t1, t2, t3, t4, t5, t6, t7) \
	typedef t0 closure_t_##name##_0; \
	typedef t1 closure_t_##name##_1; \
	typedef t2 closure_t_##name##_2; \
	typedef t3 closure_t_##name##_3; \
	typedef t4 closure_t_##name##_4; \
	typedef t5 closure_t_##name##_5; \
	typedef t6 closure_t_##name##_6; \
	typedef t7 closure_t_##name##_7; \
	CLOSURE_DECL_BODY_N_ (name, 8)
#define CLOSURE_DECL_BODY9(name, t0, t1, t2, t3, t4, t5, t6, t7, t8) \
	typedef t0 closure_t_##name##_0; \
	typedef t1 closure_t_##name##_1; \
	typedef t2 closure_t_##name##_2; \
	typedef t3 closure_t_##name##_3; \
	typedef t4 closure_t_##name##_4; \
	typedef t5 closure_t_##name##_5; \
	typedef t6 closure_t_##name##_6; \
	typedef t7 closure_t_##name##_7; \
	typedef t8 closure_t_##name##_8; \
	CLOSURE_DECL_BODY_N_ (name, 9)

#endif /*NSYNC_TESTING_CLOSURE_H_*/
