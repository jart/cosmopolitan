/*
  Copyright (c) 1990-2000 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*---------------------------------------------------------------------------

  vmsdefs.h

  Contents of three header files from Joe
  Meadows' FILE program.  Used by vmsmunch

        06-Apr-1994     Jamie Hanrahan  jeh@cmkrnl.com
                        Moved "contents of three header files" from
                        VMSmunch.h to VMSdefs.h .

        16-Sep-1995     Christian Spieler
                        Added #pragma (no)member_alignment directives
                        to achieve compatibility with DEC C and Alpha AXP

        05-Oct-1995     Christian Spieler
                        Revised fatdef, fchdef, fjndef to achieve closer
                        compatibility with DEC's system include header files
                        supplied with C version 4.0 and newer.

        10-Oct-1995     Christian Spieler
                        Use lowercase filenames for vms specific sources
                        (VMSmunch.? -> vmsmunch.?, VMSdefs.h -> vmsdefs.h)

        15-Dec-1995     Christian Spieler
                        Removed the last "tabs" from the source.

        24-Jun-1997     Onno van der Linden / Chr. Spieler
                        Modifications to support the VMS port of GNU C 2.x.

        27-Jul-1999     Chr. Spieler
                        Added Info-ZIP copyright note for identification.

  ---------------------------------------------------------------------------*/

#ifndef __vmsdefs_h
#define __vmsdefs_h 1

#if defined(__DECC) || defined(__DECCXX)
#pragma __nostandard
#endif /* __DECC || __DECCXX */

#if defined(__DECC) || defined(__DECCXX)
#pragma __member_alignment __save
#pragma __nomember_alignment
#endif /* __DECC || __DECCXX */

#if !(defined(__VAXC) || defined(VAXC)) || defined(__GNUC__)
#define __struct struct
#define __union union
#else
#define __struct variant_struct
#define __union variant_union
#endif /* !(__VAXC || VAXC) || __GNUC__ */

