/* tidylib.c -- internal library definitions

  (c) 1998-2008 (W3C) MIT, ERCIM, Keio University
  See tidy.h for the copyright notice.

  Defines HTML Tidy API implemented by tidy library.

  Very rough initial cut for discussion purposes.

  Public interface is const-correct and doesn't explicitly depend
  on any globals.  Thus, thread-safety may be introduced w/out
  changing the interface.

  Looking ahead to a C++ wrapper, C functions always pass
  this-equivalent as 1st arg.

  Created 2001-05-20 by Charles Reitzel

*/


#include "third_party/tidy/tidy-int.h"
#include "third_party/tidy/parser.h"
#include "third_party/tidy/clean.h"
#include "third_party/tidy/gdoc.h"
#include "third_party/tidy/config.h"
#include "third_party/tidy/message.h"
#include "third_party/tidy/messageobj.h"
#include "third_party/tidy/pprint.h"
#include "third_party/tidy/entities.h"
#include "third_party/tidy/tmbstr.h"
#include "third_party/tidy/utf8.h"
#include "third_party/tidy/mappedio.h"
#include "third_party/tidy/language.h"
#include "third_party/tidy/attrs.h"
#include "libc/assert.h"
#include "libc/errno.h"
#include "libc/calls/struct/stat.h"
#include "libc/sysv/consts/s.h"
#include "libc/time/time.h"
#include "third_party/tidy/sprtf.h"

/* Create/Destroy a Tidy "document" object */
static TidyDocImpl* tidyDocCreate( TidyAllocator *allocator );
static void         tidyDocRelease( TidyDocImpl* impl );

static int          tidyDocStatus( TidyDocImpl* impl );

/* Parse Markup */
static int          tidyDocParseFile( TidyDocImpl* impl, ctmbstr htmlfil );
static int          tidyDocParseStdin( TidyDocImpl* impl );
static int          tidyDocParseString( TidyDocImpl* impl, ctmbstr content );
static int          tidyDocParseBuffer( TidyDocImpl* impl, TidyBuffer* inbuf );
static int          tidyDocParseSource( TidyDocImpl* impl, TidyInputSource* docIn );


/* Execute post-parse diagnostics and cleanup.
** Note, the order is important.  You will get different
** results from the diagnostics depending on if they are run
** pre-or-post repair.
*/
static int          tidyDocRunDiagnostics( TidyDocImpl* doc );
static void         tidyDocReportDoctype( TidyDocImpl* doc );
static int          tidyDocCleanAndRepair( TidyDocImpl* doc );


/* Save cleaned up file to file/buffer/sink */
static int          tidyDocSaveFile( TidyDocImpl* impl, ctmbstr htmlfil );
static int          tidyDocSaveStdout( TidyDocImpl* impl );
static int          tidyDocSaveString( TidyDocImpl* impl, tmbstr buffer, uint* buflen );
static int          tidyDocSaveBuffer( TidyDocImpl* impl, TidyBuffer* outbuf );
static int          tidyDocSaveSink( TidyDocImpl* impl, TidyOutputSink* docOut );
static int          tidyDocSaveStream( TidyDocImpl* impl, StreamOut* out );


/* Tidy public interface
**
** Most functions return an integer:
**
** 0    -> SUCCESS
** >0   -> WARNING
** <0   -> ERROR
**
*/

TidyDoc       tidyCreate(void)
{
  TidyDocImpl* impl = tidyDocCreate( &TY_(g_default_allocator) );
  return tidyImplToDoc( impl );
}

TidyDoc tidyCreateWithAllocator( TidyAllocator *allocator )
{
  TidyDocImpl* impl = tidyDocCreate( allocator );
  return tidyImplToDoc( impl );
}

void          tidyRelease( TidyDoc tdoc )
{
  TidyDocImpl* impl = tidyDocToImpl( tdoc );
  tidyDocRelease( impl );
}

TidyDocImpl* tidyDocCreate( TidyAllocator *allocator )
{
    TidyDocImpl* doc = (TidyDocImpl*)TidyAlloc( allocator, sizeof(TidyDocImpl) );
    TidyClearMemory( doc, sizeof(*doc) );
    doc->allocator = allocator;

    TY_(InitMap)();
    TY_(InitTags)( doc );
    TY_(InitAttrs)( doc );
    TY_(InitConfig)( doc );
    TY_(InitPrintBuf)( doc );
    TY_(InitParserStack)( doc );

    /* Set the locale for tidy's output. This both configures
    ** LibTidy to use the environment's locale as well as the
    ** standard library.
    */
#if SUPPORT_LOCALIZATIONS
    if ( TY_(tidyGetLanguageSetByUser)() == no )
    {
        if( ! TY_(tidySetLanguage)( getenv( "LC_MESSAGES" ) ) )
        {
            if( ! TY_(tidySetLanguage)( getenv( "LANG" ) ) )
            {
                /*\
                *  Is. #770 #783 #780 #790 and maybe others -
                *  TY_(tidySetLanguage)( setlocale( LC_ALL, "" ) );
                *  this seems a 'bad' choice!
               \*/
            }
        }
    }
#endif

    /* By default, wire tidy messages to standard error.
    ** Document input will be set by parsing routines.
    ** Document output will be set by pretty print routines.
    ** Config input will be set by config parsing routines.
    ** But we need to start off with a way to report errors.
    */
    doc->errout = TY_(StdErrOutput)();
    return doc;
}

void          tidyDocRelease( TidyDocImpl* doc )
{
    /* doc in/out opened and closed by parse/print routines */
    if ( doc )
    {
        assert( doc->docIn == NULL );
        assert( doc->docOut == NULL );

        TY_(ReleaseStreamOut)( doc, doc->errout );
        doc->errout = NULL;

        TY_(FreePrintBuf)( doc );
        TY_(FreeNode)(doc, &doc->root);
        TidyClearMemory(&doc->root, sizeof(Node));

        if (doc->givenDoctype)
            TidyDocFree(doc, doc->givenDoctype);

        TY_(FreeConfig)( doc );
        TY_(FreeAttrTable)( doc );
        TY_(FreeAttrPriorityList)( doc );
        TY_(FreeMutedMessageList( doc ));
        TY_(FreeTags)( doc );
        /*\
         *  Issue #186 - Now FreeNode depend on the doctype, so the lexer is needed
         *  to determine which hash is to be used, so free it last.
        \*/
        TY_(FreeLexer)( doc );
        TY_(FreeParserStack)( doc );
        TidyDocFree( doc, doc );
    }
}

/* Let application store a chunk of data w/ each Tidy tdocance.
** Useful for callbacks.
*/
void        tidySetAppData( TidyDoc tdoc, void* appData )
{
  TidyDocImpl* impl = tidyDocToImpl( tdoc );
  if ( impl )
    impl->appData = appData;
}
void*       tidyGetAppData( TidyDoc tdoc )
{
  TidyDocImpl* impl = tidyDocToImpl( tdoc );
  if ( impl )
    return impl->appData;
  return NULL;
}

ctmbstr     tidyReleaseDate(void)
{
    return TY_(ReleaseDate)();
}

ctmbstr     tidyLibraryVersion(void)
{
    return TY_(tidyLibraryVersion)();
}

ctmbstr     tidyPlatform(void)
{
#ifdef PLATFORM_NAME
    return PLATFORM_NAME;
#else
    return NULL;
#endif
}


/* Get/set configuration options
*/
Bool     tidySetOptionCallback( TidyDoc tdoc, TidyOptCallback pOptCallback )
{
  TidyDocImpl* impl = tidyDocToImpl( tdoc );
  if ( impl )
  {
    impl->pOptCallback = pOptCallback;
    return yes;
  }
  return no;
}

Bool     tidySetConfigCallback(TidyDoc tdoc, TidyConfigCallback pConfigCallback)
{
  TidyDocImpl* impl = tidyDocToImpl( tdoc );
  if ( impl )
  {
    impl->pConfigCallback = pConfigCallback;
    return yes;
  }
  return no;
}

Bool    tidySetConfigChangeCallback(TidyDoc tdoc, TidyConfigChangeCallback pCallback)
{
  TidyDocImpl* impl = tidyDocToImpl( tdoc );
  if ( impl )
  {
    impl->pConfigChangeCallback = pCallback;
    return yes;
  }
  return no;
}



int     tidyLoadConfig( TidyDoc tdoc, ctmbstr cfgfil )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl )
        return TY_(ParseConfigFile)( impl, cfgfil );
    return -EINVAL;
}

int     tidyLoadConfigEnc( TidyDoc tdoc, ctmbstr cfgfil, ctmbstr charenc )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl )
        return TY_(ParseConfigFileEnc)( impl, cfgfil, charenc );
    return -EINVAL;
}

int         tidySetCharEncoding( TidyDoc tdoc, ctmbstr encnam )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl )
    {
        int enc = TY_(CharEncodingId)( impl, encnam );
        if ( enc >= 0 && TY_(AdjustCharEncoding)(impl, enc) )
            return 0;

        TY_(ReportBadArgument)( impl, "char-encoding" );
    }
    return -EINVAL;
}

int           tidySetInCharEncoding( TidyDoc tdoc, ctmbstr encnam )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl )
    {
        int enc = TY_(CharEncodingId)( impl, encnam );
        if ( enc >= 0 && TY_(SetOptionInt)( impl, TidyInCharEncoding, enc ) )
            return 0;

        TY_(ReportBadArgument)( impl, "in-char-encoding" );
    }
    return -EINVAL;
}

int           tidySetOutCharEncoding( TidyDoc tdoc, ctmbstr encnam )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl )
    {
        int enc = TY_(CharEncodingId)( impl, encnam );
        if ( enc >= 0 && TY_(SetOptionInt)( impl, TidyOutCharEncoding, enc ) )
            return 0;

        TY_(ReportBadArgument)( impl, "out-char-encoding" );
    }
    return -EINVAL;
}

TidyOptionId tidyOptGetIdForName( ctmbstr optnam )
{
    const TidyOptionImpl* option = TY_(lookupOption)( optnam );
    if ( option )
        return option->id;
    return N_TIDY_OPTIONS;  /* Error */
}

TidyIterator  tidyGetOptionList( TidyDoc tdoc )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl )
        return TY_(getOptionList)( impl );
    return (TidyIterator) -1;
}

TidyOption    tidyGetNextOption( TidyDoc tdoc, TidyIterator* pos )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    const TidyOptionImpl* option = NULL;
    if ( impl )
        option = TY_(getNextOption)( impl, pos );
    else if ( pos )
        *pos = 0;
    return tidyImplToOption( option );
}


TidyOption    tidyGetOption( TidyDoc ARG_UNUSED(tdoc), TidyOptionId optId )
{
    const TidyOptionImpl* option = TY_(getOption)( optId );
    return tidyImplToOption( option );
}
TidyOption    tidyGetOptionByName( TidyDoc ARG_UNUSED(doc), ctmbstr optnam )
{
    const TidyOptionImpl* option = TY_(lookupOption)( optnam );
    return tidyImplToOption( option );
}

