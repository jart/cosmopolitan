.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_CreateEventW,CreateEventW,0

	.text.windows
CreateEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateEventW(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateEvent,globl
	.previous