#ifdef __cplusplus
    extern "C" {
#endif

/*---------------------------------------------------------------------------
    fatdef.h
  ---------------------------------------------------------------------------*/

/* This header file was created by Joe Meadows, and is not copyrighted
   in any way. No guarantee is made as to the accuracy of the contents
   of this header file. This header file was last modified on Sep. 22th,
   1987. (Modified to include this statement) */

#define FAT$K_LENGTH 32
#define FAT$C_LENGTH 32
#define FAT$S_FATDEF 32

struct fatdef {
  __union  {
    unsigned char fat$b_rtype;          /* record type                      */
    __struct  {
      unsigned fat$v_rtype : 4;         /* record type subfield             */
      unsigned fat$v_fileorg : 4;       /* file organization                */
    } fat$r_rtype_bits;
  } fat$r_rtype_overlay;
# define FAT$S_RTYPE 4
# define FAT$V_RTYPE 0
#   define FAT$C_UNDEFINED 0
#   define FAT$C_FIXED 1
#   define FAT$C_VARIABLE 2
#   define FAT$C_VFC 3
#   define FAT$C_STREAM 4
#   define FAT$C_STREAMLF 5
#   define FAT$C_STREAMCR 6
# define FAT$S_FILEORG 4
# define FAT$V_FILEORG 4
#   define FAT$C_SEQUENTIAL 0
#   define FAT$C_RELATIVE 1
#   define FAT$C_INDEXED 2
#   define FAT$C_DIRECT 3
  __union  {
    unsigned char fat$b_rattrib;        /* record attributes                */
    __struct  {
      unsigned fat$v_fortrancc : 1;
      unsigned fat$v_impliedcc : 1;
      unsigned fat$v_printcc : 1;
      unsigned fat$v_nospan : 1;
      unsigned fat$v_msbrcw : 1;
    } fat$r_rattrib_bits;
  } fat$r_rattrib_overlay;
#   define FAT$V_FORTRANCC 0
#   define FAT$M_FORTRANCC 1
#   define FAT$V_IMPLIEDCC 1
#   define FAT$M_IMPLIEDCC 2
#   define FAT$V_PRINTCC 2
#   define FAT$M_PRINTCC 4
#   define FAT$V_NOSPAN 3
#   define FAT$M_NOSPAN 8
#   define FAT$V_MSBRCW 4
#   define FAT$M_MSBRCW 16
  unsigned short int fat$w_rsize;       /* record size in bytes             */
  __union
  {
    unsigned long int fat$l_hiblk;      /* highest allocated VBN            */
    __struct
    {
      unsigned short int fat$w_hiblkh;  /* high order word                  */
      unsigned short int fat$w_hiblkl;  /* low order word                   */
    } fat$r_hiblk_fields;
  } fat$r_hiblk_overlay;
  __union
  {
    unsigned long int fat$l_efblk;      /* end of file VBN                  */
    __struct
    {
      unsigned short int fat$w_efblkh;  /* high order word                  */
      unsigned short int fat$w_efblkl;  /* low order word                   */
    } fat$r_efblk_fields;
  } fat$r_efblk_overlay;
  unsigned short int fat$w_ffbyte;      /* first free byte in EFBLK         */
  unsigned char fat$b_bktsize;          /* bucket size in blocks            */
  unsigned char fat$b_vfcsize;          /* # of control bytes in VFC record */
  unsigned short int fat$w_maxrec;      /* maximum record size in bytes     */
  unsigned short int fat$w_defext;      /* default extend quantity          */
  unsigned short int fat$w_gbc;         /* global buffer count              */
  char fat$fill[8];
  unsigned short int fat$w_versions;
};

#if !(defined(__VAXC) || defined(VAXC)) || defined(__GNUC__)
#define fat$b_rtype fat$r_rtype_overlay.fat$b_rtype
#define fat$v_rtype fat$r_rtype_overlay.fat$r_rtype_bits.fat$v_rtype
#define fat$v_fileorg fat$r_rtype_overlay.fat$r_rtype_bits.fat$v_fileorg
#define fat$b_rattrib fat$r_rattrib_overlay.fat$b_rattrib
#define fat$v_fortrancc fat$r_rattrib_overlay.fat$r_rattrib_bits.fat$v_fortrancc
#define fat$v_impliedcc fat$r_rattrib_overlay.fat$r_rattrib_bits.fat$v_impliedcc
#define fat$v_printcc fat$r_rattrib_overlay.fat$r_rattrib_bits.fat$v_printcc
#define fat$v_nospan fat$r_rattrib_overlay.fat$r_rattrib_bits.fat$v_nospan
#define fat$v_msbrcw fat$r_rattrib_overlay.fat$r_rattrib_bits.fat$v_msbrcw
#define fat$l_hiblk fat$r_hiblk_overlay.fat$l_hiblk
#define fat$w_hiblkh fat$r_hiblk_overlay.fat$r_hiblk_fields.fat$w_hiblkh
#define fat$w_hiblkl fat$r_hiblk_overlay.fat$r_hiblk_fields.fat$w_hiblkl
#define fat$l_efblk fat$r_efblk_overlay.fat$l_efblk
#define fat$w_efblkh fat$r_efblk_overlay.fat$r_efblk_fields.fat$w_efblkh
#define fat$w_efblkl fat$r_efblk_overlay.fat$r_efblk_fields.fat$w_efblkl
#endif /* !(__VAXC || VAXC) || __GNUC__ */

#define __FATDEF_LOADED 1       /* prevent inclusion of DECC's fatdef.h */

/*---------------------------------------------------------------------------
    fchdef.h
  ---------------------------------------------------------------------------*/

/* This header file was created by Joe Meadows, and is not copyrighted
   in any way. No guarantee is made as to the accuracy of the contents
   of this header file. This header file was last modified on Sep. 22th,
   1987. (Modified to include this statement) */

#define FCH$V_BADACL 0x00B
#define FCH$M_BADACL (1 << FCH$V_BADACL)
#define FCH$V_BADBLOCK 0x00E
#define FCH$M_BADBLOCK (1 << FCH$V_BADBLOCK)
#define FCH$V_CONTIG 0x007
#define FCH$M_CONTIG (1 << FCH$V_CONTIG)
#define FCH$V_CONTIGB 0x005
#define FCH$M_CONTIGB (1 << FCH$V_CONTIGB)
#define FCH$V_DIRECTORY 0x00D
#define FCH$M_DIRECTORY (1 << FCH$V_DIRECTORY)
#define FCH$V_ERASE 0x011
#define FCH$M_ERASE (1 << FCH$V_ERASE)
#define FCH$V_LOCKED 0x006
#define FCH$M_LOCKED (1 << FCH$V_LOCKED)
#define FCH$V_MARKDEL 0x00F
#define FCH$M_MARKDEL (1 << FCH$V_MARKDEL)
#define FCH$V_NOBACKUP 0x001
#define FCH$M_NOBACKUP (1 << FCH$V_NOBACKUP)
#define FCH$V_NOCHARGE 0x010
#define FCH$M_NOCHARGE (1 << FCH$V_NOCHARGE)
#define FCH$V_READCHECK 0x003
#define FCH$M_READCHECK (1 << FCH$V_READCHECK)
#define FCH$V_SPOOL 0x00C
#define FCH$M_SPOOL (1 << FCH$V_SPOOL)
#define FCH$V_WRITCHECK 0x004
#define FCH$M_WRITCHECK (1 << FCH$V_WRITCHECK)
#define FCH$V_WRITEBACK 0x002
#define FCH$M_WRITEBACK (1 << FCH$V_WRITEBACK)

struct fchdef  {
  __union  {
    int fch$$_fill_1;
    __struct  {
      unsigned fch$$_fill_31 : 8;
      unsigned fch$v_vcc_state : 3;    /* VCC state bits              */
      unsigned fch$$_fill_32 : 7;
      unsigned fch$$_alm_state : 2;
      unsigned fch$v_associated : 1;   /* ISO 9660 Associated file    */
      unsigned fch$v_existence : 1;    /* ISO 9660 Existence file     */
      unsigned fch$v_fill_6 : 2;
    } fch$r_fill_1_chunks;
    __struct  {
      unsigned fch$v_wascontig : 1;
      unsigned fch$v_nobackup : 1 ;
      unsigned fch$v_writeback : 1;
      unsigned fch$v_readcheck : 1;
      unsigned fch$v_writcheck : 1;
      unsigned fch$v_contigb : 1;
      unsigned fch$v_locked : 1;
      unsigned fch$v_contig : 1;
      unsigned fch$$_fill_3 : 3;
      unsigned fch$v_badacl : 1;
      unsigned fch$v_spool : 1;
      unsigned fch$v_directory : 1;
      unsigned fch$v_badblock : 1;
      unsigned fch$v_markdel : 1;
      unsigned fch$v_nocharge : 1;
      unsigned fch$v_erase : 1;
      unsigned fch$$_fill_4 : 1;
      unsigned fch$v_shelved : 1;
      unsigned fch$v_scratch : 1;
      unsigned fch$v_nomove : 1;
      unsigned fch$v_noshelvable : 1;
    } fch$r_fill_1_bits;
  } fch$r_fch_union;
};

#if !(defined(__VAXC) || defined(VAXC)) || defined(__GNUC__)
#define fch$v_vcc_state fch$r_fch_union.fch$r_fill_1_chunks.fch$v_vcc_state
#define fch$v_associated fch$r_fch_union.fch$r_fill_1_chunks.fch$v_associated
#define fch$v_existence fch$r_fch_union.fch$r_fill_1_chunks.fch$v_existence
#define fch$v_wascontig fch$r_fch_union.fch$r_fill_1_bits.fch$v_wascontig
#define fch$v_nobackup fch$r_fch_union.fch$r_fill_1_bits.fch$v_nobackup
#define fch$v_writeback fch$r_fch_union.fch$r_fill_1_bits.fch$v_writeback
#define fch$v_readcheck fch$r_fch_union.fch$r_fill_1_bits.fch$v_readcheck
#define fch$v_writcheck fch$r_fch_union.fch$r_fill_1_bits.fch$v_writcheck
#define fch$v_contigb fch$r_fch_union.fch$r_fill_1_bits.fch$v_contigb
#define fch$v_locked fch$r_fch_union.fch$r_fill_1_bits.fch$v_locked
#define fch$v_contig fch$r_fch_union.fch$r_fill_1_bits.fch$v_contig
#define fch$v_badacl fch$r_fch_union.fch$r_fill_1_bits.fch$v_badacl
#define fch$v_spool fch$r_fch_union.fch$r_fill_1_bits.fch$v_spool
#define fch$v_directory fch$r_fch_union.fch$r_fill_1_bits.fch$v_directory
#define fch$v_badblock fch$r_fch_union.fch$r_fill_1_bits.fch$v_badblock
#define fch$v_markdel fch$r_fch_union.fch$r_fill_1_bits.fch$v_markdel
#define fch$v_nocharge fch$r_fch_union.fch$r_fill_1_bits.fch$v_nocharge
#define fch$v_erase fch$r_fch_union.fch$r_fill_1_bits.fch$v_erase
#define fch$v_shelved fch$r_fch_union.fch$r_fill_1_bits.fch$v_shelved
#define fch$v_scratch fch$r_fch_union.fch$r_fill_1_bits.fch$v_scratch
#define fch$v_nomove fch$r_fch_union.fch$r_fill_1_bits.fch$v_nomove
#define fch$v_noshelvable fch$r_fch_union.fch$r_fill_1_bits.fch$v_noshelvable
#endif /* !(__VAXC || VAXC) || __GNUC__ */

#define __FCHDEF_LOADED 1       /* prevent inclusion of DECC's fchdef.h */

/*---------------------------------------------------------------------------
    fjndef.h
  ---------------------------------------------------------------------------*/

/* This header file was created by Joe Meadows, and is not copyrighted
   in any way. No guarantee is made as to the accuracy of the contents
   of this header file. This header file was last modified on Sep. 22th,
   1987. (Modified to include this statement) */

#define FJN$M_ONLY_RU 1
#define FJN$M_RUJNL 2
#define FJN$M_BIJNL 4
#define FJN$M_AIJNL 8
#define FJN$M_ATJNL 16
#define FJN$M_NEVER_RU 32
#define FJN$M_JOURNAL_FILE 64
#define FJN$S_FJNDEF 1
struct fjndef  {
  unsigned fjn$v_only_ru : 1;
  unsigned fjn$v_rujnl : 1;
  unsigned fjn$v_bijnl : 1;
  unsigned fjn$v_aijnl : 1;
  unsigned fjn$v_atjnl : 1;
  unsigned fjn$v_never_ru : 1;
  unsigned fjn$v_journal_file : 1;
  unsigned fjn$v_fill_7 : 1;
} ;

#define __FJNDEF_LOADED 1       /* prevent inclusion of DECC's fjndef.h */

/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
    }
#endif

#if defined(__DECC) || defined(__DECCXX)
#pragma __member_alignment __restore
#endif /* __DECC || __DECCXX */

#if defined(__DECC) || defined(__DECCXX)
#pragma __standard
#endif /* __DECC || __DECCXX */

#endif /* !__vmsdefs_h */
