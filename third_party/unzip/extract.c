/*
  Copyright (c) 1990-2010 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2009-Jan-02 or later
  (the contents of which are also included in unzip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*---------------------------------------------------------------------------

  extract.c

  This file contains the high-level routines ("driver routines") for extrac-
  ting and testing zipfile members.  It calls the low-level routines in files
  explode.c, inflate.c, unreduce.c and unshrink.c.

  Contains:  extract_or_test_files()
             store_info()
             find_compr_idx()
             extract_or_test_entrylist()
             extract_or_test_member()
             TestExtraField()
             test_compr_eb()
             memextract()
             memflush()
             extract_izvms_block()    (VMS or VMS_TEXT_CONV)
             set_deferred_symlink()   (SYMLINKS only)
             fnfilter()
             dircomp()                (SET_DIR_ATTRIB only)
             UZbunzip2()              (USE_BZIP2 only)

  ---------------------------------------------------------------------------*/


#define __EXTRACT_C     /* identifies this source module */
#define UNZIP_INTERNAL
#include "third_party/unzip/unzip.h"
#include "third_party/unzip/globals.h"
#include "third_party/unzip/crc32.h"
#include "libc/mem/alg.h"
#include "libc/log/log.h"
#include "third_party/unzip/crypt.h"

#define GRRDUMP(buf,len) { \
    int i, j; \
 \
    for (j = 0;  j < (len)/16;  ++j) { \
        printf("        "); \
        for (i = 0;  i < 16;  ++i) \
            printf("%02x ", (uch)(buf)[i+(j<<4)]); \
        printf("\n        "); \
        for (i = 0;  i < 16;  ++i) { \
            char c = (char)(buf)[i+(j<<4)]; \
 \
            if (c == '\n') \
                printf("\\n "); \
            else if (c == '\r') \
                printf("\\r "); \
            else \
                printf(" %c ", c); \
        } \
        printf("\n"); \
    } \
    if ((len) % 16) { \
        printf("        "); \
        for (i = j<<4;  i < (len);  ++i) \
            printf("%02x ", (uch)(buf)[i]); \
        printf("\n        "); \
        for (i = j<<4;  i < (len);  ++i) { \
            char c = (char)(buf)[i]; \
 \
            if (c == '\n') \
                printf("\\n "); \
            else if (c == '\r') \
                printf("\\r "); \
            else \
                printf(" %c ", c); \
        } \
        printf("\n"); \
    } \
}

static int store_info OF((__GPRO));
#ifdef SET_DIR_ATTRIB
# if defined(UNICODE_SUPPORT) && defined(WIN32_WIDE)
static int extract_or_test_entrylistw OF((__GPRO__ unsigned numchunk,
                ulg *pfilnum, ulg *pnum_bad_pwd, zoff_t *pold_extra_bytes,
                unsigned *pnum_dirs,
                direntryw **pdirlistw,
                int error_in_archive));
# endif
static int extract_or_test_entrylist OF((__GPRO__ unsigned numchunk,
                ulg *pfilnum, ulg *pnum_bad_pwd, zoff_t *pold_extra_bytes,
                unsigned *pnum_dirs, direntry **pdirlist,
                int error_in_archive));
#else
static int extract_or_test_entrylist OF((__GPRO__ unsigned numchunk,
                ulg *pfilnum, ulg *pnum_bad_pwd, zoff_t *pold_extra_bytes,
                int error_in_archive));
#endif
static int extract_or_test_member OF((__GPRO));
#ifndef SFX
   static int TestExtraField OF((__GPRO__ uch *ef, unsigned ef_len));
   static int test_compr_eb OF((__GPRO__ uch *eb, unsigned eb_size,
        unsigned compr_offset,
        int (*test_uc_ebdata)(__GPRO__ uch *eb, unsigned eb_size,
                              uch *eb_ucptr, ulg eb_ucsize)));
#endif
#if (defined(VMS) || defined(VMS_TEXT_CONV))
   static void decompress_bits OF((uch *outptr, unsigned needlen,
                                   ZCONST uch *bitptr));
#endif
#ifdef SYMLINKS
   static void set_deferred_symlink OF((__GPRO__ slinkentry *slnk_entry));
#endif
#ifdef SET_DIR_ATTRIB
# if defined(UNICODE_SUPPORT) && defined(WIN32_WIDE)
   static int Cdecl dircompw OF((ZCONST zvoid *a, ZCONST zvoid *b));
# endif
   static int Cdecl dircomp OF((ZCONST zvoid *a, ZCONST zvoid *b));
#endif



/*******************************/
/*  Strings used in extract.c  */
/*******************************/

static ZCONST char Far VersionMsg[] =
  "   skipping: %-22s  need %s compat. v%u.%u (can do v%u.%u)\n";
static ZCONST char Far ComprMsgNum[] =
  "   skipping: %-22s  unsupported compression method %u\n";
#ifndef SFX
   static ZCONST char Far ComprMsgName[] =
     "   skipping: %-22s  `%s' method not supported\n";
   static ZCONST char Far CmprNone[]       = "store";
   static ZCONST char Far CmprShrink[]     = "shrink";
   static ZCONST char Far CmprReduce[]     = "reduce";
   static ZCONST char Far CmprImplode[]    = "implode";
   static ZCONST char Far CmprTokenize[]   = "tokenize";
   static ZCONST char Far CmprDeflate[]    = "deflate";
   static ZCONST char Far CmprDeflat64[]   = "deflate64";
   static ZCONST char Far CmprDCLImplode[] = "DCL implode";
   static ZCONST char Far CmprBzip[]       = "bzip2";
   static ZCONST char Far CmprLZMA[]       = "LZMA";
   static ZCONST char Far CmprIBMTerse[]   = "IBM/Terse";
   static ZCONST char Far CmprIBMLZ77[]    = "IBM LZ77";
   static ZCONST char Far CmprWavPack[]    = "WavPack";
   static ZCONST char Far CmprPPMd[]       = "PPMd";
   static ZCONST char Far *ComprNames[NUM_METHODS] = {
     CmprNone, CmprShrink, CmprReduce, CmprReduce, CmprReduce, CmprReduce,
     CmprImplode, CmprTokenize, CmprDeflate, CmprDeflat64, CmprDCLImplode,
     CmprBzip, CmprLZMA, CmprIBMTerse, CmprIBMLZ77, CmprWavPack, CmprPPMd
   };
   static ZCONST unsigned ComprIDs[NUM_METHODS] = {
     STORED, SHRUNK, REDUCED1, REDUCED2, REDUCED3, REDUCED4,
     IMPLODED, TOKENIZED, DEFLATED, ENHDEFLATED, DCLIMPLODED,
     BZIPPED, LZMAED, IBMTERSED, IBMLZ77ED, WAVPACKED, PPMDED
   };
#endif /* !SFX */
static ZCONST char Far FilNamMsg[] =
  "%s:  bad filename length (%s)\n";
#ifndef SFX
   static ZCONST char Far WarnNoMemCFName[] =
     "%s:  warning, no memory for comparison with local header\n";
   static ZCONST char Far LvsCFNamMsg[] =
     "%s:  mismatching \"local\" filename (%s),\n\
         continuing with \"central\" filename version\n";
#endif /* !SFX */
#if (!defined(SFX) && defined(UNICODE_SUPPORT))
   static ZCONST char Far GP11FlagsDiffer[] =
     "file #%lu (%s):\n\
         mismatch between local and central GPF bit 11 (\"UTF-8\"),\n\
         continuing with central flag (IsUTF8 = %d)\n";
#endif /* !SFX && UNICODE_SUPPORT */
static ZCONST char Far WrnStorUCSizCSizDiff[] =
  "%s:  ucsize %s <> csize %s for STORED entry\n\
         continuing with \"compressed\" size value\n";
static ZCONST char Far ExtFieldMsg[] =
  "%s:  bad extra field length (%s)\n";
static ZCONST char Far OffsetMsg[] =
  "file #%lu:  bad zipfile offset (%s):  %ld\n";
static ZCONST char Far ExtractMsg[] =
  "%8sing: %-22s  %s%s";
#ifndef SFX
   static ZCONST char Far LengthMsg[] =
     "%s  %s:  %s bytes required to uncompress to %s bytes;\n    %s\
      supposed to require %s bytes%s%s%s\n";
#endif

static ZCONST char Far BadFileCommLength[] = "%s:  bad file comment length\n";
static ZCONST char Far LocalHdrSig[] = "local header sig";
static ZCONST char Far BadLocalHdr[] = "file #%lu:  bad local header\n";
static ZCONST char Far AttemptRecompensate[] =
  "  (attempting to re-compensate)\n";
#ifndef SFX
   static ZCONST char Far BackslashPathSep[] =
     "warning:  %s appears to use backslashes as path separators\n";
#endif
static ZCONST char Far AbsolutePathWarning[] =
  "warning:  stripped absolute path spec from %s\n";
static ZCONST char Far SkipVolumeLabel[] =
  "   skipping: %-22s  %svolume label\n";

#if defined( UNIX) && defined( __APPLE__)
static ZCONST char Far AplDblNameTooLong[] =
  "error:  file name too long with AppleDouble suffix: %s\n";
#endif /* defined( UNIX) && defined( __APPLE__) */

#ifdef SET_DIR_ATTRIB   /* messages of code for setting directory attributes */
   static ZCONST char Far DirlistEntryNoMem[] =
     "warning:  cannot alloc memory for dir times/permissions/UID/GID\n";
   static ZCONST char Far DirlistSortNoMem[] =
     "warning:  cannot alloc memory to sort dir times/perms/etc.\n";
   static ZCONST char Far DirlistSetAttrFailed[] =
     "warning:  set times/attribs failed for %s\n";
   static ZCONST char Far DirlistFailAttrSum[] =
     "     failed setting times/attribs for %lu dir entries";
#endif

#ifdef SYMLINKS         /* messages of the deferred symlinks handler */
   static ZCONST char Far SymLnkWarnNoMem[] =
     "warning:  deferred symlink (%s) failed:\n\
          out of memory\n";
   static ZCONST char Far SymLnkWarnInvalid[] =
     "warning:  deferred symlink (%s) failed:\n\
          invalid placeholder file\n";
   static ZCONST char Far SymLnkDeferred[] =
     "finishing deferred symbolic links:\n";
   static ZCONST char Far SymLnkFinish[] =
     "  %-22s -> %s\n";
#endif

#ifndef WINDLL
   static ZCONST char Far ReplaceQuery[] =
# ifdef VMS
     "new version of %s? [y]es, [n]o, [A]ll, [N]one, [r]ename: ";
# else
     "replace %s? [y]es, [n]o, [A]ll, [N]one, [r]ename: ";
# endif
   static ZCONST char Far AssumeNone[] =
     " NULL\n(EOF or read error, treating as \"[N]one\" ...)\n";
   static ZCONST char Far NewNameQuery[] = "new name: ";
   static ZCONST char Far InvalidResponse[] =
     "error:  invalid response [%s]\n";
#endif /* !WINDLL */

static ZCONST char Far ErrorInArchive[] =
  "At least one %serror was detected in %s.\n";
static ZCONST char Far ZeroFilesTested[] =
  "Caution:  zero files tested in %s.\n";

#ifndef VMS
   static ZCONST char Far VMSFormatQuery[] =
     "\n%s:  stored in VMS format.  Extract anyway? (y/n) ";
#endif

#if CRYPT
   static ZCONST char Far SkipCannotGetPasswd[] =
     "   skipping: %-22s  unable to get password\n";
   static ZCONST char Far SkipIncorrectPasswd[] =
     "   skipping: %-22s  incorrect password\n";
   static ZCONST char Far FilesSkipBadPasswd[] =
     "%lu file%s skipped because of incorrect password.\n";
   static ZCONST char Far MaybeBadPasswd[] =
     "    (may instead be incorrect password)\n";
#else
   static ZCONST char Far SkipEncrypted[] =
     "   skipping: %-22s  encrypted (not supported)\n";
#endif

static ZCONST char Far NoErrInCompData[] =
  "No errors detected in compressed data of %s.\n";
static ZCONST char Far NoErrInTestedFiles[] =
  "No errors detected in %s for the %lu file%s tested.\n";
static ZCONST char Far FilesSkipped[] =
  "%lu file%s skipped because of unsupported compression or encoding.\n";

static ZCONST char Far ErrUnzipFile[] = "  error:  %s%s %s\n";
static ZCONST char Far ErrUnzipNoFile[] = "\n  error:  %s%s\n";
static ZCONST char Far NotEnoughMem[] = "not enough memory to ";
static ZCONST char Far InvalidComprData[] = "invalid compressed data to ";
static ZCONST char Far Inflate[] = "inflate";
#ifdef USE_BZIP2
  static ZCONST char Far BUnzip[] = "bunzip";
#endif

#ifndef SFX
   static ZCONST char Far Explode[] = "explode";
# ifndef LZW_CLEAN
   static ZCONST char Far Unshrink[] = "unshrink";
# endif
#endif

#if (!defined(DELETE_IF_FULL) || !defined(HAVE_UNLINK))
   static ZCONST char Far FileTruncated[] =
     "warning:  %s is probably truncated\n";
#endif

static ZCONST char Far FileUnknownCompMethod[] =
  "%s:  unknown compression method\n";
static ZCONST char Far BadCRC[] = " bad CRC %08lx  (should be %08lx)\n";

      /* TruncEAs[] also used in OS/2 mapname(), close_outfile() */
char ZCONST Far TruncEAs[] = " compressed EA data missing (%d bytes)%s";
char ZCONST Far TruncNTSD[] =
  " compressed WinNT security data missing (%d bytes)%s";

#ifndef SFX
   static ZCONST char Far InconsistEFlength[] = "bad extra-field entry:\n \
     EF block length (%u bytes) exceeds remaining EF data (%u bytes)\n";
   static ZCONST char Far InvalidComprDataEAs[] =
     " invalid compressed data for EAs\n";
# if (defined(WIN32) && defined(NTSD_EAS))
     static ZCONST char Far InvalidSecurityEAs[] =
       " EAs fail security check\n";
# endif
   static ZCONST char Far UnsuppNTSDVersEAs[] =
     " unsupported NTSD EAs version %d\n";
   static ZCONST char Far BadCRC_EAs[] = " bad CRC for extended attributes\n";
   static ZCONST char Far UnknComprMethodEAs[] =
     " unknown compression method for EAs (%u)\n";
   static ZCONST char Far NotEnoughMemEAs[] =
     " out of memory while inflating EAs\n";
   static ZCONST char Far UnknErrorEAs[] =
     " unknown error on extended attributes\n";
#endif /* !SFX */

static ZCONST char Far UnsupportedExtraField[] =
  "\nerror:  unsupported extra-field compression type (%u)--skipping\n";
static ZCONST char Far BadExtraFieldCRC[] =
  "error [%s]:  bad extra-field CRC %08lx (should be %08lx)\n";





/**************************************/
/*  Function extract_or_test_files()  */
/**************************************/

