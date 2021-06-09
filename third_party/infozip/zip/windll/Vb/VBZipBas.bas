Attribute VB_Name = "VBZipBas"

Option Explicit

'---------------------------------------------------------------
'-- Please Do Not Remove These Comments!!!
'---------------------------------------------------------------
'-- Sample VB 5 code to drive zip32.dll
'-- Contributed to the Info-ZIP project by Mike Le Voi
'--
'-- Contact me at: mlevoi@modemss.brisnet.org.au
'--
'-- Visit my home page at: http://modemss.brisnet.org.au/~mlevoi
'--
'-- Use this code at your own risk. Nothing implied or warranted
'-- to work on your machine :-)
'---------------------------------------------------------------
'--
'-- The Source Code Is Freely Available From Info-ZIP At:
'-- http://www.cdrom.com/pub/infozip/infozip.html
'--
'-- A Very Special Thanks To Mr. Mike Le Voi
'-- And Mr. Mike White Of The Info-ZIP
'-- For Letting Me Use And Modify His Orginal
'-- Visual Basic 5.0 Code! Thank You Mike Le Voi.
'---------------------------------------------------------------
'--
'-- Contributed To The Info-ZIP Project By Raymond L. King
'-- Modified June 21, 1998
'-- By Raymond L. King
'-- Custom Software Designers
'--
'-- Contact Me At: king@ntplx.net
'-- ICQ 434355
'-- Or Visit Our Home Page At: http://www.ntplx.net/~king
'--
'---------------------------------------------------------------
'
' This is the original example with some small changes.  Only
' use with the original Zip32.dll (compiled from Zip 2.31 or
' later).  Do not use this VB example with Zip32z64.dll
' (compiled from Zip 3.0).  To check the version of a dll,
' right click on the file and check properties.
'
' 6/24/2008 Ed Gordon

'---------------------------------------------------------------
' Usage notes:
'
' This code uses Zip32.dll.  You DO NOT need to register the
' DLL to use it.  You also DO NOT need to reference it in your
' VB project.  You DO have to copy the DLL to your SYSTEM
' directory, your VB project directory, or place it in a directory
' on your command PATH.
'
' A bug has been found in the Zip32.dll when called from VB.  If
' you try to pass any values other than NULL in the ZPOPT strings
' Date, szRootDir, or szTempDir they get converted from the
' VB internal wide character format to temporary byte strings by
' the calling interface as they are supposed to.  However when
' ZpSetOptions returns the passed strings are deallocated unless the
' VB debugger prevents it by a break between ZpSetOptions and
' ZpArchive.  When Zip32.dll uses these pointers later it
' can result in unpredictable behavior.  A kluge is available
' for Zip32.dll, just replacing api.c in Zip 2.3, but better to just
' use the new Zip32z64.dll where these bugs are fixed.  However,
' the kluge has been added to Zip 2.31 and later and these are
' now stable.  To determine the version of the dll you have
' right click on it, select the Version tab, and verify the
' Product Version is at least 2.31.
'
' Another bug is where -R is used with some other options and can
' crash the dll.  This is a bug in how zip processes the command
' line and should be mostly fixed in Zip 2.31.  If you run into
' problems try using -r instead for recursion.  The bug is fixed
' in Zip 3.0 but note that Zip 3.0 creates dll zip32z64.dll and
' it is not compatible with older VB including this example.  See
' the new VB example code included with Zip 3.0 for calling
' interface changes.
'
' Note that Zip32 is probably not thread safe.  It may be made
' thread safe in a later version, but for now only one thread in
' one program should use the DLL at a time.  Unlike Zip, UnZip is
' probably thread safe, but an exception to this has been
' found.  See the UnZip documentation for the latest on this.
'
' All code in this VB project is provided under the Info-Zip license.
'
' If you have any questions please contact Info-Zip at
' http://www.info-zip.org.
'
' 4/29/2004 EG (Updated 3/1/2005, 6/24/2008 EG)
'
'---------------------------------------------------------------


'-- C Style argv
'-- Holds The Zip Archive Filenames
' Max for this just over 8000 as each pointer takes up 4 bytes and
' VB only allows 32 kB of local variables and that includes function
' parameters.  - 3/19/2004 EG
'
Public Type ZIPnames
  zFiles(0 To 99) As String
