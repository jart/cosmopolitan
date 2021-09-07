.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_CreateEventExA,CreateEventExA,0

	.text.windows
CreateEventExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateEventExA(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateEventExA,globl
	.previous
