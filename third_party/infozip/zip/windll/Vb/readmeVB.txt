On Windows open this file in WordPad.

Contents of the "windll/vb" sub-archive

This directory contains a Visual Basic project example for
using the zip32.dll library.  This project updates the Zip 2.3 VB
project example and includes some bug fixes and many additional notes
but is still compatible with zip32.dll.  See the comments in the form
and project files for details.  It has been tested on VB 5 and VB 6.

Zip 2.31 itself had bug fixes as well, including some related to the
dll, and you should now use a version of zip32.dll from that or later.
This dll includes a fix for the VB dll bug where Date, szRootDir, and
szTempDir were not passed in correctly and setting these to anything
but NULL could impact the dll and maybe crash it.  You can tell which
version you have by right clicking on zip32.dll in a file listing,
looking at properties, selecting the Version tab, and verifying the
Product Version is at least 2.31.

A new dll is available as part of this Zip 3.0 release and a
new VB project is included in the VBz64 directory.  This dll and
project supports Zip64 and large files but is not backward compatible
with Zip32.dll.  You will need the new zip32z64.dll to use this project,
which can be compiled from Zip 3.0.  See windll/VBz64 for details.

Note that the files may saved in unix format with carriage returns
stripped.  These must be restored before the project can be successfully
used.  This can be done by using the -a option to unzip.  Another way to
do this is to open each file in WordPad, select and cut a line, paste
the line back, and save the file.  This will force WordPad to format
the entire file.

Ed Gordon
2/2/2007
