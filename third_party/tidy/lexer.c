/* lexer.c -- Lexer for html parser

  (c) 1998-2008 (W3C) MIT, ERCIM, Keio University
  See tidy.h for the copyright notice.

*/

/*
  Given a file stream fp it returns a sequence of tokens.

     GetToken(fp) gets the next token
     UngetToken(fp) provides one level undo

  The tags include an attribute list:

    - linked list of attribute/value nodes
    - each node has 2 NULL-terminated strings.
    - entities are replaced in attribute values

  white space is compacted if not in preformatted mode
  If not in preformatted mode then leading white space
  is discarded and subsequent white space sequences
  compacted to single space characters.

  If XmlTags is no then Tag names are folded to upper
  case and attribute names to lower case.

 Not yet done:
    -   Doctype subset and marked sections
*/

#include "third_party/tidy/tidy-int.h"
#include "third_party/tidy/lexer.h"
#include "third_party/tidy/parser.h"
#include "third_party/tidy/entities.h"
#include "third_party/tidy/streamio.h"
#include "third_party/tidy/message.h"
#include "third_party/tidy/tmbstr.h"
#include "third_party/tidy/clean.h"
#include "third_party/tidy/utf8.h"
#include "third_party/tidy/streamio.h"
#include "libc/assert.h"
#include "third_party/tidy/sprtf.h"

#if defined(ENABLE_DEBUG_LOG)
/* #define DEBUG_ALLOCATION   special EXTRA allocation debug information - VERY NOISY */
static void check_me(char *name);
static Bool show_attrs = yes;
#define MX_TXT 8
static char buffer[(MX_TXT*4)+8]; /* NOTE extra for '...'\0 tail */
static tmbstr get_text_string(Lexer* lexer, Node *node)
{
    uint len = node->end - node->start;
    tmbstr cp = lexer->lexbuf + node->start;
    tmbstr end = lexer->lexbuf + node->end;
    unsigned char c;
    uint i = 0;
    Bool insp = no;
    if (len <= ((MX_TXT * 2) + 3)) {
        buffer[0] = 0;
        while (cp < end) {
            c = *cp;
            cp++;
            if (c == '\n') {
                buffer[i++] = '\\';
                buffer[i++] = 'n';
            } else if (c == '\t') {
                buffer[i++] = '\\';
                buffer[i++] = 't';
            } else if ( c == ' ' ) {
                if (!insp)
                    buffer[i++] = c;
                insp = yes;
            } else {
                buffer[i++] = c;
                insp = no;
            }
        }
    } else {
        char *end1 = cp + MX_TXT;
        char *bgn = cp + (len - MX_TXT);
        buffer[0] = 0;
        if (bgn < end1)
            bgn = end1;
        while (cp < end1) {
            c = *cp;
            cp++;
            if (c == '\n') {
                buffer[i++] = '\\';
                buffer[i++] = 'n';
            } else if (c == '\t') {
                buffer[i++] = '\\';
                buffer[i++] = 't';
            } else if ( c == ' ' ) {
                if (!insp)
                    buffer[i++] = c;
                insp = yes;
            } else {
                buffer[i++] = c;
                insp = no;
            }
            if (i >= MX_TXT)
                break;
        }
        c = '.';
        if ((i < len)&&(cp < bgn)) {
            buffer[i++] = c;
            cp++;
            if ((i < len)&&(cp < bgn)) {
                buffer[i++] = c;
                cp++;
                if ((i < len)&&(cp < bgn)) {
                    buffer[i++] = c;
                    cp++;
                }
            }
        }
        cp = bgn;
        insp = no;
        while (cp < end) {
            c = *cp;
            cp++;
            if (c == '\n') {
                buffer[i++] = '\\';
                buffer[i++] = 'n';
            } else if (c == '\t') {
                buffer[i++] = '\\';
                buffer[i++] = 't';
            } else if ( c == ' ' ) {
                if (!insp)
                    buffer[i++] = c;
                insp = yes;
            } else {
                buffer[i++] = c;
                insp = no;
            }
        }
    }
    buffer[i] = 0;
    return buffer;
}
static void Show_Node( TidyDocImpl* doc, const char *msg, Node *node )
{
    Lexer* lexer = doc->lexer;
    Bool lex = ((msg[0] == 'l')&&(msg[1] == 'e')) ? yes : no;
    int line = ( doc->lexer ? doc->lexer->lines : 0 );
    int col  = ( doc->lexer ? doc->lexer->columns : 0 );
    tmbstr src = lex ? "lexer" : "stream";
    SPRTF("R=%d C=%d: ", line, col );
    /* DEBUG: Be able to set a TRAP on a SPECIFIC row,col */
    if ((line == 3) && (col == 1)) {
        check_me("Show_Node"); /* just a debug trap */
    }
    if (lexer && lexer->token &&
        ((lexer->token->type == TextNode)||(node && (node->type == TextNode)))) {
        if (show_attrs) {
            uint len = node ? node->end - node->start : 0;
            tmbstr cp = node ? get_text_string( lexer, node ) : "NULL";
            SPRTF("Returning %s TextNode [%s]%u %s\n", msg, cp, len, src );
        } else {
            SPRTF("Returning %s TextNode %p... %s\n", msg, node, src );
        }
    } else {
        tmbstr name = node ? node->element ? node->element : "blank" : "NULL";
        if (show_attrs) {
            AttVal* av;
            SPRTF("Returning %s node <%s", msg, name);
            if (node) {
                for (av = node->attributes; av; av = av->next) {
                    name = av->attribute;
                    if (name) {
                        SPRTF(" %s",name);
                        if (av->value) {
                            SPRTF("=\"%s\"", av->value);
                        }
                    }
                }
            }
            SPRTF("> %s\n", src);
        } else {
            SPRTF("Returning %s node %p <%s>... %s\n", msg, node,
                name, src );
        }
    }
}
#define GTDBG(a,b,c) Show_Node(a,b,c)
#else /* ENABLE_DEBUG_LOG */
#define GTDBG(a,b,c)
#endif /* defined(ENABLE_DEBUG_LOG) */

/* Forward references
*/
/* swallows closing '>' */
static AttVal *ParseAttrs( TidyDocImpl* doc, Bool *isempty );

static tmbstr ParseAttribute( TidyDocImpl* doc, Bool* isempty,
                             Node **asp, Node **php );

static tmbstr ParseValue( TidyDocImpl* doc, ctmbstr name, Bool foldCase,
                         Bool *isempty, int *pdelim );

static Node *ParseDocTypeDecl(TidyDocImpl* doc);

static void AddAttrToList( AttVal** list, AttVal* av );

/* used to classify characters for lexical purposes */
#define MAP(c) ((unsigned)c < 128 ? lexmap[(unsigned)c] : 0)
static uint lexmap[128];

#define IsValidXMLAttrName(name) TY_(IsValidXMLID)(name)
#define IsValidXMLElemName(name) TY_(IsValidXMLID)(name)

