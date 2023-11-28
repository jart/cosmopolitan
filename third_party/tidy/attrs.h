#ifndef __ATTRS_H__
#define __ATTRS_H__

/* attrs.h -- recognize HTML attributes

  (c) 1998-2007 (W3C) MIT, ERCIM, Keio University
  See tidy.h for the copyright notice.

*/

#include "third_party/tidy/forward.h"

/* declaration for methods that check attribute values */
typedef void (AttrCheck)(TidyDocImpl* doc, Node *node, AttVal *attval);

struct _Attribute
{
    TidyAttrId  id;
    tmbstr      name;
    AttrCheck*  attrchk;

    struct _Attribute* next;
};


/*
 Anchor/Node linked list
*/

struct _Anchor
{
    struct _Anchor *next;
    Node *node;
    char *name;
};

typedef struct _Anchor Anchor;

enum
{
    ATTRIBUTE_HASH_SIZE=178u
};

struct _AttrHash
{
    Attribute const*  attr;
    struct _AttrHash* next;
};

typedef struct _AttrHash AttrHash;

enum
{
    ANCHOR_HASH_SIZE=1021u
};

/* Keeps a list of attributes that are sorted ahead of the others. */
typedef struct _priorityAttribs {
    tmbstr* list;
    uint count;
    uint capacity;
} PriorityAttribs;

struct _TidyAttribImpl
{
    /* anchor/node lookup */
    Anchor*    anchor_hash[ANCHOR_HASH_SIZE];

    /* Declared literal attributes */
    Attribute* declared_attr_list;

    /* Prioritized list of attributes to write */
    PriorityAttribs priorityAttribs;

    AttrHash*  hashtab[ATTRIBUTE_HASH_SIZE];
};

typedef struct _TidyAttribImpl TidyAttribImpl;

#define XHTML_NAMESPACE "http://www.w3.org/1999/xhtml"

AttrCheck TY_(CheckUrl);

/* public method for finding attribute definition by name */
const Attribute* TY_(CheckAttribute)( TidyDocImpl* doc, Node *node, AttVal *attval );

const Attribute* TY_(FindAttribute)( TidyDocImpl* doc, AttVal *attval );

AttVal* TY_(GetAttrByName)( Node *node, ctmbstr name );

void TY_(DropAttrByName)( TidyDocImpl* doc, Node *node, ctmbstr name );

AttVal* TY_(AddAttribute)( TidyDocImpl* doc,
                           Node *node, ctmbstr name, ctmbstr value );

AttVal* TY_(RepairAttrValue)(TidyDocImpl* doc, Node* node, ctmbstr name, ctmbstr value);

/* Add an item to the list of priority attributes to write first. */
void TY_(DefinePriorityAttribute)(TidyDocImpl* doc, ctmbstr name);

/* Start an iterator for priority attributes. */
TidyIterator TY_(getPriorityAttrList)( TidyDocImpl* doc );

/* Get the next priority attribute. */
ctmbstr TY_(getNextPriorityAttr)( TidyDocImpl* doc, TidyIterator* iter );

Bool TY_(IsUrl)( TidyDocImpl* doc, ctmbstr attrname );

/* Bool IsBool( TidyDocImpl* doc, ctmbstr attrname ); */

Bool TY_(IsScript)( TidyDocImpl* doc, ctmbstr attrname );

/* may id or name serve as anchor? */
Bool TY_(IsAnchorElement)( TidyDocImpl* doc, Node* node );

/*
  In CSS1, selectors can contain only the characters A-Z, 0-9, and
  Unicode characters 161-255, plus dash (-); they cannot start with
  a dash or a digit; they can also contain escaped characters and any
  Unicode character as a numeric code (see next item).

  The backslash followed by at most four hexadecimal digits (0..9A..F)
  stands for the Unicode character with that number.

  Any character except a hexadecimal digit can be escaped to remove its
  special meaning, by putting a backslash in front.

  #508936 - CSS class naming for -clean option
*/
Bool TY_(IsCSS1Selector)( ctmbstr buf );

Bool TY_(IsValidHTMLID)(ctmbstr id);
Bool TY_(IsValidXMLID)(ctmbstr id);

/* removes anchor for specific node */
void TY_(RemoveAnchorByNode)( TidyDocImpl* doc, ctmbstr name, Node *node );

/* free all anchors */
void TY_(FreeAnchors)( TidyDocImpl* doc );


/* public methods for inititializing/freeing attribute dictionary */
void TY_(InitAttrs)( TidyDocImpl* doc );
void TY_(FreeAttrTable)( TidyDocImpl* doc );

void TY_(FreeAttrPriorityList)( TidyDocImpl* doc );

void TY_(AppendToClassAttr)( TidyDocImpl* doc, AttVal *classattr, ctmbstr classname );
/*
 the same attribute name can't be used
 more than once in each element
*/
void TY_(RepairDuplicateAttributes)( TidyDocImpl* doc, Node* node, Bool isXml );
void TY_(SortAttributes)(TidyDocImpl* doc, Node* node, TidyAttrSortStrategy strat);

