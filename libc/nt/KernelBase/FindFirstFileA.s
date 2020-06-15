.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_FindFirstFileA,FindFirstFileA,363

	.text.windows
FindFirstFileA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindFirstFileA(%rip),%rax
	jmp	__sysv2nt
	.endfn	FindFirstFileA,globl
	.previous
