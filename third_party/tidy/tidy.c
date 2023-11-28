/***************************************************************************//**
 * @file
 * HTML TidyLib command line driver.
 *
 * This console application utilizing LibTidy in order to offer a complete
 * console application offering all of the features of LibTidy.
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
 ******************************************************************************/

#include "third_party/tidy/tidy.h"
#include "third_party/tidy/tidybuffio.h"
#include "libc/assert.h"
#include "libc/mem/alg.h"
#include "libc/str/str.h"
#include "libc/runtime/runtime.h"
#include "libc/mem/mem.h"
#include "third_party/tidy/sprtf.h"

#if defined(_WIN32)
#endif

#if defined(ENABLE_DEBUG_LOG) && defined(_MSC_VER) && defined(_CRTDBG_MAP_ALLOC)
#endif

/** Tidy will send errors to this file, which will be stderr later. */
static FILE* errout = NULL;

#if defined(_WIN32)
   static uint win_cp; /* original Windows code page */
#  if (defined(_MSC_VER) && (_MSC_VER < 1900))
#    define snprintf _snprintf
#  endif
#endif


/** @defgroup console_application Tidy Console Application
 ** @copydoc tidy.c
 ** @{
 */


/* MARK: - Miscellaneous Utilities */
/***************************************************************************//**
 ** @defgroup utilities_misc Miscellaneous Utilities
 ** This group contains general utilities used in the console application.
 *******************************************************************************
 ** @{
 */


/** Indicates whether or not two filenames are the same.
 ** @result Returns a Bool indicating whether the filenames are the same.
 */
static Bool samefile(ctmbstr filename1, /**< First filename */
                     ctmbstr filename2  /**< Second filename */
                     )
{
#if FILENAMES_CASE_SENSITIVE
    return ( strcmp( filename1, filename2 ) == 0 );
#else
    return ( strcasecmp( filename1, filename2 ) == 0 );
#endif
}


/** Handles exit cleanup.
 */
static void tidy_cleanup( void )
{
#if defined(_WIN32)
    /* Restore original Windows code page. */
    SetConsoleOutputCP(win_cp);
#endif
}


/** Exits with an error in the event of an out of memory condition.
 */
static void outOfMemory(void)
{
    fprintf(stderr, "%s", tidyLocalizedString(TC_STRING_OUT_OF_MEMORY));
    exit(1);
}


/** Create a new, allocated string with a format and arguments.
 ** @result Returns a new, allocated string that you must free.
 */
static tmbstr stringWithFormat(const ctmbstr fmt, /**< The format string. */
                               ...                /**< Variable arguments. */
                               )
{
    va_list argList;
    tmbstr result = NULL;
    int len = 0;

    va_start(argList, fmt);
    len = vsnprintf( result, 0, fmt, argList );
    va_end(argList);

    if (!(result = malloc( len + 1) ))
        outOfMemory();

    va_start(argList, fmt);
    vsnprintf( result, len + 1, fmt, argList);
    va_end(argList);

    return result;
}


/** @} end utilities_misc group */
/* MARK: - Output Helping Functions */
/***************************************************************************//**
 ** @defgroup utilities_output Output Helping Functions
 ** This group functions that aid the formatting of output.
 *******************************************************************************
 ** @{
 */


/** Used by `print1Column`, `print2Columns` and `print3Columns` to manage
 ** wrapping text within columns.
 ** @result The pointer to the next part of the string to output.
 */
static const char *cutToWhiteSpace(const char *s, /**< starting point of desired string to output */
                                   uint offset,   /**< column width desired */
                                   char *sbuf     /**< the buffer to output */
                                   )
{
    if (!s)
    {
        sbuf[0] = '\0';
        return NULL;
    }
    else if (strlen(s) <= offset)
    {
        strcpy(sbuf,s);
        sbuf[offset] = '\0';
        return NULL;
    }
    else
    {
        uint j, l, n;
        /* scan forward looking for newline */
        j = 0;
        while(j < offset && s[j] != '\n')
            ++j;
        if ( j == offset ) {
            /* scan backward looking for first space */
            j = offset;
            while(j && s[j] != ' ')
                --j;
            l = j;
            n = j+1;
            /* no white space */
            if (j==0)
            {
                l = offset;
                n = offset;
            }
        } else
        {
            l = j;
            n = j+1;
        }
        strncpy(sbuf,s,l);
        sbuf[l] = '\0';
        return s+n;
    }
}


/** Outputs one column of text.
 */
static void print1Column(const char* fmt, /**< The format string for formatting the output. */
                         uint l1,         /**< The width of the column. */
                         const char *c1   /**< The content of the column. */
                         )
{
    const char *pc1=c1;
    char *c1buf = (char *)malloc(l1+1);
    if (!c1buf) outOfMemory();

    do
    {
        pc1 = cutToWhiteSpace(pc1, l1, c1buf);
        printf(fmt, c1buf[0] !='\0' ? c1buf : "");
    } while (pc1);
    free(c1buf);
}


/** Outputs two columns of text.
 */
static void print2Columns(const char* fmt, /**< The format string for formatting the output. */
                          uint l1,         /**< The width of column 1. */
                          uint l2,         /**< The width of column 2. */
                          const char *c1,  /**< The contents of column 1. */
                          const char *c2   /**< The contents of column 2. */
)
{
    const char *pc1=c1, *pc2=c2;
    char *c1buf = (char *)malloc(l1+1);
    char *c2buf = (char *)malloc(l2+1);
    if (!c1buf) outOfMemory();
    if (!c2buf) outOfMemory();

    do
    {
        pc1 = cutToWhiteSpace(pc1, l1, c1buf);
        pc2 = cutToWhiteSpace(pc2, l2, c2buf);
        printf(fmt, l1, l1, c1buf[0]!='\0'?c1buf:"",
                    l2, l2, c2buf[0]!='\0'?c2buf:"");
    } while (pc1 || pc2);
    free(c1buf);
    free(c2buf);
}


/** Outputs three columns of text.
 */
static void print3Columns(const char* fmt, /**< The three column format string. */
                          uint l1,         /**< Width of column 1. */
                          uint l2,         /**< Width of column 2. */
                          uint l3,         /**< Width of column 3. */
                          const char *c1,  /**< Content of column 1. */
                          const char *c2,  /**< Content of column 2. */
                          const char *c3   /**< Content of column 3. */
                          )
{
    const char *pc1=c1, *pc2=c2, *pc3=c3;
    char *c1buf = (char *)malloc(l1+1);
    char *c2buf = (char *)malloc(l2+1);
    char *c3buf = (char *)malloc(l3+1);
    if (!c1buf) outOfMemory();
    if (!c2buf) outOfMemory();
    if (!c3buf) outOfMemory();

    do
    {
        pc1 = cutToWhiteSpace(pc1, l1, c1buf);
        pc2 = cutToWhiteSpace(pc2, l2, c2buf);
        pc3 = cutToWhiteSpace(pc3, l3, c3buf);
        printf(fmt,
               c1buf[0]!='\0'?c1buf:"",
               c2buf[0]!='\0'?c2buf:"",
               c3buf[0]!='\0'?c3buf:"");
    } while (pc1 || pc2 || pc3);
    free(c1buf);
    free(c2buf);
    free(c3buf);
}


/** Provides the `unknown option` output to the current errout.
 */
static void unknownOption(TidyDoc tdoc, /**< The Tidy document. */
                          uint c        /**< The unknown option. */
                          )
{
    fprintf( errout, tidyLocalizedString( TC_STRING_UNKNOWN_OPTION ), (char)c );
    fprintf( errout, "\n");
}


/** @} end utilities_output group */
/* MARK: - CLI Options Utilities */
/***************************************************************************//**
 ** @defgroup options_cli CLI Options Utilities
 ** These structures, arrays, declarations, and definitions are used throughout
 ** this console application.
 *******************************************************************************
 ** @{
 */


/** @name Format strings and decorations used in output.
 ** @{
 */

static const char helpfmt[] = " %-*.*s %-*.*s\n";
static const char helpul[]  = "-----------------------------------------------------------------";
static const char fmt[]     = "%-27.27s %-9.9s  %-40.40s\n";
static const char ul[]      = "=================================================================";

/** @} */

/** This enum is used to categorize the options for help output.
 */
typedef enum
{
    CmdOptFileManip,
    CmdOptCatFIRST = CmdOptFileManip,
    CmdOptProcDir,
    CmdOptCharEnc,
    CmdOptMisc,
    CmdOptXML,
    CmdOptCatLAST
} CmdOptCategory;

/** This array contains headings that will be used in help output.
 */
static const struct {
    ctmbstr mnemonic;  /**< Used in XML as a class. */
    uint key;          /**< Key to fetch the localized string. */
} cmdopt_catname[] = {
    { "file-manip", TC_STRING_FILE_MANIP },
    { "process-directives", TC_STRING_PROCESS_DIRECTIVES },
    { "char-encoding", TC_STRING_CHAR_ENCODING },
    { "misc", TC_STRING_MISC },
    { "xml", TC_STRING_XML }
};

/** The struct and subsequent array keep the help output structured
 ** because we _also_ output all of this stuff as as XML.
 */
typedef struct {
    CmdOptCategory cat; /**< Category */
    ctmbstr name1;      /**< Name */
    uint key;           /**< Key to fetch the localized description. */
    uint subKey;        /**< Secondary substitution key. */
    ctmbstr eqconfig;   /**< Equivalent configuration option */
    ctmbstr name2;      /**< Name */
    ctmbstr name3;      /**< Name */
} CmdOptDesc;

/** All instances of %s will be substituted with localized string
 ** specified by the subKey field.
 */
