/*
  Copyright (c) 1990-2010 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2009-Jan-02 or later
  (the contents of which are also included in unzip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*---------------------------------------------------------------------------

  zipinfo.c                                              Greg Roelofs et al.

  This file contains all of the ZipInfo-specific listing routines for UnZip.

  Contains:  zi_opts()
             zi_end_central()
             zipinfo()
             zi_long()
             zi_short()
             zi_time()

  ---------------------------------------------------------------------------*/


#define UNZIP_INTERNAL
#include "third_party/unzip/unzip.h"
#include "third_party/unzip/globals.h"


#ifndef NO_ZIPINFO  /* strings use up too much space in small-memory systems */

/* Define OS-specific attributes for use on ALL platforms--the S_xxxx
 * versions of these are defined differently (or not defined) by different
 * compilers and operating systems. */

#define UNX_IFMT       0170000     /* Unix file type mask */
#define UNX_IFREG      0100000     /* Unix regular file */
#define UNX_IFSOCK     0140000     /* Unix socket (BSD, not SysV or Amiga) */
#define UNX_IFLNK      0120000     /* Unix symbolic link (not SysV, Amiga) */
#define UNX_IFBLK      0060000     /* Unix block special       (not Amiga) */
#define UNX_IFDIR      0040000     /* Unix directory */
#define UNX_IFCHR      0020000     /* Unix character special   (not Amiga) */
#define UNX_IFIFO      0010000     /* Unix fifo    (BCC, not MSC or Amiga) */
#define UNX_ISUID      04000       /* Unix set user id on execution */
#define UNX_ISGID      02000       /* Unix set group id on execution */
#define UNX_ISVTX      01000       /* Unix directory permissions control */
#define UNX_ENFMT      UNX_ISGID   /* Unix record locking enforcement flag */
#define UNX_IRWXU      00700       /* Unix read, write, execute: owner */
#define UNX_IRUSR      00400       /* Unix read permission: owner */
#define UNX_IWUSR      00200       /* Unix write permission: owner */
#define UNX_IXUSR      00100       /* Unix execute permission: owner */
#define UNX_IRWXG      00070       /* Unix read, write, execute: group */
#define UNX_IRGRP      00040       /* Unix read permission: group */
#define UNX_IWGRP      00020       /* Unix write permission: group */
#define UNX_IXGRP      00010       /* Unix execute permission: group */
#define UNX_IRWXO      00007       /* Unix read, write, execute: other */
#define UNX_IROTH      00004       /* Unix read permission: other */
#define UNX_IWOTH      00002       /* Unix write permission: other */
#define UNX_IXOTH      00001       /* Unix execute permission: other */

#define VMS_IRUSR      UNX_IRUSR   /* VMS read/owner */
#define VMS_IWUSR      UNX_IWUSR   /* VMS write/owner */
#define VMS_IXUSR      UNX_IXUSR   /* VMS execute/owner */
#define VMS_IRGRP      UNX_IRGRP   /* VMS read/group */
#define VMS_IWGRP      UNX_IWGRP   /* VMS write/group */
#define VMS_IXGRP      UNX_IXGRP   /* VMS execute/group */
#define VMS_IROTH      UNX_IROTH   /* VMS read/other */
#define VMS_IWOTH      UNX_IWOTH   /* VMS write/other */
#define VMS_IXOTH      UNX_IXOTH   /* VMS execute/other */

#define AMI_IFMT       06000       /* Amiga file type mask */
#define AMI_IFDIR      04000       /* Amiga directory */
#define AMI_IFREG      02000       /* Amiga regular file */
#define AMI_IHIDDEN    00200       /* to be supported in AmigaDOS 3.x */
#define AMI_ISCRIPT    00100       /* executable script (text command file) */
#define AMI_IPURE      00040       /* allow loading into resident memory */
#define AMI_IARCHIVE   00020       /* not modified since bit was last set */
#define AMI_IREAD      00010       /* can be opened for reading */
#define AMI_IWRITE     00004       /* can be opened for writing */
#define AMI_IEXECUTE   00002       /* executable image, a loadable runfile */
#define AMI_IDELETE    00001       /* can be deleted */

#define THS_IFMT    0xF000         /* Theos file type mask */
#define THS_IFIFO   0x1000         /* pipe */
#define THS_IFCHR   0x2000         /* char device */
#define THS_IFSOCK  0x3000         /* socket */
#define THS_IFDIR   0x4000         /* directory */
#define THS_IFLIB   0x5000         /* library */
#define THS_IFBLK   0x6000         /* block device */
#define THS_IFREG   0x8000         /* regular file */
#define THS_IFREL   0x9000         /* relative (direct) */
#define THS_IFKEY   0xA000         /* keyed */
#define THS_IFIND   0xB000         /* indexed */
#define THS_IFRND   0xC000         /* ???? */
#define THS_IFR16   0xD000         /* 16 bit real mode program */
#define THS_IFP16   0xE000         /* 16 bit protected mode prog */
#define THS_IFP32   0xF000         /* 32 bit protected mode prog */
#define THS_IMODF   0x0800         /* modified */
#define THS_INHID   0x0400         /* not hidden */
#define THS_IEUSR   0x0200         /* erase permission: owner */
#define THS_IRUSR   0x0100         /* read permission: owner */
#define THS_IWUSR   0x0080         /* write permission: owner */
#define THS_IXUSR   0x0040         /* execute permission: owner */
#define THS_IROTH   0x0004         /* read permission: other */
#define THS_IWOTH   0x0002         /* write permission: other */
#define THS_IXOTH   0x0001         /* execute permission: other */

#ifndef NSK_UNSTRUCTURED
# define NSK_UNSTRUCTURED   0
#endif
#ifndef NSK_OBJECTFILECODE
# define NSK_OBJECTFILECODE 100
#endif
#ifndef NSK_EDITFILECODE
# define NSK_EDITFILECODE   101
#endif

#define LFLAG  3   /* short "ls -l" type listing */

static int   zi_long   OF((__GPRO__ zusz_t *pEndprev, int error_in_archive));
static int   zi_short  OF((__GPRO));
static void  zi_showMacTypeCreator
                       OF((__GPRO__ uch *ebfield));
static char *zi_time   OF((__GPRO__ ZCONST ulg *datetimez,
                           ZCONST time_t *modtimez, char *d_t_str));


/**********************************************/
/*  Strings used in zipinfo.c (ZipInfo half)  */
/**********************************************/

static ZCONST char nullStr[] = "";
static ZCONST char PlurSufx[] = "s";

static ZCONST char Far ZipInfHeader2[] =
  "Zip file size: %s bytes, number of entries: %s\n";
static ZCONST char Far EndCentDirRec[] = "\nEnd-of-central-directory record:\n";
static ZCONST char Far LineSeparators[] = "-------------------------------\n\n";
static ZCONST char Far ZipFSizeVerbose[] = "\
  Zip archive file size:               %s (%sh)\n";
static ZCONST char Far ActOffsetCentDir[] = "\
  Actual end-cent-dir record offset:   %s (%sh)\n\
  Expected end-cent-dir record offset: %s (%sh)\n\
  (based on the length of the central directory and its expected offset)\n\n";
static ZCONST char Far SinglePartArchive1[] = "\
  This zipfile constitutes the sole disk of a single-part archive; its\n\
  central directory contains %s %s.\n\
  The central directory is %s (%sh) bytes long,\n";
static ZCONST char Far SinglePartArchive2[] = "\
  and its (expected) offset in bytes from the beginning of the zipfile\n\
  is %s (%sh).\n\n";
static ZCONST char Far MultiPartArchive1[] = "\
  This zipfile constitutes disk %lu of a multi-part archive.  The central\n\
  directory starts on disk %lu at an offset within that archive part\n";
static ZCONST char Far MultiPartArchive2[] = "\
  of %s (%sh) bytes.  The entire\n\
  central directory is %s (%sh) bytes long.\n";
static ZCONST char Far MultiPartArchive3[] = "\
  %s of the archive entries %s contained within this zipfile volume,\n\
  out of a total of %s %s.\n\n";
static ZCONST char Far NoMemArguments[] =
  "envargs:  cannot get memory for arguments";

static ZCONST char Far CentralDirEntry[] =
  "\nCentral directory entry #%lu:\n---------------------------\n\n";
static ZCONST char Far ZipfileStats[] =
  "%lu file%s, %s bytes uncompressed, %s bytes compressed:  %s%d.%d%%\n";

/* zi_long() strings */
static ZCONST char Far OS_FAT[] = "MS-DOS, OS/2 or NT FAT";
static ZCONST char Far OS_Amiga[] = "Amiga";
static ZCONST char Far OS_VMS[] = "VMS";
static ZCONST char Far OS_Unix[] = "Unix";
static ZCONST char Far OS_VMCMS[] = "VM/CMS";
static ZCONST char Far OS_AtariST[] = "Atari ST";
static ZCONST char Far OS_HPFS[] = "OS/2 or NT HPFS";
static ZCONST char Far OS_Macintosh[] = "Macintosh HFS";
static ZCONST char Far OS_ZSystem[] = "Z-System";
static ZCONST char Far OS_CPM[] = "CP/M";
static ZCONST char Far OS_TOPS20[] = "TOPS-20";
static ZCONST char Far OS_NTFS[] = "NTFS";
static ZCONST char Far OS_QDOS[] = "SMS/QDOS";
static ZCONST char Far OS_Acorn[] = "Acorn RISC OS";
static ZCONST char Far OS_MVS[] = "MVS";
static ZCONST char Far OS_VFAT[] = "Win32 VFAT";
static ZCONST char Far OS_AtheOS[] = "AtheOS";
static ZCONST char Far OS_BeOS[] = "BeOS";
static ZCONST char Far OS_Tandem[] = "Tandem NSK";
static ZCONST char Far OS_Theos[] = "Theos";
static ZCONST char Far OS_MacDarwin[] = "Mac OS/X (Darwin)";
#ifdef OLD_THEOS_EXTRA
  static ZCONST char Far OS_TheosOld[] = "Theos (Old)";
#endif /* OLD_THEOS_EXTRA */

static ZCONST char Far MthdNone[] = "none (stored)";
static ZCONST char Far MthdShrunk[] = "shrunk";
static ZCONST char Far MthdRedF1[] = "reduced (factor 1)";
static ZCONST char Far MthdRedF2[] = "reduced (factor 2)";
static ZCONST char Far MthdRedF3[] = "reduced (factor 3)";
static ZCONST char Far MthdRedF4[] = "reduced (factor 4)";
static ZCONST char Far MthdImplode[] = "imploded";
static ZCONST char Far MthdToken[] = "tokenized";
static ZCONST char Far MthdDeflate[] = "deflated";
static ZCONST char Far MthdDeflat64[] = "deflated (enhanced-64k)";
static ZCONST char Far MthdDCLImplode[] = "imploded (PK DCL)";
static ZCONST char Far MthdBZip2[] = "bzipped";
static ZCONST char Far MthdLZMA[] = "LZMA-ed";
static ZCONST char Far MthdTerse[] = "tersed (IBM)";
static ZCONST char Far MthdLZ77[] = "LZ77-compressed (IBM)";
static ZCONST char Far MthdWavPack[] = "WavPacked";
static ZCONST char Far MthdPPMd[] = "PPMd-ed";

static ZCONST char Far DeflNorm[] = "normal";
static ZCONST char Far DeflMax[] = "maximum";
static ZCONST char Far DeflFast[] = "fast";
static ZCONST char Far DeflSFast[] = "superfast";

static ZCONST char Far ExtraBytesPreceding[] =
  "  There are an extra %s bytes preceding this file.\n\n";

static ZCONST char Far UnknownNo[] = "unknown (%d)";

#ifdef ZIP64_SUPPORT
  static ZCONST char Far LocalHeaderOffset[] =
    "\n  offset of local header from start of archive:   %s\n\
                                                  (%sh) bytes\n";
#else
  static ZCONST char Far LocalHeaderOffset[] =
    "\n  offset of local header from start of archive:   %s (%sh) bytes\n";
#endif
static ZCONST char Far HostOS[] =
  "  file system or operating system of origin:      %s\n";
static ZCONST char Far EncodeSWVer[] =
  "  version of encoding software:                   %u.%u\n";
static ZCONST char Far MinOSCompReq[] =
  "  minimum file system compatibility required:     %s\n";
static ZCONST char Far MinSWVerReq[] =
  "  minimum software version required to extract:   %u.%u\n";
static ZCONST char Far CompressMethod[] =
  "  compression method:                             %s\n";
static ZCONST char Far SlideWindowSizeImplode[] =
  "  size of sliding dictionary (implosion):         %cK\n";
static ZCONST char Far ShannonFanoTrees[] =
  "  number of Shannon-Fano trees (implosion):       %c\n";
static ZCONST char Far CompressSubtype[] =
  "  compression sub-type (deflation):               %s\n";
static ZCONST char Far FileSecurity[] =
  "  file security status:                           %sencrypted\n";
static ZCONST char Far ExtendedLocalHdr[] =
  "  extended local header:                          %s\n";
static ZCONST char Far FileModDate[] =
  "  file last modified on (DOS date/time):          %s\n";
#ifdef USE_EF_UT_TIME
  static ZCONST char Far UT_FileModDate[] =
    "  file last modified on (UT extra field modtime): %s %s\n";
  static ZCONST char Far LocalTime[] = "local";
#ifndef NO_GMTIME
  static ZCONST char Far GMTime[] = "UTC";
