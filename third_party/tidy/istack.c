/* istack.c -- inline stack for compatibility with Mosaic

  (c) 1998-2006 (W3C) MIT, ERCIM, Keio University
  See tidy.h for the copyright notice.

*/

#include "third_party/tidy/tidy-int.h"
#include "third_party/tidy/lexer.h"
#include "third_party/tidy/attrs.h"
#include "third_party/tidy/streamio.h"
#include "third_party/tidy/tmbstr.h"

/* duplicate attributes */
AttVal *TY_(DupAttrs)( TidyDocImpl* doc, AttVal *attrs)
{
    AttVal *newattrs;

    if (attrs == NULL)
        return attrs;

    newattrs = TY_(NewAttribute)(doc);
    *newattrs = *attrs;
    newattrs->next = TY_(DupAttrs)( doc, attrs->next );
    newattrs->attribute = TY_(tmbstrdup)(doc->allocator, attrs->attribute);
    newattrs->value = TY_(tmbstrdup)(doc->allocator, attrs->value);
    newattrs->dict = TY_(FindAttribute)(doc, newattrs);
    newattrs->asp = attrs->asp ? TY_(CloneNode)(doc, attrs->asp) : NULL;
    newattrs->php = attrs->php ? TY_(CloneNode)(doc, attrs->php) : NULL;
    return newattrs;
}

static Bool IsNodePushable( Node *node )
{
    if (node->tag == NULL)
        return no;

    if (!(node->tag->model & CM_INLINE))
        return no;

    if (node->tag->model & CM_OBJECT)
        return no;

    /*\ Issue #92: OLD problem of ins and del which are marked as both
     *  inline and block, thus should NOT ever be 'inserted'
    \*/
    if (nodeIsINS(node) || nodeIsDEL(node))
        return no;

    return yes;
}

/*
  push a copy of an inline node onto stack
  but don't push if implicit or OBJECT or APPLET
  (implicit tags are ones generated from the istack)

  One issue arises with pushing inlines when
  the tag is already pushed. For instance:

      <p><em>text
      <p><em>more text

  Shouldn't be mapped to

      <p><em>text</em></p>
      <p><em><em>more text</em></em>
*/
void TY_(PushInline)( TidyDocImpl* doc, Node *node )
{
    Lexer* lexer = doc->lexer;
    IStack *istack;

    if (node->implicit)
        return;

    if ( !IsNodePushable(node) )
        return;

    if ( !nodeIsFONT(node) && TY_(IsPushed)(doc, node) )
        return;

    /* make sure there is enough space for the stack */
    if (lexer->istacksize + 1 > lexer->istacklength)
    {
        if (lexer->istacklength == 0)
            lexer->istacklength = 6;   /* this is perhaps excessive */

        lexer->istacklength = lexer->istacklength * 2;
        lexer->istack = (IStack *)TidyDocRealloc(doc, lexer->istack,
                            sizeof(IStack)*(lexer->istacklength));
    }

    istack = &(lexer->istack[lexer->istacksize]);
    istack->tag = node->tag;

    istack->element = TY_(tmbstrdup)(doc->allocator, node->element);
    istack->attributes = TY_(DupAttrs)( doc, node->attributes );
    ++(lexer->istacksize);
}

static void PopIStack( TidyDocImpl* doc )
{
    Lexer* lexer = doc->lexer;
    IStack *istack;
    AttVal *av;

    --(lexer->istacksize);
    istack = &(lexer->istack[lexer->istacksize]);

    while (istack->attributes)
    {
        av = istack->attributes;
        istack->attributes = av->next;
        TY_(FreeAttribute)( doc, av );
    }
    TidyDocFree(doc, istack->element);
    istack->element = NULL; /* remove the freed element */
}

static void PopIStackUntil( TidyDocImpl* doc, TidyTagId tid )
{
    Lexer* lexer = doc->lexer;
    IStack *istack;

    while (lexer->istacksize > 0)
    {
        PopIStack( doc );
        istack = &(lexer->istack[lexer->istacksize]);
        if ( istack->tag->id == tid )
            break;
    }
}

/* pop inline stack */
void TY_(PopInline)( TidyDocImpl* doc, Node *node )
{
    Lexer* lexer = doc->lexer;

    if (node)
    {
        if ( !IsNodePushable(node) )
            return;

        /* if node is </a> then pop until we find an <a> */
        if ( nodeIsA(node) )
        {
            PopIStackUntil( doc, TidyTag_A );
            return;
        }
    }

    if (lexer->istacksize > 0)
    {
        PopIStack( doc );

        /* #427822 - fix by Randy Waki 7 Aug 00 */
        if (lexer->insert >= lexer->istack + lexer->istacksize)
            lexer->insert = NULL;
    }
}

Bool TY_(IsPushed)( TidyDocImpl* doc, Node *node )
{
    Lexer* lexer = doc->lexer;
    int i;

    for (i = lexer->istacksize - 1; i >= 0; --i)
    {
        if (lexer->istack[i].tag == node->tag)
            return yes;
    }

    return no;
}

