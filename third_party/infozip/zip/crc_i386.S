/*
  Copyright (c) 1990-2007 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*
 * crc_i386.S, optimized CRC calculation function for Zip and UnZip,
 * created by Paul Kienitz and Christian Spieler.  Last revised 07 Jan 2007.
 *
 * GRR 961110:  incorporated Scott Field optimizations from win32/crc_i386.asm
 *              => overall 6% speedup in "unzip -tq" on 9MB zipfile (486-66)
 *
 * SPC 970402:  revised for Rodney Brown's optimizations (32-bit-wide
 *              aligned reads for most of the data from buffer), can be
 *              disabled by defining the macro NO_32_BIT_LOADS
 *
 * SPC 971012:  added Rodney Brown's additional tweaks for 32-bit-optimized
 *              CPUs (like the Pentium Pro, Pentium II, and probably some
 *              Pentium clones). This optimization is controlled by the
 *              preprocessor switch "__686" and is disabled by default.
 *              (This default is based on the assumption that most users
 *              do not yet work on a Pentium Pro or Pentium II machine ...)
 *
 * COS 050116:  Enabled the 686 build by default, because there are hardly any
 *              pre-686 CPUs in serious use nowadays. (See SPC 970402 above.)
 *
 * SPC 060103:  Updated code to incorporate newer optimizations found in zlib.
 *
 * SPC 070107:  Added conditional switch to deactivate crc32() compilation.
 *
 * FLAT memory model assumed.  Calling interface:
 *   - args are pushed onto the stack from right to left,
 *   - return value is given in the EAX register,
 *   - all other registers (with exception of EFLAGS) are preserved. (With
 *     GNU C 2.7.x, %edx and %ecx are `scratch' registers, but preserving
 *     them nevertheless adds only 4 single byte instructions.)
 *
 * This source generates the function
 * ulg crc32(ulg crc, ZCONST uch *buf, extent len).
 *
 * Loop unrolling can be disabled by defining the macro NO_UNROLLED_LOOPS.
 * This results in shorter code at the expense of reduced performance.
 */

/* This file is NOT used in conjunction with zlib, or when only creation of
 * the basic CRC_32_Table (for other purpose) is requested.
 */
#if !defined(USE_ZLIB) && !defined(CRC_TABLE_ONLY)

/* Preprocess with -DNO_UNDERLINE if your C compiler does not prefix
 * external symbols with an underline character '_'.
 */
#if defined(NO_UNDERLINE) || defined(__ELF__)
#  define _crc32            crc32
#  define _get_crc_table    get_crc_table
#endif
/* Use 16-byte alignment if your assembler supports it. Warning: gas
 * uses a log(x) parameter (.align 4 means 16-byte alignment). On SVR4
 * the parameter is a number of bytes.
 */
#ifndef ALIGNMENT
#  define ALIGNMENT .align 4,0x90
#endif

#if defined(i386) || defined(_i386) || defined(_I386) || defined(__i386)

/* This version is for 386 Unix, OS/2, MSDOS in 32 bit mode (gcc & gas).
 * Warning: it uses the AT&T syntax: mov source,dest
 * This file is only optional. If you want to use the C version,
 * remove -DASM_CRC from CFLAGS in Makefile and set OBJA to an empty string.
 */

                .file   "crc_i386.S"

#if !defined(PRE_686) && !defined(__686)
   /* Optimize for Pentium Pro and compatible CPUs by default. */
#  define __686
#endif

#if defined(NO_STD_STACKFRAME) && defined(USE_STD_STACKFRAME)
#  undef USE_STACKFRAME
#else
   /* The default is to use standard stack frame entry, because it
    * results in smaller code!
    */
#  ifndef USE_STD_STACKFRAME
#    define USE_STD_STACKFRAME
#  endif
#endif

#ifdef USE_STD_STACKFRAME
#  define _STD_ENTRY    pushl   %ebp ; movl   %esp,%ebp
#  define arg1  8(%ebp)
#  define arg2  12(%ebp)
#  define arg3  16(%ebp)
#  define _STD_LEAVE    popl    %ebp
#else /* !USE_STD_STACKFRAME */
#  define _STD_ENTRY
#  define arg1  24(%esp)
#  define arg2  28(%esp)
#  define arg3  32(%esp)
#  define _STD_LEAVE
#endif /* ?USE_STD_STACKFRAME */

