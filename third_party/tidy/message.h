#ifndef __MESSAGE_H__
#define __MESSAGE_H__

/******************************************************************************
 * @file
 * Provides General Message Writing Routines
 *
 * This module handles LibTidy's high level output routines, as well as
 * provides lookup functions and management for keys used for retrieval
 * of these messages.
 *
 * LibTidy emits two general types of output:
 *
 *  - Reports, which contain data relating to what Tidy discovered in your
 *    source file, and/or what Tidy did to your source file. In some cases
 *    general information about your source file is emitted as well. Reports
 *    are emitted in the current output buffer, but LibTidy users will probably
 *    prefer to hook into a callback in order to take advantage of the data
 *    that are available in a more flexible way.
 *
 *  - Dialogue, consisting of footnotes related to your source file, and of
 *    general information that's not related to your source file in particular.
 *    This is also written to the current output buffer when appropriate, and
 *    available via callbacks.
 *
 * Report information typically takes the form of a warning, an error, info,
 * etc., and the output routines keep track of the count of these.
 *
 * The preferred way of handling Tidy diagnostics output is either
 *   - define a new output sink, or
 *   - use a message filter callback routine.
 *
 * @author  HTACG, et al (consult git log)
 *
 * @copyright
 *     Copyright (c) 1998-2017 World Wide Web Consortium (Massachusetts
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
#include "third_party/tidy/config.h"

/** @addtogroup internal_api */
/** @{ */


/***************************************************************************//**
 ** @defgroup message_releaseinfo Tidy Release Information
 **
 ** These functions return information about the current release version date
 ** and version number. Note that the latest release date or the highest
 ** version number alone do not guarantee the latest Tidy release, as we may
 ** backport important fixes to older releases of Tidy.
 **
 ** @{
 ******************************************************************************/

/**
 *  Returns the release date of this instance of HTML Tidy.
 */
ctmbstr TY_(ReleaseDate)(void);

/**
 *  Returns the release version of this instance of HTML Tidy.
 */
ctmbstr TY_(tidyLibraryVersion)(void);


/** @} message_releaseinfo group */


/***************************************************************************//**
 ** @defgroup message_reporting Report and Dialogue Writing Functions
 **
 ** These simple functions perform the vast majority of Tidy's output, and
 ** one these should be your first choice when adding your own output.
 **
 ** A report is typically diagnostic output that is generated each time Tidy
 ** detects an issue in your document or makes a change. A dialogue is a piece
 ** of information such as a summary, a footnote, or other non-tabular data.
 ** Some of these functions emit multiple reports or dialogue in order to
 ** effect a summary.
 **
 ** @{
 ******************************************************************************/

/** @name General Report Writing
 ** If one of the convenience reporting functions does not fit your required
 ** message signature, then this designated reporting function will fit the
 ** bill. Be sure to see if a message formatter exists that can handle the
 ** variable arguments.
 */
/** @{ */


/**
 *  The designated report writing function. When a proper formatter exists,
 *  this one function can handle all report output.
 */
void TY_(Report)(TidyDocImpl* doc, Node *element, Node *node, uint code, ...);


/** @} */
/** @name Convenience Reporting Functions
 ** These convenience reporting functions are able to handle the bulk of Tidy's
 ** necessary reporting, and avoid the danger of using a variadic if you are
 ** unfamiliar with Tidy.
 */
/** @{ */


void TY_(ReportAccessError)( TidyDocImpl* doc, Node* node, uint code );
void TY_(ReportAttrError)(TidyDocImpl* doc, Node *node, AttVal *av, uint code);
void TY_(ReportBadArgument)( TidyDocImpl* doc, ctmbstr option );
void TY_(ReportEntityError)( TidyDocImpl* doc, uint code, ctmbstr entity, int c );
void TY_(ReportFileError)( TidyDocImpl* doc, ctmbstr file, uint code );
void TY_(ReportEncodingError)(TidyDocImpl* doc, uint code, uint c, Bool discarded);
void TY_(ReportEncodingWarning)(TidyDocImpl* doc, uint code, uint encoding);
void TY_(ReportMissingAttr)( TidyDocImpl* doc, Node* node, ctmbstr name );
void TY_(ReportSurrogateError)(TidyDocImpl* doc, uint code, uint c1, uint c2);
void TY_(ReportUnknownOption)( TidyDocImpl* doc, ctmbstr option );


/** @} */
/** @name General Dialogue Writing
 ** These functions produce dialogue output such as individual messages, or
 ** several messages in summary form.
 */
/** @{ */


/**
 *  Emits a single dialogue message, and is capable of accepting a variadic
 *  that is passed to the correct message formatter as needed.
 */
void TY_(Dialogue)( TidyDocImpl* doc, uint code, ... );


/** @} */
/** @name Output Dialogue Information */
/** @{ */


/**
 *  Outputs the footnotes and other dialogue information after document cleanup
 *  is complete. LibTidy users might consider capturing these individually in
 *  the message callback rather than capturing this entire buffer.
 *  Called by `tidyErrorSummary()`, in console.
 *  @todo: This name is a bit misleading and should probably be renamed to
 *  indicate its focus on printing footnotes.
 */
