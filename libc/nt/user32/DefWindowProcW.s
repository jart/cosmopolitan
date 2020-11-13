.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_DefWindowProcW,DefWindowProcW,174

	.text.windows
DefWindowProc:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_DefWindowProcW(%rip),%rax
	jmp	__sysv2nt
	.endfn	DefWindowProc,globl
	.previous
