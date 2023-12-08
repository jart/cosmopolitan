/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#define USING_IMPORTED_MAPS
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/yoink.h"
#include "third_party/python/Modules/cjkcodecs/cjkcodecs.h"
#include "third_party/python/Modules/cjkcodecs/somanyencodings.h"

PYTHON_PROVIDE("_codecs_hk");
PYTHON_PROVIDE("_codecs_hk.__map_big5hkscs");
PYTHON_PROVIDE("_codecs_hk.__map_big5hkscs_bmp");
PYTHON_PROVIDE("_codecs_hk.__map_big5hkscs_nonbmp");
PYTHON_PROVIDE("_codecs_hk.getcodec");

/*
 * _codecs_hk.c: Codecs collection for encodings from Hong Kong
 *
 * Written by Hye-Shik "Bourne to Macro" Chang <perky@FreeBSD.org>
 */

static const unsigned char big5hkscs_phint_0[] = {
32,5,95,68,15,82,130,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,208,44,4,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,192,0,4,0,0,0,0,0,0,0,0,0,0,0,0,1,22,0,15,0,0,0,0,0,
32,87,43,247,252,110,242,144,11,0,0,0,192,237,164,15,38,193,155,118,242,239,
222,251,250,247,15,50,68,175,254,239,5,0,0,0,224,251,71,128,193,2,0,132,100,4,
130,64,32,162,130,133,164,145,0,16,1,0,0,0,144,72,12,0,48,0,84,3,48,68,24,19,
53,137,38,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,64,0,32,43,153,32,16,99,40,36,
1,0,0,0,0,80,96,212,0,210,42,24,157,104,53,151,79,216,248,32,196,130,28,40,2,
0,0,0,0,214,81,10,224,0,129,134,22,67,196,53,17,55,96,230,122,109,5,12,61,0,0,
0,0,153,57,128,7,34,254,129,144,24,144,12,116,48,208,160,9,41,21,253,4,0,0,0,
0,223,128,64,8,8,176,219,196,96,237,118,125,249,29,228,211,133,166,205,5,0,0,
0,0,12,0,110,186,9,47,96,84,0,30,120,104,34,112,86,158,37,243,142,7,0,0,0,192,
94,44,188,155,223,93,108,109,4,67,96,54,74,96,216,62,7,196,200,1,0,0,0,160,
177,197,98,11,12,34,62,204,37,184,1,174,237,92,104,13,148,74,181,0,0,0,0,0,
244,3,18,17,16,68,2,53,144,235,14,153,7,209,202,5,130,161,160,0,0,0,0,52,24,
160,137,231,156,91,8,132,3,2,218,144,236,219,135,133,191,162,45,0,0,0,0,118,
58,118,98,130,148,24,1,24,125,254,141,87,39,19,210,91,55,25,12,0,0,0,0,110,
139,33,145,0,0,0,64,0,0,0,2,0,0,0,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,
0,0,0,0,0,0,0,2,0,0,0,0,0,0,142,120,110,95,63,126,221,61,247,252,155,252,174,
210,255,143,107,1,0,0,0,192,159,255,234,186,186,93,188,115,159,250,216,214,
222,37,75,94,151,218,42,1,0,0,0,224,182,153,27,216,116,230,79,21,191,41,230,
255,38,117,109,227,255,155,82,0,0,0,0,80,96,126,111,153,169,80,14,0,128,16,
216,35,0,37,16,144,244,235,117,0,0,0,0,208,219,0,160,152,178,123,6,82,32,152,
22,200,61,9,0,0,1,0,0,0,0,0,0,0,4,40,200,34,0,2,0,0,16,32,130,80,64,48,1,0,16,
0,4,0,0,0,0,74,4,1,16,20,0,128,0,4,255,253,36,
};

static const unsigned char big5hkscs_phint_12130[] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,128,2,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,
};

static const unsigned char big5hkscs_phint_21924[] = {
0,0,0,0,0,26,172,248,250,90,192,250,51,0,0,0,0,0,129,0,160,156,130,144,9,1,
180,192,176,3,86,2,160,66,45,136,1,0,0,0,0,146,119,139,96,5,201,33,6,70,56,96,
72,192,180,36,222,132,224,192,36,0,0,0,0,205,80,197,52,192,40,162,173,124,153,
24,88,18,34,196,66,162,83,142,30,0,0,0,128,52,135,11,21,209,64,250,61,0,4,210,
5,72,8,22,230,28,165,0,8,0,0,0,192,45,22,20,128,24,58,212,25,136,28,138,4,
};

/*
 * BIG5HKSCS codec
 */

CODEC_INIT(big5hkscs)
{
    return 0;
}

/*
 * There are four possible pair unicode -> big5hkscs maps as in HKSCS 2004:
 *  U+00CA U+0304 -> 8862  (U+00CA alone is mapped to 8866)
 *  U+00CA U+030C -> 8864
 *  U+00EA U+0304 -> 88a3  (U+00EA alone is mapped to 88a7)
 *  U+00EA U+030C -> 88a5
 * These are handled by not mapping tables but a hand-written code.
 */
