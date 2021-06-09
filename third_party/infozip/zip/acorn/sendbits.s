;===========================================================================
; Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.
;
; See the accompanying file LICENSE, version 1999-Oct-05 or later
; (the contents of which are also included in zip.h) for terms of use.
; If, for some reason, both of these files are missing, the Info-ZIP license
; also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
;===========================================================================
; sendbits.s for ARM by Sergio Monesi and Darren Salt.

r0      RN      0
r1      RN      1
r2      RN      2
r3      RN      3
r4      RN      4
r5      RN      5
r6      RN      6
r7      RN      7
r8      RN      8
r9      RN      9
sl      RN      10
fp      RN      11
ip      RN      12
sp      RN      13
lr      RN      14
pc      RN      15

        AREA    |Asm$$Code|, CODE, READONLY

        =       "send_bits",0
        ALIGN
        &       &FF00000C

        IMPORT  __rt_stkovf_split_small
        IMPORT  flush_outbuf

        IMPORT  bi_valid
        IMPORT  bi_buf
        IMPORT  out_size
        IMPORT  out_offset
        IMPORT  out_buf

        EXPORT  send_bits
send_bits
        MOV     ip,sp
        STMDB   sp!,{r4,r5,fp,ip,lr,pc}
        SUB     fp,ip,#4
        LDR     r5,=bi_buf
        LDR     r3,=bi_valid
        LDR     r4,[r5]
        LDR     r2,[r3]
        ORR     r4,r4,r0,LSL r2 ; |= value<<bi_valid
        ADD     r2,r2,r1        ; += length
        CMP     r2,#&10
        STRLE   r2,[r3]         ; short? store & return
        STRLE   r4,[r5]
        LDMLEDB fp,{r4,r5,fp,sp,pc}^

        SUB     r2,r2,#&10      ; adjust bi_valid, bi_buf
        MOV     ip,r4,LSR #16   ;  (done early, keeping the old bi_buf
        STR     r2,[r3]         ;  in R4 for later storage)
        STR     ip,[r5]

        LDR     r0,=out_size
        LDR     r1,=out_offset
        LDR     r0,[r0]
        LDR     r2,[r1]
        SUB     r0,r0,#1
        CMP     r2,r0           ; if out_offset >= out_size-1
        LDRHS   r0,=out_buf
        LDRHS   r0,[r0]
        BLHS    flush_outbuf    ; then flush the buffer
        LDR     r0,=out_buf
        LDR     r1,=out_offset
        LDR     r0,[r0]
        LDR     r2,[r1]
        MOV     r5,r4,LSR #8
        STRB    r4,[r0,r2]!     ; store 'old' bi_buf
        STRB    r5,[r0,#1]
        ADD     r2,r2,#2
        STR     r2,[r1]

        LDMDB   fp,{r4,r5,fp,sp,pc}^


ptr_bi          &       bi_valid
                &       bi_buf


        =       "bi_reverse",0
        ALIGN
        &       &FF00000C

        EXPORT  bi_reverse
bi_reverse
        MOV     r2,#0
loop    MOVS    r0,r0,LSR #1
        ADCS    r2,r2,r2
        SUBS    r1,r1,#1
        BNE     loop
        MOV     r0,r2
        MOVS    pc,lr


        END