Bool TY_(IsBoolAttribute)( AttVal* attval );
Bool TY_(attrIsEvent)( AttVal* attval );

AttVal* TY_(AttrGetById)( Node* node, TidyAttrId id );

uint TY_(NodeAttributeVersions)( Node* node, TidyAttrId id );

Bool TY_(AttributeIsProprietary)(Node* node, AttVal* attval);
Bool TY_(AttributeIsMismatched)(Node* node, AttVal* attval, TidyDocImpl* doc);


/* 0 == TidyAttr_UNKNOWN  */
#define AttrId(av) ((av) && (av)->dict ? (av)->dict->id : TidyAttr_UNKNOWN)
#define AttrIsId(av, atid) ((av) && (av)->dict && ((av)->dict->id == atid))

#define AttrHasValue(attr)      ((attr) && (attr)->value)
#define AttrValueIs(attr, val)  (AttrHasValue(attr) && \
                                 TY_(tmbstrcasecmp)((attr)->value, val) == 0)
#define AttrContains(attr, val) (AttrHasValue(attr) && \
                                 TY_(tmbsubstr)((attr)->value, val) != NULL)
#define AttrVersions(attr)      ((attr) && (attr)->dict ? (attr)->dict->versions : VERS_PROPRIETARY)

#define AttrsHaveSameId(a, b) (a && b && a->dict && b->dict && a->dict->id && \
                               b->dict->id && a->dict->id == b->dict->id)

