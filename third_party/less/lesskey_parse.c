/*
 * Copyright (C) 1984-2023  Mark Nudelman
 *
 * You may distribute under the terms of either the GNU General Public
 * License or the Less License, as specified in the README file.
 *
 * For more information, see the README file.
 */

#include "defines.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lesskey.h"
#include "cmd.h"
#include "xbuf.h"

#define CONTROL(c)      ((c)&037)
#define ESC             CONTROL('[')

extern void lesskey_parse_error(char *msg);
extern char *homefile(char *filename);
extern void *ecalloc(int count, unsigned int size);
extern int lstrtoi(char *str, char **end, int radix);
extern char version[];

static int linenum;
static int errors;
static int less_version = 0;
static char *lesskey_file;

static struct lesskey_cmdname cmdnames[] = 
{
	{ "back-bracket",         A_B_BRACKET },
	{ "back-line",            A_B_LINE },
	{ "back-line-force",      A_BF_LINE },
	{ "back-screen",          A_B_SCREEN },
	{ "back-scroll",          A_B_SCROLL },
	{ "back-search",          A_B_SEARCH },
	{ "back-window",          A_B_WINDOW },
	{ "clear-mark",           A_CLRMARK },
	{ "debug",                A_DEBUG },
	{ "digit",                A_DIGIT },
	{ "display-flag",         A_DISP_OPTION },
	{ "display-option",       A_DISP_OPTION },
	{ "end",                  A_GOEND },
	{ "end-scroll",           A_RRSHIFT },
	{ "examine",              A_EXAMINE },
	{ "filter",               A_FILTER },
	{ "first-cmd",            A_FIRSTCMD },
	{ "firstcmd",             A_FIRSTCMD },
	{ "flush-repaint",        A_FREPAINT },
	{ "forw-bracket",         A_F_BRACKET },
	{ "forw-forever",         A_F_FOREVER },
	{ "forw-until-hilite",    A_F_UNTIL_HILITE },
	{ "forw-line",            A_F_LINE },
	{ "forw-line-force",      A_FF_LINE },
	{ "forw-screen",          A_F_SCREEN },
	{ "forw-screen-force",    A_FF_SCREEN },
	{ "forw-scroll",          A_F_SCROLL },
	{ "forw-search",          A_F_SEARCH },
	{ "forw-window",          A_F_WINDOW },
	{ "goto-end",             A_GOEND },
	{ "goto-end-buffered",    A_GOEND_BUF },
	{ "goto-line",            A_GOLINE },
	{ "goto-mark",            A_GOMARK },
	{ "help",                 A_HELP },
	{ "index-file",           A_INDEX_FILE },
	{ "invalid",              A_UINVALID },
	{ "left-scroll",          A_LSHIFT },
	{ "next-file",            A_NEXT_FILE },
	{ "next-tag",             A_NEXT_TAG },
	{ "noaction",             A_NOACTION },
	{ "no-scroll",            A_LLSHIFT },
	{ "percent",              A_PERCENT },
	{ "pipe",                 A_PIPE },
	{ "prev-file",            A_PREV_FILE },
	{ "prev-tag",             A_PREV_TAG },
	{ "quit",                 A_QUIT },
	{ "remove-file",          A_REMOVE_FILE },
	{ "repaint",              A_REPAINT },
	{ "repaint-flush",        A_FREPAINT },
	{ "repeat-search",        A_AGAIN_SEARCH },
	{ "repeat-search-all",    A_T_AGAIN_SEARCH },
	{ "reverse-search",       A_REVERSE_SEARCH },
	{ "reverse-search-all",   A_T_REVERSE_SEARCH },
	{ "right-scroll",         A_RSHIFT },
	{ "set-mark",             A_SETMARK },
	{ "set-mark-bottom",      A_SETMARKBOT },
	{ "shell",                A_SHELL },
	{ "pshell",               A_PSHELL },
	{ "status",               A_STAT },
	{ "toggle-flag",          A_OPT_TOGGLE },
	{ "toggle-option",        A_OPT_TOGGLE },
	{ "undo-hilite",          A_UNDO_SEARCH },
	{ "clear-search",         A_CLR_SEARCH },
	{ "version",              A_VERSION },
	{ "visual",               A_VISUAL },
	{ NULL,   0 }
};

