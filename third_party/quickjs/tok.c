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
#include "libc/str/str.h"
#include "third_party/quickjs/internal.h"
#include "third_party/quickjs/libregexp.h"

asm(".ident\t\"\\n\\n\
QuickJS (MIT License)\\n\
Copyright (c) 2017-2021 Fabrice Bellard\\n\
Copyright (c) 2017-2021 Charlie Gordon\"");
asm(".include \"libc/disclaimer.inc\"");
/* clang-format off */

/* 'c' is the first character. Return JS_ATOM_NULL in case of error */
static JSAtom parse_ident(JSParseState *s, const uint8_t **pp,
                          BOOL *pident_has_escape, int c, BOOL is_private)
{
    const uint8_t *p, *p1;
    char ident_buf[128], *buf;
    size_t ident_size, ident_pos;
    JSAtom atom;
    p = *pp;
    buf = ident_buf;
    ident_size = sizeof(ident_buf);
    ident_pos = 0;
    if (is_private)
        buf[ident_pos++] = '#';
    for(;;) {
        p1 = p;
        if (c < 128) {
            buf[ident_pos++] = c;
        } else {
            ident_pos += unicode_to_utf8((uint8_t*)buf + ident_pos, c);
        }
        c = *p1++;
        if (c == '\\' && *p1 == 'u') {
            c = lre_parse_escape(&p1, TRUE);
            *pident_has_escape = TRUE;
        } else if (c >= 128) {
            c = unicode_from_utf8(p, UTF8_CHAR_LEN_MAX, &p1);
        }
        if (!lre_js_is_ident_next(c))
            break;
        p = p1;
        if (UNLIKELY(ident_pos >= ident_size - UTF8_CHAR_LEN_MAX)) {
            if (ident_realloc(s->ctx, &buf, &ident_size, ident_buf)) {
                atom = JS_ATOM_NULL;
                goto done;
            }
        }
    }
    atom = JS_NewAtomLen(s->ctx, buf, ident_pos);
 done:
    if (UNLIKELY(buf != ident_buf))
        js_free(s->ctx, buf);
    *pp = p;
    return atom;
}

void free_token(JSParseState *s, JSToken *token)
{
    switch(token->val) {
#ifdef CONFIG_BIGNUM
    case TOK_NUMBER:
        JS_FreeValue(s->ctx, token->u.num.val);
        break;
#endif
    case TOK_STRING:
    case TOK_TEMPLATE:
        JS_FreeValue(s->ctx, token->u.str.str);
        break;
    case TOK_REGEXP:
        JS_FreeValue(s->ctx, token->u.regexp.body);
        JS_FreeValue(s->ctx, token->u.regexp.flags);
        break;
    case TOK_IDENT:
    case TOK_PRIVATE_NAME:
        JS_FreeAtom(s->ctx, token->u.ident.atom);
        break;
    default:
        if (token->val >= TOK_FIRST_KEYWORD &&
            token->val <= TOK_LAST_KEYWORD) {
            JS_FreeAtom(s->ctx, token->u.ident.atom);
        }
        break;
    }
}