static const CmdOptDesc cmdopt_defs[] =  {
    { CmdOptFileManip, "-output <%s>",           TC_OPT_OUTPUT,   TC_LABEL_FILE, "output-file: <%s>", "-o <%s>" },
    { CmdOptFileManip, "-config <%s>",           TC_OPT_CONFIG,   TC_LABEL_FILE, NULL },
    { CmdOptFileManip, "-file <%s>",             TC_OPT_FILE,     TC_LABEL_FILE, "error-file: <%s>", "-f <%s>" },
    { CmdOptFileManip, "-modify",                TC_OPT_MODIFY,   0,             "write-back: yes", "-m" },
    { CmdOptProcDir,   "-indent",                TC_OPT_INDENT,   0,             "indent: auto", "-i" },
    { CmdOptProcDir,   "-wrap <%s>",             TC_OPT_WRAP,     TC_LABEL_COL,  "wrap: <%s>", "-w <%s>" },
    { CmdOptProcDir,   "-upper",                 TC_OPT_UPPER,    0,             "uppercase-tags: yes", "-u" },
    { CmdOptProcDir,   "-clean",                 TC_OPT_CLEAN,    0,             "clean: yes", "-c" },
    { CmdOptProcDir,   "-bare",                  TC_OPT_BARE,     0,             "bare: yes", "-b" },
    { CmdOptProcDir,   "-gdoc",                  TC_OPT_GDOC,     0,             "gdoc: yes", "-g" },
    { CmdOptProcDir,   "-numeric",               TC_OPT_NUMERIC,  0,             "numeric-entities: yes", "-n" },
    { CmdOptProcDir,   "-errors",                TC_OPT_ERRORS,   0,             "markup: no", "-e" },
    { CmdOptProcDir,   "-quiet",                 TC_OPT_QUIET,    0,             "quiet: yes", "-q" },
    { CmdOptProcDir,   "-omit",                  TC_OPT_OMIT,     0,             "omit-optional-tags: yes" },
    { CmdOptProcDir,   "-xml",                   TC_OPT_XML,      0,             "input-xml: yes" },
    { CmdOptProcDir,   "-asxml",                 TC_OPT_ASXML,    0,             "output-xhtml: yes", "-asxhtml" },
    { CmdOptProcDir,   "-ashtml",                TC_OPT_ASHTML,   0,             "output-html: yes" },
    { CmdOptProcDir,   "-access <%s>",           TC_OPT_ACCESS,   TC_LABEL_LEVL, "accessibility-check: <%s>" },
    { CmdOptCharEnc,   "-raw",                   TC_OPT_RAW,      0,             NULL },
    { CmdOptCharEnc,   "-ascii",                 TC_OPT_ASCII,    0,             NULL },
    { CmdOptCharEnc,   "-latin0",                TC_OPT_LATIN0,   0,             NULL },
    { CmdOptCharEnc,   "-latin1",                TC_OPT_LATIN1,   0,             NULL },
#ifndef NO_NATIVE_ISO2022_SUPPORT
    { CmdOptCharEnc,   "-iso2022",               TC_OPT_ISO2022,  0,             NULL },
#endif
    { CmdOptCharEnc,   "-utf8",                  TC_OPT_UTF8,     0,             NULL },
    { CmdOptCharEnc,   "-mac",                   TC_OPT_MAC,      0,             NULL },
    { CmdOptCharEnc,   "-win1252",               TC_OPT_WIN1252,  0,             NULL },
    { CmdOptCharEnc,   "-ibm858",                TC_OPT_IBM858,   0,             NULL },
    { CmdOptCharEnc,   "-utf16le",               TC_OPT_UTF16LE,  0,             NULL },
    { CmdOptCharEnc,   "-utf16be",               TC_OPT_UTF16BE,  0,             NULL },
    { CmdOptCharEnc,   "-utf16",                 TC_OPT_UTF16,    0,             NULL },
    { CmdOptCharEnc,   "-big5",                  TC_OPT_BIG5,     0,             NULL },
    { CmdOptCharEnc,   "-shiftjis",              TC_OPT_SHIFTJIS, 0,             NULL },
    { CmdOptMisc,      "-version",               TC_OPT_VERSION,  0,             NULL,  "-v" },
    { CmdOptMisc,      "-help",                  TC_OPT_HELP,     0,             NULL,  "-h", "-?" },
    { CmdOptMisc,      "-help-config",           TC_OPT_HELPCFG,  0,             NULL },
    { CmdOptMisc,      "-help-env",              TC_OPT_HELPENV,  0,             NULL },
    { CmdOptMisc,      "-show-config",           TC_OPT_SHOWCFG,  0,             NULL },
    { CmdOptMisc,      "-export-config",         TC_OPT_EXP_CFG,  0,             NULL },
    { CmdOptMisc,      "-export-default-config", TC_OPT_EXP_DEF,  0,             NULL },
    { CmdOptMisc,      "-help-option <%s>",      TC_OPT_HELPOPT,  TC_LABEL_OPT,  NULL },
    { CmdOptMisc,      "-language <%s>",         TC_OPT_LANGUAGE, TC_LABEL_LANG, "language: <%s>" },
    { CmdOptXML,       "-xml-help",              TC_OPT_XMLHELP,  0,             NULL },
    { CmdOptXML,       "-xml-config",            TC_OPT_XMLCFG,   0,             NULL },
    { CmdOptXML,       "-xml-strings",           TC_OPT_XMLSTRG,  0,             NULL },
    { CmdOptXML,       "-xml-error-strings",     TC_OPT_XMLERRS,  0,             NULL },
    { CmdOptXML,       "-xml-options-strings",   TC_OPT_XMLOPTS,  0,             NULL },
    { CmdOptMisc,      NULL,                   0,               0,             NULL }
};


/** Option names aren't localized, but the sample fields should be localized.
 ** For example, `<file>` should be `<archivo>` in Spanish.
 ** @param pos A CmdOptDesc array with fields that must be localized.
 */
static void localize_option_names( CmdOptDesc *pos)
{
    ctmbstr fileString = tidyLocalizedString(pos->subKey);
    pos->name1 = stringWithFormat(pos->name1, fileString);
    if ( pos->name2 )
        pos->name2 = stringWithFormat(pos->name2, fileString);
    if ( pos->name3 )
        pos->name3 = stringWithFormat(pos->name3, fileString);
    if ( pos->eqconfig )
        pos->eqconfig = stringWithFormat(pos->eqconfig, fileString);
}


/** Escape a name for XML output. For example, `-output <file>` becomes
 ** `-output &lt;file&gt;` for use in XML.
 ** @param name The option name to escape.
 ** @result Returns an allocated string.
 */
static tmbstr get_escaped_name( ctmbstr name )
{
    tmbstr escpName;
    char aux[2];
    uint len = 0;
    ctmbstr c;
    for(c=name; *c!='\0'; ++c)
        switch(*c)
    {
        case '<':
        case '>':
            len += 4;
            break;
        case '"':
            len += 6;
            break;
        default:
            len += 1;
            break;
    }

    escpName = (tmbstr)malloc(len+1);
    if (!escpName) outOfMemory();
    escpName[0] = '\0';

    aux[1] = '\0';
    for(c=name; *c!='\0'; ++c)
        switch(*c)
    {
        case '<':
            strcat(escpName, "&lt;");
            break;
        case '>':
            strcat(escpName, "&gt;");
            break;
        case '"':
            strcat(escpName, "&quot;");
            break;
        default:
            aux[0] = *c;
            strcat(escpName, aux);
            break;
    }

    return escpName;
}


/** @} end CLI Options Definitions Utilities group */
/* MARK: - Configuration Options Utilities */
/***************************************************************************//**
 ** @defgroup utilities_cli_options Configuration Options Utilities
 ** Provide utilities to manipulate configuration options for output.
 *******************************************************************************
 ** @{
 */


/** Utility to determine if an option has a picklist.
 ** @param topt The option to check.
 ** @result Returns a Bool indicating whether the option has a picklist or not.
 */
static Bool hasPickList( TidyOption topt )
{
    TidyIterator pos;

    if ( tidyOptGetType( topt ) != TidyInteger)
        return no;

    pos = tidyOptGetPickList( topt );

    return tidyOptGetNextPick( topt, &pos ) != NULL;
}

/** Returns the configuration category id for the specified configuration
 ** category id. This will be used as an XML class attribute value.
 ** @param id The TidyConfigCategory for which to lookup the category name.
 ** @result Returns the configuration category, such as "diagnostics".
 */
static ctmbstr ConfigCategoryId( TidyConfigCategory id )
{
    if (id >= TidyDiagnostics && id <= TidyInternalCategory)
        return tidyErrorCodeAsKey( id );

    fprintf(stderr, tidyLocalizedString(TC_STRING_FATAL_ERROR), (int)id);
    fprintf(stderr, "\n");

    assert(0);
    abort();
    return "never_here"; /* only for the compiler warning */
}

/** Structure maintains a description of a configuration ption.
 */
typedef struct {
    ctmbstr name;         /**< Name */
    ctmbstr cat;          /**< Category */
    uint    catid;        /**< Category ID */
    ctmbstr type;         /**< "String, ... */
    ctmbstr vals;         /**< Potential values. If NULL, use an external function */
    ctmbstr def;          /**< default */
    tmbchar tempdefs[80]; /**< storage for default such as integer */
    Bool haveVals;        /**< if yes, vals is valid */
} OptionDesc;

/** A type for a function pointer for a function used to print out options
 ** descriptions.
 ** @param TidyDoc The document.
 ** @param TidyOption The Tidy option.
 ** @param OptionDesc A pointer to the option description structure.
 */
typedef void (*OptionFunc)( TidyDoc, TidyOption, OptionDesc * );


/** Create OptionDesc "d" related to "opt"
 */
static void GetOption(TidyDoc tdoc,    /**< The tidy document. */
                      TidyOption topt, /**< The option to create a description for. */
                      OptionDesc *d    /**< [out] The new option description. */
                      )
{
    TidyOptionId optId = tidyOptGetId( topt );
    TidyOptionType optTyp = tidyOptGetType( topt );

    d->name = tidyOptGetName( topt );
    d->cat = ConfigCategoryId( tidyOptGetCategory( topt ) );
    d->catid = tidyOptGetCategory( topt );
    d->vals = NULL;
    d->def = NULL;
    d->haveVals = yes;

    /* Handle special cases first. */
    switch ( optId )
    {
        case TidyInlineTags:
        case TidyBlockTags:
        case TidyEmptyTags:
        case TidyPreTags:
            d->type = "Tag Names";
            d->vals = "tagX, tagY, ...";
            d->def = NULL;
            break;

        case TidyPriorityAttributes:
            d->type = "Attributes Names";
            d->vals = "attributeX, attributeY, ...";
            d->def = NULL;
            break;

        case TidyCharEncoding:
        case TidyInCharEncoding:
        case TidyOutCharEncoding:
            d->type = "Encoding";
            d->def = tidyOptGetEncName( tdoc, optId );
            if (!d->def)
                d->def = "?";
            d->vals = NULL;
            break;

            /* General case will handle remaining */
        default:
            switch ( optTyp )
        {
            case TidyBoolean:
                d->type = "Boolean";
                d->def = tidyOptGetCurrPick( tdoc, optId );
                break;

            case TidyInteger:
                if (hasPickList(topt))
                {
                    d->type = "Enum";
                    d->def = tidyOptGetCurrPick( tdoc, optId );
                }
                else
                {
                    uint idef;
                    d->type = "Integer";
                    if ( optId == TidyWrapLen )
                        d->vals = "0 (no wrapping), 1, 2, ...";
                    else
                        d->vals = "0, 1, 2, ...";

                    idef = tidyOptGetInt( tdoc, optId );
                    sprintf(d->tempdefs, "%u", idef);
                    d->def = d->tempdefs;
                }
                break;

            case TidyString:
                d->type = "String";
                d->vals = NULL;
                d->haveVals = no;
                d->def = tidyOptGetValue( tdoc, optId );
                break;
        }
    }
}