End Type

'-- Call Back "String"
Public Type ZipCBChar
  ch(4096) As Byte
End Type

'-- ZPOPT Is Used To Set The Options In The ZIP32.DLL
Public Type ZPOPT
  Date           As String ' US Date (8 Bytes Long) "12/31/98"?
  szRootDir      As String ' Root Directory Pathname (Up To 256 Bytes Long)
  szTempDir      As String ' Temp Directory Pathname (Up To 256 Bytes Long)
  fTemp          As Long   ' 1 If Temp dir Wanted, Else 0
  fSuffix        As Long   ' Include Suffixes (Not Yet Implemented!)
  fEncrypt       As Long   ' 1 If Encryption Wanted, Else 0
  fSystem        As Long   ' 1 To Include System/Hidden Files, Else 0
  fVolume        As Long   ' 1 If Storing Volume Label, Else 0
  fExtra         As Long   ' 1 If Excluding Extra Attributes, Else 0
  fNoDirEntries  As Long   ' 1 If Ignoring Directory Entries, Else 0
  fExcludeDate   As Long   ' 1 If Excluding Files Earlier Than Specified Date, Else 0
  fIncludeDate   As Long   ' 1 If Including Files Earlier Than Specified Date, Else 0
  fVerbose       As Long   ' 1 If Full Messages Wanted, Else 0
  fQuiet         As Long   ' 1 If Minimum Messages Wanted, Else 0
  fCRLF_LF       As Long   ' 1 If Translate CR/LF To LF, Else 0
  fLF_CRLF       As Long   ' 1 If Translate LF To CR/LF, Else 0
  fJunkDir       As Long   ' 1 If Junking Directory Names, Else 0
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
  fRecurse       As Long   ' 1 (-r), 2 (-R) If Recursing Into Sub-Directories, Else 0
  fRepair        As Long   ' 1 = Fix Archive, 2 = Try Harder To Fix, Else 0
  flevel         As Byte   ' Compression Level - 0 = Stored 6 = Default 9 = Max
End Type

'-- This Structure Is Used For The ZIP32.DLL Function Callbacks
Public Type ZIPUSERFUNCTIONS
  ZDLLPrnt     As Long        ' Callback ZIP32.DLL Print Function
  ZDLLCOMMENT  As Long        ' Callback ZIP32.DLL Comment Function
  ZDLLPASSWORD As Long        ' Callback ZIP32.DLL Password Function
  ZDLLSERVICE  As Long        ' Callback ZIP32.DLL Service Function
End Type

'-- Local Declarations
Public ZOPT  As ZPOPT
Public ZUSER As ZIPUSERFUNCTIONS

'-- This Assumes ZIP32.DLL Is In Your \Windows\System Directory!
'-- (alternatively, a copy of ZIP32.DLL needs to be located in the program
'-- directory or in some other directory listed in PATH.)
Private Declare Function ZpInit Lib "zip32.dll" _
  (ByRef Zipfun As ZIPUSERFUNCTIONS) As Long '-- Set Zip Callbacks

Private Declare Function ZpSetOptions Lib "zip32.dll" _
  (ByRef Opts As ZPOPT) As Long '-- Set Zip Options

Private Declare Function ZpGetOptions Lib "zip32.dll" _
  () As ZPOPT '-- Used To Check Encryption Flag Only

Private Declare Function ZpArchive Lib "zip32.dll" _
  (ByVal argc As Long, ByVal funame As String, _
   ByRef argv As ZIPnames) As Long '-- Real Zipping Action

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
Public zDate         As String
Public zRootDir      As String
Public zTempDir      As String
Public zSuffix       As Integer
Public zEncrypt      As Integer
Public zSystem       As Integer
Public zVolume       As Integer
Public zExtra        As Integer
Public zNoDirEntries As Integer
Public zExcludeDate  As Integer
Public zIncludeDate  As Integer
Public zVerbose      As Integer
Public zQuiet        As Integer
Public zCRLF_LF      As Integer
Public zLF_CRLF      As Integer
Public zJunkDir      As Integer
Public zRecurse      As Integer
Public zGrow         As Integer
Public zForce        As Integer
Public zMove         As Integer
Public zDelEntries   As Integer
Public zUpdate       As Integer
Public zFreshen      As Integer
Public zJunkSFX      As Integer
Public zLatestTime   As Integer
Public zComment      As Integer
Public zOffsets      As Integer
Public zPrivilege    As Integer
Public zEncryption   As Integer
Public zRepair       As Integer
Public zLevel        As Integer

