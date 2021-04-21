#ifndef COSMOPOLITAN_NET_HTTP_ESCAPE_H_
#define COSMOPOLITAN_NET_HTTP_ESCAPE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct EscapeResult {
  char *data;
  size_t size;
};

extern const signed char kHexToInt[256];
extern const char kEscapeAuthority[256];
extern const char kEscapeIp[256];
extern const char kEscapePath[256];
extern const char kEscapeSegment[256];
extern const char kEscapeParam[256];
extern const char kEscapeFragment[256];

struct EscapeResult EscapeHtml(const char *, size_t);
struct EscapeResult EscapeUrl(const char *, size_t, const char[hasatleast 256]);
struct EscapeResult EscapeUser(const char *, size_t);
struct EscapeResult EscapePass(const char *, size_t);
struct EscapeResult EscapeIp(const char *, size_t);
struct EscapeResult EscapeHost(const char *, size_t);
struct EscapeResult EscapePath(const char *, size_t);
struct EscapeResult EscapeParam(const char *, size_t);
struct EscapeResult EscapeFragment(const char *, size_t);
struct EscapeResult EscapeSegment(const char *, size_t);
struct EscapeResult EscapeJsStringLiteral(const char *, size_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_NET_HTTP_ESCAPE_H_ */