static struct lesskey_cmdname editnames[] = 
{
	{ "back-complete",      EC_B_COMPLETE },
	{ "backspace",          EC_BACKSPACE },
	{ "delete",             EC_DELETE },
	{ "down",               EC_DOWN },
	{ "end",                EC_END },
	{ "expand",             EC_EXPAND },
	{ "forw-complete",      EC_F_COMPLETE },
	{ "home",               EC_HOME },
	{ "insert",             EC_INSERT },
	{ "invalid",            EC_UINVALID },
	{ "kill-line",          EC_LINEKILL },
	{ "abort",              EC_ABORT },
	{ "left",               EC_LEFT },
	{ "literal",            EC_LITERAL },
	{ "right",              EC_RIGHT },
	{ "up",                 EC_UP },
	{ "word-backspace",     EC_W_BACKSPACE },
	{ "word-delete",        EC_W_DELETE },
	{ "word-left",          EC_W_LEFT },
	{ "word-right",         EC_W_RIGHT },
	{ NULL, 0 }
};

/*
 * Print a parse error message.
 */
static void parse_error(char *fmt, char *arg1)
{
	char buf[1024];
	int n = snprintf(buf, sizeof(buf), "%s: line %d: ", lesskey_file, linenum);
	if (n >= 0 && n < sizeof(buf))
		snprintf(buf+n, sizeof(buf)-n, fmt, arg1);
	++errors;
	lesskey_parse_error(buf);
}

/*
 * Initialize lesskey_tables.
 */
static void init_tables(struct lesskey_tables *tables)
{
	tables->currtable = &tables->cmdtable;

	tables->cmdtable.names = cmdnames;
	tables->cmdtable.is_var = 0;
	xbuf_init(&tables->cmdtable.buf);

	tables->edittable.names = editnames;
	tables->edittable.is_var = 0;
	xbuf_init(&tables->edittable.buf);

	tables->vartable.names = NULL;
	tables->vartable.is_var = 1;
	xbuf_init(&tables->vartable.buf);
}

#define CHAR_STRING_LEN 8

static char * char_string(char *buf, int ch, int lit)
{
	if (lit || (ch >= 0x20 && ch < 0x7f))
	{
		buf[0] = ch;
		buf[1] = '\0';
	} else
	{
		snprintf(buf, CHAR_STRING_LEN, "\\x%02x", ch);
	}
	return buf;
}

/*
 * Increment char pointer by one up to terminating nul byte.
 */
static char * increment_pointer(char *p)
{
	if (*p == '\0')
		return p;
	return p+1;
}

/*
 * Parse one character of a string.
 */
static char * tstr(char **pp, int xlate)
{
	char *p;
	char ch;
	int i;
	static char buf[CHAR_STRING_LEN];
	static char tstr_control_k[] =
		{ SK_SPECIAL_KEY, SK_CONTROL_K, 6, 1, 1, 1, '\0' };

	p = *pp;
	switch (*p)
	{
	case '\\':
		++p;
		switch (*p)
		{
		case '0': case '1': case '2': case '3':
		case '4': case '5': case '6': case '7':
			/*
			 * Parse an octal number.
			 */
			ch = 0;
			i = 0;
			do
				ch = 8*ch + (*p - '0');
			while (*++p >= '0' && *p <= '7' && ++i < 3);
			*pp = p;
			if (xlate && ch == CONTROL('K'))
				return tstr_control_k;
			return char_string(buf, ch, 1);
		case 'b':
			*pp = p+1;
			return ("\b");
		case 'e':
			*pp = p+1;
			return char_string(buf, ESC, 1);
		case 'n':
			*pp = p+1;
			return ("\n");
		case 'r':
			*pp = p+1;
			return ("\r");
		case 't':
			*pp = p+1;
			return ("\t");
		case 'k':
			if (xlate)
			{
				switch (*++p)
				{
				case 'b': ch = SK_BACKSPACE; break;
				case 'B': ch = SK_CTL_BACKSPACE; break;
				case 'd': ch = SK_DOWN_ARROW; break;
				case 'D': ch = SK_PAGE_DOWN; break;
				case 'e': ch = SK_END; break;
				case 'h': ch = SK_HOME; break;
				case 'i': ch = SK_INSERT; break;
				case 'l': ch = SK_LEFT_ARROW; break;
				case 'L': ch = SK_CTL_LEFT_ARROW; break;
				case 'r': ch = SK_RIGHT_ARROW; break;
				case 'R': ch = SK_CTL_RIGHT_ARROW; break;
				case 't': ch = SK_BACKTAB; break;
				case 'u': ch = SK_UP_ARROW; break;
				case 'U': ch = SK_PAGE_UP; break;
				case 'x': ch = SK_DELETE; break;
				case 'X': ch = SK_CTL_DELETE; break;
				case '1': ch = SK_F1; break;
				default:
					parse_error("invalid escape sequence \"\\k%s\"", char_string(buf, *p, 0));
					*pp = increment_pointer(p);
					return ("");
				}
				*pp = p+1;
				buf[0] = SK_SPECIAL_KEY;
				buf[1] = ch;
				buf[2] = 6;
				buf[3] = 1;
				buf[4] = 1;
				buf[5] = 1;
				buf[6] = '\0';
				return (buf);
			}
			/* FALLTHRU */
		default:
			/*
			 * Backslash followed by any other char 
			 * just means that char.
			 */
			*pp = increment_pointer(p);
			char_string(buf, *p, 1);
			if (xlate && buf[0] == CONTROL('K'))
				return tstr_control_k;
			return (buf);
		}
	case '^':
		/*
		 * Caret means CONTROL.
		 */
		*pp = increment_pointer(p+1);
		char_string(buf, CONTROL(p[1]), 1);
		if (xlate && buf[0] == CONTROL('K'))
			return tstr_control_k;
		return (buf);
	}
	*pp = increment_pointer(p);
	char_string(buf, *p, 1);
	if (xlate && buf[0] == CONTROL('K'))
		return tstr_control_k;
	return (buf);
}

