!    VMS Error Message Source File for Zip
!
! Because the facility code was formally assigned by HP, the .FACILITY
! directive below specifies /SYSTEM.  Because the messages are, in
! general, specific to Zip, this file is not compiled with /SHARED.
! For example:
!
!    MESSAGE /OBJECT = [.dest]ZIP_MSG.OBJ /NOSYMBOLS [.VMS]ZIP_MSG.MSG
!
!    LINK /SHAREABLE = [.dest]ZIP_MSG.EXE [.dest]ZIP_MSG.OBJ
!
!-----------------------------------------------------------------------

.TITLE  Info-ZIP Zip Error Messages
.FACILITY IZ_ZIP, 1955 /SYSTEM
.IDENT 'V3.0-000'

.BASE 0
OK      /SUCCESS      <Normal successful completion>
.BASE 4
EOF     /FATAL        <Unexpected end of zip file>
.BASE 6
FORM    /ERROR        <Zip file structure invalid>
.BASE 8
MEM     /FATAL        <Out of memory>
.BASE 10
LOGIC   /FATAL        <Internal logic error>
.BASE 12
BIG     /ERROR        <Entry too big to split, read, or write>
.BASE 14
NOTE    /ERROR        <Invalid comment format>
.BASE 16
TEST    /FATAL        <Zip file invalid, could not spawn unzip, or wrong unzip>
.BASE 18
ABORT   /ERROR        <Interrupted>
.BASE 20
TEMP    /FATAL        <Temporary file failure>
.BASE 22
READ    /FATAL        <Input file read failure>
.BASE 24
NONE    /WARNING      <Nothing to do!>
.BASE 26
NAME    /ERROR        <Missing or empty zip file>
.BASE 28
WRITE   /FATAL        <Output file write failure>
.BASE 30
CREAT   /FATAL        <Could not create output file>
.BASE 32
PARMS   /ERROR        <Invalid command arguments>
.BASE 36
OPEN    /ERROR        <File not found or no read permission>
.BASE 38
COMPERR /FATAL        <Not supported>
.BASE 40
ZIP64   /FATAL        <Attempt to read unsupported Zip64 archive>

.END
