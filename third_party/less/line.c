/*
 * Copyright (C) 1984-2023  Mark Nudelman
 *
 * You may distribute under the terms of either the GNU General Public
 * License or the Less License, as specified in the README file.
 *
 * For more information, see the README file.
 */

/*
 * Routines to manipulate the "line buffer".
 * The line buffer holds a line of output as it is being built
 * in preparation for output to the screen.
 */

#include "less.h"
#include "charset.h"
#include "position.h"

#if MSDOS_COMPILER==WIN32C
#define WIN32_LEAN_AND_MEAN
/* #include <windows.h> */
#endif

#define MAX_PFX_WIDTH (MAX_LINENUM_WIDTH + MAX_STATUSCOL_WIDTH + 1)
static struct {
	char *buf;    /* Buffer which holds the current output line */
	int *attr;   /* Parallel to buf, to hold attributes */
	int print;    /* Index in buf of first printable char */
	int end;      /* Number of chars in buf */
	char pfx[MAX_PFX_WIDTH]; /* Holds status column and line number */
	int pfx_attr[MAX_PFX_WIDTH];
	int pfx_end;  /* Number of chars in pfx */
} linebuf;

/*
 * Buffer of ansi sequences which have been shifted off the left edge 
 * of the screen. 
 */
static struct xbuffer shifted_ansi;

/*
 * Ring buffer of last ansi sequences sent.
 * While sending a line, these will be resent at the end
 * of any highlighted string, to restore text modes.
 * {{ Not ideal, since we don't really know how many to resend. }}
 */
#define NUM_LAST_ANSIS 3
static struct xbuffer last_ansi;
static struct xbuffer last_ansis[NUM_LAST_ANSIS];
static int curr_last_ansi;

public int size_linebuf = 0; /* Size of line buffer (and attr buffer) */
static struct ansi_state *line_ansi = NULL;
static int ansi_in_line;
static int hlink_in_line;
static int line_mark_attr;
static int cshift;   /* Current left-shift of output line buffer */
public int hshift;   /* Desired left-shift of output line buffer */
public int tabstops[TABSTOP_MAX] = { 0 }; /* Custom tabstops */
public int ntabstops = 1;        /* Number of tabstops */
public int tabdefault = 8;       /* Default repeated tabstops */
public POSITION highest_hilite;  /* Pos of last hilite in file found so far */
static POSITION line_pos;

static int end_column;  /* Printable length, accounting for backspaces, etc. */
static int right_curr;
static int right_column;
static int overstrike;  /* Next char should overstrike previous char */
static int last_overstrike = AT_NORMAL;
static int is_null_line;  /* There is no current line */
static LWCHAR pendc;
static POSITION pendpos;
static char *end_ansi_chars;
static char *mid_ansi_chars;
static int in_hilite;

static int attr_swidth(int a);
static int attr_ewidth(int a);
static int do_append(LWCHAR ch, char *rep, POSITION pos);

extern int sigs;
extern int bs_mode;
extern int proc_backspace;
extern int proc_tab;
extern int proc_return;
extern int linenums;
extern int ctldisp;
extern int twiddle;
extern int binattr;
extern int status_col;
extern int status_col_width;
extern int linenum_width;
extern int auto_wrap, ignaw;
extern int bo_s_width, bo_e_width;
extern int ul_s_width, ul_e_width;
extern int bl_s_width, bl_e_width;
extern int so_s_width, so_e_width;
extern int sc_width, sc_height;
extern int utf_mode;
extern POSITION start_attnpos;
extern POSITION end_attnpos;
extern char rscroll_char;
extern int rscroll_attr;
extern int use_color;
extern int status_line;

static char mbc_buf[MAX_UTF_CHAR_LEN];
static int mbc_buf_len = 0;
static int mbc_buf_index = 0;
static POSITION mbc_pos;
static int saved_line_end;
static int saved_end_column;

/* Configurable color map */
struct color_map { int attr; char color[12]; };
static struct color_map color_map[] = {
	{ AT_UNDERLINE,            "" },
	{ AT_BOLD,                 "" },
	{ AT_BLINK,                "" },
	{ AT_STANDOUT,             "" },
	{ AT_COLOR_ATTN,           "Wm" },
	{ AT_COLOR_BIN,            "kR" },
	{ AT_COLOR_CTRL,           "kR" },
	{ AT_COLOR_ERROR,          "kY" },
	{ AT_COLOR_LINENUM,        "c" },
	{ AT_COLOR_MARK,           "Wb" },
	{ AT_COLOR_PROMPT,         "kC" },
	{ AT_COLOR_RSCROLL,        "kc" },
	{ AT_COLOR_HEADER,         "" },
	{ AT_COLOR_SEARCH,         "kG" },
	{ AT_COLOR_SUBSEARCH(1),   "ky" },
	{ AT_COLOR_SUBSEARCH(2),   "wb" },
	{ AT_COLOR_SUBSEARCH(3),   "YM" },
	{ AT_COLOR_SUBSEARCH(4),   "Yr" },
	{ AT_COLOR_SUBSEARCH(5),   "Wc" },
};

/* State while processing an ANSI escape sequence */
struct ansi_state {
	int hindex;   /* Index into hyperlink prefix */
	int hlink;    /* Processing hyperlink address? */
	int prev_esc; /* Prev char was ESC (to detect ESC-\ seq) */
};

/*
 * Initialize from environment variables.
 */
public void init_line(void)
{
	int ax;

	end_ansi_chars = lgetenv("LESSANSIENDCHARS");
	if (isnullenv(end_ansi_chars))
		end_ansi_chars = "m";

	mid_ansi_chars = lgetenv("LESSANSIMIDCHARS");
	if (isnullenv(mid_ansi_chars))
		mid_ansi_chars = "0123456789:;[?!\"'#%()*+ ";

	linebuf.buf = (char *) ecalloc(LINEBUF_SIZE, sizeof(char));
	linebuf.attr = (int *) ecalloc(LINEBUF_SIZE, sizeof(int));
	size_linebuf = LINEBUF_SIZE;
	xbuf_init(&shifted_ansi);
	xbuf_init(&last_ansi);
	for (ax = 0;  ax < NUM_LAST_ANSIS;  ax++)
		xbuf_init(&last_ansis[ax]);
	curr_last_ansi = 0;
}

