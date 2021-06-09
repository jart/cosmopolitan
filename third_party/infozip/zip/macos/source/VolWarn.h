/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
/*

This is an Important note  about pathnames

*/

static char DuplicVolumeNote[] = {
    "\rIMPORTANT NOTE:" \
    "\r" \
    "\r This port has one weak point: It is based on pathnames !! " \
    "\r Because it's a port !! Unlike MacOS: As far as I know all other "\
    "\r Operatingsystems  (eg.: Unix, DOS, OS/2, ...) are based on pathnames" \
    "\r " \
    /* a short quote from "Inside Macintintosh, Files"; slightly modified by me */
    "\r On a Mac: Files and directories located in the same directory " \
    "\r must all have unique names. However, there is no requirement " \
    "\r that volumes have unique names. It is perfectly acceptable for two mounted" \
    "\r volumes to have the same name. This is one reason why a application should " \
    "\r use volume reference numbers rather than volume names to specify volumes," \
    "\r but for this Zip-Port I can't use reference numbers. " \
    "\r " \
    /* end quote */
    "\r" \
    "\r From the developers point of view:"\
    "\r The use of pathnames, however, is highly discouraged. If the user changes"\
    "\r names or moves things around, they are worthless." \
    "\r Full pathnames are particularly unreliable as a means of identifying files," \
    "\r directories or volumes within your application," \
    "\r for two primary reasons:" \
    "\r" \
    "\r*  The user can change the name of any element in the path at" \
    "\r   virtually any time." \
    "\r*  Volume names on the Macintosh are *not* unique. Multiple" \
    "\r   mounted volumes can have the same name. For this reason, the use of" \
    "\r   a full pathname to identify a specific volume may not produce the" \
    "\r   results you expect. If more than one volume has the same name and" \
    "\r   a full pathname is used, the File Manager currently uses the first" \
    "\r   mounted volume it finds with a matching name in the volume queue." \
    "\r" \
    "\r" \
    "\r The main reason is that an attempt to implement support exact saving of" \
    "\r the MacOS specific internal file-structures would require a throughout" \
    "\r rewrite of major parts of shared code, probably sacrifying compatibility" \
    "\r with other systems." \
    "\r I have no solution at the moment. The port will just warn you if you try" \
    "\r zip from / to a volume which has a duplicate name." \
    "\r MacZip has problems to find the archives and files." \
    "\r" \
    "\r" \
    "\r ... and the moral of this story:" \
    "\r" \
    "\r Don't mount multiple volumes with the same " \
    "\r name while zip/unzip is running" \
    "\r and "\
    "\r My (Big) recommendation: Name all your volumes with a unique name "\
    "\r (e.g: add a space character to the name) and" \
    "\r MacZip will run without any problem." \
    "\r" \
    "\r" \
    "\r Dirk Haase" \
    };
