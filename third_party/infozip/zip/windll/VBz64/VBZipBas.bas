Attribute VB_Name = "VBZipBas"

Option Explicit

'---------------------------------------------------------------
'-- Please Do Not Remove These Comments!!!
'---------------------------------------------------------------
'-- Sample VB 6 code to drive zip32z64.dll
'-- Based on the code contributed to the Info-ZIP project
'-- by Mike Le Voi
'--
'-- See the original VB example in a separate directory for
'-- more information
'--
'-- Use this code at your own risk. Nothing implied or warranted
'-- to work on your machine :-)
'---------------------------------------------------------------
'--
'-- The Source Code Is Freely Available From Info-ZIP At:
'-- ftp://ftp.info-zip.org/pub/infozip/infozip.html
'--
'-- A Very Special Thanks To Mr. Mike Le Voi
'-- And Mr. Mike White Of The Info-ZIP
'-- For Letting Me Use And Modify His Orginal
'-- Visual Basic 5.0 Code! Thank You Mike Le Voi.
'---------------------------------------------------------------

'---------------------------------------------------------------
' This example is redesigned to work with Zip32z64.dll compiled from
' Zip 3.0 with Zip64 enabled.  This allows for archives with more
' and larger files than allowed in previous versions.
'
' Modified 4/24/2004, 12/4/2007 by Ed Gordon
'---------------------------------------------------------------

'---------------------------------------------------------------
' Usage notes:
'
' This code uses Zip32z64.dll.  You DO NOT need to register the
' DLL to use it.  You also DO NOT need to reference it in your
' VB project.  You DO have to copy the DLL to your SYSTEM
' directory, your VB project directory, or place it in a directory
' on your command PATH.
'
' Note that Zip32z64 is probably not thread safe so you should avoid
' using the dll in multiple threads at the same time without first
' testing for interaction.
'
' All code provided under the Info-Zip license.  If you have
' any questions please contact Info-Zip.
'
' April 24 2004 EG
'
'---------------------------------------------------------------

'-- C Style argv
'-- Holds The Zip Archive Filenames
'
' Max for zFiles just over 8000 as each pointer takes up 4 bytes and
' VB only allows 32 kB of local variables and that includes function
' parameters.  - 3/19/2004 EG
'
' Can put names in strZipFileNames instead of using this array,
' which avoids this limit.  File names are separated by spaces.
' Enclose names in quotes if include spaces.
Public Type ZIPnames
  zFiles(1 To 100) As String
End Type

'-- Call Back "String"
Public Type ZipCBChar
  ch(4096) As Byte
End Type

'-- Version Structure
Public Type VerType
  Major As Byte
  Minor As Byte
  PatchLevel As Byte
  NotUsed As Byte
End Type
Public Type ZipVerType
  structlen       As Long         ' Length Of The Structure Being Passed
  flag            As Long         ' Bit 0: is_beta  bit 1: uses_zlib
  Beta            As String * 10  ' e.g., "g BETA" or ""
  date            As String * 20  ' e.g., "4 Sep 95" (beta) or "4 September 1995"
  ZLIB            As String * 10  ' e.g., "1.0.5" or NULL
  encryption      As Long         ' 0 if encryption not available
  ZipVersion      As VerType
  os2dllVersion   As VerType
  windllVersion   As VerType
End Type

