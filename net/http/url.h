#ifndef COSMOPOLITAN_NET_HTTP_URL_H_
#define COSMOPOLITAN_NET_HTTP_URL_H_

#define kUrlPlus   1
#define kUrlLatin1 2
#define kUrlOpaque 4

COSMOPOLITAN_C_START_

struct UrlView {
  size_t n;
  char *p;
};

struct UrlParams {
  size_t n;
  struct UrlParam {
    struct UrlView key;
    struct UrlView val;
  } * p;
};

struct Url {
  struct UrlView scheme; /* must be [A-Za-z][-+.0-9A-Za-z]* or empty */
  struct UrlView user;   /* depends on host non-absence */
  struct UrlView pass;   /* depends on user non-absence */
  struct UrlView host;   /* or reg_name */
  struct UrlView port;   /* depends on host non-absence */
  struct UrlView path;   /* or opaque_part */
  struct UrlParams params;
  struct UrlView fragment;
};

char *EncodeUrl(struct Url *, size_t *);
char *ParseUrl(const char *, size_t, struct Url *, int);
char *ParseParams(const char *, size_t, struct UrlParams *);
char *ParseHost(const char *, size_t, struct Url *);
char *EscapeUrlView(char *, struct UrlView *, const char[256]);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_NET_HTTP_URL_H_ */
