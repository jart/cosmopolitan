/*
  Copyright (c) 1990-2007 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/* crc_i386.c -- Microsoft 32-bit C/C++ adaptation of crc_i386.asm
 * Created by Rodney Brown from crc_i386.asm, modified by Chr. Spieler.
 * Last revised: 07-Jan-2007
 *
 * Original coded (in crc_i386.asm) and put into the public domain
 * by Paul Kienitz and Christian Spieler.
 *
 * Revised 06-Oct-96, Scott Field (sfield@microsoft.com)
 *   fixed to assemble with masm by not using .model directive which makes
 *   assumptions about segment alignment.  Also,
 *   avoid using loop, and j[e]cxz where possible.  Use mov + inc, rather
 *   than lodsb, and other misc. changes resulting in the following performance
 *   increases:
 *
 *      unrolled loops                NO_UNROLLED_LOOPS
 *      *8    >8      <8              *8      >8      <8
 *
 *      +54%  +42%    +35%            +82%    +52%    +25%
 *
 *   first item in each table is input buffer length, even multiple of 8
 *   second item in each table is input buffer length, > 8
 *   third item in each table is input buffer length, < 8
 *
 * Revised 02-Apr-97, Chr. Spieler, based on Rodney Brown (rdb@cmutual.com.au)
 *   Incorporated Rodney Brown's 32-bit-reads optimization as found in the
 *   UNIX AS source crc_i386.S. This new code can be disabled by defining
 *   the macro symbol NO_32_BIT_LOADS.
 *
 * Revised 12-Oct-97, Chr. Spieler, based on Rodney Brown (rdb@cmutual.com.au)
 *   Incorporated Rodney Brown's additional tweaks for 32-bit-optimized CPUs
 *   (like the Pentium Pro, Pentium II, and probably some Pentium clones).
 *   This optimization is controlled by the macro symbol __686 and is disabled
 *   by default. (This default is based on the assumption that most users
 *   do not yet work on a Pentium Pro or Pentium II machine ...)
 *
 * Revised 16-Nov-97, Chr. Spieler: Made code compatible with Borland C++
 *   32-bit, removed unneeded kludge for potentially unknown movzx mnemonic,
 *   confirmed correct working with MS VC++ (32-bit).
 *
 * Revised 22-May-98, Peter Kunath, Chr. Spieler: The 16-Nov-97 revision broke
 *   MSVC 5.0. Inside preprocessor macros, each instruction is enclosed in its
 *   own __asm {...} construct.  For MSVC, a "#pragma warning" was added to
 *   shut up the "no return value" warning message.
 *
 * Revised 13-Dec-98, Chr. Spieler: Modified path to "zip.h" header file.
 *
 * Revised 16-Jan-2005, Cosmin Truta: Added the ASM_CRC guard, for easier
 *   switching between ASM vs. non-ASM builds, when handling makefiles.
 *   Also enabled the 686 build by default, because there are hardly any
 *   pre-686 CPUs in serious use nowadays. (See the 12-Oct-97 note above.)
 *
 * Revised 03-Jan-2006, Chr. Spieler
 *   Enlarged unrolling loops to "do 16 bytes per turn"; optimized access to
 *   data buffer in loop body (adjust pointer only once in loop body and use
 *   offsets to access each item); added additional support for the "unfolded
 *   tables" optimization variant (enabled by IZ_CRCOPTIM_UNFOLDTBL).
 *
 * Revised 07-Jan-2007, Chr. Spieler
 *   Recognize additional conditional flag CRC_TABLE_ONLY that prevents
 *   compilation of the crc32() function.
 *
 * FLAT memory model assumed.
 *
 * Loop unrolling can be disabled by defining the macro NO_UNROLLED_LOOPS.
 * This results in shorter code at the expense of reduced performance.
 *
 */

#include "../zip.h"
#include "../crc32.h"

#if defined(ASM_CRC) && !defined(USE_ZLIB) && !defined(CRC_TABLE_ONLY)

#if !defined(PRE_686) && !defined(__686)
#  define __686
#endif

#ifndef ZCONST
#  define ZCONST const
#endif

/* Select wether the following inline-assember code is supported. */
#if (defined(_MSC_VER) && _MSC_VER >= 700)
#if (defined(_M_IX86) && _M_IX86 >= 300)
#  define MSC_INLINE_ASM_32BIT_SUPPORT
   /* Disable warning for no return value, typical of asm functions */
