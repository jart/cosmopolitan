/*
*   INCLUDE FILES
*/
#include "third_party/ctags/general.h"        /* must always come first */
#include "libc/runtime/runtime.h"

#include "third_party/ctags/debug.h"
#include "third_party/ctags/entry.h"
#include "third_party/ctags/keyword.h"
#include "third_party/ctags/read.h"
#include "third_party/ctags/main.h"
#include "third_party/ctags/routines.h"
#include "third_party/ctags/vstring.h"
#include "third_party/ctags/options.h"

/*
 *	 MACROS
 */
#define isType(token,t) (boolean) ((token)->type == (t))
#define isKeyword(token,k) (boolean) ((token)->keyword == (k))

/*
 *	 DATA DECLARATIONS
 */

typedef enum eException { ExceptionNone, ExceptionEOF } exception_t;

typedef enum eKeywordId {
	KEYWORD_NONE = -1,
	KEYWORD_package,
	KEYWORD_import,
	KEYWORD_const,
	KEYWORD_type,
	KEYWORD_var,
	KEYWORD_func,
	KEYWORD_struct,
	KEYWORD_interface,
	KEYWORD_map,
	KEYWORD_chan
} keywordId;

/*  Used to determine whether keyword is valid for the current language and
 *  what its ID is.
 */
typedef struct sKeywordDesc {
	const char *name;
	keywordId id;
} keywordDesc;

typedef enum eTokenType {
	TOKEN_NONE = -1,
	TOKEN_CHARACTER,
	// Don't need TOKEN_FORWARD_SLASH
	TOKEN_FORWARD_SLASH,
	TOKEN_KEYWORD,
	TOKEN_IDENTIFIER,
	TOKEN_STRING,
	TOKEN_OPEN_PAREN,
	TOKEN_CLOSE_PAREN,
	TOKEN_OPEN_CURLY,
	TOKEN_CLOSE_CURLY,
	TOKEN_OPEN_SQUARE,
	TOKEN_CLOSE_SQUARE,
	TOKEN_SEMICOLON,
	TOKEN_STAR,
	TOKEN_LEFT_ARROW,
	TOKEN_DOT,
	TOKEN_COMMA
} tokenType;

typedef struct sTokenInfo {
	tokenType type;
	keywordId keyword;
	vString *string;		/* the name of the token */
	unsigned long lineNumber;	/* line number of tag */
	fpos_t filePosition;		/* file position of line containing name */
} tokenInfo;

/*
*   DATA DEFINITIONS
*/

static int Lang_go;
static jmp_buf Exception;
static vString *scope;

typedef enum {
	GOTAG_UNDEFINED = -1,
	GOTAG_PACKAGE,
	GOTAG_FUNCTION,
	GOTAG_CONST,
	GOTAG_TYPE,
	GOTAG_VAR,
} goKind;

static kindOption GoKinds[] = {
	{TRUE, 'p', "package", "packages"},
	{TRUE, 'f', "func", "functions"},
	{TRUE, 'c', "const", "constants"},
	{TRUE, 't', "type", "types"},
	{TRUE, 'v', "var", "variables"}
};

static keywordDesc GoKeywordTable[] = {
	{"package", KEYWORD_package},
	{"import", KEYWORD_import},
	{"const", KEYWORD_const},
	{"type", KEYWORD_type},
	{"var", KEYWORD_var},
	{"func", KEYWORD_func},
	{"struct", KEYWORD_struct},
	{"interface", KEYWORD_interface},
	{"map", KEYWORD_map},
	{"chan", KEYWORD_chan}
};

/*
*   FUNCTION DEFINITIONS
*/

// XXX UTF-8
static boolean isIdentChar (const int c)
{
	return (boolean)
		(isalpha (c) || isdigit (c) || c == '$' ||
		 c == '@' || c == '_' || c == '#' || c > 128);
}

static void initialize (const langType language)
{
	size_t i;
	const size_t count =
		sizeof (GoKeywordTable) / sizeof (GoKeywordTable[0]);
	Lang_go = language;
	for (i = 0; i < count; ++i)
	{
		const keywordDesc *const p = &GoKeywordTable[i];
		addKeyword (p->name, language, (int) p->id);
	}
}

static tokenInfo *newToken (void)
{
	tokenInfo *const token = xMalloc (1, tokenInfo);
	token->type = TOKEN_NONE;
	token->keyword = KEYWORD_NONE;
	token->string = vStringNew ();
	token->lineNumber = getSourceLineNumber ();
	token->filePosition = getInputFilePosition ();
	return token;
}

