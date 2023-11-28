#ifndef __UTF8_H__
#define __UTF8_H__

/* utf8.h -- convert characters to/from UTF-8

  (c) 1998-2006 (W3C) MIT, ERCIM, Keio University
  See tidy.h for the copyright notice.

*/

#include "third_party/tidy/tidyplatform.h"
#include "third_party/tidy/access.h"
#include "third_party/tidy/tidybuffio.h"

/* UTF-8 encoding/decoding support
** Does not convert character "codepoints", i.e. to/from 10646.
*/

int TY_(DecodeUTF8BytesToChar)( uint* c, uint firstByte, ctmbstr successorBytes,
                                TidyInputSource* inp, int* count );

int TY_(EncodeCharToUTF8Bytes)( uint c, tmbstr encodebuf,
                                TidyOutputSink* outp, int* count );


uint  TY_(GetUTF8)( ctmbstr str, uint *ch );
tmbstr TY_(PutUTF8)( tmbstr buf, uint c );

#define UNICODE_BOM_BE   0xFEFF   /* big-endian (default) UNICODE BOM */
#define UNICODE_BOM      UNICODE_BOM_BE
#define UNICODE_BOM_LE   0xFFFE   /* little-endian UNICODE BOM */
#define UNICODE_BOM_UTF8 0xEFBBBF /* UTF-8 UNICODE BOM */


Bool    TY_(IsValidUTF16FromUCS4)( tchar ucs4 );
Bool    TY_(IsHighSurrogate)( tchar ch );
Bool    TY_(IsLowSurrogate)( tchar ch );

Bool    TY_(IsCombinedChar)( tchar ch );
Bool    TY_(IsValidCombinedChar)( tchar ch );

tchar   TY_(CombineSurrogatePair)( tchar high, tchar low );
Bool    TY_(SplitSurrogatePair)( tchar utf16, tchar* high, tchar* low );



#endif /* __UTF8_H__ */
