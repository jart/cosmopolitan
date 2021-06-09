;===========================================================================
; Copyright (c) 1990-2000 Info-ZIP.  All rights reserved.
;
; See the accompanying file LICENSE, version 2000-Apr-09 or later
; (the contents of which are also included in zip.h) for terms of use.
; If, for some reason, all these files are missing, the Info-ZIP license
; also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
;===========================================================================
.text

.globl  _crc32          ; (ulg val, uch *buf, extent bufsize)
.globl  _get_crc_table  ; ulg *get_crc_table(void)

_crc32:
        move.l  8(sp),d0
        bne    valid
        moveq  #0,d0
        rts
valid:  movem.l d2/d3,-(sp)
        jsr     _get_crc_table
        move.l  d0,a0
        move.l  12(sp),d0
        move.l  16(sp),a1
        move.l  20(sp),d1
        not.l   d0

        move.l  d1,d2
        lsr.l   #3,d1
        bra     decr8
loop8:  moveq  #0,d3
        move.b (a1)+,d3
        eor.b  d0,d3
        lsl.w  #2,d3
        move.l 0(a0,d3.w),d3
        lsr.l  #8,d0
        eor.l  d3,d0
        moveq  #0,d3
        move.b (a1)+,d3
        eor.b  d0,d3
        lsl.w  #2,d3
        move.l 0(a0,d3.w),d3
        lsr.l  #8,d0
        eor.l  d3,d0
        moveq  #0,d3
        move.b (a1)+,d3
        eor.b  d0,d3
        lsl.w  #2,d3
        move.l 0(a0,d3.w),d3
        lsr.l  #8,d0
        eor.l  d3,d0
        moveq  #0,d3
        move.b (a1)+,d3
        eor.b  d0,d3
        lsl.w  #2,d3
        move.l 0(a0,d3.w),d3
        lsr.l  #8,d0
        eor.l  d3,d0
        moveq  #0,d3
        move.b (a1)+,d3
        eor.b  d0,d3
        lsl.w  #2,d3
        move.l 0(a0,d3.w),d3
        lsr.l  #8,d0
        eor.l  d3,d0
        moveq  #0,d3
        move.b (a1)+,d3
        eor.b  d0,d3
        lsl.w  #2,d3
        move.l 0(a0,d3.w),d3
        lsr.l  #8,d0
        eor.l  d3,d0
        moveq  #0,d3
        move.b (a1)+,d3
        eor.b  d0,d3
        lsl.w  #2,d3
        move.l 0(a0,d3.w),d3
        lsr.l  #8,d0
        eor.l  d3,d0
        moveq  #0,d3
        move.b (a1)+,d3
        eor.b  d0,d3
        lsl.w  #2,d3
        move.l 0(a0,d3.w),d3
        lsr.l  #8,d0
        eor.l  d3,d0
decr8:  dbra   d1,loop8
        and.w   #7,d2
        bra     decr1
loop1:  moveq  #0,d3
        move.b (a1)+,d3
        eor.b  d0,d3
        lsl.w  #2,d3
        move.l 0(a0,d3.w),d3
        lsr.l  #8,d0
        eor.l  d3,d0
decr1:  dbra   d2,loop1
done:   movem.l (sp)+,d2/d3
        not.l   d0
        rts
