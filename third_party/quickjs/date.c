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
#include "libc/calls/struct/timeval.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"
#include "third_party/quickjs/internal.h"
#include "third_party/quickjs/libregexp.h"
#include "third_party/quickjs/quickjs.h"

asm(".ident\t\"\\n\\n\
QuickJS (MIT License)\\n\
Copyright (c) 2017-2021 Fabrice Bellard\\n\
Copyright (c) 2017-2021 Charlie Gordon\"");
asm(".include \"libc/disclaimer.inc\"");
/* clang-format off */

#if 0
/* OS dependent: return the UTC time in ms since 1970. */
static JSValue js___date_now(JSContext *ctx, JSValueConst this_val,
                             int argc, JSValueConst *argv)
{
    int64_t d;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    d = (int64_t)tv.tv_sec * 1000 + (tv.tv_usec / 1000);
    return JS_NewInt64(ctx, d);
}
#endif

/* OS dependent: return the UTC time in microseconds since 1970. */
JSValue js___date_clock(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int64_t d;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    d = (int64_t)tv.tv_sec * 1000000 + tv.tv_usec;
    return JS_NewInt64(ctx, d);
}

/* OS dependent. d = argv[0] is in ms from 1970. Return the difference
   between UTC time and local time 'd' in minutes */
static int getTimezoneOffset(int64_t time) {
#if defined(_WIN32)
    /* XXX: TODO */
    return 0;
#else
    time_t ti;
    struct tm tm;
    time /= 1000; /* convert to seconds */
    if (sizeof(time_t) == 4) {
        /* on 32-bit systems, we need to clamp the time value to the
           range of `time_t`. This is better than truncating values to
           32 bits and hopefully provides the same result as 64-bit
           implementation of localtime_r.
         */
        if ((time_t)-1 < 0) {
            if (time < INT32_MIN) {
                time = INT32_MIN;
            } else if (time > INT32_MAX) {
                time = INT32_MAX;
            }
        } else {
            if (time < 0) {
                time = 0;
            } else if (time > UINT32_MAX) {
                time = UINT32_MAX;
            }
        }
    }
    ti = time;
    localtime_r(&ti, &tm);
    return -tm.tm_gmtoff / 60;
#endif
}

#if 0
static JSValue js___date_getTimezoneOffset(JSContext *ctx, JSValueConst this_val,
                                           int argc, JSValueConst *argv)
{
    double dd;

    if (JS_ToFloat64(ctx, &dd, argv[0]))
        return JS_EXCEPTION;
    if (isnan(dd))
        return __JS_NewFloat64(ctx, dd);
    else
        return JS_NewInt32(ctx, getTimezoneOffset((int64_t)dd));
}

static JSValue js_get_prototype_from_ctor(JSContext *ctx, JSValueConst ctor,
                                          JSValueConst def_proto)
{
    JSValue proto;
    proto = JS_GetProperty(ctx, ctor, JS_ATOM_prototype);
    if (JS_IsException(proto))
        return proto;
    if (!JS_IsObject(proto)) {
        JS_FreeValue(ctx, proto);
        proto = JS_DupValue(ctx, def_proto);
    }
    return proto;
}

/* create a new date object */
static JSValue js___date_create(JSContext *ctx, JSValueConst this_val,
                                int argc, JSValueConst *argv)
{
    JSValue obj, proto;
    proto = js_get_prototype_from_ctor(ctx, argv[0], argv[1]);
    if (JS_IsException(proto))
        return proto;
    obj = JS_NewObjectProtoClass(ctx, proto, JS_CLASS_DATE);
    JS_FreeValue(ctx, proto);
    if (!JS_IsException(obj))
        JS_SetObjectData(ctx, obj, JS_DupValue(ctx, argv[2]));
    return obj;
}
#endif

static int64_t math_mod(int64_t a, int64_t b) {
    /* return positive modulo */
    int64_t m = a % b;
    return m + (m < 0) * b;
}

static int64_t floor_div(int64_t a, int64_t b) {
    /* integer division rounding toward -Infinity */
    int64_t m = a % b;
    return (a - (m + (m < 0) * b)) / b;
}

static JSValue js_Date_parse(JSContext *ctx, JSValueConst this_val,
                             int argc, JSValueConst *argv);

static __exception int JS_ThisTimeValue(JSContext *ctx, double *valp, JSValueConst this_val)
{
    if (JS_VALUE_GET_TAG(this_val) == JS_TAG_OBJECT) {
        JSObject *p = JS_VALUE_GET_OBJ(this_val);
        if (p->class_id == JS_CLASS_DATE && JS_IsNumber(p->u.object_data))
            return JS_ToFloat64(ctx, valp, p->u.object_data);
    }
    JS_ThrowTypeError(ctx, "not a Date object");
    return -1;
}

