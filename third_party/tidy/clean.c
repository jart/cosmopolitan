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
#include "third_party/tidy/clean.h"
#include "third_party/tidy/lexer.h"
#include "third_party/tidy/parser.h"
#include "third_party/tidy/attrs.h"
#include "third_party/tidy/message.h"
#include "third_party/tidy/tmbstr.h"
#include "libc/assert.h"
#include "third_party/tidy/utf8.h"

static Node* CleanNode( TidyDocImpl* doc, Node *node );

static void RenameElem( TidyDocImpl* doc, Node* node, TidyTagId tid )
{
    const Dict* dict = TY_(LookupTagDef)( tid );
    TidyDocFree( doc, node->element );
    node->element = TY_(tmbstrdup)( doc->allocator, dict->name );
    node->tag = dict;
}

static void FreeStyleProps(TidyDocImpl* doc, StyleProp *props)
{
    StyleProp *next;

    while (props)
    {
        next = props->next;
        TidyDocFree(doc, props->name);
        TidyDocFree(doc, props->value);
        TidyDocFree(doc, props);
        props = next;
    }
}

static StyleProp *InsertProperty( TidyDocImpl* doc, StyleProp* props, ctmbstr name, ctmbstr value )
{
    StyleProp *first, *prev, *prop;
    int cmp;

    prev = NULL;
    first = props;

    while (props)
    {
        cmp = TY_(tmbstrcmp)(props->name, name);

        if (cmp == 0)
        {
            /* this property is already defined, ignore new value */
            return first;
        }

        if (cmp > 0)
        {
            /* insert before this */

            prop = (StyleProp *)TidyDocAlloc(doc, sizeof(StyleProp));
            prop->name = TY_(tmbstrdup)(doc->allocator, name);
            prop->value = TY_(tmbstrdup)(doc->allocator, value);
            prop->next = props;

            if (prev)
                prev->next = prop;
            else
                first = prop;

            return first;
        }

        prev = props;
        props = props->next;
    }

    prop = (StyleProp *)TidyDocAlloc(doc, sizeof(StyleProp));
    prop->name = TY_(tmbstrdup)(doc->allocator, name);
    prop->value = TY_(tmbstrdup)(doc->allocator, value);
    prop->next = NULL;

    if (prev)
        prev->next = prop;
    else
        first = prop;

    return first;
}

/*
 Create sorted linked list of properties from style string
 It temporarily places nulls in place of ':' and ';' to
 delimit the strings for the property name and value.
 Some systems don't allow you to NULL literal strings,
 so to avoid this, a copy is made first.
*/
static StyleProp* CreateProps( TidyDocImpl* doc, StyleProp* prop, ctmbstr style )
{
    tmbstr name, value = NULL, name_end, value_end, line;
    Bool more;

    line = TY_(tmbstrdup)(doc->allocator, style);
    name = line;

    while (*name)
    {
        while (*name == ' ')
            ++name;

        name_end = name;

        while (*name_end)
        {
            if (*name_end == ':')
            {
                value = name_end + 1;
                break;
            }

            ++name_end;
        }

        if (*name_end != ':')
            break;

        while ( value && *value == ' ')
            ++value;

        value_end = value;
        more = no;

        while (*value_end)
        {
            if (*value_end == ';')
            {
                more = yes;
                break;
            }

            ++value_end;
        }

        *name_end = '\0';
        *value_end = '\0';

        prop = InsertProperty(doc, prop, name, value);
        *name_end = ':';

        if (more)
        {
            *value_end = ';';
            name = value_end + 1;
            continue;
        }

        break;
    }

    TidyDocFree(doc, line);  /* free temporary copy */
    return prop;
}

static tmbstr CreatePropString(TidyDocImpl* doc, StyleProp *props)
{
    tmbstr style, p, s;
    uint len;
    StyleProp *prop;

    /* compute length */

    for (len = 0, prop = props; prop; prop = prop->next)
    {
        len += TY_(tmbstrlen)(prop->name) + 2;
        if (prop->value)
            len += TY_(tmbstrlen)(prop->value) + 2;
    }

    style = (tmbstr) TidyDocAlloc(doc, len+1);
    style[0] = '\0';

    for (p = style, prop = props; prop; prop = prop->next)
    {
        s = prop->name;

        while((*p++ = *s++))
            continue;

        if (prop->value)
        {
            *--p = ':';
            *++p = ' ';
            ++p;

            s = prop->value;
            while((*p++ = *s++))
                continue;
        }
        if (prop->next == NULL)
            break;

        *--p = ';';
        *++p = ' ';
        ++p;
    }

    return style;
}

/*
  create string with merged properties
static tmbstr AddProperty( ctmbstr style, ctmbstr property )
{
    tmbstr line;
    StyleProp *prop;

    prop = CreateProps(doc, NULL, style);
    prop = CreateProps(doc, prop, property);
    line = CreatePropString(doc, prop);
    FreeStyleProps(doc, prop);
    return line;
}
*/

void TY_(FreeStyles)( TidyDocImpl* doc )
{
    Lexer* lexer = doc->lexer;
    if ( lexer )
    {
        TagStyle *style, *next;
        for ( style = lexer->styles; style; style = next )
        {
            next = style->next;
            TidyDocFree( doc, style->tag );
            TidyDocFree( doc, style->tag_class );
            TidyDocFree( doc, style->properties );
            TidyDocFree( doc, style );
        }
    }
}

static tmbstr GensymClass( TidyDocImpl* doc )
{
    tmbchar buf[512];  /* CSSPrefix is limited to 256 characters */
    ctmbstr pfx = cfgStr(doc, TidyCSSPrefix);
    if ( pfx == NULL || *pfx == 0 )
      pfx = "c";

    TY_(tmbsnprintf)(buf, sizeof(buf), "%s%u", pfx, ++doc->nClassId );
    return TY_(tmbstrdup)(doc->allocator, buf);
}

static ctmbstr FindStyle( TidyDocImpl* doc, ctmbstr tag, ctmbstr properties )
{
    Lexer* lexer = doc->lexer;
    TagStyle* style;

    for (style = lexer->styles; style; style=style->next)
    {
        if (TY_(tmbstrcmp)(style->tag, tag) == 0 &&
            TY_(tmbstrcmp)(style->properties, properties) == 0)
            return style->tag_class;
    }

    style = (TagStyle *)TidyDocAlloc( doc, sizeof(TagStyle) );
    style->tag = TY_(tmbstrdup)(doc->allocator, tag);
    style->tag_class = GensymClass( doc );
    style->properties = TY_(tmbstrdup)( doc->allocator, properties );
    style->next = lexer->styles;
    lexer->styles = style;
    return style->tag_class;
}

/*
 Add class="foo" to node
*/
static void AddClass( TidyDocImpl* doc, Node* node, ctmbstr classname )
{
    AttVal *classattr = TY_(AttrGetById)(node, TidyAttr_CLASS);;

    /*
     if there already is a class attribute
     then append class name after a space.
    */
    if (classattr)
        TY_(AppendToClassAttr)( doc, classattr, classname );
    else /* create new class attribute */
        TY_(AddAttribute)( doc, node, "class", classname );
}

void TY_(AddStyleAsClass)( TidyDocImpl* doc, Node *node, ctmbstr stylevalue )
{
    ctmbstr classname;

    classname = FindStyle( doc, node->element, stylevalue );
    AddClass( doc, node, classname);
}

/*
 Find style attribute in node, and replace it
 by corresponding class attribute. Search for
 class in style dictionary otherwise gensym
 new class and add to dictionary.

 Assumes that node doesn't have a class attribute
*/
static void Style2Rule( TidyDocImpl* doc, Node *node)
{
    AttVal *styleattr, *classattr;
    ctmbstr classname;

    styleattr = TY_(AttrGetById)(node, TidyAttr_STYLE);

    if (styleattr)
    {
        /* fix for http://tidy.sf.net/bug/850215 */
        if (!styleattr->value)
        {
            TY_(RemoveAttribute)(doc, node, styleattr);
            return;
        }

        classname = FindStyle( doc, node->element, styleattr->value );
        classattr = TY_(AttrGetById)(node, TidyAttr_CLASS);

        /*
         if there already is a class attribute
         then append class name after an underscore
        */
        if (classattr)
        {
            TY_(AppendToClassAttr)( doc, classattr, classname );
            TY_(RemoveAttribute)( doc, node, styleattr );
        }
        else /* reuse style attribute for class attribute */
        {
            TidyDocFree(doc, styleattr->attribute);
            TidyDocFree(doc, styleattr->value);
            styleattr->attribute = TY_(tmbstrdup)(doc->allocator, "class");
            styleattr->value = TY_(tmbstrdup)(doc->allocator, classname);
        }
    }
}

static void AddColorRule( Lexer* lexer, ctmbstr selector, ctmbstr color )
{
    if ( selector && color )
    {
        TY_(AddStringLiteral)(lexer, selector);
        TY_(AddStringLiteral)(lexer, " { color: ");
        TY_(AddStringLiteral)(lexer, color);
        TY_(AddStringLiteral)(lexer, " }\n");
    }
}

