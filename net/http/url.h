#ifndef COSMOPOLITAN_NET_HTTP_URL_H_
#define COSMOPOLITAN_NET_HTTP_URL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct UrlView {
  size_t n;
  char *p; /* not allocated; not nul terminated */
};

struct UrlParams {
  size_t n;
  struct Param {
    struct UrlView key;
    struct UrlView val; /* val.n may be SIZE_MAX */
  } * p;
};

struct Url {
  struct UrlView scheme;
  struct UrlView user;
  struct UrlView pass;
  struct UrlView host;
  struct UrlView port;
  struct UrlView path;
  struct UrlParams params;
  struct UrlView fragment;
};

char *ParseUrl(const char *, size_t, struct Url *);
char *ParseParams(const char *, size_t, struct UrlParams *);
char *ParseRequestUri(const char *, size_t, struct Url *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_NET_HTTP_URL_H_ */