'-- ZPOPT Is Used To Set The Options In The ZIP32z64.DLL
Public Type ZpOpt
  date           As String ' Date in either US 12/31/98 or 1998-12-31 format
  szRootDir      As String ' Root Directory Pathname (Up To 256 Bytes Long)
  szTempDir      As String ' Temp Directory Pathname (Up To 256 Bytes Long)
  fTemp          As Long   ' 1 If Temp dir Wanted, Else 0
  fSuffix        As Long   ' Include Suffixes (Not Yet Implemented!)
  fEncrypt       As Long   ' 1 If Encryption Wanted, Else 0
  fSystem        As Long   ' 1 To Include System/Hidden Files, Else 0
  fVolume        As Long   ' 1 If Storing Volume Label, Else 0
  fExtra         As Long   ' 1 If Excluding Extra Attributes, Else 0
  fNoDirEntries  As Long   ' 1 If Ignoring Directory Entries (end with /), Else 0
  fExcludeDate   As Long   ' 1 If Excluding Files After Specified Date, Else 0
  fIncludeDate   As Long   ' 1 If Including Files After Specified Date, Else 0
  fVerbose       As Long   ' 1 If Full Messages Wanted, Else 0
  fQuiet         As Long   ' 1 If Minimum Messages Wanted, Else 0
  fCRLF_LF       As Long   ' 1 If Translate CR/LF To LF, Else 0
  fLF_CRLF       As Long   ' 1 If Translate LF To CR/LF, Else 0
  fJunkDir       As Long   ' 1 If Junking Directory Names on entries, Else 0
  fGrow          As Long   ' 1 If Allow Appending To Zip File, Else 0
  fForce         As Long   ' 1 If Making Entries Using DOS File Names, Else 0
  fMove          As Long   ' 1 If Deleting Files Added Or Updated, Else 0
  fDeleteEntries As Long   ' 1 If Files Passed Have To Be Deleted, Else 0
  fUpdate        As Long   ' 1 If Updating Zip File-Overwrite Only If Newer, Else 0
  fFreshen       As Long   ' 1 If Freshing Zip File-Overwrite Only, Else 0
  fJunkSFX       As Long   ' 1 If Junking SFX Prefix, Else 0
  fLatestTime    As Long   ' 1 If Setting Zip File Time To Time Of Latest File In Archive, Else 0
  fComment       As Long   ' 1 If Putting Comment In Zip File, Else 0
  fOffsets       As Long   ' 1 If Updating Archive Offsets For SFX Files, Else 0
  fPrivilege     As Long   ' 1 If Not Saving Privileges, Else 0
  fEncryption    As Long   ' Read Only Property!!!
  szSplitSize    As String ' Size of split if splitting, Else NULL (empty string)
                           ' This string contains the size that you want to
                           ' split the archive into. i.e. 100 for 100 bytes,
                           ' 2K for 2 k bytes, where K is 1024, m for meg
                           ' and g for gig.
  szIncludeList  As String ' If used, space separated list of Include filename
                           ' patterns where match includes file - put quotes
                           ' around each filename pattern.
  IncludeListCount As Long ' place filler (not for VB) - (inits to 0) DO NOT USE
  IncludeList    As Long   ' place filler (not for VB) - (inits to 0) DO NOT USE
  szExcludeList  As String ' If used, space separated list of Exclude filename
                           ' patterns where match excludes file - put quotes
                           ' around each filename pattern.
  ExcludeListCount As Long ' place filler (not for VB) - (inits to 0) DO NOT USE
  ExcludeList    As Long   ' place filler (not for VB) - (inits to 0) DO NOT USE
  fRecurse       As Long   ' 1 (-r), 2 (-R) If Recursing Into Sub-Directories, Else 0
  fRepair        As Long   ' 1 = Fix Archive, 2 = Try Harder To Fix, Else 0
  flevel         As Byte   ' Compression Level - 0 = Stored 6 = Default 9 = Max
End Type


' Used by SetZipOptions
Public Enum ZipModeType
    Add = 0
    Delete = 1
    Update = 2
    Freshen = 3
End Enum
Public Enum CompressionLevelType
    c0_NoCompression = 0
    c1_Fast = 1
    c2_Fast = 2
    c3_Fast = 3
    c4_Med = 4
    c5_Med = 5
    c6_Default = 6
    c7_Extra = 7
    c8_Extra = 8
    c9_Max = 9
End Enum
Public Enum Translate_LF_Type
    No_Line_End_Trans = 0
    LF_To_CRLF = 1
    CRLF_To_LF = 2
End Enum
Public Enum RepairType
    NoRepair = 0
    TryFix = 1
    TryFixHarder = 2
End Enum
Public Enum VerbosenessType
    Quiet = 0
    Normal = 1
    Verbose = 2
End Enum
Public Enum RecurseType
    NoRecurse = 0
    r_RecurseIntoSubdirectories = 1
    R_RecurseUsingPatterns = 2
End Enum