#  pragma warning( disable : 4035 )
#endif
#endif

#if (defined(__BORLANDC__) && __BORLANDC__ >= 452)
#  define MSC_INLINE_ASM_32BIT_SUPPORT
#endif

#ifdef MSC_INLINE_ASM_32BIT_SUPPORT
/* This code is intended for Microsoft C/C++ (32-bit) compatible compilers. */

/*
 * These two (three) macros make up the loop body of the CRC32 cruncher.
 * registers modified:
 *   eax  : crc value "c"
 *   esi  : pointer to next data byte (or dword) "buf++"
 * registers read:
 *   edi  : pointer to base of crc_table array
 * scratch registers:
 *   ebx  : index into crc_table array
 *          (requires upper three bytes = 0 when __686 is undefined)
 */
#ifndef __686
#define Do_CRC { \
  __asm { mov   bl, al }; \
  __asm { shr   eax, 8 }; \
  __asm { xor   eax, [edi+ebx*4] }; }
#else /* __686 */
#define Do_CRC { \
  __asm { movzx ebx, al }; \
  __asm { shr   eax, 8  }; \
  __asm { xor   eax, [edi+ebx*4] }; }
#endif /* ?__686 */

#define Do_CRC_byte { \
  __asm { xor   al, byte ptr [esi] }; \
  __asm { inc   esi }; \
  Do_CRC; }

#define Do_CRC_byteof(ofs) { \
  __asm { xor   al, byte ptr [esi+(ofs)] }; \
  Do_CRC; }

#ifndef NO_32_BIT_LOADS
#ifdef IZ_CRCOPTIM_UNFOLDTBL
# define SavLen  len            /* the edx register is needed elsewhere */
# define UpdCRC_dword { \
   __asm { movzx   ebx,al }; \
   __asm { mov     edx,[edi+ebx*4+3072] }; \
   __asm { movzx   ebx,ah }; \
   __asm { shr     eax,16 }; \
   __asm { xor     edx,[edi+ebx*4+2048] }; \
   __asm { movzx   ebx,al }; \
   __asm { shr     eax,8 }; \
   __asm { xor     edx,[edi+ebx*4+1024] }; \
   __asm { mov     eax,[edi+eax*4] }; \
   __asm { xor     eax,edx }; }
# define UpdCRC_dword_sh(dwPtrIncr) { \
   __asm { movzx   ebx,al }; \
   __asm { mov     edx,[edi+ebx*4+3072] }; \
   __asm { movzx   ebx,ah }; \
   __asm { xor     edx,[edi+ebx*4+2048] }; \
   __asm { shr     eax,16 }; \
   __asm { movzx   ebx,al }; \
   __asm { add     esi, 4*dwPtrIncr }; \
   __asm { shr     eax,8 }; \
   __asm { xor     edx,[edi+ebx*4+1024] }; \
   __asm { mov     eax,[edi+eax*4] }; \
   __asm { xor     eax,edx }; }
#else /* !IZ_CRCOPTIM_UNFOLDTBL */
# define SavLen  edx            /* the edx register is free for use here */
# define UpdCRC_dword { \
    Do_CRC; \
    Do_CRC; \
    Do_CRC; \
    Do_CRC; }
# define UpdCRC_dword_sh(dwPtrIncr) { \
    Do_CRC; \
    Do_CRC; \
    __asm { add   esi, 4*(dwPtrIncr) }; \
    Do_CRC; \
    Do_CRC; }
#endif /* ?IZ_CRCOPTIM_UNFOLDTBL */

#define Do_CRC_dword { \
  __asm { xor   eax, dword ptr [esi] }; \
  UpdCRC_dword_sh(1); }

#define Do_CRC_4dword { \
  __asm { xor   eax, dword ptr [esi] }; \
  UpdCRC_dword; \
  __asm { xor   eax, dword ptr [esi+4] }; \
  UpdCRC_dword; \
  __asm { xor   eax, dword ptr [esi+8] }; \
  UpdCRC_dword; \
  __asm { xor   eax, dword ptr [esi+12] }; \
  UpdCRC_dword_sh(4); }
#endif /* !NO_32_BIT_LOADS */