static int issp(char ch)
{
	return (ch == ' ' || ch == '\t');
}

/*
 * Skip leading spaces in a string.
 */
static char * skipsp(char *s)
{
	while (issp(*s))
		s++;
	return (s);
}

/*
 * Skip non-space characters in a string.
 */
static char * skipnsp(char *s)
{
	while (*s != '\0' && !issp(*s))
		s++;
	return (s);
}

/*
 * Clean up an input line:
 * strip off the trailing newline & any trailing # comment.
 */
static char * clean_line(char *s)
{
	int i;

	s = skipsp(s);
	for (i = 0;  s[i] != '\0' && s[i] != '\n' && s[i] != '\r';  i++)
		if (s[i] == '#' && (i == 0 || s[i-1] != '\\'))
			break;
	s[i] = '\0';
	return (s);
}

/*
 * Add a byte to the output command table.
 */
static void add_cmd_char(unsigned char c, struct lesskey_tables *tables)
{
	xbuf_add_byte(&tables->currtable->buf, c);
}

static void erase_cmd_char(struct lesskey_tables *tables)
{
	xbuf_pop(&tables->currtable->buf);
}

/*
 * Add a string to the output command table.
 */
static void add_cmd_str(char *s, struct lesskey_tables *tables)
{
	for ( ;  *s != '\0';  s++)
		add_cmd_char(*s, tables);
}

/*
 * Does a given version number match the running version?
 * Operator compares the running version to the given version.
 */
static int match_version(char op, int ver)
{
	switch (op)
	{
	case '>': return less_version > ver;
	case '<': return less_version < ver;
	case '+': return less_version >= ver;
	case '-': return less_version <= ver;
	case '=': return less_version == ver;
	case '!': return less_version != ver;
	default: return 0; /* cannot happen */
	}
}

/*
 * Handle a #version line.
 * If the version matches, return the part of the line that should be executed.
 * Otherwise, return NULL.
 */
static char * version_line(char *s, struct lesskey_tables *tables)
{
	char op;
	int ver;
	char *e;
	char buf[CHAR_STRING_LEN];

	s += strlen("#version");
	s = skipsp(s);
	op = *s++;
	/* Simplify 2-char op to one char. */
	switch (op)
	{
	case '<': if (*s == '=') { s++; op = '-'; } break;
	case '>': if (*s == '=') { s++; op = '+'; } break;
	case '=': if (*s == '=') { s++; } break;
	case '!': if (*s == '=') { s++; } break;
	default: 
		parse_error("invalid operator '%s' in #version line", char_string(buf, op, 0));
		return (NULL);
	}
	s = skipsp(s);
	ver = lstrtoi(s, &e, 10);
	if (e == s)
	{
		parse_error("non-numeric version number in #version line", "");
		return (NULL);
	}
	if (!match_version(op, ver))
		return (NULL);
	return (e);
}

/*
 * See if we have a special "control" line.
 */
static char * control_line(char *s, struct lesskey_tables *tables)
{
#define PREFIX(str,pat) (strncmp(str,pat,strlen(pat)) == 0)

	if (PREFIX(s, "#line-edit"))
	{
		tables->currtable = &tables->edittable;
		return (NULL);
	}
	if (PREFIX(s, "#command"))
	{
		tables->currtable = &tables->cmdtable;
		return (NULL);
	}
	if (PREFIX(s, "#env"))
	{
		tables->currtable = &tables->vartable;
		return (NULL);
	}
	if (PREFIX(s, "#stop"))
	{
		add_cmd_char('\0', tables);
		add_cmd_char(A_END_LIST, tables);
		return (NULL);
	}
	if (PREFIX(s, "#version"))
	{
		return (version_line(s, tables));
	}
	return (s);
}

/*
 * Find an action, given the name of the action.
 */