#endif
#endif /* USE_EF_UT_TIME */
static ZCONST char Far CRC32Value[] =
  "  32-bit CRC value (hex):                         %.8lx\n";
static ZCONST char Far CompressedFileSize[] =
  "  compressed size:                                %s bytes\n";
static ZCONST char Far UncompressedFileSize[] =
  "  uncompressed size:                              %s bytes\n";
static ZCONST char Far FilenameLength[] =
  "  length of filename:                             %u characters\n";
static ZCONST char Far ExtraFieldLength[] =
  "  length of extra field:                          %u bytes\n";
static ZCONST char Far FileCommentLength[] =
  "  length of file comment:                         %u characters\n";
static ZCONST char Far FileDiskNum[] =
  "  disk number on which file begins:               disk %lu\n";
static ZCONST char Far ApparentFileType[] =
  "  apparent file type:                             %s\n";
static ZCONST char Far VMSFileAttributes[] =
  "  VMS file attributes (%06o octal):             %s\n";
static ZCONST char Far AmigaFileAttributes[] =
  "  Amiga file attributes (%06o octal):           %s\n";
static ZCONST char Far UnixFileAttributes[] =
  "  Unix file attributes (%06o octal):            %s\n";
static ZCONST char Far NonMSDOSFileAttributes[] =
  "  non-MSDOS external file attributes:             %06lX hex\n";
static ZCONST char Far MSDOSFileAttributes[] =
  "  MS-DOS file attributes (%02X hex):                none\n";
static ZCONST char Far MSDOSFileAttributesRO[] =
  "  MS-DOS file attributes (%02X hex):                read-only\n";
static ZCONST char Far MSDOSFileAttributesAlpha[] =
  "  MS-DOS file attributes (%02X hex):                %s%s%s%s%s%s%s%s\n";
static ZCONST char Far TheosFileAttributes[] =
  "  Theos file attributes (%04X hex):               %s\n";

static ZCONST char Far TheosFTypLib[] = "Library     ";
static ZCONST char Far TheosFTypDir[] = "Directory   ";
static ZCONST char Far TheosFTypReg[] = "Sequential  ";
static ZCONST char Far TheosFTypRel[] = "Direct      ";
static ZCONST char Far TheosFTypKey[] = "Keyed       ";
static ZCONST char Far TheosFTypInd[] = "Indexed     ";
static ZCONST char Far TheosFTypR16[] = " 86 program ";
static ZCONST char Far TheosFTypP16[] = "286 program ";
static ZCONST char Far TheosFTypP32[] = "386 program ";
static ZCONST char Far TheosFTypUkn[] = "???         ";

static ZCONST char Far ExtraFieldTrunc[] = "\n\
  error: EF data block (type 0x%04x) size %u exceeds remaining extra field\n\
         space %u; block length has been truncated.\n";
static ZCONST char Far ExtraFields[] = "\n\
  The central-directory extra field contains:";
static ZCONST char Far ExtraFieldType[] = "\n\
  - A subfield with ID 0x%04x (%s) and %u data bytes";
static ZCONST char Far efPKSZ64[] = "PKWARE 64-bit sizes";
static ZCONST char Far efAV[] = "PKWARE AV";
static ZCONST char Far efOS2[] = "OS/2";
static ZCONST char Far efPKVMS[] = "PKWARE VMS";
static ZCONST char Far efPKWin32[] = "PKWARE Win32";
static ZCONST char Far efPKUnix[] = "PKWARE Unix";
static ZCONST char Far efIZVMS[] = "Info-ZIP VMS";
static ZCONST char Far efIZUnix[] = "old Info-ZIP Unix/OS2/NT";
static ZCONST char Far efIZUnix2[] = "Unix UID/GID (16-bit)";
static ZCONST char Far efIZUnix3[] = "Unix UID/GID (any size)";
static ZCONST char Far efTime[] = "universal time";
static ZCONST char Far efU8Path[] = "UTF8 path name";
static ZCONST char Far efU8Commnt[] = "UTF8 entry comment";
static ZCONST char Far efJLMac[] = "old Info-ZIP Macintosh";
static ZCONST char Far efMac3[] = "new Info-ZIP Macintosh";
static ZCONST char Far efZipIt[] = "ZipIt Macintosh";
static ZCONST char Far efSmartZip[] = "SmartZip Macintosh";
static ZCONST char Far efZipIt2[] = "ZipIt Macintosh (short)";
static ZCONST char Far efVMCMS[] = "VM/CMS";
static ZCONST char Far efMVS[] = "MVS";
static ZCONST char Far efACL[] = "OS/2 ACL";
static ZCONST char Far efNTSD[] = "Security Descriptor";
static ZCONST char Far efAtheOS[] = "AtheOS";
static ZCONST char Far efBeOS[] = "BeOS";
static ZCONST char Far efQDOS[] = "SMS/QDOS";
static ZCONST char Far efAOSVS[] = "AOS/VS";
static ZCONST char Far efSpark[] = "Acorn SparkFS";
static ZCONST char Far efMD5[] = "Fred Kantor MD5";
static ZCONST char Far efASiUnix[] = "ASi Unix";
static ZCONST char Far efTandem[] = "Tandem NSK";
static ZCONST char Far efTheos[] = "Theos";
static ZCONST char Far efUnknown[] = "unknown";

static ZCONST char Far OS2EAs[] = ".\n\
    The local extra field has %lu bytes of OS/2 extended attributes.\n\
    (May not match OS/2 \"dir\" amount due to storage method)";
static ZCONST char Far izVMSdata[] = ".  The extra\n\
    field is %s and has %u bytes of VMS %s information%s";
static ZCONST char Far izVMSstored[] = "stored";
static ZCONST char Far izVMSrleenc[] = "run-length encoded";
static ZCONST char Far izVMSdeflat[] = "deflated";
static ZCONST char Far izVMScunknw[] = "compressed(?)";
static ZCONST char Far *izVMScomp[4] =
  {izVMSstored, izVMSrleenc, izVMSdeflat, izVMScunknw};
static ZCONST char Far ACLdata[] = ".\n\
    The local extra field has %lu bytes of access control list information";
static ZCONST char Far NTSDData[] = ".\n\
    The local extra field has %lu bytes of NT security descriptor data";
static ZCONST char Far UTdata[] = ".\n\
    The local extra field has UTC/GMT %s time%s";
static ZCONST char Far UTmodification[] = "modification";
static ZCONST char Far UTaccess[] = "access";
static ZCONST char Far UTcreation[] = "creation";
static ZCONST char Far U8PthCmnComplete[] = ".\n\
    The UTF8 data of the extra field (V%u, ASCII name CRC `%.8lx') are:\n   ";
static ZCONST char Far U8PthCmnF24[] = ". The first\n\
    24 UTF8 bytes in the extra field (V%u, ASCII name CRC `%.8lx') are:\n   ";
static ZCONST char Far ZipItFname[] = ".\n\
    The Mac long filename is %s";
static ZCONST char Far Mac3data[] = ".\n\
    The local extra field has %lu bytes of %scompressed Macintosh\n\
    finder attributes";
 /* MacOSdata[] is used by EF_MAC3, EF_ZIPIT, EF_ZIPIT2 and EF_JLEE e. f. */
static ZCONST char Far MacOSdata[] = ".\n\
    The associated file has type code `%c%c%c%c' and creator code `%c%c%c%c'";
static ZCONST char Far MacOSdata1[] = ".\n\
    The associated file has type code `0x%lx' and creator code `0x%lx'";
static ZCONST char Far MacOSJLEEflags[] = ".\n    File is marked as %s";
static ZCONST char Far MacOS_RF[] = "Resource-fork";
static ZCONST char Far MacOS_DF[] = "Data-fork";
static ZCONST char Far MacOSMAC3flags[] = ".\n\
    File is marked as %s, File Dates are in %d Bit";
static ZCONST char Far AtheOSdata[] = ".\n\
    The local extra field has %lu bytes of %scompressed AtheOS file attributes";
static ZCONST char Far BeOSdata[] = ".\n\
    The local extra field has %lu bytes of %scompressed BeOS file attributes";
 /* The associated file has type code `%c%c%c%c' and creator code `%c%c%c%c'" */
static ZCONST char Far QDOSdata[] = ".\n\
    The QDOS extra field subtype is `%c%c%c%c'";
static ZCONST char Far AOSVSdata[] = ".\n\
    The AOS/VS extra field revision is %d.%d";
static ZCONST char Far TandemUnstr[] = "Unstructured";
static ZCONST char Far TandemRel[]   = "Relative";
static ZCONST char Far TandemEntry[] = "Entry Sequenced";
static ZCONST char Far TandemKey[]   = "Key Sequenced";
static ZCONST char Far TandemEdit[]  = "Edit";
static ZCONST char Far TandemObj[]  = "Object";
static ZCONST char Far *TandemFileformat[6] =
  {TandemUnstr, TandemRel, TandemEntry, TandemKey, TandemEdit, TandemObj};
static ZCONST char Far Tandemdata[] = ".\n\
    The file was originally a Tandem %s file, with file code %u";
static ZCONST char Far MD5data[] = ".\n\
    The 128-bit MD5 signature is %s";
#ifdef CMS_MVS
   static ZCONST char Far VmMvsExtraField[] = ".\n\
    The stored file open mode (FLDATA TYPE) is \"%s\"";
   static ZCONST char Far VmMvsInvalid[] = "[invalid]";
#endif /* CMS_MVS */

static ZCONST char Far First20[] = ".  The first\n    20 are:  ";
static ZCONST char Far ColonIndent[] = ":\n   ";
static ZCONST char Far efFormat[] = " %02x";

static ZCONST char Far lExtraFieldType[] = "\n\
  There %s a local extra field with ID 0x%04x (%s) and\n\
  %u data bytes (%s).\n";
static ZCONST char Far efIZuid[] =
  "GMT modification/access times and Unix UID/GID";
static ZCONST char Far efIZnouid[] = "GMT modification/access times only";


static ZCONST char Far NoFileComment[] = "\n  There is no file comment.\n";
static ZCONST char Far FileCommBegin[] = "\n\
------------------------- file comment begins ----------------------------\n";
static ZCONST char Far FileCommEnd[] = "\
-------------------------- file comment ends -----------------------------\n";

/* zi_time() strings */
static ZCONST char Far BogusFmt[] = "%03d";
static ZCONST char Far shtYMDHMTime[] = "%02u-%s-%02u %02u:%02u";
static ZCONST char Far lngYMDHMSTime[] = "%u %s %u %02u:%02u:%02u";
static ZCONST char Far DecimalTime[] = "%04u%02u%02u.%02u%02u%02u";
#ifdef USE_EF_UT_TIME
  static ZCONST char Far lngYMDHMSTimeError[] = "???? ??? ?? ??:??:??";
#endif





#ifndef WINDLL

/************************/
/*  Function zi_opts()  */
/************************/

