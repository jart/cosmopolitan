;===========================================================================
; Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.
;
; See the accompanying file LICENSE, version 1999-Oct-05 or later
; (the contents of which are also included in zip.h) for terms of use.
; If, for some reason, both of these files are missing, the Info-ZIP license
; also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
;===========================================================================
; match.s for ARM by Sergio Monesi.

r0 RN 0
r1 RN 1
r2 RN 2
r3 RN 3
r4 RN 4
r5 RN 5
r6 RN 6
r7 RN 7
r8 RN 8
r9 RN 9
sl RN 10
fp RN 11
ip RN 12
sp RN 13
lr RN 14
pc RN 15

MAX_DIST   EQU  32506
WMASK      EQU  32767
MAX_MATCH  EQU  258

                AREA    |C$$code|, CODE, READONLY


; r1 = chain_lenght
; r2 = scan
; r3 = match
; r4 = len (tmp)
; r5 = best_len
; r6 = limit
; r7 = strend
; r8 = scan_end1
; r9 = scan_end
; lr = window
; fp = prev

|__max_chain_length|
        IMPORT  max_chain_length
        DCD     max_chain_length
|__window|
        IMPORT  window
        DCD     window
|__prev|
        IMPORT  prev
        DCD     prev
|__prev_length|
        IMPORT  prev_length
        DCD     prev_length
|__strstart|
        IMPORT  strstart
        DCD     strstart
|__good_match|
        IMPORT  good_match
        DCD     good_match
|__nice_match|
        IMPORT  nice_match
        DCD     nice_match
|__match_start|
        IMPORT  match_start
        DCD     match_start

        DCB     "longest_match"
        DCB     &00,&00,&00
        DCD     &ff000010

        EXPORT  longest_match
longest_match
        STMFD   sp!, {r4-r9,fp,lr}

        LDR     fp, [pc, #|__prev|-.-8]

        LDR     r1, [pc, #|__max_chain_length|-.-8]
        LDR     r1, [r1]
        LDR     lr, [pc, #|__window|-.-8]

        LDR     ip, [pc, #|__strstart|-.-8]
        LDR     ip, [ip]
        ADD     r2, lr, ip
        LDR     r5, [pc, #|__prev_length|-.-8]
        LDR     r5, [r5]
        SUBS    ip, ip, #MAX_DIST-250       ; if r6 > MAX_DIST
        SUBCSS  r6, ip, #250                ; r6 = r6 - MAXDIST
        MOVLS   r6, #0                      ; else r6 = 0

        ADD     r7, r2, #MAX_MATCH-256
        ADD     r7, r7, #256                ; r7 = r2 + MAX_MATCH (=258);

        SUB     ip, r5, #1
        LDRB    r8, [r2, ip]
        LDRB    r9, [r2, r5]

        LDR     ip, [pc, #|__good_match|-.-8]
        LDR     ip, [ip]
        CMP     r5, ip
        MOVCS   r1, r1, LSR #2

cycle
        ADD     r3, lr, r0

        LDRB    ip, [r3, r5]
        CMP     ip, r9
        BNE     cycle_end

        SUB     ip, r5, #1
        LDRB    ip, [r3, ip]
        CMP     ip, r8
        BNE     cycle_end

        LDRB    ip, [r2]
        LDRB    r4, [r3]
        CMP     ip, r4
        BNE     cycle_end

        LDRB    ip, [r3, #1]
        LDRB    r4, [r2, #1]
        CMP     ip, r4
        BNE     cycle_end

        ADD     r2, r2, #2
        ADD     r3, r3, #2

inn_cycle
        LDRB    ip, [r2, #1]!
        LDRB    r4, [r3, #1]!
        CMP     ip, r4
        BNE     exit_inn_cycle

        LDRB    ip, [r2, #1]!
        LDRB    r4, [r3, #1]!
        CMP     ip, r4
        BNE     exit_inn_cycle

        LDRB    ip, [r2, #1]!
        LDRB    r4, [r3, #1]!
        CMP     ip, r4
        BNE     exit_inn_cycle

        LDRB    ip, [r2, #1]!
        LDRB    r4, [r3, #1]!
        CMP     ip, r4
        BNE     exit_inn_cycle

        LDRB    ip, [r2, #1]!
        LDRB    r4, [r3, #1]!
        CMP     ip, r4
        BNE     exit_inn_cycle

        LDRB    ip, [r2, #1]!
        LDRB    r4, [r3, #1]!
        CMP     ip, r4
        BNE     exit_inn_cycle

        LDRB    ip, [r2, #1]!
        LDRB    r4, [r3, #1]!
        CMP     ip, r4
        BNE     exit_inn_cycle

        LDRB    ip, [r2, #1]!
        LDRB    r4, [r3, #1]!
        CMP     ip, r4
        BNE     exit_inn_cycle

        CMP     r2, r7
        BCC     inn_cycle

exit_inn_cycle
        SUB     r4, r2, r7               ; len = MAX_MATCH - (int)(strend - scan);
        ADD     r4, r4, #MAX_MATCH-256
        ADD     r4, r4, #256

        SUB     r2, r2, r4               ; scan = strend - MAX_MATCH

        CMP     r4, r5                   ; if (len > best_len) {
        BLE     cycle_end

        LDR     ip, [pc, #|__match_start|-.-8]  ; match_start = cur_match;
        STR     r0, [ip]
        MOV     r5, r4                          ; best_len = len;

        LDR     ip, [pc, #|__nice_match|-.-8]   ; if (len >= nice_match)
        LDR     ip, [ip]
        CMP     r4, ip
        BGE     exit_match                      ;   break;

        SUB     ip, r5, #1                      ; scan_end1  = scan[best_len-1];
        LDRB    r8, [r2, ip]
        LDRB    r9, [r2, r5]                    ; scan_end   = scan[best_len];

cycle_end
        MOV     ip, r0, LSL #17          ; cur_match & WMASK
        MOV     ip, ip, LSR #17

        LDR     r0, [fp, ip, ASL #1]     ; cur_match = prev[cur_match & WMASK]
        MOV     r0, r0, ASL #16
        MOV     r0, r0, LSR #16

        CMP     r0, r6                  ; cur_match > limit
        BLS     exit_match
        SUBS    r1, r1, #1              ; --chain_length
        BNE     cycle                   ; chain_length != 0

exit_match
        MOV     r0, r5

        LDMFD   sp!, {r4-r9,fp,pc}^

        END