/*
 * These two (three) macros make up the loop body of the CRC32 cruncher.
 * registers modified:
 *   eax  : crc value "c"
 *   esi  : pointer to next data byte (or lword) "buf++"
 * registers read:
 *   edi  : pointer to base of crc_table array
 * scratch registers:
 *   ebx  : index into crc_table array
 *          (requires upper three bytes = 0 when __686 is undefined)
 */
#ifndef __686   /* optimize for 386, 486, Pentium */
#define Do_CRC          /* c = (c >> 8) ^ table[c & 0xFF] */\
                movb    %al, %bl                ;/* tmp = c & 0xFF  */\
                shrl    $8, %eax                ;/* c = (c >> 8)    */\
                xorl    (%edi, %ebx, 4), %eax   ;/* c ^= table[tmp] */
#else   /* __686 : optimize for Pentium Pro and compatible CPUs */
#define Do_CRC          /* c = (c >> 8) ^ table[c & 0xFF] */\
                movzbl  %al, %ebx               ;/* tmp = c & 0xFF  */\
                shrl    $8, %eax                ;/* c = (c >> 8)    */\
                xorl    (%edi, %ebx, 4), %eax   ;/* c ^=table[tmp]  */
#endif  /* ?__686 */

#define Do_CRC_byte             /* c = (c >> 8) ^ table[(c^*buf++)&0xFF] */\
                xorb    (%esi), %al     ;/* c ^= *buf  */\
                incl    %esi            ;/* buf++      */\
                Do_CRC

#define Do_CRC_byteof(ofs)      /* c = (c >> 8) ^ table[(c^*buf++)&0xFF] */\
                xorb    ofs(%esi), %al  ;/* c ^= *buf  */\
                incl    %esi            ;/* buf++      */\
                Do_CRC

#ifndef  NO_32_BIT_LOADS
# ifdef IZ_CRCOPTIM_UNFOLDTBL
   /* the edx register is needed in crc calculation */
#  define SavLen arg3
#  define UpdCRC_lword \
                movzbl  %al, %ebx               ; \
                movl    3072(%edi,%ebx,4), %edx ; \
                movzbl  %ah, %ebx               ; \
                shrl    $16, %eax               ; \
                xor     2048(%edi,%ebx,4), %edx ; \
                movzbl  %al, %ebx               ; \
                shrl    $8,%eax                 ; \
                xorl    1024(%edi,%ebx,4), %edx ; \
                movl    (%edi,%eax,4), %eax     ; \
                xorl    %edx,%eax               ;
#  define UpdCRC_lword_sh(dwPtrIncr) \
                movzbl  %al, %ebx               ; \
                movl    3072(%edi,%ebx,4), %edx ; \
                movzbl  %ah, %ebx               ; \
                shrl    $16, %eax               ; \
                xor     2048(%edi,%ebx,4), %edx ; \
                movzbl  %al, %ebx               ; \
                addl    $4*(dwPtrIncr), %esi    ;/* ((ulg *)buf)+=dwPtrIncr */\
                shrl    $8,%eax                 ; \
                xorl    1024(%edi,%ebx,4), %edx ; \
                movl    (%edi,%eax,4),%eax      ; \
                xorl    %edx,%eax               ;
# else /* !IZ_CRCOPTIM_UNFOLDTBL */
   /* the edx register is not needed anywhere else */
#  define SavLen %edx
#  define UpdCRC_lword \
                Do_CRC \
                Do_CRC \
                Do_CRC \
                Do_CRC
#  define UpdCRC_lword_sh(dwPtrIncr) \
                Do_CRC \
                Do_CRC \
                addl    $4*(dwPtrIncr), %esi    ;/* ((ulg *)buf)++   */\
                Do_CRC \
                Do_CRC
# endif /* ?IZ_CRCOPTIM_UNFOLDTBL */
#define Do_CRC_lword \
                xorl    (%esi), %eax           ;/* c ^= *(ulg *)buf */\
                UpdCRC_lword_sh(1)              /* ... ((ulg *)buf)++ */
#define Do_CRC_4lword \
                xorl    (%esi), %eax           ;/* c ^= *(ulg *)buf */\
                UpdCRC_lword \
                xorl    4(%esi), %eax          ;/* c ^= *((ulg *)buf+1) */\
                UpdCRC_lword \
                xorl    8(%esi), %eax          ;/* c ^= *((ulg *)buf+2) */\
                UpdCRC_lword \
                xorl    12(%esi), %eax         ;/* c ^= *((ulg *)buf]+3 */\
                UpdCRC_lword_sh(4)              /* ... ((ulg *)buf)+=4 */