int zi_opts(__G__ pargc, pargv)
    int *pargc;
    char ***pargv;
    __GDEF
{
    int    argc, error=FALSE;
    int    hflag_slmv=TRUE, hflag_2=FALSE;  /* diff options => diff defaults */
    int    tflag_slm=TRUE, tflag_2v=FALSE;
    int    explicit_h=FALSE, explicit_t=FALSE;

    char **args;


    /* used by get_option */
    unsigned long option; /* option ID returned by get_option */
    int argcnt = 0;       /* current argcnt in args */
    int argnum = 0;       /* arg number */
    int optchar = 0;      /* option state */
    char *value = NULL;   /* non-option arg, option value or NULL */
    int negative = 0;     /* 1 = option negated */
    int fna = 0;          /* current first non-opt arg */
    int optnum = 0;       /* index in table */
    int showhelp = 0;     /* for --commandline */

#ifdef USE_ICONV_MAPPING
# ifdef UNIX
    extern char OEM_CP[MAX_CP_NAME];
    extern char ISO_CP[MAX_CP_NAME];
# endif
#endif

    /* since get_option() returns xfiles and files one at a time, store them in
       linked lists until have them all */

    int file_count;
    struct file_list *next_file;

    /* files to extract */
    int in_files_count = 0;
    struct file_list *in_files = NULL;
    struct file_list *next_in_files = NULL;

    /* files to exclude in -x list */
    int in_xfiles_count = 0;
    struct file_list *in_xfiles = NULL;
    struct file_list *next_in_xfiles = NULL;

    G.wildzipfn = NULL;

    /* make copy of args that can use with insert_arg() used by get_option() */
    args = copy_args(*pargv, 0);


    /* Initialize lists */
    G.filespecs = 0;
    G.xfilespecs = 0;

#ifdef MACOS
    uO.lflag = LFLAG;         /* reset default on each call */
#endif
    G.extract_flag = FALSE;   /* zipinfo does not extract to disk */

    /*
    -------------------------------------------
    Process command line using get_option
    -------------------------------------------

    Each call to get_option() returns either a command
    line option and possible value or a non-option argument.
    Arguments are permuted so that all options (-r, -b temp)
    are returned before non-option arguments (zipfile).
    Returns 0 when nothing left to read.
    */

    /* set argnum = 0 on first call to init get_option */
    argnum = 0;

    /* get_option returns the option ID and updates parameters:
           args    - usually same as argv if no argument file support
           argcnt  - current argc for args
           value   - char* to value (free() when done with it) or NULL if no value
           negated - option was negated with trailing -
    */


/* Copied from unzip.c */
#define o_sc            0x103
#define o_so            0x104


    while ((option = get_option(ZIO, &args, &argcnt, &argnum,
                                &optchar, &value, &negative,
                                &fna, &optnum, 0)))
    {
        if(option == o_BAD_ERR) {
          return(PK_PARAM);
        }

        switch (option)
        {
                case '1':      /* shortest listing:  JUST filenames */
                    if (negative)
                        uO.lflag = -2;
                    else
                        uO.lflag = 1;
                    break;
                case '2':      /* just filenames, plus headers if specified */
                    if (negative)
                        uO.lflag = -2;
                    else
                        uO.lflag = 2;
                    break;
#ifndef CMS_MVS
                case ('C'):    /* -C:  match filenames case-insensitively */
                    if (negative)
                        uO.C_flag = FALSE;
                    else
                        uO.C_flag = TRUE;
                    break;
#endif /* !CMS_MVS */
                case 'h':      /* header line */
                    if (negative)
                        hflag_2 = hflag_slmv = FALSE;
                    else {
                        hflag_2 = hflag_slmv = explicit_h = TRUE;
                        if (uO.lflag == -1)
                            uO.lflag = 0;
                    }
                    break;
#ifdef USE_ICONV_MAPPING
# ifdef UNIX
                case ('I'):    /* -I:  map ISO name to internal */
                    strncpy(ISO_CP, value, sizeof(ISO_CP));
                    free(value);
                    break;
# endif
#endif
                case 'l':      /* longer form of "ls -l" type listing */
                    if (negative)
                        uO.lflag = -2;
                    else
                        uO.lflag = 5;
                    break;
                case 'm':      /* medium form of "ls -l" type listing */
                    if (negative)
                        uO.lflag = -2;
                    else
                        uO.lflag = 4;
                    break;
#ifdef MORE
                case 'M':      /* send output through built-in "more" */
                    if (negative)
                        G.M_flag = FALSE;
                    else
                        G.M_flag = TRUE;
                    break;
#endif
#ifdef USE_ICONV_MAPPING
# ifdef UNIX
                case ('O'):    /* -O:  map OEM name to internal */
                    strncpy(OEM_CP, value, sizeof(OEM_CP));
                    free(value);
                    break;
# endif
#endif
                case 's':      /* default:  shorter "ls -l" type listing */
                    if (negative)
                        uO.lflag = -2;
                    else
                        uO.lflag = 3;
                    break;
#ifndef SFX
                case (o_sc):   /* show processed command line and exit */
                    *pargc = -1;
                    showhelp = -3;
                    break;
#endif
                case 't':      /* totals line */
                    if (negative)
                        tflag_2v = tflag_slm = FALSE;
                    else {
                        tflag_2v = tflag_slm = explicit_t = TRUE;
                        if (uO.lflag == -1)
                            uO.lflag = 0;
                    }
                    break;
                case ('T'):    /* use (sortable) decimal time format */
                    if (negative)
                        uO.T_flag = FALSE;
                    else
                        uO.T_flag = TRUE;
                    break;
#ifdef UNICODE_SUPPORT
                case ('U'):    /* escape UTF-8, or disable UTF-8 support */
                    if (negative)
                        uO.U_flag = MAX(uO.U_flag - 1, 0);
                    else
                        uO.U_flag++;
                    break;
#endif /* UNICODE_SUPPORT */
                case 'v':      /* turbo-verbose listing */
                    if (negative)
                        uO.lflag = -2;
                    else
                        uO.lflag = 10;
                    break;
#ifdef WILD_STOP_AT_DIR
                case ('W'):    /* Wildcard interpretation (stop at '/'?) */
                    if (negative)
                        uO.W_flag = FALSE;
                    else
                        uO.W_flag = TRUE;
                    break;
#endif /* WILD_STOP_AT_DIR */
                case ('x'):    /* extract:  default */
                    /* add -x file to linked list */

                    if (in_xfiles_count == 0) {
                        /* first entry */
                        if ((in_xfiles = (struct file_list *) malloc(sizeof(struct file_list))) == NULL) {
                            Info(slide, 0x401, ((char *)slide, LoadFarString(NoMemArguments)));
                            return PK_MEM;
                        }
                        in_xfiles->name = value;
                        in_xfiles->next = NULL;
                        next_in_xfiles = in_xfiles;
                    } else {
                        /* add next entry */
                        if ((next_file = (struct file_list *) malloc(sizeof(struct file_list))) == NULL) {
                            Info(slide, 0x401, ((char *)slide, LoadFarString(NoMemArguments)));
                            return PK_MEM;
                        }
                        next_in_xfiles->next = next_file;
                        next_file->name = value;
                        next_file->next = NULL;
                        next_in_xfiles = next_file;
                    }
                    in_xfiles_count++;
                case 'z':      /* print zipfile comment */
                    if (negative)
                        uO.zflag = 0;
                    else
                        uO.zflag = 1;
                    break;
                case 'Z':      /* ZipInfo mode:  ignore */
                    break;
                case o_NON_OPTION_ARG:
                    /* not an option */
                    /* no more options as permuting */


                    if (G.wildzipfn == NULL) {
                        /* first non-option argument is zip file */
                        G.wildzipfn = value;

                    } else {
                        /* add include file to list */
                        if (in_files_count == 0) {
                            /* first entry */
                            if ((next_file = (struct file_list *) malloc(sizeof(struct file_list))) == NULL) {
                                Info(slide, 0x401, ((char *)slide, LoadFarString(NoMemArguments)));
                                return PK_MEM;
                            }
                            next_file->name = value;
                            next_file->next = NULL;
                            in_files = next_file;
                            next_in_files = next_file;
                        } else {
                            /* add next entry */
                            if ((next_file = (struct file_list *) malloc(sizeof(struct file_list))) == NULL) {
                                Info(slide, 0x401, ((char *)slide, LoadFarString(NoMemArguments)));
                                return PK_MEM;
                            }
                            next_in_files->next = next_file;
                            next_file->name = value;
                            next_file->next = NULL;
                            next_in_files = next_file;
                        }
                        in_files_count++;
                    }
                    break;
                default:
                    error = TRUE;
                    break;
        } /* switch */
    } /* get_option() */

    if (showhelp == -3) {
      show_commandline(args);
      return PK_OK;
    }

    /* convert files and xfiles lists to arrays */

    /* convert files list to array */
    if (in_files_count) {
      if ((G.pfnames = (char **) malloc((in_files_count + 1) * sizeof(char *))) == NULL) {
          Info(slide, 0x401, ((char *)slide, LoadFarString(NoMemArguments)));
          return PK_MEM;
      }
      file_count = 0;
      for (next_file = in_files; next_file;) {
          G.pfnames[file_count] = next_file->name;
          in_files = next_file;
          next_file = next_file->next;
          free(in_files);
          file_count++;
      }
      G.pfnames[file_count] = NULL;
      G.filespecs = in_files_count;
    }

    /* convert xfiles list to array */
    if (in_xfiles_count) {
      if ((G.pxnames = (char **) malloc((in_xfiles_count + 1) * sizeof(char *))) == NULL) {
          Info(slide, 0x401, ((char *)slide, LoadFarString(NoMemArguments)));
          return PK_MEM;
      }
      file_count = 0;
      for (next_file = in_xfiles; next_file;) {
          G.pxnames[file_count] = next_file->name;
          in_xfiles = next_file;
          next_file = next_file->next;
          free(in_xfiles);
          file_count++;
      }
      G.pxnames[file_count] = NULL;
      G.xfilespecs = in_xfiles_count;
    }

    if (in_files_count || in_xfiles_count) {
        G.process_all_files = FALSE;
    } else {
        G.process_all_files = TRUE;      /* for speed */
    }

    /* it's possible the arg count could have been changed by get_option() */
    argc = arg_count(args);

    if ((G.wildzipfn == NULL) || error) {
        argc = -1;      /* tell the caller to stop processing */
        *pargc = argc;
        *pargv = args;
        return USAGE(error);
    }

#ifdef MORE
    if (G.M_flag && !isatty(1))  /* stdout redirected: "more" func useless */
        G.M_flag = 0;
#endif

    /* if no listing options given (or all negated), or if only -h/-t given
     * with individual files specified, use default listing format */
    if ((uO.lflag < 0) || ((argc > 0) && (uO.lflag == 0)))
        uO.lflag = LFLAG;

    /* set header and totals flags to default or specified values */
    switch (uO.lflag) {
        case 0:   /* 0:  can only occur if either -t or -h explicitly given; */
        case 2:   /*  therefore set both flags equal to normally false value */
            uO.hflag = hflag_2;
            uO.tflag = tflag_2v;
            break;
        case 1:   /* only filenames, *always* */
            uO.hflag = FALSE;
            uO.tflag = FALSE;
            uO.zflag = FALSE;
            break;
        case 3:
        case 4:
        case 5:
            uO.hflag = ((argc > 0) && !explicit_h)? FALSE : hflag_slmv;
            uO.tflag = ((argc > 0) && !explicit_t)? FALSE : tflag_slm;
            break;
        case 10:
            uO.hflag = hflag_slmv;
            uO.tflag = tflag_2v;
            break;
    }

    *pargc = argc;
    *pargv = args;
    return 0;

} /* end function zi_opts() */

#endif /* !WINDLL */





/*******************************/
/*  Function zi_end_central()  */
/*******************************/

void zi_end_central(__G)
    __GDEF
{
/*---------------------------------------------------------------------------
    Print out various interesting things about the zipfile.
  ---------------------------------------------------------------------------*/

    if (uO.lflag > 9) {
        /* verbose format */
        Info(slide, 0, ((char *)slide, LoadFarString(EndCentDirRec)));
        Info(slide, 0, ((char *)slide, LoadFarString(LineSeparators)));

        Info(slide, 0, ((char *)slide, LoadFarString(ZipFSizeVerbose),
          FmZofft(G.ziplen, "11", NULL),
          FmZofft(G.ziplen, FZOFFT_HEX_DOT_WID, "X")));
        Info(slide, 0, ((char *)slide, LoadFarString(ActOffsetCentDir),
          FmZofft(G.real_ecrec_offset, "11", "u"),
          FmZofft(G.real_ecrec_offset, FZOFFT_HEX_DOT_WID, "X"),
          FmZofft(G.expect_ecrec_offset, "11", "u"),
          FmZofft(G.expect_ecrec_offset, FZOFFT_HEX_DOT_WID, "X")));

        if (G.ecrec.number_this_disk == 0) {
            Info(slide, 0, ((char *)slide, LoadFarString(SinglePartArchive1),
              FmZofft(G.ecrec.total_entries_central_dir, NULL, "u"),
              (G.ecrec.total_entries_central_dir == 1)? "entry" : "entries",
              FmZofft(G.ecrec.size_central_directory, NULL, "u"),
              FmZofft(G.ecrec.size_central_directory,
                      FZOFFT_HEX_DOT_WID, "X")));
            Info(slide, 0, ((char *)slide, LoadFarString(SinglePartArchive2),
              FmZofft(G.ecrec.offset_start_central_directory, NULL, "u"),
              FmZofft(G.ecrec.offset_start_central_directory,
                      FZOFFT_HEX_DOT_WID, "X")));
        } else {
            Info(slide, 0, ((char *)slide, LoadFarString(MultiPartArchive1),
              (ulg)(G.ecrec.number_this_disk + 1),
              (ulg)(G.ecrec.num_disk_start_cdir + 1)));
            Info(slide, 0, ((char *)slide, LoadFarString(MultiPartArchive2),
              FmZofft(G.ecrec.offset_start_central_directory, NULL, "u"),
              FmZofft(G.ecrec.offset_start_central_directory,
                      FZOFFT_HEX_DOT_WID, "X"),
              FmZofft(G.ecrec.size_central_directory, NULL, "u"),
              FmZofft(G.ecrec.size_central_directory,
                      FZOFFT_HEX_DOT_WID, "X")));
            Info(slide, 0, ((char *)slide, LoadFarString(MultiPartArchive3),
              FmZofft(G.ecrec.num_entries_centrl_dir_ths_disk, NULL, "u"),
              (G.ecrec.num_entries_centrl_dir_ths_disk == 1)? "is" : "are",
              FmZofft(G.ecrec.total_entries_central_dir, NULL, "u"),
              (G.ecrec.total_entries_central_dir == 1) ? "entry" : "entries"));
        }
    }
    else if (uO.hflag) {
        /* print zip file size and number of contained entries: */
        Info(slide, 0, ((char *)slide, LoadFarString(ZipInfHeader2),
          FmZofft(G.ziplen, NULL, NULL),
          FmZofft(G.ecrec.total_entries_central_dir, NULL, "u")));
    }

} /* end function zi_end_central() */





/************************/
/*  Function zipinfo()  */
/************************/