int next_token(JSParseState *s)
{
    const uint8_t *p;
    int c;
    BOOL ident_has_escape;
    JSAtom atom;
    if (js_check_stack_overflow(s->ctx->rt, 0)) {
        return js_parse_error(s, "stack overflow");
    }
    free_token(s, &s->token);
    p = s->last_ptr = s->buf_ptr;
    s->got_lf = FALSE;
    s->last_line_num = s->token.line_num;
 redo:
    s->token.line_num = s->line_num;
    s->token.ptr = p;
    c = *p;
    switch(c) {
    case 0:
        if (p >= s->buf_end) {
            s->token.val = TOK_EOF;
        } else {
            goto def_token;
        }
        break;
    case '`':
        if (js_parse_template_part(s, p + 1))
            goto fail;
        p = s->buf_ptr;
        break;
    case '\'':
    case '\"':
        if (js_parse_string(s, c, TRUE, p + 1, &s->token, &p))
            goto fail;
        break;
    case '\r':  /* accept DOS and MAC newline sequences */
        if (p[1] == '\n') {
            p++;
        }
        /* fall thru */
    case '\n':
        p++;
    line_terminator:
        s->got_lf = TRUE;
        s->line_num++;
        goto redo;
    case '\f':
    case '\v':
    case ' ':
    case '\t':
        p++;
        goto redo;
    case '/':
        if (p[1] == '*') {
            /* comment */
            p += 2;
            for(;;) {
                if (*p == '\0' && p >= s->buf_end) {
                    js_parse_error(s, "unexpected end of comment");
                    goto fail;
                }
                if (p[0] == '*' && p[1] == '/') {
                    p += 2;
                    break;
                }
                if (*p == '\n') {
                    s->line_num++;
                    s->got_lf = TRUE; /* considered as LF for ASI */
                    p++;
                } else if (*p == '\r') {
                    s->got_lf = TRUE; /* considered as LF for ASI */
                    p++;
                } else if (*p >= 0x80) {
                    c = unicode_from_utf8(p, UTF8_CHAR_LEN_MAX, &p);
                    if (c == CP_LS || c == CP_PS) {
                        s->got_lf = TRUE; /* considered as LF for ASI */
                    } else if (c == -1) {
                        p++; /* skip invalid UTF-8 */
                    }
                } else {
                    p++;
                }
            }
            goto redo;
        } else if (p[1] == '/') {
            /* line comment */
            p += 2;
        skip_line_comment:
            for(;;) {
                if (*p == '\0' && p >= s->buf_end)
                    break;
                if (*p == '\r' || *p == '\n')
                    break;
                if (*p >= 0x80) {
                    c = unicode_from_utf8(p, UTF8_CHAR_LEN_MAX, &p);
                    /* LS or PS are considered as line terminator */
                    if (c == CP_LS || c == CP_PS) {
                        break;
                    } else if (c == -1) {
                        p++; /* skip invalid UTF-8 */
                    }
                } else {
                    p++;
                }
            }
            goto redo;
        } else if (p[1] == '=') {
            p += 2;
            s->token.val = TOK_DIV_ASSIGN;
        } else {
            p++;
            s->token.val = c;
        }
        break;
    case '\\':
        if (p[1] == 'u') {
            const uint8_t *p1 = p + 1;
            int c1 = lre_parse_escape(&p1, TRUE);
            if (c1 >= 0 && lre_js_is_ident_first(c1)) {
                c = c1;
                p = p1;
                ident_has_escape = TRUE;
                goto has_ident;
            } else {
                /* XXX: syntax error? */
            }
        }
        goto def_token;
    case 'a': case 'b': case 'c': case 'd':
    case 'e': case 'f': case 'g': case 'h':
    case 'i': case 'j': case 'k': case 'l':
    case 'm': case 'n': case 'o': case 'p':
    case 'q': case 'r': case 's': case 't':
    case 'u': case 'v': case 'w': case 'x':
    case 'y': case 'z':
    case 'A': case 'B': case 'C': case 'D':
    case 'E': case 'F': case 'G': case 'H':
    case 'I': case 'J': case 'K': case 'L':
    case 'M': case 'N': case 'O': case 'P':
    case 'Q': case 'R': case 'S': case 'T':
    case 'U': case 'V': case 'W': case 'X':
    case 'Y': case 'Z':
    case '_':
    case '$':
        /* identifier */
        p++;
        ident_has_escape = FALSE;
    has_ident:
        atom = parse_ident(s, &p, &ident_has_escape, c, FALSE);
        if (atom == JS_ATOM_NULL)
            goto fail;
        s->token.u.ident.atom = atom;
        s->token.u.ident.has_escape = ident_has_escape;
        s->token.u.ident.is_reserved = FALSE;
        if (s->token.u.ident.atom <= JS_ATOM_LAST_KEYWORD ||
            (s->token.u.ident.atom <= JS_ATOM_LAST_STRICT_KEYWORD &&
             (s->cur_func->js_mode & JS_MODE_STRICT)) ||
            (s->token.u.ident.atom == JS_ATOM_yield &&
             ((s->cur_func->func_kind & JS_FUNC_GENERATOR) ||
              (s->cur_func->func_type == JS_PARSE_FUNC_ARROW &&
               !s->cur_func->in_function_body && s->cur_func->parent &&
               (s->cur_func->parent->func_kind & JS_FUNC_GENERATOR)))) ||
            (s->token.u.ident.atom == JS_ATOM_await &&
             (s->is_module ||
              (((s->cur_func->func_kind & JS_FUNC_ASYNC) ||
                (s->cur_func->func_type == JS_PARSE_FUNC_ARROW &&
                 !s->cur_func->in_function_body && s->cur_func->parent &&
                 (s->cur_func->parent->func_kind & JS_FUNC_ASYNC))))))) {
                  if (ident_has_escape) {
                      s->token.u.ident.is_reserved = TRUE;
                      s->token.val = TOK_IDENT;
                  } else {
                      /* The keywords atoms are pre allocated */
                      s->token.val = s->token.u.ident.atom - 1 + TOK_FIRST_KEYWORD;
                  }
        } else {
            s->token.val = TOK_IDENT;
        }
        break;
    case '#':
        /* private name */
        {
            const uint8_t *p1;
            p++;
            p1 = p;
            c = *p1++;
            if (c == '\\' && *p1 == 'u') {
                c = lre_parse_escape(&p1, TRUE);
            } else if (c >= 128) {
                c = unicode_from_utf8(p, UTF8_CHAR_LEN_MAX, &p1);
            }
            if (!lre_js_is_ident_first(c)) {
                js_parse_error(s, "invalid first character of private name");
                goto fail;
            }
            p = p1;
            ident_has_escape = FALSE; /* not used */
            atom = parse_ident(s, &p, &ident_has_escape, c, TRUE);
            if (atom == JS_ATOM_NULL)
                goto fail;
            s->token.u.ident.atom = atom;
            s->token.val = TOK_PRIVATE_NAME;
        }
        break;
    case '.':
        if (p[1] == '.' && p[2] == '.') {
            p += 3;
            s->token.val = TOK_ELLIPSIS;
            break;
        }
        if (p[1] >= '0' && p[1] <= '9') {
            goto parse_number;
        } else {
            goto def_token;
        }
        break;
    case '0':
        /* in strict mode, octal literals are not accepted */
        if (isdigit(p[1]) && (s->cur_func->js_mode & JS_MODE_STRICT)) {
            js_parse_error(s, "octal literals are deprecated in strict mode");
            goto fail;
        }
        goto parse_number;
    case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8':
    case '9':
        /* number */
    parse_number:
        {
            JSValue ret;
            const uint8_t *p1;
            int flags, radix;
            flags = ATOD_ACCEPT_BIN_OCT | ATOD_ACCEPT_LEGACY_OCTAL |
                ATOD_ACCEPT_UNDERSCORES;
#ifdef CONFIG_BIGNUM
            flags |= ATOD_ACCEPT_SUFFIX;
            if (s->cur_func->js_mode & JS_MODE_MATH) {
                flags |= ATOD_MODE_BIGINT;
                if (s->cur_func->js_mode & JS_MODE_MATH)
                    flags |= ATOD_TYPE_BIG_FLOAT;
            }
#endif
            radix = 0;
#ifdef CONFIG_BIGNUM
            s->token.u.num.exponent = 0;
            ret = js_atof2(s->ctx, (const char *)p, (const char **)&p, radix,
                           flags, &s->token.u.num.exponent);
#else
            ret = js_atof(s->ctx, (const char *)p, (const char **)&p, radix,
                          flags);
#endif
            if (JS_IsException(ret))
                goto fail;
            /* reject `10instanceof Number` */
            if (JS_VALUE_IS_NAN(ret) ||
                lre_js_is_ident_next(unicode_from_utf8(p, UTF8_CHAR_LEN_MAX, &p1))) {
                JS_FreeValue(s->ctx, ret);
                js_parse_error(s, "invalid number literal");
                goto fail;
            }
            s->token.val = TOK_NUMBER;
            s->token.u.num.val = ret;
        }
        break;
    case '*':
        if (p[1] == '=') {
            p += 2;
            s->token.val = TOK_MUL_ASSIGN;
        } else if (p[1] == '*') {
            if (p[2] == '=') {
                p += 3;
                s->token.val = TOK_POW_ASSIGN;
            } else {
                p += 2;
                s->token.val = TOK_POW;
            }
        } else {
            goto def_token;
        }
        break;
    case '%':
        if (p[1] == '=') {
            p += 2;
            s->token.val = TOK_MOD_ASSIGN;
        } else {
            goto def_token;
        }
        break;
    case '+':
        if (p[1] == '=') {
            p += 2;
            s->token.val = TOK_PLUS_ASSIGN;
        } else if (p[1] == '+') {
            p += 2;
            s->token.val = TOK_INC;
        } else {
            goto def_token;
        }
        break;
    case '-':
        if (p[1] == '=') {
            p += 2;
            s->token.val = TOK_MINUS_ASSIGN;
        } else if (p[1] == '-') {
            if (s->allow_html_comments &&
                p[2] == '>' && s->last_line_num != s->line_num) {
                /* Annex B: `-->` at beginning of line is an html comment end.
                   It extends to the end of the line.
                 */
                goto skip_line_comment;
            }
            p += 2;
            s->token.val = TOK_DEC;
        } else {
            goto def_token;
        }
        break;
    case '<':
        if (p[1] == '=') {
            p += 2;
            s->token.val = TOK_LTE;
        } else if (p[1] == '<') {
            if (p[2] == '=') {
                p += 3;
                s->token.val = TOK_SHL_ASSIGN;
            } else {
                p += 2;
                s->token.val = TOK_SHL;
            }
        } else if (s->allow_html_comments &&
                   p[1] == '!' && p[2] == '-' && p[3] == '-') {
            /* Annex B: handle `<!--` single line html comments */
            goto skip_line_comment;
        } else {
            goto def_token;
        }
        break;
    case '>':
        if (p[1] == '=') {
            p += 2;
            s->token.val = TOK_GTE;
        } else if (p[1] == '>') {
            if (p[2] == '>') {
                if (p[3] == '=') {
                    p += 4;
                    s->token.val = TOK_SHR_ASSIGN;
                } else {
                    p += 3;
                    s->token.val = TOK_SHR;
                }
            } else if (p[2] == '=') {
                p += 3;
                s->token.val = TOK_SAR_ASSIGN;
            } else {
                p += 2;
                s->token.val = TOK_SAR;
            }
        } else {
            goto def_token;
        }
        break;
    case '=':
        if (p[1] == '=') {
            if (p[2] == '=') {
                p += 3;
                s->token.val = TOK_STRICT_EQ;
            } else {
                p += 2;
                s->token.val = TOK_EQ;
            }
        } else if (p[1] == '>') {
            p += 2;
            s->token.val = TOK_ARROW;
        } else {
            goto def_token;
        }
        break;
    case '!':
        if (p[1] == '=') {
            if (p[2] == '=') {
                p += 3;
                s->token.val = TOK_STRICT_NEQ;
            } else {
                p += 2;
                s->token.val = TOK_NEQ;
            }
        } else {
            goto def_token;
        }
        break;
    case '&':
        if (p[1] == '=') {
            p += 2;
            s->token.val = TOK_AND_ASSIGN;
        } else if (p[1] == '&') {
            if (p[2] == '=') {
                p += 3;
                s->token.val = TOK_LAND_ASSIGN;
            } else {
                p += 2;
                s->token.val = TOK_LAND;
            }
        } else {
            goto def_token;
        }
        break;
#ifdef CONFIG_BIGNUM
        /* in math mode, '^' is the power operator. '^^' is always the
           xor operator and '**' is always the power operator */
    case '^':
        if (p[1] == '=') {
            p += 2;
            if (s->cur_func->js_mode & JS_MODE_MATH)
                s->token.val = TOK_MATH_POW_ASSIGN;
            else
                s->token.val = TOK_XOR_ASSIGN;
        } else if (p[1] == '^') {
            if (p[2] == '=') {
                p += 3;
                s->token.val = TOK_XOR_ASSIGN;
            } else {
                p += 2;
                s->token.val = '^';
            }
        } else {
            p++;
            if (s->cur_func->js_mode & JS_MODE_MATH)
                s->token.val = TOK_MATH_POW;
            else
                s->token.val = '^';
        }
        break;
#else
    case '^':
        if (p[1] == '=') {
            p += 2;
            s->token.val = TOK_XOR_ASSIGN;
        } else {
            goto def_token;
        }
        break;
#endif
    case '|':
        if (p[1] == '=') {
            p += 2;
            s->token.val = TOK_OR_ASSIGN;
        } else if (p[1] == '|') {
            if (p[2] == '=') {
                p += 3;
                s->token.val = TOK_LOR_ASSIGN;
            } else {
                p += 2;
                s->token.val = TOK_LOR;
            }
        } else {
            goto def_token;
        }
        break;
    case '?':
        if (p[1] == '?') {
            if (p[2] == '=') {
                p += 3;
                s->token.val = TOK_DOUBLE_QUESTION_MARK_ASSIGN;
            } else {
                p += 2;
                s->token.val = TOK_DOUBLE_QUESTION_MARK;
            }
        } else if (p[1] == '.' && !(p[2] >= '0' && p[2] <= '9')) {
            p += 2;
            s->token.val = TOK_QUESTION_MARK_DOT;
        } else {
            goto def_token;
        }
        break;
    default:
        if (c >= 128) {
            /* unicode value */
            c = unicode_from_utf8(p, UTF8_CHAR_LEN_MAX, &p);
            switch(c) {
            case CP_PS:
            case CP_LS:
                /* XXX: should avoid incrementing line_number, but
                   needed to handle HTML comments */
                goto line_terminator;
            default:
                if (lre_is_space(c)) {
                    goto redo;
                } else if (lre_js_is_ident_first(c)) {
                    ident_has_escape = FALSE;
                    goto has_ident;
                } else {
                    js_parse_error(s, "unexpected character");
                    goto fail;
                }
            }
        }
    def_token:
        s->token.val = c;
        p++;
        break;
    }
    s->buf_ptr = p;
    //    dump_token(s, &s->token);
    return 0;
 fail:
    s->token.val = TOK_ERROR;
    return -1;
}