int extract_or_test_files(__G)    /* return PK-type error code */
     __GDEF
{
    unsigned i, j;
    zoff_t cd_bufstart;
    uch *cd_inptr;
    int cd_incnt;
    ulg filnum=0L, blknum=0L;
    int reached_end;
#ifndef SFX
    int no_endsig_found;
#endif
    int error, error_in_archive=PK_COOL;
    int *fn_matched=NULL, *xn_matched=NULL;
    zucn_t members_processed;
    ulg num_skipped=0L, num_bad_pwd=0L;
    zoff_t old_extra_bytes = 0L;
#ifdef SET_DIR_ATTRIB
    unsigned num_dirs=0;
# if defined(UNICODE_SUPPORT) && defined(WIN32_WIDE)
    direntryw *dirlistw=(direntryw *)NULL, **sorted_dirlistw=(direntryw **)NULL;
# endif
    direntry *dirlist=(direntry *)NULL, **sorted_dirlist=(direntry **)NULL;
#endif

    /*
     * First, two general initializations are applied. These have been moved
     * here from process_zipfiles() because they are only needed for accessing
     * and/or extracting the data content of the zip archive.
     */

    /* a) initialize the CRC table pointer (once) */
    if (CRC_32_TAB == NULL) {
        if ((CRC_32_TAB = get_crc_table()) == NULL) {
            return PK_MEM;
        }
    }

#if (!defined(SFX) || defined(SFX_EXDIR))
    /* b) check out if specified extraction root directory exists */
    if (uO.exdir != (char *)NULL && G.extract_flag) {
        G.create_dirs = !uO.fflag;
# if defined(UNICODE_SUPPORT) && defined(WIN32_WIDE)
        if (G.has_win32_wide) {
          wchar_t *exdirw = local_to_wchar_string(uO.exdir);
          if ((error = checkdirw(__G__ exdirw, ROOT)) > MPN_INF_SKIP) {
              /* out of memory, or file in way */
              free(exdirw);
              return (error == MPN_NOMEM ? PK_MEM : PK_ERR);
          }
          free(exdirw);
        } else {
          if ((error = checkdir(__G__ uO.exdir, ROOT)) > MPN_INF_SKIP) {
              /* out of memory, or file in way */
              return (error == MPN_NOMEM ? PK_MEM : PK_ERR);
          }
        }
# else /* ! (defined(UNICODE_SUPPORT) && defined(WIN32_WIDE)) */
        if ((error = checkdir(__G__ uO.exdir, ROOT)) > MPN_INF_SKIP) {
            /* out of memory, or file in way */
            return (error == MPN_NOMEM ? PK_MEM : PK_ERR);
        }
# endif /* ! (defined(UNICODE_SUPPORT) && defined(WIN32_WIDE)) */
    }
#endif /* !SFX || SFX_EXDIR */

/*---------------------------------------------------------------------------
    The basic idea of this function is as follows.  Since the central di-
    rectory lies at the end of the zipfile and the member files lie at the
    beginning or middle or wherever, it is not very desirable to simply
    read a central directory entry, jump to the member and extract it, and
    then jump back to the central directory.  In the case of a large zipfile
    this would lead to a whole lot of disk-grinding, especially if each mem-
    ber file is small.  Instead, we read from the central directory the per-
    tinent information for a block of files, then go extract/test the whole
    block.  Thus this routine contains two small(er) loops within a very
    large outer loop:  the first of the small ones reads a block of files
    from the central directory; the second extracts or tests each file; and
    the outer one loops over blocks.  There's some file-pointer positioning
    stuff in between, but that's about it.  Btw, it's because of this jump-
    ing around that we can afford to be lenient if an error occurs in one of
    the member files:  we should still be able to go find the other members,
    since we know the offset of each from the beginning of the zipfile.
  ---------------------------------------------------------------------------*/

    G.pInfo = G.info;

#if CRYPT
    G.newzip = TRUE;
#endif
#ifndef SFX
    G.reported_backslash = FALSE;
#endif

    /* malloc space for check on unmatched filespecs (OK if one or both NULL) */
    if (G.filespecs > 0  &&
        (fn_matched=(int *)malloc(G.filespecs*sizeof(int))) != (int *)NULL)
        for (i = 0;  i < G.filespecs;  ++i)
            fn_matched[i] = FALSE;
    if (G.xfilespecs > 0  &&
        (xn_matched=(int *)malloc(G.xfilespecs*sizeof(int))) != (int *)NULL)
        for (i = 0;  i < G.xfilespecs;  ++i)
            xn_matched[i] = FALSE;

/*---------------------------------------------------------------------------
    Begin main loop over blocks of member files.  We know the entire central
    directory is on this disk:  we would not have any of this information un-
    less the end-of-central-directory record was on this disk, and we would
    not have gotten to this routine unless this is also the disk on which
    the central directory starts.  In practice, this had better be the ONLY
    disk in the archive, but we'll add multi-disk support soon.
  ---------------------------------------------------------------------------*/

    members_processed = 0;
#ifndef SFX
    no_endsig_found = FALSE;
#endif
    reached_end = FALSE;
    while (!reached_end) {
        j = 0;
#ifdef AMIGA
        memzero(G.filenotes, DIR_BLKSIZ * sizeof(char *));
#endif

        /*
         * Loop through files in central directory, storing offsets, file
         * attributes, case-conversion and text-conversion flags until block
         * size is reached.
         */

        while ((j < DIR_BLKSIZ)) {
            G.pInfo = &G.info[j];

            if (readbuf(__G__ G.sig, 4) == 0) {
                error_in_archive = PK_EOF;
                reached_end = TRUE;     /* ...so no more left to do */
                break;
            }
            if (memcmp(G.sig, central_hdr_sig, 4)) {  /* is it a new entry? */
                /* no new central directory entry
                 * -> is the number of processed entries compatible with the
                 *    number of entries as stored in the end_central record?
                 */
                if ((members_processed
                     & (G.ecrec.have_ecr64 ? MASK_ZUCN64 : MASK_ZUCN16))
                    == G.ecrec.total_entries_central_dir) {
#ifndef SFX
                    /* yes, so look if we ARE back at the end_central record
                     */
                    no_endsig_found =
                      ( (memcmp(G.sig,
                                (G.ecrec.have_ecr64 ?
                                 end_central64_sig : end_central_sig),
                                4) != 0)
                       && (!G.ecrec.is_zip64_archive)
                       && (memcmp(G.sig, end_central_sig, 4) != 0)
                      );
#endif /* !SFX */
                } else {
                    /* no; we have found an error in the central directory
                     * -> report it and stop searching for more Zip entries
                     */
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarString(CentSigMsg), j + blknum*DIR_BLKSIZ + 1));
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarString(ReportMsg)));
                    error_in_archive = PK_BADERR;
                }
                reached_end = TRUE;     /* ...so no more left to do */
                break;
            }
            /* process_cdir_file_hdr() sets pInfo->hostnum, pInfo->lcflag */
            if ((error = process_cdir_file_hdr(__G)) != PK_COOL) {
                error_in_archive = error;   /* only PK_EOF defined */
                reached_end = TRUE;     /* ...so no more left to do */
                break;
            }
            if ((error = do_string(__G__ G.crec.filename_length, DS_FN)) !=
                 PK_COOL)
            {
                if (error > error_in_archive)
                    error_in_archive = error;
                if (error > PK_WARN) {  /* fatal:  no more left to do */
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarString(FilNamMsg),
                      FnFilter1(G.filename), "central"));
                    reached_end = TRUE;
                    break;
                }
            }
            if ((error = do_string(__G__ G.crec.extra_field_length,
                EXTRA_FIELD)) != 0)
            {
                if (error > error_in_archive)
                    error_in_archive = error;
                if (error > PK_WARN) {  /* fatal */
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarString(ExtFieldMsg),
                      FnFilter1(G.filename), "central"));
                    reached_end = TRUE;
                    break;
                }
            }
#ifdef AMIGA
            G.filenote_slot = j;
            if ((error = do_string(__G__ G.crec.file_comment_length,
                                   uO.N_flag ? FILENOTE : SKIP)) != PK_COOL)
#else
            if ((error = do_string(__G__ G.crec.file_comment_length, SKIP))
                != PK_COOL)
#endif
            {
                if (error > error_in_archive)
                    error_in_archive = error;
                if (error > PK_WARN) {  /* fatal */
                    Info(slide, 0x421, ((char *)slide,
                      LoadFarString(BadFileCommLength),
                      FnFilter1(G.filename)));
                    reached_end = TRUE;
                    break;
                }
            }
            if (G.process_all_files) {
                if (store_info(__G))
                    ++j;  /* file is OK; info[] stored; continue with next */
                else
                    ++num_skipped;
            } else {
                int   do_this_file;

                if (G.filespecs == 0)
                    do_this_file = TRUE;
                else {  /* check if this entry matches an `include' argument */
                    do_this_file = FALSE;
                    for (i = 0; i < G.filespecs; i++)
                        if (match(G.filename, G.pfnames[i], uO.C_flag WISEP)) {
                            do_this_file = TRUE;  /* ^-- ignore case or not? */
                            if (fn_matched)
                                fn_matched[i] = TRUE;
                            break;       /* found match, so stop looping */
                        }
                }
                if (do_this_file) {  /* check if this is an excluded file */
                    for (i = 0; i < G.xfilespecs; i++)
                        if (match(G.filename, G.pxnames[i], uO.C_flag WISEP)) {
                            do_this_file = FALSE; /* ^-- ignore case or not? */
                            if (xn_matched)
                                xn_matched[i] = TRUE;
                            break;
                        }
                }
                if (do_this_file) {
                    if (store_info(__G))
                        ++j;            /* file is OK */
                    else
                        ++num_skipped;  /* unsupp. compression or encryption */
                }
            } /* end if (process_all_files) */

            members_processed++;

        } /* end while-loop (adding files to current block) */

        /* save position in central directory so can come back later */
        cd_bufstart = G.cur_zipfile_bufstart;
        cd_inptr = G.inptr;
        cd_incnt = G.incnt;

    /*-----------------------------------------------------------------------
        Second loop:  process files in current block, extracting or testing
        each one.
      -----------------------------------------------------------------------*/

#if defined(UNICODE_SUPPORT) && defined(WIN32_WIDE)
        if (G.has_win32_wide)
        {
          error = extract_or_test_entrylistw(__G__ j,
                        &filnum, &num_bad_pwd, &old_extra_bytes,
# ifdef SET_DIR_ATTRIB
                        &num_dirs, &dirlistw,
# endif
                        error_in_archive);
        }
        else
#endif /* (defined(UNICODE_SUPPORT) && defined(WIN32_WIDE)) */
        {
          error = extract_or_test_entrylist(__G__ j,
                        &filnum, &num_bad_pwd, &old_extra_bytes,
#ifdef SET_DIR_ATTRIB
                        &num_dirs, &dirlist,
#endif
                        error_in_archive);
        }
        if (error != PK_COOL) {
            if (error > error_in_archive)
                error_in_archive = error;
            /* ...and keep going (unless disk full or user break) */
            if (G.disk_full > 1 || error_in_archive == IZ_CTRLC) {
                /* clear reached_end to signal premature stop ... */
                reached_end = FALSE;
                /* ... and cancel scanning the central directory */
                break;
            }
        }


        /*
         * Jump back to where we were in the central directory, then go and do
         * the next batch of files.
         */

#ifdef USE_STRM_INPUT
        zfseeko(G.zipfd, cd_bufstart, SEEK_SET);
        G.cur_zipfile_bufstart = zftello(G.zipfd);
#else /* !USE_STRM_INPUT */
        G.cur_zipfile_bufstart =
          zlseek(G.zipfd, cd_bufstart, SEEK_SET);
#endif /* ?USE_STRM_INPUT */
        read(G.zipfd, (char *)G.inbuf, INBUFSIZ);  /* been here before... */
        G.inptr = cd_inptr;
        G.incnt = cd_incnt;
        ++blknum;

#ifdef TEST
        printf("\ncd_bufstart = %ld (%.8lXh)\n", cd_bufstart, cd_bufstart);
        printf("cur_zipfile_bufstart = %ld (%.8lXh)\n", cur_zipfile_bufstart,
          cur_zipfile_bufstart);
        printf("inptr-inbuf = %d\n", G.inptr-G.inbuf);
        printf("incnt = %d\n\n", G.incnt);
#endif

    } /* end while-loop (blocks of files in central directory) */

/*---------------------------------------------------------------------------
    Process the list of deferred symlink extractions and finish up
    the symbolic links.
  ---------------------------------------------------------------------------*/

#ifdef SYMLINKS
    if (G.slink_last != NULL) {
        if (QCOND2)
            Info(slide, 0, ((char *)slide, LoadFarString(SymLnkDeferred)));
        while (G.slink_head != NULL) {
           set_deferred_symlink(__G__ G.slink_head);
           /* remove the processed entry from the chain and free its memory */
           G.slink_last = G.slink_head;
           G.slink_head = G.slink_last->next;
           free(G.slink_last);
       }
       G.slink_last = NULL;
    }
#endif /* SYMLINKS */

/*---------------------------------------------------------------------------
    Go back through saved list of directories, sort and set times/perms/UIDs
    and GIDs from the deepest level on up.
  ---------------------------------------------------------------------------*/

#ifdef SET_DIR_ATTRIB
    if (num_dirs > 0) {
# if defined(UNICODE_SUPPORT) && defined(WIN32_WIDE)
      if (G.has_win32_wide) {
        sorted_dirlistw = (direntryw **)malloc(num_dirs*sizeof(direntryw *));
        if (sorted_dirlistw == (direntryw **)NULL) {
            Info(slide, 0x401, ((char *)slide,
              LoadFarString(DirlistSortNoMem)));
            while (dirlistw != (direntryw *)NULL) {
                direntryw *dw = dirlistw;

                dirlistw = dirlistw->next;
                free(dw);
            }
        } else {
            ulg ndirs_fail = 0;

            if (num_dirs == 1)
                sorted_dirlistw[0] = dirlistw;
            else {
                for (i = 0;  i < num_dirs;  ++i) {
                    sorted_dirlistw[i] = dirlistw;
                    dirlistw = dirlistw->next;
                }
                qsort((char *)sorted_dirlistw, num_dirs, sizeof(direntryw *),
                  dircompw);
            }

            Trace((stderr, "setting directory times/perms/attributes\n"));
            for (i = 0;  i < num_dirs;  ++i) {
                direntryw *dw = sorted_dirlistw[i];

                Trace((stderr, "dir = %s\n", dw->fnw));
                if ((error = set_direc_attribsw(__G__ dw)) != PK_OK) {
                    ndirs_fail++;
                    Info(slide, 0x201, ((char *)slide,
                      LoadFarString(DirlistSetAttrFailed), dw->fnw));
                    if (!error_in_archive)
                        error_in_archive = error;
                }
                free(dw);
            }
            free(sorted_dirlistw);
            if (!uO.tflag && QCOND2) {
                if (ndirs_fail > 0)
                    Info(slide, 0, ((char *)slide,
                      LoadFarString(DirlistFailAttrSum), ndirs_fail));
            }
        }
      }
      else
# endif /* (defined(UNICODE_SUPPORT) && defined(WIN32_WIDE)) */
      {
        sorted_dirlist = (direntry **)malloc(num_dirs*sizeof(direntry *));
        if (sorted_dirlist == (direntry **)NULL) {
            Info(slide, 0x401, ((char *)slide,
              LoadFarString(DirlistSortNoMem)));
            while (dirlist != (direntry *)NULL) {
                direntry *d = dirlist;

                dirlist = dirlist->next;
                free(d);
            }
        } else {
            ulg ndirs_fail = 0;

            if (num_dirs == 1)
                sorted_dirlist[0] = dirlist;
            else {
                for (i = 0;  i < num_dirs;  ++i) {
                    sorted_dirlist[i] = dirlist;
                    dirlist = dirlist->next;
                }
                qsort((char *)sorted_dirlist, num_dirs, sizeof(direntry *),
                  dircomp);
            }

            Trace((stderr, "setting directory times/perms/attributes\n"));
            for (i = 0;  i < num_dirs;  ++i) {
                direntry *d = sorted_dirlist[i];

                Trace((stderr, "dir = %s\n", d->fn));
                if ((error = set_direc_attribs(__G__ d)) != PK_OK) {
                    ndirs_fail++;
                    Info(slide, 0x201, ((char *)slide,
                      LoadFarString(DirlistSetAttrFailed), d->fn));
                    if (!error_in_archive)
                        error_in_archive = error;
                }
                free(d);
            }
            free(sorted_dirlist);
            if (!uO.tflag && QCOND2) {
                if (ndirs_fail > 0)
                    Info(slide, 0, ((char *)slide,
                      LoadFarString(DirlistFailAttrSum), ndirs_fail));
            }
        }
      }
    }
#endif /* SET_DIR_ATTRIB */

/*---------------------------------------------------------------------------
    Check for unmatched filespecs on command line and print warning if any
    found.  Free allocated memory.  (But suppress check when central dir
    scan was interrupted prematurely.)
  ---------------------------------------------------------------------------*/

    if (fn_matched) {
        if (reached_end) for (i = 0;  i < G.filespecs;  ++i)
            if (!fn_matched[i]) {
#ifdef DLL
                if (!G.redirect_data && !G.redirect_text)
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarString(FilenameNotMatched), G.pfnames[i]));
                else
                    setFileNotFound(__G);
#else
                Info(slide, 1, ((char *)slide,
                  LoadFarString(FilenameNotMatched), G.pfnames[i]));
#endif
                if (error_in_archive <= PK_WARN)
                    error_in_archive = PK_FIND;   /* some files not found */
            }
        free((zvoid *)fn_matched);
    }
    if (xn_matched) {
        if (reached_end) for (i = 0;  i < G.xfilespecs;  ++i)
            if (!xn_matched[i])
                Info(slide, 0x401, ((char *)slide,
                  LoadFarString(ExclFilenameNotMatched), G.pxnames[i]));
        free((zvoid *)xn_matched);
    }

/*---------------------------------------------------------------------------
    Now, all locally allocated memory has been released.  When the central
    directory processing has been interrupted prematurely, it is safe to
    return immediately.  All completeness checks and summary messages are
    skipped in this case.
  ---------------------------------------------------------------------------*/
    if (!reached_end)
        return error_in_archive;

/*---------------------------------------------------------------------------
    Double-check that we're back at the end-of-central-directory record, and
    print quick summary of results, if we were just testing the archive.  We
    send the summary to stdout so that people doing the testing in the back-
    ground and redirecting to a file can just do a "tail" on the output file.
  ---------------------------------------------------------------------------*/

#ifndef SFX
    if (no_endsig_found) {                      /* just to make sure */
        Info(slide, 0x401, ((char *)slide, LoadFarString(EndSigMsg)));
        Info(slide, 0x401, ((char *)slide, LoadFarString(ReportMsg)));
        if (!error_in_archive)       /* don't overwrite stronger error */
            error_in_archive = PK_WARN;
    }
