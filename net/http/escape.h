#ifndef COSMOPOLITAN_NET_HTTP_ESCAPE_H_
#define COSMOPOLITAN_NET_HTTP_ESCAPE_H_

#define kControlWs 1
#define kControlC0 2
#define kControlC1 4

COSMOPOLITAN_C_START_

extern const char kEscapeAuthority[256];
extern const char kEscapeIp[256];
extern const char kEscapePath[256];
extern const char kEscapeSegment[256];
extern const char kEscapeParam[256];
extern const char kEscapeFragment[256];

char *EscapeHtml(const char *, size_t, size_t *);
char *EscapeUrl(const char *, size_t, size_t *, const char[256]);
char *EscapeUser(const char *, size_t, size_t *);
char *EscapePass(const char *, size_t, size_t *);
char *EscapeIp(const char *, size_t, size_t *);
char *EscapeHost(const char *, size_t, size_t *);
char *EscapePath(const char *, size_t, size_t *);
char *EscapeParam(const char *, size_t, size_t *);
char *EscapeFragment(const char *, size_t, size_t *);
char *EscapeSegment(const char *, size_t, size_t *);
char *EscapeJsStringLiteral(char **, size_t *, const char *, size_t, size_t *);

ssize_t HasControlCodes(const char *, size_t, int);
char *Underlong(const char *, size_t, size_t *);
char *DecodeLatin1(const char *, size_t, size_t *);
char *EncodeLatin1(const char *, size_t, size_t *, int);
char *EncodeHttpHeaderValue(const char *, size_t, size_t *);
char *VisualizeControlCodes(const char *, size_t, size_t *);
char *IndentLines(const char *, size_t, size_t *, size_t);
char *EncodeBase32(const char *, size_t, const char *, size_t, size_t *);
char *DecodeBase32(const char *, size_t, const char *, size_t, size_t *);
char *EncodeBase64(const char *, size_t, size_t *);
char *DecodeBase64(const char *, size_t, size_t *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_NET_HTTP_ESCAPE_H_ */