static JSValue JS_SetThisTimeValue(JSContext *ctx, JSValueConst this_val, double v)
{
    if (JS_VALUE_GET_TAG(this_val) == JS_TAG_OBJECT) {
        JSObject *p = JS_VALUE_GET_OBJ(this_val);
        if (p->class_id == JS_CLASS_DATE) {
            JS_FreeValue(ctx, p->u.object_data);
            p->u.object_data = JS_NewFloat64(ctx, v);
            return JS_DupValue(ctx, p->u.object_data);
        }
    }
    return JS_ThrowTypeError(ctx, "not a Date object");
}

static int64_t days_from_year(int64_t y) {
    return 365 * (y - 1970) + floor_div(y - 1969, 4) -
        floor_div(y - 1901, 100) + floor_div(y - 1601, 400);
}

static int64_t days_in_year(int64_t y) {
    return 365 + !(y % 4) - !(y % 100) + !(y % 400);
}

/* return the year, update days */
static int64_t year_from_days(int64_t *days) {
    int64_t y, d1, nd, d = *days;
    y = floor_div(d * 10000, 3652425) + 1970;
    /* the initial approximation is very good, so only a few
       iterations are necessary */
    for(;;) {
        d1 = d - days_from_year(y);
        if (d1 < 0) {
            y--;
            d1 += days_in_year(y);
        } else {
            nd = days_in_year(y);
            if (d1 < nd)
                break;
            d1 -= nd;
            y++;
        }
    }
    *days = d1;
    return y;
}

static int const month_days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static char const month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
static char const day_names[] = "SunMonTueWedThuFriSat";

static __exception int get_date_fields(JSContext *ctx, JSValueConst obj,
                                       double fields[9], int is_local, int force)
{
    double dval;
    int64_t d, days, wd, y, i, md, h, m, s, ms, tz = 0;
    if (JS_ThisTimeValue(ctx, &dval, obj))
        return -1;
    if (isnan(dval)) {
        if (!force)
            return FALSE; /* NaN */
        d = 0;        /* initialize all fields to 0 */
    } else {
        d = dval;
        if (is_local) {
            tz = -getTimezoneOffset(d);
            d += tz * 60000;
        }
    }
    /* result is >= 0, we can use % */
    h = math_mod(d, 86400000);
    days = (d - h) / 86400000;
    ms = h % 1000;
    h = (h - ms) / 1000;
    s = h % 60;
    h = (h - s) / 60;
    m = h % 60;
    h = (h - m) / 60;
    wd = math_mod(days + 4, 7); /* week day */
    y = year_from_days(&days);
    for(i = 0; i < 11; i++) {
        md = month_days[i];
        if (i == 1)
            md += days_in_year(y) - 365;
        if (days < md)
            break;
        days -= md;
    }
    fields[0] = y;
    fields[1] = i;
    fields[2] = days + 1;
    fields[3] = h;
    fields[4] = m;
    fields[5] = s;
    fields[6] = ms;
    fields[7] = wd;
    fields[8] = tz;
    return TRUE;
}

static double time_clip(double t) {
    if (t >= -8.64e15 && t <= 8.64e15)
        return trunc(t) + 0.0;  /* convert -0 to +0 */
    else
        return NAN;
}

/* The spec mandates the use of 'double' and it fixes the order
   of the operations */
static double set_date_fields(double fields[], int is_local) {
    int64_t y;
    double days, d, h, m1;
    int i, m, md;
    m1 = fields[1];
    m = fmod(m1, 12);
    if (m < 0)
        m += 12;
    y = (int64_t)(fields[0] + floor(m1 / 12));
    days = days_from_year(y);
    for(i = 0; i < m; i++) {
        md = month_days[i];
        if (i == 1)
            md += days_in_year(y) - 365;
        days += md;
    }
    days += fields[2] - 1;
    h = fields[3] * 3600000 + fields[4] * 60000 +
        fields[5] * 1000 + fields[6];
    d = days * 86400000 + h;
    if (is_local)
        d += getTimezoneOffset(d) * 60000;
    return time_clip(d);
}

