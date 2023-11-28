/* message.c -- General Message Writing Routines

  (c) 1998-2017 (W3C) MIT, ERCIM, Keio University, University of
  Toronto, HTACG
  See tidy.h for the copyright notice.

*/

#include "third_party/tidy/message.h"
#include "third_party/tidy/messageobj.h"
#include "third_party/tidy/tidy-int.h"
#include "third_party/tidy/lexer.h"
#include "third_party/tidy/streamio.h"
#include "third_party/tidy/tmbstr.h"
#include "third_party/tidy/utf8.h"
#include "libc/assert.h"
#include "libc/limits.h"
#include "third_party/tidy/version.inc"


/*********************************************************************
 * Release Information
 *********************************************************************/

ctmbstr TY_(ReleaseDate)(void)
{
  return TY_(release_date);
}


ctmbstr TY_(tidyLibraryVersion)(void)
{
  return TY_(library_version);
}


/*********************************************************************
 * General Message Utility Functions
 *********************************************************************/

/* Returns the given node's tag as a string. */
static char* TagToString(Node* tag, char* buf, size_t count)
{
    *buf = 0;
    if (tag)
    {
        if (TY_(nodeIsElement)(tag))
            TY_(tmbsnprintf)(buf, count, "<%s>", tag->element);
        else if (tag->type == EndTag)
            TY_(tmbsnprintf)(buf, count, "</%s>", tag->element);
        else if (tag->type == DocTypeTag)
            TY_(tmbsnprintf)(buf, count, "<!DOCTYPE>");
        else if (tag->type == TextNode)
            TY_(tmbsnprintf)(buf, count, "%s", "STRING_PLAIN_TEXT");
        else if (tag->type == XmlDecl)
            TY_(tmbsnprintf)(buf, count, "%s", "STRING_XML_DECLARATION");
        else if (tag->element)
            TY_(tmbsnprintf)(buf, count, "%s", tag->element);
    }
    return buf + TY_(tmbstrlen)(buf);
}


/* Convert an integer to a string. */
static void NtoS(int n, tmbstr str)
{
    tmbchar buf[40];
    int i;

    for (i = 0;; ++i)
    {
        buf[i] = (tmbchar)( (n % 10) + '0' );

        n = n / 10;

        if (n == 0)
            break;
    }

    n = i;

    while (i >= 0)
    {
        str[n-i] = buf[i];
        --i;
    }

    str[n+1] = '\0';
}


/* Get an HTML version string */
static ctmbstr HTMLVersion( TidyDocImpl* doc )
{
    uint versionEmitted = doc->lexer->versionEmitted;
    uint declared = doc->lexer->doctype;
    uint version = versionEmitted == 0 ? declared : versionEmitted;
    ctmbstr result = TY_(HTMLVersionNameFromCode)(version, 0);
    if (!result)
        result = tidyLocalizedString(STRING_HTML_PROPRIETARY);
    return result;
}


/*********************************************************************
 * Message Writing Functions
 * These functions provide final, formatted output to the output sink.
 *********************************************************************/


/* Writes messages to the output sink unless they are suppressed by one of the
** message callback filters, or suppressed by the configuration settings.
** Report messages are messages that are included in the "error table," and
** dialogue messages are any other output that Tidy traditionally emits.
*/
static void messageOut( TidyMessageImpl *message )
{
    TidyDocImpl *doc;
    Bool go = yes;

    if ( !message )
        return;

    doc = message->tidyDoc;

    /* The filter has had a chance to suppress *any* message from output. */
    go = message->allowMessage;

    /* Update the count of each report type. */
    switch ( message->level )
    {
        case TidyInfo:
            doc->infoMessages++;
            break;
        case TidyWarning:
            doc->warnings++;
            break;
        case TidyConfig:
            doc->optionErrors++;
            break;
        case TidyAccess:
            doc->accessErrors++;
            break;
        case TidyError:
            doc->errors++;
            break;
        case TidyBadDocument:
            doc->docErrors++;
            break;
        case TidyFatal:
            /* Ack! */
            break;
        default:
            break;
    }

    /* Suppress report messages if they've been muted. */
    go = go & !message->muted;

    /* Suppress report messages if we've already reached the reporting limit. */
    if ( message->level <= TidyFatal )
    {
        go = go & ( doc->errors < cfg(doc, TidyShowErrors) );
    }

    /* Let TidyQuiet silence a lot of things. */
    if ( cfgBool( doc, TidyQuiet ) == yes )
    {
        go = go && message->code != STRING_DOCTYPE_GIVEN;
        go = go && message->code != STRING_CONTENT_LOOKS;
        go = go && message->code != STRING_NO_SYSID;
        go = go && message->level != TidyDialogueInfo;
        go = go && message->level != TidyConfig;
        go = go && message->level != TidyInfo;
        go = go && !(message->level >= TidyDialogueSummary &&
                            message->code != STRING_NEEDS_INTERVENTION);
    }

    /* Let !TidyShowInfo silence some things. */
    if ( cfgBool( doc, TidyShowInfo ) == no )
    {
        go = go && message->level != TidyInfo;

        /* Temporary; TidyShowInfo shouldn't affect TidyDialogueInfo, but
           right now such messages are hidden until we granularize the
           output controls. */
        go = go && message->level != TidyDialogueInfo;
    }

    /* Let !TidyShowWarnings silence some things. */
    if ( cfgBool( doc, TidyShowWarnings ) == no )
    {
        go = go && message->level != TidyWarning;
    }

    /* Output the message if applicable. */
    if ( go )
    {
        TidyOutputSink *outp = &doc->errout->sink;
        ctmbstr cp;
        byte b = '\0';
        for ( cp = message->messageOutput; *cp; ++cp )
        {
            b = (*cp & 0xff);
            if (b == (byte)'\n')
                TY_(WriteChar)( b, doc->errout );   /* for EOL translation */
            else
                outp->putByte( outp->sinkData, b ); /* #383 - no encoding */
        }

        /* Always add a trailing newline. Reports require this, and dialogue
           messages will be better spaced out without having to fill the
           language file with superfluous newlines. */
        TY_(WriteChar)( '\n', doc->errout );
    }

    TY_(tidyMessageRelease)(message);
}


/*********************************************************************
 * Report Formatting
 * In order to provide a single, predictable reporting system, Tidy
 * provides an extensible messaging system that provides most of the
 * basic requirements for most reports, while permitting simple
 * implementation of new reports in a flexible manner. By adding
 * additional formatters, new messages can be added easily while
 * maintaining Tidy's internal organization.
 *********************************************************************/


/* Functions of this type will create new instances of TidyMessage specific to
** the type of report being emitted. Many messages share the same formatter for
** messages, but new ones can be written as required. Please have a look at
** the existing formatters in order to determine if an existing signature is
** compatible with the report you wish to output before adding a new formatter.
** In particular, if an existing formatter provides most of the local variables
** required to populate your format string, try to use it.
*/
typedef TidyMessageImpl*(messageFormatter)(TidyDocImpl* doc, Node *element, Node *node, uint code, uint level, va_list args);


/* Forward declarations of messageFormatter functions. */
static messageFormatter formatAccessReport;
static messageFormatter formatAttributeReport;
static messageFormatter formatEncodingReport;
static messageFormatter formatStandard;
static messageFormatter formatStandardDynamic;


