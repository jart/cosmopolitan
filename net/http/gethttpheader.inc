/* ANSI-C code produced by gperf version 3.0.4 */
/* Command-line: gperf net/http/gethttpheader.gperf  */
/* Computed positions: -k'1,9,$' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

#line 1 "net/http/gethttpheader.gperf"

#include "libc/str/str.h"
#include "net/http/http.h"
#define GPERF_DOWNCASE
#line 12 "net/http/gethttpheader.gperf"
struct HttpHeaderSlot { unsigned char name; unsigned char code; };

#define TOTAL_KEYWORDS 49
#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 19
#define MIN_HASH_VALUE 5
#define MAX_HASH_VALUE 72
/* maximum key range = 68, duplicates = 0 */

#ifndef GPERF_DOWNCASE
#define GPERF_DOWNCASE 1
static unsigned char gperf_downcase[256] =
  {
      0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
     15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
     30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,
     45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
     60,  61,  62,  63,  64,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106,
    107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121,
    122,  91,  92,  93,  94,  95,  96,  97,  98,  99, 100, 101, 102, 103, 104,
    105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134,
    135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149,
    150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164,
    165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179,
    180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194,
    195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
    210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224,
    225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254,
    255
  };
#endif

#ifndef GPERF_CASE_STRNCMP
#define GPERF_CASE_STRNCMP 1
static int
gperf_case_strncmp (register const char *s1, register const char *s2, register unsigned int n)
{
  for (; n > 0;)
    {
      unsigned char c1 = gperf_downcase[(unsigned char)*s1++];
      unsigned char c2 = gperf_downcase[(unsigned char)*s2++];
      if (c1 != 0 && c1 == c2)
        {
          n--;
          continue;
        }
      return (int)c1 - (int)c2;
    }
  return 0;
}
#endif

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73,  0, 73, 73, 73, 73,
      73, 73, 73,  0, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73,  5, 20,  0, 15,  0,
       0, 45, 20,  5, 73, 10, 35,  5,  5,  5,
      20, 73,  5, 30,  0,  0, 15, 20, 73, 15,
      73, 73, 73, 73, 73, 73, 73,  5, 20,  0,
      15,  0,  0, 45, 20,  5, 73, 10, 35,  5,
       5,  5, 20, 73,  5, 30,  0,  0, 15, 20,
      73, 15, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[8]];
      /*FALLTHROUGH*/
      case 8:
      case 7:
      case 6:
      case 5:
      case 4:
      case 3:
      case 2:
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

struct stringpool_t
  {
    char stringpool_str5[sizeof("Close")];
    char stringpool_str7[sizeof("Upgrade")];
    char stringpool_str8[sizeof("Age")];
    char stringpool_str9[sizeof("From")];
    char stringpool_str10[sizeof("Range")];
    char stringpool_str11[sizeof("Accept")];
    char stringpool_str12[sizeof("Content-Type")];
    char stringpool_str13[sizeof("If-Range")];
    char stringpool_str15[sizeof("User-Agent")];
    char stringpool_str16[sizeof("Content-Md5")];
    char stringpool_str17[sizeof("Referer")];
    char stringpool_str18[sizeof("Content-Range")];
    char stringpool_str19[sizeof("Date")];
    char stringpool_str20[sizeof("Connection")];
    char stringpool_str21[sizeof("Retry-After")];
    char stringpool_str22[sizeof("Chunked")];
    char stringpool_str23[sizeof("Via")];
    char stringpool_str24[sizeof("Host")];
    char stringpool_str25[sizeof("Accept-Language")];
    char stringpool_str26[sizeof("Public")];
    char stringpool_str27[sizeof("If-Modified-Since")];
    char stringpool_str28[sizeof("Authorization")];
    char stringpool_str29[sizeof("If-Unmodified-Since")];
    char stringpool_str30[sizeof("Allow")];
    char stringpool_str31[sizeof("Pragma")];
    char stringpool_str32[sizeof("Content-Base")];
    char stringpool_str33[sizeof("If-Match")];
    char stringpool_str34[sizeof("Vary")];
    char stringpool_str35[sizeof("Keep-Alive")];
    char stringpool_str36[sizeof("WWW-Authenticate")];
    char stringpool_str37[sizeof("Expires")];
    char stringpool_str38[sizeof("Proxy-Authenticate")];
    char stringpool_str39[sizeof("Accept-Charset")];
    char stringpool_str41[sizeof("Server")];
    char stringpool_str43[sizeof("If-None-Match")];
    char stringpool_str44[sizeof("Proxy-Authorization")];
    char stringpool_str46[sizeof("Proxy-Connection")];
    char stringpool_str48[sizeof("Location")];
    char stringpool_str49[sizeof("Etag")];
    char stringpool_str51[sizeof("Content-Language")];
    char stringpool_str52[sizeof("Max-Forwards")];
    char stringpool_str53[sizeof("Cache-Control")];
    char stringpool_str56[sizeof("Content-Location")];
    char stringpool_str61[sizeof("Content-Encoding")];
    char stringpool_str62[sizeof("Transfer-Encoding")];
    char stringpool_str68[sizeof("Last-Modified")];
    char stringpool_str69[sizeof("Content-Length")];
    char stringpool_str70[sizeof("Accept-Encoding")];
    char stringpool_str72[sizeof("Warning")];
  };
