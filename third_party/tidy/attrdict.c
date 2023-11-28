/* attrdict.c -- extended attribute information

   (c) 1998-2006 (W3C) MIT, ERCIM, Keio University
   See tidy.h for the copyright notice.

*/

#include "third_party/tidy/forward.h"
#include "third_party/tidy/attrdict.h"
#include "third_party/tidy/lexer.h"

/* These items are added to elements that support them. */
#define INCLUDE_ARIA \
  { TidyAttr_ARIA_ACTIVEDESCENDANT, xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_ATOMIC,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_AUTOCOMPLETE,     xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_BUSY,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_CHECKED,          xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_CONTROLS,         xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_DESCRIBEDBY,      xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_DISABLED,         xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_DROPEFFECT,       xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_EXPANDED,         xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_FLOWTO,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_GRABBED,          xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_HASPOPUP,         xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_HIDDEN,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_INVALID,          xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_LABEL,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_LABELLEDBY,       xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_LEVEL,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_LIVE,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_MULTILINE,        xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_MULTISELECTABLE,  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_ORIENTATION,      xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_OWNS,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_POSINSET,         xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_PRESSED,          xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_READONLY,         xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_RELEVANT,         xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_REQUIRED,         xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_SELECTED,         xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_SETSIZE,          xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_SORT,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_VALUEMAX,         xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_VALUEMIN,         xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_VALUENOW,         xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ARIA_VALUETEXT,        xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \


/* These items are added to all elements below. */
#define INCLUDE_MICRODATA \
  { TidyAttr_ITEMID,                xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ITEMPROP,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ITEMREF,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ITEMSCOPE,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ITEMTYPE,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \


/* RDFa attributes. These were moved here from the hard code in attrs.c as-is. Note that RDFa allows the
   last three items in all versions of HTML. */
#define INCLUDE_RDFA \
  { TidyAttr_ABOUT,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_DATATYPE,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_INLIST,                xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_PREFIX,                xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_PROPERTY,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_RESOURCE,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_TYPEOF,                xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_VOCAB,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_CONTENT,               HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, \
  { TidyAttr_REL,                   HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, \
  { TidyAttr_REV,                   HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, \


/* These are the core attributes that can apply to _any_ element starting with
   HTML5. In general they will be marked as not supported for previous versions
   of HTML, in which case please override the attribute in the specific item
   below. **This macro must be expanded at the END of each element. **
   Most of these attributes are defined as universal for HTML here:
     https://www.w3.org/TR/html-markup/global-attributes.html
   Attribute `role` is defined as universal for HTML5 here:
     https://www.w3.org/TR/html5/dom.html#wai-aria
 */
#define INCLUDE_CORE_ATTRIBS \
  { TidyAttr_ACCESSKEY,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_CLASS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_CONTENTEDITABLE,       xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_CONTEXTMENU,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_DIR,                   xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_DRAGGABLE,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_DROPZONE,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_HIDDEN,                xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ID,                    xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_IS,                    xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_LANG,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_ROLE,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_SLOT,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_SPELLCHECK,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_STYLE,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_TABINDEX,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_TITLE,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_TRANSLATE,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH50 }, \
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \


/* These are the core event-handler attributes that can apply to _any_ element
   starting with HTML5. In general they will be marked as not supported for
   previous versions of HTML, in which case please override the attribute in the
   specific item below. **This macro must be expanded at the END of each
   element. **
   Most of these attributes are defined as universal for HTML5 here:
     https://www.w3.org/TR/html-markup/global-attributes.html
   Attribute `oncuechange` is defined as universal for HTML5 here:
     https://www.w3.org/TR/html5/webappapis.html
 */
#define INCLUDE_CORE_EVENTS \
  { TidyAttr_OnABORT,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnBLUR,                xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnCANPLAY,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnCANPLAYTHROUGH,      xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnCHANGE,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnCLICK,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnCONTEXTMENU,         xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnCUECHANGE,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnDRAG,                xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnDRAGEND,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnDRAGENTER,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnDRAGLEAVE,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnDRAGOVER,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnDRAGSTART,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnDROP,                xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnDURATIONCHANGE,      xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnEMPTIED,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnENDED,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnERROR,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnFOCUS,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnINPUT,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnINVALID,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnKEYUP,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnLOAD,                xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnLOADEDDATA,          xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnLOADEDMETADATA,      xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnLOADSTART,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnMOUSEWHEEL,          xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnPAUSE,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnPLAY,                xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnPLAYING,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnPROGRESS,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnRATECHANGE,          xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnREADYSTATECHANGE,    xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnRESET,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnSCROLL,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnSEEKED,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnSEEKING,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnSELECT,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnSHOW,                xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnSTALLED,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnSUBMIT,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnSUSPEND,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnTIMEUPDATE,          xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnVOLUMECHANGE,        xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \
  { TidyAttr_OnWAITING,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, \


const AttrVersion TY_(W3CAttrsFor_A)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ACCESSKEY,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_CHARSET,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_COORDS,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DOWNLOAD,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_HREF,                  HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_HREFLANG,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_MEDIA,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_METHODS,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_NAME,                  HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_OnBLUR,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnFOCUS,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_REL,                   HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_REV,                   HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx },
  { TidyAttr_SDAPREF,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_SHAPE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TABINDEX,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_TARGET,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_TITLE,                 HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_TYPE,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_URN,                   HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_ABBR)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_ACRONYM)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_ADDRESS)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_SDAPREF,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_APPLET)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ALIGN,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_ALT,                   xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_ARCHIVE,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_CODE,                  xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_CODEBASE,              xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_HEIGHT,                xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_HSPACE,                xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_NAME,                  xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_OBJECT,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_VSPACE,                xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_WIDTH,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_AREA)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ACCESSKEY,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ALT,                   xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_COORDS,                xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_HREF,                  xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_HREFLANG,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_MEDIA,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_NOHREF,                xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_OnBLUR,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnFOCUS,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_REL,                   xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_SHAPE,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TABINDEX,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TARGET,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TYPE,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_ARTICLE)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_ASIDE)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_AUDIO)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_AUTOPLAY,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_CONTROLS,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_CROSSORIGIN,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_LOOP,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_MEDIAGROUP,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_PRELOAD,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_SRC,                   xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_B)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_BASE)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_HREF,                  HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_ID,                    xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TARGET,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_BASEFONT)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_COLOR,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_FACE,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_SIZE,                  xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                          },
};