int zipinfo(__G)   /* return PK-type error code */
    __GDEF
{
    int do_this_file=FALSE, error, error_in_archive=PK_COOL;
    int *fn_matched=NULL, *xn_matched=NULL;
    ulg j, members=0L;
    zusz_t tot_csize=0L, tot_ucsize=0L;
    zusz_t endprev;   /* buffers end of previous entry for zi_long()'s check
                       *  of extra bytes */


/*---------------------------------------------------------------------------
    Malloc space for check on unmatched filespecs (no big deal if one or both
    are NULL).
  ---------------------------------------------------------------------------*/

    if (G.filespecs > 0  &&
        (fn_matched=(int *)malloc(G.filespecs*sizeof(int))) != NULL)
        for (j = 0;  j < G.filespecs;  ++j)
            fn_matched[j] = FALSE;

    if (G.xfilespecs > 0  &&
        (xn_matched=(int *)malloc(G.xfilespecs*sizeof(int))) != NULL)
        for (j = 0;  j < G.xfilespecs;  ++j)
            xn_matched[j] = FALSE;

/*---------------------------------------------------------------------------
    Set file pointer to start of central directory, then loop through cen-
    tral directory entries.  Check that directory-entry signature bytes are
    actually there (just a precaution), then process the entry.  We know
    the entire central directory is on this disk:  we wouldn't have any of
    this information unless the end-of-central-directory record was on this
    disk, and we wouldn't have gotten to this routine unless this is also
    the disk on which the central directory starts.  In practice, this had
    better be the *only* disk in the archive, but maybe someday we'll add
    multi-disk support.
  ---------------------------------------------------------------------------*/

    uO.L_flag = FALSE;      /* zipinfo mode: never convert name to lowercase */
    G.pInfo = G.info;       /* (re-)initialize, (just to make sure) */
    G.pInfo->textmode = 0;  /* so one can read on screen (is this ever used?) */

    /* reset endprev for new zipfile; account for multi-part archives (?) */
    endprev = (G.crec.relative_offset_local_header == 4L)? 4L : 0L;


    for (j = 1L;; j++) {
        if (readbuf(__G__ G.sig, 4) == 0) {
            error_in_archive = PK_EOF;
            break;
        }
        if (memcmp(G.sig, central_hdr_sig, 4)) {  /* is it a CentDir entry? */
            /* no new central directory entry
             * -> is the number of processed entries compatible with the
             *    number of entries as stored in the end_central record?
             */
            if (((j - 1) &
                 (ulg)(G.ecrec.have_ecr64 ? MASK_ZUCN64 : MASK_ZUCN16))
                == (ulg)G.ecrec.total_entries_central_dir)
            {
                /* "j modulus 4T/64k" matches the reported 64/16-bit-unsigned
                 * number of directory entries -> probably, the regular
                 * end of the central directory has been reached
                 */
                break;
            } else {
                Info(slide, 0x401,
                     ((char *)slide, LoadFarString(CentSigMsg), j));
                Info(slide, 0x401,
                     ((char *)slide, LoadFarString(ReportMsg)));
                error_in_archive = PK_BADERR;   /* sig not found */
                break;
            }
        }
        /* process_cdir_file_hdr() sets pInfo->hostnum, pInfo->lcflag, ...: */
        if ((error = process_cdir_file_hdr(__G)) != PK_COOL) {
            error_in_archive = error;   /* only PK_EOF defined */
            break;
        }

        if ((error = do_string(__G__ G.crec.filename_length, DS_FN)) !=
             PK_COOL)
        {
          if (error > error_in_archive)
              error_in_archive = error;
          if (error > PK_WARN)        /* fatal */
              break;
        }

        if (!G.process_all_files) {   /* check if specified on command line */
            unsigned i;

            if (G.filespecs == 0)
                do_this_file = TRUE;
            else {  /* check if this entry matches an `include' argument */
                do_this_file = FALSE;
                for (i = 0; i < G.filespecs; i++)
                    if (match(G.filename, G.pfnames[i], uO.C_flag WISEP)) {
                        do_this_file = TRUE;
                        if (fn_matched)
                            fn_matched[i] = TRUE;
                        break;       /* found match, so stop looping */
                    }
            }
            if (do_this_file) {  /* check if this is an excluded file */
                for (i = 0; i < G.xfilespecs; i++)
                    if (match(G.filename, G.pxnames[i], uO.C_flag WISEP)) {
                        do_this_file = FALSE;  /* ^-- ignore case in match */
                        if (xn_matched)
                            xn_matched[i] = TRUE;
                        break;
                    }
            }
        }

    /*-----------------------------------------------------------------------
        If current file was specified on command line, or if no names were
        specified, do the listing for this file.  Otherwise, get rid of the
        file comment and go back for the next file.
      -----------------------------------------------------------------------*/

        if (G.process_all_files || do_this_file) {

            /* Read the extra field, if any.  The extra field info is required
             * for resolving the Zip64 sizes/offsets and may be used in more
             * analysis of the entry below.
             */
            if ((error = do_string(__G__ G.crec.extra_field_length,
                                   EXTRA_FIELD)) != 0)
            {
                if (G.extra_field != NULL) {
                    free(G.extra_field);
                    G.extra_field = NULL;
                }
                error_in_archive = error;
                /* The premature return in case of a "fatal" error (PK_EOF) is
                 * delayed until we analyze the extra field contents.
                 * This allows us to display all the other info that has been
                 * successfully read in.
                 */
            }

            switch (uO.lflag) {
                case 1:
                case 2:
                    fnprint(__G);
                    SKIP_(G.crec.file_comment_length)
                    break;

                case 3:
                case 4:
                case 5:
                    if ((error = zi_short(__G)) != PK_COOL) {
                        error_in_archive = error;   /* might be warning */
                    }
                    break;

                case 10:
                    Info(slide, 0, ((char *)slide,
                      LoadFarString(CentralDirEntry), j));
                    if ((error = zi_long(__G__ &endprev,
                                         error_in_archive)) != PK_COOL) {
                        error_in_archive = error;   /* might be warning */
                    }
                    break;

                default:
                    SKIP_(G.crec.file_comment_length)
                    break;

            } /* end switch (lflag) */
            if (error > PK_WARN)        /* fatal */
                break;

            tot_csize += G.crec.csize;
            tot_ucsize += G.crec.ucsize;
            if (G.crec.general_purpose_bit_flag & 1)
                tot_csize -= 12;   /* don't count encryption header */
            ++members;

#ifdef DLL
            if ((G.statreportcb != NULL) &&
                (*G.statreportcb)(__G__ UZ_ST_FINISH_MEMBER, G.zipfn,
                                  G.filename, (zvoid *)&G.crec.ucsize)) {
                /* cancel operation by user request */
                error_in_archive = IZ_CTRLC;
                break;
            }
#endif
#ifdef MACOS  /* MacOS is no preemptive OS, thus call event-handling by hand */
            UserStop();
#endif

        } else {        /* not listing this file */
            SKIP_(G.crec.extra_field_length)
            SKIP_(G.crec.file_comment_length)
            if (endprev != 0) endprev = 0;

        } /* end if (list member?) */

    } /* end for-loop (j: member files) */

/*---------------------------------------------------------------------------
    Check that we actually found requested files; if so, print totals.
  ---------------------------------------------------------------------------*/

    if ((error_in_archive <= PK_WARN) && uO.tflag) {
        char *sgn = "";
        int cfactor = ratio(tot_ucsize, tot_csize);

        if (cfactor < 0) {
            sgn = "-";
            cfactor = -cfactor;
        }
        Info(slide, 0, ((char *)slide, LoadFarString(ZipfileStats),
          members, (members==1L)? nullStr:PlurSufx,
          FmZofft(tot_ucsize, NULL, "u"),
          FmZofft(tot_csize, NULL, "u"),
          sgn, cfactor/10, cfactor%10));
    }

/*---------------------------------------------------------------------------
    Check for unmatched filespecs on command line and print warning if any
    found.
  ---------------------------------------------------------------------------*/

    if (fn_matched) {
        if (error_in_archive <= PK_WARN)
            for (j = 0;  j < G.filespecs;  ++j)
                if (!fn_matched[j])
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarString(FilenameNotMatched), G.pfnames[j]));
        free((zvoid *)fn_matched);
    }
    if (xn_matched) {
        if (error_in_archive <= PK_WARN)
            for (j = 0;  j < G.xfilespecs;  ++j)
                if (!xn_matched[j])
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarString(ExclFilenameNotMatched), G.pxnames[j]));
        free((zvoid *)xn_matched);
    }


    /* Skip the following checks in case of a premature listing break. */
    if (error_in_archive <= PK_WARN) {

/*---------------------------------------------------------------------------
    Double check that we're back at the end-of-central-directory record.
  ---------------------------------------------------------------------------*/

        if ( (memcmp(G.sig,
                     (G.ecrec.have_ecr64 ?
                      end_central64_sig : end_central_sig),
                     4) != 0)
            && (!G.ecrec.is_zip64_archive)
            && (memcmp(G.sig, end_central_sig, 4) != 0)
           ) {          /* just to make sure again */
            Info(slide, 0x401, ((char *)slide, LoadFarString(EndSigMsg)));
            error_in_archive = PK_WARN;   /* didn't find sig */
        }

        /* Set specific return code when no files have been found. */
        if (members == 0L && error_in_archive <= PK_WARN)
            error_in_archive = PK_FIND;

        if (uO.lflag >= 10)
            (*G.message)((zvoid *)&G, (uch *)"\n", 1L, 0);
    }

    return error_in_archive;

} /* end function zipinfo() */





/************************/
/*  Function zi_long()  */
/************************/