/*
 move presentation attribs from body to style element

 background="foo" ->  body { background-image: url(foo) }
 bgcolor="foo"    ->  body { background-color: foo }
 text="foo"       ->  body { color: foo }
 link="foo"       ->  :link { color: foo }
 vlink="foo"      ->  :visited { color: foo }
 alink="foo"      ->  :active { color: foo }
*/
static void CleanBodyAttrs( TidyDocImpl* doc, Node* body )
{
    Lexer* lexer  = doc->lexer;
    tmbstr bgurl   = NULL;
    tmbstr bgcolor = NULL;
    tmbstr color   = NULL;
    AttVal* attr;

    if (NULL != (attr = TY_(AttrGetById)(body, TidyAttr_BACKGROUND)))
    {
        bgurl = attr->value;
        attr->value = NULL;
        TY_(RemoveAttribute)( doc, body, attr );
    }

    if (NULL != (attr = TY_(AttrGetById)(body, TidyAttr_BGCOLOR)))
    {
        bgcolor = attr->value;
        attr->value = NULL;
        TY_(RemoveAttribute)( doc, body, attr );
    }

    if (NULL != (attr = TY_(AttrGetById)(body, TidyAttr_TEXT)))
    {
        color = attr->value;
        attr->value = NULL;
        TY_(RemoveAttribute)( doc, body, attr );
    }

    if ( bgurl || bgcolor || color )
    {
        TY_(AddStringLiteral)(lexer, " body {\n");
        if (bgurl)
        {
            TY_(AddStringLiteral)(lexer, "  background-image: url(");
            TY_(AddStringLiteral)(lexer, bgurl);
            TY_(AddStringLiteral)(lexer, ");\n");
            TidyDocFree(doc, bgurl);
        }
        if (bgcolor)
        {
            TY_(AddStringLiteral)(lexer, "  background-color: ");
            TY_(AddStringLiteral)(lexer, bgcolor);
            TY_(AddStringLiteral)(lexer, ";\n");
            TidyDocFree(doc, bgcolor);
        }
        if (color)
        {
            TY_(AddStringLiteral)(lexer, "  color: ");
            TY_(AddStringLiteral)(lexer, color);
            TY_(AddStringLiteral)(lexer, ";\n");
            TidyDocFree(doc, color);
        }

        TY_(AddStringLiteral)(lexer, " }\n");
    }

    if (NULL != (attr = TY_(AttrGetById)(body, TidyAttr_LINK)))
    {
        AddColorRule(lexer, " :link", attr->value);
        TY_(RemoveAttribute)( doc, body, attr );
    }

    if (NULL != (attr = TY_(AttrGetById)(body, TidyAttr_VLINK)))
    {
        AddColorRule(lexer, " :visited", attr->value);
        TY_(RemoveAttribute)( doc, body, attr );
    }

    if (NULL != (attr = TY_(AttrGetById)(body, TidyAttr_ALINK)))
    {
        AddColorRule(lexer, " :active", attr->value);
        TY_(RemoveAttribute)( doc, body, attr );
    }
}

static Bool NiceBody( TidyDocImpl* doc )
{
    Node* node = TY_(FindBody)(doc);
    if (node)
    {
        if (TY_(AttrGetById)(node, TidyAttr_BACKGROUND) ||
            TY_(AttrGetById)(node, TidyAttr_BGCOLOR)    ||
            TY_(AttrGetById)(node, TidyAttr_TEXT)       ||
            TY_(AttrGetById)(node, TidyAttr_LINK)       ||
            TY_(AttrGetById)(node, TidyAttr_VLINK)      ||
            TY_(AttrGetById)(node, TidyAttr_ALINK))
        {
            doc->badLayout |= USING_BODY;
            return no;
        }
    }

    return yes;
}

/* create style element using rules from dictionary */
static void CreateStyleElement( TidyDocImpl* doc )
{
    Lexer* lexer = doc->lexer;
    Node *node, *head, *body;
    TagStyle *style;
    AttVal *av;

    if ( lexer->styles == NULL && NiceBody(doc) )
        return;

    node = TY_(NewNode)( doc->allocator, lexer );
    node->type = StartTag;
    node->implicit = yes;
    node->element = TY_(tmbstrdup)(doc->allocator, "style");
    TY_(FindTag)( doc, node );

    /* insert type attribute */
    av = TY_(NewAttributeEx)( doc, "type", "text/css", '"' );
    TY_(InsertAttributeAtStart)( node, av );

    body = TY_(FindBody)( doc );
    lexer->txtstart = lexer->lexsize;
    if ( body )
        CleanBodyAttrs( doc, body );

    for (style = lexer->styles; style; style = style->next)
    {
        TY_(AddCharToLexer)(lexer, ' ');
        TY_(AddStringLiteral)(lexer, style->tag);
        TY_(AddCharToLexer)(lexer, '.');
        TY_(AddStringLiteral)(lexer, style->tag_class);
        TY_(AddCharToLexer)(lexer, ' ');
        TY_(AddCharToLexer)(lexer, '{');
        TY_(AddStringLiteral)(lexer, style->properties);
        TY_(AddCharToLexer)(lexer, '}');
        TY_(AddCharToLexer)(lexer, '\n');
    }

    lexer->txtend = lexer->lexsize;

    TY_(InsertNodeAtEnd)( node, TY_(TextToken)(lexer) );

    /*
     now insert style element into document head

     doc is root node. search its children for html node
     the head node should be first child of html node
    */
    if ( NULL != (head = TY_(FindHEAD)( doc )) )
        TY_(InsertNodeAtEnd)( head, node );
}


/* ensure bidirectional links are consistent */
void TY_(FixNodeLinks)(Node *node)
{
    Node *child;

    if (node->prev)
        node->prev->next = node;
    else
        node->parent->content = node;

    if (node->next)
        node->next->prev = node;
    else
        node->parent->last = node;

    for (child = node->content; child; child = child->next)
        child->parent = node;
}

/*
 used to strip child of node when
 the node has one and only one child
*/
static void StripOnlyChild(TidyDocImpl* doc, Node *node)
{
    Node *child;

    child = node->content;
    node->content = child->content;
    node->last = child->last;
    child->content = NULL;
    TY_(FreeNode)(doc, child);

    for (child = node->content; child; child = child->next)
        child->parent = node;
}

/*
  used to strip font start and end tags.
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

/*
  Create new string that consists of the
  combined style properties in s1 and s2

  To merge property lists, we build a linked
  list of property/values and insert properties
  into the list in order, merging values for
  the same property name.
*/
static tmbstr MergeProperties( TidyDocImpl* doc, ctmbstr s1, ctmbstr s2 )
{
    tmbstr s;
    StyleProp *prop;

    prop = CreateProps(doc, NULL, s1);
    prop = CreateProps(doc, prop, s2);
    s = CreatePropString(doc, prop);
    FreeStyleProps(doc, prop);
    return s;
}

/*
 Add style property to element, creating style
 attribute as needed and adding ; delimiter
*/
void TY_(AddStyleProperty)(TidyDocImpl* doc, Node *node, ctmbstr property )
{
    AttVal *av = TY_(AttrGetById)(node, TidyAttr_STYLE);

    /* if style attribute already exists then insert property */

    if ( av )
    {
        if (av->value != NULL)
        {
            tmbstr s = MergeProperties( doc, av->value, property );
            TidyDocFree( doc, av->value );
            av->value = s;
        }
        else
        {
            av->value = TY_(tmbstrdup)( doc->allocator, property );
        }
    }
    else /* else create new style attribute */
    {
        av = TY_(NewAttributeEx)( doc, "style", property, '"' );
        TY_(InsertAttributeAtStart)( node, av );
    }
}

static void MergeClasses(TidyDocImpl* doc, Node *node, Node *child)
{
    AttVal *av;
    tmbstr s1, s2, names;

    for (s2 = NULL, av = child->attributes; av; av = av->next)
    {
        if (attrIsCLASS(av))
        {
            s2 = av->value;
            break;
        }
    }

    for (s1 = NULL, av = node->attributes; av; av = av->next)
    {
        if (attrIsCLASS(av))
        {
            s1 = av->value;
            break;
        }
    }

    if (s1)
    {
        if (s2)  /* merge class names from both */
        {
            uint l1, l2;
            l1 = TY_(tmbstrlen)(s1);
            l2 = TY_(tmbstrlen)(s2);
            names = (tmbstr) TidyDocAlloc(doc, l1 + l2 + 2);
            TY_(tmbstrcpy)(names, s1);
            names[l1] = ' ';
            TY_(tmbstrcpy)(names+l1+1, s2);
            TidyDocFree(doc, av->value);
            av->value = names;
        }
    }
    else if (s2)  /* copy class names from child */
    {
        av = TY_(NewAttributeEx)( doc, "class", s2, '"' );
        TY_(InsertAttributeAtStart)( node, av );
    }
}

static void MergeStyles(TidyDocImpl* doc, Node *node, Node *child)
{
    AttVal *av;
    tmbstr s1, s2, style;

    /*
       the child may have a class attribute used
       for attaching styles, if so the class name
       needs to be copied to node's class
    */
    MergeClasses(doc, node, child);

    for (s2 = NULL, av = child->attributes; av; av = av->next)
    {
        if (attrIsSTYLE(av))
        {
            s2 = av->value;
            break;
        }
    }

    for (s1 = NULL, av = node->attributes; av; av = av->next)
    {
        if (attrIsSTYLE(av))
        {
            s1 = av->value;
            break;
        }
    }

    if (s1)
    {
        if (s2)  /* merge styles from both */
        {
            style = MergeProperties(doc, s1, s2);
            TidyDocFree(doc, av->value);
            av->value = style;
        }
    }
    else if (s2)  /* copy style of child */
    {
        av = TY_(NewAttributeEx)( doc, "style", s2, '"' );
        TY_(InsertAttributeAtStart)( node, av );
    }
}

