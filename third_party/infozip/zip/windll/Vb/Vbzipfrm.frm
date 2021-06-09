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
' This is the original VB example (with some changes) for use
' with Zip32.dll (Zip 2.31) but not Zip32z64.dll (Zip 3.0).
'
' Minor changes to use current directory and VB project files
' for the example and to turn off encryption.
'
' The VB example provided with Zip 3.0 is more extensive.  Even
' if you plan to use the updated zip32.dll instead of the new
' zip32z64.dll (Zip 3.0), there may be some things you might find
' useful in the VB example there.
'
' 2/27/2005 Ed Gordon
'---------------------------------------------------------------

Private Sub Form_Click()

  Dim retcode As Integer  ' For Return Code From ZIP32.DLL

  Cls

  '-- Set Options - Only The Common Ones Are Shown Here
  '-- These Must Be Set Before Calling The VBZip32 Function
  zDate = vbNullString
  'zDate = "2005-1-31"
  'zExcludeDate = 1
  'zIncludeDate = 0
  zJunkDir = 0     ' 1 = Throw Away Path Names
  zRecurse = 0     ' 1 = Recurse -r 2 = Recurse -R 2 = Most Useful :)
  zUpdate = 0      ' 1 = Update Only If Newer
  zFreshen = 0     ' 1 = Freshen - Overwrite Only
  zLevel = Asc(9)  ' Compression Level (0 - 9)
  zEncrypt = 0     ' Encryption = 1 For Password Else 0
  zComment = 0     ' Comment = 1 if required

  '-- Select Some Files - Wildcards Are Supported
  '-- Change The Paths Here To Your Directory
  '-- And Files!!!
  ' Change ZIPnames in VBZipBas.bas if need more than 100 files
  zArgc = 2           ' Number Of Elements Of mynames Array
  zZipFileName = "MyFirst.zip"
  zZipFileNames.zFiles(0) = "vbzipfrm.frm"
  zZipFileNames.zFiles(1) = "vbzip.vbp"
  zRootDir = ""    ' This Affects The Stored Path Name
  
  ' Older versions of Zip32.dll do not handle setting
  ' zRootDir to anything other than "".  If you need to
  ' change root directory an alternative is to just change
  ' directory.  This requires Zip32.dll to be on the command
  ' path.  This should be fixed in Zip 2.31.  1/31/2005 EG

  ' ChDir "a"

  '-- Go Zip Them Up!
  retcode = VBZip32

  '-- Display The Returned Code Or Error!
  Print "Return code:" & Str(retcode)

End Sub

Private Sub Form_Load()

  Me.Show

  Print "Click me!"

End Sub
