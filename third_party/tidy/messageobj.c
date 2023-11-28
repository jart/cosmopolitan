/* messageobj.c
 * Provides an external, extensible API for message reporting.
 *
 * (c) 2017 HTACG
 * See tidy.h for the copyright notice.
 */

#include "third_party/tidy/messageobj.h"
#include "third_party/tidy/message.h"
#include "third_party/tidy/tidy-int.h"
#include "libc/assert.h"
#include "third_party/tidy/tmbstr.h"


/*********************************************************************
 * BuildArgArray Support - declarations and forward declarations
 *********************************************************************/


/** A record of a single argument and its type. An array these
**  represents the arguments supplied to a format string, ordered
**  in the same position as they occur in the format string. Because
**  older versions of Windows don't support positional arguments,
**  Tidy doesn't either.
*/

#define FORMAT_LENGTH 21

struct printfArg {
    TidyFormatParameterType type;  /* type of the argument    */
    int formatStart;               /* where the format starts */
    int formatLength;              /* length of the format    */
    char format[FORMAT_LENGTH];    /* buffer for the format   */
    union {                        /* the argument            */
        int i;
        uint ui;
        double d;
        const char *s;
    } u;
};


/** Returns a pointer to an allocated array of `printfArg` given a format
 ** string and a va_list, or NULL if not successful or no parameters were
 ** given. Parameter `rv` will return with the count of zero or more
 ** parameters if successful, else -1.
 **
 */
static struct printfArg *BuildArgArray( TidyDocImpl *doc, ctmbstr fmt, va_list ap, int *rv );


/*********************************************************************
 * Tidy Message Object Support
 *********************************************************************/


/** Create an internal representation of a Tidy message with all of
 ** the information that that we know about the message.
 **
 ** The function signature doesn't have to stay static and is a good
 ** place to add instantiation if expanding the API.
 **
 ** We currently know the doc, node, code, line, column, level, and
 ** args, will pre-calculate all of the other members upon creation.
 ** This ensures that we can use members directly, immediately,
 ** without having to use accessors internally.
 **
 ** If any message callback filters are setup by API clients, they
 ** will be called here.
 **
 ** This version serves as the designated initializer and as such
 ** requires every known parameter.
 */