static void deleteToken (tokenInfo * const token)
{
	if (token != NULL)
	{
		vStringDelete (token->string);
		eFree (token);
	}
}

/*
 *   Parsing functions
 */

static void parseString (vString *const string, const int delimiter)
{
	boolean end = FALSE;
	while (!end)
	{
		int c = fileGetc ();
		if (c == EOF)
			end = TRUE;
		else if (c == '\\' && delimiter != '`')
		{
			c = fileGetc ();	/* This maybe a ' or ". */
			vStringPut (string, c);
		}
		else if (c == delimiter)
			end = TRUE;
		else
			vStringPut (string, c);
	}
	vStringTerminate (string);
}

static void parseIdentifier (vString *const string, const int firstChar)
{
	int c = firstChar;
	//Assert (isIdentChar (c));
	do
	{
		vStringPut (string, c);
		c = fileGetc ();
	} while (isIdentChar (c));
	vStringTerminate (string);
	fileUngetc (c);		/* always unget, LF might add a semicolon */
}

static void readToken (tokenInfo *const token)
{
	int c;
	static tokenType lastTokenType = TOKEN_NONE;

	token->type = TOKEN_NONE;
	token->keyword = KEYWORD_NONE;
	vStringClear (token->string);

getNextChar:
	do
	{
		c = fileGetc ();
		token->lineNumber = getSourceLineNumber ();
		token->filePosition = getInputFilePosition ();
		if (c == '\n' && (lastTokenType == TOKEN_IDENTIFIER ||
						  lastTokenType == TOKEN_STRING ||
						  lastTokenType == TOKEN_CLOSE_PAREN ||
						  lastTokenType == TOKEN_CLOSE_CURLY ||
						  lastTokenType == TOKEN_CLOSE_SQUARE))
		{
			token->type = TOKEN_SEMICOLON;
			goto done;
		}
	}
	while (c == '\t'  ||  c == ' ' ||  c == '\r' || c == '\n');

	switch (c)
	{
		case EOF:
			longjmp (Exception, (int)ExceptionEOF);
			break;

		case '/':
			{
				boolean hasNewline = FALSE;
				int d = fileGetc ();
				switch (d)
				{
					case '/':
						fileSkipToCharacter ('\n');
						/* Line comments start with the
						 * character sequence // and
						 * continue through the next
						 * newline. A line comment acts
						 * like a newline.  */
						fileUngetc ('\n');
						goto getNextChar;
					case '*':
						do
						{
							int d;
							do
							{
								d = fileGetc ();
								if (d == '\n')
								{
									hasNewline = TRUE;
								}
							} while (d != EOF && d != '*');

							c = fileGetc ();
							if (c == '/')
								break;
							else
								fileUngetc (c);
						} while (c != EOF && c != '\0');

						fileUngetc (hasNewline ? '\n' : ' ');
						goto getNextChar;
					default:
						token->type = TOKEN_FORWARD_SLASH;
						fileUngetc (d);
						break;
				}
			}
			break;

		case '"':
		case '\'':
		case '`':
			token->type = TOKEN_STRING;
			parseString (token->string, c);
			token->lineNumber = getSourceLineNumber ();
			token->filePosition = getInputFilePosition ();
			break;

		case '<':
			{
				int d = fileGetc ();
				if (d == '-')
				{
					token->type = TOKEN_LEFT_ARROW;
					break;
				}
				else
					goto getNextChar;
			}

		case '(':
			token->type = TOKEN_OPEN_PAREN;
			break;

		case ')':
			token->type = TOKEN_CLOSE_PAREN;
			break;

		case '{':
			token->type = TOKEN_OPEN_CURLY;
			break;

		case '}':
			token->type = TOKEN_CLOSE_CURLY;
			break;

		case '[':
			token->type = TOKEN_OPEN_SQUARE;
			break;

		case ']':
			token->type = TOKEN_CLOSE_SQUARE;
			break;

		case '*':
			token->type = TOKEN_STAR;
			break;

		case '.':
			token->type = TOKEN_DOT;
			break;

		case ',':
			token->type = TOKEN_COMMA;
			break;

		default:
			parseIdentifier (token->string, c);
			token->lineNumber = getSourceLineNumber ();
			token->filePosition = getInputFilePosition ();
			token->keyword = lookupKeyword (vStringValue (token->string), Lang_go);
			if (isKeyword (token, KEYWORD_NONE))
				token->type = TOKEN_IDENTIFIER;
			else
				token->type = TOKEN_KEYWORD;
			break;
	}

done:
	lastTokenType = token->type;
}