static JSValue get_date_field(JSContext *ctx, JSValueConst this_val,
                              int argc, JSValueConst *argv, int magic)
{
    // get_date_field(obj, n, is_local)
    double fields[9];
    int res, n, is_local;
    is_local = magic & 0x0F;
    n = (magic >> 4) & 0x0F;
    res = get_date_fields(ctx, this_val, fields, is_local, 0);
    if (res < 0)
        return JS_EXCEPTION;
    if (!res)
        return JS_NAN;
    if (magic & 0x100) {    // getYear
        fields[0] -= 1900;
    }
    return JS_NewFloat64(ctx, fields[n]);
}

static JSValue set_date_field(JSContext *ctx, JSValueConst this_val,
                              int argc, JSValueConst *argv, int magic)
{
    // _field(obj, first_field, end_field, args, is_local)
    double fields[9];
    int res, first_field, end_field, is_local, i, n;
    double d, a;
    d = NAN;
    first_field = (magic >> 8) & 0x0F;
    end_field = (magic >> 4) & 0x0F;
    is_local = magic & 0x0F;
    res = get_date_fields(ctx, this_val, fields, is_local, first_field == 0);
    if (res < 0)
        return JS_EXCEPTION;
    if (res && argc > 0) {
        n = end_field - first_field;
        if (argc < n)
            n = argc;
        for(i = 0; i < n; i++) {
            if (JS_ToFloat64(ctx, &a, argv[i]))
                return JS_EXCEPTION;
            if (!isfinite(a))
                goto done;
            fields[first_field + i] = trunc(a);
        }
        d = set_date_fields(fields, is_local);
    }
done:
    return JS_SetThisTimeValue(ctx, this_val, d);
}

/* fmt:
   0: toUTCString: "Tue, 02 Jan 2018 23:04:46 GMT"
   1: toString: "Wed Jan 03 2018 00:05:22 GMT+0100 (CET)"
   2: toISOString: "2018-01-02T23:02:56.927Z"
   3: toLocaleString: "1/2/2018, 11:40:40 PM"
   part: 1=date, 2=time 3=all
   XXX: should use a variant of strftime().
 */
static JSValue get_date_string(JSContext *ctx, JSValueConst this_val,
                               int argc, JSValueConst *argv, int magic)
{
    // _string(obj, fmt, part)
    char buf[64];
    double fields[9];
    int res, fmt, part, pos;
    int y, mon, d, h, m, s, ms, wd, tz;
    fmt = (magic >> 4) & 0x0F;
    part = magic & 0x0F;
    res = get_date_fields(ctx, this_val, fields, fmt & 1, 0);
    if (res < 0)
        return JS_EXCEPTION;
    if (!res) {
        if (fmt == 2)
            return JS_ThrowRangeError(ctx, "Date value is NaN");
        else
            return JS_NewString(ctx, "Invalid Date");
    }
    y = fields[0];
    mon = fields[1];
    d = fields[2];
    h = fields[3];
    m = fields[4];
    s = fields[5];
    ms = fields[6];
    wd = fields[7];
    tz = fields[8];
    pos = 0;
    if (part & 1) { /* date part */
        switch(fmt) {
        case 0:
            pos += snprintf(buf + pos, sizeof(buf) - pos,
                            "%.3s, %02d %.3s %0*d ",
                            day_names + wd * 3, d,
                            month_names + mon * 3, 4 + (y < 0), y);
            break;
        case 1:
            pos += snprintf(buf + pos, sizeof(buf) - pos,
                            "%.3s %.3s %02d %0*d",
                            day_names + wd * 3,
                            month_names + mon * 3, d, 4 + (y < 0), y);
            if (part == 3) {
                buf[pos++] = ' ';
            }
            break;
        case 2:
            if (y >= 0 && y <= 9999) {
                pos += snprintf(buf + pos, sizeof(buf) - pos,
                                "%04d", y);
            } else {
                pos += snprintf(buf + pos, sizeof(buf) - pos,
                                "%+07d", y);
            }
            pos += snprintf(buf + pos, sizeof(buf) - pos,
                            "-%02d-%02dT", mon + 1, d);
            break;
        case 3:
            pos += snprintf(buf + pos, sizeof(buf) - pos,
                            "%02d/%02d/%0*d", mon + 1, d, 4 + (y < 0), y);
            if (part == 3) {
                buf[pos++] = ',';
                buf[pos++] = ' ';
            }
            break;
        }
    }
    if (part & 2) { /* time part */
        switch(fmt) {
        case 0:
            pos += snprintf(buf + pos, sizeof(buf) - pos,
                            "%02d:%02d:%02d GMT", h, m, s);
            break;
        case 1:
            pos += snprintf(buf + pos, sizeof(buf) - pos,
                            "%02d:%02d:%02d GMT", h, m, s);
            if (tz < 0) {
                buf[pos++] = '-';
                tz = -tz;
            } else {
                buf[pos++] = '+';
            }
            /* tz is >= 0, can use % */
            pos += snprintf(buf + pos, sizeof(buf) - pos,
                            "%02d%02d", tz / 60, tz % 60);
            /* XXX: tack the time zone code? */
            break;
        case 2:
            pos += snprintf(buf + pos, sizeof(buf) - pos,
                            "%02d:%02d:%02d.%03dZ", h, m, s, ms);
            break;
        case 3:
            pos += snprintf(buf + pos, sizeof(buf) - pos,
                            "%02d:%02d:%02d %cM", (h + 1) % 12 - 1, m, s,
                            (h < 12) ? 'A' : 'P');
            break;
        }
    }
    return JS_NewStringLen(ctx, buf, pos);
}