/* only used for ':' and '=>', 'let' or 'function' look-ahead. *pp is
   only set if TOK_IMPORT is returned */
/* XXX: handle all unicode cases */
int simple_next_token(const uint8_t **pp, BOOL no_line_terminator)
{
    const uint8_t *p;
    uint32_t c;
    /* skip spaces and comments */
    p = *pp;
    for (;;) {
        switch(c = *p++) {
        case '\r':
        case '\n':
            if (no_line_terminator)
                return '\n';
            continue;
        case ' ':
        case '\t':
        case '\v':
        case '\f':
            continue;
        case '/':
            if (*p == '/') {
                if (no_line_terminator)
                    return '\n';
                while (*p && *p != '\r' && *p != '\n')
                    p++;
                continue;
            }
            if (*p == '*') {
                while (*++p) {
                    if ((*p == '\r' || *p == '\n') && no_line_terminator)
                        return '\n';
                    if (*p == '*' && p[1] == '/') {
                        p += 2;
                        break;
                    }
                }
                continue;
            }
            break;
        case '=':
            if (*p == '>')
                return TOK_ARROW;
            break;
        default:
            if (lre_js_is_ident_first(c)) {
                if (c == 'i') {
                    if (p[0] == 'n' && !lre_js_is_ident_next(p[1])) {
                        return TOK_IN;
                    }
                    if (p[0] == 'm' && p[1] == 'p' && p[2] == 'o' &&
                        p[3] == 'r' && p[4] == 't' &&
                        !lre_js_is_ident_next(p[5])) {
                        *pp = p + 5;
                        return TOK_IMPORT;
                    }
                } else if (c == 'o' && *p == 'f' && !lre_js_is_ident_next(p[1])) {
                    return TOK_OF;
                } else if (c == 'e' &&
                           p[0] == 'x' && p[1] == 'p' && p[2] == 'o' &&
                           p[3] == 'r' && p[4] == 't' &&
                           !lre_js_is_ident_next(p[5])) {
                    *pp = p + 5;
                    return TOK_EXPORT;
                } else if (c == 'f' && p[0] == 'u' && p[1] == 'n' &&
                         p[2] == 'c' && p[3] == 't' && p[4] == 'i' &&
                         p[5] == 'o' && p[6] == 'n' && !lre_js_is_ident_next(p[7])) {
                    return TOK_FUNCTION;
                }
                return TOK_IDENT;
            }
            break;
        }
        return c;
    }
}
