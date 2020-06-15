.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_CreateWindowExA,CreateWindowExA,1624

	.text.windows
CreateWindowExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateWindowExA(%rip),%rax
	jmp	__sysv2nt12
	.endfn	CreateWindowExA,globl
	.previous
