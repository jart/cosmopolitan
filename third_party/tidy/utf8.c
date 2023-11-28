/* utf8.c -- convert characters to/from UTF-8

  (c) 1998-2007 (W3C) MIT, ERCIM, Keio University
  See tidy.h for the copyright notice.

  Uses public interfaces to abstract input source and output
  sink, which may be user supplied or either FILE* or memory
  based Tidy implementations.  Encoding support is uniform
  regardless of I/O mechanism.

  Note, UTF-8 encoding, by itself, does not affect the actual
  "codepoints" of the underlying character encoding.  In the
  cases of ASCII, Latin1, Unicode (16-bit, BMP), these all
  refer to ISO-10646 "codepoints".  For anything else, they
  refer to some other "codepoint" set.

  Put another way, UTF-8 is a variable length method to
  represent any non-negative integer value.  The glyph
  that a integer value represents is unchanged and defined
  externally (e.g. by ISO-10646, Big5, Win1252, MacRoman,
  Latin2-9, and so on).

  Put still another way, UTF-8 is more of a _transfer_ encoding
  than a _character_ encoding, per se.
*/

#include "third_party/tidy/tidy.h"
#include "third_party/tidy/forward.h"
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "third_party/tidy/utf8.h"

/*
UTF-8 encoding/decoding functions
Return # of bytes in UTF-8 sequence; result < 0 if illegal sequence

Also see below for UTF-16 encoding/decoding functions

References :

1) UCS Transformation Format 8 (UTF-8):
ISO/IEC 10646-1:1996 Amendment 2 or ISO/IEC 10646-1:2000 Annex D
<http://anubis.dkuug.dk/JTC1/SC2/WG2/docs/n1335>
<http://www.cl.cam.ac.uk/~mgk25/ucs/ISO-10646-UTF-8.html>

Table 4 - Mapping from UCS-4 to UTF-8

2) Unicode standards:
<https://www.unicode.org/standard/standard.html>

3) Legal UTF-8 byte sequences:
<https://www.unicode.org/versions/corrigendum1.html>

Code point          1st byte    2nd byte    3rd byte    4th byte
----------          --------    --------    --------    --------
U+0000..U+007F      00..7F
U+0080..U+07FF      C2..DF      80..BF
U+0800..U+0FFF      E0          A0..BF      80..BF
U+1000..U+FFFF      E1..EF      80..BF      80..BF
U+10000..U+3FFFF    F0          90..BF      80..BF      80..BF
U+40000..U+FFFFF    F1..F3      80..BF      80..BF      80..BF
U+100000..U+10FFFF  F4          80..8F      80..BF      80..BF

The definition of UTF-8 in Annex D of ISO/IEC 10646-1:2000 also
allows for the use of five- and six-byte sequences to encode
characters that are outside the range of the Unicode character
set; those five- and six-byte sequences are illegal for the use
of UTF-8 as a transformation of Unicode characters. ISO/IEC 10646
does not allow mapping of unpaired surrogates, nor U+FFFE and U+FFFF
(but it does allow other noncharacters).

4) RFC 2279: UTF-8, a transformation format of ISO 10646:
<http://www.ietf.org/rfc/rfc2279.txt>

5) UTF-8 and Unicode FAQ:
<http://www.cl.cam.ac.uk/~mgk25/unicode.html>

6) Markus Kuhn's UTF-8 decoder stress test file:
<http://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-test.txt>

7) UTF-8 Demo:
<http://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-demo.txt>

8) UTF-8 Sampler:
<http://www.columbia.edu/kermit/utf8.html>

9) Transformation Format for 16 Planes of Group 00 (UTF-16):
ISO/IEC 10646-1:1996 Amendment 1 or ISO/IEC 10646-1:2000 Annex C
<http://anubis.dkuug.dk/JTC1/SC2/WG2/docs/n2005/n2005.pdf>
<http://www.cl.cam.ac.uk/~mgk25/ucs/ISO-10646-UTF-16.html>

10) RFC 2781: UTF-16, an encoding of ISO 10646:
<http://www.ietf.org/rfc/rfc2781.txt>

11) UTF-16 invalid surrogate pairs:
<https://www.unicode.org/faq/utf_bom.html#16>

UTF-16       UTF-8          UCS-4
D83F DFF*    F0 9F BF B*    0001FFF*
D87F DFF*    F0 AF BF B*    0002FFF*
D8BF DFF*    F0 BF BF B*    0003FFF*
D8FF DFF*    F1 8F BF B*    0004FFF*
D93F DFF*    F1 9F BF B*    0005FFF*
D97F DFF*    F1 AF BF B*    0006FFF*
                ...
DBBF DFF*    F3 BF BF B*    000FFFF*
DBFF DFF*    F4 8F BF B*    0010FFF*

* = E or F

1010  A
1011  B
1100  C
1101  D
1110  E
1111  F

*/