/*
 * Expand the line buffer.
 */
static int expand_linebuf(void)
{
	/* Double the size of the line buffer. */
	int new_size = size_linebuf * 2;
	char *new_buf = (char *) calloc(new_size, sizeof(char));
	int *new_attr = (int *) calloc(new_size, sizeof(int));
	if (new_buf == NULL || new_attr == NULL)
	{
		if (new_attr != NULL)
			free(new_attr);
		if (new_buf != NULL)
			free(new_buf);
		return 1;
	}
	/*
	 * We just calloc'd the buffers; copy the old contents.
	 */
	memcpy(new_buf, linebuf.buf, size_linebuf * sizeof(char));
	memcpy(new_attr, linebuf.attr, size_linebuf * sizeof(int));
	free(linebuf.attr);
	free(linebuf.buf);
	linebuf.buf = new_buf;
	linebuf.attr = new_attr;
	size_linebuf = new_size;
	return 0;
}

/*
 * Is a character ASCII?
 */
public int is_ascii_char(LWCHAR ch)
{
	return (ch <= 0x7F);
}

/*
 */
static void inc_end_column(int w)
{
	if (end_column > right_column && w > 0)
	{
		right_column = end_column;
		right_curr = linebuf.end;
	}
	end_column += w;
}

public POSITION line_position(void)
{
	return line_pos;
}

/*
 * Rewind the line buffer.
 */
public void prewind(void)
{
	int ax;

	linebuf.print = 6; /* big enough for longest UTF-8 sequence */
	linebuf.pfx_end = 0;
	for (linebuf.end = 0; linebuf.end < linebuf.print; linebuf.end++)
	{
		linebuf.buf[linebuf.end] = '\0';
		linebuf.attr[linebuf.end] = 0;
	}

	end_column = 0;
	right_curr = 0;
	right_column = 0;
	cshift = 0;
	overstrike = 0;
	last_overstrike = AT_NORMAL;
	mbc_buf_len = 0;
	is_null_line = 0;
	pendc = '\0';
	in_hilite = 0;
	ansi_in_line = 0;
	hlink_in_line = 0;
	line_mark_attr = 0;
	line_pos = NULL_POSITION;
	xbuf_reset(&shifted_ansi);
	xbuf_reset(&last_ansi);
	for (ax = 0;  ax < NUM_LAST_ANSIS;  ax++)
		xbuf_reset(&last_ansis[ax]);
	curr_last_ansi = 0;
}

/*
 * Set a character in the line buffer.
 */
static void set_linebuf(int n, char ch, int attr)
{
	if (n >= size_linebuf)
	{
		/*
		 * Won't fit in line buffer.
		 * Try to expand it.
		 */
		if (expand_linebuf())
			return;
	}
	linebuf.buf[n] = ch;
	linebuf.attr[n] = attr;
}

/*
 * Append a character to the line buffer.
 */
static void add_linebuf(char ch, int attr, int w)
{
	set_linebuf(linebuf.end++, ch, attr);
	inc_end_column(w);
}

/*
 * Append a string to the line buffer.
 */
static void addstr_linebuf(char *s, int attr, int cw)
{
	for ( ;  *s != '\0';  s++)
		add_linebuf(*s, attr, cw);
}

/*
 * Set a character in the line prefix buffer.
 */
static void set_pfx(int n, char ch, int attr)
{
	linebuf.pfx[n] = ch;
	linebuf.pfx_attr[n] = attr;
}

/*
 * Append a character to the line prefix buffer.
 */
static void add_pfx(char ch, int attr)
{
	set_pfx(linebuf.pfx_end++, ch, attr);
}

/*
 * Insert the status column and line number into the line buffer.
 */
public void plinestart(POSITION pos)
{
	LINENUM linenum = 0;
	int i;

	if (linenums == OPT_ONPLUS)
	{
		/*
		 * Get the line number and put it in the current line.
		 * {{ Note: since find_linenum calls forw_raw_line,
		 *    it may seek in the input file, requiring the caller 
		 *    of plinestart to re-seek if necessary. }}
		 * {{ Since forw_raw_line modifies linebuf, we must
		 *    do this first, before storing anything in linebuf. }}
		 */
		linenum = find_linenum(pos);
	}

	/*
	 * Display a status column if the -J option is set.
	 */
	if (status_col || status_line)
	{
		char c = posmark(pos);
		if (c != 0)
			line_mark_attr = AT_HILITE|AT_COLOR_MARK;
		else if (start_attnpos != NULL_POSITION &&
		         pos >= start_attnpos && pos <= end_attnpos)
			line_mark_attr = AT_HILITE|AT_COLOR_ATTN;
		if (status_col)
		{
			add_pfx(c ? c : ' ', line_mark_attr); /* column 0: status */
			while (linebuf.pfx_end < status_col_width)
				add_pfx(' ', AT_NORMAL);
		}
	}

	/*
	 * Display the line number at the start of each line
	 * if the -N option is set.
	 */
	if (linenums == OPT_ONPLUS)
	{
		char buf[INT_STRLEN_BOUND(linenum) + 2];
		int len;

		linenum = vlinenum(linenum);
		if (linenum == 0)
			len = 0;
		else
		{
			linenumtoa(linenum, buf, 10);
			len = (int) strlen(buf);
		}
		for (i = 0; i < linenum_width - len; i++)
			add_pfx(' ', AT_NORMAL);
		for (i = 0; i < len; i++)
			add_pfx(buf[i], AT_BOLD|AT_COLOR_LINENUM);
		add_pfx(' ', AT_NORMAL);
	}
	end_column = linebuf.pfx_end;
}