/** Array holding all options. Contains a trailing sentinel.
 */
typedef struct {
    TidyOption topt[N_TIDY_OPTIONS];
} AllOption_t;

/** A simple option comparator, used for sorting the options.
 ** @result Returns an integer indicating the result of the comparison.
 */
static int cmpOpt(const void* e1_, /**< Item A to compare. */
                  const void *e2_  /**< Item B to compare. */
                  )
{
    const TidyOption* e1 = (const TidyOption*)e1_;
    const TidyOption* e2 = (const TidyOption*)e2_;
    return strcmp(tidyOptGetName(*e1), tidyOptGetName(*e2));
}

/** Returns options sorted.
 */
static void getSortedOption(TidyDoc tdoc,         /**< The Tidy document. */
                            AllOption_t *tOption  /**< [out] The list of options. */
                            )
{
    TidyIterator pos = tidyGetOptionList( tdoc );
    uint i = 0;

    while ( pos )
    {
        TidyOption topt = tidyGetNextOption( tdoc, &pos );
        tOption->topt[i] = topt;
        ++i;
    }
    tOption->topt[i] = NULL; /* sentinel */

    qsort(tOption->topt,
          i, /* there are i items, not including the sentinel */
          sizeof(tOption->topt[0]),
          cmpOpt);
}

/** An iterator for the sorted options.
 */
static void ForEachSortedOption(TidyDoc tdoc,          /**< The Tidy document. */
                                OptionFunc OptionPrint /**< The printing function to be used. */
                                )
{
    AllOption_t tOption;
    const TidyOption *topt;

    getSortedOption( tdoc, &tOption );
    for( topt = tOption.topt; *topt; ++topt)
    {
        OptionDesc d;

        GetOption( tdoc, *topt, &d );
        (*OptionPrint)( tdoc, *topt, &d );
    }
}

/** An iterator for the unsorted options.
 */
static void ForEachOption(TidyDoc tdoc,          /**< The Tidy document. */
                          OptionFunc OptionPrint /**< The printing function to be used. */
)
{
    TidyIterator pos = tidyGetOptionList( tdoc );

    while ( pos )
    {
        TidyOption topt = tidyGetNextOption( tdoc, &pos );
        OptionDesc d;

        GetOption( tdoc, topt, &d );
        (*OptionPrint)( tdoc, topt, &d );
    }
}

/** Prints an option's allowed value as specified in its pick list.
 ** @param topt The Tidy option.
 */
static void PrintAllowedValuesFromPick( TidyOption topt )
{
    TidyIterator pos = tidyOptGetPickList( topt );
    Bool first = yes;
    ctmbstr def;
    while ( pos )
    {
        if (first)
            first = no;
        else
            printf(", ");
        def = tidyOptGetNextPick( topt, &pos );
        printf("%s", def);
    }
}

/** Prints an option's allowed values.
 */
static void PrintAllowedValues(TidyOption topt,    /**< The Tidy option. */
                               const OptionDesc *d /**< The OptionDesc for the option. */
                               )
{
    if (d->vals)
        printf( "%s", d->vals );
    else
        PrintAllowedValuesFromPick( topt );
}


/** @} end utilities_cli_options group */
/* MARK: - Provide the -help Service */
/***************************************************************************//**
 ** @defgroup service_help Provide the -help Service
 *******************************************************************************
 ** @{
 */


/** Retrieve the option's name(s) from the structure as a single string,
 ** localizing the field values if application. For example, this might
 ** return `-output <file>, -o <file>`.
 ** @param pos A CmdOptDesc array item for which to get the names.
 ** @result Returns the name(s) for the option as a single string.
 */
static tmbstr get_option_names( const CmdOptDesc* pos )
{
    tmbstr name;
    uint len;
    CmdOptDesc localPos = *pos;

    localize_option_names( &localPos );

    len = strlen(localPos.name1);
    if (localPos.name2)
        len += 2+strlen(localPos.name2);
    if (localPos.name3)
        len += 2+strlen(localPos.name3);

    name = (tmbstr)malloc(len+1);
    if (!name) outOfMemory();
    strcpy(name, localPos.name1);
    free((tmbstr)localPos.name1);
    if (localPos.name2)
    {
        strcat(name, ", ");
        strcat(name, localPos.name2);
        free((tmbstr)localPos.name2);
    }
    if (localPos.name3)
    {
        strcat(name, ", ");
        strcat(name, localPos.name3);
        free((tmbstr)localPos.name3);
    }
    return name;
}


/** Returns the final name of the tidy executable by eliminating the path
 ** name components from the executable name.
 ** @param prog The path of the current executable.
 */
static ctmbstr get_final_name( ctmbstr prog )
{
    ctmbstr name = prog;
    int c;
    size_t i;
    size_t len = strlen(prog);

    for (i = 0; i < len; i++)
    {
        c = prog[i];
        if ((( c == '/' ) || ( c == '\\' )) && prog[i+1])
        {
            name = &prog[i+1];
        }
    }

    return name;
}

/** Outputs all of the complete help options (text).
 ** @param tdoc The Tidydoc whose options are being printed.
 */
static void print_help_options( TidyDoc tdoc )
{
    CmdOptCategory cat = CmdOptCatFIRST;
    const CmdOptDesc* pos = cmdopt_defs;
    uint col1, col2;
    uint width = 78;

    for( cat=CmdOptCatFIRST; cat!=CmdOptCatLAST; ++cat)
    {
        ctmbstr name = tidyLocalizedString(cmdopt_catname[cat].key);
        size_t len = width < strlen(name) ? width : strlen(name);
        printf( "%s\n", name );
        printf( "%*.*s\n", (int)len, (int)len, helpul );

        /* Tidy's "standard" 78-column output was always 25:52 ratio, so let's
           try to preserve this approximately 1:2 ratio regardless of whatever
           silly thing the user might have set for a console width, with a
           maximum of 50 characters for the first column.
         */
        col1 = width / 3;             /* one third of the available */
        col1 = col1 < 1 ? 1 : col1;   /* at least 1 */
        col1 = col1 > 35 ? 35 : col1; /* no greater than 35 */
        col2 = width - col1 - 2;      /* allow two spaces */
        col2 = col2 < 1 ? 1 : col2;   /* at least 1 */

        for( pos=cmdopt_defs; pos->name1; ++pos)
        {
            tmbstr name;
            if (pos->cat != cat)
                continue;
            name = get_option_names( pos );
            print2Columns( helpfmt, col1, col2, name, tidyLocalizedString( pos->key ) );
            free(name);
        }
        printf("\n");
    }
}

/** Handles the -help service.
 */
static void help(TidyDoc tdoc, /**< The tidy document for which help is showing. */
                 ctmbstr prog  /**< The path of the current executable. */
                 )
{
    tmbstr temp_string = NULL;
    uint width = 78;

    printf( tidyLocalizedString(TC_TXT_HELP_1), get_final_name(prog), tidyLibraryVersion() );
    printf("\n");

    if ( tidyPlatform() )
        temp_string = stringWithFormat( tidyLocalizedString(TC_TXT_HELP_2A), tidyPlatform() );
    else
        temp_string = stringWithFormat( tidyLocalizedString(TC_TXT_HELP_2B) );

    width = width < strlen(temp_string) ? width : strlen(temp_string);
    printf( "%s\n", temp_string );
    printf( "%*.*s\n\n", width, width, ul);
    free( temp_string );

    print_help_options( tdoc );


    printf("\n");
#if defined(TIDY_CONFIG_FILE) && defined(TIDY_USER_CONFIG_FILE)
    temp_string = stringWithFormat( tidyLocalizedString(TC_TXT_HELP_3A), TIDY_CONFIG_FILE, TIDY_USER_CONFIG_FILE );
    printf( tidyLocalizedString(TC_TXT_HELP_3), temp_string );
    free( temp_string );
#else
    printf( tidyLocalizedString(TC_TXT_HELP_3), "\n" );
#endif
    printf("\n");
}

/** @} end service_help group */
/* MARK: - Provide the -help-config Service */
/***************************************************************************//**
 ** @defgroup service_help_config Provide the -help-config Service
 *******************************************************************************
 ** @{
 */


/** Retrieves allowed values from an option's pick list.
 ** @param topt A TidyOption for which to get the allowed values.
 ** @result A string containing the allowed values.
 */
static tmbstr GetAllowedValuesFromPick( TidyOption topt )
{
    TidyIterator pos;
    Bool first;
    ctmbstr def;
    uint len = 0;
    tmbstr val;

    pos = tidyOptGetPickList( topt );
    first = yes;
    while ( pos )
    {
        if (first)
            first = no;
        else
            len += 2;
        def = tidyOptGetNextPick( topt, &pos );
        len += strlen(def);
    }
    val = (tmbstr)malloc(len+1);
    if (!val) outOfMemory();
    val[0] = '\0';
    pos = tidyOptGetPickList( topt );
    first = yes;
    while ( pos )
    {
        if (first)
            first = no;
        else
            strcat(val, ", ");
        def = tidyOptGetNextPick( topt, &pos );
        strcat(val, def);
    }
    return val;
}

/** Retrieves allowed values for an option.
 ** @result A string containing the allowed values.
 */
static tmbstr GetAllowedValues(TidyOption topt,    /**< A TidyOption for which to get the allowed values. */
                               const OptionDesc *d /**< A pointer to the OptionDesc array. */
                               )
{
    if (d->vals)
    {
        tmbstr val = (tmbstr)malloc(1+strlen(d->vals));
        if (!val) outOfMemory();
        strcpy(val, d->vals);
        return val;
    }
    else
        return GetAllowedValuesFromPick( topt );
}

