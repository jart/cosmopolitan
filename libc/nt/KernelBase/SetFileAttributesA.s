.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_SetFileAttributesA,SetFileAttributesA,1507

	.text.windows
SetFileAttributesA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetFileAttributesA(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetFileAttributesA,globl
	.previous
