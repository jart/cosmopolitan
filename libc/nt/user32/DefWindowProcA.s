.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_DefWindowProcA,DefWindowProcA,173

	.text.windows
DefWindowProcA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_DefWindowProcA(%rip),%rax
	jmp	__sysv2nt
	.endfn	DefWindowProcA,globl
	.previous