TidyOptionId  tidyOptGetId( TidyOption topt )
{
    const TidyOptionImpl* option = tidyOptionToImpl( topt );
    if ( option )
        return option->id;
    return N_TIDY_OPTIONS;
}
ctmbstr       tidyOptGetName( TidyOption topt )
{
    const TidyOptionImpl* option = tidyOptionToImpl( topt );
    if ( option )
        return option->name;
    return NULL;
}
TidyOptionType tidyOptGetType( TidyOption topt )
{
    const TidyOptionImpl* option = tidyOptionToImpl( topt );
    if ( option )
        return option->type;
    return (TidyOptionType) -1;
}
Bool           tidyOptionIsList( TidyOption opt )
{
    const TidyOptionImpl* option = tidyOptionToImpl( opt );
    if ( option )
        return TY_(getOptionIsList)( option->id );
    return no;
}
TidyConfigCategory tidyOptGetCategory( TidyOption topt )
{
    const TidyOptionImpl* option = tidyOptionToImpl( topt );
    if ( option )
        return option->category;
    return (TidyConfigCategory) -1;
}
ctmbstr       tidyOptGetDefault( TidyOption topt )
{
    const TidyOptionImpl* option = tidyOptionToImpl( topt );
    /* Special case for TidyDoctype, because it is declared as string */
    if ( option && option->id == TidyDoctype )
    {
        const TidyOptionImpl* newopt = TY_(getOption)( TidyDoctypeMode );
        return TY_(GetPickListLabelForPick)( TidyDoctypeMode, newopt->dflt );
    }
    if ( option && option->type == TidyString )
        return option->pdflt; /* Issue #306 - fix an old typo hidden by a cast! */
    return NULL;
}
ulong          tidyOptGetDefaultInt( TidyOption topt )
{
    const TidyOptionImpl* option = tidyOptionToImpl( topt );
    if ( option && option->type != TidyString )
        return option->dflt;

    /* Special case for TidyDoctype, because it has a picklist */
    if ( option->id == TidyDoctype )
    {
        const TidyOptionImpl* newopt = TY_(getOption)( TidyDoctypeMode );
        return newopt->dflt;
    }

    return ~0U;
}
Bool          tidyOptGetDefaultBool( TidyOption topt )
{
    const TidyOptionImpl* option = tidyOptionToImpl( topt );
    if ( option && option->type != TidyString )
        return ( option->dflt ? yes : no );
    return no;
}
Bool          tidyOptIsReadOnly( TidyOption topt )
{
    const TidyOptionImpl* option = tidyOptionToImpl( topt );
    if ( option  )
        return ( option->parser == NULL );
    return yes;
}


TidyIterator  tidyOptGetPickList( TidyOption topt )
{
    const TidyOptionImpl* option = tidyOptionToImpl( topt );
    if ( option )
      return TY_(getOptionPickList)( option );
    return (TidyIterator) -1;
}
ctmbstr       tidyOptGetNextPick( TidyOption topt, TidyIterator* pos )
{
    const TidyOptionImpl* option = tidyOptionToImpl( topt );
    if ( option )
        return TY_(getNextOptionPick)( option, pos );
    return NULL;
}


ctmbstr       tidyOptGetValue( TidyDoc tdoc, TidyOptionId optId )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    ctmbstr optval = NULL;
    if ( impl )
    {
        if ( optId == TidyDoctype )
        {
            /* Special case for TidyDoctype, because it has a picklist and is a string. */
            uint pick = tidyOptGetInt( tdoc, TidyDoctypeMode );
            if ( pick != TidyDoctypeUser )
            {
                optval = TY_(GetPickListLabelForPick)( TidyDoctypeMode, pick );
            } else {
                optval = cfgStr( impl, optId );
            }
        } else {
            /* Standard case. */
            optval = cfgStr( impl, optId );
        }
    }
    return optval;
}
Bool        tidyOptSetValue( TidyDoc tdoc, TidyOptionId optId, ctmbstr val )
{
  TidyDocImpl* impl = tidyDocToImpl( tdoc );
  if ( impl )
    return TY_(ParseConfigValue)( impl, optId, val );
  return no;
}
Bool        tidyOptParseValue( TidyDoc tdoc, ctmbstr optnam, ctmbstr val )
{
  TidyDocImpl* impl = tidyDocToImpl( tdoc );
  if ( impl )
    return TY_(ParseConfigOption)( impl, optnam, val );
  return no;
}

ulong        tidyOptGetInt( TidyDoc tdoc, TidyOptionId optId )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    ulong opti = 0;
    if ( impl )
    {
        /* Special case for TidyDoctype, because it has a picklist */
        if ( optId == TidyDoctype )
            opti = cfg( impl, TidyDoctypeMode);
        else
            opti = cfg( impl, optId );
    }
    return opti;
}

Bool        tidyOptSetInt( TidyDoc tdoc, TidyOptionId optId, ulong val )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl )
    {
        /* Special case for TidyDoctype, because it has a picklist */
        if ( optId == TidyDoctype )
            return TY_(SetOptionInt)( impl, TidyDoctypeMode, val );
        else
            return TY_(SetOptionInt)( impl, optId, val );
    }
    return no;
}

Bool         tidyOptGetBool( TidyDoc tdoc, TidyOptionId optId )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    Bool optb = no;
    if ( impl )
    {
        const TidyOptionImpl* option = TY_(getOption)( optId );
        if ( option )
        {
            optb = cfgBool( impl, optId );
        }
    }
    return optb;
}

Bool        tidyOptSetBool( TidyDoc tdoc, TidyOptionId optId, Bool val )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl )
        return TY_(SetOptionBool)( impl, optId, val );
    return no;
}

ctmbstr       tidyOptGetEncName( TidyDoc tdoc, TidyOptionId optId )
{
  uint enc = tidyOptGetInt( tdoc, optId );
  return TY_(CharEncodingOptName)( enc );
}

ctmbstr       tidyOptGetCurrPick( TidyDoc tdoc, TidyOptionId optId )
{
    uint pick = tidyOptGetInt( tdoc, optId );
    return TY_(GetPickListLabelForPick)( optId, pick );
}


TidyIterator tidyOptGetDeclTagList( TidyDoc tdoc )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    TidyIterator declIter = 0;
    if ( impl )
        declIter = TY_(GetDeclaredTagList)( impl );
    return declIter;
}

ctmbstr       tidyOptGetNextDeclTag( TidyDoc tdoc, TidyOptionId optId,
                                     TidyIterator* iter )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    ctmbstr tagnam = NULL;
    if ( impl )
    {
        UserTagType tagtyp = tagtype_null;
        if ( optId == TidyInlineTags )
            tagtyp = tagtype_inline;
        else if ( optId == TidyBlockTags )
            tagtyp = tagtype_block;
        else if ( optId == TidyEmptyTags )
            tagtyp = tagtype_empty;
        else if ( optId == TidyPreTags )
            tagtyp = tagtype_pre;
        if ( tagtyp != tagtype_null )
            tagnam = TY_(GetNextDeclaredTag)( impl, tagtyp, iter );
    }
    return tagnam;
}

TidyIterator tidyOptGetPriorityAttrList( TidyDoc tdoc )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl )
        return TY_(getPriorityAttrList)( impl );
    return (TidyIterator) -1;
}

ctmbstr      tidyOptGetNextPriorityAttr(TidyDoc tdoc, TidyIterator* iter )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    ctmbstr result = NULL;
    if ( impl )
        result = TY_(getNextPriorityAttr)( impl, iter );
    else if ( iter )
        *iter = 0;
    return result;
}

TidyIterator tidyOptGetMutedMessageList( TidyDoc tdoc )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl )
        return TY_(getMutedMessageList)( impl );
    return (TidyIterator) -1;
}

ctmbstr      tidyOptGetNextMutedMessage(TidyDoc tdoc, TidyIterator* iter )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    ctmbstr result = NULL;
    if ( impl )
        result = TY_(getNextMutedMessage)( impl, iter );
    else if ( iter )
        *iter = 0;
    return result;
}

ctmbstr tidyOptGetDoc( TidyDoc ARG_UNUSED(tdoc), TidyOption opt )
{
    const TidyOptionId optId = tidyOptGetId( opt );
    return tidyLocalizedString(optId);
}

#if SUPPORT_CONSOLE_APP
/* TODO - GROUP ALL CONSOLE-ONLY FUNCTIONS */
TidyIterator tidyOptGetDocLinksList( TidyDoc ARG_UNUSED(tdoc), TidyOption opt )
{
    const TidyOptionId optId = tidyOptGetId( opt );
    const TidyOptionDoc* docDesc = TY_(OptGetDocDesc)( optId );
    if (docDesc && docDesc->links)
        return (TidyIterator)docDesc->links;
    return (TidyIterator)NULL;
}
#endif /* SUPPORT_CONSOLE_APP */

TidyOption tidyOptGetNextDocLinks( TidyDoc tdoc, TidyIterator* pos )
{
    const TidyOptionId* curr = (const TidyOptionId *)*pos;
    TidyOption opt;

    if (*curr == TidyUnknownOption)
    {
        *pos = (TidyIterator)NULL;
        return (TidyOption)0;
    }
    opt = tidyGetOption(tdoc, *curr);
    curr++;
    *pos = (*curr == TidyUnknownOption ) ?
        (TidyIterator)NULL:(TidyIterator)curr;
    return opt;
}

int tidyOptSaveFile( TidyDoc tdoc, ctmbstr cfgfil )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl )
        return TY_(SaveConfigFile)( impl, cfgfil );
    return -EINVAL;
}

int tidyOptSaveSink( TidyDoc tdoc, TidyOutputSink* sink )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl )
        return TY_(SaveConfigSink)( impl, sink );
    return -EINVAL;
}

Bool tidyOptSnapshot( TidyDoc tdoc )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl )
    {
        TY_(TakeConfigSnapshot)( impl );
        return yes;
    }
    return no;
}
Bool tidyOptResetToSnapshot( TidyDoc tdoc )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl )
    {
        TY_(ResetConfigToSnapshot)( impl );
        return yes;
    }
    return no;
}
Bool tidyOptResetAllToDefault( TidyDoc tdoc )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl )
    {
        TY_(ResetConfigToDefault)( impl );
        return yes;
    }
    return no;
}

Bool tidyOptResetToDefault( TidyDoc tdoc, TidyOptionId optId )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl )
        return TY_(ResetOptionToDefault)( impl, optId );
    return no;
}

Bool tidyOptDiffThanDefault( TidyDoc tdoc )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl )
        return TY_(ConfigDiffThanDefault)( impl );
    return no;
}
Bool          tidyOptDiffThanSnapshot( TidyDoc tdoc )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl )
        return TY_(ConfigDiffThanSnapshot)( impl );
    return no;
}

Bool tidyOptCopyConfig( TidyDoc to, TidyDoc from )
{
    TidyDocImpl* docTo = tidyDocToImpl( to );
    TidyDocImpl* docFrom = tidyDocToImpl( from );
    if ( docTo && docFrom )
    {
        TY_(CopyConfig)( docTo, docFrom );
        return yes;
    }
    return no;
}