static ctmbstr FontSize2Name(ctmbstr size)
{
    static const ctmbstr sizes[7] =
    {
        "60%", "70%", "80%", NULL,
        "120%", "150%", "200%"
    };

    /* increment of 0.8 */
    static const ctmbstr minussizes[] =
    {
        "100%", "80%", "64%", "51%",
        "40%", "32%", "26%"
    };

    /* increment of 1.2 */
    static const ctmbstr plussizes[] =
    {
        "100%", "120%", "144%", "172%",
        "207%", "248%", "298%"
    };

    if (size[0] == '\0')
        return NULL;

    if ('0' <= size[0] && size[0] <= '6')
    {
        int n = size[0] - '0';
        return sizes[n];
    }

    if (size[0] == '-')
    {
        if ('0' <= size[1] && size[1] <= '6')
        {
            int n = size[1] - '0';
            return minussizes[n];
        }
        return "smaller"; /*"70%"; */
    }

    if ('0' <= size[1] && size[1] <= '6')
    {
        int n = size[1] - '0';
        return plussizes[n];
    }

    return "larger"; /* "140%" */
}

static void AddFontFace( TidyDocImpl* doc, Node *node, ctmbstr face )
{
    tmbchar buf[256];
    TY_(tmbsnprintf)(buf, sizeof(buf), "font-family: %s", face );
    TY_(AddStyleProperty)( doc, node, buf );
}

static void AddFontSize( TidyDocImpl* doc, Node* node, ctmbstr size )
{
    ctmbstr value = NULL;

    if (nodeIsP(node))
    {
        if (TY_(tmbstrcmp)(size, "6") == 0)
            value = "h1";
        else if (TY_(tmbstrcmp)(size, "5") == 0)
            value = "h2";
        else if (TY_(tmbstrcmp)(size, "4") == 0)
            value = "h3";

        if (value)
        {
            TidyDocFree(doc, node->element);
            node->element = TY_(tmbstrdup)(doc->allocator, value);
            TY_(FindTag)(doc, node);
            return;
        }
    }

    value = FontSize2Name(size);

    if (value)
    {
        tmbchar buf[64];
        TY_(tmbsnprintf)(buf, sizeof(buf), "font-size: %s", value);
        TY_(AddStyleProperty)( doc, node, buf );
    }
}

static void AddFontColor( TidyDocImpl* doc, Node *node, ctmbstr color)
{
    tmbchar buf[128];
    TY_(tmbsnprintf)(buf, sizeof(buf), "color: %s", color);
    TY_(AddStyleProperty)( doc, node, buf );
}

/* force alignment value to lower case */
static void AddAlign( TidyDocImpl* doc, Node *node, ctmbstr align )
{
    uint i;
    tmbchar buf[128];

    TY_(tmbstrcpy)( buf, "text-align: " );
    for ( i = 12; i < sizeof(buf)/sizeof(buf[0])-1; ++i )
    {
        if ( (buf[i] = (tmbchar)TY_(ToLower)(*align++)) == '\0' )
            break;
    }
    buf[i] = '\0';
    TY_(AddStyleProperty)( doc, node, buf );
}

/*
 add style properties to node corresponding to
 the font face, size and color attributes
*/
static void AddFontStyles( TidyDocImpl* doc, Node *node, AttVal *av)
{
    while (av)
    {
        if (AttrHasValue(av))
        {
            if (attrIsFACE(av))
                AddFontFace( doc, node, av->value );
            else if (attrIsSIZE(av))
                AddFontSize( doc, node, av->value );
            else if (attrIsCOLOR(av))
                AddFontColor( doc, node, av->value );
        }
        av = av->next;
    }
}

/*
    Symptom: <p align=center>
    Action: <p style="text-align: center">
*/
static void TextAlign( TidyDocImpl* doc, Node* node )
{
    AttVal *av, *prev;

    prev = NULL;

    for (av = node->attributes; av; av = av->next)
    {
        if (attrIsALIGN(av))
        {
            if (prev)
                prev->next = av->next;
            else
                node->attributes = av->next;

            if (av->value)
                AddAlign( doc, node, av->value );

            TY_(FreeAttribute)(doc, av);
            break;
        }

        prev = av;
    }
}

/*
    Symptom: <table bgcolor="red">
    Action: <table style="background-color: red">
*/
static void TableBgColor( TidyDocImpl* doc, Node* node )
{
    AttVal* attr;
    tmbchar buf[256];

    if (NULL != (attr = TY_(AttrGetById)(node, TidyAttr_BGCOLOR)))
    {
        TY_(tmbsnprintf)(buf, sizeof(buf), "background-color: %s", attr->value );
        TY_(RemoveAttribute)( doc, node, attr );
        TY_(AddStyleProperty)( doc, node, buf );
    }
}

/*
   The clean up rules use the pnode argument to return the
   next node when the original node has been deleted
*/

/*
    Symptom: <dir> <li> where <li> is only child
    Action: coerce <dir> <li> to <div> with indent.
*/

static Bool Dir2Div( TidyDocImpl* doc, Node *node, Node **ARG_UNUSED(pnode))
{
    Node *child;

    if ( nodeIsDIR(node) || nodeIsUL(node) || nodeIsOL(node) )
    {
        child = node->content;

        if (child == NULL)
            return no;

        /* check child has no peers */

        if (child->next)
            return no;

        if ( !nodeIsLI(child) )
            return no;

        if ( !child->implicit )
            return no;

        /* coerce dir to div */
        node->tag = TY_(LookupTagDef)( TidyTag_DIV );
        TidyDocFree( doc, node->element );
        node->element = TY_(tmbstrdup)(doc->allocator, "div");
        TY_(AddStyleProperty)( doc, node, "margin-left: 2em" );
        StripOnlyChild( doc, node );
        return yes;
    }

    return no;
}

/*
    Symptom: <center>
    Action: replace <center> by <div style="text-align: center">
*/

static Bool Center2Div( TidyDocImpl* doc, Node *node, Node **pnode)
{
    if ( nodeIsCENTER(node) )
    {
        RenameElem( doc, node, TidyTag_DIV );
        TY_(AddStyleProperty)( doc, node, "text-align: center" );
        return yes;
    }

    return no;
}

/* Copy child attributes to node. Duplicate attributes are overwritten.
   Unique attributes (such as ID) disable the action.
   Attributes style and class are not dealt with. A call to MergeStyles
   will do that.
*/
static Bool CopyAttrs( TidyDocImpl* doc, Node *node, Node *child)
{
    AttVal *av1, *av2;
    TidyAttrId id;

    /* Detect attributes that cannot be merged or overwritten. */
    if (TY_(AttrGetById)(child, TidyAttr_ID) != NULL
        && TY_(AttrGetById)(node, TidyAttr_ID) != NULL)
        return no;

    /* Move child attributes to node. Attributes in node
     can be overwritten or merged. */
    for (av2 = child->attributes; av2; )
    {
        /* Dealt by MergeStyles. */
        if (attrIsSTYLE(av2) || attrIsCLASS(av2))
        {
            av2 = av2->next;
            continue;
        }
        /* Avoid duplicates in node */
        if ((id=AttrId(av2)) != TidyAttr_UNKNOWN
            && (av1=TY_(AttrGetById)(node, id))!= NULL)
            TY_(RemoveAttribute)( doc, node, av1 );

        /* Move attribute from child to node */
        TY_(DetachAttribute)( child, av2 );
        av1 = av2;
        av2 = av2->next;
        av1->next = NULL;
        TY_(InsertAttributeAtEnd)( node, av1 );
    }

    return yes;
}

/*
    Symptom <XX><XX>...</XX></XX>
    Action: merge the two XXs

  For instance, this is useful after nested <dir>s used by Word
  for indenting have been converted to <div>s

  If state is "no", no merging.
  If state is "yes", inner element is discarded. Only Style and Class
  attributes are merged using MergeStyles().
  If state is "auto", atttibutes are merged as described in CopyAttrs().
  Style and Class attributes are merged using MergeStyles().
*/
static Bool MergeNestedElements( TidyDocImpl* doc,
                                 TidyTagId Id, TidyTriState state, Node *node,
                                 Node **ARG_UNUSED(pnode))
{
    Node *child;

    if ( state == TidyNoState
         || !TagIsId(node, Id) )
        return no;

    child = node->content;

    if ( child == NULL
         || child->next != NULL
         || !TagIsId(child, Id) )
        return no;

    if ( state == TidyAutoState
         && CopyAttrs(doc, node, child) == no )
        return no;

    MergeStyles( doc, node, child );
    StripOnlyChild( doc, node );
    return yes;
}

/*
    Symptom: <ul><li><ul>...</ul></li></ul>
    Action: discard outer list
*/

static Bool NestedList( TidyDocImpl* doc, Node *node, Node **pnode )
{
    Node *child, *list;

    if ( nodeIsUL(node) || nodeIsOL(node) )
    {
        child = node->content;

        if (child == NULL)
            return no;

        /* check child has no peers */

        if (child->next)
            return no;

        list = child->content;

        if (!list)
            return no;

        if (list->tag != node->tag)
            return no;

        /* check list has no peers */
        if (list->next)
            return no;

        *pnode = list;  /* Set node to resume iteration */

        /* move inner list node into position of outer node */
        list->prev = node->prev;
        list->next = node->next;
        list->parent = node->parent;
        TY_(FixNodeLinks)(list);

        /* get rid of outer ul and its li */
        child->content = NULL;
        TY_(FreeNode)( doc, child ); /* See test #427841. */
        child = NULL;
        node->content = NULL;
        node->next = NULL;
        TY_(FreeNode)( doc, node );
        node = NULL;

        /*
          If prev node was a list the chances are this node
          should be appended to that list. Word has no way of
          recognizing nested lists and just uses indents
        */

        if (list->prev)
        {
            if ( (nodeIsUL(list->prev) || nodeIsOL(list->prev))
                 && list->prev->last )
            {
                node = list;
                list = node->prev;

                child = list->last;  /* <li> */

                list->next = node->next;
                TY_(FixNodeLinks)(list);

                node->parent = child;
                node->next = NULL;
                node->prev = child->last;
                TY_(FixNodeLinks)(node);
                CleanNode( doc, node );
            }
        }

        return yes;
    }

    return no;
}

