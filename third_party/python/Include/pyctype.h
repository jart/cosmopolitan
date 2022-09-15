#ifndef Py_LIMITED_API
#ifndef PYCTYPE_H
#define PYCTYPE_H
#include "libc/str/tab.internal.h"

#define Py_TOLOWER(c) kToLower[255 & (c)]
#define Py_TOUPPER(c) kToUpper[255 & (c)]

#define Py_ISDIGIT(C)       \
  ({                        \
    unsigned char c_ = (C); \
    '0' <= c_&& c_ <= '9';  \
  })

#define Py_ISLOWER(C)       \
  ({                        \
    unsigned char c_ = (C); \
    'a' <= c_&& c_ <= 'z';  \
  })

#define Py_ISUPPER(C)       \
  ({                        \
    unsigned char c_ = (C); \
    'A' <= c_&& c_ <= 'Z';  \
  })

#define Py_ISALPHA(C)                                     \
  ({                                                      \
    unsigned char c_ = (C);                               \
    ('A' <= c_ && c_ <= 'Z') || ('a' <= c_ && c_ <= 'z'); \
  })

#define Py_ISALNUM(C)                                        \
  ({                                                         \
    unsigned char c_ = (C);                                  \
    (('0' <= c_ && c_ <= '9') || ('A' <= c_ && c_ <= 'Z') || \
     ('a' <= c_ && c_ <= 'z'));                              \
  })

#define Py_ISSPACE(C)                                                     \
  ({                                                                      \
    unsigned char c_ = (C);                                               \
    (c_ == ' ' || c_ == '\t' || c_ == '\r' || c_ == '\n' || c_ == '\f' || \
     c_ == '\v');                                                         \
  })

#define Py_ISXDIGIT(C)                                       \
  ({                                                         \
    unsigned char c_ = (C);                                  \
    (('0' <= c_ && c_ <= '9') || ('A' <= c_ && c_ <= 'F') || \
     ('a' <= c_ && c_ <= 'f'));                              \
  })

#endif /* !PYCTYPE_H */
#endif /* !Py_LIMITED_API */