static const DBCHAR big5hkscs_pairenc_table[4] = {0x8862, 0x8864, 0x88a3, 0x88a5};

ENCODER(big5hkscs)
{
    while (*inpos < inlen) {
        Py_UCS4 c = INCHAR1;
        DBCHAR code;
        Py_ssize_t insize;

        if (c < 0x80) {
            REQUIRE_OUTBUF(1);
            **outbuf = (unsigned char)c;
            NEXT(1, 1);
            continue;
        }

        insize = 1;
        REQUIRE_OUTBUF(2);

        if (c < 0x10000) {
            if (TRYMAP_ENC(big5hkscs_bmp, code, c)) {
                if (code == MULTIC) {
                    Py_UCS4 c2;
                    if (inlen - *inpos >= 2)
                        c2 = INCHAR2;
                    else
                        c2 = 0;

                    if (inlen - *inpos >= 2 &&
                        ((c & 0xffdf) == 0x00ca) &&
                        ((c2 & 0xfff7) == 0x0304)) {
                        code = big5hkscs_pairenc_table[
                            ((c >> 4) |
                             (c2 >> 3)) & 3];
                        insize = 2;
                    }
                    else if (inlen - *inpos < 2 &&
                             !(flags & MBENC_FLUSH))
                        return MBERR_TOOFEW;
                    else {
                        if (c == 0xca)
                            code = 0x8866;
                        else /* c == 0xea */
                            code = 0x88a7;
                    }
                }
            }
            else if (TRYMAP_ENC(big5, code, c))
                ;
            else
                return 1;
        }
        else if (c < 0x20000)
            return insize;
        else if (c < 0x30000) {
            if (TRYMAP_ENC(big5hkscs_nonbmp, code, c & 0xffff))
                ;
            else
                return insize;
        }
        else
            return insize;

        OUTBYTE1(code >> 8);
        OUTBYTE2(code & 0xFF);
        NEXT(insize, 2);
    }

    return 0;
}

#define BH2S(c1, c2) (((c1) - 0x87) * (0xfe - 0x40 + 1) + ((c2) - 0x40))

DECODER(big5hkscs)
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

        if (0xc6 > c || c > 0xc8 || (c < 0xc7 && INBYTE2 < 0xa1)) {
            if (TRYMAP_DEC(big5, decoded, c, INBYTE2)) {
                OUTCHAR(decoded);
                NEXT_IN(2);
                continue;
            }
        }

        if (TRYMAP_DEC(big5hkscs, decoded, c, INBYTE2))
        {
            int s = BH2S(c, INBYTE2);
            const unsigned char *hintbase;

            assert(0x87 <= c && c <= 0xfe);
            assert(0x40 <= INBYTE2 && INBYTE2 <= 0xfe);

            if (BH2S(0x87, 0x40) <= s && s <= BH2S(0xa0, 0xfe)) {
                    hintbase = big5hkscs_phint_0;
                    s -= BH2S(0x87, 0x40);
            }
            else if (BH2S(0xc6,0xa1) <= s && s <= BH2S(0xc8,0xfe)){
                    hintbase = big5hkscs_phint_12130;
                    s -= BH2S(0xc6, 0xa1);
            }
            else if (BH2S(0xf9,0xd6) <= s && s <= BH2S(0xfe,0xfe)){
                    hintbase = big5hkscs_phint_21924;
                    s -= BH2S(0xf9, 0xd6);
            }
            else
                    return MBERR_INTERNAL;

            if (hintbase[s >> 3] & (1 << (s & 7))) {
                    OUTCHAR(decoded | 0x20000);
                    NEXT_IN(2);
            }
            else {
                    OUTCHAR(decoded);
                    NEXT_IN(2);
            }
            continue;
        }

        switch ((c << 8) | INBYTE2) {
        case 0x8862: OUTCHAR2(0x00ca, 0x0304); break;
        case 0x8864: OUTCHAR2(0x00ca, 0x030c); break;
        case 0x88a3: OUTCHAR2(0x00ea, 0x0304); break;
        case 0x88a5: OUTCHAR2(0x00ea, 0x030c); break;
        default: return 1;
        }

        NEXT_IN(2); /* all decoded code points are pairs, above. */
    }

    return 0;
}


BEGIN_MAPPINGS_LIST
  MAPPING_DECONLY(big5hkscs)
  MAPPING_ENCONLY(big5hkscs_bmp)
  MAPPING_ENCONLY(big5hkscs_nonbmp)
END_MAPPINGS_LIST

BEGIN_CODECS_LIST
  CODEC_STATELESS_WINIT(big5hkscs)
END_CODECS_LIST

I_AM_A_MODULE_FOR(hk)

#ifdef __aarch64__
_Section(".rodata.pytab.1 //")
#else
_Section(".rodata.pytab.1")
#endif
 const struct _inittab _PyImport_Inittab__codecs_hk = {
    "_codecs_hk",
    PyInit__codecs_hk,
};
