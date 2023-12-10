/*
 * Copyright (C) 1984-2023  Mark Nudelman
 *
 * You may distribute under the terms of either the GNU General Public
 * License or the Less License, as specified in the README file.
 *
 * For more information, see the README file.
 */


#include "less.h"
#include "position.h"

extern IFILE curr_ifile;
extern int sc_height;
extern int jump_sline;
extern int perma_marks;

/*
 * A mark is an ifile (input file) plus a position within the file.
 */
struct mark 
{
	/*
	 * Normally m_ifile != IFILE_NULL and m_filename == NULL.
	 * For restored marks we set m_filename instead of m_ifile
	 * because we don't want to create an ifile until the 
	 * user explicitly requests the file (by name or mark).
	 */
	char m_letter;           /* Associated character */
	IFILE m_ifile;           /* Input file being marked */
	char *m_filename;        /* Name of the input file */
	struct scrpos m_scrpos;  /* Position of the mark */
};

/*
 * The table of marks.
 * Each mark is identified by a lowercase or uppercase letter.
 * The final one is lmark, for the "last mark"; addressed by the apostrophe.
 */
#define NMARKS          ((2*26)+2)      /* a-z, A-Z, mousemark, lastmark */
#define NUMARKS         ((2*26)+1)      /* user marks (not lastmark) */
#define MOUSEMARK       (NMARKS-2)
#define LASTMARK        (NMARKS-1)
static struct mark marks[NMARKS];
public int marks_modified = 0;


/*
 * Initialize a mark struct.
 */
static void cmark(struct mark *m, IFILE ifile, POSITION pos, int ln)
{
	m->m_ifile = ifile;
	m->m_scrpos.pos = pos;
	m->m_scrpos.ln = ln;
	if (m->m_filename != NULL)
		/* Normally should not happen but a corrupt lesshst file can do it. */
		free(m->m_filename);
	m->m_filename = NULL;
}

/*
 * Initialize the mark table to show no marks are set.
 */
public void init_mark(void)
{
	int i;

	for (i = 0;  i < NMARKS;  i++)
	{
		char letter;
		switch (i) {
		case MOUSEMARK: letter = '#'; break;
		case LASTMARK: letter = '\''; break;
		default: letter = (i < 26) ? 'a'+i : 'A'+i-26; break;
		}
		marks[i].m_letter = letter;
		cmark(&marks[i], NULL_IFILE, NULL_POSITION, -1);
	}
}

/*
 * Set m_ifile and clear m_filename.
 */
static void mark_set_ifile(struct mark *m, IFILE ifile)
{
	m->m_ifile = ifile;
	/* With m_ifile set, m_filename is no longer needed. */
	free(m->m_filename);
	m->m_filename = NULL;
}

/*
 * Populate the m_ifile member of a mark struct from m_filename.
 */
static void mark_get_ifile(struct mark *m)
{
	if (m->m_ifile != NULL_IFILE)
		return; /* m_ifile is already set */
	mark_set_ifile(m, get_ifile(m->m_filename, prev_ifile(NULL_IFILE)));
}

/*
 * Return the user mark struct identified by a character.
 */
static struct mark * getumark(LWCHAR c)
{
	PARG parg;
	if (c >= 'a' && c <= 'z')
		return (&marks[c-'a']);
	if (c >= 'A' && c <= 'Z')
		return (&marks[c-'A'+26]);
	if (c == '\'')
		return (&marks[LASTMARK]);
	if (c == '#')
		return (&marks[MOUSEMARK]);
	parg.p_char = (char) c;
	error("Invalid mark letter %c", &parg);
	return (NULL);
}

/*
 * Get the mark structure identified by a character.
 * The mark struct may either be in the mark table (user mark)
 * or may be constructed on the fly for certain characters like ^, $.
 */
static struct mark * getmark(LWCHAR c)
{
	struct mark *m;
	static struct mark sm;

	switch (c)
	{
	case '^':
		/*
		 * Beginning of the current file.
		 */
		m = &sm;
		cmark(m, curr_ifile, ch_zero(), 0);
		break;
	case '$':
		/*
		 * End of the current file.
		 */
		if (ch_end_seek())
		{
			error("Cannot seek to end of file", NULL_PARG);
			return (NULL);
		}
		m = &sm;
		cmark(m, curr_ifile, ch_tell(), sc_height);
		break;
	case '.':
		/*
		 * Current position in the current file.
		 */
		m = &sm;
		get_scrpos(&m->m_scrpos, TOP);
		cmark(m, curr_ifile, m->m_scrpos.pos, m->m_scrpos.ln);
		break;
	case '\'':
		/*
		 * The "last mark".
		 */
		m = &marks[LASTMARK];
		break;
	default:
		/*
		 * Must be a user-defined mark.
		 */
		m = getumark(c);
		if (m == NULL)
			break;
		if (m->m_scrpos.pos == NULL_POSITION)
		{
			error("Mark not set", NULL_PARG);
			return (NULL);
		}
		break;
	}
	return (m);
}

/*
 * Is a mark letter invalid?
 */
public int badmark(LWCHAR c)
{
	return (getmark(c) == NULL);
}

/*
 * Set a user-defined mark.
 */
public void setmark(LWCHAR c, int where)
{
	struct mark *m;
	struct scrpos scrpos;

	m = getumark(c);
	if (m == NULL)
		return;
	get_scrpos(&scrpos, where);
	if (scrpos.pos == NULL_POSITION)
	{
		bell();
		return;
	}
	cmark(m, curr_ifile, scrpos.pos, scrpos.ln);
	marks_modified = 1;
}

