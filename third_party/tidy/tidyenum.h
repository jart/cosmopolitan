#ifndef __TIDYENUM_H__
#define __TIDYENUM_H__

/**************************************************************************//**
 * @file
 * Separated public enumerations header providing important indentifiers for
 * LibTidy and internal users, as well as code-generator macros used to
 * generate many of them.
 *
 * The use of enums simplifies enum re-use in various wrappers, e.g. SWIG,
 * generated wrappers, and COM IDL files.
 *
 * This file also contains macros to generate additional enums for use in
 * Tidy's language localizations and/or to access Tidy's strings via the API.
 * See detailed information elsewhere in this file's documentation.
 *
 * @note LibTidy does *not* guarantee the value of any enumeration member,
 * including the starting integer value, except where noted. Always use enum
 * members rather than their values!
 *
 * Enums that have starting values have starting values for a good reason,
 * mainly to prevent string key overlap.
 *
 * @author  Dave Raggett [dsr@w3.org]
 * @author  HTACG, et al (consult git log)
 *
 * @copyright
 *     Copyright (c) 1998-2017 World Wide Web Consortium (Massachusetts
 *     Institute of Technology, European Research Consortium for Informatics
 *     and Mathematics, Keio University).
 * @copyright
 *     See tidy.h for license.
 *
 * @date      Created 2001-05-20 by Charles Reitzel
 * @date      Updated 2002-07-01 by Charles Reitzel
 * @date      Further modifications: consult git log.
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 ** @defgroup public_enum_gen Tidy Strings Generation Macros
 ** @ingroup internal_api
 **
 ** Tidy aims to provide a consistent API for library users, and so we go to
 ** some lengths to provide a `tidyStrings` enum that consists of the message
 ** code for every string that Tidy can emit (used internally), and the array
 ** `tidyStringsKeys[]` containing string representations of each message code.
 **
 ** In order to keep code maintainable and make it simple to add new messages,
 ** the message code enums and `tidyStringsKeys[]` are generated dynamically
 ** with preprocessor macros defined below.
 **
 ** Any visible FOREACH_MSG_* macro (including new ones) must be applied to the
 ** `tidyStrings` enum with the `MAKE_ENUM()` macro in this file, and to the
 ** `tidyStringsKeys[]` (in `messages.c`) with `MAKE_STRUCT` in this file.
 **
 ** Modern IDE's will dynamically pre-process all of these macros, enabling
 ** code-completion of these enums and array of structs.
 **
 ** @{
 ******************************************************************************/

/* MARK: - Code Generation Macros */
/** @name Code Generation Macros
 ** These macros generate the enums and arrays from the Content Generation
 ** Macros defined below.
 ** @{
 */

/** Used to populate the contents of an enumerator, such as `tidyStrings`. */
#define MAKE_ENUM(MESSAGE) MESSAGE,

/** Used to populate the contents of a structure, such as tidyStringsKeys[]. */
#define MAKE_STRUCT(MESSAGE) {#MESSAGE, MESSAGE},


/** @} */
/* MARK: - Content Generation Macros */
/** @name Content Generation Macros
 ** These macros generate the individual entries in the enums and structs used
 ** to manage strings in Tidy.
 ** @{
 */

/** Codes for populating TidyConfigCategory enumeration. */
#define FOREACH_TIDYCONFIGCATEGORY(FN)                                    \
    FN(TidyDiagnostics)      /**< Diagnostics */                          \
    FN(TidyDisplay)          /**< Affecting screen display */             \
    FN(TidyDocumentIO)       /**< Pertaining to document I/O */           \
    FN(TidyEncoding)         /**< Relating to encoding */                 \
    FN(TidyFileIO)           /**< Pertaining to file I/O */               \
    FN(TidyMarkupCleanup)    /**< Cleanup related options */              \
    FN(TidyMarkupEntities)   /**< Entity related options */               \
    FN(TidyMarkupRepair)     /**< Document repair related options */      \
    FN(TidyMarkupTeach)      /**< Teach tidy new things */                \
    FN(TidyMarkupXForm)      /**< Transform HTML one way or another */    \
    FN(TidyPrettyPrint)      /**< Pretty printing options */              \
    FN(TidyInternalCategory) /**< Option is internal only. */             \


/** These message codes comprise every possible message that can be output by
 ** Tidy that are *not* diagnostic style messages, and are *not* console
 ** application specific messages.
 */
#define FOREACH_MSG_MISC(FN) \
/** line %d column %d */                           FN(LINE_COLUMN_STRING)          \
/** %s: line %d column %d */                       FN(FN_LINE_COLUMN_STRING)       \
/** discarding */                                  FN(STRING_DISCARDING)           \
/** error and errors */                            FN(STRING_ERROR_COUNT_ERROR)    \
/** warning and warnings */                        FN(STRING_ERROR_COUNT_WARNING)  \
/** Accessibility hello message */                 FN(STRING_HELLO_ACCESS)         \
/** HTML Proprietary */                            FN(STRING_HTML_PROPRIETARY)     \
/** plain text */                                  FN(STRING_PLAIN_TEXT)           \
/** replacing */                                   FN(STRING_REPLACING)            \
/** specified */                                   FN(STRING_SPECIFIED)            \
/** XML declaration */                             FN(STRING_XML_DECLARATION)      \
/** no */                                          FN(TIDYCUSTOMNO_STRING)         \
/** block level */                                 FN(TIDYCUSTOMBLOCKLEVEL_STRING) \
/** empty */                                       FN(TIDYCUSTOMEMPTY_STRING)      \
/** inline */                                      FN(TIDYCUSTOMINLINE_STRING)     \
/** pre */                                         FN(TIDYCUSTOMPRE_STRING)        \


/** These messages are used to generate additional dialogue style output from
 ** Tidy when certain conditions exist, and provide more verbose explanations
 ** than the short report.
 */
#define FOREACH_FOOTNOTE_MSG(FN) \
    FN(FOOTNOTE_TRIM_EMPTY_ELEMENT) \
    FN(TEXT_ACCESS_ADVICE1)         \
    FN(TEXT_ACCESS_ADVICE2)         \
    FN(TEXT_BAD_FORM)               \
    FN(TEXT_BAD_MAIN)               \
    FN(TEXT_HTML_T_ALGORITHM)       \
    FN(TEXT_INVALID_URI)            \
    FN(TEXT_INVALID_UTF16)          \
    FN(TEXT_INVALID_UTF8)           \
    FN(TEXT_M_IMAGE_ALT)            \
    FN(TEXT_M_IMAGE_MAP)            \
    FN(TEXT_M_LINK_ALT)             \
    FN(TEXT_M_SUMMARY)              \
    FN(TEXT_SGML_CHARS)             \
    FN(TEXT_USING_BODY)             \
    FN(TEXT_USING_FONT)             \
    FN(TEXT_USING_FRAMES)           \
    FN(TEXT_USING_LAYER)            \
    FN(TEXT_USING_NOBR)             \
    FN(TEXT_USING_SPACER)           \
    FN(TEXT_VENDOR_CHARS)           \
    FN(TEXT_WINDOWS_CHARS)


/** These messages are used to generate additional dialogue style output from
 ** Tidy when certain conditions exist, and provide more verbose explanations
 ** than the short report.
 */
#define FOREACH_DIALOG_MSG(FN) \
/* TidyDialogueSummary */  FN(STRING_ERROR_COUNT)          \
/* TidyDialogueSummary */  FN(STRING_NEEDS_INTERVENTION)   \
/* TidyDialogueSummary */  FN(STRING_NO_ERRORS)            \
/* TidyDialogueSummary */  FN(STRING_NOT_ALL_SHOWN)        \
/* TidyDialogueInfo */     FN(TEXT_GENERAL_INFO_PLEA)      \
/* TidyDialogueInfo */     FN(TEXT_GENERAL_INFO)


/** These are report messages, i.e., messages that appear in Tidy's table
 ** of errors and warnings.
 */
