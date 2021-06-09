;===========================================================================
; Copyright (c) 1990-2006 Info-ZIP.  All rights reserved.
;
; See the accompanying file LICENSE, version 2000-Apr-09 or later
; (the contents of which are also included in zip.h) for terms of use.
; If, for some reason, all these files are missing, the Info-ZIP license
; also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
;===========================================================================
; crc_lcc.asm, optimized CRC calculation function for Zip and UnZip,
; created by Paul Kienitz and Christian Spieler.  Last revised 02 Jan 2006.
;
; The code in this file has been copied verbatim from crc_i386.{asm|S};
; only the assembler syntax and metacommands have been adapted to
; the habits of the free LCC-Win32 C compiler package.
; This version of the code uses the "optimized for i686" variant of
; crc_i386.{asm|S}.
; IMPORTANT NOTE to the Info-ZIP editors:
; The TAB characters in this source file are required by the parser of
; the LCC-Win32 assembler program and MUST NOT be removed!!
;
; For more information (and a revision log), look into the original
; source files.
;
	.text
	.file "crc32.c"
	.text
	.type	_crc32,function
_crc32:
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%ecx
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	.line	34
	.line	37
	movl	12(%ebp),%esi
	subl	%eax,%eax
	testl	%esi,%esi
	jz	_$3
	.line	39
	call	_get_crc_table
	movl	%eax,%edi
	.line	41
	movl	8(%ebp),%eax
	movl	16(%ebp),%ecx
	notl	%eax
	testl	%ecx,%ecx
	jz	_$4
_$5:
	testl	$3,%esi
	jz	_$6
	xorb    (%esi),%al
	incl	%esi
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	decl	%ecx
	jnz	_$5
_$6:
	movl	%ecx,%edx
	shrl	$4,%ecx
	jz	_$8
_$7:
	xorl	(%esi),%eax
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	xorl	4(%esi),%eax
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	xorl	8(%esi),%eax
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	xorl	12(%esi),%eax
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	addl	$16,%esi
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	decl	%ecx
	jnz	_$7
_$8:
	movl	%edx,%ecx
	andl	$0x0f,%ecx
        shrl	$2,%ecx
	jz	_$10
_$9:
	xorl	(%esi),%eax
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	addl	$4,%esi
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	decl	%ecx
	jnz	_$9
_$10:
	movl	%edx,%ecx
	andl	$0x03,%ecx
	jz	_$4
_$11:
	xorb    (%esi),%al
	incl    %esi
	movzbl	%al,%ebx
	shrl	$8,%eax
	xorl	(%edi,%ebx,4),%eax
	decl	%ecx
	jnz	_$11
_$4:
	xorl	$0xffffffff,%eax
_$3:
	.line	52
	popl	%edi
	popl	%esi
	popl	%ebx
	leave
	ret
_$34:
	.size	_crc32,_$34-_crc32
	.globl	_crc32
	.extern	_get_crc_table