#define attrIsABBR(av)              AttrIsId( av, TidyAttr_ABBR  )
#define attrIsACCEPT(av)            AttrIsId( av, TidyAttr_ACCEPT  )
#define attrIsACCEPT_CHARSET(av)    AttrIsId( av, TidyAttr_ACCEPT_CHARSET  )
#define attrIsACCESSKEY(av)         AttrIsId( av, TidyAttr_ACCESSKEY  )
#define attrIsACTION(av)            AttrIsId( av, TidyAttr_ACTION  )
#define attrIsADD_DATE(av)          AttrIsId( av, TidyAttr_ADD_DATE  )
#define attrIsALIGN(av)             AttrIsId( av, TidyAttr_ALIGN  )
#define attrIsALINK(av)             AttrIsId( av, TidyAttr_ALINK  )
#define attrIsALT(av)               AttrIsId( av, TidyAttr_ALT  )
#define attrIsARCHIVE(av)           AttrIsId( av, TidyAttr_ARCHIVE  )
#define attrIsAXIS(av)              AttrIsId( av, TidyAttr_AXIS  )
#define attrIsBACKGROUND(av)        AttrIsId( av, TidyAttr_BACKGROUND  )
#define attrIsBGCOLOR(av)           AttrIsId( av, TidyAttr_BGCOLOR  )
#define attrIsBGPROPERTIES(av)      AttrIsId( av, TidyAttr_BGPROPERTIES  )
#define attrIsBORDER(av)            AttrIsId( av, TidyAttr_BORDER  )
#define attrIsBORDERCOLOR(av)       AttrIsId( av, TidyAttr_BORDERCOLOR  )
#define attrIsBOTTOMMARGIN(av)      AttrIsId( av, TidyAttr_BOTTOMMARGIN  )
#define attrIsCELLPADDING(av)       AttrIsId( av, TidyAttr_CELLPADDING  )
#define attrIsCELLSPACING(av)       AttrIsId( av, TidyAttr_CELLSPACING  )
#define attrIsCHARSET(av)           AttrIsId( av, TidyAttr_CHARSET  )
#define attrIsCHAR(av)              AttrIsId( av, TidyAttr_CHAR  )
#define attrIsCHAROFF(av)           AttrIsId( av, TidyAttr_CHAROFF  )
#define attrIsCHARSET(av)           AttrIsId( av, TidyAttr_CHARSET  )
#define attrIsCHECKED(av)           AttrIsId( av, TidyAttr_CHECKED  )
#define attrIsCITE(av)              AttrIsId( av, TidyAttr_CITE  )
#define attrIsCLASS(av)             AttrIsId( av, TidyAttr_CLASS  )
#define attrIsCLASSID(av)           AttrIsId( av, TidyAttr_CLASSID  )
#define attrIsCLEAR(av)             AttrIsId( av, TidyAttr_CLEAR  )
#define attrIsCODE(av)              AttrIsId( av, TidyAttr_CODE  )
#define attrIsCODEBASE(av)          AttrIsId( av, TidyAttr_CODEBASE  )
#define attrIsCODETYPE(av)          AttrIsId( av, TidyAttr_CODETYPE  )
#define attrIsCOLOR(av)             AttrIsId( av, TidyAttr_COLOR  )
#define attrIsCOLS(av)              AttrIsId( av, TidyAttr_COLS  )
#define attrIsCOLSPAN(av)           AttrIsId( av, TidyAttr_COLSPAN  )
#define attrIsCOMPACT(av)           AttrIsId( av, TidyAttr_COMPACT  )
#define attrIsCONTENT(av)           AttrIsId( av, TidyAttr_CONTENT  )
#define attrIsCOORDS(av)            AttrIsId( av, TidyAttr_COORDS  )
#define attrIsDATA(av)              AttrIsId( av, TidyAttr_DATA  )
#define attrIsDATAFLD(av)           AttrIsId( av, TidyAttr_DATAFLD  )
#define attrIsDATAFORMATAS(av)      AttrIsId( av, TidyAttr_DATAFORMATAS  )
#define attrIsDATAPAGESIZE(av)      AttrIsId( av, TidyAttr_DATAPAGESIZE  )
#define attrIsDATASRC(av)           AttrIsId( av, TidyAttr_DATASRC  )
#define attrIsDATETIME(av)          AttrIsId( av, TidyAttr_DATETIME  )
#define attrIsDECLARE(av)           AttrIsId( av, TidyAttr_DECLARE  )
#define attrIsDEFER(av)             AttrIsId( av, TidyAttr_DEFER  )
#define attrIsDIR(av)               AttrIsId( av, TidyAttr_DIR  )
#define attrIsDISABLED(av)          AttrIsId( av, TidyAttr_DISABLED  )
#define attrIsENCODING(av)          AttrIsId( av, TidyAttr_ENCODING  )
#define attrIsENCTYPE(av)           AttrIsId( av, TidyAttr_ENCTYPE  )
#define attrIsFACE(av)              AttrIsId( av, TidyAttr_FACE  )
#define attrIsFOR(av)               AttrIsId( av, TidyAttr_FOR  )
#define attrIsFRAME(av)             AttrIsId( av, TidyAttr_FRAME  )
#define attrIsFRAMEBORDER(av)       AttrIsId( av, TidyAttr_FRAMEBORDER  )
#define attrIsFRAMESPACING(av)      AttrIsId( av, TidyAttr_FRAMESPACING  )
#define attrIsGRIDX(av)             AttrIsId( av, TidyAttr_GRIDX  )
#define attrIsGRIDY(av)             AttrIsId( av, TidyAttr_GRIDY  )
#define attrIsHEADERS(av)           AttrIsId( av, TidyAttr_HEADERS  )
#define attrIsHEIGHT(av)            AttrIsId( av, TidyAttr_HEIGHT  )
#define attrIsHREF(av)              AttrIsId( av, TidyAttr_HREF  )
#define attrIsHREFLANG(av)          AttrIsId( av, TidyAttr_HREFLANG  )
#define attrIsHSPACE(av)            AttrIsId( av, TidyAttr_HSPACE  )
#define attrIsHTTP_EQUIV(av)        AttrIsId( av, TidyAttr_HTTP_EQUIV  )
#define attrIsID(av)                AttrIsId( av, TidyAttr_ID  )
#define attrIsISMAP(av)             AttrIsId( av, TidyAttr_ISMAP  )
#define attrIsITEMID(av)            AttrIsId( av, TidyAttr_ITEMID  )
#define attrIsITEMPROP(av)          AttrIsId( av, TidyAttr_ITEMPROP  )
#define attrIsITEMREF(av)           AttrIsId( av, TidyAttr_ITEMREF  )
#define attrIsITEMSCOPE(av)         AttrIsId( av, TidyAttr_ITEMSCOPE  )
#define attrIsITEMTYPE(av)          AttrIsId( av, TidyAttr_ITEMTYPE  )
#define attrIsLABEL(av)             AttrIsId( av, TidyAttr_LABEL  )
#define attrIsLANG(av)              AttrIsId( av, TidyAttr_LANG  )
#define attrIsLANGUAGE(av)          AttrIsId( av, TidyAttr_LANGUAGE  )
#define attrIsLAST_MODIFIED(av)     AttrIsId( av, TidyAttr_LAST_MODIFIED  )
#define attrIsLAST_VISIT(av)        AttrIsId( av, TidyAttr_LAST_VISIT  )
#define attrIsLEFTMARGIN(av)        AttrIsId( av, TidyAttr_LEFTMARGIN  )
#define attrIsLINK(av)              AttrIsId( av, TidyAttr_LINK  )
#define attrIsLONGDESC(av)          AttrIsId( av, TidyAttr_LONGDESC  )
#define attrIsLOWSRC(av)            AttrIsId( av, TidyAttr_LOWSRC  )
#define attrIsMARGINHEIGHT(av)      AttrIsId( av, TidyAttr_MARGINHEIGHT  )
#define attrIsMARGINWIDTH(av)       AttrIsId( av, TidyAttr_MARGINWIDTH  )
#define attrIsMAXLENGTH(av)         AttrIsId( av, TidyAttr_MAXLENGTH  )
#define attrIsMEDIA(av)             AttrIsId( av, TidyAttr_MEDIA  )
#define attrIsMETHOD(av)            AttrIsId( av, TidyAttr_METHOD  )
#define attrIsMULTIPLE(av)          AttrIsId( av, TidyAttr_MULTIPLE  )
#define attrIsNAME(av)              AttrIsId( av, TidyAttr_NAME  )
#define attrIsNOHREF(av)            AttrIsId( av, TidyAttr_NOHREF  )
#define attrIsNORESIZE(av)          AttrIsId( av, TidyAttr_NORESIZE  )
#define attrIsNOSHADE(av)           AttrIsId( av, TidyAttr_NOSHADE  )
#define attrIsNOWRAP(av)            AttrIsId( av, TidyAttr_NOWRAP  )
#define attrIsOBJECT(av)            AttrIsId( av, TidyAttr_OBJECT  )
#define attrIsOnAFTERUPDATE(av)     AttrIsId( av, TidyAttr_OnAFTERUPDATE  )
#define attrIsOnBEFOREUNLOAD(av)    AttrIsId( av, TidyAttr_OnBEFOREUNLOAD  )
#define attrIsOnBEFOREUPDATE(av)    AttrIsId( av, TidyAttr_OnBEFOREUPDATE  )
#define attrIsOnBLUR(av)            AttrIsId( av, TidyAttr_OnBLUR  )
#define attrIsOnCHANGE(av)          AttrIsId( av, TidyAttr_OnCHANGE  )
#define attrIsOnCLICK(av)           AttrIsId( av, TidyAttr_OnCLICK  )
#define attrIsOnDATAAVAILABLE(av)   AttrIsId( av, TidyAttr_OnDATAAVAILABLE  )
#define attrIsOnDATASETCHANGED(av)  AttrIsId( av, TidyAttr_OnDATASETCHANGED  )
#define attrIsOnDATASETCOMPLETE(av) AttrIsId( av, TidyAttr_OnDATASETCOMPLETE  )
#define attrIsOnDBLCLICK(av)        AttrIsId( av, TidyAttr_OnDBLCLICK  )
#define attrIsOnERRORUPDATE(av)     AttrIsId( av, TidyAttr_OnERRORUPDATE  )
#define attrIsOnFOCUS(av)           AttrIsId( av, TidyAttr_OnFOCUS  )
#define attrIsOnKEYDOWN(av)         AttrIsId( av, TidyAttr_OnKEYDOWN  )
#define attrIsOnKEYPRESS(av)        AttrIsId( av, TidyAttr_OnKEYPRESS  )
#define attrIsOnKEYUP(av)           AttrIsId( av, TidyAttr_OnKEYUP  )
#define attrIsOnLOAD(av)            AttrIsId( av, TidyAttr_OnLOAD  )
#define attrIsOnMOUSEDOWN(av)       AttrIsId( av, TidyAttr_OnMOUSEDOWN  )
#define attrIsOnMOUSEMOVE(av)       AttrIsId( av, TidyAttr_OnMOUSEMOVE  )
#define attrIsOnMOUSEOUT(av)        AttrIsId( av, TidyAttr_OnMOUSEOUT  )
#define attrIsOnMOUSEOVER(av)       AttrIsId( av, TidyAttr_OnMOUSEOVER  )
#define attrIsOnMOUSEUP(av)         AttrIsId( av, TidyAttr_OnMOUSEUP  )
#define attrIsOnRESET(av)           AttrIsId( av, TidyAttr_OnRESET  )
#define attrIsOnROWENTER(av)        AttrIsId( av, TidyAttr_OnROWENTER  )
#define attrIsOnROWEXIT(av)         AttrIsId( av, TidyAttr_OnROWEXIT  )
#define attrIsOnSELECT(av)          AttrIsId( av, TidyAttr_OnSELECT  )
#define attrIsOnSUBMIT(av)          AttrIsId( av, TidyAttr_OnSUBMIT  )
#define attrIsOnUNLOAD(av)          AttrIsId( av, TidyAttr_OnUNLOAD  )
#define attrIsPROFILE(av)           AttrIsId( av, TidyAttr_PROFILE  )
#define attrIsPROMPT(av)            AttrIsId( av, TidyAttr_PROMPT  )
#define attrIsRBSPAN(av)            AttrIsId( av, TidyAttr_RBSPAN  )
#define attrIsREADONLY(av)          AttrIsId( av, TidyAttr_READONLY  )
#define attrIsREL(av)               AttrIsId( av, TidyAttr_REL  )
#define attrIsREV(av)               AttrIsId( av, TidyAttr_REV  )
#define attrIsRIGHTMARGIN(av)       AttrIsId( av, TidyAttr_RIGHTMARGIN  )
#define attrIsROLE(av)              AttrIsId( av, TidyAttr_ROLE  )
#define attrIsROWS(av)              AttrIsId( av, TidyAttr_ROWS  )
#define attrIsROWSPAN(av)           AttrIsId( av, TidyAttr_ROWSPAN  )
#define attrIsRULES(av)             AttrIsId( av, TidyAttr_RULES  )
#define attrIsSCHEME(av)            AttrIsId( av, TidyAttr_SCHEME  )
#define attrIsSCOPE(av)             AttrIsId( av, TidyAttr_SCOPE  )
#define attrIsSCROLLING(av)         AttrIsId( av, TidyAttr_SCROLLING  )
#define attrIsSELECTED(av)          AttrIsId( av, TidyAttr_SELECTED  )
#define attrIsSHAPE(av)             AttrIsId( av, TidyAttr_SHAPE  )
#define attrIsSHOWGRID(av)          AttrIsId( av, TidyAttr_SHOWGRID  )
#define attrIsSHOWGRIDX(av)         AttrIsId( av, TidyAttr_SHOWGRIDX  )
#define attrIsSHOWGRIDY(av)         AttrIsId( av, TidyAttr_SHOWGRIDY  )
#define attrIsSIZE(av)              AttrIsId( av, TidyAttr_SIZE  )
#define attrIsSLOT(av)              AttrIsId( av, TidyAttr_SLOT  )
#define attrIsSPAN(av)              AttrIsId( av, TidyAttr_SPAN  )
#define attrIsSRC(av)               AttrIsId( av, TidyAttr_SRC  )
#define attrIsSTANDBY(av)           AttrIsId( av, TidyAttr_STANDBY  )
#define attrIsSTART(av)             AttrIsId( av, TidyAttr_START  )
#define attrIsSTYLE(av)             AttrIsId( av, TidyAttr_STYLE  )
#define attrIsSUMMARY(av)           AttrIsId( av, TidyAttr_SUMMARY  )
#define attrIsTABINDEX(av)          AttrIsId( av, TidyAttr_TABINDEX  )
#define attrIsTARGET(av)            AttrIsId( av, TidyAttr_TARGET  )
#define attrIsTEXT(av)              AttrIsId( av, TidyAttr_TEXT  )
#define attrIsTITLE(av)             AttrIsId( av, TidyAttr_TITLE  )
#define attrIsTOPMARGIN(av)         AttrIsId( av, TidyAttr_TOPMARGIN  )
#define attrIsTYPE(av)              AttrIsId( av, TidyAttr_TYPE  )
#define attrIsUSEMAP(av)            AttrIsId( av, TidyAttr_USEMAP  )
#define attrIsVALIGN(av)            AttrIsId( av, TidyAttr_VALIGN  )
#define attrIsVALUE(av)             AttrIsId( av, TidyAttr_VALUE  )
#define attrIsVALUETYPE(av)         AttrIsId( av, TidyAttr_VALUETYPE  )
#define attrIsVERSION(av)           AttrIsId( av, TidyAttr_VERSION  )
#define attrIsVLINK(av)             AttrIsId( av, TidyAttr_VLINK  )
#define attrIsVSPACE(av)            AttrIsId( av, TidyAttr_VSPACE  )
#define attrIsWIDTH(av)             AttrIsId( av, TidyAttr_WIDTH  )
#define attrIsWRAP(av)              AttrIsId( av, TidyAttr_WRAP  )
#define attrIsXMLNS(av)             AttrIsId( av, TidyAttr_XMLNS  )
#define attrIsXML_LANG(av)          AttrIsId( av, TidyAttr_XML_LANG  )
#define attrIsXML_SPACE(av)         AttrIsId( av, TidyAttr_XML_SPACE  )
#define attrIsARIA_ACTIVEDESCENDANT(av) AttrIsId( av,  TidyAttr_ARIA_ACTIVEDESCENDANT  )
#define attrIsARIA_ATOMIC(av)           AttrIsId( av,  TidyAttr_ARIA_ATOMIC  )
#define attrIsARIA_AUTOCOMPLETE(av)     AttrIsId( av,  TidyAttr_ARIA_AUTOCOMPLETE  )
#define attrIsARIA_BUSY(av)             AttrIsId( av,  TidyAttr_ARIA_BUSY  )
#define attrIsARIA_CHECKED(av)          AttrIsId( av,  TidyAttr_ARIA_CHECKED  )
#define attrIsARIA_CONTROLS(av)         AttrIsId( av,  TidyAttr_ARIA_CONTROLS  )
#define attrIsARIA_DESCRIBEDBY(av)      AttrIsId( av,  TidyAttr_ARIA_DESCRIBEDBY  )
#define attrIsARIA_DISABLED(av)         AttrIsId( av,  TidyAttr_ARIA_DISABLED  )
#define attrIsARIA_DROPEFFECT(av)       AttrIsId( av,  TidyAttr_ARIA_DROPEFFECT  )
#define attrIsARIA_EXPANDED(av)         AttrIsId( av,  TidyAttr_ARIA_EXPANDED  )
#define attrIsARIA_FLOWTO(av)           AttrIsId( av,  TidyAttr_ARIA_FLOWTO  )
#define attrIsARIA_GRABBED(av)          AttrIsId( av,  TidyAttr_ARIA_GRABBED  )
#define attrIsARIA_HASPOPUP(av)         AttrIsId( av,  TidyAttr_ARIA_HASPOPUP  )
#define attrIsARIA_HIDDEN(av)           AttrIsId( av,  TidyAttr_ARIA_HIDDEN  )
#define attrIsARIA_INVALID(av)          AttrIsId( av,  TidyAttr_ARIA_INVALID  )
#define attrIsARIA_LABEL(av)            AttrIsId( av,  TidyAttr_ARIA_LABEL  )
#define attrIsARIA_LABELLEDBY(av)       AttrIsId( av,  TidyAttr_ARIA_LABELLEDBY  )
#define attrIsARIA_LEVEL(av)            AttrIsId( av,  TidyAttr_ARIA_LEVEL  )
#define attrIsARIA_LIVE(av)             AttrIsId( av,  TidyAttr_ARIA_LIVE  )
#define attrIsARIA_MULTILINE(av)        AttrIsId( av,  TidyAttr_ARIA_MULTILINE  )
#define attrIsARIA_MULTISELECTABLE(av)  AttrIsId( av,  TidyAttr_ARIA_MULTISELECTABLE  )
#define attrIsARIA_ORIENTATION(av)      AttrIsId( av,  TidyAttr_ARIA_ORIENTATION  )
#define attrIsARIA_OWNS(av)             AttrIsId( av,  TidyAttr_ARIA_OWNS  )
#define attrIsARIA_POSINSET(av)         AttrIsId( av,  TidyAttr_ARIA_POSINSET  )
#define attrIsARIA_PRESSED(av)          AttrIsId( av,  TidyAttr_ARIA_PRESSED  )
#define attrIsARIA_READONLY(av)         AttrIsId( av,  TidyAttr_ARIA_READONLY  )
#define attrIsARIA_RELEVANT(av)         AttrIsId( av,  TidyAttr_ARIA_RELEVANT  )
#define attrIsARIA_REQUIRED(av)         AttrIsId( av,  TidyAttr_ARIA_REQUIRED  )
#define attrIsARIA_SELECTED(av)         AttrIsId( av,  TidyAttr_ARIA_SELECTED  )
#define attrIsARIA_SETSIZE(av)          AttrIsId( av,  TidyAttr_ARIA_SETSIZE  )
#define attrIsARIA_SORT(av)             AttrIsId( av,  TidyAttr_ARIA_SORT  )
#define attrIsARIA_VALUEMAX(av)         AttrIsId( av,  TidyAttr_ARIA_VALUEMAX  )
#define attrIsARIA_VALUEMIN(av)         AttrIsId( av,  TidyAttr_ARIA_VALUEMIN  )
#define attrIsARIA_VALUENOW(av)         AttrIsId( av,  TidyAttr_ARIA_VALUENOW  )
#define attrIsARIA_VALUETEXT(av)        AttrIsId( av,  TidyAttr_ARIA_VALUETEXT  )
#define attrIsSVG_FILL(av)              AttrIsId( av,  TidyAttr_FILL  )
#define attrIsSVG_FILLRULE(av)          AttrIsId( av,  TidyAttr_FILLRULE  )
#define attrIsSVG_STROKE(av)            AttrIsId( av,  TidyAttr_STROKE  )
#define attrIsSVG_STROKEDASHARRAY(av)   AttrIsId( av,  TidyAttr_STROKEDASHARRAY  )
#define attrIsSVG_STROKEDASHOFFSET(av)  AttrIsId( av,  TidyAttr_STROKEDASHOFFSET  )
#define attrIsSVG_STROKELINECAP(av)     AttrIsId( av,  TidyAttr_STROKELINECAP  )
#define attrIsSVG_STROKELINEJOIN(av)    AttrIsId( av,  TidyAttr_STROKELINEJOIN  )
#define attrIsSVG_STROKEMITERLIMIT(av)  AttrIsId( av,  TidyAttr_STROKEMITERLIMIT  )
#define attrIsSVG_STROKEWIDTH(av)       AttrIsId( av,  TidyAttr_STROKEWIDTH  )
#define attrIsSVG_COLORINTERPOLATION(a) AttrIsId(  a,  TidyAttr_COLORINTERPOLATION  )
#define attrIsSVG_COLORRENDERING(av)    AttrIsId( av,  TidyAttr_COLORRENDERING  )
#define attrIsSVG_OPACITY(av)           AttrIsId( av,  TidyAttr_OPACITY  )
#define attrIsSVG_STROKEOPACITY(av)     AttrIsId( av,  TidyAttr_STROKEOPACITY  )
#define attrIsSVG_FILLOPACITY(av)       AttrIsId( av,  TidyAttr_FILLOPACITY  )

