#ifndef COSMOPOLITAN_THIRD_PARTY_QUICKJS_CUTILS_H_
#define COSMOPOLITAN_THIRD_PARTY_QUICKJS_CUTILS_H_
#include "libc/intrin/bswap.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
/* clang-format off */

/* set if CPU is big endian */
#undef WORDS_BIGENDIAN

#define __maybe_unused __attribute__((__unused__))

#define xglue(x, y) x ## y
#define glue(x, y) xglue(x, y)
#define stringify(s)    tostring(s)
#define tostring(s)     #s

#ifndef countof
#define countof(x) (sizeof(x) / sizeof((x)[0]))
#endif

typedef int BOOL;

#ifndef FALSE
enum {
    FALSE = 0,
    TRUE = 1,
};
#endif

void pstrcpy(char *, int, const char *);
char *pstrcat(char *, int, const char *);
int strstart(const char *, const char *, const char **);
int has_suffix(const char *, const char *);

static inline int max_int(int a, int b)
{
    if (a > b)
        return a;
    else
        return b;
}

static inline int min_int(int a, int b)
{
    if (a < b)
        return a;
    else
        return b;
}

static inline uint32_t max_uint32(uint32_t a, uint32_t b)
{
    if (a > b)
        return a;
    else
        return b;
}

static inline uint32_t min_uint32(uint32_t a, uint32_t b)
{
    if (a < b)
        return a;
    else
        return b;
}

static inline int64_t max_int64(int64_t a, int64_t b)
{
    if (a > b)
        return a;
    else
        return b;
}

static inline int64_t min_int64(int64_t a, int64_t b)
{
    if (a < b)
        return a;
    else
        return b;
}

/* WARNING: undefined if a = 0 */
forceinline int clz32(unsigned int a)
{
    return __builtin_clz(a);
}

/* WARNING: undefined if a = 0 */
forceinline int clz64(uint64_t a)
{
    return __builtin_clzll(a);
}

/* WARNING: undefined if a = 0 */
forceinline int ctz32(unsigned int a)
{
    return __builtin_ctz(a);
}

/* WARNING: undefined if a = 0 */
forceinline int ctz64(uint64_t a)
{
    return __builtin_ctzll(a);
}

struct thatispacked packed_u64 {
    uint64_t v;
};

struct thatispacked packed_u32 {
    uint32_t v;
};

struct thatispacked packed_u16 {
    uint16_t v;
};

static inline uint64_t get_u64(const uint8_t *tab)
{
    return ((const struct packed_u64 *)tab)->v;
}

static inline int64_t get_i64(const uint8_t *tab)
{
    return (int64_t)((const struct packed_u64 *)tab)->v;
}

static inline void put_u64(uint8_t *tab, uint64_t val)
{
    ((struct packed_u64 *)tab)->v = val;
}

static inline uint32_t get_u32(const uint8_t *tab)
{
    return ((const struct packed_u32 *)tab)->v;
}

static inline int32_t get_i32(const uint8_t *tab)
{
    return (int32_t)((const struct packed_u32 *)tab)->v;
}

static inline void put_u32(uint8_t *tab, uint32_t val)
{
    ((struct packed_u32 *)tab)->v = val;
}

static inline uint32_t get_u16(const uint8_t *tab)
{
    return ((const struct packed_u16 *)tab)->v;
}

static inline int32_t get_i16(const uint8_t *tab)
{
    return (int16_t)((const struct packed_u16 *)tab)->v;
}

static inline void put_u16(uint8_t *tab, uint16_t val)
{
    ((struct packed_u16 *)tab)->v = val;
}

static inline uint32_t get_u8(const uint8_t *tab)
{
    return *tab;
}

static inline int32_t get_i8(const uint8_t *tab)
{
    return (int8_t)*tab;
}

static inline void put_u8(uint8_t *tab, uint8_t val)
{
    *tab = val;
}

forceinline uint16_t bswap16(uint16_t x)
{
    return bswap_16(x);
}

forceinline uint32_t bswap32(uint32_t v)
{
    return bswap_32(v);
}

forceinline uint64_t bswap64(uint64_t v)
{
    return bswap_64(v);
}

/* XXX: should take an extra argument to pass slack information to the caller */
typedef void *DynBufReallocFunc(void *opaque, void *ptr, size_t size);

typedef struct DynBuf {
    uint8_t *buf;
    size_t size;
    size_t allocated_size;
    BOOL error; /* true if a memory allocation error occurred */
    DynBufReallocFunc *realloc_func;
    void *opaque; /* for realloc_func */
} DynBuf;

void dbuf_init(DynBuf *);
void dbuf_init2(DynBuf *, void *, DynBufReallocFunc *);
int dbuf_realloc(DynBuf *, size_t);
int dbuf_write(DynBuf *, size_t, const uint8_t *, size_t);
int dbuf_put(DynBuf *, const uint8_t *, size_t);
int dbuf_put_self(DynBuf *, size_t, size_t);
int dbuf_putc(DynBuf *, uint8_t);
int dbuf_putstr(DynBuf *, const char *);
int dbuf_printf(DynBuf *, const char *, ...) printfesque(2);
void dbuf_free(DynBuf *);

int unicode_to_utf8(uint8_t *buf, unsigned int c);
int unicode_from_utf8(const uint8_t *p, int max_len, const uint8_t **pp);

static inline int dbuf_put_u16(DynBuf *s, uint16_t val) {
    return dbuf_put(s, (uint8_t *)&val, 2);
}

static inline int dbuf_put_u32(DynBuf *s, uint32_t val) {
    return dbuf_put(s, (uint8_t *)&val, 4);
}

static inline int dbuf_put_u64(DynBuf *s, uint64_t val) {
    return dbuf_put(s, (uint8_t *)&val, 8);
}

static inline BOOL dbuf_error(DynBuf *s) {
    return s->error;
}

static inline void dbuf_set_error(DynBuf *s) {
    s->error = TRUE;
}

#define UTF8_CHAR_LEN_MAX 6

static inline int from_hex(int c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    else if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    else
        return -1;
}

void rqsort(void *base, size_t nmemb, size_t size,
            int (*cmp)(const void *, const void *, void *),
            void *arg);

/* clang-format on */
COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_QUICKJS_CUTILS_H_ */
