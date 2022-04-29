#ifndef lctype_h
#define lctype_h
#include "third_party/lua/lua.h"

/*
** In ASCII, this 'ltolower' is correct for alphabetic characters and
** for '.'. That is enough for Lua needs. ('check_exp' ensures that
** the character either is an upper-case letter or is unchanged by
** the transformation, which holds for lower-case letters and '.'.)
*/
#define ltolower(c)                                                   \
  check_exp(('A' <= (c) && (c) <= 'Z') || (c) == ((c) | ('A' ^ 'a')), \
            (c) | ('A' ^ 'a'))

#define lisdigit(C)         \
  ({                        \
    unsigned char c_ = (C); \
    '0' <= c_&& c_ <= '9';  \
  })

#define lislalpha(C)                                                   \
  ({                                                                   \
    unsigned char c_ = (C);                                            \
    ('A' <= c_ && c_ <= 'Z') || ('a' <= c_ && c_ <= 'z') || c_ == '_'; \
  })

#define lislalnum(C)                                         \
  ({                                                         \
    unsigned char c_ = (C);                                  \
    (('0' <= c_ && c_ <= '9') || ('A' <= c_ && c_ <= 'Z') || \
     ('a' <= c_ && c_ <= 'z') || c_ == '_');                 \
  })

#define lisspace(C)                                                       \
  ({                                                                      \
    unsigned char c_ = (C);                                               \
    (c_ == ' ' || c_ == '\t' || c_ == '\r' || c_ == '\n' || c_ == '\f' || \
     c_ == '\v');                                                         \
  })

#define lisxdigit(C)                                         \
  ({                                                         \
    unsigned char c_ = (C);                                  \
    (('0' <= c_ && c_ <= '9') || ('A' <= c_ && c_ <= 'F') || \
     ('a' <= c_ && c_ <= 'f'));                              \
  })

#define lisbdigit(C)        \
  ({                        \
    unsigned char c_ = (C); \
    '0' <= c_&& c_ <= '1';  \
  })

#define lisprint(C)         \
  ({                        \
    unsigned char c_ = (C); \
    32 <= c_&& c_ <= 126;   \
  })

#endif