/* Attribute Retrieval macros
*/
#define attrGetHREF( nod )        TY_(AttrGetById)( nod, TidyAttr_HREF  )
#define attrGetSRC( nod )         TY_(AttrGetById)( nod, TidyAttr_SRC  )
#define attrGetID( nod )          TY_(AttrGetById)( nod, TidyAttr_ID  )
#define attrGetNAME( nod )        TY_(AttrGetById)( nod, TidyAttr_NAME  )
#define attrGetSUMMARY( nod )     TY_(AttrGetById)( nod, TidyAttr_SUMMARY  )
#define attrGetALT( nod )         TY_(AttrGetById)( nod, TidyAttr_ALT  )
#define attrGetLONGDESC( nod )    TY_(AttrGetById)( nod, TidyAttr_LONGDESC  )
#define attrGetUSEMAP( nod )      TY_(AttrGetById)( nod, TidyAttr_USEMAP  )
#define attrGetISMAP( nod )       TY_(AttrGetById)( nod, TidyAttr_ISMAP  )
#define attrGetLANGUAGE( nod )    TY_(AttrGetById)( nod, TidyAttr_LANGUAGE  )
#define attrGetTYPE( nod )        TY_(AttrGetById)( nod, TidyAttr_TYPE  )
#define attrGetVALUE( nod )       TY_(AttrGetById)( nod, TidyAttr_VALUE  )
#define attrGetCONTENT( nod )     TY_(AttrGetById)( nod, TidyAttr_CONTENT  )
#define attrGetTITLE( nod )       TY_(AttrGetById)( nod, TidyAttr_TITLE  )
#define attrGetXMLNS( nod )       TY_(AttrGetById)( nod, TidyAttr_XMLNS  )
#define attrGetDATAFLD( nod )     TY_(AttrGetById)( nod, TidyAttr_DATAFLD  )
#define attrGetWIDTH( nod )       TY_(AttrGetById)( nod, TidyAttr_WIDTH  )
#define attrGetHEIGHT( nod )      TY_(AttrGetById)( nod, TidyAttr_HEIGHT  )
#define attrGetFOR( nod )         TY_(AttrGetById)( nod, TidyAttr_FOR  )
#define attrGetSELECTED( nod )    TY_(AttrGetById)( nod, TidyAttr_SELECTED  )
#define attrGetCHARSET( nod )     TY_(AttrGetById)( nod, TidyAttr_CHARSET  )
#define attrGetCHECKED( nod )     TY_(AttrGetById)( nod, TidyAttr_CHECKED  )
#define attrGetLANG( nod )        TY_(AttrGetById)( nod, TidyAttr_LANG  )
#define attrGetTARGET( nod )      TY_(AttrGetById)( nod, TidyAttr_TARGET  )
#define attrGetHTTP_EQUIV( nod )  TY_(AttrGetById)( nod, TidyAttr_HTTP_EQUIV  )
#define attrGetREL( nod )         TY_(AttrGetById)( nod, TidyAttr_REL  )