'-- This Structure Is Used For The ZIP32z64.DLL Function Callbacks
'   Assumes Zip32z64.dll with Zip64 enabled
Public Type ZIPUSERFUNCTIONS
  ZDLLPrnt     As Long           ' Callback ZIP32z64.DLL Print Function
  ZDLLCOMMENT  As Long           ' Callback ZIP32z64.DLL Comment Function
  ZDLLPASSWORD As Long           ' Callback ZIP32z64.DLL Password Function
  ZDLLSPLIT    As Long           ' Callback ZIP32z64.DLL Split Select Function
  ' There are 2 versions of SERVICE, we use one does not need 64-bit data type
  ZDLLSERVICE  As Long           ' Callback ZIP32z64.DLL Service Function
  ZDLLSERVICE_NO_INT64  As Long  ' Callback ZIP32z64.DLL Service Function
End Type

'-- Default encryption password (used in callback if not empty string)
Public EncryptionPassword As String

'-- For setting the archive comment
Public ArchiveCommentText

'-- version info
Public ZipVersion As ZipVerType

'-- Local Declarations
Public ZOPT  As ZpOpt
Public ZUSER As ZIPUSERFUNCTIONS

'-- This Assumes ZIP32z64.DLL Is In Your \windows\system directory
'-- or a copy is in the program directory or in some other directory
'-- listed in PATH
Private Declare Function ZpInit Lib "zip32z64.dll" _
  (ByRef Zipfun As ZIPUSERFUNCTIONS) As Long '-- Set Zip Callbacks

Private Declare Function ZpArchive Lib "zip32z64.dll" _
  (ByVal argc As Long, ByVal funame As String, _
   ByRef argv As ZIPnames, ByVal strNames As String, ByRef Opts As ZpOpt) As Long '-- Real Zipping Action

Private Declare Sub ZpVersion Lib "zip32z64.dll" _
  (ByRef ZipVersion As ZipVerType) '-- Version of DLL


'-------------------------------------------------------
'-- Public Variables For Setting The ZPOPT Structure...
'-- (WARNING!!!) You Must Set The Options That You
'-- Want The ZIP32.DLL To Do!
'-- Before Calling VBZip32!
'--
'-- NOTE: See The Above ZPOPT Structure Or The VBZip32
'--       Function, For The Meaning Of These Variables
'--       And How To Use And Set Them!!!
'-- These Parameters Must Be Set Before The Actual Call
'-- To The VBZip32 Function!
'-------------------------------------------------------

'-- Public Program Variables
Public zArgc           As Integer     ' Number Of Files To Zip Up
Public zZipArchiveName As String      ' The Zip File Name ie: Myzip.zip
Public zZipFileNames   As ZIPnames    ' File Names To Zip Up
Public strZipFileNames As String      ' String of names to Zip Up
Public zZipInfo        As String      ' Holds The Zip File Information

'-- Public Constants
'-- For Zip & UnZip Error Codes!
Public Const ZE_OK = 0              ' Success (No Error)
Public Const ZE_EOF = 2             ' Unexpected End Of Zip File Error
Public Const ZE_FORM = 3            ' Zip File Structure Error
Public Const ZE_MEM = 4             ' Out Of Memory Error
Public Const ZE_LOGIC = 5           ' Internal Logic Error
Public Const ZE_BIG = 6             ' Entry Too Large To Split Error
Public Const ZE_NOTE = 7            ' Invalid Comment Format Error
Public Const ZE_TEST = 8            ' Zip Test (-T) Failed Or Out Of Memory Error
Public Const ZE_ABORT = 9           ' User Interrupted Or Termination Error
Public Const ZE_TEMP = 10           ' Error Using A Temp File
Public Const ZE_READ = 11           ' Read Or Seek Error
Public Const ZE_NONE = 12           ' Nothing To Do Error
Public Const ZE_NAME = 13           ' Missing Or Empty Zip File Error
Public Const ZE_WRITE = 14          ' Error Writing To A File
Public Const ZE_CREAT = 15          ' Could't Open To Write Error
Public Const ZE_PARMS = 16          ' Bad Command Line Argument Error
Public Const ZE_OPEN = 18           ' Could Not Open A Specified File To Read Error

'-- These Functions Are For The ZIP32z64.DLL
'--
'-- Puts A Function Pointer In A Structure
'-- For Use With Callbacks...
Public Function FnPtr(ByVal lp As Long) As Long
    
  FnPtr = lp

End Function

