/* language.c -- localization support for HTML Tidy.

  Copyright 2015 HTACG
  See tidy.h for the copyright notice.

 */

#include "third_party/tidy/language.h"
#include "third_party/tidy/tmbstr.h"
#include "libc/assert.h"

#include "third_party/tidy/language_en.inc"


/**
 *  This structure type provides universal access to all of Tidy's strings.
 *  Note limit of 8, to be changed as more added...
 */
typedef struct {
    Bool manually_set;
    languageDefinition *currentLanguage;
    languageDefinition *fallbackLanguage;
    languageDefinition *languages[9];
} tidyLanguagesType;


/**
 *  This single structure contains all localizations. Note that we preset
 *  `.currentLanguage` to language_en, which is Tidy's default language.
 */
static tidyLanguagesType tidyLanguages = {
    no,           /* library language was NOT manually set */
    &language_en, /* current language */
    &language_en, /* first fallback language */
    {
        /* Required localization! */
        &language_en,
#if SUPPORT_LOCALIZATIONS
        /* These additional languages are installed. */
        &language_en_gb,
        &language_es,
        &language_es_mx,
        &language_pt_br,
        &language_zh_cn,
        &language_fr,
        &language_de,
#endif
        NULL /* This array MUST be null terminated. */
    }
};


/**
 *  This structure maps old-fashioned Windows strings
 *  to proper POSIX names (modern Windows already uses
 *  POSIX names).
 */