/* I/O and Message handling interface
**
** By default, Tidy will define, create and use instance of input and output
** handlers for standard C buffered I/O (i.e. FILE* stdin, FILE* stdout and
** FILE* stderr for content input, content output and diagnostic output,
** respectively.  A FILE* cfgFile input handler will be used for config files.
** Command line options will just be set directly.
*/

void tidySetEmacsFile( TidyDoc tdoc, ctmbstr filePath )
{
    tidyOptSetValue( tdoc, TidyEmacsFile, filePath );
}

ctmbstr tidyGetEmacsFile( TidyDoc tdoc )
{
    return tidyOptGetValue( tdoc, TidyEmacsFile );
}


/* Use TidyReportFilter to filter messages by diagnostic level:
** info, warning, etc.  Just set diagnostic output
** handler to redirect all diagnostics output.  Return true
** to proceed with output, false to cancel.
*/
Bool tidySetReportFilter( TidyDoc tdoc, TidyReportFilter filt )
{
  TidyDocImpl* impl = tidyDocToImpl( tdoc );
  if ( impl )
  {
    impl->reportFilter = filt;
    return yes;
  }
  return no;
}

/* tidySetReportCallback functions similar to TidyReportFilter, but provides the
 * string version of the internal enum name so that LibTidy users can use
** the string as a lookup key for providing their own error localizations.
** See the string key definitions in tidyenum.h.
*/
Bool tidySetReportCallback( TidyDoc tdoc, TidyReportCallback filt )
{
  TidyDocImpl* impl = tidyDocToImpl( tdoc );
  if ( impl )
  {
    impl->reportCallback = filt;
    return yes;
  }
  return no;
}

Bool tidySetMessageCallback( TidyDoc tdoc, TidyMessageCallback filt )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl )
    {
        impl->messageCallback = filt;
        return yes;
    }
    return no;
}

TidyDoc tidyGetMessageDoc( TidyMessage tmessage )
{
    TidyMessageImpl *message = tidyMessageToImpl(tmessage);
    TidyDocImpl* doc = TY_(getMessageDoc)(*message);
    return tidyImplToDoc(doc);
}

uint tidyGetMessageCode( TidyMessage tmessage )
{
    TidyMessageImpl *message = tidyMessageToImpl(tmessage);
    return TY_(getMessageCode)(*message);
}

ctmbstr tidyGetMessageKey( TidyMessage tmessage )
{
    TidyMessageImpl *message = tidyMessageToImpl(tmessage);
    return TY_(getMessageKey)(*message);
}

int tidyGetMessageLine( TidyMessage tmessage )
{
    TidyMessageImpl *message = tidyMessageToImpl(tmessage);
    return TY_(getMessageLine)(*message);
}

int tidyGetMessageColumn( TidyMessage tmessage )
{
    TidyMessageImpl *message = tidyMessageToImpl(tmessage);
    return TY_(getMessageColumn)(*message);
}

TidyReportLevel tidyGetMessageLevel( TidyMessage tmessage )
{
    TidyMessageImpl *message = tidyMessageToImpl(tmessage);
    return TY_(getMessageLevel)(*message);
}

Bool tidyGetMessageIsMuted( TidyMessage tmessage )
{
    TidyMessageImpl *message = tidyMessageToImpl(tmessage);
    return TY_(getMessageIsMuted)(*message);
}

ctmbstr tidyGetMessageFormatDefault( TidyMessage tmessage )
{
    TidyMessageImpl *message = tidyMessageToImpl(tmessage);
    return TY_(getMessageFormatDefault)(*message);
}

ctmbstr tidyGetMessageFormat( TidyMessage tmessage )
{
    TidyMessageImpl *message = tidyMessageToImpl(tmessage);
    return TY_(getMessageFormat)(*message);
}

ctmbstr tidyGetMessageDefault( TidyMessage tmessage )
{
    TidyMessageImpl *message = tidyMessageToImpl(tmessage);
    return TY_(getMessageDefault)(*message);
}

ctmbstr tidyGetMessage( TidyMessage tmessage )
{
    TidyMessageImpl *message = tidyMessageToImpl(tmessage);
    return TY_(getMessage)(*message);
}

ctmbstr tidyGetMessagePosDefault( TidyMessage tmessage )
{
    TidyMessageImpl *message = tidyMessageToImpl(tmessage);
    return TY_(getMessagePosDefault)(*message);
}

ctmbstr tidyGetMessagePos( TidyMessage tmessage )
{
    TidyMessageImpl *message = tidyMessageToImpl(tmessage);
    return TY_(getMessagePos)(*message);
}

ctmbstr tidyGetMessagePrefixDefault( TidyMessage tmessage )
{
    TidyMessageImpl *message = tidyMessageToImpl(tmessage);
    return TY_(getMessagePrefixDefault)(*message);
}

ctmbstr tidyGetMessagePrefix( TidyMessage tmessage )
{
    TidyMessageImpl *message = tidyMessageToImpl(tmessage);
    return TY_(getMessagePrefix)(*message);
}


ctmbstr tidyGetMessageOutputDefault( TidyMessage tmessage )
{
    TidyMessageImpl *message = tidyMessageToImpl(tmessage);
    return TY_(getMessageOutputDefault)(*message);
}

ctmbstr tidyGetMessageOutput( TidyMessage tmessage )
{
    TidyMessageImpl *message = tidyMessageToImpl(tmessage);
    return TY_(getMessageOutput)(*message);
}

TidyIterator tidyGetMessageArguments( TidyMessage tmessage )
{
    TidyMessageImpl *message = tidyMessageToImpl(tmessage);
    return TY_(getMessageArguments)(*message);
}

TidyMessageArgument tidyGetNextMessageArgument( TidyMessage tmessage, TidyIterator* iter )
{
    TidyMessageImpl *message = tidyMessageToImpl(tmessage);
    return TY_(getNextMessageArgument)(*message, iter);
}

TidyFormatParameterType tidyGetArgType( TidyMessage tmessage, TidyMessageArgument* arg )
{
    TidyMessageImpl *message = tidyMessageToImpl(tmessage);
    return TY_(getArgType)(*message, arg);
}

ctmbstr tidyGetArgFormat( TidyMessage tmessage, TidyMessageArgument* arg )
{
    TidyMessageImpl *message = tidyMessageToImpl(tmessage);
    return TY_(getArgFormat)(*message, arg);
}

ctmbstr tidyGetArgValueString( TidyMessage tmessage, TidyMessageArgument* arg )
{
    TidyMessageImpl *message = tidyMessageToImpl(tmessage);
    return TY_(getArgValueString)(*message, arg);
}

uint tidyGetArgValueUInt( TidyMessage tmessage, TidyMessageArgument* arg )
{
    TidyMessageImpl *message = tidyMessageToImpl(tmessage);
    return TY_(getArgValueUInt)(*message, arg);
}

int tidyGetArgValueInt( TidyMessage tmessage, TidyMessageArgument* arg )
{
    TidyMessageImpl *message = tidyMessageToImpl(tmessage);
    return TY_(getArgValueInt)(*message, arg);
}

double tidyGetArgValueDouble( TidyMessage tmessage, TidyMessageArgument* arg )
{
    TidyMessageImpl *message = tidyMessageToImpl(tmessage);
    return TY_(getArgValueDouble)(*message, arg);
}


FILE*   tidySetErrorFile( TidyDoc tdoc, ctmbstr errfilnam )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl )
    {
        FILE* errout = fopen( errfilnam, "wb" );
        if ( errout )
        {
            uint outenc = cfg( impl, TidyOutCharEncoding );
            uint nl = cfg( impl, TidyNewline );
            TY_(ReleaseStreamOut)( impl, impl->errout );
            impl->errout = TY_(FileOutput)( impl, errout, outenc, nl );
            return errout;
        }
        else /* Emit message to current error sink */
            TY_(ReportFileError)( impl, errfilnam, FILE_CANT_OPEN );
    }
    return NULL;
}

int    tidySetErrorBuffer( TidyDoc tdoc, TidyBuffer* errbuf )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl )
    {
        uint outenc = cfg( impl, TidyOutCharEncoding );
        uint nl = cfg( impl, TidyNewline );
        TY_(ReleaseStreamOut)( impl, impl->errout );
        impl->errout = TY_(BufferOutput)( impl, errbuf, outenc, nl );
        return ( impl->errout ? 0 : -ENOMEM );
    }
    return -EINVAL;
}

int    tidySetErrorSink( TidyDoc tdoc, TidyOutputSink* sink )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl )
    {
        uint outenc = cfg( impl, TidyOutCharEncoding );
        uint nl = cfg( impl, TidyNewline );
        TY_(ReleaseStreamOut)( impl, impl->errout );
        impl->errout = TY_(UserOutput)( impl, sink, outenc, nl );
        return ( impl->errout ? 0 : -ENOMEM );
    }
    return -EINVAL;
}

/* Use TidyPPProgress to monitor the progress of the pretty printer.
 */
Bool        tidySetPrettyPrinterCallback(TidyDoc tdoc, TidyPPProgress callback)
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl )
    {
        impl->progressCallback = callback;
        return yes;
    }
    return no;
}


/* Document info */
int        tidyStatus( TidyDoc tdoc )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    int tidyStat = -EINVAL;
    if ( impl )
        tidyStat = tidyDocStatus( impl );
    return tidyStat;
}
int        tidyDetectedHtmlVersion( TidyDoc ARG_UNUSED(tdoc) )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    return TY_(HTMLVersionNumberFromCode)( impl->lexer->versionEmitted );
}

Bool        tidyDetectedXhtml( TidyDoc ARG_UNUSED(tdoc) )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    return impl->lexer->isvoyager;
}
Bool        tidyDetectedGenericXml( TidyDoc ARG_UNUSED(tdoc) )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    return impl->xmlDetected;
}

uint       tidyErrorCount( TidyDoc tdoc )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    uint count = 0xFFFFFFFF;
    if ( impl )
        count = impl->errors;
    return count;
}
uint       tidyWarningCount( TidyDoc tdoc )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    uint count = 0xFFFFFFFF;
    if ( impl )
        count = impl->warnings;
    return count;
}
uint       tidyAccessWarningCount( TidyDoc tdoc )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    uint count = 0xFFFFFFFF;
    if ( impl )
        count = impl->accessErrors;
    return count;
}
uint       tidyConfigErrorCount( TidyDoc tdoc )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    uint count = 0xFFFFFFFF;
    if ( impl )
        count = impl->optionErrors;
    return count;
}


/* Error reporting functions
*/
void         tidyErrorSummary( TidyDoc tdoc )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl )
        TY_(ErrorSummary)( impl );
}
void         tidyGeneralInfo( TidyDoc tdoc )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl )
    {
        TY_(Dialogue)( impl, TEXT_GENERAL_INFO );
        TY_(Dialogue)( impl, TEXT_GENERAL_INFO_PLEA );
    }
}