const AttrVersion TY_(W3CAttrsFor_BDI)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_BDO)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_BIG)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_BLOCKQUOTE)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CITE,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_BODY)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ALINK,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_BACKGROUND,            xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_BGCOLOR,               xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LINK,                  xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_OnAFTERPRINT,          xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_OnBEFOREPRINT,         xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_OnBEFOREUNLOAD,        xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnHASHCHANGE,          xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnLOAD,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMESSAGE,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnOFFLINE,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_OnONLINE,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_OnPAGEHIDE,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_OnPAGESHOW,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_OnPOPSTATE,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_OnREDO,                xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_OnRESIZE,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_OnSTORAGE,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_OnUNDO,                xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_OnUNLOAD,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TEXT,                  xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_VLINK,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_BR)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_CLEAR,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAPREF,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_BUTTON)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ACCESSKEY,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_AUTOFOCUS,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DISABLED,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_FORM,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_FORMACTION,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_FORMENCTYPE,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_FORMMETHOD,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_FORMNOVALIDATE,        xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_FORMTARGET,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_NAME,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_OnBLUR,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnFOCUS,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TABINDEX,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TYPE,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_VALUE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_CANVAS)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_HEIGHT,                xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_WIDTH,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_CAPTION)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ALIGN,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_CENTER)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_CITE)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_CODE)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_COL)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ALIGN,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_CHAR,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_CHAROFF,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SPAN,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_VALIGN,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_WIDTH,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_COLGROUP)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ALIGN,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_CHAR,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_CHAROFF,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SPAN,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_VALIGN,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_WIDTH,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_COMMAND)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CHECKED,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_DISABLED,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_ICON,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_LABEL,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_RADIOGROUP,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_TYPE,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_DATALIST)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_DD)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_DEL)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CITE,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DATETIME,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_DETAILS)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_OPEN,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_DFN)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_DIALOG)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_OPEN,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_DIR)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_COMPACT,               HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_SDAPREF,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_DIV)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ALIGN,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_DL)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_COMPACT,               HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_SDAPREF,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_DT)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_EM)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_EMBED)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_HEIGHT,                xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_SRC,                   xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_TYPE,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_WIDTH,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_FIELDSET)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DISABLED,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_FORM,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_NAME,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_FIGCAPTION)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_FIGURE)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_FONT)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_COLOR,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_FACE,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_SIZE,                  xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_FOOTER)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_FORM)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ACCEPT,                xxxx|xxxx|xxxx|H41T|X10T|xxxx|H41F|X10F|xxxx|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_ACCEPT_CHARSET,        xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_ACTION,                HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_AUTOCOMPLETE,          xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ENCTYPE,               HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_METHOD,                HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_NAME,                  xxxx|xxxx|xxxx|H41T|X10T|xxxx|H41F|X10F|xxxx|H41S|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_NOVALIDATE,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnRESET,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnSUBMIT,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAPREF,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_SDASUFF,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TARGET,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_FRAME)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|xxxx|xxxx|xxxx|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_FRAMEBORDER,           xxxx|xxxx|xxxx|xxxx|xxxx|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_ID,                    xxxx|xxxx|xxxx|xxxx|xxxx|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_LONGDESC,              xxxx|xxxx|xxxx|xxxx|xxxx|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_MARGINHEIGHT,          xxxx|xxxx|xxxx|xxxx|xxxx|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_MARGINWIDTH,           xxxx|xxxx|xxxx|xxxx|xxxx|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_NAME,                  xxxx|xxxx|xxxx|xxxx|xxxx|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_NORESIZE,              xxxx|xxxx|xxxx|xxxx|xxxx|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_SCROLLING,             xxxx|xxxx|xxxx|xxxx|xxxx|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_SRC,                   xxxx|xxxx|xxxx|xxxx|xxxx|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|xxxx|xxxx|xxxx|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|xxxx|xxxx|xxxx|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_FRAMESET)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|xxxx|xxxx|xxxx|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_COLS,                  xxxx|xxxx|xxxx|xxxx|xxxx|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_ID,                    xxxx|xxxx|xxxx|xxxx|xxxx|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnLOAD,                xxxx|xxxx|xxxx|xxxx|xxxx|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnUNLOAD,              xxxx|xxxx|xxxx|xxxx|xxxx|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_ROWS,                  xxxx|xxxx|xxxx|xxxx|xxxx|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|xxxx|xxxx|xxxx|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|xxxx|xxxx|xxxx|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_H1)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ALIGN,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_H2)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ALIGN,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_H3)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ALIGN,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_H4)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ALIGN,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_H5)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ALIGN,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_H6)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ALIGN,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_HEAD)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_PROFILE,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_HEADER)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_HGROUP)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_HR)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ALIGN,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|xxxx|H41T|X10T|xxxx|H41F|X10F|xxxx|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|xxxx|H41T|X10T|xxxx|H41F|X10F|xxxx|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_NOSHADE,               xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAPREF,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_SIZE,                  xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_WIDTH,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_HTML)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_MANIFEST,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_VERSION,               HT20|HT32|H40T|H41T|xxxx|H40F|H41F|xxxx|xxxx|xxxx|xxxx|XH11|XB10|xxxx|xxxx },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_I)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_IFRAME)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ALIGN,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_ALLOWFULLSCREEN,       xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_FRAMEBORDER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_HEIGHT,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LOADING,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_LONGDESC,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_MARGINHEIGHT,          xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_MARGINWIDTH,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_NAME,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_SANDBOX,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_SCROLLING,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_SEAMLESS,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_SRC,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_SRCDOC,                xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_WIDTH,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_IMG)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ALIGN,                 HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_ALT,                   HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_BORDER,                xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_CROSSORIGIN,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_HEIGHT,                xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_HSPACE,                xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_ISMAP,                 HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LOADING,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_LONGDESC,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx },
  { TidyAttr_NAME,                  xxxx|xxxx|xxxx|H41T|X10T|xxxx|H41F|X10F|xxxx|H41S|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAPREF,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_SIZES,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_SRC,                   HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_SRCSET,                xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_USEMAP,                xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_VSPACE,                xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_WIDTH,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_INPUT)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ACCEPT,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_ACCESSKEY,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_ALIGN,                 HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_ALT,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_AUTOCOMPLETE,          xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_AUTOFOCUS,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_CHECKED,               HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIRNAME,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_DISABLED,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_FORM,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_FORMACTION,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_FORMENCTYPE,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_FORMMETHOD,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_FORMNOVALIDATE,        xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_FORMTARGET,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_HEIGHT,                xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_ISMAP,                 xxxx|xxxx|xxxx|H41T|xxxx|xxxx|H41F|xxxx|xxxx|H41S|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LIST,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_MAX,                   xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_MAXLENGTH,             HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_MIN,                   xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_MULTIPLE,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_NAME,                  HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_OnBLUR,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCHANGE,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnFOCUS,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnSELECT,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_PATTERN,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_PLACEHOLDER,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_READONLY,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_REQUIRED,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_SDAPREF,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_SIZE,                  HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_SRC,                   HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_STEP,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TABINDEX,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_TYPE,                  HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_USEMAP,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_VALUE,                 HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_WIDTH,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_INS)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CITE,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DATETIME,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_ISINDEX)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_PROMPT,                xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_SDAPREF,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_KBD)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_KEYGEN)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_AUTOFOCUS,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_CHALLENGE,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_DISABLED,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_FORM,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_KEYTYPE,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_NAME,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_LABEL)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ACCESSKEY,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_FOR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_FORM,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnBLUR,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnFOCUS,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_LEGEND)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ACCESSKEY,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ALIGN,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_LI)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_TYPE,                  xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_VALUE,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_LINK)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_AS,                    xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_CHARSET,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_COLOR,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* Is #686 */
  { TidyAttr_CROSSORIGIN,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_HREF,                  HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_HREFLANG,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_INTEGRITY,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_MEDIA,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_METHODS,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_REL,                   HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_REV,                   HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx },
  { TidyAttr_SDAPREF,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_SIZES,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TARGET,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_TITLE,                 HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_TYPE,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_URN,                   HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_LISTING)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_SDAPREF,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_MAIN)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_MAP)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_NAME,                  xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_MATHML)[] =  /* [i_a]2 */
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ALIGN,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DISPLAY,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_HEIGHT,                xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_WIDTH,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_MARK)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_MENU)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_COMPACT,               HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LABEL,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_SDAPREF,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TYPE,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_MENUITEM)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_AUTOFOCUS,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_CHALLENGE,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_DISABLED,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_FORM,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_ICON,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_KEYTYPE,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_LABEL,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_NAME,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_META)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CHARSET,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_CONTENT,               HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_HTTP_EQUIV,            HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_ID,                    xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_NAME,                  HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_SCHEME,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_METER)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_FORM,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_HIGH,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_LOW,                   xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_MAX,                   xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_MIN,                   xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_OPTIMUM,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_VALUE,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_NAV)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_NEXTID)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_N,                     HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_NOFRAMES)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_NOSCRIPT)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_OBJECT)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ALIGN,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_ARCHIVE,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx },
  { TidyAttr_BORDER,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_CLASSID,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx },
  { TidyAttr_CODEBASE,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx },
  { TidyAttr_CODETYPE,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx },
  { TidyAttr_DATA,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_DECLARE,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx },
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_FORM,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_HEIGHT,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_HSPACE,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_NAME,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STANDBY,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TABINDEX,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_TYPE,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_USEMAP,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_VSPACE,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_WIDTH,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_OL)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_COMPACT,               HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_REVERSED,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_START,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_TYPE,                  xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_OPTGROUP)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DISABLED,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LABEL,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_OPTION)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DISABLED,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LABEL,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_SDAPREF,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_SELECTED,              HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_VALUE,                 HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_OUTPUT)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_FOR,                   xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_FORM,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_NAME,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_P)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ALIGN,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_PARAM)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_NAME,                  xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_TYPE,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx },
  { TidyAttr_VALUE,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_VALUETYPE,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};