static const tidyLocaleMapItemImpl localeMappings[] = {
    { "america",                "en_us" },
    { "american english",       "en_us" },
    { "american-english",       "en_us" },
    { "american",               "en_us" },
    { "aus",                    "en_au" },
    { "australia",              "en_au" },
    { "australian",             "en_au" },
    { "austria",                "de_at" },
    { "aut",                    "de_at" },
    { "bel",                    "nl_be" },
    { "belgian",                "nl_be" },
    { "belgium",                "nl_be" },
    { "bra",                    "pt-br" },
    { "brazil",                 "pt-br" },
    { "britain",                "en_gb" },
    { "can",                    "en_ca" },
    { "canada",                 "en_ca" },
    { "canadian",               "en_ca" },
    { "che",                    "de_ch" },
    { "china",                  "zh_cn" },
    { "chinese-simplified",     "zh"    },
    { "chinese-traditional",    "zh_tw" },
    { "chinese",                "zh"    },
    { "chn",                    "zh_cn" },
    { "chs",                    "zh"    },
    { "cht",                    "zh_tw" },
    { "csy",                    "cs"    },
    { "cze",                    "cs_cz" },
    { "czech",                  "cs_cz" },
    { "dan",                    "da"    },
    { "danish",                 "da"    },
    { "dea",                    "de_at" },
    { "denmark",                "da_dk" },
    { "des",                    "de_ch" },
    { "deu",                    "de"    },
    { "dnk",                    "da_dk" },
    { "dutch-belgian",          "nl_be" },
    { "dutch",                  "nl"    },
    { "ell",                    "el"    },
    { "ena",                    "en_au" },
    { "enc",                    "en_ca" },
    { "eng",                    "eb_gb" },
    { "england",                "en_gb" },
    { "english-american",       "en_us" },
    { "english-aus",            "en_au" },
    { "english-can",            "en_ca" },
    { "english-nz",             "en_nz" },
    { "english-uk",             "eb_gb" },
    { "english-us",             "en_us" },
    { "english-usa",            "en_us" },
    { "english",                "en"    },
    { "enu",                    "en_us" },
    { "enz",                    "en_nz" },
    { "esm",                    "es-mx" },
    { "esn",                    "es"    },
    { "esp",                    "es"    },
    { "fin",                    "fi"    },
    { "finland",                "fi_fi" },
    { "finnish",                "fi"    },
    { "fra",                    "fr"    },
    { "france",                 "fr_fr" },
    { "frb",                    "fr_be" },
    { "frc",                    "fr_ca" },
    { "french-belgian",         "fr_be" },
    { "french-canadian",        "fr_ca" },
    { "french-swiss",           "fr_ch" },
    { "french",                 "fr"    },
    { "frs",                    "fr_ch" },
    { "gbr",                    "en_gb" },
    { "german-austrian",        "de_at" },
    { "german-swiss",           "de_ch" },
    { "german",                 "de"    },
    { "germany",                "de_de" },
    { "grc",                    "el_gr" },
    { "great britain",          "en_gb" },
    { "greece",                 "el_gr" },
    { "greek",                  "el"    },
    { "hkg",                    "zh_hk" },
    { "holland",                "nl_nl" },
    { "hong kong",              "zh_hk" },
    { "hong-kong",              "zh_hk" },
    { "hun",                    "hu"    },
    { "hungarian",              "hu"    },
    { "hungary",                "hu_hu" },
    { "iceland",                "is_is" },
    { "icelandic",              "is"    },
    { "ireland",                "en_ie" },
    { "irl",                    "en_ie" },
    { "isl",                    "is"    },
    { "ita",                    "it"    },
    { "ita",                    "it_it" },
    { "italian-swiss",          "it_ch" },
    { "italian",                "it"    },
    { "italy",                  "it_it" },
    { "its",                    "it_ch" },
    { "japan",                  "ja_jp" },
    { "japanese",               "ja"    },
    { "jpn",                    "ja"    },
    { "kor",                    "ko"    },
    { "korea",                  "ko_kr" },
    { "korean",                 "ko"    },
    { "mex",                    "es-mx" },
    { "mexico",                 "es-mx" },
    { "netherlands",            "nl_nl" },
    { "new zealand",            "en_nz" },
    { "new-zealand",            "en_nz" },
    { "nlb",                    "nl_be" },
    { "nld",                    "nl"    },
    { "non",                    "nn"    },
    { "nor",                    "nb"    },
    { "norway",                 "no"    },
    { "norwegian-bokmal",       "nb"    },
    { "norwegian-nynorsk",      "nn"    },
    { "norwegian",              "no"    },
    { "nz",                     "en_nz" },
    { "nzl",                    "en_nz" },
    { "plk",                    "pl"    },
    { "pol",                    "pl-pl" },
    { "poland",                 "pl-pl" },
    { "polish",                 "pl"    },
    { "portugal",               "pt-pt" },
    { "portuguese-brazil",      "pt-br" },
    { "portuguese",             "pt"    },
    { "pr china",               "zh_cn" },
    { "pr-china",               "zh_cn" },
    { "prt",                    "pt-pt" },
    { "ptb",                    "pt-br" },
    { "ptg",                    "pt"    },
    { "rus",                    "ru"    },
    { "russia",                 "ru-ru" },
    { "russian",                "ru"    },
    { "sgp",                    "zh_sg" },
    { "singapore",              "zh_sg" },
    { "sky",                    "sk"    },
    { "slovak",                 "sk"    },
    { "spain",                  "es-es" },
    { "spanish-mexican",        "es-mx" },
    { "spanish-modern",         "es"    },
    { "spanish",                "es"    },
    { "sve",                    "sv"    },
    { "svk",                    "sk-sk" },
    { "swe",                    "sv-se" },
    { "sweden",                 "sv-se" },
    { "swedish",                "sv"    },
    { "swiss",                  "de_ch" },
    { "switzerland",            "de_ch" },
    { "taiwan",                 "zh_tw" },
    { "trk",                    "tr"    },
    { "tur",                    "tr-tr" },
    { "turkey",                 "tr-tr" },
    { "turkish",                "tr"    },
    { "twn",                    "zh_tw" },
    { "uk",                     "en_gb" },
    { "united kingdom",         "en_gb" },
    { "united states",          "en_us" },
    { "united-kingdom",         "en_gb" },
    { "united-states",          "en_us" },
    { "us",                     "en_us" },
    { "usa",                    "en_us" },

    /* MUST be last. */
    { NULL,                     NULL    }
};


