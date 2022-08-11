/*
 * QuickJS Javascript Engine
 *
 * Copyright (c) 2017-2021 Fabrice Bellard
 * Copyright (c) 2017-2021 Charlie Gordon
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "libc/fmt/fmt.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "third_party/quickjs/internal.h"
#include "third_party/quickjs/libregexp.h"
#include "third_party/quickjs/quickjs.h"

asm(".ident\t\"\\n\\n\
QuickJS (MIT License)\\n\
Copyright (c) 2017-2021 Fabrice Bellard\\n\
Copyright (c) 2017-2021 Charlie Gordon\"");
asm(".include \"libc/disclaimer.inc\"");
/* clang-format off */

static void *dbuf_default_realloc(void *opaque, void *ptr, size_t size)
{
    return realloc(ptr, size);
}

void dbuf_init2(DynBuf *s, void *opaque, DynBufReallocFunc *realloc_func)
{
    bzero(s, sizeof(*s));
    if (!realloc_func)
        realloc_func = dbuf_default_realloc;
    s->opaque = opaque;
    s->realloc_func = realloc_func;
}

void dbuf_init(DynBuf *s)
{
    dbuf_init2(s, NULL, NULL);
}

/* return < 0 if error */
int dbuf_realloc(DynBuf *s, size_t new_size)
{
    size_t size;
    uint8_t *new_buf;
    if (new_size > s->allocated_size) {
        if (s->error)
            return -1;
        size = s->allocated_size * 3 / 2;
        if (size > new_size)
            new_size = size;
        new_buf = s->realloc_func(s->opaque, s->buf, new_size);
        if (!new_buf) {
            s->error = TRUE;
            return -1;
        }
        s->buf = new_buf;
        s->allocated_size = new_size;
    }
    return 0;
}

int dbuf_write(DynBuf *s, size_t offset, const uint8_t *data, size_t len)
{
    size_t end;
    end = offset + len;
    if (dbuf_realloc(s, end))
        return -1;
    memcpy(s->buf + offset, data, len);
    if (end > s->size)
        s->size = end;
    return 0;
}

int dbuf_put(DynBuf *s, const uint8_t *data, size_t len)
{
    if (UNLIKELY((s->size + len) > s->allocated_size)) {
        if (dbuf_realloc(s, s->size + len))
            return -1;
    }
    memcpy(s->buf + s->size, data, len);
    s->size += len;
    return 0;
}

int dbuf_put_self(DynBuf *s, size_t offset, size_t len)
{
    if (UNLIKELY((s->size + len) > s->allocated_size)) {
        if (dbuf_realloc(s, s->size + len))
            return -1;
    }
    memcpy(s->buf + s->size, s->buf + offset, len);
    s->size += len;
    return 0;
}

int dbuf_putc(DynBuf *s, uint8_t c)
{
    return dbuf_put(s, &c, 1);
}

int dbuf_putstr(DynBuf *s, const char *str)
{
    return dbuf_put(s, (const uint8_t *)str, strlen(str));
}

int dbuf_printf(DynBuf *s, const char *fmt, ...)
{
    va_list ap;
    char buf[128];
    int len;
    
    va_start(ap, fmt);
    len = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    if (len < sizeof(buf)) {
        /* fast case */
        return dbuf_put(s, (uint8_t *)buf, len);
    } else {
        if (dbuf_realloc(s, s->size + len + 1))
            return -1;
        va_start(ap, fmt);
        vsnprintf((char *)(s->buf + s->size), s->allocated_size - s->size,
                  fmt, ap);
        va_end(ap);
        s->size += len;
    }
    return 0;
}

void dbuf_free(DynBuf *s)
{
    /* we test s->buf as a fail safe to avoid crashing if dbuf_free()
       is called twice */
    if (s->buf) {
        s->realloc_func(s->opaque, s->buf, 0);
    }
    bzero(s, sizeof(*s));
}

void dbuf_put_leb128(DynBuf *s, uint32_t v)
{
    uint32_t a;
    for(;;) {
        a = v & 0x7f;
        v >>= 7;
        if (v != 0) {
            dbuf_putc(s, a | 0x80);
        } else {
            dbuf_putc(s, a);
            break;
        }
    }
}

void dbuf_put_sleb128(DynBuf *s, int32_t v1)
{
    uint32_t v = v1;
    dbuf_put_leb128(s, (2 * v) ^ -(v >> 31));
}