'-- Callback For ZIP32z64.DLL - DLL Print Function
Public Function ZDLLPrnt(ByRef fname As ZipCBChar, ByVal x As Long) As Long
    
  Dim s0 As String
  Dim xx As Long
    
  '-- Always Put This In Callback Routines!
  On Error Resume Next
    
  s0 = ""
    
  '-- Get Zip32.DLL Message For processing
  For xx = 0 To x
    If fname.ch(xx) = 0 Then
      Exit For
    Else
      s0 = s0 + Chr(fname.ch(xx))
    End If
  Next
    
  '----------------------------------------------
  '-- This Is Where The DLL Passes Back Messages
  '-- To You! You Can Change The Message Printing
  '-- Below Here!
  '----------------------------------------------
  
  '-- Display Zip File Information
  '-- zZipInfo = zZipInfo & s0
  Form1.Print s0;
    
  DoEvents
    
  ZDLLPrnt = 0

End Function

'-- Callback For ZIP32z64.DLL - DLL Service Function
Public Function ZDLLServ(ByRef mname As ZipCBChar, _
                         ByVal LowSize As Long, _
                         ByVal HighSize As Long) As Long

    Dim s0 As String
    Dim xx As Long
    Dim FS As Currency  ' for large file sizes
    
    '-- Always Put This In Callback Routines!
    On Error Resume Next
    
    FS = (HighSize * &H10000 * &H10000) + LowSize
 '   Form1.Print "ZDLLServ returned File Size High " & HighSize & _
 '               " Low " & LowSize & " = " & FS & " bytes"
    
    s0 = ""
    '-- Get Zip32.DLL Message For processing
    For xx = 0 To 4096 ' x
    If mname.ch(xx) = 0 Then
        Exit For
    Else
        s0 = s0 + Chr(mname.ch(xx))
    End If
    Next
    ' At this point, s0 contains the message passed from the DLL
    ' It is up to the developer to code something useful here :)
    ZDLLServ = 0 ' Setting this to 1 will abort the zip!
    
End Function

'-- Callback For ZIP32z64.DLL - DLL Password Function
Public Function ZDLLPass(ByRef p As ZipCBChar, _
  ByVal n As Long, ByRef m As ZipCBChar, _
  ByRef Name As ZipCBChar) As Integer
  
  Dim filename   As String
  Dim prompt     As String
  Dim xx         As Integer
  Dim szpassword As String
  
  '-- Always Put This In Callback Routines!
  On Error Resume Next
    
  ZDLLPass = 1
  
  '-- User Entered A Password So Proccess It
  
  '-- Enter or Verify
  For xx = 0 To 255
    If m.ch(xx) = 0 Then
      Exit For
    Else
      prompt = prompt & Chr(m.ch(xx))
    End If
  Next
  
  '-- If There Is A Password Have The User Enter It!
  '-- This Can Be Changed
  
  '-- Now skip asking if default password set
  If EncryptionPassword <> "" Then
    szpassword = EncryptionPassword
  Else
    szpassword = InputBox("Please Enter The Password!", prompt)
  End If
  
  '-- The User Did Not Enter A Password So Exit The Function
  If szpassword = "" Then Exit Function
  
  For xx = 0 To n - 1
    p.ch(xx) = 0
  Next
  
  For xx = 0 To Len(szpassword) - 1
    p.ch(xx) = Asc(Mid(szpassword, xx + 1, 1))
  Next
  
  p.ch(xx) = Chr(0) ' Put Null Terminator For C
  
  ZDLLPass = 0
    
End Function

'-- Callback For ZIP32z64.DLL - DLL Comment Function
Public Function ZDLLComm(ByRef s1 As ZipCBChar) As Integer
    
    Dim comment As String
    Dim xx%, szcomment$
    
    '-- Always Put This In Callback Routines!
    On Error Resume Next
    
    ZDLLComm = 1
    If Not IsEmpty(ArchiveCommentText) Then
      ' use text given to SetZipOptions
      szcomment = ArchiveCommentText
    Else
      For xx = 0 To 4095
        szcomment = szcomment & Chr(s1.ch(xx))
        If s1.ch(xx) = 0 Then
          Exit For
        End If
      Next
      comment = InputBox("Enter or edit the comment", Default:=szcomment)
      If comment = "" Then
        ' either empty comment or Cancel button
        If MsgBox("Remove comment?" & Chr(13) & "Hit No to keep existing comment", vbYesNo) = vbYes Then
            szcomment = comment
        Else
          Exit Function
        End If
      End If
      szcomment = comment
    End If
    'If szcomment = "" Then Exit Function
    For xx = 0 To Len(szcomment) - 1
        s1.ch(xx) = Asc(Mid$(szcomment, xx + 1, 1))
    Next xx
    s1.ch(xx) = 0 ' Put null terminator for C

