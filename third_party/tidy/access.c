/* access.c -- carry out accessibility checks

  Copyright University of Toronto
  Portions (c) 1998-2009 (W3C) MIT, ERCIM, Keio University
  See tidy.h for the copyright notice.

*/


#include "third_party/tidy/tidy-int.h"
#include "third_party/tidy/access.h"
#include "third_party/tidy/message.h"
#include "third_party/tidy/tags.h"
#include "third_party/tidy/attrs.h"
#include "libc/fmt/conv.h"
#include "third_party/tidy/tmbstr.h"


/*
    The accessibility checks to perform depending on user's desire.

    1. priority 1
    2. priority 1 & 2
    3. priority 1, 2, & 3
*/

/* List of possible image types */
static const ctmbstr imageExtensions[] =
{".jpg", ".gif", ".tif", ".pct", ".pic", ".iff", ".dib",
 ".tga", ".pcx", ".png", ".jpeg", ".tiff", ".bmp"};

#define N_IMAGE_EXTS (sizeof(imageExtensions)/sizeof(ctmbstr))

/* List of possible sound file types */
static const ctmbstr soundExtensions[] =
{".wav", ".au", ".aiff", ".snd", ".ra", ".rm"};

static const int soundExtErrCodes[] =
{
    AUDIO_MISSING_TEXT_WAV,
    AUDIO_MISSING_TEXT_AU,
    AUDIO_MISSING_TEXT_AIFF,
    AUDIO_MISSING_TEXT_SND,
    AUDIO_MISSING_TEXT_RA,
    AUDIO_MISSING_TEXT_RM
};

#define N_AUDIO_EXTS (sizeof(soundExtensions)/sizeof(ctmbstr))

/* List of possible media extensions */
static const ctmbstr mediaExtensions[] =
{".mpg", ".mov", ".asx", ".avi", ".ivf", ".m1v", ".mmm", ".mp2v",
 ".mpa", ".mpe", ".mpeg", ".ram", ".smi", ".smil", ".swf",
 ".wm", ".wma", ".wmv"};

#define N_MEDIA_EXTS (sizeof(mediaExtensions)/sizeof(ctmbstr))

/* List of possible frame sources */
static const ctmbstr frameExtensions[] =
{".htm", ".html", ".shtm", ".shtml", ".cfm", ".cfml",
".asp", ".cgi", ".pl", ".smil"};

#define N_FRAME_EXTS (sizeof(frameExtensions)/sizeof(ctmbstr))

/* List of possible colour values */
static const int colorValues[][3] =
{
  {240, 248, 255 },
  {250, 235, 215 },
  {0, 255, 255 },
  {127, 255, 212 },
  {240, 255, 255 },
  {245, 245, 220 },
  {255, 228, 196 },
  {0, 0, 0 },
  {255, 235, 205 },
  {0, 0, 255 },
  {138, 43, 226 },
  {165, 42, 42 },
  {222, 184, 135 },
  {95, 158, 160 },
  {127, 255, 0 },
  {210, 105, 30 },
  {255, 127, 80 },
  {100, 149, 237 },
  {255, 248, 220 },
  {220, 20, 60 },
  {0, 255, 255 },
  {0, 0, 139 },
  {0, 139, 139 },
  {184, 134, 11 },
  {169, 169, 169 },
  {0, 100, 0 },
  {169, 169, 169 },
  {189, 183, 107 },
  {139, 0, 139 },
  {85, 107, 47 },
  {255, 140, 0 },
  {153, 50, 204 },
  {139, 0, 0 },
  {233, 150, 122 },
  {143, 188, 143 },
  {72, 61, 139 },
  {47, 79, 79 },
  {47, 79, 79 },
  {0, 206, 209 },
  {148, 0, 211 },
  {255, 20, 147 },
  {0, 191, 255 },
  {105, 105, 105 },
  {105, 105, 105 },
  {30, 144, 255 },
  {178, 34, 34 },
  {255, 250, 240 },
  {34, 139, 34 },
  {255, 0, 255 },
  {220, 220, 220 },
  {248, 248, 255 },
  {255, 215, 0 },
  {218, 165, 32 },
  {128, 128, 128 },
  {0, 128, 0 },
  {173, 255, 47 },
  {128, 128, 128 },
  {240, 255, 240 },
  {255, 105, 180 },
  {205, 92, 92 },
  {75, 0, 130 },
  {255, 255, 240 },
  {240, 230, 140 },
  {230, 230, 250 },
  {255, 240, 245 },
  {124, 252, 0 },
  {255, 250, 205 },
  {173, 216, 230 },
  {240, 128, 128 },
  {224, 255, 255 },
  {250, 250, 210 },
  {211, 211, 211 },
  {144, 238, 144 },
  {211, 211, 211 },
  {255, 182, 193 },
  {255, 160, 122 },
  {32, 178, 170 },
  {135, 206, 250 },
  {119, 136, 153 },
  {119, 136, 153 },
  {176, 196, 222 },
  {255, 255, 224 },
  {0, 255, 0 },
  {50, 205, 50 },
  {250, 240, 230 },
  {255, 0, 255 },
  {128, 0, 0 },
  {102, 205, 170 },
  {0, 0, 205 },
  {186, 85, 211 },
  {147, 112, 219 },
  {60, 179, 113 },
  {123, 104, 238 },
  {0, 250, 154 },
  {72, 209, 204 },
  {199, 21, 133 },
  {25, 25, 112 },
  {245, 255, 250 },
  {255, 228, 225 },
  {255, 228, 181 },
  {255, 222, 173 },
  {0, 0, 128 },
  {253, 245, 230 },
  {128, 128, 0 },
  {107, 142, 35 },
  {255, 165, 0 },
  {255, 69, 0 },
  {218, 112, 214 },
  {238, 232, 170 },
  {152, 251, 152 },
  {175, 238, 238 },
  {219, 112, 147 },
  {255, 239, 213 },
  {255, 218, 185 },
  {205, 133, 63 },
  {255, 192, 203 },
  {221, 160, 221 },
  {176, 224, 230 },
  {128, 0, 128 },
  {102, 51, 153 },
  {255, 0, 0 },
  {188, 143, 143 },
  {65, 105, 225 },
  {139, 69, 19 },
  {250, 128, 114 },
  {244, 164, 96 },
  {46, 139, 87 },
  {255, 245, 238 },
  {160, 82, 45 },
  {192, 192, 192 },
  {135, 206, 235 },
  {106, 90, 205 },
  {112, 128, 144 },
  {112, 128, 144 },
  {255, 250, 250 },
  {0, 255, 127 },
  {70, 130, 180 },
  {210, 180, 140 },
  {0, 128, 128 },
  {216, 191, 216 },
  {255, 99, 71 },
  {64, 224, 208 },
  {238, 130, 238 },
  {245, 222, 179 },
  {255, 255, 255 },
  {245, 245, 245 },
  {255, 255, 0 },
  {154, 205, 50 }
};

#define N_COLOR_VALS (sizeof(colorValues)/(sizeof(int[3]))

/* These arrays are used to convert color names to their RGB values */
static const ctmbstr colorNames[] =
{
  "aliceblue",
  "antiquewhite",
  "aqua",
  "aquamarine",
  "azure",
  "beige",
  "bisque",
  "black",
  "blanchedalmond",
  "blue",
  "blueviolet",
  "brown",
  "burlywood",
  "cadetblue",
  "chartreuse",
  "chocolate",
  "coral",
  "cornflowerblue",
  "cornsilk",
  "crimson",
  "cyan",
  "darkblue",
  "darkcyan",
  "darkgoldenrod",
  "darkgray",
  "darkgreen",
  "darkgrey",
  "darkkhaki",
  "darkmagenta",
  "darkolivegreen",
  "darkorange",
  "darkorchid",
  "darkred",
  "darksalmon",
  "darkseagreen",
  "darkslateblue",
  "darkslategray",
  "darkslategrey",
  "darkturquoise",
  "darkviolet",
  "deeppink",
  "deepskyblue",
  "dimgray",
  "dimgrey",
  "dodgerblue",
  "firebrick",
  "floralwhite",
  "forestgreen",
  "fuchsia",
  "gainsboro",
  "ghostwhite",
  "gold",
  "goldenrod",
  "gray",
  "green",
  "greenyellow",
  "grey",
  "honeydew",
  "hotpink",
  "indianred",
  "indigo",
  "ivory",
  "khaki",
  "lavender",
  "lavenderblush",
  "lawngreen",
  "lemonchiffon",
  "lightblue",
  "lightcoral",
  "lightcyan",
  "lightgoldenrodyellow",
  "lightgray",
  "lightgreen",
  "lightgrey",
  "lightpink",
  "lightsalmon",
  "lightseagreen",
  "lightskyblue",
  "lightslategray",
  "lightslategrey",
  "lightsteelblue",
  "lightyellow",
  "lime",
  "limegreen",
  "linen",
  "magenta",
  "maroon",
  "mediumaquamarine",
  "mediumblue",
  "mediumorchid",
  "mediumpurple",
  "mediumseagreen",
  "mediumslateblue",
  "mediumspringgreen",
  "mediumturquoise",
  "mediumvioletred",
  "midnightblue",
  "mintcream",
  "mistyrose",
  "moccasin",
  "navajowhite",
  "navy",
  "oldlace",
  "olive",
  "olivedrab",
  "orange",
  "orangered",
  "orchid",
  "palegoldenrod",
  "palegreen",
  "paleturquoise",
  "palevioletred",
  "papayawhip",
  "peachpuff",
  "peru",
  "pink",
  "plum",
  "powderblue",
  "purple",
  "rebeccapurple",
  "red",
  "rosybrown",
  "royalblue",
  "saddlebrown",
  "salmon",
  "sandybrown",
  "seagreen",
  "seashell",
  "sienna",
  "silver",
  "skyblue",
  "slateblue",
  "slategray",
  "slategrey",
  "snow",
  "springgreen",
  "steelblue",
  "tan",
  "teal",
  "thistle",
  "tomato",
  "turquoise",
  "violet",
  "wheat",
  "white",
  "whitesmoke",
  "yellow",
  "yellowgreen",
};

#define N_COLOR_NAMES (sizeof(colorNames)/sizeof(ctmbstr))
#define N_COLORS N_COLOR_NAMES


/* function prototypes */
static void InitAccessibilityChecks( TidyDocImpl* doc, int level123 );
static void FreeAccessibilityChecks( TidyDocImpl* doc );

static Bool GetRgb( ctmbstr color, int rgb[3] );
static Bool CompareColors( const int rgbBG[3], const int rgbFG[3] );
static int  ctox( tmbchar ch );

/*
static void CheckMapAccess( TidyDocImpl* doc, Node* node, Node* front);
static void GetMapLinks( TidyDocImpl* doc, Node* node, Node* front);
static void CompareAnchorLinks( TidyDocImpl* doc, Node* front, int counter);
static void FindMissingLinks( TidyDocImpl* doc, Node* node, int counter);
*/
static void CheckFormControls( TidyDocImpl* doc, Node* node );
static void MetaDataPresent( TidyDocImpl* doc, Node* node );
static void CheckEmbed( TidyDocImpl* doc, Node* node );
static void CheckListUsage( TidyDocImpl* doc, Node* node );

/*
    IsFilePath attempts to determine whether or not the URI indicated
    by path is a file rather than a TLD. For example, sample.com.au might
    be confused with an audio file.
*/
static Bool IsFilePath( ctmbstr path )
{
    const char *p = path;
    char c;
    typedef enum states { initial, protocol_found, slash_found, file_found } states;
    states state = initial;

    while ( ( c = *p++ ) != 0 && state != file_found )
    {
        switch ( state )
        {
            case initial:
                if ( c == ':' )
                    state = protocol_found;
                break;

            case protocol_found:
                if ( c =='/' )
                    state = slash_found;
                break;

            case slash_found:
                if ( c =='/' )
                    state = protocol_found;
                else
                    state = file_found;
                break;

            default:
                break;
        }

    }

    return state == file_found || state == initial;
}


/*
    GetFileExtension takes a path and returns the extension
    portion of the path (if any).
*/

