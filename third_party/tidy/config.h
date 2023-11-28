#ifndef __CONFIG_H__
#define __CONFIG_H__

/**************************************************************************//**
 * @file
 * Read configuration files and manage configuration properties.
 *
 * Config files associate a property name with a value.
 *
 * // comments can start at the beginning of a line
 * # comments can start at the beginning of a line
 * name: short values fit onto one line
 * name: a really long value that
 *  continues on the next line
 *
 * Property names are case insensitive and should be less than 60 characters
 * in length, and must start at the beginning of the line, as whitespace at
 * the start of a line signifies a line continuation.
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
#include "third_party/tidy/tidy.h"
#include "third_party/tidy/streamio.h"

/** @addtogroup internal_api */
/** @{ */


/***************************************************************************//**
 ** @defgroup configuration_options Configuration Options
 **
 ** This module organizes all of Tidy's configuration options, including
 ** picklist management, option setting and retrieval, option file utilities,
 ** and so on.
 **
 ** @{
 ******************************************************************************/


/** Determines the maximum number of items in an option's picklist. PickLists
 ** may have up to 16 items. For some reason, this limit has been hard-coded
 ** into Tidy for some time. Feel free to increase this as needed.
 */
#define TIDY_PL_SIZE 16


/** Structs of this type contain information needed in order to present
 ** picklists, relate picklist entries to public enum values, and parse
 ** strings that are accepted in order to assign the value.
 */
typedef struct PickListItem {
    ctmbstr label;      /**< PickList label for this item. */
    const int value;    /**< The option value represented by this label. */
    ctmbstr inputs[10]; /**< String values that can select this value. */
} PickListItem;


/** An array of PickListItems, fixed in size for in-code declarations.
 ** Arrays must be populated in 0 to 10 order, as the option value is assigned
 ** based on this index and *not* on the structures' value field. It remains
 ** a best practice, however, to assign a public enum value with the proper
 ** index value.
 */
typedef const PickListItem PickListItems[TIDY_PL_SIZE];


struct _tidy_option; /* forward */

/** The TidyOptionImpl type implements the `_tidy_option` structure.
 */
typedef struct _tidy_option TidyOptionImpl;


/** This typedef describes a function that is used for parsing the input
 ** given for a particular Tidy option.
 */
typedef Bool (ParseProperty)( TidyDocImpl* doc, const TidyOptionImpl* opt );


/** This structure defines the internal representation of a Tidy option.
 */
struct _tidy_option
{
    TidyOptionId        id;              /**< The unique identifier for this option. */
    TidyConfigCategory  category;        /**< The category of the option. */
    ctmbstr             name;            /**< The name of the option. */
    TidyOptionType      type;            /**< The date type for the option. */
    ulong               dflt;            /**< Default value for TidyInteger and TidyBoolean */
    ParseProperty*      parser;          /**< Function to parse input; read-only if NULL. */
    PickListItems*      pickList;        /**< The picklist of possible values for this option. */
    ctmbstr             pdflt;           /**< Default value for TidyString. */
};


/** Stored option values can be one of two internal types.
 */
typedef union
{
  ulong v;  /**< Value for TidyInteger and TidyBoolean */
  char *p;  /**< Value for TidyString */
} TidyOptionValue;


/** This type is used to define a structure for keeping track of the values
 ** for each option.
 */
typedef struct _tidy_config
{
    TidyOptionValue value[ N_TIDY_OPTIONS + 1 ];     /**< Current config values. */
    TidyOptionValue snapshot[ N_TIDY_OPTIONS + 1 ];  /**< Snapshot of values to be restored later. */
    uint  defined_tags;                              /**< Tracks user-defined tags. */
    uint c;                                          /**< Current char in input stream for reading options. */
    StreamIn* cfgIn;                                 /**< Current input source for reading options.*/
} TidyConfigImpl;


/** Used to build a table of documentation cross-references.
 */
typedef struct {
  TidyOptionId opt;          /**< Identifier. */
  TidyOptionId const *links; /**< Cross references. Last element must be 'TidyUnknownOption'. */
} TidyOptionDoc;


/** Given an option name, return an instance of an option.
 ** @param optnam The option name to retrieve.
 ** @returns The instance of the requested option.
 */
const TidyOptionImpl* TY_(lookupOption)( ctmbstr optnam );


/** Given an option ID, return an instance of an option.
 ** @param optId The option ID to retrieve.
 ** @returns The instance of the requested option.
 */
const TidyOptionImpl* TY_(getOption)( TidyOptionId optId );

/** Given an option ID, indicates whether or not the option is a list.
 ** @param optId The option ID to check.
 ** @returns Returns yes if the option value is a list.
 */
const Bool TY_(getOptionIsList)( TidyOptionId optId );

/** Initiates an iterator to cycle through all of the available options.
 ** @param doc The Tidy document to get options.
 ** @returns An iterator token to be used with TY_(getNextOption)().
 */