/* This structure ties together for each report Code the default
** TidyReportLevel, the Formatter to be used to construct the message, and the
** next code to output, if applicable. Assuming an existing formatter can,
** this it makes it simple to output new reports, or to change report level by
** modifying this array.
*/
static struct _dispatchTable {
    uint code;                 /**< The message code. */
    TidyReportLevel level;     /**< The default TidyReportLevel of the message. */
    messageFormatter *handler; /**< The formatter for the report. */
    uint next;                 /**< If multiple codes should be displayed, which is next? */
} dispatchTable[] = {
    { ADDED_MISSING_CHARSET,        TidyInfo,        formatStandard          },
    { ANCHOR_NOT_UNIQUE,            TidyWarning,     formatAttributeReport   },
    { ANCHOR_DUPLICATED,            TidyWarning,     formatAttributeReport   },
    { APOS_UNDEFINED,               TidyWarning,     formatStandard          },
    { ATTR_VALUE_NOT_LCASE,         TidyWarning,     formatAttributeReport   },
    { ATTRIBUTE_VALUE_REPLACED,     TidyInfo,        formatAttributeReport   },
    { ATTRIBUTE_IS_NOT_ALLOWED,     TidyWarning,     formatAttributeReport   },
    { BACKSLASH_IN_URI,             TidyWarning,     formatAttributeReport   },
    { BAD_ATTRIBUTE_VALUE_REPLACED, TidyWarning,     formatAttributeReport   },
    { BAD_ATTRIBUTE_VALUE,          TidyWarning,     formatAttributeReport   },
    { BAD_CDATA_CONTENT,            TidyWarning,     formatStandard          },
    { BAD_SUMMARY_HTML5,            TidyWarning,     formatStandard          },
    { BAD_SURROGATE_LEAD,           TidyWarning,     formatStandard          },
    { BAD_SURROGATE_PAIR,           TidyWarning,     formatStandard          },
    { BAD_SURROGATE_TAIL,           TidyWarning,     formatStandard          },
    { CANT_BE_NESTED,               TidyWarning,     formatStandard          },
    { COERCE_TO_ENDTAG,             TidyWarning,     formatStandard          },
    { CONTENT_AFTER_BODY,           TidyWarning,     formatStandard          },
    { CUSTOM_TAG_DETECTED,          TidyInfo,        formatStandard          },
    { DISCARDING_UNEXPECTED,        0,               formatStandardDynamic   },
    { DOCTYPE_AFTER_TAGS,           TidyWarning,     formatStandard          },
    { DUPLICATE_FRAMESET,           TidyError,       formatStandard          },
    { ELEMENT_NOT_EMPTY,            TidyWarning,     formatStandard          },
    { ELEMENT_VERS_MISMATCH_ERROR,  TidyError,       formatStandard          },
    { ELEMENT_VERS_MISMATCH_WARN,   TidyWarning,     formatStandard          },
    { ENCODING_MISMATCH,            TidyWarning,     formatEncodingReport    },
    { ESCAPED_ILLEGAL_URI,          TidyWarning,     formatAttributeReport   },
    { FILE_CANT_OPEN,               TidyBadDocument, formatStandard          },
    { FILE_CANT_OPEN_CFG,           TidyBadDocument, formatStandard          },
    { FILE_NOT_FILE,                TidyBadDocument, formatStandard          },
    { FIXED_BACKSLASH,              TidyWarning,     formatAttributeReport   },
    { FOUND_STYLE_IN_BODY,          TidyWarning,     formatStandard          },
    { ID_NAME_MISMATCH,             TidyWarning,     formatAttributeReport   },
    { ILLEGAL_NESTING,              TidyWarning,     formatStandard          },
    { ILLEGAL_URI_CODEPOINT,        TidyWarning,     formatAttributeReport   },
    { ILLEGAL_URI_REFERENCE,        TidyWarning,     formatAttributeReport   },
    { INSERTING_AUTO_ATTRIBUTE,     TidyWarning,     formatAttributeReport   },
    { INSERTING_TAG,                TidyWarning,     formatStandard          },
    { INVALID_ATTRIBUTE,            TidyWarning,     formatAttributeReport   },
    { INVALID_NCR,                  TidyWarning,     formatEncodingReport    },
    { INVALID_SGML_CHARS,           TidyWarning,     formatEncodingReport    },
    { INVALID_UTF8,                 TidyWarning,     formatEncodingReport    },
    { INVALID_UTF16,                TidyWarning,     formatEncodingReport    },
    { INVALID_XML_ID,               TidyWarning,     formatAttributeReport   },
    { JOINING_ATTRIBUTE,            TidyWarning,     formatAttributeReport   },
    { MALFORMED_COMMENT,            TidyInfo,        formatStandard          },
    { MALFORMED_COMMENT_EOS,        TidyError,       formatStandard          },
    { MALFORMED_COMMENT_DROPPING,   TidyWarning,     formatStandard          },
    { MALFORMED_COMMENT_WARN,       TidyWarning,     formatStandard          },
    { MALFORMED_DOCTYPE,            TidyWarning,     formatStandard          },
    { MISMATCHED_ATTRIBUTE_ERROR,   TidyError,       formatAttributeReport   },
    { MISMATCHED_ATTRIBUTE_WARN,    TidyWarning,     formatAttributeReport   },
    { MISSING_ATTR_VALUE,           TidyWarning,     formatAttributeReport   },
    { MISSING_ATTRIBUTE,            TidyWarning,     formatStandard          },
    { MISSING_DOCTYPE,              TidyWarning,     formatStandard          },
    { MISSING_ENDTAG_BEFORE,        TidyWarning,     formatStandard          },
    { MISSING_ENDTAG_FOR,           TidyWarning,     formatStandard          },
    { MISSING_ENDTAG_OPTIONAL,      TidyInfo,        formatStandard          },
    { MISSING_IMAGEMAP,             TidyWarning,     formatAttributeReport   },
    { MISSING_QUOTEMARK,            TidyWarning,     formatAttributeReport   },
    { MISSING_QUOTEMARK_OPEN,       TidyInfo,        formatAttributeReport   },
    { MISSING_SEMICOLON_NCR,        TidyWarning,     formatStandard          },
    { MISSING_SEMICOLON,            TidyWarning,     formatStandard          },
    { MISSING_STARTTAG,             TidyWarning,     formatStandard          },
    { MISSING_TITLE_ELEMENT,        TidyWarning,     formatStandard          },
    { MOVED_STYLE_TO_HEAD,          TidyWarning,     formatStandard          },
    { NESTED_EMPHASIS,              TidyWarning,     formatStandard          },
    { NESTED_QUOTATION,             TidyWarning,     formatStandard          },
    { NEWLINE_IN_URI,               TidyWarning,     formatAttributeReport   },
    { NOFRAMES_CONTENT,             TidyWarning,     formatStandard          },
    { NON_MATCHING_ENDTAG,          TidyWarning,     formatStandard          },
    { OBSOLETE_ELEMENT,             TidyWarning,     formatStandard          },
    { OPTION_REMOVED,               TidyConfig,      formatStandard          },
    { OPTION_REMOVED_APPLIED,       TidyConfig,      formatStandard          },
    { OPTION_REMOVED_UNAPPLIED,     TidyConfig,      formatStandard          },
    { PREVIOUS_LOCATION,            TidyInfo,        formatStandard          },
    { PROPRIETARY_ATTR_VALUE,       TidyWarning,     formatAttributeReport   },
    { PROPRIETARY_ATTRIBUTE,        TidyWarning,     formatAttributeReport   },
    { PROPRIETARY_ELEMENT,          TidyWarning,     formatStandard          },
    { REMOVED_HTML5,                TidyWarning,     formatStandard          },
    { REPEATED_ATTRIBUTE,           TidyWarning,     formatAttributeReport   },
    { REPLACING_ELEMENT,            TidyWarning,     formatStandard          },
    { REPLACING_UNEX_ELEMENT,       TidyWarning,     formatStandard          },
    { SPACE_PRECEDING_XMLDECL,      TidyWarning,     formatStandard          },
    { STRING_ARGUMENT_BAD,          TidyConfig,      formatStandard          },
    { STRING_CONTENT_LOOKS,         TidyInfo,        formatStandard          }, /* reportMarkupVersion() */
    { STRING_DOCTYPE_GIVEN,         TidyInfo,        formatStandard          }, /* reportMarkupVersion() */
    { STRING_MISSING_MALFORMED,     TidyConfig,      formatStandard          },
    { STRING_MUTING_TYPE,           TidyInfo,        formatStandard          },
    { STRING_NO_SYSID,              TidyInfo,        formatStandard          }, /* reportMarkupVersion() */
    { STRING_UNKNOWN_OPTION,        TidyConfig,      formatStandard          },
    { SUSPECTED_MISSING_QUOTE,      TidyWarning,     formatStandard          },
    { TAG_NOT_ALLOWED_IN,           TidyWarning,     formatStandard, PREVIOUS_LOCATION },
    { TOO_MANY_ELEMENTS_IN,         TidyWarning,     formatStandard, PREVIOUS_LOCATION },
    { TOO_MANY_ELEMENTS,            TidyWarning,     formatStandard          },
    { TRIM_EMPTY_ELEMENT,           TidyWarning,     formatStandard          },
    { UNESCAPED_AMPERSAND,          TidyWarning,     formatStandard          },
    { UNEXPECTED_END_OF_FILE_ATTR,  TidyWarning,     formatAttributeReport   },
    { UNEXPECTED_END_OF_FILE,       TidyWarning,     formatStandard          },
    { UNEXPECTED_ENDTAG_IN,         TidyError,       formatStandard          },
    { UNEXPECTED_ENDTAG,            TidyWarning,     formatStandard          },
    { UNEXPECTED_ENDTAG_ERR,        TidyError,       formatStandard          },
    { UNEXPECTED_EQUALSIGN,         TidyWarning,     formatAttributeReport   },
    { UNEXPECTED_GT,                TidyWarning,     formatAttributeReport   },
    { UNEXPECTED_QUOTEMARK,         TidyWarning,     formatAttributeReport   },
    { UNKNOWN_ELEMENT_LOOKS_CUSTOM, TidyError,       formatStandard          },
    { UNKNOWN_ELEMENT,              TidyError,       formatStandard          },
    { UNKNOWN_ENTITY,               TidyWarning,     formatStandard          },
    { USING_BR_INPLACE_OF,          TidyWarning,     formatStandard          },
    { VENDOR_SPECIFIC_CHARS,        TidyWarning,     formatEncodingReport    },
    { WHITE_IN_URI,                 TidyWarning,     formatAttributeReport   },
    { XML_DECLARATION_DETECTED,     TidyWarning,     formatStandard          },
    { XML_ID_SYNTAX,                TidyWarning,     formatAttributeReport   },
    { BLANK_TITLE_ELEMENT,          TidyWarning,     formatStandard          },

    { APPLET_MISSING_ALT,                            TidyAccess, formatAccessReport },
    { AREA_MISSING_ALT,                              TidyAccess, formatAccessReport },
    { ASCII_REQUIRES_DESCRIPTION,                    TidyAccess, formatAccessReport },
    { ASSOCIATE_LABELS_EXPLICITLY,                   TidyAccess, formatAccessReport },
    { ASSOCIATE_LABELS_EXPLICITLY_FOR,               TidyAccess, formatAccessReport },
    { ASSOCIATE_LABELS_EXPLICITLY_ID,                TidyAccess, formatAccessReport },
    { AUDIO_MISSING_TEXT_AIFF,                       TidyAccess, formatAccessReport },
    { AUDIO_MISSING_TEXT_AU,                         TidyAccess, formatAccessReport },
    { AUDIO_MISSING_TEXT_RA,                         TidyAccess, formatAccessReport },
    { AUDIO_MISSING_TEXT_RM,                         TidyAccess, formatAccessReport },
    { AUDIO_MISSING_TEXT_SND,                        TidyAccess, formatAccessReport },
    { AUDIO_MISSING_TEXT_WAV,                        TidyAccess, formatAccessReport },
    { COLOR_CONTRAST_ACTIVE_LINK,                    TidyAccess, formatAccessReport },
    { COLOR_CONTRAST_LINK,                           TidyAccess, formatAccessReport },
    { COLOR_CONTRAST_TEXT,                           TidyAccess, formatAccessReport },
    { COLOR_CONTRAST_VISITED_LINK,                   TidyAccess, formatAccessReport },
    { DATA_TABLE_MISSING_HEADERS,                    TidyAccess, formatAccessReport },
    { DATA_TABLE_MISSING_HEADERS_COLUMN,             TidyAccess, formatAccessReport },
    { DATA_TABLE_MISSING_HEADERS_ROW,                TidyAccess, formatAccessReport },
    { DATA_TABLE_REQUIRE_MARKUP_COLUMN_HEADERS,      TidyAccess, formatAccessReport },
    { DATA_TABLE_REQUIRE_MARKUP_ROW_HEADERS,         TidyAccess, formatAccessReport },
    { DOCTYPE_MISSING,                               TidyAccess, formatAccessReport },
    { ENSURE_PROGRAMMATIC_OBJECTS_ACCESSIBLE_APPLET, TidyAccess, formatAccessReport },
    { ENSURE_PROGRAMMATIC_OBJECTS_ACCESSIBLE_EMBED,  TidyAccess, formatAccessReport },
    { ENSURE_PROGRAMMATIC_OBJECTS_ACCESSIBLE_OBJECT, TidyAccess, formatAccessReport },
    { ENSURE_PROGRAMMATIC_OBJECTS_ACCESSIBLE_SCRIPT, TidyAccess, formatAccessReport },
    { FRAME_MISSING_LONGDESC,                        TidyAccess, formatAccessReport },
    { FRAME_MISSING_NOFRAMES,                        TidyAccess, formatAccessReport },
    { FRAME_MISSING_TITLE,                           TidyAccess, formatAccessReport },
    { FRAME_SRC_INVALID,                             TidyAccess, formatAccessReport },
    { FRAME_TITLE_INVALID_NULL,                      TidyAccess, formatAccessReport },
    { FRAME_TITLE_INVALID_SPACES,                    TidyAccess, formatAccessReport },
    { HEADER_USED_FORMAT_TEXT,                       TidyAccess, formatAccessReport },
    { HEADERS_IMPROPERLY_NESTED,                     TidyAccess, formatAccessReport },
    { IMAGE_MAP_SERVER_SIDE_REQUIRES_CONVERSION,     TidyAccess, formatAccessReport },
    { IMG_ALT_SUSPICIOUS_FILE_SIZE,                  TidyAccess, formatAccessReport },
    { IMG_ALT_SUSPICIOUS_FILENAME,                   TidyAccess, formatAccessReport },
    { IMG_ALT_SUSPICIOUS_PLACEHOLDER,                TidyAccess, formatAccessReport },
    { IMG_ALT_SUSPICIOUS_TOO_LONG,                   TidyAccess, formatAccessReport },
    { IMG_BUTTON_MISSING_ALT,                        TidyAccess, formatAccessReport },
    { IMG_MAP_CLIENT_MISSING_TEXT_LINKS,             TidyAccess, formatAccessReport },
    { IMG_MAP_SERVER_REQUIRES_TEXT_LINKS,            TidyAccess, formatAccessReport },
    { IMG_MISSING_ALT,                               TidyAccess, formatAccessReport },
    { IMG_MISSING_DLINK,                             TidyAccess, formatAccessReport },
    { IMG_MISSING_LONGDESC,                          TidyAccess, formatAccessReport },
    { IMG_MISSING_LONGDESC_DLINK,                    TidyAccess, formatAccessReport },
    { INFORMATION_NOT_CONVEYED_APPLET,               TidyAccess, formatAccessReport },
    { INFORMATION_NOT_CONVEYED_IMAGE,                TidyAccess, formatAccessReport },
    { INFORMATION_NOT_CONVEYED_INPUT,                TidyAccess, formatAccessReport },
    { INFORMATION_NOT_CONVEYED_OBJECT,               TidyAccess, formatAccessReport },
    { INFORMATION_NOT_CONVEYED_SCRIPT,               TidyAccess, formatAccessReport },
    { LANGUAGE_INVALID,                              TidyAccess, formatAccessReport },
    { LANGUAGE_NOT_IDENTIFIED,                       TidyAccess, formatAccessReport },
    { LAYOUT_TABLE_INVALID_MARKUP,                   TidyAccess, formatAccessReport },
    { LAYOUT_TABLES_LINEARIZE_PROPERLY,              TidyAccess, formatAccessReport },
    { LINK_TEXT_MISSING,                             TidyAccess, formatAccessReport },
    { LINK_TEXT_NOT_MEANINGFUL,                      TidyAccess, formatAccessReport },
    { LINK_TEXT_NOT_MEANINGFUL_CLICK_HERE,           TidyAccess, formatAccessReport },
    { LINK_TEXT_TOO_LONG,                            TidyAccess, formatAccessReport },
    { LIST_USAGE_INVALID_LI,                         TidyAccess, formatAccessReport },
    { LIST_USAGE_INVALID_OL,                         TidyAccess, formatAccessReport },
    { LIST_USAGE_INVALID_UL,                         TidyAccess, formatAccessReport },
    { METADATA_MISSING,                              TidyAccess, formatAccessReport },
    { METADATA_MISSING_REDIRECT_AUTOREFRESH,         TidyAccess, formatAccessReport },
    { MULTIMEDIA_REQUIRES_TEXT,                      TidyAccess, formatAccessReport },
    { NEW_WINDOWS_REQUIRE_WARNING_BLANK,             TidyAccess, formatAccessReport },
    { NEW_WINDOWS_REQUIRE_WARNING_NEW,               TidyAccess, formatAccessReport },
    { NOFRAMES_INVALID_CONTENT,                      TidyAccess, formatAccessReport },
    { NOFRAMES_INVALID_LINK,                         TidyAccess, formatAccessReport },
    { NOFRAMES_INVALID_NO_VALUE,                     TidyAccess, formatAccessReport },
    { OBJECT_MISSING_ALT,                            TidyAccess, formatAccessReport },
    { POTENTIAL_HEADER_BOLD,                         TidyAccess, formatAccessReport },
    { POTENTIAL_HEADER_ITALICS,                      TidyAccess, formatAccessReport },
    { POTENTIAL_HEADER_UNDERLINE,                    TidyAccess, formatAccessReport },
    { PROGRAMMATIC_OBJECTS_REQUIRE_TESTING_APPLET,   TidyAccess, formatAccessReport },
    { PROGRAMMATIC_OBJECTS_REQUIRE_TESTING_EMBED,    TidyAccess, formatAccessReport },
    { PROGRAMMATIC_OBJECTS_REQUIRE_TESTING_OBJECT,   TidyAccess, formatAccessReport },
    { PROGRAMMATIC_OBJECTS_REQUIRE_TESTING_SCRIPT,   TidyAccess, formatAccessReport },
    { REMOVE_AUTO_REDIRECT,                          TidyAccess, formatAccessReport },
    { REMOVE_AUTO_REFRESH,                           TidyAccess, formatAccessReport },
    { REMOVE_BLINK_MARQUEE,                          TidyAccess, formatAccessReport },
    { REMOVE_FLICKER_ANIMATED_GIF,                   TidyAccess, formatAccessReport },
    { REMOVE_FLICKER_APPLET,                         TidyAccess, formatAccessReport },
    { REMOVE_FLICKER_EMBED,                          TidyAccess, formatAccessReport },
    { REMOVE_FLICKER_OBJECT,                         TidyAccess, formatAccessReport },
    { REMOVE_FLICKER_SCRIPT,                         TidyAccess, formatAccessReport },
    { REPLACE_DEPRECATED_HTML_APPLET,                TidyAccess, formatAccessReport },
    { REPLACE_DEPRECATED_HTML_BASEFONT,              TidyAccess, formatAccessReport },
    { REPLACE_DEPRECATED_HTML_CENTER,                TidyAccess, formatAccessReport },
    { REPLACE_DEPRECATED_HTML_DIR,                   TidyAccess, formatAccessReport },
    { REPLACE_DEPRECATED_HTML_FONT,                  TidyAccess, formatAccessReport },
    { REPLACE_DEPRECATED_HTML_ISINDEX,               TidyAccess, formatAccessReport },
    { REPLACE_DEPRECATED_HTML_MENU,                  TidyAccess, formatAccessReport },
    { REPLACE_DEPRECATED_HTML_S,                     TidyAccess, formatAccessReport },
    { REPLACE_DEPRECATED_HTML_STRIKE,                TidyAccess, formatAccessReport },
    { REPLACE_DEPRECATED_HTML_U,                     TidyAccess, formatAccessReport },
    { SCRIPT_MISSING_NOSCRIPT,                       TidyAccess, formatAccessReport },
    { SCRIPT_NOT_KEYBOARD_ACCESSIBLE_ON_CLICK,       TidyAccess, formatAccessReport },
    { SCRIPT_NOT_KEYBOARD_ACCESSIBLE_ON_MOUSE_DOWN,  TidyAccess, formatAccessReport },
    { SCRIPT_NOT_KEYBOARD_ACCESSIBLE_ON_MOUSE_MOVE,  TidyAccess, formatAccessReport },
    { SCRIPT_NOT_KEYBOARD_ACCESSIBLE_ON_MOUSE_OUT,   TidyAccess, formatAccessReport },
    { SCRIPT_NOT_KEYBOARD_ACCESSIBLE_ON_MOUSE_OVER,  TidyAccess, formatAccessReport },
    { SCRIPT_NOT_KEYBOARD_ACCESSIBLE_ON_MOUSE_UP,    TidyAccess, formatAccessReport },
    { SKIPOVER_ASCII_ART,                            TidyAccess, formatAccessReport },
    { STYLE_SHEET_CONTROL_PRESENTATION,              TidyAccess, formatAccessReport },
    { STYLESHEETS_REQUIRE_TESTING_LINK,              TidyAccess, formatAccessReport },
    { STYLESHEETS_REQUIRE_TESTING_STYLE_ATTR,        TidyAccess, formatAccessReport },
    { STYLESHEETS_REQUIRE_TESTING_STYLE_ELEMENT,     TidyAccess, formatAccessReport },
    { TABLE_MAY_REQUIRE_HEADER_ABBR,                 TidyAccess, formatAccessReport },
    { TABLE_MAY_REQUIRE_HEADER_ABBR_NULL,            TidyAccess, formatAccessReport },
    { TABLE_MAY_REQUIRE_HEADER_ABBR_SPACES,          TidyAccess, formatAccessReport },
    { TABLE_MISSING_CAPTION,                         TidyAccess, formatAccessReport },
    { TABLE_MISSING_SUMMARY,                         TidyAccess, formatAccessReport },
    { TABLE_SUMMARY_INVALID_NULL,                    TidyAccess, formatAccessReport },
    { TABLE_SUMMARY_INVALID_PLACEHOLDER,             TidyAccess, formatAccessReport },
    { TABLE_SUMMARY_INVALID_SPACES,                  TidyAccess, formatAccessReport },
    { TEXT_EQUIVALENTS_REQUIRE_UPDATING_APPLET,      TidyAccess, formatAccessReport },
    { TEXT_EQUIVALENTS_REQUIRE_UPDATING_OBJECT,      TidyAccess, formatAccessReport },
    { TEXT_EQUIVALENTS_REQUIRE_UPDATING_SCRIPT,      TidyAccess, formatAccessReport },

    { 0, 0, NULL }
};