/* I/O Functions
**
** Initial version supports only whole-file operations.
** Do not expose Tidy StreamIn or Out data structures - yet.
*/

/* Parse/load Functions
**
** HTML/XHTML version determined from input.
*/
int  tidyParseFile( TidyDoc tdoc, ctmbstr filnam )
{
    TidyDocImpl* doc = tidyDocToImpl( tdoc );
    return tidyDocParseFile( doc, filnam );
}
int  tidyParseStdin( TidyDoc tdoc )
{
    TidyDocImpl* doc = tidyDocToImpl( tdoc );
    return tidyDocParseStdin( doc );
}
int  tidyParseString( TidyDoc tdoc, ctmbstr content )
{
    TidyDocImpl* doc = tidyDocToImpl( tdoc );
    return tidyDocParseString( doc, content );
}
int  tidyParseBuffer( TidyDoc tdoc, TidyBuffer* inbuf )
{
    TidyDocImpl* doc = tidyDocToImpl( tdoc );
    return tidyDocParseBuffer( doc, inbuf );
}
int  tidyParseSource( TidyDoc tdoc, TidyInputSource* source )
{
    TidyDocImpl* doc = tidyDocToImpl( tdoc );
    return tidyDocParseSource( doc, source );
}

#ifdef WIN32
#define M_IS_DIR _S_IFDIR
#else // !WIN32
#define M_IS_DIR S_IFDIR
#endif
int   tidyDocParseFile( TidyDocImpl* doc, ctmbstr filnam )
{
    int status = -ENOENT;
    FILE* fin = 0;
    struct stat sbuf = { 0 }; /* Is. #681 - read-only files */
    if ( stat(filnam,&sbuf) != 0 )
    {
        TY_(ReportFileError)( doc, filnam, FILE_NOT_FILE );
        return status;
    }
    if (sbuf.st_mode & M_IS_DIR) /* and /NOT/ if a DIRECTORY */
    {
        TY_(ReportFileError)(doc, filnam, FILE_NOT_FILE);
        return status;
    }

#ifdef _WIN32
    return TY_(DocParseFileWithMappedFile)( doc, filnam );
#else

    fin = fopen( filnam, "rb" );

#if PRESERVE_FILE_TIMES
    {
        /* get last modified time */
        TidyClearMemory(&doc->filetimes, sizeof(doc->filetimes));
        if (fin && cfgBool(doc, TidyKeepFileTimes) &&
            fstat(fileno(fin), &sbuf) != -1)
        {
            doc->filetimes.actime = sbuf.st_atim.tv_sec;
            doc->filetimes.modtime = sbuf.st_mtim.tv_sec;
        }
    }
#endif

    if ( fin )
    {
        StreamIn* in = TY_(FileInput)( doc, fin, cfg( doc, TidyInCharEncoding ));
        if ( !in )
        {
            fclose( fin );
            return status;
        }
        status = TY_(DocParseStream)( doc, in );
        TY_(freeFileSource)(&in->source, yes);
        TY_(freeStreamIn)(in);
    }
    else /* Error message! */
        TY_(ReportFileError)( doc, filnam, FILE_CANT_OPEN );
    return status;
#endif
}

int   tidyDocParseStdin( TidyDocImpl* doc )
{
    StreamIn* in = TY_(FileInput)( doc, stdin, cfg( doc, TidyInCharEncoding ));
    int status = TY_(DocParseStream)( doc, in );
    TY_(freeFileSource)(&in->source, yes);
    TY_(freeStreamIn)(in);
    return status;
}

int   tidyDocParseBuffer( TidyDocImpl* doc, TidyBuffer* inbuf )
{
    int status = -EINVAL;
    if ( inbuf )
    {
        StreamIn* in = TY_(BufferInput)( doc, inbuf, cfg( doc, TidyInCharEncoding ));
        status = TY_(DocParseStream)( doc, in );
        TY_(freeStreamIn)(in);
    }
    return status;
}

int   tidyDocParseString( TidyDocImpl* doc, ctmbstr content )
{
    int status = -EINVAL;
    TidyBuffer inbuf;
    StreamIn* in = NULL;

    if ( content )
    {
        tidyBufInitWithAllocator( &inbuf, doc->allocator );
        tidyBufAttach( &inbuf, (byte*)content, TY_(tmbstrlen)(content)+1 );
        in = TY_(BufferInput)( doc, &inbuf, cfg( doc, TidyInCharEncoding ));
        status = TY_(DocParseStream)( doc, in );
        tidyBufDetach( &inbuf );
        TY_(freeStreamIn)(in);
    }
    return status;
}

int   tidyDocParseSource( TidyDocImpl* doc, TidyInputSource* source )
{
    StreamIn* in = TY_(UserInput)( doc, source, cfg( doc, TidyInCharEncoding ));
    int status = TY_(DocParseStream)( doc, in );
    TY_(freeStreamIn)(in);
    return status;
}


/* Print/save Functions
**
*/
int        tidySaveFile( TidyDoc tdoc, ctmbstr filnam )
{
    TidyDocImpl* doc = tidyDocToImpl( tdoc );
    return tidyDocSaveFile( doc, filnam );
}
int        tidySaveStdout( TidyDoc tdoc )
{
    TidyDocImpl* doc = tidyDocToImpl( tdoc );
    return tidyDocSaveStdout( doc );
}
int        tidySaveString( TidyDoc tdoc, tmbstr buffer, uint* buflen )
{
    TidyDocImpl* doc = tidyDocToImpl( tdoc );
    return tidyDocSaveString( doc, buffer, buflen );
}
int        tidySaveBuffer( TidyDoc tdoc, TidyBuffer* outbuf )
{
    TidyDocImpl* doc = tidyDocToImpl( tdoc );
    return tidyDocSaveBuffer( doc, outbuf );
}
int        tidySaveSink( TidyDoc tdoc, TidyOutputSink* sink )
{
    TidyDocImpl* doc = tidyDocToImpl( tdoc );
    return tidyDocSaveSink( doc, sink );
}

int         tidyDocSaveFile( TidyDocImpl* doc, ctmbstr filnam )
{
    int status = -ENOENT;
    FILE* fout = NULL;

    /* Don't zap input file if no output */
    if ( doc->errors > 0 &&
         cfgBool(doc, TidyWriteBack) && !cfgBool(doc, TidyForceOutput) )
        status = tidyDocStatus( doc );
    else
        fout = fopen( filnam, "wb" );

    if ( fout )
    {
        uint outenc = cfg( doc, TidyOutCharEncoding );
        uint nl = cfg( doc, TidyNewline );
        StreamOut* out = TY_(FileOutput)( doc, fout, outenc, nl );

        status = tidyDocSaveStream( doc, out );

        fclose( fout );
        TidyDocFree( doc, out );

#if PRESERVE_FILE_TIMES
        if ( doc->filetimes.actime )
        {
            /* set file last accessed/modified times to original values */
            utime( filnam, &doc->filetimes );
            TidyClearMemory( &doc->filetimes, sizeof(doc->filetimes) );
        }
#endif /* PRESERVFILETIMES */
    }
    if ( status < 0 ) /* Error message! */
        TY_(ReportFileError)( doc, filnam, FILE_CANT_OPEN );
    return status;
}



/* Note, _setmode() does NOT work on Win2K Pro w/ VC++ 6.0 SP3.
** The code has been left in in case it works w/ other compilers
** or operating systems.  If stdout is in Text mode, be aware that
** it will garble UTF16 documents.  In text mode, when it encounters
** a single byte of value 10 (0xA), it will insert a single byte
** value 13 (0xD) just before it.  This has the effect of garbling
** the entire document.
*/

#if !defined(NO_SETMODE_SUPPORT)
#  if defined(_WIN32) || defined(OS2_OS)
#   endif
#endif

int         tidyDocSaveStdout( TidyDocImpl* doc )
{
#if !defined(NO_SETMODE_SUPPORT)
#  if defined(_WIN32) || defined(OS2_OS)
    int oldstdoutmode = -1, oldstderrmode = -1;
#  endif
#endif

    int status = 0;
    uint outenc = cfg( doc, TidyOutCharEncoding );
    uint nl = cfg( doc, TidyNewline );
    StreamOut* out = TY_(FileOutput)( doc, stdout, outenc, nl );

#if !defined(NO_SETMODE_SUPPORT)
#  if defined(_WIN32) || defined(OS2_OS)
    oldstdoutmode = setmode( fileno(stdout), _O_BINARY );
    oldstderrmode = setmode( fileno(stderr), _O_BINARY );
#  endif
#endif

    if ( 0 == status )
      status = tidyDocSaveStream( doc, out );

    fflush(stdout);
    fflush(stderr);

#if !defined(NO_SETMODE_SUPPORT)
#  if defined(_WIN32) || defined(OS2_OS)
    if ( oldstdoutmode != -1 )
        oldstdoutmode = setmode( fileno(stdout), oldstdoutmode );
    if ( oldstderrmode != -1 )
        oldstderrmode = setmode( fileno(stderr), oldstderrmode );
#  endif
#endif

    TidyDocFree( doc, out );
    return status;
}

int         tidyDocSaveString( TidyDocImpl* doc, tmbstr buffer, uint* buflen )
{
    uint outenc = cfg( doc, TidyOutCharEncoding );
    uint nl = cfg( doc, TidyNewline );
    TidyBuffer outbuf;
    StreamOut* out;
    int status;

    tidyBufInitWithAllocator( &outbuf, doc->allocator );
    out = TY_(BufferOutput)( doc, &outbuf, outenc, nl );
    status = tidyDocSaveStream( doc, out );

    if ( outbuf.size > *buflen )
        status = -ENOMEM;
    else
        memcpy( buffer, outbuf.bp, outbuf.size );

    *buflen = outbuf.size;
    tidyBufFree( &outbuf );
    TidyDocFree( doc, out );
    return status;
}

int         tidyDocSaveBuffer( TidyDocImpl* doc, TidyBuffer* outbuf )
{
    int status = -EINVAL;
    if ( outbuf )
    {
        uint outenc = cfg( doc, TidyOutCharEncoding );
        uint nl = cfg( doc, TidyNewline );
        StreamOut* out = TY_(BufferOutput)( doc, outbuf, outenc, nl );

        status = tidyDocSaveStream( doc, out );
        TidyDocFree( doc, out );
    }
    return status;
}

int         tidyDocSaveSink( TidyDocImpl* doc, TidyOutputSink* sink )
{
    uint outenc = cfg( doc, TidyOutCharEncoding );
    uint nl = cfg( doc, TidyNewline );
    StreamOut* out = TY_(UserOutput)( doc, sink, outenc, nl );
    int status = tidyDocSaveStream( doc, out );
    TidyDocFree( doc, out );
    return status;
}

int         tidyDocStatus( TidyDocImpl* doc )
{
    if ( doc->errors > 0 )
        return 2;
    if ( doc->warnings > 0 || doc->accessErrors > 0 )
        return 1;
    return 0;
}



int        tidyCleanAndRepair( TidyDoc tdoc )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl )
      return tidyDocCleanAndRepair( impl );
    return -EINVAL;
}

