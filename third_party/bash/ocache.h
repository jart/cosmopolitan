/* ocache.h -- a minimal object caching implementation. */

/* Copyright (C) 2002 Free Software Foundation, Inc.

   This file is part of GNU Bash, the Bourne Again SHell.

   Bash is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Bash is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Bash.  If not, see <http://www.gnu.org/licenses/>.
*/

#if !defined (_OCACHE_H_)
#define	_OCACHE_H_ 1

#ifndef PTR_T

#if defined (__STDC__)
#  define PTR_T void *
#else
#  define PTR_T char *
#endif

#endif /* PTR_T */

#define OC_MEMSET(memp, xch, nbytes)					\
do {									\
  if ((nbytes) <= 32) {							\
    register char * mzp = (char *)(memp);				\
    unsigned long mctmp = (nbytes);					\
    register long mcn;							\
    if (mctmp < 8) mcn = 0; else { mcn = (mctmp-1)/8; mctmp &= 7; }	\
    switch (mctmp) {							\
      case 0: for(;;) { *mzp++ = xch;					\
      case 7:	   *mzp++ = xch;					\
      case 6:	   *mzp++ = xch;					\
      case 5:	   *mzp++ = xch;					\
      case 4:	   *mzp++ = xch;					\
      case 3:	   *mzp++ = xch;					\
      case 2:	   *mzp++ = xch;					\
      case 1:	   *mzp++ = xch; if(mcn <= 0) break; mcn--; }		\
    }									\
  } else								\
    memset ((memp), (xch), (nbytes));					\
} while(0)

typedef struct objcache {
	PTR_T	data;
	int	cs;		/* cache size, number of objects */
	int	nc;		/* number of cache entries */
} sh_obj_cache_t;

/* Create an object cache C of N pointers to OTYPE. */
#define ocache_create(c, otype, n) \
	do { \
		(c).data = xmalloc((n) * sizeof (otype *)); \
		(c).cs = (n); \
		(c).nc = 0; \
	} while (0)

/* Destroy an object cache C. */
#define ocache_destroy(c) \
	do { \
		if ((c).data) \
			xfree ((c).data); \
		(c).data = 0; \
		(c).cs = (c).nc = 0; \
	} while (0)

/* Free all cached items, which are pointers to OTYPE, in object cache C. */
#define ocache_flush(c, otype) \
	do { \
		while ((c).nc > 0) \
			xfree (((otype **)((c).data))[--(c).nc]); \
	} while (0)

/*
 * Allocate a new item of type pointer to OTYPE, using data from object
 * cache C if any cached items exist, otherwise calling xmalloc.  Return
 * the object in R.
 */
#define ocache_alloc(c, otype, r) \
	do { \
		if ((c).nc > 0) { \
			(r) = (otype *)((otype **)((c).data))[--(c).nc]; \
		} else \
			(r) = (otype *)xmalloc (sizeof (otype)); \
	} while (0)

/*
 * Free an item R of type pointer to OTYPE, adding to object cache C if
 * there is room and calling xfree if the cache is full.  If R is added
 * to the object cache, the contents are scrambled.
 */
#define ocache_free(c, otype, r) \
	do { \
		if ((c).nc < (c).cs) { \
			OC_MEMSET ((r), 0xdf, sizeof(otype)); \
			((otype **)((c).data))[(c).nc++] = (r); \
		} else \
			xfree (r); \
	} while (0)

/*
 * One may declare and use an object cache as (for instance):
 *
 *	sh_obj_cache_t wdcache = {0, 0, 0};
 *	sh_obj_cache_t wlcache = {0, 0, 0};
 *
 *	ocache_create(wdcache, WORD_DESC, 30);
 *	ocache_create(wlcache, WORD_LIST, 30);
 *
 *	WORD_DESC *wd;
 *	ocache_alloc (wdcache, WORD_DESC, wd);
 *
 *	WORD_LIST *wl;
 *	ocache_alloc (wlcache, WORD_LIST, wl);
 *
 *	ocache_free(wdcache, WORD_DESC, wd);
 *	ocache_free(wlcache, WORD_LIST, wl);
 *
 * The use is almost arbitrary.
 */

#endif /* _OCACHE_H  */
