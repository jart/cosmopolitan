#ifndef COSMOPOLITAN_NET_HTTP_ESCAPE_H_
#define COSMOPOLITAN_NET_HTTP_ESCAPE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct EscapeResult {
  char *data;
  size_t size;
};

struct EscapeResult EscapeHtml(const char *, size_t);
struct EscapeResult EscapeUrl(const char *, size_t, const char[hasatleast 256]);
struct EscapeResult EscapeUrlPath(const char *, size_t);
struct EscapeResult EscapeUrlParam(const char *, size_t);
struct EscapeResult EscapeUrlFragment(const char *, size_t);
struct EscapeResult EscapeUrlPathSegment(const char *, size_t);
struct EscapeResult EscapeJsStringLiteral(const char *, size_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_NET_HTTP_ESCAPE_H_ */