'-- Public Program Variables
Public zArgc         As Integer     ' Number Of Files To Zip Up
Public zZipFileName  As String      ' The Zip File Name ie: Myzip.zip
Public zZipFileNames As ZIPnames    ' File Names To Zip Up
Public zZipInfo      As String      ' Holds The Zip File Information

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

'-- These Functions Are For The ZIP32.DLL
'--
'-- Puts A Function Pointer In A Structure
'-- For Use With Callbacks...
Public Function FnPtr(ByVal lp As Long) As Long
    
  FnPtr = lp

End Function

'-- Callback For ZIP32.DLL - DLL Print Function
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

'-- Callback For ZIP32.DLL - DLL Service Function
Public Function ZDLLServ(ByRef mname As ZipCBChar, ByVal x As Long) As Long

    ' x is the size of the file
    
    Dim s0 As String
    Dim xx As Long
    
    '-- Always Put This In Callback Routines!
    On Error Resume Next
    
    s0 = ""
    '-- Get Zip32.DLL Message For processing
    For xx = 0 To 4096
    If mname.ch(xx) = 0 Then
        Exit For
    Else
        s0 = s0 + Chr(mname.ch(xx))
    End If
    Next
    ' Form1.Print "-- " & s0 & " - " & x & " bytes"
    
    ' This is called for each zip entry.
    ' mname is usually the null terminated file name and x the file size.
    ' s0 has trimmed file name as VB string.

    ' At this point, s0 contains the message passed from the DLL
    ' It is up to the developer to code something useful here :)
    ZDLLServ = 0 ' Setting this to 1 will abort the zip!
    
End Function

'-- Callback For ZIP32.DLL - DLL Password Function
Public Function ZDLLPass(ByRef p As ZipCBChar, _
  ByVal n As Long, ByRef m As ZipCBChar, _
  ByRef Name As ZipCBChar) As Integer
  
  Dim prompt     As String
  Dim xx         As Integer
  Dim szpassword As String
  
  '-- Always Put This In Callback Routines!
  On Error Resume Next
    
  ZDLLPass = 1
  
  '-- If There Is A Password Have The User Enter It!
  '-- This Can Be Changed
  szpassword = InputBox("Please Enter The Password!")
  
  '-- The User Did Not Enter A Password So Exit The Function
  If szpassword = "" Then Exit Function
  
  '-- User Entered A Password So Proccess It
  For xx = 0 To 255
    If m.ch(xx) = 0 Then
      Exit For
    Else
      prompt = prompt & Chr(m.ch(xx))
    End If
  Next
  
  For xx = 0 To n - 1
    p.ch(xx) = 0
  Next
  
  For xx = 0 To Len(szpassword) - 1
    p.ch(xx) = Asc(Mid(szpassword, xx + 1, 1))
  Next
  
  p.ch(xx) = Chr(0) ' Put Null Terminator For C
  
  ZDLLPass = 0
    
End Function

'-- Callback For ZIP32.DLL - DLL Comment Function
Public Function ZDLLComm(ByRef s1 As ZipCBChar) As Integer
    
    Dim xx%, szcomment$
    
    '-- Always Put This In Callback Routines!
    On Error Resume Next
    
    ZDLLComm = 1
    szcomment = InputBox("Enter the comment")
    If szcomment = "" Then Exit Function
    For xx = 0 To Len(szcomment) - 1
        s1.ch(xx) = Asc(Mid$(szcomment, xx + 1, 1))
    Next xx
    s1.ch(xx) = Chr(0) ' Put null terminator for C

End Function

