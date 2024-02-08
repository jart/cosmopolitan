/* dispose_cmd.h -- Functions appearing in dispose_cmd.c. */

/* Copyright (C) 1993-2020 Free Software Foundation, Inc.

   This file is part of GNU Bash, the Bourne Again SHell.

   Bash is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Bash is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Bash.  If not, see <http://www.gnu.org/licenses/>.
*/

#if !defined (_DISPOSE_CMD_H_)
#define _DISPOSE_CMD_H_

#include "stdc.h"

extern void dispose_command PARAMS((COMMAND *));
extern void dispose_word_desc PARAMS((WORD_DESC *));
extern void dispose_word PARAMS((WORD_DESC *));
extern void dispose_words PARAMS((WORD_LIST *));
extern void dispose_word_array PARAMS((char **));
extern void dispose_redirects PARAMS((REDIRECT *));

#if defined (COND_COMMAND)
extern void dispose_cond_node PARAMS((COND_COM *));
#endif

extern void dispose_function_def_contents PARAMS((FUNCTION_DEF *));
extern void dispose_function_def PARAMS((FUNCTION_DEF *));

#endif /* !_DISPOSE_CMD_H_ */