static void GetFileExtension( ctmbstr path, tmbchar *ext, uint maxExt )
{
    int i = TY_(tmbstrlen)(path) - 1;

    ext[0] = '\0';

    do {
        if ( path[i] == '/' || path[i] == '\\' )
            break;
        else if ( path[i] == '.' )
        {
            TY_(tmbstrncpy)( ext, path+i, maxExt );
            break;
        }
    } while ( --i > 0 );
}

/************************************************************************
* IsImage
*
* Checks if the given filename is an image file.
* Returns 'yes' if it is, 'no' if it's not.
************************************************************************/

static Bool IsImage( ctmbstr iType )
{
    uint i;
    tmbchar ext[20];

    if ( !IsFilePath(iType) ) return 0;

    GetFileExtension( iType, ext, sizeof(ext) );

    /* Compare it to the array of known image file extensions */
    for (i = 0; i < N_IMAGE_EXTS; i++)
    {
        if ( TY_(tmbstrcasecmp)(ext, imageExtensions[i]) == 0 )
            return yes;
    }

    return no;
}


/***********************************************************************
* IsSoundFile
*
* Checks if the given filename is a sound file.
* Returns 'yes' if it is, 'no' if it's not.
***********************************************************************/

static int IsSoundFile( ctmbstr sType )
{
    uint i;
    tmbchar ext[ 20 ];

    if ( !IsFilePath(sType) ) return 0;

    GetFileExtension( sType, ext, sizeof(ext) );

    for (i = 0; i < N_AUDIO_EXTS; i++)
    {
        if ( TY_(tmbstrcasecmp)(ext, soundExtensions[i]) == 0 )
            return soundExtErrCodes[i];
    }
    return 0;
}


/***********************************************************************
* IsValidSrcExtension
*
* Checks if the 'SRC' value within the FRAME element is valid
* The 'SRC' extension must end in ".htm", ".html", ".shtm", ".shtml",
* ".cfm", ".cfml", ".asp", ".cgi", ".pl", or ".smil"
*
* Returns yes if it is, returns no otherwise.
***********************************************************************/

static Bool IsValidSrcExtension( ctmbstr sType )
{
    uint i;
    tmbchar ext[20];

    if ( !IsFilePath(sType) ) return 0;

    GetFileExtension( sType, ext, sizeof(ext) );

    for (i = 0; i < N_FRAME_EXTS; i++)
    {
        if ( TY_(tmbstrcasecmp)(ext, frameExtensions[i]) == 0 )
            return yes;
    }
    return no;
}


/*********************************************************************
* IsValidMediaExtension
*
* Checks to warn the user that synchronized text equivalents are
* required if multimedia is used.
*********************************************************************/

static Bool IsValidMediaExtension( ctmbstr sType )
{
    uint i;
    tmbchar ext[20];

    if ( !IsFilePath(sType) ) return 0;

    GetFileExtension( sType, ext, sizeof(ext) );

    for (i = 0; i < N_MEDIA_EXTS; i++)
    {
        if ( TY_(tmbstrcasecmp)(ext, mediaExtensions[i]) == 0 )
            return yes;
    }
    return no;
}


/************************************************************************
* IsWhitespace
*
* Checks if the given string is all whitespace.
* Returns 'yes' if it is, 'no' if it's not.
************************************************************************/

static Bool IsWhitespace( ctmbstr pString )
{
    Bool isWht = yes;
    ctmbstr cp;

    for ( cp = pString; isWht && cp && *cp; ++cp )
    {
        isWht = TY_(IsWhite)( *cp );
    }
    return isWht;
}

static Bool hasValue( AttVal* av )
{
    return ( av && ! IsWhitespace(av->value) );
}

/***********************************************************************
* IsPlaceholderAlt
*
* Checks to see if there is an image and photo place holder contained
* in the ALT text.
*
* Returns 'yes' if there is, 'no' if not.
***********************************************************************/

static Bool IsPlaceholderAlt( ctmbstr txt )
{
    return ( strstr(txt, "image") != NULL ||
             strstr(txt, "photo") != NULL );
}


/***********************************************************************
* IsPlaceholderTitle
*
* Checks to see if there is an TITLE place holder contained
* in the 'ALT' text.
*
* Returns 'yes' if there is, 'no' if not.

static Bool IsPlaceHolderTitle( ctmbstr txt )
{
    return ( strstr(txt, "title") != NULL );
}
***********************************************************************/


/***********************************************************************
* IsPlaceHolderObject
*
* Checks to see if there is an OBJECT place holder contained
* in the 'ALT' text.
*
* Returns 'yes' if there is, 'no' if not.
***********************************************************************/

static Bool IsPlaceHolderObject( ctmbstr txt )
{
    return ( strstr(txt, "object") != NULL );
}


/**********************************************************
* EndsWithBytes
*
* Checks to see if the ALT text ends with 'bytes'
* Returns 'yes', if true, 'no' otherwise.
**********************************************************/

static Bool EndsWithBytes( ctmbstr txt )
{
    uint len = TY_(tmbstrlen)( txt );
    return ( len >= 5 && TY_(tmbstrcmp)(txt+len-5, "bytes") == 0 );
}


/*******************************************************
* textFromOneNode
*
* Returns a list of characters contained within one
* text node.
*******************************************************/

static ctmbstr textFromOneNode( TidyDocImpl* doc, Node* node )
{
    uint i;
    uint x = 0;
    tmbstr txt = doc->access.text;

    if ( node )
    {
        /* Copy contents of a text node */
        for (i = node->start; i < node->end; ++i, ++x )
        {
            txt[x] = doc->lexer->lexbuf[i];

            /* Check buffer overflow */
            if ( x >= sizeof(doc->access.text)-1 )
                break;
        }
    }

    txt[x] = '\0';
    return txt;
}


/*********************************************************
* getTextNode
*
* Locates text nodes within a container element.
* Retrieves text that are found contained within
* text nodes, and concatenates the text.
*********************************************************/

static void getTextNode( TidyDocImpl* doc, Node* node )
{
    tmbstr txtnod = doc->access.textNode;

    /*
       Continues to traverse through container element until it no
       longer contains any more contents
    */

    /* If the tag of the node is NULL, then grab the text within the node */
    if ( TY_(nodeIsText)(node) )
    {
        uint i;

        /* Retrieves each character found within the text node */
        for (i = node->start; i < node->end; i++)
        {
            /* The text must not exceed buffer */
            if ( doc->access.counter >= TEXTBUF_SIZE-1 )
                return;

            txtnod[ doc->access.counter++ ] = doc->lexer->lexbuf[i];
        }

        /* Traverses through the contents within a container element */
        for ( node = node->content; node != NULL; node = node->next )
            getTextNode( doc, node );
    }
}


/**********************************************************
* getTextNodeClear
*
* Clears the current 'textNode' and reloads it with new
* text.  The textNode must be cleared before use.
**********************************************************/

static tmbstr getTextNodeClear( TidyDocImpl* doc, Node* node )
{
    /* Clears list */
    TidyClearMemory( doc->access.textNode, TEXTBUF_SIZE );
    doc->access.counter = 0;

    getTextNode( doc, node->content );
    return doc->access.textNode;
}

/**********************************************************
* LevelX_Enabled
*
* Tell whether access "X" is enabled.
**********************************************************/

static Bool Level1_Enabled( TidyDocImpl* doc )
{
   return doc->access.PRIORITYCHK == 1 ||
          doc->access.PRIORITYCHK == 2 ||
          doc->access.PRIORITYCHK == 3;
}
static Bool Level2_Enabled( TidyDocImpl* doc )
{
    return doc->access.PRIORITYCHK == 2 ||
           doc->access.PRIORITYCHK == 3;
}
static Bool Level3_Enabled( TidyDocImpl* doc )
{
    return doc->access.PRIORITYCHK == 3;
}

/********************************************************
* CheckColorAvailable
*
* Verify that information conveyed with color is
* available without color.
********************************************************/

static void CheckColorAvailable( TidyDocImpl* doc, Node* node )
{
    if (Level1_Enabled( doc ))
    {
        if ( nodeIsIMG(node) )
            TY_(ReportAccessError)( doc, node, INFORMATION_NOT_CONVEYED_IMAGE );

        else if ( nodeIsAPPLET(node) )
            TY_(ReportAccessError)( doc, node, INFORMATION_NOT_CONVEYED_APPLET );

        else if ( nodeIsOBJECT(node) )
            TY_(ReportAccessError)( doc, node, INFORMATION_NOT_CONVEYED_OBJECT );

        else if ( nodeIsSCRIPT(node) )
            TY_(ReportAccessError)( doc, node, INFORMATION_NOT_CONVEYED_SCRIPT );

        else if ( nodeIsINPUT(node) )
            TY_(ReportAccessError)( doc, node, INFORMATION_NOT_CONVEYED_INPUT );
    }
}

/*********************************************************************
* CheckColorContrast
*
* Checks elements for color contrast.  Must have valid contrast for
* valid visibility.
*
* This logic is extremely fragile as it does not recognize
* the fact that color is inherited by many components and
* that BG and FG colors are often set separately.  E.g. the
* background color may be set by for the body or a table
* or a cell.  The foreground color may be set by any text
* element (p, h1, h2, input, textarea), either explicitly
* or by style.  Ergo, this test will not handle most real
* world cases.  It's a start, however.
*********************************************************************/

static void CheckColorContrast( TidyDocImpl* doc, Node* node )
{
    int rgbBG[3] = {255,255,255};   /* Black text on white BG */

    if (Level3_Enabled( doc ))
    {
        Bool gotBG = yes;
        AttVal* av;

        /* Check for 'BGCOLOR' first to compare with other color attributes */
        for ( av = node->attributes; av; av = av->next )
        {
            if ( attrIsBGCOLOR(av) )
            {
                if ( hasValue(av) )
                    gotBG = GetRgb( av->value, rgbBG );
            }
        }

        /*
           Search for COLOR attributes to compare with background color
           Must have valid colour contrast
        */
        for ( av = node->attributes; gotBG && av != NULL; av = av->next )
        {
            uint errcode = 0;
            if ( attrIsTEXT(av) )
                errcode = COLOR_CONTRAST_TEXT;
            else if ( attrIsLINK(av) )
                errcode = COLOR_CONTRAST_LINK;
            else if ( attrIsALINK(av) )
                errcode = COLOR_CONTRAST_ACTIVE_LINK;
            else if ( attrIsVLINK(av) )
                errcode = COLOR_CONTRAST_VISITED_LINK;

            if ( errcode && hasValue(av) )
            {
                int rgbFG[3] = {0, 0, 0};  /* Black text */

                if ( GetRgb(av->value, rgbFG) &&
                     !CompareColors(rgbBG, rgbFG) )
                {
                    TY_(ReportAccessError)( doc, node, errcode );
                }
            }
        }
    }
}


/**************************************************************
* CompareColors
*
* Compares two RGB colors for good contrast.
**************************************************************/
static int minmax( int i1, int i2 )
{
   return MAX(i1, i2) - MIN(i1,i2);
}
static int brightness( const int rgb[3] )
{
   return ((rgb[0]*299) + (rgb[1]*587) + (rgb[2]*114)) / 1000;
}

static Bool CompareColors( const int rgbBG[3], const int rgbFG[3] )
{
    int brightBG = brightness( rgbBG );
    int brightFG = brightness( rgbFG );

    int diffBright = minmax( brightBG, brightFG );

    int diffColor = minmax( rgbBG[0], rgbFG[0] )
                  + minmax( rgbBG[1], rgbFG[1] )
                  + minmax( rgbBG[2], rgbFG[2] );

    return ( diffBright > 180 &&
             diffColor > 500 );
}


/*********************************************************************
* GetRgb
*
* Gets the red, green and blue values for this attribute for the
* background.
*
* Example: If attribute is BGCOLOR="#121005" then red = 18, green = 16,
* blue = 5.
*********************************************************************/