int        tidyRunDiagnostics( TidyDoc tdoc )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl )
      return tidyDocRunDiagnostics( impl );
    return -EINVAL;
}

int        tidyReportDoctype( TidyDoc tdoc )
{
    int iret = -EINVAL;
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    if ( impl ) {
      tidyDocReportDoctype( impl );
      iret = 0;
    }
    return iret;
}

/* Workhorse functions.
**
** Parse requires input source, all input config items
** and diagnostic sink to have all been set before calling.
**
** Emit likewise requires that document sink and all
** pretty printing options have been set.
*/
static ctmbstr integrity = "\nPanic - tree has lost its integrity\n";

int         TY_(DocParseStream)( TidyDocImpl* doc, StreamIn* in )
{
    Bool xmlIn = cfgBool( doc, TidyXmlTags );
    TidyConfigChangeCallback callback = doc->pConfigChangeCallback;

    int bomEnc;
    doc->pConfigChangeCallback = NULL;

    assert( doc != NULL && in != NULL );
    assert( doc->docIn == NULL );
    doc->docIn = in;

    TY_(ResetTags)(doc);             /* Reset table to html5 mode */
    TY_(TakeConfigSnapshot)( doc );  /* Save config state */
    TY_(AdjustConfig)( doc );        /* Ensure config internal consistency */
    TY_(FreeAnchors)( doc );

    TY_(FreeNode)(doc, &doc->root);
    TidyClearMemory(&doc->root, sizeof(Node));

    if (doc->givenDoctype)
        TidyDocFree(doc, doc->givenDoctype);
    /*\
     *  Issue #186 - Now FreeNode depend on the doctype, so the lexer is needed
     *  to determine which hash is to be used, so free it last.
    \*/
    TY_(FreeLexer)( doc );
    doc->givenDoctype = NULL;

    doc->lexer = TY_(NewLexer)( doc );
    /* doc->lexer->root = &doc->root; */
    doc->root.line = doc->lexer->lines;
    doc->root.column = doc->lexer->columns;
    doc->inputHadBOM = no;
    doc->xmlDetected = no;

    bomEnc = TY_(ReadBOMEncoding)(in);

    if (bomEnc != -1)
    {
        in->encoding = bomEnc;
        TY_(SetOptionInt)(doc, TidyInCharEncoding, bomEnc);
    }

    /* Tidy doesn't alter the doctype for generic XML docs */
    if ( xmlIn )
    {
        TY_(ParseXMLDocument)( doc );
        if ( !TY_(CheckNodeIntegrity)( &doc->root ) )
            TidyPanic( doc->allocator, integrity );
    }
    else
    {
        doc->warnings = 0;
        TY_(ParseDocument)( doc );
        if ( !TY_(CheckNodeIntegrity)( &doc->root ) )
            TidyPanic( doc->allocator, integrity );
    }

    doc->docIn = NULL;
    doc->pConfigChangeCallback = callback;

    return tidyDocStatus( doc );
}

int         tidyDocRunDiagnostics( TidyDocImpl* doc )
{
    TY_(ReportMarkupVersion)( doc );
    TY_(ReportNumWarnings)( doc );

    if ( doc->errors > 0 && !cfgBool( doc, TidyForceOutput ) )
        TY_(Dialogue)(doc, STRING_NEEDS_INTERVENTION );

     return tidyDocStatus( doc );
}

void         tidyDocReportDoctype( TidyDocImpl* doc )
{
        TY_(ReportMarkupVersion)( doc );
}


/*****************************************************************************
 *  HTML5 STUFF
 *****************************************************************************/
#if 0 && defined(ENABLE_DEBUG_LOG)
extern void show_not_html5(void);
/* -----------------------------
List tags that do not have version HTML5 (HT50|XH50)

acronym applet basefont big center dir font frame frameset isindex
listing noframes plaintext rb rbc rtc strike tt xmp nextid
align bgsound blink comment ilayer layer marquee multicol nobr noembed
nolayer nosave server servlet spacer

Listed total 35 tags that do not have version 393216
   ------------------------------ */

static void list_not_html5(void)
{
    static Bool done_list = no;
    if (done_list == no) {
        done_list = yes;
        show_not_html5();
    }
}
#endif

/* What about <blink>, <s> stike-through, <u> underline */
static struct _html5Info
{
    const char *tag;
    uint id;
} const html5Info[] = {
    {"acronym", TidyTag_ACRONYM},
    {"applet", TidyTag_APPLET  },
    {"basefont",TidyTag_BASEFONT },
    { "big", TidyTag_BIG },
    { "center", TidyTag_CENTER },
    { "dir", TidyTag_DIR },
    { "font", TidyTag_FONT },
    { "frame", TidyTag_FRAME},
    { "frameset", TidyTag_FRAMESET},
    { "noframes", TidyTag_NOFRAMES },
    { "strike", TidyTag_STRIKE },
    { "tt", TidyTag_TT },
    { 0, 0 }
};
static Bool inRemovedInfo( uint tid )
{
    int i;
    for (i = 0; ; i++) {
        if (html5Info[i].tag == 0)
            break;
        if (html5Info[i].id == tid)
            return yes;
    }
    return no;
}

/* Things that should not be in an HTML5 body. This is special for CheckHTML5(),
 and we might just want to remove CheckHTML5()'s output altogether and count
 on the default --strict-tags-attributes.
 */
static int BadBody5Attribs[] = {
    TidyAttr_BACKGROUND,
    TidyAttr_BGCOLOR,
    TidyAttr_TEXT,
    TidyAttr_LINK,
    TidyAttr_VLINK,
    TidyAttr_ALINK,
    TidyAttr_UNKNOWN /* Must be last! */
};

static Bool nodeHasAlignAttr( Node *node )
{
    /* #define attrIsALIGN(av) AttrIsId( av, TidyAttr_ALIGN  ) */
    AttVal* av;
    for ( av = node->attributes; av != NULL; av = av->next ) {
        if (attrIsALIGN(av))
            return yes;
    }
    return no;
}

/*
 *  Perform special checks for HTML, even when we're not using the default
 *  option `--strict-tags-attributes yes`. This will ensure that HTML5 warning
 *  and error output is given regardless of the new option, and ensure that
 *  cleanup takes place. This provides mostly consistent Tidy behavior even with
 *  the introduction of this new option. Note that strings have changed, though,
 *  in order to maintain consistency with the `--strict-tags-attributes`
 *  messages.
 *
 *  See also: http://www.whatwg.org/specs/web-apps/current-work/multipage/obsolete.html#obsolete
 */
static void TY_(CheckHTML5)( TidyDocImpl* doc, Node* node )
{
    Stack *stack = TY_(newStack)(doc, 16);
    Bool clean = cfgBool( doc, TidyMakeClean );
    Bool already_strict = cfgBool( doc, TidyStrictTagsAttr );
    Node* body = TY_(FindBody)( doc );
    Node* next;
    Bool warn = yes;    /* should this be a warning, error, or report??? */
    AttVal* attr = NULL;
    int i = 0;

    while (node)
    {
        next = node->next;

        if ( nodeHasAlignAttr( node ) ) {
            /* @todo: Is this for ALL elements that accept an 'align' attribute,
             * or should this be a sub-set test?
             */

            /* We will only emit this message if `--strict-tags-attributes==no`;
             * otherwise if yes this message will be output during later
             * checking.
             */
            if ( !already_strict )
                TY_(ReportAttrError)(doc, node, TY_(AttrGetById)(node, TidyAttr_ALIGN), MISMATCHED_ATTRIBUTE_WARN);
        }
        if ( node == body ) {
            i = 0;
            /* We will only emit these messages if `--strict-tags-attributes==no`;
             * otherwise if yes these messages will be output during later
             * checking.
             */
            if ( !already_strict ) {
                while ( BadBody5Attribs[i] != TidyAttr_UNKNOWN ) {
                    attr = TY_(AttrGetById)(node, BadBody5Attribs[i]);
                    if ( attr )
                        TY_(ReportAttrError)(doc, node, attr , MISMATCHED_ATTRIBUTE_WARN);
                    i++;
                }
            }
        } else
        if ( nodeIsACRONYM(node) ) {
            if (clean) {
                /* Replace with 'abbr' with warning to that effect.
                 * Maybe should use static void RenameElem( TidyDocImpl* doc, Node* node, TidyTagId tid )
                 */
                TY_(CoerceNode)(doc, node, TidyTag_ABBR, warn, no);
            } else {
                if ( !already_strict )
                    TY_(Report)(doc, node, node, REMOVED_HTML5);
            }
        } else
        if ( nodeIsAPPLET(node) ) {
            if (clean) {
                /* replace with 'object' with warning to that effect
                 * maybe should use static void RenameElem( TidyDocImpl* doc, Node* node, TidyTagId tid )
                 */
                TY_(CoerceNode)(doc, node, TidyTag_OBJECT, warn, no);
            } else {
                if ( !already_strict )
                    TY_(Report)(doc, node, node, REMOVED_HTML5);
            }
        } else
        if ( nodeIsBASEFONT(node) ) {
            /* basefont: CSS equivalent 'font-size', 'font-family' and 'color'
             * on body or class on each subsequent element.
             * Difficult - If it is the first body element, then could consider
             * adding that to the <body> as a whole, else could perhaps apply it
             * to all subsequent elements. But also in consideration is the fact
             * that it was NOT supported in many browsers.
             * - For now just report a warning
             */
            if ( !already_strict )
                TY_(Report)(doc, node, node, REMOVED_HTML5);
        } else
        if ( nodeIsBIG(node) ) {
            /* big: CSS equivalent 'font-size:larger'
             * so could replace the <big> ... </big> with
             * <span style="font-size: larger"> ... </span>
             * then replace <big> with <span>
             * Need to think about that...
             * Could use -
             *   TY_(AddStyleProperty)( doc, node, "font-size: larger" );
             *   TY_(CoerceNode)(doc, node, TidyTag_SPAN, no, no);
             * Alternatively generated a <style> but how to get the style name
             * TY_(AddAttribute)( doc, node, "class", "????" );
             * Also maybe need a specific message like
             * Element '%s' replaced with 'span' with a 'font-size: larger style attribute
             * maybe should use static void RenameElem( TidyDocImpl* doc, Node* node, TidyTagId tid )
             */
            if (clean) {
                TY_(AddStyleProperty)( doc, node, "font-size: larger" );
                TY_(CoerceNode)(doc, node, TidyTag_SPAN, warn, no);
            } else {
                if ( !already_strict )
                    TY_(Report)(doc, node, node, REMOVED_HTML5);
            }
        } else
        if ( nodeIsCENTER(node) ) {
            /* center: CSS equivalent 'text-align:center'
             * and 'margin-left:auto; margin-right:auto' on descendant blocks
             * Tidy already handles this if 'clean' by SILENTLY generating the
             * <style> and adding a <div class="c1"> around the elements.
             * see: static Bool Center2Div( TidyDocImpl* doc, Node *node, Node **pnode)
             */
            if ( !already_strict )
                TY_(Report)(doc, node, node, REMOVED_HTML5);
        } else
        if ( nodeIsDIR(node) ) {
            /* dir: replace by <ul>
             * Tidy already actions this and issues a warning
             * Should this be CHANGED???
             */
            if ( !already_strict )
                TY_(Report)(doc, node, node, REMOVED_HTML5);
        } else
        if ( nodeIsFONT(node) ) {
            /* Tidy already handles this -
             * If 'clean' replaced by CSS, else
             * if is NOT clean, and doctype html5 then warnings issued
             * done in Bool Font2Span( TidyDocImpl* doc, Node *node, Node **pnode ) (I think?)
             */
            if ( !already_strict )
                TY_(Report)(doc, node, node, REMOVED_HTML5);
        } else
        if (( nodesIsFRAME(node) ) || ( nodeIsFRAMESET(node) ) || ( nodeIsNOFRAMES(node) )) {
            /* YOW: What to do here?????? Maybe <iframe>????
             */
            if ( !already_strict )
                TY_(Report)(doc, node, node, REMOVED_HTML5);
        } else
        if ( nodeIsSTRIKE(node) ) {
            /* strike: CSS equivalent 'text-decoration:line-through'
             * maybe should use static void RenameElem( TidyDocImpl* doc, Node* node, TidyTagId tid )
             */
            if (clean) {
                TY_(AddStyleProperty)( doc, node, "text-decoration: line-through" );
                TY_(CoerceNode)(doc, node, TidyTag_SPAN, warn, no);
            } else {
                if ( !already_strict )
                    TY_(Report)(doc, node, node, REMOVED_HTML5);
            }
        } else
        if ( nodeIsTT(node) ) {
            /* tt: CSS equivalent 'font-family:monospace'
             * Tidy presently does nothing. Tidy5 issues a warning
             * But like the 'clean' <font> replacement this could also be replaced with CSS
             * maybe should use static void RenameElem( TidyDocImpl* doc, Node* node, TidyTagId tid )
             */
            if (clean) {
                TY_(AddStyleProperty)( doc, node, "font-family: monospace" );
                TY_(CoerceNode)(doc, node, TidyTag_SPAN, warn, no);
            } else {
                if ( !already_strict )
                    TY_(Report)(doc, node, node, REMOVED_HTML5);
            }
        } else
            if (TY_(nodeIsElement)(node)) {
                if (node->tag) {
                    if ( (!(node->tag->versions & VERS_HTML5) && !(node->tag->versions & VERS_PROPRIETARY)) || (inRemovedInfo(node->tag->id)) ) {
                        if ( !already_strict )
                            TY_(Report)(doc, node, node, REMOVED_HTML5);
                    }
                }
            }

        if (node->content)
        {
            TY_(push)(stack, next);
            node = node->content;
            continue;
        }

        node = next ? next : TY_(pop)(stack);
    }
    TY_(freeStack)(stack);
}
/*****************************************************************************
 *  END HTML5 STUFF
 *****************************************************************************/


