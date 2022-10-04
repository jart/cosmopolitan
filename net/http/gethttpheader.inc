/* ANSI-C code produced by gperf version 3.1 */
/* Command-line: gperf gethttpheader.gperf  */
/* Computed positions: -k'5,10-11,22,$' */
// clang-format off

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
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gperf@gnu.org>."
#endif

#line 1 "gethttpheader.gperf"

#include "libc/str/str.h"
#include "libc/str/tab.internal.h"
#include "net/http/http.h"
#define GPERF_DOWNCASE
#line 12 "gethttpheader.gperf"
struct thatispacked HttpHeaderSlot { char *name; char code; };

#define TOTAL_KEYWORDS 93
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 32
#define MIN_HASH_VALUE 3
#define MAX_HASH_VALUE 181
/* maximum key range = 179, duplicates = 0 */

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
static inline int
gperf_case_strncmp (register const char *s1, register const char *s2, register size_t n)
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
static inline unsigned int
hash (register const char *str, register size_t len)
{
  static const unsigned char asso_values[] =
    {
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182,  35, 182, 182, 182, 182,
      182, 182, 182,   5, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182,  15, 182,  30,  60,  10,
        5,  40,   0,  40, 182,  80,  40,  90,   0,  45,
        5,   0,  45,   0,   0,   0, 182,   0, 182,  45,
      182, 182, 182, 182, 182, 182, 182,  15, 182,  30,
       60,  10,   5,  40,   0,  40, 182,  80,  40,  90,
        0,  45,   5,   0,  45,   0,   0,   0, 182,   0,
      182,  45, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182
    };
  register unsigned int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[21]];
      /*FALLTHROUGH*/
      case 21:
      case 20:
      case 19:
      case 18:
      case 17:
      case 16:
      case 15:
      case 14:
      case 13:
      case 12:
      case 11:
        hval += asso_values[(unsigned char)str[10]];
      /*FALLTHROUGH*/
      case 10:
        hval += asso_values[(unsigned char)str[9]];
      /*FALLTHROUGH*/
      case 9:
      case 8:
      case 7:
      case 6:
      case 5:
        hval += asso_values[(unsigned char)str[4]];
      /*FALLTHROUGH*/
      case 4:
      case 3:
      case 2:
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

