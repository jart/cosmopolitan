
/*
*   Copyright (c) 2010, Vincent Berthoux
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   This module contains functions for generating tags for Objective C
*   language files.
*/
/*
*   INCLUDE FILES
*/
#include "third_party/ctags/general.h"	/* must always come first */

#include "libc/mem/alg.h"
#include "libc/str/str.h"

#include "third_party/ctags/keyword.h"
#include "third_party/ctags/entry.h"
#include "third_party/ctags/options.h"
#include "third_party/ctags/read.h"
#include "third_party/ctags/routines.h"
#include "third_party/ctags/vstring.h"

/* To get rid of unused parameter warning in
 * -Wextra */
#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif

typedef enum {
	K_INTERFACE,
	K_IMPLEMENTATION,
	K_PROTOCOL,
	K_METHOD,
	K_CLASSMETHOD,
	K_VAR,
	K_FIELD,
	K_FUNCTION,
	K_PROPERTY,
	K_TYPEDEF,
	K_STRUCT,
	K_ENUM,
	K_MACRO
} objcKind;

static kindOption ObjcKinds[] = {
	{TRUE, 'i', "interface", "class interface"},
	{TRUE, 'I', "implementation", "class implementation"},
	{TRUE, 'p', "protocol", "Protocol"},
	{TRUE, 'm', "method", "Object's method"},
	{TRUE, 'c', "class", "Class' method"},
	{TRUE, 'v', "var", "Global variable"},
	{TRUE, 'F', "field", "Object field"},
	{TRUE, 'f', "function", "A function"},
	{TRUE, 'p', "property", "A property"},
	{TRUE, 't', "typedef", "A type alias"},
	{TRUE, 's', "struct", "A type structure"},
	{TRUE, 'e', "enum", "An enumeration"},
	{TRUE, 'M', "macro", "A preprocessor macro"},
};

typedef enum {
	ObjcTYPEDEF,
	ObjcSTRUCT,
	ObjcENUM,
	ObjcIMPLEMENTATION,
	ObjcINTERFACE,
	ObjcPROTOCOL,
	ObjcENCODE,
	ObjcSYNCHRONIZED,
	ObjcSELECTOR,
	ObjcPROPERTY,
	ObjcEND,
	ObjcDEFS,
	ObjcCLASS,
	ObjcPRIVATE,
	ObjcPACKAGE,
	ObjcPUBLIC,
	ObjcPROTECTED,
	ObjcSYNTHESIZE,
	ObjcDYNAMIC,
	ObjcOPTIONAL,
	ObjcREQUIRED,
	ObjcSTRING,
	ObjcIDENTIFIER,

	Tok_COMA,	/* ',' */
	Tok_PLUS,	/* '+' */
	Tok_MINUS,	/* '-' */
	Tok_PARL,	/* '(' */
	Tok_PARR,	/* ')' */
	Tok_CurlL,	/* '{' */
	Tok_CurlR,	/* '}' */
	Tok_SQUAREL,	/* '[' */
	Tok_SQUARER,	/* ']' */
	Tok_semi,	/* ';' */
	Tok_dpoint,	/* ':' */
	Tok_Sharp,	/* '#' */
	Tok_Backslash,	/* '\\' */
	Tok_EOL,	/* '\r''\n' */
	Tok_any,

	Tok_EOF	/* END of file */
} objcKeyword;

typedef objcKeyword objcToken;

typedef struct sOBjcKeywordDesc {
	const char *name;
	objcKeyword id;
} objcKeywordDesc;