/*
   Test whether the last element on the stack has the same type than "node".
*/
Bool TY_(IsPushedLast)( TidyDocImpl* doc, Node *element, Node *node )
{
    Lexer* lexer = doc->lexer;

    if ( element && !IsNodePushable(element) )
        return no;

    if (lexer->istacksize > 0) {
        if (lexer->istack[lexer->istacksize - 1].tag == node->tag) {
            return yes;
        }
    }

    return no;
}

/*
  This has the effect of inserting "missing" inline
  elements around the contents of blocklevel elements
  such as P, TD, TH, DIV, PRE etc. This procedure is
  called at the start of ParseBlock. when the inline
  stack is not empty, as will be the case in:

    <i><h1>italic heading</h1></i>

  which is then treated as equivalent to

    <h1><i>italic heading</i></h1>

  This is implemented by setting the lexer into a mode
  where it gets tokens from the inline stack rather than
  from the input stream.
*/
int TY_(InlineDup)( TidyDocImpl* doc, Node* node )
{
    Lexer* lexer = doc->lexer;
    int n;

    if ((n = lexer->istacksize - lexer->istackbase) > 0)
    {
        lexer->insert = &(lexer->istack[lexer->istackbase]);
        lexer->inode = node;
    }

    return n;
}

/*
 defer duplicates when entering a table or other
 element where the inlines shouldn't be duplicated
*/
void TY_(DeferDup)( TidyDocImpl* doc )
{
    doc->lexer->insert = NULL;
    doc->lexer->inode = NULL;
}

Node *TY_(InsertedToken)( TidyDocImpl* doc )
{
    Lexer* lexer = doc->lexer;
    Node *node;
    IStack *istack;
    uint n;

    /* this will only be NULL if inode != NULL */
    if (lexer->insert == NULL)
    {
        node = lexer->inode;
        lexer->inode = NULL;
        return node;
    }

    /*
      If this is the "latest" node then update
      the position, otherwise use current values
    */

    if (lexer->inode == NULL)
    {
        lexer->lines = doc->docIn->curline;
        lexer->columns = doc->docIn->curcol;
    }

    node = TY_(NewNode)(doc->allocator, lexer);
    node->type = StartTag;
    node->implicit = yes;
    node->start = lexer->txtstart;
    /* #431734 [JTidy bug #226261 (was 126261)] - fix by Gary Peskin 20 Dec 00 */
    node->end = lexer->txtend; /* was : lexer->txtstart; */
    istack = lexer->insert;

/* #if 0 && defined(_DEBUG) */
#if definedENABLE_DEBUG_LOG
    if ( lexer->istacksize == 0 )
    {
        SPRTF( "WARNING: ZERO sized istack!\n" );
    }
#endif

    node->element = TY_(tmbstrdup)(doc->allocator, istack->element);
    node->tag = istack->tag;
    node->attributes = TY_(DupAttrs)( doc, istack->attributes );

    /* advance lexer to next item on the stack */
    n = (uint)(lexer->insert - &(lexer->istack[0]));

    /* and recover state if we have reached the end */
    if (++n < lexer->istacksize)
        lexer->insert = &(lexer->istack[n]);
    else
        lexer->insert = NULL;

    return node;
}


/*
   We have two CM_INLINE elements pushed ... the first is closing,
   but, like the browser, the second should be retained ...
   Like <b>bold <i>bold and italics</b> italics only</i>
   This function switches the tag positions on the stack,
   returning 'yes' if both were found in the expected order.
*/
Bool TY_(SwitchInline)( TidyDocImpl* doc, Node* element, Node* node )
{
    Lexer* lexer = doc->lexer;
    if ( lexer
         && element && element->tag
         && node && node->tag
         && TY_(IsPushed)( doc, element )
         && TY_(IsPushed)( doc, node )
         && ((lexer->istacksize - lexer->istackbase) >= 2) )
    {
        /* we have a chance of succeeding ... */
        int i;
        for (i = (lexer->istacksize - lexer->istackbase - 1); i >= 0; --i)
        {
            if (lexer->istack[i].tag == element->tag) {
                /* found the element tag - phew */
                IStack *istack1 = &lexer->istack[i];
                IStack *istack2 = NULL;
                --i; /* back one more, and continue */
                for ( ; i >= 0; --i)
                {
                    if (lexer->istack[i].tag == node->tag)
                    {
                        /* found the element tag - phew */
                        istack2 = &lexer->istack[i];
                        break;
                    }
                }
                if ( istack2 )
                {
                    /* perform the swap */
                    IStack tmp_istack = *istack2;
                    *istack2 = *istack1;
                    *istack1 = tmp_istack;
                    return yes;
                }
            }
        }
    }
    return no;
}

/*
  We want to push a specific a specific element on the stack,
  but it may not be the last element, which InlineDup()
  would handle. Return yes, if found and inserted.
*/
Bool TY_(InlineDup1)( TidyDocImpl* doc, Node* node, Node* element )
{
    Lexer* lexer = doc->lexer;
    int n, i;
    if ( element
         && (element->tag != NULL)
         && ((n = lexer->istacksize - lexer->istackbase) > 0) )
    {
        for ( i = n - 1; i >=0; --i ) {
            if (lexer->istack[i].tag == element->tag) {
                /* found our element tag - insert it */
                lexer->insert = &(lexer->istack[i]);
                lexer->inode = node;
                return yes;
            }
        }
    }
    return no;
}

/*
 * local variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 4
 * end:
 */
