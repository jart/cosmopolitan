/*
 * _codecs_tw.c: Codecs collection for Taiwan's encodings
 *
 * Written by Hye-Shik "Bourne to Macro" Chang <perky@FreeBSD.org>
 */

#include "third_party/python/Modules/cjkcodecs/cjkcodecs.h"
#include "third_party/python/Include/yoink.h"
#include "third_party/python/Include/import.h"

PYTHON_PROVIDE("_codecs_tw");
PYTHON_PROVIDE("_codecs_tw.__map_big5");
PYTHON_PROVIDE("_codecs_tw.__map_cp950ext");
PYTHON_PROVIDE("_codecs_tw.getcodec");

/*
 * BIG5 codec
 */

ENCODER(big5)
{
    while (*inpos < inlen) {
        Py_UCS4 c = INCHAR1;
        DBCHAR code;

        if (c < 0x80) {
            REQUIRE_OUTBUF(1);
            **outbuf = (unsigned char)c;
            NEXT(1, 1);
            continue;
        }

        if (c > 0xFFFF)
            return 1;

        REQUIRE_OUTBUF(2);

        if (TRYMAP_ENC(big5, code, c))
            ;
        else
            return 1;

        OUTBYTE1(code >> 8);
        OUTBYTE2(code & 0xFF);
        NEXT(1, 2);
    }

    return 0;
}

DECODER(big5)
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
        if (TRYMAP_DEC(big5, decoded, c, INBYTE2)) {
            OUTCHAR(decoded);
            NEXT_IN(2);
        }
        else return 1;
    }

    return 0;
}


/*
 * CP950 codec
 */

ENCODER(cp950)
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
        if (TRYMAP_ENC(cp950ext, code, c))
            ;
        else if (TRYMAP_ENC(big5, code, c))
            ;
        else
            return 1;

        OUTBYTE1(code >> 8);
        OUTBYTE2(code & 0xFF);
        NEXT(1, 2);
    }

    return 0;
}

DECODER(cp950)
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

        if (TRYMAP_DEC(cp950ext, decoded, c, INBYTE2))
            OUTCHAR(decoded);
        else if (TRYMAP_DEC(big5, decoded, c, INBYTE2))
            OUTCHAR(decoded);
        else
            return 1;

        NEXT_IN(2);
    }

    return 0;
}



BEGIN_MAPPINGS_LIST
  MAPPING_ENCDEC(big5)
  MAPPING_ENCDEC(cp950ext)
END_MAPPINGS_LIST

BEGIN_CODECS_LIST
  CODEC_STATELESS(big5)
  CODEC_STATELESS(cp950)
END_CODECS_LIST

I_AM_A_MODULE_FOR(tw)

#ifdef __aarch64__
_Section(".rodata.pytab.1 //")
#else
_Section(".rodata.pytab.1")
#endif
 const struct _inittab _PyImport_Inittab__codecs_tw = {
    "_codecs_tw",
    PyInit__codecs_tw,
};