/*
 * Return the width of the line prefix (status column and line number).
 * {{ Actual line number can be wider than linenum_width. }}
 */
public int line_pfx_width(void)
{
	int width = 0;
	if (status_col)
		width += status_col_width;
	if (linenums == OPT_ONPLUS)
		width += linenum_width + 1;
	return width;
}

/*
 * Shift line left so that the last char is just to the left
 * of the first visible column.
 */
public void pshift_all(void)
{
	int i;
	for (i = linebuf.print;  i < linebuf.end;  i++)
		if (linebuf.attr[i] == AT_ANSI)
			xbuf_add_byte(&shifted_ansi, (unsigned char) linebuf.buf[i]);
	linebuf.end = linebuf.print;
	end_column = linebuf.pfx_end;
}

/*
 * Return the printing width of the start (enter) sequence
 * for a given character attribute.
 */
static int attr_swidth(int a)
{
	int w = 0;

	a = apply_at_specials(a);

	if (a & AT_UNDERLINE)
		w += ul_s_width;
	if (a & AT_BOLD)
		w += bo_s_width;
	if (a & AT_BLINK)
		w += bl_s_width;
	if (a & AT_STANDOUT)
		w += so_s_width;

	return w;
}

/*
 * Return the printing width of the end (exit) sequence
 * for a given character attribute.
 */
static int attr_ewidth(int a)
{
	int w = 0;

	a = apply_at_specials(a);

	if (a & AT_UNDERLINE)
		w += ul_e_width;
	if (a & AT_BOLD)
		w += bo_e_width;
	if (a & AT_BLINK)
		w += bl_e_width;
	if (a & AT_STANDOUT)
		w += so_e_width;

	return w;
}

/*
 * Return the printing width of a given character and attribute,
 * if the character were added after prev_ch.
 * Adding a character with a given attribute may cause an enter or exit
 * attribute sequence to be inserted, so this must be taken into account.
 */
public int pwidth(LWCHAR ch, int a, LWCHAR prev_ch, int prev_a)
{
	int w;

	if (ch == '\b')
	{
		/*
		 * Backspace moves backwards one or two positions.
		 */
		if (prev_a & (AT_ANSI|AT_BINARY))
			return strlen(prchar('\b'));
		return (utf_mode && is_wide_char(prev_ch)) ? -2 : -1;
	}

	if (!utf_mode || is_ascii_char(ch))
	{
		if (control_char((char)ch))
		{
			/*
			 * Control characters do unpredictable things,
			 * so we don't even try to guess; say it doesn't move.
			 * This can only happen if the -r flag is in effect.
			 */
			return (0);
		}
	} else
	{
		if (is_composing_char(ch) || is_combining_char(prev_ch, ch))
		{
			/*
			 * Composing and combining chars take up no space.
			 *
			 * Some terminals, upon failure to compose a
			 * composing character with the character(s) that
			 * precede(s) it will actually take up one end_column
			 * for the composing character; there isn't much
			 * we could do short of testing the (complex)
			 * composition process ourselves and printing
			 * a binary representation when it fails.
			 */
			return (0);
		}
	}

	/*
	 * Other characters take one or two columns,
	 * plus the width of any attribute enter/exit sequence.
	 */
	w = 1;
	if (is_wide_char(ch))
		w++;
	if (linebuf.end > 0 && !is_at_equiv(linebuf.attr[linebuf.end-1], a))
		w += attr_ewidth(linebuf.attr[linebuf.end-1]);
	if (apply_at_specials(a) != AT_NORMAL &&
	    (linebuf.end == 0 || !is_at_equiv(linebuf.attr[linebuf.end-1], a)))
		w += attr_swidth(a);
	return (w);
}

/*
 * Delete to the previous base character in the line buffer.
 */
static int backc(void)
{
	LWCHAR ch;
	char *p;

	if (linebuf.end == 0)
		return (0);
	p = &linebuf.buf[linebuf.end];
	ch = step_char(&p, -1, linebuf.buf);
	/* Skip back to the next nonzero-width char. */
	while (p > linebuf.buf)
	{
		LWCHAR prev_ch;
		int width;
		linebuf.end = (int) (p - linebuf.buf);
		prev_ch = step_char(&p, -1, linebuf.buf);
		width = pwidth(ch, linebuf.attr[linebuf.end], prev_ch, linebuf.attr[linebuf.end-1]);
		end_column -= width;
		/* {{ right_column? }} */
		if (width > 0)
			break;
		ch = prev_ch;
	}
	return (1);
}

/*
 * Preserve the current position in the line buffer (for word wrapping).
 */
public void savec(void)
{
	saved_line_end = linebuf.end;
	saved_end_column = end_column;
}

/*
 * Restore the position in the line buffer (start of line for word wrapping).
 */
public void loadc(void)
{
	linebuf.end = saved_line_end;
	end_column = saved_end_column;
}

/*
 * Is a character the end of an ANSI escape sequence?
 */
public int is_ansi_end(LWCHAR ch)
{
	if (!is_ascii_char(ch))
		return (0);
	return (strchr(end_ansi_chars, (char) ch) != NULL);
}

/*
 * Can a char appear in an ANSI escape sequence, before the end char?
 */
public int is_ansi_middle(LWCHAR ch)
{
	if (!is_ascii_char(ch))
		return (0);
	if (is_ansi_end(ch))
		return (0);
	return (strchr(mid_ansi_chars, (char) ch) != NULL);
}

/*
 * Skip past an ANSI escape sequence.
 * pp is initially positioned just after the CSI_START char.
 */
