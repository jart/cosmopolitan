#ifndef __LEXER_H__
#define __LEXER_H__


/**************************************************************************//**
 * @file
 * Lexer for HTML and XML Parsers.
 *
 *   Given an input source, it returns a sequence of tokens.
 *
 *      GetToken(source) gets the next token
 *      UngetToken(source) provides one level undo
 *
 *   The tags include an attribute list:
 *
 *     - linked list of attribute/value nodes
 *     - each node has 2 NULL-terminated strings.
 *     - entities are replaced in attribute values
 *
 *   white space is compacted if not in preformatted mode
 *   If not in preformatted mode then leading white space
 *   is discarded and subsequent white space sequences
 *   compacted to single space characters.
 *
 *   If XmlTags is no then Tag names are folded to upper
 *   case and attribute names to lower case.
 *
 *  Not yet done:
 *     - Doctype subset and marked sections
 *
 * @author  HTACG, et al (consult git log)
 *
 * @copyright
 *     (c) 1998-2021 (W3C) MIT, ERCIM, Keio University, and HTACG.
 *     See tidy.h for the copyright notice.
 * @par
 *     All Rights Reserved.
 * @par
 *     See `tidy.h` for the complete license.
 *
 * @date Additional updates: consult git log
 *
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "third_party/tidy/forward.h"

/** @addtogroup internal_api */
/** @{ */


/***************************************************************************//**
 ** @defgroup lexer_h HTML and XML Lexing
 **
 ** These functions and structures form the internal API for document
 ** lexing.
 **
 ** @{
 ******************************************************************************/


/**
 *  Lexer character types.
 */
#define digit       1u
#define letter      2u
#define namechar    4u
#define white       8u
#define newline     16u
#define lowercase   32u
#define uppercase   64u
#define digithex    128u


/**
 *  node->type is one of these values
 */
typedef enum
{
  RootNode,
  DocTypeTag,
  CommentTag,
  ProcInsTag,
  TextNode,
  StartTag,
  EndTag,
  StartEndTag,
  CDATATag,
  SectionTag,
  AspTag,
  JsteTag,
  PhpTag,
  XmlDecl
} NodeType;


/**
 *  Lexer GetToken() states.
 */
typedef enum
{
  LEX_CONTENT,
  LEX_GT,
  LEX_ENDTAG,
  LEX_STARTTAG,
  LEX_COMMENT,
  LEX_DOCTYPE,
  LEX_PROCINSTR,
  LEX_CDATA,
  LEX_SECTION,
  LEX_ASP,
  LEX_JSTE,
  LEX_PHP,
  LEX_XMLDECL
} LexerState;


/**
 *  ParseDocTypeDecl state constants.
 */
typedef enum
{
  DT_INTERMEDIATE,
  DT_DOCTYPENAME,
  DT_PUBLICSYSTEM,
  DT_QUOTEDSTRING,
  DT_INTSUBSET
} ParseDocTypeDeclState;


/**
 *  Content model shortcut encoding.
 *  Descriptions are tentative.
 */
#define CM_UNKNOWN      0
#define CM_EMPTY        (1 << 0)   /**< Elements with no content. Map to HTML specification. */
#define CM_HTML         (1 << 1)   /**< Elements that appear outside of "BODY". */
#define CM_HEAD         (1 << 2)   /**< Elements that can appear within HEAD. */
#define CM_BLOCK        (1 << 3)   /**< HTML "block" elements. */
#define CM_INLINE       (1 << 4)   /**< HTML "inline" elements. */
#define CM_LIST         (1 << 5)   /**< Elements that mark list item ("LI"). */
#define CM_DEFLIST      (1 << 6)   /**< Elements that mark definition list item ("DL", "DT"). */
#define CM_TABLE        (1 << 7)   /**< Elements that can appear inside TABLE. */
#define CM_ROWGRP       (1 << 8)   /**< Used for "THEAD", "TFOOT" or "TBODY". */
#define CM_ROW          (1 << 9)   /**< Used for "TD", "TH" */
#define CM_FIELD        (1 << 10)  /**< Elements whose content must be protected against white space movement. Includes some elements that can found in forms. */
#define CM_OBJECT       (1 << 11)  /**< Used to avoid propagating inline emphasis inside some elements such as OBJECT or APPLET. */
#define CM_PARAM        (1 << 12)  /**< Elements that allows "PARAM". */
#define CM_FRAMES       (1 << 13)  /**< "FRAME", "FRAMESET", "NOFRAMES". Used in ParseFrameSet. */
#define CM_HEADING      (1 << 14)  /**< Heading elements (h1, h2, ...). */
#define CM_OPT          (1 << 15)  /**< Elements with an optional end tag. */
#define CM_IMG          (1 << 16)  /**< Elements that use "align" attribute for vertical position. */
#define CM_MIXED        (1 << 17)  /**< Elements with inline and block model. Used to avoid calling InlineDup. */
#define CM_NO_INDENT    (1 << 18)  /**< Elements whose content needs to be indented only if containing one CM_BLOCK element. */
#define CM_OBSOLETE     (1 << 19)  /**< Elements that are obsolete (such as "dir", "menu"). */
#define CM_NEW          (1 << 20)  /**< User defined elements. Used to determine how attributes without value should be printed. */
#define CM_OMITST       (1 << 21)  /**< Elements that cannot be omitted. */
#define CM_VOID         (1 << 22)  /**< Elements that are void per https://www.w3.org/TR/2011/WD-html-markup-20110113/syntax.html#syntax-elements. */


