.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_CreateEventExW,CreateEventExW,0

	.text.windows
CreateEventEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateEventExW(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateEventEx,globl
	.previous
