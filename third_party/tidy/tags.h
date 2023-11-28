#ifndef __TAGS_H__
#define __TAGS_H__

/**************************************************************************//**
 * @file
 * Recognize HTML Tags.
 *
 * The HTML tags are stored as 8 bit ASCII strings.
 * Use lookupw() to find a tag given a wide char string.
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
#include "third_party/tidy/lexer.h"
#include "third_party/tidy/config.h"
#include "third_party/tidy/attrdict.h"

/** @addtogroup internal_api */
/** @{ */


/***************************************************************************//**
 ** @defgroup tags_h HTML Tags
 **
 ** This module organizes all of Tidy's HTML tag operations, such as parsing
 ** tags, defining tags, and user-defined tags.
 **
 ** @{
 ******************************************************************************/


/** @name Basic Structures and Tag Operations.
 ** These structures form the backbone of Tidy tag processing, and the
 ** functions in this group provide basic operations with tags and nodes.
 */
/** @{ */


/** This enumeration defines the types of user-defined tags that can be
 ** created.
 */
typedef enum
{
    tagtype_null = 0,   /**< First item marker. */
    tagtype_empty = 1,  /**< Tag is an empty element. */
    tagtype_inline = 2, /**< Tag is an inline element. */
    tagtype_block = 4,  /**< Tag is a block level element. */
    tagtype_pre = 8     /**< Tag is a preformatted tag. */
} UserTagType;


/** This typedef describes a function to be used to parse HTML of a Tidy tag.
 ** @param doc The Tidy document.
 ** @param node The node being parsed.
 ** @param mode The GetTokenMode to be used for parsing the node contents.
 ** @param popStack A flag indicating that we are re-entering this parser, and
 **   it should restore a state from the stack.
 */
typedef Node* (Parser)( TidyDocImpl* doc, Node *node, GetTokenMode mode );


/** This typedef describes a function be be used to check the attributes
 ** of a Tidy tag.
 */
typedef void (CheckAttribs)( TidyDocImpl* doc, Node *node );


/** Defines a dictionary entry for a single Tidy tag, including all of the
 ** relevant information that it requires.
 */
struct _Dict
{
    TidyTagId           id;       /**< Identifier for this tag. */
    tmbstr              name;     /**< The tag name. */
    uint                versions; /**< Accumulates potential HTML versions. See TY_(ConstrainVersion). */
    AttrVersion const * attrvers; /**< Accumulates potential HTML versions for attributes. */
    uint                model;    /**< Indicates the relevant content models for the tag. See lexer.h; there is no enum. */
    Parser*             parser;   /**< Specifies the parser to use for this tag. */
    CheckAttribs*       chkattrs; /**< Specifies the function to check this tag's attributes. */
    Dict*               next;     /**< Link to next tag. */
};


/** This enum indicates the maximum size of the has table for tag hash lookup.
 */
enum
{
    ELEMENT_HASH_SIZE=178u  /**< Maximum number of tags in the hash table. */
};


/** This structure provide hash lookup for Tidy tags.
 */
typedef struct _DictHash
{
    Dict const*         tag;   /**< The current tag. */
    struct _DictHash*   next;  /**< The next tag. */
} DictHash;


/** This structure consists of the lists of all tags known to Tidy.
 */
typedef struct _TidyTagImpl
{
    Dict* xml_tags;                        /**< Placeholder for all xml tags. */
    Dict* declared_tag_list;               /**< User-declared tags. */
    DictHash* hashtab[ELEMENT_HASH_SIZE];  /**< All of Tidy's built-in tags. */
} TidyTagImpl;


/** Coordinates Config update and Tags data.
 ** @param doc The Tidy document.
 ** @param opt The option the tag is intended for.
 ** @param name The name of the new tag.
 */
void TY_(DeclareUserTag)( TidyDocImpl* doc, const TidyOptionImpl* opt, ctmbstr name );


/** Interface for finding a tag by TidyTagId.
 ** @param tid The TidyTagId to search for.
 ** @returns An instance of a Tidy tag.
 */
const Dict* TY_(LookupTagDef)( TidyTagId tid );

