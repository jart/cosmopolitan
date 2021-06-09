Zip 3.0 is the first Zip update adding large file support.  For now Zip 2.3x
remains available and supported, but users should switch to this new release.

Testing for Zip 3.0 has focused mainly on Unix, VMS, Max OS X, and Win32,
and some other ports may not be fully supported yet.  If you find your
favorite port is broke, send us the details or, better, send bug fixes.  It's
possible that support for some older ports may be dropped in the future.



Copyright (c) 1990-2008 Info-ZIP.  All rights reserved.

See the accompanying file LICENSE (the contents of which are also included
in unzip.h, zip.h and wiz.h) for terms of use.  If, for some reason, all
of these files are missing, the Info-ZIP license also may be found at:
ftp://ftp.info-zip.org/pub/infozip/license.html and
http://www.info-zip.org/pub/infozip/license.html.


Zip 3.0 is a compression and file packaging utility.  It is compatible with
PKZIP 2.04g (Phil Katz ZIP) for MSDOS systems.  There is a companion to zip
called unzip (of course) which you should be able to find in the same place
you got zip.  See the file 'WHERE' for details on ftp sites and mail
servers.

So far zip has been ported to a wide array of Unix and other mainframes,
minis, and micros including VMS, OS/2, Minix, MSDOS, Windows, Atari, Amiga,
BeOS and VM/CMS.  Although highly compatible with PKware's PKZIP and PKUNZIP
utilities of MSDOS fame, our primary objective has been one of portability
and other-than-MSDOS functionality.  Features not found in the PKWare version
include creation of zip files in a pipe or on a device; VMS, BeOS and OS/2
extended file attributes; conversion from Unix to MSDOS text file format; and,
of course, the ability to run on most of your favorite operating systems.  And
it's free.

See the file zip30.ann for a summary of new features in Zip 3.0 and WhatsNew
for the detailed list of new features and changes since Zip 2.32.  The file
CHANGES details all day-to-day changes during development.

Notes:

Multi-volume support.  This version does not support multi-volume spanned
archives as in pkzip 2.04g, and there is no intention at this point to support
spanned archives, but Zip 3.0 supports split archives.  A split archive is an
archive split into a set of files, each file a piece of the archive and each
file using an extension, such as .z02 as in the file name archive.z02, that
provides the order of the splits.  In contrast, a spanned archive is the
original multi-floppy archive supported by pkzip 2.0g where the split order
is contained in the volume labels.  The contents of split and spanned archives
are mostly identical and there is a simple procedure to convert between the
formats.  Many current unzips now support split archives.

Zip64 support.  This version supports Zip64 archives as described in the
PKWare AppNote.  These archives use additional fields to support archives
greater than 2 GB and files in archives over the 2 GB previous limit (4 GB
on some ports).  The Zip64 format also allows more than 64k entries in an
archive.  Support by the OS for files larger than 4 GB is needed for Zip to
create and read large files and archives.  On Unix, Win32, and some other
ports, large file and Zip64 support is automatically checked for and
compiled in if available.  Use of Zip64 by Zip is automatic and to maximize
backward compatibility the Zip64 fields will only be used if needed.  A
Zip64 archive requires a pkzip 4.5 compatible unzip, such as UnZip 6.0.

Unicode support.  This version has initial Unicode support.  This allows
paths and names of files in other character sets to be accurately recreated
on OS that have sufficient character set support.  On Win32, if wide
character calls are supported (not Win 9x unless Unicode support has been
added) all files (including paths with illegal characters in the current
character set) should now be readable by zip.  Unicode support is provided
using a new set of UTF-8 path and comment extra fields and a new UTF-8 bit
for flagging when the current character set is already UTF-8.  Zip 3.0
maintains backward compatibility with older archives and is mostly compliant
with the new Unicode additions in the latest PKWare AppNote.  The exception
is UTF-8 comments, which are not supported if UTF-8 is not the native
character set, but should be fully implemented in Zip 3.1.

16-bit OS support.  Though Zip 3.0 is designed to support the latest zip
standards and modern OS, some effort has been made to maintain support
for older and smaller systems.  If you find Zip 3.0 does not fit on or
otherwise does not work well on a particular OS, send in the details and
we might be able to help.

Compression methods.  In addition to the standard store and deflate methods,
Zip now can use the bzip2 compression format using the bzip2 library.  Though
bzip2 compression generally takes longer, in many cases using bzip2 results
in much better compression.  However, many unzips may not yet support
bzip2 compressed entries in archives, so test your unzip first before using
bzip2 compression.

Installation.  Please read the file INSTALL for information on how to compile
and install zip, zipsplit, zipcloak, and zipnote and please read the manual
pages ZIP.txt, ZIPSPLIT.txt, ZIPCLOAK.txt, and ZIPNOTE.txt for information on
how to use them.  Also, if you are using MSDOS or Windows, note that text
files in the distribution are generally in Unix line end format (LF only)
and Windows and DOS users will need to either convert the files as needed to
DOS line ends (CR LF) or extract the distribution contents using unzip -a.

Utilities.  At this point zipsplit, zipcloak, and zipnote should work with
large files, but they currently do not handle split archives.  A work around
is to use zip to convert a split archive to a single file archive and then use
the utilities on that archive.

Encryption.  This version supports standard zip encryption.  Until recently
the encryption code was distributed separately because of the US export
regulations but now is part of the main distribution.  See crypt.c for
details.  Decryption can be made with unzip 5.0p1 or later, or with zipcloak.