#endif /* !SFX */
    if (uO.tflag) {
        ulg num = filnum - num_bad_pwd;

        if (uO.qflag < 2) {        /* GRR 930710:  was (uO.qflag == 1) */
            if (error_in_archive)
                Info(slide, 0, ((char *)slide, LoadFarString(ErrorInArchive),
                  (error_in_archive == PK_WARN)? "warning-" : "", G.zipfn));
            else if (num == 0L)
                Info(slide, 0, ((char *)slide, LoadFarString(ZeroFilesTested),
                  G.zipfn));
            else if (G.process_all_files && (num_skipped+num_bad_pwd == 0L))
                Info(slide, 0, ((char *)slide, LoadFarString(NoErrInCompData),
                  G.zipfn));
            else
                Info(slide, 0, ((char *)slide, LoadFarString(NoErrInTestedFiles)
                  , G.zipfn, num, (num==1L)? "":"s"));
            if (num_skipped > 0L)
                Info(slide, 0, ((char *)slide, LoadFarString(FilesSkipped),
                  num_skipped, (num_skipped==1L)? "":"s"));
#if CRYPT
            if (num_bad_pwd > 0L)
                Info(slide, 0, ((char *)slide, LoadFarString(FilesSkipBadPasswd)
                  , num_bad_pwd, (num_bad_pwd==1L)? "":"s"));
#endif /* CRYPT */
        }
    }

    /* give warning if files not tested or extracted (first condition can still
     * happen if zipfile is empty and no files specified on command line) */

    if ((filnum == 0) && error_in_archive <= PK_WARN) {
        if (num_skipped > 0L)
            error_in_archive = IZ_UNSUP; /* unsupport. compression/encryption */
        else
            error_in_archive = PK_FIND;  /* no files found at all */
    }
#if CRYPT
    else if ((filnum == num_bad_pwd) && error_in_archive <= PK_WARN)
        error_in_archive = IZ_BADPWD;    /* bad passwd => all files skipped */
#endif
    else if ((num_skipped > 0L) && error_in_archive <= PK_WARN)
        error_in_archive = IZ_UNSUP;     /* was PK_WARN; Jean-loup complained */
#if CRYPT
    else if ((num_bad_pwd > 0L) && !error_in_archive)
        error_in_archive = PK_WARN;
#endif

    return error_in_archive;

} /* end function extract_or_test_files() */





/***************************/
/*  Function store_info()  */
/***************************/

static int store_info(__G)   /* return 0 if skipping, 1 if OK */
    __GDEF
{
#ifdef USE_BZIP2
#  define UNKN_BZ2 (G.crec.compression_method!=BZIPPED)
#else
#  define UNKN_BZ2 TRUE       /* bzip2 unknown */
#endif

#ifdef USE_LZMA
#  define UNKN_LZMA (G.crec.compression_method!=LZMAED)
#else
#  define UNKN_LZMA TRUE      /* LZMA unknown */
#endif

#ifdef USE_WAVP
#  define UNKN_WAVP (G.crec.compression_method!=WAVPACKED)
#else
#  define UNKN_WAVP TRUE      /* WavPack unknown */
#endif

#ifdef USE_PPMD
#  define UNKN_PPMD (G.crec.compression_method!=PPMDED)
#else
#  define UNKN_PPMD TRUE      /* PPMd unknown */
#endif

#ifdef SFX
# ifdef USE_DEFLATE64
#    define UNKN_COMPR \
     (G.crec.compression_method!=STORED && G.crec.compression_method<DEFLATED \
      && G.crec.compression_method>ENHDEFLATED \
      && UNKN_BZ2 && UNKN_LZMA && UNKN_WAVP && UNKN_PPMD)
# else
#    define UNKN_COMPR \
     (G.crec.compression_method!=STORED && G.crec.compression_method!=DEFLATED\
      && UNKN_BZ2 && UNKN_LZMA && UNKN_WAVP && UNKN_PPMD)
# endif
#else
# ifdef COPYRIGHT_CLEAN  /* no reduced files */
#    define UNKN_RED (G.crec.compression_method >= REDUCED1 && \
                      G.crec.compression_method <= REDUCED4)
# else
#    define UNKN_RED  FALSE  /* reducing not unknown */
# endif
# ifdef LZW_CLEAN  /* no shrunk files */
#    define UNKN_SHR (G.crec.compression_method == SHRUNK)
# else
#    define UNKN_SHR  FALSE  /* unshrinking not unknown */
# endif
# ifdef USE_DEFLATE64
#    define UNKN_COMPR (UNKN_RED || UNKN_SHR || \
     G.crec.compression_method==TOKENIZED || \
     (G.crec.compression_method>ENHDEFLATED && UNKN_BZ2 && UNKN_LZMA \
      && UNKN_WAVP && UNKN_PPMD))
# else
#    define UNKN_COMPR (UNKN_RED || UNKN_SHR || \
     G.crec.compression_method==TOKENIZED || \
     (G.crec.compression_method>DEFLATED && UNKN_BZ2 && UNKN_LZMA \
      && UNKN_WAVP && UNKN_PPMD))
# endif
#endif

#if (defined(USE_BZIP2) && (UNZIP_VERSION < UNZIP_BZ2VERS))
    int unzvers_support = (UNKN_BZ2 ? UNZIP_VERSION : UNZIP_BZ2VERS);
#   define UNZVERS_SUPPORT  unzvers_support
#else
#   define UNZVERS_SUPPORT  UNZIP_VERSION
#endif

/*---------------------------------------------------------------------------
    Check central directory info for version/compatibility requirements.
  ---------------------------------------------------------------------------*/

    G.pInfo->encrypted = G.crec.general_purpose_bit_flag & 1;   /* bit field */
    G.pInfo->ExtLocHdr = (G.crec.general_purpose_bit_flag & 8) == 8;  /* bit */
    G.pInfo->textfile = G.crec.internal_file_attributes & 1;    /* bit field */
    G.pInfo->crc = G.crec.crc32;
    G.pInfo->compr_size = G.crec.csize;
    G.pInfo->uncompr_size = G.crec.ucsize;

    switch (uO.aflag) {
        case 0:
            G.pInfo->textmode = FALSE;   /* bit field */
            break;
        case 1:
            G.pInfo->textmode = G.pInfo->textfile;   /* auto-convert mode */
            break;
        default:  /* case 2: */
            G.pInfo->textmode = TRUE;
            break;
    }

    if (G.crec.version_needed_to_extract[1] == VMS_) {
        if (G.crec.version_needed_to_extract[0] > VMS_UNZIP_VERSION) {
            if (!((uO.tflag && uO.qflag) || (!uO.tflag && !QCOND2)))
                Info(slide, 0x401, ((char *)slide, LoadFarString(VersionMsg),
                  FnFilter1(G.filename), "VMS",
                  G.crec.version_needed_to_extract[0] / 10,
                  G.crec.version_needed_to_extract[0] % 10,
                  VMS_UNZIP_VERSION / 10, VMS_UNZIP_VERSION % 10));
            return 0;
        }
#ifndef VMS   /* won't be able to use extra field, but still have data */
        else if (!uO.tflag && !IS_OVERWRT_ALL) { /* if -o, extract anyway */
            Info(slide, 0x481, ((char *)slide, LoadFarString(VMSFormatQuery),
              FnFilter1(G.filename)));
            fgets(G.answerbuf, sizeof(G.answerbuf) - 1, stdin);
            if ((*G.answerbuf != 'y') && (*G.answerbuf != 'Y'))
                return 0;
        }
#endif /* !VMS */
    /* usual file type:  don't need VMS to extract */
    } else if (G.crec.version_needed_to_extract[0] > UNZVERS_SUPPORT) {
        if (!((uO.tflag && uO.qflag) || (!uO.tflag && !QCOND2)))
            Info(slide, 0x401, ((char *)slide, LoadFarString(VersionMsg),
              FnFilter1(G.filename), "PK",
              G.crec.version_needed_to_extract[0] / 10,
              G.crec.version_needed_to_extract[0] % 10,
              UNZVERS_SUPPORT / 10, UNZVERS_SUPPORT % 10));
        return 0;
    }

    if (UNKN_COMPR) {
        if (!((uO.tflag && uO.qflag) || (!uO.tflag && !QCOND2))) {
#ifndef SFX
            unsigned cmpridx;

            if ((cmpridx = find_compr_idx(G.crec.compression_method))
                < NUM_METHODS)
                Info(slide, 0x401, ((char *)slide, LoadFarString(ComprMsgName),
                  FnFilter1(G.filename),
                  LoadFarStringSmall(ComprNames[cmpridx])));
            else
#endif
                Info(slide, 0x401, ((char *)slide, LoadFarString(ComprMsgNum),
                  FnFilter1(G.filename),
                  G.crec.compression_method));
        }
        return 0;
    }
#if (!CRYPT)
    if (G.pInfo->encrypted) {
        if (!((uO.tflag && uO.qflag) || (!uO.tflag && !QCOND2)))
            Info(slide, 0x401, ((char *)slide, LoadFarString(SkipEncrypted),
              FnFilter1(G.filename)));
        return 0;
    }
#endif /* !CRYPT */

#ifndef SFX
    /* store a copy of the central header filename for later comparison */
    if ((G.pInfo->cfilname = zfmalloc(strlen(G.filename) + 1)) == NULL) {
        Info(slide, 0x401, ((char *)slide, LoadFarString(WarnNoMemCFName),
          FnFilter1(G.filename)));
    } else
        zfstrcpy(G.pInfo->cfilname, G.filename);
#endif /* !SFX */

    /* map whatever file attributes we have into the local format */
    mapattr(__G);   /* GRR:  worry about return value later */

    G.pInfo->diskstart = G.crec.disk_number_start;
    G.pInfo->offset = (zoff_t)G.crec.relative_offset_local_header;
    return 1;

} /* end function store_info() */





#ifndef SFX
/*******************************/
/*  Function find_compr_idx()  */
/*******************************/

unsigned find_compr_idx(compr_methodnum)
    unsigned compr_methodnum;
{
   unsigned i;

   for (i = 0; i < NUM_METHODS; i++) {
      if (ComprIDs[i] == compr_methodnum) break;
   }
   return i;
}
#endif /* !SFX */





/******************************************/
/*  Function extract_or_test_entrylist()  */
/******************************************/

static int extract_or_test_entrylist(__G__ numchunk,
                pfilnum, pnum_bad_pwd, pold_extra_bytes,
#ifdef SET_DIR_ATTRIB
                pnum_dirs, pdirlist,
#endif
                error_in_archive)    /* return PK-type error code */
    __GDEF
    unsigned numchunk;
    ulg *pfilnum;
    ulg *pnum_bad_pwd;
    zoff_t *pold_extra_bytes;
#ifdef SET_DIR_ATTRIB
    unsigned *pnum_dirs;
    direntry **pdirlist;
#endif
    int error_in_archive;
{
    unsigned i;
    int renamed, query;
    int skip_entry;
    zoff_t bufstart, inbuf_offset, request;
    int error, errcode;

/* possible values for local skip_entry flag: */
#define SKIP_NO         0       /* do not skip this entry */
#define SKIP_Y_EXISTING 1       /* skip this entry, do not overwrite file */
#define SKIP_Y_NONEXIST 2       /* skip this entry, do not create new file */

    /*-----------------------------------------------------------------------
        Second loop:  process files in current block, extracting or testing
        each one.
      -----------------------------------------------------------------------*/

    for (i = 0; i < numchunk; ++i) {
        (*pfilnum)++;   /* *pfilnum = i + blknum*DIR_BLKSIZ + 1; */
        G.pInfo = &G.info[i];
#ifdef NOVELL_BUG_FAILSAFE
        G.dne = FALSE;  /* assume file exists until stat() says otherwise */
#endif

        /* if the target position is not within the current input buffer
         * (either haven't yet read far enough, or (maybe) skipping back-
         * ward), skip to the target position and reset readbuf(). */

        /* seek_zipf(__G__ pInfo->offset);  */
        request = G.pInfo->offset + G.extra_bytes;
        inbuf_offset = request % INBUFSIZ;
        bufstart = request - inbuf_offset;

        Trace((stderr, "\ndebug: request = %ld, inbuf_offset = %ld\n",
          (long)request, (long)inbuf_offset));
        Trace((stderr,
          "debug: bufstart = %ld, cur_zipfile_bufstart = %ld\n",
          (long)bufstart, (long)G.cur_zipfile_bufstart));
        if (request < 0) {
            Info(slide, 0x401, ((char *)slide, LoadFarStringSmall(SeekMsg),
              G.zipfn, LoadFarString(ReportMsg)));
            error_in_archive = PK_ERR;
            if (*pfilnum == 1 && G.extra_bytes != 0L) {
                Info(slide, 0x401, ((char *)slide,
                  LoadFarString(AttemptRecompensate)));
                *pold_extra_bytes = G.extra_bytes;
                G.extra_bytes = 0L;
                request = G.pInfo->offset;  /* could also check if != 0 */
                inbuf_offset = request % INBUFSIZ;
                bufstart = request - inbuf_offset;
                Trace((stderr, "debug: request = %ld, inbuf_offset = %ld\n",
                  (long)request, (long)inbuf_offset));
                Trace((stderr,
                  "debug: bufstart = %ld, cur_zipfile_bufstart = %ld\n",
                  (long)bufstart, (long)G.cur_zipfile_bufstart));
                /* try again */
                if (request < 0) {
                    Trace((stderr,
                      "debug: recompensated request still < 0\n"));
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarStringSmall(SeekMsg),
                      G.zipfn, LoadFarString(ReportMsg)));
                    error_in_archive = PK_BADERR;
                    continue;
                }
            } else {
                error_in_archive = PK_BADERR;
                continue;  /* this one hosed; try next */
            }
        }

        if (bufstart != G.cur_zipfile_bufstart) {
            Trace((stderr, "debug: bufstart != cur_zipfile_bufstart\n"));
#ifdef USE_STRM_INPUT
            zfseeko(G.zipfd, bufstart, SEEK_SET);
            G.cur_zipfile_bufstart = zftello(G.zipfd);
#else /* !USE_STRM_INPUT */
            G.cur_zipfile_bufstart =
              zlseek(G.zipfd, bufstart, SEEK_SET);
#endif /* ?USE_STRM_INPUT */
            if ((G.incnt = read(G.zipfd, (char *)G.inbuf, INBUFSIZ)) <= 0)
            {
                Info(slide, 0x401, ((char *)slide, LoadFarString(OffsetMsg),
                  *pfilnum, "lseek", (long)bufstart));
                error_in_archive = PK_BADERR;
                continue;   /* can still do next file */
            }
            G.inptr = G.inbuf + (int)inbuf_offset;
            G.incnt -= (int)inbuf_offset;
        } else {
            G.incnt += (int)(G.inptr-G.inbuf) - (int)inbuf_offset;
            G.inptr = G.inbuf + (int)inbuf_offset;
        }

        /* should be in proper position now, so check for sig */
        if (readbuf(__G__ G.sig, 4) == 0) {  /* bad offset */
            Info(slide, 0x401, ((char *)slide, LoadFarString(OffsetMsg),
              *pfilnum, "EOF", (long)request));
            error_in_archive = PK_BADERR;
            continue;   /* but can still try next one */
        }
        if (strncmp(G.sig, local_hdr_sig, 4)) {
            Info(slide, 0x401, ((char *)slide, LoadFarString(OffsetMsg),
              *pfilnum, LoadFarStringSmall(LocalHdrSig), (long)request));
            /*
                GRRDUMP(G.sig, 4)
                GRRDUMP(local_hdr_sig, 4)
             */
            error_in_archive = PK_ERR;
            if ((*pfilnum == 1 && G.extra_bytes != 0L) ||
                (G.extra_bytes == 0L && *pold_extra_bytes != 0L)) {
                Info(slide, 0x401, ((char *)slide,
                  LoadFarString(AttemptRecompensate)));
                if (G.extra_bytes) {
                    *pold_extra_bytes = G.extra_bytes;
                    G.extra_bytes = 0L;
                } else
                    G.extra_bytes = *pold_extra_bytes; /* third attempt */
                if (((error = seek_zipf(__G__ G.pInfo->offset)) != PK_OK) ||
                    (readbuf(__G__ G.sig, 4) == 0)) {  /* bad offset */
                    if (error != PK_BADERR)
                      Info(slide, 0x401, ((char *)slide,
                        LoadFarString(OffsetMsg), *pfilnum, "EOF",
                        (long)request));
                    error_in_archive = PK_BADERR;
                    continue;   /* but can still try next one */
                }
                if (strncmp(G.sig, local_hdr_sig, 4)) {
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarString(OffsetMsg), *pfilnum,
                      LoadFarStringSmall(LocalHdrSig), (long)request));
                    error_in_archive = PK_BADERR;
                    continue;
                }
            } else
                continue;  /* this one hosed; try next */
        }
        if ((error = process_local_file_hdr(__G)) != PK_COOL) {
            Info(slide, 0x421, ((char *)slide, LoadFarString(BadLocalHdr),
              *pfilnum));
            error_in_archive = error;   /* only PK_EOF defined */
            continue;   /* can still try next one */
        }
        if ((error = do_string(__G__ G.lrec.filename_length, DS_FN_L)) !=
             PK_COOL)
        {
            if (error > error_in_archive)
                error_in_archive = error;
            if (error > PK_WARN) {
                Info(slide, 0x401, ((char *)slide, LoadFarString(FilNamMsg),
                  FnFilter1(G.filename), "local"));
                continue;   /* go on to next one */
            }
        }
        if (G.extra_field != (uch *)NULL) {
            free(G.extra_field);
            G.extra_field = (uch *)NULL;
        }
        if ((error =
             do_string(__G__ G.lrec.extra_field_length, EXTRA_FIELD)) != 0)
        {
            if (error > error_in_archive)
                error_in_archive = error;
            if (error > PK_WARN) {
                Info(slide, 0x401, ((char *)slide,
                  LoadFarString(ExtFieldMsg),
                  FnFilter1(G.filename), "local"));
                continue;   /* go on */
            }
        }