static Bool GetRgb( ctmbstr color, int rgb[3] )
{
    uint x;

    /* Check if we have a color name */
    for (x = 0; x < N_COLORS; x++)
    {
        if ( strstr(colorNames[x], color) != NULL )
        {
            rgb[0] = colorValues[x][0];
            rgb[1] = colorValues[x][1];
            rgb[2] = colorValues[x][2];
            return yes;
        }
    }

    /*
       No color name so must be hex values
       Is this a number in hexadecimal format?
    */

    /* Must be 7 characters in the RGB value (including '#') */
    if ( TY_(tmbstrlen)(color) == 7 && color[0] == '#' )
    {
        rgb[0] = (ctox(color[1]) * 16) + ctox(color[2]);
        rgb[1] = (ctox(color[3]) * 16) + ctox(color[4]);
        rgb[2] = (ctox(color[5]) * 16) + ctox(color[6]);
        return yes;
    }
    return no;
}



/*******************************************************************
* ctox
*
* Converts a character to a number.
* Example: if given character is 'A' then returns 10.
*
* Returns the number that the character represents. Returns -1 if not a
* valid number.
*******************************************************************/

static int ctox( tmbchar ch )
{
    if ( ch >= '0' && ch <= '9' )
    {
         return ch - '0';
    }
    else if ( ch >= 'a' && ch <= 'f' )
    {
        return ch - 'a' + 10;
    }
    else if ( ch >= 'A' && ch <= 'F' )
    {
        return ch - 'A' + 10;
    }
    return -1;
}


/***********************************************************
* CheckImage
*
* Checks all image attributes for specific elements to
* check for validity of the values contained within
* the attributes.  An appropriate warning message is displayed
* to indicate the error.
***********************************************************/

static void CheckImage( TidyDocImpl* doc, Node* node )
{
    Bool HasAlt = no;
    Bool HasIsMap = no;
    Bool HasLongDesc = no;
    Bool HasDLINK = no;
    Bool HasValidHeight = no;
    Bool HasValidWidthBullet = no;
    Bool HasValidWidthHR = no;
    Bool HasTriggeredMissingLongDesc = no;

    AttVal* av;

    if (Level1_Enabled( doc ))
    {
        /* Checks all image attributes for invalid values within attributes */
        for (av = node->attributes; av != NULL; av = av->next)
        {
            /*
               Checks for valid ALT attribute.
               The length of the alt text must be less than 150 characters
               long.
            */
            if ( attrIsALT(av) )
            {
                if (av->value != NULL)
                {
                    if ((TY_(tmbstrlen)(av->value) < 150) &&
                        (IsPlaceholderAlt (av->value) == no) &&
                        (IsPlaceHolderObject (av->value) == no) &&
                        (EndsWithBytes (av->value) == no) &&
                        (IsImage (av->value) == no))
                    {
                        HasAlt = yes;
                    }

                    else if (TY_(tmbstrlen)(av->value) > 150)
                    {
                        HasAlt = yes;
                        TY_(ReportAccessError)( doc, node, IMG_ALT_SUSPICIOUS_TOO_LONG );
                    }

                    else if (IsImage (av->value) == yes)
                    {
                        HasAlt = yes;
                        TY_(ReportAccessError)( doc, node, IMG_ALT_SUSPICIOUS_FILENAME);
                    }

                    else if (IsPlaceholderAlt (av->value) == yes)
                    {
                        HasAlt = yes;
                        TY_(ReportAccessError)( doc, node, IMG_ALT_SUSPICIOUS_PLACEHOLDER);
                    }

                    else if (EndsWithBytes (av->value) == yes)
                    {
                        HasAlt = yes;
                        TY_(ReportAccessError)( doc, node, IMG_ALT_SUSPICIOUS_FILE_SIZE);
                    }
                }
            }

            /*
               Checks for width values of 'bullets' and 'horizontal
               rules' for validity.

               Valid pixel width for 'bullets' must be < 30, and > 150 for
               horizontal rules.
            */
            else if ( attrIsWIDTH(av) )
            {
                /* Longdesc attribute needed if width attribute is not present. */
                if ( hasValue(av) )
                {
                    int width = atoi( av->value );
                    if ( width < 30 )
                        HasValidWidthBullet = yes;

                    if ( width > 150 )
                        HasValidWidthHR = yes;
                }
            }

            /*
               Checks for height values of 'bullets' and horizontal
               rules for validity.

               Valid pixel height for 'bullets' and horizontal rules
               mustt be < 30.
            */
            else if ( attrIsHEIGHT(av) )
            {
                /* Longdesc attribute needed if height attribute not present. */
                if ( hasValue(av) && atoi(av->value) < 30 )
                    HasValidHeight = yes;
            }

            /*
               Checks for longdesc and determines validity.
               The length of the 'longdesc' must be > 1
            */
            else if ( attrIsLONGDESC(av) )
            {
                if ( hasValue(av) && TY_(tmbstrlen)(av->value) > 1 )
                    HasLongDesc = yes;
              }

            /*
               Checks for 'USEMAP' attribute.  Ensures that
               text links are provided for client-side image maps
            */
            else if ( attrIsUSEMAP(av) )
            {
                if ( hasValue(av) )
                    doc->access.HasUseMap = yes;
            }

            else if ( attrIsISMAP(av) )
            {
                HasIsMap = yes;
            }
        }


        /*
            Check to see if a dLINK is present.  The ANCHOR element must
            be present following the IMG element.  The text found between
            the ANCHOR tags must be < 6 characters long, and must contain
            the letter 'd'.
        */
        if ( nodeIsA(node->next) )
        {
            node = node->next;

            /*
                Node following the anchor must be a text node
                for dLINK to exist
            */

            if (node->content != NULL && (node->content)->tag == NULL)
            {
                /* Number of characters found within the text node */
                ctmbstr word = textFromOneNode( doc, node->content);

                if ((TY_(tmbstrcmp)(word,"d") == 0)||
                    (TY_(tmbstrcmp)(word,"D") == 0))
                {
                    HasDLINK = yes;
                }
            }
        }

        /*
            Special case check for dLINK.  This will occur if there is
            whitespace between the <img> and <a> elements.  Ignores
            whitespace and continues check for dLINK.
        */

        if ( node->next && !node->next->tag )
        {
            node = node->next;

            if ( nodeIsA(node->next) )
            {
                node = node->next;

                /*
                    Node following the ANCHOR must be a text node
                    for dLINK to exist
                */
                if (node->content != NULL && node->content->tag == NULL)
                {
                    /* Number of characters found within the text node */
                    ctmbstr word = textFromOneNode( doc, node->content );

                    if ((TY_(tmbstrcmp)(word, "d") == 0)||
                        (TY_(tmbstrcmp)(word, "D") == 0))
                    {
                        HasDLINK = yes;
                    }
                }
            }
        }

        if ((HasAlt == no)&&
            (HasValidWidthBullet == yes)&&
            (HasValidHeight == yes))
        {
        }

        if ((HasAlt == no)&&
            (HasValidWidthHR == yes)&&
            (HasValidHeight == yes))
        {
        }

        if (HasAlt == no)
        {
            TY_(ReportAccessError)( doc, node, IMG_MISSING_ALT);
        }

        if ((HasLongDesc == no)&&
            (HasValidHeight ==yes)&&
            ((HasValidWidthHR == yes)||
             (HasValidWidthBullet == yes)))
        {
            HasTriggeredMissingLongDesc = yes;
        }

        if (HasTriggeredMissingLongDesc == no)
        {
            if ((HasDLINK == yes)&&
                (HasLongDesc == no))
            {
                TY_(ReportAccessError)( doc, node, IMG_MISSING_LONGDESC);
            }

            if ((HasLongDesc == yes)&&
                (HasDLINK == no))
            {
                TY_(ReportAccessError)( doc, node, IMG_MISSING_DLINK);
            }

            if ((HasLongDesc == no)&&
                (HasDLINK == no))
            {
                TY_(ReportAccessError)( doc, node, IMG_MISSING_LONGDESC_DLINK);
            }
        }

        if (HasIsMap == yes)
        {
            TY_(ReportAccessError)( doc, node, IMAGE_MAP_SERVER_SIDE_REQUIRES_CONVERSION);

            TY_(ReportAccessError)( doc, node, IMG_MAP_SERVER_REQUIRES_TEXT_LINKS);
        }
    }
}


/***********************************************************
* CheckApplet
*
* Checks APPLET element to check for validity pertaining
* the 'ALT' attribute.  An appropriate warning message is
* displayed  to indicate the error. An appropriate warning
* message is displayed to indicate the error.  If no 'ALT'
* text is present, then there must be alternate content
* within the APPLET element.
***********************************************************/

static void CheckApplet( TidyDocImpl* doc, Node* node )
{
    Bool HasAlt = no;
    Bool HasDescription = no;

    AttVal* av;

    if (Level1_Enabled( doc ))
    {
        /* Checks for attributes within the APPLET element */
        for (av = node->attributes; av != NULL; av = av->next)
        {
            /*
               Checks for valid ALT attribute.
               The length of the alt text must be > 4 characters in length
               but must be < 150 characters long.
            */

            if ( attrIsALT(av) )
            {
                if (av->value != NULL)
                {
                    HasAlt = yes;
                }
            }
        }

        if (HasAlt == no)
        {
            /* Must have alternate text representation for that element */
            if (node->content != NULL)
            {
                ctmbstr word = NULL;

                if ( node->content->tag == NULL )
                    word = textFromOneNode( doc, node->content);

                if ( node->content->content != NULL &&
                     node->content->content->tag == NULL )
                {
                    word = textFromOneNode( doc, node->content->content);
                }

                if ( word != NULL && !IsWhitespace(word) )
                    HasDescription = yes;
            }
        }

        if ( !HasDescription && !HasAlt )
        {
            TY_(ReportAccessError)( doc, node, APPLET_MISSING_ALT );
        }
    }
}


/*******************************************************************
* CheckObject
*
* Checks to verify whether the OBJECT element contains
* 'ALT' text, and to see that the sound file selected is
* of a valid sound file type.  OBJECT must have an alternate text
* representation.
*******************************************************************/

static void CheckObject( TidyDocImpl* doc, Node* node )
{
    Bool HasAlt = no;
    Bool HasDescription = no;

    if (Level1_Enabled( doc ))
    {
        if ( node->content != NULL)
        {
            if ( node->content->type != TextNode )
            {
                Node* tnode = node->content;
                AttVal* av;

                for ( av=tnode->attributes; av; av = av->next )
                {
                    if ( attrIsALT(av) )
                    {
                        HasAlt = yes;
                        break;
                    }
                }
            }

            /* Must have alternate text representation for that element */
            if ( !HasAlt )
            {
                ctmbstr word = NULL;

                if ( TY_(nodeIsText)(node->content) )
                    word = textFromOneNode( doc, node->content );

                if ( word == NULL &&
                     TY_(nodeIsText)(node->content->content) )
                {
                    word = textFromOneNode( doc, node->content->content );
                }

                if ( word != NULL && !IsWhitespace(word) )
                    HasDescription = yes;
            }
        }

        if ( !HasAlt && !HasDescription )
        {
            TY_(ReportAccessError)( doc, node, OBJECT_MISSING_ALT );
        }
    }
}


/***************************************************************
* CheckMissingStyleSheets
*
* Ensures that stylesheets are used to control the presentation.
***************************************************************/

static Bool CheckMissingStyleSheets( TidyDocImpl* doc, Node* node )
{
    AttVal* av;
    Node* content;
    Bool sspresent = no;

    for ( content = node->content;
          !sspresent && content != NULL;
          content = content->next )
    {
        sspresent = ( nodeIsLINK(content)  ||
                      nodeIsSTYLE(content) ||
                      nodeIsFONT(content)  ||
                      nodeIsBASEFONT(content) );

        for ( av = content->attributes;
              !sspresent && av != NULL;
              av = av->next )
        {
            sspresent = ( attrIsSTYLE(av) || attrIsTEXT(av)  ||
                          attrIsVLINK(av) || attrIsALINK(av) ||
                          attrIsLINK(av) );

            if ( !sspresent && attrIsREL(av) )
            {
                sspresent = AttrValueIs(av, "stylesheet");
            }
        }

        if ( ! sspresent )
            sspresent = CheckMissingStyleSheets( doc, content );
    }
    return sspresent;
}