static const objcKeywordDesc objcKeywordTable[] = {
	{"typedef", ObjcTYPEDEF},
	{"struct", ObjcSTRUCT},
	{"enum", ObjcENUM},
	{"@implementation", ObjcIMPLEMENTATION},
	{"@interface", ObjcINTERFACE},
	{"@protocol", ObjcPROTOCOL},
	{"@encode", ObjcENCODE},
	{"@property", ObjcPROPERTY},
	{"@synchronized", ObjcSYNCHRONIZED},
	{"@selector", ObjcSELECTOR},
	{"@end", ObjcEND},
	{"@defs", ObjcDEFS},
	{"@class", ObjcCLASS},
	{"@private", ObjcPRIVATE},
	{"@package", ObjcPACKAGE},
	{"@public", ObjcPUBLIC},
	{"@protected", ObjcPROTECTED},
	{"@synthesize", ObjcSYNTHESIZE},
	{"@dynamic", ObjcDYNAMIC},
	{"@optional", ObjcOPTIONAL},
	{"@required", ObjcREQUIRED},
};

static langType Lang_ObjectiveC;

/*//////////////////////////////////////////////////////////////////
//// lexingInit             */
typedef struct _lexingState {
	vString *name;	/* current parsed identifier/operator */
	const unsigned char *cp;	/* position in stream */
} lexingState;

static void initKeywordHash (void)
{
	const size_t count = sizeof (objcKeywordTable) / sizeof (objcKeywordDesc);
	size_t i;

	for (i = 0; i < count; ++i)
	{
		addKeyword (objcKeywordTable[i].name, Lang_ObjectiveC,
			(int) objcKeywordTable[i].id);
	}
}

/*//////////////////////////////////////////////////////////////////////
//// Lexing                                     */
static boolean isNum (char c)
{
	return c >= '0' && c <= '9';
}

static boolean isLowerAlpha (char c)
{
	return c >= 'a' && c <= 'z';
}

static boolean isUpperAlpha (char c)
{
	return c >= 'A' && c <= 'Z';
}

static boolean isAlpha (char c)
{
	return isLowerAlpha (c) || isUpperAlpha (c);
}

static boolean isIdent (char c)
{
	return isNum (c) || isAlpha (c) || c == '_';
}

static boolean isSpace (char c)
{
	return c == ' ' || c == '\t';
}

/* return true if it end with an end of line */
static void eatWhiteSpace (lexingState * st)
{
	const unsigned char *cp = st->cp;
	while (isSpace (*cp))
		cp++;

	st->cp = cp;
}

static void eatString (lexingState * st)
{
	boolean lastIsBackSlash = FALSE;
	boolean unfinished = TRUE;
	const unsigned char *c = st->cp + 1;

	while (unfinished)
	{
		/* end of line should never happen.
		 * we tolerate it */
		if (c == NULL || c[0] == '\0')
			break;
		else if (*c == '"' && !lastIsBackSlash)
			unfinished = FALSE;
		else
			lastIsBackSlash = *c == '\\';

		c++;
	}

	st->cp = c;
}

static void eatComment (lexingState * st)
{
	boolean unfinished = TRUE;
	boolean lastIsStar = FALSE;
	const unsigned char *c = st->cp + 2;

	while (unfinished)
	{
		/* we've reached the end of the line..
		 * so we have to reload a line... */
		if (c == NULL || *c == '\0')
		{
			st->cp = fileReadLine ();
			/* WOOPS... no more input...
			 * we return, next lexing read
			 * will be null and ok */
			if (st->cp == NULL)
				return;
			c = st->cp;
		}
		/* we've reached the end of the comment */
		else if (*c == '/' && lastIsStar)
			unfinished = FALSE;
		else
		{
			lastIsStar = '*' == *c;
			c++;
		}
	}

	st->cp = c;
}

static void readIdentifier (lexingState * st)
{
	const unsigned char *p;
	vStringClear (st->name);

	/* first char is a simple letter */
	if (isAlpha (*st->cp) || *st->cp == '_')
		vStringPut (st->name, (int) *st->cp);

	/* Go till you get identifier chars */
	for (p = st->cp + 1; isIdent (*p); p++)
		vStringPut (st->name, (int) *p);

	st->cp = p;

	vStringTerminate (st->name);
}

