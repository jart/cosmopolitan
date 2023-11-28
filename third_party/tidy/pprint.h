#ifndef __PPRINT_H__
#define __PPRINT_H__

/**************************************************************************//**
 * @file
 * Pretty Print the parse tree.
 *
 * Pretty printer for HTML and XML documents.
 *   - Block-level and unknown elements are printed on new lines and
 *     their contents indented with a user configured amount of spaces/tabs.
 *   - Inline elements are printed inline.
 *   - Inline content is wrapped on spaces (except in attribute values or
 *     preformatted text, after start tags and before end tags.
 *
 * @author  HTACG, et al (consult git log)
 *
 * @copyright
 *     Copyright (c) 1998-2021 World Wide Web Consortium (Massachusetts
 *     Institute of Technology, European Research Consortium for Informatics
 *     and Mathematics, Keio University) and HTACG.
 * @par
 *     All Rights Reserved.
 * @par
 *     See `tidy.h` for the complete license.
 *
 * @date Additional updates: consult git log
 *
 ******************************************************************************/

#include "third_party/tidy/forward.h"

/** @addtogroup internal_api */
/** @{ */


/***************************************************************************//**
 ** @defgroup print_h HTML and XML Pretty Printing
 **
 ** These functions and structures form the internal API for document
 ** printing.
 **
 ** @{
 ******************************************************************************/


/**
 *  This typedef represents the current pretty-printing mode, and instructs
 *  the printer behavior per the content currently being output.
 */
typedef enum {
    NORMAL       = 0u,  /**< Normal output. */
    PREFORMATTED = 1u,  /**< Preformatted output. */
    COMMENT      = 2u,  /**< Comment. */
    ATTRIBVALUE  = 4u,  /**< An attribute's value. */
    NOWRAP       = 8u,  /**< Content that should not be wrapped. */
    CDATA        = 16u  /**< CDATA content. */
} PrettyPrintMode;


/**
 *  A record of the state of a single line, capturing the indent
 *  level, in-attribute, and in-string state of a line. Instances
 *  of this record are used by the pretty-printing buffer.
 *
 *  The pretty printer keeps at most two lines of text in the
 *  buffer before flushing output.  We need to capture the
 *  indent state (indent level) at the _beginning_ of _each_
 *  line, not the end of just the second line.
 *
 *  We must also keep track "In Attribute" and "In String"
 *  states at the _end_ of each line,
 */
typedef struct _TidyIndent
{
    int spaces;           /**< Indent level of the line. */
    int attrValStart;     /**< Attribute in-value state. */
    int attrStringStart;  /**< Attribute in-string state. */
} TidyIndent;


/**
 *  The pretty-printing buffer.
 */
typedef struct _TidyPrintImpl
{
    TidyAllocator *allocator; /**< Allocator */

    uint *linebuf;            /**< The line buffer proper. */
    uint lbufsize;            /**< Current size of the buffer. */
    uint linelen;             /**< Current line length. */
    uint wraphere;            /**< Point in the line to wrap text. */
    uint line;                /**< Current line. */

    uint ixInd;               /**< Index into the indent[] array. */
    TidyIndent indent[2];     /**< Two lines worth of indent state */
} TidyPrintImpl;


/**
 *  Allocates and initializes the pretty-printing buffer for a Tidy document.
 */
void TY_(InitPrintBuf)( TidyDocImpl* doc );


/**
 *  Deallocates and free a Tidy document's pretty-printing buffer.
 */
void TY_(FreePrintBuf)( TidyDocImpl* doc );


/**
 *  Flushes the current buffer to the actual output sink.
 */
void TY_(PFlushLine)( TidyDocImpl* doc, uint indent );


/**
 *  Print just the content of the HTML body element, which is useful when
 *  want to reuse material from other documents.
 *  -- Sebastiano Vigna <vigna@dsi.unimi.it>
 */
void TY_(PrintBody)( TidyDocImpl* doc );


/**
 *  Print the HTML document tree for the given document using the given node
 *  as the root of the document. Note that you can print an entire document
 *  node as body using PPrintTree()
 */
void TY_(PPrintTree)( TidyDocImpl* doc, uint mode, uint indent, Node *node );


/**
 *  Print the XML document tree for the given document using the given node
 *  as the root of the document.
 */
void TY_(PPrintXMLTree)( TidyDocImpl* doc, uint mode, uint indent, Node *node );


/** @} end print_h group */
/** @} end internal_api group */

#endif /* __PPRINT_H__ */
