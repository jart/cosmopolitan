.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_AdjustWindowRect,AdjustWindowRect,1507

	.text.windows
AdjustWindowRect:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AdjustWindowRect(%rip),%rax
	jmp	__sysv2nt
	.endfn	AdjustWindowRect,globl
	.previous
