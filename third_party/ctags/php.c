/*
*   $Id: php.c 734 2009-08-20 23:33:54Z jafl $
*
*   Copyright (c) 2000, Jesus Castagnetto <jmcastagnetto@zkey.com>
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   This module contains functions for generating tags for the PHP web page
*   scripting language. Only recognizes functions and classes, not methods or
*   variables.
*
*   Parsing PHP defines by Pavel Hlousek <pavel.hlousek@seznam.cz>, Apr 2003.
*/

/*
*   INCLUDE FILES
*/
#include "third_party/ctags/general.h"  /* must always come first */

#include "libc/mem/alg.h"
#include "libc/str/str.h"

#include "third_party/ctags/parse.h"
#include "third_party/ctags/read.h"
#include "third_party/ctags/vstring.h"

/*
*   DATA DEFINITIONS
*/
typedef enum {
	K_CLASS, K_DEFINE, K_FUNCTION, K_VARIABLE
} phpKind;

#if 0
static kindOption PhpKinds [] = {
	{ TRUE, 'c', "class",    "classes" },
	{ TRUE, 'd', "define",   "constant definitions" },
	{ TRUE, 'f', "function", "functions" },
	{ TRUE, 'v', "variable", "variables" }
};
#endif

/*
*   FUNCTION DEFINITIONS
*/

/* JavaScript patterns are duplicated in jscript.c */

#define ALPHA "[:alpha:]"
#define ALNUM "[:alnum:]"

static void installPHPRegex (const langType language)
{
	addTagRegex(language, "^[ \t]*((final|abstract)[ \t]+)*class[ \t]+([" ALPHA "_][" ALNUM "_]*)",
		"\\3", "c,class,classes", NULL);
	addTagRegex(language, "^[ \t]*interface[ \t]+([" ALPHA "_][" ALNUM "_]*)",
		"\\1", "i,interface,interfaces", NULL);
	addTagRegex(language, "^[ \t]*define[ \t]*\\([ \t]*['\"]?([" ALPHA "_][" ALNUM "_]*)",
		"\\1", "d,define,constant definitions", NULL);
	addTagRegex(language, "^[ \t]*((static|public|protected|private)[ \t]+)*function[ \t]+&?[ \t]*([" ALPHA "_][" ALNUM "_]*)",
		"\\3", "f,function,functions", NULL);
	addTagRegex(language, "^[ \t]*(\\$|::\\$|\\$this->)([" ALPHA "_][" ALNUM "_]*)[ \t]*=",
		"\\2", "v,variable,variables", NULL);
	addTagRegex(language, "^[ \t]*((var|public|protected|private|static)[ \t]+)+\\$([" ALPHA "_][" ALNUM "_]*)[ \t]*[=;]",
		"\\3", "v,variable,variables", NULL);

	/* function regex is covered by PHP regex */
	addTagRegex (language, "(^|[ \t])([A-Za-z0-9_]+)[ \t]*[=:][ \t]*function[ \t]*\\(",
		"\\2", "j,jsfunction,javascript functions", NULL);
	addTagRegex (language, "(^|[ \t])([A-Za-z0-9_.]+)\\.([A-Za-z0-9_]+)[ \t]*=[ \t]*function[ \t]*\\(",
		"\\2.\\3", "j,jsfunction,javascript functions", NULL);
	addTagRegex (language, "(^|[ \t])([A-Za-z0-9_.]+)\\.([A-Za-z0-9_]+)[ \t]*=[ \t]*function[ \t]*\\(",
		"\\3", "j,jsfunction,javascript functions", NULL);
}

/* Create parser definition structure */
extern parserDefinition* PhpParser (void)
{
	static const char *const extensions [] = { "php", "php3", "phtml", NULL };
	parserDefinition* def = parserNew ("PHP");
	def->extensions = extensions;
	def->initialize = installPHPRegex;
	def->regex      = TRUE;
	return def;
}

#if 0

static boolean isLetter(const int c)
{
	return (boolean)(isalpha(c) || (c >= 127  &&  c <= 255));
}

static boolean isVarChar1(const int c)
{
	return (boolean)(isLetter (c)  ||  c == '_');
}

static boolean isVarChar(const int c)
{
	return (boolean)(isVarChar1 (c) || isdigit (c));
}

static void findPhpTags (void)
{
	vString *name = vStringNew ();
	const unsigned char *line;

	while ((line = fileReadLine ()) != NULL)
	{
		const unsigned char *cp = line;
		const char* f;

		while (isspace (*cp))
			cp++;

		if (*(const char*)cp == '$'  &&  isVarChar1 (*(const char*)(cp+1)))
		{
			cp += 1;
			vStringClear (name);
			while (isVarChar ((int) *cp))
			{
				vStringPut (name, (int) *cp);
				++cp;
			}
			while (isspace ((int) *cp))
				++cp;
			if (*(const char*) cp == '=')
			{
				vStringTerminate (name);
				makeSimpleTag (name, PhpKinds, K_VARIABLE);
				vStringClear (name);
			}
		}
		else if ((f = strstr ((const char*) cp, "function")) != NULL &&
			(f == (const char*) cp || isspace ((int) f [-1])) &&
			isspace ((int) f [8]))
		{
			cp = ((const unsigned char *) f) + 8;

			while (isspace ((int) *cp))
				++cp;

			if (*cp == '&')	/* skip reference character and following whitespace */
			{
				cp++;

				while (isspace ((int) *cp))
					++cp; 
			}

			vStringClear (name);
			while (isalnum ((int) *cp)  ||  *cp == '_')
			{
				vStringPut (name, (int) *cp);
				++cp;
			}
			vStringTerminate (name);
			makeSimpleTag (name, PhpKinds, K_FUNCTION);
			vStringClear (name);
		} 
		else if (strncmp ((const char*) cp, "class", (size_t) 5) == 0 &&
				 isspace ((int) cp [5]))
		{
			cp += 5;

			while (isspace ((int) *cp))
				++cp;
			vStringClear (name);
			while (isalnum ((int) *cp)  ||  *cp == '_')
			{
				vStringPut (name, (int) *cp);
				++cp;
			}
			vStringTerminate (name);
			makeSimpleTag (name, PhpKinds, K_CLASS);
			vStringClear (name);
		}
		else if (strncmp ((const char*) cp, "define", (size_t) 6) == 0 &&
				 ! isalnum ((int) cp [6]))
		{
			cp += 6;

			while (isspace ((int) *cp))
				++cp;
			if (*cp != '(')
				continue;
			++cp;

			while (isspace ((int) *cp))
				++cp;
			if ((*cp == '\'') || (*cp == '"'))
				++cp;
			else if (! ((*cp == '_')  || isalnum ((int) *cp)))
				continue;
	      
			vStringClear (name);
			while (isalnum ((int) *cp)  ||  *cp == '_')
			{
				vStringPut (name, (int) *cp);
				++cp;
			}
			vStringTerminate (name);
			makeSimpleTag (name, PhpKinds, K_DEFINE);
			vStringClear (name);
		}
	}
	vStringDelete (name);
}

extern parserDefinition* PhpParser (void)
{
	static const char *const extensions [] = { "php", "php3", "phtml", NULL };
	parserDefinition* def = parserNew ("PHP");
	def->kinds      = PhpKinds;
	def->kindCount  = KIND_COUNT (PhpKinds);
	def->extensions = extensions;
	def->parser     = findPhpTags;
	return def;
}

#endif

/* vi:set tabstop=4 shiftwidth=4: */