/* OS dependent: return the UTC time in ms since 1970. */
static int64_t date_now(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)tv.tv_sec * 1000 + (tv.tv_usec / 1000);
}

static JSValue js_date_constructor(JSContext *ctx, JSValueConst new_target,
                                   int argc, JSValueConst *argv)
{
    // Date(y, mon, d, h, m, s, ms)
    JSValue rv;
    int i, n;
    double a, val;
    if (JS_IsUndefined(new_target)) {
        /* invoked as function */
        argc = 0;
    }
    n = argc;
    if (n == 0) {
        val = date_now();
    } else if (n == 1) {
        JSValue v, dv;
        if (JS_VALUE_GET_TAG(argv[0]) == JS_TAG_OBJECT) {
            JSObject *p = JS_VALUE_GET_OBJ(argv[0]);
            if (p->class_id == JS_CLASS_DATE && JS_IsNumber(p->u.object_data)) {
                if (JS_ToFloat64(ctx, &val, p->u.object_data))
                    return JS_EXCEPTION;
                val = time_clip(val);
                goto has_val;
            }
        }
        v = JS_ToPrimitive(ctx, argv[0], HINT_NONE);
        if (JS_IsString(v)) {
            dv = js_Date_parse(ctx, JS_UNDEFINED, 1, (JSValueConst *)&v);
            JS_FreeValue(ctx, v);
            if (JS_IsException(dv))
                return JS_EXCEPTION;
            if (JS_ToFloat64Free(ctx, &val, dv))
                return JS_EXCEPTION;
        } else {
            if (JS_ToFloat64Free(ctx, &val, v))
                return JS_EXCEPTION;
        }
        val = time_clip(val);
    } else {
        double fields[] = { 0, 0, 1, 0, 0, 0, 0 };
        if (n > 7)
            n = 7;
        for(i = 0; i < n; i++) {
            if (JS_ToFloat64(ctx, &a, argv[i]))
                return JS_EXCEPTION;
            if (!isfinite(a))
                break;
            fields[i] = trunc(a);
            if (i == 0 && fields[0] >= 0 && fields[0] < 100)
                fields[0] += 1900;
        }
        val = (i == n) ? set_date_fields(fields, 1) : NAN;
    }
has_val:
#if 0
    JSValueConst args[3];
    args[0] = new_target;
    args[1] = ctx->class_proto[JS_CLASS_DATE];
    args[2] = JS_NewFloat64(ctx, val);
    rv = js___date_create(ctx, JS_UNDEFINED, 3, args);
#else
    rv = js_create_from_ctor(ctx, new_target, JS_CLASS_DATE);
    if (!JS_IsException(rv))
        JS_SetObjectData(ctx, rv, JS_NewFloat64(ctx, val));
#endif
    if (!JS_IsException(rv) && JS_IsUndefined(new_target)) {
        /* invoked as a function, return (new Date()).toString(); */
        JSValue s;
        s = get_date_string(ctx, rv, 0, NULL, 0x13);
        JS_FreeValue(ctx, rv);
        rv = s;
    }
    return rv;
}

static JSValue js_Date_UTC(JSContext *ctx, JSValueConst this_val,
                           int argc, JSValueConst *argv)
{
    // UTC(y, mon, d, h, m, s, ms)
    double fields[] = { 0, 0, 1, 0, 0, 0, 0 };
    int i, n;
    double a;
    n = argc;
    if (n == 0)
        return JS_NAN;
    if (n > 7)
        n = 7;
    for(i = 0; i < n; i++) {
        if (JS_ToFloat64(ctx, &a, argv[i]))
            return JS_EXCEPTION;
        if (!isfinite(a))
            return JS_NAN;
        fields[i] = trunc(a);
        if (i == 0 && fields[0] >= 0 && fields[0] < 100)
            fields[0] += 1900;
    }
    return JS_NewFloat64(ctx, set_date_fields(fields, 0));
}