/**
 *  The real string lookup function.
 */
static ctmbstr tidyLocalizedStringImpl( uint messageType, languageDefinition *definition, uint plural )
{
    int i;
    languageDictionary *dictionary = &definition->messages;
    uint pluralForm = definition->whichPluralForm(plural);

    for (i = 0; (*dictionary)[i].value; ++i)
    {
        if ( (*dictionary)[i].key == messageType && (*dictionary)[i].pluralForm == pluralForm )
        {
            return (*dictionary)[i].value;
        }
    }
    return NULL;
}


/**
 *  Provides a string given `messageType` in the current
 *  localization, returning the correct plural form given
 *  `quantity`.
 *
 *  This isn't currently highly optimized; rewriting some
 *  of infrastructure to use hash lookups is a preferred
 *  future optimization.
 */
ctmbstr TY_(tidyLocalizedStringN)( uint messageType, uint quantity )
{
    ctmbstr result;

    result  = tidyLocalizedStringImpl( messageType, tidyLanguages.currentLanguage, quantity);

    if (!result && tidyLanguages.fallbackLanguage )
    {
        result = tidyLocalizedStringImpl( messageType, tidyLanguages.fallbackLanguage, quantity);
    }

    if (!result)
    {
        /* Fallback to en which is built in. */
        result = tidyLocalizedStringImpl( messageType, &language_en, quantity);
    }

    if (!result)
    {
        /* Last resort: Fallback to en singular which is built in. */
        result = tidyLocalizedStringImpl( messageType, &language_en, 1);
    }

    return result;
}


/**
 *  Provides a string given `messageType` in the current
 *  localization, in the non-plural form.
 *
 *  This isn't currently highly optimized; rewriting some
 *  of infrastructure to use hash lookups is a preferred
 *  future optimization.
 */
ctmbstr TY_(tidyLocalizedString)( uint messageType )
{
    return TY_(tidyLocalizedStringN)( messageType, 1 );
}


/**
 *  Retrieves the POSIX name for a string. Result is a static char so please
 *  don't try to free it. If the name looks like a cc_ll identifier, we will
 *  return it if there's no other match.
 *  @note this routine uses default allocator, see tidySetMallocCall.
 */
static tmbstr TY_(tidyNormalizedLocaleName)( ctmbstr locale )
{
    uint i;
    uint len;
    static char result[6] = "xx_yy";
    TidyAllocator * allocator = &TY_(g_default_allocator);

    tmbstr search = TY_(tmbstrdup)( allocator, locale );
    search = TY_(tmbstrtolower)(search);

    /* See if our string matches a Windows name. */
    for (i = 0; localeMappings[i].winName; ++i)
    {
        if ( strcmp( localeMappings[i].winName, search ) == 0 )
        {
            TidyFree( allocator, search );
            search = TY_(tmbstrdup)( allocator, localeMappings[i].POSIXName );
            break;
        }
    }

    /* We're going to be stupid about this and trust the user, and
     return just the first two characters if they exist and the
     4th and 5th if they exist. The worst that can happen is a
     junk language that doesn't exist and won't be set. */

    len = strlen( search );
    len = ( len <= 5 ? len : 5 );

    for ( i = 0; i < len; i++ )
    {
        if ( i == 2 )
        {
            /* Either terminate the string or ensure there's an underscore */
            if (len == 5) {
                result[i] = '_';
            }
            else {
                result[i] = '\0';
                break;      /* no need to copy after null */
            }
        }
        else
        {
            result[i] = tolower( search[i] );
        }
    }

    TidyFree( allocator, search );
    return result;
}


/**
 *  Returns the languageDefinition if the languageCode is installed in Tidy,
 *  otherwise return NULL
 */
