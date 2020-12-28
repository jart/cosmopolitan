.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_ContinueDebugEvent,ContinueDebugEvent,0

	.text.windows
ContinueDebugEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ContinueDebugEvent(%rip),%rax
	jmp	__sysv2nt
	.endfn	ContinueDebugEvent,globl
	.previous