/*********************************************************************
 * Message Formatting
 * These individual message formatters populate messages with the
 * correct, pertinent data.
 *********************************************************************/


/* Provides formatting for the Attribute-related reports. This formatter
** should be reserved for messages generated by Tidy's accessibility module,
** even if the signature matches some unrelated report that you wish to
** generate.
*/
TidyMessageImpl *formatAccessReport(TidyDocImpl* doc, Node *element, Node *node, uint code, uint level, va_list args)
{
    doc->badAccess |= BA_WAI;

    /* Currently *all* cases are handled in the default, but maintain this
       structure for possible future cases. */
    switch (code)
    {
        default:
            return TY_(tidyMessageCreateWithNode)(doc, node, code, level );
    }

    return NULL;
}

/* Provides formatting for the Attribute-related reports. This formatter
** provides local variables that are used principally in the formats used for
** the attribute related reports.
*/
TidyMessageImpl *formatAttributeReport(TidyDocImpl* doc, Node *element, Node *node, uint code, uint level, va_list args)
{
    AttVal *av = NULL;
    char const *name = "NULL";
    char const *value = "NULL";
    char tagdesc[64];

    TagToString(node, tagdesc, sizeof(tagdesc));

    if ( ( av = va_arg(args, AttVal*) ) )
    {
        if (av->attribute)
            name = av->attribute;
        if (av->value)
            value = av->value;
    }

    switch (code)
    {
        case MISSING_QUOTEMARK_OPEN:
            return TY_(tidyMessageCreateWithNode)(doc, node, code, level, name );
		
        case BACKSLASH_IN_URI:
        case ESCAPED_ILLEGAL_URI:
        case FIXED_BACKSLASH:
        case ID_NAME_MISMATCH:
        case ILLEGAL_URI_CODEPOINT:
        case ILLEGAL_URI_REFERENCE:
        case INVALID_XML_ID:
        case MISSING_IMAGEMAP:
        case MISSING_QUOTEMARK:
        case NEWLINE_IN_URI:
        case UNEXPECTED_EQUALSIGN:
        case UNEXPECTED_GT:
        case UNEXPECTED_QUOTEMARK:
        case WHITE_IN_URI:
            return TY_(tidyMessageCreateWithNode)(doc, node, code, level, tagdesc );

        case ATTRIBUTE_IS_NOT_ALLOWED:
        case JOINING_ATTRIBUTE:
        case MISSING_ATTR_VALUE:
        case PROPRIETARY_ATTRIBUTE:
            return TY_(tidyMessageCreateWithNode)(doc, node, code, level, tagdesc, name );

        case ATTRIBUTE_VALUE_REPLACED:
        case BAD_ATTRIBUTE_VALUE:
        case BAD_ATTRIBUTE_VALUE_REPLACED:
        case INSERTING_AUTO_ATTRIBUTE:
        case INVALID_ATTRIBUTE:
            return TY_(tidyMessageCreateWithNode)(doc, node, code, level, tagdesc, name, value );

        case MISMATCHED_ATTRIBUTE_ERROR:
        case MISMATCHED_ATTRIBUTE_WARN:
            return TY_(tidyMessageCreateWithNode)(doc, node, code, level, tagdesc, name, HTMLVersion(doc));

        case ANCHOR_NOT_UNIQUE:
        case ANCHOR_DUPLICATED:
        case ATTR_VALUE_NOT_LCASE:
        case PROPRIETARY_ATTR_VALUE:
        case XML_ID_SYNTAX:
            return TY_(tidyMessageCreateWithNode)(doc, node, code, level, tagdesc, value );

        case REPEATED_ATTRIBUTE:
            return TY_(tidyMessageCreateWithNode)(doc, node, code, level, tagdesc, value, name );

        case UNEXPECTED_END_OF_FILE_ATTR:
            /* on end of file adjust reported position to end of input */
            doc->lexer->lines   = doc->docIn->curline;
            doc->lexer->columns = doc->docIn->curcol;
            return TY_(tidyMessageCreateWithLexer)(doc, code, level, tagdesc );
    }

    return NULL;
}