/* read the @something directives */
static void readIdentifierObjcDirective (lexingState * st)
{
	const unsigned char *p;
	vStringClear (st->name);

	/* first char is a simple letter */
	if (*st->cp == '@')
		vStringPut (st->name, (int) *st->cp);

	/* Go till you get identifier chars */
	for (p = st->cp + 1; isIdent (*p); p++)
		vStringPut (st->name, (int) *p);

	st->cp = p;

	vStringTerminate (st->name);
}

/* The lexer is in charge of reading the file.
 * Some of sub-lexer (like eatComment) also read file.
 * lexing is finished when the lexer return Tok_EOF */
static objcKeyword lex (lexingState * st)
{
	int retType;

	/* handling data input here */
	while (st->cp == NULL || st->cp[0] == '\0')
	{
		st->cp = fileReadLine ();
		if (st->cp == NULL)
			return Tok_EOF;

		return Tok_EOL;
	}

	if (isAlpha (*st->cp))
	{
		readIdentifier (st);
		retType = lookupKeyword (vStringValue (st->name), Lang_ObjectiveC);

		if (retType == -1)	/* If it's not a keyword */
		{
			return ObjcIDENTIFIER;
		}
		else
		{
			return retType;
		}
	}
	else if (*st->cp == '@')
	{
		readIdentifierObjcDirective (st);
		retType = lookupKeyword (vStringValue (st->name), Lang_ObjectiveC);

		if (retType == -1)	/* If it's not a keyword */
		{
			return Tok_any;
		}
		else
		{
			return retType;
		}
	}
	else if (isSpace (*st->cp))
	{
		eatWhiteSpace (st);
		return lex (st);
	}
	else
		switch (*st->cp)
		{
		case '(':
			st->cp++;
			return Tok_PARL;

		case '\\':
			st->cp++;
			return Tok_Backslash;

		case '#':
			st->cp++;
			return Tok_Sharp;

		case '/':
			if (st->cp[1] == '*')	/* ergl, a comment */
			{
				eatComment (st);
				return lex (st);
			}
			else if (st->cp[1] == '/')
			{
				st->cp = NULL;
				return lex (st);
			}
			else
			{
				st->cp++;
				return Tok_any;
			}
			break;

		case ')':
			st->cp++;
			return Tok_PARR;
		case '{':
			st->cp++;
			return Tok_CurlL;
		case '}':
			st->cp++;
			return Tok_CurlR;
		case '[':
			st->cp++;
			return Tok_SQUAREL;
		case ']':
			st->cp++;
			return Tok_SQUARER;
		case ',':
			st->cp++;
			return Tok_COMA;
		case ';':
			st->cp++;
			return Tok_semi;
		case ':':
			st->cp++;
			return Tok_dpoint;
		case '"':
			eatString (st);
			return Tok_any;
		case '+':
			st->cp++;
			return Tok_PLUS;
		case '-':
			st->cp++;
			return Tok_MINUS;

		default:
			st->cp++;
			break;
		}

	/* default return if nothing is recognized,
	 * shouldn't happen, but at least, it will
	 * be handled without destroying the parsing. */
	return Tok_any;
}

/*//////////////////////////////////////////////////////////////////////
//// Parsing                                    */
typedef void (*parseNext) (vString * const ident, objcToken what);

/********** Helpers */
/* This variable hold the 'parser' which is going to
 * handle the next token */
static parseNext toDoNext;

/* Special variable used by parser eater to
 * determine which action to put after their
 * job is finished. */
static parseNext comeAfter;

/* Used by some parsers detecting certain token
 * to revert to previous parser. */
static parseNext fallback;


/********** Grammar */
static void globalScope (vString * const ident, objcToken what);
static void parseMethods (vString * const ident, objcToken what);
static void parseImplemMethods (vString * const ident, objcToken what);
static vString *tempName = NULL;
static vString *parentName = NULL;
static objcKind parentType = K_INTERFACE;

/* used to prepare tag for OCaml, just in case their is a need to
 * add additional information to the tag. */