static languageDefinition *TY_(tidyTestLanguage)( ctmbstr languageCode )
{
    uint i;
    languageDefinition *testLang;
    languageDictionary *testDict;
    ctmbstr testCode;

    for (i = 0; tidyLanguages.languages[i]; ++i)
    {
        testLang = tidyLanguages.languages[i];
        testDict = &testLang->messages;
        testCode = (*testDict)[0].value;

        if ( strcmp(testCode, languageCode) == 0 )
            return testLang;
    }

    return NULL;
}


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
Bool TY_(tidySetLanguage)( ctmbstr languageCode )
{
    languageDefinition *dict1 = NULL;
    languageDefinition *dict2 = NULL;
    tmbstr wantCode = NULL;
    char lang[3] = "";

    if ( !languageCode || !(wantCode = TY_(tidyNormalizedLocaleName)( languageCode )) )
    {
        return no;
    }

    /* We want to use the specified language as the currentLanguage, and set
     fallback language as necessary. We have either a two or five digit code,
     either or both of which might be installed. Let's test both of them:
     */

    dict1 = TY_(tidyTestLanguage( wantCode ));  /* WANTED language */

    if ( strlen( wantCode ) > 2 )
    {
        strncpy(__veil("r", lang), __veil("r", wantCode), __veil("r", 2));
        lang[2] = '\0';
        dict2 = TY_(tidyTestLanguage( lang ) ); /* BACKUP language? */
    }

    if ( dict1 && dict2 )
    {
        tidyLanguages.currentLanguage = dict1;
        tidyLanguages.fallbackLanguage = dict2;
    }
    if ( dict1 && !dict2 )
    {
        tidyLanguages.currentLanguage = dict1;
        tidyLanguages.fallbackLanguage = NULL;
    }
    if ( !dict1 && dict2 )
    {
        tidyLanguages.currentLanguage = dict2;
        tidyLanguages.fallbackLanguage = NULL;
    }
    if ( !dict1 && !dict2 )
    {
        /* No change. */
    }

    return dict1 || dict2;
}


/**
 *  Gets the current language used by Tidy.
 */
ctmbstr TY_(tidyGetLanguage)()
{
    languageDefinition *langDef = tidyLanguages.currentLanguage;
    languageDictionary *langDict = &langDef->messages;
    return (*langDict)[0].value;
}


/**
 *  Indicates whether or not the current language was set by a
 *  LibTidy user (yes) or internally by the library (no).
 */
Bool TY_(tidyGetLanguageSetByUser)()
{
    return tidyLanguages.manually_set;
}


/**
 *  Specifies to LibTidy that the user (rather than the library)
 *  selected the current language.
 */
void TY_(tidySetLanguageSetByUser)( void )
{
    tidyLanguages.manually_set = yes;
}


/**
 *  Provides a string given `messageType` in the default
 *  localization (which is `en`), for the given quantity.
 */
ctmbstr TY_(tidyDefaultStringN)( uint messageType, uint quantity )
{
    return tidyLocalizedStringImpl( messageType, &language_en, quantity);
}


/**
 *  Provides a string given `messageType` in the default
 *  localization (which is `en`), for single plural form.
 */
ctmbstr TY_(tidyDefaultString)( uint messageType )
{
    return tidyLocalizedStringImpl( messageType, &language_en, 1);
}


/**
 *  Determines the true size of the `language_en` array indicating the
 *  number of items in the array, _not_ the highest index.
 */
static const uint tidyStringKeyListSize()
{
    static uint array_size = 0;

    if ( array_size == 0 )
    {
        while ( language_en.messages[array_size].value != NULL ) {
            array_size++;
        }
    }

    return array_size;
}


/*
 *  Initializes the TidyIterator to point to the first item
 *  in Tidy's list of localization string keys. Note that
 *  these are provided for documentation generation purposes
 *  and probably aren't useful for LibTidy implementors.
 */
