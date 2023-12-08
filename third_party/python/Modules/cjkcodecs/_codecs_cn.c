/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/yoink.h"
#include "third_party/python/Modules/cjkcodecs/cjkcodecs.h"

PYTHON_PROVIDE("_codecs_cn");
PYTHON_PROVIDE("_codecs_cn.__map_gb18030ext");
PYTHON_PROVIDE("_codecs_cn.__map_gb2312");
PYTHON_PROVIDE("_codecs_cn.__map_gbcommon");
PYTHON_PROVIDE("_codecs_cn.__map_gbkext");
PYTHON_PROVIDE("_codecs_cn.getcodec");

/*
 * _codecs_cn.c: Codecs collection for Mainland Chinese encodings
 *
 * Written by Hye-Shik "Bourne to Macro" Chang <perky@FreeBSD.org>
 */

static const struct _gb18030_to_unibmp_ranges {
    Py_UCS4   first, last;
    DBCHAR       base;
} gb18030_to_unibmp_ranges[] = {
{128,163,0},{165,166,36},{169,175,38},{178,182,45},{184,214,50},{216,223,81},{
226,231,89},{235,235,95},{238,241,96},{244,246,100},{248,248,103},{251,251,104
},{253,256,105},{258,274,109},{276,282,126},{284,298,133},{300,323,148},{325,
327,172},{329,332,175},{334,362,179},{364,461,208},{463,463,306},{465,465,307
},{467,467,308},{469,469,309},{471,471,310},{473,473,311},{475,475,312},{477,
504,313},{506,592,341},{594,608,428},{610,710,443},{712,712,544},{716,728,545
},{730,912,558},{930,930,741},{938,944,742},{962,962,749},{970,1024,750},{1026
,1039,805},{1104,1104,819},{1106,8207,820},{8209,8210,7922},{8215,8215,7924},{
8218,8219,7925},{8222,8228,7927},{8231,8239,7934},{8241,8241,7943},{8244,8244,
7944},{8246,8250,7945},{8252,8363,7950},{8365,8450,8062},{8452,8452,8148},{
8454,8456,8149},{8458,8469,8152},{8471,8480,8164},{8482,8543,8174},{8556,8559,
8236},{8570,8591,8240},{8596,8597,8262},{8602,8711,8264},{8713,8718,8374},{
8720,8720,8380},{8722,8724,8381},{8726,8729,8384},{8731,8732,8388},{8737,8738,
8390},{8740,8740,8392},{8742,8742,8393},{8748,8749,8394},{8751,8755,8396},{
8760,8764,8401},{8766,8775,8406},{8777,8779,8416},{8781,8785,8419},{8787,8799,
8424},{8802,8803,8437},{8808,8813,8439},{8816,8852,8445},{8854,8856,8482},{
8858,8868,8485},{8870,8894,8496},{8896,8977,8521},{8979,9311,8603},{9322,9331,
8936},{9372,9471,8946},{9548,9551,9046},{9588,9600,9050},{9616,9618,9063},{
9622,9631,9066},{9634,9649,9076},{9652,9659,9092},{9662,9669,9100},{9672,9674,
9108},{9676,9677,9111},{9680,9697,9113},{9702,9732,9131},{9735,9736,9162},{
9738,9791,9164},{9793,9793,9218},{9795,11904,9219},{11906,11907,11329},{11909,
11911,11331},{11913,11914,11334},{11917,11926,11336},{11928,11942,11346},{
11944,11945,11361},{11947,11949,11363},{11951,11954,11366},{11956,11957,11370
},{11960,11962,11372},{11964,11977,11375},{11979,12271,11389},{12284,12287,
11682},{12292,12292,11686},{12312,12316,11687},{12319,12320,11692},{12330,
12349,11694},{12351,12352,11714},{12436,12442,11716},{12447,12448,11723},{
12535,12539,11725},{12543,12548,11730},{12586,12831,11736},{12842,12848,11982
},{12850,12962,11989},{12964,13197,12102},{13200,13211,12336},{13215,13216,
12348},{13218,13251,12350},{13253,13261,12384},{13263,13264,12393},{13267,
13268,12395},{13270,13382,12397},{13384,13426,12510},{13428,13725,12553},{
13727,13837,12851},{13839,13849,12962},{13851,14615,12973},{14617,14701,13738
},{14703,14798,13823},{14801,14814,13919},{14816,14962,13933},{14964,15181,
14080},{15183,15469,14298},{15471,15583,14585},{15585,16469,14698},{16471,
16734,15583},{16736,17206,15847},{17208,17323,16318},{17325,17328,16434},{
17330,17372,16438},{17374,17621,16481},{17623,17995,16729},{17997,18016,17102
},{18018,18210,17122},{18212,18216,17315},{18218,18299,17320},{18301,18316,
17402},{18318,18758,17418},{18760,18809,17859},{18811,18812,17909},{18814,
18817,17911},{18820,18820,17915},{18823,18842,17916},{18844,18846,17936},{
18848,18869,17939},{18872,19574,17961},{19576,19614,18664},{19620,19730,18703
},{19738,19885,18814},{19887,19967,18962},{40870,55295,19043},{59244,59244,
33469},{59336,59336,33470},{59367,59379,33471},{59413,59413,33484},{59417,
59421,33485},{59423,59429,33490},{59431,59434,33497},{59437,59440,33501},{
59443,59450,33505},{59452,59458,33513},{59460,59475,33520},{59478,59491,33536
},{59493,63787,33550},{63789,63864,37845},{63866,63892,37921},{63894,63974,
37948},{63976,63984,38029},{63986,64011,38038},{64016,64016,38064},{64018,
64018,38065},{64021,64023,38066},{64025,64030,38069},{64034,64034,38075},{
64037,64038,38076},{64042,65071,38078},{65074,65074,39108},{65093,65096,39109
},{65107,65107,39113},{65112,65112,39114},{65127,65127,39115},{65132,65280,
39116},{65375,65503,39265},{65510,65535,39394},{0,0,39420}};