/** Prints a single option.
 */
static void printOption(TidyDoc ARG_UNUSED(tdoc), /**< The Tidy document. */
                        TidyOption topt,          /**< The option to print. */
                        OptionDesc *d             /**< A pointer to the OptionDesc array. */
                        )
{
    if (tidyOptGetCategory( topt ) == TidyInternalCategory )
        return;

    if ( *d->name || *d->type )
    {
        ctmbstr pval = d->vals;
        tmbstr val = NULL;
        if (!d->haveVals)
        {
            pval = "-";
        }
        else if (pval == NULL)
        {
            val = GetAllowedValues( topt, d);
            pval = val;
        }
        print3Columns( fmt, 27, 9, 40, d->name, d->type, pval );
        if (val)
            free(val);
    }
}

/** Handles the -help-config service.
 ** @remark We will not support console word wrapping for the configuration
 **         options table. If users really have a small console, then they
 *          should make it wider or output to a file.
 ** @param tdoc The Tidy document.
 */
static void optionhelp( TidyDoc tdoc )
{
    printf( "\n" );
    printf( "%s", tidyLocalizedString( TC_TXT_HELP_CONFIG ) );

    printf( fmt,
           tidyLocalizedString( TC_TXT_HELP_CONFIG_NAME ),
           tidyLocalizedString( TC_TXT_HELP_CONFIG_TYPE ),
           tidyLocalizedString( TC_TXT_HELP_CONFIG_ALLW ) );

    printf( fmt, ul, ul, ul );

    ForEachSortedOption( tdoc, printOption );
}


/** @} end service_help_config group */
/* MARK: - Provide the -help-env Service */
/***************************************************************************//**
 ** @defgroup service_help_env Provide the -help-env Service
 *******************************************************************************
 ** @{
 */


/** Handles the -help-env service.
 ** @param tdoc The Tidy document.
 */
static void helpEnv( TidyDoc tdoc )
{
    tmbstr subst = "";
    Bool uses_env = getenv("HTML_TIDY") != NULL;
    ctmbstr env_var = uses_env ? getenv("HTML_TIDY"): tidyLocalizedString( TC_TXT_HELP_ENV_1B );

#if defined( TIDY_CONFIG_FILE ) && defined( TIDY_USER_CONFIG_FILE )
    subst = stringWithFormat( tidyLocalizedString(TC_TXT_HELP_ENV_1A), TIDY_CONFIG_FILE, TIDY_USER_CONFIG_FILE );
#endif

    env_var = env_var != NULL ? env_var : tidyLocalizedString( TC_TXT_HELP_ENV_1B );

    printf( "\n" );
    printf( tidyLocalizedString( TC_TXT_HELP_ENV_1), subst, env_var );

#if defined( TIDY_CONFIG_FILE ) && defined( TIDY_USER_CONFIG_FILE )
    if ( uses_env )
        printf( tidyLocalizedString( TC_TXT_HELP_ENV_1C ), TIDY_USER_CONFIG_FILE );
    free( subst );
#endif

    printf( "\n" );
}



/** @} end service_help_env group */
/* MARK: - Provide the -help-option Service */
/***************************************************************************//**
 ** @defgroup service_help_option Provide the -help-option Service
 *******************************************************************************
 ** @{
 */


/** Cleans up the HTML-laden option descriptions for console output. It's
 ** just a simple HTML filtering/replacement function.
 ** @param description The option description.
 ** @result Returns an allocated string with some HTML stripped away.
 */
static tmbstr cleanup_description( ctmbstr description )
{
    /* Substitutions - this might be a good spot to introduce platform
     dependent definitions for colorized output on different terminals
     that support, for example, ANSI escape sequences. The assumption
     is made the Mac and Linux targets support ANSI colors, but even
     so debugger terminals may not. Note that the line-wrapping
     function also doesn't account for non-printing characters. */
    static struct {
        ctmbstr tag;
        ctmbstr replacement;
    } const replacements[] = {
        { "lt",       "<"          },
        { "gt",       ">"          },
        { "br/",      "\n\n"       },
#if defined(LINUX_OS) || defined(MAC_OS_X)
        { "code",     "\x1b[36m"   },
        { "/code",    "\x1b[0m"    },
        { "em",       "\x1b[4m"   },
        { "/em",      "\x1b[0m"    },
        { "strong",   "\x1b[31m"   },
        { "/strong",  "\x1b[0m"    },
#endif
        /* MUST be last */
        { NULL,       NULL         },
    };

    /* State Machine Setup */
    typedef enum {
        s_DONE,
        s_DATA,
        s_WRITING,
        s_TAG_OPEN,
        s_TAG_NAME,
        s_ERROR,
        s_LAST /* MUST be last */
    } states;

    typedef enum {
        c_NIL,
        c_EOF,
        c_BRACKET_CLOSE,
        c_BRACKET_OPEN,
        c_OTHER
    } charstates;

    typedef enum {
        a_NIL,
        a_BUILD_NAME,
        a_CONSUME,
        a_EMIT,
        a_EMIT_SUBS,
        a_WRITE,
        a_ERROR
    } actions;

    typedef struct {
        states state;
        charstates charstate;
        actions action;
        states next_state;
    } transitionType;

    const transitionType transitions[] = {
        { s_DATA,           c_EOF,           a_NIL,        s_DONE           },
        { s_DATA,           c_BRACKET_OPEN,  a_CONSUME,    s_TAG_OPEN       },
        /* special case allows ; */
        { s_DATA,           c_BRACKET_CLOSE, a_EMIT,       s_WRITING        },
        { s_DATA,           c_OTHER,         a_EMIT,       s_WRITING        },
        { s_WRITING,        c_OTHER,         a_WRITE,      s_DATA           },
        { s_WRITING,        c_BRACKET_CLOSE, a_WRITE,      s_DATA           },
        { s_TAG_OPEN,       c_EOF,           a_ERROR,      s_DONE           },
        { s_TAG_OPEN,       c_OTHER,         a_NIL,        s_TAG_NAME       },
        { s_TAG_NAME,       c_BRACKET_OPEN,  a_ERROR,      s_DONE           },
        { s_TAG_NAME,       c_EOF,           a_ERROR,      s_DONE           },
        { s_TAG_NAME,       c_BRACKET_CLOSE, a_EMIT_SUBS,  s_WRITING        },
        { s_TAG_NAME,       c_OTHER,         a_BUILD_NAME, s_TAG_NAME       },
        { s_ERROR,          0,               a_ERROR,      s_DONE           },
        { s_DONE,           0,               a_NIL,        0                },
        /* MUST be last: */
        { s_LAST,           0,               0,            0                },
    };

    /* Output Setup */
    tmbstr result = NULL;
    int g_result = 100;  /* minimum buffer grow size */
    int l_result = 0;    /* buffer current size */
    int i_result = 0;    /* current string position */
    int writer_len = 0;  /* writer length */

    ctmbstr writer = NULL;

    /* Current tag name setup */
    tmbstr name = NULL; /* tag name */
    int g_name = 10;    /* buffer grow size */
    int l_name = 0;     /* buffer current size */
    int i_name = 0;     /* current string position */

    /* Pump Setup */
    int i = 0;
    states state = s_DATA;
    charstates charstate;
    char c;
    int j = 0, k = 0;
    transitionType transition;

    if ( !description || (strlen(description) < 1) )
    {
        return NULL;
    }

    /* Process the HTML Snippet */
    do {
        c = description[i];

        /* Determine secondary state. */
        switch (c)
        {
            case '\0':
                charstate = c_EOF;
                break;

            case '<':
            case '&':
                charstate = c_BRACKET_OPEN;
                break;

            case '>':
            case ';':
                charstate = c_BRACKET_CLOSE;
                break;

            default:
                charstate = c_OTHER;
                break;
        }

        /* Find the correct instruction */
        j = 0;
        while (transitions[j].state != s_LAST)
        {
            transition = transitions[j];
            if ( transition.state == state && transition.charstate == charstate ) {
                switch ( transition.action )
                {
                        /* This action is building the name of an HTML tag. */
                    case a_BUILD_NAME:
                        if ( !name )
                        {
                            l_name = g_name;
                            name = calloc(l_name, 1);
                        }

                        if ( i_name >= l_name )
                        {
                            l_name = l_name + g_name;
                            name = realloc(name, l_name);
                        }

                        strncpy(name + i_name, &c, 1);
                        i_name++;
                        i++;
                        break;

                        /* This character will be emitted into the output
                         stream. The only purpose of this action is to
                         ensure that `writer` is NULL as a flag that we
                         will output the current `c` */
                    case a_EMIT:
                        writer = NULL; /* flag to use c */
                        break;

                        /* Now that we've consumed a tag, we will emit the
                         substitution if any has been specified in
                         `replacements`. */
                    case a_EMIT_SUBS:
                        name[i_name] = '\0';
                        i_name = 0;
                        k = 0;
                        writer = "";
                        while ( replacements[k].tag )
                        {
                            if ( strcmp( replacements[k].tag, name ) == 0 )
                            {
                                writer = replacements[k].replacement;
                            }
                            k++;
                        }
                        break;

                        /* This action will add to our `result` string, expanding
                         the buffer as necessary in reasonable chunks. */
                    case a_WRITE:
                        if ( !writer )
                            writer_len = 1;
                        else
                            writer_len = strlen( writer );
                        /* Lazy buffer creation */
                        if ( !result )
                        {
                            l_result = writer_len + g_result;
                            result = calloc(l_result, 1);
                        }
                        /* Grow the buffer if needed */
                        if ( i_result + writer_len >= l_result )
                        {
                            l_result = l_result + writer_len + g_result;
                            result = realloc(result, l_result);
                        }
                        /* Add current writer to the buffer */
                        if ( !writer )
                        {
                            result[i_result] = c;
                            result[i_result +1] = '\0';
                        }
                        else
                        {
                            strncpy( result + i_result, writer, writer_len );
                        }

                        i_result += writer_len;
                        i++;
                        break;

                        /* This action could be more robust but it serves the
                         current purpose. Cross our fingers and count on our
                         localizers not to give bad HTML descriptions. */
                    case a_ERROR:
                        printf("<Error> The localized string probably has bad HTML.\n");
                        goto EXIT_CLEANLY;

                        /* Just a NOP. */
                    case a_NIL:
                        break;

                        /* The default case also handles the CONSUME action. */
                    default:
                        i++;
                        break;
                }

                state = transition.next_state;
                break;
            }
            j++;
        }
    } while ( description[i] );

EXIT_CLEANLY:

    if ( name )
        free(name);
    return result;
}