static void skipToMatched (tokenInfo *const token)
{
	int nest_level = 0;
	tokenType open_token;
	tokenType close_token;

	switch (token->type)
	{
		case TOKEN_OPEN_PAREN:
			open_token = TOKEN_OPEN_PAREN;
			close_token = TOKEN_CLOSE_PAREN;
			break;
		case TOKEN_OPEN_CURLY:
			open_token = TOKEN_OPEN_CURLY;
			close_token = TOKEN_CLOSE_CURLY;
			break;
		case TOKEN_OPEN_SQUARE:
			open_token = TOKEN_OPEN_SQUARE;
			close_token = TOKEN_CLOSE_SQUARE;
			break;
		default:
			return;
	}

	/*
	 * This routine will skip to a matching closing token.
	 * It will also handle nested tokens like the (, ) below.
	 *   (  name varchar(30), text binary(10)  )
	 */
	if (isType (token, open_token))
	{
		nest_level++;
		while (!(isType (token, close_token) && (nest_level == 0)))
		{
			readToken (token);
			if (isType (token, open_token))
			{
				nest_level++;
			}
			if (isType (token, close_token))
			{
				if (nest_level > 0)
				{
					nest_level--;
				}
			}
		}
		readToken (token);
	}
}

static void skipType (tokenInfo *const token)
{
again:
	// Type      = TypeName | TypeLit | "(" Type ")" .
	if (isType (token, TOKEN_OPEN_PAREN))
	{
		skipToMatched (token);
		return;
	}

	// TypeName  = QualifiedIdent.
	// QualifiedIdent = [ PackageName "." ] identifier .
	// PackageName    = identifier .
	if (isType (token, TOKEN_IDENTIFIER))
	{
		readToken (token);
		if (isType (token, TOKEN_DOT))
		{
			readToken (token);
			Assert (isType (token, TOKEN_IDENTIFIER));
			readToken (token);
		}
		return;
	}

	// StructType     = "struct" "{" { FieldDecl ";" } "}"
	// InterfaceType      = "interface" "{" { MethodSpec ";" } "}" .
	if (isKeyword (token, KEYWORD_struct) || isKeyword (token, KEYWORD_interface))
	{
		readToken (token);
		Assert (isType (token, TOKEN_OPEN_CURLY));
		skipToMatched (token);
		return;
	}

	// ArrayType   = "[" ArrayLength "]" ElementType .
	// SliceType = "[" "]" ElementType .
	// ElementType = Type .
	if (isType (token, TOKEN_OPEN_SQUARE))
	{
		skipToMatched (token);
		goto again;
	}

	// PointerType = "*" BaseType .
	// BaseType = Type .
	// ChannelType = ( "chan" [ "<-" ] | "<-" "chan" ) ElementType .
	if (isType (token, TOKEN_STAR) || isKeyword (token, KEYWORD_chan) || isType (token, TOKEN_LEFT_ARROW))
	{
		readToken (token);
		goto again;
	}

	// MapType     = "map" "[" KeyType "]" ElementType .
	// KeyType     = Type .
	if (isKeyword (token, KEYWORD_map))
	{
		readToken (token);
		Assert (isType (token, TOKEN_OPEN_SQUARE));
		skipToMatched (token);
		goto again;
	}

	// FunctionType   = "func" Signature .
	// Signature      = Parameters [ Result ] .
	// Result         = Parameters | Type .
	// Parameters     = "(" [ ParameterList [ "," ] ] ")" .
	if (isKeyword (token, KEYWORD_func))
	{
		readToken (token);
		Assert (isType (token, TOKEN_OPEN_PAREN));
		// Parameters
		skipToMatched (token);
		// Result is parameters or type or nothing.  skipType treats anything
		// surrounded by parentheses as a type, and does nothing if what
		// follows is not a type.
		goto again;
	}
}

// Skip to the next semicolon, skipping over matching brackets.
static void skipToTopLevelSemicolon (tokenInfo *const token)
{
	while (!isType (token, TOKEN_SEMICOLON))
	{
		readToken (token);
		skipToMatched (token);
	}
}

static void makeTag (tokenInfo *const token, const goKind kind)
{
	const char *const name = vStringValue (token->string);

	tagEntryInfo e;
	initTagEntry (&e, name);

	if (!GoKinds [kind].enabled)
		return;

	e.lineNumber = token->lineNumber;
	e.filePosition = token->filePosition;
	e.kindName = GoKinds [kind].name;
	e.kind = GoKinds [kind].letter;

	makeTagEntry (&e);

	if (scope && Option.include.qualifiedTags)
	{
		vString *qualifiedName = vStringNew ();
		vStringCopy (qualifiedName, scope);
		vStringCatS (qualifiedName, ".");
		vStringCat (qualifiedName, token->string);
		e.name = vStringValue (qualifiedName);
		makeTagEntry (&e);
		vStringDelete (qualifiedName);
	}
}