static int zi_long(__G__ pEndprev, error_in_archive)
    /* return PK-type error code */
    __GDEF
    zusz_t *pEndprev;                /* for zi_long() check of extra bytes */
    int error_in_archive;            /* may signal premature return */
{
#ifdef USE_EF_UT_TIME
    iztimes z_utime;
#endif
    int  error;
    unsigned  hostnum, hostver, extnum, extver, methid, methnum, xattr;
    char workspace[12], attribs[22];
    ZCONST char *varmsg_str;
    char unkn[16];
    static ZCONST char Far *os[NUM_HOSTS] = {
        OS_FAT, OS_Amiga, OS_VMS, OS_Unix, OS_VMCMS, OS_AtariST, OS_HPFS,
        OS_Macintosh, OS_ZSystem, OS_CPM, OS_TOPS20, OS_NTFS, OS_QDOS,
        OS_Acorn, OS_VFAT, OS_MVS, OS_BeOS, OS_Tandem, OS_Theos, OS_MacDarwin,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        OS_AtheOS
    };
    static ZCONST char Far *method[NUM_METHODS] = {
        MthdNone, MthdShrunk, MthdRedF1, MthdRedF2, MthdRedF3, MthdRedF4,
        MthdImplode, MthdToken, MthdDeflate, MthdDeflat64, MthdDCLImplode,
        MthdBZip2, MthdLZMA, MthdTerse, MthdLZ77, MthdWavPack, MthdPPMd
    };
    static ZCONST char Far *dtypelng[4] = {
        DeflNorm, DeflMax, DeflFast, DeflSFast
    };


/*---------------------------------------------------------------------------
    Check whether there's any extra space inside the zipfile.  If *pEndprev is
    zero, it's probably a signal that OS/2 extra fields are involved (with
    unknown compressed size).  We won't worry about prepended junk here...
  ---------------------------------------------------------------------------*/

    if (G.crec.relative_offset_local_header != *pEndprev && *pEndprev > 0L) {
        /*  GRR DEBUG
        Info(slide, 0, ((char *)slide,
          "  [crec.relative_offset_local_header = %lu, endprev = %lu]\n",
          G.crec.relative_offset_local_header, *pEndprev));
         */
        Info(slide, 0, ((char *)slide, LoadFarString(ExtraBytesPreceding),
          FmZofft((G.crec.relative_offset_local_header - (*pEndprev)),
          NULL, NULL)));
    }

    /* calculate endprev for next time around (problem:  extra fields may
     * differ in length between local and central-directory records) */
    *pEndprev = G.crec.relative_offset_local_header + (4L + LREC_SIZE) +
      G.crec.filename_length + G.crec.extra_field_length + G.crec.csize;

/*---------------------------------------------------------------------------
    Print out various interesting things about the compressed file.
  ---------------------------------------------------------------------------*/

    hostnum = (unsigned)(G.pInfo->hostnum);
    hostver = (unsigned)(G.pInfo->hostver);
    extnum = (unsigned)MIN(G.crec.version_needed_to_extract[1], NUM_HOSTS);
    extver = (unsigned)G.crec.version_needed_to_extract[0];
    methid = (unsigned)G.crec.compression_method;
    methnum = find_compr_idx(G.crec.compression_method);

    (*G.message)((zvoid *)&G, (uch *)"  ", 2L, 0);  fnprint(__G);

    Info(slide, 0, ((char *)slide, LoadFarString(LocalHeaderOffset),
      FmZofft(G.crec.relative_offset_local_header, NULL, "u"),
      FmZofft(G.crec.relative_offset_local_header, FZOFFT_HEX_DOT_WID, "X")));

    if (hostnum >= NUM_HOSTS) {
        sprintf(unkn, LoadFarString(UnknownNo),
                (int)G.crec.version_made_by[1]);
        varmsg_str = unkn;
    } else {
        varmsg_str = LoadFarStringSmall(os[hostnum]);
#ifdef OLD_THEOS_EXTRA
        if (hostnum == FS_VFAT_ && hostver == 20) {
            /* entry made by old non-official THEOS port zip archive */
            varmsg_str = LoadFarStringSmall(OS_TheosOld);
        }
#endif /* OLD_THEOS_EXTRA */
    }
    Info(slide, 0, ((char *)slide, LoadFarString(HostOS), varmsg_str));
    Info(slide, 0, ((char *)slide, LoadFarString(EncodeSWVer), hostver/10,
      hostver%10));

    if ((extnum >= NUM_HOSTS) || (os[extnum] == NULL)) {
        sprintf(unkn, LoadFarString(UnknownNo),
                (int)G.crec.version_needed_to_extract[1]);
        varmsg_str = unkn;
    } else {
        varmsg_str = LoadFarStringSmall(os[extnum]);
    }
    Info(slide, 0, ((char *)slide, LoadFarString(MinOSCompReq), varmsg_str));
    Info(slide, 0, ((char *)slide, LoadFarString(MinSWVerReq), extver/10,
      extver%10));

    if (methnum >= NUM_METHODS) {
        sprintf(unkn, LoadFarString(UnknownNo), G.crec.compression_method);
        varmsg_str = unkn;
    } else {
        varmsg_str = LoadFarStringSmall(method[methnum]);
    }
    Info(slide, 0, ((char *)slide, LoadFarString(CompressMethod), varmsg_str));
    if (methid == IMPLODED) {
        Info(slide, 0, ((char *)slide, LoadFarString(SlideWindowSizeImplode),
          (G.crec.general_purpose_bit_flag & 2)? '8' : '4'));
        Info(slide, 0, ((char *)slide, LoadFarString(ShannonFanoTrees),
          (G.crec.general_purpose_bit_flag & 4)? '3' : '2'));
    } else if (methid == DEFLATED || methid == ENHDEFLATED) {
        ush  dnum=(ush)((G.crec.general_purpose_bit_flag>>1) & 3);

        Info(slide, 0, ((char *)slide, LoadFarString(CompressSubtype),
          LoadFarStringSmall(dtypelng[dnum])));
    }

    Info(slide, 0, ((char *)slide, LoadFarString(FileSecurity),
      (G.crec.general_purpose_bit_flag & 1) ? nullStr : "not "));
    Info(slide, 0, ((char *)slide, LoadFarString(ExtendedLocalHdr),
      (G.crec.general_purpose_bit_flag & 8) ? "yes" : "no"));
    /* print upper 3 bits for amusement? */

    /* For printing of date & time, a "char d_t_buf[21]" is required.
     * To save stack space, we reuse the "char attribs[22]" buffer which
     * is not used yet.
     */
#   define d_t_buf attribs

    zi_time(__G__ &G.crec.last_mod_dos_datetime, NULL, d_t_buf);
    Info(slide, 0, ((char *)slide, LoadFarString(FileModDate), d_t_buf));
#ifdef USE_EF_UT_TIME
    if (G.extra_field &&
#ifdef IZ_CHECK_TZ
        G.tz_is_valid &&
#endif
        (ef_scan_for_izux(G.extra_field, G.crec.extra_field_length, 1,
                          G.crec.last_mod_dos_datetime, &z_utime, NULL)
         & EB_UT_FL_MTIME))
    {
        TIMET_TO_NATIVE(z_utime.mtime)   /* NOP unless MSC 7.0 or Macintosh */
        d_t_buf[0] = (char)0;               /* signal "show local time" */
        zi_time(__G__ &G.crec.last_mod_dos_datetime, &(z_utime.mtime), d_t_buf);
        Info(slide, 0, ((char *)slide, LoadFarString(UT_FileModDate),
          d_t_buf, LoadFarStringSmall(LocalTime)));
#ifndef NO_GMTIME
        d_t_buf[0] = (char)1;           /* signal "show UTC (GMT) time" */
        zi_time(__G__ &G.crec.last_mod_dos_datetime, &(z_utime.mtime), d_t_buf);
        Info(slide, 0, ((char *)slide, LoadFarString(UT_FileModDate),
          d_t_buf, LoadFarStringSmall(GMTime)));
#endif /* !NO_GMTIME */
    }
#endif /* USE_EF_UT_TIME */

    Info(slide, 0, ((char *)slide, LoadFarString(CRC32Value), G.crec.crc32));
    Info(slide, 0, ((char *)slide, LoadFarString(CompressedFileSize),
      FmZofft(G.crec.csize, NULL, "u")));
    Info(slide, 0, ((char *)slide, LoadFarString(UncompressedFileSize),
      FmZofft(G.crec.ucsize, NULL, "u")));
    Info(slide, 0, ((char *)slide, LoadFarString(FilenameLength),
      G.crec.filename_length));
    Info(slide, 0, ((char *)slide, LoadFarString(ExtraFieldLength),
      G.crec.extra_field_length));
    Info(slide, 0, ((char *)slide, LoadFarString(FileCommentLength),
      G.crec.file_comment_length));
    Info(slide, 0, ((char *)slide, LoadFarString(FileDiskNum),
      (ulg)(G.crec.disk_number_start + 1)));
    Info(slide, 0, ((char *)slide, LoadFarString(ApparentFileType),
      (G.crec.internal_file_attributes & 1)? "text"
         : (G.crec.internal_file_attributes & 2)? "ebcdic"
              : "binary"));             /* changed to accept EBCDIC */
#ifdef ATARI
    printf("  external file attributes (hex):                   %.8lx\n",
      G.crec.external_file_attributes);
#endif
    xattr = (unsigned)((G.crec.external_file_attributes >> 16) & 0xFFFF);
    if (hostnum == VMS_) {
        char   *p=attribs, *q=attribs+1;
        int    i, j, k;

        for (k = 0;  k < 12;  ++k)
            workspace[k] = 0;
        if (xattr & VMS_IRUSR)
            workspace[0] = 'R';
        if (xattr & VMS_IWUSR) {
            workspace[1] = 'W';
            workspace[3] = 'D';
        }
        if (xattr & VMS_IXUSR)
            workspace[2] = 'E';
        if (xattr & VMS_IRGRP)
            workspace[4] = 'R';
        if (xattr & VMS_IWGRP) {
            workspace[5] = 'W';
            workspace[7] = 'D';
        }
        if (xattr & VMS_IXGRP)
            workspace[6] = 'E';
        if (xattr & VMS_IROTH)
            workspace[8] = 'R';
        if (xattr & VMS_IWOTH) {
            workspace[9] = 'W';
            workspace[11] = 'D';
        }
        if (xattr & VMS_IXOTH)
            workspace[10] = 'E';

        *p++ = '(';
        for (k = j = 0;  j < 3;  ++j) {    /* loop over groups of permissions */
            for (i = 0;  i < 4;  ++i, ++k)  /* loop over perms within a group */
                if (workspace[k])
                    *p++ = workspace[k];
            *p++ = ',';                       /* group separator */
            if (j == 0)
                while ((*p++ = *q++) != ',')
                    ;                         /* system, owner perms are same */
        }
        *p-- = '\0';
        *p = ')';   /* overwrite last comma */
        Info(slide, 0, ((char *)slide, LoadFarString(VMSFileAttributes), xattr,
          attribs));

    } else if (hostnum == AMIGA_) {
        switch (xattr & AMI_IFMT) {
            case AMI_IFDIR:  attribs[0] = 'd';  break;
            case AMI_IFREG:  attribs[0] = '-';  break;
            default:         attribs[0] = '?';  break;
        }
        attribs[1] = (xattr & AMI_IHIDDEN)?   'h' : '-';
        attribs[2] = (xattr & AMI_ISCRIPT)?   's' : '-';
        attribs[3] = (xattr & AMI_IPURE)?     'p' : '-';
        attribs[4] = (xattr & AMI_IARCHIVE)?  'a' : '-';
        attribs[5] = (xattr & AMI_IREAD)?     'r' : '-';
        attribs[6] = (xattr & AMI_IWRITE)?    'w' : '-';
        attribs[7] = (xattr & AMI_IEXECUTE)?  'e' : '-';
        attribs[8] = (xattr & AMI_IDELETE)?   'd' : '-';
        attribs[9] = 0;   /* better dlm the string */
        Info(slide, 0, ((char *)slide, LoadFarString(AmigaFileAttributes),
          xattr, attribs));

    } else if (hostnum == THEOS_) {
        ZCONST char Far *fpFtyp;

        switch (xattr & THS_IFMT) {
            case THS_IFLIB:  fpFtyp = TheosFTypLib;  break;
            case THS_IFDIR:  fpFtyp = TheosFTypDir;  break;
            case THS_IFREG:  fpFtyp = TheosFTypReg;  break;
            case THS_IFREL:  fpFtyp = TheosFTypRel;  break;
            case THS_IFKEY:  fpFtyp = TheosFTypKey;  break;
            case THS_IFIND:  fpFtyp = TheosFTypInd;  break;
            case THS_IFR16:  fpFtyp = TheosFTypR16;  break;
            case THS_IFP16:  fpFtyp = TheosFTypP16;  break;
            case THS_IFP32:  fpFtyp = TheosFTypP32;  break;
            default:         fpFtyp = TheosFTypUkn;  break;
        }
        strcpy(attribs, LoadFarStringSmall(fpFtyp));
        attribs[12] = (xattr & THS_INHID) ? '.' : 'H';
        attribs[13] = (xattr & THS_IMODF) ? '.' : 'M';
        attribs[14] = (xattr & THS_IWOTH) ? '.' : 'W';
        attribs[15] = (xattr & THS_IROTH) ? '.' : 'R';
        attribs[16] = (xattr & THS_IEUSR) ? '.' : 'E';
        attribs[17] = (xattr & THS_IXUSR) ? '.' : 'X';
        attribs[18] = (xattr & THS_IWUSR) ? '.' : 'W';
        attribs[19] = (xattr & THS_IRUSR) ? '.' : 'R';
        attribs[20] = 0;
        Info(slide, 0, ((char *)slide, LoadFarString(TheosFileAttributes),
          xattr, attribs));

#ifdef OLD_THEOS_EXTRA
    } else if (hostnum == FS_VFAT_ && hostver == 20) {
        /* process old non-official THEOS port zip archive */
        ZCONST char Far *fpFtyp;

        switch (xattr & _THS_IFMT) {
            case _THS_IFLIB:  fpFtyp = TheosFTypLib;  break;
            case _THS_IFDIR:  fpFtyp = TheosFTypDir;  break;
            case _THS_IFREG:  fpFtyp = TheosFTypReg;  break;
            case _THS_IODRC:  fpFtyp = TheosFTypRel;  break;
            case _THS_IOKEY:  fpFtyp = TheosFTypKey;  break;
            case _THS_IOIND:  fpFtyp = TheosFTypInd;  break;
            case _THS_IOPRG:  fpFtyp = TheosFTypR16;  break;
            case _THS_IO286:  fpFtyp = TheosFTypP16;  break;
            case _THS_IO386:  fpFtyp = TheosFTypP32;  break;
            default:         fpFtyp = TheosFTypUkn;  break;
        }
        strcpy(attribs, LoadFarStringSmall(fpFtyp));
        attribs[12] = (xattr & _THS_HIDDN) ? 'H' : '.';
        attribs[13] = (xattr & _THS_IXOTH) ? '.' : 'X';
        attribs[14] = (xattr & _THS_IWOTH) ? '.' : 'W';
        attribs[15] = (xattr & _THS_IROTH) ? '.' : 'R';
        attribs[16] = (xattr & _THS_IEUSR) ? '.' : 'E';
        attribs[17] = (xattr & _THS_IXUSR) ? '.' : 'X';
        attribs[18] = (xattr & _THS_IWUSR) ? '.' : 'W';
        attribs[19] = (xattr & _THS_IRUSR) ? '.' : 'R';
        attribs[20] = 0;
        Info(slide, 0, ((char *)slide, LoadFarString(TheosFileAttributes),
          xattr, attribs));
#endif /* OLD_THEOS_EXTRA */

    } else if ((hostnum != FS_FAT_) && (hostnum != FS_HPFS_) &&
               (hostnum != FS_NTFS_) && (hostnum != FS_VFAT_) &&
               (hostnum != ACORN_) &&
               (hostnum != VM_CMS_) && (hostnum != MVS_))
    {                                 /* assume Unix-like */
        switch ((unsigned)(xattr & UNX_IFMT)) {
            case (unsigned)UNX_IFDIR:   attribs[0] = 'd';  break;
            case (unsigned)UNX_IFREG:   attribs[0] = '-';  break;
            case (unsigned)UNX_IFLNK:   attribs[0] = 'l';  break;
            case (unsigned)UNX_IFBLK:   attribs[0] = 'b';  break;
            case (unsigned)UNX_IFCHR:   attribs[0] = 'c';  break;
            case (unsigned)UNX_IFIFO:   attribs[0] = 'p';  break;
            case (unsigned)UNX_IFSOCK:  attribs[0] = 's';  break;
            default:          attribs[0] = '?';  break;
        }
        attribs[1] = (xattr & UNX_IRUSR)? 'r' : '-';
        attribs[4] = (xattr & UNX_IRGRP)? 'r' : '-';
        attribs[7] = (xattr & UNX_IROTH)? 'r' : '-';

        attribs[2] = (xattr & UNX_IWUSR)? 'w' : '-';
        attribs[5] = (xattr & UNX_IWGRP)? 'w' : '-';
        attribs[8] = (xattr & UNX_IWOTH)? 'w' : '-';

        if (xattr & UNX_IXUSR)
            attribs[3] = (xattr & UNX_ISUID)? 's' : 'x';
        else
            attribs[3] = (xattr & UNX_ISUID)? 'S' : '-';   /* S = undefined */
        if (xattr & UNX_IXGRP)
            attribs[6] = (xattr & UNX_ISGID)? 's' : 'x';   /* == UNX_ENFMT */
        else
            attribs[6] = (xattr & UNX_ISGID)? 'l' : '-';
        if (xattr & UNX_IXOTH)
            attribs[9] = (xattr & UNX_ISVTX)? 't' : 'x';   /* "sticky bit" */
        else
            attribs[9] = (xattr & UNX_ISVTX)? 'T' : '-';   /* T = undefined */
        attribs[10] = 0;

        Info(slide, 0, ((char *)slide, LoadFarString(UnixFileAttributes), xattr,
          attribs));

    } else {
        Info(slide, 0, ((char *)slide, LoadFarString(NonMSDOSFileAttributes),
            G.crec.external_file_attributes >> 8));

    } /* endif (hostnum: external attributes format) */

    if ((xattr=(unsigned)(G.crec.external_file_attributes & 0xFF)) == 0)
        Info(slide, 0, ((char *)slide, LoadFarString(MSDOSFileAttributes),
          xattr));
    else if (xattr == 1)
        Info(slide, 0, ((char *)slide, LoadFarString(MSDOSFileAttributesRO),
          xattr));
    else
        Info(slide, 0, ((char *)slide, LoadFarString(MSDOSFileAttributesAlpha),
          xattr, (xattr&1)? "rdo " : nullStr,
          (xattr&2)? "hid " : nullStr,
          (xattr&4)? "sys " : nullStr,
          (xattr&8)? "lab " : nullStr,
          (xattr&16)? "dir " : nullStr,
          (xattr&32)? "arc " : nullStr,
          (xattr&64)? "lnk " : nullStr,
          (xattr&128)? "exe" : nullStr));

/*---------------------------------------------------------------------------
    Analyze the extra field, if any, and print the file comment, if any (the
    filename has already been printed, above).  That finishes up this file
    entry...
  ---------------------------------------------------------------------------*/

    if (G.crec.extra_field_length > 0) {
        uch *ef_ptr = G.extra_field;
        ush ef_len = G.crec.extra_field_length;
        ush eb_id, eb_datalen;
        ZCONST char Far *ef_fieldname;

        if (error_in_archive > PK_WARN)   /* fatal:  can't continue */
            /* delayed "fatal error" return from extra field reading */
            return error_in_archive;
        if (G.extra_field == (uch *)NULL)
            return PK_ERR;   /* not consistent with crec length */

        Info(slide, 0, ((char *)slide, LoadFarString(ExtraFields)));

        while (ef_len >= EB_HEADSIZE) {
            eb_id = makeword(&ef_ptr[EB_ID]);
            eb_datalen = makeword(&ef_ptr[EB_LEN]);
            ef_ptr += EB_HEADSIZE;
            ef_len -= EB_HEADSIZE;

            if (eb_datalen > (ush)ef_len) {
                Info(slide, 0x421, ((char *)slide,
                  LoadFarString(ExtraFieldTrunc), eb_id, eb_datalen, ef_len));
                eb_datalen = ef_len;
            }

            switch (eb_id) {
                case EF_PKSZ64:
                    ef_fieldname = efPKSZ64;
                    if ((G.crec.relative_offset_local_header
                         & (~(zusz_t)0xFFFFFFFFL)) != 0) {
                        /* Subtract the size of the 64bit local offset from
                           the local e.f. size, local Z64 e.f. block has no
                           offset; when only local offset present, the entire
                           local PKSZ64 block is missing. */
                        *pEndprev -= (eb_datalen == 8 ? 12 : 8);
                    }
                    break;
                case EF_AV:
                    ef_fieldname = efAV;
                    break;
                case EF_OS2:
                    ef_fieldname = efOS2;
                    break;
                case EF_ACL:
                    ef_fieldname = efACL;
                    break;
                case EF_NTSD:
                    ef_fieldname = efNTSD;
                    break;
                case EF_PKVMS:
                    ef_fieldname = efPKVMS;
                    break;
                case EF_IZVMS:
                    ef_fieldname = efIZVMS;
                    break;
                case EF_PKW32:
                    ef_fieldname = efPKWin32;
                    break;
                case EF_PKUNIX:
                    ef_fieldname = efPKUnix;
                    break;
                case EF_IZUNIX:
                    ef_fieldname = efIZUnix;
                    if (hostnum == UNIX_ && *pEndprev > 0L)
                        *pEndprev += 4L;  /* also have UID/GID in local copy */
                    break;
                case EF_IZUNIX2:
                    ef_fieldname = efIZUnix2;
                    if (*pEndprev > 0L)
                        *pEndprev += 4L;  /* 4 byte UID/GID in local copy */
                    break;
                case EF_IZUNIX3:
                    ef_fieldname = efIZUnix3;
#if 0
                    if (*pEndprev > 0L)
                        *pEndprev += 4L;  /* 4 byte UID/GID in local copy */
#endif
                    break;
                case EF_TIME:
                    ef_fieldname = efTime;
                    break;
                case EF_UNIPATH:
                    ef_fieldname = efU8Path;
                    break;
                case EF_UNICOMNT:
                    ef_fieldname = efU8Commnt;
                    break;
                case EF_MAC3:
                    ef_fieldname = efMac3;
                    break;
                case EF_JLMAC:
                    ef_fieldname = efJLMac;
                    break;
                case EF_ZIPIT:
                    ef_fieldname = efZipIt;
                    break;
                case EF_ZIPIT2:
                    ef_fieldname = efZipIt2;
                    break;
                case EF_VMCMS:
                    ef_fieldname = efVMCMS;
                    break;
                case EF_MVS:
                    ef_fieldname = efMVS;
                    break;
                case EF_ATHEOS:
                    ef_fieldname = efAtheOS;
                    break;
                case EF_BEOS:
                    ef_fieldname = efBeOS;
                    break;
                case EF_QDOS:
                    ef_fieldname = efQDOS;
                    break;
                case EF_AOSVS:
                    ef_fieldname = efAOSVS;
                    break;
                case EF_SPARK:   /* from RISC OS */
                    ef_fieldname = efSpark;
                    break;
                case EF_MD5:
                    ef_fieldname = efMD5;
                    break;
                case EF_ASIUNIX:
                    ef_fieldname = efASiUnix;
                    break;
                case EF_TANDEM:
                    ef_fieldname = efTandem;
                    break;
                case EF_SMARTZIP:
                    ef_fieldname = efSmartZip;
                    break;
                case EF_THEOS:
#ifdef OLD_THEOS_EXTRA
                case EF_THEOSO:
#endif
                    ef_fieldname = efTheos;
                    break;
                default:
                    ef_fieldname = efUnknown;
                    break;
            }
            Info(slide, 0, ((char *)slide, LoadFarString(ExtraFieldType),
                 eb_id, LoadFarStringSmall(ef_fieldname), eb_datalen));

            /* additional, field-specific information: */
            switch (eb_id) {
                case EF_OS2:
                case EF_ACL:
                    if (eb_datalen >= EB_OS2_HLEN) {
                        if (eb_id == EF_OS2)
                            ef_fieldname = OS2EAs;
                        else
                            ef_fieldname = ACLdata;
                        Info(slide, 0, ((char *)slide,
                          LoadFarString(ef_fieldname), makelong(ef_ptr)));
                        *pEndprev = 0L;   /* no clue about csize of local */
                    } else {
                        goto ef_default_display;
                    }
                    break;
                case EF_NTSD:
                    if (eb_datalen >= EB_NTSD_C_LEN) {
                        Info(slide, 0, ((char *)slide, LoadFarString(NTSDData),
                          makelong(ef_ptr)));
                        *pEndprev = 0L;   /* no clue about csize of local */
                    } else {
                        goto ef_default_display;
                    }
                    break;
                case EF_IZVMS:
                    if (eb_datalen >= 8) {
                        char *p, q[8];
                        unsigned compr = makeword(ef_ptr+EB_IZVMS_FLGS)
                                        & EB_IZVMS_BCMASK;

                        *q = '\0';
                        if (compr > 3)
                            compr = 3;
                        switch (makelong(ef_ptr)) {
                            case 0x42414656: /* "VFAB" */
                                p = "FAB"; break;
                            case 0x4C4C4156: /* "VALL" */
                                p = "XABALL"; break;
                            case 0x43484656: /* "VFHC" */
                                p = "XABFHC"; break;
                            case 0x54414456: /* "VDAT" */
                                p = "XABDAT"; break;
                            case 0x54445256: /* "VRDT" */
                                p = "XABRDT"; break;
                            case 0x4F525056: /* "VPRO" */
                                p = "XABPRO"; break;
                            case 0x59454B56: /* "VKEY" */
                                p = "XABKEY"; break;
                            case 0x56534D56: /* "VMSV" */
                                p = "version";
                                if (eb_datalen >= 16) {
                                    /* put termitation first, for A_TO_N() */
                                    q[7] = '\0';
                                    q[0] = ' ';
                                    q[1] = '(';
                                    strncpy(q+2,
                                            (char *)ef_ptr+EB_IZVMS_HLEN, 4);
                                    A_TO_N(q+2);
                                    q[6] = ')';
                                }
                                break;
                            default:
                                p = "unknown";
                        }
                        Info(slide, 0, ((char *)slide,
                          LoadFarString(izVMSdata),
                          LoadFarStringSmall(izVMScomp[compr]),
                          makeword(ef_ptr+EB_IZVMS_UCSIZ), p, q));
                    } else {
                        goto ef_default_display;
                    }
                    break;
                case EF_TIME:
                    if (eb_datalen > 0) {
                        char types[80];
                        int num = 0, len;

                        *types = '\0';
                        if (*ef_ptr & 1) {
                            strcpy(types, LoadFarString(UTmodification));
                            ++num;
                        }
                        if (*ef_ptr & 2) {
                            len = strlen(types);
                            if (num)
                                types[len++] = '/';
                            strcpy(types+len, LoadFarString(UTaccess));
                            ++num;
                            if (*pEndprev > 0L)
                                *pEndprev += 4L;
                        }
                        if (*ef_ptr & 4) {
                            len = strlen(types);
                            if (num)
                                types[len++] = '/';
                            strcpy(types+len, LoadFarString(UTcreation));
                            ++num;
                            if (*pEndprev > 0L)
                                *pEndprev += 4L;
                        }
                        if (num > 0)
                            Info(slide, 0, ((char *)slide,
                              LoadFarString(UTdata), types,
                              num == 1? nullStr : PlurSufx));
                    }
                    break;
                case EF_UNIPATH:
                case EF_UNICOMNT:
                    if (eb_datalen >= 5) {
                        unsigned i, n;
                        ulg name_crc = makelong(ef_ptr+1);

                        if (eb_datalen <= 29) {
                            Info(slide, 0, ((char *)slide,
                                 LoadFarString(U8PthCmnComplete),
                                 (unsigned)ef_ptr[0], name_crc));
                            n = eb_datalen;
                        } else {
                            Info(slide, 0, ((char *)slide,
                                 LoadFarString(U8PthCmnF24),
                                 (unsigned)ef_ptr[0], name_crc));
                            n = 29;
                        }
                        for (i = 5;  i < n;  ++i)
                            Info(slide, 0, ((char *)slide,
                                 LoadFarString(efFormat), ef_ptr[i]));
                    } else {
                        goto ef_default_display;
                    }
                    break;
                case EF_MAC3:
                    if (eb_datalen >= EB_MAC3_HLEN) {
                        ulg eb_uc = makelong(ef_ptr);
                        unsigned mac3_flgs = makeword(ef_ptr+EB_FLGS_OFFS);
                        unsigned eb_is_uc = mac3_flgs & EB_M3_FL_UNCMPR;

                        Info(slide, 0, ((char *)slide, LoadFarString(Mac3data),
                          eb_uc, eb_is_uc ? "un" : nullStr));
                        if (eb_is_uc) {
                            if (*pEndprev > 0L)
                                *pEndprev += makelong(ef_ptr);
                        } else {
                            *pEndprev = 0L; /* no clue about csize of local */
                        }

                        Info(slide, 0, ((char *)slide,
                          LoadFarString(MacOSMAC3flags),
                          LoadFarStringSmall(mac3_flgs & EB_M3_FL_DATFRK ?
                                             MacOS_DF : MacOS_RF),
                          (mac3_flgs & EB_M3_FL_TIME64 ? 64 : 32)));
                        zi_showMacTypeCreator(__G__ &ef_ptr[6]);
                    } else {
                        goto ef_default_display;
                    }
                    break;
                case EF_ZIPIT2:
                    if (eb_datalen >= 5 &&
                        makelong(ef_ptr) == 0x5449505A /* "ZPIT" */) {

                        if (eb_datalen >= 12) {
                            zi_showMacTypeCreator(__G__ &ef_ptr[4]);
                        }
                    } else {
                        goto ef_default_display;
                    }
                    break;
                case EF_ZIPIT:
                    if (eb_datalen >= 5 &&
                        makelong(ef_ptr) == 0x5449505A /* "ZPIT" */) {
                        unsigned fnlen = ef_ptr[4];

                        if ((unsigned)eb_datalen >= fnlen + (5 + 8)) {
                            uch nullchar = ef_ptr[fnlen+5];

                            ef_ptr[fnlen+5] = '\0'; /* terminate filename */
                            A_TO_N(ef_ptr+5);
                            Info(slide, 0, ((char *)slide,
                              LoadFarString(ZipItFname), (char *)ef_ptr+5));
                            ef_ptr[fnlen+5] = nullchar;
                            zi_showMacTypeCreator(__G__ &ef_ptr[fnlen+5]);
                        }
                    } else {
                        goto ef_default_display;
                    }
                    break;
                case EF_JLMAC:
                    if (eb_datalen >= 40 &&
                        makelong(ef_ptr) == 0x45454C4A /* "JLEE" */)
                    {
                        zi_showMacTypeCreator(__G__ &ef_ptr[4]);

                        Info(slide, 0, ((char *)slide,
                          LoadFarString(MacOSJLEEflags),
                          LoadFarStringSmall(ef_ptr[31] & 1 ?
                                             MacOS_DF : MacOS_RF)));
                    } else {
                        goto ef_default_display;
                    }
                    break;
                case EF_SMARTZIP:
                    if ((eb_datalen == EB_SMARTZIP_HLEN) &&
                        makelong(ef_ptr) == 0x70695A64 /* "dZip" */) {
                        char filenameBuf[32];
                        zi_showMacTypeCreator(__G__ &ef_ptr[4]);
                        memcpy(filenameBuf, &ef_ptr[33], 31);
                        filenameBuf[ef_ptr[32]] = '\0';
                        A_TO_N(filenameBuf);
                        Info(slide, 0, ((char *)slide,
                             LoadFarString(ZipItFname), filenameBuf));
                    } else {
                        goto ef_default_display;
                    }
                    break;
#ifdef CMS_MVS
                case EF_VMCMS:
                case EF_MVS:
                    {
                        char type[100];

                        Info(slide, 0, ((char *)slide,
                             LoadFarString(VmMvsExtraField),
                             (getVMMVSexfield(type, ef_ptr-EB_HEADSIZE,
                             (unsigned)eb_datalen) > 0)?
                             type : LoadFarStringSmall(VmMvsInvalid)));
                    }
                    break;
#endif /* CMS_MVS */
                case EF_ATHEOS:
                case EF_BEOS:
                    if (eb_datalen >= EB_BEOS_HLEN) {
                        ulg eb_uc = makelong(ef_ptr);
                        unsigned eb_is_uc =
                          *(ef_ptr+EB_FLGS_OFFS) & EB_BE_FL_UNCMPR;

                        if (eb_id == EF_ATHEOS)
                            ef_fieldname = AtheOSdata;
                        else
                            ef_fieldname = BeOSdata;
                        Info(slide, 0, ((char *)slide,
                          LoadFarString(ef_fieldname),
                          eb_uc, eb_is_uc ? "un" : nullStr));
                        if (eb_is_uc) {
                            if (*pEndprev > 0L)
                                *pEndprev += makelong(ef_ptr);
                        } else {
                            *pEndprev = 0L; /* no clue about csize of local */
                        }
                    } else {
                        goto ef_default_display;
                    }
                    break;
                case EF_QDOS:
                    if (eb_datalen >= 4) {
                        Info(slide, 0, ((char *)slide, LoadFarString(QDOSdata),
                          ef_ptr[0], ef_ptr[1], ef_ptr[2], ef_ptr[3]));
                    } else {
                        goto ef_default_display;
                    }
                    break;
                case EF_AOSVS:
                    if (eb_datalen >= 5) {
                        Info(slide, 0, ((char *)slide, LoadFarString(AOSVSdata),
                          ((int)(uch)ef_ptr[4])/10, ((int)(uch)ef_ptr[4])%10));
                    } else {
                        goto ef_default_display;
                    }
                    break;
                case EF_TANDEM:
                    if (eb_datalen == 20) {
                        unsigned type, code;

                        type = (ef_ptr[18] & 0x60) >> 5;
                        code = makeword(ef_ptr);
                        /* Arrg..., Tandem e.f. uses BigEndian byte-order */
                        code = ((code << 8) & 0xff00) | ((code >> 8) & 0x00ff);
                        if (type == NSK_UNSTRUCTURED) {
                            if (code == NSK_EDITFILECODE)
                                type = 4;
                            else if (code == NSK_OBJECTFILECODE)
                                type = 5;
                        }
                        Info(slide, 0, ((char *)slide,
                          LoadFarString(Tandemdata),
                          LoadFarStringSmall(TandemFileformat[type]),
                          code));
                    } else {
                        goto ef_default_display;
                    }
                    break;
                case EF_MD5:
                    if (eb_datalen >= 19) {
                        char md5[33];
                        int i;

                        for (i = 0;  i < 16;  ++i)
                            sprintf(&md5[i<<1], "%02x", ef_ptr[15-i]);
                        md5[32] = '\0';
                        Info(slide, 0, ((char *)slide, LoadFarString(MD5data),
                          md5));
                        break;
                    }   /* else: fall through !! */
                default:
ef_default_display:
                    if (eb_datalen > 0) {
                        unsigned i, n;

                        if (eb_datalen <= 24) {
                            Info(slide, 0, ((char *)slide,
                                 LoadFarString(ColonIndent)));
                            n = eb_datalen;
                        } else {
                            Info(slide, 0, ((char *)slide,
                                 LoadFarString(First20)));
                            n = 20;
                        }
                        for (i = 0;  i < n;  ++i)
                            Info(slide, 0, ((char *)slide,
                                 LoadFarString(efFormat), ef_ptr[i]));
                    }
                    break;
            }
            (*G.message)((zvoid *)&G, (uch *)".", 1L, 0);

            ef_ptr += eb_datalen;
            ef_len -= eb_datalen;
        }
        (*G.message)((zvoid *)&G, (uch *)"\n", 1L, 0);
    }

    /* high bit == Unix/OS2/NT GMT times (mtime, atime); next bit == UID/GID */
    if ((xattr = (unsigned)((G.crec.external_file_attributes & 0xC000) >> 12))
        & 8)
    {
        if (hostnum == UNIX_ || hostnum == FS_HPFS_ || hostnum == FS_NTFS_)
        {
            Info(slide, 0, ((char *)slide, LoadFarString(lExtraFieldType),
              "is", EF_IZUNIX, LoadFarStringSmall(efIZUnix),
              (unsigned)(xattr&12), (xattr&4)? efIZuid : efIZnouid));
            if (*pEndprev > 0L)
                *pEndprev += (ulg)(xattr&12);
        }
        else if (hostnum == FS_FAT_ && !(xattr&4))
            Info(slide, 0, ((char *)slide, LoadFarString(lExtraFieldType),
              "may be", EF_IZUNIX, LoadFarStringSmall(efIZUnix), 8,
              efIZnouid));
    }

    if (!G.crec.file_comment_length)
        Info(slide, 0, ((char *)slide, LoadFarString(NoFileComment)));
    else {
        Info(slide, 0, ((char *)slide, LoadFarString(FileCommBegin)));
        if ((error = do_string(__G__ G.crec.file_comment_length, DISPL_8)) !=
            PK_COOL)
        {
            error_in_archive = error;   /* might be warning */
            if (error > PK_WARN)   /* fatal */
                return error;
        }
        Info(slide, 0, ((char *)slide, LoadFarString(FileCommEnd)));
    }

    return error_in_archive;

} /* end function zi_long() */