static void prepareTag (tagEntryInfo * tag, vString const *name, objcKind kind)
{
	initTagEntry (tag, vStringValue (name));
	tag->kindName = ObjcKinds[kind].name;
	tag->kind = ObjcKinds[kind].letter;

	if (parentName != NULL)
	{
		tag->extensionFields.scope[0] = ObjcKinds[parentType].name;
		tag->extensionFields.scope[1] = vStringValue (parentName);
	}
}

void pushEnclosingContext (const vString * parent, objcKind type)
{
	vStringCopy (parentName, parent);
	parentType = type;
}

void popEnclosingContext ()
{
	vStringClear (parentName);
}

/* Used to centralise tag creation, and be able to add
 * more information to it in the future */
static void addTag (vString * const ident, int kind)
{
	tagEntryInfo toCreate;
	prepareTag (&toCreate, ident, kind);
	makeTagEntry (&toCreate);
}

objcToken waitedToken, fallBackToken;

/* Ignore everything till waitedToken and jump to comeAfter.
 * If the "end" keyword is encountered break, doesn't remember
 * why though. */
static void tillToken (vString * const UNUSED (ident), objcToken what)
{
	if (what == waitedToken)
		toDoNext = comeAfter;
}

static void tillTokenOrFallBack (vString * const UNUSED (ident), objcToken what)
{
	if (what == waitedToken)
		toDoNext = comeAfter;
	else if (what == fallBackToken)
	{
		toDoNext = fallback;
	}
}

static void ignoreBalanced (vString * const UNUSED (ident), objcToken what)
{
	static int count = 0;

	switch (what)
	{
	case Tok_PARL:
	case Tok_CurlL:
	case Tok_SQUAREL:
		count++;
		break;

	case Tok_PARR:
	case Tok_CurlR:
	case Tok_SQUARER:
		count--;
		break;

	default:
		/* don't care */
		break;
	}

	if (count == 0)
		toDoNext = comeAfter;
}

static void parseFields (vString * const ident, objcToken what)
{
	switch (what)
	{
	case Tok_CurlR:
		toDoNext = &parseMethods;
		break;

	case Tok_SQUAREL:
	case Tok_PARL:
		toDoNext = &ignoreBalanced;
		comeAfter = &parseFields;
		break;

		// we got an identifier, keep track
		// of it
	case ObjcIDENTIFIER:
		vStringCopy (tempName, ident);
		break;

		// our last kept identifier must be our
		// variable name =)
	case Tok_semi:
		addTag (tempName, K_FIELD);
		vStringClear (tempName);
		break;

	default:
		/* NOTHING */
		break;
	}
}

objcKind methodKind;


static vString *fullMethodName;
static vString *prevIdent;

static void parseMethodsName (vString * const ident, objcToken what)
{
	switch (what)
	{
	case Tok_PARL:
		toDoNext = &tillToken;
		comeAfter = &parseMethodsName;
		waitedToken = Tok_PARR;
		break;

	case Tok_dpoint:
		vStringCat (fullMethodName, prevIdent);
		vStringCatS (fullMethodName, ":");
		vStringClear (prevIdent);
		break;

	case ObjcIDENTIFIER:
		vStringCopy (prevIdent, ident);
		break;

	case Tok_CurlL:
	case Tok_semi:
		// method name is not simple
		if (vStringLength (fullMethodName) != '\0')
		{
			addTag (fullMethodName, methodKind);
			vStringClear (fullMethodName);
		}
		else
			addTag (prevIdent, methodKind);

		toDoNext = &parseMethods;
		parseImplemMethods (ident, what);
		vStringClear (prevIdent);
		break;

	default:
		break;
	}
}

