;===========================================================================
; Copyright (c) 1990-2005 Info-ZIP.  All rights reserved.
;
; See the accompanying file LICENSE, version 2005-Feb-10 or later
; (the contents of which are also included in zip.h) for terms of use.
; If, for some reason, all these files are missing, the Info-ZIP license
; also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
;===========================================================================
;
; match32.asm by Jean-loup Gailly.

; match32.asm, optimized version of longest_match() in deflate.c
; To be used only with 32 bit flat model. To simplify the code, the option
; -DDYN_ALLOC is not supported.
; This file is only optional. If you don't have an assembler, use the
; C version (add -DNO_ASM to CFLAGS in makefile and remove match.o
; from OBJI). If you have reduced WSIZE in zip.h, then make sure this is
; assembled with an equivalent -DWSIZE=<whatever>.

; Caution: this module works for IBM's C/C++ compiler versions 2 and 3
; and for Watcom's 32-bit C/C++ compiler. Both pass the first (and only)
; argument for longest_match in the EAX register, not on the stack, with
; the default calling conventions (_System would use the stack).
;
;==============================================================================
;
; Do NOT assemble this source if external crc32 routine from zlib gets used.
;
    IFNDEF USE_ZLIB
;
        .386

        name    match

BSS32   segment  dword USE32 public 'BSS'
        extrn   window       : byte
        extrn   prev         : word
        extrn   prev_length  : dword
        extrn   strstart     : dword
        extrn   match_start  : dword
        extrn   max_chain_length : dword
        extrn   good_match   : dword
        extrn   nice_match   : dword
BSS32   ends

CODE32  segment dword USE32 public 'CODE'
        assume cs:CODE32, ds:FLAT, ss:FLAT

        public  match_init
        public  longest_match

    ifndef      WSIZE
        WSIZE         equ 32768         ; keep in sync with zip.h !
    endif
        MIN_MATCH     equ 3
        MAX_MATCH     equ 258
        MIN_LOOKAHEAD equ (MAX_MATCH+MIN_MATCH+1)
        MAX_DIST      equ (WSIZE-MIN_LOOKAHEAD)

; initialize or check the variables used in match.asm.

match_init proc near
        ret
match_init endp

; -----------------------------------------------------------------------
; Set match_start to the longest match starting at the given string and
; return its length. Matches shorter or equal to prev_length are discarded,
; in which case the result is equal to prev_length and match_start is
; garbage.
; IN assertions: cur_match is the head of the hash chain for the current
;   string (strstart) and its distance is <= MAX_DIST, and prev_length >= 1

; int longest_match(cur_match)

longest_match proc near

        ; return address                ; esp+16
        push    ebp                     ; esp+12
        push    edi                     ; esp+8
        push    esi                     ; esp+4

        lea     ebx,window
        add     ebx,2
        window_off equ dword ptr [esp]
        push    ebx                     ; esp

;       match        equ esi
;       scan         equ edi
;       chain_length equ ebp
;       best_len     equ ebx
;       limit        equ edx

        mov     esi,eax                 ; cur_match
        mov     edx,strstart
        mov     ebp,max_chain_length    ; chain_length = max_chain_length
        mov     edi,edx
        sub     edx,MAX_DIST            ; limit = strstart-MAX_DIST
        cld                             ; string ops increment esi and edi
        jae     short limit_ok
        sub     edx,edx                 ; limit = NIL
limit_ok:
        add     edi,window_off          ; edi = offset(window + strstart + 2)
        mov     ebx,prev_length         ; best_len = prev_length
        mov     cx,[edi-2]              ; cx = scan[0..1]
        mov     ax,[ebx+edi-3]          ; ax = scan[best_len-1..best_len]
        cmp     ebx,good_match          ; do we have a good match already?
        jb      do_scan
        shr     ebp,2                   ; chain_length >>= 2
        jmp     short do_scan

        align   4                       ; align destination of branch
long_loop:
; at this point, edi == scan+2, esi == cur_match
        mov     ax,[ebx+edi-3]          ; ax = scan[best_len-1..best_len]
        mov     cx,[edi-2]              ; cx = scan[0..1]
short_loop:
; at this point, edi == scan+2, esi == cur_match,
; ax = scan[best_len-1..best_len] and cx = scan[0..1]
        and     esi,WSIZE-1             ; not needed if WSIZE=32768
        dec     ebp                     ; --chain_length
        shl     esi,1                   ; cur_match as word index
        mov     si,prev[esi]            ; cur_match = prev[cur_match]
                                        ; top word of esi is still 0
        jz      the_end
        cmp     esi,edx                 ; cur_match <= limit ?
        jbe     short the_end
do_scan:
        cmp     ax,word ptr window[ebx+esi-1]   ; check match at best_len-1
        jne     short_loop
        cmp     cx,word ptr window[esi]         ; check min_match_length match
        jne     short_loop

        add     esi,window_off          ; esi = match
        mov     ecx,(MAX_MATCH-2)/2     ; scan for at most MAX_MATCH bytes
        mov     eax,edi                 ; eax = scan+2
        repe    cmpsw                   ; loop until mismatch
        je      maxmatch                ; match of length MAX_MATCH?
mismatch:
        mov     cl,[edi-2]              ; mismatch on first or second byte?
        xchg    eax,edi                 ; edi = scan+2, eax = end of scan
        sub     cl,[esi-2]              ; cl = 0 if first bytes equal
        sub     eax,edi                 ; eax = len
        sub     esi,window_off          ; esi = match - (2 + offset(window))
        sub     esi,eax                 ; esi = cur_match (= match - len)
        sub     cl,1                    ; set carry if cl == 0 (can't use DEC)
        adc     eax,0                   ; eax = carry ? len+1 : len
        cmp     eax,ebx                 ; len > best_len ?
        jle     long_loop
        mov     match_start,esi         ; match_start = cur_match
        mov     ebx,eax                 ; ebx = best_len = len
    ifdef FULL_SEARCH
        cmp     eax,MAX_MATCH           ; len >= MAX_MATCH ?
    else
        cmp     eax,nice_match          ; len >= nice_match ?
    endif
        jl      long_loop
the_end:
        mov     eax,ebx                 ; result = eax = best_len
        pop     ebx
        pop     esi
        pop     edi
        pop     ebp
        ret
maxmatch:                               ; come here if maximum match
        cmpsb                           ; increment esi and edi
        jmp     mismatch                ; force match_length = MAX_LENGTH

longest_match endp

CODE32  ends
;
    ENDIF ; !USE_ZLIB
;
        end
