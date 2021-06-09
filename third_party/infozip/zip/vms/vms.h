/*
  Copyright (c) 1990-2007 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*---------------------------------------------------------------------------

  vms.h

  Generic VMS header file for Info-ZIP's Zip and UnZip.

  ---------------------------------------------------------------------------*/

#ifndef __vms_h
#define __vms_h 1

#ifndef __DESCRIP_LOADED
#include <descrip.h>
#endif
#ifndef __STARLET_LOADED
#include <starlet.h>
#endif
#ifndef __SYIDEF_LOADED
#include <syidef.h>
#endif
#ifndef __ATRDEF_LOADED
#include <atrdef.h>
#endif
#ifndef __FIBDEF_LOADED
#include <fibdef.h>
#endif
#ifndef __IODEF_LOADED
#include <iodef.h>
#endif
#if !defined(_RMS_H) && !defined(__RMS_LOADED)
#include <rms.h>
#endif

#define ERR(s) !((s) & 1)       /* VMS system error */

#ifndef SYI$_VERSION
#define SYI$_VERSION 4096       /* VMS 5.4 definition */
#endif

/*
 *  Under Alpha (DEC C in VAXC mode) and under `good old' VAXC, the FIB unions
 *  are declared as variant_unions.  DEC C (Alpha) in ANSI modes and third
 *  party compilers which do not support `variant_union' define preprocessor
 *  symbols to `hide' the "intermediate union/struct" names from the
 *  programmer's API.
 *  We check the presence of these defines and for DEC's FIBDEF.H defining
 *  __union as variant_union to make sure we access the structure correctly.
 */
#define variant_union 1
#if defined(fib$w_did) || (defined(__union) && (__union == variant_union))
#  define FIB$W_DID     fib$w_did
#  define FIB$W_FID     fib$w_fid
#  define FIB$L_ACCTL   fib$l_acctl
#  define FIB$W_EXCTL   fib$w_exctl
#else
#  define FIB$W_DID     fib$r_did_overlay.fib$w_did
#  define FIB$W_FID     fib$r_fid_overlay.fib$w_fid
#  define FIB$L_ACCTL   fib$r_acctl_overlay.fib$l_acctl
#  define FIB$W_EXCTL   fib$r_exctl_overlay.fib$w_exctl
#endif
#undef variant_union


/* 2005-02-08 SMS.  Moved NAM[L] macros here from VMS.C. */

/* Define macros for use with either NAM or NAML. */

#ifdef NAML$C_MAXRSS            /* NAML is available.  Use it. */

#  define NAM_STRUCT NAML

#  define FAB_OR_NAML( fab, nam) nam
#  define FAB_OR_NAML_DNA naml$l_long_defname
#  define FAB_OR_NAML_DNS naml$l_long_defname_size
#  define FAB_OR_NAML_FNA naml$l_long_filename
#  define FAB_OR_NAML_FNS naml$l_long_filename_size

#  define CC_RMS_NAM cc$rms_naml
#  define FAB_NAM fab$l_naml
#  define NAM_DID naml$w_did
#  define NAM_DVI naml$t_dvi
#  define NAM_ESA naml$l_long_expand
#  define NAM_ESL naml$l_long_expand_size
#  define NAM_ESS naml$l_long_expand_alloc
#  define NAM_FID naml$w_fid
#  define NAM_FNB naml$l_fnb
#  define NAM_RSA naml$l_long_result
#  define NAM_RSL naml$l_long_result_size
#  define NAM_RSS naml$l_long_result_alloc
#  define NAM_MAXRSS NAML$C_MAXRSS
#  define NAM_NOP naml$b_nop
#  define NAM_M_EXP_DEV NAML$M_EXP_DEV
#  define NAM_M_SYNCHK NAML$M_SYNCHK
#  define NAM_B_DEV naml$l_long_dev_size
#  define NAM_L_DEV naml$l_long_dev
#  define NAM_B_DIR naml$l_long_dir_size
#  define NAM_L_DIR naml$l_long_dir
#  define NAM_B_NAME naml$l_long_name_size
#  define NAM_L_NAME naml$l_long_name
#  define NAM_B_TYPE naml$l_long_type_size
#  define NAM_L_TYPE naml$l_long_type
#  define NAM_B_VER naml$l_long_ver_size
#  define NAM_L_VER naml$l_long_ver

#else /* def NAML$C_MAXRSS */   /* NAML is not available.  Use NAM. */

#  define NAM_STRUCT NAM