#ifndef SFX
        /* Filename consistency checks must come after reading in the local
         * extra field, so that a UTF-8 entry name e.f. block has already
         * been processed.
         */
        if (G.pInfo->cfilname != (char Far *)NULL) {
            if (zfstrcmp(G.pInfo->cfilname, G.filename) != 0) {
# ifdef SMALL_MEM
                char *temp_cfilnam = slide + (7 * (WSIZE>>3));

                zfstrcpy((char Far *)temp_cfilnam, G.pInfo->cfilname);
#  define  cFile_PrintBuf  temp_cfilnam
# else
#  define  cFile_PrintBuf  G.pInfo->cfilname
# endif
                Info(slide, 0x401, ((char *)slide,
                  LoadFarStringSmall2(LvsCFNamMsg),
                  FnFilter2(cFile_PrintBuf), FnFilter1(G.filename)));
# undef    cFile_PrintBuf
                zfstrcpy(G.filename, G.pInfo->cfilname);
                if (error_in_archive < PK_WARN)
                    error_in_archive = PK_WARN;
            }
            zffree(G.pInfo->cfilname);
            G.pInfo->cfilname = (char Far *)NULL;
        }
#endif /* !SFX */
        /* Size consistency checks must come after reading in the local extra
         * field, so that any Zip64 extension local e.f. block has already
         * been processed.
         */
        if (G.lrec.compression_method == STORED) {
            zusz_t csiz_decrypted = G.lrec.csize;

            if (G.pInfo->encrypted)
                csiz_decrypted -= 12;
            if (G.lrec.ucsize != csiz_decrypted) {
                Info(slide, 0x401, ((char *)slide,
                  LoadFarStringSmall2(WrnStorUCSizCSizDiff),
                  FnFilter1(G.filename),
                  FmZofft(G.lrec.ucsize, NULL, "u"),
                  FmZofft(csiz_decrypted, NULL, "u")));
                G.lrec.ucsize = csiz_decrypted;
                if (error_in_archive < PK_WARN)
                    error_in_archive = PK_WARN;
            }
        }

#if CRYPT
int decrypt(const char *);
        if (G.pInfo->encrypted &&
            (error = decrypt(__G__ uO.pwdarg)) != PK_COOL) {
            if (error == PK_WARN) {
                if (!((uO.tflag && uO.qflag) || (!uO.tflag && !QCOND2)))
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarString(SkipIncorrectPasswd),
                      FnFilter1(G.filename)));
                ++(*pnum_bad_pwd);
            } else {  /* (error > PK_WARN) */
                if (error > error_in_archive)
                    error_in_archive = error;
                Info(slide, 0x401, ((char *)slide,
                  LoadFarString(SkipCannotGetPasswd),
                  FnFilter1(G.filename)));
            }
            continue;   /* go on to next file */
        }
#endif /* CRYPT */

        /*
         * just about to extract file:  if extracting to disk, check if
         * already exists, and if so, take appropriate action according to
         * fflag/uflag/overwrite_all/etc. (we couldn't do this in upper
         * loop because we don't store the possibly renamed filename[] in
         * info[])
         */
#ifdef DLL
        if (!uO.tflag && !uO.cflag && !G.redirect_data)
#else
        if (!uO.tflag && !uO.cflag)
#endif
        {
            renamed = FALSE;   /* user hasn't renamed output file yet */

startover:
            query = FALSE;
            skip_entry = SKIP_NO;
            /* for files from DOS FAT, check for use of backslash instead
             *  of slash as directory separator (bug in some zipper(s); so
             *  far, not a problem in HPFS, NTFS or VFAT systems)
             */
#ifndef SFX
            if (G.pInfo->hostnum == FS_FAT_ && !MBSCHR(G.filename, '/')) {
                char *p=G.filename;

                if (*p) do {
                    if (*p == '\\') {
                        if (!G.reported_backslash) {
                            Info(slide, 0x21, ((char *)slide,
                              LoadFarString(BackslashPathSep), G.zipfn));
                            G.reported_backslash = TRUE;
                            if (!error_in_archive)
                                error_in_archive = PK_WARN;
                        }
                        *p = '/';
                    }
                } while (*PREINCSTR(p));
            }
#endif /* !SFX */

            if (!renamed) {
               /* remove absolute path specs */
               if (G.filename[0] == '/') {
                   Info(slide, 0x401, ((char *)slide,
                        LoadFarString(AbsolutePathWarning),
                        FnFilter1(G.filename)));
                   if (!error_in_archive)
                       error_in_archive = PK_WARN;
                   do {
                       char *p = G.filename + 1;
                       do {
                           *(p-1) = *p;
                       } while (*p++ != '\0');
                   } while (G.filename[0] == '/');
               }
            }

            /* mapname can create dirs if not freshening or if renamed */
            error = mapname(__G__ renamed);
            if ((errcode = error & ~MPN_MASK) != PK_OK &&
                error_in_archive < errcode)
                error_in_archive = errcode;
            if ((errcode = error & MPN_MASK) > MPN_INF_TRUNC) {
                if (errcode == MPN_CREATED_DIR) {
#ifdef SET_DIR_ATTRIB
                    direntry *d_entry;

                    error = defer_dir_attribs(__G__ &d_entry);
                    if (d_entry == (direntry *)NULL) {
                        /* There may be no dir_attribs info available, or
                         * we have encountered a mem allocation error.
                         * In case of an error, report it and set program
                         * error state to warning level.
                         */
                        if (error) {
                            Info(slide, 0x401, ((char *)slide,
                                 LoadFarString(DirlistEntryNoMem)));
                            if (!error_in_archive)
                                error_in_archive = PK_WARN;
                        }
                    } else {
                        d_entry->next = (*pdirlist);
                        (*pdirlist) = d_entry;
                        ++(*pnum_dirs);
                    }
#endif /* SET_DIR_ATTRIB */
                } else if (errcode == MPN_VOL_LABEL) {
#ifdef DOS_OS2_W32
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarString(SkipVolumeLabel),
                      FnFilter1(G.filename),
                      uO.volflag? "hard disk " : ""));
#else
                    Info(slide, 1, ((char *)slide,
                      LoadFarString(SkipVolumeLabel),
                      FnFilter1(G.filename), ""));
#endif
                } else if (errcode > MPN_INF_SKIP &&
                           error_in_archive < PK_ERR)
                    error_in_archive = PK_ERR;
                Trace((stderr, "mapname(%s) returns error code = %d\n",
                  FnFilter1(G.filename), error));
                continue;   /* go on to next file */
            }

#ifdef QDOS
            QFilename(__G__ G.filename);
#endif
            switch (check_for_newer(__G__ G.filename)) {
                case DOES_NOT_EXIST:
#ifdef NOVELL_BUG_FAILSAFE
                    G.dne = TRUE;   /* stat() says file DOES NOT EXIST */
#endif
                    /* freshen (no new files): skip unless just renamed */
                    if (uO.fflag && !renamed)
                        skip_entry = SKIP_Y_NONEXIST;
                    break;
                case EXISTS_AND_OLDER:
#ifdef UNIXBACKUP
                    if (!uO.B_flag)
#endif
                    {
                        if (IS_OVERWRT_NONE)
                            /* never overwrite:  skip file */
                            skip_entry = SKIP_Y_EXISTING;
                        else if (!IS_OVERWRT_ALL)
                            query = TRUE;
                    }
                    break;
                case EXISTS_AND_NEWER:             /* (or equal) */
#ifdef UNIXBACKUP
                    if ((!uO.B_flag && IS_OVERWRT_NONE) ||
#else
                    if (IS_OVERWRT_NONE ||
#endif
                        (uO.uflag && !renamed)) {
                        /* skip if update/freshen & orig name */
                        skip_entry = SKIP_Y_EXISTING;
                    } else {
#ifdef UNIXBACKUP
                        if (!IS_OVERWRT_ALL && !uO.B_flag)
#else
                        if (!IS_OVERWRT_ALL)
#endif
                            query = TRUE;
                    }
                    break;
                }
            if (query) {
#ifdef WINDLL
                switch (G.lpUserFunctions->replace != NULL ?
                        (*G.lpUserFunctions->replace)(G.filename, FILNAMSIZ) :
                        IDM_REPLACE_NONE) {
                    case IDM_REPLACE_RENAME:
                        _ISO_INTERN(G.filename);
                        renamed = TRUE;
                        goto startover;
                    case IDM_REPLACE_ALL:
                        G.overwrite_mode = OVERWRT_ALWAYS;
                        /* FALL THROUGH, extract */
                    case IDM_REPLACE_YES:
                        break;
                    case IDM_REPLACE_NONE:
                        G.overwrite_mode = OVERWRT_NEVER;
                        /* FALL THROUGH, skip */
                    case IDM_REPLACE_NO:
                        skip_entry = SKIP_Y_EXISTING;
                        break;
                }
#else /* !WINDLL */
                extent fnlen;
reprompt:
                Info(slide, 0x81, ((char *)slide,
                  LoadFarString(ReplaceQuery),
                  FnFilter1(G.filename)));
                if (fgets(G.answerbuf, 9, stdin) == (char *)NULL) {
                    Info(slide, 1, ((char *)slide,
                      LoadFarString(AssumeNone)));
                    *G.answerbuf = 'N';
                    if (!error_in_archive)
                        error_in_archive = 1;  /* not extracted:  warning */
                }
                switch (*G.answerbuf) {
                    case 'r':
                    case 'R':
                        do {
                            Info(slide, 0x81, ((char *)slide,
                              LoadFarString(NewNameQuery)));
                            fgets(G.filename, FILNAMSIZ, stdin);
                            /* usually get \n here:  better check for it */
                            fnlen = strlen(G.filename);
                            if (lastchar(G.filename, fnlen) == '\n')
                                G.filename[--fnlen] = '\0';
                        } while (fnlen == 0);
# ifdef WIN32  /* WIN32 fgets( ... , stdin) returns OEM coded strings */
                        _OEM_INTERN(G.filename);
# endif
                        renamed = TRUE;
                        goto startover;   /* sorry for a goto */
                    case 'A':   /* dangerous option:  force caps */
                        G.overwrite_mode = OVERWRT_ALWAYS;
                        /* FALL THROUGH, extract */
                    case 'y':
                    case 'Y':
                        break;
                    case 'N':
                        G.overwrite_mode = OVERWRT_NEVER;
                        /* FALL THROUGH, skip */
                    case 'n':
                        /* skip file */
                        skip_entry = SKIP_Y_EXISTING;
                        break;
                    case '\n':
                    case '\r':
                        /* Improve echo of '\n' and/or '\r'
                           (sizeof(G.answerbuf) == 10 (see globals.h), so
                           there is enough space for the provided text...) */
                        strcpy(G.answerbuf, "{ENTER}");
                        /* fall through ... */
                    default:
                        Info(slide, 1, ((char *)slide,
                          LoadFarString(InvalidResponse), *G.answerbuf));
                        goto reprompt;   /* yet another goto? */
                } /* end switch (*answerbuf) */
#endif /* ?WINDLL */
            } /* end if (query) */
            if (skip_entry != SKIP_NO) {
#ifdef WINDLL
                if (skip_entry == SKIP_Y_EXISTING) {
                    /* report skipping of an existing entry */
                    Info(slide, 0, ((char *)slide,
                      ((IS_OVERWRT_NONE || !uO.uflag || renamed) ?
                       "Target file exists.\nSkipping %s\n" :
                       "Target file newer.\nSkipping %s\n"),
                      FnFilter1(G.filename)));
                }
#endif /* WINDLL */
                continue;
            }
        } /* end if (extracting to disk) */

#ifdef DLL
        if ((G.statreportcb != NULL) &&
            (*G.statreportcb)(__G__ UZ_ST_START_EXTRACT, G.zipfn,
                              G.filename, NULL)) {
            return IZ_CTRLC;        /* cancel operation by user request */
        }
#endif
#ifdef MACOS  /* MacOS is no preemptive OS, thus call event-handling by hand */
        UserStop();
#endif
#ifdef AMIGA
        G.filenote_slot = i;
#endif
        G.disk_full = 0;
        if ((error = extract_or_test_member(__G)) != PK_COOL) {
            if (error > error_in_archive)
                error_in_archive = error;       /* ...and keep going */
#ifdef DLL
            if (G.disk_full > 1 || error_in_archive == IZ_CTRLC) {
#else
            if (G.disk_full > 1) {
#endif
                return error_in_archive;        /* (unless disk full) */
            }
        }
#ifdef DLL
        if ((G.statreportcb != NULL) &&
            (*G.statreportcb)(__G__ UZ_ST_FINISH_MEMBER, G.zipfn,
                              G.filename, (zvoid *)&G.lrec.ucsize)) {
            return IZ_CTRLC;        /* cancel operation by user request */
        }
#endif
#ifdef MACOS  /* MacOS is no preemptive OS, thus call event-handling by hand */
        UserStop();
#endif
    } /* end for-loop (i:  files in current block) */
 
    return error_in_archive;
 
} /* end function extract_or_test_entrylistw() */



#if defined(UNICODE_SUPPORT) && defined(WIN32_WIDE)

static int extract_or_test_entrylistw(__G__ numchunk,
                pfilnum, pnum_bad_pwd, pold_extra_bytes,
# ifdef SET_DIR_ATTRIB
                pnum_dirs, pdirlistw,
# endif
                error_in_archive)    /* return PK-type error code */
    __GDEF
    unsigned numchunk;
    ulg *pfilnum;
    ulg *pnum_bad_pwd;
    zoff_t *pold_extra_bytes;
# ifdef SET_DIR_ATTRIB
    unsigned *pnum_dirs;
    direntryw **pdirlistw;
