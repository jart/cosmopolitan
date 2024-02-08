/*
 * Copyright (C) 1984-2023  Mark Nudelman
 *
 * You may distribute under the terms of either the GNU General Public
 * License or the Less License, as specified in the README file.
 *
 * For more information, see the README file.
 */

#if HAVE_GNU_REGEX
#define __USE_GNU 1
#include <regex.h>
#define PATTERN_TYPE             struct re_pattern_buffer *
#define SET_NULL_PATTERN(name)   name = NULL
#endif

/* ---- POSIX ---- */
#if HAVE_POSIX_REGCOMP
#include <regex.h>
#ifdef REG_EXTENDED
#define REGCOMP_FLAG             REG_EXTENDED
#else
#define REGCOMP_FLAG             0
#endif
#define PATTERN_TYPE             regex_t *
#define SET_NULL_PATTERN(name)   name = NULL
#define re_handles_caseless      TRUE
#endif

/* ---- PCRE ---- */
#if HAVE_PCRE
#include <pcre.h>
#define PATTERN_TYPE             pcre *
#define SET_NULL_PATTERN(name)   name = NULL
#define re_handles_caseless      TRUE
#endif

/* ---- PCRE2 ---- */
#if HAVE_PCRE2
#define PCRE2_CODE_UNIT_WIDTH 8
#include "third_party/pcre/pcre2.h"
#define PATTERN_TYPE             pcre2_code *
#define SET_NULL_PATTERN(name)   name = NULL
#define re_handles_caseless      TRUE
#endif

/* ---- RE_COMP  ---- */
#if HAVE_RE_COMP
char *re_comp(char*);
int re_exec(char*);
#define PATTERN_TYPE             int
#define SET_NULL_PATTERN(name)   name = 0
#endif

/* ---- REGCMP  ---- */
#if HAVE_REGCMP
char *regcmp(char*);
char *regex(char**, char*);
extern char *__loc1;
#define PATTERN_TYPE             char **
#define SET_NULL_PATTERN(name)   name = NULL
#endif

/* ---- REGCOMP  ---- */
#if HAVE_V8_REGCOMP
#include "regexp.h"
extern int reg_show_error;
#define PATTERN_TYPE             struct regexp *
#define SET_NULL_PATTERN(name)   name = NULL
#endif

/* ---- NONE  ---- */
#if NO_REGEX
#define PATTERN_TYPE             void *
#define SET_NULL_PATTERN(name)   
#endif

#ifndef re_handles_caseless
#define re_handles_caseless      FALSE
#endif
