;===========================================================================
; Copyright (c) 1990-2008 Info-ZIP.  All rights reserved.
;
; See the accompanying file LICENSE, version 2007-Mar-04 or later
; (the contents of which are also included in zip.h) for terms of use.
; If, for some reason, all these files are missing, the Info-ZIP license
; also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
;===========================================================================
;
; match.asm by Jean-loup Gailly.

; match.asm, optimized version of longest_match() in deflate.c
; Must be assembled with masm -ml. To be used only with C compact model
; or large model. (For large model, assemble with -D__LARGE__).
; This file is only optional. If you don't have masm or tasm, use the
; C version (add -DNO_ASM to CFLAGS in makefile.msc and remove match.obj
; from OBJI). If you have reduced WSIZE in zip.h, then make sure this is
; assembled with an equivalent -DWSIZE=<whatever>.
;
; The code has been prepared for two different C compiler calling conventions
; and contains some support for dynamically allocated working space.
; The different environments are selected by two conditional flags:
;   DYN_ALLOC  : select support for malloc'ed working space
;   SS_NEQ_DS  : relaxes assumption that stack and default data segments
;                are identical
; When SS_NEQ_DS is defined, the code segment is used to store some
; local variables. This (bad) coding practice is very likely to break any
; `segment protection scheme', it will most probably only work for real
; mode programs.
;
; Turbo C 2.0 does not support static allocation of more than 64K bytes per
; file, and does not have SS == DS. So TC and BC++ users must use:
;   tasm -ml -DDYN_ALLOC -DSS_NEQ_DS match;
;
; To simplify the code, the option -DDYN_ALLOC is supported for OS/2
; only if the arrays are guaranteed to have zero offset (allocated by
; halloc). We also require SS==DS. This is satisfied for MSC but not Turbo C.
;
; Per default, test code is included to check if the above requirements are
; fulfilled. This test code can be disabled by defining the compile time
; option flag NO_SECURE_TESTS when compiling for a production executable.
; This shortens the code size (but the performance gain is neglectable).
; The security tests should remain enabled, when a new C compiler
; and/or a new set of compilation options is tried.

        name    match

; Do NOT assemble this source if external crc32 routine from zlib gets used.
;
ifndef USE_ZLIB

ifdef     DEBUG
  VERBOSE_INFO EQU 1
else
  ifdef _AS_MSG_
    VERBOSE_INFO EQU 1
  else
    VERBOSE_INFO EQU 0
  endif
endif

ifndef __SMALL__
  ifndef __COMPACT__
    ifndef __MEDIUM__
      ifndef __LARGE__
        ifndef __HUGE__
;         __SMALL__ EQU 1
        endif
      endif
    endif
  endif
endif

ifdef __HUGE__
; .MODEL Huge
   ifndef @CodeSize
    @CodeSize  EQU 1
   endif
   ifndef @DataSize
    @DataSize  EQU 1
   endif
   Save_DS    EQU 1
   if VERBOSE_INFO
    if1
      %out Assembling for C, Huge memory model
    endif
   endif
else
   ifdef __LARGE__
;      .MODEL Large
      ifndef @CodeSize
       @CodeSize  EQU 1
      endif
      ifndef @DataSize
       @DataSize  EQU 1
      endif
      if VERBOSE_INFO
       if1
         %out Assembling for C, Large memory model
       endif
      endif
   else
      ifdef __COMPACT__
;         .MODEL Compact
         ifndef @CodeSize
          @CodeSize  EQU 0
         endif
         ifndef @DataSize
          @DataSize  EQU 1
         endif
         if VERBOSE_INFO
          if1
            %out Assembling for C, Compact memory model
          endif
         endif
      else
         ifdef __MEDIUM__
;            .MODEL Medium
            ifndef @CodeSize
             @CodeSize  EQU 1
            endif
            ifndef @DataSize
             @DataSize  EQU 0
            endif
            if VERBOSE_INFO
             if1
               %out Assembling for C, Medium memory model
             endif
            endif
         else
;            .MODEL Small
            ifndef @CodeSize
             @CodeSize  EQU 0
            endif
            ifndef @DataSize
             @DataSize  EQU 0
            endif
            if VERBOSE_INFO
             if1
               %out Assembling for C, Small memory model
             endif
            endif
         endif
      endif
   endif
endif

if @CodeSize
        LCOD_OFS        EQU     2
else
        LCOD_OFS        EQU     0
endif

IF @DataSize
        LDAT_OFS        EQU     2
else
        LDAT_OFS        EQU     0
endif

ifdef Save_DS
;                       (di,si,ds)+(size, return address)
        SAVE_REGS       EQU     6+(4+LCOD_OFS)
else
;                       (di,si)+(size, return address)
        SAVE_REGS       EQU     4+(4+LCOD_OFS)
endif

;
; Selection of the supported CPU instruction set and initialization
; of CPU type related macros:
;
ifdef __586
        Use_286_code    EQU     1
        Align_Size      EQU     16      ; paragraph alignment on Pentium
        Alig_PARA       EQU     1       ; paragraph aligned code segment
else
ifdef __486
        Use_286_code    EQU     1
        Align_Size      EQU     4       ; dword alignment on 32 bit processors
        Alig_PARA       EQU     1       ; paragraph aligned code segment
else
ifdef __386
        Use_286_code    EQU     1
        Align_Size      EQU     4       ; dword alignment on 32 bit processors
        Alig_PARA       EQU     1       ; paragraph aligned code segment
else
ifdef __286
        Use_286_code    EQU     1
        Align_Size      EQU     2       ; word alignment on 16 bit processors
        Alig_PARA       EQU     0       ; word aligned code segment
else
ifdef __186
        Use_186_code    EQU     1
        Align_Size      EQU     2       ; word alignment on 16 bit processors
        Alig_PARA       EQU     0       ; word aligned code segment
else
        Align_Size      EQU     2       ; word alignment on 16 bit processors
        Alig_PARA       EQU     0       ; word aligned code segment
endif   ;?__186
endif   ;?__286
endif   ;?__386
endif   ;?__486
endif   ;?__586

ifdef Use_286_code
        .286
        Have_80x86      EQU     1
else
ifdef Use_186_code
        .186
        Have_80x86      EQU     1
else
        .8086
        Have_80x86      EQU     0
endif   ;?Use_186_code
endif   ;?Use_286_code

ifndef DYN_ALLOC
        extrn   _prev         : word
        extrn   _window       : byte
        prev    equ  _prev    ; offset part
        window  equ  _window
endif

_DATA    segment  word public 'DATA'
        extrn   _nice_match   : word
        extrn   _match_start  : word
        extrn   _prev_length  : word
        extrn   _good_match   : word
        extrn   _strstart     : word
        extrn   _max_chain_length : word
ifdef DYN_ALLOC
        extrn   _prev         : word
        extrn   _window       : word
        prev    equ 0         ; offset forced to zero
        window  equ 0
        window_seg equ _window[2]
        window_off equ 0
else
        wseg    dw seg _window
        window_seg equ wseg
        window_off equ offset _window
endif
_DATA    ends

DGROUP  group _DATA

if @CodeSize
if Alig_PARA
MATCH_TEXT      SEGMENT  PARA PUBLIC 'CODE'
else
MATCH_TEXT      SEGMENT  WORD PUBLIC 'CODE'
endif
        assume  cs: MATCH_TEXT, ds: DGROUP
else    ;!@CodeSize
if Alig_PARA
_TEXT   segment para public 'CODE'
else
_TEXT   segment word public 'CODE'
endif
        assume  cs: _TEXT, ds: DGROUP
endif   ;?@CodeSize

        public  _match_init
        public  _longest_match

ifndef      WSIZE
        WSIZE         equ 32768         ; keep in sync with zip.h !
endif
        MIN_MATCH     equ 3
        MAX_MATCH     equ 258
        MIN_LOOKAHEAD equ (MAX_MATCH+MIN_MATCH+1)
        MAX_DIST      equ (WSIZE-MIN_LOOKAHEAD)

ifdef DYN_ALLOC
  ifdef SS_NEQ_DS
    prev_ptr    dw  seg _prev           ; pointer to the prev array
  endif
else
    prev_ptr    dw  seg _prev           ; pointer to the prev array
endif
ifdef SS_NEQ_DS
    match_start dw  0                   ; copy of _match_start if SS != DS
    nice_match  dw  0                   ; copy of _nice_match  if SS != DS
endif

; initialize or check the variables used in match.asm.

if @CodeSize
_match_init proc far                    ; 'proc far' for large model
else
_match_init proc near                   ; 'proc near' for compact model
endif
ifdef SS_NEQ_DS
        ma_start equ cs:match_start     ; does not work on OS/2
        nice     equ cs:nice_match
else
        assume ss: DGROUP
        ma_start equ ss:_match_start
        nice     equ ss:_nice_match
  ifndef NO_SECURE_TESTS
        mov     ax,ds
        mov     bx,ss
        cmp     ax,bx                   ; SS == DS?
        jne     fatal_err
  endif
endif
ifdef DYN_ALLOC
  ifndef NO_SECURE_TESTS
        cmp     _prev[0],0              ; verify zero offset
        jne     fatal_err
        cmp     _window[0],0
        jne     fatal_err
  endif
  ifdef SS_NEQ_DS
        mov     ax,_prev[2]             ; segment value
        mov     cs:prev_ptr,ax          ; ugly write to code, crash on OS/2
        prev_seg  equ cs:prev_ptr
  else
        prev_seg  equ ss:_prev[2]       ; works on OS/2 if SS == DS
  endif
else
        prev_seg  equ cs:prev_ptr
endif
        ret
ifndef NO_SECURE_TESTS
if @CodeSize
        extrn   _exit : far             ; 'far' for large model
else
        extrn   _exit : near            ; 'near' for compact model
endif
fatal_err:                              ; (quiet) emergency stop:
        call    _exit                   ; incompatible "global vars interface"
endif

_match_init endp

; -----------------------------------------------------------------------
; Set match_start to the longest match starting at the given string and
; return its length. Matches shorter or equal to prev_length are discarded,
; in which case the result is equal to prev_length and match_start is
; garbage.
; IN assertions: cur_match is the head of the hash chain for the current
;   string (strstart) and its distance is <= MAX_DIST, and prev_length >= 1

; int longest_match(cur_match)

        align   Align_Size

if @CodeSize
_longest_match  proc far                 ; 'proc far' for large model
else
_longest_match  proc near                ; 'proc near' for compact model
endif
        push    bp
        mov     bp,sp
        push    di
        push    si
        push    ds

if @CodeSize
        cur_match    equ word ptr [bp+6] ; [bp+6] for large model
else
        cur_match    equ word ptr [bp+4] ; [bp+4] for compact model
endif

;       window       equ es:window (es:0 for DYN_ALLOC)
;       prev         equ ds:prev
;       match        equ es:si
;       scan         equ es:di
;       chain_length equ bp
;       best_len     equ bx
;       limit        equ dx

        mov     si,cur_match            ; use bp before it is destroyed
ifdef SS_NEQ_DS
        mov     ax,_nice_match
        mov     nice,ax                 ; ugly write to code, crash on OS/2
endif
        mov     dx,_strstart
        mov     bp,_max_chain_length    ; chain_length = max_chain_length
        mov     di,dx
        sub     dx,MAX_DIST             ; limit = strstart-MAX_DIST
        cld                             ; string ops increment si and di
        jae     limit_ok
        sub     dx,dx                   ; limit = NIL
limit_ok:
        add     di,2+window_off         ; di = offset(window + strstart + 2)
        mov     bx,_prev_length         ; best_len = prev_length
        mov     es,window_seg
        mov     ax,es:[bx+di-3]         ; ax = scan[best_len-1..best_len]
        mov     cx,es:[di-2]            ; cx = scan[0..1]
        cmp     bx,_good_match          ; do we have a good match already?
        mov     ds,prev_seg             ; (does not destroy the flags)
        assume  ds: nothing
        jb      do_scan                 ; good match?
if Have_80x86
        shr     bp,2                    ; chain_length >>= 2
else
        shr     bp,1                    ; chain_length >>= 2
        shr     bp,1
endif
        jmp     short do_scan

        align   Align_Size              ; align destination of branch
long_loop:
; at this point, ds:di == scan+2, ds:si == cur_match
        mov     ax,[bx+di-3]            ; ax = scan[best_len-1..best_len]
        mov     cx,[di-2]               ; cx = scan[0..1]
        mov     ds,prev_seg             ; reset ds to address the prev array
short_loop:
; at this point, di == scan+2, si = cur_match,
; ax = scan[best_len-1..best_len] and cx = scan[0..1]
if (WSIZE-32768)
        and     si,WSIZE-1              ; not needed if WSIZE=32768
endif
        shl     si,1                    ; cur_match as word index
        dec     bp                      ; --chain_length
        mov     si,prev[si]             ; cur_match = prev[cur_match]
        jz      the_end
        cmp     si,dx                   ; cur_match <= limit ?
        jbe     the_end
do_scan:
        cmp     ax,word ptr es:window[bx+si-1] ; check match at best_len-1
        jne     short_loop
        cmp     cx,word ptr es:window[si]      ; check min_match_length match
        jne     short_loop

        mov     cx,es
        add     si,2+window_off         ; si = match
        mov     ds,cx                   ; ds = es = window
        mov     cx,(MAX_MATCH-2)/2      ; scan for at most MAX_MATCH bytes
        mov     ax,di                   ; ax = scan+2
        repe    cmpsw                   ; loop until mismatch
        je      maxmatch                ; match of length MAX_MATCH?
mismatch:
        mov     cl,[di-2]               ; mismatch on first or second byte?
        xchg    ax,di                   ; di = scan+2, ax = end of scan
        sub     cl,[si-2]               ; cl = 0 if first bytes equal
        sub     ax,di                   ; ax = len
        sub     si,2+window_off         ; si = cur_match + len
        sub     si,ax                   ; si = cur_match
        sub     cl,1                    ; set carry if cl == 0 (can't use DEC)
        adc     ax,0                    ; ax = carry ? len+1 : len
        cmp     ax,bx                   ; len > best_len ?
        jle     long_loop
        mov     ma_start,si             ; match_start = cur_match
        mov     bx,ax                   ; bx = best_len = len
        cmp     ax,nice                 ; len >= nice_match ?
        jl      long_loop
the_end:
        pop     ds
        assume  ds: DGROUP
ifdef SS_NEQ_DS
        mov     ax,ma_start             ; garbage if no match found
        mov     ds:_match_start,ax
endif
        pop     si
        pop     di
        pop     bp
        mov     ax,bx                   ; result = ax = best_len
        ret
maxmatch:                               ; come here if maximum match
        cmpsb                           ; increment si and di
        jmp     mismatch                ; force match_length = MAX_LENGTH

_longest_match  endp

if @CodeSize
MATCH_TEXT      ENDS
else
_TEXT   ENDS
endif
;
endif ;!USE_ZLIB
;
end
