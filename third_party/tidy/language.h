#ifndef language_h
#define language_h

/*********************************************************************
 * Localization support for HTML Tidy.
 *
 * This header provides the public (within libtidy) interface to
 * basic localization support. To add your own localization, create
 * a new `language_xx.h` file and add it to the struct in
 * `language.c`.
 *
 * (c) 2015 HTACG
 * See `tidy.h` for the copyright notice.
 *********************************************************************/

#include "third_party/tidy/forward.h"


/** @name Exposed Data Structures */
/** @{ */


/**
 *  These enumerations are used within instances of `languageDefinition`
 *  structures to provide additional metadata, and are localizable
 *  therein.
 */
typedef enum {

    /* Specifies the language code for a particular language. */
    TIDY_LANGUAGE = 400,

    /* Marker for the last key in the structure. */
    TIDY_MESSAGE_TYPE_LAST

} tidyLanguage;


/**
 *  Describes a record for a localization string.
 *  - key must correspond with one of Tidy's enums (see `tidyMessageTypes`
 *    below)
 *  - pluralForm corresponds to gettext plural forms case (not singularity).
 *    Most entries should be case 0, representing the single case.:
 *    https://www.gnu.org/software/gettext/manual/html_node/Plural-forms.html
 */
typedef struct languageDictionaryEntry {
    uint key;
    uint pluralForm;
    ctmbstr value;
} languageDictionaryEntry;


/**
 *  For now we'll just use an array to hold all of the dictionary
 *  entries. In the future we can convert this to a hash structure
 *  which will make looking up strings faster.
 */
typedef languageDictionaryEntry const languageDictionary[600];


/**
 *  Finally, a complete language definition. The item `pluralForm`
 *  is a function pointer that will provide the correct plural
 *  form given the value `n`. The actual function is present in
 *  each language header and is language dependent.
 */
typedef struct languageDefinition {
    uint (*whichPluralForm)(uint n);
    languageDictionary messages;
} languageDefinition;


/**
 *  The function getNextWindowsLanguage() returns pointers to this type;
 *  it gives LibTidy implementors the ability to determine how Windows
 *  locale names are mapped to POSIX language codes.
 */
typedef struct tidyLocaleMapItemImpl {
    ctmbstr winName;
    ctmbstr POSIXName;
} tidyLocaleMapItemImpl;


/** @} */
/** @name Localization Related Functions */
/** @{ */


/**
 **  Determines the current locale without affecting the C locale.
 **  Tidy has always used the default C locale, and at this point
 **  in its development we're not going to tamper with that.
 **  @param  result The buffer to use to return the string.
 **          Returns NULL on failure.
 **  @return The same buffer for convenience.
 */
tmbstr TY_(tidySystemLocale)(tmbstr result);

/**
 *  Tells Tidy to use a different language for output.
 *  @param  languageCode A Windows or POSIX language code, and must match
 *          a TIDY_LANGUAGE for an installed language.
 *  @result Indicates that a setting was applied, but not necessarily the
 *          specific request, i.e., true indicates a language and/or region
 *          was applied. If es_mx is requested but not installed, and es is
 *          installed, then es will be selected and this function will return
 *          true. However the opposite is not true; if es is requested but
 *          not present, Tidy will not try to select from the es_XX variants.
 */
Bool TY_(tidySetLanguage)( ctmbstr languageCode );

/**
 *  Gets the current language used by Tidy.
 */
ctmbstr TY_(tidyGetLanguage)(void);


/**
 *  Indicates whether or not the current language was set by a
 *  LibTidy user or internally by the library. This flag prevents
 *  subsequently created instances of TidyDocument from changing the
 *  user's language.
 *  @returns Returns yes to indicate that the current language was
 *    specified by an API user.
 */
Bool TY_(tidyGetLanguageSetByUser)(void);


/**
 *  Specifies to LibTidy that the user (rather than the library)
 *  selected the current language. This flag prevents subsequently
 *  created instances of TidyDocument from changing the user's language.
 */
void TY_(tidySetLanguageSetByUser)( void );


/**
 *  Provides a string given `messageType` in the current
 *  localization for `quantity`.
 */
ctmbstr TY_(tidyLocalizedStringN)( uint messageType, uint quantity );

/**
 *  Provides a string given `messageType` in the current
 *  localization for the single case.
 */
ctmbstr TY_(tidyLocalizedString)( uint messageType );


/** @} */
/** @name Documentation Generation */
/** @{ */


/**
 *  Provides a string given `messageType` in the default
 *  localization (which is `en`), for the given quantity.
 */
ctmbstr TY_(tidyDefaultStringN)( uint messageType, uint quantity );

/**
 *  Provides a string given `messageType` in the default
 *  localization (which is `en`).
 */
ctmbstr TY_(tidyDefaultString)( uint messageType );

/*
 *  Initializes the TidyIterator to point to the first item
 *  in Tidy's list of localization string keys. Note that
 *  these are provided for documentation generation purposes
 *  and probably aren't useful for LibTidy implementors.
 */
TidyIterator TY_(getStringKeyList)(void);

/*
 *  Provides the next key value in Tidy's list of localized
 *  strings. Note that these are provided for documentation
 *  generation purposes and probably aren't useful to
 *  libtidy implementors.
 */
uint TY_(getNextStringKey)( TidyIterator* iter );

/**
 *  Initializes the TidyIterator to point to the first item
 *  in Tidy's structure of Windows<->POSIX local mapping.
 *  Items can be retrieved with getNextWindowsLanguage();
 */
TidyIterator TY_(getWindowsLanguageList)(void);

/**
 *  Returns the next record of type `localeMapItem` in
 *  Tidy's structure of Windows<->POSIX local mapping.
 */
const tidyLocaleMapItemImpl *TY_(getNextWindowsLanguage)( TidyIterator* iter );

/**
 *  Given a `tidyLocaleMapItemImpl, return the Windows name.
 */
ctmbstr TY_(TidyLangWindowsName)( const tidyLocaleMapItemImpl *item );

/**
 *  Given a `tidyLocaleMapItemImpl, return the POSIX name.
 */
ctmbstr TY_(TidyLangPosixName)( const tidyLocaleMapItemImpl *item );

/**
 *  Initializes the TidyIterator to point to the first item
 *  in Tidy's list of installed language codes.
 *  Items can be retrieved with getNextInstalledLanguage();
 */
TidyIterator TY_(getInstalledLanguageList)(void);

/**
 *  Returns the next installed language.
 */
ctmbstr TY_(getNextInstalledLanguage)( TidyIterator* iter );


/** @} */

#endif /* language_h */