public void skip_ansi(struct ansi_state *pansi, char **pp, constant char *limit)
{
	LWCHAR c;
	do {
		c = step_char(pp, +1, limit);
	} while (*pp < limit && ansi_step(pansi, c) == ANSI_MID);
	/* Note that we discard final char, for which is_ansi_end is true. */
}

/*
 * Determine if a character starts an ANSI escape sequence.
 * If so, return an ansi_state struct; otherwise return NULL.
 */
public struct ansi_state * ansi_start(LWCHAR ch)
{
	struct ansi_state *pansi;

	if (!IS_CSI_START(ch))
		return NULL;
	pansi = ecalloc(1, sizeof(struct ansi_state));
	pansi->hindex = 0;
	pansi->hlink = 0;
	pansi->prev_esc = 0;
	return pansi;
}

/*
 * Determine whether the next char in an ANSI escape sequence
 * ends the sequence.
 */
public int ansi_step(struct ansi_state *pansi, LWCHAR ch)
{
	if (pansi->hlink)
	{
		/* Hyperlink ends with \7 or ESC-backslash. */
		if (ch == '\7')
			return ANSI_END;
		if (pansi->prev_esc)
			return (ch == '\\') ? ANSI_END : ANSI_ERR;
		pansi->prev_esc = (ch == ESC);
		return ANSI_MID;
	}
	if (pansi->hindex >= 0)
	{
		static char hlink_prefix[] = ESCS "]8;";
		if (ch == hlink_prefix[pansi->hindex] ||
		    (pansi->hindex == 0 && IS_CSI_START(ch)))
		{
			pansi->hindex++;
			if (hlink_prefix[pansi->hindex] == '\0')
				pansi->hlink = 1; /* now processing hyperlink addr */
			return ANSI_MID;
		}
		pansi->hindex = -1; /* not a hyperlink */
	}
	/* Check for SGR sequences */
	if (is_ansi_middle(ch))
		return ANSI_MID;
	if (is_ansi_end(ch))
		return ANSI_END;
	return ANSI_ERR;
}

/*
 * Free an ansi_state structure.
 */
public void ansi_done(struct ansi_state *pansi)
{
	free(pansi);
}

/*
 * Will w characters in attribute a fit on the screen?
 */
static int fits_on_screen(int w, int a)
{
	if (ctldisp == OPT_ON)
		/* We're not counting, so say that everything fits. */
		return 1;
	return (end_column - cshift + w + attr_ewidth(a) <= sc_width);
}

/*
 * Append a character and attribute to the line buffer.
 */
#define STORE_CHAR(ch,a,rep,pos) \
	do { \
		if (store_char((ch),(a),(rep),(pos))) return (1); \
	} while (0)

static int store_char(LWCHAR ch, int a, char *rep, POSITION pos)
{
	int w;
	int i;
	int replen;
	char cs;

	i = (a & (AT_UNDERLINE|AT_BOLD));
	if (i != AT_NORMAL)
		last_overstrike = i;

#if HILITE_SEARCH
	{
		int matches;
		int resend_last = 0;
		int hl_attr = 0;

		if (pos == NULL_POSITION)
		{
			/* Color the prompt unless it has ansi sequences in it. */
			hl_attr = ansi_in_line ? 0 : AT_STANDOUT|AT_COLOR_PROMPT;
		} else if (a != AT_ANSI)
		{
			hl_attr = is_hilited_attr(pos, pos+1, 0, &matches);
			if (hl_attr == 0 && status_line)
				hl_attr = line_mark_attr;
		}
		if (hl_attr)
		{
			/*
			 * This character should be highlighted.
			 * Override the attribute passed in.
			 */
			a |= hl_attr;
			if (highest_hilite != NULL_POSITION && pos != NULL_POSITION && pos > highest_hilite)
				highest_hilite = pos;
			in_hilite = 1;
		} else 
		{
			if (in_hilite)
			{
				/*
				 * This is the first non-hilited char after a hilite.
				 * Resend the last ANSI seq to restore color.
				 */
				resend_last = 1;
			}
			in_hilite = 0;
		}
		if (resend_last)
		{
			int ai;
			for (ai = 0;  ai < NUM_LAST_ANSIS;  ai++)
			{
				int ax = (curr_last_ansi + ai) % NUM_LAST_ANSIS;
				for (i = 0;  i < last_ansis[ax].end;  i++)
					STORE_CHAR(last_ansis[ax].data[i], AT_ANSI, NULL, pos);
			}
		}
	}
#endif

	if (a == AT_ANSI) {
		w = 0;
	} else {
		char *p = &linebuf.buf[linebuf.end];
		LWCHAR prev_ch = (linebuf.end > 0) ? step_char(&p, -1, linebuf.buf) : 0;
		int prev_a = (linebuf.end > 0) ? linebuf.attr[linebuf.end-1] : 0;
		w = pwidth(ch, a, prev_ch, prev_a);
	}

	if (!fits_on_screen(w, a))
		return (1);

	if (rep == NULL)
	{
		cs = (char) ch;
		rep = &cs;
		replen = 1;
	} else
	{
		replen = utf_len(rep[0]);
	}

	if (cshift == hshift)
	{
		if (line_pos == NULL_POSITION)
			line_pos = pos;
		if (shifted_ansi.end > 0)
		{
			/* Copy shifted ANSI sequences to beginning of line. */
			for (i = 0;  i < shifted_ansi.end;  i++)
				add_linebuf(shifted_ansi.data[i], AT_ANSI, 0);
			xbuf_reset(&shifted_ansi);
		}
	}

	/* Add the char to the buf, even if we will left-shift it next. */
	inc_end_column(w);
	for (i = 0;  i < replen;  i++)
		add_linebuf(*rep++, a, 0);

	if (cshift < hshift)
	{
		/* We haven't left-shifted enough yet. */
		if (a == AT_ANSI)
			xbuf_add_byte(&shifted_ansi, (unsigned char) ch); /* Save ANSI attributes */
		if (linebuf.end > linebuf.print)
		{
			/* Shift left enough to put last byte of this char at print-1. */
			int i;
			for (i = 0; i < linebuf.print; i++)
			{
				linebuf.buf[i] = linebuf.buf[i+replen];
				linebuf.attr[i] = linebuf.attr[i+replen];
			}
			linebuf.end -= replen;
			cshift += w;
			/*
			 * If the char we just left-shifted was double width,
			 * the 2 spaces we shifted may be too much.
			 * Represent the "half char" at start of line with a highlighted space.
			 */
			while (cshift > hshift)
			{
				add_linebuf(' ', rscroll_attr, 0);
				cshift--;
			}
		}
	}
	return (0);
}

