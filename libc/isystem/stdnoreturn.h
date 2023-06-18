#ifndef _STDNORETURN_H
#define _STDNORETURN_H
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112
#define noreturn _Noreturn
#elif (defined(__has_attribute) && __has_attribute(__noreturn__)) || \
    (defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ >= 208)
#define noreturn __attribute__((__noreturn__))
#else
#define noreturn
#endif
#endif /* _STDNORETURN_H */
