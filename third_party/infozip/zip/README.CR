_____________________________________________________________________________

  This is Info-ZIP's README.CR for zcrypt29.zip, last updated 27 March 2008.
_____________________________________________________________________________


The files described below contain the encryption/decryption code for Zip 2.31,
UnZip 5.52, and WiZ 5.02 (and later).  These files are included in the main
source distributions for all of these now, but the encryption patch is still
available for earlier versions of these.  This file both describes the history
of the encryption package and notes the current conditions for use.  Check
the comments at the top of crypt.c and crypt.h for additional information.

As of version 2.9, this encryption source code is copyrighted by Info-ZIP;
see the enclosed LICENSE file for details.  Older versions remain in the pub-
lic domain.  Zcrypt was originally written in Europe and, as of April 2000,
can be freely distributed from the US as well as other countries.

(The ability to export from the US is new and is due to a change in the Bureau
of Export Administration's regulations, as published in Volume 65, Number
10, of the Federal Register [14 January 2000].  Info-ZIP filed the required
notification via e-mail on 9 April 2000; see the USexport.msg file in this
archive.  However, as of June 2002, it can now be freely distributed in both
source and object forms from any country, including the USA under License
Exception TSU of the U.S. Export Administration Regulations (section 740.13(e))
of 6 June 2002.)

    LIKE ANYTHING ELSE THAT IS FREE, ZIP, UNZIP AND THEIR ASSOCIATED
    UTILITIES ARE PROVIDED AS IS AND COME WITH NO WARRANTY OF ANY KIND,
    EITHER EXPRESSED OR IMPLIED. IN NO EVENT WILL THE AUTHORS BE LIABLE
    FOR ANY DAMAGES RESULTING FROM THE USE OF THIS SOFTWARE.

The encryption code is a direct transcription of the algorithm from
Roger Schlafly, described by Phil Katz in the file appnote.txt.  This
file is distributed with the PKZIP program (even in the version without
encryption capabilities).  Note that the encryption will probably resist
attacks by amateurs if the password is well chosen and long enough (at 
least 8 characters) but it will probably not resist attacks by experts.
Paul Kocher has made available information concerning a known-plaintext
attack for the PKWARE encryption scheme; see http://www.cryptography.com/
for details.)  Short passwords consisting of lowercase letters only can be
recovered in a few hours on any workstation.  But for casual cryptography
designed to keep your mother from reading your mail, it's OK.

For more serious encryption, check into PGP (Pretty Good Privacy), a
public-key-based encryption system available from various Internet sites.
PGP has Zip and UnZip built into it.  The most recent version at the time
this was originally written was 6.5, although older versions were still
widespread.  At the time of this writing there are now GPG, PGP Universal
2.0, and various others based on OpenPGP.

We are looking at adding AES strong encryption to future versions of Zip and
UnZip.

Zip 2.3x and UnZip 5.5x and later are compatible with PKZIP 2.04g.  (Thanks
to Phil Katz for accepting our suggested minor changes to the zipfile format.)

IMPORTANT NOTE:

  Zip archives produced by Zip 2.0 or later must not be *updated* by
  Zip 1.1 or PKZIP 1.10 or PKZIP 1.93a, if they contain encrypted members
  or if they have been produced in a pipe or on a non-seekable device.
  The old versions of Zip or PKZIP would destroy the zip structure.  The
  old versions can list the contents of the zipfile but cannot extract
  it anyway (because of the new compression algorithm).  If you do not
  use encryption and compress regular disk files, you need not worry about
  this problem.


Contents that were distributed and now are part of the main source files:

  file           what it is
  ----           ----------
  README.CR      this file
  LICENSE        Info-ZIP license (terms of reuse and redistribution)
  USexport.msg   export notice sent to US Bureau of Export Administration
  WHERE          where Zip/UnZip/WiZ and related utilities can be found
  crypt.c        code for encryption and decryption
  crypt.h        code for encryption and decryption
  file_id.diz    description file for some BBSes

Most all of the files are in Unix (LF only) format.  On MSDOS systems, you
can use the -a option of UnZip to convert the source files to CRLF
format.  This is only necessary if you wish to edit the files -- they
will compile as is with Microsoft C and Turbo/Borland C++ 1.0 or
later.  However, you will have to convert the files (using "unzip -a")
to the CRLF format to compile with the older Turbo C 1.0 or 2.0.  You
should be able to find Zip and UnZip in the same place you found this 
(see ftp://ftp.info-zip.org/pub/infozip/Info-ZIP.html or the file
"WHERE" for details).

Current releases all have encryption built in.  To update previous versions
using the zcrypt sources:

  (1) Get the main sources (e.g., Zip 2.3) and unpack into a working
      directory, as usual.

  (2) Overwrite the dummy crypt.c and crypt.h from the main sources with 
      the versions from this package.  If you want to overwrite directly
      out of the zcrypt29 archive, do not use UnZip's freshen/updating
      option; the dummy files may be newer than the real sources in
      zcrypt29.  ("unzip -o zcrypt29 -d /your/working/dir" will do the
      Right Thing in most cases, although it may overwrite a newer WHERE
      file under some circumstances.)

  (3) Read the main INSTALL document and compile normally!  No makefile
      changes are necessary on account of the zcrypt sources.  You can
      check that the version you just compiled has encryption or decryption
      support enabled by typing "zip -v" or "unzip -v" and verifying that
      the last "special compilation option" says encryption or decryption
      is included.

Encryption enables new "-e" and "-P password" options in Zip, and a new
"-P password" option in UnZip--see the normal Zip and UnZip documentation
for details.  (Note that passing a plaintext password on the command line
is potentially much more insecure than being prompted for it interactively,
which is the default for UnZip and for Zip with "-e".  Also note that the
interactive method allows UnZip to deal with archives that use different
passwords for different files.)
