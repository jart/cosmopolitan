.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_MapGenericMask,MapGenericMask,0

	.text.windows
MapGenericMask:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_MapGenericMask(%rip),%rax
	jmp	__sysv2nt
	.endfn	MapGenericMask,globl
	.previous