static void string_skip_spaces(JSString *sp, int *pp) {
    while (*pp < sp->len && string_get(sp, *pp) == ' ')
        *pp += 1;
}

static void string_skip_non_spaces(JSString *sp, int *pp) {
    while (*pp < sp->len && string_get(sp, *pp) != ' ')
        *pp += 1;
}

/* parse a numeric field with an optional sign if accept_sign is TRUE */
static int string_get_digits(JSString *sp, int *pp, int64_t *pval) {
    int64_t v = 0;
    int c, p = *pp, p_start;
    if (p >= sp->len)
        return -1;
    p_start = p;
    while (p < sp->len) {
        c = string_get(sp, p);
        if (!(c >= '0' && c <= '9')) {
            if (p == p_start)
                return -1;
            else
                break;
        }
        v = v * 10 + c - '0';
        p++;
    }
    *pval = v;
    *pp = p;
    return 0;
}

static int string_get_signed_digits(JSString *sp, int *pp, int64_t *pval) {
    int res, sgn, p = *pp;
    if (p >= sp->len)
        return -1;
    sgn = string_get(sp, p);
    if (sgn == '-' || sgn == '+')
        p++;
    res = string_get_digits(sp, &p, pval);
    if (res == 0 && sgn == '-')
        *pval = -*pval;
    *pp = p;
    return res;
}

/* parse a fixed width numeric field */
static int string_get_fixed_width_digits(JSString *sp, int *pp, int n, int64_t *pval) {
    int64_t v = 0;
    int i, c, p = *pp;
    for(i = 0; i < n; i++) {
        if (p >= sp->len)
            return -1;
        c = string_get(sp, p);
        if (!(c >= '0' && c <= '9'))
            return -1;
        v = v * 10 + c - '0';
        p++;
    }
    *pval = v;
    *pp = p;
    return 0;
}

static int string_get_milliseconds(JSString *sp, int *pp, int64_t *pval) {
    /* parse milliseconds as a fractional part, round to nearest */
    /* XXX: the spec does not indicate which rounding should be used */
    int mul = 1000, ms = 0, p = *pp, c, p_start;
    if (p >= sp->len)
        return -1;
    p_start = p;
    while (p < sp->len) {
        c = string_get(sp, p);
        if (!(c >= '0' && c <= '9')) {
            if (p == p_start)
                return -1;
            else
                break;
        }
        if (mul == 1 && c >= '5')
            ms += 1;
        ms += (c - '0') * (mul /= 10);
        p++;
    }
    *pval = ms;
    *pp = p;
    return 0;
}


static int find_abbrev(JSString *sp, int p, const char *list, int count) {
    int n, i;
    if (p + 3 <= sp->len) {
        for (n = 0; n < count; n++) {
            for (i = 0; i < 3; i++) {
                if (string_get(sp, p + i) != month_names[n * 3 + i])
                    goto next;
            }
            return n;
        next:;
        }
    }
    return -1;
}

static int string_get_month(JSString *sp, int *pp, int64_t *pval) {
    int n;
    string_skip_spaces(sp, pp);
    n = find_abbrev(sp, *pp, month_names, 12);
    if (n < 0)
        return -1;
    *pval = n;
    *pp += 3;
    return 0;
}