/* Provides report formatting *and* additional status settings for Tidy's
** encoding reports. Provides the local variables typically used for this type
** of report.
** @todo: These status changes probably SHOULD be made in the calling code;
**   however these states are captured to generate future output, which may be
**   useful here in the long run.
*/
TidyMessageImpl *formatEncodingReport(TidyDocImpl* doc, Node *element, Node *node, uint code, uint level, va_list args)
{
    char buf[ 32 ] = {'\0'};
    uint c = va_arg( args, uint );
    Bool discarded = va_arg( args, Bool );
    ctmbstr action = tidyLocalizedString(discarded ? STRING_DISCARDING : STRING_REPLACING);

    switch (code)
    {
        case INVALID_NCR:
            NtoS(c, buf);
            doc->badChars |= BC_INVALID_NCR;
            break;

        case INVALID_SGML_CHARS:
            NtoS(c, buf);
            doc->badChars |= BC_INVALID_SGML_CHARS;
            break;

        case INVALID_UTF8:
            TY_(tmbsnprintf)(buf, sizeof(buf), "U+%04X", c);
            doc->badChars |= BC_INVALID_UTF8;
            break;

        case INVALID_UTF16:
            TY_(tmbsnprintf)(buf, sizeof(buf), "U+%04X", c);
            doc->badChars |= BC_INVALID_UTF16;
            break;

        case VENDOR_SPECIFIC_CHARS:
            NtoS(c, buf);
            doc->badChars |= BC_VENDOR_SPECIFIC_CHARS;
            break;

        case ENCODING_MISMATCH:
            doc->badChars |= BC_ENCODING_MISMATCH;
            return TY_(tidyMessageCreateWithLexer)(doc,
                                                   code,
                                                   level,
                                                   TY_(CharEncodingName)(doc->docIn->encoding),
                                                   TY_(CharEncodingName)(c));
            break;
    }

    return TY_(tidyMessageCreateWithLexer)(doc, code, level, action, buf );

}