/**
 *  If the document uses just HTML 2.0 tags and attributes described
 *  it is HTML 2.0. Similarly for HTML 3.2 and the 3 flavors of HTML 4.0.
 *  If there are proprietary tags and attributes then describe it as
 *  HTML Proprietary. If it includes the xml-lang or xmlns attributes
 *  but is otherwise HTML 2.0, 3.2 or 4.0 then describe it as one of the
 *  flavors of Voyager (strict, loose or frameset).
 */

/* unknown */
#define xxxx                   0u

/* W3C defined HTML/XHTML family document types */
#define HT20                   1u
#define HT32                   2u
#define H40S                   4u
#define H40T                   8u
#define H40F                  16u
#define H41S                  32u
#define H41T                  64u
#define H41F                 128u
#define X10S                 256u
#define X10T                 512u
#define X10F                1024u
#define XH11                2048u
#define XB10                4096u

/* proprietary stuff */
#define VERS_SUN            8192u
#define VERS_NETSCAPE      16384u
#define VERS_MICROSOFT     32768u

/* special flag */
#define VERS_XML           65536u

/* HTML5 */
#define HT50              131072u
#define XH50              262144u

/* compatibility symbols */
#define VERS_UNKNOWN       (xxxx)
#define VERS_HTML20        (HT20)
#define VERS_HTML32        (HT32)
#define VERS_HTML40_STRICT (H40S|H41S|X10S)
#define VERS_HTML40_LOOSE  (H40T|H41T|X10T)
#define VERS_FRAMESET      (H40F|H41F|X10F)
#define VERS_XHTML11       (XH11)
#define VERS_BASIC         (XB10)
/* HTML5 */
#define VERS_HTML5         (HT50|XH50)

/* meta symbols */
#define VERS_HTML40        (VERS_HTML40_STRICT|VERS_HTML40_LOOSE|VERS_FRAMESET)
#define VERS_IFRAME        (VERS_HTML40_LOOSE|VERS_FRAMESET)
#define VERS_LOOSE         (VERS_HTML20|VERS_HTML32|VERS_IFRAME)
#define VERS_EVENTS        (VERS_HTML40|VERS_XHTML11)
#define VERS_FROM32        (VERS_HTML32|VERS_HTML40|HT50)
#define VERS_FROM40        (VERS_HTML40|VERS_XHTML11|VERS_BASIC|VERS_HTML5)
#define VERS_XHTML         (X10S|X10T|X10F|XH11|XB10|XH50)

/* strict */
#define VERS_STRICT        (VERS_HTML5|VERS_HTML40_STRICT)

/* all W3C defined document types */
#define VERS_ALL           (VERS_HTML20|VERS_HTML32|VERS_FROM40|XH50|HT50)

/* all proprietary types */
#define VERS_PROPRIETARY   (VERS_NETSCAPE|VERS_MICROSOFT|VERS_SUN)


/**
 *  Linked list of class names and styles
 */
struct _Style;
typedef struct _Style TagStyle;

struct _Style
{
    tmbstr tag;
    tmbstr tag_class;
    tmbstr properties;
    TagStyle *next;
};


/**
 *  Linked list of style properties
 */
struct _StyleProp;
typedef struct _StyleProp StyleProp;

struct _StyleProp
{
    tmbstr name;
    tmbstr value;
    StyleProp *next;
};


/**
 *  Attribute/Value linked list node
 */
struct _AttVal
{
    AttVal*           next;
    const Attribute*  dict;
    Node*             asp;
    Node*             php;
    int               delim;
    tmbstr            attribute;
    tmbstr            value;
};


