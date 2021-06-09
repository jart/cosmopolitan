/*
  Copyright (c) 1990-2000 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*

This file implements the getenv() function.

#  Background:
#  Under Unix: Each Process (= running Program) has a set of
#  associated variables. The variables are called enviroment
#  variables and, together, constitute the process environment.
#  These variables include the search path, the terminal type,
#  and the user's login name.

#  Unfortunatelly the MacOS has no equivalent. So we need
#  a file to define the environment variables.
#  Name of this file is "MacZip.Env". It can be placed
#  in the current folder of MacZip or in the
#  preference folder of the system disk.
#  If MacZip founds the "MacZip.Env" file in the current
#  the folder of MacZip the "MacZip.Env" file in the
#  preference folder will be ignored.

#  An environment variable has a name and a value:
#  Name=Value
#  Note: Spaces are significant:
#  ZIPOPT=-r  and
#  ZIPOPT = -r are different !!!


 */

/*****************************************************************************/
/*  Includes                                                                 */
/*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unix.h>
#include <Files.h>
#include <Folders.h>

#include "pathname.h"
#include "helpers.h"

/*****************************************************************************/
/*  Module level Vars                                                        */
/*****************************************************************************/

static char ListAllKeyValues = 0;
static unsigned LineNumber   = 0;
static char CompletePath[NAME_MAX];
Boolean IgnoreEnvironment    = false;  /* used by dialog.c and initfunc.c
                                          of the Mainapp */

/*****************************************************************************/
/*  Macros, typedefs                                                         */
/*****************************************************************************/

typedef struct _EnviromentPair {
    char *key;
    char *value;
} EnviromentPair;


#define MAX_COMMAND 1024


/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/


int get_char(FILE *file);
void unget_char(int ch,FILE *file);
int get_string(char *string,int size, FILE *file, char *terms);
void skip_comments(FILE *file);
char *load_entry(FILE *file);
char *getenv(const char *name);
EnviromentPair *ParseLine(char *line);
OSErr FSpFindFolder_Name(short vRefNum, OSType folderType,
                         Boolean createFolder,FSSpec *spec, unsigned char *name);
FILE * FSp_fopen(ConstFSSpecPtr spec, const char * open_mode);
void ShowAllKeyValues(void);
void Set_LineNum(unsigned ln);

/*****************************************************************************/
/*  Functions                                                                */
/*****************************************************************************/


/* get_string(str, max, file, termstr) : like fgets() but
 *      (1) has terminator string which should include \n
 *      (2) will always leave room for the null
 *      (3) uses get_char() so LineNumber will be accurate
 *      (4) returns EOF or terminating character, whichever
 */
int get_string(char *string, int size, FILE *file, char *terms)
{
    int ch;

    while (EOF != (ch = get_char(file)) && !strchr(terms, ch)) {
        if (size > 1) {
            *string++ = (char) ch;
            size--;
        }
    }

    if (size > 0)
        {
        *string = '\0';
        }

    return ch;
}




void Set_LineNum(unsigned ln)
{
 LineNumber = ln;
}



/* get_char(file) : like getc() but increment LineNumber on newlines
 */
int get_char(FILE *file)
{
    int ch;

    ch = getc(file);
    if (ch == '\n')
        {
        Set_LineNum(LineNumber + 1);
        }

    return ch;
}




/* skip_comments(file) : read past comment (if any)
 */
void skip_comments(FILE *file)
{
    int ch;

    while (EOF != (ch = get_char(file)))
        {
        /* ch is now the first character of a line.
         */

        while (ch == ' ' || ch == '\t')
            {
            ch = get_char(file);
            }

        if (ch == EOF)
            {
            break;
            }

        /* ch is now the first non-blank character of a line.
         */

        if (ch != '\n' && ch != '#')
            {
            break;
            }

        /* ch must be a newline or comment as first non-blank
         * character on a line.
         */

        while (ch != '\n' && ch != EOF)
            {
            ch = get_char(file);
            }

        /* ch is now the newline of a line which we're going to
         * ignore.
         */
    }

    if (ch != EOF)
        {
        unget_char(ch, file);
        }
}




/* unget_char(ch, file) : like ungetc but do LineNumber processing
 */
void unget_char(int ch, FILE *file)
{
    ungetc(ch, file);
    if (ch == '\n')
        {
        Set_LineNum(LineNumber - 1);
        }
}