void TY_(ErrorSummary)( TidyDocImpl* doc );


/**
 *  Outputs document HTML version and version-related information as the final
 *  report(s) in the report table.
 *  Called by `tidyRunDiagnostics()`, from console.
 *  Called by `tidyDocReportDoctype()`, currently unused.
 */
void TY_(ReportMarkupVersion)( TidyDocImpl* doc );


/**
 *  Reports the number of warnings and errors found in the document as dialogue
 *  information.
 *  Called by `tidyRunDiagnostics()`, from console.
 */
void TY_(ReportNumWarnings)( TidyDocImpl* doc );


/** @} */
/** @} message_reporting group */


/***************************************************************************//**
 ** @defgroup message_mutinging Message Muting
 **
 ** Message types included in the `mute` option will be be printed in
 ** messageOut().
 **
 ** @{
 ******************************************************************************/

/** Maintains a list of messages not to display. */
typedef struct _mutedMessages {
    tidyStrings* list; /**< A list of messages that won't be output. */
    uint count;        /**< Current count of the list. */
    uint capacity;     /**< Current capacity of the list. */
} TidyMutedMessages;


/** Frees the list of muted messages.
 ** @param doc The Tidy document.
 */
void TY_(FreeMutedMessageList)( TidyDocImpl* doc );

/** Adds a new message ID to the list of muted messages.
 ** @param doc The Tidy document.
 ** @param opt The option that is defining the muted message.
 ** @param name The message code as a string.
 */
void TY_(DefineMutedMessage)( TidyDocImpl* doc, const TidyOptionImpl* opt, ctmbstr name );

/** Start an iterator for muted messages.
 ** @param doc The Tidy document.
 ** @returns Returns an iterator token.
 */
TidyIterator TY_(getMutedMessageList)( TidyDocImpl* doc );

/** Get the next priority attribute.
 ** @param doc The Tidy document.
 ** @param iter The iterator token.
 ** @returns The next priority attribute.
 */
ctmbstr TY_(getNextMutedMessage)( TidyDocImpl* doc, TidyIterator* iter );


/** @} message_muting group */


/***************************************************************************//**
 ** @defgroup message_keydiscovery Key Discovery
 **
 ** LibTidy users may want to use `TidyReportCallback` to enable their own
 ** localization lookup features. Because Tidy's report codes are enums the
 ** specific values can change over time. Using these functions provides the
 ** ability for LibTidy users to use LibTidy's enum values as strings for
 ** lookup purposes.
 **
 ** @{
 ******************************************************************************/

/**
 *  This function returns a string representing the enum value name that can
 *  be used as a lookup key independent of changing string values.
 *  `TidyReportCallback` will return this general string as the report
 *  message key.
 */
ctmbstr TY_(tidyErrorCodeAsKey)(uint code);

/**
 *  Given an error code string, return the integer value of it, or UINT_MAX
 *  as an error flag.
 */
uint TY_(tidyErrorCodeFromKey)(ctmbstr code);


/**
 *  Initializes the TidyIterator to point to the first item
 *  in Tidy's list of error codes that can be return with
 *  `TidyReportFilter3`.
 *  Items can be retrieved with getNextErrorCode();
 */
TidyIterator TY_(getErrorCodeList)(void);

/**
 *  Returns the next error code having initialized the iterator
 *  with `getErrorCodeList()`. You can use tidyErrorCodeAsKey
 *  to determine the key for this value.
 */
uint TY_(getNextErrorCode)( TidyIterator* iter );


/** @} message_keydiscovery group */
/** @} internal_api addtogroup */



/* accessibility flaws */

#define BA_MISSING_IMAGE_ALT       1
#define BA_MISSING_LINK_ALT        2
#define BA_MISSING_SUMMARY         4
#define BA_MISSING_IMAGE_MAP       8
#define BA_USING_FRAMES            16
#define BA_USING_NOFRAMES          32
#define BA_INVALID_LINK_NOFRAMES   64  /* WAI [6.5.1.4] */
#define BA_WAI                     (1 << 31)

/* presentation flaws */

#define USING_SPACER            1
#define USING_LAYER             2
#define USING_NOBR              4
#define USING_FONT              8
#define USING_BODY              16

/* badchar bit field */

#define BC_VENDOR_SPECIFIC_CHARS   1
#define BC_INVALID_SGML_CHARS      2
#define BC_INVALID_UTF8            4
#define BC_INVALID_UTF16           8
#define BC_ENCODING_MISMATCH       16 /* fatal error */
#define BC_INVALID_URI             32
#define BC_INVALID_NCR             64

/* other footnote bit field (temporary until formalized) */

#define FN_TRIM_EMPTY_ELEMENT     1

/* Lexer and I/O Macros */

#define REPLACED_CHAR           0
#define DISCARDED_CHAR          1


#endif /* __MESSAGE_H__ */