#define attrGetOnMOUSEMOVE( nod ) TY_(AttrGetById)( nod, TidyAttr_OnMOUSEMOVE  )
#define attrGetOnMOUSEDOWN( nod ) TY_(AttrGetById)( nod, TidyAttr_OnMOUSEDOWN  )
#define attrGetOnMOUSEUP( nod )   TY_(AttrGetById)( nod, TidyAttr_OnMOUSEUP  )
#define attrGetOnCLICK( nod )     TY_(AttrGetById)( nod, TidyAttr_OnCLICK  )
#define attrGetOnMOUSEOVER( nod ) TY_(AttrGetById)( nod, TidyAttr_OnMOUSEOVER  )
#define attrGetOnMOUSEOUT( nod )  TY_(AttrGetById)( nod, TidyAttr_OnMOUSEOUT  )
#define attrGetOnKEYDOWN( nod )   TY_(AttrGetById)( nod, TidyAttr_OnKEYDOWN  )
#define attrGetOnKEYUP( nod )     TY_(AttrGetById)( nod, TidyAttr_OnKEYUP  )
#define attrGetOnKEYPRESS( nod )  TY_(AttrGetById)( nod, TidyAttr_OnKEYPRESS  )
#define attrGetOnFOCUS( nod )     TY_(AttrGetById)( nod, TidyAttr_OnFOCUS  )
#define attrGetOnBLUR( nod )      TY_(AttrGetById)( nod, TidyAttr_OnBLUR  )