#define FOREACH_REPORT_MSG(FN)        \
    FN(ADDED_MISSING_CHARSET)         \
    FN(ANCHOR_NOT_UNIQUE)             \
    FN(ANCHOR_DUPLICATED)             \
    FN(APOS_UNDEFINED)                \
    FN(ATTR_VALUE_NOT_LCASE)          \
    FN(ATTRIBUTE_IS_NOT_ALLOWED)      \
    FN(ATTRIBUTE_VALUE_REPLACED)      \
    FN(BACKSLASH_IN_URI)              \
    FN(BAD_ATTRIBUTE_VALUE_REPLACED)  \
    FN(BAD_ATTRIBUTE_VALUE)           \
    FN(BAD_CDATA_CONTENT)             \
    FN(BAD_SUMMARY_HTML5)             \
    FN(BAD_SURROGATE_LEAD)            \
    FN(BAD_SURROGATE_PAIR)            \
    FN(BAD_SURROGATE_TAIL)            \
    FN(CANT_BE_NESTED)                \
    FN(COERCE_TO_ENDTAG)              \
    FN(CONTENT_AFTER_BODY)            \
    FN(CUSTOM_TAG_DETECTED)           \
    FN(DISCARDING_UNEXPECTED)         \
    FN(DOCTYPE_AFTER_TAGS)            \
    FN(DUPLICATE_FRAMESET)            \
    FN(ELEMENT_NOT_EMPTY)             \
    FN(ELEMENT_VERS_MISMATCH_ERROR)   \
    FN(ELEMENT_VERS_MISMATCH_WARN)    \
    FN(ENCODING_MISMATCH)             \
    FN(ESCAPED_ILLEGAL_URI)           \
    FN(FILE_CANT_OPEN)                \
    FN(FILE_CANT_OPEN_CFG)            \
    FN(FILE_NOT_FILE)                 \
    FN(FIXED_BACKSLASH)               \
    FN(FOUND_STYLE_IN_BODY)           \
    FN(ID_NAME_MISMATCH)              \
    FN(ILLEGAL_NESTING)               \
    FN(ILLEGAL_URI_CODEPOINT)         \
    FN(ILLEGAL_URI_REFERENCE)         \
    FN(INSERTING_AUTO_ATTRIBUTE)      \
    FN(INSERTING_TAG)                 \
    FN(INVALID_ATTRIBUTE)             \
    FN(INVALID_NCR)                   \
    FN(INVALID_SGML_CHARS)            \
    FN(INVALID_UTF8)                  \
    FN(INVALID_UTF16)                 \
    FN(INVALID_XML_ID)                \
    FN(JOINING_ATTRIBUTE)             \
    FN(MALFORMED_COMMENT)             \
    FN(MALFORMED_COMMENT_DROPPING)    \
    FN(MALFORMED_COMMENT_EOS)         \
    FN(MALFORMED_COMMENT_WARN)        \
    FN(MALFORMED_DOCTYPE)             \
    FN(MISMATCHED_ATTRIBUTE_ERROR)    \
    FN(MISMATCHED_ATTRIBUTE_WARN)     \
    FN(MISSING_ATTR_VALUE)            \
    FN(MISSING_ATTRIBUTE)             \
    FN(MISSING_DOCTYPE)               \
    FN(MISSING_ENDTAG_BEFORE)         \
    FN(MISSING_ENDTAG_FOR)            \
    FN(MISSING_ENDTAG_OPTIONAL)       \
    FN(MISSING_IMAGEMAP)              \
    FN(MISSING_QUOTEMARK)             \
    FN(MISSING_QUOTEMARK_OPEN)        \
    FN(MISSING_SEMICOLON_NCR)         \
    FN(MISSING_SEMICOLON)             \
    FN(MISSING_STARTTAG)              \
    FN(MISSING_TITLE_ELEMENT)         \
    FN(MOVED_STYLE_TO_HEAD)           \
    FN(NESTED_EMPHASIS)               \
    FN(NESTED_QUOTATION)              \
    FN(NEWLINE_IN_URI)                \
    FN(NOFRAMES_CONTENT)              \
    FN(NON_MATCHING_ENDTAG)           \
    FN(OBSOLETE_ELEMENT)              \
    FN(OPTION_REMOVED)                \
    FN(OPTION_REMOVED_APPLIED)        \
    FN(OPTION_REMOVED_UNAPPLIED)      \
    FN(PREVIOUS_LOCATION)             \
    FN(PROPRIETARY_ATTR_VALUE)        \
    FN(PROPRIETARY_ATTRIBUTE)         \
    FN(PROPRIETARY_ELEMENT)           \
    FN(REMOVED_HTML5)                 \
    FN(REPEATED_ATTRIBUTE)            \
    FN(REPLACING_ELEMENT)             \
    FN(REPLACING_UNEX_ELEMENT)        \
    FN(SPACE_PRECEDING_XMLDECL)       \
    FN(STRING_CONTENT_LOOKS)        \
    FN(STRING_ARGUMENT_BAD)           \
    FN(STRING_DOCTYPE_GIVEN)        \
    FN(STRING_MISSING_MALFORMED)      \
    FN(STRING_MUTING_TYPE)            \
    FN(STRING_NO_SYSID)             \
    FN(STRING_UNKNOWN_OPTION)         \
    FN(SUSPECTED_MISSING_QUOTE)       \
    FN(TAG_NOT_ALLOWED_IN)            \
    FN(TOO_MANY_ELEMENTS_IN)          \
    FN(TOO_MANY_ELEMENTS)             \
    FN(TRIM_EMPTY_ELEMENT)            \
    FN(UNESCAPED_AMPERSAND)           \
    FN(UNEXPECTED_END_OF_FILE_ATTR)   \
    FN(UNEXPECTED_END_OF_FILE)        \
    FN(UNEXPECTED_ENDTAG_ERR)         \
    FN(UNEXPECTED_ENDTAG_IN)          \
    FN(UNEXPECTED_ENDTAG)             \
    FN(UNEXPECTED_EQUALSIGN)          \
    FN(UNEXPECTED_GT)                 \
    FN(UNEXPECTED_QUOTEMARK)          \
    FN(UNKNOWN_ELEMENT_LOOKS_CUSTOM)  \
    FN(UNKNOWN_ELEMENT)               \
    FN(UNKNOWN_ENTITY)                \
    FN(USING_BR_INPLACE_OF)           \
    FN(VENDOR_SPECIFIC_CHARS)         \
    FN(WHITE_IN_URI)                  \
    FN(XML_DECLARATION_DETECTED)      \
    FN(XML_ID_SYNTAX)                 \
    FN(BLANK_TITLE_ELEMENT)


/** These are report messages added by Tidy's accessibility module.
 ** Note that commented out items don't have checks for them at this time,
 ** and it was probably intended that some test would eventually be written.
 */
