/*
 * Copyright (C) 1984-2023  Mark Nudelman
 *
 * You may distribute under the terms of either the GNU General Public
 * License or the Less License, as specified in the README file.
 *
 * For more information, see the README file.
 */

/*
 * High level routines dealing with getting lines of input 
 * from the file being viewed.
 *
 * When we speak of "lines" here, we mean PRINTABLE lines;
 * lines processed with respect to the screen width.
 * We use the term "raw line" to refer to lines simply
 * delimited by newlines; not processed with respect to screen width.
 */

#include "less.h"

extern int squeeze;
extern int hshift;
extern int quit_if_one_screen;
extern int sigs;
extern int ignore_eoi;
extern int status_col;
extern int wordwrap;
extern POSITION start_attnpos;
extern POSITION end_attnpos;
#if HILITE_SEARCH
extern int hilite_search;
extern int size_linebuf;
extern int show_attn;
#endif

/*
 * Set the status column.
 *  base  Position of first char in line.
 *  disp  First visible char.
 *        Different than base_pos if line is shifted.
 *  edisp Last visible char. 
 *  eol   End of line. Normally the newline.
 *        Different than edisp if line is chopped.
 */
static void init_status_col(POSITION base_pos, POSITION disp_pos, POSITION edisp_pos, POSITION eol_pos)
{
	int hl_before = (chop_line() && disp_pos != NULL_POSITION) ?
	    is_hilited_attr(base_pos, disp_pos, TRUE, NULL) : 0;
	int hl_after = (chop_line()) ?
	    is_hilited_attr(edisp_pos, eol_pos, TRUE, NULL) : 0;
	int attr;
	char ch;

	if (hl_before && hl_after)
	{
		attr = hl_after;
		ch = '=';
	} else if (hl_before)
	{
		attr = hl_before;
		ch = '<';
	} else if (hl_after)
	{
		attr = hl_after;
		ch = '>';
	} else 
	{
		attr = is_hilited_attr(base_pos, eol_pos, TRUE, NULL);
		ch = '*';
	}
	if (attr)
		set_status_col(ch, attr);
}

/*
 * Get the next line.
 * A "current" position is passed and a "new" position is returned.
 * The current position is the position of the first character of
 * a line.  The new position is the position of the first character
 * of the NEXT line.  The line obtained is the line starting at curr_pos.
 */
