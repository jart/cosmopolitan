#ifndef _FEATURES_H
#define _FEATURES_H

#if defined(_ALL_SOURCE) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE 1
#endif

#if defined(_DEFAULT_SOURCE) && !defined(_BSD_SOURCE)
#define _BSD_SOURCE 1
#endif

#if !defined(_POSIX_SOURCE) && !defined(_POSIX_C_SOURCE) && \
    !defined(_XOPEN_SOURCE) && !defined(_GNU_SOURCE) && !defined(_BSD_SOURCE)
#define _BSD_SOURCE   1
#define _XOPEN_SOURCE 700
#endif

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define __restrict restrict
#else
#define __restrict
#endif

#if (defined(__cplusplus) || \
     (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L))
#define __inline inline
#else
#define __inline
#endif

#endif /* _FEATURES_H */