#define FOREACH_ACCESS_MSG(FN)                                          \
/** [1.1.1.1] */     FN(IMG_MISSING_ALT)                                \
/** [1.1.1.2] */     FN(IMG_ALT_SUSPICIOUS_FILENAME)                    \
/** [1.1.1.3] */     FN(IMG_ALT_SUSPICIOUS_FILE_SIZE)                   \
/** [1.1.1.4] */     FN(IMG_ALT_SUSPICIOUS_PLACEHOLDER)                 \
/** [1.1.1.10] */    FN(IMG_ALT_SUSPICIOUS_TOO_LONG)                    \
/** [1.1.1.11] */    /* FN(IMG_MISSING_ALT_BULLET) */                   \
/** [1.1.1.12] */    /* FN(IMG_MISSING_ALT_H_RULE) */                   \
/** [1.1.2.1] */     FN(IMG_MISSING_LONGDESC_DLINK)                     \
/** [1.1.2.2] */     FN(IMG_MISSING_DLINK)                              \
/** [1.1.2.3] */     FN(IMG_MISSING_LONGDESC)                           \
/** [1.1.2.5] */     /* FN(LONGDESC_NOT_REQUIRED) */                    \
/** [1.1.3.1] */     FN(IMG_BUTTON_MISSING_ALT)                         \
/** [1.1.4.1] */     FN(APPLET_MISSING_ALT)                             \
/** [1.1.5.1] */     FN(OBJECT_MISSING_ALT)                             \
/** [1.1.6.1] */     FN(AUDIO_MISSING_TEXT_WAV)                         \
/** [1.1.6.2] */     FN(AUDIO_MISSING_TEXT_AU)                          \
/** [1.1.6.3] */     FN(AUDIO_MISSING_TEXT_AIFF)                        \
/** [1.1.6.4] */     FN(AUDIO_MISSING_TEXT_SND)                         \
/** [1.1.6.5] */     FN(AUDIO_MISSING_TEXT_RA)                          \
/** [1.1.6.6] */     FN(AUDIO_MISSING_TEXT_RM)                          \
/** [1.1.8.1] */     FN(FRAME_MISSING_LONGDESC)                         \
/** [1.1.9.1] */     FN(AREA_MISSING_ALT)                               \
/** [1.1.10.1] */    FN(SCRIPT_MISSING_NOSCRIPT)                        \
/** [1.1.12.1] */    FN(ASCII_REQUIRES_DESCRIPTION)                     \
/** [1.2.1.1] */     FN(IMG_MAP_SERVER_REQUIRES_TEXT_LINKS)             \
/** [1.4.1.1] */     FN(MULTIMEDIA_REQUIRES_TEXT)                       \
/** [1.5.1.1] */     FN(IMG_MAP_CLIENT_MISSING_TEXT_LINKS)              \
/** [2.1.1.1] */     FN(INFORMATION_NOT_CONVEYED_IMAGE)                 \
/** [2.1.1.2] */     FN(INFORMATION_NOT_CONVEYED_APPLET)                \
/** [2.1.1.3] */     FN(INFORMATION_NOT_CONVEYED_OBJECT)                \
/** [2.1.1.4] */     FN(INFORMATION_NOT_CONVEYED_SCRIPT)                \
/** [2.1.1.5] */     FN(INFORMATION_NOT_CONVEYED_INPUT)                 \
/** [2.2.1.1] */     FN(COLOR_CONTRAST_TEXT)                            \
/** [2.2.1.2] */     FN(COLOR_CONTRAST_LINK)                            \
/** [2.2.1.3] */     FN(COLOR_CONTRAST_ACTIVE_LINK)                     \
/** [2.2.1.4] */     FN(COLOR_CONTRAST_VISITED_LINK)                    \
/** [3.2.1.1] */     FN(DOCTYPE_MISSING)                                \
/** [3.3.1.1] */     FN(STYLE_SHEET_CONTROL_PRESENTATION)               \
/** [3.5.1.1] */     FN(HEADERS_IMPROPERLY_NESTED)                      \
/** [3.5.2.1] */     FN(POTENTIAL_HEADER_BOLD)                          \
/** [3.5.2.2] */     FN(POTENTIAL_HEADER_ITALICS)                       \
/** [3.5.2.3] */     FN(POTENTIAL_HEADER_UNDERLINE)                     \
/** [3.5.3.1] */     FN(HEADER_USED_FORMAT_TEXT)                        \
/** [3.6.1.1] */     FN(LIST_USAGE_INVALID_UL)                          \
/** [3.6.1.2] */     FN(LIST_USAGE_INVALID_OL)                          \
/** [3.6.1.4] */     FN(LIST_USAGE_INVALID_LI)                          \
/** [4.1.1.1] */     /* FN(INDICATE_CHANGES_IN_LANGUAGE) */             \
/** [4.3.1.1] */     FN(LANGUAGE_NOT_IDENTIFIED)                        \
/** [4.3.1.1] */     FN(LANGUAGE_INVALID)                               \
/** [5.1.2.1] */     FN(DATA_TABLE_MISSING_HEADERS)                     \
/** [5.1.2.2] */     FN(DATA_TABLE_MISSING_HEADERS_COLUMN)              \
/** [5.1.2.3] */     FN(DATA_TABLE_MISSING_HEADERS_ROW)                 \
/** [5.2.1.1] */     FN(DATA_TABLE_REQUIRE_MARKUP_COLUMN_HEADERS)       \
/** [5.2.1.2] */     FN(DATA_TABLE_REQUIRE_MARKUP_ROW_HEADERS)          \
/** [5.3.1.1] */     FN(LAYOUT_TABLES_LINEARIZE_PROPERLY)               \
/** [5.4.1.1] */     FN(LAYOUT_TABLE_INVALID_MARKUP)                    \
/** [5.5.1.1] */     FN(TABLE_MISSING_SUMMARY)                          \
/** [5.5.1.2] */     FN(TABLE_SUMMARY_INVALID_NULL)                     \
/** [5.5.1.3] */     FN(TABLE_SUMMARY_INVALID_SPACES)                   \
/** [5.5.1.6] */     FN(TABLE_SUMMARY_INVALID_PLACEHOLDER)              \
/** [5.5.2.1] */     FN(TABLE_MISSING_CAPTION)                          \
/** [5.6.1.1] */     FN(TABLE_MAY_REQUIRE_HEADER_ABBR)                  \
/** [5.6.1.2] */     FN(TABLE_MAY_REQUIRE_HEADER_ABBR_NULL)             \
/** [5.6.1.3] */     FN(TABLE_MAY_REQUIRE_HEADER_ABBR_SPACES)           \
/** [6.1.1.1] */     FN(STYLESHEETS_REQUIRE_TESTING_LINK)               \
/** [6.1.1.2] */     FN(STYLESHEETS_REQUIRE_TESTING_STYLE_ELEMENT)      \
/** [6.1.1.3] */     FN(STYLESHEETS_REQUIRE_TESTING_STYLE_ATTR)         \
/** [6.2.1.1] */     FN(FRAME_SRC_INVALID)                              \
/** [6.2.2.1] */     FN(TEXT_EQUIVALENTS_REQUIRE_UPDATING_APPLET)       \
/** [6.2.2.2] */     FN(TEXT_EQUIVALENTS_REQUIRE_UPDATING_SCRIPT)       \
/** [6.2.2.3] */     FN(TEXT_EQUIVALENTS_REQUIRE_UPDATING_OBJECT)       \
/** [6.3.1.1] */     FN(PROGRAMMATIC_OBJECTS_REQUIRE_TESTING_SCRIPT)    \
/** [6.3.1.2] */     FN(PROGRAMMATIC_OBJECTS_REQUIRE_TESTING_OBJECT)    \
/** [6.3.1.3] */     FN(PROGRAMMATIC_OBJECTS_REQUIRE_TESTING_EMBED)     \
/** [6.3.1.4] */     FN(PROGRAMMATIC_OBJECTS_REQUIRE_TESTING_APPLET)    \
/** [6.5.1.1] */     FN(FRAME_MISSING_NOFRAMES)                         \
/** [6.5.1.2] */     FN(NOFRAMES_INVALID_NO_VALUE)                      \
/** [6.5.1.3] */     FN(NOFRAMES_INVALID_CONTENT)                       \
/** [6.5.1.4] */     FN(NOFRAMES_INVALID_LINK)                          \
/** [7.1.1.1] */     FN(REMOVE_FLICKER_SCRIPT)                          \
/** [7.1.1.2] */     FN(REMOVE_FLICKER_OBJECT)                          \
/** [7.1.1.3] */     FN(REMOVE_FLICKER_EMBED)                           \
/** [7.1.1.4] */     FN(REMOVE_FLICKER_APPLET)                          \
/** [7.1.1.5] */     FN(REMOVE_FLICKER_ANIMATED_GIF)                    \
/** [7.2.1.1] */     FN(REMOVE_BLINK_MARQUEE)                           \
/** [7.4.1.1] */     FN(REMOVE_AUTO_REFRESH)                            \
/** [7.5.1.1] */     FN(REMOVE_AUTO_REDIRECT)                           \
/** [8.1.1.1] */     FN(ENSURE_PROGRAMMATIC_OBJECTS_ACCESSIBLE_SCRIPT)  \
/** [8.1.1.2] */     FN(ENSURE_PROGRAMMATIC_OBJECTS_ACCESSIBLE_OBJECT)  \
/** [8.1.1.3] */     FN(ENSURE_PROGRAMMATIC_OBJECTS_ACCESSIBLE_APPLET)  \
/** [8.1.1.4] */     FN(ENSURE_PROGRAMMATIC_OBJECTS_ACCESSIBLE_EMBED)   \
/** [9.1.1.1] */     FN(IMAGE_MAP_SERVER_SIDE_REQUIRES_CONVERSION)      \
/** [9.3.1.1] */     FN(SCRIPT_NOT_KEYBOARD_ACCESSIBLE_ON_MOUSE_DOWN)   \
/** [9.3.1.2] */     FN(SCRIPT_NOT_KEYBOARD_ACCESSIBLE_ON_MOUSE_UP)     \
/** [9.3.1.3] */     FN(SCRIPT_NOT_KEYBOARD_ACCESSIBLE_ON_CLICK)        \
/** [9.3.1.4] */     FN(SCRIPT_NOT_KEYBOARD_ACCESSIBLE_ON_MOUSE_OVER)   \
/** [9.3.1.5] */     FN(SCRIPT_NOT_KEYBOARD_ACCESSIBLE_ON_MOUSE_OUT)    \
/** [9.3.1.6] */     FN(SCRIPT_NOT_KEYBOARD_ACCESSIBLE_ON_MOUSE_MOVE)   \
/** [10.1.1.1] */    FN(NEW_WINDOWS_REQUIRE_WARNING_NEW)                \
/** [10.1.1.2] */    FN(NEW_WINDOWS_REQUIRE_WARNING_BLANK)              \
/** [10.2.1.1] */    /* FN(LABEL_NEEDS_REPOSITIONING_BEFORE_INPUT) */   \
/** [10.2.1.2] */    /* FN(LABEL_NEEDS_REPOSITIONING_AFTER_INPUT) */    \
/** [10.4.1.1] */    /* FN(FORM_CONTROL_REQUIRES_DEFAULT_TEXT) */       \
/** [10.4.1.2] */    /* FN(FORM_CONTROL_DEFAULT_TEXT_INVALID_NULL) */   \
/** [10.4.1.3] */    /* FN(FORM_CONTROL_DEFAULT_TEXT_INVALID_SPACES) */ \
/** [11.2.1.1] */    FN(REPLACE_DEPRECATED_HTML_APPLET)                 \
/** [11.2.1.2] */    FN(REPLACE_DEPRECATED_HTML_BASEFONT)               \
/** [11.2.1.3] */    FN(REPLACE_DEPRECATED_HTML_CENTER)                 \
/** [11.2.1.4] */    FN(REPLACE_DEPRECATED_HTML_DIR)                    \
/** [11.2.1.5] */    FN(REPLACE_DEPRECATED_HTML_FONT)                   \
/** [11.2.1.6] */    FN(REPLACE_DEPRECATED_HTML_ISINDEX)                \
/** [11.2.1.7] */    FN(REPLACE_DEPRECATED_HTML_MENU)                   \
/** [11.2.1.8] */    FN(REPLACE_DEPRECATED_HTML_S)                      \
/** [11.2.1.9] */    FN(REPLACE_DEPRECATED_HTML_STRIKE)                 \
/** [11.2.1.10] */   FN(REPLACE_DEPRECATED_HTML_U)                      \
/** [12.1.1.1] */    FN(FRAME_MISSING_TITLE)                            \
/** [12.1.1.2] */    FN(FRAME_TITLE_INVALID_NULL)                       \
/** [12.1.1.3] */    FN(FRAME_TITLE_INVALID_SPACES)                     \
/** [12.4.1.1] */    FN(ASSOCIATE_LABELS_EXPLICITLY)                    \
/** [12.4.1.2] */    FN(ASSOCIATE_LABELS_EXPLICITLY_FOR)                \
/** [12.4.1.3] */    FN(ASSOCIATE_LABELS_EXPLICITLY_ID)                 \
/** [13.1.1.1] */    FN(LINK_TEXT_NOT_MEANINGFUL)                       \
/** [13.1.1.2] */    FN(LINK_TEXT_MISSING)                              \
/** [13.1.1.3] */    FN(LINK_TEXT_TOO_LONG)                             \
/** [13.1.1.4] */    FN(LINK_TEXT_NOT_MEANINGFUL_CLICK_HERE)            \
/** [13.1.1.5] */    /* FN(LINK_TEXT_NOT_MEANINGFUL_MORE) */            \
/** [13.1.1.6] */    /* FN(LINK_TEXT_NOT_MEANINGFUL_FOLLOW_THIS) */     \
/** [13.2.1.1] */    FN(METADATA_MISSING)                               \
/** [13.2.1.2] */    /* FN(METADATA_MISSING_LINK) */                    \
/** [13.2.1.3] */    FN(METADATA_MISSING_REDIRECT_AUTOREFRESH)          \
/** [13.10.1.1] */   FN(SKIPOVER_ASCII_ART)


/** These message codes comprise every message is exclusive to theTidy console
 ** application. It it possible to build LibTidy without these strings.
 */
#if SUPPORT_CONSOLE_APP
#define FOREACH_MSG_CONSOLE(FN) \
        FN(TC_LABEL_COL)                    \
        FN(TC_LABEL_FILE)                   \
        FN(TC_LABEL_LANG)                   \
        FN(TC_LABEL_LEVL)                   \
        FN(TC_LABEL_OPT)                    \
        FN(TC_MAIN_ERROR_LOAD_CONFIG)       \
        FN(TC_OPT_ACCESS)                   \
        FN(TC_OPT_ASCII)                    \
        FN(TC_OPT_ASHTML)                   \
        FN(TC_OPT_ASXML)                    \
        FN(TC_OPT_BARE)                     \
        FN(TC_OPT_BIG5)                     \
        FN(TC_OPT_CLEAN)                    \
        FN(TC_OPT_CONFIG)                   \
        FN(TC_OPT_ERRORS)                   \
        FN(TC_OPT_FILE)                     \
        FN(TC_OPT_GDOC)                     \
        FN(TC_OPT_HELP)                     \
        FN(TC_OPT_HELPCFG)                  \
        FN(TC_OPT_HELPENV)                  \
        FN(TC_OPT_HELPOPT)                  \
        FN(TC_OPT_IBM858)                   \
        FN(TC_OPT_INDENT)                   \
        FN(TC_OPT_ISO2022)                  \
        FN(TC_OPT_LANGUAGE)                 \
        FN(TC_OPT_LATIN0)                   \
        FN(TC_OPT_LATIN1)                   \
        FN(TC_OPT_MAC)                      \
        FN(TC_OPT_MODIFY)                   \
        FN(TC_OPT_NUMERIC)                  \
        FN(TC_OPT_OMIT)                     \
        FN(TC_OPT_OUTPUT)                   \
        FN(TC_OPT_QUIET)                    \
        FN(TC_OPT_RAW)                      \
        FN(TC_OPT_SHIFTJIS)                 \
        FN(TC_OPT_SHOWCFG)                  \
        FN(TC_OPT_EXP_CFG)                  \
        FN(TC_OPT_EXP_DEF)                  \
        FN(TC_OPT_UPPER)                    \
        FN(TC_OPT_UTF16)                    \
        FN(TC_OPT_UTF16BE)                  \
        FN(TC_OPT_UTF16LE)                  \
        FN(TC_OPT_UTF8)                     \
        FN(TC_OPT_VERSION)                  \
        FN(TC_OPT_WIN1252)                  \
        FN(TC_OPT_WRAP)                     \
        FN(TC_OPT_XML)                      \
        FN(TC_OPT_XMLCFG)                   \
        FN(TC_OPT_XMLSTRG)                  \
        FN(TC_OPT_XMLERRS)                  \
        FN(TC_OPT_XMLOPTS)                  \
        FN(TC_OPT_XMLHELP)                  \
        FN(TC_STRING_CONF_HEADER)           \
        FN(TC_STRING_CONF_NAME)             \
        FN(TC_STRING_CONF_TYPE)             \
        FN(TC_STRING_CONF_VALUE)            \
        FN(TC_STRING_CONF_NOTE)             \
        FN(TC_STRING_OPT_NOT_DOCUMENTED)    \
        FN(TC_STRING_OUT_OF_MEMORY)         \
        FN(TC_STRING_FATAL_ERROR)           \
        FN(TC_STRING_FILE_MANIP)            \
        FN(TC_STRING_LANG_MUST_SPECIFY)     \
        FN(TC_STRING_LANG_NOT_FOUND)        \
        FN(TC_STRING_MUST_SPECIFY)          \
        FN(TC_STRING_PROCESS_DIRECTIVES)    \
        FN(TC_STRING_CHAR_ENCODING)         \
        FN(TC_STRING_MISC)                  \
        FN(TC_STRING_XML)                   \
        FN(TC_STRING_UNKNOWN_OPTION)        \
        FN(TC_STRING_UNKNOWN_OPTION_B)      \
        FN(TC_STRING_VERS_A)                \
        FN(TC_STRING_VERS_B)                \
        FN(TC_TXT_HELP_1)                   \
        FN(TC_TXT_HELP_2A)                  \
        FN(TC_TXT_HELP_2B)                  \
        FN(TC_TXT_HELP_3)                   \
        FN(TC_TXT_HELP_3A)                  \
        FN(TC_TXT_HELP_CONFIG)              \
        FN(TC_TXT_HELP_CONFIG_NAME)         \
        FN(TC_TXT_HELP_CONFIG_TYPE)         \
        FN(TC_TXT_HELP_CONFIG_ALLW)         \
        FN(TC_TXT_HELP_ENV_1)               \
        FN(TC_TXT_HELP_ENV_1A)              \
        FN(TC_TXT_HELP_ENV_1B)              \
        FN(TC_TXT_HELP_ENV_1C)              \
        FN(TC_TXT_HELP_LANG_1)              \
        FN(TC_TXT_HELP_LANG_2)              \
        FN(TC_TXT_HELP_LANG_3)
