#ifndef messageobj_h
#define messageobj_h

/**************************************************************************//**
 * @file
 * Provides an external, extensible API for message reporting.
 *
 * This module implements the `_TidyMessageImpl` structure (declared in
 * `tidy-int.h`) in order to abstract the reporting of reports and dialogue
 * from the rest of Tidy, and to enable a robust and extensible API for
 * message interrogation by LibTidy users.
 *
 * @author    Jim Derry
 * @copyright Copyright (c) 2017 HTACG. See tidy.h for license.
 * @date      Created 2017-March-10
 *
 ******************************************************************************/

#include "third_party/tidy/forward.h"

/** @addtogroup internal_api */
/** @{ */


/** @defgroup messageobj_instantiation Message Creation and Releasing */
/** @{ */


/** Creates a TidyMessageImpl, but without line numbers, such as used for
 ** information report output.
 */
TidyMessageImpl *TY_(tidyMessageCreate)( TidyDocImpl *doc,
                                         uint code,
                                         TidyReportLevel level,
                                         ... );

/** Creates a TidyMessageImpl, using the line and column from the provided
 ** Node as the message position source.
 */
TidyMessageImpl *TY_(tidyMessageCreateWithNode)( TidyDocImpl *doc,
                                                 Node *node,
                                                 uint code,
                                                 TidyReportLevel level,
                                                 ... );

/** Creates a TidyMessageImpl, using the line and column from the provided
 ** document's Lexer as the message position source.
 */
TidyMessageImpl *TY_(tidyMessageCreateWithLexer)( TidyDocImpl *doc,
                                                  uint code,
                                                  TidyReportLevel level,
                                                  ... );

/** Deallocates a TidyMessageImpl in order to free up its allocated memory
 ** when you're done using it.
 */
void TY_(tidyMessageRelease)( TidyMessageImpl *message );


/** @} end messageobj_instantiation group */
/** @defgroup messageobj_message_api Report and Dialogue API */
/** @{ */


/** get the document the message came from. */
TidyDocImpl* TY_(getMessageDoc)( TidyMessageImpl message );

/** get the message key code. */
uint TY_(getMessageCode)( TidyMessageImpl message );

/** get the message key string. */
ctmbstr TY_(getMessageKey)( TidyMessageImpl message );

/** get the line number the message applies to. */
int TY_(getMessageLine)( TidyMessageImpl message );

/** get the column the message applies to. */
int TY_(getMessageColumn)( TidyMessageImpl message );

/** get the TidyReportLevel of the message. */
TidyReportLevel TY_(getMessageLevel)( TidyMessageImpl message );

/** get whether or not the message was muted by the configuration. */
Bool TY_(getMessageIsMuted)( TidyMessageImpl message );

/** the built-in format string */
ctmbstr TY_(getMessageFormatDefault)( TidyMessageImpl message );

/** the localized format string */
ctmbstr TY_(getMessageFormat)( TidyMessageImpl message );

/** the message, formatted, default language */
ctmbstr TY_(getMessageDefault)( TidyMessageImpl message );

/** the message, formatted, localized */
ctmbstr TY_(getMessage)( TidyMessageImpl message );

/** the position part, default language */
ctmbstr TY_(getMessagePosDefault)( TidyMessageImpl message );

/** the position part, localized */
ctmbstr TY_(getMessagePos)( TidyMessageImpl message );

/** the prefix part, default language */
ctmbstr TY_(getMessagePrefixDefault)( TidyMessageImpl message );

/** the prefix part, localized */
ctmbstr TY_(getMessagePrefix)( TidyMessageImpl message );

/** the complete message, as would be output in the CLI */
ctmbstr TY_(getMessageOutputDefault)( TidyMessageImpl message );

/* the complete message, as would be output in the CLI, localized */
ctmbstr TY_(getMessageOutput)( TidyMessageImpl message );


/** @} end messageobj_message_api group */
/** @defgroup messageobj_args_api Report Arguments Interrogation API */
/** @{ */

/**
 *  Initializes the TidyIterator to point to the first item in the message's
 *  argument. Use `TY_(getNextMEssageArgument)` to get an opaque instance of
 *  `TidyMessageArgument` for which the subsequent interrogators will be of use.
 */
TidyIterator TY_(getMessageArguments)( TidyMessageImpl message );

/**
 *  Returns the next `TidyMessageArgument`, for the given message, which can
 *  then be interrogated with the API, and advances the iterator.
 */
TidyMessageArgument TY_(getNextMessageArgument)( TidyMessageImpl message, TidyIterator* iter );


/**
 *  Returns the `TidyFormatParameterType` of the given message argument.
 */
TidyFormatParameterType TY_(getArgType)( TidyMessageImpl message, TidyMessageArgument* arg );


/**
 *  Returns the format specifier of the given message argument. The memory for
 *  this string is cleared upon termination of the callback, so do be sure to
 *  make your own copy.
 */
ctmbstr TY_(getArgFormat)( TidyMessageImpl message, TidyMessageArgument* arg );


/**
 *  Returns the string value of the given message argument. An assertion
 *  will be generated if the argument type is not a string.
 */
ctmbstr TY_(getArgValueString)( TidyMessageImpl message, TidyMessageArgument* arg );


/**
 *  Returns the unsigned integer value of the given message argument. An
 *  assertion will be generated if the argument type is not an unsigned
 *  integer.
 */
uint TY_(getArgValueUInt)( TidyMessageImpl message, TidyMessageArgument* arg );


/**
 *  Returns the integer value of the given message argument. An assertion
 *  will be generated if the argument type is not an integer.
 */
int TY_(getArgValueInt)( TidyMessageImpl message, TidyMessageArgument* arg );


/**
 *  Returns the double value of the given message argument. An assertion
 *  will be generated if the argument type is not a double.
 */
double TY_(getArgValueDouble)( TidyMessageImpl message, TidyMessageArgument* arg );


/** @} end messageobj_args_api group */
/** @} end internal_api group */

#endif /* messageobj_h */
