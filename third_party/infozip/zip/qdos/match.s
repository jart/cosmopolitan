;===========================================================================
; Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.
;
; See the accompanying file LICENSE, version 1999-Oct-05 or later
; (the contents of which are also included in zip.h) for terms of use.
; If, for some reason, both of these files are missing, the Info-ZIP license
; also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
;===========================================================================
; match.a -- optional optimized asm version of longest match in deflate.c
; Written by Jean-loup Gailly
;
; Adapted for the Amiga by Carsten Steger <stegerc@informatik.tu-muenchen.de>
; using the code in match.S.
; The major change in this code consists of removing all unaligned
; word accesses, because they cause 68000-based Amigas to crash.
; For maximum speed, UNALIGNED_OK can be defined in Makefile.sasc.
; The program will then only run on 68020-based Amigas, though.
;
; This code will run with registerized parameters too, unless SAS
; changes parameter passing conventions between new releases of SAS/C.


;;Cur_Match      equr     d0      ; Must be in d0!
;;Best_Len       equr     d1
;;Loop_Counter   equr     d2
;;Scan_Start     equr     d3
;;Scan_End       equr     d4
;;Limit          equr     d5
;;Chain_Length   equr     d6
;;Scan_Test      equr     d7
;;Scan           equr     a0
;;Match          equr     a1
;;Prev_Address   equr     a2
;;Scan_Ini       equr     a3
;;Match_Ini      equr     a4

MAX_MATCH       equ     258
MIN_MATCH       equ     3
WSIZE           equ     32768
MAX_DIST        equ     WSIZE-MAX_MATCH-MIN_MATCH-1


        .globl    _max_chain_length
        .globl    _prev_length
        .globl    _prev
        .globl    _window
        .globl    _strstart
        .globl    _good_match
        .globl    _match_start
        .globl    _nice_match

        .text
        .globl   _match_init
        .globl   _longest_match

_match_init:
        rts


_longest_match:
        move.l  4(sp),d0
        movem.l d2-d7/a2-a4,-(sp)
        move.l  _max_chain_length,d6
        move.l  _prev_length,d1
        lea     _prev,a2
        lea     _window+MIN_MATCH,a4
        move.l  _strstart,d5
        move.l  a4,a3
        add.l   d5,a3
        subi.w  #MAX_DIST,d5
        bhi     limit_ok
        moveq   #0,d5
limit_ok:
        cmp.l   _good_match,d1
        bcs     length_ok
        lsr.l   #2,d6
length_ok:
        subq.l  #1,d6

        move.b  -MIN_MATCH(a3),d3
        lsl.w   #8,d3
        move.b  -MIN_MATCH+1(a3),d3
        move.b  -MIN_MATCH-1(a3,d1),d4
        lsl.w   #8,d4
        move.b  -MIN_MATCH(a3,d1),d4

        bra     do_scan

long_loop:

        move.b  -MIN_MATCH-1(a3,d1),d4
        lsl.w   #8,d4
        move.b  -MIN_MATCH(a3,d1),d4

short_loop:
        lsl.w   #1,d0
        move.w  0(a2,d0.l),d0
        cmp.w   d5,d0
        dbls    d6,do_scan
        bra     return

do_scan:
        move.l  a4,a1
        add.l   d0,a1

        move.b  -MIN_MATCH-1(a1,d1),d7
        lsl.w   #8,d7
        move.b  -MIN_MATCH(a1,d1),d7
        cmp.w   d7,d4
        bne     short_loop
        move.b  -MIN_MATCH(a1),d7
        lsl.w   #8,d7
        move.b  -MIN_MATCH+1(a1),d7
        cmp.w   d7,d3
        bne     short_loop

        move.w  #(MAX_MATCH-MIN_MATCH),d2
        move.l  a3,a0

scan_loop:
        cmpm.b  (a1)+,(a0)+
        dbne    d2,scan_loop

        sub.l   a3,a0
        addq.l  #(MIN_MATCH-1),a0
        cmp.l   d1,a0
        bls     short_loop
        move.l  a0,d1
        move.l  d0,_match_start
        cmp.l   _nice_match,d1
        bcs     long_loop
return:
        move.l  d1,d0
        movem.l (sp)+,d2-d7/a2-a4
        rts
        end