#endif /* SUPPORT_CONSOLE_APP */

/** @} */

/** @} end group public_enum_gen */


/* MARK: - Public Enumerations */
/***************************************************************************//**
 ** @defgroup public_enumerations Public Enumerations
 ** @ingroup public_api
 **
 ** @copybrief tidyenum.h
 ******************************************************************************/

/** @addtogroup public_enumerations
 ** @{ */

/** @name Configuration Options Enumerations
 **
 ** These enumerators are used to define available configuration options and
 ** their option categories.
 **
 ** @{ */


/** Option IDs are used used to get and/or set configuration option values and
 **        retrieve their descriptions.
 **
 ** @remark These enum members all have associated localized strings available
 **         which describe the purpose of the option. These descriptions are
 **         available via their enum values only.
 **
 ** @sa     `config.c:option_defs[]` for internal implementation details; that
 **         array is where you will implement options defined in this enum; and
 **         it's important to add a string describing the option to
 **         `language_en.h`, too.
 */
typedef enum
{
    TidyUnknownOption = 0,       /**< Unknown option! */

    TidyAccessibilityCheckLevel, /**< Accessibility check level */
    TidyAltText,                 /**< Default text for alt attribute */
    TidyAnchorAsName,            /**< Define anchors as name attributes */
    TidyAsciiChars,              /**< Convert quotes and dashes to nearest ASCII char */
    TidyBlockTags,               /**< Declared block tags */
    TidyBodyOnly,                /**< Output BODY content only */
    TidyBreakBeforeBR,           /**< Output newline before <br> or not? */
    TidyCharEncoding,            /**< In/out character encoding */
    TidyCoerceEndTags,           /**< Coerce end tags from start tags where probably intended */
    TidyCSSPrefix,               /**< CSS class naming for clean option */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    TidyCustomTags,              /**< Internal use ONLY */
#endif
    TidyDecorateInferredUL,      /**< Mark inferred UL elements with no indent CSS */
    TidyDoctype,                 /**< User specified doctype */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    TidyDoctypeMode,             /**< Internal use ONLY */
#endif
    TidyDropEmptyElems,          /**< Discard empty elements */
    TidyDropEmptyParas,          /**< Discard empty p elements */
    TidyDropPropAttrs,           /**< Discard proprietary attributes */
    TidyDuplicateAttrs,          /**< Keep first or last duplicate attribute */
    TidyEmacs,                   /**< If true, format error output for GNU Emacs */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    TidyEmacsFile,               /**< Internal use ONLY */
#endif
    TidyEmptyTags,               /**< Declared empty tags */
    TidyEncloseBlockText,        /**< If yes text in blocks is wrapped in P's */
    TidyEncloseBodyText,         /**< If yes text at body is wrapped in P's */
    TidyErrFile,                 /**< File name to write errors to */
    TidyEscapeCdata,             /**< Replace <![CDATA[]]> sections with escaped text */
    TidyEscapeScripts,           /**< Escape items that look like closing tags in script tags */
    TidyFixBackslash,            /**< Fix URLs by replacing \ with / */
    TidyFixComments,             /**< Fix comments with adjacent hyphens */
    TidyFixUri,                  /**< Applies URI encoding if necessary */
    TidyForceOutput,             /**< Output document even if errors were found */
    TidyGDocClean,               /**< Clean up HTML exported from Google Docs */
    TidyHideComments,            /**< Hides all (real) comments in output */
    TidyHtmlOut,                 /**< Output plain HTML, even for XHTML input.*/
    TidyInCharEncoding,          /**< Input character encoding (if different) */
    TidyIndentAttributes,        /**< Newline+indent before each attribute */
    TidyIndentCdata,             /**< Indent <!CDATA[ ... ]]> section */
    TidyIndentContent,           /**< Indent content of appropriate tags */
    TidyIndentSpaces,            /**< Indentation n spaces/tabs */
    TidyInlineTags,              /**< Declared inline tags */
    TidyJoinClasses,             /**< Join multiple class attributes */
    TidyJoinStyles,              /**< Join multiple style attributes */
    TidyKeepFileTimes,           /**< If yes last modied time is preserved */
    TidyKeepTabs,                /**< If yes keep input source tabs */
    TidyLiteralAttribs,          /**< If true attributes may use newlines */
    TidyLogicalEmphasis,         /**< Replace i by em and b by strong */
    TidyLowerLiterals,           /**< Folds known attribute values to lower case */
    TidyMakeBare,                /**< Replace smart quotes, em dashes, etc with ASCII */
    TidyMakeClean,               /**< Replace presentational clutter by style rules */
    TidyMark,                    /**< Add meta element indicating tidied doc */
    TidyMergeDivs,               /**< Merge multiple DIVs */
    TidyMergeEmphasis,           /**< Merge nested B and I elements */
    TidyMergeSpans,              /**< Merge multiple SPANs */
    TidyMetaCharset,             /**< Adds/checks/fixes meta charset in the head, based on document type */
    TidyMuteReports,             /**< Filter these messages from output. */
    TidyMuteShow,                /**< Show message ID's in the error table */
    TidyNCR,                     /**< Allow numeric character references */
    TidyNewline,                 /**< Output line ending (default to platform) */
    TidyNumEntities,             /**< Use numeric entities */
    TidyOmitOptionalTags,        /**< Suppress optional start tags and end tags */
    TidyOutCharEncoding,         /**< Output character encoding (if different) */
    TidyOutFile,                 /**< File name to write markup to */
    TidyOutputBOM,               /**< Output a Byte Order Mark (BOM) for UTF-16 encodings */
    TidyPPrintTabs,              /**< Indent using tabs instead of spaces */
    TidyPreserveEntities,        /**< Preserve entities */
    TidyPreTags,                 /**< Declared pre tags */
    TidyPriorityAttributes,      /**< Attributes to place first in an element */
    TidyPunctWrap,               /**< consider punctuation and breaking spaces for wrapping */
    TidyQuiet,                   /**< No 'Parsing X', guessed DTD or summary */
    TidyQuoteAmpersand,          /**< Output naked ampersand as &amp; */
    TidyQuoteMarks,              /**< Output " marks as &quot; */
    TidyQuoteNbsp,               /**< Output non-breaking space as entity */
    TidyReplaceColor,            /**< Replace hex color attribute values with names */
    TidyShowErrors,              /**< Number of errors to put out */
    TidyShowFilename,            /**< If true, the input filename is displayed with the error messages */
    TidyShowInfo,                /**< If true, info-level messages are shown */
    TidyShowMarkup,              /**< If false, normal output is suppressed */
    TidyShowMetaChange,          /**< show when meta http-equiv content charset was changed - compatibility */
    TidyShowWarnings,            /**< However errors are always shown */
    TidySkipNested,              /**< Skip nested tags in script and style CDATA */
    TidySortAttributes,          /**< Sort attributes */
    TidyStrictTagsAttr,          /**< Ensure tags and attributes match output HTML version */
    TidyStyleTags,               /**< Move style to head */
    TidyTabSize,                 /**< Expand tabs to n spaces */
    TidyUpperCaseAttrs,          /**< Output attributes in upper not lower case */
    TidyUpperCaseTags,           /**< Output tags in upper not lower case */
    TidyUseCustomTags,           /**< Enable Tidy to use autonomous custom tags */
    TidyVertSpace,               /**< degree to which markup is spread out vertically */
    TidyWarnPropAttrs,           /**< Warns on proprietary attributes */
    TidyWord2000,                /**< Draconian cleaning for Word2000 */
    TidyWrapAsp,                 /**< Wrap within ASP pseudo elements */
    TidyWrapAttVals,             /**< Wrap within attribute values */
    TidyWrapJste,                /**< Wrap within JSTE pseudo elements */
    TidyWrapLen,                 /**< Wrap margin */
    TidyWrapPhp,                 /**< Wrap consecutive PHP pseudo elements */
    TidyWrapScriptlets,          /**< Wrap within JavaScript string literals */
    TidyWrapSection,             /**< Wrap within <![ ... ]> section tags */
    TidyWriteBack,               /**< If true then output tidied markup */
    TidyXhtmlOut,                /**< Output extensible HTML */
    TidyXmlDecl,                 /**< Add <?xml?> for XML docs */
    TidyXmlOut,                  /**< Create output as XML */
    TidyXmlPIs,                  /**< If set to yes PIs must end with ?> */
    TidyXmlSpace,                /**< If set to yes adds xml:space attr as needed */
    TidyXmlTags,                 /**< Treat input as XML */
    N_TIDY_OPTIONS               /**< Must be last */
} TidyOptionId;


/** Categories of Tidy configuration options, which are used mostly by user
 ** interfaces to sort Tidy options into related groups.
 **
 ** @remark These enum members all have associated localized strings available
 **         suitable for use as a category label, and are available with either
 **         the enum value, or a string version of the name.
 **
 ** @sa     `config.c:option_defs[]` for internal implementation details.
 */
typedef enum
{
    TidyUnknownCategory = 300,  /**< Unknown Category! */
    FOREACH_TIDYCONFIGCATEGORY(MAKE_ENUM)
} TidyConfigCategory;


/** A Tidy configuration option can have one of these data types. */
typedef enum
{
  TidyString,          /**< String */
  TidyInteger,         /**< Integer or enumeration */
  TidyBoolean          /**< Boolean */
} TidyOptionType;


