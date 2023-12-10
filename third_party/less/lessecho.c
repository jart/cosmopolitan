/*
 * Copyright (C) 1984-2023  Mark Nudelman
 *
 * You may distribute under the terms of either the GNU General Public
 * License or the Less License, as specified in the README file.
 *
 * For more information, see the README file.
 */


/*
 * lessecho [-ox] [-cx] [-pn] [-dn] [-a] file ...
 * Simply echos its filename arguments on standard output.
 * But any argument containing spaces is enclosed in quotes.
 *
 * -ox  Specifies "x" to be the open quote character.
 * -cx  Specifies "x" to be the close quote character.
 * -pn  Specifies "n" to be the open quote character, as an integer.
 * -dn  Specifies "n" to be the close quote character, as an integer.
 * -mx  Specifies "x" to be a metachar.
 * -nn  Specifies "n" to be a metachar, as an integer.
 * -ex  Specifies "x" to be the escape char for metachars.
 * -fn  Specifies "x" to be the escape char for metachars, as an integer.
 * -a   Specifies that all arguments are to be quoted.
 *      The default is that only arguments containing spaces are quoted.
 */

#include "less.h"

static char *version = "$Revision: 1.15 $";

static int quote_all = 0;
static char openquote = '"';
static char closequote = '"';
static char *meta_escape = "\\";
static char meta_escape_buf[2];
static char* metachars = NULL;
static int num_metachars = 0;
static int size_metachars = 0;

static void pr_usage(void)
{
	fprintf(stderr,
		"usage: lessecho [-ox] [-cx] [-pn] [-dn] [-mx] [-nn] [-ex] [-fn] [-a] file ...\n");
}

static void pr_version(void)
{
	char *p;
	char buf[10];
	char *pbuf = buf;

	for (p = version;  *p != ' ';  p++)
		if (*p == '\0')
			return;
	for (p++;  *p != '$' && *p != ' ' && *p != '\0';  p++)
		*pbuf++ = *p;
	*pbuf = '\0';
	printf("%s\n", buf);
}

static void pr_error(char *s)
{
	fprintf(stderr, "%s\n", s);
	exit(1);
}

static long lstrtol(char *s, char **pend, int radix)
{
	int v;
	int neg = 0;
	long n = 0;

	/* Skip leading white space. */
	while (*s == ' ' || *s == '\t')
		s++;

	/* Check for a leading + or -. */
	if (*s == '-')
	{
		neg = 1;
		s++;
	} else if (*s == '+')
	{
		s++;
	}

	/* Determine radix if caller does not specify. */
	if (radix == 0)
	{
		radix = 10;
		if (*s == '0')
		{
			switch (*++s)
			{
			case 'x':
				radix = 16;
				s++;
				break;
			default:
				radix = 8;
				break;
			}
		}
	}

	/* Parse the digits of the number. */
	for (;;)
	{
		if (*s >= '0' && *s <= '9')
			v = *s - '0';
		else if (*s >= 'a' && *s <= 'f')
			v = *s - 'a' + 10;
		else if (*s >= 'A' && *s <= 'F')
			v = *s - 'A' + 10;
		else
			break;
		if (v >= radix)
			break;
		n = n * radix + v;
		s++;
	}

	if (pend != NULL)
	{
		/* Skip trailing white space. */
		while (*s == ' ' || *s == '\t')
			s++;
		*pend = s;
	}
	if (neg)
		return (-n);
	return (n);
}

static void add_metachar(int ch)
{
	if (num_metachars+1 >= size_metachars)
	{
		char *p;
		size_metachars = (size_metachars > 0) ? size_metachars*2 : 16;
		p = (char *) malloc(size_metachars);
		if (p == NULL)
			pr_error("Cannot allocate memory");

		if (metachars != NULL)
		{
			strcpy(p, metachars);
			free(metachars);
		}
		metachars = p;
	}
	metachars[num_metachars++] = ch;
	metachars[num_metachars] = '\0';
}

static int is_metachar(int ch)
{
	return (metachars != NULL && strchr(metachars, ch) != NULL);
}

#if !HAVE_STRCHR
char * strchr(char *s, char c)
{
	for ( ;  *s != '\0';  s++)
		if (*s == c)
			return (s);
	if (c == '\0')
		return (s);
	return (NULL);
}
#endif

int main(int argc, char *argv[])
{
	char *arg;
	char *s;
	int no_more_options;

	no_more_options = 0;
	while (--argc > 0)
	{
		arg = *++argv;
		if (*arg != '-' || no_more_options)
			break;
		switch (*++arg)
		{
		case 'a':
			quote_all = 1;
			break;
		case 'c':
			closequote = *++arg;
			break;
		case 'd':
			closequote = lstrtol(++arg, &s, 0);
			if (s == arg)
				pr_error("Missing number after -d");
			break;
		case 'e':
			if (strcmp(++arg, "-") == 0)
				meta_escape = "";
			else
				meta_escape = arg;
			break;
		case 'f':
			meta_escape_buf[0] = lstrtol(++arg, &s, 0);
			meta_escape_buf[1] = '\0';
			meta_escape = meta_escape_buf;
			if (s == arg)
				pr_error("Missing number after -f");
			break;
		case 'o':
			openquote = *++arg;
			break;
		case 'p':
			openquote = lstrtol(++arg, &s, 0);
			if (s == arg)
				pr_error("Missing number after -p");
			break;
		case 'm':
			add_metachar(*++arg);
			break;
		case 'n':
			add_metachar(lstrtol(++arg, &s, 0));
			if (s == arg)
				pr_error("Missing number after -n");
			break;
		case '?':
			pr_usage();
			return (0);
		case '-':
			if (*++arg == '\0')
			{
				no_more_options = 1;
				break;
			}
			if (strcmp(arg, "version") == 0)
			{
				pr_version();
				return (0);
			}
			if (strcmp(arg, "help") == 0)
			{
				pr_usage();
				return (0);
			}
			pr_error("Invalid option after --");
		default:
			pr_error("Invalid option letter");
		}
	}

	while (argc-- > 0)
	{
		int has_meta = 0;
		arg = *argv++;
		for (s = arg;  *s != '\0';  s++)
		{
			if (is_metachar(*s))
			{
				has_meta = 1;
				break;
			}
		}
		if (quote_all || (has_meta && strlen(meta_escape) == 0))
			printf("%c%s%c", openquote, arg, closequote);
		else 
		{
			for (s = arg;  *s != '\0';  s++)
			{
				if (is_metachar(*s))
					printf("%s", meta_escape);
				printf("%c", *s);
			}
		}
		if (argc > 0)
			printf(" ");
		else
			printf("\n");
	}
	return (0);
}
