VERSION 5.00
Begin VB.Form Form1 
   AutoRedraw      =   -1  'True
   Caption         =   "Form1"
   ClientHeight    =   3150
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   6570
   BeginProperty Font 
      Name            =   "MS Sans Serif"
      Size            =   9.75
      Charset         =   0
      Weight          =   700
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   LinkTopic       =   "Form1"
   ScaleHeight     =   3150
   ScaleWidth      =   6570
   StartUpPosition =   1  'CenterOwner
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Option Explicit

'---------------------------------------------------------------
'-- Sample VB 6 code to drive zip32z64.dll
'--
'-- Based on code contributed to the Info-ZIP project by Mike Le Voi
'--
'-- See the Original VB example in a separate directory for
'-- more information
'--
'-- Use this code at your own risk. Nothing implied or warranted
'-- to work on your machine :-)
'---------------------------------------------------------------
'--
'-- The Source Code Is Freely Available From Info-ZIP At:
'--   ftp://ftp.info-zip.org/pub/infozip/infozip.html
'-- and
'--   http://sourceforge.net/projects/infozip/
'--
'-- A Very Special Thanks To Mr. Mike Le Voi
'-- And Mr. Mike White Of The Info-ZIP project
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

'---------------------------------------------------------------
' Zip32z64.dll is the new Zip32.dll based on Zip 3.0 and compiled
' with Zip64 support enabled.  See windll.txt in the windll directory
' for more on Zip32z64 and the comments in VBZipBas.bas.
'
' Contact Info-Zip if problems.  This code is
' provided under the Info-Zip license.
'
' 4/24/2004, 12/4/2007 EG
'---------------------------------------------------------------

Private Sub Form_Click()

  Dim retcode As Integer  ' For Return Code From ZIP32.DLL
  Dim iFiles As String
  Dim FilesToZip() As String
  Dim i As Long

  Cls

  '-- Set Options - Only The Common Ones Are Shown Here
  '-- These Must Be Set Before Calling The VBZip32 Function
  
  ' In VB 6 you can see the list of possible options and the defaults
  ' by adding a space between any parameters which should make the tip box
  ' appear.  Delete a := and retype to see a list of choices.

  ' Be warned:  There are bugs in the previous dll.  See the Original VB
  ' example in the VB directory for details.
  
  If Not SetZipOptions(ZOPT, _
                       ZipMode:=Add, _
                       CompressionLevel:=c6_Default) Then
           ' Some additional options ...
           '            RootDirToZipFrom:="", _
           '   strip paths and just store names:
           '            JunkDirNames:=False, _
           '   do not store entries for the directories themselves:
           '            NoDirEntries:=True _
           '   include files only if match one of these patterns:
           '            i_IncludeFiles:="*.vbp *.frm", _
           '   exclude files that match these patterns:
           '            x_ExcludeFiles:="*.bas", _
           '            Verboseness:=Verbose, _
           '            IncludeEarlierThanDate:="2004-4-1", _
           '            RecurseSubdirectories:=r_RecurseIntoSubdirectories, _
           '            Encrypt:=False, _
           '            ArchiveComment:=False
           ' date example (format mmddyyyy or yyyy-mm-dd):
           '           ExcludeEarlierThanDate:="2002-12-10", _
           ' split example (can only create, can't update split archives in VB):
           '            SplitSize:="110k", _
' Delete
 ' If Not SetZipOptions(ZOPT, _
 '                      ZipMode:=Delete) Then
 
    ' a problem if get here - error message already displayed so just exit
    Exit Sub
  End If
  

  '-- Select Some Files - Wildcards Are Supported
  '-- Change The Paths Here To Your Directory
  '-- And Files!!!

  ' default to current (VB project) directory and zip up project files
  zZipArchiveName = "MyFirst.zip"
  
  
  ' Files to zip - use one of below
  
  '---------------
  ' Example using file name array
  
  ' Store the file paths
  ' Change Dim of zFiles at top of VBZipBas.bas if more than 100 files
  ' See note at top of VBZipBas.bas for limit on number of files
  
'  zArgc = 2           ' Number Of file paths below
'  zZipFileNames.zFiles(1) = "*.bas"
'  zZipFileNames.zFiles(2) = "*.frm"
  
  '---------------
  ' Example using file list string
  
  ' List of files to zip as string of names with space between
  ' Set zArgc = 0 as not using array
  ' Using string for file list avoids above array limit
  
  zArgc = 0
'  ReDim FilesToZip(1)      ' dim to number of files below
'  FilesToZip(1) = "x:*.*"
  ReDim FilesToZip(2)      ' dim to number of files below
  FilesToZip(1) = "*.bas"
  FilesToZip(2) = "*.frm"
 
  ' Build string of file names
  ' Best to put double quotes around each in case of spaces
  strZipFileNames = ""
  For i = 1 To UBound(FilesToZip)
    strZipFileNames = strZipFileNames & """" & FilesToZip(i) & """ "
  Next
  '---------------
  
  '-- Go Zip Them Up!
  retcode = VBZip32()

  '-- Display The Returned Code Or Error!
  Print "Return code:" & Str(retcode)

End Sub

Private Sub Form_Load()

  Me.Show

  Print "Click me!"

End Sub