static int findaction(char *actname, struct lesskey_tables *tables)
{
	int i;

	for (i = 0;  tables->currtable->names[i].cn_name != NULL;  i++)
		if (strcmp(tables->currtable->names[i].cn_name, actname) == 0)
			return (tables->currtable->names[i].cn_action);
	parse_error("unknown action: \"%s\"", actname);
	return (A_INVALID);
}

/*
 * Parse a line describing one key binding, of the form
 *  KEY ACTION [EXTRA]
 * where KEY is the user key sequence, ACTION is the 
 * resulting less action, and EXTRA is an "extra" user
 * key sequence injected after the action.
 */
static void parse_cmdline(char *p, struct lesskey_tables *tables)
{
	char *actname;
	int action;
	char *s;
	char c;

	/*
	 * Parse the command string and store it in the current table.
	 */
	do
	{
		s = tstr(&p, 1);
		add_cmd_str(s, tables);
	} while (*p != '\0' && !issp(*p));
	/*
	 * Terminate the command string with a null byte.
	 */
	add_cmd_char('\0', tables);

	/*
	 * Skip white space between the command string
	 * and the action name.
	 * Terminate the action name with a null byte.
	 */
	p = skipsp(p);
	if (*p == '\0')
	{
		parse_error("missing action", "");
		return;
	}
	actname = p;
	p = skipnsp(p);
	c = *p;
	*p = '\0';

	/*
	 * Parse the action name and store it in the current table.
	 */
	action = findaction(actname, tables);

	/*
	 * See if an extra string follows the action name.
	 */
	*p = c;
	p = skipsp(p);
	if (*p == '\0')
	{
		add_cmd_char((unsigned char) action, tables);
	} else
	{
		/*
		 * OR the special value A_EXTRA into the action byte.
		 * Put the extra string after the action byte.
		 */
		add_cmd_char((unsigned char) (action | A_EXTRA), tables);
		while (*p != '\0')
			add_cmd_str(tstr(&p, 0), tables);
		add_cmd_char('\0', tables);
	}
}

/*
 * Parse a variable definition line, of the form
 *  NAME = VALUE
 */
static void parse_varline(char *line, struct lesskey_tables *tables)
{
	char *s;
	char *p = line;
	char *eq;

	eq = strchr(line, '=');
	if (eq != NULL && eq > line && eq[-1] == '+')
	{
		/*
		 * Rather ugly way of handling a += line.
		 * {{ Note that we ignore the variable name and 
		 *    just append to the previously defined variable. }}
		 */
		erase_cmd_char(tables); /* backspace over the final null */
		p = eq+1;
	} else
	{
		do
		{
			s = tstr(&p, 0);
			add_cmd_str(s, tables);
		} while (*p != '\0' && !issp(*p) && *p != '=');
		/*
		 * Terminate the variable name with a null byte.
		 */
		add_cmd_char('\0', tables);
		p = skipsp(p);
		if (*p++ != '=')
		{
			parse_error("missing = in variable definition", "");
			return;
		}
		add_cmd_char(EV_OK|A_EXTRA, tables);
	}
	p = skipsp(p);
	while (*p != '\0')
	{
		s = tstr(&p, 0);
		add_cmd_str(s, tables);
	}
	add_cmd_char('\0', tables);
}

/*
 * Parse a line from the lesskey file.
 */
static void parse_line(char *line, struct lesskey_tables *tables)
{
	char *p;

	/*
	 * See if it is a control line.
	 */
	p = control_line(line, tables);
	if (p == NULL)
		return;
	/*
	 * Skip leading white space.
	 * Replace the final newline with a null byte.
	 * Ignore blank lines and comments.
	 */
	p = clean_line(p);
	if (*p == '\0')
		return;

	if (tables->currtable->is_var)
		parse_varline(p, tables);
	else
		parse_cmdline(p, tables);
}

/*
 * Parse a lesskey source file and store result in tables.
 */
int parse_lesskey(char *infile, struct lesskey_tables *tables)
{
	FILE *desc;
	char line[1024];

	if (infile == NULL)
		infile = homefile(DEF_LESSKEYINFILE);
	lesskey_file = infile;

	init_tables(tables);
	errors = 0;
	linenum = 0;
	if (less_version == 0)
		less_version = lstrtoi(version, NULL, 10);

	/*
	 * Open the input file.
	 */
	if (strcmp(infile, "-") == 0)
		desc = stdin;
	else if ((desc = fopen(infile, "r")) == NULL)
	{
		/* parse_error("cannot open lesskey file %s", infile); */
		return (-1);
	}

	/*
	 * Read and parse the input file, one line at a time.
	 */
	while (fgets(line, sizeof(line), desc) != NULL)
	{
		++linenum;
		parse_line(line, tables);
	}
	fclose(desc);
	return (errors);
}