/* GBK and GB2312 map differently in few code points that are listed below:
 *
 *              gb2312                          gbk
 * A1A4         U+30FB KATAKANA MIDDLE DOT      U+00B7 MIDDLE DOT
 * A1AA         U+2015 HORIZONTAL BAR           U+2014 EM DASH
 * A844         undefined                       U+2015 HORIZONTAL BAR
 */

#define GBK_DECODE(dc1, dc2, writer)                                \
    if ((dc1) == 0xa1 && (dc2) == 0xaa) {                           \
        OUTCHAR(0x2014);                                            \
    }                                                               \
    else if ((dc1) == 0xa8 && (dc2) == 0x44) {                      \
        OUTCHAR(0x2015);                                            \
    }                                                               \
    else if ((dc1) == 0xa1 && (dc2) == 0xa4) {                      \
        OUTCHAR(0x00b7);                                            \
    }                                                               \
    else if (TRYMAP_DEC(gb2312, decoded, dc1 ^ 0x80, dc2 ^ 0x80)) { \
        OUTCHAR(decoded);                                           \
    }                                                               \
    else if (TRYMAP_DEC(gbkext, decoded, dc1, dc2)) {               \
        OUTCHAR(decoded);                                           \
    }

#define GBK_ENCODE(code, assi)                                         \
    if ((code) == 0x2014) {                                            \
        (assi) = 0xa1aa;                                               \
    } else if ((code) == 0x2015) {                                     \
        (assi) = 0xa844;                                               \
    } else if ((code) == 0x00b7) {                                     \
        (assi) = 0xa1a4;                                               \
    } else if ((code) != 0x30fb && TRYMAP_ENC(gbcommon, assi, code)) { \
        ;                                                              \
    }

/*
 * GB2312 codec
 */

ENCODER(gb2312)
{
    while (*inpos < inlen) {
        Py_UCS4 c = INCHAR1;
        DBCHAR code;

        if (c < 0x80) {
            WRITEBYTE1((unsigned char)c);
            NEXT(1, 1);
            continue;
        }

        if (c > 0xFFFF)
            return 1;

        REQUIRE_OUTBUF(2);
        if (TRYMAP_ENC(gbcommon, code, c))
            ;
        else
            return 1;

        if (code & 0x8000) /* MSB set: GBK */
            return 1;

        OUTBYTE1((code >> 8) | 0x80);
        OUTBYTE2((code & 0xFF) | 0x80);
        NEXT(1, 2);
    }

    return 0;
}

DECODER(gb2312)
{
    while (inleft > 0) {
        unsigned char c = **inbuf;
        Py_UCS4 decoded;

        if (c < 0x80) {
            OUTCHAR(c);
            NEXT_IN(1);
            continue;
        }

        REQUIRE_INBUF(2);
        if (TRYMAP_DEC(gb2312, decoded, c ^ 0x80, INBYTE2 ^ 0x80)) {
            OUTCHAR(decoded);
            NEXT_IN(2);
        }
        else
            return 1;
    }

    return 0;
}