/* Find CSS equivalent in a SPAN element */
static
Bool FindCSSSpanEq( Node *node, ctmbstr *s, Bool deprecatedOnly )
{
    struct
    {
        TidyTagId id;
        ctmbstr CSSeq;
        Bool deprecated;
    }
    const CSS_SpanEq[] =
        {
            { TidyTag_B, "font-weight: bold", no },
            { TidyTag_I, "font-style: italic", no },
            { TidyTag_S, "text-decoration: line-through", yes},
            { TidyTag_STRIKE, "text-decoration: line-through", yes},
            { TidyTag_U, "text-decoration: underline", yes},
            { TidyTag_UNKNOWN, NULL, no }
        };
    uint i;

    for (i=0; CSS_SpanEq[i].CSSeq; ++i)
        if ( (!deprecatedOnly || CSS_SpanEq[i].deprecated)
             && TagIsId(node, CSS_SpanEq[i].id) )
        {
            *s = CSS_SpanEq[i].CSSeq;
            return yes;
        }
    return no;
}

/* Necessary conditions to apply BlockStyle(). */
static Bool CanApplyBlockStyle( Node *node )
{
    if (TY_(nodeHasCM)(node,CM_BLOCK | CM_LIST | CM_DEFLIST | CM_TABLE)
        && !nodeIsDIV(node) && !nodeIsP(node)
        && !nodeIsTABLE(node) && !nodeIsTR(node) && !nodeIsLI(node) )
    {
        return yes;
    }
    return no;
}

/*
  Symptom: the only child of a block-level element is a
  presentation element such as B, I or FONT

  Action: add style "font-weight: bold" to the block and
  strip the <b> element, leaving its children.

  example:

    <p>
      <b><font face="Arial" size="6">Draft Recommended Practice</font></b>
    </p>

  becomes:

      <p style="font-weight: bold; font-family: Arial; font-size: 6">
        Draft Recommended Practice
      </p>

  This code also replaces the align attribute by a style attribute.
  However, to avoid CSS problems with Navigator 4, this isn't done
  for the elements: caption, tr and table
*/
static Bool BlockStyle( TidyDocImpl* doc, Node *node, Node **ARG_UNUSED(pnode) )
{
    Node *child;
    ctmbstr CSSeq;

    /* check for bgcolor */
    if (   nodeIsTABLE(node)
        || nodeIsTD(node) || nodeIsTH(node) || nodeIsTR( node ))
        TableBgColor( doc, node );

    if (CanApplyBlockStyle(node))
    {
        /* check for align attribute */
        if ( !nodeIsCAPTION(node) )
            TextAlign( doc, node );

        child = node->content;
        if (child == NULL)
            return no;

        /* check child has no peers */
        if (child->next)
            return no;

        if ( FindCSSSpanEq(child, &CSSeq, no) )
        {
            MergeStyles( doc, node, child );
            TY_(AddStyleProperty)( doc, node, CSSeq );
            StripOnlyChild( doc, node );
            return yes;
        }
        else if ( nodeIsFONT(child) )
        {
            MergeStyles( doc, node, child );
            AddFontStyles( doc, node, child->attributes );
            StripOnlyChild( doc, node );
            return yes;
        }
    }

    return no;
}

/* Necessary conditions to apply InlineStyle(). */
static Bool CanApplyInlineStyle( Node *node )
{
    return !nodeIsFONT(node) && TY_(nodeHasCM)(node, CM_INLINE|CM_ROW);
}

/* the only child of table cell or an inline element such as em */
static Bool InlineStyle( TidyDocImpl* doc, Node *node, Node **ARG_UNUSED(pnode) )
{
    Node *child;
    ctmbstr CSSeq;

    if ( CanApplyInlineStyle(node) )
    {
        child = node->content;

        if (child == NULL)
            return no;

        /* check child has no peers */

        if (child->next)
            return no;

        if ( FindCSSSpanEq(child, &CSSeq, no) )
        {
            MergeStyles( doc, node, child );
            TY_(AddStyleProperty)( doc, node, CSSeq );
            StripOnlyChild( doc, node );
            return yes;
        }
        else if ( nodeIsFONT(child) )
        {
            MergeStyles( doc, node, child );
            AddFontStyles( doc, node, child->attributes );
            StripOnlyChild( doc, node );
            return yes;
        }
    }

    return no;
}

/*
    Transform element to equivalent CSS
*/
static Bool InlineElementToCSS( TidyDocImpl* doc, Node* node,
                                Node **ARG_UNUSED(pnode)  )
{
    ctmbstr CSSeq;

    /* if node is the only child of parent element then leave alone
          Do so only if BlockStyle may be successful. */
    if ( node->parent->content == node && node->next == NULL &&
         (CanApplyBlockStyle(node->parent)
          || CanApplyInlineStyle(node->parent)) )
        return no;

    if ( FindCSSSpanEq(node, &CSSeq, yes) )
    {
        RenameElem( doc, node, TidyTag_SPAN );
        TY_(AddStyleProperty)( doc, node, CSSeq );
        return yes;
    }
    return no;
}

/*
  Replace font elements by span elements, deleting
  the font element's attributes and replacing them
  by a single style attribute.
*/
static Bool Font2Span( TidyDocImpl* doc, Node *node, Node **pnode )
{
    AttVal *av, *style, *next;

    if ( nodeIsFONT(node) )
    {
        /* if node is the only child of parent element then leave alone
          Do so only if BlockStyle may be successful. */
        if ( node->parent->content == node && node->next == NULL &&
             CanApplyBlockStyle(node->parent) )
            return no;

        AddFontStyles( doc, node, node->attributes );

        /* extract style attribute and free the rest */
        av = node->attributes;
        style = NULL;

        while (av)
        {
            next = av->next;

            if (attrIsSTYLE(av))
            {
                av->next = NULL;
                style = av;
            }
            else
            {
                TY_(FreeAttribute)( doc, av );
            }
            av = next;
        }

        node->attributes = style;
        RenameElem( doc, node, TidyTag_SPAN );
        return yes;
    }

    return no;
}

/*
  Applies all matching rules to a node.
*/
Node* CleanNode( TidyDocImpl* doc, Node *node )
{
    Node *next = NULL;
    TidyTriState mergeDivs = cfgAutoBool(doc, TidyMergeDivs);
    TidyTriState mergeSpans = cfgAutoBool(doc, TidyMergeSpans);

    for (next = node; TY_(nodeIsElement)(node); node = next)
    {
        if ( Dir2Div(doc, node, &next) )
            continue;

        /* Special case: true result means
        ** that arg node and its parent no longer exist.
        ** So we must jump back up the CreateStyleProperties()
        ** call stack until we have a valid node reference.
        */
        if ( NestedList(doc, node, &next) )
            return next;

        if ( Center2Div(doc, node, &next) )
            continue;

        if ( MergeNestedElements(doc, TidyTag_DIV, mergeDivs, node, &next) )
            continue;

        if ( MergeNestedElements(doc, TidyTag_SPAN, mergeSpans, node, &next) )
            continue;

        if ( BlockStyle(doc, node, &next) )
            continue;

        if ( InlineStyle(doc, node, &next) )
            continue;

        if ( InlineElementToCSS(doc, node, &next) )
            continue;

        if ( Font2Span(doc, node, &next) )
            continue;

        break;
    }

    return next;
}

/* Special case: if the current node is destroyed by
** CleanNode() lower in the tree, this node and its parent
** no longer exist.  So we must jump back up the CleanTree()
** call stack until we have a valid node reference.
*/

static Node* CleanTree( TidyDocImpl* doc, Node *node )
{
    if (node->content)
    {
        Node *child;
        for (child = node->content; child != NULL; child = child->next)
        {
            child = CleanTree( doc, child );
            if ( !child )
                break;
        }
    }

    return CleanNode( doc, node );
}

static void DefineStyleRules( TidyDocImpl* doc, Node *node )
{
    Node *child;

    if (node->content)
    {
        for (child = node->content;
                child != NULL; child = child->next)
        {
            DefineStyleRules( doc, child );
        }
    }

    Style2Rule( doc, node );
}

void TY_(CleanDocument)( TidyDocImpl* doc )
{
    /* placeholder.  CleanTree()/CleanNode() will not
    ** zap root element
    */
    CleanTree( doc, &doc->root );

    if ( cfgBool(doc, TidyMakeClean) )
    {
        DefineStyleRules( doc, &doc->root );
        CreateStyleElement( doc );
    }
}

/* simplifies <b><b> ... </b> ...</b> etc. */
void TY_(NestedEmphasis)( TidyDocImpl* doc, Node* node )
{
    Node *next;

    while (node)
    {
        next = node->next;

        if ( (nodeIsB(node) || nodeIsI(node))
             && node->parent && node->parent->tag == node->tag)
        {
            /* strip redundant inner element */
            DiscardContainer( doc, node, &next );
            node = next;
            continue;
        }

        if ( node->content )
            TY_(NestedEmphasis)( doc, node->content );

        node = next;
    }
}