TidyIterator TY_(getOptionList)( TidyDocImpl* doc );


/** Gets the next option provided by the iterator.
 ** @param doc The Tidy document to get options.
 ** @param iter The iterator token initialized by TY_(getOptionList)().
 ** @returns The instance of the next option.
 */
const TidyOptionImpl* TY_(getNextOption)( TidyDocImpl* doc, TidyIterator* iter );


/** Initiates an iterator to cycle through all of the available picklist
 ** possibilities.
 ** @param option An instance of an option for which to iterate a picklist.
 ** @returns An iterator token to be used with TY_(getNextOptionPick)().
 */
TidyIterator TY_(getOptionPickList)( const TidyOptionImpl* option );


/** Gets the next picklist possibility provided by the iterator.
 ** @param option The instance of the option for which to iterate a picklist.
 ** @param iter The iterator token initialized by TY_(getOptionPickList)().
 ** @returns The next picklist entry.
 */
ctmbstr TY_(getNextOptionPick)( const TidyOptionImpl* option, TidyIterator* iter );


#if SUPPORT_CONSOLE_APP
/** Returns the cross-reference information structure for optID, which is
 ** used for generating documentation.
 ** @param optId The option ID to get cross-reference information for.
 ** @returns Cross reference information.
 */
const TidyOptionDoc* TY_(OptGetDocDesc)( TidyOptionId optId );
#endif /* SUPPORT_CONSOLE_APP */


/** Initialize the configuration for the given Tidy document.
 ** @param doc The Tidy document.
 */
void TY_(InitConfig)( TidyDocImpl* doc );


/** Frees the configuration memory for the given Tidy document.
 ** @param doc The Tidy document.
 */
void TY_(FreeConfig)( TidyDocImpl* doc );


/** Gets the picklist label for a given value.
 ** @param optId the option id having a picklist to check.
 ** @param pick the picklist item to retrieve.
 ** @returns The label for the pick.
 */
ctmbstr TY_(GetPickListLabelForPick)( TidyOptionId optId, uint pick );


/** Sets the integer value for the given option Id.
 ** @param doc The Tidy document.
 ** @param optId The option ID to set.
 ** @param val The value to set.
 ** @returns Success or failure.
 */
Bool TY_(SetOptionInt)( TidyDocImpl* doc, TidyOptionId optId, ulong val );


/** Sets the bool value for the given option Id.
 ** @param doc The Tidy document.
 ** @param optId The option ID to set.
 ** @param val The value to set.
 ** @returns Success or failure.
 */
Bool TY_(SetOptionBool)( TidyDocImpl* doc, TidyOptionId optId, Bool val );


/** Resets the given option to its default value.
 ** @param doc The Tidy document.
 ** @param optId The option ID to set.
 ** @returns Success or failure.
 */
Bool TY_(ResetOptionToDefault)( TidyDocImpl* doc, TidyOptionId optId );


/** Resets all options in the document to their default values.
 ** @param doc The Tidy document.
 */
void TY_(ResetConfigToDefault)( TidyDocImpl* doc );


/** Stores a snapshot of all of the configuration values that can be
 ** restored later.
 ** @param doc The Tidy document.
 */
void TY_(TakeConfigSnapshot)( TidyDocImpl* doc );


/** Restores all of the configuration values to their snapshotted values.
 ** @param doc The Tidy document.
 */
void TY_(ResetConfigToSnapshot)( TidyDocImpl* doc );


/** Copies the configuration from one document to another.
 ** @param docTo The destination Tidy document.
 ** @param docFrom The source Tidy document.
 */
void TY_(CopyConfig)( TidyDocImpl* docTo, TidyDocImpl* docFrom );


/** Attempts to parse the given config file into the document.
 ** @param doc The Tidy document.
 ** @param cfgfil The file to load.
 ** @returns a file system error code.
 */
int  TY_(ParseConfigFile)( TidyDocImpl* doc, ctmbstr cfgfil );


/** Attempts to parse the given config file into the document, using
 ** the provided encoding.
 ** @param doc The Tidy document.
 ** @param cfgfil The file to load.
 ** @param charenc The name of the encoding to use for reading the file.
 ** @returns a file system error code.
 */
int  TY_(ParseConfigFileEnc)( TidyDocImpl* doc,
                              ctmbstr cfgfil, ctmbstr charenc );


/** Saves the current configuration for options not having default values
 ** into the specified file.
 ** @param doc The Tidy document.
 ** @param cfgfil The file to save.
 ** @returns a file system error code.
 */
int  TY_(SaveConfigFile)( TidyDocImpl* doc, ctmbstr cfgfil );


/** Writes the current configuration for options not having default values
 ** into the specified sink.
 ** @param doc The Tidy document.
 ** @param sink The sink to save into.
 ** @returns a file system error code.
 */
int  TY_(SaveConfigSink)( TidyDocImpl* doc, TidyOutputSink* sink );