/*************************/
/*  Function zi_short()  */
/*************************/

static int zi_short(__G)   /* return PK-type error code */
    __GDEF
{
#ifdef USE_EF_UT_TIME
    iztimes     z_utime;
    time_t      *z_modtim;
#endif
    int         k, error, error_in_archive=PK_COOL;
    unsigned    hostnum, hostver, methid, methnum, xattr;
    char        *p, workspace[12], attribs[32];
    char        methbuf[16];
    static ZCONST char dtype[5]="NXFS"; /* normal, maximum, fast, superfast */
    static ZCONST char Far os[NUM_HOSTS+1][4] = {
        "fat", "ami", "vms", "unx", "cms", "atr", "hpf", "mac", "zzz",
        "cpm", "t20", "ntf", "qds", "aco", "vft", "mvs", "be ", "nsk",
        "ths", "osx", "???", "???", "???", "???", "???", "???", "???",
        "???", "???", "???", "ath", "???"
    };
#ifdef OLD_THEOS_EXTRA
    static ZCONST char Far os_TheosOld[] = "tho";
#endif
    static ZCONST char Far method[NUM_METHODS+1][5] = {
        "stor", "shrk", "re:1", "re:2", "re:3", "re:4", "i#:#", "tokn",
        "def#", "d64#", "dcli", "bzp2", "lzma", "ters", "lz77", "wavp",
        "ppmd", "u###"
    };


/*---------------------------------------------------------------------------
    Print out various interesting things about the compressed file.
  ---------------------------------------------------------------------------*/

    methid = (unsigned)(G.crec.compression_method);
    methnum = find_compr_idx(G.crec.compression_method);
    hostnum = (unsigned)(G.pInfo->hostnum);
    hostver = (unsigned)(G.pInfo->hostver);
/*
    extnum = (unsigned)MIN(G.crec.version_needed_to_extract[1], NUM_HOSTS);
    extver = (unsigned)G.crec.version_needed_to_extract[0];
 */

    zfstrcpy(methbuf, method[methnum]);
    if (methid == IMPLODED) {
        methbuf[1] = (char)((G.crec.general_purpose_bit_flag & 2)? '8' : '4');
        methbuf[3] = (char)((G.crec.general_purpose_bit_flag & 4)? '3' : '2');
    } else if (methid == DEFLATED || methid == ENHDEFLATED) {
        ush  dnum=(ush)((G.crec.general_purpose_bit_flag>>1) & 3);
        methbuf[3] = dtype[dnum];
    } else if (methnum >= NUM_METHODS) {   /* unknown */
        sprintf(&methbuf[1], "%03u", G.crec.compression_method);
    }

    for (k = 0;  k < 15;  ++k)
        attribs[k] = ' ';
    attribs[15] = 0;

    xattr = (unsigned)((G.crec.external_file_attributes >> 16) & 0xFFFF);
    switch (hostnum) {
        case VMS_:
            {   int    i, j;

                for (k = 0;  k < 12;  ++k)
                    workspace[k] = 0;
                if (xattr & VMS_IRUSR)
                    workspace[0] = 'R';
                if (xattr & VMS_IWUSR) {
                    workspace[1] = 'W';
                    workspace[3] = 'D';
                }
                if (xattr & VMS_IXUSR)
                    workspace[2] = 'E';
                if (xattr & VMS_IRGRP)
                    workspace[4] = 'R';
                if (xattr & VMS_IWGRP) {
                    workspace[5] = 'W';
                    workspace[7] = 'D';
                }
                if (xattr & VMS_IXGRP)
                  workspace[6] = 'E';
                if (xattr & VMS_IROTH)
                    workspace[8] = 'R';
                if (xattr & VMS_IWOTH) {
                    workspace[9] = 'W';
                    workspace[11] = 'D';
                }
                if (xattr & VMS_IXOTH)
                    workspace[10] = 'E';

                p = attribs;
                for (k = j = 0;  j < 3;  ++j) {     /* groups of permissions */
                    for (i = 0;  i < 4;  ++i, ++k)  /* perms within a group */
                        if (workspace[k])
                            *p++ = workspace[k];
                    *p++ = ',';                     /* group separator */
                }
                *--p = ' ';   /* overwrite last comma */
                if ((p - attribs) < 12)
                    sprintf(&attribs[12], "%u.%u", hostver/10, hostver%10);
            }
            break;

        case AMIGA_:
            switch (xattr & AMI_IFMT) {
                case AMI_IFDIR:  attribs[0] = 'd';  break;
                case AMI_IFREG:  attribs[0] = '-';  break;
                default:         attribs[0] = '?';  break;
            }
            attribs[1] = (xattr & AMI_IHIDDEN)?   'h' : '-';
            attribs[2] = (xattr & AMI_ISCRIPT)?   's' : '-';
            attribs[3] = (xattr & AMI_IPURE)?     'p' : '-';
            attribs[4] = (xattr & AMI_IARCHIVE)?  'a' : '-';
            attribs[5] = (xattr & AMI_IREAD)?     'r' : '-';
            attribs[6] = (xattr & AMI_IWRITE)?    'w' : '-';
            attribs[7] = (xattr & AMI_IEXECUTE)?  'e' : '-';
            attribs[8] = (xattr & AMI_IDELETE)?   'd' : '-';
            sprintf(&attribs[12], "%u.%u", hostver/10, hostver%10);
            break;

        case THEOS_:
            switch (xattr & THS_IFMT) {
                case THS_IFLIB: *attribs = 'L'; break;
                case THS_IFDIR: *attribs = 'D'; break;
                case THS_IFCHR: *attribs = 'C'; break;
                case THS_IFREG: *attribs = 'S'; break;
                case THS_IFREL: *attribs = 'R'; break;
                case THS_IFKEY: *attribs = 'K'; break;
                case THS_IFIND: *attribs = 'I'; break;
                case THS_IFR16: *attribs = 'P'; break;
                case THS_IFP16: *attribs = '2'; break;
                case THS_IFP32: *attribs = '3'; break;
                default:        *attribs = '?'; break;
            }
            attribs[1] = (xattr & THS_INHID) ? '.' : 'H';
            attribs[2] = (xattr & THS_IMODF) ? '.' : 'M';
            attribs[3] = (xattr & THS_IWOTH) ? '.' : 'W';
            attribs[4] = (xattr & THS_IROTH) ? '.' : 'R';
            attribs[5] = (xattr & THS_IEUSR) ? '.' : 'E';
            attribs[6] = (xattr & THS_IXUSR) ? '.' : 'X';
            attribs[7] = (xattr & THS_IWUSR) ? '.' : 'W';
            attribs[8] = (xattr & THS_IRUSR) ? '.' : 'R';
            sprintf(&attribs[12], "%u.%u", hostver/10, hostver%10);
            break;

        case FS_VFAT_:
#ifdef OLD_THEOS_EXTRA
            if (hostver == 20) {
                switch (xattr & _THS_IFMT) {
                    case _THS_IFLIB: *attribs = 'L'; break;
                    case _THS_IFDIR: *attribs = 'd'; break;
                    case _THS_IFCHR: *attribs = 'c'; break;
                    case _THS_IFREG: *attribs = 'S'; break;
                    case _THS_IODRC: *attribs = 'D'; break;
                    case _THS_IOKEY: *attribs = 'K'; break;
                    case _THS_IOIND: *attribs = 'I'; break;
                    case _THS_IOPRG: *attribs = 'P'; break;
                    case _THS_IO286: *attribs = '2'; break;
                    case _THS_IO386: *attribs = '3'; break;
                    default:         *attribs = '?'; break;
                }
                attribs[1] = (xattr & _THS_HIDDN) ? 'H' : '.';
                attribs[2] = (xattr & _THS_IXOTH) ? '.' : 'X';
                attribs[3] = (xattr & _THS_IWOTH) ? '.' : 'W';
                attribs[4] = (xattr & _THS_IROTH) ? '.' : 'R';
                attribs[5] = (xattr & _THS_IEUSR) ? '.' : 'E';
                attribs[6] = (xattr & _THS_IXUSR) ? '.' : 'X';
                attribs[7] = (xattr & _THS_IWUSR) ? '.' : 'W';
                attribs[8] = (xattr & _THS_IRUSR) ? '.' : 'R';
                sprintf(&attribs[12], "%u.%u", hostver/10, hostver%10);
                break;
            } /* else: fall through! */
#endif /* OLD_THEOS_EXTRA */

        case FS_FAT_:
        case FS_HPFS_:
        case FS_NTFS_:
        case VM_CMS_:
        case MVS_:
        case ACORN_:
            if (hostnum != FS_FAT_ ||
                (unsigned)(xattr & 0700) !=
                 ((unsigned)0400 |
                  ((unsigned)!(G.crec.external_file_attributes & 1) << 7) |
                  ((unsigned)(G.crec.external_file_attributes & 0x10) << 2))
               )
            {
                xattr = (unsigned)(G.crec.external_file_attributes & 0xFF);
                sprintf(attribs, ".r.-...     %u.%u", hostver/10, hostver%10);
                attribs[2] = (xattr & 0x01)? '-' : 'w';
                attribs[5] = (xattr & 0x02)? 'h' : '-';
                attribs[6] = (xattr & 0x04)? 's' : '-';
                attribs[4] = (xattr & 0x20)? 'a' : '-';
                if (xattr & 0x10) {
                    attribs[0] = 'd';
                    attribs[3] = 'x';
                } else
                    attribs[0] = '-';
                if (IS_VOLID(xattr))
                    attribs[0] = 'V';
                else if ((p = MBSRCHR(G.filename, '.')) != (char *)NULL) {
                    ++p;
                    if (STRNICMP(p, "com", 3) == 0 ||
                        STRNICMP(p, "exe", 3) == 0 ||
                        STRNICMP(p, "btm", 3) == 0 ||
                        STRNICMP(p, "cmd", 3) == 0 ||
                        STRNICMP(p, "bat", 3) == 0)
                        attribs[3] = 'x';
                }
                break;
            } /* else: fall through! */

        default:   /* assume Unix-like */
            switch ((unsigned)(xattr & UNX_IFMT)) {
                case (unsigned)UNX_IFDIR:   attribs[0] = 'd';  break;
                case (unsigned)UNX_IFREG:   attribs[0] = '-';  break;
                case (unsigned)UNX_IFLNK:   attribs[0] = 'l';  break;
                case (unsigned)UNX_IFBLK:   attribs[0] = 'b';  break;
                case (unsigned)UNX_IFCHR:   attribs[0] = 'c';  break;
                case (unsigned)UNX_IFIFO:   attribs[0] = 'p';  break;
                case (unsigned)UNX_IFSOCK:  attribs[0] = 's';  break;
                default:          attribs[0] = '?';  break;
            }
            attribs[1] = (xattr & UNX_IRUSR)? 'r' : '-';
            attribs[4] = (xattr & UNX_IRGRP)? 'r' : '-';
            attribs[7] = (xattr & UNX_IROTH)? 'r' : '-';
            attribs[2] = (xattr & UNX_IWUSR)? 'w' : '-';
            attribs[5] = (xattr & UNX_IWGRP)? 'w' : '-';
            attribs[8] = (xattr & UNX_IWOTH)? 'w' : '-';

            if (xattr & UNX_IXUSR)
                attribs[3] = (xattr & UNX_ISUID)? 's' : 'x';
            else
                attribs[3] = (xattr & UNX_ISUID)? 'S' : '-';  /* S==undefined */
            if (xattr & UNX_IXGRP)
                attribs[6] = (xattr & UNX_ISGID)? 's' : 'x';  /* == UNX_ENFMT */
            else
                /* attribs[6] = (xattr & UNX_ISGID)? 'l' : '-';  real 4.3BSD */
                attribs[6] = (xattr & UNX_ISGID)? 'S' : '-';  /* SunOS 4.1.x */
            if (xattr & UNX_IXOTH)
                attribs[9] = (xattr & UNX_ISVTX)? 't' : 'x';  /* "sticky bit" */
            else
                attribs[9] = (xattr & UNX_ISVTX)? 'T' : '-';  /* T==undefined */

            sprintf(&attribs[12], "%u.%u", hostver/10, hostver%10);
            break;

    } /* end switch (hostnum: external attributes format) */

#ifdef OLD_THEOS_EXTRA
    Info(slide, 0, ((char *)slide, "%s %s %s ", attribs,
      LoadFarStringSmall(((hostnum == FS_VFAT_ && hostver == 20) ?
                          os_TheosOld :
                          os[hostnum])),
      FmZofft(G.crec.ucsize, "8", "u")));
#else
    Info(slide, 0, ((char *)slide, "%s %s %s ", attribs,
      LoadFarStringSmall(os[hostnum]),
      FmZofft(G.crec.ucsize, "8", "u")));
#endif
    Info(slide, 0, ((char *)slide, "%c",
      (G.crec.general_purpose_bit_flag & 1)?
      ((G.crec.internal_file_attributes & 1)? 'T' : 'B') :  /* encrypted */
      ((G.crec.internal_file_attributes & 1)? 't' : 'b'))); /* plaintext */
    k = (G.crec.extra_field_length ||
         /* a local-only "UX" (old Unix/OS2/NT GMT times "IZUNIX") e.f.? */
         ((G.crec.external_file_attributes & 0x8000) &&
          (hostnum == UNIX_ || hostnum == FS_HPFS_ || hostnum == FS_NTFS_)));
    Info(slide, 0, ((char *)slide, "%c", k?
      ((G.crec.general_purpose_bit_flag & 8)? 'X' : 'x') :  /* extra field */
      ((G.crec.general_purpose_bit_flag & 8)? 'l' : '-'))); /* no extra field */
      /* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ extended local header or not */

    if (uO.lflag == 4) {
        zusz_t csiz = G.crec.csize;

        if (G.crec.general_purpose_bit_flag & 1)
            csiz -= 12;    /* if encrypted, don't count encryption header */
        Info(slide, 0, ((char *)slide, "%3d%%",
          (ratio(G.crec.ucsize,csiz)+5)/10));
    } else if (uO.lflag == 5)
        Info(slide, 0, ((char *)slide, " %s",
          FmZofft(G.crec.csize, "8", "u")));

    /* For printing of date & time, a "char d_t_buf[16]" is required.
     * To save stack space, we reuse the "char attribs[16]" buffer whose
     * content is no longer needed.
     */
#   define d_t_buf attribs
#ifdef USE_EF_UT_TIME
    z_modtim = G.extra_field &&
#ifdef IZ_CHECK_TZ
               G.tz_is_valid &&
#endif
               (ef_scan_for_izux(G.extra_field, G.crec.extra_field_length, 1,
                                 G.crec.last_mod_dos_datetime, &z_utime, NULL)
                & EB_UT_FL_MTIME)
              ? &z_utime.mtime : NULL;
    TIMET_TO_NATIVE(z_utime.mtime)     /* NOP unless MSC 7.0 or Macintosh */
    d_t_buf[0] = (char)0;              /* signal "show local time" */
#else
#   define z_modtim NULL
#endif
    Info(slide, 0, ((char *)slide, " %s %s ", methbuf,
      zi_time(__G__ &G.crec.last_mod_dos_datetime, z_modtim, d_t_buf)));
    fnprint(__G);

/*---------------------------------------------------------------------------
    Skip the file comment, if any (the filename has already been printed,
    above).  That finishes up this file entry...
  ---------------------------------------------------------------------------*/

    SKIP_(G.crec.file_comment_length)

    return error_in_archive;

} /* end function zi_short() */





