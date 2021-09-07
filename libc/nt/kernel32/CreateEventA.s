.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_CreateEventA,CreateEventA,0

	.text.windows
CreateEventA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateEventA(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateEventA,globl
	.previous
