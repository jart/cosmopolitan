/*
  Copyright (c) 1990-2001 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
#ifndef PATHNAME_H
#define PATHNAME_H 1


char *StripPartialDir(char *CompletePath,
                      const char *PartialPath, const char *FullPath);

char *Real2RfDfFilen(char *RfDfFilen, const char *RealPath, short CurrentFork,
                     short MacZipMode, Boolean DataForkOnly);
char *RfDfFilen2Real(char *RealFn, const char *RfDfFilen, short MacZipMode,
                     Boolean DataForkOnly, short *CurrentFork);

unsigned short GetVolumeFromPath(const char *FullPath, char *VolumeName);
char *GetCompletePath(char *CompletePath, const char *name, FSSpec *Spec,
                      OSErr *err);
char *TruncFilename(char *DirPath, const char *FilePath);
char *GetFilename(char *CompletePath, const char *name);
char *GetFullPathFromSpec(char *CompletePath, FSSpec *Spec, OSErr *err);
char *GetFullPathFromID(char *CompletePath, short vRefNum, long dirID,
                        ConstStr255Param name, OSErr *err);

char *GetAppName(void);
void createArchiveName(char *Path);
void FindDesktopFolder(char *Path);
char *FindNewExtractFolder(char *ExtractPath, Boolean uniqueFolder);
OSErr FSpFindFolder(
    short vRefNum,          /* Volume reference number. */
    OSType folderType,      /* Folder type taken by FindFolder. */
    Boolean createFolder,   /* Should we create it if non-existant. */
    FSSpec *spec);          /* Pointer to resulting directory. */

char *MakeFilenameShorter(const char *LongFilename);

/*
Rule: UnKnown_EF should always be zero.
      JohnnyLee_EF, NewZipMode_EF should always greater than all
      other definitions
*/
#define UnKnown_EF           0
#define TomBrownZipIt1_EF   10
#define TomBrownZipIt2_EF   20
#define JohnnyLee_EF        30
#define NewZipMode_EF       40



#define ResourceFork    -1
#define DataFork        1
#define NoFork          0


#ifndef NAME_MAX
#define NAME_MAX    1024
#endif

#endif   /*  PATHNAME_H  */