/**************************************/
/*  Function zi_showMacTypeCreator()  */
/**************************************/

static void zi_showMacTypeCreator(__G__ ebfield)
    __GDEF
    uch *ebfield;
{
    /* not every Type / Creator character is printable */
    if (isprint(native(ebfield[0])) && isprint(native(ebfield[1])) &&
        isprint(native(ebfield[2])) && isprint(native(ebfield[3])) &&
        isprint(native(ebfield[4])) && isprint(native(ebfield[5])) &&
        isprint(native(ebfield[6])) && isprint(native(ebfield[7]))) {
       Info(slide, 0, ((char *)slide, LoadFarString(MacOSdata),
            native(ebfield[0]), native(ebfield[1]),
            native(ebfield[2]), native(ebfield[3]),
            native(ebfield[4]), native(ebfield[5]),
            native(ebfield[6]), native(ebfield[7])));
    } else {
       Info(slide, 0, ((char *)slide, LoadFarString(MacOSdata1),
            (((ulg)ebfield[0]) << 24) +
            (((ulg)ebfield[1]) << 16) +
            (((ulg)ebfield[2]) << 8)  +
            ((ulg)ebfield[3]),
            (((ulg)ebfield[4]) << 24) +
            (((ulg)ebfield[5]) << 16) +
            (((ulg)ebfield[6]) << 8)  +
            ((ulg)ebfield[7])));
    }
} /* end function zi_showMacTypeCreator() */