/**
 *  Mosaic handles inlines via a separate stack from other elements
 *  We duplicate this to recover from inline markup errors such as:
 *     ~~~
 *     <i>italic text
 *     <p>more italic text</b> normal text
 *     ~~~
 *  which for compatibility with Mosaic is mapped to:
 *     ~~~
 *     <i>italic text</i>
 *     <p><i>more italic text</i> normal text
 *     ~~~
 *  Note that any inline end tag pop's the effect of the current
 *  inline start tag, so that `</b>` pop's `<i>` in the above example.
*/
struct _IStack
{
    IStack*     next;
    const Dict* tag;        /**< tag's dictionary definition */
    tmbstr      element;    /**< name (NULL for text nodes) */
    AttVal*     attributes;
};


/**
 *  HTML/XHTML/XML Element, Comment, PI, DOCTYPE, XML Decl, etc., etc.
 */
struct _Node
{
    Node*       parent;         /**< tree structure */
    Node*       prev;
    Node*       next;
    Node*       content;
    Node*       last;

    AttVal*     attributes;
    const Dict* was;            /**< old tag when it was changed */
    const Dict* tag;            /**< tag's dictionary definition */

    tmbstr      element;        /**< name (NULL for text nodes) */

    uint        start;          /**< start of span onto text array */
    uint        end;            /**< end of span onto text array */
    NodeType    type;           /**< TextNode, StartTag, EndTag etc. */

    uint        line;           /**< current line of document */
    uint        column;         /**< current column of document */

    int         idx;            /**< general purpose register */

    Bool        closed;         /**< true if closed by explicit end tag */
    Bool        implicit;       /**< true if inferred */
    Bool        linebreak;      /**< true if followed by a line break */
};


/**
 *  The following are private to the lexer.
 *  Use `NewLexer()` to create a lexer, and `FreeLexer()` to free it.
 */
struct _Lexer
{
    uint lines;                /**< lines seen */
    uint columns;              /**< at start of current token */
    Bool waswhite;             /**< used to collapse contiguous white space */
    Bool pushed;               /**< true after token has been pushed back */
    Bool insertspace;          /**< when space is moved after end tag */
    Bool excludeBlocks;        /**< Netscape compatibility */
    Bool exiled;               /**< true if moved out of table */
    Bool isvoyager;            /**< true if xmlns attribute on html element (i.e., "Voyager" was the W3C codename for XHTML). */
    uint versions;             /**< bit vector of HTML versions */
    uint doctype;              /**< version as given by doctype (if any) */
    uint versionEmitted;       /**< version of doctype emitted */
    Bool bad_doctype;          /**< e.g. if html or PUBLIC is missing */
    uint txtstart;             /**< start of current node */
    uint txtend;               /**< end of current node */
    LexerState state;          /**< state of lexer's finite state machine */

    Node* token;               /**< last token returned by GetToken() */
    Node* itoken;              /**< last duplicate inline returned by GetToken() */
    Node* root;                /**< remember root node of the document */
    Node* parent;              /**< remember parent node for CDATA elements */

    Bool seenEndBody;          /**< true if a `</body>` tag has been encountered */
    Bool seenEndHtml;          /**< true if a `</html>` tag has been encountered */

    /*
      Lexer character buffer

      Parse tree nodes span onto this buffer
      which contains the concatenated text
      contents of all of the elements.

      lexsize must be reset for each file.
    */
    tmbstr lexbuf;             /**< MB character buffer */
    uint lexlength;            /**< allocated */
    uint lexsize;              /**< used */

    /* Inline stack for compatibility with Mosaic */
    Node* inode;               /**< for deferring text node */
    IStack* insert;            /**< for inferring inline tags */
    IStack* istack;
    uint istacklength;         /**< allocated */
    uint istacksize;           /**< used */
    uint istackbase;           /**< start of frame */

    TagStyle *styles;          /**< used for cleaning up presentation markup */

    TidyAllocator* allocator;  /**< allocator */
};


/**
 *  modes for GetToken()
 */
typedef enum
{
  IgnoreWhitespace,     /**< */
  MixedContent,         /**< for elements which don't accept PCDATA */
  Preformatted,         /**< white space preserved as is */
  IgnoreMarkup,         /**< for CDATA elements such as script, style */
  OtherNamespace,       /**< */
  CdataContent          /**< */
} GetTokenMode;


/** @name Lexer Functions
 *  @{
 */


/**
 *  Choose what version to use for new doctype
 */
int TY_(HTMLVersion)( TidyDocImpl* doc );


/**
 *  Everything is allowed in proprietary version of HTML.
 *  This is handled here rather than in the tag/attr dicts
 */