static void parseMethodsImplemName (vString * const ident, objcToken what)
{
	switch (what)
	{
	case Tok_PARL:
		toDoNext = &tillToken;
		comeAfter = &parseMethodsImplemName;
		waitedToken = Tok_PARR;
		break;

	case Tok_dpoint:
		vStringCat (fullMethodName, prevIdent);
		vStringCatS (fullMethodName, ":");
		vStringClear (prevIdent);
		break;

	case ObjcIDENTIFIER:
		vStringCopy (prevIdent, ident);
		break;

	case Tok_CurlL:
	case Tok_semi:
		// method name is not simple
		if (vStringLength (fullMethodName) != '\0')
		{
			addTag (fullMethodName, methodKind);
			vStringClear (fullMethodName);
		}
		else
			addTag (prevIdent, methodKind);

		toDoNext = &parseImplemMethods;
		parseImplemMethods (ident, what);
		vStringClear (prevIdent);
		break;

	default:
		break;
	}
}

static void parseImplemMethods (vString * const ident, objcToken what)
{
	switch (what)
	{
	case Tok_PLUS:	/* + */
		toDoNext = &parseMethodsImplemName;
		methodKind = K_CLASSMETHOD;
		break;

	case Tok_MINUS:	/* - */
		toDoNext = &parseMethodsImplemName;
		methodKind = K_METHOD;
		break;

	case ObjcEND:	/* @end */
		popEnclosingContext ();
		toDoNext = &globalScope;
		break;

	case Tok_CurlL:	/* { */
		toDoNext = &ignoreBalanced;
		ignoreBalanced (ident, what);
		comeAfter = &parseImplemMethods;
		break;

	default:
		break;
	}
}

static void parseProperty (vString * const ident, objcToken what)
{
	switch (what)
	{
	case Tok_PARL:
		toDoNext = &tillToken;
		comeAfter = &parseProperty;
		waitedToken = Tok_PARR;
		break;

		// we got an identifier, keep track
		// of it
	case ObjcIDENTIFIER:
		vStringCopy (tempName, ident);
		break;

		// our last kept identifier must be our
		// variable name =)
	case Tok_semi:
		addTag (tempName, K_PROPERTY);
		vStringClear (tempName);
		break;

	default:
		break;
	}
}

static void parseMethods (vString * const UNUSED (ident), objcToken what)
{
	switch (what)
	{
	case Tok_PLUS:	/* + */
		toDoNext = &parseMethodsName;
		methodKind = K_CLASSMETHOD;
		break;

	case Tok_MINUS:	/* - */
		toDoNext = &parseMethodsName;
		methodKind = K_METHOD;
		break;

	case ObjcPROPERTY:
		toDoNext = &parseProperty;
		break;

	case ObjcEND:	/* @end */
		popEnclosingContext ();
		toDoNext = &globalScope;
		break;

	case Tok_CurlL:	/* { */
		toDoNext = &parseFields;
		break;

	default:
		break;
	}
}


static void parseProtocol (vString * const ident, objcToken what)
{
	if (what == ObjcIDENTIFIER)
	{
		pushEnclosingContext (ident, K_PROTOCOL);
		addTag (ident, K_PROTOCOL);
	}
	toDoNext = &parseMethods;
}

static void parseImplementation (vString * const ident, objcToken what)
{
	if (what == ObjcIDENTIFIER)
	{
		addTag (ident, K_IMPLEMENTATION);
		pushEnclosingContext (ident, K_IMPLEMENTATION);
	}
	toDoNext = &parseImplemMethods;
}

static void parseInterface (vString * const ident, objcToken what)
{
	if (what == ObjcIDENTIFIER)
	{
		addTag (ident, K_INTERFACE);
		pushEnclosingContext (ident, K_INTERFACE);
	}

	toDoNext = &parseMethods;
}