/** Handles the -help-option service.
 */
static void optionDescribe(TidyDoc tdoc, /**< The Tidy Document */
                           char *option  /**< The name of the option. */
                           )
{
    tmbstr result = NULL;
    Bool allocated = no;
    TidyOptionId topt = tidyOptGetIdForName( option );
    uint tcat = tidyOptGetCategory( tidyGetOption(tdoc, topt));

    if (topt < N_TIDY_OPTIONS && tcat != TidyInternalCategory )
    {
        result = cleanup_description( tidyOptGetDoc( tdoc, tidyGetOption( tdoc, topt ) ) );
        allocated = yes;
    }
    else
    {
        result = (tmbstr)tidyLocalizedString(TC_STRING_UNKNOWN_OPTION_B);
    }

    printf( "\n" );
    printf( "`--%s`\n\n", option );
    print1Column( "%-78.78s\n", 78, result );
    printf( "\n" );
    if ( allocated )
        free ( result );
}


/** @} end service_help_option group */
/* MARK: - Provide the -lang help Service */
/***************************************************************************//**
 ** @defgroup service_lang_help Provide the -lang help Service
 *******************************************************************************
 ** @{
 */


/** Prints the Windows language names that Tidy recognizes, using the specified
 ** format string.
 ** @param format A format string used to display the Windows language names,
 **        or NULL to use the built-in default format.
 */
void tidyPrintWindowsLanguageNames( ctmbstr format )
{
    const tidyLocaleMapItem *item;
    TidyIterator i = getWindowsLanguageList();
    ctmbstr winName;
    ctmbstr posixName;

    while (i) {
        item = getNextWindowsLanguage(&i);
        winName = TidyLangWindowsName( item );
        posixName = TidyLangPosixName( item );
        if ( format )
            printf( format, winName, posixName );
        else
            printf( "%-20s -> %s\n", winName, posixName );
    }
}


/** Prints the languages the are currently built into Tidy, using the specified
 ** format string.
 ** @param format A format string used to display the Windows language names,
 **        or NULL to use the built-in default format.
 */
void tidyPrintTidyLanguageNames( ctmbstr format )
{
    ctmbstr item;
    TidyIterator i = getInstalledLanguageList();

    while (i) {
        item = getNextInstalledLanguage(&i);
        if ( format )
            printf( format, item );
        else
            printf( "%s\n", item );
    }
}


/** Handles the -lang help service.
 ** @remark We will not support console word wrapping for the tables. If users
 **         really have a small console, then they should make it wider or
 **         output to a file.
 ** @param tdoc The Tidy document.
 */
static void lang_help( TidyDoc tdoc )
{
    printf( "\n" );
    printf( "%s", tidyLocalizedString(TC_TXT_HELP_LANG_1) );
    printf( "\n" );
    tidyPrintWindowsLanguageNames("  %-20s -> %s\n");
    printf( "\n" );
    printf( "%s", tidyLocalizedString(TC_TXT_HELP_LANG_2) );
    printf( "\n" );
    tidyPrintTidyLanguageNames("  %s\n");
    printf( "\n" );
    printf( tidyLocalizedString(TC_TXT_HELP_LANG_3), tidyGetLanguage() );
    printf( "\n" );
}


/** @} end service_lang_help group */
/* MARK: - Provide the -show-config Service */
/***************************************************************************//**
 ** @defgroup service_show_config Provide the -show-config Service
 *******************************************************************************
 ** @{
 */


/** Prints the option value for a given option.
 */
static void printOptionValues(TidyDoc ARG_UNUSED(tdoc),  /**< The Tidy document. */
                              TidyOption topt,           /**< The option for which to show values. */
                              OptionDesc *d              /**< The OptionDesc array. */
                              )
{
    TidyOptionId optId = tidyOptGetId( topt );

    if ( tidyOptGetCategory(topt) == TidyInternalCategory )
        return;

    switch ( optId )
    {
        case TidyInlineTags:
        case TidyBlockTags:
        case TidyEmptyTags:
        case TidyPreTags:
        {
            TidyIterator pos = tidyOptGetDeclTagList( tdoc );
            while ( pos )
            {
                d->def = tidyOptGetNextDeclTag(tdoc, optId, &pos);
                if ( pos )
                {
                    printf( fmt, d->name, d->type, d->def );
                    d->name = "";
                    d->type = "";
                }
            }
        }
            break;
        case TidyPriorityAttributes: /* Is #697 - This case seems missing */
        {
            TidyIterator itAttr = tidyOptGetPriorityAttrList(tdoc);
            if (itAttr && (itAttr != (TidyIterator)-1))
            {
                while (itAttr)
                {
                    d->def = tidyOptGetNextPriorityAttr(tdoc, &itAttr);
                    if (itAttr)
                    {
                        printf(fmt, d->name, d->type, d->def);
                        d->name = "";
                        d->type = "";
                    }
                }
            }
        }
            break;
        default:
            break;
    }

    /* fix for http://tidy.sf.net/bug/873921 */
    if ( *d->name || *d->type || (d->def && *d->def) )
    {
        if ( ! d->def )
            d->def = "";
        printf( fmt, d->name, d->type, d->def );
    }
}

/** Handles the -show-config service.
 ** @remark We will not support console word wrapping for the table. If users
 **         really have a small console, then they should make it wider or
 **         output to a file.
 ** @param tdoc The Tidy Document.
 */
static void optionvalues( TidyDoc tdoc )
{
    printf( "\n%s\n", tidyLocalizedString(TC_STRING_CONF_HEADER) );
    printf( fmt, tidyLocalizedString(TC_STRING_CONF_NAME),
           tidyLocalizedString(TC_STRING_CONF_TYPE),
           tidyLocalizedString(TC_STRING_CONF_VALUE) );
    printf( fmt, ul, ul, ul );

    ForEachSortedOption( tdoc, printOptionValues );
}


/** @} end service_show_config group */
/* MARK: - Provide the -export-config Services */
/***************************************************************************//**
 ** @defgroup service_export_config Provide the -export-config Services
 *******************************************************************************
 ** @{
 */

/* Is #697 - specialised service to 'invert' a buffers content
   split on a space character */
static void invertBuffer(TidyBuffer *src, TidyBuffer *dst)
{
    uint len = src->size;
    char *in = (char *)src->bp;
    char *cp;
    if (!in)
        return;
    while (len)
    {
        unsigned char uc;
        len--;
        uc = in[len];
        if (uc == ' ')
        {
            in[len] = 0;
            cp = &in[len + 1];
            if (dst->size)
                tidyBufAppend(dst, " ", 1);
            tidyBufAppend(dst, cp, strlen(cp));
        }
    }
    if (dst->size)
        tidyBufAppend(dst, " ", 1);
    tidyBufAppend(dst, in, strlen(in));
}

/** Prints the option value for a given option.
 */
static void printOptionExportValues(TidyDoc ARG_UNUSED(tdoc),  /**< The Tidy document. */
                                    TidyOption topt,           /**< The option for which to show values. */
                                    OptionDesc *d              /**< The OptionDesc array. */
                                    )
{
    TidyOptionId optId = tidyOptGetId( topt );
    TidyBuffer buf1, buf2;

    if ( tidyOptGetCategory(topt) == TidyInternalCategory )
        return;

    switch ( optId )
    {
        case TidyInlineTags:
        case TidyBlockTags:
        case TidyEmptyTags:
        case TidyPreTags:
        {
            TidyIterator pos = tidyOptGetDeclTagList( tdoc );
            if ( pos )  /* Is #697 - one or more values */
            {
                tidyBufInit(&buf1);
                tidyBufInit(&buf2);
                while (pos)
                {
                    d->def = tidyOptGetNextDeclTag(tdoc, optId, &pos);
                    if (d->def)
                    {
                        if (buf1.size)
                            tidyBufAppend(&buf1, " ", 1);
                        tidyBufAppend(&buf1, (void *)d->def, strlen(d->def));
                    }
                }
                invertBuffer(&buf1, &buf2); /* Is #697 - specialised service to invert words */
                tidyBufAppend(&buf2, (void *)"\0", 1); /* is this really required? */
                printf("%s: %s\n", d->name, buf2.bp);
                d->name = "";
                d->type = "";
                d->def = 0;
                tidyBufFree(&buf1);
                tidyBufFree(&buf2);
            }
        }
            break;
        case TidyPriorityAttributes: /* Is #697 - This case seems missing */
        {
            TidyIterator itAttr = tidyOptGetPriorityAttrList(tdoc);
            if (itAttr && (itAttr != (TidyIterator)-1))
            {
                tidyBufInit(&buf1);
                while (itAttr)
                {
                    d->def = tidyOptGetNextPriorityAttr(tdoc, &itAttr);
                    if (d->def)
                    {
                        if (buf1.size)
                            tidyBufAppend(&buf1, " ", 1);
                        tidyBufAppend(&buf1, (void *)d->def, strlen(d->def));
                    }
                }
                tidyBufAppend(&buf1, (void *)"\0", 1); /* is this really required? */
                printf("%s: %s\n", d->name, buf1.bp);
                d->name = "";
                d->type = "";
                d->def = 0;
                tidyBufFree(&buf1);
            }
        }
        break;
        default:
            break;
    }

    /* fix for http://tidy.sf.net/bug/873921 */
    if ( *d->name || *d->type || (d->def && *d->def) )
    {
        if ( ! d->def )
            d->def = "";
        printf( "%s: %s\n", d->name, d->def );
    }
}

/** Handles the -export-config service.
 ** @param tdoc The Tidy Document.
 */
static void exportOptionValues( TidyDoc tdoc )
{
    ForEachSortedOption( tdoc, printOptionExportValues );
}

/** Handles the -export-default-config service.
 ** @param tdoc The Tidy Document.
 */
static void exportDefaultOptionValues( TidyDoc tdoc )
{
    tidyOptResetAllToDefault( tdoc );
    ForEachSortedOption( tdoc, printOptionExportValues );
}