# endif
    int error_in_archive;
{
    int cfn;
    unsigned i;
    int renamed, query;
    int skip_entry;
    zoff_t bufstart, inbuf_offset, request;
    int error, errcode;

/* possible values for local skip_entry flag: */
#define SKIP_NO         0       /* do not skip this entry */
#define SKIP_Y_EXISTING 1       /* skip this entry, do not overwrite file */
#define SKIP_Y_NONEXIST 2       /* skip this entry, do not create new file */

    /*-----------------------------------------------------------------------
        Second loop:  process files in current block, extracting or testing
        each one.
      -----------------------------------------------------------------------*/

    for (i = 0; i < numchunk; ++i) {
        (*pfilnum)++;   /* *pfilnum = i + blknum*DIR_BLKSIZ + 1; */
        G.pInfo = &G.info[i];
# ifdef NOVELL_BUG_FAILSAFE
        G.dne = FALSE;  /* assume file exists until stat() says otherwise */
# endif

        /* if the target position is not within the current input buffer
         * (either haven't yet read far enough, or (maybe) skipping back-
         * ward), skip to the target position and reset readbuf(). */

        /* seek_zipf(__G__ pInfo->offset);  */
        request = G.pInfo->offset + G.extra_bytes;
        inbuf_offset = request % INBUFSIZ;
        bufstart = request - inbuf_offset;

        Trace((stderr, "\ndebug: request = %ld, inbuf_offset = %ld\n",
          (long)request, (long)inbuf_offset));
        Trace((stderr,
          "debug: bufstart = %ld, cur_zipfile_bufstart = %ld\n",
          (long)bufstart, (long)G.cur_zipfile_bufstart));
        if (request < 0) {
            Info(slide, 0x401, ((char *)slide, LoadFarStringSmall(SeekMsg),
              G.zipfn, LoadFarString(ReportMsg)));
            error_in_archive = PK_ERR;
            if (*pfilnum == 1 && G.extra_bytes != 0L) {
                Info(slide, 0x401, ((char *)slide,
                  LoadFarString(AttemptRecompensate)));
                *pold_extra_bytes = G.extra_bytes;
                G.extra_bytes = 0L;
                request = G.pInfo->offset;  /* could also check if != 0 */
                inbuf_offset = request % INBUFSIZ;
                bufstart = request - inbuf_offset;
                Trace((stderr, "debug: request = %ld, inbuf_offset = %ld\n",
                  (long)request, (long)inbuf_offset));
                Trace((stderr,
                  "debug: bufstart = %ld, cur_zipfile_bufstart = %ld\n",
                  (long)bufstart, (long)G.cur_zipfile_bufstart));
                /* try again */
                if (request < 0) {
                    Trace((stderr,
                      "debug: recompensated request still < 0\n"));
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarStringSmall(SeekMsg),
                      G.zipfn, LoadFarString(ReportMsg)));
                    error_in_archive = PK_BADERR;
                    continue;
                }
            } else {
                error_in_archive = PK_BADERR;
                continue;  /* this one hosed; try next */
            }
        }

        if (bufstart != G.cur_zipfile_bufstart) {
            Trace((stderr, "debug: bufstart != cur_zipfile_bufstart\n"));
# ifdef USE_STRM_INPUT
            zfseeko(G.zipfd, bufstart, SEEK_SET);
            G.cur_zipfile_bufstart = zftello(G.zipfd);
# else /* !USE_STRM_INPUT */
            G.cur_zipfile_bufstart =
              zlseek(G.zipfd, bufstart, SEEK_SET);
# endif /* ?USE_STRM_INPUT */
            if ((G.incnt = read(G.zipfd, (char *)G.inbuf, INBUFSIZ)) <= 0)
            {
                Info(slide, 0x401, ((char *)slide, LoadFarString(OffsetMsg),
                  *pfilnum, "lseek", (long)bufstart));
                error_in_archive = PK_BADERR;
                continue;   /* can still do next file */
            }
            G.inptr = G.inbuf + (int)inbuf_offset;
            G.incnt -= (int)inbuf_offset;
        } else {
            G.incnt += (int)(G.inptr-G.inbuf) - (int)inbuf_offset;
            G.inptr = G.inbuf + (int)inbuf_offset;
        }

        /* should be in proper position now, so check for sig */
        if (readbuf(__G__ G.sig, 4) == 0) {  /* bad offset */
            Info(slide, 0x401, ((char *)slide, LoadFarString(OffsetMsg),
              *pfilnum, "EOF", (long)request));
            error_in_archive = PK_BADERR;
            continue;   /* but can still try next one */
        }
        if (memcmp(G.sig, local_hdr_sig, 4)) {
            Info(slide, 0x401, ((char *)slide, LoadFarString(OffsetMsg),
              *pfilnum, LoadFarStringSmall(LocalHdrSig), (long)request));
            /*
                GRRDUMP(G.sig, 4)
                GRRDUMP(local_hdr_sig, 4)
             */
            error_in_archive = PK_ERR;
            if ((*pfilnum == 1 && G.extra_bytes != 0L) ||
                (G.extra_bytes == 0L && *pold_extra_bytes != 0L)) {
                Info(slide, 0x401, ((char *)slide,
                  LoadFarString(AttemptRecompensate)));
                if (G.extra_bytes) {
                    *pold_extra_bytes = G.extra_bytes;
                    G.extra_bytes = 0L;
                } else
                    G.extra_bytes = *pold_extra_bytes; /* third attempt */
                if (((error = seek_zipf(__G__ G.pInfo->offset)) != PK_OK) ||
                    (readbuf(__G__ G.sig, 4) == 0)) {  /* bad offset */
                    if (error != PK_BADERR)
                      Info(slide, 0x401, ((char *)slide,
                        LoadFarString(OffsetMsg), *pfilnum, "EOF",
                        (long)request));
                    error_in_archive = PK_BADERR;
                    continue;   /* but can still try next one */
                }
                if (memcmp(G.sig, local_hdr_sig, 4)) {
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarString(OffsetMsg), *pfilnum,
                      LoadFarStringSmall(LocalHdrSig), (long)request));
                    error_in_archive = PK_BADERR;
                    continue;
                }
            } else
                continue;  /* this one hosed; try next */
        }
        if ((error = process_local_file_hdr(__G)) != PK_COOL) {
            Info(slide, 0x421, ((char *)slide, LoadFarString(BadLocalHdr),
              *pfilnum));
            error_in_archive = error;   /* only PK_EOF defined */
            continue;   /* can still try next one */
        }
# if (!defined(SFX) && defined(UNICODE_SUPPORT))
        if (((G.lrec.general_purpose_bit_flag & UTF8_BIT) == UTF8_BIT)
            != (G.pInfo->GPFIsUTF8 != 0)) {
            if (QCOND2) {
#  ifdef SMALL_MEM
                char *temp_cfilnam = slide + (7 * (WSIZE>>3));

                zfstrcpy((char Far *)temp_cfilnam, G.pInfo->cfilname);
#   define  cFile_PrintBuf  temp_cfilnam
#  else
#   define  cFile_PrintBuf  G.pInfo->cfilname
#  endif
                Info(slide, 0x421, ((char *)slide,
                  LoadFarStringSmall2(GP11FlagsDiffer),
                  *pfilnum, FnFilter1(cFile_PrintBuf), G.pInfo->GPFIsUTF8));
#  undef    cFile_PrintBuf
            }
            if (error_in_archive < PK_WARN)
                error_in_archive = PK_WARN;
        }
# endif /* !SFX && UNICODE_SUPPORT */
        if ((error = do_string(__G__ G.lrec.filename_length, DS_FN_L)) !=
             PK_COOL)
        {
            if (error > error_in_archive)
                error_in_archive = error;
            if (error > PK_WARN) {
                Info(slide, 0x401, ((char *)slide, LoadFarString(FilNamMsg),
                  FnFilter1(G.filename), "local"));
                continue;   /* go on to next one */
            }
        }
        if (G.extra_field != (uch *)NULL) {
            free(G.extra_field);
            G.extra_field = (uch *)NULL;
        }
        if (G.unipath_filename) {
            free(G.unipath_filename);
            G.unipath_filename = NULL;
        }
# ifdef WIN32_WIDE
        if (G.unipath_widefilename) {
            free(G.unipath_widefilename);
            G.unipath_widefilename = NULL;
        }
# endif
        if ((error =
             do_string(__G__ G.lrec.extra_field_length, EXTRA_FIELD)) != 0)
        {
            if (error > error_in_archive)
                error_in_archive = error;
            if (error > PK_WARN) {
                Info(slide, 0x401, ((char *)slide,
                  LoadFarString(ExtFieldMsg),
                  FnFilter1(G.filename), "local"));
                continue;   /* go on */
            }
        }
# ifdef WIN32_WIDE
        if (G.unipath_widefilename == NULL) {
            if (G.unipath_filename) {
                /* Get wide path from UTF-8 */
                G.unipath_widefilename = utf8_to_wchar_string(G.unipath_filename);
            }
            else {
                G.unipath_widefilename = utf8_to_wchar_string(G.filename);
            }
            if (G.pInfo->lcflag) {      /* replace with lowercase filename */
                wcslwr(G.unipath_widefilename);
            }
#  if 0
            if (G.pInfo->vollabel && length > 8 && G.unipath_widefilename[8] == '.') {
                wchar_t *p = G.unipath_widefilename+8;
                while (*p++)
                    p[-1] = *p;  /* disk label, and 8th char is dot:  remove dot */
            }
#  endif
        }
# endif /* WIN32_WIDE */

# ifndef SFX
        if (G.pInfo->cfilname != (char Far *)NULL) {
            if (zfstrcmp(G.pInfo->cfilname, G.filename) != 0) {
#  ifdef SMALL_MEM
                char *temp_cfilnam = slide + (7 * (WSIZE>>3));

                zfstrcpy((char Far *)temp_cfilnam, G.pInfo->cfilname);
#   define  cFile_PrintBuf  temp_cfilnam
#  else
#   define  cFile_PrintBuf  G.pInfo->cfilname
#  endif
                Info(slide, 0x401, ((char *)slide,
                  LoadFarStringSmall2(LvsCFNamMsg),
                  FnFilter2(cFile_PrintBuf), FnFilter1(G.filename)));
#  undef    cFile_PrintBuf
                zfstrcpy(G.filename, G.pInfo->cfilname);
                if (error_in_archive < PK_WARN)
                    error_in_archive = PK_WARN;
            }
            zffree(G.pInfo->cfilname);
            G.pInfo->cfilname = (char Far *)NULL;
        }
# endif /* !SFX */
        /* Size consistency checks must come after reading in the local extra
         * field, so that any Zip64 extension local e.f. block has already
         * been processed.
         */
        if (G.lrec.compression_method == STORED) {
            zusz_t csiz_decrypted = G.lrec.csize;

            if (G.pInfo->encrypted)
                csiz_decrypted -= 12;
            if (G.lrec.ucsize != csiz_decrypted) {
                Info(slide, 0x401, ((char *)slide,
                  LoadFarStringSmall2(WrnStorUCSizCSizDiff),
                  FnFilter1(G.filename),
                  FmZofft(G.lrec.ucsize, NULL, "u"),
                  FmZofft(csiz_decrypted, NULL, "u")));
                G.lrec.ucsize = csiz_decrypted;
                if (error_in_archive < PK_WARN)
                    error_in_archive = PK_WARN;
            }
        }

# if CRYPT
        if (G.pInfo->encrypted &&
            (error = decrypt(__G__ uO.pwdarg)) != PK_COOL) {
            if (error == PK_WARN) {
                if (!((uO.tflag && uO.qflag) || (!uO.tflag && !QCOND2)))
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarString(SkipIncorrectPasswd),
                      FnFilter1(G.filename)));
                ++(*pnum_bad_pwd);
            } else {  /* (error > PK_WARN) */
                if (error > error_in_archive)
                    error_in_archive = error;
                Info(slide, 0x401, ((char *)slide,
                  LoadFarString(SkipCannotGetPasswd),
                  FnFilter1(G.filename)));
            }
            continue;   /* go on to next file */
        }
# endif /* CRYPT */

        /*
         * just about to extract file:  if extracting to disk, check if
         * already exists, and if so, take appropriate action according to
         * fflag/uflag/overwrite_all/etc. (we couldn't do this in upper
         * loop because we don't store the possibly renamed filename[] in
         * info[])
         */
# ifdef DLL
        if (!uO.tflag && !uO.cflag && !G.redirect_data)
# else
        if (!uO.tflag && !uO.cflag)
# endif
        {
            renamed = FALSE;   /* user hasn't renamed output file yet */

startover:
            query = FALSE;
            skip_entry = SKIP_NO;
            /* for files from DOS FAT, check for use of backslash instead
             *  of slash as directory separator (bug in some zipper(s); so
             *  far, not a problem in HPFS, NTFS or VFAT systems)
             */
# ifndef SFX
            if (G.pInfo->hostnum == FS_FAT_ && !MBSCHR(G.filename, '/')) {
                char *p=G.filename;

                if (*p) do {
                    if (*p == '\\') {
                        if (!G.reported_backslash) {
                            Info(slide, 0x21, ((char *)slide,
                              LoadFarString(BackslashPathSep), G.zipfn));
                            G.reported_backslash = TRUE;
                            if (!error_in_archive)
                                error_in_archive = PK_WARN;
                        }
                        *p = '/';
                    }
                } while (*PREINCSTR(p));
            }
# endif /* !SFX */

            if (!renamed) {
               /* remove absolute path specs */
               if (G.filename[0] == '/') {
                   Info(slide, 0x401, ((char *)slide,
                        LoadFarString(AbsolutePathWarning),
                        FnFilter1(G.filename)));
                   if (!error_in_archive)
                       error_in_archive = PK_WARN;
                   do {
                       char *p = G.filename + 1;
                       do {
                           *(p-1) = *p;
                       } while (*p++ != '\0');
                   } while (G.filename[0] == '/');
               }
            }

            /* mapname can create dirs if not freshening or if renamed */
            error = mapnamew(__G__ renamed);

            if ((errcode = error & ~MPN_MASK) != PK_OK &&
                error_in_archive < errcode)
                error_in_archive = errcode;
            if ((errcode = error & MPN_MASK) > MPN_INF_TRUNC) {
                if (errcode == MPN_CREATED_DIR) {
# ifdef SET_DIR_ATTRIB
                  direntryw *d_entryw;

                  error = defer_dir_attribsw(__G__ &d_entryw);
                  if (d_entryw == (direntryw *)NULL) {
                      /* There may be no dir_attribs info available, or
                       * we have encountered a mem allocation error.
                       * In case of an error, report it and set program
                       * error state to warning level.
                       */
                      if (error) {
                          Info(slide, 0x401, ((char *)slide,
                               LoadFarString(DirlistEntryNoMem)));
                          if (!error_in_archive)
                              error_in_archive = PK_WARN;
                      }
                  } else {
                      d_entryw->next = (*pdirlistw);
                      (*pdirlistw) = d_entryw;
                      ++(*pnum_dirs);
                  }
# endif /* SET_DIR_ATTRIB */
                } else if (errcode == MPN_VOL_LABEL) {
# ifdef DOS_OS2_W32
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarString(SkipVolumeLabel),
                      FnFilter1(G.filename),
                      uO.volflag? "hard disk " : ""));
# else
                    Info(slide, 1, ((char *)slide,
                      LoadFarString(SkipVolumeLabel),
                      FnFilter1(G.filename), ""));
# endif
                } else if (errcode > MPN_INF_SKIP &&
                           error_in_archive < PK_ERR)
                    error_in_archive = PK_ERR;
                Trace((stderr, "mapname(%s) returns error code = %d\n",
                  FnFilter1(G.filename), error));
                continue;   /* go on to next file */
            }

            cfn = check_for_newerw(__G__ G.unipath_widefilename);

            switch (cfn) {
                case DOES_NOT_EXIST:
# ifdef NOVELL_BUG_FAILSAFE
                    G.dne = TRUE;   /* stat() says file DOES NOT EXIST */
# endif
                    /* freshen (no new files): skip unless just renamed */
                    if (uO.fflag && !renamed)
                        skip_entry = SKIP_Y_NONEXIST;
                    break;
                case EXISTS_AND_OLDER:
# ifdef UNIXBACKUP
                    if (!uO.B_flag)
# endif
                    {
                        if (IS_OVERWRT_NONE)
                            /* never overwrite:  skip file */
                            skip_entry = SKIP_Y_EXISTING;
                        else if (!IS_OVERWRT_ALL)
                            query = TRUE;
                    }
                    break;
                case EXISTS_AND_NEWER:             /* (or equal) */
# ifdef UNIXBACKUP
                    if ((!uO.B_flag && IS_OVERWRT_NONE) ||
# else
                    if (IS_OVERWRT_NONE ||
# endif
                        (uO.uflag && !renamed)) {
                        /* skip if update/freshen & orig name */
                        skip_entry = SKIP_Y_EXISTING;
                    } else {
# ifdef UNIXBACKUP
                        if (!IS_OVERWRT_ALL && !uO.B_flag)
# else
                        if (!IS_OVERWRT_ALL)
# endif
                            query = TRUE;
                    }
                    break;
            }
            if (query) {
# ifdef WINDLL
                switch (G.lpUserFunctions->replace != NULL ?
                        (*G.lpUserFunctions->replace)(G.filename, FILNAMSIZ) :
                        IDM_REPLACE_NONE) {
                    case IDM_REPLACE_RENAME:
                        _ISO_INTERN(G.filename);
                        renamed = TRUE;
                        goto startover;
                    case IDM_REPLACE_ALL:
                        G.overwrite_mode = OVERWRT_ALWAYS;
                        /* FALL THROUGH, extract */
                    case IDM_REPLACE_YES:
                        break;
                    case IDM_REPLACE_NONE:
                        G.overwrite_mode = OVERWRT_NEVER;
                        /* FALL THROUGH, skip */
                    case IDM_REPLACE_NO:
                        skip_entry = SKIP_Y_EXISTING;
                        break;
                }
# else /* !WINDLL */
                extent fnlen;
reprompt:
                Info(slide, 0x81, ((char *)slide,
                  LoadFarString(ReplaceQuery),
                  FnFilter1(G.filename)));
                if (fgets(G.answerbuf, sizeof(G.answerbuf), stdin)
                    == (char *)NULL) {
                    Info(slide, 1, ((char *)slide,
                      LoadFarString(AssumeNone)));
                    *G.answerbuf = 'N';
                    if (!error_in_archive)
                        error_in_archive = 1;  /* not extracted:  warning */
                }
                switch (*G.answerbuf) {
                    case 'r':
                    case 'R':
                        do {
                            Info(slide, 0x81, ((char *)slide,
                              LoadFarString(NewNameQuery)));
                            fgets(G.filename, FILNAMSIZ, stdin);
                            /* usually get \n here:  better check for it */
                            fnlen = strlen(G.filename);
                            if (lastchar(G.filename, fnlen) == '\n')
                                G.filename[--fnlen] = '\0';
                        } while (fnlen == 0);
#  ifdef WIN32  /* WIN32 fgets( ... , stdin) returns OEM coded strings */
                        _OEM_INTERN(G.filename);
#  endif
                        renamed = TRUE;
                        goto startover;   /* sorry for a goto */
                    case 'A':   /* dangerous option:  force caps */
                        G.overwrite_mode = OVERWRT_ALWAYS;
                        /* FALL THROUGH, extract */
                    case 'y':
                    case 'Y':
                        break;
                    case 'N':
                        G.overwrite_mode = OVERWRT_NEVER;
                        /* FALL THROUGH, skip */
                    case 'n':
                        /* skip file */
                        skip_entry = SKIP_Y_EXISTING;
                        break;
                    case '\n':
                    case '\r':
                        /* Improve echo of '\n' and/or '\r'
                           (sizeof(G.answerbuf) == 10 (see globals.h), so
                           there is enough space for the provided text...) */
                        strcpy(G.answerbuf, "{ENTER}");
                        /* fall through ... */
                    default:
                        /* usually get \n here:  remove it for nice display
                           (fnlen can be re-used here, we are outside the
                           "enter new filename" loop) */
                        fnlen = strlen(G.answerbuf);
                        if (lastchar(G.answerbuf, fnlen) == '\n')
                            G.answerbuf[--fnlen] = '\0';
                        Info(slide, 1, ((char *)slide,
                          LoadFarString(InvalidResponse), G.answerbuf));
                        goto reprompt;   /* yet another goto? */
                } /* end switch (*answerbuf) */
# endif /* ?WINDLL */
            } /* end if (query) */
            if (skip_entry != SKIP_NO) {
# ifdef WINDLL
                if (skip_entry == SKIP_Y_EXISTING) {
                    /* report skipping of an existing entry */
                    Info(slide, 0, ((char *)slide,
                      ((IS_OVERWRT_NONE || !uO.uflag || renamed) ?
                       "Target file exists.  Skipping %s\n" :
                       "Target file newer.  Skipping %s\n"),
                      FnFilter1(G.filename)));
                }
# endif /* WINDLL */
                continue;
            }
        } /* end if (extracting to disk) */