/*
 * GBK codec
 */

ENCODER(gbk)
{
    while (*inpos < inlen) {
        Py_UCS4 c = INCHAR1;
        DBCHAR code;

        if (c < 0x80) {
            WRITEBYTE1((unsigned char)c);
            NEXT(1, 1);
            continue;
        }

        if (c > 0xFFFF)
            return 1;

        REQUIRE_OUTBUF(2);

        GBK_ENCODE(c, code)
        else
            return 1;

        OUTBYTE1((code >> 8) | 0x80);
        if (code & 0x8000)
            OUTBYTE2((code & 0xFF)); /* MSB set: GBK */
        else
            OUTBYTE2((code & 0xFF) | 0x80); /* MSB unset: GB2312 */
        NEXT(1, 2);
    }

    return 0;
}

DECODER(gbk)
{
    while (inleft > 0) {
        unsigned char c = INBYTE1;
        Py_UCS4 decoded;

        if (c < 0x80) {
            OUTCHAR(c);
            NEXT_IN(1);
            continue;
        }

        REQUIRE_INBUF(2);

        GBK_DECODE(c, INBYTE2, writer)
        else
            return 1;

        NEXT_IN(2);
    }

    return 0;
}


/*
 * GB18030 codec
 */

ENCODER(gb18030)
{
    while (*inpos < inlen) {
        Py_UCS4 c = INCHAR1;
        DBCHAR code;

        if (c < 0x80) {
            WRITEBYTE1(c);
            NEXT(1, 1);
            continue;
        }

        if (c >= 0x10000) {
            Py_UCS4 tc = c - 0x10000;
            assert (c <= 0x10FFFF);

            REQUIRE_OUTBUF(4);

            OUTBYTE4((unsigned char)(tc % 10) + 0x30);
            tc /= 10;
            OUTBYTE3((unsigned char)(tc % 126) + 0x81);
            tc /= 126;
            OUTBYTE2((unsigned char)(tc % 10) + 0x30);
            tc /= 10;
            OUTBYTE1((unsigned char)(tc + 0x90));

            NEXT(1, 4);
            continue;
        }

        REQUIRE_OUTBUF(2);

        GBK_ENCODE(c, code)
        else if (TRYMAP_ENC(gb18030ext, code, c))
            ;
        else {
            const struct _gb18030_to_unibmp_ranges *utrrange;

            REQUIRE_OUTBUF(4);

            for (utrrange = gb18030_to_unibmp_ranges;
                 utrrange->first != 0;
                 utrrange++)
                if (utrrange->first <= c &&
                    c <= utrrange->last) {
                    Py_UCS4 tc;

                    tc = c - utrrange->first +
                         utrrange->base;

                    OUTBYTE4((unsigned char)(tc % 10) + 0x30);
                    tc /= 10;
                    OUTBYTE3((unsigned char)(tc % 126) + 0x81);
                    tc /= 126;
                    OUTBYTE2((unsigned char)(tc % 10) + 0x30);
                    tc /= 10;
                    OUTBYTE1((unsigned char)tc + 0x81);

                    NEXT(1, 4);
                    break;
                }

            if (utrrange->first == 0)
                return 1;
            continue;
        }

        OUTBYTE1((code >> 8) | 0x80);
        if (code & 0x8000)
            OUTBYTE2((code & 0xFF)); /* MSB set: GBK or GB18030ext */
        else
            OUTBYTE2((code & 0xFF) | 0x80); /* MSB unset: GB2312 */

        NEXT(1, 2);
    }

    return 0;
}