#  define FAB_OR_NAML( fab, nam) fab
#  define FAB_OR_NAML_DNA fab$l_dna
#  define FAB_OR_NAML_DNS fab$b_dns
#  define FAB_OR_NAML_FNA fab$l_fna
#  define FAB_OR_NAML_FNS fab$b_fns

#  define CC_RMS_NAM cc$rms_nam
#  define FAB_NAM fab$l_nam
#  define NAM_DID nam$w_did
#  define NAM_DVI nam$t_dvi
#  define NAM_ESA nam$l_esa
#  define NAM_ESL nam$b_esl
#  define NAM_ESS nam$b_ess
#  define NAM_FID nam$w_fid
#  define NAM_FNB nam$l_fnb
#  define NAM_RSA nam$l_rsa
#  define NAM_RSL nam$b_rsl
#  define NAM_RSS nam$b_rss
#  define NAM_MAXRSS NAM$C_MAXRSS
#  define NAM_NOP nam$b_nop
#  define NAM_M_EXP_DEV NAM$M_EXP_DEV
#  define NAM_M_SYNCHK NAM$M_SYNCHK
#  define NAM_B_DEV nam$b_dev
#  define NAM_L_DEV nam$l_dev
#  define NAM_B_DIR nam$b_dir
#  define NAM_L_DIR nam$l_dir
#  define NAM_B_NAME nam$b_name
#  define NAM_L_NAME nam$l_name
#  define NAM_B_TYPE nam$b_type
#  define NAM_L_TYPE nam$l_type
#  define NAM_B_VER nam$b_ver
#  define NAM_L_VER nam$l_ver

#endif /* def NAML$C_MAXRSS */


struct EB_header    /* Common header of extra block */
{   ush tag;
    ush size;
    uch data[1];
};

#ifndef EB_HEADSIZE
#  define EB_HEADSIZE 4
#endif

/*------ Old style Info-ZIP extra field definitions -----*/

#if (!defined(VAXC) && !defined(_RMS_H) && !defined(__RMS_LOADED))

struct XAB {                    /* This definition may be skipped */
    unsigned char xab$b_cod;
    unsigned char xab$b_bln;
    short int xabdef$$_fill_1;
    char *xab$l_nxt;
};

#endif /* !VAXC && !_RMS_H && !__RMS_LOADED */

#ifndef EB_IZVMS_BCMASK
#  define EB_IZVMS_BCMASK   07  /* 3 bits for compression type */
#endif
#ifndef EB_IZVMS_BCSTOR
#  define EB_IZVMS_BCSTOR   0   /*  Stored */
#endif
#ifndef EB_IZVMS_BC00
#  define EB_IZVMS_BC00     1   /*  0byte -> 0bit compression */
#endif
#ifndef EB_IZVMS_BCDEFL
#  define EB_IZVMS_BCDEFL   2   /*  Deflated */
#endif

/*
 *  Extra record format
 *  ===================
 *  signature       (2 bytes)   = 'I','M'
 *  size            (2 bytes)
 *  block signature (4 bytes)
 *  flags           (2 bytes)
 *  uncomprssed size(2 bytes)
 *  reserved        (4 bytes)
 *  data            ((size-12) bytes)
 *  ....
 */

struct IZ_block                 /* Extra field block header structure */
{
    ush sig;
    ush size;
    ulg bid;
    ush flags;
    ush length;
    ulg reserved;
    uch body[1];                /* The actual size is unknown */
};

/*
 *   Extra field signature and block signatures
 */

#define IZ_SIGNATURE "IM"
#define FABSIG  "VFAB"
#define XALLSIG "VALL"
#define XFHCSIG "VFHC"
#define XDATSIG "VDAT"
#define XRDTSIG "VRDT"
#define XPROSIG "VPRO"
#define XKEYSIG "VKEY"
#define XNAMSIG "VNAM"
#define VERSIG  "VMSV"

/*
 *   Block sizes
 */

#define FABL    (cc$rms_fab.fab$b_bln)
#define RABL    (cc$rms_rab.rab$b_bln)
#define XALLL   (cc$rms_xaball.xab$b_bln)
#define XDATL   (cc$rms_xabdat.xab$b_bln)
#define XFHCL   (cc$rms_xabfhc.xab$b_bln)
#define XKEYL   (cc$rms_xabkey.xab$b_bln)
#define XPROL   (cc$rms_xabpro.xab$b_bln)
#define XRDTL   (cc$rms_xabrdt.xab$b_bln)
#define XSUML   (cc$rms_xabsum.xab$b_bln)
#define EXTBSL  4               /* Block signature length */
#define RESL    8               /* Reserved 8 bytes */
#define EXTHL   (EB_HEADSIZE+EXTBSL+RESL)

