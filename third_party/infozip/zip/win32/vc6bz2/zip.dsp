# Microsoft Developer Studio Project File - Name="zip" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=zip - Win32 Debug bzip2
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "zip.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "zip.mak" CFG="zip - Win32 Debug bzip2"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "zip - Win32 ASM Release bzip2" (based on "Win32 (x86) Console Application")
!MESSAGE "zip - Win32 ASM Debug bzip2" (based on "Win32 (x86) Console Application")
!MESSAGE "zip - Win32 Release bzip2" (based on "Win32 (x86) Console Application")
!MESSAGE "zip - Win32 Debug bzip2" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "zip - Win32 ASM Release bzip2"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "zip___Win32_ASM_Release_bzip2"
# PROP BASE Intermediate_Dir "zip___Win32_ASM_Release_bzip2"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "zip___Win32_ASM_Release_bzip2"
# PROP Intermediate_Dir "zip___Win32_ASM_Release_bzip2"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "ASM_CRC" /D "ASMV" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../../bzip2" /D "ASM_CRC" /D "ASMV" /D "WIN32" /D "BZIP2_SUPPORT" /D "BZ_NO_STDIO" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "zip - Win32 ASM Debug bzip2"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "zip___Win32_ASM_Debug_bzip2"
# PROP BASE Intermediate_Dir "zip___Win32_ASM_Debug_bzip2"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "zip___Win32_ASM_Debug_bzip2"
# PROP Intermediate_Dir "zip___Win32_ASM_Debug_bzip2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "ASM_CRC" /D "ASMV" /FR /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../bzip2" /D "ASM_CRC" /D "ASMV" /D "WIN32" /D "BZIP2_SUPPORT" /D "BZ_NO_STDIO" /FR /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "zip - Win32 Release bzip2"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "zip___Win32_Release_bzip2"
# PROP BASE Intermediate_Dir "zip___Win32_Release_bzip2"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "zip___Win32_Release_bzip2"
# PROP Intermediate_Dir "zip___Win32_Release_bzip2"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NO_ASM" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../../bzip2" /D "NO_ASM" /D "WIN32" /D "BZIP2_SUPPORT" /D "BZ_NO_STDIO" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "zip - Win32 Debug bzip2"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "zip___Win32_Debug_bzip2"
# PROP BASE Intermediate_Dir "zip___Win32_Debug_bzip2"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "zip___Win32_Debug_bzip2"
# PROP Intermediate_Dir "zip___Win32_Debug_bzip2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "NO_ASM" /D "WIN32" /FR /FD /GZ /c
# SUBTRACT BASE CPP /WX
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../bzip2" /D "NO_ASM" /D "WIN32" /D "BZIP2_SUPPORT" /D "BZ_NO_STDIO" /FR /FD /GZ /c
# SUBTRACT CPP /WX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "zip - Win32 ASM Release bzip2"
# Name "zip - Win32 ASM Debug bzip2"
# Name "zip - Win32 Release bzip2"
# Name "zip - Win32 Debug bzip2"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\bzip2\blocksort.c
# End Source File
# Begin Source File

SOURCE=..\..\bzip2\bzlib.c
# End Source File
# Begin Source File

SOURCE=..\..\bzip2\compress.c
# End Source File
# Begin Source File

SOURCE=..\..\crc32.c
# End Source File
# Begin Source File

SOURCE=..\..\bzip2\crctable.c
# End Source File
# Begin Source File

SOURCE=..\..\crypt.c
# End Source File
# Begin Source File

SOURCE=..\..\bzip2\decompress.c
# End Source File
# Begin Source File

SOURCE=..\..\deflate.c
# End Source File
# Begin Source File

SOURCE=..\..\fileio.c
# End Source File
# Begin Source File

SOURCE=..\..\globals.c
# End Source File
# Begin Source File

SOURCE=..\..\bzip2\huffman.c
# End Source File
# Begin Source File

