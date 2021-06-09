;===========================================================================
; Copyright (c) 1990-2007 Info-ZIP.  All rights reserved.
;
; See the accompanying file LICENSE, version 2000-Apr-09 or later
; (the contents of which are also included in zip.h) for terms of use.
; If, for some reason, all these files are missing, the Info-ZIP license
; also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
;===========================================================================
; crc_i386.asm, optimized CRC calculation function for Zip and UnZip,
; created by Paul Kienitz and Christian Spieler.  Last revised 07 Jan 2007.
;
; Revised 06-Oct-96, Scott Field (sfield@microsoft.com)
;   fixed to assemble with masm by not using .model directive which makes
;   assumptions about segment alignment.  Also,
;   avoid using loop, and j[e]cxz where possible.  Use mov + inc, rather
;   than lodsb, and other misc. changes resulting in the following performance
;   increases:
;
;      unrolled loops                NO_UNROLLED_LOOPS
;      *8    >8      <8              *8      >8      <8
;
;      +54%  +42%    +35%            +82%    +52%    +25%
;
;   first item in each table is input buffer length, even multiple of 8
;   second item in each table is input buffer length, > 8
;   third item in each table is input buffer length, < 8
;
; Revised 02-Apr-97, Chr. Spieler, based on Rodney Brown (rdb@cmutual.com.au)
;   Incorporated Rodney Brown's 32-bit-reads optimization as found in the
;   UNIX AS source crc_i386.S. This new code can be disabled by defining
;   the macro symbol NO_32_BIT_LOADS.
;
; Revised 12-Oct-97, Chr. Spieler, based on Rodney Brown (rdb@cmutual.com.au)
;   Incorporated Rodney Brown's additional tweaks for 32-bit-optimized CPUs
;   (like the Pentium Pro, Pentium II, and probably some Pentium clones).
;   This optimization is controlled by the macro symbol __686 and is disabled
;   by default. (This default is based on the assumption that most users
;   do not yet work on a Pentium Pro or Pentium II machine ...)
;
; Revised 25-Mar-98, Cosmin Truta (cosmint@cs.ubbcluj.ro)
;   Working without .model directive caused tasm32 version 5.0 to produce
;   bad object code. The optimized alignments can be optionally disabled
;   by defining NO_ALIGN, thus allowing to use .model flat. There is no need
;   to define this macro if using other versions of tasm.
;
; Revised 16-Jan-2005, Cosmin Truta (cosmint@cs.ubbcluj.ro)
;   Enabled the 686 build by default, because there are hardly any pre-686 CPUs
;   in serious use nowadays. (See the 12-Oct-97 note above.)
;
; Revised 03-Jan-2006, Chr. Spieler
;   Enlarged unrolling loops to "do 16 bytes per turn"; optimized access to
;   data buffer in loop body (adjust pointer only once in loop body and use
;   offsets to access each item); added additional support for the "unfolded
;   tables" optimization variant (enabled by IZ_CRCOPTIM_UNFOLDTBL).
;
; Revised 07-Jan-2007, Chr. Spieler
;   Recognize additional conditional flag CRC_TABLE_ONLY that prevents
;   compilation of the crc32() function.
;
; FLAT memory model assumed.
;
; Loop unrolling can be disabled by defining the macro NO_UNROLLED_LOOPS.
; This results in shorter code at the expense of reduced performance.
;
;==============================================================================
;
; Do NOT assemble this source if external crc32 routine from zlib gets used,
; or only the precomputed CRC_32_Table is needed.
;
    IFNDEF USE_ZLIB
    IFNDEF CRC_TABLE_ONLY
;
        .386p
        name    crc_i386

    IFDEF NO_ALIGN
        .model flat
    ENDIF

    IFNDEF PRE_686
    IFNDEF __686
__686   EQU     1 ; optimize for Pentium Pro, Pentium II and compatible CPUs
    ENDIF
    ENDIF

extrn   _get_crc_table:near    ; ZCONST ulg near *get_crc_table(void);