/** @}
 ** @name Configuration Options Pick List and Parser Enumerations
 **
 ** These enums define enumerated states for the configuration options that
 ** take values that are not simple yes/no, strings, or simple integers.
 **
 ** @{ */

/** AutoBool values used by ParseBool, ParseTriState, ParseIndent, ParseBOM
 ** @remark This enum's starting value is guaranteed to remain stable.
 */
typedef enum
{
   TidyNoState = 0, /**< maps to 'no' */
   TidyYesState,    /**< maps to 'yes' */
   TidyAutoState    /**< Automatic */
} TidyTriState;


/** Values used by ParseUseCustomTags, which describes how Autonomous Custom
 ** tags (ACT's) found by Tidy are treated.
 **
 ** @remark These enum members all have associated localized strings available
 **         for internal LibTidy use, and also have public string keys in the
 **         form MEMBER_STRING, e.g., TIDYCUSTOMBLOCKLEVEL_STRING
 **
 ** @remark This enum's starting value is guaranteed to remain stable.
 */
typedef enum
{
    TidyCustomNo = 0,       /**< Do not allow autonomous custom tags */
    TidyCustomBlocklevel,   /**< ACT's treated as blocklevel */
    TidyCustomEmpty,        /**< ACT's treated as empty tags */
    TidyCustomInline,       /**< ACT's treated as inline tags */
    TidyCustomPre           /**< ACT's treated as pre tags */
} TidyUseCustomTagsState;


/** TidyNewline option values to control output line endings.
 ** @remark This enum's starting value is guaranteed to remain stable.
 */
typedef enum
{
    TidyLF = 0,     /**< Use Unix style: LF */
    TidyCRLF,       /**< Use DOS/Windows style: CR+LF */
    TidyCR          /**< Use Macintosh style: CR */
} TidyLineEnding;


/** TidyEncodingOptions option values specify the input and/or output encoding.
 ** @remark This enum's starting value is guaranteed to remain stable.
 */
typedef enum
{
    TidyEncRaw = 0,
    TidyEncAscii,
    TidyEncLatin0,
    TidyEncLatin1,
    TidyEncUtf8,
#ifndef NO_NATIVE_ISO2022_SUPPORT
    TidyEncIso2022,
#endif
    TidyEncMac,
    TidyEncWin1252,
    TidyEncIbm858,
    TidyEncUtf16le,
    TidyEncUtf16be,
    TidyEncUtf16,
    TidyEncBig5,
    TidyEncShiftjis
} TidyEncodingOptions;


/** Mode controlling treatment of doctype
 ** @remark This enum's starting value is guaranteed to remain stable.
 */
typedef enum
{
    TidyDoctypeHtml5 = 0, /**< <!DOCTYPE html> */
    TidyDoctypeOmit,      /**< Omit DOCTYPE altogether */
    TidyDoctypeAuto,      /**< Keep DOCTYPE in input.  Set version to content */
    TidyDoctypeStrict,    /**< Convert document to HTML 4 strict content model */
    TidyDoctypeLoose,     /**< Convert document to HTML 4 transitional content model */
    TidyDoctypeUser       /**< Set DOCTYPE FPI explicitly */
} TidyDoctypeModes;

/** Mode controlling treatment of duplicate Attributes
 ** @remark This enum's starting value is guaranteed to remain stable.
 */
typedef enum
{
    TidyKeepFirst = 0, /**< Keep the first instance of an attribute */
    TidyKeepLast       /**< Keep the last instance of an attribute */
} TidyDupAttrModes;

/** Mode controlling treatment of sorting attributes
 ** @remark This enum's starting value is guaranteed to remain stable.
 */
typedef enum
{
    TidySortAttrNone = 0,  /**< Don't sort attributes */
    TidySortAttrAlpha      /**< Sort attributes alphabetically */
} TidyAttrSortStrategy;

/** Mode controlling capitalization of things, such as attributes.
 ** @remark This enum's starting value is guaranteed to remain stable.
 */
typedef enum
{
    TidyUppercaseNo = 0,   /**< Don't uppercase. */
    TidyUppercaseYes,      /**< Do uppercase. */
    TidyUppercasePreserve  /**< Preserve case. */
} TidyUppercase;


/** @}
 ** @name Document Tree
 ** @{
 */

/** Node types
 */
typedef enum
{
  TidyNode_Root,        /**< Root */
  TidyNode_DocType,     /**< DOCTYPE */
  TidyNode_Comment,     /**< Comment */
  TidyNode_ProcIns,     /**< Processing Instruction */
  TidyNode_Text,        /**< Text */
  TidyNode_Start,       /**< Start Tag */
  TidyNode_End,         /**< End Tag */
  TidyNode_StartEnd,    /**< Start/End (empty) Tag */
  TidyNode_CDATA,       /**< Unparsed Text */
  TidyNode_Section,     /**< XML Section */
  TidyNode_Asp,         /**< ASP Source */
  TidyNode_Jste,        /**< JSTE Source */
  TidyNode_Php,         /**< PHP Source */
  TidyNode_XmlDecl      /**< XML Declaration */
} TidyNodeType;


/** Known HTML element types
 */
typedef enum
{
  TidyTag_UNKNOWN,       /**< Unknown tag! Must be first */
  TidyTag_A,             /**< A */
  TidyTag_ABBR,          /**< ABBR */
  TidyTag_ACRONYM,       /**< ACRONYM */
  TidyTag_ADDRESS,       /**< ADDRESS */
  TidyTag_ALIGN,         /**< ALIGN */
  TidyTag_APPLET,        /**< APPLET */
  TidyTag_AREA,          /**< AREA */
  TidyTag_B,             /**< B */
  TidyTag_BASE,          /**< BASE */
  TidyTag_BASEFONT,      /**< BASEFONT */
  TidyTag_BDO,           /**< BDO */
  TidyTag_BGSOUND,       /**< BGSOUND */
  TidyTag_BIG,           /**< BIG */
  TidyTag_BLINK,         /**< BLINK */
  TidyTag_BLOCKQUOTE,    /**< BLOCKQUOTE */
  TidyTag_BODY,          /**< BODY */
  TidyTag_BR,            /**< BR */
  TidyTag_BUTTON,        /**< BUTTON */
  TidyTag_CAPTION,       /**< CAPTION */
  TidyTag_CENTER,        /**< CENTER */
  TidyTag_CITE,          /**< CITE */
  TidyTag_CODE,          /**< CODE */
  TidyTag_COL,           /**< COL */
  TidyTag_COLGROUP,      /**< COLGROUP */
  TidyTag_COMMENT,       /**< COMMENT */
  TidyTag_DD,            /**< DD */
  TidyTag_DEL,           /**< DEL */
  TidyTag_DFN,           /**< DFN */
  TidyTag_DIR,           /**< DIR */
  TidyTag_DIV,           /**< DIF */
  TidyTag_DL,            /**< DL */
  TidyTag_DT,            /**< DT */
  TidyTag_EM,            /**< EM */
  TidyTag_EMBED,         /**< EMBED */
  TidyTag_FIELDSET,      /**< FIELDSET */
  TidyTag_FONT,          /**< FONT */
  TidyTag_FORM,          /**< FORM */
  TidyTag_FRAME,         /**< FRAME */
  TidyTag_FRAMESET,      /**< FRAMESET */
  TidyTag_H1,            /**< H1 */
  TidyTag_H2,            /**< H2 */
  TidyTag_H3,            /**< H3 */
  TidyTag_H4,            /**< H4 */
  TidyTag_H5,            /**< H5 */
  TidyTag_H6,            /**< H6 */
  TidyTag_HEAD,          /**< HEAD */
  TidyTag_HR,            /**< HR */
  TidyTag_HTML,          /**< HTML */
  TidyTag_I,             /**< I */
  TidyTag_IFRAME,        /**< IFRAME */
  TidyTag_ILAYER,        /**< ILAYER */
  TidyTag_IMG,           /**< IMG */
  TidyTag_INPUT,         /**< INPUT */
  TidyTag_INS,           /**< INS */
  TidyTag_ISINDEX,       /**< ISINDEX */
  TidyTag_KBD,           /**< KBD */
  TidyTag_KEYGEN,        /**< KEYGEN */
  TidyTag_LABEL,         /**< LABEL */
  TidyTag_LAYER,         /**< LAYER */
  TidyTag_LEGEND,        /**< LEGEND */
  TidyTag_LI,            /**< LI */
  TidyTag_LINK,          /**< LINK */
  TidyTag_LISTING,       /**< LISTING */
  TidyTag_MAP,           /**< MAP */
  TidyTag_MATHML,        /**< MATH  (HTML5) [i_a]2 MathML embedded in [X]HTML */
  TidyTag_MARQUEE,       /**< MARQUEE */
  TidyTag_MENU,          /**< MENU */
  TidyTag_META,          /**< META */
  TidyTag_MULTICOL,      /**< MULTICOL */
  TidyTag_NOBR,          /**< NOBR */
  TidyTag_NOEMBED,       /**< NOEMBED */
  TidyTag_NOFRAMES,      /**< NOFRAMES */
  TidyTag_NOLAYER,       /**< NOLAYER */
  TidyTag_NOSAVE,        /**< NOSAVE */
  TidyTag_NOSCRIPT,      /**< NOSCRIPT */
  TidyTag_OBJECT,        /**< OBJECT */
  TidyTag_OL,            /**< OL */
  TidyTag_OPTGROUP,      /**< OPTGROUP */
  TidyTag_OPTION,        /**< OPTION */
  TidyTag_P,             /**< P */
  TidyTag_PARAM,         /**< PARAM */
  TidyTag_PICTURE,       /**< PICTURE (HTML5) */
  TidyTag_PLAINTEXT,     /**< PLAINTEXT */
  TidyTag_PRE,           /**< PRE */
  TidyTag_Q,             /**< Q */
  TidyTag_RB,            /**< RB */
  TidyTag_RBC,           /**< RBC */
  TidyTag_RP,            /**< RP */
  TidyTag_RT,            /**< RT */
  TidyTag_RTC,           /**< RTC */
  TidyTag_RUBY,          /**< RUBY */
  TidyTag_S,             /**< S */
  TidyTag_SAMP,          /**< SAMP */
  TidyTag_SCRIPT,        /**< SCRIPT */
  TidyTag_SELECT,        /**< SELECT */
  TidyTag_SERVER,        /**< SERVER */
  TidyTag_SERVLET,       /**< SERVLET */
  TidyTag_SMALL,         /**< SMALL */
  TidyTag_SPACER,        /**< SPACER */
  TidyTag_SPAN,          /**< SPAN */
  TidyTag_STRIKE,        /**< STRIKE */
  TidyTag_STRONG,        /**< STRONG */
  TidyTag_STYLE,         /**< STYLE */
  TidyTag_SUB,           /**< SUB */
  TidyTag_SUP,           /**< SUP */
  TidyTag_SVG,           /**< SVG  (HTML5) */
  TidyTag_TABLE,         /**< TABLE */
  TidyTag_TBODY,         /**< TBODY */
  TidyTag_TD,            /**< TD */
  TidyTag_TEXTAREA,      /**< TEXTAREA */
  TidyTag_TFOOT,         /**< TFOOT */
  TidyTag_TH,            /**< TH */
  TidyTag_THEAD,         /**< THEAD */
  TidyTag_TITLE,         /**< TITLE */
  TidyTag_TR,            /**< TR */
  TidyTag_TT,            /**< TT */
  TidyTag_U,             /**< U */
  TidyTag_UL,            /**< UL */
  TidyTag_VAR,           /**< VAR */
  TidyTag_WBR,           /**< WBR */
  TidyTag_XMP,           /**< XMP */
  TidyTag_NEXTID,        /**< NEXTID */

  TidyTag_ARTICLE,       /**< ARTICLE */
  TidyTag_ASIDE,         /**< ASIDE */
  TidyTag_AUDIO,         /**< AUDIO */
  TidyTag_BDI,           /**< BDI */
  TidyTag_CANVAS,        /**< CANVAS */
  TidyTag_COMMAND,       /**< COMMAND */
  TidyTag_DATA,          /**< DATA */
  TidyTag_DATALIST,      /**< DATALIST */
  TidyTag_DETAILS,       /**< DETAILS */
  TidyTag_DIALOG,        /**< DIALOG */
  TidyTag_FIGCAPTION,    /**< FIGCAPTION */
  TidyTag_FIGURE,        /**< FIGURE */
  TidyTag_FOOTER,        /**< FOOTER */
  TidyTag_HEADER,        /**< HEADER */
  TidyTag_HGROUP,        /**< HGROUP */
  TidyTag_MAIN,          /**< MAIN */
  TidyTag_MARK,          /**< MARK */
  TidyTag_MENUITEM,      /**< MENUITEM */
  TidyTag_METER,         /**< METER */
  TidyTag_NAV,           /**< NAV */
  TidyTag_OUTPUT,        /**< OUTPUT */
  TidyTag_PROGRESS,      /**< PROGRESS */
  TidyTag_SECTION,       /**< SECTION */
  TidyTag_SOURCE,        /**< SOURCE */
  TidyTag_SUMMARY,       /**< SUMMARY */
  TidyTag_TEMPLATE,      /**< TEMPLATE */
  TidyTag_TIME,          /**< TIME */
  TidyTag_TRACK,         /**< TRACK */
  TidyTag_VIDEO,         /**< VIDEO */
  TidyTag_SLOT,          /**< SLOT */

  N_TIDY_TAGS            /**< Must be last */
} TidyTagId;


