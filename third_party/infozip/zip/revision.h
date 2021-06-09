/*
  revision.h - Zip 3

  Copyright (c) 1990-2008 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*
 *  revision.h by Mark Adler.
 */

#ifndef __revision_h
#define __revision_h 1

/* For api version checking */
#define Z_MAJORVER   3
#define Z_MINORVER   0
#define Z_PATCHLEVEL 0
#define Z_BETALEVEL "i BETA"

#define VERSION "3.0"
#define REVDATE "July 5th 2008"

#define DW_MAJORVER    Z_MAJORVER
#define DW_MINORVER    Z_MINORVER
#define DW_PATCHLEVEL  Z_PATCHLEVEL

#ifndef IZ_COMPANY_NAME               /* might be already defined... */
#  define IZ_COMPANY_NAME "Info-ZIP"
#endif

#if !defined(WINDLL) && !defined(IZ_VERSION_SYMBOLS_ONLY)
/* Copyright notice for binary executables--this notice only applies to
 * those (zip, zipcloak, zipsplit, and zipnote), not to this file
 * (revision.h).
 */

#ifndef DEFCPYRT
/* copyright[] et.al. get defined only once ! */
/* keep array sizes in sync with number of text */
/* lines in the array definitions below !!      */
extern ZCONST char *copyright[1];
extern ZCONST char * far swlicense[50];
extern ZCONST char * far versinfolines[7];
extern ZCONST char * far cryptnote[7];

#else /* DEFCPYRT */

ZCONST char *copyright[] = {
"Copyright (c) 1990-2008 Info-ZIP - Type '%s \"-L\"' for software license."
/* XXX still necessary ???? */
#ifdef AZTEC_C
,        /* extremely lame compiler bug workaround */
#endif
};

ZCONST char * far versinfolines[] = {
"This is %s %s (%s), by Info-ZIP.",
"Currently maintained by E. Gordon.  Please send bug reports to",
"the authors using the web page at www.info-zip.org; see README for details.",
"",
"Latest sources and executables are at ftp://ftp.info-zip.org/pub/infozip,",
"as of above date; see http://www.info-zip.org/ for other sites.",
""
};

/* new notice - 4 March 2007 */
ZCONST char * far cryptnote[] = {
"Encryption notice:",
"\tThe encryption code of this program is not copyrighted and is",
"\tput in the public domain.  It was originally written in Europe",
"\tand, to the best of our knowledge, can be freely distributed",
"\tin both source and object forms from any country, including",
"\tthe USA under License Exception TSU of the U.S. Export",
"\tAdministration Regulations (section 740.13(e)) of 6 June 2002."
};

ZCONST char * far swlicense[] = {
"Copyright (c) 1990-2008 Info-ZIP.  All rights reserved.",
"",
"For the purposes of this copyright and license, \"Info-ZIP\" is defined as",
"the following set of individuals:",
"",
"   Mark Adler, John Bush, Karl Davis, Harald Denker, Jean-Michel Dubois,",
"   Jean-loup Gailly, Hunter Goatley, Ed Gordon, Ian Gorman, Chris Herborth,",
"   Dirk Haase, Greg Hartwig, Robert Heath, Jonathan Hudson, Paul Kienitz,",
"   David Kirschbaum, Johnny Lee, Onno van der Linden, Igor Mandrichenko,",
"   Steve P. Miller, Sergio Monesi, Keith Owens, George Petrov, Greg Roelofs,",
"   Kai Uwe Rommel, Steve Salisbury, Dave Smith, Steven M. Schweda,",
"   Christian Spieler, Cosmin Truta, Antoine Verheijen, Paul von Behren,",
"   Rich Wales, Mike White",
"",
"This software is provided \"as is,\" without warranty of any kind, express",
"or implied.  In no event shall Info-ZIP or its contributors be held liable",
"for any direct, indirect, incidental, special or consequential damages",
"arising out of the use of or inability to use this software.",
"",
"Permission is granted to anyone to use this software for any purpose,",
"including commercial applications, and to alter it and redistribute it",
"freely, subject to the above disclaimer and the following restrictions:",
"",
"    1. Redistributions of source code (in whole or in part) must retain",
"       the above copyright notice, definition, disclaimer, and this list",
"       of conditions.",
"",
"    2. Redistributions in binary form (compiled executables and libraries)",
"       must reproduce the above copyright notice, definition, disclaimer,",
"       and this list of conditions in documentation and/or other materials",
"       provided with the distribution.  The sole exception to this condition",
"       is redistribution of a standard UnZipSFX binary (including SFXWiz) as",
"       part of a self-extracting archive; that is permitted without inclusion",
"       of this license, as long as the normal SFX banner has not been removed",
"       from the binary or disabled.",
"",
"    3. Altered versions--including, but not limited to, ports to new operating",
"       systems, existing ports with new graphical interfaces, versions with",
"       modified or added functionality, and dynamic, shared, or static library",
"       versions not from Info-ZIP--must be plainly marked as such and must not",
"       be misrepresented as being the original source or, if binaries,",
"       compiled from the original source.  Such altered versions also must not",
"       be misrepresented as being Info-ZIP releases--including, but not",
"       limited to, labeling of the altered versions with the names \"Info-ZIP\"",
"       (or any variation thereof, including, but not limited to, different",
"       capitalizations), \"Pocket UnZip,\" \"WiZ\" or \"MacZip\" without the",
"       explicit permission of Info-ZIP.  Such altered versions are further",
"       prohibited from misrepresentative use of the Zip-Bugs or Info-ZIP",
"       e-mail addresses or the Info-ZIP URL(s), such as to imply Info-ZIP",
"       will provide support for the altered versions.",
"",
"    4. Info-ZIP retains the right to use the names \"Info-ZIP,\" \"Zip,\" \"UnZip,\"",
"       \"UnZipSFX,\" \"WiZ,\" \"Pocket UnZip,\" \"Pocket Zip,\" and \"MacZip\" for its",
"       own source and binary releases."
};
#endif /* DEFCPYRT */
#endif /* !WINDLL && !IZ_VERSION_SYMBOLS_ONLY */
#endif /* !__revision_h */