/* replace i by em and b by strong */
void TY_(EmFromI)( TidyDocImpl* doc, Node* node )
{
    while (node)
    {
        if ( nodeIsI(node) )
            RenameElem( doc, node, TidyTag_EM );
        else if ( nodeIsB(node) )
            RenameElem( doc, node, TidyTag_STRONG );

        if ( node->content )
            TY_(EmFromI)( doc, node->content );

        node = node->next;
    }
}

static Bool HasOneChild(Node *node)
{
    return (node->content && node->content->next == NULL);
}

/*
 Some people use dir or ul without an li
 to indent the content. The pattern to
 look for is a list with a single implicit
 li. This is recursively replaced by an
 implicit blockquote.
*/
void TY_(List2BQ)( TidyDocImpl* doc, Node* node )
{
    while (node)
    {
        if (node->content)
            TY_(List2BQ)( doc, node->content );

        if ( node->tag && node->tag->parser == TY_(ParseList) &&
             HasOneChild(node) && node->content->implicit )
        {
            StripOnlyChild( doc, node );
            RenameElem( doc, node, TidyTag_BLOCKQUOTE );
            node->implicit = yes;
        }

        node = node->next;
    }
}


/*
 Replace implicit blockquote by div with an indent
 taking care to reduce nested blockquotes to a single
 div with the indent set to match the nesting depth
*/
void TY_(BQ2Div)( TidyDocImpl* doc, Node *node )
{
    Stack *stack = TY_(newStack)(doc, 16);
    Node *next;

    tmbchar indent_buf[ 32 ];
    uint indent;

    while (node)
    {
        next = node->next;

        if ( nodeIsBLOCKQUOTE(node) && node->implicit )
        {
            indent = 1;

            while( HasOneChild(node) &&
                   nodeIsBLOCKQUOTE(node->content) &&
                   node->implicit)
            {
                ++indent;
                StripOnlyChild( doc, node );
            }

            TY_(tmbsnprintf)(indent_buf, sizeof(indent_buf), "margin-left: %dem",
                             2*indent);

            RenameElem( doc, node, TidyTag_DIV );
            TY_(AddStyleProperty)(doc, node, indent_buf );

            if (node->content)
            {
                TY_(push)(stack, next);
                node = node->content;
                continue;
            }
        }
        else if (node->content)
        {
            TY_(push)(stack, next);
            node = node->content;
            continue;
        }

        node = next ? next : TY_(pop)(stack);
    }
    TY_(freeStack)(stack);
}


static Node* FindEnclosingCell( TidyDocImpl* ARG_UNUSED(doc), Node *node)
{
    Node *check;

    for ( check=node; check; check = check->parent )
    {
      if ( nodeIsTD(check) )
        return check;
    }
    return NULL;
}

/* node is <![if ...]> prune up to <![endif]> */
static Node* PruneSection( TidyDocImpl* doc, Node *node )
{
    Lexer* lexer = doc->lexer;

    for (;;)
    {
        if (node == NULL)
            return NULL;

        ctmbstr lexbuf = lexer->lexbuf + node->start;
        if ( TY_(tmbstrncmp)(lexbuf, "if !supportEmptyParas", 21) == 0 )
        {
          Node* cell = FindEnclosingCell( doc, node );
          if ( cell )
          {
            /* Need to put &nbsp; into cell so it doesn't look weird
            */
            Node* nbsp = TY_(NewLiteralTextNode)( lexer, "\240" );
            assert( (byte)'\240' == (byte)160 );
            TY_(InsertNodeBeforeElement)( node, nbsp );
          }
        }

        /* discard node and returns next, unless it is a text node */
        if ( node->type == TextNode )
            node = node->next;
        else
            node = TY_(DiscardElement)( doc, node );

        if (node == NULL)
            return NULL;

        if (node->type == SectionTag)
        {
            if (TY_(tmbstrncmp)(lexer->lexbuf + node->start, "if", 2) == 0)
            {
                node = PruneSection( doc, node );
                continue;
            }

            if (TY_(tmbstrncmp)(lexer->lexbuf + node->start, "endif", 5) == 0)
            {
                node = TY_(DiscardElement)( doc, node );
                break;
            }
        }
    }

    return node;
}

void TY_(DropSections)( TidyDocImpl* doc, Node* node )
{
    Lexer* lexer = doc->lexer;
    while (node)
    {
        if (node->type == SectionTag)
        {
            /* prune up to matching endif */
            if ((TY_(tmbstrncmp)(lexer->lexbuf + node->start, "if", 2) == 0) &&
                (TY_(tmbstrncmp)(lexer->lexbuf + node->start, "if !vml", 7) != 0)) /* #444394 - fix 13 Sep 01 */
            {
                node = PruneSection( doc, node );
                continue;
            }

            /* discard others as well */
            node = TY_(DiscardElement)( doc, node );
            continue;
        }

        if (node->content)
            TY_(DropSections)( doc, node->content );

        node = node->next;
    }
}

static void PurgeWord2000Attributes( TidyDocImpl* doc, Node* node )
{
    AttVal *attr, *next, *prev = NULL;

    for ( attr = node->attributes; attr; attr = next )
    {
        next = attr->next;

        /* special check for class="Code" denoting pre text */
        /* Pass thru user defined styles as HTML class names */
        if (attrIsCLASS(attr))
        {
            if (AttrValueIs(attr, "Code") ||
                 TY_(tmbstrncmp)(attr->value, "Mso", 3) != 0 )
            {
                prev = attr;
                continue;
            }
        }

        if (attrIsCLASS(attr) ||
            attrIsSTYLE(attr) ||
            attrIsLANG(attr)  ||
             ( (attrIsHEIGHT(attr) || attrIsWIDTH(attr)) &&
               (nodeIsTD(node) || nodeIsTR(node) || nodeIsTH(node)) ) ||
             (attr->attribute && TY_(tmbstrncmp)(attr->attribute, "x:", 2) == 0) )
        {
            if (prev)
                prev->next = next;
            else
                node->attributes = next;

            TY_(FreeAttribute)( doc, attr );
        }
        else
            prev = attr;
    }
}

/* Word2000 uses span excessively, so we strip span out */
static Node* StripSpan( TidyDocImpl* doc, Node* span )
{
    Node *node, *prev = NULL, *content;

    /*
     deal with span elements that have content
     by splicing the content in place of the span
     after having processed it
    */

    TY_(CleanWord2000)( doc, span->content );
    content = span->content;

    if (span->prev)
        prev = span->prev;
    else if (content)
    {
        node = content;
        content = content->next;
        TY_(RemoveNode)(node);
        TY_(InsertNodeBeforeElement)(span, node);
        prev = node;
    }

    while (content)
    {
        node = content;
        content = content->next;
        TY_(RemoveNode)(node);
        TY_(InsertNodeAfterElement)(prev, node);
        prev = node;
    }

    if (span->next == NULL)
        span->parent->last = prev;

    node = span->next;
    span->content = NULL;
    TY_(DiscardElement)( doc, span );
    return node;
}

/* map non-breaking spaces to regular spaces */
void TY_(NormalizeSpaces)(Lexer *lexer, Node *node)
{
    while ( node )
    {
        if ( node->content )
            TY_(NormalizeSpaces)( lexer, node->content );

        if (TY_(nodeIsText)(node))
        {
            uint i, c;
            tmbstr p = lexer->lexbuf + node->start;

            for (i = node->start; i < node->end; ++i)
            {
                c = (byte) lexer->lexbuf[i];

                /* look for UTF-8 multibyte character */
                if ( c > 0x7F )
                    i += TY_(GetUTF8)( lexer->lexbuf + i, &c );

                if ( c == 160 )
                    c = ' ';

                p = TY_(PutUTF8)(p, c);
            }
            node->end = p - lexer->lexbuf;
        }

        node = node->next;
    }
}

/* used to hunt for hidden preformatted sections */
static Bool NoMargins(Node *node)
{
    AttVal *attval = TY_(AttrGetById)(node, TidyAttr_STYLE);

    if ( !AttrHasValue(attval) )
        return no;

    /* search for substring "margin-top: 0" */
    if (!TY_(tmbsubstr)(attval->value, "margin-top: 0"))
        return no;

    /* search for substring "margin-bottom: 0" */
    if (!TY_(tmbsubstr)(attval->value, "margin-bottom: 0"))
        return no;

    return yes;
}

/* does element have a single space as its content? */
static Bool SingleSpace( Lexer* lexer, Node* node )
{
    if ( node->content )
    {
        node = node->content;

        if ( node->next != NULL )
            return no;

        if ( node->type != TextNode )
            return no;

        if ( (node->end - node->start) == 1 &&
             lexer->lexbuf[node->start] == ' ' )
            return yes;

        if ( (node->end - node->start) == 2 )
        {
            uint c = 0;
            TY_(GetUTF8)( lexer->lexbuf + node->start, &c );
            if ( c == 160 )
                return yes;
        }
    }

    return no;
}