/*
 * Check and report HTML tags and attributes that are:
 *  - Proprietary, and/or
 *  - Not supported in the current version of HTML, defined as the version
 *    of HTML that we are emitting.
 * Proprietary items are reported as WARNINGS, and version mismatches will
 * be reported as WARNING or ERROR in the following conditions:
 *  - ERROR if the emitted doctype is a strict doctype.
 *  - WARNING if the emitted doctype is a non-strict doctype.
 * The propriety checks are *always* run as they have always been an integral
 * part of Tidy. The version checks are controlled by `strict-tags-attributes`.
 */
static void TY_(CheckHTMLTagsAttribsVersions)( TidyDocImpl* doc, Node* node )
{
    Stack *stack = TY_(newStack)(doc, 16);
    Node *next;
    uint versionEmitted = doc->lexer->versionEmitted;
    uint declared = doc->lexer->doctype;
    uint version = versionEmitted == 0 ? declared : versionEmitted;
    int tagReportType = VERS_STRICT & version ? ELEMENT_VERS_MISMATCH_ERROR : ELEMENT_VERS_MISMATCH_WARN;
    int attrReportType = VERS_STRICT & version ? MISMATCHED_ATTRIBUTE_ERROR : MISMATCHED_ATTRIBUTE_WARN;
    Bool check_versions = cfgBool( doc, TidyStrictTagsAttr );
    AttVal *next_attr, *attval;
    Bool attrIsProprietary = no;
    Bool attrIsMismatched = yes;
    Bool tagLooksCustom = no;
    Bool htmlIs5 = (doc->lexer->doctype & VERS_HTML5) > 0;

    while (node)
    {
        next = node->next;

        /* This bit here handles our HTML tags */
        if ( TY_(nodeIsElement)(node) && node->tag ) {

            /* Leave XML stuff alone. */
            if ( !cfgBool(doc, TidyXmlTags) )
            {
                /* Version mismatches take priority. */
                if ( check_versions && !(node->tag->versions & version) )
                {
                    TY_(Report)(doc, NULL, node, tagReportType );
                }
                /* If it's not mismatched, it could still be proprietary. */
                else if ( node->tag->versions & VERS_PROPRIETARY )
                {
                    if ( !cfgBool(doc, TidyMakeClean) ||
                        ( !nodeIsNOBR(node) && !nodeIsWBR(node) ) )
                    {
                        /* It looks custom, despite whether it's a known tag. */
                        tagLooksCustom = TY_(nodeIsAutonomousCustomFormat)( node );

                        /* If we're in HTML5 mode and the tag does not look
                           like a valid custom tag, then issue a warning.
                           Appearance is good enough because invalid tags have
                           been dropped. Also, if we're not in HTML5 mode, then
                           then everything that reaches here gets the warning.
                           Everything else can be ignored. */

                        if ( (htmlIs5 && !tagLooksCustom) || !htmlIs5 )
                        {
                            TY_(Report)(doc, NULL, node, PROPRIETARY_ELEMENT );
                        }

                        if ( nodeIsLAYER(node) )
                            doc->badLayout |= USING_LAYER;
                        else if ( nodeIsSPACER(node) )
                            doc->badLayout |= USING_SPACER;
                        else if ( nodeIsNOBR(node) )
                            doc->badLayout |= USING_NOBR;
                    }
                }
            }
        }

        /* And this bit here handles our attributes */
        if (TY_(nodeIsElement)(node))
        {
            attval = node->attributes;

            while (attval)
            {
                next_attr = attval->next;

                attrIsProprietary = TY_(AttributeIsProprietary)(node, attval);
                /* Is. #729 - always check version match if HTML5 */
                attrIsMismatched = (check_versions | htmlIs5) ? TY_(AttributeIsMismatched)(node, attval, doc) : no;
                /* Let the PROPRIETARY_ATTRIBUTE warning have precedence. */
                if ( attrIsProprietary )
                {
                    if ( cfgBool(doc, TidyWarnPropAttrs) )
                        TY_(ReportAttrError)(doc, node, attval, PROPRIETARY_ATTRIBUTE);
                }
                else if ( attrIsMismatched )
                {
                    if (htmlIs5)
                    {
                        /* Is. #729 - In html5 TidyStrictTagsAttr controls error or warn */
                        TY_(ReportAttrError)(doc, node, attval,
                            check_versions ? MISMATCHED_ATTRIBUTE_ERROR : MISMATCHED_ATTRIBUTE_WARN);
                    }
                    else
                        TY_(ReportAttrError)(doc, node, attval, attrReportType);

                }

                /* @todo: do we need a new option to drop mismatches? Or should we
                 simply drop them? */
                if ( ( attrIsProprietary || attrIsMismatched ) && cfgBool(doc, TidyDropPropAttrs) )
                    TY_(RemoveAttribute)( doc, node, attval );

                attval = next_attr;
            }
        }

        if (node->content)
        {
            TY_(push)(stack, next);
            node = node->content;
            continue;
        }

        node = next ? next : TY_(pop)(stack);
    }
    TY_(freeStack)(stack);
}


#if defined(ENABLE_DEBUG_LOG)
/* *** FOR DEBUG ONLY *** */
const char *dbg_get_lexer_type( void *vp )
{
    Node *node = (Node *)vp;
    switch ( node->type )
    {
    case RootNode:      return "Root";
    case DocTypeTag:    return "DocType";
    case CommentTag:    return "Comment";
    case ProcInsTag:    return "ProcIns";
    case TextNode:      return "Text";
    case StartTag:      return "StartTag";
    case EndTag:        return "EndTag";
    case StartEndTag:   return "StartEnd";
    case CDATATag:      return "CDATA";
    case SectionTag:    return "Section";
    case AspTag:        return "Asp";
    case JsteTag:       return "Jste";
    case PhpTag:        return "Php";
    case XmlDecl:       return "XmlDecl";
    }
    return "Uncased";
}

/* NOTE: THis matches the above lexer type, except when element has a name */
const char *dbg_get_element_name( void *vp )
{
    Node *node = (Node *)vp;
    switch ( node->type )
    {
    case TidyNode_Root:       return "Root";
    case TidyNode_DocType:    return "DocType";
    case TidyNode_Comment:    return "Comment";
    case TidyNode_ProcIns:    return "ProcIns";
    case TidyNode_Text:       return "Text";
    case TidyNode_CDATA:      return "CDATA";
    case TidyNode_Section:    return "Section";
    case TidyNode_Asp:        return "Asp";
    case TidyNode_Jste:       return "Jste";
    case TidyNode_Php:        return "Php";
    case TidyNode_XmlDecl:    return "XmlDecl";

    case TidyNode_Start:
    case TidyNode_End:
    case TidyNode_StartEnd:
    default:
        if (node->element)
            return node->element;
    }
    return "Unknown";
}

void dbg_show_node( TidyDocImpl* doc, Node *node, int caller, int indent )
{
    AttVal* av;
    Lexer* lexer = doc->lexer;
    ctmbstr call = "";
    ctmbstr name = dbg_get_element_name(node);
    ctmbstr type = dbg_get_lexer_type(node);
    ctmbstr impl = node->implicit ? "implicit" : "";
    switch ( caller )
    {
    case 1: call = "discard";   break;
    case 2: call = "trim";      break;
    case 3: call = "test";      break;
    }
    while (indent--)
        SPRTF(" ");
    if (strcmp(type,name))
        SPRTF("%s %s %s %s", type, name, impl, call );
    else
        SPRTF("%s %s %s", name, impl, call );
    if (lexer && (strcmp("Text",name) == 0)) {
        uint len = node->end - node->start;
        uint i;
        SPRTF(" (%d) '", len);
        if (len < 40) {
            /* show it all */
            for (i = node->start; i < node->end; i++) {
                SPRTF("%c", lexer->lexbuf[i]);
            }
        } else {
            /* partial display */
            uint max = 19;
            for (i = node->start; i < max; i++) {
                SPRTF("%c", lexer->lexbuf[i]);
            }
            SPRTF("...");
            i = node->end - 19;
            for (; i < node->end; i++) {
                SPRTF("%c", lexer->lexbuf[i]);
            }
        }
        SPRTF("'");
    }
    for (av = node->attributes; av; av = av->next) {
        name = av->attribute;
        if (name) {
            SPRTF(" %s",name);
            if (av->value) {
                SPRTF("=\"%s\"", av->value);
            }
        }
    }

    SPRTF("\n");
}

