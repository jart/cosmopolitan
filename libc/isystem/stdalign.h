#ifndef _STDALIGN_H
#define _STDALIGN_H
#if !defined(__cplusplus) && defined(__STDC_VERSION__) && \
    __STDC_VERSION__ >= 201112L && __STDC_VERSION__ <= 201710L
/* note: this is a c11 language feature that was deprecated in c23
         in which case you don't need to bother including this hdr */
#define alignas              _Alignas
#define alignof              _Alignof
#define __alignas_is_defined 1
#define __alignof_is_defined 1
#endif
#endif /* _STDALIGN_H */