End Function

' This function can be used to set options in VB
Public Function SetZipOptions(ByRef ZipOpts As ZpOpt, _
  Optional ByVal ZipMode As ZipModeType = Add, _
  Optional ByVal RootDirToZipFrom As String = "", _
  Optional ByVal CompressionLevel As CompressionLevelType = c6_Default, _
  Optional ByVal RecurseSubdirectories As RecurseType = NoRecurse, _
  Optional ByVal Verboseness As VerbosenessType = Normal, _
  Optional ByVal i_IncludeFiles As String = "", _
  Optional ByVal x_ExcludeFiles As String = "", _
  Optional ByVal UpdateSFXOffsets As Boolean = False, Optional ByVal JunkDirNames As Boolean = False, _
  Optional ByVal Encrypt As Boolean = False, Optional ByVal Password As String = "", _
  Optional ByVal Repair As RepairType = NoRepair, Optional ByVal NoDirEntries As Boolean = False, _
  Optional ByVal GrowExistingArchive As Boolean = False, _
  Optional ByVal JunkSFXPrefix As Boolean = False, Optional ByVal ForceUseOfDOSNames As Boolean = False, _
  Optional ByVal Translate_LF As Translate_LF_Type = No_Line_End_Trans, _
  Optional ByVal Move_DeleteAfterAddedOrUpdated As Boolean = False, _
  Optional ByVal SetZipTimeToLatestTime As Boolean = False, _
  Optional ByVal IncludeSystemAndHiddenFiles As Boolean = False, _
  Optional ByVal ExcludeEarlierThanDate As String = "", _
  Optional ByVal IncludeEarlierThanDate As String = "", _
  Optional ByVal IncludeVolumeLabel As Boolean = False, _
  Optional ByVal ArchiveComment As Boolean = False, _
  Optional ByVal ArchiveCommentTextString = Empty, _
  Optional ByVal UsePrivileges As Boolean = False, _
  Optional ByVal ExcludeExtraAttributes As Boolean = False, Optional ByVal SplitSize As String = "", _
  Optional ByVal TempDirPath As String = "") As Boolean

  Dim SplitNum As Long
  Dim SplitMultS As String
  Dim SplitMult As Long
  
  ' set some defaults
  ZipOpts.date = vbNullString
  ZipOpts.szRootDir = vbNullString
  ZipOpts.szTempDir = vbNullString
  ZipOpts.fTemp = 0
  ZipOpts.fSuffix = 0
  ZipOpts.fEncrypt = 0
  ZipOpts.fSystem = 0
  ZipOpts.fVolume = 0
  ZipOpts.fExtra = 0
  ZipOpts.fNoDirEntries = 0
  ZipOpts.fExcludeDate = 0
  ZipOpts.fIncludeDate = 0
  ZipOpts.fVerbose = 0
  ZipOpts.fQuiet = 0
  ZipOpts.fCRLF_LF = 0
  ZipOpts.fLF_CRLF = 0
  ZipOpts.fJunkDir = 0
  ZipOpts.fGrow = 0
  ZipOpts.fForce = 0
  ZipOpts.fMove = 0
  ZipOpts.fDeleteEntries = 0
  ZipOpts.fUpdate = 0
  ZipOpts.fFreshen = 0
  ZipOpts.fJunkSFX = 0
  ZipOpts.fLatestTime = 0
  ZipOpts.fComment = 0
  ZipOpts.fOffsets = 0
  ZipOpts.fPrivilege = 0
  ZipOpts.szSplitSize = vbNullString
  ZipOpts.IncludeListCount = 0
  ZipOpts.szIncludeList = vbNullString
  ZipOpts.ExcludeListCount = 0
  ZipOpts.szExcludeList = vbNullString
  ZipOpts.fRecurse = 0
  ZipOpts.fRepair = 0
  ZipOpts.flevel = 0
  
  If RootDirToZipFrom <> "" Then
    ZipOpts.szRootDir = RootDirToZipFrom
  End If
  ZipOpts.flevel = Asc(CompressionLevel)
  If UpdateSFXOffsets Then ZipOpts.fOffsets = 1
  
  If i_IncludeFiles <> "" Then
    ZipOpts.szIncludeList = i_IncludeFiles
  End If
  If x_ExcludeFiles <> "" Then
    ZipOpts.szExcludeList = x_ExcludeFiles
  End If
  
  If ZipMode = Add Then
    ' default
  ElseIf ZipMode = Delete Then
    ZipOpts.fDeleteEntries = 1
  ElseIf ZipMode = Update Then
    ZipOpts.fUpdate = 1
  Else
    ZipOpts.fFreshen = 1
  End If
  ZipOpts.fRepair = Repair
  If GrowExistingArchive Then ZipOpts.fGrow = 1
  If Move_DeleteAfterAddedOrUpdated Then ZipOpts.fMove = 1
    
  If Verboseness = Quiet Then
    ZipOpts.fQuiet = 1
  ElseIf Verboseness = Verbose Then
    ZipOpts.fVerbose = 1
  End If
  
  If ArchiveComment = False And Not IsEmpty(ArchiveCommentTextString) Then
    MsgBox "Must set ArchiveComment = True to set ArchiveCommentTextString"
    Exit Function
  End If
  If IsEmpty(ArchiveCommentTextString) Then
    ArchiveCommentText = Empty
  Else
    ArchiveCommentText = ArchiveCommentTextString
  End If
  If ArchiveComment Then ZipOpts.fComment = 1
  
  If NoDirEntries Then ZipOpts.fNoDirEntries = 1
  If JunkDirNames Then ZipOpts.fJunkDir = 1
  If Encrypt Then ZipOpts.fEncrypt = 1
  EncryptionPassword = Password
  If JunkSFXPrefix Then ZipOpts.fJunkSFX = 1
  If ForceUseOfDOSNames Then ZipOpts.fForce = 1
  If Translate_LF = LF_To_CRLF Then ZipOpts.fLF_CRLF = 1
  If Translate_LF = CRLF_To_LF Then ZipOpts.fCRLF_LF = 1
  ZipOpts.fRecurse = RecurseSubdirectories
  If IncludeSystemAndHiddenFiles Then ZipOpts.fSystem = 1
  
  If SetZipTimeToLatestTime Then ZipOpts.fLatestTime = 1
  If ExcludeEarlierThanDate <> "" And IncludeEarlierThanDate <> "" Then
    MsgBox "Both ExcludeEarlierThanDate and IncludeEarlierThanDate not " & Chr(10) & _
           "supported at same time"
    Exit Function
  End If
  If ExcludeEarlierThanDate <> "" Then
    ZipOpts.fIncludeDate = 1
    ZipOpts.date = ExcludeEarlierThanDate
  End If
  If IncludeEarlierThanDate <> "" Then
    ZipOpts.fExcludeDate = 1
    ZipOpts.date = IncludeEarlierThanDate
  End If
  
  If TempDirPath <> "" Then
    ZipOpts.szTempDir = TempDirPath
    ZipOpts.fTemp = 1
  End If
  
  If SplitSize <> "" Then
    SplitSize = Trim(SplitSize)
    SplitMultS = Right(SplitSize, 1)
    SplitMultS = UCase(SplitMultS)
    If (SplitMultS = "K") Then
        SplitMult = 1024
        SplitNum = Val(Left(SplitSize, Len(SplitSize) - 1))
    ElseIf SplitMultS = "M" Then
        SplitMult = 1024 * 1024&
        SplitNum = Val(Left(SplitSize, Len(SplitSize) - 1))
    ElseIf SplitMultS = "G" Then
        SplitMult = 1024 * 1024 * 1024&
        SplitNum = Val(Left(SplitSize, Len(SplitSize) - 1))
    Else
        SplitMult = 1024 * 1024&
        SplitNum = Val(SplitSize)
    End If
    SplitNum = SplitNum * SplitMult
    If SplitNum = 0 Then
        MsgBox "SplitSize of 0 not supported"
        Exit Function
    ElseIf SplitNum < 64 * 1024& Then
        MsgBox "SplitSize must be at least 64k"
        Exit Function
    End If
    ZipOpts.szSplitSize = SplitSize
  End If
  
  If IncludeVolumeLabel Then ZipOpts.fVolume = 1
  If UsePrivileges Then ZipOpts.fPrivilege = 1
  If ExcludeExtraAttributes Then ZipOpts.fExtra = 1
  
  SetZipOptions = True
    