/*
 * Clear a user-defined mark.
 */
public void clrmark(LWCHAR c)
{
	struct mark *m;

	m = getumark(c);
	if (m == NULL)
		return;
	if (m->m_scrpos.pos == NULL_POSITION)
	{
		bell();
		return;
	}
	m->m_scrpos.pos = NULL_POSITION;
	marks_modified = 1;
}

/*
 * Set lmark (the mark named by the apostrophe).
 */
public void lastmark(void)
{
	struct scrpos scrpos;

	if (ch_getflags() & CH_HELPFILE)
		return;
	get_scrpos(&scrpos, TOP);
	if (scrpos.pos == NULL_POSITION)
		return;
	cmark(&marks[LASTMARK], curr_ifile, scrpos.pos, scrpos.ln);
	marks_modified = 1;
}

/*
 * Go to a mark.
 */
public void gomark(LWCHAR c)
{
	struct mark *m;
	struct scrpos scrpos;

	m = getmark(c);
	if (m == NULL)
		return;

	/*
	 * If we're trying to go to the lastmark and 
	 * it has not been set to anything yet,
	 * set it to the beginning of the current file.
	 * {{ Couldn't we instead set marks[LASTMARK] in edit()? }}
	 */
	if (m == &marks[LASTMARK] && m->m_scrpos.pos == NULL_POSITION)
		cmark(m, curr_ifile, ch_zero(), jump_sline);

	mark_get_ifile(m);

	/* Save scrpos; if it's LASTMARK it could change in edit_ifile. */
	scrpos = m->m_scrpos;
	if (m->m_ifile != curr_ifile)
	{
		/*
		 * Not in the current file; edit the correct file.
		 */
		if (edit_ifile(m->m_ifile))
			return;
	}

	jump_loc(scrpos.pos, scrpos.ln);
}

/*
 * Return the position associated with a given mark letter.
 *
 * We don't return which screen line the position 
 * is associated with, but this doesn't matter much,
 * because it's always the first non-blank line on the screen.
 */
public POSITION markpos(LWCHAR c)
{
	struct mark *m;

	m = getmark(c);
	if (m == NULL)
		return (NULL_POSITION);

	if (m->m_ifile != curr_ifile)
	{
		error("Mark not in current file", NULL_PARG);
		return (NULL_POSITION);
	}
	return (m->m_scrpos.pos);
}

/*
 * Return the mark associated with a given position, if any.
 */
public char posmark(POSITION pos)
{
	int i;

	/* Only user marks */
	for (i = 0;  i < NUMARKS;  i++)
	{
		if (marks[i].m_ifile == curr_ifile && marks[i].m_scrpos.pos == pos)
		{
			if (i < 26) return 'a' + i;
			if (i < 26*2) return 'A' + (i - 26);
			return '#';
		}
	}
	return 0;
}

/*
 * Clear the marks associated with a specified ifile.
 */
public void unmark(IFILE ifile)
{
	int i;

	for (i = 0;  i < NMARKS;  i++)
		if (marks[i].m_ifile == ifile)
			marks[i].m_scrpos.pos = NULL_POSITION;
}

/*
 * Check if any marks refer to a specified ifile vi m_filename
 * rather than m_ifile.
 */
public void mark_check_ifile(IFILE ifile)
{
	int i;
	char *filename = get_real_filename(ifile);

	for (i = 0;  i < NMARKS;  i++)
	{
		struct mark *m = &marks[i];
		char *mark_filename = m->m_filename;
		if (mark_filename != NULL)
		{
			mark_filename = lrealpath(mark_filename);
			if (strcmp(filename, mark_filename) == 0)
				mark_set_ifile(m, ifile);
			free(mark_filename);
		}
	}
}

#if CMD_HISTORY

/*
 * Save marks to history file.
 */
public void save_marks(FILE *fout, char *hdr)
{
	int i;

	if (!perma_marks)
		return;

	fprintf(fout, "%s\n", hdr);
	for (i = 0;  i < NMARKS;  i++)
	{
		char *filename;
		struct mark *m = &marks[i];
		char pos_str[INT_STRLEN_BOUND(m->m_scrpos.pos) + 2];
		if (m->m_scrpos.pos == NULL_POSITION)
			continue;
		postoa(m->m_scrpos.pos, pos_str, 10);
		filename = m->m_filename;
		if (filename == NULL)
			filename = get_real_filename(m->m_ifile);
		if (strcmp(filename, "-") != 0)
			fprintf(fout, "m %c %d %s %s\n",
				m->m_letter, m->m_scrpos.ln, pos_str, filename);
	}
}

/*
 * Restore one mark from the history file.
 */
public void restore_mark(char *line)
{
	struct mark *m;
	int ln;
	POSITION pos;

#define skip_whitespace while (*line == ' ') line++
	if (*line++ != 'm')
		return;
	skip_whitespace;
	m = getumark(*line++);
	if (m == NULL)
		return;
	skip_whitespace;
	ln = lstrtoi(line, &line, 10);
	if (ln < 0)
		return;
	if (ln < 1)
		ln = 1;
	if (ln > sc_height)
		ln = sc_height;
	skip_whitespace;
	pos = lstrtopos(line, &line, 10);
	if (pos < 0)
		return;
	skip_whitespace;
	cmark(m, NULL_IFILE, pos, ln);
	m->m_filename = save(line);
}

#endif /* CMD_HISTORY */