static JSValue js_Date_parse(JSContext *ctx, JSValueConst this_val,
                             int argc, JSValueConst *argv)
{
    // parse(s)
    JSValue s, rv;
    int64_t fields[] = { 0, 1, 1, 0, 0, 0, 0 };
    double fields1[7];
    int64_t tz, hh, mm;
    double d;
    int p, i, c, sgn, l;
    JSString *sp;
    BOOL is_local;
    rv = JS_NAN;
    s = JS_ToString(ctx, argv[0]);
    if (JS_IsException(s))
        return JS_EXCEPTION;
    sp = JS_VALUE_GET_STRING(s);
    p = 0;
    if (p < sp->len && (((c = string_get(sp, p)) >= '0' && c <= '9') || c == '+' || c == '-')) {
        /* ISO format */
        /* year field can be negative */
        if (string_get_signed_digits(sp, &p, &fields[0]))
            goto done;
        for (i = 1; i < 7; i++) {
            if (p >= sp->len)
                break;
            switch(i) {
            case 1:
            case 2:
                c = '-';
                break;
            case 3:
                c = 'T';
                break;
            case 4:
            case 5:
                c = ':';
                break;
            case 6:
                c = '.';
                break;
            }
            if (string_get(sp, p) != c)
                break;
            p++;
            if (i == 6) {
                if (string_get_milliseconds(sp, &p, &fields[i]))
                    goto done;
            } else {
                if (string_get_digits(sp, &p, &fields[i]))
                    goto done;
            }
        }
        /* no time: UTC by default */
        is_local = (i > 3);
        fields[1] -= 1;
        /* parse the time zone offset if present: [+-]HH:mm or [+-]HHmm */
        tz = 0;
        if (p < sp->len) {
            sgn = string_get(sp, p);
            if (sgn == '+' || sgn == '-') {
                p++;
                l = sp->len - p;
                if (l != 4 && l != 5)
                    goto done;
                if (string_get_fixed_width_digits(sp, &p, 2, &hh))
                    goto done;
                if (l == 5) {
                    if (string_get(sp, p) != ':')
                        goto done;
                    p++;
                }
                if (string_get_fixed_width_digits(sp, &p, 2, &mm))
                    goto done;
                tz = hh * 60 + mm;
                if (sgn == '-')
                    tz = -tz;
                is_local = FALSE;
            } else if (sgn == 'Z') {
                p++;
                is_local = FALSE;
            } else {
                goto done;
            }
            /* error if extraneous characters */
            if (p != sp->len)
                goto done;
        }
    } else {
        /* toString or toUTCString format */
        /* skip the day of the week */
        string_skip_non_spaces(sp, &p);
        string_skip_spaces(sp, &p);
        if (p >= sp->len)
            goto done;
        c = string_get(sp, p);
        if (c >= '0' && c <= '9') {
            /* day of month first */
            if (string_get_digits(sp, &p, &fields[2]))
                goto done;
            if (string_get_month(sp, &p, &fields[1]))
                goto done;
        } else {
            /* month first */
            if (string_get_month(sp, &p, &fields[1]))
                goto done;
            string_skip_spaces(sp, &p);
            if (string_get_digits(sp, &p, &fields[2]))
                goto done;
        }
        /* year */
        string_skip_spaces(sp, &p);
        if (string_get_signed_digits(sp, &p, &fields[0]))
            goto done;
        /* hour, min, seconds */
        string_skip_spaces(sp, &p);
        for(i = 0; i < 3; i++) {
            if (i == 1 || i == 2) {
                if (p >= sp->len)
                    goto done;
                if (string_get(sp, p) != ':')
                    goto done;
                p++;
            }
            if (string_get_digits(sp, &p, &fields[3 + i]))
                goto done;
        }
        // XXX: parse optional milliseconds?
        /* parse the time zone offset if present: [+-]HHmm */
        is_local = FALSE;
        tz = 0;
        for (tz = 0; p < sp->len; p++) {
            sgn = string_get(sp, p);
            if (sgn == '+' || sgn == '-') {
                p++;
                if (string_get_fixed_width_digits(sp, &p, 2, &hh))
                    goto done;
                if (string_get_fixed_width_digits(sp, &p, 2, &mm))
                    goto done;
                tz = hh * 60 + mm;
                if (sgn == '-')
                    tz = -tz;
                break;
            }
        }
    }
    for(i = 0; i < 7; i++)
        fields1[i] = fields[i];
    d = set_date_fields(fields1, is_local) - tz * 60000;
    rv = JS_NewFloat64(ctx, d);
done:
    JS_FreeValue(ctx, s);
    return rv;
}

static JSValue js_Date_now(JSContext *ctx, JSValueConst this_val,
                           int argc, JSValueConst *argv)
{
    // now()
    return JS_NewInt64(ctx, date_now());
}

static JSValue js_date_Symbol_toPrimitive(JSContext *ctx, JSValueConst this_val,
                                          int argc, JSValueConst *argv)
{
    // Symbol_toPrimitive(hint)
    JSValueConst obj = this_val;
    JSAtom hint = JS_ATOM_NULL;
    int hint_num;
    if (!JS_IsObject(obj))
        return JS_ThrowTypeErrorNotAnObject(ctx);
    if (JS_IsString(argv[0])) {
        hint = JS_ValueToAtom(ctx, argv[0]);
        if (hint == JS_ATOM_NULL)
            return JS_EXCEPTION;
        JS_FreeAtom(ctx, hint);
    }
    switch (hint) {
    case JS_ATOM_number:
#ifdef CONFIG_BIGNUM
    case JS_ATOM_integer:
#endif
        hint_num = HINT_NUMBER;
        break;
    case JS_ATOM_string:
    case JS_ATOM_default:
        hint_num = HINT_STRING;
        break;
    default:
        return JS_ThrowTypeError(ctx, "invalid hint");
    }
    return JS_ToPrimitive(ctx, obj, hint_num | HINT_FORCE_ORDINARY);
}