/*
 This is a major clean up to strip out all the extra stuff you get
 when you save as web page from Word 2000. It doesn't yet know what
 to do with VML tags, but these will appear as errors unless you
 declare them as new tags, such as o:p which needs to be declared
 as inline.
*/
void TY_(CleanWord2000)( TidyDocImpl* doc, Node *node)
{
    /* used to a list from a sequence of bulleted p's */
    Lexer* lexer = doc->lexer;
    Node* list = NULL;
    AttVal *next_attr, *attval;

    while ( node )
    {
        /* get rid of Word's xmlns attributes */
        if ( nodeIsHTML(node) )
        {
            /* check that it's a Word 2000 document */
            if ( !TY_(IsWord2000) (doc) ) /* Is. #896 */
                return;

            /* Output proprietary attributes to maintain errout compatibility
             * with traditional Tidy. This is a result of moving all of the
             * proprietary checks to near the end of the cleanup process,
             * meaning this result would not ordinarily be displayed.
             */
            attval = node->attributes;
            while ( attval ) {
                next_attr = attval->next;

                /* Issue #591 - take care of a NULL attribute, too. */
                if ( !attval->attribute || ( strcmp(attval->attribute, "xmlns") != 0 ))
                    TY_(ReportAttrError)(doc, node, attval, PROPRIETARY_ATTRIBUTE);
                attval = next_attr;
            }

            TY_(FreeAttrs)( doc, node );
        }

        /* fix up preformatted sections by looking for a
        ** sequence of paragraphs with zero top/bottom margin
        */
        if ( nodeIsP(node) )
        {
            if (NoMargins(node))
            {
                Node *pre, *next;
                TY_(CoerceNode)(doc, node, TidyTag_PRE, no, yes);

                PurgeWord2000Attributes( doc, node );

                if (node->content)
                    TY_(CleanWord2000)( doc, node->content );

                pre = node;
                node = node->next;

                /* continue to strip p's */

                while ( nodeIsP(node) && NoMargins(node) )
                {
                    next = node->next;
                    TY_(RemoveNode)(node);
                    TY_(InsertNodeAtEnd)(pre, TY_(NewLineNode)(lexer));
                    TY_(InsertNodeAtEnd)(pre, node);
                    StripSpan( doc, node );
                    node = next;
                }

                if (node == NULL)
                    break;
            }
        }

        if (node->tag && (node->tag->model & CM_BLOCK)
            && SingleSpace(lexer, node))
        {
            node = StripSpan( doc, node );
            continue;
        }
        /* discard Word's style verbiage */
        if ( nodeIsSTYLE(node) || nodeIsMETA(node) ||
             node->type == CommentTag )
        {
            node = TY_(DiscardElement)( doc, node );
            continue;
        }

        /* strip out all span and font tags Word scatters so liberally! */
        if ( nodeIsSPAN(node) || nodeIsFONT(node) )
        {
            node = StripSpan( doc, node );
            continue;
        }

        if ( nodeIsLINK(node) )
        {
            AttVal *attr = TY_(AttrGetById)(node, TidyAttr_REL);

            if (AttrValueIs(attr, "File-List"))
            {
                node = TY_(DiscardElement)( doc, node );
                continue;
            }
        }

        /* discards <o:p> which encodes the paragraph mark */
        if ( node->tag && TY_(tmbstrcmp)(node->tag->name,"o:p")==0)
        {
            /* Output proprietary elements to maintain errout compatibility
             * with traditional Tidy. This is a result of moving all of the
             * proprietary checks to near the end of the cleanup process,
             * meaning this result would not ordinarily be displayed.
             */
            Node* next;
            TY_(Report)(doc, NULL, node, PROPRIETARY_ELEMENT);
            DiscardContainer( doc, node, &next );
            node = next;
            continue;
        }

        /* discard empty paragraphs */

        if ( node->content == NULL && nodeIsP(node) )
        {
            /*  Use the existing function to ensure consistency */
            Node *next = TY_(TrimEmptyElement)( doc, node );
            node = next;
            continue;
        }

        if ( nodeIsP(node) )
        {
            AttVal *attr, *atrStyle;

            attr = TY_(AttrGetById)(node, TidyAttr_CLASS);
            atrStyle = TY_(AttrGetById)(node, TidyAttr_STYLE);
            /*
               (JES) Sometimes Word marks a list item with the following hokie syntax
               <p class="MsoNormal" style="...;mso-list:l1 level1 lfo1;
                translate these into <li>
            */
            /* map sequence of <p class="MsoListBullet"> to <ul>...</ul> */
            /* map <p class="MsoListNumber"> to <ol>...</ol> */
            if ( AttrValueIs(attr, "MsoListBullet") ||
                 AttrValueIs(attr, "MsoListNumber") ||
                 AttrContains(atrStyle, "mso-list:") )
            {
                TidyTagId listType = TidyTag_UL;
                if (AttrValueIs(attr, "MsoListNumber"))
                    listType = TidyTag_OL;

                TY_(CoerceNode)(doc, node, TidyTag_LI, no, yes);

                if ( !list || TagId(list) != listType )
                {
                    const Dict* tag = TY_(LookupTagDef)( listType );
                    list = TY_(InferredTag)(doc, tag->id);
                    TY_(InsertNodeBeforeElement)(node, list);
                }

                PurgeWord2000Attributes( doc, node );

                if ( node->content )
                    TY_(CleanWord2000)( doc, node->content );

                /* remove node and append to contents of list */
                TY_(RemoveNode)(node);
                TY_(InsertNodeAtEnd)(list, node);
                node = list;
            }
            /* map sequence of <p class="Code"> to <pre>...</pre> */
            else if (AttrValueIs(attr, "Code"))
            {
                Node *br = TY_(NewLineNode)(lexer);
                TY_(NormalizeSpaces)(lexer, node->content);

                if ( !list || TagId(list) != TidyTag_PRE )
                {
                    list = TY_(InferredTag)(doc, TidyTag_PRE);
                    TY_(InsertNodeBeforeElement)(node, list);
                }

                /* remove node and append to contents of list */
                TY_(RemoveNode)(node);
                TY_(InsertNodeAtEnd)(list, node);
                StripSpan( doc, node );
                TY_(InsertNodeAtEnd)(list, br);
                node = list->next;
            }
            else
                list = NULL;
        }
        else
            list = NULL;

        if (!node)
            return;

        /* strip out style and class attributes */
        if (TY_(nodeIsElement)(node))
            PurgeWord2000Attributes( doc, node );

        if (node->content)
            TY_(CleanWord2000)( doc, node->content );

        node = node->next;
    }
}

Bool TY_(IsWord2000)( TidyDocImpl* doc )
{
    AttVal *attval;
    Node *node, *head;
    Node *html = TY_(FindHTML)( doc );

    if (html && TY_(GetAttrByName)(html, "xmlns:o"))
        return yes;

    /* search for <meta name="GENERATOR" content="Microsoft ..."> */
    head = TY_(FindHEAD)( doc );

    if (head)
    {
        for (node = head->content; node; node = node->next)
        {
            if ( !nodeIsMETA(node) )
                continue;

            attval = TY_(AttrGetById)( node, TidyAttr_NAME );

            if ( !AttrValueIs(attval, "generator") )
                continue;

            attval =  TY_(AttrGetById)( node, TidyAttr_CONTENT );

            if ( AttrContains(attval, "Microsoft") )
                return yes;
        }
    }

    return no;
}

/* where appropriate move object elements from head to body */
void TY_(BumpObject)( TidyDocImpl* doc, Node *html )
{
    Node *node, *next, *head = NULL, *body = NULL;

    if (!html)
        return;

    for ( node = html->content; node != NULL; node = node->next )
    {
        if ( nodeIsHEAD(node) )
            head = node;

        if ( nodeIsBODY(node) )
            body = node;
    }

    if ( head != NULL && body != NULL )
    {
        for (node = head->content; node != NULL; node = next)
        {
            next = node->next;

            if ( nodeIsOBJECT(node) )
            {
                Node *child;
                Bool bump = no;

                for (child = node->content; child != NULL; child = child->next)
                {
                    /* bump to body unless content is param */
                    if ( (TY_(nodeIsText)(child) && !TY_(IsBlank)(doc->lexer, node))
                         || !nodeIsPARAM(child) )
                    {
                            bump = yes;
                            break;
                    }
                }

                if ( bump )
                {
                    TY_(RemoveNode)( node );
                    TY_(InsertNodeAtStart)( body, node );
                }
            }
        }
    }
}


