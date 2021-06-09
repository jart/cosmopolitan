;===========================================================================
; Copyright (c) 1990-2000 Info-ZIP.  All rights reserved.
;
; See the accompanying file LICENSE, version 2000-Apr-09 or later
; (the contents of which are also included in zip.h) for terms of use.
; If, for some reason, all these files are missing, the Info-ZIP license
; also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
;===========================================================================
; crc_68 created by Paul Kienitz, last modified 04 Jan 96.
;
; Return an updated 32 bit CRC value, given the old value and a block of data.
; The CRC table used to compute the value is gotten by calling get_crc_table().
; This replaces the older updcrc() function used in Zip and fUnZip.  The
; prototype of the function is:
;
;    ulg crc32(ulg crcval, uch *text, extent textlen);
;
; On the Amiga, type extent is always unsigned long, not unsigned int, because
; int can be short or long at whim, but size_t is long.
;
; If using this source on a non-Amiga 680x0 system, note that we treat
; a0/a1/d0/d1 as scratch registers not preserved across function calls.
; We do not bother to support registerized arguments for crc32() -- the
; textlen parm is usually large enough so that savings outside the loop
; are pointless.
;
; Define NO_UNROLLED_LOOPS to use a simple short loop which might be more
; efficient on certain machines with dinky instruction caches ('020?), or for
; processing short strings.  If loops are unrolled, the textlen parm must be
; less than 512K; if not unrolled, it must be less than 64K.
;
; 1999/09/23: for Human68k: Modified by Shimazaki Ryo.

        xdef    _crc32          ; (ulg val, uch *buf, extent bufsize)

DO_CRC0 MACRO
        moveq  #0,ltemp
        move.b (textbuf)+,ltemp
        eor.b  crcval,ltemp
        lsl.w  #2,ltemp
        move.l (crc_table,ltemp.w),ltemp
        lsr.l  #8,crcval
        eor.l  ltemp,crcval
        ENDM


DO_CRC2 MACRO
        move.b (textbuf)+,btemp
        eor.b  crcval,btemp
        lsr.l  #8,crcval
        move.l (crc_table,btemp.w*4),ltemp
        eor.l  ltemp,crcval
        ENDM

crc_table       reg     a0      array of unsigned long
crcval          reg     d0      unsigned long initial value
textbuf         reg     a1      array of unsigned char
textbufsize     reg     d1      unsigned long (count of bytes in textbuf)
btemp           reg     d2
ltemp           reg     d3


        xref    _get_crc_table  ; ulg *get_crc_table(void)



        quad
_crc32:
        move.l  8(sp),d0
        bne.s   valid
;;;;;   moveq  #0,d0
         rts
valid:  movem.l btemp/ltemp,-(sp)
        jsr     _get_crc_table
        movea.l d0,crc_table
        move.l  12(sp),crcval
        move.l  16(sp),textbuf
        move.l  20(sp),textbufsize
        not.l   crcval

    ifdef   NO_UNROLLED_LOOPS

    if CPU==68000
        bra.s   decr
loop:    DO_CRC0
decr:    dbra   textbufsize,loop
        bra.s   done

    else
twenty: moveq   #0,btemp
        bra.s   decr2
loop2:   DO_CRC2
decr2:   dbra   textbufsize,loop2
    endif

    ELSE    ; !NO_UNROLLED_LOOPS

    if CPU==68000
        moveq   #7,btemp
        and     textbufsize,btemp
        lsr.l   #3,textbufsize
        bra     decr8
loop8:   DO_CRC0
         DO_CRC0
         DO_CRC0
         DO_CRC0
         DO_CRC0
         DO_CRC0
         DO_CRC0
         DO_CRC0
decr8:   dbra   textbufsize,loop8
        bra.s   decr1
loop1:   DO_CRC0
decr1:   dbra   btemp,loop1
        bra     done

    else
twenty: moveq   #0,btemp
        move.l  textbufsize,-(sp)
        lsr.l   #3,textbufsize
        bra     decr82
         quad
loop82:  DO_CRC2
         DO_CRC2
         DO_CRC2
         DO_CRC2
         DO_CRC2
         DO_CRC2
         DO_CRC2
         DO_CRC2
decr82:  dbra   textbufsize,loop82
        moveq   #7,textbufsize
        and.l   (sp)+,textbufsize
        bra.s   decr12
loop12:  DO_CRC2
decr12:  dbra   textbufsize,loop12
    endif

    ENDC    ; ?NO_UNROLLED_LOOPS

done:   movem.l (sp)+,btemp/ltemp
        not.l   crcval
;;;;;   move.l  crcval,d0               ; crcval already is d0
        rts