void TY_(ConstrainVersion)( TidyDocImpl* doc, uint vers );

Bool TY_(IsWhite)(uint c);
Bool TY_(IsDigit)(uint c);
Bool TY_(IsLetter)(uint c);
Bool TY_(IsHTMLSpace)(uint c);
Bool TY_(IsNewline)(uint c);
Bool TY_(IsNamechar)(uint c);
Bool TY_(IsXMLLetter)(uint c);
Bool TY_(IsXMLNamechar)(uint c);

Bool TY_(IsUpper)(uint c);
uint TY_(ToLower)(uint c);
uint TY_(ToUpper)(uint c);

Lexer* TY_(NewLexer)( TidyDocImpl* doc );
void TY_(FreeLexer)( TidyDocImpl* doc );


/**
 *  Store character c as UTF-8 encoded byte stream
 */
void TY_(AddCharToLexer)( Lexer *lexer, uint c );


/**
 *  Used for elements and text nodes.
 *   - Element name is NULL for text nodes.
 *   - start and end are offsets into lexbuf,
 *     which contains the textual content of
 *     all elements in the parse tree.
 *   - parent and content allow traversal
 *     of the parse tree in any direction.
 *   - attributes are represented as a linked
 *     list of AttVal nodes which hold the
 *     strings for attribute/value pairs.
*/
Node* TY_(NewNode)( TidyAllocator* allocator, Lexer* lexer );


/**
 *  Used to clone heading nodes when split by an `<HR>`
 */
Node* TY_(CloneNode)( TidyDocImpl* doc, Node *element );


/**
 *  Free node's attributes
 */
void TY_(FreeAttrs)( TidyDocImpl* doc, Node *node );


/**
 *  Doesn't repair attribute list linkage
 */
void TY_(FreeAttribute)( TidyDocImpl* doc, AttVal *av );


/**
 * Detach attribute from node
 */
void TY_(DetachAttribute)( Node *node, AttVal *attr );


/**
 *  Detach attribute from node then free it.
 */
void TY_(RemoveAttribute)( TidyDocImpl* doc, Node *node, AttVal *attr );


/**
 *  Free document nodes by iterating through peers and recursing
 *  through children. Set `next` to `NULL` before calling `FreeNode()`
 *  to avoid freeing peer nodes. Doesn't patch up prev/next links.
 */
void TY_(FreeNode)( TidyDocImpl* doc, Node *node );


Node* TY_(TextToken)( Lexer *lexer );


/**
 *  Used for creating preformatted text from Word2000.
 */
Node* TY_(NewLineNode)( Lexer *lexer );


/**
 *  Used for adding a &nbsp; for Word2000.
 */
Node* TY_(NewLiteralTextNode)(Lexer *lexer, ctmbstr txt );


void TY_(AddStringLiteral)( Lexer* lexer, ctmbstr str );
Node* TY_(FindDocType)( TidyDocImpl* doc );
Node* TY_(FindHTML)( TidyDocImpl* doc );
Node* TY_(FindHEAD)( TidyDocImpl* doc );
Node* TY_(FindTITLE)(TidyDocImpl* doc);
Node* TY_(FindBody)( TidyDocImpl* doc );
Node* TY_(FindXmlDecl)(TidyDocImpl* doc);


/**
 *  Returns containing block element, if any
 */
Node* TY_(FindContainer)( Node* node );


/**
 *  Add meta element for Tidy.
 */
Bool TY_(AddGenerator)( TidyDocImpl* doc );

uint TY_(ApparentVersion)( TidyDocImpl* doc );

ctmbstr TY_(HTMLVersionNameFromCode)( uint vers, Bool isXhtml );

uint TY_(HTMLVersionNumberFromCode)( uint vers );

Bool TY_(WarnMissingSIInEmittedDocType)( TidyDocImpl* doc );

Bool TY_(SetXHTMLDocType)( TidyDocImpl* doc );


/**
 *  Fixup doctype if missing.
 */
Bool TY_(FixDocType)( TidyDocImpl* doc );


/**
 *  Ensure XML document starts with <?xml version="1.0"?>,and
 *  add encoding attribute if not using ASCII or UTF-8 output.
 */
Bool TY_(FixXmlDecl)( TidyDocImpl* doc );


Node* TY_(InferredTag)(TidyDocImpl* doc, TidyTagId id);

void TY_(UngetToken)( TidyDocImpl* doc );

Node* TY_(GetToken)( TidyDocImpl* doc, GetTokenMode mode );

void TY_(InitMap)(void);


/**
 *  Create a new attribute.
 */
