/*
 * Copyright (C) 1984-2023  Mark Nudelman
 *
 * You may distribute under the terms of either the GNU General Public
 * License or the Less License, as specified in the README file.
 *
 * For more information, see the README file.
 */

#include "xbuf.h"

/*
 * Format of a lesskey file:
 *
 *      LESSKEY_MAGIC (4 bytes)
 *       sections...
 *      END_LESSKEY_MAGIC (4 bytes)
 *
 * Each section is:
 *
 *      section_MAGIC (1 byte)
 *      section_length (2 bytes)
 *      key table (section_length bytes)
 */
#define C0_LESSKEY_MAGIC        '\0'
#define C1_LESSKEY_MAGIC        'M'
#define C2_LESSKEY_MAGIC        '+'
#define C3_LESSKEY_MAGIC        'G'

#define CMD_SECTION             'c'
#define EDIT_SECTION            'e'
#define VAR_SECTION             'v'
#define END_SECTION             'x'

#define C0_END_LESSKEY_MAGIC    'E'
#define C1_END_LESSKEY_MAGIC    'n'
#define C2_END_LESSKEY_MAGIC    'd'

/* */
#define KRADIX          64

struct lesskey_cmdname
{
	char *cn_name;
	int cn_action;
};

struct lesskey_table
{
	struct lesskey_cmdname *names;
	struct xbuffer buf;
	int is_var;
};

struct lesskey_tables
{
	struct lesskey_table *currtable;
	struct lesskey_table cmdtable;
	struct lesskey_table edittable;
	struct lesskey_table vartable;
};

extern int parse_lesskey(char *infile, struct lesskey_tables *tables);