/*******************************************************************
* CheckFrame
*
* Checks if the URL is valid and to check if a 'LONGDESC' is needed
* within the FRAME element.  If a 'LONGDESC' is needed, the value must
* be valid. The URL must end with the file extension, htm, or html.
* Also, checks to ensure that the 'SRC' and 'TITLE' values are valid.
*******************************************************************/

static void CheckFrame( TidyDocImpl* doc, Node* node )
{
    Bool HasTitle = no;
    AttVal* av;

    doc->access.numFrames++;

    if (Level1_Enabled( doc ))
    {
        /* Checks for attributes within the FRAME element */
        for (av = node->attributes; av != NULL; av = av->next)
        {
            /* Checks if 'LONGDESC' value is valid only if present */
            if ( attrIsLONGDESC(av) )
            {
                if ( hasValue(av) && TY_(tmbstrlen)(av->value) > 1 )
                {
                    doc->access.HasCheckedLongDesc++;
                }
            }

            /* Checks for valid 'SRC' value within the frame element */
            else if ( attrIsSRC(av) )
            {
                if ( hasValue(av) && !IsValidSrcExtension(av->value) )
                {
                    TY_(ReportAccessError)( doc, node, FRAME_SRC_INVALID );
                }
            }

            /* Checks for valid 'TITLE' value within frame element */
            else if ( attrIsTITLE(av) )
            {
                if ( hasValue(av) )
                    HasTitle = yes;

                if ( !HasTitle )
                {
                    if ( av->value == NULL || TY_(tmbstrlen)(av->value) == 0 )
                    {
                        HasTitle = yes;
                        TY_(ReportAccessError)( doc, node, FRAME_TITLE_INVALID_NULL);
                    }
                    else
                    {
                        if ( IsWhitespace(av->value) && TY_(tmbstrlen)(av->value) > 0 )
                        {
                            HasTitle = yes;
                            TY_(ReportAccessError)( doc, node, FRAME_TITLE_INVALID_SPACES );
                        }
                    }
                }
            }
        }

        if ( !HasTitle )
        {
            TY_(ReportAccessError)( doc, node, FRAME_MISSING_TITLE);
        }

        if ( doc->access.numFrames==3 && doc->access.HasCheckedLongDesc<3 )
        {
            doc->access.numFrames = 0;
            TY_(ReportAccessError)( doc, node, FRAME_MISSING_LONGDESC );
        }
    }
}


/****************************************************************
* CheckIFrame
*
* Checks if 'SRC' value is valid.  Must end in appropriate
* file extension.
****************************************************************/

static void CheckIFrame( TidyDocImpl* doc, Node* node )
{
    if (Level1_Enabled( doc ))
    {
        /* Checks for valid 'SRC' value within the IFRAME element */
        AttVal* av = attrGetSRC( node );
        if ( hasValue(av) )
        {
            if ( !IsValidSrcExtension(av->value) )
                TY_(ReportAccessError)( doc, node, FRAME_SRC_INVALID );
        }
    }
}


/**********************************************************************
* CheckAnchorAccess
*
* Checks that the sound file is valid, and to ensure that
* text transcript is present describing the 'HREF' within the
* ANCHOR element.  Also checks to see ensure that the 'TARGET' attribute
* (if it exists) is not NULL and does not contain '_new' or '_blank'.
**********************************************************************/

static void CheckAnchorAccess( TidyDocImpl* doc, Node* node )
{
    AttVal* av;
    Bool HasDescription = no;
    Bool HasTriggeredLink = no;

    /* Checks for attributes within the ANCHOR element */
    for ( av = node->attributes; av != NULL; av = av->next )
    {
        if (Level1_Enabled( doc ))
        {
            /* Must be of valid sound file type */
            if ( attrIsHREF(av) )
            {
                if ( hasValue(av) )
                {
                    tmbchar ext[ 20 ];
                    GetFileExtension (av->value, ext, sizeof(ext) );

                    /* Checks to see if multimedia is used */
                    if ( IsValidMediaExtension(av->value) )
                    {
                        TY_(ReportAccessError)( doc, node, MULTIMEDIA_REQUIRES_TEXT );
                    }

                    /*
                        Checks for validity of sound file, and checks to see if
                        the file is described within the document, or by a link
                        that is present which gives the description.
                    */
                    if ( TY_(tmbstrlen)(ext) < 6 && TY_(tmbstrlen)(ext) > 0 )
                    {
                        int errcode = IsSoundFile( av->value );
                        if ( errcode )
                        {
                            if (node->next != NULL)
                            {
                                if (node->next->tag == NULL)
                                {
                                    ctmbstr word = textFromOneNode( doc, node->next);

                                    /* Must contain at least one letter in the text */
                                    if (IsWhitespace (word) == no)
                                    {
                                        HasDescription = yes;
                                    }
                                }
                            }

                            /* Must contain text description of sound file */
                            if ( !HasDescription )
                            {
                                TY_(ReportAccessError)( doc, node, errcode );
                            }
                        }
                    }
                }
            }
        }

        if (Level2_Enabled( doc ))
        {
            /* Checks 'TARGET' attribute for validity if it exists */
            if ( attrIsTARGET(av) )
            {
                if (AttrValueIs(av, "_new"))
                {
                    TY_(ReportAccessError)( doc, node, NEW_WINDOWS_REQUIRE_WARNING_NEW);
                }
                else if (AttrValueIs(av, "_blank"))
                {
                    TY_(ReportAccessError)( doc, node, NEW_WINDOWS_REQUIRE_WARNING_BLANK);
                }
            }
        }
    }

    if (Level2_Enabled( doc ))
    {
        if ((node->content != NULL)&&
            (node->content->tag == NULL))
        {
            ctmbstr word = textFromOneNode( doc, node->content);

            if ((word != NULL)&&
                (IsWhitespace (word) == no))
            {
                if (TY_(tmbstrcmp) (word, "more") == 0)
                {
                    HasTriggeredLink = yes;
                }

                if (TY_(tmbstrcmp) (word, "click here") == 0)
                {
                    TY_(ReportAccessError)( doc, node, LINK_TEXT_NOT_MEANINGFUL_CLICK_HERE);
                }

                if (HasTriggeredLink == no)
                {
                    if (TY_(tmbstrlen)(word) < 6)
                    {
                        TY_(ReportAccessError)( doc, node, LINK_TEXT_NOT_MEANINGFUL);
                    }
                }

                if (TY_(tmbstrlen)(word) > 60)
                {
                    TY_(ReportAccessError)( doc, node, LINK_TEXT_TOO_LONG);
                }

            }
        }

        if (node->content == NULL)
        {
            TY_(ReportAccessError)( doc, node, LINK_TEXT_MISSING);
        }
    }
}


/************************************************************
* CheckArea
*
* Checks attributes within the AREA element to
* determine if the 'ALT' text and 'HREF' values are valid.
* Also checks to see ensure that the 'TARGET' attribute
* (if it exists) is not NULL and does not contain '_new'
* or '_blank'.
************************************************************/

static void CheckArea( TidyDocImpl* doc, Node* node )
{
    Bool HasAlt = no;
    AttVal* av;

    /* Checks all attributes within the AREA element */
    for (av = node->attributes; av != NULL; av = av->next)
    {
        if (Level1_Enabled( doc ))
        {
            /*
              Checks for valid ALT attribute.
              The length of the alt text must be > 4 characters long
              but must be less than 150 characters long.
            */

            if ( attrIsALT(av) )
            {
                /* The check for validity */
                if (av->value != NULL)
                {
                    HasAlt = yes;
                }
            }
        }

        if (Level2_Enabled( doc ))
        {
            if ( attrIsTARGET(av) )
            {
                if (AttrValueIs(av, "_new"))
                {
                    TY_(ReportAccessError)( doc, node, NEW_WINDOWS_REQUIRE_WARNING_NEW);
                }
                else if (AttrValueIs(av, "_blank"))
                {
                    TY_(ReportAccessError)( doc, node, NEW_WINDOWS_REQUIRE_WARNING_BLANK);
                }
            }
        }
    }

    if (Level1_Enabled( doc ))
    {
        /* AREA must contain alt text */
        if (HasAlt == no)
        {
            TY_(ReportAccessError)( doc, node, AREA_MISSING_ALT);
        }
    }
}


/***************************************************
* CheckScript
*
* Checks the SCRIPT element to ensure that a
* NOSCRIPT section follows the SCRIPT.
***************************************************/

static void CheckScriptAcc( TidyDocImpl* doc, Node* node )
{
    if (Level1_Enabled( doc ))
    {
        /* NOSCRIPT element must appear immediately following SCRIPT element */
        if ( node->next == NULL || !nodeIsNOSCRIPT(node->next) )
        {
            TY_(ReportAccessError)( doc, node, SCRIPT_MISSING_NOSCRIPT);
        }
    }
}


/**********************************************************
* CheckRows
*
* Check to see that each table has a row of headers if
* a column of columns doesn't exist.
**********************************************************/

static void CheckRows( TidyDocImpl* doc, Node* node )
{
    int numTR = 0;
    int numValidTH = 0;

    doc->access.CheckedHeaders++;

    for (; node != NULL; node = node->next )
    {
        numTR++;
        if ( nodeIsTH(node->content) )
        {
            doc->access.HasTH = yes;
            if ( TY_(nodeIsText)(node->content->content) )
            {
                ctmbstr word = textFromOneNode( doc, node->content->content);
                if ( !IsWhitespace(word) )
                    numValidTH++;
            }
        }
    }

    if (numTR == numValidTH)
        doc->access.HasValidRowHeaders = yes;

    if ( numTR >= 2 &&
         numTR > numValidTH &&
         numValidTH >= 2 &&
         doc->access.HasTH == yes )
        doc->access.HasInvalidRowHeader = yes;
}


/**********************************************************
* CheckColumns
*
* Check to see that each table has a column of headers if
* a row of columns doesn't exist.
**********************************************************/

static void CheckColumns( TidyDocImpl* doc, Node* node )
{
    Node* tnode;
    int numTH = 0;
    Bool isMissingHeader = no;

    doc->access.CheckedHeaders++;

    /* Table must have row of headers if headers for columns don't exist */
    if ( nodeIsTH(node->content) )
    {
        doc->access.HasTH = yes;

        for ( tnode = node->content; tnode; tnode = tnode->next )
        {
            if ( nodeIsTH(tnode) )
            {
                if ( TY_(nodeIsText)(tnode->content) )
                {
                    ctmbstr word = textFromOneNode( doc, tnode->content);
                    if ( !IsWhitespace(word) )
                        numTH++;
                }
            }
            else
            {
                isMissingHeader = yes;
            }
        }
    }

    if ( !isMissingHeader && numTH > 0 )
        doc->access.HasValidColumnHeaders = yes;

    if ( isMissingHeader && numTH >= 2 )
        doc->access.HasInvalidColumnHeader = yes;
}


/*****************************************************
* CheckTH
*
* Checks to see if the header provided for a table
* requires an abbreviation. (only required if the
* length of the header is greater than 15 characters)
*****************************************************/

static void CheckTH( TidyDocImpl* doc, Node* node )
{
    Bool HasAbbr = no;
    ctmbstr word = NULL;
    AttVal* av;

    if (Level3_Enabled( doc ))
    {
        /* Checks TH element for 'ABBR' attribute */
        for (av = node->attributes; av != NULL; av = av->next)
        {
            if ( attrIsABBR(av) )
            {
                /* Value must not be NULL and must be less than 15 characters */
                if ((av->value != NULL)&&
                    (IsWhitespace (av->value) == no))
                {
                    HasAbbr = yes;
                }

                if ((av->value == NULL)||
                    (TY_(tmbstrlen)(av->value) == 0))
                {
                    HasAbbr = yes;
                    TY_(ReportAccessError)( doc, node, TABLE_MAY_REQUIRE_HEADER_ABBR_NULL);
                }

                if ((IsWhitespace (av->value) == yes)&&
                    (TY_(tmbstrlen)(av->value) > 0))
                {
                    HasAbbr = yes;
                    TY_(ReportAccessError)( doc, node, TABLE_MAY_REQUIRE_HEADER_ABBR_SPACES);
                }
            }
        }

        /* If the header is greater than 15 characters, an abbreviation is needed */
        word = textFromOneNode( doc, node->content);

        if ((word != NULL)&&
            (IsWhitespace (word) == no))
        {
            /* Must have 'ABBR' attribute if header is > 15 characters */
            if ((TY_(tmbstrlen)(word) > 15)&&
                (HasAbbr == no))
            {
                TY_(ReportAccessError)( doc, node, TABLE_MAY_REQUIRE_HEADER_ABBR);
            }
        }
    }
}