;
    IFNDEF NO_STD_STACKFRAME
        ; Use a `standard' stack frame setup on routine entry and exit.
        ; Actually, this option is set as default, because it results
        ; in smaller code !!
STD_ENTRY       MACRO
                push    ebp
                mov     ebp,esp
        ENDM

        Arg1    EQU     08H[ebp]
        Arg2    EQU     0CH[ebp]
        Arg3    EQU     10H[ebp]

STD_LEAVE       MACRO
                pop     ebp
        ENDM

    ELSE  ; NO_STD_STACKFRAME

STD_ENTRY       MACRO
        ENDM

        Arg1    EQU     18H[esp]
        Arg2    EQU     1CH[esp]
        Arg3    EQU     20H[esp]

STD_LEAVE       MACRO
        ENDM

    ENDIF ; ?NO_STD_STACKFRAME

; These two (three) macros make up the loop body of the CRC32 cruncher.
; registers modified:
;   eax  : crc value "c"
;   esi  : pointer to next data byte (or dword) "buf++"
; registers read:
;   edi  : pointer to base of crc_table array
; scratch registers:
;   ebx  : index into crc_table array
;          (requires upper three bytes = 0 when __686 is undefined)
    IFNDEF  __686 ; optimize for 386, 486, Pentium
Do_CRC  MACRO
                mov     bl,al                ; tmp = c & 0xFF
                shr     eax,8                ; c = (c >> 8)
                xor     eax,[edi+ebx*4]      ;  ^ table[tmp]
        ENDM
    ELSE ; __686 : optimize for Pentium Pro, Pentium II and compatible CPUs
Do_CRC  MACRO
                movzx   ebx,al                 ; tmp = c & 0xFF
                shr     eax,8                  ; c = (c >> 8)
                xor     eax,[edi+ebx*4]        ;  ^ table[tmp]
        ENDM
    ENDIF ; ?__686
Do_CRC_byte     MACRO
                xor     al, byte ptr [esi]     ; c ^= *buf
                inc     esi                    ; buf++
                Do_CRC                         ; c = (c >> 8) ^ table[c & 0xFF]
        ENDM
Do_CRC_byteof   MACRO   ofs
                xor     al, byte ptr [esi+ofs] ; c ^= *(buf+ofs)
                Do_CRC                         ; c = (c >> 8) ^ table[c & 0xFF]
        ENDM
    IFNDEF  NO_32_BIT_LOADS
      IFDEF IZ_CRCOPTIM_UNFOLDTBL
        ; the edx register is needed in crc calculation
        SavLen  EQU     Arg3

UpdCRC_dword    MACRO
                movzx   ebx,al                 ; tmp = c & 0xFF
                mov     edx,[edi+ebx*4+3072]   ;  table[256*3+tmp]
                movzx   ebx,ah                 ; tmp = (c>>8) & 0xFF
                shr     eax,16                 ;
                xor     edx,[edi+ebx*4+2048]   ;  ^ table[256*2+tmp]
                movzx   ebx,al                 ; tmp = (c>>16) & 0xFF
                shr     eax,8                  ; tmp = (c>>24)
                xor     edx,[edi+ebx*4+1024]   ;  ^ table[256*1+tmp]
                mov     eax,[edi+eax*4]        ;  ^ table[256*0+tmp]
                xor     eax,edx                ; ..
        ENDM
UpdCRC_dword_sh MACRO   dwPtrIncr
                movzx   ebx,al                 ; tmp = c & 0xFF
                mov     edx,[edi+ebx*4+3072]   ;  table[256*3+tmp]
                movzx   ebx,ah                 ; tmp = (c>>8) & 0xFF
                xor     edx,[edi+ebx*4+2048]   ;  ^ table[256*2+tmp]
                shr     eax,16                 ;
                movzx   ebx,al                 ; tmp = (c>>16) & 0xFF
                add     esi, 4*dwPtrIncr       ; ((ulg *)buf) += dwPtrIncr
                shr     eax,8                  ; tmp = (c>>24)
                xor     edx,[edi+ebx*4+1024]   ;  ^ table[256*1+tmp]
                mov     eax,[edi+eax*4]        ;  ^ table[256*0+tmp]
                xor     eax,edx                ; ..
        ENDM
      ELSE ; IZ_CRCOPTIM_UNFOLDTBL
        ; the edx register is not needed anywhere else
        SavLen  EQU     edx

UpdCRC_dword    MACRO
                Do_CRC
                Do_CRC
                Do_CRC
                Do_CRC
        ENDM
UpdCRC_dword_sh MACRO   dwPtrIncr
                Do_CRC
                Do_CRC
                add     esi, 4*dwPtrIncr       ; ((ulg *)buf) += dwPtrIncr
                Do_CRC
                Do_CRC
        ENDM
      ENDIF ; ?IZ_CRCOPTIM_UNFOLDTBL
Do_CRC_dword    MACRO
                xor     eax, dword ptr [esi]   ; c ^= *(ulg *)buf
                UpdCRC_dword_sh 1              ; ... ((ulg *)buf)++
        ENDM
Do_CRC_4dword   MACRO
                xor     eax, dword ptr [esi]    ; c ^= *(ulg *)buf
                UpdCRC_dword
                xor     eax, dword ptr [esi+4]  ; c ^= *((ulg *)buf+1)
                UpdCRC_dword
                xor     eax, dword ptr [esi+8]  ; c ^= *((ulg *)buf+2)
                UpdCRC_dword
                xor     eax, dword ptr [esi+12] ; c ^= *((ulg *)buf]+3
                UpdCRC_dword_sh	4               ; ... ((ulg *)buf)+=4
        ENDM
    ENDIF ; !NO_32_BIT_LOADS

    IFNDEF NO_ALIGN
_TEXT   segment use32 para public 'CODE'
    ELSE
_TEXT   segment use32
    ENDIF
        assume  CS: _TEXT

        public  _crc32
_crc32          proc    near  ; ulg crc32(ulg crc, ZCONST uch *buf, extent len)
                STD_ENTRY
                push    edi
                push    esi
                push    ebx
                push    edx
                push    ecx

                mov     esi,Arg2            ; 2nd arg: uch *buf
                sub     eax,eax             ;> if (!buf)
                test    esi,esi             ;>   return 0;
                jz      fine                ;> else {

                call    _get_crc_table
                mov     edi,eax
                mov     eax,Arg1            ; 1st arg: ulg crc
    IFNDEF __686
                sub     ebx,ebx             ; ebx=0; make bl usable as a dword
    ENDIF
                mov     ecx,Arg3            ; 3rd arg: extent len
                not     eax                 ;>   c = ~crc;

                test    ecx,ecx
    IFNDEF  NO_UNROLLED_LOOPS
                jz      bail
    IFNDEF  NO_32_BIT_LOADS
align_loop:
                test    esi,3               ; align buf pointer on next
                jz      SHORT aligned_now   ;  dword boundary
                Do_CRC_byte
                dec     ecx
                jnz     align_loop
aligned_now:
    ENDIF ; !NO_32_BIT_LOADS
                mov     SavLen,ecx          ; save current len for later
                shr     ecx,4               ; ecx = len / 16
                jz      No_Sixteens
    IFNDEF NO_ALIGN
; align loop head at start of 486 internal cache line !!
                align   16
    ENDIF
Next_Sixteen:
    IFNDEF  NO_32_BIT_LOADS
                Do_CRC_4dword
    ELSE ; NO_32_BIT_LOADS
                Do_CRC_byteof   0
                Do_CRC_byteof   1
                Do_CRC_byteof   2
                Do_CRC_byteof   3
                Do_CRC_byteof   4
                Do_CRC_byteof   5
                Do_CRC_byteof   6
                Do_CRC_byteof   7
                Do_CRC_byteof   8
                Do_CRC_byteof   9
                Do_CRC_byteof   10
                Do_CRC_byteof   11
                Do_CRC_byteof   12
                Do_CRC_byteof   13
                Do_CRC_byteof   14
                Do_CRC_byteof   15
                add     esi, 16                 ; buf += 16
    ENDIF ; ?NO_32_BIT_LOADS
                dec     ecx
                jnz     Next_Sixteen
No_Sixteens:
                mov     ecx,SavLen
                and     ecx,00000000FH      ; ecx = len % 16
    IFNDEF  NO_32_BIT_LOADS
                shr     ecx,2               ; ecx = len / 4
                jz      SHORT No_Fours
Next_Four:
                Do_CRC_dword
                dec     ecx
                jnz     Next_Four
No_Fours:
                mov     ecx,SavLen
                and     ecx,000000003H      ; ecx = len % 4
    ENDIF ; !NO_32_BIT_LOADS
    ENDIF ; !NO_UNROLLED_LOOPS
                jz      SHORT bail          ;>   if (len)
    IFNDEF NO_ALIGN
; align loop head at start of 486 internal cache line !!
                align   16
    ENDIF
loupe:                                      ;>     do {
                Do_CRC_byte                 ;        c = CRC32(c,*buf++,crctab);
                dec     ecx                 ;>     } while (--len);
                jnz     loupe

bail:                                       ;> }
                not     eax                 ;> return ~c;
fine:
                pop     ecx
                pop     edx
                pop     ebx
                pop     esi
                pop     edi
                STD_LEAVE
                ret
_crc32          endp

_TEXT   ends
;
    ENDIF ; !CRC_TABLE_ONLY
    ENDIF ; !USE_ZLIB
;
end