const AttrVersion TY_(W3CAttrsFor_PICTURE)[] = /* Issue #151 - support for 'picture' tag - TODO: check list of attributes */
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_MEDIA,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SRCSET,                xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_TYPE,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_PLAINTEXT)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_SDAFORM,            HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,            0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_PRE)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_WIDTH,                 HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  { TidyAttr_XML_SPACE,             xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|xxxx },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_PROGRESS)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_FORM,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_MAX,                   xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_VALUE,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_Q)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CITE,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_RB)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_RBC)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_RP)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_RT)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_RBSPAN,                xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_RTC)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_RUBY)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_S)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_SAMP)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_SCRIPT)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ASYNC,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_CHARSET,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_CROSSORIGIN,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_DEFER,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_EVENT,                 xxxx|xxxx|H40T|H41T|xxxx|H40F|H41F|xxxx|H40S|H41S|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_FOR,                   xxxx|xxxx|H40T|H41T|xxxx|H40F|H41F|xxxx|H40S|H41S|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_ID,                    xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_INTEGRITY,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_LANGUAGE,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_SRC,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_TYPE,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_SPACE,             xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|xxxx|xxxx },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_SECTION)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_SELECT)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_AUTOFOCUS,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DISABLED,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_FORM,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_MULTIPLE,              HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_NAME,                  HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_OnBLUR,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCHANGE,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnFOCUS,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_REQUIRED,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_SDAPREF,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_SIZE,                  HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TABINDEX,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_SMALL)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_SLOT)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_NAME,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_SOURCE)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_MEDIA,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_SRC,                   xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_SRCSET,                xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_TYPE,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_SPAN)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_STRIKE)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_STRONG)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_STYLE)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_MEDIA,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_SCOPED,                xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TYPE,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  { TidyAttr_XML_SPACE,             xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|xxxx|xxxx },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_SUB)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_SUMMARY)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_SUP)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_SVG)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ALIGN,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_HEIGHT,                xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_WIDTH,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */

  /* svg xmln:xlink=uri */
  { TidyAttr_XMLNSXLINK,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 },

  { TidyAttr_X,                     xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_Y,                     xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_VIEWBOX,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_PRESERVEASPECTRATIO,   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_ZOOMANDPAN,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_VERSION,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_BASEPROFILE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_CONTENTSCRIPTTYPE,     xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_CONTENTSTYLETYPE,      xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_COLOR,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 },
  { TidyAttr_FILL,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 },
  { TidyAttr_FILLRULE,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 },
  { TidyAttr_STROKE,                xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 },
  { TidyAttr_STROKEDASHARRAY,       xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 },
  { TidyAttr_STROKEDASHOFFSET,      xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 },
  { TidyAttr_STROKELINECAP,         xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 },
  { TidyAttr_STROKELINEJOIN,        xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 },
  { TidyAttr_STROKEMITERLIMIT,      xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 },
  { TidyAttr_STROKEWIDTH,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 },
  { TidyAttr_COLORINTERPOLATION,    xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 },
  { TidyAttr_COLORRENDERING,        xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 },
  { TidyAttr_OPACITY,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 },
  { TidyAttr_STROKEOPACITY,         xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 },
  { TidyAttr_FILLOPACITY,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_TABLE)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ALIGN,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_BGCOLOR,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_BORDER,                xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_CELLPADDING,           xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_CELLSPACING,           xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DATAPAGESIZE,          xxxx|xxxx|H40T|H41T|xxxx|H40F|H41F|xxxx|H40S|H41S|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_FRAME,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_RULES,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SUMMARY,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx },
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_WIDTH,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_TBODY)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ALIGN,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_CHAR,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_CHAROFF,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_VALIGN,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_TD)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ABBR,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx },
  { TidyAttr_ALIGN,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx },
  { TidyAttr_AXIS,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx },
  { TidyAttr_BGCOLOR,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_CHAR,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_CHAROFF,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_COLSPAN,               xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_HEADERS,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_HEIGHT,                xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_NOWRAP,                xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ROWSPAN,               xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_SCOPE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_VALIGN,                xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx },
  { TidyAttr_WIDTH,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_TEXTAREA)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ACCESSKEY,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_AUTOFOCUS,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_COLS,                  HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DISABLED,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_FORM,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_MAXLENGTH,             xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_NAME,                  HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_OnBLUR,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCHANGE,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnFOCUS,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnSELECT,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_PLACEHOLDER,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_READONLY,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 },
  { TidyAttr_REQUIRED,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_ROWS,                  HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_SDAPREF,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TABINDEX,              xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_WRAP,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_TFOOT)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ALIGN,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_CHAR,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_CHAROFF,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_VALIGN,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_TH)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ABBR,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx },
  { TidyAttr_ALIGN,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx },
  { TidyAttr_AXIS,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx },
  { TidyAttr_BGCOLOR,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_CHAR,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_CHAROFF,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_COLSPAN,               xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_HEADERS,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_HEIGHT,                xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_NOWRAP,                xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ROWSPAN,               xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_SCOPE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_VALIGN,                xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx },
  { TidyAttr_WIDTH,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_THEAD)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ALIGN,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_CHAR,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_CHAROFF,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_VALIGN,                xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_TEMPLATE)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_TIME)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_DATETIME,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_PUBDATE,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_DATA)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_VALUE,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_TITLE)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_TR)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_ALIGN,                 xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx },
  { TidyAttr_BGCOLOR,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_CHAR,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_CHAROFF,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx },
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_VALIGN,                xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|xxxx|xxxx },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_TRACK)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_DEFAULT,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_KIND,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_LABEL,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_SRC,                   xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_SRCLANG,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_TT)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|xxxx|xxxx }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_U)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_UL)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_COMPACT,               HT20|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_TYPE,                  xxxx|HT32|H40T|H41T|X10T|H40F|H41F|X10F|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_VAR)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_CLASS,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_DIR,                   xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_ID,                    xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_LANG,                  xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|xxxx|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnCLICK,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnDBLCLICK,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYDOWN,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYPRESS,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnKEYUP,               xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEDOWN,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEMOVE,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOUT,            xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEOVER,           xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_OnMOUSEUP,             xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_STYLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|xxxx|HT50|XH50 }, /* CORE override */
  { TidyAttr_TITLE,                 xxxx|xxxx|H40T|H41T|X10T|H40F|H41F|X10F|H40S|H41S|X10S|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XMLNS,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|XH11|XB10|HT50|XH50 }, /* CORE override */
  { TidyAttr_XML_LANG,              xxxx|xxxx|xxxx|xxxx|X10T|xxxx|xxxx|X10F|xxxx|xxxx|X10S|XH11|XB10|xxxx|XH50 }, /* CORE override */
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_VIDEO)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_AUTOPLAY,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_CONTROLS,              xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_CROSSORIGIN,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_HEIGHT,                xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_LOOP,                  xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_MEDIAGROUP,            xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_MUTED,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_PLAYSINLINE,           xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_POSTER,                xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_PRELOAD,               xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_SRC,                   xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  { TidyAttr_WIDTH,                 xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|HT50|XH50 },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_WBR)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

const AttrVersion TY_(W3CAttrsFor_XMP)[] =
{
  INCLUDE_ARIA
  INCLUDE_MICRODATA
  { TidyAttr_SDAFORM,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  { TidyAttr_SDAPREF,               HT20|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx },
  INCLUDE_CORE_ATTRIBS
  INCLUDE_CORE_EVENTS
  INCLUDE_RDFA
  { TidyAttr_UNKNOWN,               0                                                                          },
};

/*
 * local variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 4
 * end:
 */