/* Provides general formatting for the majority of Tidy's reports. Because most
** reports use the same basic data derived from the element and node, this
** formatter covers the vast majority of Tidy's report messages. Note that this
** formatter guarantees the values of TidyReportLevel in the dispatchTable[].
** Some of the cases in this formatter start new contexts from the va_list
** when the required data is simple. For complex local variable needs, it may
** be preferred to write a new formatter.
*/
TidyMessageImpl *formatStandard(TidyDocImpl* doc, Node *element, Node *node, uint code, uint level, va_list args)
{
    char nodedesc[ 256 ] = {0};
    char elemdesc[ 256 ] = {0};
    Node* rpt = ( element ? element : node );

    TagToString(node, nodedesc, sizeof(nodedesc));

    if ( element )
        TagToString(element, elemdesc, sizeof(elemdesc));

    switch ( code )
    {
        case CUSTOM_TAG_DETECTED:
        {
            ctmbstr tagtype;
            switch ( cfg( doc, TidyUseCustomTags ) )
            {
                case TidyCustomBlocklevel:
                    tagtype = tidyLocalizedString( TIDYCUSTOMBLOCKLEVEL_STRING );
                    break;
                case TidyCustomEmpty:
                    tagtype = tidyLocalizedString( TIDYCUSTOMEMPTY_STRING );
                    break;
                case TidyCustomInline:
                    tagtype = tidyLocalizedString( TIDYCUSTOMINLINE_STRING );
                    break;
                case TidyCustomPre:
                default:
                    tagtype = tidyLocalizedString( TIDYCUSTOMPRE_STRING );
                    break;
            }
            return TY_(tidyMessageCreateWithNode)(doc, element, code, level, elemdesc, tagtype );
        }

        case STRING_NO_SYSID:
            return TY_(tidyMessageCreate)( doc, code, level );

        case FILE_CANT_OPEN:
        case FILE_CANT_OPEN_CFG:
        case FILE_NOT_FILE:
        case STRING_CONTENT_LOOKS:
        case STRING_DOCTYPE_GIVEN:
        case STRING_MISSING_MALFORMED:
        case STRING_MUTING_TYPE:
        {
            ctmbstr str;
            if ( (str = va_arg( args, ctmbstr)) )
                return TY_(tidyMessageCreate)( doc, code, level, str );

        } break;

        case APOS_UNDEFINED:
        case MISSING_SEMICOLON_NCR:
        case MISSING_SEMICOLON:
        case UNESCAPED_AMPERSAND:
        case UNKNOWN_ENTITY:
        {
            ctmbstr entityname;
            if ( !(entityname = va_arg( args, ctmbstr)) )
            {
                entityname = "NULL";
            }
            return TY_(tidyMessageCreateWithLexer)(doc, code, level, entityname);
        }

        case MISSING_ATTRIBUTE:
        {
            ctmbstr name;
            if ( (name = va_arg( args, ctmbstr)) )
                return TY_(tidyMessageCreateWithNode)(doc, node, code, level, nodedesc, name );
        } break;

        case STRING_UNKNOWN_OPTION:
        case OPTION_REMOVED:
        {
            ctmbstr str;
            if ( (str = va_arg( args, ctmbstr)) )
                return TY_(tidyMessageCreateWithLexer)(doc, code, level, str);
        } break;

        case OPTION_REMOVED_UNAPPLIED:
        case STRING_ARGUMENT_BAD:
        {
            ctmbstr s1 = va_arg( args, ctmbstr );
            ctmbstr s2 = va_arg( args, ctmbstr );
            return TY_(tidyMessageCreateWithLexer)(doc, code, level, s1, s2);
        }

        case OPTION_REMOVED_APPLIED:
        {
            ctmbstr s1 = va_arg( args, ctmbstr );
            ctmbstr s2 = va_arg( args, ctmbstr );
            ctmbstr s3 = va_arg( args, ctmbstr );
            return TY_(tidyMessageCreateWithLexer)(doc, code, level, s1, s2, s3);
        }


        case BAD_SURROGATE_LEAD:
        case BAD_SURROGATE_PAIR:
        case BAD_SURROGATE_TAIL:
        {
            uint c1 = va_arg( args, uint );
            uint c2 = va_arg( args, uint );
            return TY_(tidyMessageCreateWithLexer)(doc, code, level, c1, c2);
        }

        case SPACE_PRECEDING_XMLDECL:
            /* @TODO: Should this be a TidyInfo "silent" fix? */
            return TY_(tidyMessageCreateWithNode)(doc, node, code, level );

        case CANT_BE_NESTED:
        case NOFRAMES_CONTENT:
        case USING_BR_INPLACE_OF:
            /* Can we use `rpt` here? No; `element` has a value in every case. */
            return TY_(tidyMessageCreateWithNode)(doc, node, code, level, nodedesc );

        case ELEMENT_VERS_MISMATCH_ERROR:
        case ELEMENT_VERS_MISMATCH_WARN:
            return TY_(tidyMessageCreateWithNode)(doc, node, code, level, nodedesc, HTMLVersion(doc) );

        case TAG_NOT_ALLOWED_IN:
            /* Can we use `rpt` here? No; `element` has a value in every case. */
            return TY_(tidyMessageCreateWithNode)(doc, node, code, level, nodedesc, element ? element->element : NULL );

        case INSERTING_TAG:
        case MISSING_STARTTAG:
        case TOO_MANY_ELEMENTS:
        case UNEXPECTED_ENDTAG:
        case UNEXPECTED_ENDTAG_ERR:  /* generated by XML docs */
            /* Can we use `rpt` here? No; `element` has a value in every case. */
            return TY_(tidyMessageCreateWithNode)(doc, node, code, level, node->element );

        case UNEXPECTED_ENDTAG_IN:
            /* Can we use `rpt` here? No; `element` has a value in every case. */
            return TY_(tidyMessageCreateWithNode)(doc, node, code, level, node->element, element ? element->element : NULL );

        case BAD_CDATA_CONTENT:
        case CONTENT_AFTER_BODY:
        case DOCTYPE_AFTER_TAGS:
        case DUPLICATE_FRAMESET:
        case MALFORMED_COMMENT:
        case MALFORMED_COMMENT_DROPPING:
        case MALFORMED_COMMENT_EOS:
        case MALFORMED_COMMENT_WARN:
        case MALFORMED_DOCTYPE:
        case MISSING_DOCTYPE:
        case MISSING_TITLE_ELEMENT:
        case NESTED_QUOTATION:
        case SUSPECTED_MISSING_QUOTE:
        case XML_DECLARATION_DETECTED:
        case BLANK_TITLE_ELEMENT:
            return TY_(tidyMessageCreateWithNode)(doc, rpt, code, level );

        case ELEMENT_NOT_EMPTY:
        case FOUND_STYLE_IN_BODY:
        case ILLEGAL_NESTING:
        case MOVED_STYLE_TO_HEAD:
        case TRIM_EMPTY_ELEMENT:
        case UNEXPECTED_END_OF_FILE:
            return TY_(tidyMessageCreateWithNode)(doc, rpt, code, level, elemdesc );

        case OBSOLETE_ELEMENT:
        case REPLACING_ELEMENT:
        case REPLACING_UNEX_ELEMENT:
            return TY_(tidyMessageCreateWithNode)(doc, rpt, code, level, elemdesc, nodedesc );

        case ADDED_MISSING_CHARSET:
        case BAD_SUMMARY_HTML5:
        case NESTED_EMPHASIS:
        case PROPRIETARY_ELEMENT:
        case REMOVED_HTML5:
        case UNKNOWN_ELEMENT:
        case UNKNOWN_ELEMENT_LOOKS_CUSTOM:
            return TY_(tidyMessageCreateWithNode)(doc, rpt, code, level, nodedesc );

        case MISSING_ENDTAG_FOR:
        case MISSING_ENDTAG_OPTIONAL:
        case PREVIOUS_LOCATION:
            return TY_(tidyMessageCreateWithNode)(doc, rpt, code, level, element? element->element : NULL );

        case MISSING_ENDTAG_BEFORE:
            return TY_(tidyMessageCreateWithNode)(doc, rpt, code, level, element? element->element : NULL, nodedesc );

        case COERCE_TO_ENDTAG:
        case NON_MATCHING_ENDTAG:
            return TY_(tidyMessageCreateWithNode)(doc, rpt, code, level, node->element, node->element );
        case TOO_MANY_ELEMENTS_IN:
            return TY_(tidyMessageCreateWithNode)(doc, rpt, code, level, node->element, element ? element->element : NULL);

    }

    return NULL;
}


/* Provides general formatting as formatStandard, except TidyReportLevel is set
** dynamically for these items as it cannot be predicted except at runtime.
 */
TidyMessageImpl *formatStandardDynamic(TidyDocImpl* doc, Node *element, Node *node, uint code, uint level, va_list args)
{
    char nodedesc[ 256 ] = {0};

    TagToString(node, nodedesc, sizeof(nodedesc));

    switch (code)
    {
        case DISCARDING_UNEXPECTED:
            /* Force error if in a bad form, or Issue #166 - repeated <main> element. */
            /* Can we use `rpt` here? No; `element` has a value in every case. */
            return TY_(tidyMessageCreateWithNode)(doc, node, code, doc->badForm ? TidyError : TidyWarning, nodedesc );
            break;
    }

    return NULL;
}


/*********************************************************************
 * High Level Message Writing Functions
 * When adding new reports to LibTidy, preference should be given
 * to one of the existing, general purpose message writing functions
 * above, if possible, otherwise try to use one of these, or as a
 * last resort add a new one in this section.
 *********************************************************************/