/** Attempts to parse the provided value for the given option name. Returns
 ** false if unknown option, missing parameter, or the option doesn't
 ** use the parameter.
 ** @param doc The Tidy document.
 ** @param optnam The name of the option to be set.
 ** @param optVal The string value to attempt to parse.
 ** @returns Success or failure.
 */
Bool  TY_(ParseConfigOption)( TidyDocImpl* doc, ctmbstr optnam, ctmbstr optVal );


/** Attempts to parse the provided value for the given option id. Returns
 ** false if unknown option, missing parameter, or the option doesn't
 ** use the parameter.
 ** @param doc The Tidy document.
 ** @param optId The ID of the option to be set.
 ** @param optVal The string value to attempt to parse.
 ** @returns Success or failure.
 */
Bool  TY_(ParseConfigValue)( TidyDocImpl* doc, TidyOptionId optId, ctmbstr optVal );


/** Ensure that char encodings are self consistent.
 ** @param doc The Tidy document to adjust.
 ** @param encoding The encoding being applied.
 ** @returns A bool indicating success or failure.
 */
Bool  TY_(AdjustCharEncoding)( TidyDocImpl* doc, int encoding );


/** Ensure that the configuration options are self consistent.
 ** THIS PROCESS IS DESTRUCTIVE TO THE USER STATE. It examines
 ** certain user-specified options and changes other options
 ** as a result. This means that documented API functions such
 ** as tidyOptGetValue() won't return the user-set values after
 ** this is used. As a result, *don't* just use this function
 ** at every opportunity, but only where needed, which is ONLY
 ** prior to parsing a stream, and again prior to saving a
 ** stream (because we reset after parsing.)
 ** @param doc The Tidy document to adjust.
 */
void  TY_(AdjustConfig)( TidyDocImpl* doc );


/** Indicates whether or not the current configuration is completely default.
 ** @param doc The Tidy document.
 ** @returns The result.
 */
Bool  TY_(ConfigDiffThanDefault)( TidyDocImpl* doc );


/** Indicates whether or not the current configuration is different from the
 ** stored snapshot.
 ** @param doc The Tidy document.
 ** @returns The result.
 */
Bool  TY_(ConfigDiffThanSnapshot)( TidyDocImpl* doc );


/** Returns the character encoding ID for the given character encoding
 ** string.
 ** @param doc The Tidy document.
 ** @param charenc The name of the character encoding.
 ** @returns The Id of the character encoding.
 */
int TY_(CharEncodingId)( TidyDocImpl* doc, ctmbstr charenc );


/** Returns the full name of the encoding for the given ID.
 ** @param encoding The Id of the encoding.
 ** @returns The name of the character encoding.
 */
ctmbstr TY_(CharEncodingName)( int encoding );


/** Returns the Tidy command line option name of the encoding for the given ID.
 ** @param encoding The Id of the encoding.
 ** @returns The Tidy command line option representing the encoding.
 */
ctmbstr TY_(CharEncodingOptName)( int encoding );


/** Coordinates Config update and list data.
 ** @param doc The Tidy document.
 ** @param opt The option the list item is intended for.
 ** @param name The name of the new list item.
 */
void TY_(DeclareListItem)( TidyDocImpl* doc, const TidyOptionImpl* opt, ctmbstr name );

#ifdef _DEBUG

/* Debug lookup functions will be type-safe and assert option type match */
ulong   TY_(_cfgGet)( TidyDocImpl* doc, TidyOptionId optId );
Bool    TY_(_cfgGetBool)( TidyDocImpl* doc, TidyOptionId optId );
TidyTriState TY_(_cfgGetAutoBool)( TidyDocImpl* doc, TidyOptionId optId );
ctmbstr TY_(_cfgGetString)( TidyDocImpl* doc, TidyOptionId optId );

#define cfg(doc, id)            TY_(_cfgGet)( (doc), (id) )
#define cfgBool(doc, id)        TY_(_cfgGetBool)( (doc), (id) )
#define cfgAutoBool(doc, id)    TY_(_cfgGetAutoBool)( (doc), (id) )
#define cfgStr(doc, id)         TY_(_cfgGetString)( (doc), (id) )

#else

/* Release build macros for speed */

/** Access the raw, non-string uint value of the given option ID. */
#define cfg(doc, id)            ((doc)->config.value[ (id) ].v)

/** Access the Bool value of the given option ID. */
#define cfgBool(doc, id)        ((Bool) cfg(doc, id))

/** Access the TidyTriState value of the given option ID. */
#define cfgAutoBool(doc, id)    ((TidyTriState) cfg(doc, id))

/** Access the string value of the given option ID. */
#define cfgStr(doc, id)         ((ctmbstr) (doc)->config.value[ (id) ].p)

#endif /* _DEBUG */


/** @} configuration_options group */
/** @} internal_api addtogroup */


#endif /* __CONFIG_H__ */