'-- Main ZIP32.DLL Subroutine.
'-- This Is Where It All Happens!!!
'--
'-- (WARNING!) Do Not Change This Function!!!
'--
Public Function VBZip32() As Long
    
  Dim retcode As Long
    
  On Error Resume Next '-- Nothing Will Go Wrong :-)
    
  retcode = 0
    
  '-- Set Address Of ZIP32.DLL Callback Functions
  '-- (WARNING!) Do Not Change!!!
  ZUSER.ZDLLPrnt = FnPtr(AddressOf ZDLLPrnt)
  ZUSER.ZDLLPASSWORD = FnPtr(AddressOf ZDLLPass)
  ZUSER.ZDLLCOMMENT = FnPtr(AddressOf ZDLLComm)
  ZUSER.ZDLLSERVICE = FnPtr(AddressOf ZDLLServ)
    
  '-- Set ZIP32.DLL Callbacks
  retcode = ZpInit(ZUSER)
  If retcode = 0 Then
    MsgBox "Zip32.dll did not initialize.  Is it in the current directory " & _
                "or on the command path?", vbOKOnly, "VB Zip"
    Exit Function
  End If
    
  '-- Setup ZIP32 Options
  '-- (WARNING!) Do Not Change!
  ZOPT.Date = zDate                  ' "12/31/79"? US Date?
  ZOPT.szRootDir = zRootDir          ' Root Directory Pathname
  ZOPT.szTempDir = zTempDir          ' Temp Directory Pathname
  ZOPT.fSuffix = zSuffix             ' Include Suffixes (Not Yet Implemented)
  ZOPT.fEncrypt = zEncrypt           ' 1 If Encryption Wanted
  ZOPT.fSystem = zSystem             ' 1 To Include System/Hidden Files
  ZOPT.fVolume = zVolume             ' 1 If Storing Volume Label
  ZOPT.fExtra = zExtra               ' 1 If Including Extra Attributes
  ZOPT.fNoDirEntries = zNoDirEntries ' 1 If Ignoring Directory Entries
  ZOPT.fExcludeDate = zExcludeDate   ' 1 If Excluding Files Earlier Than A Specified Date
  ZOPT.fIncludeDate = zIncludeDate   ' 1 If Including Files Earlier Than A Specified Date
  ZOPT.fVerbose = zVerbose           ' 1 If Full Messages Wanted
  ZOPT.fQuiet = zQuiet               ' 1 If Minimum Messages Wanted
  ZOPT.fCRLF_LF = zCRLF_LF           ' 1 If Translate CR/LF To LF
  ZOPT.fLF_CRLF = zLF_CRLF           ' 1 If Translate LF To CR/LF
  ZOPT.fJunkDir = zJunkDir           ' 1 If Junking Directory Names
  ZOPT.fGrow = zGrow                 ' 1 If Allow Appending To Zip File
  ZOPT.fForce = zForce               ' 1 If Making Entries Using DOS Names
  ZOPT.fMove = zMove                 ' 1 If Deleting Files Added Or Updated
  ZOPT.fDeleteEntries = zDelEntries  ' 1 If Files Passed Have To Be Deleted
  ZOPT.fUpdate = zUpdate             ' 1 If Updating Zip File-Overwrite Only If Newer
  ZOPT.fFreshen = zFreshen           ' 1 If Freshening Zip File-Overwrite Only
  ZOPT.fJunkSFX = zJunkSFX           ' 1 If Junking SFX Prefix
  ZOPT.fLatestTime = zLatestTime     ' 1 If Setting Zip File Time To Time Of Latest File In Archive
  ZOPT.fComment = zComment           ' 1 If Putting Comment In Zip File
  ZOPT.fOffsets = zOffsets           ' 1 If Updating Archive Offsets For SFX Files
  ZOPT.fPrivilege = zPrivilege       ' 1 If Not Saving Privelages
  ZOPT.fEncryption = zEncryption     ' Read Only Property!
  ZOPT.fRecurse = zRecurse           ' 1 or 2 If Recursing Into Subdirectories
  ZOPT.fRepair = zRepair             ' 1 = Fix Archive, 2 = Try Harder To Fix
  ZOPT.flevel = zLevel               ' Compression Level - (0 To 9) Should Be 0!!!
    
  '-- Set ZIP32.DLL Options
  retcode = ZpSetOptions(ZOPT)
    
  '-- Go Zip It Them Up!
  retcode = ZpArchive(zArgc, zZipFileName, zZipFileNames)
  
  '-- Return The Function Code
  VBZip32 = retcode

End Function

