.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetProcessImageFileNameA,GetProcessImageFileNameA,676

	.text.windows
GetProcessImageFileNameA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetProcessImageFileNameA(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetProcessImageFileNameA,globl
	.previous
