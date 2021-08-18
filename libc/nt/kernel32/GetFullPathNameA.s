.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetFullPathNameA,GetFullPathNameA,0

	.text.windows
GetFullPathNameA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetFullPathNameA(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetFullPathNameA,globl
	.previous
