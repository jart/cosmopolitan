.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_FindFirstFileExA,FindFirstFileExA,0

	.text.windows
FindFirstFileExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindFirstFileExA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	FindFirstFileExA,globl
	.previous