static JSValue js_date_getTimezoneOffset(JSContext *ctx, JSValueConst this_val,
                                         int argc, JSValueConst *argv)
{
    // getTimezoneOffset()
    double v;
    if (JS_ThisTimeValue(ctx, &v, this_val))
        return JS_EXCEPTION;
    if (isnan(v))
        return JS_NAN;
    else
        return JS_NewInt64(ctx, getTimezoneOffset((int64_t)trunc(v)));
}

static JSValue js_date_getTime(JSContext *ctx, JSValueConst this_val,
                               int argc, JSValueConst *argv)
{
    // getTime()
    double v;

    if (JS_ThisTimeValue(ctx, &v, this_val))
        return JS_EXCEPTION;
    return JS_NewFloat64(ctx, v);
}

static JSValue js_date_setTime(JSContext *ctx, JSValueConst this_val,
                               int argc, JSValueConst *argv)
{
    // setTime(v)
    double v;

    if (JS_ThisTimeValue(ctx, &v, this_val) || JS_ToFloat64(ctx, &v, argv[0]))
        return JS_EXCEPTION;
    return JS_SetThisTimeValue(ctx, this_val, time_clip(v));
}

static JSValue js_date_setYear(JSContext *ctx, JSValueConst this_val,
                               int argc, JSValueConst *argv)
{
    // setYear(y)
    double y;
    JSValueConst args[1];

    if (JS_ThisTimeValue(ctx, &y, this_val) || JS_ToFloat64(ctx, &y, argv[0]))
        return JS_EXCEPTION;
    y = +y;
    if (isfinite(y)) {
        y = trunc(y);
        if (y >= 0 && y < 100)
            y += 1900;
    }
    args[0] = JS_NewFloat64(ctx, y);
    return set_date_field(ctx, this_val, 1, args, 0x011);
}

static JSValue js_date_toJSON(JSContext *ctx, JSValueConst this_val,
                              int argc, JSValueConst *argv)
{
    // toJSON(key)
    JSValue obj, tv, method, rv;
    double d;
    rv = JS_EXCEPTION;
    tv = JS_UNDEFINED;
    obj = JS_ToObject(ctx, this_val);
    tv = JS_ToPrimitive(ctx, obj, HINT_NUMBER);
    if (JS_IsException(tv))
        goto exception;
    if (JS_IsNumber(tv)) {
        if (JS_ToFloat64(ctx, &d, tv) < 0)
            goto exception;
        if (!isfinite(d)) {
            rv = JS_NULL;
            goto done;
        }
    }
    method = JS_GetPropertyStr(ctx, obj, "toISOString");
    if (JS_IsException(method))
        goto exception;
    if (!JS_IsFunction(ctx, method)) {
        JS_ThrowTypeError(ctx, "object needs toISOString method");
        JS_FreeValue(ctx, method);
        goto exception;
    }
    rv = JS_CallFree(ctx, method, obj, 0, NULL);
exception:
done:
    JS_FreeValue(ctx, obj);
    JS_FreeValue(ctx, tv);
    return rv;
}

static const JSCFunctionListEntry js_date_funcs[] = {
    JS_CFUNC_DEF("now", 0, js_Date_now ),
    JS_CFUNC_DEF("parse", 1, js_Date_parse ),
    JS_CFUNC_DEF("UTC", 7, js_Date_UTC ),
};