/*\
*  Issue #456 - Check meta charset
*  1. if there is no meta charset, it adds one, according to doctype, no warning.
*  2. if there is a meta charset, it moves it to the top if HEAD. Not sure this required?
*  3. if it doesn't match the output encoding, and fix. Naybe no warning?
*  4. if there are duplicates, discard them, with warning.
\*/
Bool TY_(TidyMetaCharset)(TidyDocImpl* doc)
{
    AttVal *charsetAttr;
    AttVal *contentAttr;
    AttVal *httpEquivAttr;
    Bool charsetFound = no;
    uint outenc = cfg(doc, TidyOutCharEncoding);
    ctmbstr enc = TY_(GetEncodingNameFromTidyId)(outenc);
    Node *currentNode;
    Node *head = TY_(FindHEAD)(doc);
    Node *metaTag;
    Node *prevNode;
    TidyBuffer buf;
    TidyBuffer charsetString;
    /* tmbstr httpEquivAttrValue; */
    /* tmbstr lcontent; */
    tmbstr newValue;
    Bool add_meta = cfgBool(doc, TidyMetaCharset);

    /* We can't do anything we don't have a head or encoding is NULL */
    if (!head || !enc || !TY_(tmbstrlen)(enc))
        return no;
    if (outenc == RAW)
        return no;
#ifndef NO_NATIVE_ISO2022_SUPPORT
    if (outenc == ISO2022)
        return no;
#endif
    if (cfgAutoBool(doc, TidyBodyOnly) == TidyYesState)
        return no; /* nothing to do here if showing body only */

    tidyBufInit(&charsetString);
    /* Set up the content test 'charset=value' */
    tidyBufClear(&charsetString);
    tidyBufAppend(&charsetString, "charset=", 8);
    tidyBufAppend(&charsetString, (char*)enc, TY_(tmbstrlen)(enc));
    tidyBufAppend(&charsetString, "\0", 1); /* zero terminate the buffer */
    /* process the children of the head */
    /* Issue #656 - guard against 'currentNode' being set NULL in loop */
    for (currentNode = head->content; currentNode;
        currentNode = (currentNode ? currentNode->next : NULL))
    {
        if (!nodeIsMETA(currentNode))
            continue;   /* not a meta node */
        charsetAttr = attrGetCHARSET(currentNode);
        httpEquivAttr = attrGetHTTP_EQUIV(currentNode);
        if (!charsetAttr && !httpEquivAttr)
            continue;   /* has no charset attribute */
                        /*
                        Meta charset comes in quite a few flavors:
                        1. <meta charset="value"> - expected for (X)HTML5.
                        */
        if (charsetAttr && !httpEquivAttr)
        {
            /* we already found one, so remove the rest. */
            if (charsetFound || !charsetAttr->value)
            {
                prevNode = currentNode->prev;
                TY_(Report)(doc, head, currentNode, DISCARDING_UNEXPECTED);
                TY_(DiscardElement)(doc, currentNode);
                currentNode = prevNode;
                continue;
            }
            charsetFound = yes;
            /* Fix mismatched attribute value */
            if (TY_(tmbstrcasecmp)(charsetAttr->value, enc) != 0)
            {
                newValue = (tmbstr)TidyDocAlloc(doc, TY_(tmbstrlen)(enc) + 1);   /* allocate + 1 for 0 */
                TY_(tmbstrcpy)(newValue, enc);
                /* Note: previously http-equiv had been modified, without warning
                in void TY_(VerifyHTTPEquiv)(TidyDocImpl* doc, Node *head)
                */
                TY_(ReportAttrError)(doc, currentNode, charsetAttr, ATTRIBUTE_VALUE_REPLACED);
                TidyDocFree(doc, charsetAttr->value);   /* free current value */
                charsetAttr->value = newValue;
            }
            /* Make sure it's the first element. */
            if (currentNode != head->content->next) {
                TY_(RemoveNode)(currentNode);
                TY_(InsertNodeAtStart)(head, currentNode);
            }
            continue;
        }
        /*
        2. <meta http-equiv="content-type" content="text/html; charset=UTF-8">
        expected for HTML4. This is normally ok - but can clash.
        */
        if (httpEquivAttr && !charsetAttr)
        {
            contentAttr = TY_(AttrGetById)(currentNode, TidyAttr_CONTENT);
            if (!contentAttr)
                continue;   /* has no 'content' attribute */
            if (!httpEquivAttr->value)
            {
                prevNode = currentNode->prev;
                TY_(Report)(doc, head, currentNode, DISCARDING_UNEXPECTED);
                TY_(DiscardElement)(doc, currentNode);
                currentNode = prevNode;
                continue;
            }
            /* httpEquivAttrValue = TY_(tmbstrtolower)(httpEquivAttr->value); */
            if (TY_(tmbstrcasecmp)(httpEquivAttr->value, (tmbstr) "content-type") != 0)
                continue;   /* is not 'content-type' */
            if (!contentAttr->value)
            {
                continue; /* has no 'content' attribute has NO VALUE! */
            }
            /* check encoding matches
            If a miss-match found here, fix it. previous silently done
            in void TY_(VerifyHTTPEquiv)(TidyDocImpl* doc, Node *head)
            lcontent = TY_(tmbstrtolower)(contentAttr->value);
            */
            if (TY_(tmbstrcasecmp)(contentAttr->value, (ctmbstr)charsetString.bp) == 0)
            {
                /* we already found one, so remove the rest. */
                if (charsetFound)
                {
                    prevNode = currentNode->prev;
                    TY_(Report)(doc, head, currentNode, DISCARDING_UNEXPECTED);
                    TY_(DiscardElement)(doc, currentNode);
                    currentNode = prevNode;
                    continue;
                }
                charsetFound = yes;
            }
            else
            {
                /* fix a mismatch */
                if (charsetFound)
                {
                    prevNode = currentNode->prev;
                    TY_(Report)(doc, head, currentNode, DISCARDING_UNEXPECTED);
                    TY_(DiscardElement)(doc, currentNode);
                    currentNode = prevNode;
                }
                else
                {
                    /* correct the content */
                    newValue = (tmbstr)TidyDocAlloc(doc, 19 + TY_(tmbstrlen)(enc) + 1);
                    TY_(tmbstrcpy)(newValue, "text/html; charset=");
                    TY_(tmbstrcpy)(newValue + 19, enc);
                    if (cfgBool(doc, TidyShowMetaChange))   /* Issue #456 - backward compatibility only */
                        TY_(ReportAttrError)(doc, currentNode, contentAttr, ATTRIBUTE_VALUE_REPLACED);
                    TidyDocFree(doc, contentAttr->value);
                    contentAttr->value = newValue;
                    charsetFound = yes;
                }
            }
            continue;
        }
        /*
        3. <meta charset="utf-8" http-equiv="Content-Type" content="...">
        This is generally bad. Discard and warn.
        */
        if (httpEquivAttr && charsetAttr)
        {
            /* printf("WARN ABOUT HTTP EQUIV AND CHARSET ATTR! \n"); */
            prevNode = currentNode->prev;
            TY_(Report)(doc, head, currentNode, DISCARDING_UNEXPECTED);
            TY_(DiscardElement)(doc, currentNode);
            currentNode = prevNode;
        }
    }

    /* completed head scan - add appropriate meta - if 'yes' and none exists */
    if (add_meta && !charsetFound)
    {
        /* add appropriate meta charset tag - no warning */
        metaTag = TY_(InferredTag)(doc, TidyTag_META);
        switch (TY_(HTMLVersion)(doc))
        {
        case HT50:
        case XH50:
            TY_(AddAttribute)(doc, metaTag, "charset", enc);
            break;
        default:
            tidyBufInit(&buf);
            tidyBufAppend(&buf, "text/html; ", 11);
            tidyBufAppend(&buf, charsetString.bp, TY_(tmbstrlen)((ctmbstr)charsetString.bp));
            tidyBufAppend(&buf, "\0", 1);   /* zero terminate the buffer */
            TY_(AddAttribute)(doc, metaTag, "http-equiv", "Content-Type"); /* add 'http-equiv' const. */
            TY_(AddAttribute)(doc, metaTag, "content", (char*)buf.bp);  /* add 'content="<enc>"' */
            tidyBufFree(&buf);
        }
        TY_(InsertNodeAtStart)(head, metaTag);
        TY_(Report)(doc, metaTag, head, ADDED_MISSING_CHARSET); /* actually just 'Info:' */
    }
    tidyBufFree(&charsetString);
    return yes;
}


void TY_(DropComments)(TidyDocImpl* doc, Node* node)
{
    Node* next;

    while (node)
    {
        next = node->next;

        if (node->type == CommentTag)
        {
            TY_(RemoveNode)(node);
            TY_(FreeNode)(doc, node);
            node = next;
            continue;
        }

        if (node->content)
            TY_(DropComments)(doc, node->content);

        node = next;
    }
}

void TY_(DropFontElements)(TidyDocImpl* doc, Node* node, Node **ARG_UNUSED(pnode))
{
    Node* next;

    while (node)
    {
        next = node->next;

        if (nodeIsFONT(node))
        {
            DiscardContainer(doc, node, &next);
            node = next;
            continue;
        }

        if (node->content)
            TY_(DropFontElements)(doc, node->content, &next);

        node = next;
    }
}

void TY_(WbrToSpace)(TidyDocImpl* doc, Node* node)
{
    Node* next;

    while (node)
    {
        next = node->next;

        if (nodeIsWBR(node))
        {
            Node* text;
            text = TY_(NewLiteralTextNode)(doc->lexer, " ");
            TY_(InsertNodeAfterElement)(node, text);
            TY_(RemoveNode)(node);
            TY_(FreeNode)(doc, node);
            node = next;
            continue;
        }

        if (node->content)
            TY_(WbrToSpace)(doc, node->content);

        node = next;
   }
}

