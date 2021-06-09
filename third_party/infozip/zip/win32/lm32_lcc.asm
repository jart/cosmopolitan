;===========================================================================
; Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.
;
; See the accompanying file LICENSE, version 1999-Oct-05 or later
; (the contents of which are also included in zip.h) for terms of use.
; If, for some reason, both of these files are missing, the Info-ZIP license
; also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
;===========================================================================
; match32.asm by Jean-loup Gailly.

; match32.asm, optimized version of longest_match() in deflate.c
; To be used only with 32 bit flat model. To simplify the code, the option
; -DDYN_ALLOC is not supported.
; This file is only optional. If you don't have an assembler, use the
; C version (add -DNO_ASM to CFLAGS in makefile and remove match.o
; from OBJI). If you have reduced WSIZE in zip.h, then make sure this is
; assembled with an equivalent -DWSIZE=<whatever>.
;
; Win32 (Windows NT) version - 1994/04/13 by Steve Salisbury
; * works with Microsoft MASM 6.1X and Microsoft Visual C++ / 32-bit edition
;
; The code in this file has been copied verbatim from match32.{asm|S};
; only the assembler syntax and metacommands have been adapted to
; the habits of the free LCC-Win32 C compiler package.
; IMPORTANT NOTE to the Info-ZIP editors:
; The TAB characters in this source file are required by the parser of
; the LCC-Win32 assembler program and MUST NOT be removed!!
;
;==============================================================================
;
; Do NOT assemble this source if external crc32 routine from zlib gets used.
;


;/* This version is for 386 Unix or OS/2 in 32 bit mode.
; * Warning: it uses the AT&T syntax: mov source,dest
; * This file is only optional. If you want to force the C version,
; * add -DNO_ASM to CFLAGS in Makefile and set OBJA to an empty string.
; * If you have reduced WSIZE in (g)zip.h, then make sure this is
; * assembled with an equivalent -DWSIZE=<whatever>.
; * This version assumes static allocation of the arrays (-DDYN_ALLOC not used).
; */

        .text
        .file	"match.S"


        .text
        .type	_match_init,function

_match_init:
        ret
_$98:
        .size	_match_init,_$98-_match_init
        .globl	_match_init

;/*-----------------------------------------------------------------------
; * Set match_start to the longest match starting at the given string and
; * return its length. Matches shorter or equal to prev_length are discarded,
; * in which case the result is equal to prev_length and match_start is
; * garbage.
; * IN assertions: cur_match is the head of the hash chain for the current
; *   string (strstart) and its distance is <= MAX_DIST, and prev_length >= 1
; */

        .align	4
        .type	_longest_match,function

_longest_match: ;/* int longest_match(cur_match) */

;       cur_match     equ 20(%esp)
;     /* return address */               /* esp+16 */
        push    %ebp
        push    %edi
;/* esp+8  */
        push    %esi
;/* esp+4  */
        push    %ebx
;/* esp    */

;/*
; *      match        equ esi
; *      scan         equ edi
; *      chain_length equ ebp
; *      best_len     equ ebx
; *      limit        equ edx
; */
        mov     20(%esp),%esi
        mov     _strstart,%edx
        mov     _max_chain_length,%ebp
        mov     %edx,%edi
        sub     $(32768-262),%edx
        cld
        jae     limit_ok
        sub     %edx,%edx
limit_ok:
        add     $2+_window,%edi
        mov     _prev_length,%ebx
        movw    -2(%edi),%cx
        movw    -3(%ebx,%edi),%ax
        cmp     _good_match,%ebx
        jb      do_scan
        shr     $2,%ebp
        jmp     do_scan

        .align	4
long_loop:
;/* at this point, edi == scan+2, esi == cur_match */
        movw    -3(%ebx,%edi),%ax
        movw     -2(%edi),%cx
short_loop:
;/*
; * at this point, di == scan+2, si == cur_match,
; * ax = scan[best_len-1..best_len] and cx = scan[0..1]
; */
        and     $(32768-1), %esi
        dec     %ebp
        movw    _prev(,%esi,2),%si
        jz      the_end
        cmp     %edx,%esi
        jbe     the_end
do_scan:
        cmpw    _window-1(%ebx,%esi),%ax
        jne     short_loop
        cmpw    _window(%esi),%cx
        jne     short_loop

        add     $2+_window,%esi
        mov     $((258>>1)-1),%ecx
        mov     %edi,%eax
        repe;   cmpsw
;/* loop until mismatch */
        je      maxmatch
;/* match of length MAX_MATCH? */
mismatch:
        movb    -2(%edi),%cl
        xchg    %edi,%eax
        subb    -2(%esi),%cl
        sub     %edi,%eax
        sub     $2+_window,%esi
        sub     %eax,%esi
        subb    $1,%cl
        adc     $0,%eax
        cmp     %ebx,%eax
        jle     long_loop
        mov     %esi,_match_start
        mov     %eax,%ebx
        cmp     _nice_match,%eax
; /* len >= nice_match ? */
        jl      long_loop
the_end:
        mov     %ebx,%eax
        pop     %ebx
        pop     %esi
        pop     %edi
        pop     %ebp
        ret
        .align	4
maxmatch:
        cmpsb
        jmp     mismatch
_$99:

        .size	_longest_match,_$99-_longest_match
        .globl	_longest_match

        .extern	_nice_match
        .extern	_good_match
        .extern	_max_chain_length
        .extern	_match_start
        .extern	_strstart
        .extern	_prev_length
        .extern	_prev
        .extern	_window