/* This function performs the heavy lifting for TY_(Report)(). Critically we
** can accept the va_list needed for recursion.
*/
static void vReport(TidyDocImpl* doc, Node *element, Node *node, uint code, va_list args)
{
    int i = 0;
    va_list args_copy;

    while ( dispatchTable[i].code != 0 )
    {
        if ( dispatchTable[i].code == code )
        {
            TidyMessageImpl *message;
            messageFormatter *handler = dispatchTable[i].handler;
            TidyReportLevel level = dispatchTable[i].level;

            va_copy(args_copy, args);
            message = handler( doc, element, node, code, level, args_copy );
            va_end(args_copy);

            messageOut( message );

            if ( dispatchTable[i].next )
            {
                va_copy(args_copy, args);
                vReport(doc, element, node, dispatchTable[i].next, args_copy);
                va_end(args_copy);
            }
            break;
        }
        i++;
    }
}


/* This single Report output function uses the correct formatter with all
** possible, relevant data that can be reported. The only real drawbacks are
** having to pass NULL when some of the values aren't used, and the lack of
** type safety by using the variable arguments. To counter this some convenience
** report output functions exist, too. Any new reports you wish to create must
** be able to use this function signature, although convenience functions should
** be added to abstract the full function signature and to preserve type safety.
*/
void TY_(Report)(TidyDocImpl* doc, Node *element, Node *node, uint code, ...)
{
    va_list args;
    va_start(args, code);
    vReport(doc, element, node, code, args);
    va_end(args);
}


/*********************************************************************
 * Convenience Reporting Functions
 * Functions that don't require the full signature of TY_(Report),
 * and help protect type safety by avoiding variable arguments in the
 * rest of Tidy's code.
 *********************************************************************/

void TY_(ReportAccessError)( TidyDocImpl* doc, Node* node, uint code )
{
    TY_(Report)( doc, NULL, node, code );
}


void TY_(ReportAttrError)(TidyDocImpl* doc, Node *node, AttVal *av, uint code)
{
    TY_(Report)( doc, NULL, node, code, av );
}


void TY_(ReportBadArgument)( TidyDocImpl* doc, ctmbstr option )
{
    assert( option != NULL );
    TY_(Report)( doc, NULL, NULL, STRING_MISSING_MALFORMED, option );
}


void TY_(ReportEntityError)( TidyDocImpl* doc, uint code, ctmbstr entity, int ARG_UNUSED(c) )
{
    /* Note that the report formatter currently doesn't use argument c */
    TY_(Report)( doc, NULL, NULL, code, entity, c );
}


void TY_(ReportFileError)( TidyDocImpl* doc, ctmbstr file, uint code )
{
    TY_(Report)( doc, NULL, NULL, code, file );
}


void TY_(ReportEncodingError)(TidyDocImpl* doc, uint code, uint c, Bool discarded)
{
    TY_(Report)( doc, NULL, NULL, code, c, discarded );
}

void TY_(ReportEncodingWarning)(TidyDocImpl* doc, uint code, uint encoding)
{
    /* va_list in formatter expects trailing `no` argument */
    TY_(Report)( doc, NULL, NULL, code, encoding, no );
}


void TY_(ReportMissingAttr)( TidyDocImpl* doc, Node* node, ctmbstr name )
{
    TY_(Report)( doc, NULL, node, MISSING_ATTRIBUTE, name );
}


void TY_(ReportSurrogateError)(TidyDocImpl* doc, uint code, uint c1, uint c2)
{
    TY_(Report)( doc, NULL, NULL, code, c1,c2 );
}


void TY_(ReportUnknownOption)( TidyDocImpl* doc, ctmbstr option )
{
    /* lexer is not defined when this is called */
    TY_(Report)( doc, NULL, NULL, STRING_UNKNOWN_OPTION, option );
}


/*********************************************************************
 * Dialogue Output Functions
 * As for issuing reports, Tidy manages all dialogue output from a
 * single source in order to manage message categories in a simple
 * an consistent manner.
 *********************************************************************/


/* This structure ties together for each dialogue Code the default
** TidyReportLevel. This it makes it simple to output new dialogue
** messages, or to change report level by modifying this array.
*/
static struct _dialogueDispatchTable {
    uint code;                 /**< The message code. */
    TidyReportLevel level;     /**< The default TidyReportLevel of the message. */
} dialogueDispatchTable[] = {
    { STRING_HELLO_ACCESS,         TidyDialogueInfo     }, /* AccessibilityChecks() */
    { TEXT_GENERAL_INFO,           TidyDialogueInfo     }, /* tidyGeneralInfo() */
    { TEXT_GENERAL_INFO_PLEA,      TidyDialogueInfo     }, /* tidyGeneralInfo() */

    { STRING_NEEDS_INTERVENTION,   TidyDialogueSummary  }, /* tidyDocRunDiagnostics() */
    { STRING_ERROR_COUNT,          TidyDialogueSummary  }, /* ReportNumWarnings() */
    { STRING_NO_ERRORS,            TidyDialogueSummary  }, /* ReportNumWarnings() */
    { STRING_NOT_ALL_SHOWN,        TidyDialogueSummary  }, /* ReportNumWarnings() */

    { FOOTNOTE_TRIM_EMPTY_ELEMENT, TidyDialogueFootnote },
    { TEXT_ACCESS_ADVICE1,         TidyDialogueFootnote }, /* errorSummary() */
    { TEXT_ACCESS_ADVICE2,         TidyDialogueFootnote },
    { TEXT_BAD_FORM,               TidyDialogueFootnote },
    { TEXT_BAD_MAIN,               TidyDialogueFootnote },
    { TEXT_HTML_T_ALGORITHM,       TidyDialogueFootnote },
    { TEXT_INVALID_URI,            TidyDialogueFootnote },
    { TEXT_INVALID_UTF8,           TidyDialogueFootnote },
    { TEXT_INVALID_UTF16,          TidyDialogueFootnote },
    { TEXT_M_IMAGE_ALT,            TidyDialogueFootnote },
    { TEXT_M_IMAGE_MAP,            TidyDialogueFootnote },
    { TEXT_M_LINK_ALT,             TidyDialogueFootnote },
    { TEXT_M_SUMMARY,              TidyDialogueFootnote },
    { TEXT_SGML_CHARS,             TidyDialogueFootnote },
    { TEXT_USING_BODY,             TidyDialogueFootnote },
    { TEXT_USING_FONT,             TidyDialogueFootnote },
    { TEXT_USING_FRAMES,           TidyDialogueFootnote },
    { TEXT_USING_LAYER,            TidyDialogueFootnote },
    { TEXT_USING_NOBR,             TidyDialogueFootnote },
    { TEXT_USING_SPACER,           TidyDialogueFootnote },
    { TEXT_VENDOR_CHARS,           TidyDialogueFootnote },

    { 0, 0 }
};


/* This message formatter for dialogue messages should be capable of formatting
** every message, because they're not all that complex and there aren't that
** many.
*/
static TidyMessageImpl *formatDialogue( TidyDocImpl* doc, uint code, TidyReportLevel level, va_list args )
{
    switch (code)
    {
        case TEXT_SGML_CHARS:
        case TEXT_VENDOR_CHARS:
        {
            ctmbstr str = va_arg(args, ctmbstr);
            return TY_(tidyMessageCreate)( doc, code, level, str );
        }

        case STRING_ERROR_COUNT:
        case STRING_NOT_ALL_SHOWN:
            return TY_(tidyMessageCreate)( doc, code, level,
                                           doc->warnings, "STRING_ERROR_COUNT_WARNING",
                                           doc->errors, "STRING_ERROR_COUNT_ERROR" );

        case FOOTNOTE_TRIM_EMPTY_ELEMENT:
        case STRING_HELLO_ACCESS:
        case STRING_NEEDS_INTERVENTION:
        case STRING_NO_ERRORS:
        case TEXT_ACCESS_ADVICE1:
        case TEXT_ACCESS_ADVICE2:
        case TEXT_BAD_FORM:
        case TEXT_BAD_MAIN:
        case TEXT_GENERAL_INFO:
        case TEXT_GENERAL_INFO_PLEA:
        case TEXT_HTML_T_ALGORITHM:
        case TEXT_INVALID_URI:
        case TEXT_INVALID_UTF8:
        case TEXT_INVALID_UTF16:
        case TEXT_M_IMAGE_ALT:
        case TEXT_M_IMAGE_MAP:
        case TEXT_M_LINK_ALT:
        case TEXT_M_SUMMARY:
        case TEXT_USING_BODY:
        case TEXT_USING_FONT:
        case TEXT_USING_FRAMES:
        case TEXT_USING_LAYER:
        case TEXT_USING_NOBR:
        case TEXT_USING_SPACER:
        default:
            return TY_(tidyMessageCreate)( doc, code, level );
    }

    return NULL;
}


/* This single Dialogue output function determines the correct message level
** and formats the message with the appropriate formatter, and then outputs it.
** This one dialogue function should be sufficient for every use case.
*/
void TY_(Dialogue)(TidyDocImpl* doc, uint code, ...)
{
    int i = 0;
    va_list args;

    while ( dialogueDispatchTable[i].code != 0 )
    {
        if ( dialogueDispatchTable[i].code == code )
        {
            TidyMessageImpl *message;
            TidyReportLevel level = dialogueDispatchTable[i].level;
            va_start(args, code);
            message = formatDialogue( doc, code, level, args );
            va_end(args);
            messageOut( message );
            break;
        }
        i++;
    }
}