AttVal* TY_(NewAttribute)( TidyDocImpl* doc );


/**
 *  Create a new attribute with given name and value.
 */
AttVal* TY_(NewAttributeEx)( TidyDocImpl* doc, ctmbstr name, ctmbstr value,
                             int delim );


/**
 *  Insert attribute at the end of attribute list of a node.
 */
void TY_(InsertAttributeAtEnd)( Node *node, AttVal *av );

/**
 *  Insert attribute at the start of attribute list of a node.
 */
void TY_(InsertAttributeAtStart)( Node *node, AttVal *av );


/** @}
 *  @name Inline Stack Functions
 *  @{
 */


/**
 *  Duplicate attributes.
 */
AttVal* TY_(DupAttrs)( TidyDocImpl* doc, AttVal* attrs );


/**
 *  Push a copy of an inline node onto stack, but don't push if
 *  implicit or OBJECT or APPLET (implicit tags are ones generated
 *  from the istack).
 *
 *  One issue arises with pushing inlines when the tag is already pushed.
 *  For instance:
 *    ~~~
 *    <p><em>text
 *    <p><em>more text
 *    ~~~
 *  Shouldn't be mapped to
 *    ~~~
 *    <p><em>text</em></p>
 *    <p><em><em>more text</em></em>
 *    ~~~
 */
void TY_(PushInline)( TidyDocImpl* doc, Node* node );


/**
 * Pop inline stack.
 */
void TY_(PopInline)( TidyDocImpl* doc, Node* node );


Bool TY_(IsPushed)( TidyDocImpl* doc, Node* node );
Bool TY_(IsPushedLast)( TidyDocImpl* doc, Node *element, Node *node );


/**
 *  This has the effect of inserting "missing" inline elements around the
 *  contents of blocklevel elements such as P, TD, TH, DIV, PRE etc. This
 *  procedure is called at the start of `ParseBlock`, when the inline
 *  stack is not empty, as will be the case in:
 *    ~~~
 *    <i><h1>italic heading</h1></i>
 *    ~~~
 *  which is then treated as equivalent to
 *    ~~~
 *    <h1><i>italic heading</i></h1>
 *    ~~~
 *  This is implemented by setting the lexer into a mode where it gets
 *  tokens from the inline stack rather than from the input stream.
 */
int TY_(InlineDup)( TidyDocImpl* doc, Node *node );


/**
 *  Defer duplicates when entering a table or other
 *  element where the inlines shouldn't be duplicated.
 */
void TY_(DeferDup)( TidyDocImpl* doc );


Node* TY_(InsertedToken)( TidyDocImpl* doc );

/**
 *  Stack manipulation for inline elements
 */
Bool TY_(SwitchInline)( TidyDocImpl* doc, Node* element, Node* node );


Bool TY_(InlineDup1)( TidyDocImpl* doc, Node* node, Node* element );


/** @}
 *  @name Generic stack of nodes.
 *  @{
 */


/**
 * This typedef represents a stack of addresses to nodes. Tidy uses these to
 * try to limit recursion by pushing nodes to a stack when possible instead
 * of recursing.
 */
typedef struct _Stack {
    int top;                        /**< Current top position. */
    unsigned capacity;              /**< Current capacity. Can be expanded. */
    Node **firstNode;               /** A pointer to the first pointer to a Node in an array of node addresses. */
    TidyAllocator* allocator;       /**< Tidy's allocator, used at instantiation and expanding. */
} Stack;


/**
 * Create a new stack with a given starting capacity. If memory allocation
 * fails, then the allocator will panic the program automatically.
 */
Stack* TY_(newStack)(TidyDocImpl *doc, uint capacity);


/**
 *  Increase the stack size. This will be called automatically when the
 *  current stack is full. If memory allocation fails, then the allocator
 *  will panic the program automatically.
 */
void TY_(growStack)(Stack *stack);


/**
 * Stack is full when top is equal to the last index.
 */
Bool TY_(stackFull)(Stack *stack);


/**
 * Stack is empty when top is equal to -1
 */
Bool TY_(stackEmpty)(Stack *stack);


/**
 * Push an item to the stack.
 */
void TY_(push)(Stack *stack, Node *node);


/**
 * Pop an item from the stack.
 */
Node* TY_(pop)(Stack *stack);


/**
 * Peek at the stack.
 */
Node* TY_(peek)(Stack *stack);

/**
 *  Frees the stack when done.
 */
void TY_(freeStack)(Stack *stack);


/** @}
 */


#ifdef __cplusplus
}
#endif


/** @} end parser_h group */
/** @} end internal_api group */

#endif /* __LEXER_H__ */