static TidyMessageImpl *tidyMessageCreateInitV( TidyDocImpl *doc,
                                                Node *node,
                                                uint code,
                                                int line,
                                                int column,
                                                TidyReportLevel level,
                                                va_list args )
{
    TidyMessageImpl *result = TidyDocAlloc(doc, sizeof(TidyMessageImpl));
    TidyDoc tdoc = tidyImplToDoc(doc);
    va_list args_copy;
    enum { sizeMessageBuf=2048 };
    ctmbstr pattern;
    uint i = 0;


    /* Things we know... */

    result->tidyDoc = doc;
    result->tidyNode = node;
    result->code = code;
    result->line = line;
    result->column = column;
    result->level = level;
    /* Is #719 - set 'muted' before any callbacks. */
    result->muted = no;
    i = 0;
    while ((doc->muted.list) && (doc->muted.list[i] != 0))
    {
        if (doc->muted.list[i] == code)
        {
            result->muted = yes;
            break;
        }
        i++;
    }

    /* Things we create... */

    va_copy(args_copy, args);
    result->arguments = BuildArgArray(doc, tidyDefaultString(code), args_copy, &result->argcount);
    va_end(args_copy);

    result->messageKey = TY_(tidyErrorCodeAsKey)(code);

    result->messageFormatDefault = tidyDefaultString(code);
    result->messageFormat = tidyLocalizedString(code);

    result->messageDefault = TidyDocAlloc(doc, sizeMessageBuf);
    va_copy(args_copy, args);
    TY_(tmbvsnprintf)(result->messageDefault, sizeMessageBuf, result->messageFormatDefault, args_copy);
    va_end(args_copy);

    result->message = TidyDocAlloc(doc, sizeMessageBuf);
    va_copy(args_copy, args);
    TY_(tmbvsnprintf)(result->message, sizeMessageBuf, result->messageFormat, args_copy);
    va_end(args_copy);

    /* Some things already hit us localized, and some things need to be
       localized here. Look for these codewords and replace them here.
     */
    TY_(strrep)(result->messageDefault, "STRING_PLAIN_TEXT",      tidyDefaultString(STRING_PLAIN_TEXT));
    TY_(strrep)(result->message,        "STRING_PLAIN_TEXT",      tidyLocalizedString(STRING_PLAIN_TEXT));

    TY_(strrep)(result->messageDefault, "STRING_XML_DECLARATION", tidyDefaultString(STRING_XML_DECLARATION));
    TY_(strrep)(result->message,        "STRING_XML_DECLARATION", tidyLocalizedString(STRING_XML_DECLARATION));

    TY_(strrep)(result->messageDefault, "STRING_ERROR_COUNT_WARNING", tidyDefaultStringN(STRING_ERROR_COUNT_WARNING, doc->warnings));
    TY_(strrep)(result->message,        "STRING_ERROR_COUNT_WARNING", tidyLocalizedStringN(STRING_ERROR_COUNT_WARNING, doc->warnings));

    TY_(strrep)(result->messageDefault, "STRING_ERROR_COUNT_ERROR", tidyDefaultStringN(STRING_ERROR_COUNT_ERROR, doc->errors));
    TY_(strrep)(result->message,        "STRING_ERROR_COUNT_ERROR", tidyLocalizedStringN(STRING_ERROR_COUNT_ERROR, doc->errors));


    result->messagePosDefault = TidyDocAlloc(doc, sizeMessageBuf);
    result->messagePos = TidyDocAlloc(doc, sizeMessageBuf);

    if ( cfgBool(doc, TidyEmacs) && cfgStr(doc, TidyEmacsFile) )
    {
        /* Change formatting to be parsable by GNU Emacs */
        TY_(tmbsnprintf)(result->messagePosDefault, sizeMessageBuf, "%s:%d:%d: ", cfgStr(doc, TidyEmacsFile), line, column);
        TY_(tmbsnprintf)(result->messagePos, sizeMessageBuf, "%s:%d:%d: ", cfgStr(doc, TidyEmacsFile), line, column);
    }
    else if ( cfgBool(doc, TidyShowFilename) && cfgStr(doc, TidyEmacsFile) )
    {
        /* Include filename in output */
        TY_(tmbsnprintf)(result->messagePosDefault, sizeMessageBuf, tidyDefaultString(FN_LINE_COLUMN_STRING),
            cfgStr(doc, TidyEmacsFile), line, column);
        TY_(tmbsnprintf)(result->messagePos, sizeMessageBuf, tidyLocalizedString(FN_LINE_COLUMN_STRING),
            cfgStr(doc, TidyEmacsFile), line, column);
    }
    else
    {
        /* traditional format */
        TY_(tmbsnprintf)(result->messagePosDefault, sizeMessageBuf, tidyDefaultString(LINE_COLUMN_STRING), line, column);
        TY_(tmbsnprintf)(result->messagePos, sizeMessageBuf, tidyLocalizedString(LINE_COLUMN_STRING), line, column);
    }

    result->messagePrefixDefault = tidyDefaultString(level);

    result->messagePrefix = tidyLocalizedString(level);

    if ( line > 0 && column > 0 )
        pattern = "%s%s%s";      /* pattern if there's location information */
    else
        pattern = "%.0s%s%s";    /* otherwise if there isn't */

    if ( level > TidyFatal )
        pattern = "%.0s%.0s%s";  /* dialog doesn't have pos or prefix */

    result->messageOutputDefault = TidyDocAlloc(doc, sizeMessageBuf);
    TY_(tmbsnprintf)(result->messageOutputDefault, sizeMessageBuf, pattern,
                     result->messagePosDefault, result->messagePrefixDefault,
                     result->messageDefault);

    result->messageOutput = TidyDocAlloc(doc, sizeMessageBuf);
    TY_(tmbsnprintf)(result->messageOutput, sizeMessageBuf, pattern,
                     result->messagePos, result->messagePrefix,
                     result->message);

    if ( ( cfgBool(doc, TidyMuteShow) == yes ) && level <= TidyFatal )
    {
        /*\ Issue #655 - Unsafe to use output buffer as one of the va_list
         *  input parameters in some snprintf implementations.
        \*/
        ctmbstr pc = TY_(tidyErrorCodeAsKey)(code);
        i = TY_(tmbstrlen)(result->messageOutputDefault);
        if (i < sizeMessageBuf)
            TY_(tmbsnprintf)(result->messageOutputDefault + i, sizeMessageBuf - i, " (%s)", pc );
        i = TY_(tmbstrlen)(result->messageOutput);
        if (i < sizeMessageBuf)
            TY_(tmbsnprintf)(result->messageOutput + i, sizeMessageBuf - i, " (%s)", pc );
    }

    result->allowMessage = yes;

    /* reportFilter is a simple error filter that provides minimal information
       to callback functions, and includes the message buffer in LibTidy's
       configured localization. As it's a "legacy" API, it does not receive
       TidyDialogue messages.*/
    if ( (result->level <= TidyFatal) && doc->reportFilter )
    {
        result->allowMessage = result->allowMessage & doc->reportFilter( tdoc, result->level, result->line, result->column, result->messageOutput );
    }

    /* reportCallback is intended to allow LibTidy users to localize messages
       via their own means by providing a key and the parameters to fill it.
       As it's a "legacy" API, it does not receive TidyDialogue messages. */
    if ( (result->level <= TidyFatal) && doc->reportCallback )
    {
        TidyDoc tdoc = tidyImplToDoc( doc );
        va_copy(args_copy, args);
        result->allowMessage = result->allowMessage & doc->reportCallback( tdoc, result->level, result->line, result->column, result->messageKey, args_copy );
        va_end(args_copy);
    }

    /* messageCallback is the newest interface to interrogate Tidy's
       emitted messages. */
    if ( doc->messageCallback )
    {
        result->allowMessage = result->allowMessage & doc->messageCallback( tidyImplToMessage(result) );
    }

    return result;
}