#define STORE_STRING(s,a,pos) \
	do { if (store_string((s),(a),(pos))) return (1); } while (0)

static int store_string(char *s, int a, POSITION pos)
{
	if (!fits_on_screen(strlen(s), a))
		return 1;
	for ( ;  *s != 0;  s++)
		STORE_CHAR(*s, a, NULL, pos);
	return 0;
}

/*
 * Append a tab to the line buffer.
 * Store spaces to represent the tab.
 */
#define STORE_TAB(a,pos) \
	do { if (store_tab((a),(pos))) return (1); } while (0)

static int store_tab(int attr, POSITION pos)
{
	int to_tab = end_column - linebuf.pfx_end;

	if (ntabstops < 2 || to_tab >= tabstops[ntabstops-1])
		to_tab = tabdefault -
		     ((to_tab - tabstops[ntabstops-1]) % tabdefault);
	else
	{
		int i;
		for (i = ntabstops - 2;  i >= 0;  i--)
			if (to_tab >= tabstops[i])
				break;
		to_tab = tabstops[i+1] - to_tab;
	}

	do {
		STORE_CHAR(' ', attr, " ", pos);
	} while (--to_tab > 0);
	return 0;
}

#define STORE_PRCHAR(c, pos) \
	do { if (store_prchar((c), (pos))) return 1; } while (0)

static int store_prchar(LWCHAR c, POSITION pos)
{
	/*
	 * Convert to printable representation.
	 */
	STORE_STRING(prchar(c), AT_BINARY|AT_COLOR_CTRL, pos);
	return 0;
}

static int flush_mbc_buf(POSITION pos)
{
	int i;

	for (i = 0; i < mbc_buf_index; i++)
		if (store_prchar(mbc_buf[i], pos))
			return mbc_buf_index - i;
	return 0;
}

/*
 * Append a character to the line buffer.
 * Expand tabs into spaces, handle underlining, boldfacing, etc.
 * Returns 0 if ok, 1 if couldn't fit in buffer.
 */
public int pappend(int c, POSITION pos)
{
	int r;

	if (pendc)
	{
		if (c == '\r' && pendc == '\r')
			return (0);
		if (do_append(pendc, NULL, pendpos))
			/*
			 * Oops.  We've probably lost the char which
			 * was in pendc, since caller won't back up.
			 */
			return (1);
		pendc = '\0';
	}

	if (c == '\r' && (proc_return == OPT_ON || (bs_mode == BS_SPECIAL && proc_return == OPT_OFF)))
	{
		if (mbc_buf_len > 0)  /* utf_mode must be on. */
		{
			/* Flush incomplete (truncated) sequence. */
			r = flush_mbc_buf(mbc_pos);
			mbc_buf_index = r + 1;
			mbc_buf_len = 0;
			if (r)
				return (mbc_buf_index);
		}

		/*
		 * Don't put the CR into the buffer until we see 
		 * the next char.  If the next char is a newline,
		 * discard the CR.
		 */
		pendc = c;
		pendpos = pos;
		return (0);
	}

	if (!utf_mode)
	{
		r = do_append(c, NULL, pos);
	} else
	{
		/* Perform strict validation in all possible cases. */
		if (mbc_buf_len == 0)
		{
		retry:
			mbc_buf_index = 1;
			*mbc_buf = c;
			if (IS_ASCII_OCTET(c))
				r = do_append(c, NULL, pos);
			else if (IS_UTF8_LEAD(c))
			{
				mbc_buf_len = utf_len(c);
				mbc_pos = pos;
				return (0);
			} else
				/* UTF8_INVALID or stray UTF8_TRAIL */
				r = flush_mbc_buf(pos);
		} else if (IS_UTF8_TRAIL(c))
		{
			mbc_buf[mbc_buf_index++] = c;
			if (mbc_buf_index < mbc_buf_len)
				return (0);
			if (is_utf8_well_formed(mbc_buf, mbc_buf_index))
				r = do_append(get_wchar(mbc_buf), mbc_buf, mbc_pos);
			else
				/* Complete, but not shortest form, sequence. */
				mbc_buf_index = r = flush_mbc_buf(mbc_pos);
			mbc_buf_len = 0;
		} else
		{
			/* Flush incomplete (truncated) sequence.  */
			r = flush_mbc_buf(mbc_pos);
			mbc_buf_index = r + 1;
			mbc_buf_len = 0;
			/* Handle new char.  */
			if (!r)
				goto retry;
		}
	}
	if (r)
	{
		/* How many chars should caller back up? */
		r = (!utf_mode) ? 1 : mbc_buf_index;
	}
	return (r);
}

static int store_control_char(LWCHAR ch, char *rep, POSITION pos)
{
	if (ctldisp == OPT_ON)
	{
		/* Output the character itself. */
		STORE_CHAR(ch, AT_NORMAL, rep, pos);
	} else 
	{
		/* Output a printable representation of the character. */
		STORE_PRCHAR((char) ch, pos);
	}
	return (0);
}