# ifdef DLL
        if ((G.statreportcb != NULL) &&
            (*G.statreportcb)(__G__ UZ_ST_START_EXTRACT, G.zipfn,
                              G.filename, NULL)) {
            return IZ_CTRLC;        /* cancel operation by user request */
        }
# endif
        G.disk_full = 0;
        if ((error = extract_or_test_member(__G)) != PK_COOL) {
            if (error > error_in_archive)
                error_in_archive = error;       /* ...and keep going */
# ifdef DLL
            if (G.disk_full > 1 || error_in_archive == IZ_CTRLC) {
# else
            if (G.disk_full > 1) {
# endif
                return error_in_archive;        /* (unless disk full) */
            }
        }
# ifdef DLL
        if ((G.statreportcb != NULL) &&
            (*G.statreportcb)(__G__ UZ_ST_FINISH_MEMBER, G.zipfn,
                              G.filename, (zvoid *)&G.lrec.ucsize)) {
            return IZ_CTRLC;        /* cancel operation by user request */
        }
# endif
    } /* end for-loop (i:  files in current block) */

    return error_in_archive;

} /* end function extract_or_test_entrylistw() */

#endif /* defined(UNICODE_SUPPORT) && defined(WIN32_WIDE) */




/* wsize is used in extract_or_test_member() and UZbunzip2() */
#if (defined(DLL) && !defined(NO_SLIDE_REDIR))
#  define wsize G._wsize    /* wsize is a variable */
#else
#  define wsize WSIZE       /* wsize is a constant */
#endif

/***************************************/
/*  Function extract_or_test_member()  */
/***************************************/

static int extract_or_test_member(__G)    /* return PK-type error code */
     __GDEF
{
    char *nul="[empty] ", *txt="[text]  ", *bin="[binary]";
#ifdef CMS_MVS
    char *ebc="[ebcdic]";
#endif
    register int b;
    int r, error=PK_COOL;


/*---------------------------------------------------------------------------
    Initialize variables, buffers, etc.
  ---------------------------------------------------------------------------*/

    G.bits_left = 0;
    G.bitbuf = 0L;       /* unreduce and unshrink only */
    G.zipeof = 0;
    G.newfile = TRUE;
    G.crc32val = CRCVAL_INITIAL;

#ifdef SYMLINKS
    /* If file is a (POSIX-compatible) symbolic link and we are extracting
     * to disk, prepare to restore the link. */
    G.symlnk = (G.pInfo->symlink &&
                !uO.tflag && !uO.cflag && (G.lrec.ucsize > 0));
#endif /* SYMLINKS */

    if (uO.tflag) {
        if (!uO.qflag)
            Info(slide, 0, ((char *)slide, LoadFarString(ExtractMsg), "test",
              FnFilter1(G.filename), "", ""));
    } else {
#ifdef DLL
        if (uO.cflag && !G.redirect_data)
#else
        if (uO.cflag)
#endif
        {
#if (defined(OS2) && defined(__IBMC__) && (__IBMC__ >= 200))
            G.outfile = freopen("", "wb", stdout);   /* VAC++ ignores setmode */
#else
            G.outfile = stdout;
#endif
#ifdef DOS_FLX_NLM_OS2_W32
#if (defined(__HIGHC__) && !defined(FLEXOS))
            setmode(G.outfile, _BINARY);
#else /* !(defined(__HIGHC__) && !defined(FLEXOS)) */
            setmode(fileno(G.outfile), O_BINARY);
#endif /* ?(defined(__HIGHC__) && !defined(FLEXOS)) */
#           define NEWLINE "\r\n"
#else /* !DOS_FLX_NLM_OS2_W32 */
#           define NEWLINE "\n"
#endif /* ?DOS_FLX_NLM_OS2_W32 */
#ifdef VMS
            /* VMS:  required even for stdout! */
            if ((r = open_outfile(__G)) != 0)
                switch (r) {
                  case OPENOUT_SKIPOK:
                    return PK_OK;
                  case OPENOUT_SKIPWARN:
                    return PK_WARN;
                  default:
                    return PK_DISK;
                }
        } else if ((r = open_outfile(__G)) != 0)
            switch (r) {
              case OPENOUT_SKIPOK:
                return PK_OK;
              case OPENOUT_SKIPWARN:
                return PK_WARN;
              default:
                return PK_DISK;
            }
#else /* !VMS */
        } else if (open_outfile(__G))
            return PK_DISK;
#endif /* ?VMS */
    }

/*---------------------------------------------------------------------------
    Unpack the file.
  ---------------------------------------------------------------------------*/

    defer_leftover_input(__G);    /* so NEXTBYTE bounds check will work */
    switch (G.lrec.compression_method) {
        case STORED:
            if (!uO.tflag && QCOND2) {
#ifdef SYMLINKS
                if (G.symlnk)   /* can also be deflated, but rarer... */
                    Info(slide, 0, ((char *)slide, LoadFarString(ExtractMsg),
                      "link", FnFilter1(G.filename), "", ""));
                else
#endif /* SYMLINKS */
                Info(slide, 0, ((char *)slide, LoadFarString(ExtractMsg),
                  "extract", FnFilter1(G.filename),
                  (uO.aflag != 1 /* && G.pInfo->textfile==G.pInfo->textmode */)?
                  "" : (G.lrec.ucsize == 0L? nul : (G.pInfo->textfile? txt :
                  bin)), uO.cflag? NEWLINE : ""));
            }
#if (defined(DLL) && !defined(NO_SLIDE_REDIR))
            if (G.redirect_slide) {
                wsize = G.redirect_size; redirSlide = G.redirect_buffer;
            } else {
                wsize = WSIZE; redirSlide = slide;
            }
#endif
            G.outptr = redirSlide;
            G.outcnt = 0L;
            while ((b = NEXTBYTE) != EOF) {
                *G.outptr++ = (uch)b;
                if (++G.outcnt == wsize) {
                    error = flush(__G__ redirSlide, G.outcnt, 0);
                    G.outptr = redirSlide;
                    G.outcnt = 0L;
                    if (error != PK_COOL || G.disk_full) break;
                }
            }
            if (G.outcnt) {        /* flush final (partial) buffer */
                r = flush(__G__ redirSlide, G.outcnt, 0);
                if (error < r) error = r;
            }
            break;

#ifndef SFX
#ifndef LZW_CLEAN
        case SHRUNK:
            if (!uO.tflag && QCOND2) {
                Info(slide, 0, ((char *)slide, LoadFarString(ExtractMsg),
                  LoadFarStringSmall(Unshrink), FnFilter1(G.filename),
                  (uO.aflag != 1 /* && G.pInfo->textfile==G.pInfo->textmode */)?
                  "" : (G.pInfo->textfile? txt : bin), uO.cflag? NEWLINE : ""));
            }
            if ((r = unshrink(__G)) != PK_COOL) {
                if (r < PK_DISK) {
                    if ((uO.tflag && uO.qflag) || (!uO.tflag && !QCOND2))
                        Info(slide, 0x401, ((char *)slide,
                          LoadFarStringSmall(ErrUnzipFile), r == PK_MEM3 ?
                          LoadFarString(NotEnoughMem) :
                          LoadFarString(InvalidComprData),
                          LoadFarStringSmall2(Unshrink),
                          FnFilter1(G.filename)));
                    else
                        Info(slide, 0x401, ((char *)slide,
                          LoadFarStringSmall(ErrUnzipNoFile), r == PK_MEM3 ?
                          LoadFarString(NotEnoughMem) :
                          LoadFarString(InvalidComprData),
                          LoadFarStringSmall2(Unshrink)));
                }
                error = r;
            }
            break;
#endif /* !LZW_CLEAN */

#ifndef COPYRIGHT_CLEAN
        case REDUCED1:
        case REDUCED2:
        case REDUCED3:
        case REDUCED4:
            if (!uO.tflag && QCOND2) {
                Info(slide, 0, ((char *)slide, LoadFarString(ExtractMsg),
                  "unreduc", FnFilter1(G.filename),
                  (uO.aflag != 1 /* && G.pInfo->textfile==G.pInfo->textmode */)?
                  "" : (G.pInfo->textfile? txt : bin), uO.cflag? NEWLINE : ""));
            }
            if ((r = unreduce(__G)) != PK_COOL) {
                /* unreduce() returns only PK_COOL, PK_DISK, or IZ_CTRLC */
                error = r;
            }
            break;
#endif /* !COPYRIGHT_CLEAN */

        case IMPLODED:
            if (!uO.tflag && QCOND2) {
                Info(slide, 0, ((char *)slide, LoadFarString(ExtractMsg),
                  "explod", FnFilter1(G.filename),
                  (uO.aflag != 1 /* && G.pInfo->textfile==G.pInfo->textmode */)?
                  "" : (G.pInfo->textfile? txt : bin), uO.cflag? NEWLINE : ""));
            }
            if ((r = explode(__G)) != 0) {
                if (r == 5) { /* treat 5 specially */
                    int warning = ((zusz_t)G.used_csize <= G.lrec.csize);

                    if ((uO.tflag && uO.qflag) || (!uO.tflag && !QCOND2))
                        Info(slide, 0x401, ((char *)slide,
                          LoadFarString(LengthMsg),
                          "", warning ? "warning" : "error",
                          FmZofft(G.used_csize, NULL, NULL),
                          FmZofft(G.lrec.ucsize, NULL, "u"),
                          warning ? "  " : "",
                          FmZofft(G.lrec.csize, NULL, "u"),
                          " [", FnFilter1(G.filename), "]"));
                    else
                        Info(slide, 0x401, ((char *)slide,
                          LoadFarString(LengthMsg),
                          "\n", warning ? "warning" : "error",
                          FmZofft(G.used_csize, NULL, NULL),
                          FmZofft(G.lrec.ucsize, NULL, "u"),
                          warning ? "  " : "",
                          FmZofft(G.lrec.csize, NULL, "u"),
                          "", "", "."));
                    error = warning ? PK_WARN : PK_ERR;
                } else if (r < PK_DISK) {
                    if ((uO.tflag && uO.qflag) || (!uO.tflag && !QCOND2))
                        Info(slide, 0x401, ((char *)slide,
                          LoadFarStringSmall(ErrUnzipFile), r == 3?
                          LoadFarString(NotEnoughMem) :
                          LoadFarString(InvalidComprData),
                          LoadFarStringSmall2(Explode),
                          FnFilter1(G.filename)));
                    else
                        Info(slide, 0x401, ((char *)slide,
                          LoadFarStringSmall(ErrUnzipNoFile), r == 3?
                          LoadFarString(NotEnoughMem) :
                          LoadFarString(InvalidComprData),
                          LoadFarStringSmall2(Explode)));
                    error = ((r == 3) ? PK_MEM3 : PK_ERR);
                } else {
                    error = r;
                }
            }
            break;
#endif /* !SFX */

        case DEFLATED:
#ifdef USE_DEFLATE64
        case ENHDEFLATED:
#endif
            if (!uO.tflag && QCOND2) {
                Info(slide, 0, ((char *)slide, LoadFarString(ExtractMsg),
                  "inflat", FnFilter1(G.filename),
                  (uO.aflag != 1 /* && G.pInfo->textfile==G.pInfo->textmode */)?
                  "" : (G.pInfo->textfile? txt : bin), uO.cflag? NEWLINE : ""));
            }
#ifndef USE_ZLIB  /* zlib's function is called inflate(), too */
#  define UZinflate inflate
#endif
            if ((r = UZinflate(__G__
                               (G.lrec.compression_method == ENHDEFLATED)))
                != 0) {
                if (r < PK_DISK) {
                    if ((uO.tflag && uO.qflag) || (!uO.tflag && !QCOND2))
                        Info(slide, 0x401, ((char *)slide,
                          LoadFarStringSmall(ErrUnzipFile), r == 3?
                          LoadFarString(NotEnoughMem) :
                          LoadFarString(InvalidComprData),
                          LoadFarStringSmall2(Inflate),
                          FnFilter1(G.filename)));
                    else
                        Info(slide, 0x401, ((char *)slide,
                          LoadFarStringSmall(ErrUnzipNoFile), r == 3?
                          LoadFarString(NotEnoughMem) :
                          LoadFarString(InvalidComprData),
                          LoadFarStringSmall2(Inflate)));
                    error = ((r == 3) ? PK_MEM3 : PK_ERR);
                } else {
                    error = r;
                }
            }
            break;

#ifdef USE_BZIP2
        case BZIPPED:
            if (!uO.tflag && QCOND2) {
                Info(slide, 0, ((char *)slide, LoadFarString(ExtractMsg),
                  "bunzipp", FnFilter1(G.filename),
                  (uO.aflag != 1 /* && G.pInfo->textfile==G.pInfo->textmode */)?
                  "" : (G.pInfo->textfile? txt : bin), uO.cflag? NEWLINE : ""));
            }
            if ((r = UZbunzip2(__G)) != 0) {
                if (r < PK_DISK) {
                    if ((uO.tflag && uO.qflag) || (!uO.tflag && !QCOND2))
                        Info(slide, 0x401, ((char *)slide,
                          LoadFarStringSmall(ErrUnzipFile), r == 3?
                          LoadFarString(NotEnoughMem) :
                          LoadFarString(InvalidComprData),
                          LoadFarStringSmall2(BUnzip),
                          FnFilter1(G.filename)));
                    else
                        Info(slide, 0x401, ((char *)slide,
                          LoadFarStringSmall(ErrUnzipNoFile), r == 3?
                          LoadFarString(NotEnoughMem) :
                          LoadFarString(InvalidComprData),
                          LoadFarStringSmall2(BUnzip)));
                    error = ((r == 3) ? PK_MEM3 : PK_ERR);
                } else {
                    error = r;
                }
            }
            break;
#endif /* USE_BZIP2 */

        default:   /* should never get to this point */
            Info(slide, 0x401, ((char *)slide,
              LoadFarString(FileUnknownCompMethod), FnFilter1(G.filename)));
            /* close and delete file before return? */
            undefer_input(__G);
            return PK_WARN;

    } /* end switch (compression method) */

/*---------------------------------------------------------------------------
    Close the file and set its date and time (not necessarily in that order),
    and make sure the CRC checked out OK.  Logical-AND the CRC for 64-bit
    machines (redundant on 32-bit machines).
  ---------------------------------------------------------------------------*/

#ifdef VMS                  /* VMS:  required even for stdout! (final flush) */
    if (!uO.tflag)           /* don't close NULL file */
        close_outfile(__G);
#else
#ifdef DLL
    if (!uO.tflag && (!uO.cflag || G.redirect_data)) {
        if (G.redirect_data)
            FINISH_REDIRECT();
        else
            close_outfile(__G);
    }
#else
    if (!uO.tflag && !uO.cflag)   /* don't close NULL file or stdout */
        close_outfile(__G);