/** @} end service_export_config group */
/* MARK: - Provide the -version Service */
/***************************************************************************//**
 ** @defgroup service_version Provide the -version Service
 *******************************************************************************
 ** @{
 */


/** Handles the -version service.
 */
static void version( TidyDoc tdoc )
{
    if ( tidyPlatform() )
        printf( tidyLocalizedString( TC_STRING_VERS_A ), tidyPlatform(), tidyLibraryVersion() );
    else
        printf( tidyLocalizedString( TC_STRING_VERS_B ), tidyLibraryVersion() );

    printf("\n");
}


/** @} end service_version group */
/* MARK: - Provide the -xml-config Service */
/***************************************************************************//**
 ** @defgroup service_xml_config Provide the -xml-config Service
 *******************************************************************************
 ** @{
 */


/** Prints for XML an option's <description>.
 */
static void printXMLDescription(TidyDoc tdoc,   /**< The Tidy document. */
                                TidyOption topt /**< The option. */
                                )
{
    ctmbstr doc = tidyOptGetDoc( tdoc, topt );

    if (doc)
        printf("  <description>%s</description>\n", doc);
    else
    {
        printf("  <description />\n");
        fprintf(stderr, tidyLocalizedString(TC_STRING_OPT_NOT_DOCUMENTED),
                tidyOptGetName( topt ));
        fprintf(stderr, "\n");

    }
}

/** Prints for XML an option's `<seealso>`.
 */
static void printXMLCrossRef(TidyDoc tdoc,   /**< The Tidy document. */
                             TidyOption topt /**< The option. */
                             )
{
    TidyOption optLinked;
    TidyIterator pos = tidyOptGetDocLinksList(tdoc, topt);
    while( pos )
    {
        optLinked = tidyOptGetNextDocLinks(tdoc, &pos );
        printf("  <seealso>%s</seealso>\n",tidyOptGetName(optLinked));
    }
}


/** Prints for XML an option's `<eqconfig>`.
 */
static void printXMLCrossRefEqConsole(TidyDoc tdoc,   /**< The Tidy document. */
                                      TidyOption topt /**< The option. */
                                      )
{
    const CmdOptDesc* pos = cmdopt_defs;
    const CmdOptDesc* hit = NULL;
    CmdOptDesc localHit;
    enum { sizeBuffer = 50 }; /* largest config name is 27 chars so far... */
    char buffer[sizeBuffer];

    for( pos=cmdopt_defs; pos->name1; ++pos)
    {
        snprintf(buffer, sizeBuffer, "%s:", tidyOptGetName( topt ));
        if ( pos->eqconfig && (strncmp(buffer, pos->eqconfig, strlen(buffer)) == 0) )
        {
            hit = pos;
            break;
        }
    }

    if ( hit )
    {
        tmbstr localName;
        localHit = *hit;
        localize_option_names( &localHit );
        printf("  <eqconsole>%s</eqconsole>\n", localName = get_escaped_name(localHit.name1));
        free((tmbstr)localHit.name1);
        free(localName);
        if ( localHit.name2 )
        {
            printf("  <eqconsole>%s</eqconsole>\n", localName = get_escaped_name(localHit.name2));
            free((tmbstr)localHit.name2);
            free(localName);
        }
        if ( localHit.name3 )
        {
            printf("  <eqconsole>%s</eqconsole>\n", localName = get_escaped_name(localHit.name3));
            free((tmbstr)localHit.name3);
            free(localName);
        }
        if ( localHit.eqconfig ) /* Is. #791 */
        {
            free((tmbstr)localHit.eqconfig);
        }

    }
    else
        printf("  %s\n", "  <eqconsole />");
}


/** Prints for XML an option.
 */
static void printXMLOption(TidyDoc tdoc,    /**< The Tidy document. */
                           TidyOption topt, /**< The option. */
                           OptionDesc *d    /**< The OptionDesc for the option. */
                           )
{
    if ( tidyOptGetCategory(topt) == TidyInternalCategory )
        return;

    printf( " <option class=\"%s\">\n", d->cat );
    printf  ("  <name>%s</name>\n",d->name);
    printf  ("  <type>%s</type>\n",d->type);
    if (d->def)
        printf("  <default>%s</default>\n",d->def);
    else
        printf("  <default />\n");
    if (d->haveVals)
    {
        printf("  <example>");
        PrintAllowedValues( topt, d );
        printf("</example>\n");
    }
    else
    {
        printf("  <example />\n");
    }
    printXMLDescription( tdoc, topt );
    printXMLCrossRef( tdoc, topt );
    printXMLCrossRefEqConsole( tdoc, topt );
    printf( " </option>\n" );
}


/** Handles the -xml-config service.
 ** @param tdoc The Tidy document.
 */
static void XMLoptionhelp( TidyDoc tdoc )
{
    printf( "<?xml version=\"1.0\"?>\n"
           "<config version=\"%s\">\n", tidyLibraryVersion());
    ForEachOption( tdoc, printXMLOption );
    printf( "</config>\n" );
}


/** @} end service_xml_config group */
/* MARK: - Provide the -xml-error-strings Service */
/***************************************************************************//**
 ** @defgroup service_xml_error_strings Provide the -xml-error-strings Service
 *******************************************************************************
 ** @{
 */


/** Handles the -xml-error-strings service.
 ** This service is primarily helpful to developers who need to generate an
 ** updated list of strings to expect when using one of the message callbacks.
 ** Included in the output is the current string associated with the error
 ** symbol.
 ** @param tdoc The Tidy document.
 **/
static void xml_error_strings( TidyDoc tdoc )
{
    uint errorCode;
    ctmbstr localizedString;
    TidyIterator j = getErrorCodeList();

    printf( "<?xml version=\"1.0\"?>\n" );
    printf( "<error_strings version=\"%s\">\n", tidyLibraryVersion());

    while (j) {
        errorCode = getNextErrorCode(&j);
        localizedString = tidyLocalizedString(errorCode);
        printf( " <error_string>\n" );
        printf( "  <name>%s</name>\n", tidyErrorCodeAsKey(errorCode));
        if ( localizedString )
            printf( "  <string class=\"%s\"><![CDATA[%s]]></string>\n", tidyGetLanguage(), localizedString );
        else
            printf( "  <string class=\"%s\">NULL</string>\n", tidyGetLanguage() );

        printf( " </error_string>\n" );
    }

    printf( "</error_strings>\n" );
}


/** @} end service_xml_error_strings group */
/* MARK: - Provide the -xml-help Service */
/***************************************************************************//**
 ** @defgroup service_xmlhelp Provide the -xml-help Service
 *******************************************************************************
 ** @{
 */

/** Outputs an XML element for a CLI option, escaping special characters as
 ** required. For example, it might print `<name>-output &lt;file&gt;</name>`.
 */
static void print_xml_help_option_element(ctmbstr element, /**< XML element name. */
                                          ctmbstr name     /**< The contents of the element. */
                                          )
{
    tmbstr escpName;
    if (!name)
        return;

    printf("  <%s>%s</%s>\n", element, escpName = get_escaped_name(name), element);
    free(escpName);
}

/** Provides the -xml-help service.
 */
static void xml_help( void )
{
    const CmdOptDesc* pos;
    CmdOptDesc localPos;

    printf( "<?xml version=\"1.0\"?>\n"
           "<cmdline version=\"%s\">\n", tidyLibraryVersion());

    for( pos=cmdopt_defs; pos->name1; ++pos)
    {
        localPos = *pos;
        localize_option_names(&localPos);
        printf(" <option class=\"%s\">\n", cmdopt_catname[pos->cat].mnemonic );
        print_xml_help_option_element("name", localPos.name1);
        print_xml_help_option_element("name", localPos.name2);
        print_xml_help_option_element("name", localPos.name3);
        print_xml_help_option_element("description", tidyLocalizedString( pos->key ) );
        if (pos->eqconfig)
            print_xml_help_option_element("eqconfig", localPos.eqconfig);
        else
            printf("  <eqconfig />\n");
        printf(" </option>\n");

        if (localPos.name1) free((tmbstr)localPos.name1);
        if (localPos.name2) free((tmbstr)localPos.name2);
        if (localPos.name3) free((tmbstr)localPos.name3);
        if (localPos.eqconfig) free((tmbstr)localPos.eqconfig); /* Is. #791 */
    }

    printf( "</cmdline>\n" );
}


/** @} end service_xmlhelp group */
/* MARK: - Provide the -xml-options-strings Service */
/***************************************************************************//**
 ** @defgroup service_xml_opts_strings Provide the -xml-options-strings Service
 *******************************************************************************
 ** @{
 */


/** Handles printing of option description for -xml-options-strings service.
 **/
static void printXMLOptionString(TidyDoc tdoc,    /**< The Tidy document. */
                                 TidyOption topt, /**< The option. */
                                 OptionDesc *d    /**< The OptionDesc array. */
                                 )
{
    if ( tidyOptGetCategory(topt) == TidyInternalCategory )
        return;

    printf( " <option>\n" );
    printf( "  <name>%s</name>\n",d->name);
    printf( "  <string class=\"%s\"><![CDATA[%s]]></string>\n", tidyGetLanguage(), tidyOptGetDoc( tdoc, topt ) );
    printf( " </option>\n" );
}


/** Handles the -xml-options-strings service.
 ** This service is primarily helpful to developers and localizers to test
 ** that option description strings as represented on screen output are
 ** correct and do not break tidy.
 ** @param tdoc The Tidy document.
 */
static void xml_options_strings( TidyDoc tdoc )
{
    printf( "<?xml version=\"1.0\"?>\n"
           "<options_strings version=\"%s\">\n", tidyLibraryVersion());
    ForEachOption( tdoc, printXMLOptionString);
    printf( "</options_strings>\n" );
}


/** @} end service_xml_opts_strings group */
/* MARK: - Provide the -xml-strings Service */
/***************************************************************************//**
 ** @defgroup service_xml_strings Provide the -xml-strings Service
 *******************************************************************************
 ** @{
 */


/** Handles the -xml-strings service.
 ** This service was primarily helpful to developers and localizers to compare
 ** localized strings to the built in `en` strings. It's probably better to use
 ** our POT/PO workflow with your favorite tools, or simply diff the language
 ** header files directly.
 ** @note The attribute `id` is not a specification, promise, or part of an
 **       API. You must not depend on this value. For strings meant for error
 **       output, the `label` attribute will contain the stringified version of
 **       the internal key for the string.
 */