/** Assigns the node's tag.
 ** @param doc The Tidy document.
 ** @param node The node to assign the tag to.
 ** @returns Returns a bool indicating whether or not the tag was assigned.
 */
Bool    TY_(FindTag)( TidyDocImpl* doc, Node *node );


/** Finds the parser function for a given node.
 ** @param doc The Tidy document.
 ** @param node The node to lookup.
 ** @returns The parser for the given node.
 */
Parser* TY_(FindParser)( TidyDocImpl* doc, Node *node );


/** Defines a new user-defined tag.
 ** @param doc The Tidy document.
 ** @param tagType The type of user-defined tag to define.
 ** @param name The name of the new tag.
 */
void    TY_(DefineTag)( TidyDocImpl* doc, UserTagType tagType, ctmbstr name );


/** Frees user-defined tags of the given type, or all user tags in given
 ** `tagtype_null`.
 ** @param doc The Tidy document.
 ** @param tagType The type of tag to free, or `tagtype_null` to free all
 **        user-defined tags.
 */
void    TY_(FreeDeclaredTags)( TidyDocImpl* doc, UserTagType tagType );


/** Initiates an iterator for a list of user-declared tags, including autonomous
 ** custom tags detected in the document if @ref TidyUseCustomTags is not set to
 ** **no**.
 ** @param doc An instance of a TidyDocImp to query.
 ** @result Returns a TidyIterator, which is a token used to represent the
 **         current position in a list within LibTidy.
 */
TidyIterator   TY_(GetDeclaredTagList)( TidyDocImpl* doc );


/** Given a valid TidyIterator initiated with TY_(GetDeclaredTagList)(),
 ** returns a string representing a user-declared or autonomous custom tag.
 ** @remark Specifying tagType limits the scope of the tags to one of
 **         @ref UserTagType types. Note that autonomous custom tags (if used)
 **         are added to one of these option types, depending on the value of
 **         @ref TidyUseCustomTags.
 ** @param doc The Tidy document.
 ** @param tagType The type of tag to iterate through.
 ** @param iter The iterator token provided initially by
 **        TY_(GetDeclaredTagList)().
 ** @result A string containing the next tag.
 */
ctmbstr        TY_(GetNextDeclaredTag)( TidyDocImpl* doc, UserTagType tagType,
                                        TidyIterator* iter );


/** Initializes tags and tag structures for the given Tidy document.
 ** @param doc The Tidy document.
 */
void TY_(InitTags)( TidyDocImpl* doc );


/** Frees the tags and structures used by Tidy for tags.
 ** @param doc The Tidy document.
 */
void TY_(FreeTags)( TidyDocImpl* doc );


/** Tidy defaults to HTML5 mode. If the <!DOCTYPE ...> is found to NOT be
 ** HTML5, then adjust the tags table to HTML4 mode.
 ** @param doc The Tidy document.
 */
void TY_(AdjustTags)( TidyDocImpl *doc );


/** Reset the tags table back to default HTML5 mode.
 ** @param doc The Tidy document.
 */
void TY_(ResetTags)( TidyDocImpl *doc );


/** Indicates whether or not the Tidy is processing in HTML5 mode.
 ** @param doc The Tidy document.
 ** @returns Returns `yes` if processing in HTML5 mode.
 */
Bool TY_(IsHTML5Mode)( TidyDocImpl *doc );


/** @} */
/** @name Parser Methods And Attribute Checker Functions for Tags
 ** These functions define the parsers and attribute checking functions for
 ** each of Tidy's tags.
 */
/** @{ */


Parser TY_(ParseHTML);
Parser TY_(ParseHead);
Parser TY_(ParseTitle);
Parser TY_(ParseScript);
Parser TY_(ParseFrameSet);
Parser TY_(ParseNoFrames);
Parser TY_(ParseBody);
Parser TY_(ParsePre);
Parser TY_(ParseList);
Parser TY_(ParseDefList);
Parser TY_(ParseBlock);
Parser TY_(ParseInline);
Parser TY_(ParseEmpty);
Parser TY_(ParseTableTag);
Parser TY_(ParseColGroup);
Parser TY_(ParseRowGroup);
Parser TY_(ParseRow);
Parser TY_(ParseSelect);
Parser TY_(ParseOptGroup);
Parser TY_(ParseText);
Parser TY_(ParseDatalist);
Parser TY_(ParseNamespace);