static void parseStructMembers (vString * const ident, objcToken what)
{
	static parseNext prev = NULL;

	if (prev != NULL)
	{
		comeAfter = prev;
		prev = NULL;
	}

	switch (what)
	{
	case ObjcIDENTIFIER:
		vStringCopy (tempName, ident);
		break;

	case Tok_semi:	/* ';' */
		addTag (tempName, K_FIELD);
		vStringClear (tempName);
		break;

		// some types are complex, the only one
		// we will loose is the function type.
	case Tok_CurlL:	/* '{' */
	case Tok_PARL:	/* '(' */
	case Tok_SQUAREL:	/* '[' */
		toDoNext = &ignoreBalanced;
		prev = comeAfter;
		comeAfter = &parseStructMembers;
		ignoreBalanced (ident, what);
		break;

	case Tok_CurlR:
		toDoNext = comeAfter;
		break;

	default:
		/* don't care */
		break;
	}
}

/* Called just after the struct keyword */
static void parseStruct (vString * const ident, objcToken what)
{
	static boolean gotName = FALSE;

	switch (what)
	{
	case ObjcIDENTIFIER:
		if (!gotName)
		{
			addTag (ident, K_STRUCT);
			pushEnclosingContext (ident, K_STRUCT);
			gotName = TRUE;
		}
		else
		{
			gotName = FALSE;
			popEnclosingContext ();
			toDoNext = comeAfter;
			comeAfter (ident, what);
		}
		break;

	case Tok_CurlL:
		toDoNext = &parseStructMembers;
		break;

		/* maybe it was just a forward declaration
		 * in which case, we pop the context */
	case Tok_semi:
		if (gotName)
			popEnclosingContext ();

		toDoNext = comeAfter;
		comeAfter (ident, what);
		break;

	default:
		/* we don't care */
		break;
	}
}

/* Parse enumeration members, ignoring potential initialization */
static void parseEnumFields (vString * const ident, objcToken what)
{
	static parseNext prev = NULL;

	if (prev != NULL)
	{
		comeAfter = prev;
		prev = NULL;
	}

	switch (what)
	{
	case ObjcIDENTIFIER:
		addTag (ident, K_ENUM);
		prev = comeAfter;
		waitedToken = Tok_COMA;
		/* last item might not have a coma */
		fallBackToken = Tok_CurlR;
		fallback = comeAfter;
		comeAfter = parseEnumFields;
		toDoNext = &tillTokenOrFallBack;
		break;

	case Tok_CurlR:
		toDoNext = comeAfter;
		popEnclosingContext ();
		break;

	default:
		/* don't care */
		break;
	}
}

/* parse enum ... { ... */
static void parseEnum (vString * const ident, objcToken what)
{
	static boolean named = FALSE;

	switch (what)
	{
	case ObjcIDENTIFIER:
		if (!named)
		{
			addTag (ident, K_ENUM);
			pushEnclosingContext (ident, K_ENUM);
			named = TRUE;
		}
		else
		{
			named = FALSE;
			popEnclosingContext ();
			toDoNext = comeAfter;
			comeAfter (ident, what);
		}
		break;

	case Tok_CurlL:	/* '{' */
		toDoNext = &parseEnumFields;
		named = FALSE;
		break;

	case Tok_semi:	/* ';' */
		if (named)
			popEnclosingContext ();
		toDoNext = comeAfter;
		comeAfter (ident, what);
		break;

	default:
		/* don't care */
		break;
	}
}

/* Parse something like
 * typedef .... ident ;
 * ignoring the defined type but in the case of struct,
 * in which case struct are parsed.
 */
static void parseTypedef (vString * const ident, objcToken what)
{
	switch (what)
	{
	case ObjcSTRUCT:
		toDoNext = &parseStruct;
		comeAfter = &parseTypedef;
		break;

	case ObjcENUM:
		toDoNext = &parseEnum;
		comeAfter = &parseTypedef;
		break;

	case ObjcIDENTIFIER:
		vStringCopy (tempName, ident);
		break;

	case Tok_semi:	/* ';' */
		addTag (tempName, K_TYPEDEF);
		vStringClear (tempName);
		toDoNext = &globalScope;
		break;

	default:
		/* we don't care */
		break;
	}
}