#endif
#endif /* VMS */

            /* GRR: CONVERT close_outfile() TO NON-VOID:  CHECK FOR ERRORS! */


    if (G.disk_full) {            /* set by flush() */
        if (G.disk_full > 1) {
#if (defined(DELETE_IF_FULL) && defined(HAVE_UNLINK))
            /* delete the incomplete file if we can */
            if (unlink(G.filename) != 0)
                Trace((stderr, "extract.c:  could not delete %s\n",
                  FnFilter1(G.filename)));
#else
            /* warn user about the incomplete file */
            Info(slide, 0x421, ((char *)slide, LoadFarString(FileTruncated),
              FnFilter1(G.filename)));
#endif
            error = PK_DISK;
        } else {
            error = PK_WARN;
        }
    }

    if (error > PK_WARN) {/* don't print redundant CRC error if error already */
        undefer_input(__G);
        return error;
    }
    if (G.crc32val != G.lrec.crc32) {
        /* if quiet enough, we haven't output the filename yet:  do it */
        if ((uO.tflag && uO.qflag) || (!uO.tflag && !QCOND2))
            Info(slide, 0x401, ((char *)slide, "%-22s ",
              FnFilter1(G.filename)));
        Info(slide, 0x401, ((char *)slide, LoadFarString(BadCRC), G.crc32val,
          G.lrec.crc32));
#if CRYPT
        if (G.pInfo->encrypted)
            Info(slide, 0x401, ((char *)slide, LoadFarString(MaybeBadPasswd)));
#endif
        error = PK_ERR;
    } else if (uO.tflag) {
#ifndef SFX
        if (G.extra_field) {
            if ((r = TestExtraField(__G__ G.extra_field,
                                    G.lrec.extra_field_length)) > error)
                error = r;
        } else
#endif /* !SFX */
        if (!uO.qflag)
            Info(slide, 0, ((char *)slide, " OK\n"));
    } else {
        if (QCOND2 && !error)   /* GRR:  is stdout reset to text mode yet? */
            Info(slide, 0, ((char *)slide, "\n"));
    }

    undefer_input(__G);
    return error;

} /* end function extract_or_test_member() */





#ifndef SFX

/*******************************/
/*  Function TestExtraField()  */
/*******************************/

static int TestExtraField(__G__ ef, ef_len)
    __GDEF
    uch *ef;
    unsigned ef_len;
{
    ush ebID;
    unsigned ebLen;
    unsigned eb_cmpr_offs = 0;
    int r;

    /* we know the regular compressed file data tested out OK, or else we
     * wouldn't be here ==> print filename if any extra-field errors found
     */
    while (ef_len >= EB_HEADSIZE) {
        ebID = makeword(ef);
        ebLen = (unsigned)makeword(ef+EB_LEN);

        if (ebLen > (ef_len - EB_HEADSIZE)) {
           /* Discovered some extra field inconsistency! */
            if (uO.qflag)
                Info(slide, 1, ((char *)slide, "%-22s ",
                  FnFilter1(G.filename)));
            Info(slide, 1, ((char *)slide, LoadFarString(InconsistEFlength),
              ebLen, (ef_len - EB_HEADSIZE)));
            return PK_ERR;
        }

        switch (ebID) {
            case EF_OS2:
            case EF_ACL:
            case EF_MAC3:
            case EF_BEOS:
            case EF_ATHEOS:
                switch (ebID) {
                  case EF_OS2:
                  case EF_ACL:
                    eb_cmpr_offs = EB_OS2_HLEN;
                    break;
                  case EF_MAC3:
                    if (ebLen >= EB_MAC3_HLEN &&
                        (makeword(ef+(EB_HEADSIZE+EB_FLGS_OFFS))
                         & EB_M3_FL_UNCMPR) &&
                        (makelong(ef+EB_HEADSIZE) == ebLen - EB_MAC3_HLEN))
                        eb_cmpr_offs = 0;
                    else
                        eb_cmpr_offs = EB_MAC3_HLEN;
                    break;
                  case EF_BEOS:
                  case EF_ATHEOS:
                    if (ebLen >= EB_BEOS_HLEN &&
                        (*(ef+(EB_HEADSIZE+EB_FLGS_OFFS)) & EB_BE_FL_UNCMPR) &&
                        (makelong(ef+EB_HEADSIZE) == ebLen - EB_BEOS_HLEN))
                        eb_cmpr_offs = 0;
                    else
                        eb_cmpr_offs = EB_BEOS_HLEN;
                    break;
                }
                if ((r = test_compr_eb(__G__ ef, ebLen, eb_cmpr_offs, NULL))
                    != PK_OK) {
                    if (uO.qflag)
                        Info(slide, 1, ((char *)slide, "%-22s ",
                          FnFilter1(G.filename)));
                    switch (r) {
                        case IZ_EF_TRUNC:
                            Info(slide, 1, ((char *)slide,
                              LoadFarString(TruncEAs),
                              ebLen-(eb_cmpr_offs+EB_CMPRHEADLEN), "\n"));
                            break;
                        case PK_ERR:
                            Info(slide, 1, ((char *)slide,
                              LoadFarString(InvalidComprDataEAs)));
                            break;
                        case PK_MEM3:
                        case PK_MEM4:
                            Info(slide, 1, ((char *)slide,
                              LoadFarString(NotEnoughMemEAs)));
                            break;
                        default:
                            if ((r & 0xff) != PK_ERR)
                                Info(slide, 1, ((char *)slide,
                                  LoadFarString(UnknErrorEAs)));
                            else {
                                ush m = (ush)(r >> 8);
                                if (m == DEFLATED)            /* GRR KLUDGE! */
                                    Info(slide, 1, ((char *)slide,
                                      LoadFarString(BadCRC_EAs)));
                                else
                                    Info(slide, 1, ((char *)slide,
                                      LoadFarString(UnknComprMethodEAs), m));
                            }
                            break;
                    }
                    return r;
                }
                break;

            case EF_NTSD:
                Trace((stderr, "ebID: %i / ebLen: %u\n", ebID, ebLen));
                r = ebLen < EB_NTSD_L_LEN ? IZ_EF_TRUNC :
                    ((ef[EB_HEADSIZE+EB_NTSD_VERSION] > EB_NTSD_MAX_VER) ?
                     (PK_WARN | 0x4000) :
                     test_compr_eb(__G__ ef, ebLen, EB_NTSD_L_LEN, TEST_NTSD));
                if (r != PK_OK) {
                    if (uO.qflag)
                        Info(slide, 1, ((char *)slide, "%-22s ",
                          FnFilter1(G.filename)));
                    switch (r) {
                        case IZ_EF_TRUNC:
                            Info(slide, 1, ((char *)slide,
                              LoadFarString(TruncNTSD),
                              ebLen-(EB_NTSD_L_LEN+EB_CMPRHEADLEN), "\n"));
                            break;
#if (defined(WIN32) && defined(NTSD_EAS))
                        case PK_WARN:
                            Info(slide, 1, ((char *)slide,
                              LoadFarString(InvalidSecurityEAs)));
                            break;
#endif
                        case PK_ERR:
                            Info(slide, 1, ((char *)slide,
                              LoadFarString(InvalidComprDataEAs)));
                            break;
                        case PK_MEM3:
                        case PK_MEM4:
                            Info(slide, 1, ((char *)slide,
                              LoadFarString(NotEnoughMemEAs)));
                            break;
                        case (PK_WARN | 0x4000):
                            Info(slide, 1, ((char *)slide,
                              LoadFarString(UnsuppNTSDVersEAs),
                              (int)ef[EB_HEADSIZE+EB_NTSD_VERSION]));
                            r = PK_WARN;
                            break;
                        default:
                            if ((r & 0xff) != PK_ERR)
                                Info(slide, 1, ((char *)slide,
                                  LoadFarString(UnknErrorEAs)));
                            else {
                                ush m = (ush)(r >> 8);
                                if (m == DEFLATED)            /* GRR KLUDGE! */
                                    Info(slide, 1, ((char *)slide,
                                      LoadFarString(BadCRC_EAs)));
                                else
                                    Info(slide, 1, ((char *)slide,
                                      LoadFarString(UnknComprMethodEAs), m));
                            }
                            break;
                    }
                    return r;
                }
                break;
            case EF_PKVMS:
                if (makelong(ef+EB_HEADSIZE) !=
                    crc32(CRCVAL_INITIAL, ef+(EB_HEADSIZE+4),
                          (extent)(ebLen-4)))
                    Info(slide, 1, ((char *)slide,
                      LoadFarString(BadCRC_EAs)));
                break;
            case EF_PKW32:
            case EF_PKUNIX:
            case EF_ASIUNIX:
            case EF_IZVMS:
            case EF_IZUNIX:
            case EF_VMCMS:
            case EF_MVS:
            case EF_SPARK:
            case EF_TANDEM:
            case EF_THEOS:
            case EF_AV:
            default:
                break;
        }
        ef_len -= (ebLen + EB_HEADSIZE);
        ef += (ebLen + EB_HEADSIZE);
    }

    if (!uO.qflag)
        Info(slide, 0, ((char *)slide, " OK\n"));

    return PK_COOL;

} /* end function TestExtraField() */





/******************************/
/*  Function test_compr_eb()  */
/******************************/

#ifdef PROTO
static int test_compr_eb(
    __GPRO__
    uch *eb,
    unsigned eb_size,
    unsigned compr_offset,
    int (*test_uc_ebdata)(__GPRO__ uch *eb, unsigned eb_size,
                          uch *eb_ucptr, ulg eb_ucsize))
#else /* !PROTO */
static int test_compr_eb(__G__ eb, eb_size, compr_offset, test_uc_ebdata)
    __GDEF
    uch *eb;
    unsigned eb_size;
    unsigned compr_offset;
    int (*test_uc_ebdata)();
#endif /* ?PROTO */
{
    ulg eb_ucsize;
    uch *eb_ucptr;
    int r;

    if (compr_offset < 4)                /* field is not compressed: */
        return PK_OK;                    /* do nothing and signal OK */

    if ((eb_size < (EB_UCSIZE_P + 4)) ||
        ((eb_ucsize = makelong(eb+(EB_HEADSIZE+EB_UCSIZE_P))) > 0L &&
         eb_size <= (compr_offset + EB_CMPRHEADLEN)))
        return IZ_EF_TRUNC;               /* no compressed data! */

    if (
#ifdef INT_16BIT
        (((ulg)(extent)eb_ucsize) != eb_ucsize) ||
#endif
        (eb_ucptr = (uch *)malloc((extent)eb_ucsize)) == (uch *)NULL)
        return PK_MEM4;

    r = memextract(__G__ eb_ucptr, eb_ucsize,
                   eb + (EB_HEADSIZE + compr_offset),
                   (ulg)(eb_size - compr_offset));

    if (r == PK_OK && test_uc_ebdata != NULL)
        r = (*test_uc_ebdata)(__G__ eb, eb_size, eb_ucptr, eb_ucsize);

    free(eb_ucptr);
    return r;

} /* end function test_compr_eb() */

#endif /* !SFX */





/***************************/
/*  Function memextract()  */
/***************************/

int memextract(__G__ tgt, tgtsize, src, srcsize)  /* extract compressed */
    __GDEF                                        /*  extra field block; */
    uch *tgt;                                     /*  return PK-type error */
    ulg tgtsize;                                  /*  level */
    ZCONST uch *src;
    ulg srcsize;
{
    zoff_t old_csize=G.csize;
    uch   *old_inptr=G.inptr;
    int    old_incnt=G.incnt;
    int    r, error=PK_OK;
    ush    method;
    ulg    extra_field_crc;


    method = makeword(src);
    extra_field_crc = makelong(src+2);

    /* compressed extra field exists completely in memory at this location: */
    G.inptr = (uch *)src + (2 + 4);     /* method and extra_field_crc */
    G.incnt = (int)(G.csize = (long)(srcsize - (2 + 4)));
    G.mem_mode = TRUE;
    G.outbufptr = tgt;
    G.outsize = tgtsize;

    switch (method) {
        case STORED:
            memcpy((char *)tgt, (char *)G.inptr, (extent)G.incnt);
            G.outcnt = (ulg)G.csize;    /* for CRC calculation */
            break;
        case DEFLATED:
#ifdef USE_DEFLATE64
        case ENHDEFLATED:
#endif
            G.outcnt = 0L;
            if ((r = UZinflate(__G__ (method == ENHDEFLATED))) != 0) {
                if (!uO.tflag)
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarStringSmall(ErrUnzipNoFile), r == 3?
                      LoadFarString(NotEnoughMem) :
                      LoadFarString(InvalidComprData),
                      LoadFarStringSmall2(Inflate)));
                error = (r == 3)? PK_MEM3 : PK_ERR;
            }
            if (G.outcnt == 0L)   /* inflate's final FLUSH sets outcnt */
                break;
            break;
        default:
            if (uO.tflag)
                error = PK_ERR | ((int)method << 8);
            else {
                Info(slide, 0x401, ((char *)slide,
                  LoadFarString(UnsupportedExtraField), method));
                error = PK_ERR;  /* GRR:  should be passed on up via SetEAs() */
            }
            break;
    }

    G.inptr = old_inptr;
    G.incnt = old_incnt;
    G.csize = old_csize;
    G.mem_mode = FALSE;

    if (!error) {
        register ulg crcval = crc32(CRCVAL_INITIAL, tgt, (extent)G.outcnt);

        if (crcval != extra_field_crc) {
            if (uO.tflag)
                error = PK_ERR | (DEFLATED << 8);  /* kludge for now */
            else {
                Info(slide, 0x401, ((char *)slide,
                  LoadFarString(BadExtraFieldCRC), G.zipfn, crcval,
                  extra_field_crc));
                error = PK_ERR;
            }
        }
    }
    return error;

} /* end function memextract() */





/*************************/
/*  Function memflush()  */
/*************************/

int memflush(__G__ rawbuf, size)
    __GDEF
    ZCONST uch *rawbuf;
    ulg size;
{
    if (size > G.outsize)
        /* Here, PK_DISK is a bit off-topic, but in the sense of marking
           "overflow of output space", its use may be tolerated. */
        return PK_DISK;   /* more data than output buffer can hold */



    memcpy((char *)G.outbufptr, (char *)rawbuf, (extent)size);
    G.outbufptr += (unsigned int)size;
    G.outsize -= size;
    G.outcnt += size;

    return 0;

} /* end function memflush() */





#if (defined(VMS) || defined(VMS_TEXT_CONV))

/************************************/
/*  Function extract_izvms_block()  */
/************************************/

/*
 * Extracts block from p. If resulting length is less than needed, fill
 * extra space with corresponding bytes from 'init'.
 * Currently understands 3 formats of block compression:
 * - Simple storing
 * - Compression of zero bytes to zero bits
 * - Deflation (see memextract())
 * The IZVMS block data is returned in malloc'd space.
 */
uch *extract_izvms_block(__G__ ebdata, size, retlen, init, needlen)
    __GDEF
    ZCONST uch *ebdata;
    unsigned size;
    unsigned *retlen;
    ZCONST uch *init;
    unsigned needlen;
{
    uch *ucdata;       /* Pointer to block allocated */
    int cmptype;
    unsigned usiz, csiz;

    cmptype = (makeword(ebdata+EB_IZVMS_FLGS) & EB_IZVMS_BCMASK);
    csiz = size - EB_IZVMS_HLEN;
    usiz = (cmptype == EB_IZVMS_BCSTOR ?
            csiz : makeword(ebdata+EB_IZVMS_UCSIZ));

    if (retlen)
        *retlen = usiz;

    if ((ucdata = (uch *)malloc(MAX(needlen, usiz))) == NULL)
        return NULL;

    if (init && (usiz < needlen))
        memcpy((char *)ucdata, (ZCONST char *)init, needlen);

    switch (cmptype)
    {
        case EB_IZVMS_BCSTOR: /* The simplest case */
            memcpy(ucdata, ebdata+EB_IZVMS_HLEN, usiz);
            break;
        case EB_IZVMS_BC00:
            decompress_bits(ucdata, usiz, ebdata+EB_IZVMS_HLEN);
            break;
        case EB_IZVMS_BCDEFL:
            memextract(__G__ ucdata, (ulg)usiz,
                       ebdata+EB_IZVMS_HLEN, (ulg)csiz);
            break;
        default:
            free(ucdata);
            ucdata = NULL;
    }
    return ucdata;

} /* end of extract_izvms_block */





/********************************/
/*  Function decompress_bits()  */
/********************************/
/*
 *  Simple uncompression routine. The compression uses bit stream.
 *  Compression scheme:
 *
 *  if (byte!=0)
 *      putbit(1),putbyte(byte)
 *  else
 *      putbit(0)
 */
static void decompress_bits(outptr, needlen, bitptr)
    uch *outptr;        /* Pointer into output block */
    unsigned needlen;   /* Size of uncompressed block */
    ZCONST uch *bitptr; /* Pointer into compressed data */
{
    ulg bitbuf = 0;
    int bitcnt = 0;

#define _FILL   {       bitbuf |= (*bitptr++) << bitcnt;\
                        bitcnt += 8;                    \
                }

    while (needlen--)
    {
        if (bitcnt <= 0)
            _FILL;

        if (bitbuf & 1)
        {
            bitbuf >>= 1;
            if ((bitcnt -= 1) < 8)
                _FILL;
            *outptr++ = (uch)bitbuf;
            bitcnt -= 8;
            bitbuf >>= 8;
        }
        else
        {
            *outptr++ = '\0';
            bitcnt -= 1;
            bitbuf >>= 1;
        }
    }
} /* end function decompress_bits() */