DECODER(gb18030)
{
    while (inleft > 0) {
        unsigned char c = INBYTE1, c2;
        Py_UCS4 decoded;

        if (c < 0x80) {
            OUTCHAR(c);
            NEXT_IN(1);
            continue;
        }

        REQUIRE_INBUF(2);

        c2 = INBYTE2;
        if (c2 >= 0x30 && c2 <= 0x39) { /* 4 bytes seq */
            const struct _gb18030_to_unibmp_ranges *utr;
            unsigned char c3, c4;
            Py_UCS4 lseq;

            REQUIRE_INBUF(4);
            c3 = INBYTE3;
            c4 = INBYTE4;
            if (c  < 0x81 || c  > 0xFE ||
                c3 < 0x81 || c3 > 0xFE ||
                c4 < 0x30 || c4 > 0x39)
                return 1;
            c -= 0x81;  c2 -= 0x30;
            c3 -= 0x81; c4 -= 0x30;

            if (c < 4) { /* U+0080 - U+FFFF */
                lseq = ((Py_UCS4)c * 10 + c2) * 1260 +
                    (Py_UCS4)c3 * 10 + c4;
                if (lseq < 39420) {
                    for (utr = gb18030_to_unibmp_ranges;
                         lseq >= (utr + 1)->base;
                         utr++) ;
                    OUTCHAR(utr->first - utr->base + lseq);
                    NEXT_IN(4);
                    continue;
                }
            }
            else if (c >= 15) { /* U+10000 - U+10FFFF */
                lseq = 0x10000 + (((Py_UCS4)c-15) * 10 + c2)
                    * 1260 + (Py_UCS4)c3 * 10 + c4;
                if (lseq <= 0x10FFFF) {
                    OUTCHAR(lseq);
                    NEXT_IN(4);
                    continue;
                }
            }
            return 1;
        }

        GBK_DECODE(c, c2, writer)
        else if (TRYMAP_DEC(gb18030ext, decoded, c, c2))
            OUTCHAR(decoded);
        else
            return 1;

        NEXT_IN(2);
    }

    return 0;
}


/*
 * HZ codec
 */

ENCODER_INIT(hz)
{
    state->i = 0;
    return 0;
}

ENCODER_RESET(hz)
{
    if (state->i != 0) {
        WRITEBYTE2('~', '}');
        state->i = 0;
        NEXT_OUT(2);
    }
    return 0;
}

ENCODER(hz)
{
    while (*inpos < inlen) {
        Py_UCS4 c = INCHAR1;
        DBCHAR code;

        if (c < 0x80) {
            if (state->i) {
                WRITEBYTE2('~', '}');
                NEXT_OUT(2);
                state->i = 0;
            }
            WRITEBYTE1((unsigned char)c);
            NEXT(1, 1);
            if (c == '~') {
                WRITEBYTE1('~');
                NEXT_OUT(1);
            }
            continue;
        }

        if (c > 0xFFFF)
            return 1;

        if (TRYMAP_ENC(gbcommon, code, c))
            ;
        else
            return 1;

        if (code & 0x8000) /* MSB set: GBK */
            return 1;

        if (state->i == 0) {
            WRITEBYTE4('~', '{', code >> 8, code & 0xff);
            NEXT(1, 4);
            state->i = 1;
        }
        else {
            WRITEBYTE2(code >> 8, code & 0xff);
            NEXT(1, 2);
        }
    }

    return 0;
}

DECODER_INIT(hz)
{
    state->i = 0;
    return 0;
}

DECODER_RESET(hz)
{
    state->i = 0;
    return 0;
}

DECODER(hz)
{
    while (inleft > 0) {
        unsigned char c = INBYTE1;
        Py_UCS4 decoded;

        if (c == '~') {
            unsigned char c2 = INBYTE2;

            REQUIRE_INBUF(2);
            if (c2 == '~' && state->i == 0)
                OUTCHAR('~');
            else if (c2 == '{' && state->i == 0)
                state->i = 1; /* set GB */
            else if (c2 == '\n' && state->i == 0)
                ; /* line-continuation */
            else if (c2 == '}' && state->i == 1)
                state->i = 0; /* set ASCII */
            else
                return 1;
            NEXT_IN(2);
            continue;
        }

        if (c & 0x80)
            return 1;

        if (state->i == 0) { /* ASCII mode */
            OUTCHAR(c);
            NEXT_IN(1);
        }
        else { /* GB mode */
            REQUIRE_INBUF(2);
            if (TRYMAP_DEC(gb2312, decoded, c, INBYTE2)) {
                OUTCHAR(decoded);
                NEXT_IN(2);
            }
            else
                return 1;
        }
    }

    return 0;
}


BEGIN_MAPPINGS_LIST
  MAPPING_DECONLY(gb2312)
  MAPPING_DECONLY(gbkext)
  MAPPING_ENCONLY(gbcommon)
  MAPPING_ENCDEC(gb18030ext)
END_MAPPINGS_LIST

BEGIN_CODECS_LIST
  CODEC_STATELESS(gb2312)
  CODEC_STATELESS(gbk)
  CODEC_STATELESS(gb18030)
  CODEC_STATEFUL(hz)
END_CODECS_LIST

I_AM_A_MODULE_FOR(cn)

#ifdef __aarch64__
_Section(".rodata.pytab.1 //")
#else
_Section(".rodata.pytab.1")
#endif
 const struct _inittab _PyImport_Inittab__codecs_cn = {
    "_codecs_cn",
    PyInit__codecs_cn,
};