/*
  Filters from Word and PowerPoint often use smart
  quotes resulting in character codes between 128
  and 159. Unfortunately, the corresponding HTML 4.0
  entities for these are not widely supported. The
  following converts dashes and quotation marks to
  the nearest ASCII equivalent. My thanks to
  Andrzej Novosiolov for his help with this code.

  Note: The old code in the pretty printer applied
  this to all node types and attribute values while
  this routine applies it only to text nodes. First,
  Microsoft Office products rarely put the relevant
  characters into these tokens, second support for
  them is much better now and last but not least, it
  can be harmful to replace these characters since
  US-ASCII quote marks are often used as syntax
  characters, a simple

    <a onmouseover="alert('&#x2018;')">...</a>

  would be broken if the U+2018 is replaced by "'".
  The old code would neither take care whether the
  quote mark is already used as delimiter,

    <p title='&#x2018;'>...</p>

  got

    <p title='''>...</p>

  Since browser support is much better nowadays and
  high-quality typography is better than ASCII it'd
  be probably a good idea to drop the feature...
*/
void TY_(DowngradeTypography)(TidyDocImpl* doc, Node* node)
{
    Node* next;
    Lexer* lexer = doc->lexer;

    while (node)
    {
        next = node->next;

        if (TY_(nodeIsText)(node))
        {
            uint i, c;
            tmbstr p = lexer->lexbuf + node->start;

            for (i = node->start; i < node->end; ++i)
            {
                c = (unsigned char) lexer->lexbuf[i];

                if (c > 0x7F)
                    i += TY_(GetUTF8)(lexer->lexbuf + i, &c);

                if (c >= 0x2013 && c <= 0x201E)
                {
                    switch (c)
                    {
                    case 0x2013: /* en dash */
                    case 0x2014: /* em dash */
                        c = '-';
                        break;
                    case 0x2018: /* left single  quotation mark */
                    case 0x2019: /* right single quotation mark */
                    case 0x201A: /* single low-9 quotation mark */
                        c = '\'';
                        break;
                    case 0x201C: /* left double  quotation mark */
                    case 0x201D: /* right double quotation mark */
                    case 0x201E: /* double low-9 quotation mark */
                        c = '"';
                        break;
                    }
                }

                p = TY_(PutUTF8)(p, c);
            }

            node->end = p - lexer->lexbuf;
        }

        if (node->content)
            TY_(DowngradeTypography)(doc, node->content);

        node = next;
    }
}

void TY_(ReplacePreformattedSpaces)(TidyDocImpl* doc, Node* node)
{
    Node* next;

    while (node)
    {
        next = node->next;

        if (node->tag && node->tag->parser == TY_(ParsePre))
        {
            TY_(NormalizeSpaces)(doc->lexer, node->content);
            node = next;
            continue;
        }

        if (node->content)
            TY_(ReplacePreformattedSpaces)(doc, node->content);

        node = next;
    }
}

void TY_(ConvertCDATANodes)(TidyDocImpl* doc, Node* node)
{
    Node* next;

    while (node)
    {
        next = node->next;

        if (node->type == CDATATag)
            node->type = TextNode;

        if (node->content)
            TY_(ConvertCDATANodes)(doc, node->content);

        node = next;
    }
}

/*
  FixLanguageInformation ensures that the document contains (only)
  the attributes for language information desired by the output
  document type. For example, for XHTML 1.0 documents both
  'xml:lang' and 'lang' are desired, for XHTML 1.1 only 'xml:lang'
  is desired and for HTML 4.01 only 'lang' is desired.
*/
void TY_(FixLanguageInformation)(TidyDocImpl* doc, Node* node, Bool wantXmlLang, Bool wantLang)
{
    Stack *stack = TY_(newStack)(doc, 16);
    Node* next;

    while (node)
    {
        next = node->next;

        /* todo: report modifications made here to the report system */

        if (TY_(nodeIsElement)(node))
        {
            AttVal* lang = TY_(AttrGetById)(node, TidyAttr_LANG);
            AttVal* xmlLang = TY_(AttrGetById)(node, TidyAttr_XML_LANG);

            if (lang && xmlLang)
            {
                /*
                  todo: check whether both attributes are in sync,
                  here or elsewhere, where elsewhere is probably
                  preferable.
                  AD - March 2005: not mandatory according the standards.
                */
            }
            else if (lang && wantXmlLang)
            {
                if (TY_(NodeAttributeVersions)( node, TidyAttr_XML_LANG )
                    & doc->lexer->versionEmitted)
                    TY_(RepairAttrValue)(doc, node, "xml:lang", lang->value);
            }
            else if (xmlLang && wantLang)
            {
                if (TY_(NodeAttributeVersions)( node, TidyAttr_LANG )
                    & doc->lexer->versionEmitted)
                    TY_(RepairAttrValue)(doc, node, "lang", xmlLang->value);
            }

            if (lang && !wantLang)
                TY_(RemoveAttribute)(doc, node, lang);

            if (xmlLang && !wantXmlLang)
                TY_(RemoveAttribute)(doc, node, xmlLang);
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

/*
  Set/fix/remove <html xmlns='...'>
*/
void TY_(FixXhtmlNamespace)(TidyDocImpl* doc, Bool wantXmlns)
{
    Node* html = TY_(FindHTML)(doc);
    AttVal* xmlns;

    if (!html)
        return;

    xmlns = TY_(AttrGetById)(html, TidyAttr_XMLNS);

    if (wantXmlns)
    {
        if (!AttrValueIs(xmlns, XHTML_NAMESPACE))
            TY_(RepairAttrValue)(doc, html, "xmlns", XHTML_NAMESPACE);
    }
    else if (xmlns)
    {
        TY_(RemoveAttribute)(doc, html, xmlns);
    }
}

/*
  ...
*/
void TY_(FixAnchors)(TidyDocImpl* doc, Node *node, Bool wantName, Bool wantId)
{
    Stack *stack = TY_(newStack)(doc, 16);
    Node* next;

    while (node)
    {
        next = node->next;

        if (TY_(IsAnchorElement)(doc, node))
        {
            AttVal *name = TY_(AttrGetById)(node, TidyAttr_NAME);
            AttVal *id = TY_(AttrGetById)(node, TidyAttr_ID);
            Bool hadName = name!=NULL;
            Bool hadId = id!=NULL;
            Bool IdEmitted = no;
            Bool NameEmitted = no;

            /* todo: how are empty name/id attributes handled? */

            if (name && id)
            {
                Bool NameHasValue = AttrHasValue(name);
                Bool IdHasValue = AttrHasValue(id);
                if ( (NameHasValue != IdHasValue) ||
                     (NameHasValue && IdHasValue &&
                     TY_(tmbstrcmp)(name->value, id->value) != 0 ) )
                    TY_(ReportAttrError)( doc, node, name, ID_NAME_MISMATCH);
            }
            else if (name && wantId)
            {
                if (TY_(NodeAttributeVersions)( node, TidyAttr_ID )
                    & doc->lexer->versionEmitted)
                {
                    if (TY_(IsValidHTMLID)(name->value))
                    {
                        TY_(RepairAttrValue)(doc, node, "id", name->value);
                        IdEmitted = yes;
                    }
                    else
                        TY_(ReportAttrError)(doc, node, name, INVALID_XML_ID);
                 }
            }
            else if (id && wantName)
            {
                if (TY_(NodeAttributeVersions)( node, TidyAttr_NAME )
                    & doc->lexer->versionEmitted)
                {
                    /* todo: do not assume id is valid */
                    TY_(RepairAttrValue)(doc, node, "name", id->value);
                    NameEmitted = yes;
                }
            }

            if (id && !wantId
                /* make sure that Name has been emitted if requested */
                && (hadName || !wantName || NameEmitted) ) {
                if (!wantId && !wantName)
                    TY_(RemoveAnchorByNode)(doc, id->value, node);
                TY_(RemoveAttribute)(doc, node, id);
            }

            if (name && !wantName
                /* make sure that Id has been emitted if requested */
                && (hadId || !wantId || IdEmitted) ) {
                if (!wantId && !wantName)
                    TY_(RemoveAnchorByNode)(doc, name->value, node);
                TY_(RemoveAttribute)(doc, node, name);
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

/* Issue #567 - move style elements from body to head
 * ==================================================
 */
static void StyleToHead(TidyDocImpl* doc, Node *head, Node *node, Bool fix, int indent)
{
    Stack *stack = TY_(newStack)(doc, 16);
    Node *next;

    while (node)
    {
        next = node->next;

        if (nodeIsSTYLE(node))
        {
            if (fix)
            {
                TY_(RemoveNode)(node); /* unhook style node from body */
                TY_(InsertNodeAtEnd)(head, node);   /* add to end of head */
                TY_(Report)(doc, node, head, MOVED_STYLE_TO_HEAD); /* report move */
            }
            else
            {
                TY_(Report)(doc, node, head, FOUND_STYLE_IN_BODY);
            }
        }
        else if (node->content)
        {
            TY_(push)(stack, next);
            node = node->content;
            indent++;
            continue;
        }

        if (next)
            node = next;
        else
        {
            node = TY_(pop)(stack);
            indent--;
        }
    }
    TY_(freeStack)(stack);
}


void TY_(CleanStyle)(TidyDocImpl* doc, Node *html)
{
    Node *head = NULL, *body = NULL;
    Bool fix = cfgBool(doc, TidyStyleTags);

    if (!html)
        return; /* oops, not given a start node */

    head = TY_(FindHEAD)( doc );
    body = TY_(FindBody)( doc );

    if ((head != NULL) && (body != NULL))
    {
		StyleToHead(doc, head, body, fix, 0); /* found head and body */
    }
}
/* ==================================================
 */

/*
 * CleanHead - clean the head node, if it exists, and we
 * are going to show it in the output.
 * Issue #692 - Remove multiple title elements
 */
void TY_(CleanHead)(TidyDocImpl* doc)
{
    Node *head, *node, *next;
    uint titles = 0;
    if (cfgAutoBool(doc, TidyBodyOnly) == TidyYesState)
        return; /* not going to show head, so forget it */
    head = TY_(FindHEAD)(doc);
    if (!head)
        return;
    node = head->content;
    while (node)
    {
        next = node->next;  /* get any 'next' */
        if (nodeIsTITLE(node))
        {
            titles++;
            if (titles > 1)
            {
                TY_(Report)(doc, head, node, DISCARDING_UNEXPECTED);
                TY_(DiscardElement)(doc, node); /* delete this node */
            }
        }
        node = next;
    }
}

/*
 * local variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 4
 * end:
 */