/* Make this non-recursive, because we really do want to eliminate
   recursion that makes us crash, even when debugging.
 */
void dbg_show_all_nodes( TidyDocImpl* doc, Node *node, int indent )
{
    Stack *stack = TY_(newStack)(doc, 16);
    Node *child = NULL;
    Node *next = NULL;

    dbg_show_node( doc, node, 0, indent++ );

    if ( (child = node->content) )
    {
        while ( child )
        {
            if ( (next = child->next) )
            {
                next->idx = indent;
            }

            dbg_show_node( doc, child, 0, indent );

            if (child->content)
            {
                TY_(push)(stack, next);
                indent++;
                child = child->content;
                continue;
            }

            if (next)
            {
                child = next;
            }
            else
            {
                if ( (child = TY_(pop)(stack)) )
                {
                    indent = child->idx;
                }
            }

        }
        TY_(freeStack)(stack);
    }
}
#endif

int         tidyDocCleanAndRepair( TidyDocImpl* doc )
{
    Bool word2K   = cfgBool( doc, TidyWord2000 );
    Bool logical  = cfgBool( doc, TidyLogicalEmphasis );
    Bool clean    = cfgBool( doc, TidyMakeClean );
    Bool gdoc     = cfgBool( doc, TidyGDocClean );
    Bool htmlOut  = cfgBool( doc, TidyHtmlOut );
    Bool xmlOut   = cfgBool( doc, TidyXmlOut );
    Bool xhtmlOut = cfgBool( doc, TidyXhtmlOut );
    Bool xmlDecl  = cfgBool( doc, TidyXmlDecl );
    Bool tidyMark = cfgBool( doc, TidyMark );
    Bool tidyXmlTags = cfgBool( doc, TidyXmlTags );
    Bool wantNameAttr = cfgBool( doc, TidyAnchorAsName );
    Bool mergeEmphasis = cfgBool( doc, TidyMergeEmphasis );
    Node* node;
    TidyConfigChangeCallback callback = doc->pConfigChangeCallback;
    doc->pConfigChangeCallback = NULL;

#if defined(ENABLE_DEBUG_LOG)
    SPRTF("All nodes BEFORE clean and repair\n");
    dbg_show_all_nodes( doc, &doc->root, 0  );
#endif
    if (tidyXmlTags)
    {
        doc->pConfigChangeCallback = callback;
        return tidyDocStatus( doc );
    }

    /* Issue #567 - move style elements from body to head */
    TY_(CleanStyle)(doc, &doc->root);

    /* simplifies <b><b> ... </b> ...</b> etc. */
    if ( mergeEmphasis )
        TY_(NestedEmphasis)( doc, &doc->root );

    /* cleans up <dir>indented text</dir> etc. */
    TY_(List2BQ)( doc, &doc->root );
    TY_(BQ2Div)( doc, &doc->root );

    /* replaces i by em and b by strong */
    if ( logical )
        TY_(EmFromI)( doc, &doc->root );

    if ( word2K && TY_(IsWord2000)(doc) )
    {
        /* prune Word2000's <![if ...]> ... <![endif]> */
        TY_(DropSections)( doc, &doc->root );

        /* drop style & class attributes and empty p, span elements */
        TY_(CleanWord2000)( doc, &doc->root );
        TY_(DropEmptyElements)(doc, &doc->root);
    }

    /* replaces presentational markup by style rules */
    if ( clean )
        TY_(CleanDocument)( doc );

    /* clean up html exported by Google Docs */
    if ( gdoc )
        TY_(CleanGoogleDocument)( doc );

    /*  Reconcile http-equiv meta element with output encoding  */
    TY_(TidyMetaCharset)(doc);

    if ( !TY_(CheckNodeIntegrity)( &doc->root ) )
        TidyPanic( doc->allocator, integrity );

    /* remember given doctype for reporting */
    node = TY_(FindDocType)(doc);

    if (node)
    {
        AttVal* fpi = TY_(GetAttrByName)(node, "PUBLIC");
        if (AttrHasValue(fpi))
        {
            if (doc->givenDoctype)
                TidyDocFree(doc, doc->givenDoctype);
            doc->givenDoctype = TY_(tmbstrdup)(doc->allocator,fpi->value);
        }
    }

    if ( doc->root.content )
    {
        /* If we had XHTML input but want HTML output */
        if ( htmlOut && doc->lexer->isvoyager )
        {
            Node* node = TY_(FindDocType)(doc);
            /* Remove reference, but do not free */
            if (node)
              TY_(RemoveNode)(node);
        }

        if (xhtmlOut && !htmlOut)
        {
            TY_(SetXHTMLDocType)(doc);
            TY_(FixAnchors)(doc, &doc->root, wantNameAttr, yes);
            TY_(FixXhtmlNamespace)(doc, yes);
            TY_(FixLanguageInformation)(doc, &doc->root, yes, yes);
        }
        else
        {
            TY_(FixDocType)(doc);
            TY_(FixAnchors)(doc, &doc->root, wantNameAttr, yes);
            TY_(FixXhtmlNamespace)(doc, no);
            TY_(FixLanguageInformation)(doc, &doc->root, no, yes);
        }

        if (tidyMark )
            TY_(AddGenerator)(doc);

    }

    /* ensure presence of initial <?xml version="1.0"?> */
    if ( xmlOut && xmlDecl )
        TY_(FixXmlDecl)( doc );

    /* At this point the apparent doctype is going to be as stable as
       it can ever be, so we can start detecting things that shouldn't
       be in this version of HTML
     */
    if (doc->lexer)
    {
        /*\
         *  Issue #429 #426 - These services can only be used
         *  when there is a document loaded, ie a lexer created.
         *  But really should not be calling a Clean and Repair
         *  service with no doc!
        \*/
        if (doc->lexer->versionEmitted & VERS_HTML5)
            TY_(CheckHTML5)( doc, &doc->root );
        TY_(CheckHTMLTagsAttribsVersions)( doc, &doc->root );

        if ( !doc->lexer->isvoyager && doc->xmlDetected )
        {
            TY_(Report)(doc, NULL, TY_(FindXmlDecl)(doc), XML_DECLARATION_DETECTED );

        }
    }

    TY_(CleanHead)(doc); /* Is #692 - discard multiple <title> tags */

#if defined(ENABLE_DEBUG_LOG)
    SPRTF("All nodes AFTER clean and repair\n");
    dbg_show_all_nodes( doc, &doc->root, 0  );
#endif

    doc->pConfigChangeCallback = callback;
    return tidyDocStatus( doc );
}

static
Bool showBodyOnly( TidyDocImpl* doc, TidyTriState bodyOnly )
{
    Node* node;

    switch( bodyOnly )
    {
    case TidyNoState:
        return no;
    case TidyYesState:
        return yes;
    default:
        node = TY_(FindBody)( doc );
        if (node && node->implicit )
            return yes;
    }
    return no;
}


int         tidyDocSaveStream( TidyDocImpl* doc, StreamOut* out )
{
    Bool showMarkup  = cfgBool( doc, TidyShowMarkup );
    Bool forceOutput = cfgBool( doc, TidyForceOutput );
    Bool outputBOM   = ( cfgAutoBool(doc, TidyOutputBOM) == TidyYesState );
    Bool smartBOM    = ( cfgAutoBool(doc, TidyOutputBOM) == TidyAutoState );
    Bool xmlOut      = cfgBool( doc, TidyXmlOut );
    Bool xhtmlOut    = cfgBool( doc, TidyXhtmlOut );
    TidyTriState bodyOnly    = cfgAutoBool( doc, TidyBodyOnly );

    Bool dropComments = cfgBool(doc, TidyHideComments);
    Bool makeClean    = cfgBool(doc, TidyMakeClean);
    Bool asciiChars   = cfgBool(doc, TidyAsciiChars);
    Bool makeBare     = cfgBool(doc, TidyMakeBare);
    Bool escapeCDATA  = cfgBool(doc, TidyEscapeCdata);
    TidyAttrSortStrategy sortAttrStrat = cfg(doc, TidySortAttributes);
    TidyConfigChangeCallback callback = doc->pConfigChangeCallback;
    doc->pConfigChangeCallback = NULL;


    if (escapeCDATA)
        TY_(ConvertCDATANodes)(doc, &doc->root);

    if (dropComments)
        TY_(DropComments)(doc, &doc->root);

    if (makeClean)
    {
        /* noop */
        TY_(DropFontElements)(doc, &doc->root, NULL);
    }

    if ((makeClean && asciiChars) || makeBare)
        TY_(DowngradeTypography)(doc, &doc->root);

    if (makeBare)
        /* Note: no longer replaces &nbsp; in */
        /* attribute values / non-text tokens */
        TY_(NormalizeSpaces)(doc->lexer, &doc->root);
    else
        TY_(ReplacePreformattedSpaces)(doc, &doc->root);

    TY_(SortAttributes)(doc, &doc->root, sortAttrStrat);

    if ( showMarkup && (doc->errors == 0 || forceOutput) )
    {
        /* Output a Byte Order Mark if required */
        if ( outputBOM || (doc->inputHadBOM && smartBOM) )
            TY_(outBOM)( out );

        /* No longer necessary. No DOCTYPE == HTML 3.2,
        ** which gives you only the basic character entities,
        ** which are safe in any browser.
        ** if ( !TY_(FindDocType)(doc) )
        **    TY_(SetOptionBool)( doc, TidyNumEntities, yes );
        */

        doc->docOut = out;
        if ( xmlOut && !xhtmlOut )
            TY_(PPrintXMLTree)( doc, NORMAL, 0, &doc->root );
        else if ( showBodyOnly( doc, bodyOnly ) )
            TY_(PrintBody)( doc );
        else
            TY_(PPrintTree)( doc, NORMAL, 0, &doc->root );

        TY_(PFlushLine)( doc, 0 );
        doc->docOut = NULL;
    }

    /* @jsd: removing this should solve #673, and allow saving of the buffer multiple times. */
//    TY_(ResetConfigToSnapshot)( doc );
    doc->pConfigChangeCallback = callback;

    return tidyDocStatus( doc );
}

/* Tree traversal functions
**
** The big issue here is the degree to which we should mimic
** a DOM and/or SAX nodes.
**
** Is it 100% possible (and, if so, how difficult is it) to
** emit SAX events from this API?  If SAX events are possible,
** is that 100% of data needed to build a DOM?
*/

TidyNode   tidyGetRoot( TidyDoc tdoc )
{
    TidyDocImpl* impl = tidyDocToImpl( tdoc );
    Node* node = NULL;
    if ( impl )
        node = &impl->root;
    return tidyImplToNode( node );
}