CheckAttribs TY_(CheckAttributes);


/** @} */
/** @name Other Tag and Node Lookup Functions
 ** These functions perform additional lookup on tags and nodes.
 */
/** @{ */


/** Gets the TidyTagId of the given node. 0 == TidyTag_UNKNOWN.
 */
#define TagId(node)        ((node) && (node)->tag ? (node)->tag->id : TidyTag_UNKNOWN)


/** Determines if the given node is of the given tag id type.
 */
#define TagIsId(node, tid) ((node) && (node)->tag && (node)->tag->id == tid)


/** Inquires whether or not the given node is a text node.
 ** @param node The node being interrogated.
 ** @returns The status of the inquiry.
 */
Bool TY_(nodeIsText)( Node* node );


/** Inquires whether or not the given node is an element node.
 ** @param node The node being interrogated.
 ** @returns The status of the inquiry.
 */
Bool TY_(nodeIsElement)( Node* node );


/** Inquires whether or not the given node has any text.
 ** @param doc The Tidy document.
 ** @param node The node being interrogated.
 ** @returns The status of the inquiry.
 */
Bool TY_(nodeHasText)( TidyDocImpl* doc, Node* node );


/** Inquires whether the given element looks like it's an autonomous custom
 ** element tag.
 ** @param element A string to be checked.
 ** @returns The status of the inquiry.
 */
Bool TY_(elementIsAutonomousCustomFormat)( ctmbstr element );


/** Inquires whether the given node looks like it's an autonomous custom
 ** element tag.
 ** @param node The node being interrogated.
 ** @returns The status of the inquiry.
 */
Bool TY_(nodeIsAutonomousCustomFormat)( Node* node );


/** True if the node looks like it's an autonomous custom element tag, and
 ** TidyCustomTags is not disabled, and we're in HTML5 mode, which are all
 ** requirements for valid autonomous custom tags.
 ** @param doc The Tidy document.
 ** @param node The node being interrogated.
 ** @returns The status of the inquiry.
 */
Bool TY_(nodeIsAutonomousCustomTag)( TidyDocImpl* doc, Node* node );


/** Does the node have the indicated content model? True if any of the bits
 ** requested are set.
 ** @param node The node being interrogated.
 ** @param contentModel The content model to check against.
 ** @returns The status of the inquiry.
 */
Bool TY_(nodeHasCM)( Node* node, uint contentModel );


/** Does the content model of the node include block?
 ** @param node The node being interrogated.
 ** @returns The status of the inquiry.
 */
Bool TY_(nodeCMIsBlock)( Node* node );


/** Does the content model of the node include inline?
 ** @param node The node being interrogated.
 ** @returns The status of the inquiry.
 */
Bool TY_(nodeCMIsInline)( Node* node );


/** Does the content model of the node include empty?
 ** @param node The node being interrogated.
 ** @returns The status of the inquiry.
 */
Bool TY_(nodeCMIsEmpty)( Node* node );


/** Is the node a header, such as H1, H2, ..., H6?
 ** @param node The node being interrogated.
 ** @returns The status of the inquiry.
 */
Bool TY_(nodeIsHeader)( Node* node );


/** Inquires as to the header level of the given node: 1, 2, ..., 6.
 ** @param node The node being interrogated.
 ** @returns The header level.
 */
uint TY_(nodeHeaderLevel)( Node* node );