TidyMessageImpl *TY_(tidyMessageCreate)( TidyDocImpl *doc,
                                         uint code,
                                         TidyReportLevel level,
                                         ... )
{
    TidyMessageImpl *result;
    va_list args;
    va_start(args, level);
    result = tidyMessageCreateInitV(doc, NULL, code, 0, 0, level, args);
    va_end(args);

    return result;
}


TidyMessageImpl *TY_(tidyMessageCreateWithNode)( TidyDocImpl *doc,
                                                 Node *node,
                                                 uint code,
                                                 TidyReportLevel level,
                                                 ... )
{
    TidyMessageImpl *result;
    va_list args_copy;
    int line = ( node ? node->line :
                ( doc->lexer ? doc->lexer->lines : 0 ) );
    int col  = ( node ? node->column :
                ( doc->lexer ? doc->lexer->columns : 0 ) );

    va_start(args_copy, level);
    result = tidyMessageCreateInitV(doc, node, code, line, col, level, args_copy);
    va_end(args_copy);

    return result;
}


TidyMessageImpl *TY_(tidyMessageCreateWithLexer)( TidyDocImpl *doc,
                                                  uint code,
                                                  TidyReportLevel level,
                                                  ... )
{
    TidyMessageImpl *result;
    va_list args_copy;
    int line = ( doc->lexer ? doc->lexer->lines : 0 );
    int col  = ( doc->lexer ? doc->lexer->columns : 0 );

    va_start(args_copy, level);
    result = tidyMessageCreateInitV(doc, NULL, code, line, col, level, args_copy);
    va_end(args_copy);

    return result;
}


void TY_(tidyMessageRelease)( TidyMessageImpl *message )
{
    if ( !message )
        return;
    TidyDocFree( tidyDocToImpl(message->tidyDoc), message->arguments );
    TidyDocFree( tidyDocToImpl(message->tidyDoc), message->messageDefault );
    TidyDocFree( tidyDocToImpl(message->tidyDoc), message->message );
    TidyDocFree( tidyDocToImpl(message->tidyDoc), message->messagePosDefault );
    TidyDocFree( tidyDocToImpl(message->tidyDoc), message->messagePos );
    TidyDocFree( tidyDocToImpl(message->tidyDoc), message->messageOutputDefault );
    TidyDocFree( tidyDocToImpl(message->tidyDoc), message->messageOutput );
    TidyDocFree(tidyDocToImpl(message->tidyDoc), message); /* Issue #597 - and discard the message structure */
}


/*********************************************************************
 * Modern Message Callback Functions
 *********************************************************************/


TidyDocImpl* TY_(getMessageDoc)( TidyMessageImpl message )
{
    return message.tidyDoc;
}

uint TY_(getMessageCode)( TidyMessageImpl message )
{
    return message.code;
}

ctmbstr TY_(getMessageKey)( TidyMessageImpl message )
{
    return message.messageKey;
}

int TY_(getMessageLine)( TidyMessageImpl message )
{
    return message.line;
}

int TY_(getMessageColumn)( TidyMessageImpl message )
{
    return message.column;
}

TidyReportLevel TY_(getMessageLevel)( TidyMessageImpl message )
{
    return message.level;
}

