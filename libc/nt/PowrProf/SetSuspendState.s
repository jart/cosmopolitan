.include "o/libc/nt/codegen.inc"
.imp	PowrProf,__imp_SetSuspendState,SetSuspendState,0

	.text.windows
SetSuspendState:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetSuspendState(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetSuspendState,globl
	.previous