static const struct stringpool_t stringpool_contents =
  {
    "Close",
    "Upgrade",
    "Age",
    "From",
    "Range",
    "Accept",
    "Content-Type",
    "If-Range",
    "User-Agent",
    "Content-Md5",
    "Referer",
    "Content-Range",
    "Date",
    "Connection",
    "Retry-After",
    "Chunked",
    "Via",
    "Host",
    "Accept-Language",
    "Public",
    "If-Modified-Since",
    "Authorization",
    "If-Unmodified-Since",
    "Allow",
    "Pragma",
    "Content-Base",
    "If-Match",
    "Vary",
    "Keep-Alive",
    "WWW-Authenticate",
    "Expires",
    "Proxy-Authenticate",
    "Accept-Charset",
    "Server",
    "If-None-Match",
    "Proxy-Authorization",
    "Proxy-Connection",
    "Location",
    "Etag",
    "Content-Language",
    "Max-Forwards",
    "Cache-Control",
    "Content-Location",
    "Content-Encoding",
    "Transfer-Encoding",
    "Last-Modified",
    "Content-Length",
    "Accept-Encoding",
    "Warning"
  };
#define stringpool ((const char *) &stringpool_contents)
#ifdef __GNUC__
__inline
#if defined __GNUC_STDC_INLINE__ || defined __GNUC_GNU_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
const struct HttpHeaderSlot *
in_word_set (register const char *str, register unsigned int len)
{
  static const struct HttpHeaderSlot wordlist[] =
    {
      {-1}, {-1}, {-1}, {-1}, {-1},
#line 23 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str5,                kHttpClose               },
      {-1},
#line 52 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str7,              kHttpUpgrade             },
#line 18 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str8,                  kHttpAge                 },
#line 36 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str9,                 kHttpFrom                },
#line 49 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str10,                kHttpRange               },
#line 14 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str11,               kHttpAccept              },
#line 32 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str12,         kHttpContentType         },
#line 41 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str13,             kHttpIfRange             },
      {-1},
#line 53 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str15,           kHttpUserAgent           },
#line 30 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str16,          kHttpContentMd5          },
#line 50 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str17,              kHttpReferer             },
#line 31 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str18,        kHttpContentRange        },
#line 33 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str19,                 kHttpDate                },
#line 24 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str20,           kHttpConnection          },
#line 57 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str21,          kHttpRetryAfter          },
#line 22 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str22,              kHttpChunked             },
#line 54 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str23,                  kHttpVia                 },
#line 37 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str24,                 kHttpHost                },
#line 17 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str25,      kHttpAcceptLanguage      },
#line 56 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str26,               kHttpPublic              },
#line 39 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str27,    kHttpIfModifiedSince     },
#line 20 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str28,        kHttpAuthorization       },
#line 42 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str29,  kHttpIfUnmodifiedSince   },
#line 19 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str30,                kHttpAllow               },
#line 45 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str31,               kHttpPragma              },
#line 25 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str32,         kHttpContentBase         },
#line 38 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str33,             kHttpIfMatch             },
#line 59 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str34,                 kHttpVary                },
#line 43 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str35,           kHttpKeepAlive           },
#line 61 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str36,     kHttpWwwAuthenticate     },
#line 35 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str37,              kHttpExpires             },
#line 46 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str38,   kHttpProxyAuthenticate   },
#line 15 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str39,       kHttpAcceptCharset       },
      {-1},
#line 58 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str41,               kHttpServer              },
      {-1},
#line 40 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str43,        kHttpIfNoneMatch         },
#line 47 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str44,  kHttpProxyAuthorization  },
      {-1},
#line 48 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str46,     kHttpProxyConnection     },
      {-1},
#line 55 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str48,             kHttpLocation            },
#line 34 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str49,                 kHttpEtag                },
      {-1},
#line 27 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str51,     kHttpContentLanguage     },
#line 44 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str52,         kHttpMaxForwards         },
#line 21 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str53,        kHttpCacheControl        },
      {-1}, {-1},
#line 29 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str56,     kHttpContentLocation     },
      {-1}, {-1}, {-1}, {-1},
#line 26 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str61,     kHttpContentEncoding     },
#line 51 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str62,    kHttpTransferEncoding    },
      {-1}, {-1}, {-1}, {-1}, {-1},
#line 62 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str68,        kHttpLastModified        },
#line 28 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str69,       kHttpContentLength       },
#line 16 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str70,      kHttpAcceptEncoding      },
      {-1},
#line 60 "net/http/gethttpheader.gperf"
      {(int)(long)&((struct stringpool_t *)0)->stringpool_str72,              kHttpWarning             }
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int o = wordlist[key].name;
          if (o >= 0)
            {
              register const char *s = o + stringpool;

              if ((((unsigned char)*str ^ (unsigned char)*s) & ~32) == 0 && !gperf_case_strncmp (str, s, len) && s[len] == '\0')
                return &wordlist[key];
            }
        }
    }
  return 0;
}