/*********************************************************************
 * Output Dialogue Information
 * In addition to reports that are added to the table, Tidy emits
 * various dialogue type information. Most of these are specific to
 * exact circumstances, although `TY_(Dialogue)` should be used
 * instead of adding a new function, if possible.
 *********************************************************************/


/* Outputs the footnotes and other dialogue information after document cleanup
** is complete. LibTidy users might consider capturing these individually in
** the message callback rather than capturing this entire buffer.
** Called by tidyErrorSummary(), in console.
** @todo: This name is a bit misleading and should probably be renamed to
** indicate its focus on printing footnotes.
*/
void TY_(ErrorSummary)( TidyDocImpl* doc )
{
    ctmbstr encnam = tidyLocalizedString(STRING_SPECIFIED);
    int charenc = cfg( doc, TidyCharEncoding );
    if ( charenc == WIN1252 )
        encnam = "Windows-1252";
    else if ( charenc == MACROMAN )
        encnam = "MacRoman";
    else if ( charenc == IBM858 )
        encnam = "ibm858";
    else if ( charenc == LATIN0 )
        encnam = "latin0";

    /* adjust badAccess to that it is 0 if frames are ok */
    if ( doc->badAccess & (BA_USING_FRAMES | BA_USING_NOFRAMES) )
    {
        if (!((doc->badAccess & BA_USING_FRAMES) && !(doc->badAccess & BA_USING_NOFRAMES)))
        {
            doc->badAccess &= ~(BA_USING_FRAMES | BA_USING_NOFRAMES);
        }
    }

    if (doc->badChars)
    {
        if (doc->badChars & BC_VENDOR_SPECIFIC_CHARS)
            TY_(Dialogue)( doc, TEXT_VENDOR_CHARS, encnam );

        if ((doc->badChars & BC_INVALID_SGML_CHARS) || (doc->badChars & BC_INVALID_NCR))
            TY_(Dialogue)( doc, TEXT_SGML_CHARS, encnam );

        if (doc->badChars & BC_INVALID_UTF8)
            TY_(Dialogue)( doc, TEXT_INVALID_UTF8 );

        if (doc->badChars & BC_INVALID_UTF16)
            TY_(Dialogue)( doc, TEXT_INVALID_UTF16 );

        if (doc->badChars & BC_INVALID_URI)
            TY_(Dialogue)( doc, TEXT_INVALID_URI );
    }

    if (doc->badForm)
    {
        if (doc->badForm & flg_BadForm) /* Issue #166 - changed to BIT flag to support other errors */
            TY_(Dialogue)( doc, TEXT_BAD_FORM );

        if (doc->badForm & flg_BadMain) /* Issue #166 - repeated <main> element */
            TY_(Dialogue)( doc, TEXT_BAD_MAIN );
    }

    if (doc->badAccess)
    {
        /* Tidy "classic" accessibility tests */
        if ( cfg(doc, TidyAccessibilityCheckLevel) == 0 )
        {
            if (doc->badAccess & BA_MISSING_SUMMARY)
                TY_(Dialogue)( doc, TEXT_M_SUMMARY );

            if (doc->badAccess & BA_MISSING_IMAGE_ALT)
                TY_(Dialogue)( doc, TEXT_M_IMAGE_ALT );

            if (doc->badAccess & BA_MISSING_IMAGE_MAP)
                TY_(Dialogue)( doc, TEXT_M_IMAGE_MAP );

            if (doc->badAccess & BA_MISSING_LINK_ALT)
                TY_(Dialogue)( doc, TEXT_M_LINK_ALT );

            if ((doc->badAccess & BA_USING_FRAMES) && !(doc->badAccess & BA_USING_NOFRAMES))
                TY_(Dialogue)( doc, TEXT_USING_FRAMES );
        }

        if ( cfg(doc, TidyAccessibilityCheckLevel) > 0 )
            TY_(Dialogue)( doc, TEXT_ACCESS_ADVICE2 );
        else
            TY_(Dialogue)( doc, TEXT_ACCESS_ADVICE1 );
    }

    if (doc->badLayout)
    {
        if (doc->badLayout & USING_LAYER)
            TY_(Dialogue)( doc, TEXT_USING_LAYER );

        if (doc->badLayout & USING_SPACER)
            TY_(Dialogue)( doc, TEXT_USING_SPACER );

        if (doc->badLayout & USING_FONT)
            TY_(Dialogue)( doc, TEXT_USING_FONT );

        if (doc->badLayout & USING_NOBR)
            TY_(Dialogue)( doc, TEXT_USING_NOBR );

        if (doc->badLayout & USING_BODY)
            TY_(Dialogue)( doc, TEXT_USING_BODY );
    }

    if (doc->footnotes)
    {
        if (doc->footnotes & FN_TRIM_EMPTY_ELEMENT)
            TY_(Dialogue)( doc, FOOTNOTE_TRIM_EMPTY_ELEMENT );
    }
}


/* Outputs document HTML version and version-related information.
** Called by tidyRunDiagnostics(), from console.
** Called by tidyDocReportDoctype(), currently unused.
*/
void TY_(ReportMarkupVersion)( TidyDocImpl* doc )
{
    if ( doc->givenDoctype )
        TY_(Report)( doc, NULL, NULL, STRING_DOCTYPE_GIVEN, doc->givenDoctype );

    if ( ! cfgBool(doc, TidyXmlTags) )
    {
        Bool isXhtml = doc->lexer->isvoyager;
        uint apparentVers = TY_(ApparentVersion)( doc );
        ctmbstr vers = TY_(HTMLVersionNameFromCode)( apparentVers, isXhtml );

        if ( !vers )
            vers = tidyLocalizedString(STRING_HTML_PROPRIETARY);

        TY_(Report)( doc, NULL, NULL, STRING_CONTENT_LOOKS, vers );

        /* Warn about missing system identifier (SI) in emitted doctype */
        if ( TY_(WarnMissingSIInEmittedDocType)( doc ) )
            TY_(Report)( doc, NULL, NULL, STRING_NO_SYSID );
    }
}


/* Reports the number of warnings and errors found in the document.
** Called by tidyRunDiagnostics(), from console.
*/
void TY_(ReportNumWarnings)( TidyDocImpl* doc )
{
    if ( doc->warnings > 0 || doc->errors > 0 )
    {
        if ( doc->errors > cfg(doc, TidyShowErrors) || !cfgBool(doc, TidyShowWarnings) )
        {
            TY_(Dialogue)( doc, STRING_NOT_ALL_SHOWN );
        }
        else
        {
            TY_(Dialogue)( doc, STRING_ERROR_COUNT );
        }

    }
    else
    {
        TY_(Dialogue)( doc, STRING_NO_ERRORS );
    }
}


/*********************************************************************
 * Message Muting
 *********************************************************************/


void TY_(FreeMutedMessageList)( TidyDocImpl* doc )
{
    TidyMutedMessages *list = &(doc->muted);

    if ( list->list )
        TidyFree( doc->allocator, list->list );
}


void TY_(DefineMutedMessage)(TidyDocImpl* doc, const TidyOptionImpl* opt, ctmbstr name)
{
    enum { capacity = 10 };
    TidyMutedMessages *list = &(doc->muted);
    tidyStrings message = TY_(tidyErrorCodeFromKey)( name );

    if ( message <= REPORT_MESSAGE_FIRST || message >= REPORT_MESSAGE_LAST)
    {
        TY_(Report)( doc, NULL, NULL, STRING_ARGUMENT_BAD, opt->name, name );
        return;
    }

    if ( !list->list )
    {
        list->list = TidyAlloc(doc->allocator, sizeof(tidyStrings) * capacity );
        list->list[0] = 0;
        list->capacity = capacity;
        list->count = 0;
    }

    if ( list->count >= list->capacity )
    {
        list->capacity = list->capacity * 2;
        list->list = TidyRealloc(doc->allocator, list->list, sizeof(tidyStrings) * list->capacity + 1 );
    }

    list->list[list->count] = message;
    list->count++;
    list->list[list->count] = 0;

    /* Must come *after* adding to the list, in case it's muted, too. */
    TY_(Report)( doc, NULL, NULL, STRING_MUTING_TYPE, name );
}


TidyIterator TY_(getMutedMessageList)( TidyDocImpl* doc )
{
    TidyMutedMessages *list = &(doc->muted);
    size_t result = list->count > 0 ? 1 : 0;

    return (TidyIterator) result;
}


ctmbstr TY_(getNextMutedMessage)( TidyDocImpl* doc, TidyIterator* iter )
{
    TidyMutedMessages *list = &(doc->muted);
    size_t index;
    ctmbstr result = NULL;
    assert( iter != NULL );
    index = (size_t)*iter;

    if ( index > 0 && index <= list->count )
    {
        result = TY_(tidyErrorCodeAsKey)(list->list[index-1]);
        index++;
    }
    *iter = (TidyIterator) ( index <= list->count ? index : (size_t)0 );

    return result;
}


/*********************************************************************
 * Key Discovery
 *********************************************************************/