/*****************************************************************
* CheckMultiHeaders
*
* Layout tables should make sense when linearized.
* TABLE must contain at least one TH element.
* This technique applies only to tables used for layout purposes,
* not to data tables. Checks for column of multiple headers.
*****************************************************************/

static void CheckMultiHeaders( TidyDocImpl* doc, Node* node )
{
    Node* TNode;
    Node* temp;

    Bool validColSpanRows = yes;
    Bool validColSpanColumns = yes;

    int flag = 0;

    if (Level1_Enabled( doc ))
    {
        if (node->content != NULL)
        {
            TNode = node->content;

            /*
               Checks for column of multiple headers found
               within a data table.
            */
            while (TNode != NULL)
            {
                if ( nodeIsTR(TNode) )
                {
                    flag = 0; /* Issue #168 - access test 5-2-1-2 */
                    if (TNode->content != NULL)
                    {
                        temp = TNode->content;

                        /* The number of TH elements found within TR element */
                        if (flag == 0)
                        {
                            while (temp != NULL)
                            {
                                /*
                                   Must contain at least one TH element
                                   within in the TR element
                                */
                                if ( nodeIsTH(temp) )
                                {
                                    AttVal* av;
                                    for (av = temp->attributes; av != NULL; av = av->next)
                                    {
                                        if ( attrIsCOLSPAN(av)
                                             && (atoi(av->value) > 1) )
                                            validColSpanColumns = no;

                                        if ( attrIsROWSPAN(av)
                                             && (atoi(av->value) > 1) )
                                            validColSpanRows = no;
                                    }
                                }

                                temp = temp->next;
                            }

                            flag = 1;
                        }
                    }
                }

                TNode = TNode->next;
            }

            /* Displays HTML 4 Table Algorithm when multiple column of headers used */
            if (validColSpanRows == no)
            {
                TY_(ReportAccessError)( doc, node, DATA_TABLE_REQUIRE_MARKUP_ROW_HEADERS );
                TY_(Dialogue)( doc, TEXT_HTML_T_ALGORITHM );
            }

            if (validColSpanColumns == no)
            {
                TY_(ReportAccessError)( doc, node, DATA_TABLE_REQUIRE_MARKUP_COLUMN_HEADERS );
                TY_(Dialogue)( doc, TEXT_HTML_T_ALGORITHM );
            }
        }
    }
}


/****************************************************
* CheckTable
*
* Checks the TABLE element to ensure that the
* table is not missing any headers.  Must have either
* a row or column of headers.
****************************************************/

static void CheckTable( TidyDocImpl* doc, Node* node )
{
    Node* TNode;
    Node* temp;

    tmbstr word = NULL;

    int numTR = 0;

    Bool HasSummary = no;
    Bool HasCaption = no;

    if (Level3_Enabled( doc ))
    {
        AttVal* av;
        /* Table must have a 'SUMMARY' describing the purpose of the table */
        for (av = node->attributes; av != NULL; av = av->next)
        {
            if ( attrIsSUMMARY(av) )
            {
                if ( hasValue(av) )
                {
                    HasSummary = yes;

                    if (AttrContains(av, "summary") &&
                        AttrContains(av, "table"))
                    {
                        TY_(ReportAccessError)( doc, node, TABLE_SUMMARY_INVALID_PLACEHOLDER );
                    }
                }

                if ( av->value == NULL || TY_(tmbstrlen)(av->value) == 0 )
                {
                    HasSummary = yes;
                    TY_(ReportAccessError)( doc, node, TABLE_SUMMARY_INVALID_NULL );
                }
                else if ( IsWhitespace(av->value) && TY_(tmbstrlen)(av->value) > 0 )
                {
                    HasSummary = yes;
                    TY_(ReportAccessError)( doc, node, TABLE_SUMMARY_INVALID_SPACES );
                }
            }
        }

        /* TABLE must have content. */
        if (node->content == NULL)
        {
            TY_(ReportAccessError)( doc, node, DATA_TABLE_MISSING_HEADERS);

            return;
        }
    }

    if (Level1_Enabled( doc ))
    {
        /* Checks for multiple headers */
        CheckMultiHeaders( doc, node );
    }

    if (Level2_Enabled( doc ))
    {
        /* Table must have a CAPTION describing the purpose of the table */
        if ( nodeIsCAPTION(node->content) )
        {
            TNode = node->content;

            if (TNode->content && TNode->content->tag == NULL)
            {
                word = getTextNodeClear( doc, TNode);
            }

            if ( !IsWhitespace(word) )
            {
                HasCaption = yes;
            }
        }

        if (HasCaption == no)
        {
            TY_(ReportAccessError)( doc, node, TABLE_MISSING_CAPTION);
        }
    }


    if (node->content != NULL)
    {
        if ( nodeIsCAPTION(node->content) && nodeIsTR(node->content->next) )
        {
            CheckColumns( doc, node->content->next );
        }
        else if ( nodeIsTR(node->content) )
        {
            CheckColumns( doc, node->content );
        }
    }

    if ( ! doc->access.HasValidColumnHeaders )
    {
        if (node->content != NULL)
        {
            if ( nodeIsCAPTION(node->content) && nodeIsTR(node->content->next) )
            {
                CheckRows( doc, node->content->next);
            }
            else if ( nodeIsTR(node->content) )
            {
                CheckRows( doc, node->content);
            }
        }
    }


    if (Level3_Enabled( doc ))
    {
        /* Suppress warning for missing 'SUMMARY for HTML 2.0 and HTML 3.2 */
        if (HasSummary == no)
        {
            TY_(ReportAccessError)( doc, node, TABLE_MISSING_SUMMARY);
        }
    }

    if (Level2_Enabled( doc ))
    {
        if (node->content != NULL)
        {
            temp = node->content;

            while (temp != NULL)
            {
                if ( nodeIsTR(temp) )
                {
                    numTR++;
                }

                temp = temp->next;
            }

            if (numTR == 1)
            {
                TY_(ReportAccessError)( doc, node, LAYOUT_TABLES_LINEARIZE_PROPERLY);
            }
        }

        if ( doc->access.HasTH )
        {
            TY_(ReportAccessError)( doc, node, LAYOUT_TABLE_INVALID_MARKUP);
        }
    }

    if (Level1_Enabled( doc ))
    {
        if ( doc->access.CheckedHeaders == 2 )
        {
            if ( !doc->access.HasValidRowHeaders &&
                 !doc->access.HasValidColumnHeaders &&
                 !doc->access.HasInvalidRowHeader &&
                 !doc->access.HasInvalidColumnHeader  )
            {
                TY_(ReportAccessError)( doc, node, DATA_TABLE_MISSING_HEADERS);
            }

            if ( !doc->access.HasValidRowHeaders &&
                 doc->access.HasInvalidRowHeader )
            {
                TY_(ReportAccessError)( doc, node, DATA_TABLE_MISSING_HEADERS_ROW);
            }

            if ( !doc->access.HasValidColumnHeaders &&
                 doc->access.HasInvalidColumnHeader )
            {
                TY_(ReportAccessError)( doc, node, DATA_TABLE_MISSING_HEADERS_COLUMN);
            }
        }
    }
}


/***************************************************
* CheckASCII
*
* Checks for valid text equivalents for XMP and PRE
* elements for ASCII art.  Ensures that there is
* a skip over link to skip multi-lined ASCII art.
***************************************************/

static void CheckASCII( TidyDocImpl* doc, Node* node )
{
    Node* temp1;
    Node* temp2;

    tmbstr skipOver = NULL;
    Bool IsAscii = no;
    int HasSkipOverLink = 0;

    uint i, x;
    int newLines = -1;
    tmbchar compareLetter;
    int matchingCount = 0;
    AttVal* av;

    if (Level1_Enabled( doc ) && node->content)
    {
        /*
           Checks the text within the PRE and XMP tags to see if ascii
           art is present
        */
        for (i = node->content->start + 1; i < node->content->end; i++)
        {
            matchingCount = 0;

            /* Counts the number of lines of text */
            if (doc->lexer->lexbuf[i] == '\n')
            {
                newLines++;
            }

            compareLetter = doc->lexer->lexbuf[i];

            /* Counts consecutive character matches */
            for (x = i; x < i + 5; x++)
            {
                if (doc->lexer->lexbuf[x] == compareLetter)
                {
                    matchingCount++;
                }

                else
                {
                    break;
                }
            }

            /* Must have at least 5 consecutive character matches */
            if (matchingCount >= 5)
            {
                break;
            }
        }

        /*
           Must have more than 6 lines of text OR 5 or more consecutive
           letters that are the same for there to be ascii art
        */
        if (newLines >= 6 || matchingCount >= 5)
        {
            IsAscii = yes;
        }

        /* Checks for skip over link if ASCII art is present */
        if (IsAscii == yes)
        {
            if (node->prev != NULL && node->prev->prev != NULL)
            {
                temp1 = node->prev->prev;

                /* Checks for 'HREF' attribute */
                for (av = temp1->attributes; av != NULL; av = av->next)
                {
                    if ( attrIsHREF(av) && hasValue(av) )
                    {
                        skipOver = av->value;
                        HasSkipOverLink++;
                    }
                }
            }
        }
    }

    if (Level2_Enabled( doc ))
    {
        /*
           Checks for A element following PRE to ensure proper skipover link
           only if there is an A element preceding PRE.
        */
        if (HasSkipOverLink == 1)
        {
            if ( nodeIsA(node->next) )
            {
                temp2 = node->next;

                /* Checks for 'NAME' attribute */
                for (av = temp2->attributes; av != NULL; av = av->next)
                {
                    if ( attrIsNAME(av) && hasValue(av) )
                    {
                        /*
                           Value within the 'HREF' attribute must be the same
                           as the value within the 'NAME' attribute for valid
                           skipover.
                        */
                        if ( strstr(skipOver, av->value) != NULL )
                        {
                            HasSkipOverLink++;
                        }
                    }
                }
            }
        }

        if (IsAscii == yes)
        {
            TY_(ReportAccessError)( doc, node, ASCII_REQUIRES_DESCRIPTION);
            if (Level3_Enabled( doc ) && (HasSkipOverLink < 2))
                TY_(ReportAccessError)( doc, node, SKIPOVER_ASCII_ART);
        }

    }
}


/***********************************************************
* CheckFormControls
*
* <form> must have valid 'FOR' attribute, and <label> must
* have valid 'ID' attribute for valid form control.
***********************************************************/

static void CheckFormControls( TidyDocImpl* doc, Node* node )
{
    if ( !doc->access.HasValidFor &&
         doc->access.HasValidId )
    {
        TY_(ReportAccessError)( doc, node, ASSOCIATE_LABELS_EXPLICITLY_FOR);
    }

    if ( !doc->access.HasValidId &&
         doc->access.HasValidFor )
    {
        TY_(ReportAccessError)( doc, node, ASSOCIATE_LABELS_EXPLICITLY_ID);
    }

    if ( !doc->access.HasValidId &&
         !doc->access.HasValidFor )
    {
        TY_(ReportAccessError)( doc, node, ASSOCIATE_LABELS_EXPLICITLY);
    }
}


/************************************************************
* CheckLabel
*
* Check for valid 'FOR' attribute within the LABEL element
************************************************************/

static void CheckLabel( TidyDocImpl* doc, Node* node )
{
    if (Level2_Enabled( doc ))
    {
        /* Checks for valid 'FOR' attribute */
        AttVal* av = attrGetFOR( node );
        if ( hasValue(av) )
            doc->access.HasValidFor = yes;

        if ( ++doc->access.ForID == 2 )
        {
            doc->access.ForID = 0;
            CheckFormControls( doc, node );
        }
    }
}


