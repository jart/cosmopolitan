/* mailcheck.h -- variables and function declarations for mail checking. */

/* Copyright (C) 1987-2020 Free Software Foundation, Inc.

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

#if !defined (_MAILCHECK_H_)
#define _MAILCHECK_H_

/* Functions from mailcheck.c */
extern int time_to_check_mail PARAMS((void));
extern void reset_mail_timer PARAMS((void));
extern void reset_mail_files PARAMS((void));
extern void free_mail_files PARAMS((void));
extern char *make_default_mailpath PARAMS((void));
extern void remember_mail_dates PARAMS((void));
extern void init_mail_dates PARAMS((void));
extern void check_mail PARAMS((void));

#endif /* _MAILCHECK_H */