public POSITION forw_line_seg(POSITION curr_pos, int skipeol, int rscroll, int nochop)
{
	POSITION base_pos;
	POSITION new_pos;
	POSITION edisp_pos = 0;
	int c;
	int blankline;
	int endline;
	int chopped;
	int backchars;
	POSITION wrap_pos;
	int skipped_leading;

get_forw_line:
	if (curr_pos == NULL_POSITION)
	{
		null_line();
		return (NULL_POSITION);
	}
#if HILITE_SEARCH
	if (hilite_search == OPT_ONPLUS || is_filtering() || status_col)
	{
		/*
		 * If we are ignoring EOI (command F), only prepare
		 * one line ahead, to avoid getting stuck waiting for
		 * slow data without displaying the data we already have.
		 * If we're not ignoring EOI, we *could* do the same, but
		 * for efficiency we prepare several lines ahead at once.
		 */
		prep_hilite(curr_pos, curr_pos + 3*size_linebuf, 
				ignore_eoi ? 1 : -1);
		curr_pos = next_unfiltered(curr_pos);
	}
#endif
	if (ch_seek(curr_pos))
	{
		null_line();
		return (NULL_POSITION);
	}

	/*
	 * Step back to the beginning of the line.
	 */
	base_pos = curr_pos;
	for (;;)
	{
		if (ABORT_SIGS())
		{
			null_line();
			return (NULL_POSITION);
		}
		c = ch_back_get();
		if (c == EOI)
			break;
		if (c == '\n')
		{
			(void) ch_forw_get();
			break;
		}
		--base_pos;
	}

	/*
	 * Read forward again to the position we should start at.
	 */
	prewind();
	plinestart(base_pos);
	(void) ch_seek(base_pos);
	new_pos = base_pos;
	while (new_pos < curr_pos)
	{
		if (ABORT_SIGS())
		{
			null_line();
			return (NULL_POSITION);
		}
		c = ch_forw_get();
		backchars = pappend(c, new_pos);
		new_pos++;
		if (backchars > 0)
		{
			pshift_all();
			if (wordwrap && (c == ' ' || c == '\t'))
			{
				do
				{
					new_pos++;
					c = ch_forw_get();
				} while (c == ' ' || c == '\t');
				backchars = 1;
			}
			new_pos -= backchars;
			while (--backchars >= 0)
				(void) ch_back_get();
		}
	}
	(void) pflushmbc();
	pshift_all();

	/*
	 * Read the first character to display.
	 */
	c = ch_forw_get();
	if (c == EOI)
	{
		null_line();
		return (NULL_POSITION);
	}
	blankline = (c == '\n' || c == '\r');
	wrap_pos = NULL_POSITION;
	skipped_leading = FALSE;

	/*
	 * Read each character in the line and append to the line buffer.
	 */
	chopped = FALSE;
	for (;;)
	{
		if (ABORT_SIGS())
		{
			null_line();
			return (NULL_POSITION);
		}
		if (c == '\n' || c == EOI)
		{
			/*
			 * End of the line.
			 */
			backchars = pflushmbc();
			new_pos = ch_tell();
			if (backchars > 0 && (nochop || !chop_line()) && hshift == 0)
			{
				new_pos -= backchars + 1;
				endline = FALSE;
			} else
				endline = TRUE;
			edisp_pos = new_pos;
			break;
		}
		if (c != '\r')
			blankline = 0;

		/*
		 * Append the char to the line and get the next char.
		 */
		backchars = pappend(c, ch_tell()-1);
		if (backchars > 0)
		{
			/*
			 * The char won't fit in the line; the line
			 * is too long to print in the screen width.
			 * End the line here.
			 */
			if (skipeol)
			{
				/* Read to end of line. */
				edisp_pos = ch_tell();
				do
				{
					if (ABORT_SIGS())
					{
						null_line();
						return (NULL_POSITION);
					}
					c = ch_forw_get();
				} while (c != '\n' && c != EOI);
				new_pos = ch_tell();
				endline = TRUE;
				quit_if_one_screen = FALSE;
				chopped = TRUE;
			} else
			{
				if (!wordwrap)
					new_pos = ch_tell() - backchars;
				else
				{
					/*
					 * We're word-wrapping, so go back to the last space.
					 * However, if it's the space itself that couldn't fit,
					 * simply ignore it and any subsequent spaces.
					 */
					if (c == ' ' || c == '\t')
					{
						do
						{
							new_pos = ch_tell();
							c = ch_forw_get();
						} while (c == ' ' || c == '\t');
						if (c == '\r')
							c = ch_forw_get();
						if (c == '\n')
							new_pos = ch_tell();
					} else if (wrap_pos == NULL_POSITION)
						new_pos = ch_tell() - backchars;
					else
					{
						new_pos = wrap_pos;
						loadc();
					}
				}
				endline = FALSE;
			}
			break;
		}
		if (wordwrap)
		{
			if (c == ' ' || c == '\t')
			{
				if (skipped_leading)
				{
					wrap_pos = ch_tell();
					savec();
				}
			} else
				skipped_leading = TRUE;
		}
		c = ch_forw_get();
	}

#if HILITE_SEARCH
	if (blankline && show_attn)
	{
		/* Add spurious space to carry possible attn hilite. */
		pappend(' ', ch_tell()-1);
	}
#endif
	pdone(endline, rscroll && chopped, 1);

#if HILITE_SEARCH
	if (is_filtered(base_pos))
	{
		/*
		 * We don't want to display this line.
		 * Get the next line.
		 */
		curr_pos = new_pos;
		goto get_forw_line;
	}
	if (status_col)
		init_status_col(base_pos, line_position(), edisp_pos, new_pos);
#endif

	if (squeeze && blankline)
	{
		/*
		 * This line is blank.
		 * Skip down to the last contiguous blank line
		 * and pretend it is the one which we are returning.
		 */
		while ((c = ch_forw_get()) == '\n' || c == '\r')
			if (ABORT_SIGS())
			{
				null_line();
				return (NULL_POSITION);
			}
		if (c != EOI)
			(void) ch_back_get();
		new_pos = ch_tell();
	}

	return (new_pos);
}

public POSITION forw_line(POSITION curr_pos)
{

	return forw_line_seg(curr_pos, (chop_line() || hshift > 0), TRUE, FALSE);
}

/*
 * Get the previous line.
 * A "current" position is passed and a "new" position is returned.
 * The current position is the position of the first character of
 * a line.  The new position is the position of the first character
 * of the PREVIOUS line.  The line obtained is the one starting at new_pos.
 */