Bool TY_(getMessageIsMuted)( TidyMessageImpl message )
{
    return message.muted;
}

ctmbstr TY_(getMessageFormatDefault)( TidyMessageImpl message )
{
    return message.messageFormatDefault;
}

ctmbstr TY_(getMessageFormat)( TidyMessageImpl message )
{
    return message.messageFormat;
}

ctmbstr TY_(getMessageDefault)( TidyMessageImpl message )
{
    return message.messageDefault;
}

ctmbstr TY_(getMessage)( TidyMessageImpl message )
{
    return message.message;
}

ctmbstr TY_(getMessagePosDefault)( TidyMessageImpl message )
{
    return message.messagePosDefault;
}

ctmbstr TY_(getMessagePos)( TidyMessageImpl message )
{
    return message.messagePos;
}

ctmbstr TY_(getMessagePrefixDefault)( TidyMessageImpl message )
{
    return message.messagePrefixDefault;
}

ctmbstr TY_(getMessagePrefix)( TidyMessageImpl message )
{
    return message.messagePrefix;
}


ctmbstr TY_(getMessageOutputDefault)( TidyMessageImpl message )
{
    return message.messageOutputDefault;
}

ctmbstr TY_(getMessageOutput)( TidyMessageImpl message )
{
    return message.messageOutput;
}


/*********************************************************************
 * Message Argument Interrogation
 *********************************************************************/


TidyIterator TY_(getMessageArguments)( TidyMessageImpl message )
{
    if (message.argcount > 0)
        return (TidyIterator) (size_t)1;
    else
        return (TidyIterator) (size_t)0;
}

TidyMessageArgument TY_(getNextMessageArgument)( TidyMessageImpl message, TidyIterator* iter )
{
    size_t item = 0;
    size_t itemIndex;
    assert( iter != NULL );

    itemIndex = (size_t)*iter;

    if ( itemIndex >= 1 && itemIndex <= (size_t)message.argcount )
    {
        item = itemIndex;
        itemIndex++;
    }

    /* Just as TidyIterator is really just a dumb, one-based index, the
       TidyMessageArgument is really just a dumb, zero-based index; however
       this type of iterator and opaque interrogation is simply how Tidy
       does things. */
    *iter = (TidyIterator)( itemIndex <= (size_t)message.argcount ? itemIndex : (size_t)0 );
    return (TidyMessageArgument)item;
}


TidyFormatParameterType TY_(getArgType)( TidyMessageImpl message, TidyMessageArgument* arg )
{
    int argNum = (int)(size_t)*arg - 1;
    assert( argNum <= message.argcount );

    return message.arguments[argNum].type;
}


ctmbstr TY_(getArgFormat)( TidyMessageImpl message, TidyMessageArgument* arg )
{
    int argNum = (int)(size_t)*arg - 1;
    assert( argNum <= message.argcount );

    return message.arguments[argNum].format;
}


ctmbstr TY_(getArgValueString)( TidyMessageImpl message, TidyMessageArgument* arg )
{
    int argNum = (int)(size_t)*arg - 1;
    assert( argNum <= message.argcount );
    assert( message.arguments[argNum].type == tidyFormatType_STRING);

    return message.arguments[argNum].u.s;
}


uint TY_(getArgValueUInt)( TidyMessageImpl message, TidyMessageArgument* arg )
{
    int argNum = (int)(size_t)*arg - 1;
    assert( argNum <= message.argcount );
    assert( message.arguments[argNum].type == tidyFormatType_UINT);

    return message.arguments[argNum].u.ui;
}


int TY_(getArgValueInt)( TidyMessageImpl message, TidyMessageArgument* arg )
{
    int argNum = (int)(size_t)*arg - 1;
    assert( argNum <= message.argcount );
    assert( message.arguments[argNum].type == tidyFormatType_INT);

    return message.arguments[argNum].u.i;
}


double TY_(getArgValueDouble)( TidyMessageImpl message, TidyMessageArgument* arg )
{
    int argNum = (int)(size_t)*arg - 1;
    assert( argNum <= message.argcount );
    assert( message.arguments[argNum].type == tidyFormatType_DOUBLE);

    return message.arguments[argNum].u.d;
}



/*********************************************************************
 * BuildArgArray support
 * Adapted loosely from Mozilla `prprf.c`, Mozilla Public License:
 *   - https://www.mozilla.org/en-US/MPL/2.0/
 *********************************************************************/