End Function

Function ChopNulls(ByVal Str) As String
  Dim A As Integer
  Dim C As String
    
  For A = 1 To Len(Str)
    If Mid(Str, A, 1) = Chr(0) Then
      ChopNulls = Left(Str, A - 1)
      Exit Function
    End If
  Next
  ChopNulls = Str
    
End Function
Sub DisplayVersion()
  
  ' display version of DLL
  Dim Beta As Boolean
  Dim ZLIB As Boolean
  Dim Zip64 As Boolean
  Dim Flags As String
  Dim A As Integer
  
  ZipVersion.structlen = Len(ZipVersion)
  ZpVersion ZipVersion
  ' Check flag
  If ZipVersion.flag And 1 Then
    Flags = Flags & " Beta,"
    Beta = True
  Else
    Flags = Flags & " No Beta,"
  End If
  If ZipVersion.flag And 2 Then
    Flags = Flags & " ZLIB,"
    ZLIB = True
  Else
    Flags = Flags & " No ZLIB,"
  End If
  If ZipVersion.flag And 4 Then
    Flags = Flags & " Zip64, "
    Zip64 = True
  Else
    Flags = Flags & " No Zip64, "
  End If
  If ZipVersion.encryption Then
    Flags = Flags & "Encryption"
  Else
    Flags = Flags & " No encryption"
  End If
  
  Form1.Caption = "Using Zip32z64.DLL Version " & _
                  ZipVersion.ZipVersion.Major & "." & ZipVersion.ZipVersion.Minor & " " & _
                  ChopNulls(ZipVersion.Beta) & "  [" & ChopNulls(ZipVersion.date) & "]" & _
                  " - FLAGS: " & Flags

  If Not Zip64 Then
    A = MsgBox("Zip32z64.dll not compiled with Zip64 enabled - continue?", _
               vbOKCancel, _
               "Wrong dll")
    If A = vbCancel Then
        End
    End If
  End If
  
