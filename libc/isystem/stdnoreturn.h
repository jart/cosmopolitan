#ifndef _STDNORETURN_H
#define _STDNORETURN_H
#ifndef __cplusplus
/* note: this is a c11 language feature that was deprecated in c23
         where you're encouraged to use [[noreturn]] just like c++
         in which case you don't need to bother including this hdr */
#define noreturn _Noreturn
#endif
#endif /* _STDNORETURN_H */
