# Microsoft Developer Studio Project File - Name="zipsplit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=zipsplit - Win32 ASM Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "zipsplit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "zipsplit.mak" CFG="zipsplit - Win32 ASM Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "zipsplit - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "zipsplit - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "zipsplit - Win32 ASM Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "zipsplit - Win32 ASM Release" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "zipsplit - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "zipsplit___Win32_Release"
# PROP BASE Intermediate_Dir "zipsplit___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "zipsplit___Win32_Release"
# PROP Intermediate_Dir "zipsplit___Win32_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "UTIL" /D "WIN32" /D "NO_ASM" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "zipsplit - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "zipsplit___Win32_Debug"
# PROP BASE Intermediate_Dir "zipsplit___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "zipsplit___Win32_Debug"
# PROP Intermediate_Dir "zipsplit___Win32_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "UTIL" /D "WIN32" /D "NO_ASM" /FR /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "zipsplit - Win32 ASM Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "zipsplit___Win32_ASM_Debug"
# PROP BASE Intermediate_Dir "zipsplit___Win32_ASM_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "zipsplit___Win32_ASM_Debug"
# PROP Intermediate_Dir "zipsplit___Win32_ASM_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "UTIL" /D "WIN32" /FR /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "UTIL" /D "WIN32" /FR /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "zipsplit - Win32 ASM Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "zipsplit___Win32_ASM_Release"
# PROP BASE Intermediate_Dir "zipsplit___Win32_ASM_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "zipsplit___Win32_ASM_Release"
# PROP Intermediate_Dir "zipsplit___Win32_ASM_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "UTIL" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "UTIL" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ENDIF 

# Begin Target

# Name "zipsplit - Win32 Release"
# Name "zipsplit - Win32 Debug"
# Name "zipsplit - Win32 ASM Debug"
# Name "zipsplit - Win32 ASM Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\crc32.c
# End Source File
# Begin Source File

SOURCE=..\..\fileio.c
# End Source File
# Begin Source File

SOURCE=..\..\globals.c
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

SOURCE=..\..\zipfile.c
# End Source File
# Begin Source File

SOURCE=..\..\zipsplit.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\crc32.h
# End Source File
# Begin Source File

SOURCE=..\..\ebcdic.h
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

SOURCE=..\win32zip.h
# End Source File
# Begin Source File

SOURCE=..\..\zip.h
# End Source File
# Begin Source File

SOURCE=..\..\ziperr.h
# End Source File
# End Group
# Begin Group "Assembler Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\crc_i386.asm

!IF  "$(CFG)" == "zipsplit - Win32 Release"

!ELSEIF  "$(CFG)" == "zipsplit - Win32 Debug"

!ELSEIF  "$(CFG)" == "zipsplit - Win32 ASM Debug"

# Begin Custom Build - Assembling...
IntDir=.\zipsplit___Win32_ASM_Debug
InputPath=..\crc_i386.asm
InputName=crc_i386

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml /nologo /c /Cx /coff /Zi /Fo"$(IntDir)\$(InputName).obj" "$(InputPath)"

# End Custom Build

!ELSEIF  "$(CFG)" == "zipsplit - Win32 ASM Release"

# Begin Custom Build - Assembling...
IntDir=.\zipsplit___Win32_ASM_Release
InputPath=..\crc_i386.asm
InputName=crc_i386

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml /nologo /c /Cx /coff /Zi /Fo"$(IntDir)\$(InputName).obj" "$(InputPath)"

# End Custom Build

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
