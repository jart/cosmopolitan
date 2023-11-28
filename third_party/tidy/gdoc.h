#ifndef __GDOC_H__
#define __GDOC_H__
#include "third_party/tidy/forward.h"

/* gdoc.h -- clean up html exported by Google Docs

  (c) 2012 (W3C) MIT, ERCIM, Keio University
  See tidy.h for the copyright notice.

  - strip the script element, as the style sheet is a mess
  - strip class attributes
  - strip span elements, leaving their content in place
  - replace <a name=...></a> by id on parent element
  - strip empty <p> elements

*/

void TY_(CleanGoogleDocument)( TidyDocImpl* doc );

#endif /* __GDOC_H__ */
