#ifndef COSMOPOLITAN_NET_HTTP_URI_H_
#define COSMOPOLITAN_NET_HTTP_URI_H_
#include "libc/dns/dns.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

enum UriScheme {
  kUriSchemeHttp = 1,
  kUriSchemeHttps,
  kUriSchemeFile,
  kUriSchemeData,
  kUriSchemeZip,
  kUriSchemeSip,
  kUriSchemeSips,
  kUriSchemeTel,
  kUriSchemeSsh,
  kUriSchemeGs,
  kUriSchemeS3
};

struct Uri {
  /*
   * e.g. "", "http", "sip", "http", "dns+http", etc.
   */
  struct UriSlice {
    /*
     * !i && !n means absent
     *  i && !n means empty
     */
    unsigned i, n;
  } scheme;

  /*
   * Holds remainder for exotic URI schemes, e.g. data.
   */
  struct UriSlice opaque;

  /*
   * e.g. sip:user@host, //user:pass@host
   */
  struct UriSlice userinfo;

  /*
   * e.g. "", "example.com", "1.2.3.4", "::1", etc.
   */
  struct UriSlice host;

  /*
   * e.g. "", "5060", "80", etc.
   */
  struct UriSlice port;

  /*
   * e.g. /dir/index.html means
   * - memcmp("/dir/index.html",
   *          p + segs.p[0].i,
   *          (segs.p[segs.i - 1].n +
   *           (segs.p[segs.i - 1].i -
   *            segs.p[0].i))) == 0
   * - memcmp("/dir", p + segs.p[0].i, segs.p[0].n) == 0
   * - memcmp("/index.html", p + segs.p[1].i, segs.p[1].n) == 0
   */
  struct UriSlices {
    unsigned i, n;
    struct UriSlice *p;
  } segs;

  /* e.g. ;lr;isup-oli=00;day=tuesday */
  struct UriKeyvals {
    unsigned i, n;
    struct UriKeyval {
      struct UriSlice k, v;
    } * p;
  } params;

  /*
   * e.g. /dir;super=rare/index.html
   *
   * let 𝑖 ∈ [0,params.i)
   * paramsegs.p[𝑖].r ∈ [0,segs.i]
   */
  struct UriRefs {
    unsigned i, n;
    struct UriRef {
      unsigned r;
    } * p;
  } paramsegs;

  /* e.g. ?boop&subject=project%20x&lol=cat */
  struct UriKeyvals queries;

  /* e.g. #anchor */
  struct UriSlice fragment;
};

int uricspn(const char *data, size_t size);
int uriparse(struct Uri *, const char *, size_t) paramsnonnull((1));
enum UriScheme urischeme(struct UriSlice, const char *)
    paramsnonnull() nosideeffect;
struct UriSlice uripath(const struct Uri *) paramsnonnull() nosideeffect;
char *urislice2cstr(char *, size_t, struct UriSlice, const char *, const char *)
    paramsnonnull((1, 4));

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_NET_HTTP_URI_H_ */