#define attrGetBGCOLOR( nod )     TY_(AttrGetById)( nod, TidyAttr_BGCOLOR  )

#define attrGetLINK( nod )        TY_(AttrGetById)( nod, TidyAttr_LINK  )
#define attrGetALINK( nod )       TY_(AttrGetById)( nod, TidyAttr_ALINK  )
#define attrGetVLINK( nod )       TY_(AttrGetById)( nod, TidyAttr_VLINK  )

#define attrGetTEXT( nod )        TY_(AttrGetById)( nod, TidyAttr_TEXT  )
#define attrGetSTYLE( nod )       TY_(AttrGetById)( nod, TidyAttr_STYLE  )
#define attrGetABBR( nod )        TY_(AttrGetById)( nod, TidyAttr_ABBR  )
#define attrGetCOLSPAN( nod )     TY_(AttrGetById)( nod, TidyAttr_COLSPAN  )
#define attrGetFONT( nod )        TY_(AttrGetById)( nod, TidyAttr_FONT  )
#define attrGetBASEFONT( nod )    TY_(AttrGetById)( nod, TidyAttr_BASEFONT  )
#define attrGetROWSPAN( nod )     TY_(AttrGetById)( nod, TidyAttr_ROWSPAN  )

#define attrGetROLE( nod )        TY_(AttrGetById)( nod, TidyAttr_ROLE  )