SOURCE=..\nt.c
# End Source File
# Begin Source File

SOURCE=..\..\bzip2\randtable.c
# End Source File
# Begin Source File

SOURCE=..\..\trees.c
# End Source File
# Begin Source File

SOURCE=..\..\ttyio.c
# End Source File
# Begin Source File

SOURCE=..\..\util.c
# End Source File
# Begin Source File

SOURCE=..\win32.c
# End Source File
# Begin Source File

SOURCE=..\win32i64.c
# End Source File
# Begin Source File

SOURCE=..\win32zip.c
# End Source File
# Begin Source File

SOURCE=..\..\zbz2err.c
# End Source File
# Begin Source File

SOURCE=..\..\zip.c
# End Source File
# Begin Source File

SOURCE=..\zip.rc
# End Source File
# Begin Source File

SOURCE=..\..\zipfile.c
# End Source File
# Begin Source File

SOURCE=..\..\zipup.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\bzip2\bzlib.h
# End Source File
# Begin Source File

SOURCE=..\..\crc32.h
# End Source File
# Begin Source File

SOURCE=..\..\crypt.h
# End Source File
# Begin Source File

SOURCE=..\..\ebcdic.h
# End Source File
# Begin Source File

SOURCE=..\nt.h
# End Source File
# Begin Source File

SOURCE=..\osdep.h
# End Source File
# Begin Source File

SOURCE=..\..\revision.h
# End Source File
# Begin Source File

SOURCE=..\..\tailor.h
# End Source File
# Begin Source File

SOURCE=..\..\ttyio.h
# End Source File
# Begin Source File

SOURCE=..\win32zip.h
# End Source File
# Begin Source File

SOURCE=..\..\zip.h
# End Source File
# Begin Source File

SOURCE=..\..\ziperr.h
# End Source File
# Begin Source File

SOURCE=..\zipup.h
# End Source File
# End Group
# Begin Group "Assembler Files"

# PROP Default_Filter "asm;obj"
# Begin Source File

SOURCE=..\crc_i386.asm

!IF  "$(CFG)" == "zip - Win32 ASM Release bzip2"

# Begin Custom Build - Assembling...
IntDir=.\zip___Win32_ASM_Release_bzip2
InputPath=..\crc_i386.asm
InputName=crc_i386

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml /nologo /c /Cx /coff /Zi /Fo"$(IntDir)\$(InputName).obj" "$(InputPath)"

# End Custom Build

!ELSEIF  "$(CFG)" == "zip - Win32 ASM Debug bzip2"

# Begin Custom Build - Assembling...
IntDir=.\zip___Win32_ASM_Debug_bzip2
InputPath=..\crc_i386.asm
InputName=crc_i386

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml /nologo /c /Cx /coff /Zi /Fo"$(IntDir)\$(InputName).obj" "$(InputPath)"

# End Custom Build

!ELSEIF  "$(CFG)" == "zip - Win32 Release bzip2"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zip - Win32 Debug bzip2"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\match32.asm

!IF  "$(CFG)" == "zip - Win32 ASM Release bzip2"

# Begin Custom Build - Assembling...
IntDir=.\zip___Win32_ASM_Release_bzip2
InputPath=..\match32.asm
InputName=match32

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml /nologo /c /Cx /coff /Zi /Fo"$(IntDir)\$(InputName).obj" "$(InputPath)"

# End Custom Build

!ELSEIF  "$(CFG)" == "zip - Win32 ASM Debug bzip2"

# Begin Custom Build - Assembling...
IntDir=.\zip___Win32_ASM_Debug_bzip2
InputPath=..\match32.asm
InputName=match32

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml /nologo /c /Cx /coff /Zi /Fo"$(IntDir)\$(InputName).obj" "$(InputPath)"

# End Custom Build

!ELSEIF  "$(CFG)" == "zip - Win32 Release bzip2"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zip - Win32 Debug bzip2"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
