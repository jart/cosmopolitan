;===========================================================================
; Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.
;
; See the accompanying file LICENSE, version 1999-Oct-05 or later
; (the contents of which are also included in zip.h) for terms of use.
; If, for some reason, both of these files are missing, the Info-ZIP license
; also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
;===========================================================================
; SWI veneers used by Zip/Unzip
;

r0              RN      0
r1              RN      1
r2              RN      2
r3              RN      3
r4              RN      4
r5              RN      5
r6              RN      6
r7              RN      7
r8              RN      8
r9              RN      9
r10             RN      10
r11             RN      11
r12             RN      12
sp              RN      13
lr              RN      14
pc              RN      15

sl              RN      10
fp              RN      11
ip              RN      12


XOS_Bit                         EQU &020000

OS_GBPB                         EQU &00000C
OS_File                         EQU &000008
OS_FSControl                    EQU &000029
OS_CLI                          EQU &000005
OS_ReadC                        EQU &000004
OS_ReadVarVal                   EQU &000023
DDEUtils_Prefix                 EQU &042580
Territory_ReadCurrentTimeZone   EQU &043048

        MACRO
        STARTCODE $name
        EXPORT $name
$name
        MEND


                AREA    |C$$code|, CODE, READONLY

; os_error *SWI_OS_FSControl_26(char *source, char *dest, int actionmask);

        STARTCODE SWI_OS_FSControl_26

        MOV     ip, lr

        MOV     r3, r2
        MOV     r2, r1
        MOV     r1, r0
        MOV     r0, #26

        SWI     OS_FSControl + XOS_Bit

        MOVVC   r0, #0

        MOVS    pc, ip


; os_error *SWI_OS_FSControl_27(char *filename, int actionmask);

        STARTCODE SWI_OS_FSControl_27

        MOV     ip, lr

        MOV     r3, r1
        MOV     r1, r0
        MOV     r0, #27

        SWI     OS_FSControl + XOS_Bit

        MOVVC   r0, #0

        MOVS    pc, ip


; os_error *SWI_OS_GBPB_9(char *dirname, void *buf, int *number,
;                         int *offset, int size, char *match);

        STARTCODE SWI_OS_GBPB_9

        MOV     ip, sp
        STMFD   sp!, {r2-r6,lr}
        LDMIA   ip, {r5,r6}
        LDR     r4, [r3]
        LDR     r3, [r2]
        MOV     r2, r1
        MOV     r1, r0
        MOV     r0, #9
        SWI     OS_GBPB + XOS_Bit
        LDMVSFD sp!, {r2-r6,pc}^
        MOV     r0, #0
        LDMFD   sp, {r5,r6}
        STR     r3, [r5]
        STR     r4, [r6]
        LDMFD   sp!, {r2-r6,pc}^


; os_error *SWI_OS_File_1(char *filename, int loadaddr, int execaddr, int attrib);

        STARTCODE SWI_OS_File_1

        STMFD   sp!, {r5,lr}
        MOV     r5, r3
        MOV     r3, r2
        MOV     r2, r1
        MOV     r1, r0
        MOV     r0, #1
        SWI     OS_File + XOS_Bit
        MOVVC   r0, #0
        LDMFD   sp!, {r5,pc}^