/** Returns a pointer to an allocated array of `printfArg` given a format
 ** string and a va_list, or NULL if not successful or no parameters were
 ** given. Parameter `rv` will return with the count of zero or more
 ** parameters if successful, else -1.
 **
 ** We'll also be sure to use the document's allocator if specified, thus
 ** the requirement to pass in a TidyDocImpl.
 **
 ** Currently Tidy only uses %c, %d, %s, %u, %X, although doubles are
 ** supported as well. Unsupported arguments will result in failure as
 ** described above.
 */
static struct printfArg* BuildArgArray( TidyDocImpl *doc, ctmbstr fmt, va_list ap, int* rv )
{
    int number = 0; /* the quantity of valid arguments found; returned as rv. */
    int cn = -1;    /* keeps track of which parameter index is current. */
    int i = 0;      /* typical index. */
    int pos = -1;   /* starting position of current argument. */
    const char* p;  /* current position in format string. */
    char c;         /* current character. */
    struct printfArg* nas;

    /* first pass: determine number of valid % to allocate space. */

    p = fmt;
    *rv = 0;

    while( ( c = *p++ ) != 0 )
    {
        if( c != '%' )
            continue;

        if( ( c = *p++ ) == '%' )	/* skip %% case */
            continue;
        else
            number++;
    }


    if( number == 0 )
        return NULL;


    nas = (struct printfArg*)TidyDocAlloc( doc, number * sizeof( struct printfArg ) );
    if( !nas )
    {
        *rv = -1;
        return NULL;
    }


    for( i = 0; i < number; i++ )
    {
        nas[i].type = tidyFormatType_UNKNOWN;
    }


    /* second pass: set nas[].type and location. */

    p = fmt;
    while( ( c = *p++ ) != 0 )
    {
        if( c != '%' )
            continue;

        if( ( c = *p++ ) == '%' )
            continue; /* skip %% case */

        pos = p - fmt - 2; /* p already incremented twice */

        /* width -- width via parameter */
        if (c == '*')
        {
            /* not supported feature */
            *rv = -1;
            break;
        }

        /* width field -- skip */
        while ((c >= '0') && (c <= '9'))
        {
            c = *p++;
        }

        /* precision */
        if (c == '.')
        {
            c = *p++;
            if (c == '*') {
                /* not supported feature */
                *rv = -1;
                break;
            }

            while ((c >= '0') && (c <= '9'))
            {
                c = *p++;
            }
        }


        cn++;

        /* size and format */
        nas[cn].type = tidyFormatType_UINT;
        switch (c)
        {
            case 'c': /* unsigned int (char) */
            case 'u': /* unsigned int */
            case 'X': /* unsigned int as hex */
            case 'x': /* unsigned int as hex */
            case 'o': /* octal */
                nas[cn].u.ui = va_arg( ap, unsigned int );
                break;

            case 'd': /* signed int */
            case 'i': /* signed int */
                nas[cn].type = tidyFormatType_INT;
                nas[cn].u.i = va_arg( ap, int );
                break;


            case 's': /* string */
                nas[cn].type = tidyFormatType_STRING;
                nas[cn].u.s = va_arg( ap, char* );
                break;

            case 'e': /* double */
            case 'E': /* double */
            case 'f': /* double */
            case 'F': /* double */
            case 'g': /* double */
            case 'G': /* double */
                nas[cn].type = tidyFormatType_DOUBLE;
                nas[cn].u.d = va_arg( ap, double );
                break;

            default:
                nas[cn].type = tidyFormatType_UNKNOWN;
                *rv = -1;
                break;
        }

        /* position and format */
        nas[cn].formatStart = pos;
        nas[cn].formatLength = (p - fmt) - pos;

        /* the format string exceeds the buffer length */
        if ( nas[cn].formatLength >= FORMAT_LENGTH )
        {
            *rv = -1;
            break;
        }
        else
        {
            strncpy(nas[cn].format, fmt + nas[cn].formatStart, nas[cn].formatLength);
            nas[cn].format[nas[cn].formatLength] = 0; /* Is. #800 - If count <= srcLen, no 0 added! */
        }


        /* Something's not right. */
        if( nas[cn].type == tidyFormatType_UNKNOWN )
        {
            *rv = -1;
            break;
        }
    }


    /* third pass: fill the nas[cn].ap */

    if( *rv < 0 )
    {
        TidyDocFree( doc, nas );;
        return NULL;
    }

    *rv = number;
    return nas;
}