/* ========================================================================= */
ulg crc32(crc, buf, len)
    ulg crc;                    /* crc shift register */
    ZCONST uch *buf;            /* pointer to bytes to pump through */
    extent len;                 /* number of bytes in buf[] */
/* Run a set of bytes through the crc shift register.  If buf is a NULL
   pointer, then initialize the crc shift register contents instead.
   Return the current crc in either case. */
{
    __asm {
                push    edx
                push    ecx

                mov     esi,buf         ;/* 2nd arg: uch *buf              */
                sub     eax,eax         ;/*> if (!buf)                     */
                test    esi,esi         ;/*>   return 0;                   */
                jz      fine            ;/*> else {                        */

                call    get_crc_table
                mov     edi,eax
                mov     eax,crc         ;/* 1st arg: ulg crc               */
#ifndef __686
                sub     ebx,ebx         ;/* ebx=0; => bl usable as a dword */
#endif
                mov     ecx,len         ;/* 3rd arg: extent len            */
                not     eax             ;/*>   c = ~crc;                   */

                test    ecx,ecx
#ifndef NO_UNROLLED_LOOPS
                jz      bail
#  ifndef NO_32_BIT_LOADS
align_loop:
                test    esi,3           ;/* align buf pointer on next      */
                jz      aligned_now     ;/*  dword boundary                */
    }
                Do_CRC_byte             ;
    __asm {
                dec     ecx
                jnz     align_loop
aligned_now:
#  endif /* !NO_32_BIT_LOADS */
                mov     SavLen,ecx      ;/* save current len for later  */
                shr     ecx,4           ;/* ecx = len / 16    */
                jz      No_Sixteens
; align loop head at start of 486 internal cache line !!
                align   16
Next_Sixteen:
    }
#  ifndef NO_32_BIT_LOADS
                Do_CRC_4dword ;
#  else /* NO_32_BIT_LOADS */
                Do_CRC_byteof(0) ;
                Do_CRC_byteof(1) ;
                Do_CRC_byteof(2) ;
                Do_CRC_byteof(3) ;
                Do_CRC_byteof(4) ;
                Do_CRC_byteof(5) ;
                Do_CRC_byteof(6) ;
                Do_CRC_byteof(7) ;
                Do_CRC_byteof(8) ;
                Do_CRC_byteof(9) ;
                Do_CRC_byteof(10) ;
                Do_CRC_byteof(11) ;
                Do_CRC_byteof(12) ;
                Do_CRC_byteof(13) ;
                Do_CRC_byteof(14) ;
                Do_CRC_byteof(15) ;
    __asm {     add     esi,16 };
#  endif /* ?NO_32_BIT_LOADS */
    __asm {
                dec     ecx
                jnz     Next_Sixteen
No_Sixteens:
                mov     ecx,SavLen
                and     ecx,00000000FH  ;/* ecx = len % 16    */
#  ifndef NO_32_BIT_LOADS
                shr     ecx,2
                jz      No_Fours
Next_Four:
    }
                Do_CRC_dword ;
    __asm {
                dec     ecx
                jnz     Next_Four
No_Fours:
                mov     ecx,SavLen
                and     ecx,000000003H  ;/* ecx = len % 4    */
#  endif /* !NO_32_BIT_LOADS */
#endif /* !NO_UNROLLED_LOOPS */
                jz      bail            ;/*>  if (len)                       */
; align loop head at start of 486 internal cache line !!
                align   16
loupe:                                  ;/*>    do {                         */
    }
                Do_CRC_byte             ;/*       c = CRC32(c,*buf++,crctab);*/
    __asm {
                dec     ecx             ;/*>    } while (--len);             */
                jnz     loupe

bail:                                   ;/*> }                               */
                not     eax             ;/*> return ~c;                      */
fine:
                pop     ecx
                pop     edx
    }
#ifdef NEED_RETURN
    return _EAX;
#endif
}
#endif /* MSC_INLINE_ASM_32BIT_SUPPORT */
#if (defined(_MSC_VER) && _MSC_VER >= 700)
#if (defined(_M_IX86) && _M_IX86 >= 300)
   /* Reenable missing return value warning */
#  pragma warning( default : 4035 )
#endif
#endif
#endif /* ASM_CRC && !USE_ZLIB && !CRC_TABLE_ONLY */