/*********************************************************************
 * LibTidy users may want to to enable their own localization lookup
 * lookup methods. Because Tidy's errors codes are enums, the actual
 * values can change over time. This table will the LibTidy users
 * always have a static value available for use.
 *
 * For macro documentation, refer to the comments in `tidyenum.h`.
 *********************************************************************/

typedef struct tidyStringsKeyItem {
    ctmbstr key;
    int value;
} tidyStringsKeyItem;

static const tidyStringsKeyItem tidyStringsKeys[] = {
    FOREACH_TIDYCONFIGCATEGORY(MAKE_STRUCT)
    FOREACH_MSG_MISC(MAKE_STRUCT)
    FOREACH_FOOTNOTE_MSG(MAKE_STRUCT)
    FOREACH_DIALOG_MSG(MAKE_STRUCT)
    FOREACH_REPORT_MSG(MAKE_STRUCT)
    FOREACH_ACCESS_MSG(MAKE_STRUCT)
#if SUPPORT_CONSOLE_APP
    FOREACH_MSG_CONSOLE(MAKE_STRUCT)
#endif
    { "TIDYSTRINGS_FIRST",                        TIDYSTRINGS_FIRST },
    { "TIDYSTRINGS_LAST",                         TIDYSTRINGS_LAST  },
    { NULL,                                       0                 },
};


/**
 *  Given an error code, return the string associated with it.
 */
ctmbstr TY_(tidyErrorCodeAsKey)(uint code)
{
    uint i = 0;
    while (tidyStringsKeys[i].key) {
        if ( tidyStringsKeys[i].value == code )
            return tidyStringsKeys[i].key;
        i++;
    }
    return "UNDEFINED";
}


/**
 *  Given an error code string, return its uint.
 */
uint TY_(tidyErrorCodeFromKey)(ctmbstr code)
{
    uint i = 0;
    while (tidyStringsKeys[i].key) {
        if ( strcmp(tidyStringsKeys[i].key, code) == 0 )
            return tidyStringsKeys[i].value;
        i++;
    }
    return UINT_MAX;
}


/**
 *  Determines the number of error codes used by Tidy.
 */
static const uint tidyErrorCodeListSize()
{
    static uint array_size = 0;

    if ( array_size == 0 )
    {
        while ( tidyStringsKeys[array_size].key ) {
            array_size++;
        }
    }

    return array_size;
}

/**
 *  Initializes the TidyIterator to point to the first item
 *  in Tidy's list of error codes. Individual items must be
 *  retrieved with getNextErrorCode();
 */
TidyIterator TY_(getErrorCodeList)()
{
    return (TidyIterator)(size_t)1;
}

/**
 *  Returns the next error code.
 */
uint TY_(getNextErrorCode)( TidyIterator* iter )
{
    const tidyStringsKeyItem *item = NULL;
    size_t itemIndex;
    assert( iter != NULL );

    itemIndex = (size_t)*iter;

    if ( itemIndex > 0 && itemIndex <= tidyErrorCodeListSize() )
    {
        item = &tidyStringsKeys[itemIndex - 1];
        itemIndex++;
    }

    *iter = (TidyIterator)( itemIndex <= tidyErrorCodeListSize() ? itemIndex : (size_t)0 );
    return item ? item->value : 0;
}


/*********************************************************************
 * Documentation of configuration options
 *
 * Although most of the strings now come from the language module,
 * generating the documentation by the console application requires a
 * series of cross-references that are generated in this messaging
 * module.
 *********************************************************************/


#if SUPPORT_CONSOLE_APP
/* Cross-references definitions.
 * Note that each list must be terminated with `TidyUnknownOption`.
 */
static const TidyOptionId TidyAsciiCharsLinks[] =      { TidyMakeClean, TidyUnknownOption };
static const TidyOptionId TidyBlockTagsLinks[] =       { TidyEmptyTags, TidyInlineTags, TidyPreTags, TidyUseCustomTags, TidyUnknownOption };
static const TidyOptionId TidyCharEncodingLinks[] =    { TidyInCharEncoding, TidyOutCharEncoding, TidyUnknownOption };
static const TidyOptionId TidyDuplicateAttrsLinks[] =  { TidyJoinClasses, TidyJoinStyles, TidyUnknownOption };
static const TidyOptionId TidyEmacsLinks[] =           { TidyShowFilename, TidyUnknownOption };
static const TidyOptionId TidyEmptyTagsLinks[] =       { TidyBlockTags, TidyInlineTags, TidyPreTags, TidyUseCustomTags, TidyUnknownOption };
static const TidyOptionId TidyErrFileLinks[] =         { TidyOutFile, TidyUnknownOption };
static const TidyOptionId TidyInCharEncodingLinks[] =  { TidyCharEncoding, TidyUnknownOption };
static const TidyOptionId TidyIndentContentLinks[] =   { TidyIndentSpaces, TidyUnknownOption };
static const TidyOptionId TidyIndentSpacesLinks[] =    { TidyIndentContent, TidyUnknownOption };
static const TidyOptionId TidyInlineTagsLinks[] =      { TidyBlockTags, TidyEmptyTags, TidyPreTags, TidyUseCustomTags, TidyUnknownOption };
static const TidyOptionId TidyMergeDivsLinks[] =       { TidyMakeClean, TidyMergeSpans, TidyUnknownOption };
static const TidyOptionId TidyMergeSpansLinks[] =      { TidyMakeClean, TidyMergeDivs, TidyUnknownOption };
static const TidyOptionId TidyMuteLinks[] =            { TidyMuteShow, TidyUnknownOption };
static const TidyOptionId TidyMuteShowLinks[] =        { TidyMuteReports, TidyUnknownOption };
static const TidyOptionId TidyNumEntitiesLinks[] =     { TidyDoctype, TidyPreserveEntities, TidyUnknownOption };
static const TidyOptionId TidyOutCharEncodingLinks[] = { TidyCharEncoding, TidyUnknownOption };
static const TidyOptionId TidyOutFileLinks[] =         { TidyErrFile, TidyUnknownOption };
static const TidyOptionId TidyPreTagsLinks[] =         { TidyBlockTags, TidyEmptyTags, TidyInlineTags, TidyUseCustomTags, TidyUnknownOption };
static const TidyOptionId TidyShowFilenameLinks[] =    { TidyEmacs, TidyUnknownOption };
static const TidyOptionId TidySortAttributesLinks[] =  { TidyPriorityAttributes, TidyUnknownOption };
static const TidyOptionId TidyUseCustomTagsLinks[] =   { TidyBlockTags, TidyEmptyTags, TidyInlineTags, TidyPreTags, TidyUnknownOption };
static const TidyOptionId TidyWrapAttValsLinks[] =     { TidyWrapScriptlets, TidyLiteralAttribs, TidyUnknownOption };
static const TidyOptionId TidyWrapScriptletsLinks[] =  { TidyWrapAttVals, TidyUnknownOption };
static const TidyOptionId TidyXmlDeclLinks[] =         { TidyCharEncoding, TidyOutCharEncoding, TidyUnknownOption };

/* Cross-reference assignments.
 * We can't build a complex array at compile time and we're not counting on
 * any type of initialization, so this two-stage building process is required.
 */
static const TidyOptionDoc docs_xrefs[] =
{
    { TidyAsciiChars,      TidyAsciiCharsLinks      },
    { TidyBlockTags,       TidyBlockTagsLinks       },
    { TidyCharEncoding,    TidyCharEncodingLinks    },
    { TidyDuplicateAttrs,  TidyDuplicateAttrsLinks  },
    { TidyEmacs,           TidyEmacsLinks           },
    { TidyEmptyTags,       TidyEmptyTagsLinks       },
    { TidyErrFile,         TidyErrFileLinks         },
    { TidyInCharEncoding,  TidyInCharEncodingLinks  },
    { TidyIndentContent,   TidyIndentContentLinks   },
    { TidyIndentSpaces,    TidyIndentSpacesLinks    },
    { TidyInlineTags,      TidyInlineTagsLinks      },
    { TidyMergeDivs,       TidyMergeDivsLinks       },
    { TidyMergeSpans,      TidyMergeSpansLinks      },
    { TidyMuteShow,        TidyMuteShowLinks        },
    { TidyNumEntities,     TidyNumEntitiesLinks     },
    { TidyOutCharEncoding, TidyOutCharEncodingLinks },
    { TidyOutFile,         TidyOutFileLinks         },
    { TidyPreTags,         TidyPreTagsLinks         },
    { TidyShowFilename,    TidyShowFilenameLinks    },
    { TidySortAttributes,  TidySortAttributesLinks  },
    { TidyMuteReports,     TidyMuteLinks            },
    { TidyUseCustomTags,   TidyUseCustomTagsLinks   },
    { TidyWrapAttVals,     TidyWrapAttValsLinks     },
    { TidyWrapScriptlets,  TidyWrapScriptletsLinks  },
    { TidyXmlDecl,         TidyXmlDeclLinks         },
    { N_TIDY_OPTIONS                                }
};


/* Cross-reference retrieval. */
const TidyOptionDoc* TY_(OptGetDocDesc)( TidyOptionId optId )
{
    uint i = 0;

    while( docs_xrefs[i].opt != N_TIDY_OPTIONS )
    {
        if ( docs_xrefs[i].opt == optId )
            return &docs_xrefs[i];
        ++i;
    }
    return NULL;
}

#endif /* SUPPORT_CONSOLE_APP */


/*
 * local variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 4
 * end:
 */