static int store_ansi(LWCHAR ch, char *rep, POSITION pos)
{
	switch (ansi_step(line_ansi, ch))
	{
	case ANSI_MID:
		STORE_CHAR(ch, AT_ANSI, rep, pos);
		if (line_ansi->hlink)
			hlink_in_line = 1;
		xbuf_add_byte(&last_ansi, (unsigned char) ch);
		break;
	case ANSI_END:
		STORE_CHAR(ch, AT_ANSI, rep, pos);
		ansi_done(line_ansi);
		line_ansi = NULL;
		xbuf_add_byte(&last_ansi, (unsigned char) ch);
		xbuf_set(&last_ansis[curr_last_ansi], &last_ansi);
		xbuf_reset(&last_ansi);
		curr_last_ansi = (curr_last_ansi + 1) % NUM_LAST_ANSIS;
		break;
	case ANSI_ERR:
		{
			/* Remove whole unrecognized sequence.  */
			char *start = (cshift < hshift) ? xbuf_char_data(&shifted_ansi): linebuf.buf;
			int *end = (cshift < hshift) ? &shifted_ansi.end : &linebuf.end;
			char *p = start + *end;
			LWCHAR bch;
			do {
				bch = step_char(&p, -1, start);
			} while (p > start && !IS_CSI_START(bch));
			*end = (int) (p - start);
		}
		xbuf_reset(&last_ansi);
		ansi_done(line_ansi);
		line_ansi = NULL;
		break;
	}
	return (0);
} 

static int store_bs(LWCHAR ch, char *rep, POSITION pos)
{
	if (proc_backspace == OPT_ONPLUS || (bs_mode == BS_CONTROL && proc_backspace == OPT_OFF))
		return store_control_char(ch, rep, pos);
	if (linebuf.end > 0 &&
		((linebuf.end <= linebuf.print && linebuf.buf[linebuf.end-1] == '\0') ||
	     (linebuf.end > 0 && linebuf.attr[linebuf.end - 1] & (AT_ANSI|AT_BINARY))))
		STORE_PRCHAR('\b', pos);
	else if (proc_backspace == OPT_OFF && bs_mode == BS_NORMAL)
		STORE_CHAR(ch, AT_NORMAL, NULL, pos);
	else if (proc_backspace == OPT_ON || (bs_mode == BS_SPECIAL && proc_backspace == OPT_OFF))
		overstrike = backc();
	return 0;
}

static int do_append(LWCHAR ch, char *rep, POSITION pos)
{
	int a = AT_NORMAL;
	int in_overstrike = overstrike;

	if (ctldisp == OPT_ONPLUS && line_ansi == NULL)
	{
		line_ansi = ansi_start(ch);
		if (line_ansi != NULL)
			ansi_in_line = 1;
	}

	overstrike = 0;
	if (line_ansi != NULL)
		return store_ansi(ch, rep, pos);

	if (ch == '\b')
		return store_bs(ch, rep, pos);

	if (in_overstrike > 0)
	{
		/*
		 * Overstrike the character at the current position
		 * in the line buffer.  This will cause either 
		 * underline (if a "_" is overstruck), 
		 * bold (if an identical character is overstruck),
		 * or just replacing the character in the buffer.
		 */
		LWCHAR prev_ch;
		overstrike = utf_mode ? -1 : 0;
		if (utf_mode)
		{
			/* To be correct, this must be a base character.  */
			prev_ch = get_wchar(&linebuf.buf[linebuf.end]);
		} else
		{
			prev_ch = (unsigned char) linebuf.buf[linebuf.end];
		}
		a = linebuf.attr[linebuf.end];
		if (ch == prev_ch)
		{
			/*
			 * Overstriking a char with itself means make it bold.
			 * But overstriking an underscore with itself is
			 * ambiguous.  It could mean make it bold, or
			 * it could mean make it underlined.
			 * Use the previous overstrike to resolve it.
			 */
			if (ch == '_')
			{
				if ((a & (AT_BOLD|AT_UNDERLINE)) != AT_NORMAL)
					a |= (AT_BOLD|AT_UNDERLINE);
				else if (last_overstrike != AT_NORMAL)
					a |= last_overstrike;
				else
					a |= AT_BOLD;
			} else
				a |= AT_BOLD;
		} else if (ch == '_')
		{
			a |= AT_UNDERLINE;
			ch = prev_ch;
			rep = &linebuf.buf[linebuf.end];
		} else if (prev_ch == '_')
		{
			a |= AT_UNDERLINE;
		}
		/* Else we replace prev_ch, but we keep its attributes.  */
	} else if (in_overstrike < 0)
	{
		if (   is_composing_char(ch)
		    || is_combining_char(get_wchar(&linebuf.buf[linebuf.end]), ch))
			/* Continuation of the same overstrike.  */
			a = last_overstrike;
		else
			overstrike = 0;
	}

	if (ch == '\t')
	{
		/*
		 * Expand a tab into spaces.
		 */
		if (proc_tab == OPT_ONPLUS || (bs_mode == BS_CONTROL && proc_tab == OPT_OFF))
			return store_control_char(ch, rep, pos);
		STORE_TAB(a, pos);
		return (0);
	}
	if ((!utf_mode || is_ascii_char(ch)) && control_char((char)ch))
	{
		return store_control_char(ch, rep, pos);
	} else if (utf_mode && ctldisp != OPT_ON && is_ubin_char(ch))
	{
		STORE_STRING(prutfchar(ch), AT_BINARY, pos);
	} else
	{
		STORE_CHAR(ch, a, rep, pos);
	}
	return (0);
}

/*
 *
 */
public int pflushmbc(void)
{
	int r = 0;

	if (mbc_buf_len > 0)
	{
		/* Flush incomplete (truncated) sequence.  */
		r = flush_mbc_buf(mbc_pos);
		mbc_buf_len = 0;
	}
	return r;
}

/*
 * Switch to normal attribute at end of line.
 */
