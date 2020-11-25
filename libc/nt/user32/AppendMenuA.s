.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_AppendMenuA,AppendMenuA,1515

	.text.windows
AppendMenuA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AppendMenuA(%rip),%rax
	jmp	__sysv2nt
	.endfn	AppendMenuA,globl
	.previous