#define attrGetARIA_ACTIVEDESCENDANT( nod ) TY_(AttrGetById)( nod,  TidyAttr_ARIA_ACTIVEDESCENDANT  )
#define attrGetARIA_ATOMIC( nod )           TY_(AttrGetById)( nod,  TidyAttr_ARIA_ATOMIC  )
#define attrGetARIA_AUTOCOMPLETE( nod )     TY_(AttrGetById)( nod,  TidyAttr_ARIA_AUTOCOMPLETE  )
#define attrGetARIA_BUSY( nod )             TY_(AttrGetById)( nod,  TidyAttr_ARIA_BUSY  )
#define attrGetARIA_CHECKED( nod )          TY_(AttrGetById)( nod,  TidyAttr_ARIA_CHECKED  )
#define attrGetARIA_CONTROLS( nod )         TY_(AttrGetById)( nod,  TidyAttr_ARIA_CONTROLS  )
#define attrGetARIA_DESCRIBEDBY( nod )      TY_(AttrGetById)( nod,  TidyAttr_ARIA_DESCRIBEDBY  )
#define attrGetARIA_DISABLED( nod )         TY_(AttrGetById)( nod,  TidyAttr_ARIA_DISABLED  )
#define attrGetARIA_DROPEFFECT( nod )       TY_(AttrGetById)( nod,  TidyAttr_ARIA_DROPEFFECT  )
#define attrGetARIA_EXPANDED( nod )         TY_(AttrGetById)( nod,  TidyAttr_ARIA_EXPANDED  )
#define attrGetARIA_FLOWTO( nod )           TY_(AttrGetById)( nod,  TidyAttr_ARIA_FLOWTO  )
#define attrGetARIA_GRABBED( nod )          TY_(AttrGetById)( nod,  TidyAttr_ARIA_GRABBED  )
#define attrGetARIA_HASPOPUP( nod )         TY_(AttrGetById)( nod,  TidyAttr_ARIA_HASPOPUP  )
#define attrGetARIA_HIDDEN( nod )           TY_(AttrGetById)( nod,  TidyAttr_ARIA_HIDDEN  )
#define attrGetARIA_INVALID( nod )          TY_(AttrGetById)( nod,  TidyAttr_ARIA_INVALID  )
#define attrGetARIA_LABEL( nod )            TY_(AttrGetById)( nod,  TidyAttr_ARIA_LABEL  )
#define attrGetARIA_LABELLEDBY( nod )       TY_(AttrGetById)( nod,  TidyAttr_ARIA_LABELLEDBY  )
#define attrGetARIA_LEVEL( nod )            TY_(AttrGetById)( nod,  TidyAttr_ARIA_LEVEL  )
#define attrGetARIA_LIVE( nod )             TY_(AttrGetById)( nod,  TidyAttr_ARIA_LIVE  )
#define attrGetARIA_MULTILINE( nod )        TY_(AttrGetById)( nod,  TidyAttr_ARIA_MULTILINE  )
#define attrGetARIA_MULTISELECTABLE( nod )  TY_(AttrGetById)( nod,  TidyAttr_ARIA_MULTISELECTABLE  )
#define attrGetARIA_ORIENTATION( nod )      TY_(AttrGetById)( nod,  TidyAttr_ARIA_ORIENTATION  )
#define attrGetARIA_OWNS( nod )             TY_(AttrGetById)( nod,  TidyAttr_ARIA_OWNS  )
#define attrGetARIA_POSINSET( nod )         TY_(AttrGetById)( nod,  TidyAttr_ARIA_POSINSET  )
#define attrGetARIA_PRESSED( nod )          TY_(AttrGetById)( nod,  TidyAttr_ARIA_PRESSED  )
#define attrGetARIA_READONLY( nod )         TY_(AttrGetById)( nod,  TidyAttr_ARIA_READONLY  )
#define attrGetARIA_RELEVANT( nod )         TY_(AttrGetById)( nod,  TidyAttr_ARIA_RELEVANT  )
#define attrGetARIA_REQUIRED( nod )         TY_(AttrGetById)( nod,  TidyAttr_ARIA_REQUIRED  )
#define attrGetARIA_SELECTED( nod )         TY_(AttrGetById)( nod,  TidyAttr_ARIA_SELECTED  )
#define attrGetARIA_SETSIZE( nod )          TY_(AttrGetById)( nod,  TidyAttr_ARIA_SETSIZE  )
#define attrGetARIA_SORT( nod )             TY_(AttrGetById)( nod,  TidyAttr_ARIA_SORT  )
#define attrGetARIA_VALUEMAX( nod )         TY_(AttrGetById)( nod,  TidyAttr_ARIA_VALUEMAX  )
#define attrGetARIA_VALUEMIN( nod )         TY_(AttrGetById)( nod,  TidyAttr_ARIA_VALUEMIN  )
#define attrGetARIA_VALUENOW( nod )         TY_(AttrGetById)( nod,  TidyAttr_ARIA_VALUENOW  )
#define attrGetARIA_VALUETEXT( nod )        TY_(AttrGetById)( nod,  TidyAttr_ARIA_VALUETEXT  )

#endif /* __ATTRS_H__ */