static void ignorePreprocStuff (vString * const UNUSED (ident), objcToken what)
{
	static boolean escaped = FALSE;

	switch (what)
	{
	case Tok_Backslash:
		escaped = TRUE;
		break;

	case Tok_EOL:
		if (escaped)
		{
			escaped = FALSE;
		}
		else
		{
			toDoNext = &globalScope;
		}
		break;

	default:
		escaped = FALSE;
		break;
	}
}

static void parseMacroName (vString * const ident, objcToken what)
{
	if (what == ObjcIDENTIFIER)
		addTag (ident, K_MACRO);

	toDoNext = &ignorePreprocStuff;
}

static void parsePreproc (vString * const ident, objcToken what)
{
	switch (what)
	{
	case ObjcIDENTIFIER:
		if (strcmp (vStringValue (ident), "define") == 0)
			toDoNext = &parseMacroName;
		else
			toDoNext = &ignorePreprocStuff;
		break;

	default:
		toDoNext = &ignorePreprocStuff;
		break;
	}
}

/* Handle the "strong" top levels, all 'big' declarations
 * happen here */
static void globalScope (vString * const ident, objcToken what)
{
	switch (what)
	{
	case Tok_Sharp:
		toDoNext = &parsePreproc;
		break;

	case ObjcSTRUCT:
		toDoNext = &parseStruct;
		comeAfter = &globalScope;
		break;

	case ObjcIDENTIFIER:
		/* we keep track of the identifier if we
		 * come across a function. */
		vStringCopy (tempName, ident);
		break;

	case Tok_PARL:
		/* if we find an opening parenthesis it means we
		 * found a function (or a macro...) */
		addTag (tempName, K_FUNCTION);
		vStringClear (tempName);
		comeAfter = &globalScope;
		toDoNext = &ignoreBalanced;
		ignoreBalanced (ident, what);
		break;

	case ObjcINTERFACE:
		toDoNext = &parseInterface;
		break;

	case ObjcIMPLEMENTATION:
		toDoNext = &parseImplementation;
		break;

	case ObjcPROTOCOL:
		toDoNext = &parseProtocol;
		break;

	case ObjcTYPEDEF:
		toDoNext = parseTypedef;
		comeAfter = &globalScope;
		break;

	case Tok_CurlL:
		comeAfter = &globalScope;
		toDoNext = &ignoreBalanced;
		ignoreBalanced (ident, what);
		break;

	case ObjcEND:
	case ObjcPUBLIC:
	case ObjcPROTECTED:
	case ObjcPRIVATE:

	default:
		/* we don't care */
		break;
	}
}

/*////////////////////////////////////////////////////////////////
//// Deal with the system                                       */

static void findObjcTags (void)
{
	vString *name = vStringNew ();
	lexingState st;
	objcToken tok;

	parentName = vStringNew ();
	tempName = vStringNew ();
	fullMethodName = vStringNew ();
	prevIdent = vStringNew ();

	st.name = vStringNew ();
	st.cp = fileReadLine ();
	toDoNext = &globalScope;
	tok = lex (&st);
	while (tok != Tok_EOF)
	{
		(*toDoNext) (st.name, tok);
		tok = lex (&st);
	}

	vStringDelete (name);
	vStringDelete (parentName);
	vStringDelete (tempName);
	vStringDelete (fullMethodName);
	vStringDelete (prevIdent);
	parentName = NULL;
	tempName = NULL;
	prevIdent = NULL;
	fullMethodName = NULL;
}

static void objcInitialize (const langType language)
{
	Lang_ObjectiveC = language;

	initKeywordHash ();
}

extern parserDefinition *ObjcParser (void)
{
	static const char *const extensions[] = { "m", "h", NULL };
	parserDefinition *def = parserNew ("ObjectiveC");
	def->kinds = ObjcKinds;
	def->kindCount = KIND_COUNT (ObjcKinds);
	def->extensions = extensions;
	def->parser = findObjcTags;
	def->initialize = objcInitialize;

	return def;
}