#define kNumUTF8Sequences        7
#define kMaxUTF8Bytes            4

#define kUTF8ByteSwapNotAChar    0xFFFE
#define kUTF8NotAChar            0xFFFF

#define kMaxUTF8FromUCS4         0x10FFFF

#define kUTF16SurrogatesBegin    0x10000
#define kMaxUTF16FromUCS4        0x10FFFF

/* UTF-16 surrogate pair areas */
#define kUTF16LowSurrogateBegin  0xD800
#define kUTF16LowSurrogateEnd    0xDBFF
#define kUTF16HighSurrogateBegin 0xDC00
#define kUTF16HighSurrogateEnd   0xDFFF


/* offsets into validUTF8 table below */
static const int offsetUTF8Sequences[kMaxUTF8Bytes + 1] =
{
    0, /* 1 byte */
    1, /* 2 bytes */
    2, /* 3 bytes */
    4, /* 4 bytes */
    kNumUTF8Sequences /* must be last */
};

static const struct validUTF8Sequence
{
     uint lowChar;
     uint highChar;
     int  numBytes;
     byte validBytes[8];
} validUTF8[kNumUTF8Sequences] =
{
/*   low       high   #bytes  byte 1      byte 2      byte 3      byte 4 */
    {0x0000,   0x007F,   1, {0x00, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x0080,   0x07FF,   2, {0xC2, 0xDF, 0x80, 0xBF, 0x00, 0x00, 0x00, 0x00}},
    {0x0800,   0x0FFF,   3, {0xE0, 0xE0, 0xA0, 0xBF, 0x80, 0xBF, 0x00, 0x00}},
    {0x1000,   0xFFFF,   3, {0xE1, 0xEF, 0x80, 0xBF, 0x80, 0xBF, 0x00, 0x00}},
    {0x10000,  0x3FFFF,  4, {0xF0, 0xF0, 0x90, 0xBF, 0x80, 0xBF, 0x80, 0xBF}},
    {0x40000,  0xFFFFF,  4, {0xF1, 0xF3, 0x80, 0xBF, 0x80, 0xBF, 0x80, 0xBF}},
    {0x100000, 0x10FFFF, 4, {0xF4, 0xF4, 0x80, 0x8F, 0x80, 0xBF, 0x80, 0xBF}}
};

int TY_(DecodeUTF8BytesToChar)( uint* c, uint firstByte, ctmbstr successorBytes,
                                TidyInputSource* inp, int* count )
{
    byte tempbuf[10];
    byte *buf = &tempbuf[0];
    uint ch = 0, n = 0;
    int i, bytes = 0;
    Bool hasError = no;

    if ( successorBytes )
        buf = (byte*) successorBytes;

    /* special check if we have been passed an EOF char */
    if ( firstByte == EndOfStream )
    {
        /* at present */
        *c = firstByte;
        *count = 1;
        return 0;
    }

    ch = firstByte; /* first byte is passed in separately */

    if (ch <= 0x7F) /* 0XXX XXXX one byte */
    {
        n = ch;
        bytes = 1;
    }
    else if ((ch & 0xE0) == 0xC0)  /* 110X XXXX  two bytes */
    {
        n = ch & 31;
        bytes = 2;
    }
    else if ((ch & 0xF0) == 0xE0)  /* 1110 XXXX  three bytes */
    {
        n = ch & 15;
        bytes = 3;
    }
    else if ((ch & 0xF8) == 0xF0)  /* 1111 0XXX  four bytes */
    {
        n = ch & 7;
        bytes = 4;
    }
    else if ((ch & 0xFC) == 0xF8)  /* 1111 10XX  five bytes */
    {
        n = ch & 3;
        bytes = 5;
        hasError = yes;
    }
    else if ((ch & 0xFE) == 0xFC)  /* 1111 110X  six bytes */
    {
        n = ch & 1;
        bytes = 6;
        hasError = yes;
    }
    else
    {
        /* not a valid first byte of a UTF-8 sequence */
        n = ch;
        bytes = 1;
        hasError = yes;
    }

    /* successor bytes should have the form 10XX XXXX */

    /* If caller supplied buffer, use it.  Else see if caller
    ** supplied an input source, use that.
    */
    if ( successorBytes )
    {
        for ( i=0; i < bytes-1; ++i )
        {
            if ( !buf[i] || (buf[i] & 0xC0) != 0x80 )
            {
                hasError = yes;
                bytes = i+1;
                break;
            }
            n = (n << 6) | (buf[i] & 0x3F);
        }
    }
    else if ( inp )
    {
        for ( i=0; i < bytes-1 && !inp->eof(inp->sourceData); ++i )
        {
            int b = inp->getByte( inp->sourceData );
            buf[i] = (tmbchar) b;

            /* End of data or illegal successor byte value */
            if ( b == EOF || (buf[i] & 0xC0) != 0x80 )
            {
                hasError = yes;
                bytes = i+1;
                if ( b != EOF )
                    inp->ungetByte( inp->sourceData, buf[i] );
                break;
            }
            n = (n << 6) | (buf[i] & 0x3F);
        }
    }
    else if ( bytes > 1 )
    {
        hasError = yes;
        bytes = 1;
    }

    if (!hasError && ((n == kUTF8ByteSwapNotAChar) || (n == kUTF8NotAChar)))
        hasError = yes;

    if (!hasError && (n > kMaxUTF8FromUCS4))
        hasError = yes;

    if (!hasError)
    {
        int lo, hi;

        lo = offsetUTF8Sequences[bytes - 1];
        hi = offsetUTF8Sequences[bytes] - 1;

        /* check for overlong sequences */
        if ((n < validUTF8[lo].lowChar) || (n > validUTF8[hi].highChar))
            hasError = yes;
        else
        {
            hasError = yes; /* assume error until proven otherwise */

            for (i = lo; i <= hi; i++)
            {
                int tempCount;
                byte theByte;

                for (tempCount = 0; tempCount < bytes; tempCount++)
                {
                    if (!tempCount)
                        theByte = (tmbchar) firstByte;
                    else
                        theByte = buf[tempCount - 1];

                    if ( theByte >= validUTF8[i].validBytes[(tempCount * 2)] &&
                         theByte <= validUTF8[i].validBytes[(tempCount * 2) + 1] )
                        hasError = no;
                    if (hasError)
                        break;
                }
            }
        }
    }

#if 1 && defined(_DEBUG)
    if ( hasError )
    {
       /* debug */
       fprintf( stderr, "UTF-8 decoding error of %d bytes : ", bytes );
       fprintf( stderr, "0x%02x ", firstByte );
       for (i = 1; i < bytes; i++)
           fprintf( stderr, "0x%02x ", buf[i - 1] );
       fprintf( stderr, " = U+%04X\n", n );
    }
#endif

    *count = bytes;
    *c = n;
    if ( hasError )
        return -1;
    return 0;
}

int TY_(EncodeCharToUTF8Bytes)( uint c, tmbstr encodebuf,
                                TidyOutputSink* outp, int* count )
{
    byte tempbuf[10] = {0};
    byte* buf = &tempbuf[0];
    int bytes = 0;
    Bool hasError = no;

    if ( encodebuf )
        buf = (byte*) encodebuf;

    if (c <= 0x7F)  /* 0XXX XXXX one byte */
    {
        buf[0] = (tmbchar) c;
        bytes = 1;
    }
    else if (c <= 0x7FF)  /* 110X XXXX  two bytes */
    {
        buf[0] = (tmbchar) ( 0xC0 | (c >> 6) );
        buf[1] = (tmbchar) ( 0x80 | (c & 0x3F) );
        bytes = 2;
    }
    else if (c <= 0xFFFF)  /* 1110 XXXX  three bytes */
    {
        buf[0] = (tmbchar) (0xE0 | (c >> 12));
        buf[1] = (tmbchar) (0x80 | ((c >> 6) & 0x3F));
        buf[2] = (tmbchar) (0x80 | (c & 0x3F));
        bytes = 3;
        if ( c == kUTF8ByteSwapNotAChar || c == kUTF8NotAChar )
            hasError = yes;
    }
    else if (c <= 0x1FFFFF)  /* 1111 0XXX  four bytes */
    {
        buf[0] = (tmbchar) (0xF0 | (c >> 18));
        buf[1] = (tmbchar) (0x80 | ((c >> 12) & 0x3F));
        buf[2] = (tmbchar) (0x80 | ((c >> 6) & 0x3F));
        buf[3] = (tmbchar) (0x80 | (c & 0x3F));
        bytes = 4;
        if (c > kMaxUTF8FromUCS4)
            hasError = yes;
    }
    else if (c <= 0x3FFFFFF)  /* 1111 10XX  five bytes */
    {
        buf[0] = (tmbchar) (0xF8 | (c >> 24));
        buf[1] = (tmbchar) (0x80 | (c >> 18));
        buf[2] = (tmbchar) (0x80 | ((c >> 12) & 0x3F));
        buf[3] = (tmbchar) (0x80 | ((c >> 6) & 0x3F));
        buf[4] = (tmbchar) (0x80 | (c & 0x3F));
        bytes = 5;
        hasError = yes;
    }
    else if (c <= 0x7FFFFFFF)  /* 1111 110X  six bytes */
    {
        buf[0] = (tmbchar) (0xFC | (c >> 30));
        buf[1] = (tmbchar) (0x80 | ((c >> 24) & 0x3F));
        buf[2] = (tmbchar) (0x80 | ((c >> 18) & 0x3F));
        buf[3] = (tmbchar) (0x80 | ((c >> 12) & 0x3F));
        buf[4] = (tmbchar) (0x80 | ((c >> 6) & 0x3F));
        buf[5] = (tmbchar) (0x80 | (c & 0x3F));
        bytes = 6;
        hasError = yes;
    }
    else
        hasError = yes;

    /* don't output invalid UTF-8 byte sequence to a stream */
    if ( !hasError && outp != NULL )
    {
        int ix;
        for ( ix=0; ix < bytes; ++ix )
          outp->putByte( outp->sinkData, buf[ix] );
    }

#if 1 && defined(_DEBUG)
    if ( hasError )
    {
        int i;
        fprintf( stderr, "UTF-8 encoding error for U+%x : ", c );
        for (i = 0; i < bytes; i++)
            fprintf( stderr, "0x%02x ", buf[i] );
        fprintf( stderr, "\n" );
    }
#endif

    *count = bytes;
    if (hasError)
        return -1;
    return 0;
}


/* return one less than the number of bytes used by the UTF-8 byte sequence */
/* str points to the UTF-8 byte sequence */
/* the Unicode char is returned in *ch */
uint TY_(GetUTF8)( ctmbstr str, uint *ch )
{
    uint n;
    int bytes;

    int err;

    bytes = 0;

    /* first byte "str[0]" is passed in separately from the */
    /* rest of the UTF-8 byte sequence starting at "str[1]" */
    err = TY_(DecodeUTF8BytesToChar)( &n, str[0], str+1, NULL, &bytes );
    if (err)
    {
#if 1 && defined(_DEBUG)
        fprintf(stderr, "pprint UTF-8 decoding error for U+%x : ", n);
#endif
        n = 0xFFFD; /* replacement char */
    }

    *ch = n;
    return bytes - 1;
}

/* store char c as UTF-8 encoded byte stream */
tmbstr TY_(PutUTF8)( tmbstr buf, uint c )
{
    int err, count = 0;

    err = TY_(EncodeCharToUTF8Bytes)( c, buf, NULL, &count );
    if (err)
    {
#if 1 && defined(_DEBUG)
        fprintf(stderr, "pprint UTF-8 encoding error for U+%x : ", c);
#endif
        /* replacement char 0xFFFD encoded as UTF-8 */
        buf[0] = (byte) 0xEF;
        buf[1] = (byte) 0xBF;
        buf[2] = (byte) 0xBD;
        count = 3;
    }

    buf += count;
    return buf;
}

Bool    TY_(IsValidUTF16FromUCS4)( tchar ucs4 )
{
  return ( ucs4 <= kMaxUTF16FromUCS4 );
}

Bool    TY_(IsHighSurrogate)( tchar ch )
{
    return ( ch >= kUTF16HighSurrogateBegin && ch <= kUTF16HighSurrogateEnd );
}
Bool    TY_(IsLowSurrogate)( tchar ch )
{
    return ( ch >= kUTF16LowSurrogateBegin && ch <= kUTF16LowSurrogateEnd );
}

tchar   TY_(CombineSurrogatePair)( tchar high, tchar low )
{
    assert( TY_(IsHighSurrogate)(high) && TY_(IsLowSurrogate)(low) );
    return ( ((low - kUTF16LowSurrogateBegin) * 0x400) +
             high - kUTF16HighSurrogateBegin + 0x10000 );
}

Bool   TY_(SplitSurrogatePair)( tchar utf16, tchar* low, tchar* high )
{
    Bool status = ( TY_(IsValidCombinedChar)( utf16 ) && high && low );
    if ( status )
    {
        *low  = (utf16 - kUTF16SurrogatesBegin) / 0x400 + kUTF16LowSurrogateBegin;
        *high = (utf16 - kUTF16SurrogatesBegin) % 0x400 + kUTF16HighSurrogateBegin;
    }
    return status;
}

Bool    TY_(IsValidCombinedChar)( tchar ch )
{
    return ( ch >= kUTF16SurrogatesBegin &&
             (ch & 0x0000FFFE) != 0x0000FFFE &&
             (ch & 0x0000FFFF) != 0x0000FFFF );
}

Bool    TY_(IsCombinedChar)( tchar ch )
{
    return ( ch >= kUTF16SurrogatesBegin );
}

/*
 * local variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 4
 * end:
 */