Bug reports.  All bug reports or patches should go to zip-bugs via the web
site contact form at http://www.info-zip.org/zip-bug.html (we have discontinued
the old email address zip-bugs@lists.wku.edu because of too much spam lately)
and suggestions for new features can be submitted there also (although we don't
promise to use all of them).  We also are on SourceForge at
http://sourceforge.net/projects/infozip/ and now automatically get Bug Reports
and Feature Requests submitted there.  In addition, a new Info-ZIP discussion
forum is available as well.  See below.  Though bug reports can be posted there,
we don't have automatic monitoring of all postings set up yet so you may want
to use the web form or SoureForge for a quicker response.  A good approach may
be to post the details on the forum so others can benefit from the posting,
then use the web reply form to let us know you did that if you don't get a
reply in a reasonable time.

Ports.  If you're considering a port, please check in with zip-bugs FIRST,
since the code is constantly being updated behind the scenes.  We'll
arrange to give you access to the latest source.

Discussion group.  If you'd like to keep up to date with our Zip (and companion
UnZip utility) development, join the ranks of BETA testers, add your own
thoughts and contributions, etc., check out the new discussion forum.  This is
the latest offering, after the various Info-ZIP mailing-lists on
mxserver@lists.wku.edu (courtesy of Hunter Goatley) were no longer available
and the temporary QuickTopic discussion group for Info-ZIP issues at
http://www.quicktopic.com/27/H/V6ZQZ54uKNL died a horrible death due to large
amounts of spam.  The new discussion forum is now available at
http://www.info-zip.org/board/board.pl (thanks again to Hunter Goatley) and
can be used to discuss issues, request features, and is one place new betas
and releases are announced.  It also is a place to post bug reports, and
patches can be submitted as attachments.  However, we don't yet get
automatic notification of all postings there so try one of the other methods
if you don't get a response.  You can also post Bug Reports and Feature
Requests at Source Forge.  However, the web site contact form remains
available if you would rather not post on the public forums.

Frequently asked questions on zip and unzip:

Q. When unzipping I get an error message about "compression method 8".

A. This is standard deflate, which has been around for awhile.  Please
   get a current version of unzip.  See the file 'WHERE' for details.


Q. How about "compression method 12"?

A. Compression method 12 is bzip2 and requires a relatively modern unzip.
   Please get the latest version of unzip.


Q. I can't extract this zip file that I just downloaded.  I get
   "zipfile is part of multi-disk archive" or some other message.

A. Please make sure that you made the transfer in binary mode.  Check
   in particular that your copy has exactly the same size as the original.
   Note that the above message also may actually mean you have only part
   of a multi-part archive.  Also note that UnZip 5.x does not and UnZip 6.0
   probably won't have multi-disk (split) archive support.  A work around
   is to use Zip 3.0 to convert the split archive to a single-file archive
   then use UnZip on that archive.  As a last result, if there's something
   readable in what you have, zip -FF should be able to recover it.


Q. When running unzip, I get a message about "End-of-central-directory
   signature not found".

A. This usually means that your zip archive is damaged, or that you
   have an uncompressed file with the same name in the same directory.
   In the first case, it makes more sense to contact the person you
   obtained the zip file from rather than the Info-ZIP software
   developers, and to make sure that your copy is strictly identical to
   the original.  In the second case, use "unzip zipfile.zip" instead
   of "unzip zipfile", to let unzip know which file is the zip archive
   you want to extract.


Q. Why doesn't zip do <something> just like PKZIP does?

A. Zip is not a PKZIP clone and is not intended to be one.  In some
   cases we feel PKZIP does not do the right thing (e.g., not
   including pathnames by default); in some cases the operating system
   itself is responsible (e.g., under Unix it is the shell which
   expands wildcards, not zip).  Info-ZIP's and PKWARE's zipfiles
   are interchangeable, not the programs.

   For example, if you are used to the following PKZIP command:
               pkzip -rP foo *.c
   you must use instead on Unix:
               zip -R foo "*.c"
   (the quotes are needed to let the shell know that it should
    not expand the *.c argument but instead pass it on to the program,
    but are not needed on ports that do not expand file paths like
    MSDOS)


Q. Can I distribute zip and unzip sources and/or executables?

A. You may redistribute the latest official distributions without any
   modification, without even asking us for permission. You can charge
   for the cost of the media (CDROM, diskettes, etc...) and a small copying
   fee.  If you want to distribute modified versions please contact us at
   www.Info-ZIP.org first. You must not distribute beta versions.
   The latest official distributions are always on ftp.Info-ZIP.org in
   directory /pub/infozip and subdirectories and at SourceForge.


Q. Can I use the executables of zip and unzip to distribute my software?

A. Yes, so long as it is made clear in the product documentation that
   zip or unzip are not being sold, that the source code is freely
   available, and that there are no extra or hidden charges resulting
   from its use by or inclusion with the commercial product.  See the
   Info-ZIP license for more.  Here is an example of a suitable notice:

     NOTE:  <Product> is packaged on this CD using Info-ZIP's compression
     utility.  The installation program uses UnZip to read zip files from
     the CD.  Info-ZIP's software (Zip, UnZip and related utilities) is
     freely distributed under the Info-ZIP license and can be obtained as
     source code or executables from various anonymous-ftp sites,
     including ftp://ftp.info-zip.org/pub/infozip.


Q. Can I use the source code of zip and unzip in my commercial application?

A. Yes, as long as the conditions in the Info-ZIP license are met.  We
   recommend you include in your product documentation an acknowledgment
   and note that the original compression sources are available at
   www.Info-ZIP.org. If you have special requirements contact us.