/************************************************************
* CheckInputLabel
*
* Checks for valid 'ID' attribute within the INPUT element.
* Checks to see if there is a LABEL directly before
* or after the INPUT element determined by the 'TYPE'.
* Each INPUT element must have a LABEL describing the form.
************************************************************/

static void CheckInputLabel( TidyDocImpl* doc, Node* node )
{
    if (Level2_Enabled( doc ))
    {
        AttVal* av;

        /* Checks attributes within the INPUT element */
        for (av = node->attributes; av != NULL; av = av->next)
        {
            /* Must have valid 'ID' value */
            if ( attrIsID(av) && hasValue(av) )
                doc->access.HasValidId = yes;
        }

        if ( ++doc->access.ForID == 2 )
        {
            doc->access.ForID = 0;
            CheckFormControls( doc, node );
        }
    }
}


/***************************************************************
* CheckInputAttributes
*
* INPUT element must have a valid 'ALT' attribute if the
* 'VALUE' attribute is present.
***************************************************************/

static void CheckInputAttributes( TidyDocImpl* doc, Node* node )
{
    Bool HasAlt = no;
    Bool MustHaveAlt = no;
    AttVal* av;

    /* Checks attributes within the INPUT element */
    for (av = node->attributes; av != NULL; av = av->next)
    {
        /* 'VALUE' must be found if the 'TYPE' is 'text' or 'checkbox' */
        if ( attrIsTYPE(av) && hasValue(av) )
        {
            if (Level1_Enabled( doc ))
            {
                if (AttrValueIs(av, "image"))
                {
                    MustHaveAlt = yes;
                }
            }

        }

        if ( attrIsALT(av) && hasValue(av) )
        {
            HasAlt = yes;
        }
    }

    if ( MustHaveAlt && !HasAlt )
    {
        TY_(ReportAccessError)( doc, node, IMG_BUTTON_MISSING_ALT );
    }

}


/***************************************************************
* CheckFrameSet
*
* Frameset must have valid NOFRAME section.  Must contain some
* text but must not contain information telling user to update
* browsers,
***************************************************************/

static void CheckFrameSet( TidyDocImpl* doc, Node* node )
{
    Node* temp;
    Bool HasNoFrames = no;

    if (Level1_Enabled( doc ))
    {
        if ( doc->badAccess & BA_INVALID_LINK_NOFRAMES )
        {
           TY_(ReportAccessError)( doc, node, NOFRAMES_INVALID_LINK);
           doc->badAccess &= ~BA_INVALID_LINK_NOFRAMES; /* emit only once */
        }
        for ( temp = node->content; temp != NULL ; temp = temp->next )
        {
            if ( nodeIsNOFRAMES(temp) )
            {
                HasNoFrames = yes;

                if ( temp->content && nodeIsP(temp->content->content) )
                {
                    Node* para = temp->content->content;
                    if ( TY_(nodeIsText)(para->content) )
                    {
                        ctmbstr word = textFromOneNode( doc, para->content );
                        if ( word && strstr(word, "browser") != NULL )
                            TY_(ReportAccessError)( doc, para, NOFRAMES_INVALID_CONTENT );
                    }
                }
                else if (temp->content == NULL)
                    TY_(ReportAccessError)( doc, temp, NOFRAMES_INVALID_NO_VALUE);
                else if ( temp->content &&
                          IsWhitespace(textFromOneNode(doc, temp->content)) )
                    TY_(ReportAccessError)( doc, temp, NOFRAMES_INVALID_NO_VALUE);
            }
        }

        if (HasNoFrames == no)
            TY_(ReportAccessError)( doc, node, FRAME_MISSING_NOFRAMES);
    }
}


/***********************************************************
* CheckHeaderNesting
*
* Checks for heading increases and decreases.  Headings must
* not increase by more than one header level, but may
* decrease at from any level to any level.  Text within
* headers must not be more than 20 words in length.
***********************************************************/

static void CheckHeaderNesting( TidyDocImpl* doc, Node* node )
{
    Node* temp;
    uint i;
    int numWords = 1;

    Bool IsValidIncrease = no;
    Bool NeedsDescription = no;

    if (Level2_Enabled( doc ))
    {
        /*
           Text within header element cannot contain more than 20 words without
           a separate description
        */
        if (node->content != NULL && node->content->tag == NULL)
        {
            ctmbstr word = textFromOneNode( doc, node->content);

            for (i = 0; i < TY_(tmbstrlen)(word); i++)
            {
                if (word[i] == ' ')
                {
                    numWords++;
                }
            }

            if (numWords > 20)
            {
                NeedsDescription = yes;
            }
        }

        /* Header following must be same level or same plus 1 for
        ** valid heading increase size.  E.g. H1 -> H1, H2.  H3 -> H3, H4
        */
        if ( TY_(nodeIsHeader)(node) )
        {
            uint level = TY_(nodeHeaderLevel)( node );
            IsValidIncrease = yes;

            for ( temp = node->next; temp != NULL; temp = temp->next )
            {
                uint nested = TY_(nodeHeaderLevel)( temp );
                if ( nested >= level )
                {
                    IsValidIncrease = ( nested <= level + 1 );
                    break;
                }
            }
        }

        if ( !IsValidIncrease )
            TY_(ReportAccessError)( doc, node, HEADERS_IMPROPERLY_NESTED );

        if ( NeedsDescription )
            TY_(ReportAccessError)( doc, node, HEADER_USED_FORMAT_TEXT );
    }
}


/*************************************************************
* CheckParagraphHeader
*
* Checks to ensure that P elements are not headings.  Must be
* greater than 10 words in length, and they must not be in bold,
* or italics, or underlined, etc.
*************************************************************/

static void CheckParagraphHeader( TidyDocImpl* doc, Node* node )
{
    Bool IsNotHeader = no;
    Node* temp;

    if (Level2_Enabled( doc ))
    {
        /* Cannot contain text formatting elements */
        if (node->content != NULL)
        {
            if (node->content->tag != NULL)
            {
                temp = node->content;

                while (temp != NULL)
                {
                    if (temp->tag == NULL)
                    {
                        IsNotHeader = yes;
                        break;
                    }

                    temp = temp->next;
                }
            }

            if ( !IsNotHeader )
            {
                if ( nodeIsSTRONG(node->content) )
                {
                    TY_(ReportAccessError)( doc, node, POTENTIAL_HEADER_BOLD);
                }

                if ( nodeIsU(node->content) )
                {
                    TY_(ReportAccessError)( doc, node, POTENTIAL_HEADER_UNDERLINE);
                }

                if ( nodeIsEM(node->content) )
                {
                    TY_(ReportAccessError)( doc, node, POTENTIAL_HEADER_ITALICS);
                }
            }
        }
    }
}


/****************************************************************
* CheckEmbed
*
* Checks to see if 'SRC' is a multimedia type.  Must have
* synchronized captions if used.
****************************************************************/

static void CheckEmbed( TidyDocImpl* doc, Node* node )
{
    if (Level1_Enabled( doc ))
    {
        AttVal* av = attrGetSRC( node );
        if ( hasValue(av) && IsValidMediaExtension(av->value) )
        {
             TY_(ReportAccessError)( doc, node, MULTIMEDIA_REQUIRES_TEXT );
        }
    }
}


/*********************************************************************
* CheckHTMLAccess
*
* Checks HTML element for valid 'LANG' attribute.  Must be a valid
* language.  ie. 'fr' or 'en'
********************************************************************/

static void CheckHTMLAccess( TidyDocImpl* doc, Node* node )
{
    Bool ValidLang = no;

    if (Level3_Enabled( doc ))
    {
        AttVal* av = attrGetLANG( node );
        if ( av )
        {
            ValidLang = yes;
            if ( !hasValue(av) )
                TY_(ReportAccessError)( doc, node, LANGUAGE_INVALID );
        }
        if ( !ValidLang )
            TY_(ReportAccessError)( doc, node, LANGUAGE_NOT_IDENTIFIED );
    }
}


/********************************************************
* CheckBlink
*
* Document must not contain the BLINK element.
* It is invalid HTML/XHTML.
*********************************************************/

static void CheckBlink( TidyDocImpl* doc, Node* node )
{

    if (Level2_Enabled( doc ))
    {
        /* Checks to see if text is found within the BLINK element. */
        if ( TY_(nodeIsText)(node->content) )
        {
            ctmbstr word = textFromOneNode( doc, node->content );
            if ( !IsWhitespace(word) )
            {
                TY_(ReportAccessError)( doc, node, REMOVE_BLINK_MARQUEE );
            }
        }
    }
}


/********************************************************
* CheckMarquee
*
* Document must not contain the MARQUEE element.
* It is invalid HTML/XHTML.
********************************************************/


static void CheckMarquee( TidyDocImpl* doc, Node* node )
{
    if (Level2_Enabled( doc ))
    {
        /* Checks to see if there is text in between the MARQUEE element */
        if ( TY_(nodeIsText)(node) )
        {
            ctmbstr word = textFromOneNode( doc, node->content);
            if ( !IsWhitespace(word) )
            {
                TY_(ReportAccessError)( doc, node, REMOVE_BLINK_MARQUEE );
            }
        }
    }
}


/**********************************************************
* CheckLink
*
* 'REL' attribute within the LINK element must not contain
* 'stylesheet'.  HTML/XHTML document is unreadable when
* style sheets are applied.  -- CPR huh?
**********************************************************/

static void CheckLink( TidyDocImpl* doc, Node* node )
{
    Bool HasRel = no;
    Bool HasType = no;

    if (Level1_Enabled( doc ))
    {
        AttVal* av;
        /* Check for valid 'REL' and 'TYPE' attribute */
        for (av = node->attributes; av != NULL; av = av->next)
        {
            if ( attrIsREL(av) && hasValue(av) )
            {
                if (AttrContains(av, "stylesheet"))
                    HasRel = yes;
            }

            if ( attrIsTYPE(av) && hasValue(av) )
            {
                HasType = yes;
            }
        }

        if (HasRel && HasType)
            TY_(ReportAccessError)( doc, node, STYLESHEETS_REQUIRE_TESTING_LINK );
    }
}


/*******************************************************
* CheckStyle
*
* Document must not contain STYLE element.  HTML/XHTML
* document is unreadable when style sheets are applied.
*******************************************************/

static void CheckStyle( TidyDocImpl* doc, Node* node )
{
    if (Level1_Enabled( doc ))
    {
        TY_(ReportAccessError)( doc, node, STYLESHEETS_REQUIRE_TESTING_STYLE_ELEMENT );
    }
}


/*************************************************************
* DynamicContent
*
* Verify that equivalents of dynamic content are updated and
* available as often as the dynamic content.
*************************************************************/


static void DynamicContent( TidyDocImpl* doc, Node* node )
{
    if (Level1_Enabled( doc ))
    {
        uint msgcode = 0;
        if ( nodeIsAPPLET(node) )
            msgcode = TEXT_EQUIVALENTS_REQUIRE_UPDATING_APPLET;
        else if ( nodeIsSCRIPT(node) )
            msgcode = TEXT_EQUIVALENTS_REQUIRE_UPDATING_SCRIPT;
        else if ( nodeIsOBJECT(node) )
            msgcode = TEXT_EQUIVALENTS_REQUIRE_UPDATING_OBJECT;

        if ( msgcode )
            TY_(ReportAccessError)( doc, node, msgcode );
    }
}


/*************************************************************
* ProgrammaticObjects
*
* Verify that the page is usable when programmatic objects
* are disabled.
*************************************************************/

static void ProgrammaticObjects( TidyDocImpl* doc, Node* node )
{
    if (Level1_Enabled( doc ))
    {
        int msgcode = 0;
        if ( nodeIsSCRIPT(node) )
            msgcode = PROGRAMMATIC_OBJECTS_REQUIRE_TESTING_SCRIPT;
        else if ( nodeIsOBJECT(node) )
            msgcode = PROGRAMMATIC_OBJECTS_REQUIRE_TESTING_OBJECT;
        else if ( nodeIsEMBED(node) )
            msgcode = PROGRAMMATIC_OBJECTS_REQUIRE_TESTING_EMBED;
        else if ( nodeIsAPPLET(node) )
            msgcode = PROGRAMMATIC_OBJECTS_REQUIRE_TESTING_APPLET;

        if ( msgcode )
            TY_(ReportAccessError)( doc, node, msgcode );
    }
}