TidyNode   tidyGetHtml( TidyDoc tdoc )
{
  TidyDocImpl* impl = tidyDocToImpl( tdoc );
  Node* node = NULL;
  if ( impl )
      node = TY_(FindHTML)( impl );
  return tidyImplToNode( node );
}

TidyNode    tidyGetHead( TidyDoc tdoc )
{
  TidyDocImpl* impl = tidyDocToImpl( tdoc );
  Node* node = NULL;
  if ( impl )
      node = TY_(FindHEAD)( impl );
  return tidyImplToNode( node );
}

TidyNode    tidyGetBody( TidyDoc tdoc )
{
  TidyDocImpl* impl = tidyDocToImpl( tdoc );
  Node* node = NULL;
  if ( impl )
      node = TY_(FindBody)( impl );
  return tidyImplToNode( node );
}

/* parent / child */
TidyNode    tidyGetParent( TidyNode tnod )
{
  Node* nimp = tidyNodeToImpl( tnod );
  return tidyImplToNode( nimp->parent );
}
TidyNode    tidyGetChild( TidyNode tnod )
{
  Node* nimp = tidyNodeToImpl( tnod );
  return tidyImplToNode( nimp->content );
}

/* remove a node */
TidyNode    tidyDiscardElement( TidyDoc tdoc, TidyNode tnod )
{
  TidyDocImpl* doc = tidyDocToImpl( tdoc );
  Node* nimp = tidyNodeToImpl( tnod );
  Node* next = TY_(DiscardElement)( doc, nimp );
  return tidyImplToNode( next );
}

/* siblings */
TidyNode    tidyGetNext( TidyNode tnod )
{
  Node* nimp = tidyNodeToImpl( tnod );
  return tidyImplToNode( nimp->next );
}
TidyNode    tidyGetPrev( TidyNode tnod )
{
  Node* nimp = tidyNodeToImpl( tnod );
  return tidyImplToNode( nimp->prev );
}

/* Node info */
TidyNodeType tidyNodeGetType( TidyNode tnod )
{
  Node* nimp = tidyNodeToImpl( tnod );
  TidyNodeType ntyp = TidyNode_Root;
  if ( nimp )
    ntyp = (TidyNodeType) nimp->type;
  return ntyp;
}

uint tidyNodeLine( TidyNode tnod )
{
  Node* nimp = tidyNodeToImpl( tnod );
  uint line = 0;
  if ( nimp )
    line = nimp->line;
  return line;
}
uint tidyNodeColumn( TidyNode tnod )
{
  Node* nimp = tidyNodeToImpl( tnod );
  uint col = 0;
  if ( nimp )
    col = nimp->column;
  return col;
}

ctmbstr tidyNodeGetName( TidyNode tnod )
{
  Node* nimp = tidyNodeToImpl( tnod );
  ctmbstr nnam = NULL;
  if ( nimp )
    nnam = nimp->element;
  return nnam;
}


Bool tidyNodeHasText( TidyDoc tdoc, TidyNode tnod )
{
  TidyDocImpl* doc = tidyDocToImpl( tdoc );
  if ( doc )
      return TY_(nodeHasText)( doc, tidyNodeToImpl(tnod) );
  return no;
}


Bool tidyNodeGetText( TidyDoc tdoc, TidyNode tnod, TidyBuffer* outbuf )
{
  TidyDocImpl* doc = tidyDocToImpl( tdoc );
  Node* nimp = tidyNodeToImpl( tnod );
  if ( doc && nimp && outbuf )
  {
      uint outenc     = cfg( doc, TidyOutCharEncoding );
      uint nl         = cfg( doc, TidyNewline );
      StreamOut* out  = TY_(BufferOutput)( doc, outbuf, outenc, nl );
      Bool xmlOut     = cfgBool( doc, TidyXmlOut );
      Bool xhtmlOut   = cfgBool( doc, TidyXhtmlOut );

      doc->docOut = out;
      if ( xmlOut && !xhtmlOut )
          TY_(PPrintXMLTree)( doc, NORMAL, 0, nimp );
      else
          TY_(PPrintTree)( doc, NORMAL, 0, nimp );

      TY_(PFlushLine)( doc, 0 );
      doc->docOut = NULL;

      TidyDocFree( doc, out );
      return yes;
  }
  return no;
}

Bool tidyNodeGetValue( TidyDoc tdoc, TidyNode tnod, TidyBuffer* buf )
{
    TidyDocImpl *doc = tidyDocToImpl( tdoc );
    Node *node = tidyNodeToImpl( tnod );
    if ( doc == NULL || node == NULL || buf == NULL )
        return no;

    switch( node->type ) {
    case TextNode:
    case CDATATag:
    case CommentTag:
    case ProcInsTag:
    case SectionTag:
    case AspTag:
    case JsteTag:
    case PhpTag:
    {
        tidyBufClear( buf );
        tidyBufAppend( buf, doc->lexer->lexbuf + node->start,
                       node->end - node->start );
        break;
    }
    default:
        /* The node doesn't have a value */
        return no;
    }

    return yes;
}

Bool tidyNodeIsProp( TidyDoc ARG_UNUSED(tdoc), TidyNode tnod )
{
  Node* nimp = tidyNodeToImpl( tnod );
  Bool isProprietary = yes;
  if ( nimp )
  {
    switch ( nimp->type )
    {
    case RootNode:
    case DocTypeTag:
    case CommentTag:
    case XmlDecl:
    case ProcInsTag:
    case TextNode:
    case CDATATag:
        isProprietary = no;
        break;

    case SectionTag:
    case AspTag:
    case JsteTag:
    case PhpTag:
        isProprietary = yes;
        break;

    case StartTag:
    case EndTag:
    case StartEndTag:
        isProprietary = ( nimp->tag
                          ? (nimp->tag->versions&VERS_PROPRIETARY)!=0
                          : yes );
        break;
    }
  }
  return isProprietary;
}

TidyTagId tidyNodeGetId(TidyNode tnod)
{
    Node* nimp = tidyNodeToImpl(tnod);

    TidyTagId tagId = TidyTag_UNKNOWN;
    if (nimp && nimp->tag)
        tagId = nimp->tag->id;

    return tagId;
}


/* Iterate over attribute values */
TidyAttr   tidyAttrFirst( TidyNode tnod )
{
  Node* nimp = tidyNodeToImpl( tnod );
  AttVal* attval = NULL;
  if ( nimp )
    attval = nimp->attributes;
  return tidyImplToAttr( attval );
}
TidyAttr    tidyAttrNext( TidyAttr tattr )
{
  AttVal* attval = tidyAttrToImpl( tattr );
  AttVal* nxtval = NULL;
  if ( attval )
    nxtval = attval->next;
  return tidyImplToAttr( nxtval );
}

ctmbstr       tidyAttrName( TidyAttr tattr )
{
  AttVal* attval = tidyAttrToImpl( tattr );
  ctmbstr anam = NULL;
  if ( attval )
    anam = attval->attribute;
  return anam;
}
ctmbstr       tidyAttrValue( TidyAttr tattr )
{
  AttVal* attval = tidyAttrToImpl( tattr );
  ctmbstr aval = NULL;
  if ( attval )
    aval = attval->value;
  return aval;
}

void           tidyAttrDiscard( TidyDoc tdoc, TidyNode tnod, TidyAttr tattr )
{
  TidyDocImpl* impl = tidyDocToImpl( tdoc );
  Node* nimp = tidyNodeToImpl( tnod );
  AttVal* attval = tidyAttrToImpl( tattr );
  TY_(RemoveAttribute)( impl, nimp, attval );
}

TidyAttrId tidyAttrGetId( TidyAttr tattr )
{
  AttVal* attval = tidyAttrToImpl( tattr );
  TidyAttrId attrId = TidyAttr_UNKNOWN;
  if ( attval && attval->dict )
    attrId = attval->dict->id;
  return attrId;
}

TidyAttr tidyAttrGetById( TidyNode tnod, TidyAttrId attId )
{
    Node* nimp = tidyNodeToImpl(tnod);
    return tidyImplToAttr( TY_(AttrGetById)( nimp, attId ) );
}


Bool tidyAttrIsEvent( TidyAttr tattr )
{
    return TY_(attrIsEvent)( tidyAttrToImpl(tattr) );
}


/*******************************************************************
 ** Message Key Management
 *******************************************************************/
ctmbstr tidyErrorCodeAsKey(uint code)
{
    return TY_(tidyErrorCodeAsKey)( code );
}

uint tidyErrorCodeFromKey(ctmbstr code)
{
    return TY_(tidyErrorCodeFromKey)( code );
}

TidyIterator getErrorCodeList()
{
    return TY_(getErrorCodeList)();
}

uint getNextErrorCode( TidyIterator* iter )
{
    return TY_(getNextErrorCode)(iter);
}


/*******************************************************************
 ** Localization Support
 *******************************************************************/


Bool tidySetLanguage( ctmbstr languageCode )
{
    Bool result = TY_(tidySetLanguage)( languageCode );

    if ( result )
        TY_(tidySetLanguageSetByUser)();

    return result;
}

ctmbstr tidyGetLanguage()
{
    return TY_(tidyGetLanguage)();
}

ctmbstr tidyLocalizedStringN( uint messageType, uint quantity )
{
    return TY_(tidyLocalizedStringN)( messageType, quantity);
}

ctmbstr tidyLocalizedString( uint messageType )
{
    return TY_(tidyLocalizedString)( messageType );
}

ctmbstr tidyDefaultStringN( uint messageType, uint quantity )
{
    return TY_(tidyDefaultStringN)( messageType, quantity);
}

ctmbstr tidyDefaultString( uint messageType )
{
    return TY_(tidyDefaultString)( messageType );
}

TidyIterator getStringKeyList()
{
    return TY_(getStringKeyList)();
}

uint getNextStringKey( TidyIterator* iter )
{
    return TY_(getNextStringKey)( iter );
}

TidyIterator getWindowsLanguageList()
{
    return TY_(getWindowsLanguageList)();
}

//#define tidyOptionToImpl( topt )    ((const TidyOptionImpl*)(topt))
//#define tidyImplToOption( option )  ((TidyOption)(option))

const tidyLocaleMapItem* getNextWindowsLanguage( TidyIterator* iter )
{
    /* Get a real structure */
    const tidyLocaleMapItemImpl *item = TY_(getNextWindowsLanguage)( iter );

    /* Return it as the opaque version */
    return ((tidyLocaleMapItem*)(item));
}


ctmbstr TidyLangWindowsName( const tidyLocaleMapItem *item )
{
    return TY_(TidyLangWindowsName)( (tidyLocaleMapItemImpl*)(item) );
}


ctmbstr TidyLangPosixName( const tidyLocaleMapItem *item )
{
    return TY_(TidyLangPosixName)( (tidyLocaleMapItemImpl*)(item) );
}


TidyIterator getInstalledLanguageList()
{
    return TY_(getInstalledLanguageList)();
}


ctmbstr getNextInstalledLanguage( TidyIterator* iter )
{
    return TY_(getNextInstalledLanguage)( iter );
}




/*
 * local variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 4
 * end:
 */
