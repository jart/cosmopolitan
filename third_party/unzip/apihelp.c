/*
  Copyright (c) 1990-2001 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in unzip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/* apihelp.c */

#ifdef API_DOC

#define UNZIP_INTERNAL
#include "third_party/unzip/unzip.h"
#include "third_party/unzip/unzvers.h"


APIDocStruct APIDoc[] = {
    {
        "UZPVERSION"  , "UzpVersion"  ,
        "UzpVer *UzpVersion(void);",
        "Get version numbers of the API and the underlying UnZip code.\n\n"
        "\t\tThis is used for comparing the version numbers of the run-time\n"
        "\t\tDLL code with those expected from the unzip.h at compile time.\n"
        "\t\tIf the version numbers do not match, there may be compatibility\n"
        "\t\tproblems with further use of the DLL.\n\n"
        "  Example:\t/* Check the major version number of the DLL code. */\n"
        "\t\tUzpVer *pVersion;\n"
        "\t\tpVersion = UzpVersion();\n"
        "\t\tif (pVersion->unzip.major != UZ_MAJORVER)\n"
        "\t\t  fprintf(stderr, \"error: using wrong version of DLL\\n\");\n\n"
        "\t\tSee unzip.h for details and unzipstb.c for an example.\n"
    },

    {
        "UZPMAIN"  , "UzpMain"  ,
        "int UzpMain(int argc, char *argv[]);",
        "Provide a direct entry point to the command line interface.\n\n"
        "\t\tThis is used by the UnZip stub but you can use it in your\n"
        "\t\town program as well.  Output is sent to stdout.\n"
        "\t\t0 on return indicates success.\n\n"
        "  Example:\t/* Extract 'test.zip' silently, junking paths. */\n"
        "\t\tchar *argv[] = { \"-q\", \"-j\", \"test.zip\" };\n"
        "\t\tint argc = 3;\n"
        "\t\tif (UzpMain(argc,argv))\n"
        "\t\t  printf(\"error: unzip failed\\n\");\n\n"
        "\t\tSee unzip.h for details.\n"
    },

    {
        "UZPALTMAIN"  , "UzpAltMain"  ,
        "int UzpAltMain(int argc, char *argv[], UzpInit *init);",
        "Provide a direct entry point to the command line interface,\n"
        "optionally installing replacement I/O handler functions.\n\n"
        "\t\tAs with UzpMain(), output is sent to stdout by default.\n"
        "\t\t`InputFn *inputfn' is not yet implemented.  0 on return\n"
        "\t\tindicates success.\n\n"
        "  Example:\t/* Replace normal output and `more' functions. */\n"
        "\t\tchar *argv[] = { \"-q\", \"-j\", \"test.zip\" };\n"
        "\t\tint argc = 3;\n"
        "\t\tUzpInit init = { 16, MyMessageFn, NULL, MyPauseFn };\n"
        "\t\tif (UzpAltMain(argc,argv,&init))\n"
        "\t\t  printf(\"error: unzip failed\\n\");\n\n"
        "\t\tSee unzip.h for details.\n"
    },

    {
        "UZPUNZIPTOMEMORY", "UzpUnzipToMemory",
        "int UzpUnzipToMemory(char *zip, char *file, UzpBuffer *retstr);",
        "Pass the name of the zip file and the name of the file\n"
        "\t\tyou wish to extract.  UzpUnzipToMemory will create a\n"
        "\t\tbuffer and return it in *retstr;  0 on return indicates\n"
        "\t\tfailure.\n\n"
        "\t\tSee unzip.h for details.\n"
    },

    {
        "UZPFILETREE", "UzpFileTree",
        "int UzpFileTree(char *name, cbList(callBack),\n"
        "\t\t\tchar *cpInclude[], char *cpExclude[]);",
        "Pass the name of the zip file, a callback function, an\n"
        "\t\tinclude and exclude file list. UzpFileTree calls the\n"
        "\t\tcallback for each valid file found in the zip file.\n"
        "\t\t0 on return indicates failure.\n\n"
        "\t\tSee unzip.h for details.\n"
    },

    { 0 }
};


static int function_help OF((__GPRO__ APIDocStruct *doc, char *fname));



static int function_help(__G__ doc, fname)
    __GDEF
    APIDocStruct *doc;
    char *fname;
{
    strcpy(slide, fname);
    /* strupr(slide);    non-standard */
    while (doc->compare && STRNICMP(doc->compare,slide,strlen(fname)))
        doc++;
    if (!doc->compare)
        return 0;
    else
        Info(slide, 0, ((char *)slide,
          "  Function:\t%s\n\n  Syntax:\t%s\n\n  Purpose:\t%s",
          doc->function, doc->syntax, doc->purpose));

    return 1;
}



void APIhelp(__G__ argc, argv)
    __GDEF
    int argc;
    char **argv;
{
    if (argc > 1) {
        struct APIDocStruct *doc;

        if (function_help(__G__ APIDoc, argv[1]))
            return;
#ifdef SYSTEM_API_DETAILS
        if (function_help(__G__ SYSTEM_API_DETAILS, argv[1]))
            return;
#endif
        Info(slide, 0, ((char *)slide,
          "%s is not a documented command.\n\n", argv[1]));
    }

    Info(slide, 0, ((char *)slide, "\
This API provides a number of external C and REXX functions for handling\n\
zipfiles in OS/2.  Programmers are encouraged to expand this API.\n\
\n\
C functions: -- See unzip.h for details\n\
  UzpVer *UzpVersion(void);\n\
  int UzpMain(int argc, char *argv[]);\n\
  int UzpAltMain(int argc, char *argv[], UzpInit *init);\n\
  int UzpUnzipToMemory(char *zip, char *file, UzpBuffer *retstr);\n\
  int UzpFileTree(char *name, cbList(callBack),\n\
                  char *cpInclude[], char *cpExclude[]);\n\n"));

#ifdef SYSTEM_API_BRIEF
    Info(slide, 0, ((char *)slide, SYSTEM_API_BRIEF));
#endif

    Info(slide, 0, ((char *)slide,
      "\nFor more information, type 'unzip -A <function-name>'\n"));
}

#endif /* API_DOC */
