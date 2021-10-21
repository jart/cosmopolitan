.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_ReleaseDC,ReleaseDC,2280

	.text.windows
ReleaseDC:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReleaseDC(%rip),%rax
	jmp	__sysv2nt
	.endfn	ReleaseDC,globl
	.previous