; os_error *SWI_OS_File_5(char *filename, int *objtype, int *loadaddr,
;                         int *execaddr, int *length, int *attrib);

        STARTCODE SWI_OS_File_5

        STMFD   sp!, {r1-r5,lr}
        MOV     r1, r0
        MOV     r0, #5
        SWI     OS_File + XOS_Bit
        LDMVSFD sp!, {r1-r5,pc}^
        LDR     lr, [sp]
        TEQ     lr, #0
        STRNE   r0, [lr]
        LDR     lr, [sp, #4]
        TEQ     lr ,#0
        STRNE   r2, [lr]
        LDR     lr, [sp, #8]
        TEQ     lr, #0
        STRNE   r3, [lr]
        LDR     lr, [sp ,#24]
        TEQ     lr, #0
        STRNE   r4, [lr]
        LDR     lr, [sp ,#28]
        TEQ     lr, #0
        STRNE   r5, [lr]
        MOV     r0, #0
        LDMFD   sp!, {r1-r5,pc}^


; os_error *SWI_OS_File_6(char *filename);

        STARTCODE SWI_OS_File_6

        STMFD   sp!, {r4-r5,lr}
        MOV     r1, r0
        MOV     r0, #6
        SWI     OS_File + XOS_Bit
        MOVVC   r0, #0
        LDMFD   sp!, {r4-r5,pc}^


; os_error *SWI_OS_File_7(char *filename, int loadaddr, int execaddr, int size);

        STARTCODE SWI_OS_File_7

        STMFD   sp!, {r4-r5,lr}
        MOV     r5, r3
        MOV     r4, #0
        MOV     r3, r2
        MOV     r2, r1
        MOV     r1, r0
        MOV     r0, #7
        SWI     OS_File + XOS_Bit
        MOVVC   r0, #0
        LDMFD   sp!, {r4-r5,pc}^


; os_error *SWI_OS_CLI(char *cmd);

        STARTCODE SWI_OS_CLI

        MOV     ip, lr
        SWI     OS_CLI + XOS_Bit
        MOVVC   r0, #0
        MOVS    pc, ip


; int SWI_OS_ReadC(void);

        STARTCODE SWI_OS_ReadC

        MOV     ip, lr
        SWI     OS_ReadC + XOS_Bit
        MOVS    pc, ip


; os_error *SWI_OS_ReadVarVal(char *var, char *buf, int len, int *bytesused);

        STARTCODE SWI_OS_ReadVarVal

        STMFD   sp!, {r4,lr}
        MOV     ip, r3
        MOV     r3, #0
        MOV     r4, #0
        SWI     OS_ReadVarVal + XOS_Bit
        LDMVSFD sp!, {r4,pc}^
        TEQ     ip, #0
        STRNE   r2, [ip]
        MOV     r0, #0
        LDMFD   sp!, {r4,pc}^


; os_error *SWI_OS_FSControl_54(char *buffer, int dir, char *fsname, int *size);

        STARTCODE SWI_OS_FSControl_54

        STMFD   sp!, {r3-r6,lr}
        LDR     r5, [r3]
        MOV     r3, r2
        MOV     r2, r1
        MOV     r1, r0
        MOV     r0, #54
        SWI     OS_FSControl + XOS_Bit
        LDMVSFD sp!, {r3-r6,pc}^
        MOV     r0, #0
        LDMFD   sp!, {r3}
        STR     r5, [r3]
        LDMFD   sp!, {r4-r6,pc}^


; os_error *SWI_OS_FSControl_37(char *pathname, char *buffer, int *size);

        STARTCODE SWI_OS_FSControl_37

        STMFD   sp!, {r2,r3-r5,lr}
        LDR     r5, [r2]
        MOV     r3, #0
        MOV     r4, #0
        MOV     r2, r1
        MOV     r1, r0
        MOV     r0, #37
        SWI     OS_FSControl + XOS_Bit
        LDMVSFD sp!, {r2,r3-r5,pc}^
        MOV     r0, #0
        LDMFD   sp!, {r2}
        STR     r5, [r2]
        LDMFD   sp!, {r3-r5,pc}^


; os_error *SWI_DDEUtils_Prefix(char *dir);

        STARTCODE SWI_DDEUtils_Prefix

        MOV     ip, lr
        SWI     DDEUtils_Prefix + XOS_Bit
        MOVVC   r0, #0
        MOVS    pc, ip

; int SWI_Read_Timezone(void);

        STARTCODE SWI_Read_Timezone

        MOV     ip, lr
        SWI     Territory_ReadCurrentTimeZone + XOS_Bit
        MOVVC   r0, r1
        MOVVS   r0, #0
        MOVS    pc, ip


        END
