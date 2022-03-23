.include "o/libc/nt/codegen.inc"
.imp	pdh,__imp_PdhGetFormattedCounterValue,PdhGetFormattedCounterValue,0

	.text.windows
PdhGetFormattedCounterValue:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_PdhGetFormattedCounterValue(%rip),%rax
	jmp	__sysv2nt
	.endfn	PdhGetFormattedCounterValue,globl
	.previous