/*************************************************************
* AccessibleCompatible
*
* Verify that programmatic objects are directly accessible.
*************************************************************/

static void AccessibleCompatible( TidyDocImpl* doc, Node* node )
{
    if (Level1_Enabled( doc ))
    {
        int msgcode = 0;
        if ( nodeIsSCRIPT(node) )
            msgcode = ENSURE_PROGRAMMATIC_OBJECTS_ACCESSIBLE_SCRIPT;
        else if ( nodeIsOBJECT(node) )
            msgcode = ENSURE_PROGRAMMATIC_OBJECTS_ACCESSIBLE_OBJECT;
        else if ( nodeIsEMBED(node) )
            msgcode = ENSURE_PROGRAMMATIC_OBJECTS_ACCESSIBLE_EMBED;
        else if ( nodeIsAPPLET(node) )
            msgcode = ENSURE_PROGRAMMATIC_OBJECTS_ACCESSIBLE_APPLET;

        if ( msgcode )
            TY_(ReportAccessError)( doc, node, msgcode );
    }
}


/**************************************************
* CheckFlicker
*
* Verify that the page does not cause flicker.
**************************************************/

static void CheckFlicker( TidyDocImpl* doc, Node* node )
{
    if (Level1_Enabled( doc ))
    {
        int msgcode = 0;
        if ( nodeIsSCRIPT(node) )
            msgcode = REMOVE_FLICKER_SCRIPT;
        else if ( nodeIsOBJECT(node) )
            msgcode = REMOVE_FLICKER_OBJECT;
        else if ( nodeIsEMBED(node) )
            msgcode = REMOVE_FLICKER_EMBED;
        else if ( nodeIsAPPLET(node) )
            msgcode = REMOVE_FLICKER_APPLET;

        /* Checks for animated gif within the <img> tag. */
        else if ( nodeIsIMG(node) )
        {
            AttVal* av = attrGetSRC( node );
            if ( hasValue(av) )
            {
                tmbchar ext[20];
                GetFileExtension( av->value, ext, sizeof(ext) );
                if ( TY_(tmbstrcasecmp)(ext, ".gif") == 0 )
                    msgcode = REMOVE_FLICKER_ANIMATED_GIF;
            }
        }

        if ( msgcode )
            TY_(ReportAccessError)( doc, node, msgcode );
    }
}


/**********************************************************
* CheckDeprecated
*
* APPLET, BASEFONT, CENTER, FONT, ISINDEX,
* S, STRIKE, and U should not be used.  Becomes deprecated
* HTML if any of the above are used.
**********************************************************/

static void CheckDeprecated( TidyDocImpl* doc, Node* node )
{
    if (Level2_Enabled( doc ))
    {
        int msgcode = 0;
        if ( nodeIsAPPLET(node) )
            msgcode = REPLACE_DEPRECATED_HTML_APPLET;
        else if ( nodeIsBASEFONT(node) )
            msgcode = REPLACE_DEPRECATED_HTML_BASEFONT;
        else if ( nodeIsCENTER(node) )
            msgcode = REPLACE_DEPRECATED_HTML_CENTER;
        else if ( nodeIsDIR(node) )
            msgcode = REPLACE_DEPRECATED_HTML_DIR;
        else if ( nodeIsFONT(node) )
            msgcode = REPLACE_DEPRECATED_HTML_FONT;
        else if ( nodeIsISINDEX(node) )
            msgcode = REPLACE_DEPRECATED_HTML_ISINDEX;
        else if ( nodeIsMENU(node) )
            msgcode = REPLACE_DEPRECATED_HTML_MENU;
        else if ( nodeIsS(node) )
            msgcode = REPLACE_DEPRECATED_HTML_S;
        else if ( nodeIsSTRIKE(node) )
            msgcode = REPLACE_DEPRECATED_HTML_STRIKE;
        else if ( nodeIsU(node) )
            msgcode = REPLACE_DEPRECATED_HTML_U;

        if ( msgcode )
            TY_(ReportAccessError)( doc, node, msgcode );
    }
}


/************************************************************
* CheckScriptKeyboardAccessible
*
* Elements must have a device independent event handler if
* they have any of the following device dependent event
* handlers.
************************************************************/

static void CheckScriptKeyboardAccessible( TidyDocImpl* doc, Node* node )
{
    Node* content;
    int HasOnMouseDown = 0;
    int HasOnMouseUp = 0;
    int HasOnClick = 0;
    int HasOnMouseOut = 0;
    int HasOnMouseOver = 0;
    int HasOnMouseMove = 0;

    if (Level2_Enabled( doc ))
    {
        AttVal* av;
        /* Checks all elements for their attributes */
        for (av = node->attributes; av != NULL; av = av->next)
        {
            /* Must also have 'ONKEYDOWN' attribute with 'ONMOUSEDOWN' */
            if ( attrIsOnMOUSEDOWN(av) )
                HasOnMouseDown++;

            /* Must also have 'ONKEYUP' attribute with 'ONMOUSEUP' */
            if ( attrIsOnMOUSEUP(av) )
                HasOnMouseUp++;

            /* Must also have 'ONKEYPRESS' attribute with 'ONCLICK' */
            if ( attrIsOnCLICK(av) )
                HasOnClick++;

            /* Must also have 'ONBLUR' attribute with 'ONMOUSEOUT' */
            if ( attrIsOnMOUSEOUT(av) )
                HasOnMouseOut++;

            if ( attrIsOnMOUSEOVER(av) )
                HasOnMouseOver++;

            if ( attrIsOnMOUSEMOVE(av) )
                HasOnMouseMove++;

            if ( attrIsOnKEYDOWN(av) )
                HasOnMouseDown++;

            if ( attrIsOnKEYUP(av) )
                HasOnMouseUp++;

            if ( attrIsOnKEYPRESS(av) )
                HasOnClick++;

            if ( attrIsOnBLUR(av) )
                HasOnMouseOut++;
        }

        if ( HasOnMouseDown == 1 )
            TY_(ReportAccessError)( doc, node, SCRIPT_NOT_KEYBOARD_ACCESSIBLE_ON_MOUSE_DOWN);

        if ( HasOnMouseUp == 1 )
            TY_(ReportAccessError)( doc, node, SCRIPT_NOT_KEYBOARD_ACCESSIBLE_ON_MOUSE_UP);

        if ( HasOnClick == 1 )
            TY_(ReportAccessError)( doc, node, SCRIPT_NOT_KEYBOARD_ACCESSIBLE_ON_CLICK);
        if ( HasOnMouseOut == 1 )
            TY_(ReportAccessError)( doc, node, SCRIPT_NOT_KEYBOARD_ACCESSIBLE_ON_MOUSE_OUT);

        if ( HasOnMouseOver == 1 )
            TY_(ReportAccessError)( doc, node, SCRIPT_NOT_KEYBOARD_ACCESSIBLE_ON_MOUSE_OVER);

        if ( HasOnMouseMove == 1 )
            TY_(ReportAccessError)( doc, node, SCRIPT_NOT_KEYBOARD_ACCESSIBLE_ON_MOUSE_MOVE);

        /* Recursively check all child nodes.
         */
        for ( content = node->content; content != NULL; content = content->next )
            CheckScriptKeyboardAccessible( doc, content );
    }
}


/**********************************************************
* CheckMetaData
*
* Must have at least one of these elements in the document.
* META, LINK, TITLE or ADDRESS.  <meta> must contain
* a "content" attribute that doesn't contain a URL, and
* an "http-Equiv" attribute that doesn't contain 'refresh'.
**********************************************************/


static Bool CheckMetaData( TidyDocImpl* doc, Node* node, Bool HasMetaData )
{
    Bool HasHttpEquiv = no;
    Bool HasContent = no;
    Bool ContainsAttr = no;

    if (Level2_Enabled( doc ))
    {
        if ( nodeIsMETA(node) )
        {
            AttVal* av;
            for (av = node->attributes; av != NULL; av = av->next)
            {
                if ( attrIsHTTP_EQUIV(av) && hasValue(av) )
                {
                    ContainsAttr = yes;

                    /* Must not have an auto-refresh */
                    if (AttrValueIs(av, "refresh"))
                    {
                        HasHttpEquiv = yes;
                        TY_(ReportAccessError)( doc, node, REMOVE_AUTO_REFRESH );
                    }
                }

                if ( attrIsCONTENT(av) && hasValue(av) )
                {
                    ContainsAttr = yes;

                    /* If the value is not an integer, then it must not be a URL */
                    if ( TY_(tmbstrncmp)(av->value, "http:", 5) == 0)
                    {
                        HasContent = yes;
                        TY_(ReportAccessError)( doc, node, REMOVE_AUTO_REDIRECT);
                    }
                }
                if (TY_(IsHTML5Mode)(doc) && attrIsCHARSET(av) && hasValue(av))
                {
                    ContainsAttr = yes;
                }
            }

            if ( HasContent || HasHttpEquiv )
            {
                HasMetaData = yes;
                TY_(ReportAccessError)( doc, node, METADATA_MISSING_REDIRECT_AUTOREFRESH);
            }
            else
            {
                if ( ContainsAttr && !HasContent && !HasHttpEquiv )
                    HasMetaData = yes;
            }
        }

        if ( !HasMetaData &&
             nodeIsADDRESS(node) &&
             nodeIsA(node->content) )
        {
            HasMetaData = yes;
        }

        if ( !HasMetaData &&
             !nodeIsTITLE(node) &&
             TY_(nodeIsText)(node->content) )
        {
            ctmbstr word = textFromOneNode( doc, node->content );
            if ( !IsWhitespace(word) )
                HasMetaData = yes;
        }

        if( !HasMetaData && nodeIsLINK(node) )
        {
            AttVal* av = attrGetREL(node);
            if( !AttrContains(av, "stylesheet") )
                HasMetaData = yes;
        }

        /* Check for MetaData */
        for ( node = node->content; node; node = node->next )
        {
            HasMetaData = CheckMetaData( doc, node, HasMetaData );
        }
    }
    return HasMetaData;
}


/*******************************************************
* MetaDataPresent
*
* Determines if MetaData is present in document
*******************************************************/

static void MetaDataPresent( TidyDocImpl* doc, Node* node )
{
    if (Level2_Enabled( doc ))
    {
        TY_(ReportAccessError)( doc, node, METADATA_MISSING );
    }
}


/*****************************************************
* CheckDocType
*
* Checks that every HTML/XHTML document contains a
* '!DOCTYPE' before the root node. ie.  <HTML>
*****************************************************/

static void CheckDocType( TidyDocImpl* doc )
{
    if (Level2_Enabled( doc ))
    {
        Node* DTnode = TY_(FindDocType)(doc);

        /* If the doctype has been added by tidy, DTnode->end will be 0. */
        if (DTnode && DTnode->end != 0)
        {
            ctmbstr word = textFromOneNode( doc, DTnode);
            if (TY_(IsHTML5Mode)(doc))
            {
                if ((strstr(word, "HTML") == NULL) &&
                    (strstr(word, "html") == NULL))
                    DTnode = NULL;
            }
            else {
                if ((strstr(word, "HTML PUBLIC") == NULL) &&
                    (strstr(word, "html PUBLIC") == NULL))
                    DTnode = NULL;
            }
        }
        if (!DTnode)
           TY_(ReportAccessError)( doc, &doc->root, DOCTYPE_MISSING);
    }
}



/********************************************************
* CheckMapLinks
*
* Checks to see if an HREF for A element matches HREF
* for AREA element.  There must be an HREF attribute
* of an A element for every HREF of an AREA element.
********************************************************/

static Bool urlMatch( ctmbstr url1, ctmbstr url2 )
{
  /* TODO: Make host part case-insensitive and
  ** remainder case-sensitive.
  */
  return ( TY_(tmbstrcmp)( url1, url2 ) == 0 );
}