typedef unsigned char byte;

struct iosb
{
    ush status;
    ush count;
    ulg spec;
};

/*------------ PKWARE extra block definitions ----------*/

/* Structure of PKWARE extra header */

#ifdef VMS_ZIP

#if defined(__DECC) || defined(__DECCXX)
#pragma __nostandard
#endif /* __DECC || __DECCXX */

#if defined(__DECC) || defined(__DECCXX)
#pragma __member_alignment __save
#pragma __nomember_alignment
#endif /* __DECC || __DECCXX */

#ifdef VMS_ORIGINAL_PK_LAYOUT
/*  The original order of ATR fields in the PKZIP VMS-extra field leads
 *  to unaligned fields in the PK_info structure representing the
 *  extra field layout.  When compiled for Alpha AXP, this results in
 *  some performance (and code size) penalty.  It is not allowed to
 *  apply structure padding, since this is explicitely forbidden in
 *  the specification (APPNOTE.TXT) for the PK VMS extra field.
 */
typedef struct
{
    ush tag_ra; ush len_ra;     byte ra[ATR$S_RECATTR];
    ush tag_uc; ush len_uc;     byte uc[ATR$S_UCHAR];
    ush tag_jr; ush len_jr;     byte jr[ATR$S_JOURNAL];
    ush tag_cd; ush len_cd;     byte cd[ATR$S_CREDATE];
    ush tag_rd; ush len_rd;     byte rd[ATR$S_REVDATE];
    ush tag_ed; ush len_ed;     byte ed[ATR$S_EXPDATE];
    ush tag_bd; ush len_bd;     byte bd[ATR$S_BAKDATE];
    ush tag_rn; ush len_rn;     ush  rn;
    ush tag_ui; ush len_ui;     byte ui[ATR$S_UIC];
    ush tag_fp; ush len_fp;     byte fp[ATR$S_FPRO];
    ush tag_rp; ush len_rp;     byte rp[ATR$S_RPRO];
} PK_info_t;
#else /* !VMS_ORIGINAL_PK_LAYOUT */
/*  The Info-ZIP support for the PK VMS extra field uses a reordered
 *  field layout to achieve ``natural alignment'' of the PK_info structure
 *  members whenever possible.  This rearrangement does not violate the
 *  PK's VMS extra field specification and should not break any ``well
 *  behaving'' (PK)Unzip utility. (`Well behaving' means that (PK)Unzip
 *  should use the field tag to identify the ATR$ field rather than
 *  assuming a fixed order of ATR$ fields in the PK VMS extra field.)
 */
typedef struct
{
    ush tag_ra; ush len_ra;     byte ra[ATR$S_RECATTR];
    ush tag_uc; ush len_uc;     byte uc[ATR$S_UCHAR];
    ush tag_cd; ush len_cd;     byte cd[ATR$S_CREDATE];
    ush tag_rd; ush len_rd;     byte rd[ATR$S_REVDATE];
    ush tag_ed; ush len_ed;     byte ed[ATR$S_EXPDATE];
    ush tag_bd; ush len_bd;     byte bd[ATR$S_BAKDATE];
    ush tag_rn; ush len_rn;     ush  rn;
    ush tag_ui; ush len_ui;     byte ui[ATR$S_UIC];
    ush tag_fp; ush len_fp;     byte fp[ATR$S_FPRO];
    ush tag_rp; ush len_rp;     byte rp[ATR$S_RPRO];
    ush tag_jr; ush len_jr;     byte jr[ATR$S_JOURNAL];
} PK_info_t;
#endif /* ?VMS_ORIGINAL_PK_LAYOUT */

#if defined(__DECC) || defined(__DECCXX)
#pragma __member_alignment __restore
#endif /* __DECC || __DECCXX */

#if defined(__DECC) || defined(__DECCXX)
#pragma __standard
#endif /* __DECC || __DECCXX */

#endif /* VMS_ZIP */

/* PKWARE "VMS" tag */
#define PK_SIGNATURE        0x000C

/* Total number of attributes to be saved */
#define VMS_ATTR_COUNT  11
#define VMS_MAX_ATRCNT  20

struct PK_field
{
    ush         tag;
    ush         size;
    byte        value[1];
};

#define PK_FLDHDR_SIZE  4

struct PK_header
{
    ush tag;
    ush size;
    ulg crc32;
    byte data[1];
};

#define PK_HEADER_SIZE  8

char *vms_file_version( char *s);

#endif /* !__vms_h */
