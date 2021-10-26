.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_MultiByteToWideChar,MultiByteToWideChar,0

	.text.windows
MultiByteToWideChar:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_MultiByteToWideChar(%rip),%rax
	jmp	__sysv2nt6
	.endfn	MultiByteToWideChar,globl
	.previous