static void parsePackage (tokenInfo *const token)
{
	tokenInfo *const name = newToken ();

	readToken (name);
	Assert (isType (name, TOKEN_IDENTIFIER));
	makeTag (name, GOTAG_PACKAGE);
	if (!scope && Option.include.qualifiedTags)
	{
		scope = vStringNew ();
		vStringCopy (scope, name->string);
	}

	deleteToken (name);
}

static void parseFunctionOrMethod (tokenInfo *const token)
{
	// FunctionDecl = "func" identifier Signature [ Body ] .
	// Body         = Block.
	//
	// MethodDecl   = "func" Receiver MethodName Signature [ Body ] .
	// Receiver     = "(" [ identifier ] [ "*" ] BaseTypeName ")" .
	// BaseTypeName = identifier .
	tokenInfo *const name = newToken ();

	// Skip over receiver.
	readToken (name);
	if (isType (name, TOKEN_OPEN_PAREN))
		skipToMatched (name);

	Assert (isType (name, TOKEN_IDENTIFIER));

	// Skip over parameters.
	readToken (token);
	skipToMatched (token);

	// Skip over result.
	skipType (token);

	// Skip over function body.
	if (isType (token, TOKEN_OPEN_CURLY))
		skipToMatched (token);

	makeTag (name, GOTAG_FUNCTION);

	deleteToken (name);
}

static void parseConstTypeVar (tokenInfo *const token, goKind kind)
{
	// ConstDecl      = "const" ( ConstSpec | "(" { ConstSpec ";" } ")" ) .
	// ConstSpec      = IdentifierList [ [ Type ] "=" ExpressionList ] .
	// IdentifierList = identifier { "," identifier } .
	// ExpressionList = Expression { "," Expression } .
	// TypeDecl     = "type" ( TypeSpec | "(" { TypeSpec ";" } ")" ) .
	// TypeSpec     = identifier Type .
	// VarDecl     = "var" ( VarSpec | "(" { VarSpec ";" } ")" ) .
	// VarSpec     = IdentifierList ( Type [ "=" ExpressionList ] | "=" ExpressionList ) .
	tokenInfo *const name = newToken ();
	boolean usesParens = FALSE;

	readToken (name);

	if (isType (name, TOKEN_OPEN_PAREN))
	{
		usesParens = TRUE;
		readToken (name);
	}

again:
	while (1)
	{
		makeTag (name, kind);
		readToken (token);
		if (!isType (token, TOKEN_COMMA) && !isType (token, TOKEN_CLOSE_PAREN))
			break;
		readToken (name);
	}

	skipType (token);
	skipToTopLevelSemicolon (token);

	if (usesParens)
	{
		readToken (name);
		if (!isType (name, TOKEN_CLOSE_PAREN))
			goto again;
	}

	deleteToken (name);
}

static void parseGoFile (tokenInfo *const token)
{
	do
	{
		readToken (token);

		if (isType (token, TOKEN_KEYWORD))
		{
			switch (token->keyword)
			{
				case KEYWORD_package:
					parsePackage (token);
					break;
				case KEYWORD_func:
					parseFunctionOrMethod (token);
					break;
				case KEYWORD_const:
					parseConstTypeVar (token, GOTAG_CONST);
					break;
				case KEYWORD_type:
					parseConstTypeVar (token, GOTAG_TYPE);
					break;
				case KEYWORD_var:
					parseConstTypeVar (token, GOTAG_VAR);
					break;
				default:
					break;
			}
		}
	} while (TRUE);
}

static void findGoTags (void)
{
	tokenInfo *const token = newToken ();
	exception_t exception;

	exception = (exception_t) (setjmp (Exception));
	while (exception == ExceptionNone)
		parseGoFile (token);

	deleteToken (token);
	vStringDelete (scope);
	scope = NULL;
}

extern parserDefinition *GoParser (void)
{
	static const char *const extensions[] = { "go", NULL };
	parserDefinition *def = parserNew ("Go");
	def->kinds = GoKinds;
	def->kindCount = KIND_COUNT (GoKinds);
	def->extensions = extensions;
	def->parser = findGoTags;
	def->initialize = initialize;
	return def;
}
