/*
  clean.c -- clean up misuse of presentation markup

  (c) 1998-2008 (W3C) MIT, ERCIM, Keio University
  See tidy.h for the copyright notice.

  Filters from other formats such as Microsoft Word
  often make excessive use of presentation markup such
  as font tags, B, I, and the align attribute. By applying
  a set of production rules, it is straight forward to
  transform this to use CSS.

  Some rules replace some of the children of an element by
  style properties on the element, e.g.

  <p><b>...</b></p> -> <p style="font-weight: bold">...</p>

  Such rules are applied to the element's content and then
  to the element itself until none of the rules more apply.
  Having applied all the rules to an element, it will have
  a style attribute with one or more properties.

  Other rules strip the element they apply to, replacing
  it by style properties on the contents, e.g.

  <dir><li><p>...</li></dir> -> <p style="margin-left 1em">...

  These rules are applied to an element before processing
  its content and replace the current element by the first
  element in the exposed content.

  After applying both sets of rules, you can replace the
  style attribute by a class value and style rule in the
  document head. To support this, an association of styles
  and class names is built.

  A naive approach is to rely on string matching to test
  when two property lists are the same. A better approach
  would be to first sort the properties before matching.

*/


#include "third_party/tidy/tidy-int.h"
#include "third_party/tidy/gdoc.h"
#include "third_party/tidy/lexer.h"
#include "third_party/tidy/parser.h"
#include "third_party/tidy/tags.h"
#include "third_party/tidy/attrs.h"
#include "third_party/tidy/message.h"
#include "third_party/tidy/tmbstr.h"
#include "third_party/tidy/utf8.h"

/*
  Extricate "element", replace it by its content and delete it.
*/
static void DiscardContainer( TidyDocImpl* doc, Node *element, Node **pnode)
{
    if (element->content)
    {
        Node *node, *parent = element->parent;

        element->last->next = element->next;

        if (element->next)
        {
            element->next->prev = element->last;
        }
        else
            parent->last = element->last;

        if (element->prev)
        {
            element->content->prev = element->prev;
            element->prev->next = element->content;
        }
        else
            parent->content = element->content;

        for (node = element->content; node; node = node->next)
            node->parent = parent;

        *pnode = element->content;

        element->next = element->content = NULL;
        TY_(FreeNode)(doc, element);
    }
    else
    {
        *pnode = TY_(DiscardElement)(doc, element);
    }
}

static void CleanNode( TidyDocImpl* doc, Node *node )
{
    Stack *stack = TY_(newStack)(doc, 16);
    Node *child, *next;

    if ( (child = node->content) )
    {
        while (child)
        {
            next = child->next;

            if (TY_(nodeIsElement)(child))
            {
                if (nodeIsSTYLE(child))
                    TY_(DiscardElement)(doc, child);
                if (nodeIsP(child) && !child->content)
                    TY_(DiscardElement)(doc, child);
                else if (nodeIsSPAN(child))
                    DiscardContainer( doc, child, &next);
                else if (nodeIsA(child) && !child->content)
                 {
                    AttVal *id = TY_(GetAttrByName)( child, "name" );
                    /* Recent Google Docs is using "id" instead of "name" in
                    ** the exported html.
                    */
                    if (!id)
                        id = TY_(GetAttrByName)( child, "id" );

                    if (id)
                        TY_(RepairAttrValue)( doc, child->parent, "id", id->value );

                    TY_(DiscardElement)(doc, child);
                }
                else
                {
                    if (child->attributes)
                        TY_(DropAttrByName)( doc, child, "class" );

                    TY_(push)(stack,next);
                    child = child->content;
                    continue;
                }
            }
            child = next ? next : TY_(pop)(stack);
        }
        TY_(freeStack)(stack);
    }
}

/* insert meta element to force browser to recognize doc as UTF8 */
static void SetUTF8( TidyDocImpl* doc )
{
    Node *head = TY_(FindHEAD)( doc );

    if (head)
    {
        Node *node = TY_(InferredTag)(doc, TidyTag_META);
        TY_(AddAttribute)( doc, node, "http-equiv", "Content-Type" );
        TY_(AddAttribute)( doc, node, "content", "text/html; charset=UTF-8" );
        TY_(InsertNodeAtStart)( head, node );
    }
}

/* clean html exported by Google Docs

    - strip the script element, as the style sheet is a mess
    - strip class attributes
    - strip span elements, leaving their content in place
    - replace <a name=...></a> by id on parent element
    - strip empty <p> elements
*/
void TY_(CleanGoogleDocument)( TidyDocImpl* doc )
{
    /* placeholder.  CleanTree()/CleanNode() will not
    ** zap root element
    */
    CleanNode( doc, &doc->root );
    SetUTF8( doc );
}

/*
 * local variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 4
 * end:
 */
