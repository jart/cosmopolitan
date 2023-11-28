#ifndef __TIDY_INT_H__
#define __TIDY_INT_H__

/* tidy-int.h -- internal library declarations

  (c) 1998-2007 (W3C) MIT, ERCIM, Keio University
  See tidy.h for the copyright notice.

*/

#include "third_party/tidy/tidy.h"
#include "third_party/tidy/config.h"
#include "third_party/tidy/lexer.h"
#include "third_party/tidy/tags.h"
#include "third_party/tidy/attrs.h"
#include "third_party/tidy/pprint.h"
#include "third_party/tidy/access.h"
#include "third_party/tidy/message.h"
#include "libc/time/struct/utimbuf.h"
#include "third_party/tidy/parser.h"

#ifndef MAX
#define MAX(a,b) (((a) > (b))?(a):(b))
#endif
#ifndef MIN
#define MIN(a,b) (((a) < (b))?(a):(b))
#endif

/*\
 *  Issue #166 - repeated <main> element
 *  Change the previous on/off uint flag badForm
 *  to a BIT flag to support other than <form>
 *  errors. This could be extended more...
\*/
#define flg_BadForm     0x00000001
#define flg_BadMain     0x00000002

struct _TidyDocImpl
{
    /* The Document Tree (and backing store buffer) */
    Node                root;       /* This MUST remain the first declared
                                       variable in this structure */
    Lexer*              lexer;

    /* Config + Markup Declarations */
    TidyConfigImpl           config;
    TidyTagImpl              tags;
    TidyAttribImpl           attribs;
    TidyAccessImpl           access;
    TidyMutedMessages        muted;

    /* The Pretty Print buffer */
    TidyPrintImpl            pprint;

    /* I/O */
    StreamIn*                docIn;
    StreamOut*               docOut;
    StreamOut*               errout;

    TidyReportFilter         reportFilter;
    TidyReportCallback       reportCallback;
    TidyMessageCallback      messageCallback;
    TidyOptCallback          pOptCallback;
    TidyConfigCallback       pConfigCallback;
    TidyConfigChangeCallback pConfigChangeCallback;
    TidyPPProgress           progressCallback;

    TidyParserStack          stack;

    /* Parse + Repair Results */
    uint                optionErrors;
    uint                errors;
    uint                warnings;
    uint                accessErrors;
    uint                infoMessages;
    uint                docErrors;
    int                 parseStatus;

    uint                badAccess;   /* for accessibility errors */
    uint                badLayout;   /* for bad style errors */
    uint                badChars;    /* for bad char encodings */
    uint                badForm;     /* bit field, for badly placed form tags, or other format errors */
    uint                footnotes;   /* bit field, for other footnotes, until formalized */

    Bool                HTML5Mode;   /* current mode is html5 */
    Bool                xmlDetected; /* true if XML was used/detected */

    uint                indent_char; /* space or tab character, for indenting */

    /* Memory allocator */
    TidyAllocator*      allocator;

    /* Miscellaneous */
    void*               appData;
    uint                nClassId;
    Bool                inputHadBOM;

#if PRESERVE_FILE_TIMES
    struct utimbuf      filetimes;
#endif
    tmbstr              givenDoctype;
};

/** The basic struct for communicating a message within LibTidy. All of the
**  relevant information pertaining to a message can be retrieved with the
**  accessor functions and one of these records.
*/
struct _TidyMessageImpl
{
    TidyDocImpl         *tidyDoc;     /* document instance this message is attributed to */
    Node                *tidyNode;    /* the node reporting the message, if applicable */
    uint                code;         /* the message code */
    int                 line;         /* the line message applies to */
    int                 column;       /* the column the message applies to */
    TidyReportLevel     level;        /* the severity level of the message */
    Bool                allowMessage; /* indicates whether or not a filter rejected a message */
    Bool                muted;        /* indicates whether or not a configuration mutes this message */

    int                 argcount;    /* the number of arguments */
    struct printfArg*   arguments;   /* the arguments' values and types */

    ctmbstr             messageKey;             /* the message code as a key string */

    ctmbstr             messageFormatDefault;   /* the built-in format string */
    ctmbstr             messageFormat;          /* the localized format string */

    tmbstr              messageDefault;         /* the message, formatted, default language */
    tmbstr              message;                /* the message, formatted, localized */

    tmbstr              messagePosDefault;      /* the position part, default language */
    tmbstr              messagePos;             /* the position part, localized */

    ctmbstr             messagePrefixDefault;   /* the prefix part, default language */
    ctmbstr             messagePrefix;          /* the prefix part, localized */

    tmbstr              messageOutputDefault;   /* the complete string Tidy would output */
    tmbstr              messageOutput;          /* the complete string, localized */
};


#define tidyDocToImpl( tdoc )           ((TidyDocImpl*)(tdoc))
#define tidyImplToDoc( doc )            ((TidyDoc)(doc))

#define tidyMessageToImpl( tmessage )   ((TidyMessageImpl*)(tmessage))
#define tidyImplToMessage( message )    ((TidyMessage)(message))

#define tidyNodeToImpl( tnod )          ((Node*)(tnod))
#define tidyImplToNode( node )          ((TidyNode)(node))

#define tidyAttrToImpl( tattr )         ((AttVal*)(tattr))
#define tidyImplToAttr( attval )        ((TidyAttr)(attval))

#define tidyOptionToImpl( topt )        ((const TidyOptionImpl*)(topt))
#define tidyImplToOption( option )      ((TidyOption)(option))


/** Wrappers for easy memory allocation using the document's allocator */
#define TidyDocAlloc(doc, size) TidyAlloc((doc)->allocator, size)
#define TidyDocRealloc(doc, block, size) TidyRealloc((doc)->allocator, block, size)
#define TidyDocFree(doc, block) TidyFree((doc)->allocator, block)
#define TidyDocPanic(doc, msg) TidyPanic((doc)->allocator, msg)

int          TY_(DocParseStream)( TidyDocImpl* impl, StreamIn* in );

/*
   [i_a] generic node tree traversal code; used in several spots.

   Define your own callback, which returns one of the NodeTraversalSignal values
   to instruct the tree traversal routine TraverseNodeTree() what to do.

   Pass custom data to/from the callback using the 'propagate' reference.
 */
typedef enum
{
    ContinueTraversal,       /* visit siblings and children */
    SkipChildren,            /* visit siblings of this node; ignore its children */
    SkipSiblings,            /* ignore subsequent siblings of this node; ignore their children; traverse  */
    SkipChildrenAndSiblings, /* visit siblings of this node; ignore its children */
    VisitParent,             /* REVERSE traversal: visit the parent of the current node */
    ExitTraversal            /* terminate traversal on the spot */
} NodeTraversalSignal;

typedef NodeTraversalSignal NodeTraversalCallBack(TidyDocImpl* doc, Node* node, void *propagate);

NodeTraversalSignal TY_(TraverseNodeTree)(TidyDocImpl* doc, Node* node, NodeTraversalCallBack *cb, void *propagate);

#endif /* __TIDY_INT_H__ */