TidyIterator TY_(getStringKeyList)()
{
    return (TidyIterator)(size_t)1;
}

/*
 *  Provides the next key value in Tidy's list of localized
 *  strings. Note that these are provided for documentation
 *  generation purposes and probably aren't useful to
 *  libtidy implementors.
 */
uint TY_(getNextStringKey)( TidyIterator* iter )
{
    uint item = 0;
    size_t itemIndex;
    assert( iter != NULL );

    itemIndex = (size_t)*iter;

    if ( itemIndex > 0 && itemIndex <= tidyStringKeyListSize() )
    {
        item = language_en.messages[ itemIndex - 1 ].key;
        itemIndex++;
    }

    *iter = (TidyIterator)( itemIndex <= tidyStringKeyListSize() ? itemIndex : (size_t)0 );
    return item;
}


/**
 *  Determines the true size of the `localeMappings` array indicating the
 *  number of items in the array, _not_ the highest index.
 */
static const uint tidyLanguageListSize()
{
    static uint array_size = 0;

    if ( array_size == 0 )
    {
        while ( localeMappings[array_size].winName ) {
            array_size++;
        }
    }

    return array_size;
}

/**
 *  Initializes the TidyIterator to point to the first item
 *  in Tidy's structure of Windows<->POSIX local mapping.
 *  Items can be retrieved with getNextWindowsLanguage();
 */
TidyIterator TY_(getWindowsLanguageList)()
{
    return (TidyIterator)(size_t)1;
}

/**
 *  Returns the next record of type `localeMapItem` in
 *  Tidy's structure of Windows<->POSIX local mapping.
 */
const tidyLocaleMapItemImpl *TY_(getNextWindowsLanguage)( TidyIterator *iter )
{
    const tidyLocaleMapItemImpl *item = NULL;
    size_t itemIndex;
    assert( iter != NULL );

    itemIndex = (size_t)*iter;

    if ( itemIndex > 0 && itemIndex <= tidyLanguageListSize() )
    {
        item = &localeMappings[ itemIndex -1 ];
        itemIndex++;
    }

    *iter = (TidyIterator)( itemIndex <= tidyLanguageListSize() ? itemIndex : (size_t)0 );
    return item;
}


/**
 *  Given a `tidyLocaleMapItemImpl, return the Windows name.
 */
ctmbstr TY_(TidyLangWindowsName)( const tidyLocaleMapItemImpl *item )
{
    return item->winName;
}


/**
 *  Given a `tidyLocaleMapItemImpl, return the POSIX name.
 */
ctmbstr TY_(TidyLangPosixName)( const tidyLocaleMapItemImpl *item )
{
    return item->POSIXName;
}


/**
 *  Determines the number of languages installed in Tidy.
 */
static const uint tidyInstalledLanguageListSize()
{
    static uint array_size = 0;

    if ( array_size == 0 )
    {
        while ( tidyLanguages.languages[array_size] ) {
            array_size++;
        }
    }

    return array_size;
}

/**
 *  Initializes the TidyIterator to point to the first item
 *  in Tidy's list of installed language codes.
 *  Items can be retrieved with getNextInstalledLanguage();
 */
TidyIterator TY_(getInstalledLanguageList)()
{
    return (TidyIterator)(size_t)1;
}

/**
 *  Returns the next installed language.
 */
ctmbstr TY_(getNextInstalledLanguage)( TidyIterator* iter )
{
    ctmbstr item = NULL;
    size_t itemIndex;
    assert( iter != NULL );

    itemIndex = (size_t)*iter;

    if ( itemIndex > 0 && itemIndex <= tidyInstalledLanguageListSize() )
    {
        item = tidyLanguages.languages[itemIndex - 1]->messages[0].value;
        itemIndex++;
    }

    *iter = (TidyIterator)( itemIndex <= tidyInstalledLanguageListSize() ? itemIndex : (size_t)0 );
    return item;
}

/*
 * end:
 */