static Bool FindLinkA( TidyDocImpl* doc, Node* node, ctmbstr url )
{
  Bool found = no;
  for ( node = node->content; !found && node; node = node->next )
  {
    if ( nodeIsA(node) )
    {
      AttVal* href = attrGetHREF( node );
      found = ( hasValue(href) && urlMatch(url, href->value) );
    }
    else
        found = FindLinkA( doc, node, url );
  }
  return found;
}

static void CheckMapLinks( TidyDocImpl* doc, Node* node )
{
    Node* child;

    if (!Level3_Enabled( doc ))
        return;

    /* Stores the 'HREF' link of an AREA element within a MAP element */
    for ( child = node->content; child != NULL; child = child->next )
    {
        if ( nodeIsAREA(child) )
        {
            /* Checks for 'HREF' attribute */
            AttVal* href = attrGetHREF( child );
            if ( hasValue(href) &&
                 !FindLinkA( doc, &doc->root, href->value ) )
            {
                TY_(ReportAccessError)( doc, node, IMG_MAP_CLIENT_MISSING_TEXT_LINKS );
            }
        }
    }
}


/****************************************************
* CheckForStyleAttribute
*
* Checks all elements within the document to check
* for the use of 'STYLE' attribute.
****************************************************/

static void CheckForStyleAttribute( TidyDocImpl* doc, Node* node )
{
    Node* content;
    if (Level1_Enabled( doc ))
    {
        /* Must not contain 'STYLE' attribute */
        AttVal* style = attrGetSTYLE( node );
        if ( hasValue(style) )
        {
            TY_(ReportAccessError)( doc, node, STYLESHEETS_REQUIRE_TESTING_STYLE_ATTR );
        }
    }

    /* Recursively check all child nodes.
    */
    for ( content = node->content; content != NULL; content = content->next )
        CheckForStyleAttribute( doc, content );
}


/*****************************************************
* CheckForListElements
*
* Checks document for list elements (<ol>, <ul>, <li>)
*****************************************************/

static void CheckForListElements( TidyDocImpl* doc, Node* node )
{
    if ( nodeIsLI(node) )
    {
        doc->access.ListElements++;
    }
    else if ( nodeIsOL(node) || nodeIsUL(node) )
    {
        doc->access.OtherListElements++;
    }

    for ( node = node->content; node != NULL; node = node->next )
    {
        CheckForListElements( doc, node );
    }
}


/******************************************************
* CheckListUsage
*
* Ensures that lists are properly used.  <ol> and <ul>
* must contain <li> within itself, and <li> must not be
* by itself.
******************************************************/

static void CheckListUsage( TidyDocImpl* doc, Node* node )
{
    int msgcode = 0;

    if (!Level2_Enabled( doc ))
        return;

    if ( nodeIsOL(node) )
        msgcode = LIST_USAGE_INVALID_OL;
    else if ( nodeIsUL(node) )
        msgcode = LIST_USAGE_INVALID_UL;

    if ( msgcode )
    {
       /*
       ** Check that OL/UL
       ** a) has LI child,
       ** b) was not added by Tidy parser
       ** IFF OL/UL node is implicit
       */
       if ( !nodeIsLI(node->content) ) {
            TY_(ReportAccessError)( doc, node, msgcode );
       } else if ( node->implicit ) {  /* if a tidy added node */
            TY_(ReportAccessError)( doc, node, LIST_USAGE_INVALID_LI );
       }
    }
    else if ( nodeIsLI(node) )
    {
        /* Check that LI parent
        ** a) exists,
        ** b) is either OL or UL
        ** IFF the LI parent was added by Tidy
        ** ie, if it is marked 'implicit', then
        ** emit warnings LIST_USAGE_INVALID_UL or
        ** warning LIST_USAGE_INVALID_OL tests
        */
        if ( node->parent == NULL ||
             ( !nodeIsOL(node->parent) && !nodeIsUL(node->parent) ) )
        {
            TY_(ReportAccessError)( doc, node, LIST_USAGE_INVALID_LI );
        } else if ( node->implicit && node->parent &&
                    ( nodeIsOL(node->parent) || nodeIsUL(node->parent) ) ) {
            /* if tidy added LI node, then */
            msgcode = nodeIsUL(node->parent) ?
                LIST_USAGE_INVALID_UL : LIST_USAGE_INVALID_OL;
            TY_(ReportAccessError)( doc, node, msgcode );
        }
    }
}

/************************************************************
* InitAccessibilityChecks
*
* Initializes the AccessibilityChecks variables as necessary
************************************************************/

static void InitAccessibilityChecks( TidyDocImpl* doc, int level123 )
{
    TidyClearMemory( &doc->access, sizeof(doc->access) );
    doc->access.PRIORITYCHK = level123;
}

/************************************************************
* CleanupAccessibilityChecks
*
* Cleans up the AccessibilityChecks variables as necessary
************************************************************/


static void FreeAccessibilityChecks( TidyDocImpl* ARG_UNUSED(doc) )
{
    /* free any memory allocated for the lists

    Linked List of Links not used.  Just search document as
    AREA tags are encountered.  Same algorithm, but no
    data structures necessary.

    current = start;
    while (current)
    {
        void    *templink = (void *)current;

        current = current->next;
        TidyDocFree(doc, templink);
    }
    start = NULL;
    */
}

/************************************************************
* AccessibilityChecks
*
* Traverses through the individual nodes of the tree
* and checks attributes and elements for accessibility.
* after the tree structure has been formed.
************************************************************/

static void AccessibilityCheckNode( TidyDocImpl* doc, Node* node )
{
    Node* content;

    /* Check BODY for color contrast */
    if ( nodeIsBODY(node) )
    {
        CheckColorContrast( doc, node );
    }

    /* Checks document for MetaData */
    else if ( nodeIsHEAD(node) )
    {
        if ( !CheckMetaData( doc, node, no ) )
          MetaDataPresent( doc, node );
    }

    /* Check the ANCHOR tag */
    else if ( nodeIsA(node) )
    {
        CheckAnchorAccess( doc, node );
    }

    /* Check the IMAGE tag */
    else if ( nodeIsIMG(node) )
    {
        CheckFlicker( doc, node );
        CheckColorAvailable( doc, node );
        CheckImage( doc, node );
    }

        /* Checks MAP for client-side text links */
    else if ( nodeIsMAP(node) )
    {
        CheckMapLinks( doc, node );
    }

    /* Check the AREA tag */
    else if ( nodeIsAREA(node) )
    {
        CheckArea( doc, node );
    }

    /* Check the APPLET tag */
    else if ( nodeIsAPPLET(node) )
    {
        CheckDeprecated( doc, node );
        ProgrammaticObjects( doc, node );
        DynamicContent( doc, node );
        AccessibleCompatible( doc, node );
        CheckFlicker( doc, node );
        CheckColorAvailable( doc, node );
        CheckApplet(doc, node );
    }

    /* Check the OBJECT tag */
    else if ( nodeIsOBJECT(node) )
    {
        ProgrammaticObjects( doc, node );
        DynamicContent( doc, node );
        AccessibleCompatible( doc, node );
        CheckFlicker( doc, node );
        CheckColorAvailable( doc, node );
        CheckObject( doc, node );
    }

    /* Check the FRAME tag */
    else if ( nodeIsFRAME(node) )
    {
        CheckFrame( doc, node );
    }

    /* Check the IFRAME tag */
    else if ( nodeIsIFRAME(node) )
    {
        CheckIFrame( doc, node );
    }

    /* Check the SCRIPT tag */
    else if ( nodeIsSCRIPT(node) )
    {
        DynamicContent( doc, node );
        ProgrammaticObjects( doc, node );
        AccessibleCompatible( doc, node );
        CheckFlicker( doc, node );
        CheckColorAvailable( doc, node );
        CheckScriptAcc( doc, node );
    }

    /* Check the TABLE tag */
    else if ( nodeIsTABLE(node) )
    {
        CheckColorContrast( doc, node );
        CheckTable( doc, node );
    }

    /* Check the PRE for ASCII art */
    else if ( nodeIsPRE(node) || nodeIsXMP(node) )
    {
        CheckASCII( doc, node );
    }

    /* Check the LABEL tag */
    else if ( nodeIsLABEL(node) )
    {
        CheckLabel( doc, node );
    }

    /* Check INPUT tag for validity */
    else if ( nodeIsINPUT(node) )
    {
        CheckColorAvailable( doc, node );
        CheckInputLabel( doc, node );
        CheckInputAttributes( doc, node );
    }

    /* Checks FRAMESET element for NOFRAME section */
    else if ( nodeIsFRAMESET(node) )
    {
        CheckFrameSet( doc, node );
    }

    /* Checks for header elements for valid header increase */
    else if ( TY_(nodeIsHeader)(node) )
    {
        CheckHeaderNesting( doc, node );
    }

    /* Checks P element to ensure that it is not a header */
    else if ( nodeIsP(node) )
    {
        CheckParagraphHeader( doc, node );
    }

    /* Checks HTML element for valid 'LANG' */
    else if ( nodeIsHTML(node) )
    {
        CheckHTMLAccess( doc, node );
    }

    /* Checks BLINK for any blinking text */
    else if ( nodeIsBLINK(node) )
    {
        CheckBlink( doc, node );
    }

    /* Checks MARQUEE for any MARQUEE text */
    else if ( nodeIsMARQUEE(node) )
    {
        CheckMarquee( doc, node );
    }

    /* Checks LINK for 'REL' attribute */
    else if ( nodeIsLINK(node) )
    {
        CheckLink( doc, node );
    }

    /* Checks to see if STYLE is used */
    else if ( nodeIsSTYLE(node) )
    {
        CheckColorContrast( doc, node );
        CheckStyle( doc, node );
    }

    /* Checks to see if EMBED is used */
    else if ( nodeIsEMBED(node) )
    {
        CheckEmbed( doc, node );
        ProgrammaticObjects( doc, node );
        AccessibleCompatible( doc, node );
        CheckFlicker( doc, node );
    }

    /* Deprecated HTML if the following tags are found in the document */
    else if ( nodeIsBASEFONT(node) ||
              nodeIsCENTER(node)   ||
              nodeIsISINDEX(node)  ||
              nodeIsU(node)        ||
              nodeIsFONT(node)     ||
              nodeIsDIR(node)      ||
              nodeIsS(node)        ||
              nodeIsSTRIKE(node)   ||
              nodeIsMENU(node) )
    {
        CheckDeprecated( doc, node );
    }

    /* Checks for 'ABBR' attribute if needed */
    else if ( nodeIsTH(node) )
    {
        CheckTH( doc, node );
    }

    /* Ensures that lists are properly used */
    else if ( nodeIsLI(node) || nodeIsOL(node) || nodeIsUL(node) )
    {
        CheckListUsage( doc, node );
    }

    /* Recursively check all child nodes.
    */
    for ( content = node->content; content != NULL; content = content->next )
    {
        AccessibilityCheckNode( doc, content );
    }
}


void TY_(AccessibilityChecks)( TidyDocImpl* doc )
{
    /* Initialize */
    InitAccessibilityChecks( doc, cfg(doc, TidyAccessibilityCheckLevel) );

    /* Hello there, ladies and gentlemen... */
    TY_(Dialogue)( doc, STRING_HELLO_ACCESS );

    /* Checks all elements for script accessibility */
    CheckScriptKeyboardAccessible( doc, &doc->root );

    /* Checks entire document for the use of 'STYLE' attribute */
    CheckForStyleAttribute( doc, &doc->root );

    /* Checks for '!DOCTYPE' */
    CheckDocType( doc );


    /* Checks to see if stylesheets are used to control the layout */
    if ( Level2_Enabled( doc )
         && ! CheckMissingStyleSheets( doc, &doc->root ) )
    {
        TY_(ReportAccessError)( doc, &doc->root, STYLE_SHEET_CONTROL_PRESENTATION );
    }

    /* Check to see if any list elements are found within the document */
    CheckForListElements( doc, &doc->root );

    /* Recursively apply all remaining checks to
    ** each node in document.
    */
    AccessibilityCheckNode( doc, &doc->root );

    /* Cleanup */
    FreeAccessibilityChecks( doc );
}

