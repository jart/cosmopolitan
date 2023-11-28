#ifndef __ACCESS_H__
#define __ACCESS_H__

/*********************************************************************
 * carry out accessibility checks
 *
 * This module carries out processes for all accessibility checks. It
 * traverses through all the content within the tree and evaluates the
 * tags for accessibility.
 *
 * To perform the following checks, 'AccessibilityChecks' must be
 * called AFTER the tree structure has been formed.
 *
 * If, in the command prompt or configuration file, there is no
 * specification of which accessibility priorities to check, then no
 * accessibility checks will be performed.
 *
 * The accessibility checks to perform depending on user's desire:
 *   1. priority 1
 *   2. priority 1 & 2
 *   3. priority 1, 2, & 3
 *
 * Reference document: https://www.w3.org/TR/WAI-WEBCONTENT/
 *
 * Copyright University of Toronto
 * Portions (c) 1998-2006 (W3C) MIT, ERCIM, Keio University
 * See `tidy.h` for the copyright notice.
 * Programmed by: Mike Lam and Chris Ridpath
 * Modifications by: Terry Teague (TRT)
 * Further modifications: consult git log.
 *********************************************************************/

#include "third_party/tidy/forward.h"


enum {
  TEXTBUF_SIZE=128u
};

struct _TidyAccessImpl;
typedef struct _TidyAccessImpl TidyAccessImpl;

struct _TidyAccessImpl
{
    /* gets set from Tidy variable AccessibilityCheckLevel */
    int PRIORITYCHK; /**< */

    /* Number of characters that are found within the concatenated text */
    int counter;

    /* list of characters in the text nodes found within a container element */
    tmbchar textNode[ TEXTBUF_SIZE ];

    /* The list of characters found within one text node */
    tmbchar text[ TEXTBUF_SIZE ];

    /* Number of frame elements found within a frameset */
    int numFrames;

    /* Number of 'longdesc' attributes found within a frameset */
    int HasCheckedLongDesc;

    int  CheckedHeaders;
    int  ListElements;
    int  OtherListElements;

    /* For 'USEMAP' identifier */
    Bool HasUseMap;
    Bool HasName;
    Bool HasMap;

    /* For tracking nodes that are deleted from the original parse tree - TRT */
    /* Node *access_tree; */

    Bool HasTH;
    Bool HasValidFor;
    Bool HasValidId;
    Bool HasValidRowHeaders;
    Bool HasValidColumnHeaders;
    Bool HasInvalidRowHeader;
    Bool HasInvalidColumnHeader;
    int  ForID;

};


void TY_(AccessibilityChecks)( TidyDocImpl* doc );


#endif /* __ACCESS_H__ */