#define nodeIsHTML( node )       TagIsId( node, TidyTag_HTML )
#define nodeIsHEAD( node )       TagIsId( node, TidyTag_HEAD )
#define nodeIsTITLE( node )      TagIsId( node, TidyTag_TITLE )
#define nodeIsBASE( node )       TagIsId( node, TidyTag_BASE )
#define nodeIsMETA( node )       TagIsId( node, TidyTag_META )
#define nodeIsBODY( node )       TagIsId( node, TidyTag_BODY )
#define nodeIsFRAMESET( node )   TagIsId( node, TidyTag_FRAMESET )
#define nodeIsFRAME( node )      TagIsId( node, TidyTag_FRAME )
#define nodeIsIFRAME( node )     TagIsId( node, TidyTag_IFRAME )
#define nodeIsNOFRAMES( node )   TagIsId( node, TidyTag_NOFRAMES )
#define nodeIsHR( node )         TagIsId( node, TidyTag_HR )
#define nodeIsH1( node )         TagIsId( node, TidyTag_H1 )
#define nodeIsH2( node )         TagIsId( node, TidyTag_H2 )
#define nodeIsPRE( node )        TagIsId( node, TidyTag_PRE )
#define nodeIsLISTING( node )    TagIsId( node, TidyTag_LISTING )
#define nodeIsP( node )          TagIsId( node, TidyTag_P )
#define nodeIsUL( node )         TagIsId( node, TidyTag_UL )
#define nodeIsOL( node )         TagIsId( node, TidyTag_OL )
#define nodeIsDL( node )         TagIsId( node, TidyTag_DL )
#define nodeIsDIR( node )        TagIsId( node, TidyTag_DIR )
#define nodeIsLI( node )         TagIsId( node, TidyTag_LI )
#define nodeIsDT( node )         TagIsId( node, TidyTag_DT )
#define nodeIsDD( node )         TagIsId( node, TidyTag_DD )
#define nodeIsTABLE( node )      TagIsId( node, TidyTag_TABLE )
#define nodeIsCAPTION( node )    TagIsId( node, TidyTag_CAPTION )
#define nodeIsTD( node )         TagIsId( node, TidyTag_TD )
#define nodeIsTH( node )         TagIsId( node, TidyTag_TH )
#define nodeIsTR( node )         TagIsId( node, TidyTag_TR )
#define nodeIsCOL( node )        TagIsId( node, TidyTag_COL )
#define nodeIsCOLGROUP( node )   TagIsId( node, TidyTag_COLGROUP )
#define nodeIsBR( node )         TagIsId( node, TidyTag_BR )
#define nodeIsA( node )          TagIsId( node, TidyTag_A )
#define nodeIsLINK( node )       TagIsId( node, TidyTag_LINK )
#define nodeIsB( node )          TagIsId( node, TidyTag_B )
#define nodeIsI( node )          TagIsId( node, TidyTag_I )
#define nodeIsSTRONG( node )     TagIsId( node, TidyTag_STRONG )
#define nodeIsEM( node )         TagIsId( node, TidyTag_EM )
#define nodeIsBIG( node )        TagIsId( node, TidyTag_BIG )
#define nodeIsSMALL( node )      TagIsId( node, TidyTag_SMALL )
#define nodeIsPARAM( node )      TagIsId( node, TidyTag_PARAM )
#define nodeIsOPTION( node )     TagIsId( node, TidyTag_OPTION )
#define nodeIsOPTGROUP( node )   TagIsId( node, TidyTag_OPTGROUP )
#define nodeIsIMG( node )        TagIsId( node, TidyTag_IMG )
#define nodeIsMAP( node )        TagIsId( node, TidyTag_MAP )
#define nodeIsAREA( node )       TagIsId( node, TidyTag_AREA )
#define nodeIsNOBR( node )       TagIsId( node, TidyTag_NOBR )
#define nodeIsWBR( node )        TagIsId( node, TidyTag_WBR )
#define nodeIsFONT( node )       TagIsId( node, TidyTag_FONT )
#define nodeIsLAYER( node )      TagIsId( node, TidyTag_LAYER )
#define nodeIsSPACER( node )     TagIsId( node, TidyTag_SPACER )
#define nodeIsCENTER( node )     TagIsId( node, TidyTag_CENTER )
#define nodeIsSTYLE( node )      TagIsId( node, TidyTag_STYLE )
#define nodeIsSCRIPT( node )     TagIsId( node, TidyTag_SCRIPT )
#define nodeIsNOSCRIPT( node )   TagIsId( node, TidyTag_NOSCRIPT )
#define nodeIsFORM( node )       TagIsId( node, TidyTag_FORM )
#define nodeIsTEXTAREA( node )   TagIsId( node, TidyTag_TEXTAREA )
#define nodeIsBLOCKQUOTE( node ) TagIsId( node, TidyTag_BLOCKQUOTE )
#define nodeIsAPPLET( node )     TagIsId( node, TidyTag_APPLET )
#define nodeIsOBJECT( node )     TagIsId( node, TidyTag_OBJECT )
#define nodeIsDIV( node )        TagIsId( node, TidyTag_DIV )
#define nodeIsSPAN( node )       TagIsId( node, TidyTag_SPAN )
#define nodeIsINPUT( node )      TagIsId( node, TidyTag_INPUT )
#define nodeIsQ( node )          TagIsId( node, TidyTag_Q )
#define nodeIsLABEL( node )      TagIsId( node, TidyTag_LABEL )
#define nodeIsH3( node )         TagIsId( node, TidyTag_H3 )
#define nodeIsH4( node )         TagIsId( node, TidyTag_H4 )
#define nodeIsH5( node )         TagIsId( node, TidyTag_H5 )
#define nodeIsH6( node )         TagIsId( node, TidyTag_H6 )
#define nodeIsADDRESS( node )    TagIsId( node, TidyTag_ADDRESS )
#define nodeIsXMP( node )        TagIsId( node, TidyTag_XMP )
#define nodeIsSELECT( node )     TagIsId( node, TidyTag_SELECT )
#define nodeIsBLINK( node )      TagIsId( node, TidyTag_BLINK )
#define nodeIsMARQUEE( node )    TagIsId( node, TidyTag_MARQUEE )
#define nodeIsEMBED( node )      TagIsId( node, TidyTag_EMBED )
#define nodeIsBASEFONT( node )   TagIsId( node, TidyTag_BASEFONT )
#define nodeIsISINDEX( node )    TagIsId( node, TidyTag_ISINDEX )
#define nodeIsS( node )          TagIsId( node, TidyTag_S )
#define nodeIsSTRIKE( node )     TagIsId( node, TidyTag_STRIKE )
#define nodeIsSUB( node )        TagIsId( node, TidyTag_SUB )
#define nodeIsSUP( node )        TagIsId( node, TidyTag_SUP )
#define nodeIsU( node )          TagIsId( node, TidyTag_U )
#define nodeIsMENU( node )       TagIsId( node, TidyTag_MENU )
#define nodeIsMAIN( node )       TagIsId( node, TidyTag_MAIN )
#define nodeIsBUTTON( node )     TagIsId( node, TidyTag_BUTTON )
#define nodeIsCANVAS( node )     TagIsId( node, TidyTag_CANVAS )
#define nodeIsPROGRESS( node )   TagIsId( node, TidyTag_PROGRESS )

#define nodeIsINS( node )        TagIsId( node, TidyTag_INS )
#define nodeIsDEL( node )        TagIsId( node, TidyTag_DEL )

#define nodeIsSVG( node )        TagIsId( node, TidyTag_SVG )

/* HTML5 */
#define nodeIsDATALIST( node )   TagIsId( node, TidyTag_DATALIST )
#define nodeIsDATA( node )       TagIsId( node, TidyTag_DATA )
#define nodeIsMATHML( node )     TagIsId( node, TidyTag_MATHML ) /* #130 MathML attr and entity fix! */
#define nodeIsTEMPLATE( node )   TagIsId( node, TidyTag_TEMPLATE )

/* NOT in HTML 5 */
#define nodeIsACRONYM( node )    TagIsId( node, TidyTag_ACRONYM )
#define nodesIsFRAME( node )     TagIsId( node, TidyTag_FRAME )
#define nodeIsTT( node )         TagIsId( node, TidyTag_TT )


/** @} name */
/** @} tags_h group */
/** @} internal_api addtogroup */


#endif /* __TAGS_H__ */