static const JSCFunctionListEntry js_date_proto_funcs[] = {
    JS_CFUNC_DEF("valueOf", 0, js_date_getTime ),
    JS_CFUNC_MAGIC_DEF("toString", 0, get_date_string, 0x13 ),
    JS_CFUNC_DEF("[Symbol.toPrimitive]", 1, js_date_Symbol_toPrimitive ),
    JS_CFUNC_MAGIC_DEF("toUTCString", 0, get_date_string, 0x03 ),
    JS_ALIAS_DEF("toGMTString", "toUTCString" ),
    JS_CFUNC_MAGIC_DEF("toISOString", 0, get_date_string, 0x23 ),
    JS_CFUNC_MAGIC_DEF("toDateString", 0, get_date_string, 0x11 ),
    JS_CFUNC_MAGIC_DEF("toTimeString", 0, get_date_string, 0x12 ),
    JS_CFUNC_MAGIC_DEF("toLocaleString", 0, get_date_string, 0x33 ),
    JS_CFUNC_MAGIC_DEF("toLocaleDateString", 0, get_date_string, 0x31 ),
    JS_CFUNC_MAGIC_DEF("toLocaleTimeString", 0, get_date_string, 0x32 ),
    JS_CFUNC_DEF("getTimezoneOffset", 0, js_date_getTimezoneOffset ),
    JS_CFUNC_DEF("getTime", 0, js_date_getTime ),
    JS_CFUNC_MAGIC_DEF("getYear", 0, get_date_field, 0x101 ),
    JS_CFUNC_MAGIC_DEF("getFullYear", 0, get_date_field, 0x01 ),
    JS_CFUNC_MAGIC_DEF("getUTCFullYear", 0, get_date_field, 0x00 ),
    JS_CFUNC_MAGIC_DEF("getMonth", 0, get_date_field, 0x11 ),
    JS_CFUNC_MAGIC_DEF("getUTCMonth", 0, get_date_field, 0x10 ),
    JS_CFUNC_MAGIC_DEF("getDate", 0, get_date_field, 0x21 ),
    JS_CFUNC_MAGIC_DEF("getUTCDate", 0, get_date_field, 0x20 ),
    JS_CFUNC_MAGIC_DEF("getHours", 0, get_date_field, 0x31 ),
    JS_CFUNC_MAGIC_DEF("getUTCHours", 0, get_date_field, 0x30 ),
    JS_CFUNC_MAGIC_DEF("getMinutes", 0, get_date_field, 0x41 ),
    JS_CFUNC_MAGIC_DEF("getUTCMinutes", 0, get_date_field, 0x40 ),
    JS_CFUNC_MAGIC_DEF("getSeconds", 0, get_date_field, 0x51 ),
    JS_CFUNC_MAGIC_DEF("getUTCSeconds", 0, get_date_field, 0x50 ),
    JS_CFUNC_MAGIC_DEF("getMilliseconds", 0, get_date_field, 0x61 ),
    JS_CFUNC_MAGIC_DEF("getUTCMilliseconds", 0, get_date_field, 0x60 ),
    JS_CFUNC_MAGIC_DEF("getDay", 0, get_date_field, 0x71 ),
    JS_CFUNC_MAGIC_DEF("getUTCDay", 0, get_date_field, 0x70 ),
    JS_CFUNC_DEF("setTime", 1, js_date_setTime ),
    JS_CFUNC_MAGIC_DEF("setMilliseconds", 1, set_date_field, 0x671 ),
    JS_CFUNC_MAGIC_DEF("setUTCMilliseconds", 1, set_date_field, 0x670 ),
    JS_CFUNC_MAGIC_DEF("setSeconds", 2, set_date_field, 0x571 ),
    JS_CFUNC_MAGIC_DEF("setUTCSeconds", 2, set_date_field, 0x570 ),
    JS_CFUNC_MAGIC_DEF("setMinutes", 3, set_date_field, 0x471 ),
    JS_CFUNC_MAGIC_DEF("setUTCMinutes", 3, set_date_field, 0x470 ),
    JS_CFUNC_MAGIC_DEF("setHours", 4, set_date_field, 0x371 ),
    JS_CFUNC_MAGIC_DEF("setUTCHours", 4, set_date_field, 0x370 ),
    JS_CFUNC_MAGIC_DEF("setDate", 1, set_date_field, 0x231 ),
    JS_CFUNC_MAGIC_DEF("setUTCDate", 1, set_date_field, 0x230 ),
    JS_CFUNC_MAGIC_DEF("setMonth", 2, set_date_field, 0x131 ),
    JS_CFUNC_MAGIC_DEF("setUTCMonth", 2, set_date_field, 0x130 ),
    JS_CFUNC_DEF("setYear", 1, js_date_setYear ),
    JS_CFUNC_MAGIC_DEF("setFullYear", 3, set_date_field, 0x031 ),
    JS_CFUNC_MAGIC_DEF("setUTCFullYear", 3, set_date_field, 0x030 ),
    JS_CFUNC_DEF("toJSON", 1, js_date_toJSON ),
};

void JS_AddIntrinsicDate(JSContext *ctx)
{
    JSValueConst obj;
    /* Date */
    ctx->class_proto[JS_CLASS_DATE] = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, ctx->class_proto[JS_CLASS_DATE], js_date_proto_funcs,
                               countof(js_date_proto_funcs));
    obj = JS_NewGlobalCConstructor(ctx, "Date", js_date_constructor, 7,
                                   ctx->class_proto[JS_CLASS_DATE]);
    JS_SetPropertyFunctionList(ctx, obj, js_date_funcs, countof(js_date_funcs));
}
