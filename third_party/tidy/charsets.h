#ifndef __CHARSETS_H__
#define __CHARSETS_H__
#include "third_party/tidy/access.h"
#include "third_party/tidy/tidyplatform.h"

/* charsets.h -- character set information and mappings

  (c) 1998-2021 (W3C) MIT, ERCIM, Keio University
  See tidy.h for the copyright notice.

*/

uint TY_(GetEncodingIdFromName)(ctmbstr name);
uint TY_(GetEncodingIdFromCodePage)(uint cp);
uint TY_(GetEncodingCodePageFromName)(ctmbstr name);
uint TY_(GetEncodingCodePageFromId)(uint id);
ctmbstr TY_(GetEncodingNameFromId)(uint id);
ctmbstr TY_(GetEncodingNameFromCodePage)(uint cp);

#endif /* __CHARSETS_H__ */