static struct _doctypes
{
    uint score;
    uint vers;
    uint vers_out;
    Bool xhtml;
    ctmbstr name;
    ctmbstr fpi;
    ctmbstr si;
} const W3C_Doctypes[] =
{
  {  2, HT20, 200, no,  "HTML 2.0",               "-//IETF//DTD HTML 2.0//EN",              NULL,                                                       },
  {  2, HT20, 200, no,  "HTML 2.0",               "-//IETF//DTD HTML//EN",                  NULL,                                                       },
  {  2, HT20, 200, no,  "HTML 2.0",               "-//W3C//DTD HTML 2.0//EN",               NULL,                                                       },
  {  1, HT32, 320, no,  "HTML 3.2",               "-//W3C//DTD HTML 3.2//EN",               NULL,                                                       },
  {  1, HT32, 320, no,  "HTML 3.2",               "-//W3C//DTD HTML 3.2 Final//EN",         NULL,                                                       },
  {  1, HT32, 320, no,  "HTML 3.2",               "-//W3C//DTD HTML 3.2 Draft//EN",         NULL,                                                       },
  {  6, H40S, 400, no,  "HTML 4.0 Strict",        "-//W3C//DTD HTML 4.0//EN",               "http://www.w3.org/TR/REC-html40/strict.dtd"                },
  {  8, H40T, 400, no,  "HTML 4.0 Transitional",  "-//W3C//DTD HTML 4.0 Transitional//EN",  "http://www.w3.org/TR/REC-html40/loose.dtd"                 },
  {  7, H40F, 400, no,  "HTML 4.0 Frameset",      "-//W3C//DTD HTML 4.0 Frameset//EN",      "http://www.w3.org/TR/REC-html40/frameset.dtd"              },
  {  3, H41S, 401, no,  "HTML 4.01 Strict",       "-//W3C//DTD HTML 4.01//EN",              "http://www.w3.org/TR/html4/strict.dtd"                     },
  {  5, H41T, 401, no,  "HTML 4.01 Transitional", "-//W3C//DTD HTML 4.01 Transitional//EN", "http://www.w3.org/TR/html4/loose.dtd"                      },
  {  4, H41F, 401, no,  "HTML 4.01 Frameset",     "-//W3C//DTD HTML 4.01 Frameset//EN",     "http://www.w3.org/TR/html4/frameset.dtd"                   },
  {  9, X10S, 100, yes, "XHTML 1.0 Strict",       "-//W3C//DTD XHTML 1.0 Strict//EN",       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"         },
  { 11, X10T, 100, yes, "XHTML 1.0 Transitional", "-//W3C//DTD XHTML 1.0 Transitional//EN", "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"   },
  { 10, X10F, 100, yes, "XHTML 1.0 Frameset",     "-//W3C//DTD XHTML 1.0 Frameset//EN",     "http://www.w3.org/TR/xhtml1/DTD/xhtml1-frameset.dtd"       },
  { 12, XH11, 110, yes, "XHTML 1.1",              "-//W3C//DTD XHTML 1.1//EN",              "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"              },
  { 13, XB10, 100, yes, "XHTML Basic 1.0",        "-//W3C//DTD XHTML Basic 1.0//EN",        "http://www.w3.org/TR/xhtml-basic/xhtml-basic10.dtd"        },

  { 20, HT50, 500, no,  "HTML5",                  NULL,                                     NULL                                                        },
  { 21, XH50, 500, yes, "XHTML5",                 NULL,                                     NULL                                                        },

  /* final entry */
  {  0,    0, 0,  no,  NULL,                     NULL,                                     NULL                                                        }
};

/*
 * Issue #643 - Since VERS_FROM40 was extended to include VERS_HTML5
 * to be used in the expanded entity table some 155 times,
 * need a special macro here to denote just HTML 4 plus XHTML,
 * which is actually the former define of VERS_FROM40
 */
#define VERS_HMTL40PX        (VERS_HTML40|VERS_XHTML11|VERS_BASIC)

int TY_(HTMLVersion)(TidyDocImpl* doc)
{
    uint i;
    uint j = 0;
    uint score = 0;
    uint vers = doc->lexer->versions;
    uint dtver = doc->lexer->doctype;
    TidyDoctypeModes dtmode = (TidyDoctypeModes)cfg(doc, TidyDoctypeMode);
    Bool xhtml = (cfgBool(doc, TidyXmlOut) || doc->lexer->isvoyager) &&
                 !cfgBool(doc, TidyHtmlOut);
    Bool html4 = ((dtmode == TidyDoctypeStrict) || (dtmode == TidyDoctypeLoose) ||
                  (VERS_HMTL40PX & dtver) ? yes : no);
    Bool html5 = (!html4 && ((dtmode == TidyDoctypeAuto) ||
                  (dtmode == TidyDoctypeHtml5)) ? yes : no);

    if (xhtml && dtver == VERS_UNKNOWN) return XH50;
    if (dtver == VERS_UNKNOWN) return HT50;
    /* Issue #167 - if NOT XHTML, and doctype is default VERS_HTML5, then return HT50 */
    if (!xhtml && (dtver == VERS_HTML5)) return HT50;
    /* Issue #377 - If xhtml and (doctype == html5) and constrained vers contains XH50 return that,
       and really if tidy defaults to 'html5', then maybe 'auto' should also apply! */
    if (xhtml && html5 && ((vers & VERS_HTML5) == XH50)) return XH50;

    for (i = 0; W3C_Doctypes[i].name; ++i)
    {
        if ((xhtml && !(VERS_XHTML & W3C_Doctypes[i].vers)) ||
            (html4 && !(VERS_HMTL40PX & W3C_Doctypes[i].vers)))
            continue;

        if (vers & W3C_Doctypes[i].vers &&
            (W3C_Doctypes[i].score < score || !score))
        {
            score = W3C_Doctypes[i].score;
            j = i;
        }
    }

    if (score)
        return W3C_Doctypes[j].vers;

    return VERS_UNKNOWN;
}

static ctmbstr GetFPIFromVers(uint vers)
{
    uint i;

    for (i = 0; W3C_Doctypes[i].name; ++i)
        if (W3C_Doctypes[i].vers == vers)
            return W3C_Doctypes[i].fpi;

    return NULL;
}

static ctmbstr GetSIFromVers(uint vers)
{
    uint i;

    for (i = 0; W3C_Doctypes[i].name; ++i)
        if (W3C_Doctypes[i].vers == vers)
            return W3C_Doctypes[i].si;

    return NULL;
}

static ctmbstr GetNameFromVers(uint vers)
{
    uint i;

    for (i = 0; W3C_Doctypes[i].name; ++i)
        if (W3C_Doctypes[i].vers == vers)
            return W3C_Doctypes[i].name;

    return NULL;
}

static uint GetVersFromFPI(ctmbstr fpi)
{
    uint i;

    for (i = 0; W3C_Doctypes[i].name; ++i)
        if (W3C_Doctypes[i].fpi != NULL && TY_(tmbstrcasecmp)(W3C_Doctypes[i].fpi, fpi) == 0)
            return W3C_Doctypes[i].vers;

    return 0;
}

#ifdef ENABLE_DEBUG_LOG
#  ifndef EndBuf
#    define EndBuf(a)   ( a + strlen(a) )
#  endif

/* Issue #377 - Output diminishing version bits */
typedef struct tagV2S {
    uint bit;
    ctmbstr val;
}V2S, *PV2S;

static V2S v2s[] = {
    { HT20, "HT20" },
    { HT32, "HT32" },
    { H40S, "H40S" },
    { H40T, "H40T" },
    { H40F, "H40F" },
    { H41S, "H41S" },
    { H41T, "H41T" },
    { H41F, "H41F" },
    { X10S, "X10S" },
    { X10T, "X10T" },
    { X10F, "X10F" },
    { XH11, "XH11" },
    { XB10, "XB10" }, /* 4096u */
    /* { VERS_SUN, "VSUN" }, */
    /* { VERS_NETSCAPE, "VNET" }, */
    /* { VERS_MICROSOFT, "VMIC" }, 32768u */
    { VERS_XML, "VXML" }, /* 65536u */
        /* HTML5 */
    { HT50, "HT50" }, /* 131072u */
    { XH50, "XH50" }, /* 262144u */
    { 0,     0  }
};

/* Process the above table, adding a bit name,
   or '----' when not present   */
static char *add_vers_string( tmbstr buf, uint vers )
{
    PV2S pv2s = v2s;
    int len = (int)strlen(buf);
    while (pv2s->val) {
        if (vers & pv2s->bit) {
            if (len) {
                strcat(buf,"|");
                len++;
            }
            strcat(buf,pv2s->val);
            len += (int)strlen(pv2s->val);
            vers &= ~(pv2s->bit);
            if (!vers)
                break;
        } else {
            if (len) {
                strcat(buf,"|");
                len++;
            }
            strcat(buf,"----");
            len += 4;

        }
        pv2s++;
    }
    if (vers) { /* Should not have any here! */
        if (len)
            strcat(buf,"|");
        sprintf(EndBuf(buf),"%u",vers);
    }
    return buf;

}

/* Issue #377 - Show first Before: list, and then on any change
   Note the VERS_PROPRIETARY are exclude since they always remain */
void TY_(ConstrainVersion)(TidyDocImpl* doc, uint vers)
{
    static char vcur[256];
    static Bool dnfirst = no;
    uint curr = doc->lexer->versions; /* get current */
    doc->lexer->versions &= (vers | VERS_PROPRIETARY);
    if (curr != doc->lexer->versions) { /* only if different */
        if (!dnfirst) {
            dnfirst = yes;
            vcur[0] = 0;
            curr &= ~(VERS_PROPRIETARY);
            add_vers_string( vcur, curr );
            SPRTF("Before: %s\n", vcur);
        }
        vcur[0] = 0;
        curr = doc->lexer->versions;
        curr &= ~(VERS_PROPRIETARY);
        add_vers_string( vcur, curr );
        SPRTF("After : %s\n", vcur);
    }
}
#else /* !#if defined(ENABLE_DEBUG_LOG) */
/* everything is allowed in proprietary version of HTML */
/* this is handled here rather than in the tag/attr dicts */
void TY_(ConstrainVersion)(TidyDocImpl* doc, uint vers)
{
    doc->lexer->versions &= (vers | VERS_PROPRIETARY);
}
#endif /* #if defined(ENABLE_DEBUG_LOG) y/n */

Bool TY_(IsWhite)(uint c)
{
    uint map = MAP(c);

    return (map & white)!=0;
}

Bool TY_(IsNewline)(uint c)
{
    uint map = MAP(c);
    return (map & newline)!=0;
}

Bool TY_(IsDigit)(uint c)
{
    uint map;

    map = MAP(c);

    return (map & digit)!=0;
}

static Bool IsDigitHex(uint c)
{
    uint map;

    map = MAP(c);

    return (map & digithex)!=0;
}

Bool TY_(IsLetter)(uint c)
{
    uint map;

    map = MAP(c);

    return (map & letter)!=0;
}

Bool TY_(IsHTMLSpace)(uint c)
{
    return c == 0x020 || c == 0x009 || c == 0x00a || c == 0x00c || c == 0x00d;
}

Bool TY_(IsNamechar)(uint c)
{
    uint map = MAP(c);
    return (map & namechar)!=0;
}

Bool TY_(IsXMLLetter)(uint c)
{
    return ((c >= 0x41 && c <= 0x5a) ||
        (c >= 0x61 && c <= 0x7a) ||
        (c >= 0xc0 && c <= 0xd6) ||
        (c >= 0xd8 && c <= 0xf6) ||
        (c >= 0xf8 && c <= 0xff) ||
        (c >= 0x100 && c <= 0x131) ||
        (c >= 0x134 && c <= 0x13e) ||
        (c >= 0x141 && c <= 0x148) ||
        (c >= 0x14a && c <= 0x17e) ||
        (c >= 0x180 && c <= 0x1c3) ||
        (c >= 0x1cd && c <= 0x1f0) ||
        (c >= 0x1f4 && c <= 0x1f5) ||
        (c >= 0x1fa && c <= 0x217) ||
        (c >= 0x250 && c <= 0x2a8) ||
        (c >= 0x2bb && c <= 0x2c1) ||
        c == 0x386 ||
        (c >= 0x388 && c <= 0x38a) ||
        c == 0x38c ||
        (c >= 0x38e && c <= 0x3a1) ||
        (c >= 0x3a3 && c <= 0x3ce) ||
        (c >= 0x3d0 && c <= 0x3d6) ||
        c == 0x3da ||
        c == 0x3dc ||
        c == 0x3de ||
        c == 0x3e0 ||
        (c >= 0x3e2 && c <= 0x3f3) ||
        (c >= 0x401 && c <= 0x40c) ||
        (c >= 0x40e && c <= 0x44f) ||
        (c >= 0x451 && c <= 0x45c) ||
        (c >= 0x45e && c <= 0x481) ||
        (c >= 0x490 && c <= 0x4c4) ||
        (c >= 0x4c7 && c <= 0x4c8) ||
        (c >= 0x4cb && c <= 0x4cc) ||
        (c >= 0x4d0 && c <= 0x4eb) ||
        (c >= 0x4ee && c <= 0x4f5) ||
        (c >= 0x4f8 && c <= 0x4f9) ||
        (c >= 0x531 && c <= 0x556) ||
        c == 0x559 ||
        (c >= 0x561 && c <= 0x586) ||
        (c >= 0x5d0 && c <= 0x5ea) ||
        (c >= 0x5f0 && c <= 0x5f2) ||
        (c >= 0x621 && c <= 0x63a) ||
        (c >= 0x641 && c <= 0x64a) ||
        (c >= 0x671 && c <= 0x6b7) ||
        (c >= 0x6ba && c <= 0x6be) ||
        (c >= 0x6c0 && c <= 0x6ce) ||
        (c >= 0x6d0 && c <= 0x6d3) ||
        c == 0x6d5 ||
        (c >= 0x6e5 && c <= 0x6e6) ||
        (c >= 0x905 && c <= 0x939) ||
        c == 0x93d ||
        (c >= 0x958 && c <= 0x961) ||
        (c >= 0x985 && c <= 0x98c) ||
        (c >= 0x98f && c <= 0x990) ||
        (c >= 0x993 && c <= 0x9a8) ||
        (c >= 0x9aa && c <= 0x9b0) ||
        c == 0x9b2 ||
        (c >= 0x9b6 && c <= 0x9b9) ||
        (c >= 0x9dc && c <= 0x9dd) ||
        (c >= 0x9df && c <= 0x9e1) ||
        (c >= 0x9f0 && c <= 0x9f1) ||
        (c >= 0xa05 && c <= 0xa0a) ||
        (c >= 0xa0f && c <= 0xa10) ||
        (c >= 0xa13 && c <= 0xa28) ||
        (c >= 0xa2a && c <= 0xa30) ||
        (c >= 0xa32 && c <= 0xa33) ||
        (c >= 0xa35 && c <= 0xa36) ||
        (c >= 0xa38 && c <= 0xa39) ||
        (c >= 0xa59 && c <= 0xa5c) ||
        c == 0xa5e ||
        (c >= 0xa72 && c <= 0xa74) ||
        (c >= 0xa85 && c <= 0xa8b) ||
        c == 0xa8d ||
        (c >= 0xa8f && c <= 0xa91) ||
        (c >= 0xa93 && c <= 0xaa8) ||
        (c >= 0xaaa && c <= 0xab0) ||
        (c >= 0xab2 && c <= 0xab3) ||
        (c >= 0xab5 && c <= 0xab9) ||
        c == 0xabd ||
        c == 0xae0 ||
        (c >= 0xb05 && c <= 0xb0c) ||
        (c >= 0xb0f && c <= 0xb10) ||
        (c >= 0xb13 && c <= 0xb28) ||
        (c >= 0xb2a && c <= 0xb30) ||
        (c >= 0xb32 && c <= 0xb33) ||
        (c >= 0xb36 && c <= 0xb39) ||
        c == 0xb3d ||
        (c >= 0xb5c && c <= 0xb5d) ||
        (c >= 0xb5f && c <= 0xb61) ||
        (c >= 0xb85 && c <= 0xb8a) ||
        (c >= 0xb8e && c <= 0xb90) ||
        (c >= 0xb92 && c <= 0xb95) ||
        (c >= 0xb99 && c <= 0xb9a) ||
        c == 0xb9c ||
        (c >= 0xb9e && c <= 0xb9f) ||
        (c >= 0xba3 && c <= 0xba4) ||
        (c >= 0xba8 && c <= 0xbaa) ||
        (c >= 0xbae && c <= 0xbb5) ||
        (c >= 0xbb7 && c <= 0xbb9) ||
        (c >= 0xc05 && c <= 0xc0c) ||
        (c >= 0xc0e && c <= 0xc10) ||
        (c >= 0xc12 && c <= 0xc28) ||
        (c >= 0xc2a && c <= 0xc33) ||
        (c >= 0xc35 && c <= 0xc39) ||
        (c >= 0xc60 && c <= 0xc61) ||
        (c >= 0xc85 && c <= 0xc8c) ||
        (c >= 0xc8e && c <= 0xc90) ||
        (c >= 0xc92 && c <= 0xca8) ||
        (c >= 0xcaa && c <= 0xcb3) ||
        (c >= 0xcb5 && c <= 0xcb9) ||
        c == 0xcde ||
        (c >= 0xce0 && c <= 0xce1) ||
        (c >= 0xd05 && c <= 0xd0c) ||
        (c >= 0xd0e && c <= 0xd10) ||
        (c >= 0xd12 && c <= 0xd28) ||
        (c >= 0xd2a && c <= 0xd39) ||
        (c >= 0xd60 && c <= 0xd61) ||
        (c >= 0xe01 && c <= 0xe2e) ||
        c == 0xe30 ||
        (c >= 0xe32 && c <= 0xe33) ||
        (c >= 0xe40 && c <= 0xe45) ||
        (c >= 0xe81 && c <= 0xe82) ||
        c == 0xe84 ||
        (c >= 0xe87 && c <= 0xe88) ||
        c == 0xe8a ||
        c == 0xe8d ||
        (c >= 0xe94 && c <= 0xe97) ||
        (c >= 0xe99 && c <= 0xe9f) ||
        (c >= 0xea1 && c <= 0xea3) ||
        c == 0xea5 ||
        c == 0xea7 ||
        (c >= 0xeaa && c <= 0xeab) ||
        (c >= 0xead && c <= 0xeae) ||
        c == 0xeb0 ||
        (c >= 0xeb2 && c <= 0xeb3) ||
        c == 0xebd ||
        (c >= 0xec0 && c <= 0xec4) ||
        (c >= 0xf40 && c <= 0xf47) ||
        (c >= 0xf49 && c <= 0xf69) ||
        (c >= 0x10a0 && c <= 0x10c5) ||
        (c >= 0x10d0 && c <= 0x10f6) ||
        c == 0x1100 ||
        (c >= 0x1102 && c <= 0x1103) ||
        (c >= 0x1105 && c <= 0x1107) ||
        c == 0x1109 ||
        (c >= 0x110b && c <= 0x110c) ||
        (c >= 0x110e && c <= 0x1112) ||
        c == 0x113c ||
        c == 0x113e ||
        c == 0x1140 ||
        c == 0x114c ||
        c == 0x114e ||
        c == 0x1150 ||
        (c >= 0x1154 && c <= 0x1155) ||
        c == 0x1159 ||
        (c >= 0x115f && c <= 0x1161) ||
        c == 0x1163 ||
        c == 0x1165 ||
        c == 0x1167 ||
        c == 0x1169 ||
        (c >= 0x116d && c <= 0x116e) ||
        (c >= 0x1172 && c <= 0x1173) ||
        c == 0x1175 ||
        c == 0x119e ||
        c == 0x11a8 ||
        c == 0x11ab ||
        (c >= 0x11ae && c <= 0x11af) ||
        (c >= 0x11b7 && c <= 0x11b8) ||
        c == 0x11ba ||
        (c >= 0x11bc && c <= 0x11c2) ||
        c == 0x11eb ||
        c == 0x11f0 ||
        c == 0x11f9 ||
        (c >= 0x1e00 && c <= 0x1e9b) ||
        (c >= 0x1ea0 && c <= 0x1ef9) ||
        (c >= 0x1f00 && c <= 0x1f15) ||
        (c >= 0x1f18 && c <= 0x1f1d) ||
        (c >= 0x1f20 && c <= 0x1f45) ||
        (c >= 0x1f48 && c <= 0x1f4d) ||
        (c >= 0x1f50 && c <= 0x1f57) ||
        c == 0x1f59 ||
        c == 0x1f5b ||
        c == 0x1f5d ||
        (c >= 0x1f5f && c <= 0x1f7d) ||
        (c >= 0x1f80 && c <= 0x1fb4) ||
        (c >= 0x1fb6 && c <= 0x1fbc) ||
        c == 0x1fbe ||
        (c >= 0x1fc2 && c <= 0x1fc4) ||
        (c >= 0x1fc6 && c <= 0x1fcc) ||
        (c >= 0x1fd0 && c <= 0x1fd3) ||
        (c >= 0x1fd6 && c <= 0x1fdb) ||
        (c >= 0x1fe0 && c <= 0x1fec) ||
        (c >= 0x1ff2 && c <= 0x1ff4) ||
        (c >= 0x1ff6 && c <= 0x1ffc) ||
        c == 0x2126 ||
        (c >= 0x212a && c <= 0x212b) ||
        c == 0x212e ||
        (c >= 0x2180 && c <= 0x2182) ||
        (c >= 0x3041 && c <= 0x3094) ||
        (c >= 0x30a1 && c <= 0x30fa) ||
        (c >= 0x3105 && c <= 0x312c) ||
        (c >= 0xac00 && c <= 0xd7a3) ||
        (c >= 0x4e00 && c <= 0x9fa5) ||
        c == 0x3007 ||
        (c >= 0x3021 && c <= 0x3029) ||
        (c >= 0x4e00 && c <= 0x9fa5) ||
        c == 0x3007 ||
        (c >= 0x3021 && c <= 0x3029));
}

Bool TY_(IsXMLNamechar)(uint c)
{
    return (TY_(IsXMLLetter)(c) ||
        c == '.' || c == '_' ||
        c == ':' || c == '-' ||
        (c >= 0x300 && c <= 0x345) ||
        (c >= 0x360 && c <= 0x361) ||
        (c >= 0x483 && c <= 0x486) ||
        (c >= 0x591 && c <= 0x5a1) ||
        (c >= 0x5a3 && c <= 0x5b9) ||
        (c >= 0x5bb && c <= 0x5bd) ||
        c == 0x5bf ||
        (c >= 0x5c1 && c <= 0x5c2) ||
        c == 0x5c4 ||
        (c >= 0x64b && c <= 0x652) ||
        c == 0x670 ||
        (c >= 0x6d6 && c <= 0x6dc) ||
        (c >= 0x6dd && c <= 0x6df) ||
        (c >= 0x6e0 && c <= 0x6e4) ||
        (c >= 0x6e7 && c <= 0x6e8) ||
        (c >= 0x6ea && c <= 0x6ed) ||
        (c >= 0x901 && c <= 0x903) ||
        c == 0x93c ||
        (c >= 0x93e && c <= 0x94c) ||
        c == 0x94d ||
        (c >= 0x951 && c <= 0x954) ||
        (c >= 0x962 && c <= 0x963) ||
        (c >= 0x981 && c <= 0x983) ||
        c == 0x9bc ||
        c == 0x9be ||
        c == 0x9bf ||
        (c >= 0x9c0 && c <= 0x9c4) ||
        (c >= 0x9c7 && c <= 0x9c8) ||
        (c >= 0x9cb && c <= 0x9cd) ||
        c == 0x9d7 ||
        (c >= 0x9e2 && c <= 0x9e3) ||
        c == 0xa02 ||
        c == 0xa3c ||
        c == 0xa3e ||
        c == 0xa3f ||
        (c >= 0xa40 && c <= 0xa42) ||
        (c >= 0xa47 && c <= 0xa48) ||
        (c >= 0xa4b && c <= 0xa4d) ||
        (c >= 0xa70 && c <= 0xa71) ||
        (c >= 0xa81 && c <= 0xa83) ||
        c == 0xabc ||
        (c >= 0xabe && c <= 0xac5) ||
        (c >= 0xac7 && c <= 0xac9) ||
        (c >= 0xacb && c <= 0xacd) ||
        (c >= 0xb01 && c <= 0xb03) ||
        c == 0xb3c ||
        (c >= 0xb3e && c <= 0xb43) ||
        (c >= 0xb47 && c <= 0xb48) ||
        (c >= 0xb4b && c <= 0xb4d) ||
        (c >= 0xb56 && c <= 0xb57) ||
        (c >= 0xb82 && c <= 0xb83) ||
        (c >= 0xbbe && c <= 0xbc2) ||
        (c >= 0xbc6 && c <= 0xbc8) ||
        (c >= 0xbca && c <= 0xbcd) ||
        c == 0xbd7 ||
        (c >= 0xc01 && c <= 0xc03) ||
        (c >= 0xc3e && c <= 0xc44) ||
        (c >= 0xc46 && c <= 0xc48) ||
        (c >= 0xc4a && c <= 0xc4d) ||
        (c >= 0xc55 && c <= 0xc56) ||
        (c >= 0xc82 && c <= 0xc83) ||
        (c >= 0xcbe && c <= 0xcc4) ||
        (c >= 0xcc6 && c <= 0xcc8) ||
        (c >= 0xcca && c <= 0xccd) ||
        (c >= 0xcd5 && c <= 0xcd6) ||
        (c >= 0xd02 && c <= 0xd03) ||
        (c >= 0xd3e && c <= 0xd43) ||
        (c >= 0xd46 && c <= 0xd48) ||
        (c >= 0xd4a && c <= 0xd4d) ||
        c == 0xd57 ||
        c == 0xe31 ||
        (c >= 0xe34 && c <= 0xe3a) ||
        (c >= 0xe47 && c <= 0xe4e) ||
        c == 0xeb1 ||
        (c >= 0xeb4 && c <= 0xeb9) ||
        (c >= 0xebb && c <= 0xebc) ||
        (c >= 0xec8 && c <= 0xecd) ||
        (c >= 0xf18 && c <= 0xf19) ||
        c == 0xf35 ||
        c == 0xf37 ||
        c == 0xf39 ||
        c == 0xf3e ||
        c == 0xf3f ||
        (c >= 0xf71 && c <= 0xf84) ||
        (c >= 0xf86 && c <= 0xf8b) ||
        (c >= 0xf90 && c <= 0xf95) ||
        c == 0xf97 ||
        (c >= 0xf99 && c <= 0xfad) ||
        (c >= 0xfb1 && c <= 0xfb7) ||
        c == 0xfb9 ||
        (c >= 0x20d0 && c <= 0x20dc) ||
        c == 0x20e1 ||
        (c >= 0x302a && c <= 0x302f) ||
        c == 0x3099 ||
        c == 0x309a ||
        (c >= 0x30 && c <= 0x39) ||
        (c >= 0x660 && c <= 0x669) ||
        (c >= 0x6f0 && c <= 0x6f9) ||
        (c >= 0x966 && c <= 0x96f) ||
        (c >= 0x9e6 && c <= 0x9ef) ||
        (c >= 0xa66 && c <= 0xa6f) ||
        (c >= 0xae6 && c <= 0xaef) ||
        (c >= 0xb66 && c <= 0xb6f) ||
        (c >= 0xbe7 && c <= 0xbef) ||
        (c >= 0xc66 && c <= 0xc6f) ||
        (c >= 0xce6 && c <= 0xcef) ||
        (c >= 0xd66 && c <= 0xd6f) ||
        (c >= 0xe50 && c <= 0xe59) ||
        (c >= 0xed0 && c <= 0xed9) ||
        (c >= 0xf20 && c <= 0xf29) ||
        c == 0xb7 ||
        c == 0x2d0 ||
        c == 0x2d1 ||
        c == 0x387 ||
        c == 0x640 ||
        c == 0xe46 ||
        c == 0xec6 ||
        c == 0x3005 ||
        (c >= 0x3031 && c <= 0x3035) ||
        (c >= 0x309d && c <= 0x309e) ||
        (c >= 0x30fc && c <= 0x30fe));
}

Bool TY_(IsUpper)(uint c)
{
    uint map = MAP(c);

    return (map & uppercase)!=0;
}

uint TY_(ToLower)(uint c)
{
    uint map = MAP(c);

    if (map & uppercase)
        c += 'a' - 'A';

    return c;
}

uint TY_(ToUpper)(uint c)
{
    uint map = MAP(c);

    if (map & lowercase)
        c += (uint) ('A' - 'a' );

    return c;
}

/*
 return last character in string
 this is useful when trailing quotemark
 is missing on an attribute
*/
static tmbchar LastChar( tmbstr str )
{
    if ( str && *str )
    {
        int n = TY_(tmbstrlen)(str);
        return str[n-1];
    }
    return 0;
}

Lexer* TY_(NewLexer)( TidyDocImpl* doc )
{
    Lexer* lexer = (Lexer*) TidyDocAlloc( doc, sizeof(Lexer) );

    if ( lexer != NULL )
    {
        TidyClearMemory( lexer, sizeof(Lexer) );

        lexer->allocator = doc->allocator;
        lexer->lines = 1;
        lexer->columns = 1;
        lexer->state = LEX_CONTENT;

        lexer->versions = (VERS_ALL|VERS_PROPRIETARY);
        lexer->doctype = VERS_UNKNOWN;
        lexer->root = &doc->root;
    }
    return lexer;
}

static Bool EndOfInput( TidyDocImpl* doc )
{
    assert( doc->docIn != NULL );
    return ( !doc->docIn->pushed && TY_(IsEOF)(doc->docIn) );
}

void TY_(FreeLexer)( TidyDocImpl* doc )
{
    Lexer *lexer = doc->lexer;
    if ( lexer )
    {
        TY_(FreeStyles)( doc );

        /* See GetToken() */
        if ( lexer->pushed || lexer->itoken )
        {
            if (lexer->pushed)
                TY_(FreeNode)( doc, lexer->itoken );
            TY_(FreeNode)( doc, lexer->token );
        }

        while ( lexer->istacksize > 0 )
            TY_(PopInline)( doc, NULL );

        TidyDocFree( doc, lexer->istack );
        TidyDocFree( doc, lexer->lexbuf );
        TidyDocFree( doc, lexer );
        doc->lexer = NULL;
    }
}

/* Lexer uses bigger memory chunks than pprint as
** it must hold the entire input document. not just
** the last line or three.
*/
static void AddByte( Lexer *lexer, tmbchar ch )
{
    if ( lexer->lexsize + 2 >= lexer->lexlength )
    {
        tmbstr buf = NULL;
        uint allocAmt = lexer->lexlength;
        uint prev = allocAmt; /* Is. #761 */
        while ( lexer->lexsize + 2 >= allocAmt )
        {
            if ( allocAmt == 0 )
                allocAmt = 8192;
            else
                allocAmt *= 2;
            if (allocAmt < prev) /* Is. #761 - watch for wrap - and */
                TidyPanic(lexer->allocator, "\nPanic: out of internal memory!\nDocument input too big!\n");
        }
        buf = (tmbstr) TidyRealloc( lexer->allocator, lexer->lexbuf, allocAmt );
        if ( buf )
        {
          TidyClearMemory( buf + lexer->lexlength,
                           allocAmt - lexer->lexlength );
          lexer->lexbuf = buf;
          lexer->lexlength = allocAmt;
        }
    }

    lexer->lexbuf[ lexer->lexsize++ ] = ch;
    lexer->lexbuf[ lexer->lexsize ]   = '\0';  /* debug */
}

static void ChangeChar( Lexer *lexer, tmbchar c )
{
    if ( lexer->lexsize > 0 )
    {
        lexer->lexbuf[ lexer->lexsize-1 ] = c;
    }
}

/* store character c as UTF-8 encoded byte stream */
void TY_(AddCharToLexer)( Lexer *lexer, uint c )
{
    int i, err, count = 0;
    tmbchar buf[10] = {0};

    err = TY_(EncodeCharToUTF8Bytes)( c, buf, NULL, &count );
    if (err)
    {
        /* replacement character 0xFFFD encoded as UTF-8 */
        buf[0] = (byte) 0xEF;
        buf[1] = (byte) 0xBF;
        buf[2] = (byte) 0xBD;
        count = 3;
    }

    for ( i = 0; i < count; ++i )
        AddByte( lexer, buf[i] );
}

static void AddStringToLexer( Lexer *lexer, ctmbstr str )
{
    uint c;

    /*  Many (all?) compilers will sign-extend signed chars (the default) when
    **  converting them to unsigned integer values.  We must cast our char to
    **  unsigned char before assigning it to prevent this from happening.
    */
    while( 0 != (c = (unsigned char) *str++ ))
        TY_(AddCharToLexer)( lexer, c );
}


static void SetLexerLocus( TidyDocImpl* doc, Lexer *lexer )
{
    lexer->lines = doc->docIn->curline;
    lexer->columns = doc->docIn->curcol;
}

/*
    Issue #483
    Have detected the first of a surrogate pair...
    Try to find, decode the second...
    Already have '&' start...
*/

typedef enum {
    SP_ok,
    SP_failed,
    SP_error
}SPStatus;

static SPStatus GetSurrogatePair(TidyDocImpl* doc, Bool isXml, uint *pch)
{
    Lexer* lexer = doc->lexer;
    uint bufSize = 32;
    uint c, ch = 0, offset = 0;
    tmbstr buf = 0;
    SPStatus status = SP_error;  /* assume failed */
    int type = 0;   /* assume numeric */
    uint fch = *pch;
    int i;  /* has to be signed due to for i >= 0 */
    if (!lexer)
        return status;
    buf = (tmbstr)TidyRealloc(lexer->allocator, buf, bufSize);
    if (!buf)
        return status;
    while ((c = TY_(ReadChar)(doc->docIn)) != EndOfStream )
    {
        if (c == ';')
        {
            break;  /* reached end of entity */
        }
        if ((offset + 2) > bufSize)
        {
            bufSize *= 2;
            buf = (tmbstr)TidyRealloc(lexer->allocator, buf, bufSize);
            if (!buf)
            {
                break;
            }
        }
        buf[offset++] = c;  /* add char to buffer */
        if (offset == 1)
        {
            if (c != '#')   /* is a numeric entity */
                break;
        }
        else if (offset == 2 && ((c == 'x') || (!isXml && c == 'X')))
        {
            type = 1;   /* set hex digits */
        }
        else
        {
            if (type)   /* if hex digits */
            {
                if (!IsDigitHex(c))
                    break;
            }
            else    /* if numeric */
            {
                if (!TY_(IsDigit)(c))
                    break;
            }
        }
    }

    if (c == ';')
    {
        int scanned;

        buf[offset] = 0;
        if (type)
            scanned = sscanf(buf + 2, "%x", &ch);
        else
            scanned = sscanf(buf + 1, "%d", &ch);

        if (scanned == 1 && TY_(IsHighSurrogate)(ch))
        {
            ch = TY_(CombineSurrogatePair)(ch, fch);
            if (TY_(IsValidCombinedChar)(ch))
            {
                *pch = ch;  /* return combined pair value */
                status = SP_ok; /* full success - pair used */
            }
            else
            {
                status = SP_failed; /* is one of the 32 out-of-range pairs */
                *pch = 0xFFFD;  /* return substitute character */
                TY_(ReportSurrogateError)(doc, BAD_SURROGATE_PAIR, fch, ch); /* SP WARNING: -  */
            }
        }
    }

    if (status == SP_error)
    {
        /* Error condition - can only put back all the chars */
        if (c == ';') /* if last, not added to buffer */
            TY_(UngetChar)(c, doc->docIn);
        if (buf && offset)
        {
            /* correct the order for unget - last first */
            for (i = offset - 1; i >= 0; i--)
            {
                c = buf[i];
                TY_(UngetChar)(c, doc->docIn);
            }
        }
    }

    if (buf)
        TidyFree(lexer->allocator, buf);

    return status;
}

/*
  No longer attempts to insert missing ';' for unknown
 entities unless one was present already, since this
  gives unexpected results.

  For example:   <a href="something.htm?foo&bar&fred">
  was tidied to: <a href="something.htm?foo&amp;bar;&amp;fred;">
  rather than:   <a href="something.htm?foo&amp;bar&amp;fred">

  My thanks for Maurice Buxton for spotting this.

  Also Randy Waki pointed out the following case for the
  04 Aug 00 version (bug #433012):

  For example:   <a href="something.htm?id=1&lang=en">
  was tidied to: <a href="something.htm?id=1&lang;=en">
  rather than:   <a href="something.htm?id=1&amp;lang=en">

  where "lang" is a known entity (#9001), but browsers would
  misinterpret "&lang;" because it had a value > 256.

  So the case of an apparently known entity with a value > 256 and
  missing a semicolon is handled specially.

  "ParseEntity" is also a bit of a misnomer - it handles entities and
  numeric character references. Invalid NCR's are now reported.
*/
static void ParseEntity( TidyDocImpl* doc, GetTokenMode mode )
{
    typedef enum
    {
        ENT_default,
        ENT_numdec,
        ENT_numhex
    } ENTState;

    typedef Bool (*ENTfn)(uint);
    const ENTfn entFn[] = {
        TY_(IsNamechar),
        TY_(IsDigit),
        IsDigitHex
    };
    uint start;
    ENTState entState = ENT_default;
    uint charRead = 0;
    Bool semicolon = no, found = no;
    Bool isXml = cfgBool( doc, TidyXmlTags );
    Bool preserveEntities = cfgBool( doc, TidyPreserveEntities );
    uint c, ch, startcol, entver = 0;
    Lexer* lexer = doc->lexer;

    start = lexer->lexsize - 1;  /* to start at "&" */
    startcol = doc->docIn->curcol - 1;

    while ( (c = TY_(ReadChar)(doc->docIn)) != EndOfStream )
    {
        if ( c == ';' )
        {
            semicolon = yes;
            break;
        }
        ++charRead;

        if (charRead == 1 && c == '#')
        {
            if ( !cfgBool(doc, TidyNCR) ||
                 cfg(doc, TidyInCharEncoding) == BIG5 ||
                 cfg(doc, TidyInCharEncoding) == SHIFTJIS )
            {
                TY_(UngetChar)('#', doc->docIn);
                return;
            }

            TY_(AddCharToLexer)( lexer, c );
            entState = ENT_numdec;
            continue;
        }
        else if (charRead == 2 && entState == ENT_numdec
                 && (c == 'x' || (!isXml && c == 'X')) )
        {
            TY_(AddCharToLexer)( lexer, c );
            entState = ENT_numhex;
            continue;
        }

        if ( entFn[entState](c) )
        {
            TY_(AddCharToLexer)( lexer, c );
            continue;
        }

        /* otherwise put it back */
        TY_(UngetChar)( c, doc->docIn );
        break;
    }

    /* make sure entity is NULL terminated */
    lexer->lexbuf[lexer->lexsize] = '\0';

    /* Should contrain version to XML/XHTML if &apos;
    ** is encountered.  But this is not possible with
    ** Tidy's content model bit mask.
    */
    if ( TY_(tmbstrcmp)(lexer->lexbuf+start, "&apos") == 0
         && !cfgBool(doc, TidyXmlOut)
         && !lexer->isvoyager
         && !cfgBool(doc, TidyXhtmlOut)
         && !(TY_(HTMLVersion)(doc) == HT50) ) /* Issue #239 - no warning if in HTML5++ mode */
        TY_(ReportEntityError)( doc, APOS_UNDEFINED, lexer->lexbuf+start, 39 );

    if (( mode == OtherNamespace ) && ( c == ';' ))
    {
        /* #130 MathML attr and entity fix! */
        found = yes;
        ch = 255;
        entver = XH50|HT50;
        preserveEntities = yes;
    }
    else
    {
        /* Lookup entity code and version
        */
        found = TY_(EntityInfo)( lexer->lexbuf+start, isXml, &ch, &entver );
    }

    /* Issue #483 - Deal with 'surrogate pairs' */
    /* TODO: Maybe warning/error, like found a leading surrogate
       but no following surrogate! Maybe should avoid outputting
       invalid utf-8 for this entity - maybe substitute?  */
    if (!preserveEntities && found && TY_(IsLowSurrogate)(ch))
    {
        uint c1;
        if ((c1 = TY_(ReadChar)(doc->docIn)) == '&')
        {
            SPStatus status;
            /* Have a following entity,
               so there is a chance of having a valid surrogate pair */
            c1 = ch;    /* keep first value, in case of error */
            status = GetSurrogatePair(doc, isXml, &ch);
            if (status == SP_error)
            {
                TY_(ReportSurrogateError)(doc, BAD_SURROGATE_TAIL, c1, 0); /* SP WARNING: - using substitute character */
                TY_(UngetChar)('&', doc->docIn);  /* otherwise put it back */
            }
        }
        else
        {
            /* put this non-entity lead char back */
            TY_(UngetChar)(c1, doc->docIn);
            /* Have leading surrogate pair, with no tail */
            TY_(ReportSurrogateError)(doc, BAD_SURROGATE_TAIL, ch, 0); /* SP WARNING: - using substitute character */
            ch = 0xFFFD;
        }
    }
    else if (!preserveEntities && found && TY_(IsHighSurrogate)(ch))
    {
        /* Have trailing surrogate pair, with no lead */
        TY_(ReportSurrogateError)(doc, BAD_SURROGATE_LEAD, ch, 0); /* SP WARNING: - using substitute character */
        ch = 0xFFFD;
    }

    /* deal with unrecognized or invalid entities */
    /* #433012 - fix by Randy Waki 17 Feb 01 */
    /* report invalid NCR's - Terry Teague 01 Sep 01 */
    if ( !found || (ch >= 128 && ch <= 159) || (ch >= 256 && c != ';') )
    {
        /* set error position just before offending character */
        SetLexerLocus( doc, lexer );
        lexer->columns = startcol;

        if (lexer->lexsize > start + 1)
        {
            if (ch >= 128 && ch <= 159)
            {
                /* invalid numeric character reference */

                uint c1 = 0;
                int replaceMode = DISCARDED_CHAR;

                /* Always assume Win1252 in this circumstance. */
                c1 = TY_(DecodeWin1252)( ch );

                if ( c1 )
                    replaceMode = REPLACED_CHAR;

                if ( c != ';' )  /* issue warning if not terminated by ';' */
                    TY_(ReportEntityError)( doc, MISSING_SEMICOLON_NCR,
                                            lexer->lexbuf+start, c );

                TY_(ReportEncodingError)(doc, INVALID_NCR, ch, replaceMode == DISCARDED_CHAR);

                if ( c1 )
                {
                    /* make the replacement */
                    lexer->lexsize = start;
                    TY_(AddCharToLexer)( lexer, c1 );
                    semicolon = no;
                }
                else
                {
                    /* discard */
                    lexer->lexsize = start;
                    semicolon = no;
               }

            }
            else
                TY_(ReportEntityError)( doc, UNKNOWN_ENTITY,
                                        lexer->lexbuf+start, ch );

            if (semicolon)
                TY_(AddCharToLexer)( lexer, ';' );
        }
        else
        {
            /*\
             *  Issue #207 - A naked & is allowed in HTML5, as an unambiguous ampersand!
            \*/
            if (TY_(HTMLVersion)(doc) != HT50)
            {
                TY_(ReportEntityError)( doc, UNESCAPED_AMPERSAND,
                                    lexer->lexbuf+start, ch );
            }
        }
    }
    else
    {
        if ( c != ';' )    /* issue warning if not terminated by ';' */
        {
            /* set error position just before offending character */
            SetLexerLocus( doc, lexer );
            lexer->columns = startcol;
            TY_(ReportEntityError)( doc, MISSING_SEMICOLON, lexer->lexbuf+start, c );
        }

        if (preserveEntities)
            TY_(AddCharToLexer)( lexer, ';' );
        else
        {
            lexer->lexsize = start;
            if ( ch == 160 && (mode == Preformatted) )
                ch = ' ';
            TY_(AddCharToLexer)( lexer, ch );

            if ( ch == '&' && !cfgBool(doc, TidyQuoteAmpersand) )
                AddStringToLexer( lexer, "amp;" );
        }

        /* Detect extended vs. basic entities */
        TY_(ConstrainVersion)( doc, entver );
    }
}

static tmbchar ParseTagName( TidyDocImpl* doc )
{
    Lexer *lexer = doc->lexer;
    uint c = lexer->lexbuf[ lexer->txtstart ];
    Bool xml = cfgBool(doc, TidyXmlTags);

    /* fold case of first character in buffer */
    if (!xml && TY_(IsUpper)(c))
        lexer->lexbuf[lexer->txtstart] = (tmbchar) TY_(ToLower)(c);

    while ((c = TY_(ReadChar)(doc->docIn)) != EndOfStream)
    {
        if ((!xml && !TY_(IsNamechar)(c)) ||
            (xml && !TY_(IsXMLNamechar)(c)))
            break;

        /* fold case of subsequent characters */
        if (!xml && TY_(IsUpper)(c))
             c = TY_(ToLower)(c);

        TY_(AddCharToLexer)(lexer, c);
    }

    lexer->txtend = lexer->lexsize;
    return (tmbchar) c;
}

/*
  Used for elements and text nodes
  element name is NULL for text nodes
  start and end are offsets into lexbuf
  which contains the textual content of
  all elements in the parse tree.

  parent and content allow traversal
  of the parse tree in any direction.
  attributes are represented as a linked
  list of AttVal nodes which hold the
  strings for attribute/value pairs.
*/


Node *TY_(NewNode)(TidyAllocator* allocator, Lexer *lexer)
{
    Node* node = (Node*) TidyAlloc( allocator, sizeof(Node) );
    TidyClearMemory( node, sizeof(Node) );
    if ( lexer )
    {
        node->line = lexer->lines;
        node->column = lexer->columns;
    }
    node->type = TextNode;
#if defined(ENABLE_DEBUG_LOG) && defined(DEBUG_ALLOCATION)
    SPRTF("Allocated node %p\n", node );
#endif
    return node;
}

/* used to clone heading nodes when split by an <HR> */
Node *TY_(CloneNode)( TidyDocImpl* doc, Node *element )
{
    Lexer* lexer = doc->lexer;
    Node *node = TY_(NewNode)( lexer->allocator, lexer );

    node->start = lexer->lexsize;
    node->end   = lexer->lexsize;

    if ( element )
    {
        node->parent     = element->parent;
        node->type       = element->type;
        node->closed     = element->closed;
        node->implicit   = element->implicit;
        node->tag        = element->tag;
        node->element    = TY_(tmbstrdup)( doc->allocator, element->element );
        node->attributes = TY_(DupAttrs)( doc, element->attributes );
    }
    return node;
}

/* free node's attributes */
void TY_(FreeAttrs)( TidyDocImpl* doc, Node *node )
{
    while ( node->attributes )
    {
        AttVal *av = node->attributes;

        if ( av->attribute )
        {
            if ( (attrIsID(av) || attrIsNAME(av)) &&
                 TY_(IsAnchorElement)(doc, node) )
            {
                TY_(RemoveAnchorByNode)( doc, av->value, node );
            }
        }

        node->attributes = av->next;
        TY_(FreeAttribute)( doc, av );
    }
}

/* doesn't repair attribute list linkage */
void TY_(FreeAttribute)( TidyDocImpl* doc, AttVal *av )
{
    TY_(FreeNode)( doc, av->asp );
    TY_(FreeNode)( doc, av->php );
    TidyDocFree( doc, av->attribute );
    TidyDocFree( doc, av->value );
    TidyDocFree( doc, av );
}

/* detach attribute from node
*/
void TY_(DetachAttribute)( Node *node, AttVal *attr )
{
    AttVal *av, *prev = NULL;

    for ( av = node->attributes; av; av = av->next )
    {
        if ( av == attr )
        {
            if ( prev )
                prev->next = attr->next;
            else
                node->attributes = attr->next;
            break;
        }
        prev = av;
    }
}

/* detach attribute from node then free it
*/
void TY_(RemoveAttribute)( TidyDocImpl* doc, Node *node, AttVal *attr )
{
    TY_(DetachAttribute)( node, attr );
    TY_(FreeAttribute)( doc, attr );
}

/*
  Free document nodes by iterating through peers and recursing
  through children. Set next to NULL before calling TY_(FreeNode)()
  to avoid freeing peer nodes. Doesn't patch up prev/next links.
 */
void TY_(FreeNode)( TidyDocImpl* doc, Node *node )
{
#if defined(ENABLE_DEBUG_LOG) && defined(DEBUG_ALLOCATION)
    /* avoid showing free of root node! */
    if (node) {
        if (RootNode != node->type) {
            SPRTF("Free node %p\n", node);
        }
        else {
            SPRTF("Root node %p\n", node);
        }
    }
#endif

    while ( node )
    {
        Node* next = node->next;

        TY_(FreeAttrs)( doc, node );
        TY_(FreeNode)( doc, node->content );
        TidyDocFree( doc, node->element );
        if (RootNode != node->type)
            TidyDocFree( doc, node );
        else
            node->content = NULL;

        node = next;
    }
}

Node* TY_(TextToken)( Lexer *lexer )
{
    Node *node = TY_(NewNode)( lexer->allocator, lexer );
    node->start = lexer->txtstart;
    node->end = lexer->txtend;
    return node;
}

/* used for creating preformatted text from Word2000 */
Node *TY_(NewLineNode)( Lexer *lexer )
{
    Node *node = TY_(NewNode)( lexer->allocator, lexer );
    node->start = lexer->lexsize;
    TY_(AddCharToLexer)( lexer, (uint)'\n' );
    node->end = lexer->lexsize;
    return node;
}

/* used for adding a &nbsp; for Word2000 */
Node* TY_(NewLiteralTextNode)( Lexer *lexer, ctmbstr txt )
{
    Node *node = TY_(NewNode)( lexer->allocator, lexer );
    node->start = lexer->lexsize;
    AddStringToLexer( lexer, txt );
    node->end = lexer->lexsize;
    return node;
}

static Node* TagToken( TidyDocImpl* doc, NodeType type )
{
    Lexer* lexer = doc->lexer;
    Node* node = TY_(NewNode)( lexer->allocator, lexer );
    node->type = type;
    node->element = TY_(tmbstrndup)( doc->allocator,
                                     lexer->lexbuf + lexer->txtstart,
                                     lexer->txtend - lexer->txtstart );
    node->start = lexer->txtstart;
    node->end = lexer->txtstart;

    if ( type == StartTag || type == StartEndTag || type == EndTag )
        TY_(FindTag)(doc, node);

    return node;
}

static Node* NewToken(TidyDocImpl* doc, NodeType type)
{
    Lexer* lexer = doc->lexer;
    Node* node = TY_(NewNode)(lexer->allocator, lexer);
    node->type = type;
    node->start = lexer->txtstart;
    node->end = lexer->txtend;
    return node;
}

#define CommentToken(doc) NewToken(doc, CommentTag)
#define DocTypeToken(doc) NewToken(doc, DocTypeTag)
#define PIToken(doc)      NewToken(doc, ProcInsTag)
#define AspToken(doc)     NewToken(doc, AspTag)
#define JsteToken(doc)    NewToken(doc, JsteTag)
#define PhpToken(doc)     NewToken(doc, PhpTag)
#define XmlDeclToken(doc) NewToken(doc, XmlDecl)
#define SectionToken(doc) NewToken(doc, SectionTag)
#define CDATAToken(doc)   NewToken(doc, CDATATag)

void TY_(AddStringLiteral)( Lexer* lexer, ctmbstr str )
{
    byte c;
    while(0 != (c = *str++) ) {
        /*\
         *  Issue #286
         *  Previously this used TY_(AddCharToLexer)( lexer, c );
         *  which uses err = TY_(EncodeCharToUTF8Bytes)( c, buf, NULL, &count );
         *  But this is transferring already 'translated' data from an
         *  internal location to the lexer, so should use AddByte()
        \*/
        AddByte( lexer, c );
    }
}

/*
void AddStringLiteralLen( Lexer* lexer, ctmbstr str, int len )
{
    byte c;
    int ix;

    for ( ix=0; ix < len && (c = *str++); ++ix )
        TY_(AddCharToLexer)(lexer, c);
}
*/

/* find doctype element */
Node *TY_(FindDocType)( TidyDocImpl* doc )
{
    Node* node;
    for ( node = (doc ? doc->root.content : NULL);
          node && node->type != DocTypeTag;
          node = node->next )
        /**/;
    return node;
}

/* find parent container element */
Node* TY_(FindContainer)( Node* node )
{
    for ( node = (node ? node->parent : NULL);
          node && TY_(nodeHasCM)(node, CM_INLINE);
          node = node->parent )
        /**/;

    return node;
}


/* find html element */
Node *TY_(FindHTML)( TidyDocImpl* doc )
{
    Node *node;
    for ( node = (doc ? doc->root.content : NULL);
          node && !nodeIsHTML(node);
          node = node->next )
        /**/;

    return node;
}

/* find XML Declaration */
Node *TY_(FindXmlDecl)(TidyDocImpl* doc)
{
    Node *node;
    for ( node = (doc ? doc->root.content : NULL);
          node && !(node->type == XmlDecl);
          node = node->next )
        /**/;

    return node;
}


Node *TY_(FindHEAD)( TidyDocImpl* doc )
{
    Node *node = TY_(FindHTML)( doc );

    if ( node )
    {
        for ( node = node->content;
              node && !nodeIsHEAD(node);
              node = node->next )
            /**/;
    }

    return node;
}

Node *TY_(FindTITLE)(TidyDocImpl* doc)
{
    Node *node = TY_(FindHEAD)(doc);

    if (node)
        for (node = node->content;
             node && !nodeIsTITLE(node);
             node = node->next) {}

    return node;
}

Node *TY_(FindBody)( TidyDocImpl* doc )
{
    Node *node = ( doc ? doc->root.content : NULL );

    while ( node && !nodeIsHTML(node) )
        node = node->next;

    if (node == NULL)
        return NULL;

    node = node->content;
    while ( node && !nodeIsBODY(node) && !nodeIsFRAMESET(node) )
        node = node->next;

    if ( node && nodeIsFRAMESET(node) )
    {
        node = node->content;
        while ( node && !nodeIsNOFRAMES(node) )
            node = node->next;

        if ( node )
        {
            node = node->content;
            while ( node && !nodeIsBODY(node) )
                node = node->next;
        }
    }

    return node;
}

/* add meta element for Tidy */
Bool TY_(AddGenerator)( TidyDocImpl* doc )
{
    AttVal *attval;
    Node *node;
    Node *head = TY_(FindHEAD)( doc );
    tmbchar buf[256];

    if (head)
    {
#ifdef PLATFORM_NAME
        TY_(tmbsnprintf)(buf, sizeof(buf), "HTML Tidy for HTML5 for "PLATFORM_NAME" version %s",
                         tidyLibraryVersion());
#else
        TY_(tmbsnprintf)(buf, sizeof(buf), "HTML Tidy for HTML5 version %s", tidyLibraryVersion());
#endif

        for ( node = head->content; node; node = node->next )
        {
            if ( nodeIsMETA(node) )
            {
                attval = TY_(AttrGetById)(node, TidyAttr_NAME);

                if (AttrValueIs(attval, "generator"))
                {
                    attval = TY_(AttrGetById)(node, TidyAttr_CONTENT);

                    if (AttrHasValue(attval) &&
                        TY_(tmbstrncasecmp)(attval->value, "HTML Tidy", 9) == 0)
                    {
                        /* update the existing content to reflect the */
                        /* actual version of Tidy currently being used */

                        TidyDocFree(doc, attval->value);
                        attval->value = TY_(tmbstrdup)(doc->allocator, buf);
                        return no;
                    }
                }
            }
        }

        if ( cfg(doc, TidyAccessibilityCheckLevel) == 0 )
        {
            node = TY_(InferredTag)(doc, TidyTag_META);
            TY_(AddAttribute)( doc, node, "name", "generator" );
            TY_(AddAttribute)( doc, node, "content", buf );
            TY_(InsertNodeAtStart)( head, node );
            return yes;
        }
    }

    return no;
}

/*\ examine <!DOCTYPE ...> to identify version
 *  Issue #167 and #169
 *   If HTML5
 *        <!DOCTYPE html>
 *       <!DOCTYPE html SYSTEM "about:legacy-compat">
 *   else others
\*/
static uint FindGivenVersion( TidyDocImpl* doc, Node* doctype )
{
    AttVal * fpi = TY_(GetAttrByName)(doctype, "PUBLIC");
    uint vers;

    if (!fpi || !fpi->value)
    {
        /*\
         * Is. #815 - change to case-insensitive test
         * See REC: https://www.w3.org/TR/html5/syntax.html#the-doctype
        \*/
        if (doctype->element && (TY_(tmbstrcasecmp)(doctype->element,"html") == 0))
        {
            return VERS_HTML5;  /* TODO: do we need to check MORE? */
        }
        /* TODO: Consider warning, error message */
        return VERS_UNKNOWN;
    }
    vers = GetVersFromFPI(fpi->value);

    if (VERS_XHTML & vers)
    {
        TY_(SetOptionBool)(doc, TidyXmlOut, yes);
        TY_(SetOptionBool)(doc, TidyXhtmlOut, yes);
        doc->lexer->isvoyager = yes;
    }

    /* todo: add a warning if case does not match? */
    TidyDocFree(doc, fpi->value);
    fpi->value = TY_(tmbstrdup)(doc->allocator, GetFPIFromVers(vers));

    return vers;
}

/* return guessed version */
uint TY_(ApparentVersion)( TidyDocImpl* doc )
{
    if ((doc->lexer->doctype == XH11 ||
         doc->lexer->doctype == XB10) &&
        (doc->lexer->versions & doc->lexer->doctype))
        return doc->lexer->doctype;
    else
        return TY_(HTMLVersion)(doc);
}

ctmbstr TY_(HTMLVersionNameFromCode)( uint vers, Bool ARG_UNUSED(isXhtml) )
{
    ctmbstr name = GetNameFromVers(vers);
    return name;
}

uint TY_(HTMLVersionNumberFromCode)( uint vers )
{
    uint i;

    for (i = 0; W3C_Doctypes[i].name; ++i)
        if (W3C_Doctypes[i].vers == vers)
            return W3C_Doctypes[i].vers_out;

    return VERS_UNKNOWN;
}

Bool TY_(WarnMissingSIInEmittedDocType)( TidyDocImpl* doc )
{
    Bool isXhtml = doc->lexer->isvoyager;
    Node* doctype;

    /* Do not warn in XHTML mode */
    if ( isXhtml )
        return no;

    /* Do not warn if emitted doctype is proprietary */
    if ( TY_(HTMLVersionNameFromCode)(doc->lexer->versionEmitted, isXhtml ) == NULL )
        return no;

    /* Do not warn if no SI is possible */
    if ( GetSIFromVers(doc->lexer->versionEmitted) == NULL )
        return no;

    if ( (doctype = TY_(FindDocType)( doc )) != NULL
         && TY_(GetAttrByName)(doctype, "SYSTEM") == NULL )
        return yes;

    return no;
}


/* Put DOCTYPE declaration between the
** <?xml version "1.0" ... ?> declaration, if any,
** and the <html> tag.  Should also work for any comments,
** etc. that may precede the <html> tag.
*/

static Node* NewDocTypeNode( TidyDocImpl* doc )
{
    Node* doctype = NULL;
    Node* html = TY_(FindHTML)( doc );

    if ( !html )
        return NULL;

    doctype = TY_(NewNode)( doc->allocator, NULL );
    doctype->type = DocTypeTag;
    TY_(InsertNodeBeforeElement)(html, doctype);
    return doctype;
}

Bool TY_(SetXHTMLDocType)( TidyDocImpl* doc )
{
    Lexer *lexer = doc->lexer;
    Node *doctype = TY_(FindDocType)( doc );
    TidyDoctypeModes dtmode = (TidyDoctypeModes)cfg(doc, TidyDoctypeMode);
    ctmbstr pub = "PUBLIC";
    ctmbstr sys = "SYSTEM";

    lexer->versionEmitted = TY_(ApparentVersion)( doc );

    if (dtmode == TidyDoctypeOmit)
    {
        if (doctype)
            TY_(DiscardElement)(doc, doctype);
        return yes;
    }

    if (dtmode == TidyDoctypeUser && !cfgStr(doc, TidyDoctype))
        return no;

    if (!doctype)
    {
        doctype = NewDocTypeNode(doc);
        doctype->element = TY_(tmbstrdup)(doc->allocator, "html");
    }
    else
    {
        doctype->element = TY_(tmbstrtolower)(doctype->element);
    }

    switch(dtmode)
    {
    case TidyDoctypeHtml5:
        /* HTML5 */
        TY_(RepairAttrValue)(doc, doctype, pub, NULL);
        TY_(RepairAttrValue)(doc, doctype, sys, NULL);
        lexer->versionEmitted = XH50;
        break;
    case TidyDoctypeStrict:
        /* XHTML 1.0 Strict */
        TY_(RepairAttrValue)(doc, doctype, pub, GetFPIFromVers(X10S));
        TY_(RepairAttrValue)(doc, doctype, sys, GetSIFromVers(X10S));
        lexer->versionEmitted = X10S;
        break;
    case TidyDoctypeLoose:
        /* XHTML 1.0 Transitional */
        TY_(RepairAttrValue)(doc, doctype, pub, GetFPIFromVers(X10T));
        TY_(RepairAttrValue)(doc, doctype, sys, GetSIFromVers(X10T));
        lexer->versionEmitted = X10T;
        break;
    case TidyDoctypeUser:
        /* user defined document type declaration */
        TY_(RepairAttrValue)(doc, doctype, pub, cfgStr(doc, TidyDoctype));
        TY_(RepairAttrValue)(doc, doctype, sys, "");
        break;
    case TidyDoctypeAuto:
        if (lexer->doctype == VERS_UNKNOWN || lexer->doctype == VERS_HTML5) {
          lexer->versionEmitted = XH50;
          return yes;
        }
        else if (lexer->versions & XH11 && lexer->doctype == XH11)
        {
            if (!TY_(GetAttrByName)(doctype, sys))
                TY_(RepairAttrValue)(doc, doctype, sys, GetSIFromVers(XH11));
            lexer->versionEmitted = XH11;
            return yes;
        }
        else if (lexer->versions & XH11 && !(lexer->versions & VERS_HTML40))
        {
            TY_(RepairAttrValue)(doc, doctype, pub, GetFPIFromVers(XH11));
            TY_(RepairAttrValue)(doc, doctype, sys, GetSIFromVers(XH11));
            lexer->versionEmitted = XH11;
        }
        else if (lexer->versions & XB10 && lexer->doctype == XB10)
        {
            if (!TY_(GetAttrByName)(doctype, sys))
                TY_(RepairAttrValue)(doc, doctype, sys, GetSIFromVers(XB10));
            lexer->versionEmitted = XB10;
            return yes;
        }
        else if (lexer->versions & VERS_HTML40_STRICT)
        {
            TY_(RepairAttrValue)(doc, doctype, pub, GetFPIFromVers(X10S));
            TY_(RepairAttrValue)(doc, doctype, sys, GetSIFromVers(X10S));
            lexer->versionEmitted = X10S;
        }
        else if (lexer->versions & VERS_FRAMESET)
        {
            TY_(RepairAttrValue)(doc, doctype, pub, GetFPIFromVers(X10F));
            TY_(RepairAttrValue)(doc, doctype, sys, GetSIFromVers(X10F));
            lexer->versionEmitted = X10F;
        }
        else if (lexer->versions & VERS_LOOSE)
        {
            TY_(RepairAttrValue)(doc, doctype, pub, GetFPIFromVers(X10T));
            TY_(RepairAttrValue)(doc, doctype, sys, GetSIFromVers(X10T));
            lexer->versionEmitted = X10T;
        }
        else if (lexer->versions & VERS_HTML5)
        {
            /*\
             *  Issue #273 - If still a html5/xhtml5 bit
             *  existing, that is the 'ConstrainVersion' has
             *  not eliminated all HTML5, then nothing to do here.
             *  Certainly do **not** delete the DocType node!
             *  see: http://www.w3.org/QA/Tips/Doctype
            \*/
        }
        else
        {
            if (doctype)
                TY_(DiscardElement)(doc, doctype);
            return no;
        }
        break;
    case TidyDoctypeOmit:
        assert(0);
        break;
    }

    return no;
}

/* fixup doctype if missing */
Bool TY_(FixDocType)( TidyDocImpl* doc )
{
    Lexer* lexer = doc->lexer;
    Node* doctype = TY_(FindDocType)( doc );
    uint dtmode = cfg( doc, TidyDoctypeMode );
    uint guessed = VERS_UNKNOWN;
    Bool hadSI = no;

    /* Issue #167 - found doctype, and doctype is default VERS_HTML5, set VERS_HTML5 and return yes */
    if (doctype && (dtmode == TidyDoctypeAuto) &&
        (lexer->doctype == VERS_HTML5) )
    {
        /* The version emitted cannot be a composite value! */
        lexer->versionEmitted = HT50;
        return yes;
    }
    if (dtmode == TidyDoctypeAuto &&
        lexer->versions & lexer->doctype &&
        !(VERS_XHTML & lexer->doctype && !lexer->isvoyager)
        && TY_(FindDocType)(doc))
    {
        lexer->versionEmitted = lexer->doctype;
        return yes;
    }

    if (dtmode == TidyDoctypeOmit)
    {
        if (doctype)
            TY_(DiscardElement)( doc, doctype );
        lexer->versionEmitted = TY_(ApparentVersion)( doc );
        return yes;
    }

    if (cfgBool(doc, TidyXmlOut))
        return yes;

    if (doctype)
        hadSI = TY_(GetAttrByName)(doctype, "SYSTEM") != NULL;

    if ((dtmode == TidyDoctypeStrict ||
         dtmode == TidyDoctypeLoose) && doctype)
    {
        TY_(DiscardElement)(doc, doctype);
        doctype = NULL;
    }

    switch (dtmode)
    {
    case TidyDoctypeHtml5:
        guessed = HT50;
        break;
    case TidyDoctypeStrict:
        guessed = H41S;
        break;
    case TidyDoctypeLoose:
        guessed = H41T;
        break;
    case TidyDoctypeAuto:
        guessed = TY_(HTMLVersion)(doc);
        break;
    }

    lexer->versionEmitted = guessed;
    if (guessed == VERS_UNKNOWN)
        return no;

    if (doctype)
    {
        doctype->element = TY_(tmbstrtolower)(doctype->element);
    }
    else
    {
        doctype = NewDocTypeNode(doc);
        doctype->element = TY_(tmbstrdup)(doc->allocator, "html");
    }

    TY_(RepairAttrValue)(doc, doctype, "PUBLIC", GetFPIFromVers(guessed));

    if (hadSI)
        TY_(RepairAttrValue)(doc, doctype, "SYSTEM", GetSIFromVers(guessed));

    return yes;
}

/* ensure XML document starts with <?xml version="1.0"?> */
/* add encoding attribute if not using ASCII or UTF-8 output */
Bool TY_(FixXmlDecl)( TidyDocImpl* doc )
{
    Node* xml;
    AttVal *version, *encoding;
    Lexer*lexer = doc->lexer;
    Node* root = &doc->root;

    if ( root->content && root->content->type == XmlDecl )
    {
        xml = root->content;
    }
    else
    {
        xml = TY_(NewNode)(lexer->allocator, lexer);
        xml->type = XmlDecl;
        if ( root->content )
            TY_(InsertNodeBeforeElement)(root->content, xml);
        else
            root->content = xml;
    }

    version = TY_(GetAttrByName)(xml, "version");
    encoding = TY_(GetAttrByName)(xml, "encoding");

    /*
      We need to insert a check if declared encoding
      and output encoding mismatch and fix the XML
      declaration accordingly!!!
    */

    if ( encoding == NULL && cfg(doc, TidyOutCharEncoding) != UTF8 )
    {
        ctmbstr enc = TY_(GetEncodingNameFromTidyId)(cfg(doc, TidyOutCharEncoding));
        if ( enc )
            TY_(AddAttribute)( doc, xml, "encoding", enc );
    }

    if ( version == NULL )
        TY_(AddAttribute)( doc, xml, "version", "1.0" );
    return yes;
}

Node* TY_(InferredTag)(TidyDocImpl* doc, TidyTagId id)
{
    Lexer *lexer = doc->lexer;
    Node *node = TY_(NewNode)( lexer->allocator, lexer );
    const Dict* dict = TY_(LookupTagDef)(id);

    assert( dict != NULL );

    node->type = StartTag;
    node->implicit = yes;
    node->element = TY_(tmbstrdup)(doc->allocator, dict->name);
    node->tag = dict;
    node->start = lexer->txtstart;
    node->end = lexer->txtend;

    return node;
}

static Bool ExpectsContent(Node *node)
{
    if (node->type != StartTag)
        return no;

    /* unknown element? */
    if (node->tag == NULL)
        return yes;

    if (node->tag->model & CM_EMPTY)
        return no;

    return yes;
}

/*
  create a text node for the contents of
  a CDATA element like style or script
  which ends with </foo> for some foo.
*/

typedef enum
{
    CDATA_INTERMEDIATE,
    CDATA_STARTTAG,
    CDATA_ENDTAG
} CDATAState;

static Node *GetCDATA( TidyDocImpl* doc, Node *container )
{
    Lexer* lexer = doc->lexer;
    uint start = 0;
    int nested = 0;
    CDATAState state = CDATA_INTERMEDIATE;
    uint i;
    Bool isEmpty = yes;
    Bool matches = no;
    uint c;
    Bool hasSrc = (TY_(AttrGetById)(container, TidyAttr_SRC) != NULL) ? yes : no;
    /*\ Issue #65 (1642186) and #280 - is script or style, and the option on
     *  If yes, then avoid incrementing nested...
    \*/
    Bool nonested = ((nodeIsSCRIPT(container) || (nodeIsSTYLE(container))) &&
        cfgBool(doc, TidySkipNested)) ? yes : no;

    SetLexerLocus( doc, lexer );
    lexer->waswhite = no;
    lexer->txtstart = lexer->txtend = lexer->lexsize;

    /* seen start tag, look for matching end tag */
    while ((c = TY_(ReadChar)(doc->docIn)) != EndOfStream)
    {
        TY_(AddCharToLexer)(lexer, c);
        lexer->txtend = lexer->lexsize;

        if (state == CDATA_INTERMEDIATE)
        {
            if (c != '<')
            {
                if (isEmpty && !TY_(IsWhite)(c))
                    isEmpty = no;
                continue;
            }

            c = TY_(ReadChar)(doc->docIn);

            if (TY_(IsLetter)(c))
            {
                /* <head><script src=foo><meta name=foo content=bar>*/
                if (hasSrc && isEmpty && nodeIsSCRIPT(container))
                {
                    /* ReportError(doc, container, NULL, MISSING_ENDTAG_FOR); */
                    lexer->lexsize = lexer->txtstart;
                    TY_(UngetChar)(c, doc->docIn);
                    TY_(UngetChar)('<', doc->docIn);
                    return NULL;
                }
                TY_(AddCharToLexer)(lexer, c);
                start = lexer->lexsize - 1;
                state = CDATA_STARTTAG;
            }
            else if (c == '/')
            {
                TY_(AddCharToLexer)(lexer, c);

                c = TY_(ReadChar)(doc->docIn);

                if (!TY_(IsLetter)(c))
                {
                    TY_(UngetChar)(c, doc->docIn);
                    continue;
                }
                TY_(UngetChar)(c, doc->docIn);

                start = lexer->lexsize;
                state = CDATA_ENDTAG;
            }
            else if (c == '\\')
            {
                /* recognize document.write("<script><\/script>") */
                TY_(AddCharToLexer)(lexer, c);

                c = TY_(ReadChar)(doc->docIn);

                if (c != '/')
                {
                    TY_(UngetChar)(c, doc->docIn);
                    continue;
                }

                TY_(AddCharToLexer)(lexer, c);

                if (nonested) {
                    /*\
                     *  Issue #65 - for version 5.1.14.EXP2
                     *  If the nonested option is ON then the <script>
                     *  tag did not bump nested, so no need to treat this as
                     *  an end tag just to decrease nested, just continue!
                    \*/
                    continue;
                }

                c = TY_(ReadChar)(doc->docIn);

                if (!TY_(IsLetter)(c))
                {
                    TY_(UngetChar)(c, doc->docIn);
                    continue;
                }
                TY_(UngetChar)(c, doc->docIn);

                start = lexer->lexsize;
                state = CDATA_ENDTAG;
            }
            else
            {
                TY_(UngetChar)(c, doc->docIn);
            }
        }
        /* '<' + Letter found */
        else if (state == CDATA_STARTTAG)
        {
            if (TY_(IsLetter)(c))
                continue;

            matches = TY_(tmbstrncasecmp)(container->element, lexer->lexbuf + start,
                                          TY_(tmbstrlen)(container->element)) == 0;
            if (matches && !nonested)
                nested++;

            state = CDATA_INTERMEDIATE;
        }
        /* '<' + '/' + Letter found */
        else if (state == CDATA_ENDTAG)
        {
            if (TY_(IsLetter)(c))
                continue;

            matches = TY_(tmbstrncasecmp)(container->element, lexer->lexbuf + start,
                                          TY_(tmbstrlen)(container->element)) == 0;

            if (isEmpty && !matches)
            {
                /* ReportError(doc, container, NULL, MISSING_ENDTAG_FOR); */

                for (i = lexer->lexsize - 1; i >= start; --i)
                    TY_(UngetChar)((uint)lexer->lexbuf[i], doc->docIn);
                TY_(UngetChar)('/', doc->docIn);
                TY_(UngetChar)('<', doc->docIn);
                break;
            }

            if (matches && nested-- <= 0)
            {
                for (i = lexer->lexsize - 1; i >= start; --i)
                    TY_(UngetChar)((uint)lexer->lexbuf[i], doc->docIn);
                TY_(UngetChar)('/', doc->docIn);
                TY_(UngetChar)('<', doc->docIn);
                lexer->lexsize -= (lexer->lexsize - start) + 2;
                break;
            }
            else if (lexer->lexbuf[start - 2] != '\\')
            {
                /* if the end tag is not already escaped using backslash */
                SetLexerLocus( doc, lexer );
                lexer->columns -= 3;

                /*\ if javascript insert backslash before /
                 *  Issue #348 - Add option, escape-scripts, to skip
                \*/
                if ((TY_(IsJavaScript)(container)) && cfgBool(doc, TidyEscapeScripts) &&
                    !TY_(IsHTML5Mode)(doc) )    /* Is #700 - This only applies to legacy html4 mode */
                {
                    /* Issue #281 - only warn if adding the escape! */
                    TY_(Report)(doc, NULL, NULL, BAD_CDATA_CONTENT);

                    for (i = lexer->lexsize; i > start-1; --i)
                        lexer->lexbuf[i] = lexer->lexbuf[i-1];

                    lexer->lexbuf[start-1] = '\\';
                    lexer->lexsize++;
                }
            }
            state = CDATA_INTERMEDIATE;
        }
    }
    if (isEmpty)
        lexer->lexsize = lexer->txtstart = lexer->txtend;
    else
        lexer->txtend = lexer->lexsize;

    if (c == EndOfStream)
        TY_(Report)(doc, container, NULL, MISSING_ENDTAG_FOR );

    return TY_(TextToken)(lexer);
}

void TY_(UngetToken)( TidyDocImpl* doc )
{
    doc->lexer->pushed = yes;
}

#if defined(ENABLE_DEBUG_LOG)
#  define CondReturnTextNode(doc, skip) \
            if (lexer->txtend > lexer->txtstart) { \
                Node *_node = TY_(TextToken)(lexer); \
                lexer->token = _node; \
                GTDBG(doc,"text_node",_node); \
                return _node; \
            }

#else
#  define CondReturnTextNode(doc, skip) \
            if (lexer->txtend > lexer->txtstart) \
            { \
                lexer->token = TY_(TextToken)(lexer); \
                return lexer->token; \
            }
#endif

/*
  modes for GetToken()

  MixedContent   -- for elements which don't accept PCDATA
  Preformatted   -- white space preserved as is
  IgnoreMarkup   -- for CDATA elements such as script, style
*/
static Node* GetTokenFromStream( TidyDocImpl* doc, GetTokenMode mode );

Node* TY_(GetToken)( TidyDocImpl* doc, GetTokenMode mode )
{
    Node *node;
    Lexer* lexer = doc->lexer;

    if (lexer->pushed || lexer->itoken)
    {
        /* Deal with previously returned duplicate inline token */
        if (lexer->itoken)
        {
            /* itoken rejected */
            if (lexer->pushed)
            {
                lexer->pushed = no;
                node = lexer->itoken;
                GTDBG(doc,"lex-itoken", node);
                return node;
            }
            /* itoken has been accepted */
            lexer->itoken = NULL;
        }

        /* duplicate inlines in preference to pushed text nodes when appropriate */
        lexer->pushed = no;
        if (lexer->token->type != TextNode
            || !(lexer->insert || lexer->inode)) {
            node = lexer->token;
            GTDBG(doc,"lex-token", node);
            return node;
        }
        lexer->itoken = TY_(InsertedToken)( doc );
        node = lexer->itoken;
        GTDBG(doc,"lex-inserted", node);
        return node;
    }

    assert( !(lexer->pushed || lexer->itoken) );

    /* at start of block elements, unclosed inline
       elements are inserted into the token stream
       Issue #341 - Can NOT insert a token if NO istacksize
     */
    if ((lexer->insert || lexer->inode) && lexer->istacksize)
    {
        /*\ Issue #92: could fix by the following, but instead chose not to stack these 2
         *  if ( !(lexer->insert && (nodeIsINS(lexer->insert) || nodeIsDEL(lexer->insert))) ) {
        \*/
        lexer->token = TY_(InsertedToken)( doc );
        node = lexer->token;
        GTDBG(doc,"lex-inserted2", node);
        return node;
    }

    if (mode == CdataContent)
    {
        assert( lexer->parent != NULL );
        node = GetCDATA(doc, lexer->parent);
        GTDBG(doc,"lex-cdata", node);
        return node;
    }

    return GetTokenFromStream( doc, mode );
}

#if defined(ENABLE_DEBUG_LOG)
static void check_me(char *name)
{
    SPRTF("Have node %s\n", name);
}
#endif

static Node* GetTokenFromStream( TidyDocImpl* doc, GetTokenMode mode )
{
    Lexer* lexer = doc->lexer;
    uint c, lexdump, badcomment = 0;
    Bool isempty = no;
    AttVal *attributes = NULL;
    Node *node;
    Bool fixComments;

    switch ( cfgAutoBool(doc, TidyFixComments) )
    {
        case TidyYesState:
            fixComments = yes;
            break;

        case TidyNoState:
            fixComments = no;
            break;

        default:
            fixComments = (TY_(HTMLVersion)(doc) & HT50) == 0;
            break;
    }

    /* Lexer->token must be set on return. Nullify it for safety. */
    lexer->token = NULL;

    SetLexerLocus( doc, lexer );
    lexer->waswhite = no;

    lexer->txtstart = lexer->txtend = lexer->lexsize;

    while ((c = TY_(ReadChar)(doc->docIn)) != EndOfStream)
    {
        if (lexer->insertspace)
        {
            TY_(AddCharToLexer)(lexer, ' ');
            lexer->waswhite = yes;
            lexer->insertspace = no;
        }

        if (c == 160 && (mode == Preformatted))
            c = ' ';

        TY_(AddCharToLexer)(lexer, c);

        switch (lexer->state)
        {
            case LEX_CONTENT:  /* element content */

                /*
                 Discard white space if appropriate. Its cheaper
                 to do this here rather than in parser methods
                 for elements that don't have mixed content.
                */
                if (TY_(IsWhite)(c) && (mode == IgnoreWhitespace)
                      && lexer->lexsize == lexer->txtstart + 1)
                {
                    --(lexer->lexsize);
                    lexer->waswhite = no;
                    SetLexerLocus( doc, lexer );
                    continue;
                }

                if (c == '<')
                {
                    lexer->state = LEX_GT;
                    continue;
                }

                if (TY_(IsWhite)(c))
                {
                    /* was previous character white? */
                    if (lexer->waswhite)
                    {
                        if (mode != Preformatted && mode != IgnoreMarkup)
                        {
                            --(lexer->lexsize);
                            SetLexerLocus( doc, lexer );
                        }
                    }
                    else /* prev character wasn't white */
                    {
                        lexer->waswhite = yes;

                        if (mode != Preformatted && mode != IgnoreMarkup && c != ' ')
                            ChangeChar(lexer, ' ');
                    }

                    continue;
                }
                else if (c == '&' && mode != IgnoreMarkup)
                    ParseEntity( doc, mode );

                /* this is needed to avoid trimming trailing whitespace */
                if (mode == IgnoreWhitespace)
                    mode = MixedContent;

                lexer->waswhite = no;
                continue;

            case LEX_GT:  /* < */

                /* check for endtag */
                if (c == '/')
                {
                    if ((c = TY_(ReadChar)(doc->docIn)) == EndOfStream)
                    {
                        TY_(UngetChar)(c, doc->docIn);
                        continue;
                    }

                    TY_(AddCharToLexer)(lexer, c);

                    if (TY_(IsLetter)(c) || (cfgBool(doc, TidyXmlTags) && TY_(IsXMLNamechar)(c)))
                    {
                        lexer->lexsize -= 3;
                        lexer->txtend = lexer->lexsize;
                        TY_(UngetChar)(c, doc->docIn);
                        lexer->state = LEX_ENDTAG;
                        lexer->lexbuf[lexer->lexsize] = '\0';  /* debug */
                        doc->docIn->curcol -= 2;

                        /* if some text before the </ return it now */
                        if (lexer->txtend > lexer->txtstart)
                        {
                            /* trim space character before end tag */
                            if (mode == IgnoreWhitespace && lexer->lexbuf[lexer->lexsize - 1] == ' ')
                            {
                                lexer->lexsize -= 1;
                                lexer->txtend = lexer->lexsize;
                            }
                            lexer->token = TY_(TextToken)(lexer);
                            node = lexer->token;
                            GTDBG(doc,"text", node);
                            return node;
                        }

                        continue;       /* no text so keep going */
                    }

                    /* otherwise treat as CDATA */
                    lexer->waswhite = no;
                    lexer->state = LEX_CONTENT;
                    continue;
                }

                if (mode == IgnoreMarkup)
                {
                    /* otherwise treat as CDATA */
                    lexer->waswhite = no;
                    lexer->state = LEX_CONTENT;
                    continue;
                }

                /*
                   look out for comments, doctype or marked sections
                   this isn't quite right, but its getting there ...
                */
                if (c == '!')
                {
                    c = TY_(ReadChar)(doc->docIn);

                    if (c == '-')
                    {
                        c = TY_(ReadChar)(doc->docIn);

                        if (c == '-')
                        {
                            lexer->state = LEX_COMMENT;  /* comment */
                            lexer->lexsize -= 2;
                            lexer->txtend = lexer->lexsize;

                            CondReturnTextNode(doc, 4)

                            lexer->txtstart = lexer->lexsize;
                            continue;
                        }

                        /*
                           TY_(Report)(doc, NULL, NULL, MALFORMED_COMMENT_DROPPING );
                           Warning now done later - see issue #487
                         */
                    }
                    else if (c == 'd' || c == 'D')
                    {
                        /* todo: check for complete "<!DOCTYPE" not just <!D */

                        uint skip = 0;

                        lexer->state = LEX_DOCTYPE; /* doctype */
                        lexer->lexsize -= 2;
                        lexer->txtend = lexer->lexsize;
                        mode = IgnoreWhitespace;

                        /* skip until white space or '>' */

                        for (;;)
                        {
                            c = TY_(ReadChar)(doc->docIn);
                            ++skip;

                            if (c == EndOfStream || c == '>')
                            {
                                TY_(UngetChar)(c, doc->docIn);
                                break;
                            }


                            if (!TY_(IsWhite)(c))
                                continue;

                            /* and skip to end of whitespace */

                            for (;;)
                            {
                                c = TY_(ReadChar)(doc->docIn);
                                ++skip;

                                if (c == EndOfStream || c == '>')
                                {
                                    TY_(UngetChar)(c, doc->docIn);
                                    break;
                                }


                                if (TY_(IsWhite)(c))
                                    continue;

                                TY_(UngetChar)(c, doc->docIn);
                                break;
                            }

                            break;
                        }

                        CondReturnTextNode(doc, (skip + 3))

                        lexer->txtstart = lexer->lexsize;
                        continue;
                    }
                    else if (c == '[')
                    {
                        /* Word 2000 embeds <![if ...]> ... <![endif]> sequences */
                        lexer->lexsize -= 2;
                        lexer->state = LEX_SECTION;
                        lexer->txtend = lexer->lexsize;

                        CondReturnTextNode(doc, 2)

                        lexer->txtstart = lexer->lexsize;
                        continue;
                    }


                    /*
                       We only print this message if there's a missing
                       starting hyphen; this comment will be dropped.
                     */
                    TY_(Report)(doc, NULL, NULL, MALFORMED_COMMENT_DROPPING ); /* Is. #487 */

                    /* else swallow characters up to and including next '>' */
                    while ((c = TY_(ReadChar)(doc->docIn)) != '>')
                    {
                        if (c == EndOfStream)
                        {
                            TY_(UngetChar)(c, doc->docIn);
                            break;
                        }
                    }

                    lexer->lexsize -= 2;
                    lexer->lexbuf[lexer->lexsize] = '\0';
                    lexer->state = LEX_CONTENT;
                    continue;
                }

                /*
                   processing instructions
                */

                if (c == '?')
                {
                    lexer->lexsize -= 2;
                    lexer->state = LEX_PROCINSTR;
                    lexer->txtend = lexer->lexsize;

                    CondReturnTextNode(doc, 2)

                    lexer->txtstart = lexer->lexsize;
                    continue;
                }

                /* Microsoft ASP's e.g. <% ... server-code ... %> */
                if (c == '%')
                {
                    lexer->lexsize -= 2;
                    lexer->state = LEX_ASP;
                    lexer->txtend = lexer->lexsize;

                    CondReturnTextNode(doc, 2)

                    lexer->txtstart = lexer->lexsize;
                    continue;
                }

                /* Netscapes JSTE e.g. <# ... server-code ... #> */
                if (c == '#')
                {
                    lexer->lexsize -= 2;
                    lexer->state = LEX_JSTE;
                    lexer->txtend = lexer->lexsize;

                    CondReturnTextNode(doc, 2)

                    lexer->txtstart = lexer->lexsize;
                    continue;
                }

                /* check for start tag */
                if (TY_(IsLetter)(c) || (cfgBool(doc, TidyXmlTags) && TY_(IsXMLNamechar)(c)))
                {
                    TY_(UngetChar)(c, doc->docIn);     /* push back letter */
                    TY_(UngetChar)('<', doc->docIn);
                    lexer->lexsize -= 2;      /* discard "<" + letter */
                    lexer->txtend = lexer->lexsize;
                    lexer->state = LEX_STARTTAG;         /* ready to read tag name */

                    CondReturnTextNode(doc, 2)

                    /* lexer->txtstart = lexer->lexsize; missing here? */
                    continue;       /* no text so keep going */
                }

                /* otherwise treat as CDATA */
                /* fix for bug 762102 (486) */
                /* Issue #384 - Fix skipping parsing character, particularly '<<' */
                TY_(UngetChar)(c, doc->docIn);
                lexer->lexsize -= 1;
                lexer->state = LEX_CONTENT;
                lexer->waswhite = no;
                continue;

            case LEX_ENDTAG:  /* </letter */
                lexer->txtstart = lexer->lexsize - 1;
                doc->docIn->curcol += 2;
                c = ParseTagName( doc );
                lexer->token = TagToken( doc, EndTag );  /* create endtag token */
                lexer->lexsize = lexer->txtend = lexer->txtstart;

                /* skip to '>' */
                while ( c != '>' && c != EndOfStream )
                {
                    c = TY_(ReadChar)(doc->docIn);
                }

                if (c == EndOfStream)
                {
                    TY_(FreeNode)( doc, lexer->token );
                    continue;
                }

                lexer->state = LEX_CONTENT;
                lexer->waswhite = no;
                node = lexer->token;
                GTDBG(doc,"endtag", node);
                return node;  /* the endtag token */

            case LEX_STARTTAG: /* first letter of tagname */
                c = TY_(ReadChar)(doc->docIn);
                ChangeChar(lexer, (tmbchar)c);
                lexer->txtstart = lexer->lexsize - 1; /* set txtstart to first letter */
                c = ParseTagName( doc );
                isempty = no;
                attributes = NULL;
                lexer->token = TagToken( doc, StartTag ); /* [i_a]2 'isempty' is always false, thanks to code 2 lines above */

                /* parse attributes, consuming closing ">" */
                if (c != '>')
                {
                    if (c == '/')
                        TY_(UngetChar)(c, doc->docIn);

                    attributes = ParseAttrs( doc, &isempty );
                }

                if (isempty)
                    lexer->token->type = StartEndTag;

                lexer->token->attributes = attributes;
                lexer->lexsize = lexer->txtend = lexer->txtstart;

                /* swallow newline following start tag */
                /* special check needed for CRLF sequence */
                /* this doesn't apply to empty elements */
                /* nor to preformatted content that needs escaping */
                /*\
                 * Issue #230: Need to KEEP this user newline character in certain
                 * circumstances, certainly for <pre>, <script>, <style>...
                 * Any others?
                 * Issue #238: maybe **ONLY** for <pre>
                \*/
                if ( nodeIsPRE(lexer->token) )
                {
                    mode = Preformatted;
                }

                if ((mode != Preformatted && ExpectsContent(lexer->token))
                    || nodeIsBR(lexer->token) || nodeIsHR(lexer->token))
                {
                    c = TY_(ReadChar)(doc->docIn);

                    if ((c == '\n') && (mode != IgnoreWhitespace)) /* Issue #329 - Can NOT afford to lose this newline */
                        TY_(UngetChar)(c, doc->docIn);  /* Issue #329 - make sure the newline is maintained for now */
                    else if (c != '\n' && c != '\f')
                        TY_(UngetChar)(c, doc->docIn);

                    lexer->waswhite = yes;  /* to swallow leading whitespace */
                }
                else
                    lexer->waswhite = no;

                lexer->state = LEX_CONTENT;
                if (lexer->token->tag == NULL)
                {
                    if (mode != OtherNamespace) /* [i_a]2 only issue warning if NOT 'OtherNamespace', and tag null */
                    {
                        /* Special case for HTML5 unknown tags: if it looks
                           like an autonomous custom tag, then emit a variation
                           of the standard message. We don't want to do this
                           for older HTML, because it's not truly supported
                           by the standard, although Tidy will allow it. */
                        if ( (doc->lexer->doctype & VERS_HTML5) > 0 && TY_(elementIsAutonomousCustomFormat)( lexer->token->element ) )
                            TY_(Report)( doc, NULL, lexer->token, UNKNOWN_ELEMENT_LOOKS_CUSTOM );
                        else
                            TY_(Report)( doc, NULL, lexer->token, UNKNOWN_ELEMENT );
                    }
                }
                else if ( !cfgBool(doc, TidyXmlTags) )
                {
                    TY_(ConstrainVersion)( doc, lexer->token->tag->versions );
                    TY_(RepairDuplicateAttributes)( doc, lexer->token, no );
                } else
                    TY_(RepairDuplicateAttributes)( doc, lexer->token, yes );
                node = lexer->token;
                GTDBG(doc,"starttag", node);
                return node;  /* return start tag */

            case LEX_COMMENT:  /* seen <!-- so look for --> */

                if (c != '-')
                    continue;

                c = TY_(ReadChar)(doc->docIn);

                /* Fix hyphens at beginning of tag */
                if ( c != '-' && fixComments && lexer->lexsize - lexer->txtstart == 1 )
                {
                    lexer->lexbuf[lexer->lexsize - 1] = '=';
                }

                TY_(AddCharToLexer)(lexer, c);

                if (c != '-')
                    continue;

            end_comment:
                c = TY_(ReadChar)(doc->docIn);

                if (c == '>')
                {
                    if (badcomment)
                    {
                        /*
                           We've got bad comments that we either fixed or
                           ignored; provide proper user feedback based on
                           doctype and whether or not we fixed them.
                         */
                        if ( (TY_(HTMLVersion)(doc) & HT50) )
                        {
                            if ( fixComments )
                                TY_(Report)(doc, NULL, NULL, MALFORMED_COMMENT );
                            /* Otherwise for HTML5, it's safe to ignore. */
                        }
                        else
                        {
                            if ( fixComments )
                                TY_(Report)(doc, NULL, NULL, MALFORMED_COMMENT );
                            else
                                TY_(Report)(doc, NULL, NULL, MALFORMED_COMMENT_WARN );
                        }
                    }

                    /* do not store closing -- in lexbuf */
                    lexer->lexsize -= 2;
                    lexer->txtend = lexer->lexsize;
                    lexer->lexbuf[lexer->lexsize] = '\0';
                    lexer->state = LEX_CONTENT;
                    lexer->waswhite = no;
                    lexer->token = CommentToken(doc);

                    /* now look for a line break */

                    c = TY_(ReadChar)(doc->docIn);

                    if (c == '\n')
                        lexer->token->linebreak = yes;
                    else
                        TY_(UngetChar)(c, doc->docIn);

                    node = lexer->token;
                    GTDBG(doc,"comment", node);
                    return node;
                }

                /* note position of first such error in the comment */
                if (!badcomment)
                {
                    SetLexerLocus( doc, lexer );
                    lexer->columns -= 3;
                }

                badcomment++;

                /* fix hyphens in the middle */
                if ( fixComments )
                    lexer->lexbuf[lexer->lexsize - 2] = '=';

                /* if '-' then look for '>' to end the comment */
                if (c == '-')
                {
                    TY_(AddCharToLexer)(lexer, c);
                    goto end_comment;
                }

                /* fix hyphens end, and continue to look for --> */
                if ( fixComments )
                    lexer->lexbuf[lexer->lexsize - 1] = '=';

                /* http://tidy.sf.net/bug/1266647 */
                TY_(AddCharToLexer)(lexer, c);

                continue;

            case LEX_DOCTYPE:  /* seen <!d so look for '>' munging whitespace */

                /* use ParseDocTypeDecl() to tokenize doctype declaration */
                TY_(UngetChar)(c, doc->docIn);
                lexer->lexsize -= 1;
                lexer->token = ParseDocTypeDecl(doc);

                lexer->txtend = lexer->lexsize;
                lexer->lexbuf[lexer->lexsize] = '\0';
                lexer->state = LEX_CONTENT;
                lexer->waswhite = no;

                /* make a note of the version named by the 1st doctype */
                if (lexer->doctype == VERS_UNKNOWN && lexer->token && !cfgBool(doc, TidyXmlTags))
                {
                    lexer->doctype = FindGivenVersion(doc, lexer->token);
                    if (lexer->doctype != VERS_HTML5)
                    {
                        /*\
                         *  Back to legacy HTML4 mode for -
                         *  Issue #167 & #169 - TidyTag_A
                         *  Issue #196        - TidyTag_CAPTION
                         *  others?
                        \*/
                        TY_(AdjustTags)(doc); /* Dynamically modify the tags table  */
                    }
                }
                node = lexer->token;
                GTDBG(doc,"doctype", node);
                return node;

            case LEX_PROCINSTR:  /* seen <? so look for '>' */
                /* check for PHP preprocessor instructions <?php ... ?> */

                if  (lexer->lexsize - lexer->txtstart == 3)
                {
                    if (TY_(tmbstrncmp)(lexer->lexbuf + lexer->txtstart, "php", 3) == 0)
                    {
                        lexer->state = LEX_PHP;
                        continue;
                    }
                }

                if  (lexer->lexsize - lexer->txtstart == 4)
                {
                    if (TY_(tmbstrncmp)(lexer->lexbuf + lexer->txtstart, "xml", 3) == 0 &&
                        TY_(IsWhite)(lexer->lexbuf[lexer->txtstart + 3]))
                    {
                        lexer->state = LEX_XMLDECL;
                        attributes = NULL;
                        continue;
                    }
                }

                if (cfgBool(doc, TidyXmlPIs) || lexer->isvoyager) /* insist on ?> as terminator */
                {
                    if (c != '?')
                        continue;

                    /* now look for '>' */
                    c = TY_(ReadChar)(doc->docIn);

                    if (c == EndOfStream)
                    {
                        TY_(Report)(doc, NULL, NULL, UNEXPECTED_END_OF_FILE );
                        TY_(UngetChar)(c, doc->docIn);
                        continue;
                    }

                    TY_(AddCharToLexer)(lexer, c);
                }


                if (c != '>')
                    continue;

                lexer->lexsize -= 1;

                if (lexer->lexsize)
                {
                    uint i;
                    Bool closed;

                    for (i = 0; i < lexer->lexsize - lexer->txtstart &&
                        !TY_(IsWhite)(lexer->lexbuf[i + lexer->txtstart]); ++i)
                        /**/;

                    closed = lexer->lexbuf[lexer->lexsize - 1] == '?';

                    if (closed)
                        lexer->lexsize -= 1;

                    lexer->txtstart += i;
                    lexer->txtend = lexer->lexsize;
                    lexer->lexbuf[lexer->lexsize] = '\0';

                    lexer->token = PIToken(doc);
                    lexer->token->closed = closed;
                    lexer->token->element = TY_(tmbstrndup)(doc->allocator,
                                                            lexer->lexbuf +
                                                            lexer->txtstart - i, i);
                }
                else
                {
                    lexer->txtend = lexer->lexsize;
                    lexer->lexbuf[lexer->lexsize] = '\0';
                    lexer->token = PIToken(doc);
                }

                lexer->state = LEX_CONTENT;
                lexer->waswhite = no;
                node = lexer->token;
                GTDBG(doc,"procinstr", node);
                return node;

            case LEX_ASP:  /* seen <% so look for "%>" */
                if (c != '%')
                    continue;

                /* now look for '>' */
                c = TY_(ReadChar)(doc->docIn);


                if (c != '>')
                {
                    TY_(UngetChar)(c, doc->docIn);
                    continue;
                }

                lexer->lexsize -= 1;
                lexer->txtend = lexer->lexsize;
                lexer->lexbuf[lexer->lexsize] = '\0';
                lexer->state = LEX_CONTENT;
                lexer->waswhite = no;
                lexer->token = AspToken(doc);
                node = lexer->token;
                GTDBG(doc,"ASP", node);
                return node;  /* the endtag token */



            case LEX_JSTE:  /* seen <# so look for "#>" */
                if (c != '#')
                    continue;

                /* now look for '>' */
                c = TY_(ReadChar)(doc->docIn);


                if (c != '>')
                {
                    TY_(UngetChar)(c, doc->docIn);
                    continue;
                }

                lexer->lexsize -= 1;
                lexer->txtend = lexer->lexsize;
                lexer->lexbuf[lexer->lexsize] = '\0';
                lexer->state = LEX_CONTENT;
                lexer->waswhite = no;
                lexer->token = JsteToken(doc);
                node = lexer->token;
                GTDBG(doc,"JSTE", node);
                return node;  /* the JSTE token */


            case LEX_PHP: /* seen "<?php" so look for "?>" */
                if (c != '?')
                    continue;

                /* now look for '>' */
                c = TY_(ReadChar)(doc->docIn);

                if (c != '>')
                {
                    TY_(UngetChar)(c, doc->docIn);
                    continue;
                }

                lexer->lexsize -= 1;
                lexer->txtend = lexer->lexsize;
                lexer->lexbuf[lexer->lexsize] = '\0';
                lexer->state = LEX_CONTENT;
                lexer->waswhite = no;
                lexer->token = PhpToken(doc);
                node = lexer->token;
                GTDBG(doc,"PHP", node);
                return node;  /* the PHP token */

            case LEX_XMLDECL: /* seen "<?xml" so look for "?>" */

                if (TY_(IsWhite)(c) && c != '?')
                    continue;

                /* get pseudo-attribute */
                if (c != '?')
                {
                    tmbstr name;
                    Node *asp, *php;
                    AttVal *av = NULL;
                    int pdelim = 0;
                    isempty = no;

                    TY_(UngetChar)(c, doc->docIn);

                    name = ParseAttribute( doc, &isempty, &asp, &php );

                    if (!name)
                    {
                        /* check if attributes are created by ASP markup */
                        if (asp)
                        {
                            av = TY_(NewAttribute)(doc);
                            av->asp = asp;
                            AddAttrToList( &attributes, av );
                        }

                        /* check if attributes are created by PHP markup */
                        if (php)
                        {
                            av = TY_(NewAttribute)(doc);
                            av->php = php;
                            AddAttrToList( &attributes, av );
                        }

                        /* fix for http://tidy.sf.net/bug/788031 */
                        lexer->lexsize -= 1;
                        lexer->txtend = lexer->txtstart;
                        lexer->lexbuf[lexer->txtend] = '\0';
                        lexer->state = LEX_CONTENT;
                        lexer->waswhite = no;
                        lexer->token = XmlDeclToken(doc);
                        lexer->token->attributes = attributes;
                        node = lexer->token;
                        GTDBG(doc,"xml", node);
                        return node;  /* the xml token */
                    }

                    av = TY_(NewAttribute)(doc);
                    av->attribute = name;
                    av->value = ParseValue( doc, name, yes, &isempty, &pdelim );
                    av->delim = pdelim;
                    av->dict = TY_(FindAttribute)( doc, av );

                    AddAttrToList( &attributes, av );
                    /* continue; */
                }

                /* now look for '>' */
                c = TY_(ReadChar)(doc->docIn);

                if (c != '>')
                {
                    TY_(UngetChar)(c, doc->docIn);
                    continue;
                }
                lexer->lexsize -= 1;
                lexer->txtend = lexer->txtstart;
                lexer->lexbuf[lexer->txtend] = '\0';
                lexer->state = LEX_CONTENT;
                lexer->waswhite = no;
                lexer->token = XmlDeclToken(doc);
                lexer->token->attributes = attributes;
                node = lexer->token;
                GTDBG(doc,"XML", node);
                return node;  /* the XML token */

            case LEX_SECTION: /* seen "<![" so look for "]>" */
                if (c == '[')
                {
                    if (lexer->lexsize == (lexer->txtstart + 6) &&
                        TY_(tmbstrncmp)(lexer->lexbuf+lexer->txtstart, "CDATA[", 6) == 0)
                    {
                        lexer->state = LEX_CDATA;
                        lexer->lexsize -= 6;
                        continue;
                    }
                }

                if (c == '>')
                {
                    /* Is. #462 - reached '>' before ']' */
                    TY_(UngetChar)(c, doc->docIn);
                } else if (c != ']')
                    continue;

                /* now look for '>' */
                c = TY_(ReadChar)(doc->docIn);

                lexdump = 1;
                if (c != '>')
                {
                    /* Issue #153 - can also be ]'-->' */
                    if (c == '-')
                    {
                        c = TY_(ReadChar)(doc->docIn);
                        if (c == '-')
                        {
                            c = TY_(ReadChar)(doc->docIn);
                            if (c != '>')
                            {
                                TY_(UngetChar)(c, doc->docIn);
                                TY_(UngetChar)('-', doc->docIn);
                                TY_(UngetChar)('-', doc->docIn);
                                continue;
                            }
                            /* this failed!
                               TY_(AddCharToLexer)(lexer, '-'); TY_(AddCharToLexer)(lexer, '-'); lexdump = 0;
                               got output <![endif]--]> - needs further fix in pprint section output
                             */
                        }
                        else
                        {
                            TY_(UngetChar)(c, doc->docIn);
                            TY_(UngetChar)('-', doc->docIn);
                            continue;
                        }
                    }
                    else
                    {
                        TY_(UngetChar)(c, doc->docIn);
                        continue;
                    }
                }

                lexer->lexsize -= lexdump;
                lexer->txtend = lexer->lexsize;
                lexer->lexbuf[lexer->lexsize] = '\0';
                lexer->state = LEX_CONTENT;
                lexer->waswhite = no;
                lexer->token = SectionToken(doc);
                node = lexer->token;
                GTDBG(doc,"SECTION", node);
                return node;  /* the SECTION token */

            case LEX_CDATA: /* seen "<![CDATA[" so look for "]]>" */
                if (c != ']')
                    continue;

                /* now look for ']' */
                c = TY_(ReadChar)(doc->docIn);

                if (c != ']')
                {
                    TY_(UngetChar)(c, doc->docIn);
                    continue;
                }

                /* now look for '>' */
                c = TY_(ReadChar)(doc->docIn);

                if (c != '>')
                {
                    TY_(UngetChar)(c, doc->docIn);
                    TY_(UngetChar)(']', doc->docIn);
                    continue;
                }

                lexer->lexsize -= 1;
                lexer->txtend = lexer->lexsize;
                lexer->lexbuf[lexer->lexsize] = '\0';
                lexer->state = LEX_CONTENT;
                lexer->waswhite = no;
                lexer->token = CDATAToken(doc);
                node = lexer->token;
                GTDBG(doc,"CDATA", node);
                return node;  /* the CDATA token */
        }
    }

    if (lexer->state == LEX_CONTENT)  /* text string */
    {
        lexer->txtend = lexer->lexsize;

        if (lexer->txtend > lexer->txtstart)
        {
            TY_(UngetChar)(c, doc->docIn);

            if (lexer->lexbuf[lexer->lexsize - 1] == ' ')
            {
                lexer->lexsize -= 1;
                lexer->txtend = lexer->lexsize;
            }
            lexer->token = TY_(TextToken)(lexer);
            node = lexer->token;
            GTDBG(doc,"textstring", node);
            return node;  /* the textstring token */
        }
    }
    else if (lexer->state == LEX_COMMENT) /* comment */
    {
        if (c == EndOfStream)
        {
            /* We print this if we reached end of the stream mid-comment. */
            TY_(Report)(doc, NULL, NULL, MALFORMED_COMMENT_EOS );
        }

        lexer->txtend = lexer->lexsize;
        lexer->lexbuf[lexer->lexsize] = '\0';
        lexer->state = LEX_CONTENT;
        lexer->waswhite = no;
        lexer->token = CommentToken(doc);
        node = lexer->token;
        GTDBG(doc,"COMMENT", node);
        return node;  /* the COMMENT token */
    }

    /* check attributes before return NULL */
    if (attributes)
        TY_(FreeAttribute)( doc, attributes );

    DEBUG_LOG(SPRTF("Returning NULL...\n"));
    return NULL;
}

static void MapStr( ctmbstr str, uint code )
{
    while ( *str )
    {
        uint i = (byte) *str++;
        lexmap[i] |= code;
    }
}

void TY_(InitMap)(void)
{
    MapStr("\r\n\f", newline|white);
    MapStr(" \t", white);
    MapStr("-.:_", namechar);
    MapStr("0123456789", digit|digithex|namechar);
    MapStr("abcdefghijklmnopqrstuvwxyz", lowercase|letter|namechar);
    MapStr("ABCDEFGHIJKLMNOPQRSTUVWXYZ", uppercase|letter|namechar);
    MapStr("abcdefABCDEF", digithex);
}

/*
 parser for ASP within start tags

 Some people use ASP for to customize attributes
 Tidy isn't really well suited to dealing with ASP
 This is a workaround for attributes, but won't
 deal with the case where the ASP is used to tailor
 the attribute value. Here is an example of a work
 around for using ASP in attribute values:

  href='<%=rsSchool.Fields("ID").Value%>'

 where the ASP that generates the attribute value
 is masked from Tidy by the quotemarks.

*/

static Node *ParseAsp( TidyDocImpl* doc )
{
    Lexer* lexer = doc->lexer;
    uint c;
    Node *asp = NULL;

    lexer->txtstart = lexer->lexsize;

    for (;;)
    {
        if ((c = TY_(ReadChar)(doc->docIn)) == EndOfStream)
            break;

        TY_(AddCharToLexer)(lexer, c);


        if (c != '%')
            continue;

        if ((c = TY_(ReadChar)(doc->docIn)) == EndOfStream)
            break;

        TY_(AddCharToLexer)(lexer, c);

        if (c == '>')
        {
            lexer->lexsize -= 2;
            break;
        }
    }

    lexer->txtend = lexer->lexsize;
    if (lexer->txtend > lexer->txtstart)
        asp = AspToken(doc);

    lexer->txtstart = lexer->txtend;
    return asp;
}


/*
 PHP is like ASP but is based upon XML
 processing instructions, e.g. <?php ... ?>
*/
static Node *ParsePhp( TidyDocImpl* doc )
{
    Lexer* lexer = doc->lexer;
    uint c;
    Node *php = NULL;

    lexer->txtstart = lexer->lexsize;

    for (;;)
    {
        if ((c = TY_(ReadChar)(doc->docIn)) == EndOfStream)
            break;

        TY_(AddCharToLexer)(lexer, c);


        if (c != '?')
            continue;

        if ((c = TY_(ReadChar)(doc->docIn)) == EndOfStream)
            break;

        TY_(AddCharToLexer)(lexer, c);

        if (c == '>')
        {
            lexer->lexsize -= 2;
            break;
        }
    }

    lexer->txtend = lexer->lexsize;
    if (lexer->txtend > lexer->txtstart)
        php = PhpToken(doc);

    lexer->txtstart = lexer->txtend;
    return php;
}

/* consumes the '>' terminating start tags */
/* @TODO: float the errors back to the calling method */
static tmbstr  ParseAttribute( TidyDocImpl* doc, Bool *isempty,
                              Node **asp, Node **php )
{
    Lexer* lexer = doc->lexer;
    int start, len = 0;
    tmbstr attr = NULL;
    uint c, lastc;

    *asp = NULL;  /* clear asp pointer */
    *php = NULL;  /* clear php pointer */

 /* skip white space before the attribute */

    for (;;)
    {
        c = TY_(ReadChar)( doc->docIn );


        if (c == '/')
        {
            c = TY_(ReadChar)( doc->docIn );

            if (c == '>')
            {
                *isempty = yes;
                return NULL;
            }

            TY_(UngetChar)(c, doc->docIn);
            c = '/';
            break;
        }

        if (c == '>')
            return NULL;

        if (c =='<')
        {
            c = TY_(ReadChar)(doc->docIn);

            if (c == '%')
            {
                *asp = ParseAsp( doc );
                return NULL;
            }
            else if (c == '?')
            {
                *php = ParsePhp( doc );
                return NULL;
            }

            TY_(UngetChar)(c, doc->docIn);
            TY_(UngetChar)('<', doc->docIn);
            TY_(ReportAttrError)( doc, lexer->token, NULL, UNEXPECTED_GT );
            return NULL;
        }

        if (c == '=')
        {
            TY_(ReportAttrError)( doc, lexer->token, NULL, UNEXPECTED_EQUALSIGN );
            continue;
        }

        if (c == '"' || c == '\'')
        {
            TY_(ReportAttrError)( doc, lexer->token, NULL, UNEXPECTED_QUOTEMARK );
            continue;
        }

        if (c == EndOfStream)
        {
            TY_(ReportAttrError)( doc, lexer->token, NULL, UNEXPECTED_END_OF_FILE_ATTR );
            TY_(UngetChar)(c, doc->docIn);
            return NULL;
        }


        if (!TY_(IsWhite)(c))
           break;
    }

    start = lexer->lexsize;
    lastc = c;

    for (;;)
    {
     /* but push back '=' for parseValue() */
        if (c == '=' || c == '>')
        {
            TY_(UngetChar)(c, doc->docIn);
            break;
        }

        if (c == '<' || c == EndOfStream)
        {
            TY_(UngetChar)(c, doc->docIn);
            break;
        }

        if (lastc == '-' && (c == '"' || c == '\''))
        {
            lexer->lexsize--;
            --len;
            TY_(UngetChar)(c, doc->docIn);
            break;
        }

        if (TY_(IsWhite)(c))
            break;

        if (c == '/') /* Issue #395 - potential self closing tag */
        {
            c = TY_(ReadChar)(doc->docIn);  /* read next */
            if (c == '>')
            {
                /* got a self closing tag - put is back and continue... */
                TY_(UngetChar)(c, doc->docIn);
                break;
            }
            else
            {
                /* Not '/>' - put it back */
                TY_(UngetChar)(c, doc->docIn);
                c = '/';  /* restore original char */
            }
        }

        /* what should be done about non-namechar characters? */
        /* currently these are incorporated into the attr name */

        if ( cfg(doc, TidyUpperCaseAttrs) != TidyUppercasePreserve )
        {
            if ( !cfgBool(doc, TidyXmlTags) && TY_(IsUpper)(c) )
                c = TY_(ToLower)(c);
        }

        TY_(AddCharToLexer)( lexer, c );
        lastc = c;
        c = TY_(ReadChar)(doc->docIn);
    }

    /* handle attribute names with multibyte chars */
    len = lexer->lexsize - start;
    attr = (len > 0 ? TY_(tmbstrndup)(doc->allocator,
                                      lexer->lexbuf+start, len) : NULL);
    lexer->lexsize = start;
    return attr;
}

/*
 invoked when < is seen in place of attribute value
 but terminates on whitespace if not ASP, PHP or Tango
 this routine recognizes ' and " quoted strings
*/
static int ParseServerInstruction( TidyDocImpl* doc )
{
    Lexer* lexer = doc->lexer;
    uint c;
    int delim = '"';
    Bool isrule = no;

    c = TY_(ReadChar)(doc->docIn);
    TY_(AddCharToLexer)(lexer, c);

    /* check for ASP, PHP or Tango */
    if (c == '%' || c == '?' || c == '@')
        isrule = yes;

    for (;;)
    {
        c = TY_(ReadChar)(doc->docIn);

        if (c == EndOfStream)
            break;

        if (c == '>')
        {
            if (isrule)
                TY_(AddCharToLexer)(lexer, c);
            else
                TY_(UngetChar)(c, doc->docIn);

            break;
        }

        /* if not recognized as ASP, PHP or Tango */
        /* then also finish value on whitespace */
        if (!isrule)
        {
            if (TY_(IsWhite)(c))
                break;
        }

        TY_(AddCharToLexer)(lexer, c);

        if (c == '"')
        {
            do
            {
                c = TY_(ReadChar)(doc->docIn);
                if (c == EndOfStream) /* #427840 - fix by Terry Teague 30 Jun 01 */
                {
                    TY_(ReportAttrError)( doc, lexer->token, NULL, UNEXPECTED_END_OF_FILE_ATTR );
                    TY_(UngetChar)(c, doc->docIn);
                    return 0;
                }
                if (c == '>') /* #427840 - fix by Terry Teague 30 Jun 01 */
                {
                    TY_(UngetChar)(c, doc->docIn);
                    TY_(ReportAttrError)( doc, lexer->token, NULL, UNEXPECTED_GT );
                    return 0;
                }
                TY_(AddCharToLexer)(lexer, c);
            }
            while (c != '"');
            delim = '\'';
            continue;
        }

        if (c == '\'')
        {
            do
            {
                c = TY_(ReadChar)(doc->docIn);
                if (c == EndOfStream) /* #427840 - fix by Terry Teague 30 Jun 01 */
                {
                    TY_(ReportAttrError)( doc, lexer->token, NULL, UNEXPECTED_END_OF_FILE_ATTR );
                    TY_(UngetChar)(c, doc->docIn);
                    return 0;
                }
                if (c == '>') /* #427840 - fix by Terry Teague 30 Jun 01 */
                {
                    TY_(UngetChar)(c, doc->docIn);
                    TY_(ReportAttrError)( doc, lexer->token, NULL, UNEXPECTED_GT );
                    return 0;
                }
                TY_(AddCharToLexer)(lexer, c);
            }
            while (c != '\'');
        }
    }

    return delim;
}

/* values start with "=" or " = " etc. */
/* doesn't consume the ">" at end of start tag */

static tmbstr ParseValue( TidyDocImpl* doc, ctmbstr name,
                          Bool foldCase, Bool *isempty, int *pdelim)
{
    Lexer* lexer = doc->lexer;
    int len = 0, start;
    Bool seen_gt = no;
    Bool munge = yes;
    uint c, lastc, delim, quotewarning;
    tmbstr value;

    delim = (tmbchar) 0;
    *pdelim = '"';

    /*
     Henry Zrepa reports that some folk are using the
     embed element with script attributes where newlines
     are significant and must be preserved
    */
    if ( cfgBool(doc, TidyLiteralAttribs) )
        munge = no;

 /* skip white space before the '=' */

    for (;;)
    {
        c = TY_(ReadChar)(doc->docIn);

        if (c == EndOfStream)
        {
            TY_(UngetChar)(c, doc->docIn);
            break;
        }

        if (!TY_(IsWhite)(c))
           break;
    }

/*
  c should be '=' if there is a value
  other legal possibilities are white
  space, '/' and '>'
*/

    if (c != '=' && c != '"' && c != '\'')
    {
        TY_(UngetChar)(c, doc->docIn);
        return NULL;
    }

 /* skip white space after '=' */

    for (;;)
    {
        c = TY_(ReadChar)(doc->docIn);

        if (c == EndOfStream)
        {
            TY_(UngetChar)(c, doc->docIn);
            break;
        }

        if (!TY_(IsWhite)(c))
           break;
    }

 /* check for quote marks */

    if (c == '"' || c == '\'')
        delim = c;
    else if (c == '<')
    {
        start = lexer->lexsize;
        TY_(AddCharToLexer)(lexer, c);
        *pdelim = ParseServerInstruction( doc );
        len = lexer->lexsize - start;
        lexer->lexsize = start;
        return (len > 0 ? TY_(tmbstrndup)(doc->allocator,
                                          lexer->lexbuf+start, len) : NULL);
    }
    else
        TY_(UngetChar)(c, doc->docIn);

 /*
   and read the value string
   check for quote mark if needed
 */

    quotewarning = 0;
    start = lexer->lexsize;
    c = '\0';

    for (;;)
    {
        lastc = c;  /* track last character */
        c = TY_(ReadChar)(doc->docIn);

        if (c == EndOfStream)
        {
            TY_(ReportAttrError)( doc, lexer->token, NULL, UNEXPECTED_END_OF_FILE_ATTR );
            TY_(UngetChar)(c, doc->docIn);
            break;
        }

        if (delim == (tmbchar)0)
        {
            if (c == '>')
            {
                TY_(UngetChar)(c, doc->docIn);
                break;
            }

            if (c == '"' || c == '\'')
            {
                uint q = c;

                /* handle <input onclick=s("btn1")> and <a title=foo""">...</a> */
                /* this doesn't handle <a title=foo"/> which browsers treat as  */
                /* 'foo"/' nor  <a title=foo" /> which browser treat as 'foo"'  */

                c = TY_(ReadChar)(doc->docIn);
                if (c == '>')
                {
                    TY_(AddCharToLexer)(lexer, q);
                    TY_(UngetChar)(c, doc->docIn);
                    break;
                }
                else
                {
                    TY_(UngetChar)(c, doc->docIn);
                    c = q;
                }
            }

            if (c == '<')
            {
                TY_(UngetChar)(c, doc->docIn);
                c = '>';
                TY_(UngetChar)(c, doc->docIn);
                TY_(ReportAttrError)( doc, lexer->token, NULL, UNEXPECTED_GT );
                break;
            }

            /*
             For cases like <br clear=all/> need to avoid treating /> as
             part of the attribute value, however care is needed to avoid
             so treating <a href=http://www.acme.com/> in this way, which
             would map the <a> tag to <a href="http://www.acme.com"/>
            */
            if (c == '/')
            {
                /* peek ahead in case of /> */
                c = TY_(ReadChar)(doc->docIn);

                if ( c == '>' && !TY_(IsUrl)(doc, name) )
                {
                    *isempty = yes;
                    TY_(UngetChar)(c, doc->docIn);
                    break;
                }

                /* unget peeked character */
                TY_(UngetChar)(c, doc->docIn);
                c = '/';
            }
        }
        else  /* delim is '\'' or '"' */
        {
            if (c == delim)
                break;

            if (c == '\n' || c == '<' || c == '>')
                ++quotewarning;

            if (c == '>')
                seen_gt = yes;
        }

        if (c == '&')
        {
            TY_(AddCharToLexer)(lexer, c);
            ParseEntity( doc, IgnoreWhitespace );
            if (lexer->lexbuf[lexer->lexsize - 1] == '\n' && munge)
                ChangeChar(lexer, ' ');
            continue;
        }

        /*
         kludge for JavaScript attribute values
         with line continuations in string literals
        */
        if (c == '\\')
        {
            c = TY_(ReadChar)(doc->docIn);

            if (c != '\n')
            {
                TY_(UngetChar)(c, doc->docIn);
                c = '\\';
            }
        }

        if (TY_(IsWhite)(c))
        {
            if ( delim == 0 )
                break;

            if (munge)
            {
                /* discard line breaks in quoted URLs */
                /* #438650 - fix by Randy Waki */
                if ( c == '\n' && TY_(IsUrl)(doc, name) )
                {
                    /* warn that we discard this newline */
                    TY_(ReportAttrError)( doc, lexer->token, NULL, NEWLINE_IN_URI);
                    continue;
                }

                c = ' ';

                if (lastc == ' ')
                {
                    if (TY_(IsUrl)(doc, name) )
                        TY_(ReportAttrError)( doc, lexer->token, NULL, WHITE_IN_URI);
                    continue;
                }
            }
        }
        else if (foldCase && TY_(IsUpper)(c))
            c = TY_(ToLower)(c);

        TY_(AddCharToLexer)(lexer, c);
    }

    if (quotewarning > 10 && seen_gt && munge)
    {
        /*
           there is almost certainly a missing trailing quote mark
           as we have see too many newlines, < or > characters.

           an exception is made for Javascript attributes and the
           javascript URL scheme which may legitimately include < and >,
           and for attributes starting with "<xml " as generated by
           Microsoft Office.
        */
        if ( !TY_(IsScript)(doc, name) &&
             !(TY_(IsUrl)(doc, name) && TY_(tmbstrncmp)(lexer->lexbuf+start, "javascript:", 11) == 0) &&
             !(TY_(tmbstrncmp)(lexer->lexbuf+start, "<xml ", 5) == 0)
           )
            TY_(Report)( doc, NULL, NULL, SUSPECTED_MISSING_QUOTE );
    }

    len = lexer->lexsize - start;
    lexer->lexsize = start;


    if (len > 0 || delim)
    {
        /* ignore leading and trailing white space for all but title, alt, value */
        /* and prompts attributes unless --literal-attributes is set to yes      */
        /* #994841 - Whitespace is removed from value attributes                 */

        /* Issue #217 - Also only if/while (len > 0) - MUST NEVER GO NEGATIVE! */
        if ((len > 0) && munge &&
            TY_(tmbstrcasecmp)(name, "alt") &&
            TY_(tmbstrcasecmp)(name, "title") &&
            TY_(tmbstrcasecmp)(name, "value") &&
            TY_(tmbstrcasecmp)(name, "prompt"))
        {
            while (TY_(IsWhite)(lexer->lexbuf[start+len-1]) && (len > 0))
                --len;

            /* Issue #497 - Fix leading space trimming */
            while (TY_(IsWhite)(lexer->lexbuf[start]) && (len > 0))
            {
                ++start;
                --len;
            }
        }

        value = TY_(tmbstrndup)(doc->allocator, lexer->lexbuf + start, len);
    }
    else
        value = NULL;

    /* note delimiter if given */
    *pdelim = delim;

    return value;
}

/* attr must be non-NULL */
static Bool IsValidAttrName( ctmbstr attr )
{
    uint i, c = attr[0];

    /* first character should be a letter */
    if (!TY_(IsLetter)(c))
        return no;

    /* remaining characters should be namechars */
    for( i = 1; i < TY_(tmbstrlen)(attr); i++)
    {
        c = attr[i];

        if (TY_(IsNamechar)(c))
            continue;

        return no;
    }

    return yes;
}

/* create a new attribute */
AttVal *TY_(NewAttribute)( TidyDocImpl* doc )
{
    AttVal *av = (AttVal*) TidyDocAlloc( doc, sizeof(AttVal) );
    TidyClearMemory( av, sizeof(AttVal) );
    return av;
}

/* create a new attribute with given name and value */
AttVal* TY_(NewAttributeEx)( TidyDocImpl* doc, ctmbstr name, ctmbstr value,
                             int delim )
{
    AttVal *av = TY_(NewAttribute)(doc);
    av->attribute = TY_(tmbstrdup)(doc->allocator, name);
    av->value = TY_(tmbstrdup)(doc->allocator, value);
    av->delim = delim;
    av->dict = TY_(FindAttribute)( doc, av );
    return av;
}

static void AddAttrToList( AttVal** list, AttVal* av )
{
  if ( *list == NULL )
    *list = av;
  else
  {
    AttVal* here = *list;
    while ( here->next )
      here = here->next;
    here->next = av;
  }
}

void TY_(InsertAttributeAtEnd)( Node *node, AttVal *av )
{
    AddAttrToList(&node->attributes, av);
}

void TY_(InsertAttributeAtStart)( Node *node, AttVal *av )
{
    av->next = node->attributes;
    node->attributes = av;
}

/* swallows closing '>' */

static AttVal* ParseAttrs( TidyDocImpl* doc, Bool *isempty )
{
    Lexer* lexer = doc->lexer;
    AttVal *av, *list;
    tmbstr value;
    int delim;
    Node *asp, *php;

    list = NULL;

    while ( !EndOfInput(doc) )
    {
        tmbstr attribute = ParseAttribute( doc, isempty, &asp, &php );

        if (attribute == NULL)
        {
            /* check if attributes are created by ASP markup */
            if (asp)
            {
                av = TY_(NewAttribute)(doc);
                av->asp = asp;
                AddAttrToList( &list, av );
                continue;
            }

            /* check if attributes are created by PHP markup */
            if (php)
            {
                av = TY_(NewAttribute)(doc);
                av->php = php;
                AddAttrToList( &list, av );
                continue;
            }

            break;
        }

        value = ParseValue( doc, attribute, no, isempty, &delim );

        if (attribute && (IsValidAttrName(attribute) ||
            (cfgBool(doc, TidyXmlTags) && IsValidXMLAttrName(attribute))))
        {
            av = TY_(NewAttribute)(doc);
            av->delim = delim ? delim : '"';
            av->attribute = attribute;
            av->value = value;
            av->dict = TY_(FindAttribute)( doc, av );
            AddAttrToList( &list, av );
            if ( !delim && value )
                TY_(ReportAttrError)( doc, lexer->token, av, MISSING_QUOTEMARK_OPEN);
        }
        else
        {
            av = TY_(NewAttribute)(doc);
            av->attribute = attribute;
            av->value = value;

            if (LastChar(attribute) == '"')
                TY_(ReportAttrError)( doc, lexer->token, av, MISSING_QUOTEMARK);
            else if (value == NULL)
                TY_(ReportAttrError)(doc, lexer->token, av, MISSING_ATTR_VALUE);
            else
                TY_(ReportAttrError)(doc, lexer->token, av, INVALID_ATTRIBUTE);

            TY_(FreeAttribute)( doc, av );
        }
    }

    return list;
}

/*
  Returns document type declarations like

  <!DOCTYPE foo PUBLIC "fpi" "sysid">
  <!DOCTYPE bar SYSTEM "sysid">
  <!DOCTYPE baz [ <!ENTITY ouml "&#246"> ]>

  as

  <foo PUBLIC="fpi" SYSTEM="sysid" />
  <bar SYSTEM="sysid" />
  <baz> &lt;!ENTITY ouml &quot;&amp;#246&quot;&gt; </baz>
*/
static Node *ParseDocTypeDecl(TidyDocImpl* doc)
{
    Lexer *lexer = doc->lexer;
    int start = lexer->lexsize;
    ParseDocTypeDeclState state = DT_DOCTYPENAME;
    uint c;
    uint delim = 0;
    Bool hasfpi = yes;

    Node* node = TY_(NewNode)(lexer->allocator, lexer);
    node->type = DocTypeTag;
    node->start = lexer->txtstart;
    node->end = lexer->txtend;

    lexer->waswhite = no;

    /* todo: reset lexer->lexsize when appropriate to avoid wasting memory */

    while ((c = TY_(ReadChar)(doc->docIn)) != EndOfStream)
    {
        /* convert newlines to spaces */
        if (state != DT_INTSUBSET)
            c = c == '\n' ? ' ' : c;

        /* convert white-space sequences to single space character */
        if (TY_(IsWhite)(c) && state != DT_INTSUBSET)
        {
            if (!lexer->waswhite)
            {
                TY_(AddCharToLexer)(lexer, c);
                lexer->waswhite = yes;
            }
            else
            {
                /* discard space */
                continue;
            }
        }
        else
        {
            TY_(AddCharToLexer)(lexer, c);
            lexer->waswhite = no;
        }

        switch(state)
        {
        case DT_INTERMEDIATE:
            /* determine what's next */
            if (TY_(ToUpper)(c) == 'P' || TY_(ToUpper)(c) == 'S')
            {
                start = lexer->lexsize - 1;
                state = DT_PUBLICSYSTEM;
                continue;
            }
            else if (c == '[')
            {
                start = lexer->lexsize;
                state = DT_INTSUBSET;
                continue;
            }
            else if (c == '\'' || c == '"')
            {
                start = lexer->lexsize;
                delim = c;
                state = DT_QUOTEDSTRING;
                continue;
            }
            else if (c == '>')
            {
                AttVal* si;

                node->end = --(lexer->lexsize);

                si = TY_(GetAttrByName)(node, "SYSTEM");
                if (si)
                    TY_(CheckUrl)(doc, node, si);

                if (!node->element || !IsValidXMLElemName(node->element))
                {
                    TY_(Report)(doc, NULL, NULL, MALFORMED_DOCTYPE);
                    TY_(FreeNode)(doc, node);
                    return NULL;
                }
                return node;
            }
            else
            {
                /* error */
            }
            break;
        case DT_DOCTYPENAME:
            /* read document type name */
            if (TY_(IsWhite)(c) || c == '>' || c == '[')
            {
                node->element = TY_(tmbstrndup)(doc->allocator,
                                                lexer->lexbuf + start,
                                                lexer->lexsize - start - 1);
                if (c == '>' || c == '[')
                {
                    --(lexer->lexsize);
                    TY_(UngetChar)(c, doc->docIn);
                }

                state = DT_INTERMEDIATE;
                continue;
            }
            break;
        case DT_PUBLICSYSTEM:
            /* read PUBLIC/SYSTEM */
            if (TY_(IsWhite)(c) || c == '>')
            {
                char *attname = TY_(tmbstrndup)(doc->allocator,
                                                lexer->lexbuf + start,
                                                lexer->lexsize - start - 1);
                hasfpi = !(TY_(tmbstrcasecmp)(attname, "SYSTEM") == 0);

                TidyDocFree(doc, attname);

                /* todo: report an error if SYSTEM/PUBLIC not uppercase */

                if (c == '>')
                {
                    --(lexer->lexsize);
                    TY_(UngetChar)(c, doc->docIn);
                }

                state = DT_INTERMEDIATE;
                continue;
            }
            break;
        case DT_QUOTEDSTRING:
            /* read quoted string */
            if (c == delim)
            {
                char *value = TY_(tmbstrndup)(doc->allocator,
                                              lexer->lexbuf + start,
                                              lexer->lexsize - start - 1);
                AttVal* att = TY_(AddAttribute)(doc, node, hasfpi ? "PUBLIC" : "SYSTEM", value);
                TidyDocFree(doc, value);
                att->delim = delim;
                hasfpi = no;
                state = DT_INTERMEDIATE;
                delim = 0;
                continue;
            }
            break;
        case DT_INTSUBSET:
            /* read internal subset */
            if (c == ']')
            {
                Node* subset;
                lexer->txtstart = start;
                lexer->txtend = lexer->lexsize - 1;
                subset = TY_(TextToken)(lexer);
                TY_(InsertNodeAtEnd)(node, subset);
                state = DT_INTERMEDIATE;
            }
            break;
        }
    }

    /* document type declaration not finished */
    TY_(Report)(doc, NULL, NULL, MALFORMED_DOCTYPE);
    TY_(FreeNode)(doc, node);
    return NULL;
}


/****************************************************************************//*
 ** MARK: - Node Stack
 ***************************************************************************/


/**
 * Create a new stack with a given starting capacity. If memory allocation
 * fails, then the allocator will panic the program automatically.
 */
Stack* TY_(newStack)(TidyDocImpl *doc, uint capacity)
{
    Stack *stack = (Stack *)TidyAlloc(doc->allocator, sizeof(Stack));
    stack->top = -1;
    stack->capacity = capacity;
    stack->firstNode = (Node **)TidyAlloc(doc->allocator, stack->capacity * sizeof(Node**));
    stack->allocator = doc->allocator;
    return stack;
}


/**
 *  Increase the stack size. This will be called automatically when the
 *  current stack is full. If memory allocation fails, then the allocator
 *  will panic the program automatically.
 */
void TY_(growStack)(Stack *stack)
{
    uint new_capacity = stack->capacity * 2;

    Node **firstNode = (Node **)TidyAlloc(stack->allocator, new_capacity * sizeof(Node**));

    memcpy( firstNode, stack->firstNode, sizeof(Node**) * (stack->top + 1) );
    TidyFree(stack->allocator, stack->firstNode);

    stack->firstNode = firstNode;
    stack->capacity = new_capacity;
}


/**
 * Stack is full when top is equal to the last index.
 */
Bool TY_(stackFull)(Stack *stack)
{
    return stack->top == stack->capacity - 1;
}


/**
 * Stack is empty when top is equal to -1
 */
Bool TY_(stackEmpty)(Stack *stack)
{
    return stack->top == -1;
}


/**
 * Push an item to the stack.
 */
void TY_(push)(Stack *stack, Node *node)
{
    if (TY_(stackFull)(stack))
        TY_(growStack)(stack);

    if (node)
        stack->firstNode[++stack->top] = node;
}


/**
 * Pop an item from the stack.
 */
Node* TY_(pop)(Stack *stack)
{
    return TY_(stackEmpty)(stack) ? NULL : stack->firstNode[stack->top--];
}


/**
 * Peek at the stack.
 */
FUNC_UNUSED Node* TY_(peek)(Stack *stack)
{
    return TY_(stackEmpty)(stack) ? NULL : stack->firstNode[stack->top--];
}

/**
 *  Frees the stack when done.
 */
void TY_(freeStack)(Stack *stack)
{
    TidyFree( stack->allocator, stack->firstNode );
    stack->top = -1;
    stack->capacity = 0;
    stack->firstNode = NULL;
    stack->allocator = NULL;
}