public POSITION back_line(POSITION curr_pos)
{
	POSITION base_pos;
	POSITION new_pos;
	POSITION edisp_pos = 0;
	POSITION begin_new_pos;
	int c;
	int endline;
	int chopped;
	int backchars;
	POSITION wrap_pos;
	int skipped_leading;

get_back_line:
	if (curr_pos == NULL_POSITION || curr_pos <= ch_zero())
	{
		null_line();
		return (NULL_POSITION);
	}
#if HILITE_SEARCH
	if (hilite_search == OPT_ONPLUS || is_filtering() || status_col)
		prep_hilite((curr_pos < 3*size_linebuf) ? 
				0 : curr_pos - 3*size_linebuf, curr_pos, -1);
#endif
	if (ch_seek(curr_pos-1))
	{
		null_line();
		return (NULL_POSITION);
	}

	if (squeeze)
	{
		/*
		 * Find out if the "current" line was blank.
		 */
		(void) ch_forw_get();    /* Skip the newline */
		c = ch_forw_get();       /* First char of "current" line */
		(void) ch_back_get();    /* Restore our position */
		(void) ch_back_get();

		if (c == '\n' || c == '\r')
		{
			/*
			 * The "current" line was blank.
			 * Skip over any preceding blank lines,
			 * since we skipped them in forw_line().
			 */
			while ((c = ch_back_get()) == '\n' || c == '\r')
				if (ABORT_SIGS())
				{
					null_line();
					return (NULL_POSITION);
				}
			if (c == EOI)
			{
				null_line();
				return (NULL_POSITION);
			}
			(void) ch_forw_get();
		}
	}

	/*
	 * Scan backwards until we hit the beginning of the line.
	 */
	for (;;)
	{
		if (ABORT_SIGS())
		{
			null_line();
			return (NULL_POSITION);
		}
		c = ch_back_get();
		if (c == '\n')
		{
			/*
			 * This is the newline ending the previous line.
			 * We have hit the beginning of the line.
			 */
			base_pos = ch_tell() + 1;
			break;
		}
		if (c == EOI)
		{
			/*
			 * We have hit the beginning of the file.
			 * This must be the first line in the file.
			 * This must, of course, be the beginning of the line.
			 */
			base_pos = ch_tell();
			break;
		}
	}

	/*
	 * Now scan forwards from the beginning of this line.
	 * We keep discarding "printable lines" (based on screen width)
	 * until we reach the curr_pos.
	 *
	 * {{ This algorithm is pretty inefficient if the lines
	 *    are much longer than the screen width, 
	 *    but I don't know of any better way. }}
	 */
	new_pos = base_pos;
	if (ch_seek(new_pos))
	{
		null_line();
		return (NULL_POSITION);
	}
	endline = FALSE;
	prewind();
	plinestart(new_pos);
    loop:
	wrap_pos = NULL_POSITION;
	skipped_leading = FALSE;
	begin_new_pos = new_pos;
	(void) ch_seek(new_pos);
	chopped = FALSE;

	for (;;)
	{
		c = ch_forw_get();
		if (c == EOI || ABORT_SIGS())
		{
			null_line();
			return (NULL_POSITION);
		}
		new_pos++;
		if (c == '\n')
		{
			backchars = pflushmbc();
			if (backchars > 0 && !chop_line() && hshift == 0)
			{
				backchars++;
				goto shift;
			}
			endline = TRUE;
			edisp_pos = new_pos;
			break;
		}
		backchars = pappend(c, ch_tell()-1);
		if (backchars > 0)
		{
			/*
			 * Got a full printable line, but we haven't
			 * reached our curr_pos yet.  Discard the line
			 * and start a new one.
			 */
			if (chop_line() || hshift > 0)
			{
				endline = TRUE;
				chopped = TRUE;
				quit_if_one_screen = FALSE;
				edisp_pos = new_pos;
				break;
			}
		shift:
			if (!wordwrap)
			{
				pshift_all();
				new_pos -= backchars;
			} else
			{
				if (c == ' ' || c == '\t')
				{
					for (;;)
					{
						c = ch_forw_get();
						if (c == ' ' || c == '\t')
							new_pos++;
						else
						{
							if (c == '\r')
							{
								c = ch_forw_get();
								if (c == '\n')
									new_pos++;
							}
							if (c == '\n')
								new_pos++;
							break;
						}
					}
					if (new_pos >= curr_pos)
						break;
					pshift_all();
				} else
				{
					pshift_all();
					if (wrap_pos == NULL_POSITION)
						new_pos -= backchars;
					else
						new_pos = wrap_pos;
				}
			}
			goto loop;
		}
		if (wordwrap)
		{
			if (c == ' ' || c == '\t')
			{
				if (skipped_leading)
					wrap_pos = new_pos;
			} else
				skipped_leading = TRUE;
		}
		if (new_pos >= curr_pos)
		{
			edisp_pos = new_pos;
			break;
		}
	}

	pdone(endline, chopped, 0);

#if HILITE_SEARCH
	if (is_filtered(base_pos))
	{
		/*
		 * We don't want to display this line.
		 * Get the previous line.
		 */
		curr_pos = begin_new_pos;
		goto get_back_line;
	}
	if (status_col)
		init_status_col(base_pos, line_position(), edisp_pos, new_pos);
#endif

	return (begin_new_pos);
}

/*
 * Set attnpos.
 */
public void set_attnpos(POSITION pos)
{
	int c;

	if (pos != NULL_POSITION)
	{
		if (ch_seek(pos))
			return;
		for (;;)
		{
			c = ch_forw_get();
			if (c == EOI)
				break;
			if (c == '\n' || c == '\r')
			{
				(void) ch_back_get();
				break;
			}
			pos++;
		}
		end_attnpos = pos;
		for (;;)
		{
			c = ch_back_get();
			if (c == EOI || c == '\n' || c == '\r')
				break;
			pos--;
		}
	}
	start_attnpos = pos;
}