static void add_attr_normal(void)
{
	if (ctldisp != OPT_ONPLUS || !is_ansi_end('m'))
		return;
	addstr_linebuf("\033[m", AT_ANSI, 0);
	if (hlink_in_line) /* Don't send hyperlink clear if we know we don't need to. */
		addstr_linebuf("\033]8;;\033\\", AT_ANSI, 0);
}

/*
 * Terminate the line in the line buffer.
 */
public void pdone(int endline, int chopped, int forw)
{
	(void) pflushmbc();

	if (pendc && (pendc != '\r' || !endline))
		/*
		 * If we had a pending character, put it in the buffer.
		 * But discard a pending CR if we are at end of line
		 * (that is, discard the CR in a CR/LF sequence).
		 */
		(void) do_append(pendc, NULL, pendpos);

	if (chopped && rscroll_char)
	{
		/*
		 * Display the right scrolling char.
		 * If we've already filled the rightmost screen char 
		 * (in the buffer), overwrite it.
		 */
		if (end_column >= sc_width + cshift)
		{
			/* We've already written in the rightmost char. */
			end_column = right_column;
			linebuf.end = right_curr;
		}
		add_attr_normal();
		while (end_column < sc_width-1 + cshift) 
		{
			/*
			 * Space to last (rightmost) char on screen.
			 * This may be necessary if the char we overwrote
			 * was double-width.
			 */
			add_linebuf(' ', rscroll_attr, 1);
		}
		/* Print rscroll char. It must be single-width. */
		add_linebuf(rscroll_char, rscroll_attr, 1);
	} else
	{
		add_attr_normal();
	}

	/*
	 * If we're coloring a status line, fill out the line with spaces.
	 */
	if (status_line && line_mark_attr != 0) {
		while (end_column +1 < sc_width + cshift)
			add_linebuf(' ', line_mark_attr, 1);
	}

	/*
	 * Add a newline if necessary,
	 * and append a '\0' to the end of the line.
	 * We output a newline if we're not at the right edge of the screen,
	 * or if the terminal doesn't auto wrap,
	 * or if this is really the end of the line AND the terminal ignores
	 * a newline at the right edge.
	 * (In the last case we don't want to output a newline if the terminal 
	 * doesn't ignore it since that would produce an extra blank line.
	 * But we do want to output a newline if the terminal ignores it in case
	 * the next line is blank.  In that case the single newline output for
	 * that blank line would be ignored!)
	 */
	if (end_column < sc_width + cshift || !auto_wrap || (endline && ignaw) || ctldisp == OPT_ON)
	{
		add_linebuf('\n', AT_NORMAL, 0);
	} 
	else if (ignaw && end_column >= sc_width + cshift && forw)
	{
		/*
		 * Terminals with "ignaw" don't wrap until they *really* need
		 * to, i.e. when the character *after* the last one to fit on a
		 * line is output. But they are too hard to deal with when they
		 * get in the state where a full screen width of characters
		 * have been output but the cursor is sitting on the right edge
		 * instead of at the start of the next line.
		 * So we nudge them into wrapping by outputting a space 
		 * character plus a backspace.  But do this only if moving 
		 * forward; if we're moving backward and drawing this line at
		 * the top of the screen, the space would overwrite the first
		 * char on the next line.  We don't need to do this "nudge" 
		 * at the top of the screen anyway.
		 */
		add_linebuf(' ', AT_NORMAL, 1);
		add_linebuf('\b', AT_NORMAL, -1);
	}
	set_linebuf(linebuf.end, '\0', AT_NORMAL);
}

/*
 * Set an attribute on each char of the line in the line buffer.
 */
public void set_attr_line(int a)
{
	int i;

	for (i = linebuf.print;  i < linebuf.end;  i++)
		if ((linebuf.attr[i] & AT_COLOR) == 0 || (a & AT_COLOR) == 0)
			linebuf.attr[i] |= a;
}

/*
 * Set the char to be displayed in the status column.
 */
public void set_status_col(char c, int attr)
{
	set_pfx(0, c, attr);
}

/*
 * Get a character from the current line.
 * Return the character as the function return value,
 * and the character attribute in *ap.
 */
public int gline(int i, int *ap)
{
	if (is_null_line)
	{
		/*
		 * If there is no current line, we pretend the line is
		 * either "~" or "", depending on the "twiddle" flag.
		 */
		if (twiddle)
		{
			if (i == 0)
			{
				*ap = AT_BOLD;
				return '~';
			}
			--i;
		}
		/* Make sure we're back to AT_NORMAL before the '\n'.  */
		*ap = AT_NORMAL;
		return i ? '\0' : '\n';
	}

	if (i < linebuf.pfx_end)
	{
		*ap = linebuf.pfx_attr[i];
		return linebuf.pfx[i];
	}
	i += linebuf.print - linebuf.pfx_end;
	*ap = linebuf.attr[i];
	return (linebuf.buf[i] & 0xFF);
}

/*
 * Indicate that there is no current line.
 */
public void null_line(void)
{
	is_null_line = 1;
	cshift = 0;
}

/*
 * Analogous to forw_line(), but deals with "raw lines":
 * lines which are not split for screen width.
 * {{ This is supposed to be more efficient than forw_line(). }}
 */
public POSITION forw_raw_line(POSITION curr_pos, char **linep, int *line_lenp)
{
	int n;
	int c;
	POSITION new_pos;

	if (curr_pos == NULL_POSITION || ch_seek(curr_pos) ||
		(c = ch_forw_get()) == EOI)
		return (NULL_POSITION);

	n = 0;
	for (;;)
	{
		if (c == '\n' || c == EOI || ABORT_SIGS())
		{
			new_pos = ch_tell();
			break;
		}
		if (n >= size_linebuf-1)
		{
			if (expand_linebuf())
			{
				/*
				 * Overflowed the input buffer.
				 * Pretend the line ended here.
				 */
				new_pos = ch_tell() - 1;
				break;
			}
		}
		linebuf.buf[n++] = c;
		c = ch_forw_get();
	}
	linebuf.buf[n] = '\0';
	if (linep != NULL)
		*linep = linebuf.buf;
	if (line_lenp != NULL)
		*line_lenp = n;
	return (new_pos);
}