static void xml_strings( void )
{
    uint i;
    TidyIterator j;

    ctmbstr current_language = tidyGetLanguage();
    ctmbstr current_label;
    Bool skip_current = strcmp( current_language, "en" ) == 0;
    Bool matches_base;

    printf( "<?xml version=\"1.0\"?>\n"
           "<localized_strings version=\"%s\">\n", tidyLibraryVersion());

    j = getStringKeyList();
    while (j) {
        i = getNextStringKey(&j);
        current_label = tidyErrorCodeAsKey(i);
        if (!strcmp(current_label, "UNDEFINED"))
            current_label = "";
        printf( "<localized_string id=\"%u\" label=\"%s\">\n", i, current_label );
        printf( " <string class=\"%s\">", "en" );
        printf("%s", tidyDefaultString(i));
        printf( "</string>\n" );
        if ( !skip_current ) {
            matches_base = strcmp( tidyLocalizedString(i), tidyDefaultString(i) ) == 0;
            printf( " <string class=\"%s\" same_as_base=\"%s\">", tidyGetLanguage(), matches_base ? "yes" : "no" );
            printf("%s", tidyLocalizedString(i));
            printf( "</string>\n" );
        }
        printf( "</localized_string>\n");
    }

    printf( "</localized_strings>\n" );
}


/** @} end service_xml_strings group */
/* MARK: - Experimental Stuff */
/***************************************************************************//**
 ** @defgroup experimental_stuff Experimental Stuff
 ** From time to time the developers might leave stuff here that you can use
 ** to experiment on their own, or that they're using to experiment with.
 *******************************************************************************
 ** @{
 */


/** This callback from LibTidy allows the console application to examine an
 ** error message before allowing LibTidy to display it. Currently the body
 ** of the function is not compiled into Tidy, but if you're interested in
 ** how to use the new message API, then enable it. Possible applications in
 ** future console Tidy might be to do things like:
 ** - allow user-defined filtering
 ** - sort the report output by line number
 ** - other things that are user facing and best not put into LibTidy
 **   proper.
 */
static Bool reportCallback(TidyMessage tmessage)
{
#if 0
    TidyIterator pos;
    TidyMessageArgument arg;
    TidyFormatParameterType messageType;
    ctmbstr messageFormat;

    printf("FILTER: %s\n%s\n%s\n", tidyGetMessageKey( tmessage ), tidyGetMessageOutput( tmessage ), tidyGetMessageOutputDefault( tmessage ));

    /* loop through the arguments, if any, and print their details */
    pos = tidyGetMessageArguments( tmessage );
    while ( pos )
    {
        arg = tidyGetNextMessageArgument( tmessage, &pos );
        messageType = tidyGetArgType( tmessage, &arg );
        messageFormat = tidyGetArgFormat( tmessage, &arg );
        printf( "  Type = %u, Format = %s, Value = ", messageType, messageFormat );

        switch (messageType)
        {
            case tidyFormatType_STRING:
                printf("%s\n", tidyGetArgValueString( tmessage, &arg ));
                break;

            case tidyFormatType_INT:
                printf("%d\n", tidyGetArgValueInt( tmessage, &arg));
                break;

            case tidyFormatType_UINT:
                printf("%u\n", tidyGetArgValueUInt( tmessage, &arg));
                break;

            case tidyFormatType_DOUBLE:
                printf("%g\n", tidyGetArgValueDouble( tmessage, &arg));
                break;

            default:
                printf("%s", "unknown so far\n");
        }
    }

    return no;  /* suppress LibTidy's own output of this message */
#else
    return yes; /* needed so Tidy will not block output of this message */
#endif
}


/** @} end experimental_stuff group */
/* MARK: - main() */
/***************************************************************************//**
 ** @defgroup main Main
 ** Let's do something here!
 *******************************************************************************
 ** @{
 */