static inline const struct thatispacked HttpHeaderSlot *
LookupHttpHeader (register const char *str, register size_t len)
{
  static const struct thatispacked HttpHeaderSlot wordlist[] =
    {
      {""}, {""}, {""},
#line 27 "gethttpheader.gperf"
      {"DNT",                              kHttpDnt},
#line 14 "gethttpheader.gperf"
      {"Host",                             kHttpHost},
#line 46 "gethttpheader.gperf"
      {"Allow",                            kHttpAllow},
      {""}, {""},
#line 73 "gethttpheader.gperf"
      {"Location",                         kHttpLocation},
      {""}, {""},
#line 17 "gethttpheader.gperf"
      {"Accept",                           kHttpAccept},
#line 80 "gethttpheader.gperf"
      {"TE",                               kHttpTe},
#line 58 "gethttpheader.gperf"
      {"Age",                              kHttpAge},
#line 40 "gethttpheader.gperf"
      {"Date",                             kHttpDate},
      {""}, {""}, {""},
#line 86 "gethttpheader.gperf"
      {"Via",                              kHttpVia},
      {""},
#line 16 "gethttpheader.gperf"
      {"Connection",                       kHttpConnection},
      {""}, {""},
#line 67 "gethttpheader.gperf"
      {"If-Match",                         kHttpIfMatch},
      {""},
#line 36 "gethttpheader.gperf"
      {"Range",                            kHttpRange},
#line 92 "gethttpheader.gperf"
      {"X-XSS-Protection",                 kHttpXXssProtection},
      {""},
#line 50 "gethttpheader.gperf"
      {"Access-Control-Allow-Headers",     kHttpAccessControlAllowHeaders},
#line 55 "gethttpheader.gperf"
      {"Access-Control-RequestHeaders",    kHttpAccessControlRequestHeaders},
#line 57 "gethttpheader.gperf"
      {"Access-Control-Request-Methods",   kHttpAccessControlRequestMethods},
#line 53 "gethttpheader.gperf"
      {"Access-Control-MaxAge",            kHttpAccessControlMaxAge},
#line 83 "gethttpheader.gperf"
      {"Upgrade",                          kHttpUpgrade},
#line 69 "gethttpheader.gperf"
      {"If-Range",                         kHttpIfRange},
#line 37 "gethttpheader.gperf"
      {"Content-Length",                   kHttpContentLength},
#line 88 "gethttpheader.gperf"
      {"X-Frame-Options",                  kHttpXFrameOptions},
#line 66 "gethttpheader.gperf"
      {"Expect",                           kHttpExpect},
#line 90 "gethttpheader.gperf"
      {"Alt-Svc",                          kHttpAltSvc},
      {""},
#line 61 "gethttpheader.gperf"
      {"Content-Description",              kHttpContentDescription},
      {""},
#line 85 "gethttpheader.gperf"
      {"WWW-Authenticate",                 kHttpWwwAuthenticate},
      {""}, {""},
#line 45 "gethttpheader.gperf"
      {"ETag",                             kHttpEtag},
#line 20 "gethttpheader.gperf"
      {"User-Agent",                       kHttpUserAgent},
#line 23 "gethttpheader.gperf"
      {"Origin",                           kHttpOrigin},
#line 60 "gethttpheader.gperf"
      {"Content-Base",                     kHttpContentBase},
#line 47 "gethttpheader.gperf"
      {"Content-Range",                    kHttpContentRange},
#line 39 "gethttpheader.gperf"
      {"Vary",                             kHttpVary},
#line 24 "gethttpheader.gperf"
      {"Upgrade-Insecure-Requests",        kHttpUpgradeInsecureRequests},
#line 63 "gethttpheader.gperf"
      {"Content-Language",                 kHttpContentLanguage},
#line 42 "gethttpheader.gperf"
      {"Expires",                          kHttpExpires},
#line 106 "gethttpheader.gperf"
      {"CDN-Loop",                         kHttpCdnLoop},
#line 95 "gethttpheader.gperf"
      {"Sec-CH-UA",                        kHttpSecChUa},
      {""},
#line 26 "gethttpheader.gperf"
      {"Cookie",                           kHttpCookie},
#line 89 "gethttpheader.gperf"
      {"X-Content-Type-Options",           kHttpXContentTypeOptions},
#line 93 "gethttpheader.gperf"
      {"Accept-Ranges",                    kHttpAcceptRanges},
#line 35 "gethttpheader.gperf"
      {"Save-Data",                        kHttpSaveData},
#line 94 "gethttpheader.gperf"
      {"Set-Cookie",                       kHttpSetCookie},
#line 41 "gethttpheader.gperf"
      {"Server",                           kHttpServer},
#line 49 "gethttpheader.gperf"
      {"Access-Control-Allow-Credentials", kHttpAccessControlAllowCredentials},
      {""},
#line 98 "gethttpheader.gperf"
      {"Sec-Fetch-Site",                   kHttpSecFetchSite},
#line 71 "gethttpheader.gperf"
      {"Keep-Alive",                       kHttpKeepAlive},
#line 102 "gethttpheader.gperf"
      {"CF-RAY",                           kHttpCfRay},
#line 82 "gethttpheader.gperf"
      {"Transfer-Encoding",                kHttpTransferEncoding},
      {""},
#line 62 "gethttpheader.gperf"
      {"Content-Disposition",              kHttpContentDisposition},
#line 18 "gethttpheader.gperf"
      {"Accept-Language",                  kHttpAcceptLanguage},
#line 77 "gethttpheader.gperf"
      {"Proxy-Connection",                 kHttpProxyConnection},
#line 52 "gethttpheader.gperf"
      {"Access-Control-Allow-Origin",      kHttpAccessControlAllowOrigin},
#line 68 "gethttpheader.gperf"
      {"If-None-Match",                    kHttpIfNoneMatch},
#line 70 "gethttpheader.gperf"
      {"If-Unmodified-Since",              kHttpIfUnmodifiedSince},
      {""},
#line 78 "gethttpheader.gperf"
      {"Public",                           kHttpPublic},
#line 28 "gethttpheader.gperf"
      {"Sec-GPC",                          kHttpSecGpc},
      {""},
#line 48 "gethttpheader.gperf"
      {"Accept-Charset",                   kHttpAcceptCharset},
      {""},
#line 54 "gethttpheader.gperf"
      {"Access-Control-Method",            kHttpAccessControlMethod},
#line 38 "gethttpheader.gperf"
      {"Content-Type",                     kHttpContentType},
#line 75 "gethttpheader.gperf"
      {"Proxy-Authenticate",               kHttpProxyAuthenticate},
#line 72 "gethttpheader.gperf"
      {"Link",                             kHttpLink},
      {""},
#line 31 "gethttpheader.gperf"
      {"X-Requested-With",                 kHttpXRequestedWith},
#line 84 "gethttpheader.gperf"
      {"Warning",                          kHttpWarning},
      {""},
#line 56 "gethttpheader.gperf"
      {"Access-Control-Request-Method",    kHttpAccessControlRequestMethod},
      {""},
#line 65 "gethttpheader.gperf"
      {"Content-MD5",                      kHttpContentMd5},
#line 81 "gethttpheader.gperf"
      {"Trailer",                          kHttpTrailer},
      {""},
#line 29 "gethttpheader.gperf"
      {"From",                             kHttpFrom},
      {""},
#line 43 "gethttpheader.gperf"
      {"Content-Encoding",                 kHttpContentEncoding},
#line 21 "gethttpheader.gperf"
      {"Referer",                          kHttpReferer},
#line 59 "gethttpheader.gperf"
      {"Authorization",                    kHttpAuthorization},
#line 100 "gethttpheader.gperf"
      {"Sec-Fetch-User",                   kHttpSecFetchUser},
      {""},
#line 64 "gethttpheader.gperf"
      {"Content-Location",                 kHttpContentLocation},
      {""}, {""}, {""}, {""},
#line 104 "gethttpheader.gperf"
      {"CF-Connecting-IP",                 kHttpCfConnectingIp},
#line 105 "gethttpheader.gperf"
      {"CF-IPCountry",                     kHttpCfIpcountry},
#line 15 "gethttpheader.gperf"
      {"Cache-Control",                    kHttpCacheControl},
#line 76 "gethttpheader.gperf"
      {"Proxy-Authorization",              kHttpProxyAuthorization},
      {""},
#line 25 "gethttpheader.gperf"
      {"Pragma",                           kHttpPragma},
      {""}, {""},
#line 101 "gethttpheader.gperf"
      {"Sec-Fetch-Dest",                   kHttpSecFetchDest},
      {""}, {""}, {""},
#line 51 "gethttpheader.gperf"
      {"Access-Control-Allow-Methods",     kHttpAccessControlAllowMethods},
      {""}, {""}, {""},
#line 74 "gethttpheader.gperf"
      {"Max-Forwards",                     kHttpMaxForwards},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 32 "gethttpheader.gperf"
      {"X-Forwarded-Host",                 kHttpXForwardedHost},
      {""}, {""}, {""},
#line 19 "gethttpheader.gperf"
      {"Accept-Encoding",                  kHttpAcceptEncoding},
      {""}, {""}, {""}, {""},
#line 103 "gethttpheader.gperf"
      {"CF-Visitor",                       kHttpCfVisitor},
      {""},
#line 30 "gethttpheader.gperf"
      {"If-Modified-Since",                kHttpIfModifiedSince},
      {""}, {""}, {""}, {""},
#line 34 "gethttpheader.gperf"
      {"X-CSRF-Token",                     kHttpXCsrfToken},
      {""}, {""}, {""}, {""}, {""},
#line 44 "gethttpheader.gperf"
      {"Last-Modified",                    kHttpLastModified},
#line 99 "gethttpheader.gperf"
      {"Sec-Fetch-Mode",                   kHttpSecFetchMode},
#line 91 "gethttpheader.gperf"
      {"Referrer-Policy",                  kHttpReferrerPolicy},
#line 79 "gethttpheader.gperf"
      {"Retry-After",                      kHttpRetryAfter},
      {""}, {""}, {""},
#line 87 "gethttpheader.gperf"
      {"Strict-Transport-Security",        kHttpStrictTransportSecurity},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""},
#line 22 "gethttpheader.gperf"
      {"X-Forwarded-For",                  kHttpXForwardedFor},
      {""},
#line 33 "gethttpheader.gperf"
      {"X-Forwarded-Proto",                kHttpXForwardedProto},
#line 97 "gethttpheader.gperf"
      {"Sec-CH-UA-Platform",               kHttpSecChUaPlatform},
      {""}, {""},
#line 96 "gethttpheader.gperf"
      {"Sec-CH-UA-Mobile",                 kHttpSecChUaMobile}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register unsigned int key = hash (str, len);

      if (key <= MAX_HASH_VALUE)
        {
          register const char *s = wordlist[key].name;

          if ((((unsigned char)*str ^ (unsigned char)*s) & ~32) == 0 && !gperf_case_strncmp (str, s, len) && s[len] == '\0')
            return &wordlist[key];
        }
    }
  return 0;
}
