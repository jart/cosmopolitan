/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
/*---------------------------------------------------------------------------

  ZipSx.h

  This header-files is global to the project ZipSioux.

  ---------------------------------------------------------------------------*/


/*****************************************************************************/
/*  Macros, typedefs                                                         */
/*****************************************************************************/

#define MACOS
#define USE_SIOUX
#define MACZIP

#define OLDROUTINENAMES     0           /* use new function names only */
#define OLDROUTINELOCATIONS 0           /* use new headerlocations only */
#define SystemSevenOrLater  1           /* Runs only on System 7.0 or later */

/* These functions are defined as a macro instead of a function.
so we have to undefine them for replacing (see printf.c)  */
#undef getc
#undef getchar
#undef putchar

#ifndef ZCONST
#  define ZCONST const
#endif

#define NAME_MAX    1024


/*****************************************************************************/
/*  Includes standard headers                                                */
/*****************************************************************************/
#include <ansi_prefix.mac.h>
#include <stdio.h>
#include <TextUtils.h>
#include <Folders.h>
#include <Aliases.h>
#include <Resources.h>
#include <Gestalt.h>
#include <Traps.h>
#include <Processes.h>
#include <MacWindows.h>


/* Many things are different for mac-users, so we need
   special mac functions :-)  */
int Zmacstat (const char *path, struct stat *buf);
#define stat(path, bufPtr) Zmacstat(path, bufPtr)
#define lstat(path, bufPtr) Zmacstat(path, bufPtr)

int fprintf(FILE *file, const char *format, ...);
int printf(const char *format, ...);
void perror(const char *parm1);



/*
#define MAC_DEBUG  1
  */






#ifdef MAC_DEBUG
#define LOG_DEBUG   7   /* debug-level messages */
int Print2Syslog(UInt8 priority, const char *format, ...);
#include <ctype.h>


#define Notify(msg)                                                 \
        {                                                           \
        (void)Print2Syslog(LOG_DEBUG, "%s (file: %s line: %d)",     \
                     msg, __FILE__, __LINE__);        \
        }




#define Assert_it(cond,msg,kind)                                        \
        {                                                               \
        if (!(cond))                                                    \
            {                                                           \
            (void)Print2Syslog(LOG_DEBUG,"%s failed: [%s] cond: [%s] (file: %s line: %d)", \
                          kind, msg, #cond, __FILE__, __LINE__);        \
            }                                                           \
        }




#define AssertBool(b,msg) Assert_it (((b) == TRUE) || ((b) == FALSE),(msg),("AssertBool "))



#define AssertStr(s,msg)                                            \
    {                                                               \
        int s_i = 0;                                                \
        Assert_it ((s),(msg),("1. AssertStr "));                    \
        while ((s)[s_i]) {                                          \
            Assert_it ((!iscntrl((s)[s_i]) || ((s)[s_i] == 0x0A) ||  \
                       ((s)[s_i] == 0x0D)),(s),("2. AssertStr "));  \
            s_i++;                                                  \
        }                                                           \
    }



#define AssertTime(t,msg)   Assert_it (((t).tm_sec  >=  0) && ((t).tm_sec  < 62) &&   \
                ((t).tm_min  >=  0) && ((t).tm_min  < 60) &&   \
                ((t).tm_hour >=  0) && ((t).tm_hour < 24) &&   \
                ((t).tm_mday >=  1) && ((t).tm_mday < 32) &&   \
                ((t).tm_mon  >=  0) && ((t).tm_mon  < 12) &&   \
                ((t).tm_wday >=  0) && ((t).tm_wday < 7)  &&   \
                ((t).tm_yday >=  0) && ((t).tm_yday < 366),(msg),("AssertStr "))



#define AssertIntRange(myvalue,minimum,maximum, msg)  Assert_it ( \
        ((myvalue) >= (minimum)) && ((myvalue) <= (maximum)), msg,("AssertIntRange "))




#define AssertStrNoOverlap(str1,str2,msg)                           \
    {                                                               \
        long s_i = 0;                                               \
        AssertStr((str1),(msg))                                     \
        AssertStr((str2),(msg))                                     \
        if ((str1) < (str2))                                        \
            {                                                       \
            s_i = strlen((str2));                                   \
            Assert_it ( (((str1) + s_i) < (str2)),(msg),("AssertStrNoOverlap "));   \
            }                                                       \
        else                                                        \
            {                                                       \
            s_i = strlen((str1));                                   \
            Assert_it ( (((str2) + s_i) < (str1)),(msg),("AssertStrNoOverlap "));   \
            }                                                       \
    }                                                               \




#else
#define Assert_it(cond,msg,kind)
#define AssertBool(b,msg)
#define AssertStr(s,msg)
#define AssertTime(t,msg)
#define AssertIntRange(myvalue,minimum,maximum,msg)
#define AssertStrNoOverlap(str1,str2,msg)
#endif

