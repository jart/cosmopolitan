.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_KillTimer,KillTimer,2093

	.text.windows
KillTimer:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_KillTimer(%rip),%rax
	jmp	__sysv2nt
	.endfn	KillTimer,globl
	.previous