/************************/
/*  Function zi_time()  */
/************************/

static char *zi_time(__G__ datetimez, modtimez, d_t_str)
    __GDEF
    ZCONST ulg *datetimez;
    ZCONST time_t *modtimez;
    char *d_t_str;
{
    unsigned yr, mo, dy, hh, mm, ss;
    char monthbuf[4];
    ZCONST char *monthstr;
    static ZCONST char Far month[12][4] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
#ifdef USE_EF_UT_TIME
    struct tm *t;
#endif



/*---------------------------------------------------------------------------
    Convert the file-modification date and time info to a string of the form
    "1991 Feb 23 17:15:00", "23-Feb-91 17:15" or "19910223.171500", depending
    on values of lflag and T_flag.  If using Unix-time extra fields, convert
    to local time or not, depending on value of first character in d_t_str[].
  ---------------------------------------------------------------------------*/

#ifdef USE_EF_UT_TIME
    if (modtimez != NULL) {
#ifndef NO_GMTIME
        /* check for our secret message from above... */
        t = (d_t_str[0] == (char)1)? gmtime(modtimez) : localtime(modtimez);
#else
        t = localtime(modtimez);
#endif
        if (uO.lflag > 9 && t == (struct tm *)NULL)
            /* time conversion error in verbose listing format,
             * return string with '?' instead of data
             */
            return (strcpy(d_t_str, LoadFarString(lngYMDHMSTimeError)));
    } else
        t = (struct tm *)NULL;
    if (t != (struct tm *)NULL) {
        mo = (unsigned)(t->tm_mon + 1);
        dy = (unsigned)(t->tm_mday);
        yr = (unsigned)(t->tm_year);

        hh = (unsigned)(t->tm_hour);
        mm = (unsigned)(t->tm_min);
        ss = (unsigned)(t->tm_sec);
    } else
#endif /* USE_EF_UT_TIME */
    {
        yr = ((unsigned)(*datetimez >> 25) & 0x7f) + 80;
        mo = ((unsigned)(*datetimez >> 21) & 0x0f);
        dy = ((unsigned)(*datetimez >> 16) & 0x1f);

        hh = (((unsigned)*datetimez >> 11) & 0x1f);
        mm = (((unsigned)*datetimez >> 5) & 0x3f);
        ss = (((unsigned)*datetimez << 1) & 0x3e);
    }

    if (mo == 0 || mo > 12) {
        sprintf(monthbuf, LoadFarString(BogusFmt), mo);
        monthstr = monthbuf;
    } else
        monthstr = LoadFarStringSmall(month[mo-1]);

    if (uO.lflag > 9)   /* verbose listing format */
        sprintf(d_t_str, LoadFarString(lngYMDHMSTime), yr+1900, monthstr, dy,
          hh, mm, ss);
    else if (uO.T_flag)
        sprintf(d_t_str, LoadFarString(DecimalTime), yr+1900, mo, dy,
          hh, mm, ss);
    else   /* was:  if ((uO.lflag >= 3) && (uO.lflag <= 5)) */
        sprintf(d_t_str, LoadFarString(shtYMDHMTime), yr%100, monthstr, dy,
          hh, mm);

    return d_t_str;

} /* end function zi_time() */

#endif /* !NO_ZIPINFO */

