/* `dir', `vdir' and `ls' directory listing programs for GNU.

   Modified by Chet Ramey for Readline.

   Copyright (C) 1985, 1988, 1990-1991, 1995-2010, 2012, 2015
   Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Written by Richard Stallman and David MacKenzie.  */

/* Color support by Peter Anvin <Peter.Anvin@linux.org> and Dennis
   Flaherty <dennisf@denix.elk.miles.com> based on original patches by
   Greg Lee <lee@uhunix.uhcc.hawaii.edu>.  */

#ifndef _COLORS_H_
#define _COLORS_H_

#include <stdio.h> // size_t

#if defined(__TANDEM) && defined(HAVE_STDBOOL_H) && (__STDC_VERSION__ < 199901L)
typedef int _Bool;
#endif

#if defined (HAVE_STDBOOL_H)
#  include <stdbool.h> // bool
#else
typedef int _rl_bool_t;

#ifdef bool
#  undef bool
#endif
#define bool _rl_bool_t

#ifndef true
#  define true 1
#  define false 0
#endif

#endif /* !HAVE_STDBOOL_H */

/* Null is a valid character in a color indicator (think about Epson
   printers, for example) so we have to use a length/buffer string
   type. */
struct bin_str
  {
    size_t len;
    const char *string;
  };

/* file type indicators (dir, sock, fifo, ...)
   Default value is initialized in parse-colors.c.
   It is then modified from the values of $LS_COLORS. */
extern struct bin_str _rl_color_indicator[];

/* The LS_COLORS variable is in a termcap-like format. */
typedef struct _color_ext_type
  {
    struct bin_str ext;         	/* The extension we're looking for */
    struct bin_str seq;         	/* The sequence to output when we do */
    struct _color_ext_type *next;	/* Next in list */
  } COLOR_EXT_TYPE;

/* file extensions indicators (.txt, .log, .jpg, ...)
   Values are taken from $LS_COLORS in rl_parse_colors(). */
extern COLOR_EXT_TYPE *_rl_color_ext_list;

#define FILETYPE_INDICATORS				\
  {							\
    C_ORPHAN, C_FIFO, C_CHR, C_DIR, C_BLK, C_FILE,	\
    C_LINK, C_SOCK, C_FILE, C_DIR			\
  }

/* Whether we used any colors in the output so far.  If so, we will
   need to restore the default color later.  If not, we will need to
   call prep_non_filename_text before using color for the first time. */

enum indicator_no
  {
    C_LEFT, C_RIGHT, C_END, C_RESET, C_NORM, C_FILE, C_DIR, C_LINK,
    C_FIFO, C_SOCK,
    C_BLK, C_CHR, C_MISSING, C_ORPHAN, C_EXEC, C_DOOR, C_SETUID, C_SETGID,
    C_STICKY, C_OTHER_WRITABLE, C_STICKY_OTHER_WRITABLE, C_CAP, C_MULTIHARDLINK,
    C_CLR_TO_EOL
  };


#if !S_IXUGO
# define S_IXUGO (S_IXUSR | S_IXGRP | S_IXOTH)
#endif

enum filetype
  {
    unknown,
    fifo,
    chardev,
    directory,
    blockdev,
    normal,
    symbolic_link,
    sock,
    whiteout,
    arg_directory
  };

/* Prefix color, currently same as socket */
#define C_PREFIX	C_SOCK

extern void _rl_put_indicator (const struct bin_str *ind);
extern void _rl_set_normal_color (void);
extern bool _rl_print_prefix_color (void);
extern bool _rl_print_color_indicator (const char *f);
extern void _rl_prep_non_filename_text (void);

#endif /* !_COLORS_H_ */