/** Known HTML attributes
 */
typedef enum
{
  TidyAttr_UNKNOWN,                /**< UNKNOWN= */
  TidyAttr_ABBR,                   /**< ABBR= */
  TidyAttr_ACCEPT,                 /**< ACCEPT= */
  TidyAttr_ACCEPT_CHARSET,         /**< ACCEPT_CHARSET= */
  TidyAttr_ACCESSKEY,              /**< ACCESSKEY= */
  TidyAttr_ACTION,                 /**< ACTION= */
  TidyAttr_ADD_DATE,               /**< ADD_DATE= */
  TidyAttr_ALIGN,                  /**< ALIGN= */
  TidyAttr_ALINK,                  /**< ALINK= */
  TidyAttr_ALLOWFULLSCREEN,        /**< ALLOWFULLSCREEN= */
  TidyAttr_ALT,                    /**< ALT= */
  TidyAttr_ARCHIVE,                /**< ARCHIVE= */
  TidyAttr_AXIS,                   /**< AXIS= */
  TidyAttr_BACKGROUND,             /**< BACKGROUND= */
  TidyAttr_BGCOLOR,                /**< BGCOLOR= */
  TidyAttr_BGPROPERTIES,           /**< BGPROPERTIES= */
  TidyAttr_BORDER,                 /**< BORDER= */
  TidyAttr_BORDERCOLOR,            /**< BORDERCOLOR= */
  TidyAttr_BOTTOMMARGIN,           /**< BOTTOMMARGIN= */
  TidyAttr_CELLPADDING,            /**< CELLPADDING= */
  TidyAttr_CELLSPACING,            /**< CELLSPACING= */
  TidyAttr_CHAR,                   /**< CHAR= */
  TidyAttr_CHAROFF,                /**< CHAROFF= */
  TidyAttr_CHARSET,                /**< CHARSET= */
  TidyAttr_CHECKED,                /**< CHECKED= */
  TidyAttr_CITE,                   /**< CITE= */
  TidyAttr_CLASS,                  /**< CLASS= */
  TidyAttr_CLASSID,                /**< CLASSID= */
  TidyAttr_CLEAR,                  /**< CLEAR= */
  TidyAttr_CODE,                   /**< CODE= */
  TidyAttr_CODEBASE,               /**< CODEBASE= */
  TidyAttr_CODETYPE,               /**< CODETYPE= */
  TidyAttr_COLOR,                  /**< COLOR= */
  TidyAttr_COLS,                   /**< COLS= */
  TidyAttr_COLSPAN,                /**< COLSPAN= */
  TidyAttr_COMPACT,                /**< COMPACT= */
  TidyAttr_CONTENT,                /**< CONTENT= */
  TidyAttr_COORDS,                 /**< COORDS= */
  TidyAttr_DATA,                   /**< DATA= */
  TidyAttr_DATAFLD,                /**< DATAFLD= */
  TidyAttr_DATAFORMATAS,           /**< DATAFORMATAS= */
  TidyAttr_DATAPAGESIZE,           /**< DATAPAGESIZE= */
  TidyAttr_DATASRC,                /**< DATASRC= */
  TidyAttr_DATETIME,               /**< DATETIME= */
  TidyAttr_DECLARE,                /**< DECLARE= */
  TidyAttr_DEFER,                  /**< DEFER= */
  TidyAttr_DIR,                    /**< DIR= */
  TidyAttr_DISABLED,               /**< DISABLED= */
  TidyAttr_DOWNLOAD,               /**< DOWNLOAD= */
  TidyAttr_ENCODING,               /**< ENCODING= */
  TidyAttr_ENCTYPE,                /**< ENCTYPE= */
  TidyAttr_FACE,                   /**< FACE= */
  TidyAttr_FOR,                    /**< FOR= */
  TidyAttr_FRAME,                  /**< FRAME= */
  TidyAttr_FRAMEBORDER,            /**< FRAMEBORDER= */
  TidyAttr_FRAMESPACING,           /**< FRAMESPACING= */
  TidyAttr_GRIDX,                  /**< GRIDX= */
  TidyAttr_GRIDY,                  /**< GRIDY= */
  TidyAttr_HEADERS,                /**< HEADERS= */
  TidyAttr_HEIGHT,                 /**< HEIGHT= */
  TidyAttr_HREF,                   /**< HREF= */
  TidyAttr_HREFLANG,               /**< HREFLANG= */
  TidyAttr_HSPACE,                 /**< HSPACE= */
  TidyAttr_HTTP_EQUIV,             /**< HTTP_EQUIV= */
  TidyAttr_ID,                     /**< ID= */
  TidyAttr_IS,                     /**< IS= */
  TidyAttr_ISMAP,                  /**< ISMAP= */
  TidyAttr_ITEMID,                 /**< ITEMID= */
  TidyAttr_ITEMPROP,               /**< ITEMPROP= */
  TidyAttr_ITEMREF,                /**< ITEMREF= */
  TidyAttr_ITEMSCOPE,              /**< ITEMSCOPE= */
  TidyAttr_ITEMTYPE,               /**< ITEMTYPE= */
  TidyAttr_LABEL,                  /**< LABEL= */
  TidyAttr_LANG,                   /**< LANG= */
  TidyAttr_LANGUAGE,               /**< LANGUAGE= */
  TidyAttr_LAST_MODIFIED,          /**< LAST_MODIFIED= */
  TidyAttr_LAST_VISIT,             /**< LAST_VISIT= */
  TidyAttr_LEFTMARGIN,             /**< LEFTMARGIN= */
  TidyAttr_LINK,                   /**< LINK= */
  TidyAttr_LONGDESC,               /**< LONGDESC= */
  TidyAttr_LOWSRC,                 /**< LOWSRC= */
  TidyAttr_MARGINHEIGHT,           /**< MARGINHEIGHT= */
  TidyAttr_MARGINWIDTH,            /**< MARGINWIDTH= */
  TidyAttr_MAXLENGTH,              /**< MAXLENGTH= */
  TidyAttr_MEDIA,                  /**< MEDIA= */
  TidyAttr_METHOD,                 /**< METHOD= */
  TidyAttr_MULTIPLE,               /**< MULTIPLE= */
  TidyAttr_NAME,                   /**< NAME= */
  TidyAttr_NOHREF,                 /**< NOHREF= */
  TidyAttr_NORESIZE,               /**< NORESIZE= */
  TidyAttr_NOSHADE,                /**< NOSHADE= */
  TidyAttr_NOWRAP,                 /**< NOWRAP= */
  TidyAttr_OBJECT,                 /**< OBJECT= */
  TidyAttr_OnAFTERUPDATE,          /**< OnAFTERUPDATE= */
  TidyAttr_OnBEFOREUNLOAD,         /**< OnBEFOREUNLOAD= */
  TidyAttr_OnBEFOREUPDATE,         /**< OnBEFOREUPDATE= */
  TidyAttr_OnBLUR,                 /**< OnBLUR= */
  TidyAttr_OnCHANGE,               /**< OnCHANGE= */
  TidyAttr_OnCLICK,                /**< OnCLICK= */
  TidyAttr_OnDATAAVAILABLE,        /**< OnDATAAVAILABLE= */
  TidyAttr_OnDATASETCHANGED,       /**< OnDATASETCHANGED= */
  TidyAttr_OnDATASETCOMPLETE,      /**< OnDATASETCOMPLETE= */
  TidyAttr_OnDBLCLICK,             /**< OnDBLCLICK= */
  TidyAttr_OnERRORUPDATE,          /**< OnERRORUPDATE= */
  TidyAttr_OnFOCUS,                /**< OnFOCUS= */
  TidyAttr_OnKEYDOWN,              /**< OnKEYDOWN= */
  TidyAttr_OnKEYPRESS,             /**< OnKEYPRESS= */
  TidyAttr_OnKEYUP,                /**< OnKEYUP= */
  TidyAttr_OnLOAD,                 /**< OnLOAD= */
  TidyAttr_OnMOUSEDOWN,            /**< OnMOUSEDOWN= */
  TidyAttr_OnMOUSEMOVE,            /**< OnMOUSEMOVE= */
  TidyAttr_OnMOUSEOUT,             /**< OnMOUSEOUT= */
  TidyAttr_OnMOUSEOVER,            /**< OnMOUSEOVER= */
  TidyAttr_OnMOUSEUP,              /**< OnMOUSEUP= */
  TidyAttr_OnRESET,                /**< OnRESET= */
  TidyAttr_OnROWENTER,             /**< OnROWENTER= */
  TidyAttr_OnROWEXIT,              /**< OnROWEXIT= */
  TidyAttr_OnSELECT,               /**< OnSELECT= */
  TidyAttr_OnSUBMIT,               /**< OnSUBMIT= */
  TidyAttr_OnUNLOAD,               /**< OnUNLOAD= */
  TidyAttr_PROFILE,                /**< PROFILE= */
  TidyAttr_PROMPT,                 /**< PROMPT= */
  TidyAttr_RBSPAN,                 /**< RBSPAN= */
  TidyAttr_READONLY,               /**< READONLY= */
  TidyAttr_REL,                    /**< REL= */
  TidyAttr_REV,                    /**< REV= */
  TidyAttr_RIGHTMARGIN,            /**< RIGHTMARGIN= */
  TidyAttr_ROLE,                   /**< ROLE= */
  TidyAttr_ROWS,                   /**< ROWS= */
  TidyAttr_ROWSPAN,                /**< ROWSPAN= */
  TidyAttr_RULES,                  /**< RULES= */
  TidyAttr_SCHEME,                 /**< SCHEME= */
  TidyAttr_SCOPE,                  /**< SCOPE= */
  TidyAttr_SCROLLING,              /**< SCROLLING= */
  TidyAttr_SELECTED,               /**< SELECTED= */
  TidyAttr_SHAPE,                  /**< SHAPE= */
  TidyAttr_SHOWGRID,               /**< SHOWGRID= */
  TidyAttr_SHOWGRIDX,              /**< SHOWGRIDX= */
  TidyAttr_SHOWGRIDY,              /**< SHOWGRIDY= */
  TidyAttr_SIZE,                   /**< SIZE= */
  TidyAttr_SPAN,                   /**< SPAN= */
  TidyAttr_SRC,                    /**< SRC= */
  TidyAttr_SRCSET,                 /**< SRCSET= (HTML5) */
  TidyAttr_STANDBY,                /**< STANDBY= */
  TidyAttr_START,                  /**< START= */
  TidyAttr_STYLE,                  /**< STYLE= */
  TidyAttr_SUMMARY,                /**< SUMMARY= */
  TidyAttr_TABINDEX,               /**< TABINDEX= */
  TidyAttr_TARGET,                 /**< TARGET= */
  TidyAttr_TEXT,                   /**< TEXT= */
  TidyAttr_TITLE,                  /**< TITLE= */
  TidyAttr_TOPMARGIN,              /**< TOPMARGIN= */
  TidyAttr_TRANSLATE,              /**< TRANSLATE= */
  TidyAttr_TYPE,                   /**< TYPE= */
  TidyAttr_USEMAP,                 /**< USEMAP= */
  TidyAttr_VALIGN,                 /**< VALIGN= */
  TidyAttr_VALUE,                  /**< VALUE= */
  TidyAttr_VALUETYPE,              /**< VALUETYPE= */
  TidyAttr_VERSION,                /**< VERSION= */
  TidyAttr_VLINK,                  /**< VLINK= */
  TidyAttr_VSPACE,                 /**< VSPACE= */
  TidyAttr_WIDTH,                  /**< WIDTH= */
  TidyAttr_WRAP,                   /**< WRAP= */
  TidyAttr_XML_LANG,               /**< XML_LANG= */
  TidyAttr_XML_SPACE,              /**< XML_SPACE= */
  TidyAttr_XMLNS,                  /**< XMLNS= */

  TidyAttr_EVENT,                  /**< EVENT= */
  TidyAttr_METHODS,                /**< METHODS= */
  TidyAttr_N,                      /**< N= */
  TidyAttr_SDAFORM,                /**< SDAFORM= */
  TidyAttr_SDAPREF,                /**< SDAPREF= */
  TidyAttr_SDASUFF,                /**< SDASUFF= */
  TidyAttr_URN,                    /**< URN= */

  TidyAttr_ASYNC,                  /**< ASYNC= */
  TidyAttr_AUTOCOMPLETE,           /**< AUTOCOMPLETE= */
  TidyAttr_AUTOFOCUS,              /**< AUTOFOCUS= */
  TidyAttr_AUTOPLAY,               /**< AUTOPLAY= */
  TidyAttr_CHALLENGE,              /**< CHALLENGE= */
  TidyAttr_CONTENTEDITABLE,        /**< CONTENTEDITABLE= */
  TidyAttr_CONTEXTMENU,            /**< CONTEXTMENU= */
  TidyAttr_CONTROLS,               /**< CONTROLS= */
  TidyAttr_CROSSORIGIN,            /**< CROSSORIGIN= */
  TidyAttr_DEFAULT,                /**< DEFAULT= */
  TidyAttr_DIRNAME,                /**< DIRNAME= */
  TidyAttr_DRAGGABLE,              /**< DRAGGABLE= */
  TidyAttr_DROPZONE,               /**< DROPZONE= */
  TidyAttr_FORM,                   /**< FORM= */
  TidyAttr_FORMACTION,             /**< FORMACTION= */
  TidyAttr_FORMENCTYPE,            /**< FORMENCTYPE= */
  TidyAttr_FORMMETHOD,             /**< FORMMETHOD= */
  TidyAttr_FORMNOVALIDATE,         /**< FORMNOVALIDATE= */
  TidyAttr_FORMTARGET,             /**< FORMTARGET= */
  TidyAttr_HIDDEN,                 /**< HIDDEN= */
  TidyAttr_HIGH,                   /**< HIGH= */
  TidyAttr_ICON,                   /**< ICON= */
  TidyAttr_KEYTYPE,                /**< KEYTYPE= */
  TidyAttr_KIND,                   /**< KIND= */
  TidyAttr_LIST,                   /**< LIST= */
  TidyAttr_LOOP,                   /**< LOOP= */
  TidyAttr_LOW,                    /**< LOW= */
  TidyAttr_MANIFEST,               /**< MANIFEST= */
  TidyAttr_MAX,                    /**< MAX= */
  TidyAttr_MEDIAGROUP,             /**< MEDIAGROUP= */
  TidyAttr_MIN,                    /**< MIN= */
  TidyAttr_MUTED,                  /**< MUTED= */
  TidyAttr_NOVALIDATE,             /**< NOVALIDATE= */
  TidyAttr_OPEN,                   /**< OPEN= */
  TidyAttr_OPTIMUM,                /**< OPTIMUM= */
  TidyAttr_OnABORT,                /**< OnABORT= */
  TidyAttr_OnAFTERPRINT,           /**< OnAFTERPRINT= */
  TidyAttr_OnBEFOREPRINT,          /**< OnBEFOREPRINT= */
  TidyAttr_OnCANPLAY,              /**< OnCANPLAY= */
  TidyAttr_OnCANPLAYTHROUGH,       /**< OnCANPLAYTHROUGH= */
  TidyAttr_OnCONTEXTMENU,          /**< OnCONTEXTMENU= */
  TidyAttr_OnCUECHANGE,            /**< OnCUECHANGE= */
  TidyAttr_OnDRAG,                 /**< OnDRAG= */
  TidyAttr_OnDRAGEND,              /**< OnDRAGEND= */
  TidyAttr_OnDRAGENTER,            /**< OnDRAGENTER= */
  TidyAttr_OnDRAGLEAVE,            /**< OnDRAGLEAVE= */
  TidyAttr_OnDRAGOVER,             /**< OnDRAGOVER= */
  TidyAttr_OnDRAGSTART,            /**< OnDRAGSTART= */
  TidyAttr_OnDROP,                 /**< OnDROP= */
  TidyAttr_OnDURATIONCHANGE,       /**< OnDURATIONCHANGE= */
  TidyAttr_OnEMPTIED,              /**< OnEMPTIED= */
  TidyAttr_OnENDED,                /**< OnENDED= */
  TidyAttr_OnERROR,                /**< OnERROR= */
  TidyAttr_OnHASHCHANGE,           /**< OnHASHCHANGE= */
  TidyAttr_OnINPUT,                /**< OnINPUT= */
  TidyAttr_OnINVALID,              /**< OnINVALID= */
  TidyAttr_OnLOADEDDATA,           /**< OnLOADEDDATA= */
  TidyAttr_OnLOADEDMETADATA,       /**< OnLOADEDMETADATA= */
  TidyAttr_OnLOADSTART,            /**< OnLOADSTART= */
  TidyAttr_OnMESSAGE,              /**< OnMESSAGE= */
  TidyAttr_OnMOUSEWHEEL,           /**< OnMOUSEWHEEL= */
  TidyAttr_OnOFFLINE,              /**< OnOFFLINE= */
  TidyAttr_OnONLINE,               /**< OnONLINE= */
  TidyAttr_OnPAGEHIDE,             /**< OnPAGEHIDE= */
  TidyAttr_OnPAGESHOW,             /**< OnPAGESHOW= */
  TidyAttr_OnPAUSE,                /**< OnPAUSE= */
  TidyAttr_OnPLAY,                 /**< OnPLAY= */
  TidyAttr_OnPLAYING,              /**< OnPLAYING= */
  TidyAttr_OnPOPSTATE,             /**< OnPOPSTATE= */
  TidyAttr_OnPROGRESS,             /**< OnPROGRESS= */
  TidyAttr_OnRATECHANGE,           /**< OnRATECHANGE= */
  TidyAttr_OnREADYSTATECHANGE,     /**< OnREADYSTATECHANGE= */
  TidyAttr_OnREDO,                 /**< OnREDO= */
  TidyAttr_OnRESIZE,               /**< OnRESIZE= */
  TidyAttr_OnSCROLL,               /**< OnSCROLL= */
  TidyAttr_OnSEEKED,               /**< OnSEEKED= */
  TidyAttr_OnSEEKING,              /**< OnSEEKING= */
  TidyAttr_OnSHOW,                 /**< OnSHOW= */
  TidyAttr_OnSTALLED,              /**< OnSTALLED= */
  TidyAttr_OnSTORAGE,              /**< OnSTORAGE= */
  TidyAttr_OnSUSPEND,              /**< OnSUSPEND= */
  TidyAttr_OnTIMEUPDATE,           /**< OnTIMEUPDATE= */
  TidyAttr_OnUNDO,                 /**< OnUNDO= */
  TidyAttr_OnVOLUMECHANGE,         /**< OnVOLUMECHANGE= */
  TidyAttr_OnWAITING,              /**< OnWAITING= */
  TidyAttr_PATTERN,                /**< PATTERN= */
  TidyAttr_PLACEHOLDER,            /**< PLACEHOLDER= */
  TidyAttr_PLAYSINLINE,            /**< PLAYSINLINE= */
  TidyAttr_POSTER,                 /**< POSTER= */
  TidyAttr_PRELOAD,                /**< PRELOAD= */
  TidyAttr_PUBDATE,                /**< PUBDATE= */
  TidyAttr_RADIOGROUP,             /**< RADIOGROUP= */
  TidyAttr_REQUIRED,               /**< REQUIRED= */
  TidyAttr_REVERSED,               /**< REVERSED= */
  TidyAttr_SANDBOX,                /**< SANDBOX= */
  TidyAttr_SCOPED,                 /**< SCOPED= */
  TidyAttr_SEAMLESS,               /**< SEAMLESS= */
  TidyAttr_SIZES,                  /**< SIZES= */
  TidyAttr_SPELLCHECK,             /**< SPELLCHECK= */
  TidyAttr_SRCDOC,                 /**< SRCDOC= */
  TidyAttr_SRCLANG,                /**< SRCLANG= */
  TidyAttr_STEP,                   /**< STEP= */
  TidyAttr_ARIA_ACTIVEDESCENDANT,  /**< ARIA_ACTIVEDESCENDANT */
  TidyAttr_ARIA_ATOMIC,            /**< ARIA_ATOMIC= */
  TidyAttr_ARIA_AUTOCOMPLETE,      /**< ARIA_AUTOCOMPLETE= */
  TidyAttr_ARIA_BUSY,              /**< ARIA_BUSY= */
  TidyAttr_ARIA_CHECKED,           /**< ARIA_CHECKED= */
  TidyAttr_ARIA_CONTROLS,          /**< ARIA_CONTROLS= */
  TidyAttr_ARIA_DESCRIBEDBY,       /**< ARIA_DESCRIBEDBY= */
  TidyAttr_ARIA_DISABLED,          /**< ARIA_DISABLED= */
  TidyAttr_ARIA_DROPEFFECT,        /**< ARIA_DROPEFFECT= */
  TidyAttr_ARIA_EXPANDED,          /**< ARIA_EXPANDED= */
  TidyAttr_ARIA_FLOWTO,            /**< ARIA_FLOWTO= */
  TidyAttr_ARIA_GRABBED,           /**< ARIA_GRABBED= */
  TidyAttr_ARIA_HASPOPUP,          /**< ARIA_HASPOPUP= */
  TidyAttr_ARIA_HIDDEN,            /**< ARIA_HIDDEN= */
  TidyAttr_ARIA_INVALID,           /**< ARIA_INVALID= */
  TidyAttr_ARIA_LABEL,             /**< ARIA_LABEL= */
  TidyAttr_ARIA_LABELLEDBY,        /**< ARIA_LABELLEDBY= */
  TidyAttr_ARIA_LEVEL,             /**< ARIA_LEVEL= */
  TidyAttr_ARIA_LIVE,              /**< ARIA_LIVE= */
  TidyAttr_ARIA_MULTILINE,         /**< ARIA_MULTILINE= */
  TidyAttr_ARIA_MULTISELECTABLE,   /**< ARIA_MULTISELECTABLE= */
  TidyAttr_ARIA_ORIENTATION,       /**< ARIA_ORIENTATION= */
  TidyAttr_ARIA_OWNS,              /**< ARIA_OWNS= */
  TidyAttr_ARIA_POSINSET,          /**< ARIA_POSINSET= */
  TidyAttr_ARIA_PRESSED,           /**< ARIA_PRESSED= */
  TidyAttr_ARIA_READONLY,          /**< ARIA_READONLY= */
  TidyAttr_ARIA_RELEVANT,          /**< ARIA_RELEVANT= */
  TidyAttr_ARIA_REQUIRED,          /**< ARIA_REQUIRED= */
  TidyAttr_ARIA_SELECTED,          /**< ARIA_SELECTED= */
  TidyAttr_ARIA_SETSIZE,           /**< ARIA_SETSIZE= */
  TidyAttr_ARIA_SORT,              /**< ARIA_SORT= */
  TidyAttr_ARIA_VALUEMAX,          /**< ARIA_VALUEMAX= */
  TidyAttr_ARIA_VALUEMIN,          /**< ARIA_VALUEMIN= */
  TidyAttr_ARIA_VALUENOW,          /**< ARIA_VALUENOW= */
  TidyAttr_ARIA_VALUETEXT,         /**< ARIA_VALUETEXT= */

  /* SVG attributes (SVG 1.1) */
  TidyAttr_X,                      /**< X= */
  TidyAttr_Y,                      /**< Y= */
  TidyAttr_VIEWBOX,                /**< VIEWBOX= */
  TidyAttr_PRESERVEASPECTRATIO,    /**< PRESERVEASPECTRATIO= */
  TidyAttr_ZOOMANDPAN,             /**< ZOOMANDPAN= */
  TidyAttr_BASEPROFILE,            /**< BASEPROFILE= */
  TidyAttr_CONTENTSCRIPTTYPE,      /**< CONTENTSCRIPTTYPE= */
  TidyAttr_CONTENTSTYLETYPE,       /**< CONTENTSTYLETYPE= */

  /* MathML <math> attributes */
  TidyAttr_DISPLAY,                /**< DISPLAY= (html5) */

  /* RDFa global attributes */
  TidyAttr_ABOUT,                  /**< ABOUT= */
  TidyAttr_DATATYPE,               /**< DATATYPE= */
  TidyAttr_INLIST,                 /**< INLIST= */
  TidyAttr_PREFIX,                 /**< PREFIX= */
  TidyAttr_PROPERTY,               /**< PROPERTY= */
  TidyAttr_RESOURCE,               /**< RESOURCE= */
  TidyAttr_TYPEOF,                 /**< TYPEOF= */
  TidyAttr_VOCAB,                  /**< VOCAB= */

  TidyAttr_INTEGRITY,              /**< INTEGRITY= */

  TidyAttr_AS,                     /**< AS= */

  TidyAttr_XMLNSXLINK,             /**< svg xmls:xlink="url" */
  TidyAttr_SLOT,                   /**< SLOT= */
  TidyAttr_LOADING,                /**< LOADING= */

  /* SVG paint attributes (SVG 1.1) */
  TidyAttr_FILL,                   /**< FILL= */
  TidyAttr_FILLRULE,               /**< FILLRULE= */
  TidyAttr_STROKE,                 /**< STROKE= */
  TidyAttr_STROKEDASHARRAY,        /**< STROKEDASHARRAY= */
  TidyAttr_STROKEDASHOFFSET,       /**< STROKEDASHOFFSET= */
  TidyAttr_STROKELINECAP,          /**< STROKELINECAP= */
  TidyAttr_STROKELINEJOIN,         /**< STROKELINEJOIN= */
  TidyAttr_STROKEMITERLIMIT,       /**< STROKEMITERLIMIT= */
  TidyAttr_STROKEWIDTH,            /**< STROKEWIDTH= */
  TidyAttr_COLORINTERPOLATION,     /**< COLORINTERPOLATION= */
  TidyAttr_COLORRENDERING,         /**< COLORRENDERING= */
  TidyAttr_OPACITY,                /**< OPACITY= */
  TidyAttr_STROKEOPACITY,          /**< STROKEOPACITY= */
  TidyAttr_FILLOPACITY,            /**< FILLOPACITY= */

  N_TIDY_ATTRIBS                   /**< Must be last */
} TidyAttrId;