int main( int argc, char** argv )
{
    ctmbstr prog = argv[0];
    ctmbstr cfgfil = NULL, errfil = NULL, htmlfil = NULL;
    TidyDoc tdoc = NULL;
    int status = 0;

    uint contentErrors = 0;
    uint contentWarnings = 0;
    uint accessWarnings = 0;

#if defined(ENABLE_DEBUG_LOG) && defined(_MSC_VER)
#  if defined(_CRTDBG_MAP_ALLOC)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#  endif
#endif

    tdoc = tidyCreate();

    tidySetMessageCallback( tdoc, reportCallback); /* experimental group */
    errout = stderr;  /* initialize to stderr */

    /* Set an atexit handler. */
    atexit( tidy_cleanup );

#if defined(_WIN32)
    /* Force Windows console to use UTF, otherwise many characters will
     * be garbage. Note that East Asian languages *are* supported, but
     * only when Windows OS locale (not console only!) is set to an
     * East Asian language.
     */
    win_cp = GetConsoleOutputCP();
    SetConsoleOutputCP(CP_UTF8);
#endif

    /*
     * Look for default configuration files using any of
     * the following possibilities:
     *  - TIDY_CONFIG_FILE - from tidyplatform.h, typically /etc/tidy.conf
     *  - HTML_TIDY        - environment variable
     *  - TIDY_USER_CONFIG_FILE - from tidyplatform.h, typically ~/tidy.conf
     */

#ifdef TIDY_CONFIG_FILE
    if ( tidyFileExists( tdoc, TIDY_CONFIG_FILE) )
    {
        status = tidyLoadConfig( tdoc, TIDY_CONFIG_FILE );
        if ( status != 0 ) {
            fprintf(errout, tidyLocalizedString( TC_MAIN_ERROR_LOAD_CONFIG ), TIDY_CONFIG_FILE, status);
            fprintf(errout, "\n");
        }
    }
#endif /* TIDY_CONFIG_FILE */

    if ( (cfgfil = getenv("HTML_TIDY")) != NULL )
    {
        status = tidyLoadConfig( tdoc, cfgfil );
        if ( status != 0 ) {
            fprintf(errout, tidyLocalizedString( TC_MAIN_ERROR_LOAD_CONFIG ), cfgfil, status);
            fprintf(errout, "\n");
        }
    }
#ifdef TIDY_USER_CONFIG_FILE
    else if ( tidyFileExists( tdoc, TIDY_USER_CONFIG_FILE) )
    {
        status = tidyLoadConfig( tdoc, TIDY_USER_CONFIG_FILE );
        if ( status != 0 ) {
            fprintf(errout, tidyLocalizedString( TC_MAIN_ERROR_LOAD_CONFIG ), TIDY_USER_CONFIG_FILE, status);
            fprintf(errout, "\n");
        }
    }
#endif /* TIDY_USER_CONFIG_FILE */


    /*
     * Read command line
     */

    while ( argc > 0 )
    {
        if (argc > 1 && argv[1][0] == '-')
        {
            /* support -foo and --foo */
            ctmbstr arg = argv[1] + 1;

            if ( strcasecmp(arg, "xml") == 0)
                tidyOptSetBool( tdoc, TidyXmlTags, yes );

            else if ( strcasecmp(arg,   "asxml") == 0 ||
                     strcasecmp(arg, "asxhtml") == 0 )
            {
                tidyOptSetBool( tdoc, TidyXhtmlOut, yes );
            }
            else if ( strcasecmp(arg,   "ashtml") == 0 )
                tidyOptSetBool( tdoc, TidyHtmlOut, yes );

            else if ( strcasecmp(arg, "indent") == 0 )
            {
                tidyOptSetInt( tdoc, TidyIndentContent, TidyAutoState );
                if ( tidyOptGetInt(tdoc, TidyIndentSpaces) == 0 )
                    tidyOptResetToDefault( tdoc, TidyIndentSpaces );
            }
            else if ( strcasecmp(arg, "omit") == 0 )
                tidyOptSetBool( tdoc, TidyOmitOptionalTags, yes );

            else if ( strcasecmp(arg, "upper") == 0 )
                tidyOptSetBool( tdoc, TidyUpperCaseTags, yes );

            else if ( strcasecmp(arg, "clean") == 0 )
                tidyOptSetBool( tdoc, TidyMakeClean, yes );

            else if ( strcasecmp(arg, "gdoc") == 0 )
                tidyOptSetBool( tdoc, TidyGDocClean, yes );

            else if ( strcasecmp(arg, "bare") == 0 )
                tidyOptSetBool( tdoc, TidyMakeBare, yes );

            else if ( strcasecmp(arg, "raw") == 0     ||
                     strcasecmp(arg, "ascii") == 0    ||
                     strcasecmp(arg, "latin0") == 0   ||
                     strcasecmp(arg, "latin1") == 0   ||
                     strcasecmp(arg, "utf8") == 0     ||
#ifndef NO_NATIVE_ISO2022_SUPPORT
                     strcasecmp(arg, "iso2022") == 0  ||
#endif
                     strcasecmp(arg, "utf16le") == 0  ||
                     strcasecmp(arg, "utf16be") == 0  ||
                     strcasecmp(arg, "utf16") == 0    ||
                     strcasecmp(arg, "shiftjis") == 0 ||
                     strcasecmp(arg, "big5") == 0     ||
                     strcasecmp(arg, "mac") == 0      ||
                     strcasecmp(arg, "win1252") == 0  ||
                     strcasecmp(arg, "ibm858") == 0 )
            {
                tidySetCharEncoding( tdoc, arg );
            }
            else if ( strcasecmp(arg, "numeric") == 0 )
                tidyOptSetBool( tdoc, TidyNumEntities, yes );

            else if ( strcasecmp(arg, "modify") == 0 ||
                     strcasecmp(arg, "change") == 0 ||  /* obsolete */
                     strcasecmp(arg, "update") == 0 )   /* obsolete */
            {
                tidyOptSetBool( tdoc, TidyWriteBack, yes );
            }
            else if ( strcasecmp(arg, "errors") == 0 )
                tidyOptSetBool( tdoc, TidyShowMarkup, no );

            else if ( strcasecmp(arg, "quiet") == 0 )
                tidyOptSetBool( tdoc, TidyQuiet, yes );

            /* Currently user must specify a language
             prior to anything that causes output */
            else if ( strcasecmp(arg, "language") == 0 ||
                     strcasecmp(arg,     "lang") == 0 )
                if ( argc >= 3)
                {
                    if ( strcasecmp(argv[2], "help") == 0 )
                    {
                        lang_help( tdoc );
                        exit(0);
                    }
                    if ( !tidySetLanguage( argv[2] ) )
                    {
                        printf(tidyLocalizedString(TC_STRING_LANG_NOT_FOUND),
                               argv[2], tidyGetLanguage());
                        printf("\n");
                    }
                    --argc;
                    ++argv;
                }
                else
                {
                    printf( "%s\n", tidyLocalizedString(TC_STRING_LANG_MUST_SPECIFY));
                }

            else if ( strcasecmp(arg, "help") == 0 ||
                        strcasecmp(arg, "-help") == 0 ||
                        strcasecmp(arg,    "h") == 0 || *arg == '?' )
            {
                help( tdoc, prog );
                tidyRelease( tdoc );
                return 0; /* success */
            }
            else if ( strcasecmp(arg, "xml-help") == 0)
            {
                xml_help( );
                tidyRelease( tdoc );
                return 0; /* success */
            }
            else if ( strcasecmp(arg, "xml-error-strings") == 0)
            {
                xml_error_strings( tdoc );
                tidyRelease( tdoc );
                return 0; /* success */
            }
            else if ( strcasecmp(arg, "xml-options-strings") == 0)
            {
                xml_options_strings( tdoc );
                tidyRelease( tdoc );
                return 0; /* success */
            }
            else if ( strcasecmp(arg, "xml-strings") == 0)
            {
                xml_strings( );
                tidyRelease( tdoc );
                return 0; /* success */
            }
            else if ( strcasecmp(arg, "help-config") == 0 )
            {
                optionhelp( tdoc );
                tidyRelease( tdoc );
                return 0; /* success */
            }
            else if ( strcasecmp(arg, "help-env") == 0 )
            {
                helpEnv( tdoc );
                tidyRelease( tdoc );
                return 0; /* success */
            }
            else if ( strcasecmp(arg, "help-option") == 0 )
            {
                if ( argc >= 3)
                {
                    optionDescribe( tdoc, argv[2] );
                }
                else
                {
                    printf( "%s\n", tidyLocalizedString(TC_STRING_MUST_SPECIFY));
                }
                tidyRelease( tdoc );
                return 0; /* success */
            }
            else if ( strcasecmp(arg, "xml-config") == 0 )
            {
                XMLoptionhelp( tdoc );
                tidyRelease( tdoc );
                return 0; /* success */
            }
            else if ( strcasecmp(arg, "show-config") == 0 )
            {
                optionvalues( tdoc );
                tidyRelease( tdoc );
                return 0; /* success */
            }
            else if ( strcasecmp(arg, "export-config") == 0 )
            {
                exportOptionValues( tdoc );
                tidyRelease( tdoc );
                return 0; /* success */
            }
            else if ( strcasecmp(arg, "export-default-config") == 0 )
            {
                exportDefaultOptionValues( tdoc );
                tidyRelease( tdoc );
                return 0; /* success */
            }
            else if ( strcasecmp(arg, "config") == 0 )
            {
                if ( argc >= 3 )
                {
                    ctmbstr post;

                    tidyLoadConfig( tdoc, argv[2] );

                    /* Set new error output stream if setting changed */
                    post = tidyOptGetValue( tdoc, TidyErrFile );
                    if ( post && (!errfil || !samefile(errfil, post)) )
                    {
                        errfil = post;
                        errout = tidySetErrorFile( tdoc, post );
                    }

                    --argc;
                    ++argv;
                }
            }

            else if ( strcasecmp(arg, "output") == 0 ||
                        strcasecmp(arg, "-output-file") == 0 ||
                        strcasecmp(arg, "o") == 0 )
            {
                if ( argc >= 3 )
                {
                    tidyOptSetValue( tdoc, TidyOutFile, argv[2] );
                    --argc;
                    ++argv;
                }
            }
            else if ( strcasecmp(arg,  "file") == 0 ||
                        strcasecmp(arg, "-file") == 0 ||
                        strcasecmp(arg,     "f") == 0 )
            {
                if ( argc >= 3 )
                {
                    errfil = argv[2];
                    errout = tidySetErrorFile( tdoc, errfil );
                    --argc;
                    ++argv;
                }
            }
            else if ( strcasecmp(arg,  "wrap") == 0 ||
                        strcasecmp(arg, "-wrap") == 0 ||
                        strcasecmp(arg,     "w") == 0 )
            {
                if ( argc >= 3 )
                {
                    uint wraplen = 0;
                    int nfields = sscanf( argv[2], "%u", &wraplen );
                    tidyOptSetInt( tdoc, TidyWrapLen, wraplen );
                    if (nfields > 0)
                    {
                        --argc;
                        ++argv;
                    }
                }
            }
            else if ( strcasecmp(arg,  "version") == 0 ||
                        strcasecmp(arg, "-version") == 0 ||
                        strcasecmp(arg,        "v") == 0 )
            {
                version( tdoc );
                tidyRelease( tdoc );
                return 0;  /* success */

            }
            else if ( strncmp(argv[1], "--", 2 ) == 0)
            {
                if ( tidyOptParseValue(tdoc, argv[1]+2, argv[2]) )
                {
                    /* Set new error output stream if setting changed */
                    ctmbstr post = tidyOptGetValue( tdoc, TidyErrFile );
                    if ( post && (!errfil || !samefile(errfil, post)) )
                    {
                        errfil = post;
                        errout = tidySetErrorFile( tdoc, post );
                    }

                    ++argv;
                    --argc;
                }
            }
                else if ( strcasecmp(arg, "access") == 0 )
                {
                    if ( argc >= 3 )
                    {
                        uint acclvl = 0;
                        int nfields = sscanf( argv[2], "%u", &acclvl );
                        tidyOptSetInt( tdoc, TidyAccessibilityCheckLevel, acclvl );
                        if (nfields > 0)
                        {
                            --argc;
                            ++argv;
                        }
                    }
                }

                else
                {
                    uint c;
                    ctmbstr s = argv[1];

                    while ( (c = *++s) != '\0' )
                    {
                        switch ( c )
                        {
                            case 'i':
                                tidyOptSetInt( tdoc, TidyIndentContent, TidyAutoState );
                                if ( tidyOptGetInt(tdoc, TidyIndentSpaces) == 0 )
                                    tidyOptResetToDefault( tdoc, TidyIndentSpaces );
                                break;

                            case 'u':
                                tidyOptSetBool( tdoc, TidyUpperCaseTags, yes );
                                break;

                            case 'c':
                                tidyOptSetBool( tdoc, TidyMakeClean, yes );
                                break;

                            case 'g':
                                tidyOptSetBool( tdoc, TidyGDocClean, yes );
                                break;

                            case 'b':
                                tidyOptSetBool( tdoc, TidyMakeBare, yes );
                                break;

                            case 'n':
                                tidyOptSetBool( tdoc, TidyNumEntities, yes );
                                break;

                            case 'm':
                                tidyOptSetBool( tdoc, TidyWriteBack, yes );
                                break;

                            case 'e':
                                tidyOptSetBool( tdoc, TidyShowMarkup, no );
                                break;

                            case 'q':
                                tidyOptSetBool( tdoc, TidyQuiet, yes );
                                break;

                            default:
                                unknownOption( tdoc, c );
                                break;
                        }
                    }
                }

            --argc;
            ++argv;
            continue;
        }


        if ( argc > 1 )
        {
            htmlfil = argv[1];
#ifdef ENABLE_DEBUG_LOG
            SPRTF("Tidy: '%s'\n", htmlfil);
#else /* !ENABLE_DEBUG_LOG */
            /* Is #713 - show-filename option */
            if (tidyOptGetBool(tdoc, TidyShowFilename))
            {
                fprintf(errout, "Tidy: '%s'", htmlfil);
                fprintf(errout, "\n");
            }
#endif /* ENABLE_DEBUG_LOG yes/no */
            if ( tidyOptGetBool(tdoc, TidyEmacs) || tidyOptGetBool(tdoc, TidyShowFilename))
                tidySetEmacsFile( tdoc, htmlfil );
            status = tidyParseFile( tdoc, htmlfil );
        }
        else
        {
            htmlfil = "stdin";
            status = tidyParseStdin( tdoc );
        }

        if ( status >= 0 )
            status = tidyCleanAndRepair( tdoc );

        if ( status >= 0 ) {
            status = tidyRunDiagnostics( tdoc );
        }
        if ( status > 1 ) /* If errors, do we want to force output? */
            status = ( tidyOptGetBool(tdoc, TidyForceOutput) ? status : -1 );

        if ( status >= 0 && tidyOptGetBool(tdoc, TidyShowMarkup) )
        {
            if ( tidyOptGetBool(tdoc, TidyWriteBack) && argc > 1 )
                status = tidySaveFile( tdoc, htmlfil );
            else
            {
                ctmbstr outfil = tidyOptGetValue( tdoc, TidyOutFile );
                if ( outfil ) {
                    status = tidySaveFile( tdoc, outfil );
                } else {
#ifdef ENABLE_DEBUG_LOG
                    static char tmp_buf[264];
                    sprintf(tmp_buf,"%s.html",get_log_file());
                    status = tidySaveFile( tdoc, tmp_buf );
                    SPRTF("Saved tidied content to '%s'\n",tmp_buf);
#else
                    status = tidySaveStdout( tdoc );
#endif
                }
            }
        }

        contentErrors   += tidyErrorCount( tdoc );
        contentWarnings += tidyWarningCount( tdoc );
        accessWarnings  += tidyAccessWarningCount( tdoc );

        --argc;
        ++argv;

        if ( argc <= 1 )
            break;
    } /* read command line loop */

    /* blank line for screen formatting */
    if ( errout == stderr && !contentErrors && !tidyOptGetBool( tdoc, TidyQuiet ) )
        fprintf(errout, "\n");

    /* footnote printing only if errors or warnings */
    if ( contentErrors + contentWarnings > 0 )
        tidyErrorSummary(tdoc);

    /* prints the general info, if applicable */
    tidyGeneralInfo(tdoc);

    /* called to free hash tables etc. */
    tidyRelease( tdoc );

    /* return status can be used by scripts */
    if ( contentErrors > 0 )
        return 2;

    if ( contentWarnings > 0 )
        return 1;

    /* 0 signifies all is ok */
    return 0;
}


/** @} end main group */
/** @} end console_application group */


/*
 * local variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 4
 * end:
 */
