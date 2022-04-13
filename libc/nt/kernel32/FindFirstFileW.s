.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_FindFirstFileW,FindFirstFileW,0

	.text.windows
__FindFirstFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindFirstFileW(%rip),%rax
	jmp	__sysv2nt
	.endfn	__FindFirstFile,globl
	.previous