/* this function reads one file entry -- the next -- from a file.
*  it skips any leading blank lines, ignores comments, and returns
*  NULL if for any reason the entry can't be read and parsed.
*/

char *load_entry(FILE *file)
{
    int ch;
    static char cmd[MAX_COMMAND];

    skip_comments(file);

    ch = get_string(cmd, MAX_COMMAND, file, "\n");

    if (ch == EOF)
        {
        return NULL;
        }

    return cmd;
}





EnviromentPair *ParseLine(char *line)
{
char *tmpPtr;
static EnviromentPair *Env;
unsigned short length = strlen(line);

Env->key   = "";
Env->value = "";

for (tmpPtr = line; *tmpPtr; tmpPtr++)
    {
    if (*tmpPtr == '=')
        {
        *tmpPtr = 0;
        Env->key = line;
        if (strlen(Env->key) < length)
            {
            Env->value = ++tmpPtr;
            }
        return Env;
        }
    }
return Env;
}





char *getenv(const char *name)
{
FILE *fp;
char *LineStr = NULL;
EnviromentPair *Env1;
FSSpec spec;
OSErr err;

if (IgnoreEnvironment)
    return NULL;  /* user wants to ignore the environment vars */

if (name == NULL)
    return NULL;

GetCompletePath(CompletePath,"MacZip.Env",&spec,&err);

/* try open the file in the current folder */
fp = FSp_fopen(&spec,"r");
if (fp == NULL)
    { /* Okey, lets try open the file in the preference folder */
    FSpFindFolder_Name(
                   kOnSystemDisk,
                   kPreferencesFolderType,
                   kDontCreateFolder,
                   &spec,
                   "\pMacZip.Env");
    fp = FSp_fopen(&spec,"r");
    if (fp == NULL)
        {
        return NULL; /* there is no enviroment-file */
        }
    }

LineStr = load_entry(fp);
while (LineStr != NULL)
    {   /* parse the file line by line */
    Env1 = ParseLine(LineStr);
    if (strlen(Env1->value) > 0)
        {       /* we found a key/value pair */
        if (ListAllKeyValues)
            printf("\n   Line:%3d   [%s] = [%s]",LineNumber,Env1->key,Env1->value);
        if (stricmp(name,Env1->key) == 0)
            {   /* we found the value of a given key */
            return Env1->value;
            }
        }
    LineStr = load_entry(fp);  /* read next line */
    }
fclose(fp);

return NULL;
}





OSErr FSpFindFolder_Name(
    short vRefNum,          /* Volume reference number. */
    OSType folderType,      /* Folder type taken by FindFolder. */
    Boolean createFolder,   /* Should we create it if non-existant. */
    FSSpec *spec,           /* Pointer to resulting directory. */
    unsigned char *name)    /* Name of the file in the folder */
{
    short foundVRefNum;
    long foundDirID;
    OSErr err;

    err = FindFolder(vRefNum, folderType, createFolder,
                     &foundVRefNum, &foundDirID);
    if (err != noErr)
        {
        return err;
        }

    err = FSMakeFSSpec(foundVRefNum, foundDirID, name, spec);
    return err;
}




void ShowAllKeyValues(void)
{
OSErr err;
FSSpec spec;
Boolean tmpIgnoreEnvironment = IgnoreEnvironment;

ListAllKeyValues = 1;
IgnoreEnvironment = false;

GetCompletePath(CompletePath,"MacZip.Env",&spec,&err);
if (err != 0)
    { /* Okey, lets try open the file in the preference folder */
    FSpFindFolder_Name(
                   kOnSystemDisk,
                   kPreferencesFolderType,
                   kDontCreateFolder,
                   &spec,
                   "\pMacZip.Env");
    GetFullPathFromSpec(CompletePath,&spec, &err);
    if (err != 0)
        {
        return; /* there is no enviroment-file */
        }
    }

printf("\nLocation of the current \"MacZip.Env\" file:\n [%s]",CompletePath);

printf("\n\nList of all environment variables\n");
getenv(" ");
printf("\n\nEnd\n\n");

/* restore used variables */
ListAllKeyValues = 0;
LineNumber = 0;
IgnoreEnvironment = tmpIgnoreEnvironment;
}










