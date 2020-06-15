.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_FindNextFileA,FindNextFileA,374

	.text.windows
FindNextFileA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindNextFileA(%rip),%rax
	jmp	__sysv2nt
	.endfn	FindNextFileA,globl
	.previous