/** @}
 ** @name I/O and Message Handling Interface
 **
 ** Messages used throughout LibTidy and exposed to the public API have
 ** attributes which are communicated with these enumerations.
 **
 ** @{
 */


/** Message severity level, used throughout LibTidy to indicate the severity
 ** or status of a message
 **
 ** @remark These enum members all have associated localized strings available
 **         via their enum values. These strings are suitable for use as labels.
 */
typedef enum
{
    TidyInfo = 350,         /**< Report: Information about markup usage */
    TidyWarning,            /**< Report: Warning message */
    TidyConfig,             /**< Report: Configuration error */
    TidyAccess,             /**< Report: Accessibility message */
    TidyError,              /**< Report: Error message - output suppressed */
    TidyBadDocument,        /**< Report: I/O or file system error */
    TidyFatal,              /**< Report: Crash! */
    TidyDialogueSummary,    /**< Dialogue: Summary-related information */
    TidyDialogueInfo,       /**< Dialogue: Non-document related information */
    TidyDialogueFootnote,   /**< Dialogue: Footnote */
    TidyDialogueDoc = TidyDialogueFootnote, /**< Dialogue: Deprecated (renamed) */
} TidyReportLevel;


/** Indicates the data type of a format string parameter used when Tidy
 ** emits reports and dialogue as part of the messaging callback functions.
 ** See `messageobj.h` for more information on this API.
 */