/*
 * Analogous to back_line(), but deals with "raw lines".
 * {{ This is supposed to be more efficient than back_line(). }}
 */
public POSITION back_raw_line(POSITION curr_pos, char **linep, int *line_lenp)
{
	int n;
	int c;
	POSITION new_pos;

	if (curr_pos == NULL_POSITION || curr_pos <= ch_zero() ||
		ch_seek(curr_pos-1))
		return (NULL_POSITION);

	n = size_linebuf;
	linebuf.buf[--n] = '\0';
	for (;;)
	{
		c = ch_back_get();
		if (c == '\n' || ABORT_SIGS())
		{
			/*
			 * This is the newline ending the previous line.
			 * We have hit the beginning of the line.
			 */
			new_pos = ch_tell() + 1;
			break;
		}
		if (c == EOI)
		{
			/*
			 * We have hit the beginning of the file.
			 * This must be the first line in the file.
			 * This must, of course, be the beginning of the line.
			 */
			new_pos = ch_zero();
			break;
		}
		if (n <= 0)
		{
			int old_size_linebuf = size_linebuf;
			char *fm;
			char *to;
			if (expand_linebuf())
			{
				/*
				 * Overflowed the input buffer.
				 * Pretend the line ended here.
				 */
				new_pos = ch_tell() + 1;
				break;
			}
			/*
			 * Shift the data to the end of the new linebuf.
			 */
			for (fm = linebuf.buf + old_size_linebuf - 1,
			      to = linebuf.buf + size_linebuf - 1;
			     fm >= linebuf.buf;  fm--, to--)
				*to = *fm;
			n = size_linebuf - old_size_linebuf;
		}
		linebuf.buf[--n] = c;
	}
	if (linep != NULL)
		*linep = &linebuf.buf[n];
	if (line_lenp != NULL)
		*line_lenp = size_linebuf - 1 - n;
	return (new_pos);
}

/*
 * Skip cols printable columns at the start of line.
 * Return number of bytes skipped.
 */
public int skip_columns(int cols, char **linep, int *line_lenp)
{
	char *line = *linep;
	char *eline = line + *line_lenp;
	LWCHAR pch = 0;
	int bytes;

	while (cols > 0 && line < eline)
	{
		LWCHAR ch = step_char(&line, +1, eline);
		struct ansi_state *pansi = ansi_start(ch);
		if (pansi != NULL)
		{
			skip_ansi(pansi, &line, eline);
			ansi_done(pansi);
			pch = 0;
		} else
		{
			int w = pwidth(ch, 0, pch, 0);
			cols -= w;
			pch = ch;
		}
	}
	bytes = line - *linep;
	*linep = line;
	*line_lenp -= bytes;
	return (bytes);
}

/*
 * Append a string to the line buffer.
 */
static int pappstr(constant char *str)
{
	while (*str != '\0')
	{
		if (pappend(*str++, NULL_POSITION))
			/* Doesn't fit on screen. */
			return 1;
	}
	return 0;
}

/*
 * Load a string into the line buffer.
 * If the string is too long to fit on the screen,
 * truncate the beginning of the string to fit.
 */
public void load_line(constant char *str)
{
	int save_hshift = hshift;

	hshift = 0;
	for (;;)
	{
		prewind();
		if (pappstr(str) == 0)
			break;
		/*
		 * Didn't fit on screen; increase left shift by one.
		 * {{ This gets very inefficient if the string
		 * is much longer than the screen width. }}
		 */
		hshift += 1;
	}
	set_linebuf(linebuf.end, '\0', AT_NORMAL);
	hshift = save_hshift;
}

/*
 * Find the shift necessary to show the end of the longest displayed line.
 */
public int rrshift(void)
{
	POSITION pos;
	int save_width;
	int line;
	int longest = 0;

	save_width = sc_width;
	sc_width = INT_MAX;
	pos = position(TOP);
	for (line = 0; line < sc_height && pos != NULL_POSITION; line++)
	{
		pos = forw_line(pos);
		if (end_column > longest)
			longest = end_column;
	}
	sc_width = save_width;
	if (longest < sc_width)
		return 0;
	return longest - sc_width;
}

/*
 * Get the color_map index associated with a given attribute.
 */
static int lookup_color_index(int attr)
{
	int cx;
	for (cx = 0;  cx < sizeof(color_map)/sizeof(*color_map);  cx++)
		if (color_map[cx].attr == attr)
			return cx;
	return -1;
}

static int color_index(int attr)
{
	if (use_color && (attr & AT_COLOR))
		return lookup_color_index(attr & AT_COLOR);
	if (attr & AT_UNDERLINE)
		return lookup_color_index(AT_UNDERLINE);
	if (attr & AT_BOLD)
		return lookup_color_index(AT_BOLD);
	if (attr & AT_BLINK)
		return lookup_color_index(AT_BLINK);
	if (attr & AT_STANDOUT)
		return lookup_color_index(AT_STANDOUT);
	return -1;
}

/*
 * Set the color string to use for a given attribute.
 */
public int set_color_map(int attr, char *colorstr)
{
	int cx = color_index(attr);
	if (cx < 0)
		return -1;
	if (strlen(colorstr)+1 > sizeof(color_map[cx].color))
		return -1;
	if (*colorstr != '\0' && parse_color(colorstr, NULL, NULL) == CT_NULL)
		return -1;
	strcpy(color_map[cx].color, colorstr);
	return 0;
}

/*
 * Get the color string to use for a given attribute.
 */
public char * get_color_map(int attr)
{
	int cx = color_index(attr);
	if (cx < 0)
		return NULL;
	return color_map[cx].color;
}
