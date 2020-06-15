.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_ContinueDebugEvent,ContinueDebugEvent,157

	.text.windows
ContinueDebugEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ContinueDebugEvent(%rip),%rax
	jmp	__sysv2nt
	.endfn	ContinueDebugEvent,globl
	.previous