#endif  /* !NO_32_BIT_LOADS */


                .text

                .globl  _crc32

_crc32:                         /* ulg crc32(ulg crc, uch *buf, extent len) */
                _STD_ENTRY
                pushl   %edi
                pushl   %esi
                pushl   %ebx
                pushl   %edx
                pushl   %ecx

                movl    arg2, %esi           /* 2nd arg: uch *buf            */
                subl    %eax, %eax           /* > if (!buf)                  */
                testl   %esi, %esi           /* >   return 0;                */
                jz      .L_fine              /* > else {                     */
                call    _get_crc_table
                movl    %eax, %edi
                movl    arg1, %eax           /* 1st arg: ulg crc             */
#ifndef __686
                subl    %ebx, %ebx           /* ebx=0; bl usable as dword    */
#endif
                movl    arg3, %ecx           /* 3rd arg: extent len          */
                notl    %eax                 /* >   c = ~crc;                */

                testl   %ecx, %ecx
#ifndef  NO_UNROLLED_LOOPS
                jz      .L_bail
#  ifndef  NO_32_BIT_LOADS
                /* Assert now have positive length */
.L_align_loop:
                testl   $3, %esi        /* Align buf on lword boundary */
                jz      .L_aligned_now
                Do_CRC_byte
                decl    %ecx
                jnz     .L_align_loop
.L_aligned_now:
#  endif  /* !NO_32_BIT_LOADS */
                movl    %ecx, SavLen         /* save current value of len */
                shrl    $4, %ecx             /* ecx = len / 16   */
                jz      .L_No_Sixteens
/*  align loop head at start of 486 internal cache line !! */
                ALIGNMENT
.L_Next_Sixteen:
#  ifndef NO_32_BIT_LOADS
                 Do_CRC_4lword
#  else   /* NO_32_BIT_LOADS */
                 Do_CRC_byteof(0)
                 Do_CRC_byteof(1)
                 Do_CRC_byteof(2)
                 Do_CRC_byteof(3)
                 Do_CRC_byteof(4)
                 Do_CRC_byteof(5)
                 Do_CRC_byteof(6)
                 Do_CRC_byteof(7)
                 Do_CRC_byteof(8)
                 Do_CRC_byteof(9)
                 Do_CRC_byteof(10)
                 Do_CRC_byteof(11)
                 Do_CRC_byteof(12)
                 Do_CRC_byteof(13)
                 Do_CRC_byteof(14)
                 Do_CRC_byteof(15)
                 addl    $16,%esi        ;/* buf += 16 */
#  endif  /* ?NO_32_BIT_LOADS */
                decl    %ecx
                jnz     .L_Next_Sixteen

.L_No_Sixteens:
                movl    SavLen, %ecx
                andl    $15, %ecx         /* ecx = len % 16   */
# ifndef NO_32_BIT_LOADS
                shrl    $2,%ecx           /* ecx = len / 4    */
                jz      .L_No_Fours
.L_Next_Four:
                Do_CRC_lword
                decl    %ecx
                jnz     .L_Next_Four
.L_No_Fours:
                movl    SavLen,%ecx
                andl    $3,%ecx          /* ecx = len % 4 */
# endif /* !NO_32_BIT_LOADS */
#endif /* !NO_UNROLLED_LOOPS */
                jz      .L_bail          /* > if (len)                       */
/* align loop head at start of 486 internal cache line !! */
                ALIGNMENT
.L_loupe:                                /* >   do {                         */
                 Do_CRC_byte             /*       c = CRC32(c,*buf++,crctab);*/
                decl    %ecx             /* >   } while (--len);             */
                jnz     .L_loupe

.L_bail:                                 /* > }                              */
                notl    %eax             /* > return ~c;                     */
.L_fine:
                popl    %ecx
                popl    %edx
                popl    %ebx
                popl    %esi
                popl    %edi
                _STD_LEAVE
                ret

#else
 error: this asm version is for 386 only
#endif /* i386 || _i386 || _I386 || __i386 */

#endif /* !USE_ZLIB && !CRC_TABLE_ONLY */