#endif /* VMS || VMS_TEXT_CONV */





#ifdef SYMLINKS
/***********************************/
/* Function set_deferred_symlink() */
/***********************************/

static void set_deferred_symlink(__G__ slnk_entry)
    __GDEF
    slinkentry *slnk_entry;
{
    int sts;
    extent ucsize = slnk_entry->targetlen;
    char *linkfname = slnk_entry->fname;
    char *linktarget = (char *)malloc(ucsize+1);

#ifdef VMS
    static int vms_symlink_works = -1;

    if (vms_symlink_works < 0)
    {
        /* Test symlink() with an invalid file name.  If errno comes
         * back ENOSYS ("Function not implemented"), then don't try to
         * use it below on the symlink placeholder text files.
         */
        vms_symlink_works = symlink( "", "?");
        if (errno == ENOSYS)
            vms_symlink_works = 0;
        else
            vms_symlink_works = 1;
    }
#endif /* def VMS */

    if (!linktarget) {
        Info(slide, 0x201, ((char *)slide,
          LoadFarString(SymLnkWarnNoMem), FnFilter1(linkfname)));
        return;
    }
    linktarget[ucsize] = '\0';
    G.outfile = zfopen(linkfname, FOPR); /* open link placeholder for reading */
    /* Check that the following conditions are all fulfilled:
     * a) the placeholder file exists,
     * b) the placeholder file contains exactly "ucsize" bytes
     *    (read the expected placeholder content length + 1 extra byte, this
     *    should return the expected content length),
     * c) the placeholder content matches the link target specification as
     *    stored in the symlink control structure.
     */
    if (!G.outfile ||
        fread(linktarget, 1, ucsize+1, G.outfile) != ucsize ||
        strcmp(slnk_entry->target, linktarget))
    {
        Info(slide, 0x201, ((char *)slide,
          LoadFarString(SymLnkWarnInvalid), FnFilter1(linkfname)));
        free(linktarget);
        if (G.outfile)
            fclose(G.outfile);
        return;
    }
    fclose(G.outfile);                  /* close "data" file for good... */

#ifdef VMS
    if (vms_symlink_works == 0)
    {
        /* Should we be using some UnZip error message function instead
         * of perror() (or equivalent) for these "symlink error"
         * messages?
         */
        Info(slide, 0, ((char *)slide, LoadFarString(SymLnkFinish),
          FnFilter1(linkfname), FnFilter2(linktarget)));

        fprintf( stderr, "Symlink error: %s\n", strerror( ENOSYS));
        free(linktarget);
        return;
    }
#endif /* def VMS */

    unlink(linkfname);                  /* ...and delete it */
    sts = symlink(linktarget, linkfname);       /* create the real link */
    if (QCOND2 || (sts != 0))
        Info(slide, 0, ((char *)slide, LoadFarString(SymLnkFinish),
          FnFilter1(linkfname), FnFilter2(linktarget)));
    if (sts != 0)
        perror("symlink error");
    free(linktarget);
#ifdef SET_SYMLINK_ATTRIBS
    set_symlnk_attribs(__G__ slnk_entry);
#endif
    return;                             /* can't set time on symlinks */

} /* end function set_deferred_symlink() */
#endif /* SYMLINKS */




/*************************/
/*  Function fnfilter()  */        /* here instead of in list.c for SFX */
/*************************/

/*
  If Unicode is supported, assume we have what we need to do this
  check using wide characters, avoiding MBCS issues.
 */

char *fnfilter(raw, space, size)   /* convert name to safely printable form */
    ZCONST char *raw;
    uch *space;
    extent size;
{
#ifndef UZ_FNFILTER_REPLACECHAR
    /* A convenient choice for the replacement of unprintable char codes is
     * the "single char wildcard", as this character is quite unlikely to
     * appear in filenames by itself.  The following default definition
     * sets the replacement char to a question mark as the most common
     * "single char wildcard"; this setting should be overridden in the
     * appropiate system-specific configuration header when needed.
     */
# define UZ_FNFILTER_REPLACECHAR      '?'
#endif

#ifndef NATIVE   /* ASCII:  filter ANSI escape codes, etc. */

# ifdef UNICODE_SUPPORT
/* If Unicode support is enabled, do the isprint() checks by first
   converting to wide characters and checking those.  That avoids
   issues doing checks on multi-byte characters.  After the replacements
   the wide string is converted back to the local character set. */

    wchar_t *wstring;    /* wchar_t version of raw */
    size_t wslen;        /* length of wstring */
    wchar_t *wc;         /* pointer to char in wstring */
    wchar_t *wostring;   /* wchar_t version of output string */
    size_t woslen;       /* length of wostring */
    wchar_t *woc;        /* pointer to char in wostring */
	char *newraw;        /* new raw */
	ZCONST uch *r;
    uch *s=space;
    uch *slim=NULL;
    uch *se=NULL;
    int have_overflow = FALSE;

    wslen = mbstowcs(NULL, raw, 0 );
	if ((wstring = malloc((wslen + 1) * sizeof(wchar_t))) == NULL) {
        strcpy( (char *)space, raw);
        return (char *)space;
    }
	if ((wostring = malloc(2 * (wslen + 1) * sizeof(wchar_t))) == NULL) {
		free(wstring);
        strcpy( (char *)space, raw);
        return (char *)space;
    }
    wslen = mbstowcs(wstring, raw, wslen + 1);
	wc = wstring;
	woc = wostring;

    while (*wc) {
        if (!iswprint(*wc)) {
            if (*wc < 32) {
                /* ASCII control codes are escaped as "^{letter}". */
                *woc++ = (wchar_t)'^';
                *woc++ = (wchar_t)(64 + *wc);
            } else {
                /* Other unprintable codes are replaced by the
                 * placeholder character. */
                *woc++ = (wchar_t)UZ_FNFILTER_REPLACECHAR;
            }
        } else {
            *woc++ = *wc;
        }
        *wc++;
    }
    *woc = (wchar_t)0;

    /* convert back to local string to work with output buffer */
    woslen = wcstombs(NULL, wostring, 0 );
	if ((newraw = malloc(woslen + 1)) == NULL) {
		free(wstring);
		free(wostring);
        strcpy( (char *)space, raw);
        return (char *)space;
    }
    woslen = wcstombs(newraw, wostring, (woslen * MB_CUR_MAX) + 1 );


    if (size > 0) {
        slim = space + size - 4;
    }
    r = (ZCONST uch *)newraw;
    while (*r) {
        if (size > 0 && s >= slim && se == NULL) {
            se = s;
        }
#  ifdef QDOS
        if (qlflag & 2) {
            if (*r == '/' || *r == '.') {
                if (se != NULL && (s > (space + (size-3)))) {
                    have_overflow = TRUE;
                    break;
                }
                ++r;
                *s++ = '_';
                continue;
            }
        } else
#  endif
        {
            if (se != NULL && (s > (space + (size-3)))) {
                have_overflow = TRUE;
                break;
            }
            *s++ = *r++;
        }
    }
    if (have_overflow) {
        strcpy((char *)se, "...");
    } else {
        *s = '\0';
    }

    free(wstring);
    free(wostring);
    free(newraw);

# else /* !UNICODE_SUPPORT */

    ZCONST uch *r=(ZCONST uch *)raw;
    uch *s=space;
    uch *slim=NULL;
    uch *se=NULL;
    int have_overflow = FALSE;

    if (size > 0) {
        slim = space + size
#  ifdef _MBCS
                     - (MB_CUR_MAX - 1)
#  endif
                     - 4;
    }
    while (*r) {
        if (size > 0 && s >= slim && se == NULL) {
            se = s;
        }
#  ifdef QDOS
        if (qlflag & 2) {
            if (*r == '/' || *r == '.') {
                if (se != NULL && (s > (space + (size-3)))) {
                    have_overflow = TRUE;
                    break;
                }
                ++r;
                *s++ = '_';
                continue;
            }
        } else
#  endif
#  ifdef HAVE_WORKING_ISPRINT
        if (!isprint(*r)) {
            if (*r < 32) {
                /* ASCII control codes are escaped as "^{letter}". */
                if (se != NULL && (s > (space + (size-4)))) {
                    have_overflow = TRUE;
                    break;
                }
                *s++ = '^', *s++ = (uch)(64 + *r++);
            } else {
                /* Other unprintable codes are replaced by the
                 * placeholder character. */
                if (se != NULL && (s > (space + (size-3)))) {
                    have_overflow = TRUE;
                    break;
                }
                *s++ = UZ_FNFILTER_REPLACECHAR;
                INCSTR(r);
            }
#  else /* !HAVE_WORKING_ISPRINT */
        if (*r < 32) {
            /* ASCII control codes are escaped as "^{letter}". */
            if (se != NULL && (s > (space + (size-4)))) {
                have_overflow = TRUE;
                break;
            }
            *s++ = '^', *s++ = (uch)(64 + *r++);
#  endif /* ?HAVE_WORKING_ISPRINT */
        } else {
#  ifdef _MBCS
            unsigned i = CLEN(r);
            if (se != NULL && (s > (space + (size-i-2)))) {
                have_overflow = TRUE;
                break;
            }
            for (; i > 0; i--)
                *s++ = *r++;
#  else
            if (se != NULL && (s > (space + (size-3)))) {
                have_overflow = TRUE;
                break;
            }
            *s++ = *r++;
#  endif
         }
    }
    if (have_overflow) {
        strcpy((char *)se, "...");
    } else {
        *s = '\0';
    }
# endif /* !UNICODE_SUPPORT */

# ifdef WINDLL
    INTERN_TO_ISO((char *)space, (char *)space);  /* translate to ANSI */
# else
#  if (defined(WIN32) && !defined(_WIN32_WCE))
    /* Win9x console always uses OEM character coding, and
       WinNT console is set to OEM charset by default, too */
    INTERN_TO_OEM((char *)space, (char *)space);
#  endif /* (WIN32 && !_WIN32_WCE) */
# endif /* ?WINDLL */

    return (char *)space;

#else /* NATIVE:  EBCDIC or whatever */
    return (char *)raw;
#endif

} /* end function fnfilter() */




#ifdef SET_DIR_ATTRIB
/* must sort saved directories so can set perms from bottom up */

/************************/
/*  Function dircomp()  */
/************************/

static int Cdecl dircomp(a, b)  /* used by qsort(); swiped from Zip */
    ZCONST zvoid *a, *b;
{
    /* order is significant:  this sorts in reverse order (deepest first) */
    return strcmp((*(direntry **)b)->fn, (*(direntry **)a)->fn);
 /* return namecmp((*(direntry **)b)->fn, (*(direntry **)a)->fn); */
}

# if defined(UNICODE_SUPPORT) && defined(WIN32_WIDE)
static int Cdecl dircompw(a, b)  /* used by qsort(); swiped from Zip */
    ZCONST zvoid *a, *b;
{
    /* order is significant:  this sorts in reverse order (deepest first) */
    return wcscmp((*(direntryw **)b)->fnw, (*(direntryw **)a)->fnw);
 /* return namecmp((*(direntry **)b)->fn, (*(direntry **)a)->fn); */
}
# endif /* (defined(UNICODE_SUPPORT) && defined(WIN32_WIDE)) */

#endif /* SET_DIR_ATTRIB */


#ifdef USE_BZIP2

/**************************/
/*  Function UZbunzip2()  */
/**************************/

int UZbunzip2(__G)
__GDEF
/* decompress a bzipped entry using the libbz2 routines */
{
    int retval = 0;     /* return code: 0 = "no error" */
    int err=BZ_OK;
    int repeated_buf_err;
    bz_stream bstrm;

#if (defined(DLL) && !defined(NO_SLIDE_REDIR))
    if (G.redirect_slide)
        wsize = G.redirect_size, redirSlide = G.redirect_buffer;
    else
        wsize = WSIZE, redirSlide = slide;
#endif

    bstrm.next_out = (char *)redirSlide;
    bstrm.avail_out = wsize;

    bstrm.next_in = (char *)G.inptr;
    bstrm.avail_in = G.incnt;

    {
        /* local buffer for efficiency */
        /* $TODO Check for BZIP LIB version? */

        bstrm.bzalloc = NULL;
        bstrm.bzfree = NULL;
        bstrm.opaque = NULL;

        Trace((stderr, "initializing bzlib()\n"));
        err = BZ2_bzDecompressInit(&bstrm, 0, 0);

        if (err == BZ_MEM_ERROR)
            return 3;
        else if (err != BZ_OK)
            Trace((stderr, "oops!  (BZ2_bzDecompressInit() err = %d)\n", err));
    }

#ifdef FUNZIP
    while (err != BZ_STREAM_END) {
#else /* !FUNZIP */
    while (G.csize > 0) {
        Trace((stderr, "first loop:  G.csize = %ld\n", G.csize));
#endif /* ?FUNZIP */
        while (bstrm.avail_out > 0) {
            err = BZ2_bzDecompress(&bstrm);

            if (err == BZ_DATA_ERROR) {
                retval = 2; goto uzbunzip_cleanup_exit;
            } else if (err == BZ_MEM_ERROR) {
                retval = 3; goto uzbunzip_cleanup_exit;
            } else if (err != BZ_OK && err != BZ_STREAM_END)
                Trace((stderr, "oops!  (bzip(first loop) err = %d)\n", err));

#ifdef FUNZIP
            if (err == BZ_STREAM_END)    /* "END-of-entry-condition" ? */
#else /* !FUNZIP */
            if (G.csize <= 0L)          /* "END-of-entry-condition" ? */
#endif /* ?FUNZIP */
                break;

            if (bstrm.avail_in == 0) {
                if (fillinbuf(__G) == 0) {
                    /* no "END-condition" yet, but no more data */
                    retval = 2; goto uzbunzip_cleanup_exit;
                }

                bstrm.next_in = (char *)G.inptr;
                bstrm.avail_in = G.incnt;
            }
            Trace((stderr, "     avail_in = %u\n", bstrm.avail_in));
        }
        /* flush slide[] */
        if ((retval = FLUSH(wsize - bstrm.avail_out)) != 0)
            goto uzbunzip_cleanup_exit;
        Trace((stderr, "inside loop:  flushing %ld bytes (ptr diff = %ld)\n",
          (long)(wsize - bstrm.avail_out),
          (long)(bstrm.next_out-(char *)redirSlide)));
        bstrm.next_out = (char *)redirSlide;
        bstrm.avail_out = wsize;
    }

    /* no more input, so loop until we have all output */
    Trace((stderr, "beginning final loop:  err = %d\n", err));
    repeated_buf_err = FALSE;
    while (err != BZ_STREAM_END) {
        err = BZ2_bzDecompress(&bstrm);
        if (err == BZ_DATA_ERROR) {
            retval = 2; goto uzbunzip_cleanup_exit;
        } else if (err == BZ_MEM_ERROR) {
            retval = 3; goto uzbunzip_cleanup_exit;
        } else if (err != BZ_OK && err != BZ_STREAM_END) {
            Trace((stderr, "oops!  (bzip(final loop) err = %d)\n", err));
            DESTROYGLOBALS();
            EXIT(PK_MEM3);
        }
        /* final flush of slide[] */
        if ((retval = FLUSH(wsize - bstrm.avail_out)) != 0)
            goto uzbunzip_cleanup_exit;
        Trace((stderr, "final loop:  flushing %ld bytes (ptr diff = %ld)\n",
          (long)(wsize - bstrm.avail_out),
          (long)(bstrm.next_out-(char *)redirSlide)));
        bstrm.next_out = (char *)redirSlide;
        bstrm.avail_out = wsize;
    }
#ifdef LARGE_FILE_SUPPORT
    Trace((stderr, "total in = %llu, total out = %llu\n",
      (zusz_t)(bstrm.total_in_lo32) + ((zusz_t)(bstrm.total_in_hi32))<<32,
      (zusz_t)(bstrm.total_out_lo32) + ((zusz_t)(bstrm.total_out_hi32))<<32));
#else
    Trace((stderr, "total in = %lu, total out = %lu\n", bstrm.total_in_lo32,
      bstrm.total_out_lo32));
#endif

    G.inptr = (uch *)bstrm.next_in;
    G.incnt = (G.inbuf + INBUFSIZ) - G.inptr;  /* reset for other routines */

uzbunzip_cleanup_exit:
    err = BZ2_bzDecompressEnd(&bstrm);
    if (err != BZ_OK)
        Trace((stderr, "oops!  (BZ2_bzDecompressEnd() err = %d)\n", err));

    return retval;
} /* end function UZbunzip2() */
#endif /* USE_BZIP2 */