End Sub

'-- Main ZIP32.DLL Subroutine.
'-- This Is Where It All Happens!!!
'--
'-- (WARNING!) Do Not Change This Function!!!
'--
Public Function VBZip32() As Long
    
  Dim retcode As Long
  Dim FileNotFound As Boolean
    
  ' On Error Resume Next '-- Nothing Will Go Wrong :-)
  On Error GoTo ZipError
    
  retcode = 0
    
  '-- Set Address Of ZIP32.DLL Callback Functions
  '-- (WARNING!) Do Not Change!!! (except as noted below)
  ZUSER.ZDLLPrnt = FnPtr(AddressOf ZDLLPrnt)
  ZUSER.ZDLLPASSWORD = FnPtr(AddressOf ZDLLPass)
  ZUSER.ZDLLCOMMENT = FnPtr(AddressOf ZDLLComm)
  ZUSER.ZDLLSERVICE_NO_INT64 = FnPtr(AddressOf ZDLLServ)
  
  ' If you need to set destination of each split set this
  'ZUSER.ZDLLSPLIT = FnPtr(AddressOf ZDLLSplitSelect)

  '-- Set ZIP32.DLL Callbacks - return 1 if DLL loaded 0 if not
  retcode = ZpInit(ZUSER)
  If retcode = 0 And FileNotFound Then
    MsgBox "Probably could not find Zip32z64.DLL - have you copied" & Chr(10) & _
           "it to the System directory, your program directory, " & Chr(10) & _
           "or a directory on your command PATH?"
    VBZip32 = retcode
    Exit Function
  End If
  
  DisplayVersion
    
  If strZipFileNames = "" Then
    ' not using string of names to zip (so using array of names)
    strZipFileNames = vbNullString
  End If
  
  '-- Go Zip It Them Up!
  retcode = ZpArchive(zArgc, zZipArchiveName, zZipFileNames, strZipFileNames, ZOPT)
  
  '-- Return The Function Code
  VBZip32 = retcode

  Exit Function

ZipError:
  MsgBox "Error:  " & Err.Description
  If Err = 48 Then
    FileNotFound = True
  End If
  Resume Next

End Function

