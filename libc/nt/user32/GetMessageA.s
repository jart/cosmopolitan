.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_GetMessageA,GetMessageA,1895

	.text.windows
GetMessageA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetMessageA(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetMessageA,globl
	.previous