typedef enum
{
    tidyFormatType_INT = 0,         /**< Argument is signed integer. */
    tidyFormatType_UINT,            /**< Argument is unsigned integer. */
    tidyFormatType_STRING,          /**< Argument is a string. */
    tidyFormatType_DOUBLE,          /**< Argument is a double. */
    tidyFormatType_UNKNOWN  = 20    /**< Argument type is unknown! */
} TidyFormatParameterType;


/** @} */
/** @} end group public_enumerations*/


/* MARK: - Public Enumerations (con't) */
/** @addtogroup public_enumerations
 ** @{ */

/** @name Messages
 ** @{ */

/** The enumeration contains a list of every possible string that Tidy and the
 ** console application can output, _except_ for strings from the following
 ** enumerations:
 ** - `TidyOptionId`
 ** - `TidyConfigCategory`
 ** - `TidyReportLevel`
 **
 ** They are used as keys internally within Tidy, and have corresponding text
 ** keys that are used in message callback filters (these are defined in
 ** `tidyStringsKeys[]`, but API users don't require access to it directly).
 */
typedef enum
{
    /* This MUST be present and first. */
    TIDYSTRINGS_FIRST = 500,

    FOREACH_MSG_MISC(MAKE_ENUM)
    FOREACH_FOOTNOTE_MSG(MAKE_ENUM)
    FOREACH_DIALOG_MSG(MAKE_ENUM)
    REPORT_MESSAGE_FIRST,
    FOREACH_REPORT_MSG(MAKE_ENUM)
    REPORT_MESSAGE_LAST,
    FOREACH_ACCESS_MSG(MAKE_ENUM)

#if SUPPORT_CONSOLE_APP
    FOREACH_MSG_CONSOLE(MAKE_ENUM)
#endif

    /* This MUST be present and last. */
    TIDYSTRINGS_LAST

} tidyStrings;


/** @} */
/** @} end group public_enumerations */


#ifdef __cplusplus
}  /* extern "C" */
#endif
#endif /* __TIDYENUM_H__ */

